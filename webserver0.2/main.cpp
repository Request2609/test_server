#include<signal.h>
#include"unp.h"
#include"threadPool.h"
#include"func.h"
#include"task.h"
#include"locker.h"
#include"socketfd.h"
#include"epfd.h"

#define POOL_NUM 20
sockfd * serv_sock ;
ThreadPool<tasks> pool( POOL_NUM ) ;
typedef void (*sighandler_t)(int) ;

void sig_handle( int signo )
{
    if( signo == SIGINT)
    {
        pool.~ThreadPool();
        delete serv_sock ;  
        exit(0) ;
    }

    if(signo == SIGPIPE)
    {
        cout << "=======" << endl ;
        return ;
    }
}

//将套接字添加到epoll树中
int epoll_add( int epfd, int fd )
{
    struct linger tmp ;
    tmp.l_onoff = 0;  
    tmp.l_linger = 0 ;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, &tmp, sizeof( tmp ) );
    setnonblocking( fd ) ;
    struct epoll_event ev ;
    ev.events = EPOLLIN|EPOLLET ;
    ev.data.fd = fd ;
    int ret = epoll_ctl( epfd,EPOLL_CTL_ADD, fd, &ev);
    
    if(ret<0)
    {
        return -1 ;
    }

    return 1 ;
}

int run_epoll( epollfd ep, int servfd)
{   
    while(1){
    
        struct epoll_event all[N] ;
    
        int ret = epoll_wait( ep.epfd, all, N,-1) ;
    
    
        if(ret < 0)
    
        {
            cout << "epoll_wait ret < 0" << endl;
            return -1 ;
        }
    
        for( int i = 0; i < ret; i++ )
        {
        
            int fd =all[i].data.fd ;
        
            //活跃事件加入到事件列表
            if( fd == servfd )
            {
                struct sockaddr_in cli ;
                socklen_t len = sizeof(cli) ;
                fd = accept( fd, ( struct sockaddr* )&cli, &len) ;
                if( fd < 0 )
                {
                    cout<<"create fd error!" <<endl ;
                    return -1 ;
                }
                int ret = epoll_add( ep.epfd, fd ) ;
                if(ret == -1 )
                {
                    return -1 ;
                }
            }

            //可读事件
            else if( all[i].events&EPOLLIN ){
    
                tasks* tk = new tasks( fd ) ;   
                tk->epfd = ep.epfd ;
                pool.append( tk ) ;
            }

            else
            {
                continue ;
            }
        }
    }

    return 1 ;
}

int main(int argc, char** argv)
{   
    chdir("serverHttp");

    if(argc != 3)
    {
        cout << "Usage:<filename><ip><port>" << endl ;
        return 0 ;
    }
    serv_sock = sockfd :: getsockfd( argv[1], argv[2] ) ;
    
    serv_sock->s_bind() ;
    serv_sock->s_listen() ;

    sighandler_t ret, ret_pipe ;
    setnonblocking(serv_sock->fd) ;
    epollfd ep ;
    ep.epollfd_create_handle( serv_sock->fd ) ;
    ret = signal( SIGINT, sig_handle ) ;
    ret_pipe = signal(SIGPIPE, sig_handle) ;

    run_epoll( ep, serv_sock->fd) ;
    delete serv_sock ;
}


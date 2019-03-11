#pragma once
#include"unp.h"

class epollfd
{
public:
    int epfd ;

public:
    epollfd(int epfd = 0)
    {
        epfd = 0;
    }
    ~epollfd()
    {
        close(epfd) ;
    }

    void epollfd_create_handle( int sockfd )
    {
        //创建epoll句柄
        epfd = epoll_create(2048);
        //将服务器端套接字注册到epoll树中,监听同类型的时间
        struct epoll_event ev ;
        ev.events =EPOLLIN|EPOLLET ; 
        ev.data.fd = sockfd ;
        
        int ret = epoll_ctl( epfd, EPOLL_CTL_ADD, sockfd, &ev ) ;
        
        if( ret < 0 )
        {
            cout << "注册出错"<< endl ;
            exit(1) ;
        }
    }
};


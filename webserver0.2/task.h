#pragma once
#include "unp.h"
#include"func.h"
#define MAX 1024

class tasks
{
private :
    
    int connfd ;
public :
    int epfd ;
    tasks( int connfd ) {
        setnonblocking( connfd ) ;
        this->connfd  = connfd ; }

    ~tasks() { 
    }

    void do_read() ;
    void del_fd() ;
};

 
void tasks:: del_fd()
{
    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL ) ;
    if( ret < 0 )
    {
        cout << "删除文件描述符出错!" << endl ;
        exit(1) ;
    }

    close( connfd ) ;
}

void tasks :: do_read()
{
    string info ;
    char line[MAX] ;
    bzero( line , sizeof(line) );
    int s = sizeof(line) ; 
    int len ;

    while((len = get_line( connfd , line , s ))){
        if( len == 0 )
        {
            return ;
        }
        
        else if( len == -1 )
        {
            continue ;
        }
     
        else{
            
            while( len )
            {
                if(len == -1)
                {
                    break ;
                }
                char buf[MAX] = {0};
                int s =sizeof(buf) ;
                len = get_line(connfd , buf , s) ;
                info = buf ;
                if(info.find( "userinfo" ) != string::npos ){
                    info = get_info( info ) ;
                    break ;
                }
             }
        }

        if(strncasecmp( "get" , line ,3 ) == 0 )
        {   

            http_request( line , connfd ) ;
        }

        else if( strncasecmp( "post" , line, 4 ) == 0 )
        {
            http_post(line,connfd, info);
        }
        else {
            continue ;
        }
        
        break ;
    }

    del_fd() ;
    return ;
}

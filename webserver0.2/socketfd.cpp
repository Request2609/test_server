#include "unp.h" 
#include "socketfd.h"


//全局只能有一个服务端socket,使用单例模式

sockfd* sockfd:: sock = NULL ;

sockfd :: sockfd()
{
    sock = NULL ;
}

sockfd* sockfd :: getsockfd( const char* ip, const char* port )
{
    if(sock == NULL )
    {
        sock = new sockfd() ;
        sock->fd = socket( AF_INET, SOCK_STREAM, 0) ;
        if( sock->fd < 0 )
        {
            free(sock) ;
            cout << "build socket error!" << endl ;
            exit( 1 ) ;
        }
        
/*        struct linger tmp ;
        tmp.l_onoff = 0;  
        tmp.l_linger = 0 ;
        setsockopt(sock->fd, SOL_SOCKET, SO_LINGER, &tmp, sizeof( tmp ) );
*/
        ( sock->serv ).sin_family = AF_INET ; 
        ( sock->serv ).sin_port = htons( atoi(port) ) ;
        ( sock->serv ).sin_addr.s_addr = inet_addr( ip );
        memset(sock->serv.sin_zero,'\0',8);

    }
    return sock ;
}

void sockfd :: s_bind()
{
    //这块一定得设置为1,才有效
    int flag = 1 ;
    int ret = setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag) ) ;
    if( ret < 0 )
    {
        free(sock) ;
        cout << "build socketopt error!" << endl ;
        exit( 1 ) ;
    }
    ret = bind( sock->fd, ( struct sockaddr* )&serv, sizeof( serv ) ) ;

    if( ret < 0 )
    {   
        free( sock ) ;
        cout << "bind socket error!" << endl ;
        exit( 1 ) ;
    }
    
    cout << "binded......" << endl ;
}

void sockfd :: s_listen(){
    
    int ret = listen( sock->fd , MAXLINE ) ;

    if( ret < 0 )
    {
        free(sock) ;
        cout << "listen socket error!" << endl ;
        exit( 1 ) ;
    }
    
    cout << "listen......" << endl ;
}

sockfd :: ~sockfd()
{
    close( fd ) ;
    free( sock ) ;
    sock = NULL ;
}

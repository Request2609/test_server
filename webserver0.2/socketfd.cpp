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
        struct linger tmp = { 0, 1};                                                                                                                                                
        setsockopt( sock->fd, SOL_SOCKET, SO_LINGER, &tmp, sizeof( tmp ) );

        cout << sock->fd <<endl ;
        
        if( sock->fd < 0 )
        {
            free(sock) ;
            cout << "build socket error!" << endl ;
            exit( 1 ) ;
        }
        
        ( sock->serv ).sin_family = AF_INET ; 
        ( sock->serv ).sin_port = htons( atoi(port) ) ;
        ( sock->serv ).sin_addr.s_addr = inet_addr( ip );
        memset(sock->serv.sin_zero,'\0',8);

        int flag = 0 ;
        int ret = setsockopt( sock->fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag) ) ;
        if( ret < 0 )
        {
            free(sock) ;
            cout << "build socketopt error!" << endl ;
            exit( 1 ) ;
        }
    }
    return sock ;
}

void sockfd :: s_bind()
{
    int ret = bind( sock->fd, ( struct sockaddr* )&serv, sizeof( serv ) ) ;
    cout<<serv.sin_family<<endl ;
    cout<<serv.sin_addr.s_addr<<endl ;

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

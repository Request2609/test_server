#pragma once
#include "unp.h"
#include<memory>
#include"func.h"

class sockfd{

private :
    
    static sockfd* sock ;
   // static struct sockaddr_in serv ;
    
public :
    struct sockaddr_in serv ;
    int fd ; 
    sockfd() ; 
    ~sockfd() ;
    static sockfd* getsockfd(const char* ip, const char* port ) ;
    //设置非阻塞
    void s_bind() ;
    void s_listen() ;
    
};


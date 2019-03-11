#include <iostream>
#include <string>
#include <cstring>
#include"unp.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include<string.h>

#define MAX 1024
using namespace std;

int main( int argc, char **argv ) {
  
    int connfd ;
    string info = argv[0] ;
    string name = info.substr( 0, info.find("&")) ;
    string password = info.substr( info.find("&")+1);
    password = password.substr(0,password.find(",")) ;
    connfd = atoi( info.substr(info.find(",")+1).c_str());
    
    
    if(strcmp(password.c_str(), "ck") == 0 && strcmp(name.c_str(), "ckck") == 0)
    {
        dup2( connfd, STDOUT_FILENO ) ;
        string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=utf-8\r\n\r\n";
        string body = "<html><head><title>登录成功页面</title></head>";
        body += "<body><font size=10><strong><p>成功登录</p></strong></font></body></html>";
        response += body;
        printf("%s", response.c_str());
    }

    else
    {
        dup2( connfd, STDOUT_FILENO ) ;
        string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=utf-8\r\n\r\n";
        string body = "<html><head><title>登录失败页面</title></head>";
        body += "<body><font size=10><strong><p>登录失败</p></strong></font></body></html>";
        response += body;
        printf("%s", response.c_str());
        
    }
    //close(connfd) ;
    return 0;
}

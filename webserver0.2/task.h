#pragma once
#include "unp.h"
#include"func.h"
#define MAX 1024
#define POST 2
#define GET 1 
#define BUFFER_SIZE 4096

class tasks
{
private :
    int connfd ;

public :
    tasks( int connfd ) ;
    ~tasks() ;

public :
    int epfd ;
    //普通的get请求行
    string request ;
    //post请求的尾部数据
    string post_request ;
public :
    void do_read() ;
    void  get_request(string str1) ;
    void del_fd() ;
    int get_infos() ;
    int process_request(string str1);
    bool is_post(string str1) ;
};

tasks :: tasks(int connfd)
{
    setnonblocking( connfd ) ;
    this->connfd  = connfd ; 
}

tasks :: ~tasks()
{
}

//判断是否为post请求
bool tasks :: is_post(string str1)
{
    return strncasecmp(str1.c_str(), "post", 4) == 0 ?1:0 ;
}

//处理请求行
int tasks :: process_request(string str1)
{

        if(is_post(str1))
        {
            //处理post请求
            string :: size_type l = str1.find("\r\n");
            if(l == str1.npos)
            {
                request = "" ;
                return 0 ;
            }
            request = str1.substr(0,l+1) ;
            get_request(str1) ;  
            return POST ;
        }

        else
        {
            //找第一次出现\r\n的位置
            int len = str1.find("\r\n") ;
            request  = str1.substr(0,len+1) ;
            return GET ;
        } 
}

//获取post提交的数据
void  tasks ::get_request(string str1) 
{
    string s = "" ;
    string :: size_type l = str1.find("userinfo") ;
    if(l == str1.npos)
    {
        cout << "没有找到相应信息" << endl ;
        post_request = "" ;
        return  ;
    }
    post_request = str1.substr(l) ;
}

//将套接字从epoll树上处理掉
void tasks :: del_fd()
{
    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL ) ;
    if( ret < 0 )
    {
        cout << strerror(errno) <<endl ;
        cout << "删除文件描述符出错!" << endl ;
        exit(1) ;
    }
    close( connfd ) ;
}

//获取用户信息
int tasks :: get_infos()
{
    char line[BUFFER_SIZE] ;
    int ret ;
    string str1 ;
    //读取缓冲区中的数据
    while((ret = recv(connfd, line, sizeof(line), 4096)) >= 0)
    {
        
        if(ret == BUFFER_SIZE)
        {
            str1+=line ;
        }
        if(ret < BUFFER_SIZE&&ret > 0)
        {
            str1+=line ;
            break ;
        }
        
        else if(ret == 0)
        {
             return 0 ;
        }

        else
        {
            cout << "recv error! " <<__LINE__ << endl ;
            exit(1) ;
        }
    }
    int re = process_request(str1) ;
    return re ;
}

//线程池中的任务调度函数,开始处理请求
void tasks :: do_read()
{
    string info ;
    char line[BUFFER_SIZE] ;
    bzero( line , sizeof(line) );
    int ret =  get_infos() ;

    if(ret < 0)
    {
        cout << "读取数据出错!" << endl ;
        del_fd() ;
        exit(1) ;
    }

    if(ret == 0)
    {
        del_fd() ;
        return  ;
    }
    
    if(ret == GET)
    {
        if(request.length() == 0)
        {
            del_fd() ;
            return ;    
        }
        http_request(request.c_str(), connfd ) ;
    }

    else{
        
        if(post_request.length() == 0 || request.length() == 0)
        {
            del_fd() ;
            return ;
        }
        string info = get_info(post_request) ;
        http_post(request.c_str(), connfd,info) ;
    }

    del_fd() ;
    return ;
}


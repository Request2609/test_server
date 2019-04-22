#include"func.h"

#include<dirent.h>

#define MAX 1024
#define BUFFER_SIZE 4096

string get_info( string oldStr )
{
     
    size_t idx1 = oldStr.find("=") ;
    size_t idx2 = oldStr.find("&") ;    
    string name = oldStr.substr(idx1,idx2) ;
    name = name.substr(1,name.find("&")-1) ;
    idx1 = idx2+strlen("password=") ;
    const char* tmp = oldStr.c_str()+idx1 ;
    int passlen = 0 ;
    while( *tmp != '&' )
    {
        passlen++ ;
        tmp ++ ;
    }

    idx2 = passlen + idx1 -1 ;
    string password = oldStr.substr( idx1, idx2 ) ;
    password = password.substr(1,password.find("&")-1) ;
    return name +"&"+ password ;
}

void send_response_head( int connfd, int nu, const char* ss, const char * type, long l )
{
    char buf[1024] ;
        
    //状态行
    sprintf(buf ,"http/1.1 %d %s\r\n" ,nu ,ss ) ;
    send( connfd , buf , strlen(buf), 0 );

    //消息包头
    sprintf(buf , "Content-Type: %s\r\n",type) ;
    send( connfd , buf , strlen(buf) , 0);
    sprintf(buf+strlen(buf) , "Content-Length: %ld\r\n",l) ;
    send(connfd , buf ,strlen(buf) ,0) ;
    sprintf(buf,"\r\n") ;
    send(connfd , buf , 2, 0) ;
}

void http_post( const char* request, int connfd, string info )
{

    char method[MAX] ;
    char proctol[MAX] ;
    char _path[MAX] ;
    sscanf(request ,"%s %s %s",method, _path , proctol ) ;
    char *file = _path+1 ;
    string a = _path ;
    char tmp[MAX] ;
    if(a.find( "register" ) != string::npos)
    {
        //将套接字传给CGI进程
        sprintf( tmp ,"%s,%d",info.c_str(),connfd );
        //切换进程
        pid_t pid ;
        if((pid=fork()) ==0 ){

            execl( file, tmp, NULL ) ;
        }
        wait(NULL) ;
    }
}

//处理get请求
void http_request(const char* buf ,int connfd )
{

    char method[MAX] ;
    char proctol[MAX] ;
    char _path[MAX] ;
    
    sscanf(buf ,"%s %s %s",method, _path , proctol );
    
    decode_str( _path, _path ) ;
    //主要是path
    char* file = _path + 1 ;
    cout <<_path << endl ;
    //初始时,要是没有特殊请求,返回客户端着1.html文件
    if( strcmp(_path , "/") == 0)
    {
        strcpy( _path , "1.html") ;

        struct stat st ;
        int ret = stat(_path , &st) ;
        if( ret == -1 )
        {
            send_error(connfd) ;
            return ;
        }
        
        send_response_head( connfd , 200 , "OK" , get_file_type(_path) , st.st_size);
        send_file( _path , connfd );
    }

    else
    {   
        struct stat st ;
        int ret = stat(file , &st) ;
        if( ret == -1 )
        {
            send_error(connfd) ;
            return ;
        }

        if( S_ISDIR( st.st_mode ) )
        {
            char bb[6] = ".html" ;

            send_response_head( connfd , 200 , "OK" , get_file_type( bb ) , 4096 );
            send_dir( connfd, file ) ;   
        }
     
        else if( S_ISREG( st.st_mode ) )
        {
            send_response_head( connfd , 200 , "OK" , get_file_type(file) , st.st_size);
            send_file(file , connfd );
        }
    }    
}

//发送目录的内容
void send_dir(int connfd ,const char* dirname)
{
    
    char buf[1024] ;
    char buf1[1024];
    bzero( buf1, sizeof(buf1)) ;
    bzero( buf, sizeof(buf)) ;
    
    char buf2[400] ="<div style=\"position:absolute;z-index:-1;width:100%;height:100%;\">";
    strcat(buf2, "<img src=\"back1.jpg\" width=\"100%\" height=\"100%\"/></div>") ;
    sprintf(buf, "<html><head><title>%s</title></head>\r\n","Music-Center");
    
    sprintf( buf+strlen(buf), "%s", buf2 ) ;
    struct dirent **ptr ; 
    //将目录名全放在数组中
    cout << buf <<endl ;
    int num = scandir(dirname,&ptr,NULL,alphasort);
    char path[1024];
    int flag = 1 ;
    
    for( int i = 0 ;i<num ;i++)
    {
            char *name = ptr[i]->d_name ;
            //拼接文件的完整路径
            //字符串拼接
            //如果是隐藏文件就跳过
            if( name[0] == '.' )
            {
                continue ;
            }   

            sprintf(path,"%s/%s",dirname,name);
            struct stat st ;
            stat(path,&st);
           cout << "*********"<<path <<endl ;
            //将文件路径解码,并发送到浏览器
            encode_str(buf1,sizeof(buf1),path);
            //如果是文件
            if(S_ISREG(st.st_mode))
            {
                flag = 0 ; 
                sprintf(buf+strlen(buf),"<p><tr><td><a href=\"%s\"</a><font size=5><strong>%s</strong></font></td></tr></p>",buf1,name);
            }
            
            else if( S_ISDIR(st.st_mode) )
            {
                sprintf(buf+strlen(buf),"<tr><td><a href=\"%s/\"><font size=3><strong>%s/</strong></font></a></td><td>%ld</td></tr>",buf1,name,(long)st.st_size);   
            }

            send(connfd,buf,sizeof(buf),0);
            bzero(buf,sizeof(buf));
    }

    if( flag ==1 )
    {
        sprintf(buf+strlen(buf),"<p><tr><td><a href=\"%s\"</a><font size=10><strong>%s</strong></font></td></tr></p>","1.html","No source temparay!Come back!"); 
    }

    sprintf(buf+strlen(buf),"</body></html>");
    send(connfd,buf,sizeof(buf),0);
}

//编码
void encode_str( char *to , int tosize ,const char *from )
{

    int tolen ;

    for( tolen = 0 ;*from!='\0'&&tolen+4<tosize ;++from )
    {

        if(isalnum(*from) || strchr ("/_.-~",*from)!=(char*)0)
        {
            *to = *from ;
            ++to ;
            ++tolen ;
        }
        else
        {
            //2位十六进制数不够
            sprintf(to ,"%%%02x",(int)*from&0xff);

            to+=3 ;
            tolen +=3 ;
        }
    }
    *to = '\0' ;
}

//解码
void decode_str(char *to ,char *from )
{

    for( ; *from!='\0' ;++to,++from)
    {

        if(from[0] =='%'&& isxdigit(from[1])&&isxdigit(from[2]))
        {
            *to = hexit(from[1])*16 +hexit(from[2]) ;
            from+=2 ;
        }
        else{
            *to = *from ;
        }
    }
    *to = '\0' ;
}

int hexit(char c)
{

    if(c>='0'&&c<='9')
        return c-'0' ;
    if(c>='a'&&c<='f')
        return c-'a'+10 ;
    if(c>='A'&&c<='F')
        return c-'A'+10 ;
    return 0 ;
}

void send_error( int connfd )
{
    
    struct stat st ;
    
    int ret = stat("404.html", &st) ;
    if( ret == -1 ) exit(-1) ;

    send_response_head(connfd , 404 , "File Not found",".html",st.st_size );;
    send_file("404.html",connfd);
    return ;

}
//发送普通文件
void send_file(const char* file , int connfd)
{
        int fd = open( file , O_RDONLY ) ;
        
        if(fd == -1)
        {
            return  ;
        }

        char buf[BUFFER_SIZE] ;
        int len = 0 ;
        while((len = read(fd , buf, sizeof(buf))) > 0)
        {
           send( connfd , buf ,len , 0) ;
        }
       
        if( len == -1 )
        {   
            perror("read file error!") ;
            exit(1) ;
        }
}  

//设置非阻塞
int  setnonblocking( int fd )
{
    int old_ = fcntl( fd, F_GETFL ) ;
    int new_ = old_ | O_NONBLOCK ;
    fcntl( fd, F_SETFL, new_ ) ;
    return old_ ;
}

//判断各种文件的格式
const char* get_file_type(char* name)
{

    char *dot ;

    //自右向左查找.字符,如果不存在.,返回NULL
    dot = strrchr(name ,'.'); 
    if(dot == NULL)
        return "text/plain; charset=utf-8";
    if(strcmp(dot ,".html")==0||strcmp(dot,".htm")==0)
        return "text/html; charset=utf-8";
    if(strcmp(dot,".jpg")==0)
        return "image/jpg";
    if(strcmp(dot,".jpeg") == 0)
        return "image/jpeg";
    if(strcmp(dot,".gif")==0 )
        return "image/gif";
    if(strcmp(dot,".png")==0)
        return "image/png";
    if(strcmp(dot,"text/css")==0)
            return "text/css";
    if(strcmp(dot,".au")==0)
        return "audio/basic" ;
    if(strcmp(dot,".wav")==0)
        return "audio/wav";
    if(strcmp(dot,".mpeg")==0||strcmp(dot,".mp4")==0)
        return "video/mpeg";
   if(strcmp(dot,".vrml")==0||strcmp(dot,"wrl")==0)
        return "model/vrml";
    if(strcmp(dot,".mp3")==0)
        return "audio/mpeg";
    if(strcmp(dot,"ogg")==0)
        return "application/ogg";
    if(strcmp(dot,".pac")==0)
        return "application/x-ns-proxy-autoconfig";
    return "text/plain; charset=utf-8";
}


#pragma once
#include"unp.h"
#define N 100

void encode_str( char* to, int tosize, const char* from ) ;
void decode_str( char* to, char* from ) ;
int hexit( char c ) ;
int  setnonblocking( int fd ) ;
void add_fd( int& epfd , int& sfd ) ;
void send_dir( int connfd, const char* dirname ) ;
void http_post(const char* request, int connfd, string info ) ;
void run_epoll( int& epfd , epoll_event evfds[] , int sfd );
void do_read( int& connfd  ) ;
string get_info( string oldStr ) ;
int get_line( int& connfd, char* line , int& len) ;
void http_request( const char *buf , int connfd ) ;
void send_file(const char* file , int connfd) ;
const char* get_file_type(char *name);
void send_response_head(int connfd , int nu , const char* ss , const char *type ,long l);
void send_error(int connfd);

#include <stdio.h>  
#include <sys/socket.h>  
#include <sys/types.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <ctype.h>  
#include <strings.h>  
#include <string.h>  
#include <sys/stat.h>  
#include <pthread.h>  
#include <sys/wait.h>  
#include <stdlib.h>  
  
#define ISspace(x) isspace((int)(x))  
  
#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n" 
int init(u_short *);
int init(u_short *port)  
{  
    int httpd = 0;  
    struct sockaddr_in name;  
  
    /*建立 socket */  
    httpd = socket(PF_INET, SOCK_STREAM, 0);  
    if (httpd == -1)  
        error_die("socket");  
    memset(&name, 0, sizeof(name));  
    name.sin_family = AF_INET;  
    name.sin_port = htons(*port);  
    name.sin_addr.s_addr = htonl(INADDR_ANY);  
    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)  
        error_die("bind");  
    /*如果当前指定端口是 0，则动态随机分配一个端口*/  
    if (*port == 0)  /* if dynamically allocating a port */  
    {  
        int namelen = sizeof(name);  
        if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)  
            error_die("getsockname");  
        *port = ntohs(name.sin_port);  
    }  
    /*开始监听*/  
    if (listen(httpd, 5) < 0)  
        error_die("listen");  
    /*返回 socket id */  
    return(httpd);  
}  

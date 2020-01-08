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
int get_line(int, char *, int);
void headers(int, const char *);
void unsupported(int); 
void read(int, FILE *);  
void serve_file(int, const char *);
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
int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;

    /*把终止条件统一为 \n 换行符，标准化 buf 数组*/
    while ((i < size - 1) && (c != '\n'))
    {
        /*一次仅接收一个字节*/
        n = recv(sock, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        if (n > 0)
        {
            /*收到 \r 则继续接收下个字节，因为换行符可能是 \r\n */
            if (c == '\r')
            {
                /*使用 MSG_PEEK 标志使下一次读取依然可以得到这次读取的内容，可认为接收窗口不滑动*/
                n = recv(sock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                /*但如果是换行符则把它吸收掉*/
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            /*存到缓冲区*/
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    buf[i] = '\0';

    /*返回 buf 数组大小*/
    return(i);
}
void headers(int client, const char *filename)
{
    char buf[1024];
    (void)filename;  /* could use filename to determine file type */

    /*正常的 HTTP header */
    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    /*服务器信息*/
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}
void unsupported(int client)
{
    char buf[1024];

    /* HTTP method 不被支持*/
    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    /*服务器信息*/
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}
void cat(int client, FILE *resource)
{
    char buf[1024];

    /*读取文件中的所有数据写到 socket */
    fgets(buf, sizeof(buf), resource);
    while (!feof(resource))
    {
        send(client, buf, strlen(buf), 0);
        fgets(buf, sizeof(buf), resource);
    }
}
void serve_file(int client, const char *filename)
{
    FILE *resource = NULL;
    int numchars = 1;
    char buf[1024];

    /*读取并丢弃 header */
    buf[0] = 'A'; buf[1] = '\0';
    while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
        numchars = get_line(client, buf, sizeof(buf));

    /*打开 sever 的文件*/
    resource = fopen(filename, "r");
    if (resource == NULL)
        not_found(client);
    else
    {
        /*写 HTTP header */
        headers(client, filename);
        /*复制文件*/
        read(client, resource);
    }
    fclose(resource);
}

#ifndef _LINUX_HTTP_CLIENT_
#define _LINUX_HTTP_CLIENT_

#ifdef WIN32
    #include <Winsock2.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #define closesocket close
#endif

typedef struct linux_http_client{
 int socket;
 int remote_port;
 char remote_ip[16];
 struct sockaddr_in _addr;
 int connected;
} http_client;

int http_client_create(http_client *,const char *host, int port);
int http_client_conn(http_client *);
int http_client_recv(http_client *,char **lpbuff,int size);
int http_client_send(http_client *,char *buff,int size);
int http_client_close(http_client *);

#endif
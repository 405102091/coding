//#include "stdafx.h"
#include "stdio.h"
#include "winsock2.h"
#include "windows.h"
#include "tchar.h"
#include "locale.h"
//#pragma   comment(lib,"ws2_32.lib")
#define   winsock_version   0x0101
int main()
{
    SOCKADDR_IN   saServer;
    LPHOSTENT   lphostent;
    WSADATA   wsadata;
    SOCKET   hsocket;
    int   nRet;
    const   char*   host_name="www.vipfenxiang.com";
    char*   req="GET /xunlei/7414.html HTTP/1.1\r\n"
                "Host: www.vipfenxiang.com\r\n"
                "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; ) Opera/UCWEB7.0.2.37/28/999\r\n"
                "\r\n";
                //"Content-Type: application/x-www-form-urlencoded\r\n"
                //"Content-Length: 34\r\n\r\n";
//    "name=XXXXXX&pass=XXXXXXXXX&type=1";   //if   you   change   xxx,   you   may   have   to   change   content-length   correspondingly
    if(WSAStartup(winsock_version,&wsadata))
        printf("can't   initial   socket");
    lphostent=gethostbyname(host_name);
    if(lphostent==NULL)
        printf("lphostent   is   null");
    hsocket   =   socket(AF_INET,   SOCK_STREAM,   IPPROTO_TCP);
    saServer.sin_family   =   AF_INET;
    saServer.sin_port   =   htons(80);
    saServer.sin_addr   =   *((LPIN_ADDR)*lphostent->h_addr_list);
    nRet   =   connect(hsocket,   (LPSOCKADDR)&saServer,   sizeof(SOCKADDR_IN));
    if   (nRet   ==   SOCKET_ERROR)
    {
        printf("can't   connect");
        closesocket(hsocket);
        return 1;
    }
    else
        printf("connected   with   %s\n",host_name);
    nRet   =   send(hsocket,   req,   strlen(req),   0);
    if   (nRet   ==   SOCKET_ERROR)
    {
        printf("send()   failed");
        closesocket(hsocket);

    }
    else
        printf("send()   OK\n");
    char   dest[100000];
    wchar_t wcdest[100000];
    nRet=1;
    while(nRet>0)
    {
        nRet=recv(hsocket,(LPSTR)dest,sizeof(dest),0);
        if(nRet>0)
            dest[nRet]=0;
        else
            dest[0]=0;
        MultiByteToWideChar( CP_UTF8, 0, dest, (int) strlen(dest)+1, wcdest, sizeof(wcdest)/sizeof(wchar_t));
        //memset(dest,0,100000);
        WideCharToMultiByte( CP_ACP, 0, wcdest, -1, dest, 100000, NULL, NULL);
        printf("\nReceived   bytes:%d\n",nRet);
        printf("Result:\n%s",dest);
    }
}

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <locale.h>
#include <regex>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
#include <chrono>
#include <thread>
#pragma   comment(lib,"ws2_32.lib")
#pragma   comment(lib,"user32.lib")
#define   winsock_version   0x0101
using namespace std;

const int buff_size=100000;
char   http_recv[buff_size];
bool HttpConnect(char* host_name,char* request)
{
	SOCKADDR_IN   saServer;
    LPHOSTENT   lphostent;
    WSADATA   wsadata;
    SOCKET   hsocket;
    int   nRet;
	if(WSAStartup(winsock_version,&wsadata))
        printf("can't initial socket\n");
    lphostent=gethostbyname(host_name);
    if(lphostent==NULL)
        printf("lphostent is null\n");

    hsocket   =   socket(AF_INET,   SOCK_STREAM,   IPPROTO_TCP);
    saServer.sin_family   =   AF_INET;
    saServer.sin_port   =   htons(80);
    saServer.sin_addr   =   *((LPIN_ADDR)*lphostent->h_addr_list);
    nRet   =   connect(hsocket,   (LPSOCKADDR)&saServer,   sizeof(SOCKADDR_IN));
	if   (nRet   ==   SOCKET_ERROR)
    {
        printf("can't connect\n");
        closesocket(hsocket);
        return 0;
    }
    else
        printf("connected with %s\n",host_name);
    nRet   =   send(hsocket,   request,   strlen(request),   0);
    if   (nRet   ==   SOCKET_ERROR)
    {
        printf("send() failed\n");
        closesocket(hsocket);

    }
    else
        printf("send() OK\n");
    
	wchar_t wcdest[buff_size];
    nRet=1;

    do
    {
        nRet=recv(hsocket,(LPSTR)http_recv,buff_size-1,0);
        if(nRet>0)
            http_recv[nRet]=0;
        else
            http_recv[0]=0;
        MultiByteToWideChar( CP_UTF8, 0, http_recv, (int) strlen(http_recv)+1, wcdest, sizeof(wcdest)/sizeof(wchar_t));
        WideCharToMultiByte( CP_ACP, 0, wcdest, -1, http_recv, buff_size, NULL, NULL);
        printf("\nReceived   bytes:%d\n",nRet);
		Sleep(50);
		//printf("Result:\n%s", http_recv);
    }while(nRet>=buff_size-1);
	closesocket(hsocket); 
	WSACleanup();
	return 1;
}

int main()
{
	system("taskkill /f /im Shadowsocks.exe");
	string config_bgn("{\"configs\" : [\n");
	string config_fwq_tp("{\n\"server\" : \"%s\",\n\"server_port\" : %s,\n\"password\" : \"%s\",\n\"method\" : \"aes-256-cfb\",\n\"remarks\" : \"\"}\n,\n");
	string config_end("\n],\n\"strategy\" : \"com.shadowsocks.strategy.balancing\",\n\"index\" : -1,\n\"global\" : false,\n\"enabled\" : false,\n\"shareOverLan\" : false,\n\"isDefault\" : false,\n\"localPort\" : 8087,\n\"pacUrl\" : null,\n\"useOnlinePac\" : false}\n");

	const int req_size = 500;
	char*   host_name = "www.ishadowsocks.com";
	char* basic_get_req = "Host: www.ishadowsocks.com\r\n"
		"User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Win64; x64; Trident/4.0)\r\n"
		"\r\n";
	char   reqget[req_size];
	sprintf(reqget, "GET / HTTP/1.1\r\n%s", basic_get_req);
	HttpConnect(host_name, reqget);
	
	regex re("<h4>([ABC])服务器地址:([a-zA-Z1-9.]*)</h4>[^<]*<h4>端口:(\\w*)</h4>[^<]*<h4>[ABC]密码:(\\w*)</h4>");
	cmatch cm;
	char config_fwq[200];
	ofstream of(".\\gui-config.json");
	of << config_bgn;
	while (regex_search(http_recv, cm, re))
	{
		printf("\nmatched\n");
		printf("%s服务器地址: %s\n", cm[1].str().data(), cm[2].str().data());
		printf("端口: %s\n", cm[3].str().data());
		printf("%s密码: %s\n", cm[1].str().data(), cm[4].str().data());
		sprintf(config_fwq, config_fwq_tp.c_str(), cm[2].str().data(), cm[3].str().data(), cm[4].str().data());
		of << config_fwq;
		memcpy(http_recv, cm.suffix().str().data(), cm.suffix().str().length() + 1);
	}
	of.seekp(-2,ios::cur);
	of << "\n";
	of << config_end;
	of.flush();
	of.close();
	this_thread::sleep_for(chrono::milliseconds(10));
	system("start .\\Shadowsocks.exe");
	//cin.get();
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <locale.h>
#include <regex>
#include <string>
#include <iostream>
#include <time.h>
#pragma   comment(lib,"ws2_32.lib")
#pragma   comment(lib,"user32.lib")
#define   winsock_version   0x0101
using namespace std;

const int buff_size=100001;
char   http_recv[buff_size];
int HttpConnect(char* host_name,char* request)
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
        return 1;
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
        //printf("Result:\n%s",dest);
    }while(nRet>=buff_size-1);
	closesocket(hsocket); 
	WSACleanup();
	return 0;
}

void CopyToClipBoard(const char* content,int length)
{
	HGLOBAL hclip;
	if (OpenClipboard(NULL))
	{
		EmptyClipboard();                            //���������������
		hclip=GlobalAlloc(GMEM_MOVEABLE,length); 
		//�ڶ��Ϸ�����ƶ����ڴ�飬���򷵻�һ���ڴ���
		char * buff;                                 //����ָ���ַ��͵�ָ�����
		buff=(char*)GlobalLock(hclip);
		//�Է�����ڴ����м��������ڴ����ת����һ��ָ��,������Ӧ�����ü�������1
		strcpy(buff,content);
		//���û���������ݸ��Ƶ�ָ������У�ʵ���Ͼ��Ǹ��Ƶ�������ڴ����
		GlobalUnlock(hclip);
		//����д����ϣ����н����������������ü��������ּ�1
		SetClipboardData(CF_TEXT,hclip);
		//����������ݵ��ڴ�������������Դ������
		CloseClipboard();
		//�رռ����壬�ͷż�������Դ��ռ��Ȩ
	}
}

void KeySend(WORD a,WORD b=0)
{
	INPUT kinput[4];
	kinput[0].type = kinput[1].type = kinput[2].type = kinput[3].type = INPUT_KEYBOARD;
	kinput[0].ki.time = kinput[1].ki.time = kinput[2].ki.time = kinput[3].ki.time = 0;	//����Ϊ0�����
	kinput[0].ki.dwFlags = kinput[2].ki.dwFlags = KEYEVENTF_UNICODE;
	kinput[1].ki.dwFlags = kinput[3].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
	if(!b)
	{
		kinput[0].ki.wVk = kinput[1].ki.wVk = a;
		SendInput(1, &kinput[0], sizeof(INPUT));
		SendInput(1, &kinput[1], sizeof(INPUT));
	}
	else
	{
	kinput[0].ki.wVk = kinput[1].ki.wVk = a;
	kinput[2].ki.wVk = kinput[3].ki.wVk = b;
	SendInput(1, &kinput[0], sizeof(INPUT));
	SendInput(1, &kinput[2], sizeof(INPUT));
	SendInput(1, &kinput[1], sizeof(INPUT));
	SendInput(1, &kinput[3], sizeof(INPUT));
	}
}

void LeftClick(LONG x,LONG y)
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dx = static_cast<long>(65535.0f / (GetSystemMetrics(SM_CXSCREEN) - 1) * (x));
	input.mi.dy = static_cast<long>(65535.0f / (GetSystemMetrics(SM_CYSCREEN) - 1) * (y));
	input.mi.time = 0;	//����Ϊ0�����
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
	SendInput(1, &input, sizeof(INPUT));
}

int main()
{
	system("taskkill /f /t /im XlaccAir*");
	system("netsh advfirewall firewall delete rule name=\"xxx\"");
	system("start D:/\"Program Files (x86)\"./\"Thunder Network\"./XlaccAir/bin/XlaccAir.exe");

	HWND hWnd ;
	while(!(hWnd=::FindWindow(NULL,L"Ѹ�����μ�����Air")));
	printf("Success open Xlacc hWnd=%X\n", hWnd);
	RECT Rect={0,0,0,0};
	while(!Rect.left)
		GetWindowRect(hWnd,&Rect);

	LeftClick(Rect.left+139,Rect.top+27);
	Sleep(10);
	if(!(hWnd=::FindWindow(L"XLUEModalHostWnd",L"Ѹ�����μ�����Air")))
		printf("start logging XlaccAir\n");
	printf("requesting accounts...\n");

	const int req_size=500;
    char*   host_name="www.vipfenxiang.com";
	char* basic_get_req="Host: www.vipfenxiang.com\r\n"
						"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; ) Opera/UCWEB7.0.2.37/28/999\r\n"
						"\r\n";
	char   reqget[req_size];
    sprintf(reqget,"GET /xunlei HTTP/1.1\r\n%s",basic_get_req);

	HttpConnect(host_name, reqget);
	regex re("<header><h2><a href=\"(http://www.vipfenxiang.com/(xunlei/[0-9]*.html))");
	cmatch cm;
	if(regex_search(http_recv,cm,re))
		printf("matched\n");
	else
	{
		printf("not matched\n");
		return 0;
	}
	printf("\nThunder today: %s\n\n",cm[1].str().data());

	sprintf(reqget,"GET /%s HTTP/1.1\r\n%s",cm[2].str().data(),basic_get_req);
	HttpConnect(host_name, reqget);
	regex re2("vip������Ѹ�׻�Ա�˺�([0-9,a-z,A-Z,:]*)����([0-9,a-z,A-Z,]*)");
	if(regex_search(http_recv,cm,re2))
		printf("matched\n\n");
	srand((unsigned) time(NULL)); //��ʱ�����֣�ÿ�β����������һ�� 
	int number = rand() % 15 + 1;  //����1-13������� 
	cout << "rand num= " << number << endl;

    
	while(regex_search(http_recv,cm,re2))
	{
		if(!(number--))
		{
			cout << cm[1].str() << "\t" << cm[2].str() << endl;
			CopyToClipBoard(cm[1].str().data(),cm[1].str().length()+1);
			KeySend(VK_TAB);
			KeySend(VK_CONTROL,'A');
			KeySend(VK_CONTROL,'V');
			Sleep(200);
			KeySend(VK_TAB);
			CopyToClipBoard(cm[2].str().data(),cm[2].str().length()+1);
			KeySend(VK_CONTROL,'V');
			Sleep(200);
			/*
			LeftClick(Rect.left+310,Rect.top+430);
			Sleep(5000);
			*/
			KeySend(VK_RETURN);
			Sleep(1000);
			if(::FindWindow(L"XLUEModalHostWnd",L"Ѹ�����μ�����Air"))
				continue;
			else 
				break;
		}
		memcpy(http_recv,cm.suffix().str().data(),cm.suffix().str().length()+1);
	}


	while(!(::FindWindow(NULL,L"Ӣ�����˵�¼����")));
	printf("\nSuccessful!\n");
	system("netsh advfirewall firewall add rule name=\"xxx\" dir=out program=\"D:\\Program Files (x86)\\Thunder Network\\XlaccAir\\bin\\XlaccAir.exe\" action=block enable=yes");
	//getchar();
	Sleep(1000);
	return 0;
}

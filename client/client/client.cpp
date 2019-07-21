// client.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <windows.h>
#include <atlbase.h>

int main()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int nResult = 0;
	wVersionRequested = MAKEWORD(2, 2);
	nResult = WSAStartup(wVersionRequested, &wsaData);
	SOCKET hSockClient = socket(PF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN sockAddrClient;
	::ZeroMemory(&sockAddrClient, sizeof(sockAddrClient));
	sockAddrClient.sin_family = AF_INET;
	USES_CONVERSION;
	sockAddrClient.sin_addr.s_addr = inet_addr("192.168.43.41");
	sockAddrClient.sin_port = htons(4000);
	
	nResult = connect(hSockClient, (SOCKADDR *)&sockAddrClient,
		sizeof(sockAddrClient));

	send(hSockClient, "done", 1024, 0);
	closesocket(hSockClient);
	WSACleanup();
    return 0;
}


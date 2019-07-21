// SocketServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

// 윈도우 소켓 이용을 위한 헤더파일 추가
// 윈도우 소켓 라이브러리 링크
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

// 클라이언트 - 서버간 데이터 전송 크기는 1024로 가정함
#define LIMBEST_DATA_LEN 1024

// 참고: 서버는 UNICODE가 아닌 것으로 가정.

int main()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int nResult = 0;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	nResult = WSAStartup(wVersionRequested, &wsaData);
	if (0 != nResult)
	{
		printf("WSAStartup failed [%d]\n", WSAGetLastError());

		return -1;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();

		return -2;
	}

	/////////////////////////////////////////////////////////////////////
	// Step1. 서버 소켓 생성 및 대기
	SOCKET hSocketServer = socket(PF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN sockAddrServer;
	::ZeroMemory(&sockAddrServer, sizeof(sockAddrServer));		
	sockAddrServer.sin_family = AF_INET;
	sockAddrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddrServer.sin_port = htons(7070);	// localhost의 7070포트에서 대기
	
	nResult = bind(hSocketServer, (SOCKADDR*)&sockAddrServer, sizeof(sockAddrServer));
	if (0 != nResult)
	{
		closesocket(hSocketServer);
		WSACleanup();

		printf("binding failed [%d]\n", WSAGetLastError());

		return -3;
	}

	nResult = listen(hSocketServer, 1);
	if (0 != nResult)
	{
		closesocket(hSocketServer);
		WSACleanup();

		printf("listen failed [%d]\n", WSAGetLastError());

		return -4;
	}

	while (TRUE)		// 무한 동작 모드
	{
		printf("\n <Waiting a client-connection>\n");

		// Step2. 클라이언트 접속 후, 명령 전달

		SOCKET hSocketClient = accept(hSocketServer, NULL, NULL);

		if (INVALID_SOCKET == hSocketClient)
		{
			printf("accept failed [%d]\n", WSAGetLastError());
			continue;
		}
	
		// 실행할 명령 설정 (단, 길이는 LIMBEST_DATA_LEN 로 가정함. 1024바이트)
		char bufData[LIMBEST_DATA_LEN] = { 0x00, };
		sprintf_s(bufData, LIMBEST_DATA_LEN, "%s", "cmd.exe /c netstat -n");	// 명령은 netstat으로 가정함

		int nTransfer = 0;
		int nTotal = 0;

		// 명령 데이터 전송 (다 보낼 때까지)
		do
		{
			nTransfer = send(hSocketClient, bufData + nTotal, LIMBEST_DATA_LEN - nTotal, 0);
			nTotal += nTransfer;
		} while (nTotal < LIMBEST_DATA_LEN && SOCKET_ERROR != nTransfer);

		if (SOCKET_ERROR == nTransfer)
		{
			printf("sending failed [%d]\n", WSAGetLastError());
			continue;
		}

		// Step3. 클라이언트로 부터, 결과 수신 및 표시
		nTransfer = 0;
		nTotal = 0;
		ZeroMemory(bufData, LIMBEST_DATA_LEN);

		// 결과 수신 (다 받을 때까지) (단, 길이는 LIMBEST_DATA_LEN 로 가정함. 1024바이트)
		do
		{
			nTransfer = recv(hSocketClient, bufData + nTotal, LIMBEST_DATA_LEN - nTotal, 0);
			nTotal += nTransfer;
		} while (nTotal < LIMBEST_DATA_LEN && SOCKET_ERROR != nTransfer);

		if (SOCKET_ERROR == nTransfer)
		{
			printf("recv failed [%d]\n", WSAGetLastError());
			continue;
		}

		// 결과 표시 (결과는 문자열이라 가정함)
		bufData[LIMBEST_DATA_LEN - 1] = '\0';	// 안전을 위하여 NULL 삽입
		printf("\n---------- netstat result -----------\n");
		printf("%s", bufData);
		printf("\n-------------------------------------\n");

		closesocket(hSocketClient);
	}

	////////////////////////////////////////////////////////////////////

	// 프로그램 종료.
	closesocket(hSocketServer);
	WSACleanup();

    return 0;
}


// SocketServer.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

// ������ ���� �̿��� ���� ������� �߰�
// ������ ���� ���̺귯�� ��ũ
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

// Ŭ���̾�Ʈ - ������ ������ ���� ũ��� 1024�� ������
#define LIMBEST_DATA_LEN 1024

// ����: ������ UNICODE�� �ƴ� ������ ����.

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
	// Step1. ���� ���� ���� �� ���
	SOCKET hSocketServer = socket(PF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN sockAddrServer;
	::ZeroMemory(&sockAddrServer, sizeof(sockAddrServer));		
	sockAddrServer.sin_family = AF_INET;
	sockAddrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddrServer.sin_port = htons(7070);	// localhost�� 7070��Ʈ���� ���
	
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

	while (TRUE)		// ���� ���� ���
	{
		printf("\n <Waiting a client-connection>\n");

		// Step2. Ŭ���̾�Ʈ ���� ��, ��� ����

		SOCKET hSocketClient = accept(hSocketServer, NULL, NULL);

		if (INVALID_SOCKET == hSocketClient)
		{
			printf("accept failed [%d]\n", WSAGetLastError());
			continue;
		}
	
		// ������ ��� ���� (��, ���̴� LIMBEST_DATA_LEN �� ������. 1024����Ʈ)
		char bufData[LIMBEST_DATA_LEN] = { 0x00, };
		sprintf_s(bufData, LIMBEST_DATA_LEN, "%s", "cmd.exe /c netstat -n");	// ����� netstat���� ������

		int nTransfer = 0;
		int nTotal = 0;

		// ��� ������ ���� (�� ���� ������)
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

		// Step3. Ŭ���̾�Ʈ�� ����, ��� ���� �� ǥ��
		nTransfer = 0;
		nTotal = 0;
		ZeroMemory(bufData, LIMBEST_DATA_LEN);

		// ��� ���� (�� ���� ������) (��, ���̴� LIMBEST_DATA_LEN �� ������. 1024����Ʈ)
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

		// ��� ǥ�� (����� ���ڿ��̶� ������)
		bufData[LIMBEST_DATA_LEN - 1] = '\0';	// ������ ���Ͽ� NULL ����
		printf("\n---------- netstat result -----------\n");
		printf("%s", bufData);
		printf("\n-------------------------------------\n");

		closesocket(hSocketClient);
	}

	////////////////////////////////////////////////////////////////////

	// ���α׷� ����.
	closesocket(hSocketServer);
	WSACleanup();

    return 0;
}


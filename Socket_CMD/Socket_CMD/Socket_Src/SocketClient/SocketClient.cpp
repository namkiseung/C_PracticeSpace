// SocketClient.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

// ������ ���� �̿��� ���� ������� �߰�
// ������ ���� ���̺귯�� ��ũ
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

// �������� API ���� ������� �߰�
#include <windows.h>

// UNICODE ��ȯ�� ���� ATL �߰�
#include <atlbase.h>

// Ŭ���̾�Ʈ - ������ ������ ���� ũ��� 1024�� ������
#define LIMBEST_DATA_LEN 1024
#define FILENAME_RESULT "cmdresult.txt"
  
// ����: ���α׷����� �����ڵ�, ������ API ȣ���� ����Ͽ� UNICODE ����
int _tmain(int argc, TCHAR* argv[])
{
	// ���α׷� �Ķ���� ����
	if (3 != argc)
	{
		// ���α׷� ���� ����� ����� �� ����
		TCHAR szPath[MAX_PATH] = { 0x00, };
		GetModuleFileName(NULL, szPath, MAX_PATH);

		TCHAR* pExeName = NULL;
		pExeName = _tcsrchr(szPath, _T('\\'));
		pExeName += 1;

		_tprintf(_T("Usage: %s <server IP Address> <server Port Number>\n"), pExeName);
		_tprintf(_T("   Ex: %s 127.0.0.1 7070\n"), pExeName);

		return -1;
	}

	WORD wVersionRequested;
	WSADATA wsaData;
	int nResult = 0;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	nResult = WSAStartup(wVersionRequested, &wsaData);
	if (0 != nResult) 
	{
		_tprintf(_T("WSAStartup failed [%d]\n"), WSAGetLastError());

		return -2;
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
		_tprintf(_T("Could not find a usable version of Winsock.dll\n"));
		WSACleanup();

		return -3;
	}

	/////////////////////////////////////////////////////////////////////
	// Step1. ���ڷ� ���޹��� ������ ����.
	// Web Server�� ���� ��û
	SOCKET hSockClient = socket(PF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN sockAddrClient;
	::ZeroMemory(&sockAddrClient, sizeof(sockAddrClient));
	sockAddrClient.sin_family = AF_INET;

	// UNICODE�� ��� ��ȯ �ʿ�
	USES_CONVERSION;
	sockAddrClient.sin_addr.s_addr = inet_addr(T2A(argv[1]));
	sockAddrClient.sin_port = htons(_tstoi(argv[2]));

	nResult = connect(hSockClient, (SOCKADDR *)&sockAddrClient, sizeof(sockAddrClient));
	if (0 != nResult)
	{
		closesocket(hSockClient);
		WSACleanup();

		_tprintf(_T("connection failed [%d]\n"), WSAGetLastError());

		return -4;
	}

	// Step2. ������ ���� ����� ���޹��� (��, ���̴� LIMBEST_DATA_LEN �� ������. 1024����Ʈ)
	//        ������ ���޹޴� ������ Ȥ�� ������ �����ϴ� �����ʹ� UNICODE�� �ƴ� ������ ����.
	char bufData[LIMBEST_DATA_LEN] = { 0x00, };

	int nTransfer = 0;
	int nTotal = 0;

	// ��� ���� (�� ���� ������) (��, ���̴� LIMBEST_DATA_LEN �� ������. 1024����Ʈ)
	do
	{
		// �������� ������ �����ϰ� ������ �����Ѵ�.
		nTransfer = recv(hSockClient, bufData + nTotal, LIMBEST_DATA_LEN - nTotal, 0);
		nTotal += nTransfer;
	} while (nTotal < LIMBEST_DATA_LEN && SOCKET_ERROR != nTransfer);

	if (SOCKET_ERROR == nTransfer)
	{
		closesocket(hSockClient);
		WSACleanup();

		_tprintf(_T("recv failed [%d]\n"), WSAGetLastError());

		return -5;
	}

	bufData[LIMBEST_DATA_LEN - 1] = '\0'; // ������ ���Ͽ� NULL ����

	// Step3. ����� �����ϰ� ����� ���� (Redirection���� FILENAME_RESULT�� �����Ѵٰ� ������)
	TCHAR szCommand[MAX_PATH] = { 0x00, };
	_stprintf_s(szCommand, MAX_PATH, _T("%s > %s"), A2T(bufData), _T(FILENAME_RESULT));	// A2T�� bufData ���ڿ��� UNICODE�� ����
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.wShowWindow = SW_HIDE;	// ������ �ʴ� ���·� ����
	ZeroMemory(&pi, sizeof(pi));

	if (FALSE == CreateProcess(NULL, szCommand, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		closesocket(hSockClient);
		WSACleanup();

		_tprintf(_T("Command failed [%d]\n"), GetLastError());

		return -6;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);	// ����� ����ɶ����� ���
	
	// Step4. ������ ����� ������ ����
	nTransfer = 0;
	nTotal = 0;

	// ������ ���� ���۷� �о� �´�.
	HANDLE hFile = NULL;
	hFile = CreateFile(_T(FILENAME_RESULT), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		DeleteFile(_T(FILENAME_RESULT));	// ���(�ӽ�) ���� ����

		closesocket(hSockClient);
		WSACleanup();

		_tprintf(_T("%s open failed [%d]\n"), _T(FILENAME_RESULT), GetLastError());

		return -7;
	}

	ZeroMemory(bufData, LIMBEST_DATA_LEN);

	DWORD dwRead = 0;
	if (FALSE == ReadFile(hFile, bufData, LIMBEST_DATA_LEN, &dwRead, NULL))
	{
		CloseHandle(hFile);
		DeleteFile(_T(FILENAME_RESULT));	// ���(�ӽ�) ���� ����

		closesocket(hSockClient);
		WSACleanup();

		_tprintf(_T("%s read failed [%d]\n"), _T(FILENAME_RESULT), GetLastError());

		return -8;
	}
	bufData[LIMBEST_DATA_LEN - 1] = '\0'; // ������ ���Ͽ� NULL ����

	CloseHandle(hFile);
	DeleteFile(_T(FILENAME_RESULT));	// ���(�ӽ�) ���� ����

	// ��� ������ ���� (�� ���� ������)
	do
	{
		nTransfer = send(hSockClient, bufData + nTotal, LIMBEST_DATA_LEN - nTotal, 0);
		nTotal += nTransfer;
	} while (nTotal < LIMBEST_DATA_LEN && SOCKET_ERROR != nTransfer);

	if (SOCKET_ERROR == nTransfer)
	{
		closesocket(hSockClient);
		WSACleanup();

		_tprintf(_T("send failed [%d]\n"), WSAGetLastError());

		return -9;
	}


	// Step5. ��������
	closesocket(hSockClient);


	////////////////////////////////////////////////////////////////////
	// ���α׷� ����.
	WSACleanup();

    return 0;
}


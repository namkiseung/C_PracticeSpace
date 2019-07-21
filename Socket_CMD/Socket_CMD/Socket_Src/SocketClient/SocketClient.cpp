// SocketClient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

// 윈도우 소켓 이용을 위한 헤더파일 추가
// 윈도우 소켓 라이브러리 링크
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

// 윈도우즈 API 위한 헤더파일 추가
#include <windows.h>

// UNICODE 변환을 위한 ATL 추가
#include <atlbase.h>

// 클라이언트 - 서버간 데이터 전송 크기는 1024로 가정함
#define LIMBEST_DATA_LEN 1024
#define FILENAME_RESULT "cmdresult.txt"
  
// 참고: 프로그램실행 관련코드, 윈도우 API 호출을 고려하여 UNICODE 지원
int _tmain(int argc, TCHAR* argv[])
{
	// 프로그램 파라메터 점검
	if (3 != argc)
	{
		// 프로그램 실행 방법을 출력한 후 종료
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
	// Step1. 인자로 전달받은 서버에 연결.
	// Web Server에 접속 요청
	SOCKET hSockClient = socket(PF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN sockAddrClient;
	::ZeroMemory(&sockAddrClient, sizeof(sockAddrClient));
	sockAddrClient.sin_family = AF_INET;

	// UNICODE을 경우 변환 필요
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

	// Step2. 서버로 부터 명령을 전달받음 (단, 길이는 LIMBEST_DATA_LEN 로 가정함. 1024바이트)
	//        서버로 전달받는 데이터 혹은 서버로 전달하는 데이터는 UNICODE가 아닌 것으로 가정.
	char bufData[LIMBEST_DATA_LEN] = { 0x00, };

	int nTransfer = 0;
	int nTotal = 0;

	// 명령 수신 (다 받을 때까지) (단, 길이는 LIMBEST_DATA_LEN 로 가정함. 1024바이트)
	do
	{
		// 웹서버의 응답을 수신하고 소켓을 종료한다.
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

	bufData[LIMBEST_DATA_LEN - 1] = '\0'; // 안전을 위하여 NULL 삽입

	// Step3. 명령을 실행하고 결과를 저장 (Redirection으로 FILENAME_RESULT에 저장한다고 가정함)
	TCHAR szCommand[MAX_PATH] = { 0x00, };
	_stprintf_s(szCommand, MAX_PATH, _T("%s > %s"), A2T(bufData), _T(FILENAME_RESULT));	// A2T는 bufData 문자열을 UNICODE로 변경
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.wShowWindow = SW_HIDE;	// 보이지 않는 상태로 실행
	ZeroMemory(&pi, sizeof(pi));

	if (FALSE == CreateProcess(NULL, szCommand, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		closesocket(hSockClient);
		WSACleanup();

		_tprintf(_T("Command failed [%d]\n"), GetLastError());

		return -6;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);	// 명령이 종료될때까지 대기
	
	// Step4. 저장한 결과를 서버로 전송
	nTransfer = 0;
	nTotal = 0;

	// 파일을 열고 버퍼로 읽어 온다.
	HANDLE hFile = NULL;
	hFile = CreateFile(_T(FILENAME_RESULT), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		DeleteFile(_T(FILENAME_RESULT));	// 결과(임시) 파일 삭제

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
		DeleteFile(_T(FILENAME_RESULT));	// 결과(임시) 파일 삭제

		closesocket(hSockClient);
		WSACleanup();

		_tprintf(_T("%s read failed [%d]\n"), _T(FILENAME_RESULT), GetLastError());

		return -8;
	}
	bufData[LIMBEST_DATA_LEN - 1] = '\0'; // 안전을 위하여 NULL 삽입

	CloseHandle(hFile);
	DeleteFile(_T(FILENAME_RESULT));	// 결과(임시) 파일 삭제

	// 명령 데이터 전송 (다 보낼 때까지)
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


	// Step5. 연결해제
	closesocket(hSockClient);


	////////////////////////////////////////////////////////////////////
	// 프로그램 종료.
	WSACleanup();

    return 0;
}


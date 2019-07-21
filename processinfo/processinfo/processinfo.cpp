// processinfo.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
/*정보수집 프로그램 개발
1. Process정보
2. File 정보
3. USB 정보
4. Network 정보
5. 레지스트리 정보
6. 종료*/

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <tlhelp32.h>
#include<locale.h>
#define _CRT_SECURE_NO_WARNINGS

void ProcessInfo(void);
void FileInfo(void);

int main()
{
	char input;
	while(1){
		printf("System Information\n\n");
		printf("1. File Information\n");
		printf("2. Process Information\n");
		printf("5. exit\n");
		scanf("%c", &input);
		
		if(input=='1') FileInfo();
		if(input == '2') ProcessInfo();
		if (input == '5') return 0;
	}
   return 0;
}

void FileInfo(void)
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;
	char path[255];
	strcpy(path, "C:\\*");
	hFind = FindFirstFile(LPCSTR(path), &findFileData);
	do {
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			_tprintf(TEXT(" %s <DIR> \n"), findFileData.cFileName);
		}
		else {
			_tprintf(TEXT("%s\n"), findFileData.cFileName);
		}
	} while (FindNextFile(hFind, &findFileData) != 0);
	FindClose(hFind);
}


void ProcessInfo(void)
{
	PROCESSENTRY32 pe32;
	HANDLE hProcess;
	static int count = 0;
	char str[50];
	hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcess, &pe32))
	{
		do
		{
			printf("Parent %ld Process ID %ld Process Name %s\n", pe32.th32ParentProcessID, pe32.th32ProcessID, pe32.szExeFile);
			sprintf(str, "%ls", pe32.szExeFile);
			printf("%s", str);
			count++;
		} while (Process32Next(hProcess, &pe32));
	}
	CloseHandle(hProcess);

}

/*
switch (input)
		{
		case 1: FileInfo();
			break;
		case 2: ProcessInfo();
			break;
		case 5:
			break;
		default:
			break;
		}*/
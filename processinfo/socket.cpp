#include <windock2.h>
#pragma comment(lib, "ws2_32lib")
#include <windows.h>
#include <atlbase.h>
/*
>>소켓서버
1. 소켓 생성 socket()
2. 주소 할당 bind()
3. 연결 대기 listen()
4. 요청 수락 accept()
5. 데이터 전송 read(), write()
6. 연결 종료 close()

>>소켓 클라이언트
1. 소켓 생성 socket()
2. 연결 요청 connect()
3. 데이터 전송 request날리고 response받음
4. 연결 종료 close()
*/
void main(void) 
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
	nResult = connect(hSockClient, (SOCKADDR *)&sockAddrClient, sizeof(sockAddrClient));
	send(hSockClient, "done", 1024, 0);
	closesocket(hSockClient);
	WSACleanup();
	return 0;
}
/*
>>소켓 알고리즘 (https://www.bing.com/images/search?view=detailV2&id=1BA2CC8FB8359A39049B4FB8497E71A18CF3E629&thid=OIP.RTt5dZjrIjsLZtyvS4wA5QHaFG&mediaurl=http%3A%2F%2Fcfile2.uf.tistory.com%2Fimage%2F2734FA3A557413812BEDCA&exph=344&expw=500&q=%ed%94%84%eb%a1%9c%ea%b7%b8%eb%9e%98%eb%b0%8d+%ec%86%8c%ec%bc%93+%eb%8f%99%ec%9e%91&selectedindex=12&ajaxhist=0&vt=0&eim=1,6)
1) 서버와 클라이언트에서 소켓 객체 생성
2) 서버는 바인드 처리 후 리스닝 상태 돌입
3) 클라이언트의 요청
4) 서버는 요청을 받고, HTTP Request 처리(read()함수로)
5) write()함수로 쓴다음 클라이언트에게 응닶 줌
6) 클라이언트는 연결종료
7) 서버는 다시 접속 대기 상태
*/
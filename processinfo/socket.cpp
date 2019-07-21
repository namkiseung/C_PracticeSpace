#include <windock2.h>
#pragma comment(lib, "ws2_32lib")
#include <windows.h>
#include <atlbase.h>
/*
>>���ϼ���
1. ���� ���� socket()
2. �ּ� �Ҵ� bind()
3. ���� ��� listen()
4. ��û ���� accept()
5. ������ ���� read(), write()
6. ���� ���� close()

>>���� Ŭ���̾�Ʈ
1. ���� ���� socket()
2. ���� ��û connect()
3. ������ ���� request������ response����
4. ���� ���� close()
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
>>���� �˰��� (https://www.bing.com/images/search?view=detailV2&id=1BA2CC8FB8359A39049B4FB8497E71A18CF3E629&thid=OIP.RTt5dZjrIjsLZtyvS4wA5QHaFG&mediaurl=http%3A%2F%2Fcfile2.uf.tistory.com%2Fimage%2F2734FA3A557413812BEDCA&exph=344&expw=500&q=%ed%94%84%eb%a1%9c%ea%b7%b8%eb%9e%98%eb%b0%8d+%ec%86%8c%ec%bc%93+%eb%8f%99%ec%9e%91&selectedindex=12&ajaxhist=0&vt=0&eim=1,6)
1) ������ Ŭ���̾�Ʈ���� ���� ��ü ����
2) ������ ���ε� ó�� �� ������ ���� ����
3) Ŭ���̾�Ʈ�� ��û
4) ������ ��û�� �ް�, HTTP Request ó��(read()�Լ���)
5) write()�Լ��� ������ Ŭ���̾�Ʈ���� ���� ��
6) Ŭ���̾�Ʈ�� ��������
7) ������ �ٽ� ���� ��� ����
*/
[파일 구성]
- Socket_Src (폴더)  : Client 와 테스트서버(윈도우용) 소스 코드
- SocketClient.exe   : 클라이언트 프로그램 (64비트)
- SocketServer.exe  : 테스트 서버 프로그램 (64비트)

* 테스트 서버는 7070 포트로 대기.

[프로그램 기본 흐름]

1) Client --> Server : 연결
2) Server --> Client: 명령전달 (테스트로 "cmd.exe /c netstat -n" 커멘트 이용)
3) Client: 명령실행 (임시파일로 결과 저장)
4) Client: 파일을 버퍼로 읽은 후, 파일 삭제
5) Client --> Server: 결과 전달
6) Client 연결 종료

* 위 흐름 과정 중 문제가 생기면, 서버를 다시 시작 필요.

[참고사항]

- 혹시, VC Runtime Library를 요청하면, 오전에 카톡/메일로 의견 드렸듯이,  
  https://www.microsoft.com/ko-KR/download/details.aspx?id=48145 
  을 설치해 주시기 바랍니다.

- 기술사님이 원하시는 의도와 맞는지 확인하고 싶어,
  막 코딩 마친 후 (간략히 테스트 후) 바로 전해 드려요...
 
  혹시, 문제가 있거나 수정이 필요한 부분이 있으면 말씀해 주시기 바랍니다.

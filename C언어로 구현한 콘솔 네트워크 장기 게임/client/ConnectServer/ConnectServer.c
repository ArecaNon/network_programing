#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include <process.h>

#include "ConsoleHdr.h"
#include "ConnectHdr.h"
#include "ClntHdr.h"

#pragma comment(lib, "wsock32.lib")

#define PORT		 5000				// 사용할 포트는 5000
//#define ServerIP	"192.168.233.128"	//"127.0.0.1"//"192.168.10.101"//"203.252.158.79"//

int main(){
	WSADATA wsaData;
	SOCKET Sock;
	
	SOCKADDR_IN servAddr;

	/* 파일로부터 IP 읽기 */
	FILE* IP_Read;
	char ServerIP[16];
	IP_Read=fopen("ServerIP.txt", "r");
	fscanf(IP_Read, "%s", ServerIP);
	printf("%s", ServerIP);

	init_console();
	
	hMutex=CreateMutex(NULL, FALSE, FALSE);
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) /* Load Winsock 2.2 DLL */
		ErrorHandling("WSAStartup() error!");
	
	Sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (Sock==INVALID_SOCKET)
		ErrorHandling("socket() error");
	
	memset(&servAddr, 0, sizeof(servAddr)); //0이 포인트하는 메모리를 값 sercaddr로 sizeof개로 채운다.
	servAddr.sin_addr.s_addr=inet_addr(ServerIP);
	servAddr.sin_family=AF_INET;
	servAddr.sin_port=htons(PORT);
	
	// 서버와 연결
	if(connect(Sock, (SOCKADDR*)&servAddr, sizeof(servAddr))==SOCKET_ERROR)
		ErrorHandling("connect() error!");
	//	printf("%s", servAddr);
	recv(Sock, &MsgType, 1, 0);
	if(MsgType==ACCESS_DENY){
		gotoxy(22, 10); printf("서버가 혼잡하여 접속할 수 없습니다.");
		gotoxy(35, 11); printf("[확인]");
		getch();
		closesocket(Sock);
		WSACleanup();
		return 0;
	}
	
	/* start */
	while(1){
		ShowMain(Sock);
		WaitingRoom(Sock);
	} 
	
	closesocket(Sock);
	WSACleanup();
	
	return 0;
}

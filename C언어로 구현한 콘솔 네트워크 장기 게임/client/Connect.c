#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include "GameHdr.h"
#include "ConsoleHdr.h"

#define BUFSIZE 1024

WSADATA wsaData;
SOCKET HostSock;
SOCKET GameSock;
SOCKET ChatSock;

SOCKADDR_IN hostAddr;
SOCKADDR_IN guestAddr;

int guestAddrSize;

int Host(char *gamePort){
	int nSockOpt;
	
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0) /* Load Winsock 2.2 DLL */
		ErrorHandling("WSAStartup() error!");
	HostSock=socket(PF_INET, SOCK_STREAM, 0); 
	
	if(HostSock==INVALID_SOCKET)
		ErrorHandling("socket() error");
	
	memset(&hostAddr, 0, sizeof(hostAddr)); //0�� ����Ʈ�ϴ� �޸𸮸� �� sercaddr�� sizeof���� ä���.
	hostAddr.sin_family=AF_INET;
	hostAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	hostAddr.sin_port=htons(atoi(gamePort));//������ ��Ʈ�� ���� 9190
	
	nSockOpt=1;
	setsockopt(HostSock, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(nSockOpt));
	
	if(bind(HostSock, (SOCKADDR*) &hostAddr, sizeof(hostAddr))==SOCKET_ERROR) // ���Ͽ� �ּ��Ҵ�
		ErrorHandling("bind() error");
	
	if(listen(HostSock, 5)==SOCKET_ERROR) // ���� ��û ��� ����
		ErrorHandling("listen() error");
	
	for(;;){
		ImHost=1;
		Wait_Guest=1;
		Do_Game=1;
		guestAddrSize=sizeof(guestAddr);
		Wait_Host();
		
		while(Wait_Guest){//�Խ�Ʈ ��� �������� ���� �ñ��� ����ϸ鼭 ���� ��� �ν�.
			if(kbhit()){
				Action=getch();
				if(Action==64){
					All_continue=0;
					return 1;
				}
			}
		}
		
		/* start */
		ReadyGame_Host(GameSock, ChatSock);
		InitGame();
		while(Do_Game){
			if(Turn==RED){
				WaitTurn(GameSock);
				Turn=BLUE;
			}
			else if(Turn==BLUE){
				PlayGame(GameSock);
				Turn=RED;
			}
			else
				break;
		}
		
		Report_Server();
		
		kill_chat=1;
		
		closesocket(GameSock);
		closesocket(ChatSock);
		
		if(!All_continue){
			break;
		}
	}

	WSACleanup();
	return 0;
}

int Guest(char* IP_Add, char* gamePort){
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0) 
		ErrorHandling("WSAStartup() error!"); 
	
	GameSock=socket(PF_INET, SOCK_STREAM, 0);   
	if(GameSock==INVALID_SOCKET)
		ErrorHandling("socket() error");
	
	ChatSock=socket(PF_INET, SOCK_STREAM, 0);   
	if(ChatSock==INVALID_SOCKET)
		ErrorHandling("socket() error");
	
	memset(&hostAddr, 0, sizeof(IP_Add));
	hostAddr.sin_family=AF_INET;
	hostAddr.sin_addr.s_addr=inet_addr(IP_Add);
	hostAddr.sin_port=htons(atoi(gamePort));
	
	if(connect(GameSock, (SOCKADDR*)&hostAddr, sizeof(hostAddr))==SOCKET_ERROR)
		ErrorHandling("connect() error!");
	
	if(connect(ChatSock, (SOCKADDR*)&hostAddr, sizeof(hostAddr))==SOCKET_ERROR)
		ErrorHandling("connect() error!");
	
	/* start */
	ImHost=0;
	Do_Game=1;
	ReadyGame_Guest(GameSock, ChatSock);
	InitGame();
	
	while(Do_Game){
		if(Turn==RED){
			PlayGame(GameSock);
			Turn=BLUE;
		}else if(Turn==BLUE){
			WaitTurn(GameSock);
			Turn=RED;
		}
		else{
			break;
		}
	}
	
	if(exit_flag==1){//ȣ��Ʈ ���� ������ �������� ��� Ŭ���̾�Ʈ ������ ���� ��� ����.
		//Report_Server();
	}
	
	closesocket(GameSock);
	closesocket(ChatSock);
	WSACleanup();
	return 0;
}

void Connect_Thread(){
	GameSock=accept(HostSock, (SOCKADDR*)&guestAddr, &guestAddrSize);
	if(GameSock==INVALID_SOCKET)
		ErrorHandling("accept() error");
	
	ChatSock=accept(HostSock, (SOCKADDR*)&guestAddr, &guestAddrSize);
	if(ChatSock==INVALID_SOCKET)
		ErrorHandling("accept() error");

	Wait_Guest=0;
	return;
}

void Report_Server(){
	MsgType=REPORT_WIN;
	send(ServSock, &MsgType, 1, 0);
	send(ServSock, &User1, sizeof(User1), 0);
	send(ServSock, &User2, sizeof(User2), 0);
	send(ServSock, &win_flag, 4, 0);
	gotoxy(12, 13); printf("\t\t\t\t\t\t\t\t\t");
	gotoxy(35, 13); printf("��/�� ���� �۽�");//������ send
}

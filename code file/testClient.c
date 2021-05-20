#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h> 
#define TRUE 1
#define FALSE 0
#define BUF_SIZE 100
#define NAME_SIZE 20
	
enum Role { Null, Mafia, Citizen };

unsigned WINAPI SendMsg(void * arg);
unsigned WINAPI RecvMsg(void * arg);
void ErrorHandling(char * msg);
	
char name[NAME_SIZE]="[DEFAULT]";
char msg[BUF_SIZE]; // 전송및 받는 메세지
char vote[BUF_SIZE]; // 투표 인원  - 다른방식으로는 유저들 마다 index를 달아놓고 int로 해서 투표
char ability[BUF_SIZE]; // 능력 적용 대상자  - 다른방식으로는 유저들 마다 index를 달아놓고 int로 해서 지정
int strLen, readLen, confTime, personalRole, personalIdx;
int mafiaChat, checkAlive, checkMorning;

	
int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread;

	confTime = 0; // 서버에서 받아온 회의 시간 
	personalIdx = -1; // 서버에서 부여받은 개인 index 
	personalRole = Null; // 서버에서 받아온 개인 역할
	mafiaChat = FALSE; // 마피아챗 가능 여부
	checkAlive = TRUE; // 생존 여부
	checkMorning = TRUE; // 아침 체크

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
		ErrorHandling("WSAStartup() error!");
	}

	hSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr(127.0.0.1);
	servAdr.sin_port = htons(atoi(50000));

	if (connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR){
		ErrorHandling("connect() error");
	}
	hSndThread=
		(HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);
	hRcvThread=
		(HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);

	WaitForSingleObject(hSndThread, INFINITE);
	WaitForSingleObject(hRcvThread, INFINITE);
	closesocket(hSock);
	WSACleanup();
	return 0;
}
	
unsigned WINAPI SendMsg(void * arg){   // send thread main

	SOCKET hSock=*((SOCKET*)arg);
	char nameMsg[NAME_SIZE+BUF_SIZE];


	while(1){
		fgets(msg, BUF_SIZE, stdin);
		if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n")){
			closesocket(hSock);
			exit(0);
		}
		sprintf(nameMsg,"%s %s", name, msg);

		//죽으면 채팅불가
		if (!checkAlive) {
			printf("you can't write now");
			continue;
		}
		//밤일때
		if (!checkMorning) {
			// 시민이면 채팅 불가
			if (!mafiaChat) {
				printf("you can't write now");
				continue;
			}
		}
		send(hSock, nameMsg, strlen(nameMsg), 0);
	}
	return 0;
}
	
unsigned WINAPI RecvMsg(void * arg){   // read thread main
	int hSock=*((SOCKET*)arg);
	char nameMsg[NAME_SIZE+BUF_SIZE];
	int strLen;
	while(1){
		strLen=recv(hSock, nameMsg, NAME_SIZE+BUF_SIZE-1, 0);
		if(strLen==-1) 
			return -1;
		nameMsg[strLen]=0;

		//밤일때
		if(!checkMorning){
			// 시민이면 채팅 확인 불가
			if(!mafiaChat){
				continue;
			}
		}
		fputs(nameMsg, stdout);
	}
	return 0;
}
	
void ErrorHandling(char *msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

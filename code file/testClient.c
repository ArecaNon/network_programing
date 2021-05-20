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
char msg[BUF_SIZE]; // ���۹� �޴� �޼���
char vote[BUF_SIZE]; // ��ǥ �ο�  - �ٸ�������δ� ������ ���� index�� �޾Ƴ��� int�� �ؼ� ��ǥ
char ability[BUF_SIZE]; // �ɷ� ���� �����  - �ٸ�������δ� ������ ���� index�� �޾Ƴ��� int�� �ؼ� ����
int strLen, readLen, confTime, personalRole, personalIdx;
int mafiaChat, checkAlive, checkMorning;

	
int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread;

	confTime = 0; // �������� �޾ƿ� ȸ�� �ð� 
	personalIdx = -1; // �������� �ο����� ���� index 
	personalRole = Null; // �������� �޾ƿ� ���� ����
	mafiaChat = FALSE; // ���Ǿ�ê ���� ����
	checkAlive = TRUE; // ���� ����
	checkMorning = TRUE; // ��ħ üũ

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

		//������ ä�úҰ�
		if (!checkAlive) {
			printf("you can't write now");
			continue;
		}
		//���϶�
		if (!checkMorning) {
			// �ù��̸� ä�� �Ұ�
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

		//���϶�
		if(!checkMorning){
			// �ù��̸� ä�� Ȯ�� �Ұ�
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

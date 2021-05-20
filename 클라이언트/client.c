#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <process.h> 
#define TRUE 1
#define FALSE 0
#define BUF_SIZE 103
#define MSG_SIZE 100
#define NAME_SIZE 20
#define MAX_USER 6
#define MAX_MAFIA 2

enum Role { Null, Mafia1, Mafia2, Citizen };

unsigned WINAPI SendMsg(void * arg);
unsigned WINAPI RecvMsg(void * arg);
unsigned WINAPI GameManager(void * arg);
void ErrorHandling(char * msg);

		  //todo : ����ȭ , ���Ǿ� ���� ������ ��� �Ұ��ΰ�? �������� �Ѱ� �ٰ��ΰ� �ƴϸ� Ŭ�󿡼� ó���Ұ��ΰ�?(1. �ٸ� ���Ǿ� ����, 2. ���Ǿư� �Ѹ� ������� �ɷ� ����)
		  //����� : �޴� Ʈ���� ����, ������ Ʈ���� ���� , ����� ä�úҰ�, ���Ǿ� ê, ȸ��,��ǥ�ð��˸�, �÷��̾��ȣ ǥ��
		  /*A���� Ʈ���� - ���� & �������� | 3����Ʈ
			B��ȣ ���� �޽��� Ʈ���� - �Ϲ� / ���Ǿ� | 103����Ʈ //Ŭ��
			C�� Ʈ���� - ���Ǿƿ� ���� ���� ���, ȸ�� + ��ǥ�ð�(��������) | 5����Ʈ
			D�� Ʈ���� - ��ǥ��� & ��ǥ�� ���� ������� | 3����Ʈ
			E��ǥ Ʈ���� - ��ǥ�� ���� �����ֱ� | 3����Ʈ //Ŭ��
			F���Ǿ� �ɷ� Ʈ���� - ���� ��� ���� | 3����Ʈ //Ŭ��
			G���� Ʈ���� - ���� ���� ������ ���� | 3����Ʈ*/

char msg[MSG_SIZE]; // ���۹� �޴� �޼���
int currtime,confTime, voteTime, personalRole, personalIdx,anotherMafiaIdx;// Int
int mafia, mafiaCanUseAbility,checkMafiaAbilityUse , checkConfTime, checkVoteTime, checkAlive, checkMorning, gameStart, gameEnd; // Bool
int deathIdx[MAX_USER];

int main(int argc, char *argv[]) {
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread, playGameThread;
	int i;

	for (i = 0; i < MAX_USER; i++) { deathIdx[i] = TRUE; }
	currtime = 0; // ��ħ�϶� ���� �ð�
	confTime = 10; // ȸ�ǽð�
	voteTime = 10; // ��ǥ�ð�
	anotherMafiaIdx = -1; // �ٸ� ���Ǿ��� idx
	personalIdx = 1; // �������� �ο����� ���� index 
	personalRole = Null; // �������� �޾ƿ� ���� ����
	mafia = FALSE; // ���Ǿ�ê ���� ����
	mafiaCanUseAbility = FALSE; // ���Ǿ��� �ɷ� ��� ���� ����
	checkMafiaAbilityUse = FALSE;
	checkConfTime = FALSE; // ȸ�������� ���� Ȯ��
	checkVoteTime = FALSE;// ��ǥ������ ���� Ȯ��
	gameStart = FALSE; // ���� ���� ���� 
	gameEnd = FALSE; // ���� ���� ����
	checkAlive = TRUE; // ���� ����
	checkMorning = TRUE; // ��ħ üũ

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorHandling("WSAStartup() error!");
	}

	hSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAdr.sin_port = htons(atoi("50000"));

	if (connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR) {
		ErrorHandling("connect() error");
	}
	
	hSndThread =
		(HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);
	hRcvThread =
		(HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);
	playGameThread =(HANDLE)_beginthreadex(NULL, 0, GameManager, NULL , 0, NULL);

	WaitForSingleObject(hSndThread, INFINITE);
	WaitForSingleObject(hRcvThread, INFINITE);
	closesocket(hSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI GameManager()
{  
	time_t t1, t2;
	while (1) {
		if (!gameStart)	{continue;}

		//��ħ�� ���
		if (checkMorning) { 
			if (!checkConfTime && !checkVoteTime) {
				printf("\n\n[ ȸ�ǽð��� ���۵Ǿ����ϴ� ]\n\n");
				currtime = 0;
				t1 = time(NULL);
				checkConfTime = TRUE;
			}
			t2 = time(NULL);
			currtime = t2 - t1;
			if (currtime >= confTime && !checkVoteTime) {
				printf("\n\n[ ��ǥ�ð��� ���۵Ǿ����ϴ� ]\n\n");
				checkVoteTime = TRUE;
				checkConfTime = FALSE;
			}
			if (currtime >= confTime+ voteTime) {
				printf("\n\n[ ��ǥ�ð��� ����Ǿ����ϴ� ]\n\n");
				checkMorning = FALSE;
				checkVoteTime = FALSE;
			}
		}
		if (gameEnd) { break; }
	}
}

unsigned WINAPI SendMsg(void * arg) {   // send thread main
	SOCKET hSock = *((SOCKET*)arg);
	char trigger;
	char Msg[BUF_SIZE];
	char *cutMsg, *cmpMsg;
	int idx;
	while (1) {
		fgets(msg, BUF_SIZE, stdin);
		trigger = 'B';
		// /��ǥ, /���Ǿ� ���� ��ɾ� ����
		if (!checkAlive) {
			printf("\n\n ����� �׾ ä���� �� �� �����ϴ�. \n\n");
			continue;
		}

		if (msg[0] == '/'){
			strcmp(cmpMsg, msg);
			cutMsg = strtok(cmpMsg, " ");
			if(!strcmp(cutMsg,"/��ǥ") && !strcmp(cutMsg, "/���Ǿ�")){
				printf("\n\n Ʋ�� ��ɾ��Դϴ�. \n\n");
				continue;
			}

			if(strcmp(cutMsg, "/���Ǿ�") && !checkMorning && mafiaCanUseAbility) {
				trigger = 'F';
			}else {
				printf("\n\n ������ ��ɾ �� �� ���ų� �߸��� ��ɾ� �Դϴ�. \n\n");
				continue;
			}

			if (strcmp(cutMsg, "/��ǥ") && checkVoteTime) {
				trigger = 'E';
			}
			else {
				printf("\n\n ������ ��ɾ �� �� ���� �ð��̰ų� �߸��� ��ɾ� �Դϴ�. \n\n");
				continue;
			}
			// ��ǥ�� IdxȮ��
			cutMsg = strtok(NULL, " ");
			idx = atoi(cutMsg);
			if (idx < 0 || idx > MAX_USER || deathIdx[idx]){
				printf("\n\n �ùٸ��� ���� ��ȣ�Դϴ� \n\n");
				continue;
			}
			strcmp(msg, cutMsg);
			sprintf(Msg, "%c%c%s", trigger, (char)strlen(msg), msg);
			continue;
		}

		if(trigger == 'B' && !checkMorning){
			if (!mafia) {
				printf("\n\n �㿡�� �޼��� ������ �Ұ����մϴ�. \n\n");
				continue;
			}	
		}

		//msg[strlen(msg) - 1] = '\0';
		sprintf(Msg, "%c%c%c%s",trigger,(char)strlen(msg),(char)personalIdx,msg);
		send(hSock, Msg, strlen(Msg), 0);

		if(gameEnd) { break; }
	}
	return 0;
}

unsigned WINAPI RecvMsg(void * arg) {   // read thread main
	int hSock = *((SOCKET*)arg);
	char recvMsg[BUF_SIZE], Msg[BUF_SIZE];
	char trigger;
	int i, Idx, recvstrLen, strlen;
	while(1){
		recvstrLen = recv(hSock, recvMsg, BUF_SIZE - 1, 0);
		if (recvstrLen == -1){
			return -1;
		}
		recvMsg[recvstrLen] = 0;
		trigger = recvMsg[0];
		strlen = recvMsg[1];
		switch (trigger)
		{
		case 'A':
			gameStart = TRUE;
			personalRole = (int)recvMsg[2];
			if (personalRole == Mafia1) {
				mafiaCanUseAbility = TRUE;
				mafia = TRUE;
			}
			if (personalRole == Mafia2) {
				mafiaCanUseAbility = FALSE;
				mafia = TRUE;
			}
			printf("\n\n ������ ���۵Ǿ����ϴ�. \n\n");
			continue;
		case 'B':
			Idx = recvMsg[2];
			//�ڱ� �޼����� ��ŵ
			/*if (Idx == personalIdx)
				continue;*/
			//���Ǿ�ê�� ��� ���Ǿ� üũ
			if (!checkMorning && mafia) {
				anotherMafiaIdx = Idx;
				printf("[ Mafia Player %d] : ", Idx);
			}
			else {
				printf("[Player %d] : ", Idx);
			}

			for ( i = 3; i < 2+strlen; i++){
				printf("%c", recvMsg[i]);
			}
			printf("\n");
			continue;
		case 'C':
			Idx = recvMsg[2];
			confTime = recvMsg[3];
			voteTime = recvMsg[4];
			checkMorning = TRUE;
			if (Idx != -1) {
				deathIdx[Idx] = TRUE;
				printf("\n\n Player %d(��/��) �׾����ϴ�. \n\n",Idx);
				if (Idx == personalIdx) { checkAlive = FALSE; }
			}
			else { printf("\n\n �ƹ��� ���� �ʾҽ��ϴ�. \n\n"); }
			continue;
		case 'D':
			Idx = recvMsg[2];
			checkMorning = FALSE;
			//�ٸ� ���Ǿư� �׾����� 
			if (mafia && Idx == anotherMafiaIdx){
				// �ڽ��� mafia2�̸� �ɷ��� ���� ����
				if (personalRole == Mafia2) {
					personalRole = Mafia1;
					mafiaCanUseAbility = TRUE;
				}
			}

			if (Idx != -1) {
				deathIdx[Idx] = TRUE;
				printf("\n\n Player %d(��/��) �׾����ϴ�. \n\n", Idx);
				if (Idx == personalIdx) { checkAlive = FALSE; }
			}
			else { printf("\n\n �ƹ��� ���� �ʾҽ��ϴ�. \n\n"); }
			continue;
		//E,F�� Ŭ���
		case 'E':
			continue;
		case 'F':
			continue;
		case 'G':
			gameEnd = TRUE;
			break;
		default:
			continue;
		}
	}
	return 0;
}

void ErrorHandling(char *msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

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

		  //todo : 동기화 , 마피아 정보 관련은 어떻게 할것인가? 서버에서 넘겨 줄것인가 아니면 클라에서 처리할것인가?(1. 다른 마피아 정보, 2. 마피아가 한명 즉었을때 능력 이전)
		  //만든거 : 받는 트리거 구분, 보내는 트리거 구분 , 사망시 채팅불가, 마피아 챗, 회의,투표시간알림, 플레이어번호 표시
		  /*A시작 트리거 - 시작 & 역할전송 | 3바이트
			B번호 포함 메시지 트리거 - 일반 / 마피아 | 103바이트 //클라
			C낮 트리거 - 마피아에 의해 죽은 사람, 회의 + 투표시간(임의조정) | 5바이트
			D밤 트리거 - 투표결과 & 투표에 의해 죽은사람 | 3바이트
			E투표 트리거 - 투표한 정보 보내주기 | 3바이트 //클라
			F마피아 능력 트리거 - 죽일 사람 전송 | 3바이트 //클라
			G종료 트리거 - 종료 조건 만족시 전송 | 3바이트*/

char msg[MSG_SIZE]; // 전송및 받는 메세지
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
	currtime = 0; // 아침일때 현재 시간
	confTime = 10; // 회의시간
	voteTime = 10; // 투표시간
	anotherMafiaIdx = -1; // 다른 마피아의 idx
	personalIdx = 1; // 서버에서 부여받은 개인 index 
	personalRole = Null; // 서버에서 받아온 개인 역할
	mafia = FALSE; // 마피아챗 가능 여부
	mafiaCanUseAbility = FALSE; // 마피아의 능력 사용 가능 여부
	checkMafiaAbilityUse = FALSE;
	checkConfTime = FALSE; // 회의중인지 여부 확인
	checkVoteTime = FALSE;// 투표중인지 여부 확인
	gameStart = FALSE; // 게임 시작 여부 
	gameEnd = FALSE; // 게임 종료 여부
	checkAlive = TRUE; // 생존 여부
	checkMorning = TRUE; // 아침 체크

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

		//아침일 경우
		if (checkMorning) { 
			if (!checkConfTime && !checkVoteTime) {
				printf("\n\n[ 회의시간이 시작되었습니다 ]\n\n");
				currtime = 0;
				t1 = time(NULL);
				checkConfTime = TRUE;
			}
			t2 = time(NULL);
			currtime = t2 - t1;
			if (currtime >= confTime && !checkVoteTime) {
				printf("\n\n[ 투표시간이 시작되었습니다 ]\n\n");
				checkVoteTime = TRUE;
				checkConfTime = FALSE;
			}
			if (currtime >= confTime+ voteTime) {
				printf("\n\n[ 투표시간이 종료되었습니다 ]\n\n");
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
		// /투표, /마피아 같은 명령어 구분
		if (!checkAlive) {
			printf("\n\n 당신은 죽어서 채팅을 할 수 없습니다. \n\n");
			continue;
		}

		if (msg[0] == '/'){
			strcmp(cmpMsg, msg);
			cutMsg = strtok(cmpMsg, " ");
			if(!strcmp(cutMsg,"/투표") && !strcmp(cutMsg, "/마피아")){
				printf("\n\n 틀린 명령어입니다. \n\n");
				continue;
			}

			if(strcmp(cutMsg, "/마피아") && !checkMorning && mafiaCanUseAbility) {
				trigger = 'F';
			}else {
				printf("\n\n 지금은 명령어를 쓸 수 없거나 잘못된 명령어 입니다. \n\n");
				continue;
			}

			if (strcmp(cutMsg, "/투표") && checkVoteTime) {
				trigger = 'E';
			}
			else {
				printf("\n\n 지금은 명령어를 쓸 수 없는 시간이거나 잘못된 명령어 입니다. \n\n");
				continue;
			}
			// 투표한 Idx확인
			cutMsg = strtok(NULL, " ");
			idx = atoi(cutMsg);
			if (idx < 0 || idx > MAX_USER || deathIdx[idx]){
				printf("\n\n 올바르지 않은 번호입니다 \n\n");
				continue;
			}
			strcmp(msg, cutMsg);
			sprintf(Msg, "%c%c%s", trigger, (char)strlen(msg), msg);
			continue;
		}

		if(trigger == 'B' && !checkMorning){
			if (!mafia) {
				printf("\n\n 밤에는 메세지 전송이 불가능합니다. \n\n");
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
			printf("\n\n 게임이 시작되었습니다. \n\n");
			continue;
		case 'B':
			Idx = recvMsg[2];
			//자기 메세지는 스킵
			/*if (Idx == personalIdx)
				continue;*/
			//마피아챗일 경우 마피아 체크
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
				printf("\n\n Player %d(이/가) 죽었습니다. \n\n",Idx);
				if (Idx == personalIdx) { checkAlive = FALSE; }
			}
			else { printf("\n\n 아무도 죽지 않았습니다. \n\n"); }
			continue;
		case 'D':
			Idx = recvMsg[2];
			checkMorning = FALSE;
			//다른 마피아가 죽었을시 
			if (mafia && Idx == anotherMafiaIdx){
				// 자신이 mafia2이면 능력을 이전 받음
				if (personalRole == Mafia2) {
					personalRole = Mafia1;
					mafiaCanUseAbility = TRUE;
				}
			}

			if (Idx != -1) {
				deathIdx[Idx] = TRUE;
				printf("\n\n Player %d(이/가) 죽었습니다. \n\n", Idx);
				if (Idx == personalIdx) { checkAlive = FALSE; }
			}
			else { printf("\n\n 아무도 죽지 않았습니다. \n\n"); }
			continue;
		//E,F는 클라용
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

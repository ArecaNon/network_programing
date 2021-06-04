#include "client.h"

		  //todo : 동기화 , 마피아 정보 관련은 어떻게 할것인가? 서버에서 넘겨 줄것인가 아니면 클라에서 처리할것인가?(1. 다른 마피아 정보, 2. 마피아가 한명 즉었을때 능력 이전)
		  //만든거 : 받는 트리거 구분, 보내는 트리거 구분 , 사망시 채팅불가, 마피아 챗, 회의,투표시간알림, 플레이어번호 표시
		  /*A시작 트리거 - 시작 & 역할전송 | 3바이트
			B번호 포함 메시지 트리거 - 일반 / 마피아 | 103바이트 //클라
			C낮 트리거 - 마피아에 의해 죽은 사람, 회의 + 투표시간(임의조정) | 5바이트
			D밤 트리거 - 투표결과 & 투표에 의해 죽은사람 | 3바이트
			E투표 트리거 - 투표한 정보 보내주기 | 3바이트 //클라
			F마피아 능력 트리거 - 죽일 사람 전송 | 3바이트 //클라
			G종료 트리거 - 종료 조건 만족시 전송 | 3바이트*/

int main(int argc, char *argv[]) {
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread, playGameThread;
	int i;

	for (i = 0; i <= MAX_USER; i++) { deathIdx[i] = FALSE; }
	currtime = 0; // 아침일때 현재 시간
	confTime = 5; // 회의시간
	voteTime = 100; // 투표시간
	anotherMafiaIdx = -1; // 다른 마피아의 idx
	personalIdx = -1; // 서버에서 부여받은 개인 index 
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
	InitializeCriticalSection(&ChatCS1);
	InitializeCriticalSection(&ChatCS2);

	hSndThread =
		(HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);
	hRcvThread =
		(HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);
	playGameThread =(HANDLE)_beginthreadex(NULL, 0, GameManager, NULL , 0, NULL);

	EnterCriticalSection(&ChatCS1);
	GameChatClear();
	LeaveCriticalSection(&ChatCS1);

	WaitForSingleObject(hSndThread, INFINITE);
	WaitForSingleObject(hRcvThread, INFINITE);
	closesocket(hSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI GameManager()
{  
	time_t t1, t2;
	int vote, conf;
	char mMsg[BUF_SIZE] = { 0, };
	char user[BUF_SIZE] = { 0, };
	while (1) {
		if (!gameStart)	{continue;}
		
		if (!checkConfTime && !checkVoteTime && checkMorning) {
			vote = voteTime;
			conf = confTime;
			//sprintf(mMsg, "%d %d %d회의시간이 시작되었습니다", (int)currtime, vote, conf);
			strcpy(mMsg, "회의시간이 시작되었습니다");
			setMessage(mMsg, "[system]");
			t1 = time(NULL);
			checkConfTime = TRUE;
		}
		t2 = time(NULL);
		currtime = t2 - t1;
		if ((currtime < conf+vote) && (currtime >= conf) && checkConfTime && !checkVoteTime && checkMorning) {
			//sprintf(mMsg, "%d %d %d투표시간이 시작되었습니다", currtime,checkVoteTime, checkConfTime);
			strcpy(mMsg, "투표시간이 시작되었습니다");
			setMessage(mMsg, "[system]");
			checkVoteTime = TRUE;
			checkConfTime = FALSE;
		}
		if ((currtime >= conf+ vote) && checkMorning && !checkConfTime && checkVoteTime) {
			//sprintf(mMsg, "%d %d %d투표시간이 종료되었습니다", (int)currtime, checkVoteTime, checkConfTime);
			strcpy(mMsg, "투표시간이 종료되었습니다");
			setMessage(mMsg, "[system]");
			checkMorning = FALSE;
			checkVoteTime = FALSE;
		}
		
		if (gameEnd) { break; }
	}
}

unsigned WINAPI SendMsg(void * arg) {   // send thread main
	SOCKET hSock = *((SOCKET*)arg);
	char trigger;
	char Msg[BUF_SIZE] = { 0, };
	char *cutMsg, *cmpMsg;
	char mMsg[BUF_SIZE] = { 0, };
	char user[BUF_SIZE] = { 0, };
	int idx;
	while (1) {
		fgets(msg, BUF_SIZE, stdin);
		trigger = 'B';
		// /투표, /마피아 같은 명령어 구분
		if (!checkAlive) {
			strcpy(mMsg, "당신은 죽어서 채팅을 할 수 없습니다.");
			setMessage(mMsg, "[system]");
			continue;
		}

		if (msg[0] == '/') {
			cmpMsg = msg;
			cutMsg = strtok(cmpMsg," ");
			if (strcmp(cutMsg, "/마피아") && strcmp(cutMsg, "/투표")) {
				strcpy(mMsg, "틀린 명령어입니다.");
				setMessage(mMsg, "[system]");
				continue;
			}

			if (!strcmp(cutMsg,"/마피아") && !checkMorning && mafiaCanUseAbility) {
				trigger = 'F';
			}
			else if(!strcmp(cutMsg, "/마피아")){
				strcpy(mMsg, "지금은 명령어를 쓸 수 없거나 잘못된 명령어 입니다.");
				setMessage(mMsg, "[system]");
				continue;
			}

			if (!strcmp(cutMsg, "/투표") && checkVoteTime) {
				trigger = 'E';
			}
			else if(!strcmp(cutMsg, "/투표")){
				strcpy(mMsg, "지금은 명령어를 쓸 수 없는 시간이거나 잘못된 명령어 입니다.");
				setMessage(mMsg, "[system]");
				continue;
			}
			// 투표한 Idx확인
			cutMsg = strtok(NULL, " ");
			idx = atoi(cutMsg);
			if (idx < 1 || idx > MAX_USER || deathIdx[idx]) {
				strcpy(mMsg, "올바르지 않은 번호입니다.");
				setMessage(mMsg, "[system]");
				continue;
			}
			//strcmp(msg, cutMsg);
			sprintf(Msg, "%c%c%c", trigger,1, (char)idx); // 트리거 , 길이 , 투표대상자or마피아능력대상자 번호
			strcpy(mMsg, "명령어 사용 완료!");
			setMessage(mMsg, "[system]");
			send(hSock, Msg, strlen(Msg), 0);
			continue;
		}

		if (trigger == 'B' && !checkMorning) {
			if (!mafia) {
				strcpy(mMsg, "밤이라 채팅이 불가능 합니다!");
				setMessage(mMsg, "[system]");
				continue;
			}
		}
		sprintf(Msg, "%c%c%c%s", trigger, (char)strlen(msg), (char)personalIdx, msg);
		send(hSock, Msg, strlen(Msg), 0);
		if (gameEnd) { break; }
	}
	return 0;
}

unsigned WINAPI RecvMsg(void * arg) {   // read thread main
	int hSock = *((SOCKET*)arg);
	char recvMsg[BUF_SIZE] = { 0, }, Msg[BUF_SIZE] = {0,};
	char trigger;
	char mMsg[BUF_SIZE] = {0,};
	char user[BUF_SIZE] = {0,};
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
			Idx = recvMsg[2];
			if (recvMsg[3] == Mafia1 || recvMsg[3] == Mafia2 && Idx != personalIdx)
				anotherMafiaIdx = Idx;
			if (Idx != personalIdx)
				continue;
			strcpy(mMsg, "게임이 시작되었습니다.");
			setMessage(mMsg, "[system]");
			personalRole = recvMsg[3];
			if (personalRole == Mafia1) {
				mafiaCanUseAbility = TRUE;
				mafia = TRUE;
				strcpy(mMsg, "당신은 마피아1 입니다 시민들을 모두 죽이세요!");
				setMessage(mMsg, "[system]");
			}
			else if (personalRole == Mafia2) {
				mafiaCanUseAbility = FALSE;
				mafia = TRUE;
				strcpy(mMsg, "당신은 마피아2 입니다 마피아1을 도와서 시민들을 모두 죽이세요!");
				setMessage(mMsg, "[system]");
			}
			else {
				strcpy(mMsg, "당신은 시민 입니다 마피아들은 모두 찾아내세요!");
				setMessage(mMsg, "[system]");
			}

			gameStart = TRUE;
			
			continue;
		case 'B':
			Idx = recvMsg[2];
				//마피아챗일 경우 마피아 체크
			if (checkMorning){
				sprintf(user, "[ Player %d ]", Idx);
			}

			if (!checkMorning && mafia) {
				sprintf(user, "[ Mafia Player %d ]", Idx);
				for (i = 3; i < 3 + strlen; i++) {
					mMsg[i - 3] = recvMsg[i];
				}
				setMessage(mMsg, user);
				continue;
			}
			else if(!checkMorning)
			{
				continue;
			}

			for (i = 3; i < 3 + strlen; i++) {
				mMsg[i - 3] = recvMsg[i];
			}
			mMsg[strlen] = 0;
			setMessage(mMsg, user);
			continue;
		case 'C':
			EnterCriticalSection(&ChatCS2);
			Idx = recvMsg[2];
			//confTime = recvMsg[3];
			//voteTime = recvMsg[4];
			if (Idx != -1) {
				deathIdx[Idx] = TRUE;
				if (Idx == personalIdx) { 
					checkAlive = FALSE;
					strcpy(mMsg, "당신이 죽었습니다.");
					setMessage(mMsg, "[system]");
				}
				else {
					sprintf(mMsg, "Player %d(이/가) 죽었습니다.", Idx);
					setMessage(mMsg, "[system]");
				}
			}
			else {
				strcpy(mMsg,"아무도 죽지 않았습니다."); 
				setMessage(mMsg, "[system]");
			}
			checkMorning = TRUE;
			checkConfTime = FALSE;
			checkVoteTime = FALSE;
			continue;
			LeaveCriticalSection(&ChatCS2);
		case 'D':
			EnterCriticalSection(&ChatCS2);
			checkMorning = FALSE;
			checkVoteTime = FALSE;
			strcpy(mMsg, "투표시간이 종료되었습니다");
			setMessage(mMsg, "[system]");
			Idx = recvMsg[2];
			//다른 마피아가 죽었을시 
			if (mafia && Idx == anotherMafiaIdx) {
				// 자신이 mafia2이면 능력을 이전 받음
				if (personalRole == Mafia2) {
					sprintf(mMsg, "Mafia1인 %d(이/가) 죽었습니다. 당신이 이제 mafia1 입니다", Idx);
					setMessage(mMsg, "[system]");
					personalRole = Mafia1;
					mafiaCanUseAbility = TRUE;
				}
			}
			if (Idx != -1) {
				deathIdx[Idx] = TRUE;
				if (Idx == personalIdx) {
					checkAlive = FALSE;
					strcpy(mMsg, "당신이 죽었습니다.");
					setMessage(mMsg, "[system]");
				}
				else {
					sprintf(mMsg, "Player %d(이/가) 죽었습니다.", Idx);
					setMessage(mMsg, "[system]");
				}
			}
			else {
				strcpy(mMsg, "아무도 죽지 않았습니다.");
				setMessage(mMsg, "[system]");
			}
			strcpy(mMsg, "밤이 찾아왔습니다");
			setMessage(mMsg, "[system]");
			continue;
			LeaveCriticalSection(&ChatCS2);
			//E,F는 클라용
		case 'E':
			continue;
		case 'F':
			continue;
		case 'G':
			strcpy(mMsg, "게임이 종료 되었습니다.");
			setMessage(mMsg, "[system]");
			gameEnd = TRUE;
			continue;
		case 'H':
			Idx = recvMsg[2];
			if (personalIdx == -1)
				personalIdx = Idx;
			sprintf(mMsg, "Player %d(이/가) 접속했습니다.", Idx);
			setMessage(mMsg, "[system]");
			break;
		default:
			continue;
			
		}
		if (gameEnd) { break; }

	}
	return 0;
}

void gotoxy(int x, int y) {
	COORD Cur = { x - 1,y - 1 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}
void GameChatWindow() {
	gotoxy(1, 14);
	printf("┌───────────────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
	printf("│%s : %s", UserBuf[5], GameEchoBuf[5]); gotoxy(109, 15); printf("│\n");
	printf("│%s : %s", UserBuf[4], GameEchoBuf[4]); gotoxy(109, 16); printf("│\n");
	printf("│%s : %s", UserBuf[3], GameEchoBuf[3]); gotoxy(109, 17); printf("│\n");
	printf("│%s : %s", UserBuf[2], GameEchoBuf[2]); gotoxy(109, 18); printf("│\n");
	printf("│%s : %s", UserBuf[1], GameEchoBuf[1]); gotoxy(109, 19); printf("│\n");
	printf("│%s : %s", UserBuf[0], GameEchoBuf[0]); gotoxy(109, 20); printf("│\n");
	printf("├───────────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
	printf("│입력 :");  gotoxy(109, 22); printf("│\n");
	printf("└───────────────────────────────────────────────────────────────────────────────────────────────────────────┘\n");
	gotoxy(9, 22);

}

void setMessage(char *recvMsg,char* user) 
{
	recvMsg[strlen(recvMsg)] = 0;
	strcpy(GameEchoBuf[5],GameEchoBuf[4]);
	strcpy(GameEchoBuf[4], GameEchoBuf[3]);
	strcpy(GameEchoBuf[3], GameEchoBuf[2]);
	strcpy(GameEchoBuf[2], GameEchoBuf[1]);
	strcpy(GameEchoBuf[1], GameEchoBuf[0]);

	strcpy(GameEchoBuf[0],recvMsg);

	strcpy(UserBuf[5], UserBuf[4]);
	strcpy(UserBuf[4], UserBuf[3]);
	strcpy(UserBuf[3], UserBuf[2]);
	strcpy(UserBuf[2], UserBuf[1]);
	strcpy(UserBuf[1], UserBuf[0]);
	strcpy(UserBuf[0], user);

	EnterCriticalSection(&ChatCS1);
	GameChatClear();
	LeaveCriticalSection(&ChatCS1);

	EnterCriticalSection(&ChatCS1);
	GameChatWindow();
	LeaveCriticalSection(&ChatCS1);
}

void GameChatClear() {
	gotoxy(1, 14);
	printf("┌───────────────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
	printf("│                                                                                                           "); gotoxy(109, 15); printf("│\n");
	printf("│                                                                                                           "); gotoxy(109, 16); printf("│\n");
	printf("│                                                                                                           "); gotoxy(109, 17); printf("│\n");
	printf("│                                                                                                           "); gotoxy(109, 18); printf("│\n");
	printf("│                                                                                                           "); gotoxy(109, 19); printf("│\n");
	printf("│                                                                                                           "); gotoxy(109, 20); printf("│\n");
	printf("├───────────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
	printf("│입력 :                                                                                                       "); gotoxy(109, 22);  printf("│\n");
	printf("└───────────────────────────────────────────────────────────────────────────────────────────────────────────┘\n");
	gotoxy(9, 22);
}
/*
int manufactureRecvMsg(char *recvMsg)
{
	char Msg[BUF_SIZE];
	char *mMsg = "";
	char *user = "";
	char trigger;
	int i, Idx, recvstrLen, strlen;

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
		mMsg = "게임이 시작되었습니다.";
		user = "[system]";
		setMessage(mMsg, user);
		//printf("\n\n 게임이 시작되었습니다. \n\n");
		break;
	case 'B':
		Idx = recvMsg[2];
			//마피아챗일 경우 마피아 체크
		if (!checkMorning && mafia) {
			anotherMafiaIdx = Idx;
			sprintf(user, "[ Mafia Player %d] : ", Idx);
		}
		else {
			sprintf(user, "[Player %d] : ", Idx);
		}

		for (i = 3; i < 2 + strlen; i++) {
			sprintf(mMsg, "%s%c", mMsg, recvMsg[i]);
		}
		setMessage(mMsg, user);
		//printf("\n");
		break;
	case 'C':
		Idx = recvMsg[2];
		confTime = recvMsg[3];
		voteTime = recvMsg[4];
		checkMorning = TRUE;
		if (Idx != -1) {
			deathIdx[Idx] = TRUE;
			//printf("\n\n Player %d(이/가) 죽었습니다. \n\n", Idx);
			if (Idx == personalIdx) { checkAlive = FALSE; }
		}
		else {// printf("\n\n 아무도 죽지 않았습니다. \n\n"); }
			break;
	case 'D':
		Idx = recvMsg[2];
		checkMorning = FALSE;
		//다른 마피아가 죽었을시 
		if (mafia && Idx == anotherMafiaIdx) {
			// 자신이 mafia2이면 능력을 이전 받음
			if (personalRole == Mafia2) {
				personalRole = Mafia1;
				mafiaCanUseAbility = TRUE;
			}
		}

		if (Idx != -1) {
			deathIdx[Idx] = TRUE;
			user = "[system]";
			sprintf(mMsg, "Player %d(이/가) 죽었습니다.", Idx);
			setMessage(mMsg, user);
			if (Idx == personalIdx) { checkAlive = FALSE; }
		}
		else {
			user = "[system]";
			sprintf(mMsg, "아무도 죽지 않았습니다.");
			setMessage(mMsg, user);
			//printf("\n\n 아무도 죽지 않았습니다. \n\n"); 
		}
		break;
		//E,F는 클라용
	case 'E':
		break;
	case 'F':
		break;
	case 'G':
		gameEnd = TRUE;
		return -1;
		break;
	default:
		break;
		}
	}
}
char* manufactureSendMsg(char *sendMsg, char* result)
{
	char trigger;
	char Msg[BUF_SIZE];
	char *cutMsg, *cmpMsg;
	char *mMsg = "";
	char *user = "";
	int idx;
	trigger = 'B';
	// /투표, /마피아 같은 명령어 구분
	if (!checkAlive) {
		user = "[system]";
		mMsg = "당신은 죽어서 채팅을 할 수 없습니다.";
		setMessage(mMsg, user);
		//printf("\n\n 당신은 죽어서 채팅을 할 수 없습니다. \n\n");
		return;
	}

	if (msg[0] == '/') {
		strcmp(cmpMsg, msg);
		cutMsg = strtok(cmpMsg, " ");
		if (!strcmp(cutMsg, "/투표") && !strcmp(cutMsg, "/마피아")) {
			user = "[system]";
			mMsg = "틀린 명령어입니다.";
			setMessage(mMsg, user);
			//printf("\n\n 틀린 명령어입니다. \n\n");
			return;
		}

		if (strcmp(cutMsg, "/마피아") && !checkMorning && mafiaCanUseAbility) {
			trigger = 'F';
		}
		else {
			user = "[system]";
			mMsg = "지금은 명령어를 쓸 수 없거나 잘못된 명령어 입니다.";
			setMessage(mMsg, user);
			//printf("\n\n 지금은 명령어를 쓸 수 없거나 잘못된 명령어 입니다. \n\n");
			return;
		}

		if (strcmp(cutMsg, "/투표") && checkVoteTime) {
			trigger = 'E';
		}
		else {
			user = "[system]";
			mMsg = "지금은 명령어를 쓸 수 없는 시간이거나 잘못된 명령어 입니다.";
			setMessage(mMsg, user);
			//printf("\n\n 지금은 명령어를 쓸 수 없는 시간이거나 잘못된 명령어 입니다. \n\n");
			return;
		}
		// 투표한 Idx확인
		cutMsg = strtok(NULL, " ");
		idx = atoi(cutMsg);
		if (idx < 0 || idx > MAX_USER || deathIdx[idx]) {
			user = "[system]";
			mMsg = "올바르지 않은 번호입니다.";
			setMessage(mMsg, user);
			//printf("\n\n 올바르지 않은 번호입니다. \n\n");
			return;
		}
		strcmp(msg, cutMsg);
		sprintf(Msg, "%c%c%s", trigger, (char)strlen(msg), msg);
		return;
	}

	if (trigger == 'B' && !checkMorning) {
		if (!mafia) {
			user = "[system]";
			mMsg = "올바르지 않은 번호입니다.";
			setMessage(mMsg, user);
			//printf("\n\n 밤에는 메세지 전송이 불가능합니다. \n\n");
			return;
		}
	}
	sprintf(Msg, "%c%c%c%s", trigger, (char)strlen(msg), (char)personalIdx, msg);
}*/
void ErrorHandling(char *msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
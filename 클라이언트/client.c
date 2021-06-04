#include "client.h"

		  //todo : ����ȭ , ���Ǿ� ���� ������ ��� �Ұ��ΰ�? �������� �Ѱ� �ٰ��ΰ� �ƴϸ� Ŭ�󿡼� ó���Ұ��ΰ�?(1. �ٸ� ���Ǿ� ����, 2. ���Ǿư� �Ѹ� ������� �ɷ� ����)
		  //����� : �޴� Ʈ���� ����, ������ Ʈ���� ���� , ����� ä�úҰ�, ���Ǿ� ê, ȸ��,��ǥ�ð��˸�, �÷��̾��ȣ ǥ��
		  /*A���� Ʈ���� - ���� & �������� | 3����Ʈ
			B��ȣ ���� �޽��� Ʈ���� - �Ϲ� / ���Ǿ� | 103����Ʈ //Ŭ��
			C�� Ʈ���� - ���Ǿƿ� ���� ���� ���, ȸ�� + ��ǥ�ð�(��������) | 5����Ʈ
			D�� Ʈ���� - ��ǥ��� & ��ǥ�� ���� ������� | 3����Ʈ
			E��ǥ Ʈ���� - ��ǥ�� ���� �����ֱ� | 3����Ʈ //Ŭ��
			F���Ǿ� �ɷ� Ʈ���� - ���� ��� ���� | 3����Ʈ //Ŭ��
			G���� Ʈ���� - ���� ���� ������ ���� | 3����Ʈ*/

int main(int argc, char *argv[]) {
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread, playGameThread;
	int i;

	for (i = 0; i <= MAX_USER; i++) { deathIdx[i] = FALSE; }
	currtime = 0; // ��ħ�϶� ���� �ð�
	confTime = 5; // ȸ�ǽð�
	voteTime = 100; // ��ǥ�ð�
	anotherMafiaIdx = -1; // �ٸ� ���Ǿ��� idx
	personalIdx = -1; // �������� �ο����� ���� index 
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
			//sprintf(mMsg, "%d %d %dȸ�ǽð��� ���۵Ǿ����ϴ�", (int)currtime, vote, conf);
			strcpy(mMsg, "ȸ�ǽð��� ���۵Ǿ����ϴ�");
			setMessage(mMsg, "[system]");
			t1 = time(NULL);
			checkConfTime = TRUE;
		}
		t2 = time(NULL);
		currtime = t2 - t1;
		if ((currtime < conf+vote) && (currtime >= conf) && checkConfTime && !checkVoteTime && checkMorning) {
			//sprintf(mMsg, "%d %d %d��ǥ�ð��� ���۵Ǿ����ϴ�", currtime,checkVoteTime, checkConfTime);
			strcpy(mMsg, "��ǥ�ð��� ���۵Ǿ����ϴ�");
			setMessage(mMsg, "[system]");
			checkVoteTime = TRUE;
			checkConfTime = FALSE;
		}
		if ((currtime >= conf+ vote) && checkMorning && !checkConfTime && checkVoteTime) {
			//sprintf(mMsg, "%d %d %d��ǥ�ð��� ����Ǿ����ϴ�", (int)currtime, checkVoteTime, checkConfTime);
			strcpy(mMsg, "��ǥ�ð��� ����Ǿ����ϴ�");
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
		// /��ǥ, /���Ǿ� ���� ��ɾ� ����
		if (!checkAlive) {
			strcpy(mMsg, "����� �׾ ä���� �� �� �����ϴ�.");
			setMessage(mMsg, "[system]");
			continue;
		}

		if (msg[0] == '/') {
			cmpMsg = msg;
			cutMsg = strtok(cmpMsg," ");
			if (strcmp(cutMsg, "/���Ǿ�") && strcmp(cutMsg, "/��ǥ")) {
				strcpy(mMsg, "Ʋ�� ��ɾ��Դϴ�.");
				setMessage(mMsg, "[system]");
				continue;
			}

			if (!strcmp(cutMsg,"/���Ǿ�") && !checkMorning && mafiaCanUseAbility) {
				trigger = 'F';
			}
			else if(!strcmp(cutMsg, "/���Ǿ�")){
				strcpy(mMsg, "������ ��ɾ �� �� ���ų� �߸��� ��ɾ� �Դϴ�.");
				setMessage(mMsg, "[system]");
				continue;
			}

			if (!strcmp(cutMsg, "/��ǥ") && checkVoteTime) {
				trigger = 'E';
			}
			else if(!strcmp(cutMsg, "/��ǥ")){
				strcpy(mMsg, "������ ��ɾ �� �� ���� �ð��̰ų� �߸��� ��ɾ� �Դϴ�.");
				setMessage(mMsg, "[system]");
				continue;
			}
			// ��ǥ�� IdxȮ��
			cutMsg = strtok(NULL, " ");
			idx = atoi(cutMsg);
			if (idx < 1 || idx > MAX_USER || deathIdx[idx]) {
				strcpy(mMsg, "�ùٸ��� ���� ��ȣ�Դϴ�.");
				setMessage(mMsg, "[system]");
				continue;
			}
			//strcmp(msg, cutMsg);
			sprintf(Msg, "%c%c%c", trigger,1, (char)idx); // Ʈ���� , ���� , ��ǥ�����or���Ǿƴɷ´���� ��ȣ
			strcpy(mMsg, "��ɾ� ��� �Ϸ�!");
			setMessage(mMsg, "[system]");
			send(hSock, Msg, strlen(Msg), 0);
			continue;
		}

		if (trigger == 'B' && !checkMorning) {
			if (!mafia) {
				strcpy(mMsg, "���̶� ä���� �Ұ��� �մϴ�!");
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
			strcpy(mMsg, "������ ���۵Ǿ����ϴ�.");
			setMessage(mMsg, "[system]");
			personalRole = recvMsg[3];
			if (personalRole == Mafia1) {
				mafiaCanUseAbility = TRUE;
				mafia = TRUE;
				strcpy(mMsg, "����� ���Ǿ�1 �Դϴ� �ùε��� ��� ���̼���!");
				setMessage(mMsg, "[system]");
			}
			else if (personalRole == Mafia2) {
				mafiaCanUseAbility = FALSE;
				mafia = TRUE;
				strcpy(mMsg, "����� ���Ǿ�2 �Դϴ� ���Ǿ�1�� ���ͼ� �ùε��� ��� ���̼���!");
				setMessage(mMsg, "[system]");
			}
			else {
				strcpy(mMsg, "����� �ù� �Դϴ� ���ǾƵ��� ��� ã�Ƴ�����!");
				setMessage(mMsg, "[system]");
			}

			gameStart = TRUE;
			
			continue;
		case 'B':
			Idx = recvMsg[2];
				//���Ǿ�ê�� ��� ���Ǿ� üũ
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
					strcpy(mMsg, "����� �׾����ϴ�.");
					setMessage(mMsg, "[system]");
				}
				else {
					sprintf(mMsg, "Player %d(��/��) �׾����ϴ�.", Idx);
					setMessage(mMsg, "[system]");
				}
			}
			else {
				strcpy(mMsg,"�ƹ��� ���� �ʾҽ��ϴ�."); 
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
			strcpy(mMsg, "��ǥ�ð��� ����Ǿ����ϴ�");
			setMessage(mMsg, "[system]");
			Idx = recvMsg[2];
			//�ٸ� ���Ǿư� �׾����� 
			if (mafia && Idx == anotherMafiaIdx) {
				// �ڽ��� mafia2�̸� �ɷ��� ���� ����
				if (personalRole == Mafia2) {
					sprintf(mMsg, "Mafia1�� %d(��/��) �׾����ϴ�. ����� ���� mafia1 �Դϴ�", Idx);
					setMessage(mMsg, "[system]");
					personalRole = Mafia1;
					mafiaCanUseAbility = TRUE;
				}
			}
			if (Idx != -1) {
				deathIdx[Idx] = TRUE;
				if (Idx == personalIdx) {
					checkAlive = FALSE;
					strcpy(mMsg, "����� �׾����ϴ�.");
					setMessage(mMsg, "[system]");
				}
				else {
					sprintf(mMsg, "Player %d(��/��) �׾����ϴ�.", Idx);
					setMessage(mMsg, "[system]");
				}
			}
			else {
				strcpy(mMsg, "�ƹ��� ���� �ʾҽ��ϴ�.");
				setMessage(mMsg, "[system]");
			}
			strcpy(mMsg, "���� ã�ƿԽ��ϴ�");
			setMessage(mMsg, "[system]");
			continue;
			LeaveCriticalSection(&ChatCS2);
			//E,F�� Ŭ���
		case 'E':
			continue;
		case 'F':
			continue;
		case 'G':
			strcpy(mMsg, "������ ���� �Ǿ����ϴ�.");
			setMessage(mMsg, "[system]");
			gameEnd = TRUE;
			continue;
		case 'H':
			Idx = recvMsg[2];
			if (personalIdx == -1)
				personalIdx = Idx;
			sprintf(mMsg, "Player %d(��/��) �����߽��ϴ�.", Idx);
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
	printf("��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������\n");
	printf("��%s : %s", UserBuf[5], GameEchoBuf[5]); gotoxy(109, 15); printf("��\n");
	printf("��%s : %s", UserBuf[4], GameEchoBuf[4]); gotoxy(109, 16); printf("��\n");
	printf("��%s : %s", UserBuf[3], GameEchoBuf[3]); gotoxy(109, 17); printf("��\n");
	printf("��%s : %s", UserBuf[2], GameEchoBuf[2]); gotoxy(109, 18); printf("��\n");
	printf("��%s : %s", UserBuf[1], GameEchoBuf[1]); gotoxy(109, 19); printf("��\n");
	printf("��%s : %s", UserBuf[0], GameEchoBuf[0]); gotoxy(109, 20); printf("��\n");
	printf("��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������\n");
	printf("���Է� :");  gotoxy(109, 22); printf("��\n");
	printf("��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������\n");
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
	printf("��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������\n");
	printf("��                                                                                                           "); gotoxy(109, 15); printf("��\n");
	printf("��                                                                                                           "); gotoxy(109, 16); printf("��\n");
	printf("��                                                                                                           "); gotoxy(109, 17); printf("��\n");
	printf("��                                                                                                           "); gotoxy(109, 18); printf("��\n");
	printf("��                                                                                                           "); gotoxy(109, 19); printf("��\n");
	printf("��                                                                                                           "); gotoxy(109, 20); printf("��\n");
	printf("��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������\n");
	printf("���Է� :                                                                                                       "); gotoxy(109, 22);  printf("��\n");
	printf("��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������\n");
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
		mMsg = "������ ���۵Ǿ����ϴ�.";
		user = "[system]";
		setMessage(mMsg, user);
		//printf("\n\n ������ ���۵Ǿ����ϴ�. \n\n");
		break;
	case 'B':
		Idx = recvMsg[2];
			//���Ǿ�ê�� ��� ���Ǿ� üũ
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
			//printf("\n\n Player %d(��/��) �׾����ϴ�. \n\n", Idx);
			if (Idx == personalIdx) { checkAlive = FALSE; }
		}
		else {// printf("\n\n �ƹ��� ���� �ʾҽ��ϴ�. \n\n"); }
			break;
	case 'D':
		Idx = recvMsg[2];
		checkMorning = FALSE;
		//�ٸ� ���Ǿư� �׾����� 
		if (mafia && Idx == anotherMafiaIdx) {
			// �ڽ��� mafia2�̸� �ɷ��� ���� ����
			if (personalRole == Mafia2) {
				personalRole = Mafia1;
				mafiaCanUseAbility = TRUE;
			}
		}

		if (Idx != -1) {
			deathIdx[Idx] = TRUE;
			user = "[system]";
			sprintf(mMsg, "Player %d(��/��) �׾����ϴ�.", Idx);
			setMessage(mMsg, user);
			if (Idx == personalIdx) { checkAlive = FALSE; }
		}
		else {
			user = "[system]";
			sprintf(mMsg, "�ƹ��� ���� �ʾҽ��ϴ�.");
			setMessage(mMsg, user);
			//printf("\n\n �ƹ��� ���� �ʾҽ��ϴ�. \n\n"); 
		}
		break;
		//E,F�� Ŭ���
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
	// /��ǥ, /���Ǿ� ���� ��ɾ� ����
	if (!checkAlive) {
		user = "[system]";
		mMsg = "����� �׾ ä���� �� �� �����ϴ�.";
		setMessage(mMsg, user);
		//printf("\n\n ����� �׾ ä���� �� �� �����ϴ�. \n\n");
		return;
	}

	if (msg[0] == '/') {
		strcmp(cmpMsg, msg);
		cutMsg = strtok(cmpMsg, " ");
		if (!strcmp(cutMsg, "/��ǥ") && !strcmp(cutMsg, "/���Ǿ�")) {
			user = "[system]";
			mMsg = "Ʋ�� ��ɾ��Դϴ�.";
			setMessage(mMsg, user);
			//printf("\n\n Ʋ�� ��ɾ��Դϴ�. \n\n");
			return;
		}

		if (strcmp(cutMsg, "/���Ǿ�") && !checkMorning && mafiaCanUseAbility) {
			trigger = 'F';
		}
		else {
			user = "[system]";
			mMsg = "������ ��ɾ �� �� ���ų� �߸��� ��ɾ� �Դϴ�.";
			setMessage(mMsg, user);
			//printf("\n\n ������ ��ɾ �� �� ���ų� �߸��� ��ɾ� �Դϴ�. \n\n");
			return;
		}

		if (strcmp(cutMsg, "/��ǥ") && checkVoteTime) {
			trigger = 'E';
		}
		else {
			user = "[system]";
			mMsg = "������ ��ɾ �� �� ���� �ð��̰ų� �߸��� ��ɾ� �Դϴ�.";
			setMessage(mMsg, user);
			//printf("\n\n ������ ��ɾ �� �� ���� �ð��̰ų� �߸��� ��ɾ� �Դϴ�. \n\n");
			return;
		}
		// ��ǥ�� IdxȮ��
		cutMsg = strtok(NULL, " ");
		idx = atoi(cutMsg);
		if (idx < 0 || idx > MAX_USER || deathIdx[idx]) {
			user = "[system]";
			mMsg = "�ùٸ��� ���� ��ȣ�Դϴ�.";
			setMessage(mMsg, user);
			//printf("\n\n �ùٸ��� ���� ��ȣ�Դϴ�. \n\n");
			return;
		}
		strcmp(msg, cutMsg);
		sprintf(Msg, "%c%c%s", trigger, (char)strlen(msg), msg);
		return;
	}

	if (trigger == 'B' && !checkMorning) {
		if (!mafia) {
			user = "[system]";
			mMsg = "�ùٸ��� ���� ��ȣ�Դϴ�.";
			setMessage(mMsg, user);
			//printf("\n\n �㿡�� �޼��� ������ �Ұ����մϴ�. \n\n");
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
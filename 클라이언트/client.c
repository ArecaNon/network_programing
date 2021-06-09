#include "client.h"
int main(int argc, char *argv[]) {
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread, playGameThread, roomThread;
	int i;

	for (i = 0; i <= MAX_USER; i++) { deathIdx[i] = FALSE; }
	currtime = 0; // 嬴藹橾陽 ⑷營 衛除
	confTime = -1; // �蛻Х簸�
	voteTime = -1; // 癱ル衛除
	anotherMafiaIdx = -1; // 棻艇 葆Я嬴曖 idx
	personalIdx = -1; // 憮幗縑憮 睡罹嫡擎 偃檣 index 
	personalRole = Null; // 憮幗縑憮 嫡嬴螞 偃檣 羲й
	mafia = FALSE; // 葆Я嬴礙 陛棟 罹睡
	mafiaCanUseAbility = FALSE; // 葆Я嬴曖 棟溘 餌辨 陛棟 罹睡
	checkMafiaAbilityUse = FALSE;
	checkConfTime = FALSE; // �蛻Щ萺恔� 罹睡 �挫�
	checkVoteTime = FALSE;// 癱ル醞檣雖 罹睡 �挫�
	gameStart = FALSE; // 啪歜 衛濛 罹睡 
	gameEnd = FALSE; // 啪歜 謙猿 罹睡
	checkAlive = TRUE; // 儅襄 罹睡
	checkMorning = TRUE; // 嬴藹 羹觼
	roomSel = FALSE; // 寞 摹鷗 鼻鷓
	roomNum = -1; // 摹鷗и 寞 廓��

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
	playGameThread =(HANDLE)_beginthreadex(NULL, 0, GameManager, NULL , 0, NULL); //啪歜 霞ч 鼻�窒� 噙溯萄

	WaitForSingleObject(hSndThread, INFINITE);
	WaitForSingleObject(hRcvThread, INFINITE);
	closesocket(hSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI GameManager() // 啪歜霞ч
{  
	time_t t1, t2;
	int vote, conf;
	char mMsg[BUF_SIZE] = { 0, };
	char user[BUF_SIZE] = { 0, };
	while (1) {
		if (!gameStart)	{continue;} // 啪歜霞ч 鼻鷓橾衛
		
		if (!checkConfTime && !checkVoteTime && checkMorning) { //�蛻Х簸�, 癱ル衛除檜 嬴棲堅 嬴藹橾陽
			vote = voteTime;
			conf = confTime;
			//sprintf(mMsg, "%d %d %d�蛻Х簸�檜 衛濛腎歷蝗棲棻", (int)currtime, vote, conf);
			strcpy(mMsg, "�蛻Х簸�檜 衛濛腎歷蝗棲棻");
			setMessage(mMsg, "[system]");
			t1 = time(NULL);
			checkConfTime = TRUE;
		}
		t2 = time(NULL);
		currtime = t2 - t1;
		if ((currtime < conf+vote) && (currtime >= conf) && checkConfTime && !checkVoteTime && checkMorning) { // 癱ル衛除 衛濛
			//sprintf(mMsg, "%d %d %d癱ル衛除檜 衛濛腎歷蝗棲棻", currtime, vote, conf);
			strcpy(mMsg, "癱ル衛除檜 衛濛腎歷蝗棲棻(ex)/癱ル 1)");
			setMessage(mMsg, "[system]");
			checkVoteTime = TRUE;
			checkConfTime = FALSE;
		}
		if ((currtime >= conf+ vote) && checkMorning && !checkConfTime && checkVoteTime) { // 癱ル衛除謙猿塽 嫘戲煎 瞪��
			//sprintf(mMsg, "%d %d %d癱ル衛除檜 謙猿腎歷蝗棲棻", (int)currtime, vote, conf);
			strcpy(mMsg, "癱ル衛除檜 謙猿腎歷蝗棲棻");
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
		
		if (!roomSel)
		{
			trigger = 'K'; // 寞摹鷗 お葬剪
			if (msg[0] == '/') {
				cmpMsg = msg;
				cutMsg = strtok(cmpMsg, " ");
				if (strcmp(cutMsg, "/殮濰")) {
					strcpy(RoomMsg,"[system] : ぎ萼 貲滄橫殮棲棻.");
					EnterCriticalSection(&ChatCS1);
					GameChatClear();
					RoomSelectWindow();
					Sleep(1000);
					strcpy(RoomMsg, "[system] : 寞擊 摹鷗ж褊衛螃(ex)/殮濰 1)");
					RoomSelectWindow();
					LeaveCriticalSection(&ChatCS1);
					Sleep(1000);
					continue;
				}
				// 癱ルи Idx�挫�
				cutMsg = strtok(NULL, " ");
				idx = atoi(cutMsg);
				if (idx < 1 || idx > MAX_ROOM_SIZE) {
					strcpy(RoomMsg, "[system] : 螢夥腦雖 彊擎 廓��殮棲棻.");
					EnterCriticalSection(&ChatCS1);
					GameChatClear();
					RoomSelectWindow();
					Sleep(1000);
					strcpy(RoomMsg, "[system] : 寞擊 摹鷗ж褊衛螃(ex)/殮濰 1)");
					RoomSelectWindow();
					LeaveCriticalSection(&ChatCS1);
					Sleep(1000);
					continue;
				}
				// 寞摹鷗 諫猿塽 寞 殮濰 詭撮雖 send
				roomSel = TRUE;
				roomNum = idx;
				sprintf(Msg, "%c%c%c", trigger, 1, (char)roomNum); // お葬剪 , 望檜 , 癱ル渠鼻濠or葆Я嬴棟溘渠鼻濠 廓��
				strcpy(RoomMsg, "[system] : 貲滄橫 餌辨 諫猿!");
				EnterCriticalSection(&ChatCS1);
				GameChatClear();
				RoomSelectWindow();
				Sleep(1000);
				LeaveCriticalSection(&ChatCS1);
				send(hSock, Msg, strlen(Msg), 0);
				continue;
			}
			//殮濰 貲滄橫陛 ぎ萼唳辦
			strcpy(RoomMsg, "[system] : ぎ萼 貲滄橫殮棲棻.");
			EnterCriticalSection(&ChatCS1);
			GameChatClear();
			RoomSelectWindow();
			Sleep(1000);
			strcpy(RoomMsg, "[system] : 寞擊 摹鷗ж褊衛螃(ex)/殮濰 1)");
			RoomSelectWindow();
			LeaveCriticalSection(&ChatCS1);
			continue;
		}

		trigger = 'B';//橾奩 瓣た お葬剪
		//避戲賊 瓣た碳陛棟
		if (!checkAlive) {
			strcpy(mMsg, "渡褐擎 避橫憮 瓣た擊 й 熱 橈蝗棲棻.");
			setMessage(mMsg, "[system]");
			continue;
		}
		// /癱ル, /葆Я嬴 偽擎 貲滄橫 掘碟
		if (msg[0] == '/') {
			cmpMsg = msg;
			cutMsg = strtok(cmpMsg," ");
			if (strcmp(cutMsg, "/葆Я嬴") && strcmp(cutMsg, "/癱ル")) {
				strcpy(mMsg, "ぎ萼 貲滄橫殮棲棻.");
				setMessage(mMsg, "[system]");
				continue;
			}

			if (!strcmp(cutMsg,"/葆Я嬴") && !checkMorning && mafiaCanUseAbility) {
				trigger = 'F';//葆Я嬴 貲滄橫 お葬剪
			}
			else if(!strcmp(cutMsg, "/葆Я嬴")){
				strcpy(mMsg, "雖旎擎 貲滄橫蒂 噩 熱 橈剪釭 澀跤脹 貲滄橫 殮棲棻.");
				setMessage(mMsg, "[system]");
				continue;
			}

			if (!strcmp(cutMsg, "/癱ル") && checkVoteTime) {
				trigger = 'E';//癱ル 貲滄橫 お葬剪
			}
			else if(!strcmp(cutMsg, "/癱ル")){
				strcpy(mMsg, "雖旎擎 貲滄橫蒂 噩 熱 橈朝 衛除檜剪釭 澀跤脹 貲滄橫 殮棲棻.");
				setMessage(mMsg, "[system]");
				continue;
			}
			// 癱ルи Idx�挫�
			cutMsg = strtok(NULL, " ");
			idx = atoi(cutMsg);
			if (idx < 1 || idx > MAX_USER || deathIdx[idx]) {
				strcpy(mMsg, "螢夥腦雖 彊擎 廓��殮棲棻.");
				setMessage(mMsg, "[system]");
				continue;
			}
			//strcmp(msg, cutMsg);
			sprintf(Msg, "%c%c%c", trigger,1, (char)idx); // お葬剪 , 望檜 , 癱ル渠鼻濠or葆Я嬴棟溘渠鼻濠 廓��
			strcpy(mMsg, "貲滄橫 餌辨 諫猿!");
			setMessage(mMsg, "[system]");
			send(hSock, Msg, strlen(Msg), 0);
			continue;
		}
		//嫘橾陽 橾奩 瓣た橾唳辦
		if (trigger == 'B' && !checkMorning) {
			if (!mafia) {//葆Я嬴陛 嬴棲賊 瓣た碳陛棟
				strcpy(mMsg, "嫘檜塭 瓣た檜 碳陛棟 м棲棻!");
				setMessage(mMsg, "[system]");
				continue;
			}
		}
		sprintf(Msg, "%c%c%c%s", trigger, (char)strlen(msg), (char)personalIdx, msg); // お葬剪諦л眷 詭撮雖 瞪殖
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
		case 'A': // 衛濛 お葬剪
			Idx = recvMsg[2];
			confTime = recvMsg[4]; // �蛻� 衛除
			voteTime = recvMsg[5]; // 癱ル 衛除
			if (recvMsg[3] == Mafia1 || recvMsg[3] == Mafia2 && Idx != personalIdx) //濠褐檜 葆Я嬴橾陽 棻艇 葆Я嬴詭撮雖陛 諮擊唳辦
				anotherMafiaIdx = Idx; // 棻艇 葆Я嬴 idx 盪濰
			if (Idx != personalIdx)
				continue;

			strcpy(mMsg, "啪歜檜 衛濛腎歷蝗棲棻.");
			setMessage(mMsg, "[system]");
			personalRole = recvMsg[3]; // 偃檣羲й 盪濰
			if (personalRole == Mafia1) {
				mafiaCanUseAbility = TRUE;
				mafia = TRUE;
				strcpy(mMsg, "渡褐擎 葆Я嬴1 殮棲棻 衛團菟擊 賅舒 避檜撮蹂!(ex)/葆Я嬴 1)");
				setMessage(mMsg, "[system]");
			}
			else if (personalRole == Mafia2) {
				mafiaCanUseAbility = FALSE;
				mafia = TRUE;
				strcpy(mMsg, "渡褐擎 葆Я嬴2 殮棲棻 葆Я嬴1擊 紫諦憮 衛團菟擊 賅舒 避檜撮蹂!(ex)/葆Я嬴 1)");
				setMessage(mMsg, "[system]");
			}
			else {
				strcpy(mMsg, "渡褐擎 衛團 殮棲棻 葆Я嬴菟擎 賅舒 瓊嬴頂撮蹂!");
				setMessage(mMsg, "[system]");
			}

			gameStart = TRUE;
			
			continue;
		case 'B': // 瓣た
			Idx = recvMsg[2];
			if (checkMorning){ // 嬴藹橾 唳辦 賅萄 Ы溯檜橫煎 嗡
				sprintf(user, "[ Player %d ]", Idx);
			}

			if (!checkMorning && mafia) { // 嫘橾 唳辦 葆Я嬴陛 瓣た蟻擊陽 葆Я嬴 礙戲煎 瞪��
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
		case 'C': // 雪お葬剪 葆Я嬴 棟溘 唸婁 瞪殖
			EnterCriticalSection(&ChatCS2);
			Idx = recvMsg[2];
			if (Idx != -1) {
				deathIdx[Idx] = TRUE;
				if (Idx == personalIdx) { 
					checkAlive = FALSE;
					strcpy(mMsg, "渡褐檜 避歷蝗棲棻.");
					setMessage(mMsg, "[system]");
				}
				else {
					sprintf(mMsg, "Player %d(檜/陛) 避歷蝗棲棻.", Idx);
					setMessage(mMsg, "[system]");
				}
			}
			else {
				strcpy(mMsg,"嬴鼠紫 避雖 彊懊蝗棲棻."); 
				setMessage(mMsg, "[system]");
			}
			checkMorning = TRUE;
			checkConfTime = FALSE;
			checkVoteTime = FALSE;
			LeaveCriticalSection(&ChatCS2);
			continue;
		case 'D'://嫘お葬剪 癱ル 唸婁 瞪殖 
			EnterCriticalSection(&ChatCS2);
			checkMorning = FALSE;
			checkVoteTime = FALSE;
			strcpy(mMsg, "癱ル衛除檜 謙猿腎歷蝗棲棻");
			setMessage(mMsg, "[system]");
			Idx = recvMsg[2];
			//棻艇 葆Я嬴陛 避歷擊衛 
			if (mafia && Idx == anotherMafiaIdx) {
				// 濠褐檜 mafia2檜賊 棟溘擊 檜瞪 嫡擠
				if (personalRole == Mafia2) {
					sprintf(mMsg, "Mafia1檣 %d(檜/陛) 避歷蝗棲棻. 渡褐檜 檜薯 mafia1 殮棲棻", Idx);
					setMessage(mMsg, "[system]");
					personalRole = Mafia1;
					mafiaCanUseAbility = TRUE;
				}
			}
			if (Idx != -1) {
				deathIdx[Idx] = TRUE;
				if (Idx == personalIdx) {
					checkAlive = FALSE;
					strcpy(mMsg, "渡褐檜 避歷蝗棲棻.");
					setMessage(mMsg, "[system]");
				}
				else {
					sprintf(mMsg, "Player %d(檜/陛) 避歷蝗棲棻.", Idx);
					setMessage(mMsg, "[system]");
				}
			}
			else {
				strcpy(mMsg, "嬴鼠紫 避雖 彊懊蝗棲棻.");
				setMessage(mMsg, "[system]");
			}
			strcpy(mMsg, "嫘檜 瓊嬴諮蝗棲棻");
			setMessage(mMsg, "[system]");
			LeaveCriticalSection(&ChatCS2);
			continue;
			//E,F朝 贗塭陛 憮幗縑 瞪殖л
		case 'E':
			continue;
		case 'F':
			continue;
		case 'G'://啪歜 謙猿 お葬剪
			strcpy(mMsg, "啪歜檜 謙猿 腎歷蝗棲棻.");
			setMessage(mMsg, "[system]");
			if(recvMsg[2] == 0)
				strcpy(mMsg, "衛團檜 檜啣蝗棲棻.");
			else
				strcpy(mMsg, "葆Я嬴陛 檜啣蝗棲棻.");
			setMessage(mMsg, "[system]");
			gameEnd = TRUE;
			continue;
		case 'H'://寞 蕾樓 お葬剪
			Idx = recvMsg[2];
			if (personalIdx == -1)
			{
				personalIdx = Idx;
				sprintf(mMsg, "%d廓寞縑 殮濰ц蝗棲棻", roomNum);
				setMessage(mMsg, "[system]");
			}
			sprintf(mMsg, "Player %d(檜/陛) 蕾樓ц蝗棲棻.", Idx);
			setMessage(mMsg, "[system]");
			continue;
		case 'I'://黴濰 お葬剪
			Idx = recvMsg[2];
			sprintf(mMsg, "Player %d(檜/陛) 黴濰ц蝗棲棻.", Idx);
			setMessage(mMsg, "[system]");
			continue;
		case 'J':// 寞 薑爾 瞪殖 お葬剪
			strcpy(RoomMsg, "[system] : 寞擊 摹鷗ж褊衛螃(ex)/殮濰 1)");
			EnterCriticalSection(&ChatCS1);
			RoomBuf[0] = (int)recvMsg[2];
			RoomBuf[1] = (int)recvMsg[3];
			LeaveCriticalSection(&ChatCS1);
			RoomSelectWindow();
			continue;
		case 'K':
			continue;
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
	printf("忙式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖\n");
	printf("弛%s : %s", UserBuf[5], GameEchoBuf[5]); gotoxy(109, 15); printf("弛\n");
	printf("弛%s : %s", UserBuf[4], GameEchoBuf[4]); gotoxy(109, 16); printf("弛\n");
	printf("弛%s : %s", UserBuf[3], GameEchoBuf[3]); gotoxy(109, 17); printf("弛\n");
	printf("弛%s : %s", UserBuf[2], GameEchoBuf[2]); gotoxy(109, 18); printf("弛\n");
	printf("弛%s : %s", UserBuf[1], GameEchoBuf[1]); gotoxy(109, 19); printf("弛\n");
	printf("弛%s : %s", UserBuf[0], GameEchoBuf[0]); gotoxy(109, 20); printf("弛\n");
	printf("戍式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣\n");
	printf("弛殮溘 :");  gotoxy(109, 22); printf("弛\n");
	printf("戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎\n");
	gotoxy(9, 22);

}

void RoomSelectWindow() {
	gotoxy(1, 14);
	printf("忙式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖\n");
	printf("弛 %d廓寞 : %d貲", 1, RoomBuf[0]); gotoxy(109, 15); printf("弛\n");
	printf("弛 %d廓寞 : %d貲", 2, RoomBuf[1]); gotoxy(109, 16); printf("弛\n");
	printf("弛 %s", RoomMsg); gotoxy(109, 17); printf("弛\n");
	printf("弛"); gotoxy(109, 18); printf("弛\n");
	printf("弛"); gotoxy(109, 19); printf("弛\n");
	printf("弛"); gotoxy(109, 20); printf("弛\n");
	printf("戍式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣\n");
	printf("弛殮溘 :");  gotoxy(109, 22); printf("弛\n");
	printf("戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎\n");
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
	printf("忙式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖\n");
	printf("弛                                                                                                           "); gotoxy(109, 15); printf("弛\n");
	printf("弛                                                                                                           "); gotoxy(109, 16); printf("弛\n");
	printf("弛                                                                                                           "); gotoxy(109, 17); printf("弛\n");
	printf("弛                                                                                                           "); gotoxy(109, 18); printf("弛\n");
	printf("弛                                                                                                           "); gotoxy(109, 19); printf("弛\n");
	printf("弛                                                                                                           "); gotoxy(109, 20); printf("弛\n");
	printf("戍式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣\n");
	printf("弛殮溘 :                                                                                                       "); gotoxy(109, 22);  printf("弛\n");
	printf("戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎\n");
	gotoxy(9, 22);
}
void ErrorHandling(char *msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
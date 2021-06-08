#include "client.h"
		  //todo : 翕晦�� , 葆Я嬴 薑爾 婦溼擎 橫飩啪 й匙檣陛? 憮幗縑憮 剩啖 還匙檣陛 嬴棲賊 贗塭縑憮 籀葬й匙檣陛?(1. 棻艇 葆Я嬴 薑爾, 2. 葆Я嬴陛 и貲 闊歷擊陽 棟溘 檜瞪)
		  //虜萇剪 : 嫡朝 お葬剪 掘碟, 爾頂朝 お葬剪 掘碟 , 餌蜂衛 瓣た碳陛, 葆Я嬴 礙, �蛻�,癱ル衛除憲葡, Ы溯檜橫廓�� ル衛
		  /*A衛濛 お葬剪 - 衛濛 & 羲й瞪歎 | 3夥檜お
			B廓�� んл 詭衛雖 お葬剪 - 橾奩 / 葆Я嬴 | 103夥檜お //贗塭
			C雪 お葬剪 - 葆Я嬴縑 曖п 避擎 餌塋, �蛻� + 癱ル衛除(歜曖褻薑) | 5夥檜お
			D嫘 お葬剪 - 癱ル唸婁 & 癱ル縑 曖п 避擎餌塋 | 3夥檜お
			E癱ル お葬剪 - 癱ルи 薑爾 爾頂輿晦 | 3夥檜お //贗塭
			F葆Я嬴 棟溘 お葬剪 - 避橾 餌塋 瞪歎 | 3夥檜お //贗塭
			G謙猿 お葬剪 - 謙猿 褻勒 虜褶衛 瞪歎 | 3夥檜お*/

int main(int argc, char *argv[]) {
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread, playGameThread, roomThread;
	int i;

	for (i = 0; i <= MAX_USER; i++) { deathIdx[i] = FALSE; }
	currtime = 0; // 嬴藹橾陽 ⑷營 衛除
	confTime = 5; // �蛻Х簸�
	voteTime = 100; // 癱ル衛除
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
	int vote, conf;
	char mMsg[BUF_SIZE] = { 0, };
	char user[BUF_SIZE] = { 0, };
	while (1) {
		if (!gameStart)	{continue;}
		
		if (!checkConfTime && !checkVoteTime && checkMorning) {
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
		if ((currtime < conf+vote) && (currtime >= conf) && checkConfTime && !checkVoteTime && checkMorning) {
			//sprintf(mMsg, "%d %d %d癱ル衛除檜 衛濛腎歷蝗棲棻", currtime,checkVoteTime, checkConfTime);
			strcpy(mMsg, "癱ル衛除檜 衛濛腎歷蝗棲棻");
			setMessage(mMsg, "[system]");
			checkVoteTime = TRUE;
			checkConfTime = FALSE;
		}
		if ((currtime >= conf+ vote) && checkMorning && !checkConfTime && checkVoteTime) {
			//sprintf(mMsg, "%d %d %d癱ル衛除檜 謙猿腎歷蝗棲棻", (int)currtime, checkVoteTime, checkConfTime);
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
			trigger = 'K';
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
				//strcmp(msg, cutMsg);
				roomSel = TRUE;
				roomNum = idx;
				sprintf(Msg, "%c%c%c", trigger, 1, (char)roomNum); // お葬剪 , 望檜 , 癱ル渠鼻濠or葆Я嬴棟溘渠鼻濠 廓��
				strcpy(RoomMsg, "[system] : 貲滄橫 餌辨 諫猿!");
				EnterCriticalSection(&ChatCS1);
				GameChatClear();
				RoomSelectWindow();
				Sleep(1000);
				strcpy(RoomMsg, "[system] : 寞擊 摹鷗ж褊衛螃(ex)/殮濰 1)");
				RoomSelectWindow();
				LeaveCriticalSection(&ChatCS1);
				send(hSock, Msg, strlen(Msg), 0);
				continue;
			}
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

		trigger = 'B';
		// /癱ル, /葆Я嬴 偽擎 貲滄橫 掘碟
		if (!checkAlive) {
			strcpy(mMsg, "渡褐擎 避橫憮 瓣た擊 й 熱 橈蝗棲棻.");
			setMessage(mMsg, "[system]");
			continue;
		}

		if (msg[0] == '/') {
			cmpMsg = msg;
			cutMsg = strtok(cmpMsg," ");
			if (strcmp(cutMsg, "/葆Я嬴") && strcmp(cutMsg, "/癱ル")) {
				strcpy(mMsg, "ぎ萼 貲滄橫殮棲棻.");
				setMessage(mMsg, "[system]");
				continue;
			}

			if (!strcmp(cutMsg,"/葆Я嬴") && !checkMorning && mafiaCanUseAbility) {
				trigger = 'F';
			}
			else if(!strcmp(cutMsg, "/葆Я嬴")){
				strcpy(mMsg, "雖旎擎 貲滄橫蒂 噩 熱 橈剪釭 澀跤脹 貲滄橫 殮棲棻.");
				setMessage(mMsg, "[system]");
				continue;
			}

			if (!strcmp(cutMsg, "/癱ル") && checkVoteTime) {
				trigger = 'E';
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

		if (trigger == 'B' && !checkMorning) {
			if (!mafia) {
				strcpy(mMsg, "嫘檜塭 瓣た檜 碳陛棟 м棲棻!");
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
			strcpy(mMsg, "啪歜檜 衛濛腎歷蝗棲棻.");
			setMessage(mMsg, "[system]");
			personalRole = recvMsg[3];
			if (personalRole == Mafia1) {
				mafiaCanUseAbility = TRUE;
				mafia = TRUE;
				strcpy(mMsg, "渡褐擎 葆Я嬴1 殮棲棻 衛團菟擊 賅舒 避檜撮蹂!");
				setMessage(mMsg, "[system]");
			}
			else if (personalRole == Mafia2) {
				mafiaCanUseAbility = FALSE;
				mafia = TRUE;
				strcpy(mMsg, "渡褐擎 葆Я嬴2 殮棲棻 葆Я嬴1擊 紫諦憮 衛團菟擊 賅舒 避檜撮蹂!");
				setMessage(mMsg, "[system]");
			}
			else {
				strcpy(mMsg, "渡褐擎 衛團 殮棲棻 葆Я嬴菟擎 賅舒 瓊嬴頂撮蹂!");
				setMessage(mMsg, "[system]");
			}

			gameStart = TRUE;
			
			continue;
		case 'B':
			Idx = recvMsg[2];
				//葆Я嬴礙橾 唳辦 葆Я嬴 羹觼
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
			continue;
			LeaveCriticalSection(&ChatCS2);
		case 'D':
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
			continue;
			LeaveCriticalSection(&ChatCS2);
			//E,F朝 贗塭辨
		case 'E':
			continue;
		case 'F':
			continue;
		case 'G':
			strcpy(mMsg, "啪歜檜 謙猿 腎歷蝗棲棻.");
			setMessage(mMsg, "[system]");
			if(recvMsg[2] == 0)
				strcpy(mMsg, "衛團檜 檜啣蝗棲棻.");
			else
				strcpy(mMsg, "葆Я嬴陛 檜啣蝗棲棻.");
			setMessage(mMsg, "[system]");
			gameEnd = TRUE;
			continue;
		case 'H':
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
		case 'I':
			Idx = recvMsg[2];
			sprintf(mMsg, "Player %d(檜/陛) 黴濰ц蝗棲棻.", Idx);
			setMessage(mMsg, "[system]");
			if (Idx = personalIdx)
			{
				roomSel = FALSE;
				roomNum = -1;
				personalIdx = -1;
				RoomSelectWindow();
			}
			continue;
		case 'J':
			strcpy(RoomMsg, "[system] : 寞擊 摹鷗ж褊衛螃(ex)/殮濰 1)");
			EnterCriticalSection(&ChatCS1);
			RoomBuf[0] = (int)recvMsg[2];
			RoomBuf[1] = (int)recvMsg[3];
			RoomSelectWindow();
			LeaveCriticalSection(&ChatCS1);
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
		mMsg = "啪歜檜 衛濛腎歷蝗棲棻.";
		user = "[system]";
		setMessage(mMsg, user);
		//printf("\n\n 啪歜檜 衛濛腎歷蝗棲棻. \n\n");
		break;
	case 'B':
		Idx = recvMsg[2];
			//葆Я嬴礙橾 唳辦 葆Я嬴 羹觼
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
			//printf("\n\n Player %d(檜/陛) 避歷蝗棲棻. \n\n", Idx);
			if (Idx == personalIdx) { checkAlive = FALSE; }
		}
		else {// printf("\n\n 嬴鼠紫 避雖 彊懊蝗棲棻. \n\n"); }
			break;
	case 'D':
		Idx = recvMsg[2];
		checkMorning = FALSE;
		//棻艇 葆Я嬴陛 避歷擊衛 
		if (mafia && Idx == anotherMafiaIdx) {
			// 濠褐檜 mafia2檜賊 棟溘擊 檜瞪 嫡擠
			if (personalRole == Mafia2) {
				personalRole = Mafia1;
				mafiaCanUseAbility = TRUE;
			}
		}

		if (Idx != -1) {
			deathIdx[Idx] = TRUE;
			user = "[system]";
			sprintf(mMsg, "Player %d(檜/陛) 避歷蝗棲棻.", Idx);
			setMessage(mMsg, user);
			if (Idx == personalIdx) { checkAlive = FALSE; }
		}
		else {
			user = "[system]";
			sprintf(mMsg, "嬴鼠紫 避雖 彊懊蝗棲棻.");
			setMessage(mMsg, user);
			//printf("\n\n 嬴鼠紫 避雖 彊懊蝗棲棻. \n\n"); 
		}
		break;
		//E,F朝 贗塭辨
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
	// /癱ル, /葆Я嬴 偽擎 貲滄橫 掘碟
	if (!checkAlive) {
		user = "[system]";
		mMsg = "渡褐擎 避橫憮 瓣た擊 й 熱 橈蝗棲棻.";
		setMessage(mMsg, user);
		//printf("\n\n 渡褐擎 避橫憮 瓣た擊 й 熱 橈蝗棲棻. \n\n");
		return;
	}

	if (msg[0] == '/') {
		strcmp(cmpMsg, msg);
		cutMsg = strtok(cmpMsg, " ");
		if (!strcmp(cutMsg, "/癱ル") && !strcmp(cutMsg, "/葆Я嬴")) {
			user = "[system]";
			mMsg = "ぎ萼 貲滄橫殮棲棻.";
			setMessage(mMsg, user);
			//printf("\n\n ぎ萼 貲滄橫殮棲棻. \n\n");
			return;
		}

		if (strcmp(cutMsg, "/葆Я嬴") && !checkMorning && mafiaCanUseAbility) {
			trigger = 'F';
		}
		else {
			user = "[system]";
			mMsg = "雖旎擎 貲滄橫蒂 噩 熱 橈剪釭 澀跤脹 貲滄橫 殮棲棻.";
			setMessage(mMsg, user);
			//printf("\n\n 雖旎擎 貲滄橫蒂 噩 熱 橈剪釭 澀跤脹 貲滄橫 殮棲棻. \n\n");
			return;
		}

		if (strcmp(cutMsg, "/癱ル") && checkVoteTime) {
			trigger = 'E';
		}
		else {
			user = "[system]";
			mMsg = "雖旎擎 貲滄橫蒂 噩 熱 橈朝 衛除檜剪釭 澀跤脹 貲滄橫 殮棲棻.";
			setMessage(mMsg, user);
			//printf("\n\n 雖旎擎 貲滄橫蒂 噩 熱 橈朝 衛除檜剪釭 澀跤脹 貲滄橫 殮棲棻. \n\n");
			return;
		}
		// 癱ルи Idx�挫�
		cutMsg = strtok(NULL, " ");
		idx = atoi(cutMsg);
		if (idx < 0 || idx > MAX_USER || deathIdx[idx]) {
			user = "[system]";
			mMsg = "螢夥腦雖 彊擎 廓��殮棲棻.";
			setMessage(mMsg, user);
			//printf("\n\n 螢夥腦雖 彊擎 廓��殮棲棻. \n\n");
			return;
		}
		strcmp(msg, cutMsg);
		sprintf(Msg, "%c%c%s", trigger, (char)strlen(msg), msg);
		return;
	}

	if (trigger == 'B' && !checkMorning) {
		if (!mafia) {
			user = "[system]";
			mMsg = "螢夥腦雖 彊擎 廓��殮棲棻.";
			setMessage(mMsg, user);
			//printf("\n\n 嫘縑朝 詭撮雖 瞪歎檜 碳陛棟м棲棻. \n\n");
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
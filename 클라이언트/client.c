#include "client.h"
int main(int argc, char *argv[]) {
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread, playGameThread, roomThread;
	int i;

	for (i = 0; i <= MAX_USER; i++) { deathIdx[i] = FALSE; }
	currtime = 0; // ¾ÆÄ§ÀÏ¶§ ÇöÀç ½Ã°£
	confTime = -1; // È¸ÀÇ½Ã°£
	voteTime = -1; // ÅõÇ¥½Ã°£
	anotherMafiaIdx = -1; // ´Ù¸¥ ¸¶ÇÇ¾ÆÀÇ idx
	personalIdx = -1; // ¼­¹ö¿¡¼­ ºÎ¿©¹ÞÀº °³ÀÎ index 
	personalRole = Null; // ¼­¹ö¿¡¼­ ¹Þ¾Æ¿Â °³ÀÎ ¿ªÇÒ
	mafia = FALSE; // ¸¶ÇÇ¾ÆÃª °¡´É ¿©ºÎ
	mafiaCanUseAbility = FALSE; // ¸¶ÇÇ¾ÆÀÇ ´É·Â »ç¿ë °¡´É ¿©ºÎ
	checkMafiaAbilityUse = FALSE;
	checkConfTime = FALSE; // È¸ÀÇÁßÀÎÁö ¿©ºÎ È®ÀÎ
	checkVoteTime = FALSE;// ÅõÇ¥ÁßÀÎÁö ¿©ºÎ È®ÀÎ
	gameStart = FALSE; // °ÔÀÓ ½ÃÀÛ ¿©ºÎ 
	gameEnd = FALSE; // °ÔÀÓ Á¾·á ¿©ºÎ
	checkAlive = TRUE; // »ýÁ¸ ¿©ºÎ
	checkMorning = TRUE; // ¾ÆÄ§ Ã¼Å©
	roomSel = FALSE; // ¹æ ¼±ÅÃ »óÅÂ
	roomNum = -1; // ¼±ÅÃÇÑ ¹æ ¹øÈ£

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
	playGameThread =(HANDLE)_beginthreadex(NULL, 0, GameManager, NULL , 0, NULL); //°ÔÀÓ ÁøÇà »óÈ²¿ë ¾²·¹µå

	WaitForSingleObject(hSndThread, INFINITE);
	WaitForSingleObject(hRcvThread, INFINITE);
	closesocket(hSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI GameManager() // °ÔÀÓÁøÇà
{  
	time_t t1, t2;
	int vote, conf;
	char mMsg[BUF_SIZE] = { 0, };
	char user[BUF_SIZE] = { 0, };
	while (1) {
		if (!gameStart)	{continue;} // °ÔÀÓÁøÇà »óÅÂÀÏ½Ã
		
		if (!checkConfTime && !checkVoteTime && checkMorning) { //È¸ÀÇ½Ã°£, ÅõÇ¥½Ã°£ÀÌ ¾Æ´Ï°í ¾ÆÄ§ÀÏ¶§
			vote = voteTime;
			conf = confTime;
			//sprintf(mMsg, "%d %d %dÈ¸ÀÇ½Ã°£ÀÌ ½ÃÀÛµÇ¾ú½À´Ï´Ù", (int)currtime, vote, conf);
			strcpy(mMsg, "È¸ÀÇ½Ã°£ÀÌ ½ÃÀÛµÇ¾ú½À´Ï´Ù");
			setMessage(mMsg, "[system]");
			t1 = time(NULL);
			checkConfTime = TRUE;
		}
		t2 = time(NULL);
		currtime = t2 - t1;
		if ((currtime < conf+vote) && (currtime >= conf) && checkConfTime && !checkVoteTime && checkMorning) { // ÅõÇ¥½Ã°£ ½ÃÀÛ
			//sprintf(mMsg, "%d %d %dÅõÇ¥½Ã°£ÀÌ ½ÃÀÛµÇ¾ú½À´Ï´Ù", currtime, vote, conf);
			strcpy(mMsg, "ÅõÇ¥½Ã°£ÀÌ ½ÃÀÛµÇ¾ú½À´Ï´Ù(ex)/ÅõÇ¥ 1)");
			setMessage(mMsg, "[system]");
			checkVoteTime = TRUE;
			checkConfTime = FALSE;
		}
		if ((currtime >= conf+ vote) && checkMorning && !checkConfTime && checkVoteTime) { // ÅõÇ¥½Ã°£Á¾·á¹× ¹ãÀ¸·Î ÀüÈ¯
			//sprintf(mMsg, "%d %d %dÅõÇ¥½Ã°£ÀÌ Á¾·áµÇ¾ú½À´Ï´Ù", (int)currtime, vote, conf);
			strcpy(mMsg, "ÅõÇ¥½Ã°£ÀÌ Á¾·áµÇ¾ú½À´Ï´Ù");
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
			trigger = 'K'; // ¹æ¼±ÅÃ Æ®¸®°Å
			if (msg[0] == '/') {
				cmpMsg = msg;
				cutMsg = strtok(cmpMsg, " ");
				if (strcmp(cutMsg, "/ÀÔÀå")) {
					strcpy(RoomMsg,"[system] : Æ²¸° ¸í·É¾îÀÔ´Ï´Ù.");
					EnterCriticalSection(&ChatCS1);
					GameChatClear();
					RoomSelectWindow();
					Sleep(1000);
					strcpy(RoomMsg, "[system] : ¹æÀ» ¼±ÅÃÇÏ½Ê½Ã¿À(ex)/ÀÔÀå 1)");
					RoomSelectWindow();
					LeaveCriticalSection(&ChatCS1);
					Sleep(1000);
					continue;
				}
				// ÅõÇ¥ÇÑ IdxÈ®ÀÎ
				cutMsg = strtok(NULL, " ");
				idx = atoi(cutMsg);
				if (idx < 1 || idx > MAX_ROOM_SIZE) {
					strcpy(RoomMsg, "[system] : ¿Ã¹Ù¸£Áö ¾ÊÀº ¹øÈ£ÀÔ´Ï´Ù.");
					EnterCriticalSection(&ChatCS1);
					GameChatClear();
					RoomSelectWindow();
					Sleep(1000);
					strcpy(RoomMsg, "[system] : ¹æÀ» ¼±ÅÃÇÏ½Ê½Ã¿À(ex)/ÀÔÀå 1)");
					RoomSelectWindow();
					LeaveCriticalSection(&ChatCS1);
					Sleep(1000);
					continue;
				}
				// ¹æ¼±ÅÃ ¿Ï·á¹× ¹æ ÀÔÀå ¸Þ¼¼Áö send
				roomSel = TRUE;
				roomNum = idx;
				sprintf(Msg, "%c%c%c", trigger, 1, (char)roomNum); // Æ®¸®°Å , ±æÀÌ , ÅõÇ¥´ë»óÀÚor¸¶ÇÇ¾Æ´É·Â´ë»óÀÚ ¹øÈ£
				strcpy(RoomMsg, "[system] : ¸í·É¾î »ç¿ë ¿Ï·á!");
				EnterCriticalSection(&ChatCS1);
				GameChatClear();
				RoomSelectWindow();
				Sleep(1000);
				LeaveCriticalSection(&ChatCS1);
				send(hSock, Msg, strlen(Msg), 0);
				continue;
			}
			//ÀÔÀå ¸í·É¾î°¡ Æ²¸°°æ¿ì
			strcpy(RoomMsg, "[system] : Æ²¸° ¸í·É¾îÀÔ´Ï´Ù.");
			EnterCriticalSection(&ChatCS1);
			GameChatClear();
			RoomSelectWindow();
			Sleep(1000);
			strcpy(RoomMsg, "[system] : ¹æÀ» ¼±ÅÃÇÏ½Ê½Ã¿À(ex)/ÀÔÀå 1)");
			RoomSelectWindow();
			LeaveCriticalSection(&ChatCS1);
			continue;
		}

		trigger = 'B';//ÀÏ¹Ý Ã¤ÆÃ Æ®¸®°Å
		//Á×À¸¸é Ã¤ÆÃºÒ°¡´É
		if (!checkAlive) {
			strcpy(mMsg, "´ç½ÅÀº Á×¾î¼­ Ã¤ÆÃÀ» ÇÒ ¼ö ¾ø½À´Ï´Ù.");
			setMessage(mMsg, "[system]");
			continue;
		}
		// /ÅõÇ¥, /¸¶ÇÇ¾Æ °°Àº ¸í·É¾î ±¸ºÐ
		if (msg[0] == '/') {
			cmpMsg = msg;
			cutMsg = strtok(cmpMsg," ");
			if (strcmp(cutMsg, "/¸¶ÇÇ¾Æ") && strcmp(cutMsg, "/ÅõÇ¥")) {
				strcpy(mMsg, "Æ²¸° ¸í·É¾îÀÔ´Ï´Ù.");
				setMessage(mMsg, "[system]");
				continue;
			}

			if (!strcmp(cutMsg,"/¸¶ÇÇ¾Æ") && !checkMorning && mafiaCanUseAbility) {
				trigger = 'F';//¸¶ÇÇ¾Æ ¸í·É¾î Æ®¸®°Å
			}
			else if(!strcmp(cutMsg, "/¸¶ÇÇ¾Æ")){
				strcpy(mMsg, "Áö±ÝÀº ¸í·É¾î¸¦ ¾µ ¼ö ¾ø°Å³ª Àß¸øµÈ ¸í·É¾î ÀÔ´Ï´Ù.");
				setMessage(mMsg, "[system]");
				continue;
			}

			if (!strcmp(cutMsg, "/ÅõÇ¥") && checkVoteTime) {
				trigger = 'E';//ÅõÇ¥ ¸í·É¾î Æ®¸®°Å
			}
			else if(!strcmp(cutMsg, "/ÅõÇ¥")){
				strcpy(mMsg, "Áö±ÝÀº ¸í·É¾î¸¦ ¾µ ¼ö ¾ø´Â ½Ã°£ÀÌ°Å³ª Àß¸øµÈ ¸í·É¾î ÀÔ´Ï´Ù.");
				setMessage(mMsg, "[system]");
				continue;
			}
			// ÅõÇ¥ÇÑ IdxÈ®ÀÎ
			cutMsg = strtok(NULL, " ");
			idx = atoi(cutMsg);
			if (idx < 1 || idx > MAX_USER || deathIdx[idx]) {
				strcpy(mMsg, "¿Ã¹Ù¸£Áö ¾ÊÀº ¹øÈ£ÀÔ´Ï´Ù.");
				setMessage(mMsg, "[system]");
				continue;
			}
			//strcmp(msg, cutMsg);
			sprintf(Msg, "%c%c%c", trigger,1, (char)idx); // Æ®¸®°Å , ±æÀÌ , ÅõÇ¥´ë»óÀÚor¸¶ÇÇ¾Æ´É·Â´ë»óÀÚ ¹øÈ£
			strcpy(mMsg, "¸í·É¾î »ç¿ë ¿Ï·á!");
			setMessage(mMsg, "[system]");
			send(hSock, Msg, strlen(Msg), 0);
			continue;
		}
		//¹ãÀÏ¶§ ÀÏ¹Ý Ã¤ÆÃÀÏ°æ¿ì
		if (trigger == 'B' && !checkMorning) {
			if (!mafia) {//¸¶ÇÇ¾Æ°¡ ¾Æ´Ï¸é Ã¤ÆÃºÒ°¡´É
				strcpy(mMsg, "¹ãÀÌ¶ó Ã¤ÆÃÀÌ ºÒ°¡´É ÇÕ´Ï´Ù!");
				setMessage(mMsg, "[system]");
				continue;
			}
		}
		sprintf(Msg, "%c%c%c%s", trigger, (char)strlen(msg), (char)personalIdx, msg); // Æ®¸®°Å¿ÍÇÔ²² ¸Þ¼¼Áö Àü´Þ
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
		case 'A': // ½ÃÀÛ Æ®¸®°Å
			Idx = recvMsg[2];
			confTime = recvMsg[4]; // È¸ÀÇ ½Ã°£
			voteTime = recvMsg[5]; // ÅõÇ¥ ½Ã°£
			if (recvMsg[3] == Mafia1 || recvMsg[3] == Mafia2 && Idx != personalIdx) //ÀÚ½ÅÀÌ ¸¶ÇÇ¾ÆÀÏ¶§ ´Ù¸¥ ¸¶ÇÇ¾Æ¸Þ¼¼Áö°¡ ¿ÔÀ»°æ¿ì
				anotherMafiaIdx = Idx; // ´Ù¸¥ ¸¶ÇÇ¾Æ idx ÀúÀå
			if (Idx != personalIdx)
				continue;

			strcpy(mMsg, "°ÔÀÓÀÌ ½ÃÀÛµÇ¾ú½À´Ï´Ù.");
			setMessage(mMsg, "[system]");
			personalRole = recvMsg[3]; // °³ÀÎ¿ªÇÒ ÀúÀå
			if (personalRole == Mafia1) {
				mafiaCanUseAbility = TRUE;
				mafia = TRUE;
				strcpy(mMsg, "´ç½ÅÀº ¸¶ÇÇ¾Æ1 ÀÔ´Ï´Ù ½Ã¹ÎµéÀ» ¸ðµÎ Á×ÀÌ¼¼¿ä!(ex)/¸¶ÇÇ¾Æ 1)");
				setMessage(mMsg, "[system]");
			}
			else if (personalRole == Mafia2) {
				mafiaCanUseAbility = FALSE;
				mafia = TRUE;
				strcpy(mMsg, "´ç½ÅÀº ¸¶ÇÇ¾Æ2 ÀÔ´Ï´Ù ¸¶ÇÇ¾Æ1À» µµ¿Í¼­ ½Ã¹ÎµéÀ» ¸ðµÎ Á×ÀÌ¼¼¿ä!(ex)/¸¶ÇÇ¾Æ 1)");
				setMessage(mMsg, "[system]");
			}
			else {
				strcpy(mMsg, "´ç½ÅÀº ½Ã¹Î ÀÔ´Ï´Ù ¸¶ÇÇ¾ÆµéÀº ¸ðµÎ Ã£¾Æ³»¼¼¿ä!");
				setMessage(mMsg, "[system]");
			}

			gameStart = TRUE;
			
			continue;
		case 'B': // Ã¤ÆÃ
			Idx = recvMsg[2];
			if (checkMorning){ // ¾ÆÄ§ÀÏ °æ¿ì ¸ðµå ÇÃ·¹ÀÌ¾î·Î ¶ä
				sprintf(user, "[ Player %d ]", Idx);
			}

			if (!checkMorning && mafia) { // ¹ãÀÏ °æ¿ì ¸¶ÇÇ¾Æ°¡ Ã¤ÆÃÃÆÀ»¶§ ¸¶ÇÇ¾Æ ÃªÀ¸·Î ÀüÈ¯
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
		case 'C': // ³·Æ®¸®°Å ¸¶ÇÇ¾Æ ´É·Â °á°ú Àü´Þ
			EnterCriticalSection(&ChatCS2);
			Idx = recvMsg[2];
			if (Idx != -1) {
				deathIdx[Idx] = TRUE;
				if (Idx == personalIdx) { 
					checkAlive = FALSE;
					strcpy(mMsg, "´ç½ÅÀÌ Á×¾ú½À´Ï´Ù.");
					setMessage(mMsg, "[system]");
				}
				else {
					sprintf(mMsg, "Player %d(ÀÌ/°¡) Á×¾ú½À´Ï´Ù.", Idx);
					setMessage(mMsg, "[system]");
				}
			}
			else {
				strcpy(mMsg,"¾Æ¹«µµ Á×Áö ¾Ê¾Ò½À´Ï´Ù."); 
				setMessage(mMsg, "[system]");
			}
			checkMorning = TRUE;
			checkConfTime = FALSE;
			checkVoteTime = FALSE;
			LeaveCriticalSection(&ChatCS2);
			continue;
		case 'D'://¹ãÆ®¸®°Å ÅõÇ¥ °á°ú Àü´Þ 
			EnterCriticalSection(&ChatCS2);
			checkMorning = FALSE;
			checkVoteTime = FALSE;
			strcpy(mMsg, "ÅõÇ¥½Ã°£ÀÌ Á¾·áµÇ¾ú½À´Ï´Ù");
			setMessage(mMsg, "[system]");
			Idx = recvMsg[2];
			//´Ù¸¥ ¸¶ÇÇ¾Æ°¡ Á×¾úÀ»½Ã 
			if (mafia && Idx == anotherMafiaIdx) {
				// ÀÚ½ÅÀÌ mafia2ÀÌ¸é ´É·ÂÀ» ÀÌÀü ¹ÞÀ½
				if (personalRole == Mafia2) {
					sprintf(mMsg, "Mafia1ÀÎ %d(ÀÌ/°¡) Á×¾ú½À´Ï´Ù. ´ç½ÅÀÌ ÀÌÁ¦ mafia1 ÀÔ´Ï´Ù", Idx);
					setMessage(mMsg, "[system]");
					personalRole = Mafia1;
					mafiaCanUseAbility = TRUE;
				}
			}
			if (Idx != -1) {
				deathIdx[Idx] = TRUE;
				if (Idx == personalIdx) {
					checkAlive = FALSE;
					strcpy(mMsg, "´ç½ÅÀÌ Á×¾ú½À´Ï´Ù.");
					setMessage(mMsg, "[system]");
				}
				else {
					sprintf(mMsg, "Player %d(ÀÌ/°¡) Á×¾ú½À´Ï´Ù.", Idx);
					setMessage(mMsg, "[system]");
				}
			}
			else {
				strcpy(mMsg, "¾Æ¹«µµ Á×Áö ¾Ê¾Ò½À´Ï´Ù.");
				setMessage(mMsg, "[system]");
			}
			strcpy(mMsg, "¹ãÀÌ Ã£¾Æ¿Ô½À´Ï´Ù");
			setMessage(mMsg, "[system]");
			LeaveCriticalSection(&ChatCS2);
			continue;
			//E,F´Â Å¬¶ó°¡ ¼­¹ö¿¡ Àü´ÞÇÔ
		case 'E':
			continue;
		case 'F':
			continue;
		case 'G'://°ÔÀÓ Á¾·á Æ®¸®°Å
			strcpy(mMsg, "°ÔÀÓÀÌ Á¾·á µÇ¾ú½À´Ï´Ù.");
			setMessage(mMsg, "[system]");
			if(recvMsg[2] == 0)
				strcpy(mMsg, "½Ã¹ÎÀÌ ÀÌ°å½À´Ï´Ù.");
			else
				strcpy(mMsg, "¸¶ÇÇ¾Æ°¡ ÀÌ°å½À´Ï´Ù.");
			setMessage(mMsg, "[system]");
			gameEnd = TRUE;
			continue;
		case 'H'://¹æ Á¢¼Ó Æ®¸®°Å
			Idx = recvMsg[2];
			if (personalIdx == -1)
			{
				personalIdx = Idx;
				sprintf(mMsg, "%d¹ø¹æ¿¡ ÀÔÀåÇß½À´Ï´Ù", roomNum);
				setMessage(mMsg, "[system]");
			}
			sprintf(mMsg, "Player %d(ÀÌ/°¡) Á¢¼ÓÇß½À´Ï´Ù.", Idx);
			setMessage(mMsg, "[system]");
			continue;
		case 'I'://ÅðÀå Æ®¸®°Å
			Idx = recvMsg[2];
			sprintf(mMsg, "Player %d(ÀÌ/°¡) ÅðÀåÇß½À´Ï´Ù.", Idx);
			setMessage(mMsg, "[system]");
			continue;
		case 'J':// ¹æ Á¤º¸ Àü´Þ Æ®¸®°Å
			strcpy(RoomMsg, "[system] : ¹æÀ» ¼±ÅÃÇÏ½Ê½Ã¿À(ex)/ÀÔÀå 1)");
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
	printf("¦£¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¤\n");
	printf("¦¢%s : %s", UserBuf[5], GameEchoBuf[5]); gotoxy(109, 15); printf("¦¢\n");
	printf("¦¢%s : %s", UserBuf[4], GameEchoBuf[4]); gotoxy(109, 16); printf("¦¢\n");
	printf("¦¢%s : %s", UserBuf[3], GameEchoBuf[3]); gotoxy(109, 17); printf("¦¢\n");
	printf("¦¢%s : %s", UserBuf[2], GameEchoBuf[2]); gotoxy(109, 18); printf("¦¢\n");
	printf("¦¢%s : %s", UserBuf[1], GameEchoBuf[1]); gotoxy(109, 19); printf("¦¢\n");
	printf("¦¢%s : %s", UserBuf[0], GameEchoBuf[0]); gotoxy(109, 20); printf("¦¢\n");
	printf("¦§¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦©\n");
	printf("¦¢ÀÔ·Â :");  gotoxy(109, 22); printf("¦¢\n");
	printf("¦¦¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¥\n");
	gotoxy(9, 22);

}

void RoomSelectWindow() {
	gotoxy(1, 14);
	printf("¦£¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¤\n");
	printf("¦¢ %d¹ø¹æ : %d¸í", 1, RoomBuf[0]); gotoxy(109, 15); printf("¦¢\n");
	printf("¦¢ %d¹ø¹æ : %d¸í", 2, RoomBuf[1]); gotoxy(109, 16); printf("¦¢\n");
	printf("¦¢ %s", RoomMsg); gotoxy(109, 17); printf("¦¢\n");
	printf("¦¢"); gotoxy(109, 18); printf("¦¢\n");
	printf("¦¢"); gotoxy(109, 19); printf("¦¢\n");
	printf("¦¢"); gotoxy(109, 20); printf("¦¢\n");
	printf("¦§¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦©\n");
	printf("¦¢ÀÔ·Â :");  gotoxy(109, 22); printf("¦¢\n");
	printf("¦¦¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¥\n");
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
	printf("¦£¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¤\n");
	printf("¦¢                                                                                                           "); gotoxy(109, 15); printf("¦¢\n");
	printf("¦¢                                                                                                           "); gotoxy(109, 16); printf("¦¢\n");
	printf("¦¢                                                                                                           "); gotoxy(109, 17); printf("¦¢\n");
	printf("¦¢                                                                                                           "); gotoxy(109, 18); printf("¦¢\n");
	printf("¦¢                                                                                                           "); gotoxy(109, 19); printf("¦¢\n");
	printf("¦¢                                                                                                           "); gotoxy(109, 20); printf("¦¢\n");
	printf("¦§¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦©\n");
	printf("¦¢ÀÔ·Â :                                                                                                       "); gotoxy(109, 22);  printf("¦¢\n");
	printf("¦¦¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¥\n");
	gotoxy(9, 22);
}
void ErrorHandling(char *msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
#include "GameHdr.h"
#include "ConsoleHdr.h"

void Game_Recv_Thread(void* sock){
	int i;
	InitializeCriticalSection(&ChatCS);

	memset(UserBuf, 0, 13*6);
	while(Do_Game){
		if(recv(sock, &MsgType, 1, 0)<0)
			break;
		if(MsgType==SEND_CHAT_MSG){
			Sleep(1);//������. CPU ������ ����.
			recv(sock, &Recv_ChatMsgSize, 4, 0);
			memset(Recv_EchoString, 0, Recv_ChatMsgSize);
			recv(sock, &Recv_EchoString, Recv_ChatMsgSize, 0);
			/* ä�� �޽��� ��� �κ� ���� */
			for(i=0;i<CHAT_BUF_SIZE;i++){
				GameEchoBuf[5][i]=GameEchoBuf[4][i];
				GameEchoBuf[4][i]=GameEchoBuf[3][i];
				GameEchoBuf[3][i]=GameEchoBuf[2][i];
				GameEchoBuf[2][i]=GameEchoBuf[1][i];
				GameEchoBuf[1][i]=GameEchoBuf[0][i];
			}
			for(i=0;i<CHAT_BUF_SIZE;i++){
				GameEchoBuf[0][i]=Recv_EchoString[i];
			}
			
			/* ä�� �۽��� ��� �κ� ���� */
			for(i=0;i<USER_ID_SIZE;i++){
				UserBuf[5][i]=UserBuf[4][i];
				UserBuf[4][i]=UserBuf[3][i];
				UserBuf[3][i]=UserBuf[2][i];
				UserBuf[2][i]=UserBuf[1][i];
				UserBuf[1][i]=UserBuf[0][i];
			}
			
			for(i=0;i<USER_ID_SIZE;i++){
				if(Turn==BLUE && myturn)
					UserBuf[0][i]=User2[i];
				if(Turn==BLUE && !myturn)
					UserBuf[0][i]=User1[i];
				if(Turn==RED && myturn)
					UserBuf[0][i]=User1[i];
				if(Turn==RED && !myturn)
					UserBuf[0][i]=User2[i];
			}
		}
		
		EnterCriticalSection(&ChatCS);
		GameChatClear();
		LeaveCriticalSection(&ChatCS);
		
		EnterCriticalSection(&ChatCS);
		GameChatWindow();
		LeaveCriticalSection(&ChatCS);
		
		if(kill_chat)
			break;
	}
	
	DeleteCriticalSection(&ChatCS);
	return;
}

void Game_Send_Thread(void* sock){
	int i;
	InitializeCriticalSection(&ChatCS);
	
	while(Do_Game){
		Sleep(1);//������. CPU ������ ����.
		if(!myturn && kbhit()){//kbhit()�� getch() ���� �뵵.
			Action=getch();
			if(Action==13)
				chat_flag=1;
		}
		
		if(chat_flag){
			//while(1){
			memset(Send_EchoString, 0, Send_ChatMsgSize);
			show_cursor(1);
			
			EnterCriticalSection(&ChatCS);
			gotoxy(33, 22); printf("\r���Է� : \t\t\t\t\t\t\t\t");
			LeaveCriticalSection(&ChatCS);
			
			gotoxy(10, 22);
			EnterCriticalSection(&ChatCS);
			//gets(Send_EchoString);
			if( InputString(Send_EchoString, sizeof(Send_EchoString)) ){
				//show_cursor(0);
				//break; //���� ä���� ������� �ϴ� ��ƾ. �ֿ� ����� �ƴϹǷ� �ϴ� ����.
			}
			LeaveCriticalSection(&ChatCS);
			show_cursor(0);
			
			EnterCriticalSection(&ChatCS);
			gotoxy(33, 22); printf("\r���Է� : \t\t\t\t\t\t\t\t");
			LeaveCriticalSection(&ChatCS);
			
			MsgType=SEND_CHAT_MSG;
			Send_ChatMsgSize=sizeof(Send_EchoString);
			send(sock, &MsgType, 1, 0);
			send(sock, &Send_ChatMsgSize, 4, 0);
			send(sock, &Send_EchoString, Send_ChatMsgSize, 0);
			
			/* ä�� �޽��� ��� �κ� ���� */
			for(i=0;i<CHAT_BUF_SIZE;i++){
				GameEchoBuf[5][i]=GameEchoBuf[4][i];
				GameEchoBuf[4][i]=GameEchoBuf[3][i];
				GameEchoBuf[3][i]=GameEchoBuf[2][i];
				GameEchoBuf[2][i]=GameEchoBuf[1][i];
				GameEchoBuf[1][i]=GameEchoBuf[0][i];
			}
			for(i=0;i<CHAT_BUF_SIZE;i++){
				GameEchoBuf[0][i]=Send_EchoString[i];
			}
			
			/* ä�� �۽��� ��� �κ� ���� */
			for(i=0;i<USER_ID_SIZE;i++){
				UserBuf[5][i]=UserBuf[4][i];
				UserBuf[4][i]=UserBuf[3][i];
				UserBuf[3][i]=UserBuf[2][i];
				UserBuf[2][i]=UserBuf[1][i];
				UserBuf[1][i]=UserBuf[0][i];
			}
			
			for(i=0;i<USER_ID_SIZE;i++){
				if(Turn==BLUE && myturn)
					UserBuf[0][i]=User1[i];
				if(Turn==BLUE && !myturn)
					UserBuf[0][i]=User2[i];
				if(Turn==RED && myturn)
					UserBuf[0][i]=User2[i];
				if(Turn==RED && !myturn)
					UserBuf[0][i]=User1[i];
			}
			
			EnterCriticalSection(&ChatCS);
			GameChatClear();
			LeaveCriticalSection(&ChatCS);
			
			EnterCriticalSection(&ChatCS);
			GameChatWindow();
			LeaveCriticalSection(&ChatCS);
			
			chat_flag=0;
			Lock=0;
		}
		
		if(kill_chat)
			break;
	}
	
	DeleteCriticalSection(&ChatCS);
	return;
}

void GameChatClear(){
	gotoxy(1, 14);
	printf("������������������������������������������������������������������������������\n");
	printf("��\t\t\t\t\t\t\t\t"); gotoxy(77, 15); printf("��\n");
	printf("��\t\t\t\t\t\t\t\t"); gotoxy(77, 16); printf("��\n");
	printf("��\t\t\t\t\t\t\t\t"); gotoxy(77, 17); printf("��\n");
	printf("��\t\t\t\t\t\t\t\t"); gotoxy(77, 18); printf("��\n");
	printf("��\t\t\t\t\t\t\t\t"); gotoxy(77, 19); printf("��\n");
	printf("��\t\t\t\t\t\t\t\t"); gotoxy(77, 20); printf("��\n");
}
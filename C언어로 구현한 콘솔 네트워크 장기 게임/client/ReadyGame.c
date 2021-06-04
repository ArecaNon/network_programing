#include "GameHdr.h"
#include "ConsoleHdr.h"

void Wait_Host(){
	int i;
	win_flag=YET_WIN;
	system("cls");
	printf("Room Number : %d", RoomNum);
	gotoxy(22, 1); printf("┏━━━━━━━━━━━━━━━━━━┓\n");
	for(i=0;i<10;i++){
		gotoxy(22, i+2); printf("┃");	gotoxy(60, i+2); printf("┃\n");
	}
	gotoxy(22, i+2); printf("┗━━━━━━━━━━━━━━━━━━┛\n");
	printf("\t\t\t다른 사용자를 대기하고 있습니다.");
	printf("\n");//15~21
	printf("┌─────────────────────────────────────┐\n");
	for(i=15;i<21;i++){
		gotoxy(0, i); printf("│");	gotoxy(77, i); printf("│\n");
	}
	printf("├─────────────────────────────────────┤\n");
	printf("│입력 : 게임 시작 후 채팅 기능을 사용하십시오."); gotoxy(77, 22); printf("│\n");
	printf("└─────────────────────────────────────┘\n");
	printf("[F6] 대기실로");
	
	hThreadCon=(HANDLE)_beginthreadex(NULL, 0, Connect_Thread, NULL, 0, (unsigned*)&dwThreadIDCon);//게스트 대기는 별도의 스레드로 수행.
}

void ReadyGame_Host(int sock, int ChatSock){
	int Cursor;
	
	recv(sock, &User2, sizeof(User2), 0);
	recv(sock, &User_Rank2, 4, 0);
	
	Cursor=SelectBoardType(sock);
	
	kill_chat=0;
	
	gotoxy(15, 13); printf("\t\t\t\t\t\t\t\t\t");
	gotoxy(22, 13); printf("상대의 준비 완료 메시지를 대기하고 있습니다.");
	recv(sock, &MsgType, 1, 0);//게스트의 게임 준비가 완료되었음을, 혹은 게임 준비 중 종료하였음을 통보받음.
	
	if(MsgType==GAME_READY){//호스트로부터 준비 완료를 통보받은 후 게임 시작을 게스트에게 통보.
		MsgType=GAME_START;
		send(sock, &MsgType, 1, 0);
	}
	
	switch(Cursor){//선택된 상/마 배치 상태를 저장.
	case 0:
		Host_Board_Type=EKEK;
		break;
	case 1:
		Host_Board_Type=EKKE;
		break;
	case 2:
		Host_Board_Type=KEKE;
		break;
	case 3:
		Host_Board_Type=KEEK;
		break;
	}
	send(sock, &Host_Board_Type, 4, 0);//호스트 측의 배치 정보 send.
	recv(sock, &Guest_Board_Type, 4, 0);//게스트 측의 배치 정보 recv.
	
	hThreadRecv = (HANDLE)_beginthreadex(NULL, 0, Game_Recv_Thread, ChatSock, 0, (unsigned*)&dwThreadIDRecv);
	hThreadSend = (HANDLE)_beginthreadex(NULL, 0, Game_Send_Thread, ChatSock, 0, (unsigned*)&dwThreadIDSend);
}

void ReadyGame_Guest(int sock, int ChatSock){
	int Cursor;
	
	send(sock, &User2, sizeof(User2), 0);//요놈들은 서버에서 직접 할 수 있도록 하자.
	send(sock, &User_Rank2, 4, 0);
	
	Cursor=SelectBoardType(sock);
	
	win_flag=YET_WIN;
	
	kill_chat=0;
	
	send(sock, &MsgType, 1, 0);//게임 준비가 완료되었음을, 혹은 게임 준비 중 종료하였음을 호스트에게 통보.
	
	gotoxy(15, 13); printf("\t\t\t\t\t\t\t\t\t");
	gotoxy(22, 13); printf("방장의 게임 시작 메시지를 대기하고 있습니다.");

	if(recv(sock, &MsgType, 1, 0)<0){//게임 시작을 호스트로부터 통보 받음.
		gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
		gotoxy(25, 13); printf("상대가 게임을 종료하여 승점이 추가됩니다.\n");
		Do_Game=0;
		if(ImHost)
			All_continue=1;
		if(!ImHost)
			All_continue=0;
		return;
	}
	
	switch(Cursor){//선택된 상/마 배치 상태를 저장.
	case 0:
		Guest_Board_Type=EKEK;
		break;
	case 1:
		Guest_Board_Type=EKKE;
		break;
	case 2:
		Guest_Board_Type=KEKE;
		break;
	case 3:
		Guest_Board_Type=KEEK;
		break;
	}
	
	send(sock, &Guest_Board_Type, 4, 0);//게스트 측의 배치 정보 send.
	recv(sock, &Host_Board_Type, 4, 0);//호스트 측의 배치 정보 recv.
	
	hThreadRecv = (HANDLE)_beginthreadex(NULL, 0, Game_Recv_Thread, ChatSock, 0, (unsigned*)&dwThreadIDRecv);
	hThreadSend = (HANDLE)_beginthreadex(NULL, 0, Game_Send_Thread, ChatSock, 0, (unsigned*)&dwThreadIDSend);
}

int SelectBoardType(int sock){
	int i;
	int Cursor=0;
	char getButton;
	
	Action=0;
	while(Action!=32){
		system("cls");
		ShowUser();//유저 목록.
		gotoxy(22, 1); printf("┏━━━━━━━━━━━━━━━━━━┓\n");
		for(i=0;i<10;i++){
			gotoxy(22, i+2); printf("┃");	gotoxy(60, i+2); printf("┃\n");
		}
		gotoxy(22, i+2); printf("┗━━━━━━━━━━━━━━━━━━┛\n");
		gotoxy(22, 13); printf("space 키로 象과 馬의 배치를 정하십시오.");
		gotoxy(8, 14); printf("[경기 시작 3수 이전에는 방을 나가셔도 전적에 영향을 미치지 않습니다.]");
		
		gotoxy(33, 2);printf("[  ] 象 馬 象 馬 ↑\n");
		gotoxy(33, 4);printf("[  ] 象 馬 馬 象\n");
		gotoxy(33, 6);printf("[  ] 馬 象 馬 象\n");
		gotoxy(33, 8);printf("[  ] 馬 象 象 馬 ↓\n");
		
		gotoxy(34, Cursor*2+2);printf("√");
		
		getButton=getch();
		Action=getButton;
		
		switch(Action){
		case 72 ://up
			if(Cursor>0)
				Cursor--;
			break;
		case 80 ://down
			if(Cursor<3)
				Cursor++;
			break;
		case 32://space
			MsgType=GAME_READY;
			break;
		}
	}
	return Cursor;
}

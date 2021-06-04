#include "GameHdr.h"
#include "ConsoleHdr.h"

void Wait_Host(){
	int i;
	win_flag=YET_WIN;
	system("cls");
	printf("Room Number : %d", RoomNum);
	gotoxy(22, 1); printf("����������������������������������������\n");
	for(i=0;i<10;i++){
		gotoxy(22, i+2); printf("��");	gotoxy(60, i+2); printf("��\n");
	}
	gotoxy(22, i+2); printf("����������������������������������������\n");
	printf("\t\t\t�ٸ� ����ڸ� ����ϰ� �ֽ��ϴ�.");
	printf("\n");//15~21
	printf("������������������������������������������������������������������������������\n");
	for(i=15;i<21;i++){
		gotoxy(0, i); printf("��");	gotoxy(77, i); printf("��\n");
	}
	printf("������������������������������������������������������������������������������\n");
	printf("���Է� : ���� ���� �� ä�� ����� ����Ͻʽÿ�."); gotoxy(77, 22); printf("��\n");
	printf("������������������������������������������������������������������������������\n");
	printf("[F6] ���Ƿ�");
	
	hThreadCon=(HANDLE)_beginthreadex(NULL, 0, Connect_Thread, NULL, 0, (unsigned*)&dwThreadIDCon);//�Խ�Ʈ ���� ������ ������� ����.
}

void ReadyGame_Host(int sock, int ChatSock){
	int Cursor;
	
	recv(sock, &User2, sizeof(User2), 0);
	recv(sock, &User_Rank2, 4, 0);
	
	Cursor=SelectBoardType(sock);
	
	kill_chat=0;
	
	gotoxy(15, 13); printf("\t\t\t\t\t\t\t\t\t");
	gotoxy(22, 13); printf("����� �غ� �Ϸ� �޽����� ����ϰ� �ֽ��ϴ�.");
	recv(sock, &MsgType, 1, 0);//�Խ�Ʈ�� ���� �غ� �Ϸ�Ǿ�����, Ȥ�� ���� �غ� �� �����Ͽ����� �뺸����.
	
	if(MsgType==GAME_READY){//ȣ��Ʈ�κ��� �غ� �ϷḦ �뺸���� �� ���� ������ �Խ�Ʈ���� �뺸.
		MsgType=GAME_START;
		send(sock, &MsgType, 1, 0);
	}
	
	switch(Cursor){//���õ� ��/�� ��ġ ���¸� ����.
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
	send(sock, &Host_Board_Type, 4, 0);//ȣ��Ʈ ���� ��ġ ���� send.
	recv(sock, &Guest_Board_Type, 4, 0);//�Խ�Ʈ ���� ��ġ ���� recv.
	
	hThreadRecv = (HANDLE)_beginthreadex(NULL, 0, Game_Recv_Thread, ChatSock, 0, (unsigned*)&dwThreadIDRecv);
	hThreadSend = (HANDLE)_beginthreadex(NULL, 0, Game_Send_Thread, ChatSock, 0, (unsigned*)&dwThreadIDSend);
}

void ReadyGame_Guest(int sock, int ChatSock){
	int Cursor;
	
	send(sock, &User2, sizeof(User2), 0);//������ �������� ���� �� �� �ֵ��� ����.
	send(sock, &User_Rank2, 4, 0);
	
	Cursor=SelectBoardType(sock);
	
	win_flag=YET_WIN;
	
	kill_chat=0;
	
	send(sock, &MsgType, 1, 0);//���� �غ� �Ϸ�Ǿ�����, Ȥ�� ���� �غ� �� �����Ͽ����� ȣ��Ʈ���� �뺸.
	
	gotoxy(15, 13); printf("\t\t\t\t\t\t\t\t\t");
	gotoxy(22, 13); printf("������ ���� ���� �޽����� ����ϰ� �ֽ��ϴ�.");

	if(recv(sock, &MsgType, 1, 0)<0){//���� ������ ȣ��Ʈ�κ��� �뺸 ����.
		gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
		gotoxy(25, 13); printf("��밡 ������ �����Ͽ� ������ �߰��˴ϴ�.\n");
		Do_Game=0;
		if(ImHost)
			All_continue=1;
		if(!ImHost)
			All_continue=0;
		return;
	}
	
	switch(Cursor){//���õ� ��/�� ��ġ ���¸� ����.
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
	
	send(sock, &Guest_Board_Type, 4, 0);//�Խ�Ʈ ���� ��ġ ���� send.
	recv(sock, &Host_Board_Type, 4, 0);//ȣ��Ʈ ���� ��ġ ���� recv.
	
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
		ShowUser();//���� ���.
		gotoxy(22, 1); printf("����������������������������������������\n");
		for(i=0;i<10;i++){
			gotoxy(22, i+2); printf("��");	gotoxy(60, i+2); printf("��\n");
		}
		gotoxy(22, i+2); printf("����������������������������������������\n");
		gotoxy(22, 13); printf("space Ű�� �ڰ� ة�� ��ġ�� ���Ͻʽÿ�.");
		gotoxy(8, 14); printf("[��� ���� 3�� �������� ���� �����ŵ� ������ ������ ��ġ�� �ʽ��ϴ�.]");
		
		gotoxy(33, 2);printf("[  ] �� ة �� ة ��\n");
		gotoxy(33, 4);printf("[  ] �� ة ة ��\n");
		gotoxy(33, 6);printf("[  ] ة �� ة ��\n");
		gotoxy(33, 8);printf("[  ] ة �� �� ة ��\n");
		
		gotoxy(34, Cursor*2+2);printf("��");
		
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

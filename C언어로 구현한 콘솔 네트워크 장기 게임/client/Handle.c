#include "GameHdr.h"
#include "ConsoleHdr.h"

void PlayGame(int sock){
	int select_flag=0;
	int undo_flag=0;
	int draw_flag=0;
	chat_flag=0;
	myturn=1;
	selected=EMPTY;
	selected_x=-1;
	selected_y=-1;//���� �ʱ�ȭ.
	
	while(myturn) {
		InitializeCriticalSection(&BoardCS);
		EnterCriticalSection(&BoardCS);//����� ä�� �޽��� ���޿� ���� �߻��ϴ� ������ ������.
		system("cls");
		ShowUser();
		LeaveCriticalSection(&BoardCS);
		
		EnterCriticalSection(&BoardCS);
		ShowBoard(x, y);
		printf("\n");
		PrintManual(selected);//�ȳ�â.
		printf("\n");
		LeaveCriticalSection(&BoardCS);
		
		EnterCriticalSection(&BoardCS);
		GameChatWindow();//ä��â.
		LeaveCriticalSection(&BoardCS);
		DeleteCriticalSection(&BoardCS);
		
		if(Victory(sock)){
			Do_Game=0;//����� ������ �� ���� ����.
		}
		
		if(Turn==BLUE)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		else if(Turn==RED)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(cursor_x*4+24, cursor_y+2);
		printf("[");
		gotoxy(cursor_x*4+27, cursor_y+2);
		printf("]");//Ŀ�� ���
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		
		while (1) {//F1~12�� �ƽ�Ű �ڵ尡 �Ҵ���� �ʾƼ� ���ϴ� ��ġ.
			Action = getch();
			if(Action!=0)
				break;
		}
		switch(Action){
		case 72 ://up
			if(cursor_y>0)
				cursor_y--;
			break;
		case 80 ://down
			if(cursor_y<9)
				cursor_y++;
			break;
		case 75 ://left
			if(cursor_x>0)
				cursor_x--;
			break;
		case 77 ://right
			if(cursor_x<8)
				cursor_x++;
			break;
		case 32 ://space
			
			for(y=0;y<10;y++){//������ ����� ���� �⹰ ���� ���.
				for(x=0;x<9;x++){
					undo_Board[x][y]=Board[x][y];
				}
			}
			if(select_flag && Board[cursor_x][cursor_y]/10!=Turn){//�⹰�� ���õ� ���¿��� �̵� ��ġ ���� ��.
				moved_x=cursor_x;
				moved_y=cursor_y;
				if(MovePiece()){		
					myturn=0;
					if(Board[moved_x][moved_y]%1000==1){//�̵��� ��ġ�� ������� ���� ���� ��� �¸� ó��.
						if(Board[moved_x][moved_y]/1000==2)
							win_flag=BLUE_WIN;
						if(Board[moved_x][moved_y]/1000==1)
							win_flag=RED_WIN;
						MsgType=NOTICE_WIN;
						send(sock, &MsgType, 1, 0);
						send(sock, &win_flag, 4, 0);
					}
					
					Board[moved_x][moved_y]=selected;
					Board[selected_x][selected_y]=EMPTY;//�⹰�� ��ǥ�� �̵�.
					
					Move_Count=Move_Count++;
					
					MsgType=MOVE_PIECE;
					send(sock, &MsgType, 1, 0);//�޽��� Ÿ�� ����.
					
					send(sock, &selected, 4, 0);//������ �⹰ ���� ����.
					send(sock, &selected_x, 4, 0);
					send(sock, &selected_y, 4, 0);
					send(sock, &moved_x, 4, 0);
					send(sock, &moved_y, 4, 0);//�⹰�� ��ġ ��ȭ�� ������ ȭ�鿡 �ݿ��ϱ� ���� ����/���� ��ǥ send.
					
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					
					selected=EMPTY;
					selected_x=-1;
					selected_y=-1;//���� �ʱ�ȭ.
					
				}
			}
			if(!select_flag && Board[cursor_x][cursor_y]!=EMPTY && Board[cursor_x][cursor_y]/10==Turn){//�⹰ ���� ��.
				select_flag=1;
				selected=Board[cursor_x][cursor_y];//�ش� ��ġ�� �⹰�� ����.
				selected_x=cursor_x;
				selected_y=cursor_y;//���õ� �⹰��  ���ȣ�� ǥ���ϱ� ����.
			}
			
			break;
		case 27 ://ESC
			select_flag=0;
			selected=EMPTY;
			selected_x=-1;
			selected_y=-1;
			break;
		case 13://Enter
			chat_flag=1;
			Lock=1;
			while(Lock){}//ä�� �Ϸ���� ���� ��.
			break;
		case 59://F1
			MsgType=REQUEST_UNDO;
			if(!undo_flag && Move_Count>2){
				send(sock, &MsgType, 1, 0);//������ ��û.
				gotoxy(22, 13); printf("��û�� ���� ������ ����� ��ٸ��� �ֽ��ϴ�.");
				recv(sock, &MsgType, 1, 0);//���� ���.
				if(MsgType==ACCEPT_UNDO){
					for(y=0;y<10;y++){//�⹰ ���� ����
						for(x=0;x<9;x++){
							Board[x][y]=undo_Board[x][y];
						}
					}
					send(sock, Board, 360, 0);//������ �⹰ ���¸� ��뿡�� ����.
					Move_Count=Move_Count--;
				}else if(MsgType==DENY_UNDO){
					gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
					gotoxy(23, 13); printf("���濡�� ������ ��û�� �������߽��ϴ�.");
					getch();
				}
				undo_flag=1;
			}else if(undo_flag){
				gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
				gotoxy(22, 13); printf("������ ��û�� �� ���� �� ���� �����մϴ�.");
				getch();
			}else if(Move_Count<3){
				gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
				gotoxy(20, 13); printf("3�� �̻� �����Ͽ��� �����Ⱑ �����մϴ�. ���� %d��.", Move_Count);
				getch();
			}
			break;
		case 60://F2
			MsgType=REQUEST_DRAW;
			if(!draw_flag){
				send(sock, &MsgType, 1, 0);//���º� ��û.
				gotoxy(22, 13); printf("��û�� ���� ������ ����� ��ٸ��� �ֽ��ϴ�.");
				recv(sock, &MsgType, 1, 0);//���� ���.
				if(MsgType==ACCEPT_DRAW){
					win_flag=DRAW_WIN;
					return;
				}else if(MsgType==DENY_DRAW){
					gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
					gotoxy(23, 13); printf("���濡�� ���º� ��û�� �������߽��ϴ�.");
					getch();
				}
				draw_flag=1;
			}else if(draw_flag){
				gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
				gotoxy(22, 13); printf("���º� ��û�� �� ���� �� ���� �����մϴ�.");
				getch();
			}
			break;
		case 63://F5
			gotoxy(13, 13); printf("��뿡�� �׺��Ͻðڽ��ϱ�? [Space] : Ȯ�� / [ESC] : ���");
			Action=0;
			while(Action!=32&&Action!=27)
				Action=getch();
			
			if(Action==32){
				myturn=0;
				if(Turn==RED)
					win_flag=BLUE_WIN;
				if(Turn==BLUE)
					win_flag=RED_WIN;
				MsgType=NOTICE_WIN;
				send(sock, &MsgType, 1, 0);
				send(sock, &win_flag, 4, 0);
			}
			
			break;
		case 64://F6
			gotoxy(13, 13); printf("���� ���� �뱹�� �����Ͻðڽ��ϱ�? [Space] : Ȯ�� / [ESC] : ���");
			Action=0;
			while(Action!=32&&Action!=27)
				Action=getch();
			
			if(Action==32){
				myturn=0;
				Do_Game=0;
				All_continue=0;
				MsgType=EXIT;
				send(sock, &MsgType, 1, 0);
			}
			break;
		}
	}
}

void WaitTurn(int sock){
	while(1){
		InitializeCriticalSection(&BoardCS);
		EnterCriticalSection(&BoardCS);//����� ä�� �޽��� ���޿� ���� �߻��ϴ� ������ ������.
		system("cls");
		ShowUser();
		LeaveCriticalSection(&BoardCS);
		
		EnterCriticalSection(&BoardCS);
		ShowBoard(x, y);
		printf("\n");
		printf("\t\t\t����� �Է��� ����ϰ� �ֽ��ϴ�.");
		printf("\n");
		LeaveCriticalSection(&BoardCS);
		
		EnterCriticalSection(&BoardCS);
		GameChatWindow();//ä��â.
		LeaveCriticalSection(&BoardCS);
		
		if(Victory(sock)){
			Do_Game=0;//����� ������ �� ���� ����.
			return;
		}
		
		DeleteCriticalSection(&BoardCS);
		
		/* �޽��� Ÿ�� ��� �� ���� ���� ���� ���� ó�� */
		if(recv(sock, &MsgType, 1, 0)<0){
			exit_flag=1;
			if(Turn==RED)
				win_flag=BLUE_WIN;
			if(Turn==BLUE)
				win_flag=RED_WIN;
			gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
			gotoxy(22, 13); printf("��밡 �Ϲ������� ������ �����Ͽ����ϴ�.\n");
			if(Move_Count<3){
				win_flag=DRAW_WIN;
			}
			Do_Game=0;
			if(ImHost){
				All_continue=1;
			}
			if(!ImHost){
				All_continue=0;
			}
			return;
		}
		
		/* �⹰ �̵�. */
		if(MsgType==MOVE_PIECE){
			if(recv(sock, &selected, 4, 0)<0 || recv(sock, &selected_x, 4, 0)<0 || recv(sock, &selected_y, 4, 0)<0 
				|| recv(sock, &moved_x, 4, 0)<0 || recv(sock, &moved_y, 4, 0)<0){//�⹰ ���� �� ��ǥ recv. ���� �߻� �� ���� ����.
				return;
			}
			Board[selected_x][selected_y]=EMPTY;
			Board[moved_x][moved_y]=selected;
			while(chat_flag){//ä�� �Է� ���߿��� ȭ�� ���� ����.
				Sleep(1);//CPU ������ ����.
			}
			break;
		}
		
		/* ������ ��û. */
		if(MsgType==REQUEST_UNDO){
			while(chat_flag){//ä�� �Է� ���߿��� ȭ�� ���� ����.
				Sleep(1);//CPU ������ ����.
			}
			gotoxy(13, 13); printf("������ ��û�� �����Ͻðڽ��ϱ�? [Space] : ���� / [ESC] : ����");
			Action=0;
			while(Action!=32&&Action!=27)
				Action=getch();
			
			if(Action==32){
				MsgType=ACCEPT_UNDO;
			}
			if(Action==27){
				MsgType=DENY_UNDO;
			}
			send(sock, &MsgType, 1, 0);//���� send.
			if(MsgType==ACCEPT_UNDO){
				recv(sock, Board, 360, 0);//���� ���·� �����ϱ� ���� �⹰ ���� recv.
				Move_Count=Move_Count-1;
			}
		}
		
		/* ���º� ��û */
		if(MsgType==REQUEST_DRAW){
			while(chat_flag){//ä�� �Է� ���߿��� ȭ�� ���� ����.
				Sleep(1);//CPU ������ ����.
			}
			gotoxy(13, 13); printf("���º� ��û�� �����Ͻðڽ��ϱ�? [Space] : ���� / [ESC] : ����");
			Action=0;
			while(Action!=32&&Action!=27)
				Action=getch();
			if(Action==32){
				MsgType=ACCEPT_DRAW;
				win_flag=DRAW_WIN;
			}
			if(Action==27){
				MsgType=DENY_DRAW;
			}
			send(sock, &MsgType, 1, 0);//���� send.
		}
		
		/* ���� ����(���� �¸�, Ȥ�� ������ �׺�) */
		if(MsgType==NOTICE_WIN){
			recv(sock, &win_flag, 4, 0);
		}
		
		/* ������ ���� ���� */
		if(MsgType==EXIT){
			if(Turn==RED)
				win_flag=BLUE_WIN;
			if(Turn==BLUE)
				win_flag=RED_WIN;
			gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
			gotoxy(22, 13); printf("��밡 �Ϲ������� ������ �����Ͽ����ϴ�.\n");
			if(Move_Count<3){
				win_flag=DRAW_WIN;
			}
			Do_Game=0;
			if(ImHost){
				All_continue=1;
			}
			if(!ImHost){
				All_continue=0;
			}
			return;
		}
	}
}

int Victory(int sock){
	if(win_flag!=YET_WIN){
		myturn=0;
		
		EnterCriticalSection(&BoardCS);
		gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
		if(win_flag==BLUE_WIN){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			gotoxy(10, 13); printf("���� �¸��Դϴ�.");
		}
		if(win_flag==RED_WIN){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			gotoxy(10, 13); printf("���� �¸��Դϴ�.");
		}
		if(win_flag==DRAW_WIN){
			gotoxy(10, 13); printf("���º��Դϴ�.");
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		printf(" ���� ����� ���� �� ��û�Ͻʽÿ�. [Space] : �����");
		LeaveCriticalSection(&BoardCS);
		Action=0;
		while(Action!=32 /*&& Action!=27*/){
			Action=getch();
			
			if(Action==13){
				chat_flag=1;
				Lock=1;
				while(Lock){}//ä�� �Ϸ���� ���� ��.
			}
			if(Action==32){
				All_continue=1;
				kill_chat=1;
				return 1;
			}
			/*if(Action==27){//���� ����� ����.
				All_continue=0;
				kill_chat=1;
				return 1;
			}*/
		}
		return 1;
	}
	return 0;
}

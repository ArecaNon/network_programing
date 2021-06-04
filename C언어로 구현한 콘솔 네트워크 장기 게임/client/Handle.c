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
	selected_y=-1;//정보 초기화.
	
	while(myturn) {
		InitializeCriticalSection(&BoardCS);
		EnterCriticalSection(&BoardCS);//상대의 채팅 메시지 전달에 의해 발생하는 간섭을 방지함.
		system("cls");
		ShowUser();
		LeaveCriticalSection(&BoardCS);
		
		EnterCriticalSection(&BoardCS);
		ShowBoard(x, y);
		printf("\n");
		PrintManual(selected);//안내창.
		printf("\n");
		LeaveCriticalSection(&BoardCS);
		
		EnterCriticalSection(&BoardCS);
		GameChatWindow();//채팅창.
		LeaveCriticalSection(&BoardCS);
		DeleteCriticalSection(&BoardCS);
		
		if(Victory(sock)){
			Do_Game=0;//결과가 나왔을 시 게임 종료.
		}
		
		if(Turn==BLUE)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		else if(Turn==RED)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(cursor_x*4+24, cursor_y+2);
		printf("[");
		gotoxy(cursor_x*4+27, cursor_y+2);
		printf("]");//커서 출력
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		
		while (1) {//F1~12에 아스키 코드가 할당되지 않아서 취하는 조치.
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
			
			for(y=0;y<10;y++){//무르기 기능을 위한 기물 상태 백업.
				for(x=0;x<9;x++){
					undo_Board[x][y]=Board[x][y];
				}
			}
			if(select_flag && Board[cursor_x][cursor_y]/10!=Turn){//기물이 선택된 상태에서 이동 위치 선택 시.
				moved_x=cursor_x;
				moved_y=cursor_y;
				if(MovePiece()){		
					myturn=0;
					if(Board[moved_x][moved_y]%1000==1){//이동한 위치에 상대편의 왕이 있을 경우 승리 처리.
						if(Board[moved_x][moved_y]/1000==2)
							win_flag=BLUE_WIN;
						if(Board[moved_x][moved_y]/1000==1)
							win_flag=RED_WIN;
						MsgType=NOTICE_WIN;
						send(sock, &MsgType, 1, 0);
						send(sock, &win_flag, 4, 0);
					}
					
					Board[moved_x][moved_y]=selected;
					Board[selected_x][selected_y]=EMPTY;//기물의 좌표를 이동.
					
					Move_Count=Move_Count++;
					
					MsgType=MOVE_PIECE;
					send(sock, &MsgType, 1, 0);//메시지 타입 전송.
					
					send(sock, &selected, 4, 0);//움직인 기물 정보 전송.
					send(sock, &selected_x, 4, 0);
					send(sock, &selected_y, 4, 0);
					send(sock, &moved_x, 4, 0);
					send(sock, &moved_y, 4, 0);//기물의 위치 변화를 상대방의 화면에 반영하기 위해 기존/현재 좌표 send.
					
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					
					selected=EMPTY;
					selected_x=-1;
					selected_y=-1;//정보 초기화.
					
				}
			}
			if(!select_flag && Board[cursor_x][cursor_y]!=EMPTY && Board[cursor_x][cursor_y]/10==Turn){//기물 선택 시.
				select_flag=1;
				selected=Board[cursor_x][cursor_y];//해당 위치의 기물을 선택.
				selected_x=cursor_x;
				selected_y=cursor_y;//선택된 기물을  대괄호로 표시하기 위함.
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
			while(Lock){}//채팅 완료까지 스핀 락.
			break;
		case 59://F1
			MsgType=REQUEST_UNDO;
			if(!undo_flag && Move_Count>2){
				send(sock, &MsgType, 1, 0);//무르기 요청.
				gotoxy(22, 13); printf("요청에 대한 상대방의 대답을 기다리고 있습니다.");
				recv(sock, &MsgType, 1, 0);//수락 대기.
				if(MsgType==ACCEPT_UNDO){
					for(y=0;y<10;y++){//기물 상태 복원
						for(x=0;x<9;x++){
							Board[x][y]=undo_Board[x][y];
						}
					}
					send(sock, Board, 360, 0);//복원된 기물 상태를 상대에게 전달.
					Move_Count=Move_Count--;
				}else if(MsgType==DENY_UNDO){
					gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
					gotoxy(23, 13); printf("상대방에게 무르기 요청을 거절당했습니다.");
					getch();
				}
				undo_flag=1;
			}else if(undo_flag){
				gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
				gotoxy(22, 13); printf("무르기 요청은 한 수에 한 번만 가능합니다.");
				getch();
			}else if(Move_Count<3){
				gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
				gotoxy(20, 13); printf("3수 이상 진행하여야 무르기가 가능합니다. 현재 %d수.", Move_Count);
				getch();
			}
			break;
		case 60://F2
			MsgType=REQUEST_DRAW;
			if(!draw_flag){
				send(sock, &MsgType, 1, 0);//무승부 요청.
				gotoxy(22, 13); printf("요청에 대한 상대방의 대답을 기다리고 있습니다.");
				recv(sock, &MsgType, 1, 0);//수락 대기.
				if(MsgType==ACCEPT_DRAW){
					win_flag=DRAW_WIN;
					return;
				}else if(MsgType==DENY_DRAW){
					gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
					gotoxy(23, 13); printf("상대방에게 무승부 요청을 거절당했습니다.");
					getch();
				}
				draw_flag=1;
			}else if(draw_flag){
				gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
				gotoxy(22, 13); printf("무승부 요청은 한 수에 한 번만 가능합니다.");
				getch();
			}
			break;
		case 63://F5
			gotoxy(13, 13); printf("상대에게 항복하시겠습니까? [Space] : 확인 / [ESC] : 취소");
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
			gotoxy(13, 13); printf("진행 중인 대국을 종료하시겠습니까? [Space] : 확인 / [ESC] : 취소");
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
		EnterCriticalSection(&BoardCS);//상대의 채팅 메시지 전달에 의해 발생하는 간섭을 방지함.
		system("cls");
		ShowUser();
		LeaveCriticalSection(&BoardCS);
		
		EnterCriticalSection(&BoardCS);
		ShowBoard(x, y);
		printf("\n");
		printf("\t\t\t상대의 입력을 대기하고 있습니다.");
		printf("\n");
		LeaveCriticalSection(&BoardCS);
		
		EnterCriticalSection(&BoardCS);
		GameChatWindow();//채팅창.
		LeaveCriticalSection(&BoardCS);
		
		if(Victory(sock)){
			Do_Game=0;//결과가 나왔을 시 게임 종료.
			return;
		}
		
		DeleteCriticalSection(&BoardCS);
		
		/* 메시지 타입 대기 및 상대방 강제 종료 시의 처리 */
		if(recv(sock, &MsgType, 1, 0)<0){
			exit_flag=1;
			if(Turn==RED)
				win_flag=BLUE_WIN;
			if(Turn==BLUE)
				win_flag=RED_WIN;
			gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
			gotoxy(22, 13); printf("상대가 일방적으로 접속을 해제하였습니다.\n");
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
		
		/* 기물 이동. */
		if(MsgType==MOVE_PIECE){
			if(recv(sock, &selected, 4, 0)<0 || recv(sock, &selected_x, 4, 0)<0 || recv(sock, &selected_y, 4, 0)<0 
				|| recv(sock, &moved_x, 4, 0)<0 || recv(sock, &moved_y, 4, 0)<0){//기물 정보 및 좌표 recv. 문제 발생 시 강제 종료.
				return;
			}
			Board[selected_x][selected_y]=EMPTY;
			Board[moved_x][moved_y]=selected;
			while(chat_flag){//채팅 입력 도중에는 화면 갱신 금지.
				Sleep(1);//CPU 점유율 제한.
			}
			break;
		}
		
		/* 무르기 요청. */
		if(MsgType==REQUEST_UNDO){
			while(chat_flag){//채팅 입력 도중에는 화면 갱신 금지.
				Sleep(1);//CPU 점유율 제한.
			}
			gotoxy(13, 13); printf("무르기 요청을 수락하시겠습니까? [Space] : 수락 / [ESC] : 거절");
			Action=0;
			while(Action!=32&&Action!=27)
				Action=getch();
			
			if(Action==32){
				MsgType=ACCEPT_UNDO;
			}
			if(Action==27){
				MsgType=DENY_UNDO;
			}
			send(sock, &MsgType, 1, 0);//응답 send.
			if(MsgType==ACCEPT_UNDO){
				recv(sock, Board, 360, 0);//이전 상태로 복원하기 위한 기물 상태 recv.
				Move_Count=Move_Count-1;
			}
		}
		
		/* 무승부 요청 */
		if(MsgType==REQUEST_DRAW){
			while(chat_flag){//채팅 입력 도중에는 화면 갱신 금지.
				Sleep(1);//CPU 점유율 제한.
			}
			gotoxy(13, 13); printf("무승부 요청을 수락하시겠습니까? [Space] : 수락 / [ESC] : 거절");
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
			send(sock, &MsgType, 1, 0);//응답 send.
		}
		
		/* 승패 결정(게임 승리, 혹은 상대방의 항복) */
		if(MsgType==NOTICE_WIN){
			recv(sock, &win_flag, 4, 0);
		}
		
		/* 상대방의 게임 종료 */
		if(MsgType==EXIT){
			if(Turn==RED)
				win_flag=BLUE_WIN;
			if(Turn==BLUE)
				win_flag=RED_WIN;
			gotoxy(22, 13); printf("\t\t\t\t\t\t\t");
			gotoxy(22, 13); printf("상대가 일방적으로 게임을 종료하였습니다.\n");
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
			gotoxy(10, 13); printf("楚의 승리입니다.");
		}
		if(win_flag==RED_WIN){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			gotoxy(10, 13); printf("漢의 승리입니다.");
		}
		if(win_flag==DRAW_WIN){
			gotoxy(10, 13); printf("무승부입니다.");
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		printf(" 게임 종료는 게임 중 요청하십시오. [Space] : 재시합");
		LeaveCriticalSection(&BoardCS);
		Action=0;
		while(Action!=32 /*&& Action!=27*/){
			Action=getch();
			
			if(Action==13){
				chat_flag=1;
				Lock=1;
				while(Lock){}//채팅 완료까지 스핀 락.
			}
			if(Action==32){
				All_continue=1;
				kill_chat=1;
				return 1;
			}
			/*if(Action==27){//종료 기능은 제한.
				All_continue=0;
				kill_chat=1;
				return 1;
			}*/
		}
		return 1;
	}
	return 0;
}

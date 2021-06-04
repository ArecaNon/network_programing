#include "ConsoleHdr.h"
#include "ConnectHdr.h"
#include "ClntHdr.h"

void WaitingRoom(int sock){
	hThread=(HANDLE)_beginthread(Recv_Thread, 0, sock);
	
	MsgType=ROOM_CHANGE_REQUEST;
	send(sock, &MsgType, 1, 0);
	MsgType=ROOM_MINUS_REQUEST;
	send(sock, &MsgType, 1, 0);//입장하자마자 방 개설 목록을 출력하기 위한 요청.

	InitializeCriticalSection(&WRoomCS);
	while(1){
		other_func=0;
		
		MsgType=RE_REQUEST;
		send(sock, &MsgType, 1, 0);
		send(sock, &MyNumber, 4, 0);//대국 종료 후 대기실로 나왔을 시 회원 정보 갱신을 위한 요청
		
		//여기서부터 대기실 화면
		system("cls");
		Action=0;
		EnterCriticalSection(&WRoomCS);
		printf("┐┌───────────────────────────────────┐┌\n");
		printf("││Room Number	: %d", room.room_number); gotoxy(75, 2); printf("││\n");
		printf("││Subject	: %s", room.room_name); gotoxy(75, 3); printf("││\n");
		printf("││Host Info.	: %s :: %d급", room.leader_ID, room.leader_rank); gotoxy(75, 4); printf("││\n");
		printf("┘└───────────────────────────────────┘└\n");
		printf("◁◁		좌/우 키와 스페이스 키로 방을 선택합니다.		  ▷▷\n");
		LeaveCriticalSection(&WRoomCS);
		
		EnterCriticalSection(&WRoomCS);
		WRoomChatWindow();
		LeaveCriticalSection(&WRoomCS);
		
		while (1) {//F1~12에 아스키 코드가 할당되지 않아서 취하는 조치.
			Action = getch();
			if(Action!=0)
				break;
		}
		
		if(Action==75 || Action==77){//방 목록 이동
			MsgType=ROOM_CHANGE_REQUEST;
			send(sock, &MsgType, 1, 0);
			if(Action==75){
				gotoxy(1, 6); printf("◀◀◀");
				MsgType=ROOM_MINUS_REQUEST;
				send(sock, &MsgType, 1, 0);
			}
			if(Action==77){
				gotoxy(73, 6); printf("▶▶▶");
				MsgType=ROOM_PLUS_REQUEST;
				send(sock, &MsgType, 1, 0);
			}
		}else if(Action==32 || Action==63){//방 참가. space / F6
			other_func=1;
			if(Action==63){
				EnterRoomNumber(sock);
				room.room_number=atoi(char_move_room_number);
			}
			
			MsgType=ENTER_ROOM_REQUEST;//실시간 화면이 아니므로 해당 방 정보가 유효한 지 파악할 필요가 있음. 메타 데이터를 주고 받아 확인하면 될듯 싶다.
			send(sock, &MsgType, 1, 0);
			send(sock, &Wait_index, 4, 0);//사용자를 게임 중으로 설정하기 위함.
			send(sock, &room.room_number, 4, 0);//해당 방을 게임 중으로 설정하기 위함.
			
			while(MsgType!=ENTER_ROOM_ACK && MsgType!=ENTER_ROOM_DENY){}
			Sleep(10);
			
			if(MsgType==ENTER_ROOM_ACK){
				system("cls");
				/////////////////////////////////
				/////////게임 메인 호출./////////
				/////////////////////////////////
				GameMain(2, ID, MyRank, room.IP, room.leader_ID, room.leader_rank, sock, room.host_port, room.room_number);
				Sleep(10);
				MsgType=EXIT_ROOM_REQUEST;
				send(sock, &MsgType, 1, 0);
				send(sock, &Wait_index, 4, 0);
				send(sock, &room.room_number, 4, 0);
			}else{
				memset(&room.room_number, 0, 4);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				gotoxy(19, 3); printf("[선택된 방은 더 이상 유효하지 않은 방입니다.]\t\t");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				getch();
			}
			
		}else if(Action==13){//채팅
			EnterCriticalSection(&WRoomCS);
			Send_Chat(sock);
			LeaveCriticalSection(&WRoomCS);
			
		}else if(Action==59){//F1 : 도움말
			system("cls");
			other_func=1;
			Help();
			getch();
			
		}else if(Action==60){//F2 : 내 정보
			system("cls");
			other_func=1;
			MsgType=MY_INFO_REQUEST;
			send(sock, &MsgType, 1, 0);
			send(sock, &ID, sizeof(ID), 0);
			printf("%s", ID);
			Show_MyInfo(sock);
			
		}else if(Action==61){//F3 : 방 개설
			other_func=1;
			if(!CreateRoom(sock)){//ESC를 누르면 input 함수가 1을 반환하므로 도중에 취소할 수 있도록 함.
				GenPortNumber();//포트 번호 생성.
				MsgType=CREATE_ROOM_REQUEST;
				Data_size=sizeof(Room_Name);
				send(sock, &MsgType, 1, 0);
				send(sock, &Data_size, 4, 0);
				send(sock, &Room_Name, Data_size, 0);
				send(sock, &Wait_index, 4, 0);
				send(sock, &HostPort, 6, 0);
				Sleep(10);
				/////////////////////////////////
				/////////게임 메인 호출./////////
				/////////////////////////////////
				GameMain(1, ID, MyRank, NULL, NULL, 0, sock, HostPort, room.room_number);
				Sleep(10);
				MsgType=BREAK_ROOM_REQUEST;
				send(sock, &MsgType, 1, 0);
				send(sock, &Wait_index, 4, 0);
				send(sock, &Room_index, 4, 0);
			}
			
		}else if(Action==62){//F4 : 대기자 리스트
			other_func=1;
			MsgType=WAIT_LIST_REQUEST;
			send(sock, &MsgType, 1, 0);
			//SuspendThread(hThread);
			WaitingList();
			//ResumeThread(hThread);
		}
		system("cls");
	}
	DeleteCriticalSection(&WRoomCS);
}

void WaitingList(){
	int i;//전체 회원 수 카운트
	int j=0;//접속 회원 수 카운트. 화면 표시를 위해 사용.
	int time=0;
	int nowlogin=0;
	
	system("cls");
	printf("now Loading");
	while(time<3){
		Sleep(333);
		printf(".");
		time++;
	}
	Sleep(1000);
	system("cls");
	
	gotoxy(10, 3); printf("┌────────────────────────────┐");
	gotoxy(10, 4); printf("│대기자 명단"); gotoxy(68, 4); printf("│");
	gotoxy(10, 5); printf("├────────────────────────────┤");
	for(i=0;i<MAX_USER;i++){
		if(mem_meta_data[i]==WAITING_MEMBER_SLOT){
			gotoxy(10, j+6); printf("│%d급 %s", WaitMem[i].rank, WaitMem[i].ID); gotoxy(68, j+6); printf("│");
			j++;
		}
	}
	gotoxy(10, j+6); printf("├────────────────────────────┤");
	gotoxy(10, j+7); printf("│ "); gotoxy(68, j+7); printf("│");
	gotoxy(10, j+8); printf("└────────────────────────────┘");
	gotoxy(12, j+9); printf("[ESC] : 이전 화면");
	gotoxy(27, j+7);
	
	getch();
	
	system("cls");
}

int CreateRoom(int sock){
	int ret;
	system("cls");
	gotoxy(3, 3); printf("┌────────────────────────────────────┐");
	gotoxy(3, 4); printf("│방 생성"); gotoxy(77, 4); printf("│");
	gotoxy(3, 5); printf("├────────────────────────────────────┤");
	gotoxy(3, 6); printf("│방 제목	: "); gotoxy(77, 6); printf("│");
	gotoxy(3, 7); printf("└────────────────────────────────────┘");
	gotoxy(3, 8); printf("[ESC] 이전 화면으로");
	gotoxy(19, 6); show_cursor(1); ret=InputMsg(Room_Name, sizeof(Room_Name)); show_cursor(0);
	
	return ret;
}

void EnterRoomNumber(int sock){
	system("cls");
	show_cursor(1);
	gotoxy(10, 3); printf("┌────────────────────────────┐");
	gotoxy(10, 4); printf("│방 번호로 입장"); gotoxy(68, 4); printf("│");
	gotoxy(10, 5); printf("├────────────────────────────┤");
	gotoxy(10, 6); printf("│방 번호	: "); gotoxy(68, 6); printf("│");
	gotoxy(10, 7); printf("└────────────────────────────┘");
	gotoxy(27, 6); InputNum(char_move_room_number, sizeof(char_move_room_number));
	show_cursor(0);
	system("cls");
}

void Show_MyInfo(int sock){
	int Acion;
	int compare_flag=0;
	
	system("cls");
	show_cursor(0);
	gotoxy(10, 3); printf("┌────────────────────────────┐");
	gotoxy(10, 4); printf("│내 정보 [전적 : %d승 %d패, %d급]", mem.win, mem.lose, mem.rank); gotoxy(68, 4); printf("│");
	gotoxy(10, 5); printf("├────────────────────────────┤");
	gotoxy(10, 6); printf("│");
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
	printf("일부 정보는 공개되지 않습니다."); 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	
	gotoxy(68, 6); printf("│");
	gotoxy(10, 7); printf("│"); gotoxy(68, 7); printf("│");
	gotoxy(10, 8); printf("│ID		: "); gotoxy(68, 8); printf("│");
	gotoxy(10, 9); printf("│"); gotoxy(68, 9); printf("│");
	gotoxy(10, 10); printf("│Password	: "); gotoxy(68, 10); printf("│");
	gotoxy(10, 11); printf("│"); gotoxy(68, 11); printf("│");
	gotoxy(10, 12); printf("│Password 확인: "); gotoxy(68, 12); printf("│");
	gotoxy(10, 13); printf("│"); gotoxy(68, 13); printf("│");
	gotoxy(10, 14); printf("│이름		: "); gotoxy(68, 14); printf("│");
	gotoxy(10, 15); printf("│"); gotoxy(68, 15); printf("│");
	gotoxy(10, 16); printf("│주민등록번호	: "); gotoxy(34, 16); printf("-"); gotoxy(68, 16); printf("│");
	gotoxy(10, 17); printf("│"); gotoxy(68, 17); printf("│");
	gotoxy(10, 18); printf("│E-mail 주소	: "); gotoxy(68, 18); printf("│");
	gotoxy(10, 19); printf("│"); gotoxy(68, 19); printf("│");
	gotoxy(10, 20); printf("└────────────────────────────┘");
	gotoxy(12, 21); printf("[Enter] : 정보 수정");
	gotoxy(12, 22); printf("[ESC] : 이전 화면");
	
	gotoxy(27, 8); printf("%s", mem.id);
	gotoxy(27, 10); printf("************");
	gotoxy(27, 12); printf("************");
	gotoxy(27, 14); printf("%s", mem.name);
	gotoxy(27, 16); printf("%s", mem.civil_number1); gotoxy(36, 16); printf("*******");
	gotoxy(27, 18); printf("%s", mem.email_address);
	
	Action=getch();
	if(Action==27)
		return;
	if(Action==13){
		gotoxy(27, 10); printf("\t\t\t\t\t");
		gotoxy(27, 12); printf("\t\t\t\t\t");
		gotoxy(27, 18); printf("\t\t\t\t\t");
		show_cursor(1);
		
		/* 패스워드 일치 검사 */
		while(!compare_flag){
			gotoxy(27, 10); InputPass(mem.pass, sizeof(mem.pass));
			gotoxy(27, 12); InputPass(Config_Pass, sizeof(Config_Pass));
			if(strcmp(mem.pass, Config_Pass)){
				compare_flag=0;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				show_cursor(0);
				gotoxy(27, 10); printf("Password가 일치하지 않습니다. [확인]");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				getch();
				show_cursor(1);
				gotoxy(27, 10); printf("\t\t\t\t\t");
				gotoxy(27, 12); printf("\t\t\t\t\t");
			}
			else
				break;
		}
		
		gotoxy(27, 18); InputValue(mem.email_address, sizeof(mem.email_address));

		MsgType=MODIFY_REQUEST;
		send(sock, &MsgType, 1, 0);
		send(sock, &mem.mem_number, 4, 0);
		send(sock, &mem.pass, sizeof(mem.pass), 0);
		send(sock, &mem.email_address, sizeof(mem.email_address), 0);

		gotoxy(12, 23); printf("회원 정보가 갱신되었습니다. [확인]");
		getch();
		
		show_cursor(0);
	}
}

void GenPortNumber(){
	unsigned short int port=0;
	unsigned short int temp=atoi(TempPort);
	
	while(1){
		if(port<1024 || port>65535 || temp==port){
			srand(GetTickCount());
			port=rand();
		}
		else
			break;
	}
	itoa(port, HostPort, 10);
	strcpy(TempPort, HostPort); 
}

void Help(){
	gotoxy(10, 2); printf("┌────────────────────────────┐");
	gotoxy(10, 3); printf("│도움말"); gotoxy(68, 3); printf("│");
	gotoxy(10, 4); printf("├────────────────────────────┤");
	gotoxy(10, 5); printf("│"); gotoxy(68, 5); printf("│");
	gotoxy(10, 6); printf("│"); gotoxy(68, 6); printf("│");
	gotoxy(10, 7); printf("│"); gotoxy(68, 7); printf("│");
	gotoxy(10, 8); printf("│"); gotoxy(68, 8); printf("│");
	gotoxy(10, 9); printf("│"); gotoxy(68, 9); printf("│");
	gotoxy(10, 10); printf("│"); gotoxy(68, 10); printf("│");
	gotoxy(10, 11); printf("│"); gotoxy(68, 11); printf("│");
	gotoxy(10, 12); printf("│"); gotoxy(68, 12); printf("│");
	gotoxy(10, 13); printf("│"); gotoxy(68, 13); printf("│");
	gotoxy(10, 14); printf("│"); gotoxy(68, 14); printf("│");
	gotoxy(10, 15); printf("│"); gotoxy(68, 15); printf("│");
	gotoxy(10, 16); printf("│"); gotoxy(68, 16); printf("│");
	gotoxy(10, 17); printf("│"); gotoxy(68, 17); printf("│");
	gotoxy(10, 18); printf("│"); gotoxy(68, 18); printf("│");
	gotoxy(10, 19); printf("│"); gotoxy(68, 19); printf("│");
	gotoxy(10, 20); printf("│"); gotoxy(68, 20); printf("│");
	gotoxy(10, 21); printf("│"); gotoxy(68, 21); printf("│");
	gotoxy(10, 22); printf("│"); gotoxy(68, 22); printf("│");
	gotoxy(10, 23); printf("└────────────────────────────┘");
	gotoxy(12, 24); printf("[ESC] : 이전 화면");
	
	
	gotoxy(12, 5); printf("1. 개설된 방 목록은 사용자 화면에 실시간으로 반영되지 않");
	gotoxy(12, 6); printf("으며, 좌/우 키를 이용하면 방 목록이 갱신됩니다.");
	
	gotoxy(12, 8); printf("2. 대국 시작 준비 중의 퇴장으로 인한 문제 발생 방지를 위");
	gotoxy(12, 9); printf("해 대국 시작 직전 및 종료 직전에는 퇴장이 불가합니다.");
	gotoxy(12, 10); printf("대국 시작 후 3수 이전의 퇴장은 전적에 영향을 끼치지 않으");
	gotoxy(12, 11); printf("므로 이 시점에 대국을 종료하시면 됩니다.");
	
	gotoxy(12, 13); printf("3. 동일한 IP의 호스트가 동시에 방을 개설 시 나중에 개설");
	gotoxy(12, 14); printf("된 방은 비정상적으로 작동하게 됩니다.");
	gotoxy(12, 15); printf("동일 IP 접속을 차단한다면 문제가 해결됩니다만 원활한 프");
	gotoxy(12, 16); printf("로젝트 테스트를 위해 그 해결 방법은 사용하지 않았습니다.");
	
	gotoxy(12, 18); printf("4. 빠른 결과 확인을 위해 승급 제도는 승리 시 승급, 패배");
	gotoxy(12, 19); printf("시 강등되는 간단한 방식을 취하고 있습니다.");
	
	gotoxy(12, 21); printf("5. 대국 종료 시 서버에는 전적 기록이 갱신되나 클라이언트");
	gotoxy(12, 22); printf("의 화면에는 기록이 즉시 반영되지 않습니다.");
}
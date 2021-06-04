#include "ConsoleHdr.h"
#include "ConnectHdr.h"
#include "ClntHdr.h"

void ShowMain(int sock){
	Login_flag=0;
	show_cursor(0);
	while(!Login_flag){
		system("cls");
		gotoxy(25, 5); printf("┌────────────┐\n");
		gotoxy(25, 6); printf("│LOGIN			  │\n");
		gotoxy(25, 7); printf("├────────────┤\n");
		gotoxy(25, 8); printf("│ID	: "); gotoxy(51, 8); printf("│");
		gotoxy(25, 9); printf("│PASS	: "); gotoxy(51, 9); printf("│");
		gotoxy(25, 10); printf("└────────────┘\n");
		
		gotoxy(27, 11); printf("[Enter] : 로그인\n");
		gotoxy(30, 12); printf("[F2] : 회원 가입\n");
		gotoxy(30, 13); printf("[F3] : ID/Pass 찾기\n");

		gotoxy(1, 1); printf("*테스트용 ID/Pass :\n");
		printf("test/test\n");
		printf("test/test1\n");
		printf("test/test2\n");
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(33, 17); printf("[Caution!]");
		gotoxy(3, 18); printf("동일 IP로 접속하여 동시에 방을 개설할 경우 문제가 발생합니다. 현재는 원활한");
		gotoxy(3, 19); printf("테스트 진행을 위해 동일 IP 접속이 가능하게 처리한 상태입니다만, server측의");
		gotoxy(3, 20); printf("AcceptLogin.c파일 34번 라인의 주석을 해제할 경우 동일 IP접근이 차단됩니다.");
		gotoxy(3, 21); printf("방 개설 이외의 모든 기능은 정상적으로 작동합니다.");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

		while (1) {//F1~12에 아스키 코드가 할당되지 않아서 취하는 조치.
			Action=getch();
			if(Action!=0)
				break;
		}

		/* 엔터 입력 시 로그인 수행.*/
		if(Action==13){
			gotoxy(30, 12); printf("\t\t\t");
			gotoxy(30, 13); printf("\t\t\t");
			Login_flag=Login(sock);
			if(!Login_flag){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				gotoxy(7, 11); printf("Warning! : 로그인에 실패하였습니다. 다음 사항을 확인해 주십시오 : ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				gotoxy(25, 12); printf("1. ID와 Password는 정확한가?");
				gotoxy(25, 13); printf("2. 현재 접속 중인 ID는 아닌가?");
				gotoxy(25, 14); printf("3. 현재 접속 중인 IP는 아닌가?");
				gotoxy(33, 15); printf("[Enter] 확인");
				Action=0;
				while(Action!=13)
					Action=getch();
			}
		}
		
		/* F2 입력 시 회원가입 수행.*/
		if(Action==60){
			Join(sock);
		}
		
		if(Action==61){
			FindIDPass(sock);
		}
	}
}

int Login(int sock){
	init_console();
	show_cursor(1);
	
	gotoxy(35, 8);
	InputValue(ID, sizeof(ID));
	gotoxy(35, 9);
	InputPass(pass, sizeof(pass));
	show_cursor(0);
	
	IDSize=sizeof(ID);
	PassSize=sizeof(pass);
	MsgType=LOGIN_REQUEST;
	
	send(sock, &MsgType, 1, 0);
	
	send(sock, &IDSize, 4, 0);
	send(sock, &ID, IDSize, 0);
	
	send(sock, &PassSize, 4, 0);
	send(sock, &pass, PassSize, 0);
	
	recv(sock, &MsgType, 1, 0);
	recv(sock, &Wait_index, 4, 0);
	recv(sock, &MyRank, 4, 0);
	recv(sock, &MyNumber, 4, 0);
	
	if(MsgType==LOGIN_ACCEPT){
		return 1;
	}
	if(MsgType==LOGIN_DENY){
		return 0;
	}

	return 0;
}

int Join(int sock){
	int compare_flag=0;
	
	system("cls");
	show_cursor(0);
	gotoxy(10, 3); printf("┌────────────────────────────┐");
	gotoxy(10, 4); printf("│회원가입"); gotoxy(68, 4); printf("│");
	gotoxy(10, 5); printf("├────────────────────────────┤");
	gotoxy(10, 6); printf("│"); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
	printf("ID와 Password는 영문 12자/한글 6자 이내로 지정하십시오."); 
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
	gotoxy(12, 21); printf("[Enter] : 입력 시작");
	gotoxy(12, 22); printf("[ESC] : 이전 화면");
	
	Action=0;
	while(Action!=13&&Action!=27)
		Action=getch();
	
	if(Action==27)
		return 0;
	
	if(Action==13){
		MsgType=JOIN_REQUEST;
		send(sock, &MsgType, 1, 0);
		gotoxy(12, 22); printf("\t\t\t\t");
		show_cursor(1);
		
		/* ID 중복 검사 */
		while(MsgType!=JOIN_ACCEPT){
			gotoxy(27, 8); InputValue(mem.id, sizeof(mem.id));
			Data_size=sizeof(mem.id);
			send(sock, &Data_size, 4, 0);
			send(sock, &mem.id, Data_size, 0);
			recv(sock, &MsgType, 1, 0);
			if(MsgType==JOIN_DENY){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				show_cursor(0);
				gotoxy(27, 8); printf("이미 존재하는 ID입니다. [확인]");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				getch();
				show_cursor(1);
				gotoxy(27, 8); printf("\t\t\t\t\t");
			}
		}
		
		MsgType=0;//추후 주민등록번호 검사를 위해 메시지 타입 변수 초기화.
		
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
		
		gotoxy(27, 14); InputValue(mem.name, sizeof(mem.name));
		
		/* 주민 번호 중복 검사 */
		while(MsgType!=JOIN_ACCEPT){
			gotoxy(27, 16); InputCivilNum(mem.civil_number1, sizeof(mem.civil_number1));
			Data_size=sizeof(mem.civil_number1);
			send(sock, &Data_size, 4, 0);
			send(sock, &mem.civil_number1, Data_size, 0);
			
			gotoxy(36, 16); InputCivilNum(mem.civil_number2, sizeof(mem.civil_number2));
			Data_size=sizeof(mem.civil_number2);
			send(sock, &Data_size, 4, 0);
			send(sock, &mem.civil_number2, Data_size, 0);
			
			recv(sock, &MsgType, 1, 0);
			
			if(MsgType==JOIN_DENY){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				show_cursor(0);
				gotoxy(27, 16); printf("이미 존재하는 주민 번호입니다. [확인]");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				getch();
				show_cursor(1);
				gotoxy(27, 16); printf("\t\t\t\t\t");
				gotoxy(34, 16); printf("-");
			}
		}
		gotoxy(27, 18); InputValue(mem.email_address, sizeof(mem.email_address));
		
		mem.rank=16;
		mem.win=0;
		mem.lose=0;
		Data_size=sizeof(mem);
		send(sock, &Data_size, 4, 0);
		send(sock, &mem, Data_size, 0);
		system("cls");
	}
	return 0;
}

void FindIDPass(int sock){
	system("cls");
	gotoxy(10, 3); printf("┌────────────────────────────┐");
	gotoxy(10, 4); printf("│ID/Pass 찾기"); gotoxy(68, 4); printf("│");
	gotoxy(10, 5); printf("├────────────────────────────┤");
	gotoxy(10, 6); printf("│이름		: "); gotoxy(68, 6); printf("│");
	gotoxy(10, 7); printf("│"); gotoxy(68, 7); printf("│");
	gotoxy(10, 8); printf("│주민등록번호	: "); gotoxy(34, 8); printf("-"); gotoxy(68, 8); printf("│");
	gotoxy(10, 9); printf("│"); gotoxy(68, 9); printf("│");
	gotoxy(10, 10); printf("│E-mail 주소	: "); gotoxy(68, 10); printf("│");
	gotoxy(10, 11); printf("│"); gotoxy(68, 11); printf("│");
	gotoxy(10, 12); printf("└────────────────────────────┘");
	gotoxy(10, 13); printf("[Enter] : 입력 시작");
	gotoxy(10, 14); printf("[ESC] : 이전 화면");
	
	Action=0;
	while(Action!=13&&Action!=27)
		Action=getch();
	
	if(Action==27)
		return;
	
	if(Action==13){
		show_cursor(1);
		gotoxy(27, 6); InputValue(mem.id, sizeof(mem.id));
		gotoxy(27, 8); InputCivilNum(mem.civil_number1, sizeof(mem.civil_number1));
		gotoxy(36, 8); InputCivilNum(mem.civil_number2, sizeof(mem.civil_number2));
		gotoxy(27, 10); InputValue(mem.email_address, sizeof(mem.email_address));
		MsgType=FIND_REQUEST;
		send(sock, &MsgType, 1, 0);
		send(sock, &mem.civil_number1, sizeof(mem.civil_number1), 0);
		send(sock, &mem.civil_number2, sizeof(mem.civil_number2), 0);
		send(sock, &mem.email_address, sizeof(mem.email_address), 0);
		
		recv(sock, &MsgType, 1, 0);
		if(MsgType==FIND_ACCEPT){
			recv(sock, &mem.id, sizeof(mem.id), 0);
			recv(sock, &mem.pass, sizeof(mem.pass), 0);

			system("cls");

			show_cursor(0);
			gotoxy(25, 5); printf("┌────────────┐\n");
			gotoxy(25, 6); printf("│LOGIN			  │\n");
			gotoxy(25, 7); printf("├────────────┤\n");
			gotoxy(25, 8); printf("│ID	: %s", mem.id); gotoxy(51, 8); printf("│");
			gotoxy(25, 9); printf("│PASS	: %s", mem.pass); gotoxy(51, 9); printf("│");
			gotoxy(25, 10); printf("└────────────┘\n");

			gotoxy(16, 11); printf("입력하신 정보에 대한 ID 정보는 위와 같습니다. \n \t \t \t \t [확인]");
			getch();
		}
		if(MsgType==FIND_DENY){

			system("cls");

			show_cursor(0);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			gotoxy(25, 5); printf("┌────────────┐\n");
			gotoxy(25, 6); printf("│LOGIN			  │\n");
			gotoxy(25, 7); printf("├────────────┤\n");
			gotoxy(25, 8); printf("│ID	: ?"); gotoxy(51, 8); printf("│");
			gotoxy(25, 9); printf("│PASS	: ?"); gotoxy(51, 9); printf("│");
			gotoxy(25, 10); printf("└────────────┘\n");
			gotoxy(14, 11); printf("입력하신 정보에 대한 ID 정보는 존재하지 않습니다. \n \t \t \t \t [확인]");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

			getch();
		}
		show_cursor(0);

		system("cls");
	}
}

#include "ConsoleHdr.h"
#include "ConnectHdr.h"
#include "ClntHdr.h"

void ShowMain(int sock){
	Login_flag=0;
	show_cursor(0);
	while(!Login_flag){
		system("cls");
		gotoxy(25, 5); printf("����������������������������\n");
		gotoxy(25, 6); printf("��LOGIN			  ��\n");
		gotoxy(25, 7); printf("����������������������������\n");
		gotoxy(25, 8); printf("��ID	: "); gotoxy(51, 8); printf("��");
		gotoxy(25, 9); printf("��PASS	: "); gotoxy(51, 9); printf("��");
		gotoxy(25, 10); printf("����������������������������\n");
		
		gotoxy(27, 11); printf("[Enter] : �α���\n");
		gotoxy(30, 12); printf("[F2] : ȸ�� ����\n");
		gotoxy(30, 13); printf("[F3] : ID/Pass ã��\n");

		gotoxy(1, 1); printf("*�׽�Ʈ�� ID/Pass :\n");
		printf("test/test\n");
		printf("test/test1\n");
		printf("test/test2\n");
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(33, 17); printf("[Caution!]");
		gotoxy(3, 18); printf("���� IP�� �����Ͽ� ���ÿ� ���� ������ ��� ������ �߻��մϴ�. ����� ��Ȱ��");
		gotoxy(3, 19); printf("�׽�Ʈ ������ ���� ���� IP ������ �����ϰ� ó���� �����Դϴٸ�, server����");
		gotoxy(3, 20); printf("AcceptLogin.c���� 34�� ������ �ּ��� ������ ��� ���� IP������ ���ܵ˴ϴ�.");
		gotoxy(3, 21); printf("�� ���� �̿��� ��� ����� ���������� �۵��մϴ�.");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

		while (1) {//F1~12�� �ƽ�Ű �ڵ尡 �Ҵ���� �ʾƼ� ���ϴ� ��ġ.
			Action=getch();
			if(Action!=0)
				break;
		}

		/* ���� �Է� �� �α��� ����.*/
		if(Action==13){
			gotoxy(30, 12); printf("\t\t\t");
			gotoxy(30, 13); printf("\t\t\t");
			Login_flag=Login(sock);
			if(!Login_flag){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				gotoxy(7, 11); printf("Warning! : �α��ο� �����Ͽ����ϴ�. ���� ������ Ȯ���� �ֽʽÿ� : ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				gotoxy(25, 12); printf("1. ID�� Password�� ��Ȯ�Ѱ�?");
				gotoxy(25, 13); printf("2. ���� ���� ���� ID�� �ƴѰ�?");
				gotoxy(25, 14); printf("3. ���� ���� ���� IP�� �ƴѰ�?");
				gotoxy(33, 15); printf("[Enter] Ȯ��");
				Action=0;
				while(Action!=13)
					Action=getch();
			}
		}
		
		/* F2 �Է� �� ȸ������ ����.*/
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
	gotoxy(10, 3); printf("������������������������������������������������������������");
	gotoxy(10, 4); printf("��ȸ������"); gotoxy(68, 4); printf("��");
	gotoxy(10, 5); printf("������������������������������������������������������������");
	gotoxy(10, 6); printf("��"); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
	printf("ID�� Password�� ���� 12��/�ѱ� 6�� �̳��� �����Ͻʽÿ�."); 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	
	gotoxy(68, 6); printf("��");
	gotoxy(10, 7); printf("��"); gotoxy(68, 7); printf("��");
	gotoxy(10, 8); printf("��ID		: "); gotoxy(68, 8); printf("��");
	gotoxy(10, 9); printf("��"); gotoxy(68, 9); printf("��");
	gotoxy(10, 10); printf("��Password	: "); gotoxy(68, 10); printf("��");
	gotoxy(10, 11); printf("��"); gotoxy(68, 11); printf("��");
	gotoxy(10, 12); printf("��Password Ȯ��: "); gotoxy(68, 12); printf("��");
	gotoxy(10, 13); printf("��"); gotoxy(68, 13); printf("��");
	gotoxy(10, 14); printf("���̸�		: "); gotoxy(68, 14); printf("��");
	gotoxy(10, 15); printf("��"); gotoxy(68, 15); printf("��");
	gotoxy(10, 16); printf("���ֹε�Ϲ�ȣ	: "); gotoxy(34, 16); printf("-"); gotoxy(68, 16); printf("��");
	gotoxy(10, 17); printf("��"); gotoxy(68, 17); printf("��");
	gotoxy(10, 18); printf("��E-mail �ּ�	: "); gotoxy(68, 18); printf("��");
	gotoxy(10, 19); printf("��"); gotoxy(68, 19); printf("��");
	gotoxy(10, 20); printf("������������������������������������������������������������");
	gotoxy(12, 21); printf("[Enter] : �Է� ����");
	gotoxy(12, 22); printf("[ESC] : ���� ȭ��");
	
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
		
		/* ID �ߺ� �˻� */
		while(MsgType!=JOIN_ACCEPT){
			gotoxy(27, 8); InputValue(mem.id, sizeof(mem.id));
			Data_size=sizeof(mem.id);
			send(sock, &Data_size, 4, 0);
			send(sock, &mem.id, Data_size, 0);
			recv(sock, &MsgType, 1, 0);
			if(MsgType==JOIN_DENY){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				show_cursor(0);
				gotoxy(27, 8); printf("�̹� �����ϴ� ID�Դϴ�. [Ȯ��]");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				getch();
				show_cursor(1);
				gotoxy(27, 8); printf("\t\t\t\t\t");
			}
		}
		
		MsgType=0;//���� �ֹε�Ϲ�ȣ �˻縦 ���� �޽��� Ÿ�� ���� �ʱ�ȭ.
		
		/* �н����� ��ġ �˻� */
		while(!compare_flag){
			gotoxy(27, 10); InputPass(mem.pass, sizeof(mem.pass));
			gotoxy(27, 12); InputPass(Config_Pass, sizeof(Config_Pass));
			if(strcmp(mem.pass, Config_Pass)){
				compare_flag=0;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				show_cursor(0);
				gotoxy(27, 10); printf("Password�� ��ġ���� �ʽ��ϴ�. [Ȯ��]");
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
		
		/* �ֹ� ��ȣ �ߺ� �˻� */
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
				gotoxy(27, 16); printf("�̹� �����ϴ� �ֹ� ��ȣ�Դϴ�. [Ȯ��]");
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
	gotoxy(10, 3); printf("������������������������������������������������������������");
	gotoxy(10, 4); printf("��ID/Pass ã��"); gotoxy(68, 4); printf("��");
	gotoxy(10, 5); printf("������������������������������������������������������������");
	gotoxy(10, 6); printf("���̸�		: "); gotoxy(68, 6); printf("��");
	gotoxy(10, 7); printf("��"); gotoxy(68, 7); printf("��");
	gotoxy(10, 8); printf("���ֹε�Ϲ�ȣ	: "); gotoxy(34, 8); printf("-"); gotoxy(68, 8); printf("��");
	gotoxy(10, 9); printf("��"); gotoxy(68, 9); printf("��");
	gotoxy(10, 10); printf("��E-mail �ּ�	: "); gotoxy(68, 10); printf("��");
	gotoxy(10, 11); printf("��"); gotoxy(68, 11); printf("��");
	gotoxy(10, 12); printf("������������������������������������������������������������");
	gotoxy(10, 13); printf("[Enter] : �Է� ����");
	gotoxy(10, 14); printf("[ESC] : ���� ȭ��");
	
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
			gotoxy(25, 5); printf("����������������������������\n");
			gotoxy(25, 6); printf("��LOGIN			  ��\n");
			gotoxy(25, 7); printf("����������������������������\n");
			gotoxy(25, 8); printf("��ID	: %s", mem.id); gotoxy(51, 8); printf("��");
			gotoxy(25, 9); printf("��PASS	: %s", mem.pass); gotoxy(51, 9); printf("��");
			gotoxy(25, 10); printf("����������������������������\n");

			gotoxy(16, 11); printf("�Է��Ͻ� ������ ���� ID ������ ���� �����ϴ�. \n \t \t \t \t [Ȯ��]");
			getch();
		}
		if(MsgType==FIND_DENY){

			system("cls");

			show_cursor(0);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			gotoxy(25, 5); printf("����������������������������\n");
			gotoxy(25, 6); printf("��LOGIN			  ��\n");
			gotoxy(25, 7); printf("����������������������������\n");
			gotoxy(25, 8); printf("��ID	: ?"); gotoxy(51, 8); printf("��");
			gotoxy(25, 9); printf("��PASS	: ?"); gotoxy(51, 9); printf("��");
			gotoxy(25, 10); printf("����������������������������\n");
			gotoxy(14, 11); printf("�Է��Ͻ� ������ ���� ID ������ �������� �ʽ��ϴ�. \n \t \t \t \t [Ȯ��]");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

			getch();
		}
		show_cursor(0);

		system("cls");
	}
}

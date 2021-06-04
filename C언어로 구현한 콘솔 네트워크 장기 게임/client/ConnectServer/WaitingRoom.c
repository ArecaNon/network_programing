#include "ConsoleHdr.h"
#include "ConnectHdr.h"
#include "ClntHdr.h"

void WaitingRoom(int sock){
	hThread=(HANDLE)_beginthread(Recv_Thread, 0, sock);
	
	MsgType=ROOM_CHANGE_REQUEST;
	send(sock, &MsgType, 1, 0);
	MsgType=ROOM_MINUS_REQUEST;
	send(sock, &MsgType, 1, 0);//�������ڸ��� �� ���� ����� ����ϱ� ���� ��û.

	InitializeCriticalSection(&WRoomCS);
	while(1){
		other_func=0;
		
		MsgType=RE_REQUEST;
		send(sock, &MsgType, 1, 0);
		send(sock, &MyNumber, 4, 0);//�뱹 ���� �� ���Ƿ� ������ �� ȸ�� ���� ������ ���� ��û
		
		//���⼭���� ���� ȭ��
		system("cls");
		Action=0;
		EnterCriticalSection(&WRoomCS);
		printf("������������������������������������������������������������������������������\n");
		printf("����Room Number	: %d", room.room_number); gotoxy(75, 2); printf("����\n");
		printf("����Subject	: %s", room.room_name); gotoxy(75, 3); printf("����\n");
		printf("����Host Info.	: %s :: %d��", room.leader_ID, room.leader_rank); gotoxy(75, 4); printf("����\n");
		printf("������������������������������������������������������������������������������\n");
		printf("����		��/�� Ű�� �����̽� Ű�� ���� �����մϴ�.		  ����\n");
		LeaveCriticalSection(&WRoomCS);
		
		EnterCriticalSection(&WRoomCS);
		WRoomChatWindow();
		LeaveCriticalSection(&WRoomCS);
		
		while (1) {//F1~12�� �ƽ�Ű �ڵ尡 �Ҵ���� �ʾƼ� ���ϴ� ��ġ.
			Action = getch();
			if(Action!=0)
				break;
		}
		
		if(Action==75 || Action==77){//�� ��� �̵�
			MsgType=ROOM_CHANGE_REQUEST;
			send(sock, &MsgType, 1, 0);
			if(Action==75){
				gotoxy(1, 6); printf("������");
				MsgType=ROOM_MINUS_REQUEST;
				send(sock, &MsgType, 1, 0);
			}
			if(Action==77){
				gotoxy(73, 6); printf("������");
				MsgType=ROOM_PLUS_REQUEST;
				send(sock, &MsgType, 1, 0);
			}
		}else if(Action==32 || Action==63){//�� ����. space / F6
			other_func=1;
			if(Action==63){
				EnterRoomNumber(sock);
				room.room_number=atoi(char_move_room_number);
			}
			
			MsgType=ENTER_ROOM_REQUEST;//�ǽð� ȭ���� �ƴϹǷ� �ش� �� ������ ��ȿ�� �� �ľ��� �ʿ䰡 ����. ��Ÿ �����͸� �ְ� �޾� Ȯ���ϸ� �ɵ� �ʹ�.
			send(sock, &MsgType, 1, 0);
			send(sock, &Wait_index, 4, 0);//����ڸ� ���� ������ �����ϱ� ����.
			send(sock, &room.room_number, 4, 0);//�ش� ���� ���� ������ �����ϱ� ����.
			
			while(MsgType!=ENTER_ROOM_ACK && MsgType!=ENTER_ROOM_DENY){}
			Sleep(10);
			
			if(MsgType==ENTER_ROOM_ACK){
				system("cls");
				/////////////////////////////////
				/////////���� ���� ȣ��./////////
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
				gotoxy(19, 3); printf("[���õ� ���� �� �̻� ��ȿ���� ���� ���Դϴ�.]\t\t");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				getch();
			}
			
		}else if(Action==13){//ä��
			EnterCriticalSection(&WRoomCS);
			Send_Chat(sock);
			LeaveCriticalSection(&WRoomCS);
			
		}else if(Action==59){//F1 : ����
			system("cls");
			other_func=1;
			Help();
			getch();
			
		}else if(Action==60){//F2 : �� ����
			system("cls");
			other_func=1;
			MsgType=MY_INFO_REQUEST;
			send(sock, &MsgType, 1, 0);
			send(sock, &ID, sizeof(ID), 0);
			printf("%s", ID);
			Show_MyInfo(sock);
			
		}else if(Action==61){//F3 : �� ����
			other_func=1;
			if(!CreateRoom(sock)){//ESC�� ������ input �Լ��� 1�� ��ȯ�ϹǷ� ���߿� ����� �� �ֵ��� ��.
				GenPortNumber();//��Ʈ ��ȣ ����.
				MsgType=CREATE_ROOM_REQUEST;
				Data_size=sizeof(Room_Name);
				send(sock, &MsgType, 1, 0);
				send(sock, &Data_size, 4, 0);
				send(sock, &Room_Name, Data_size, 0);
				send(sock, &Wait_index, 4, 0);
				send(sock, &HostPort, 6, 0);
				Sleep(10);
				/////////////////////////////////
				/////////���� ���� ȣ��./////////
				/////////////////////////////////
				GameMain(1, ID, MyRank, NULL, NULL, 0, sock, HostPort, room.room_number);
				Sleep(10);
				MsgType=BREAK_ROOM_REQUEST;
				send(sock, &MsgType, 1, 0);
				send(sock, &Wait_index, 4, 0);
				send(sock, &Room_index, 4, 0);
			}
			
		}else if(Action==62){//F4 : ����� ����Ʈ
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
	int i;//��ü ȸ�� �� ī��Ʈ
	int j=0;//���� ȸ�� �� ī��Ʈ. ȭ�� ǥ�ø� ���� ���.
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
	
	gotoxy(10, 3); printf("������������������������������������������������������������");
	gotoxy(10, 4); printf("������� ���"); gotoxy(68, 4); printf("��");
	gotoxy(10, 5); printf("������������������������������������������������������������");
	for(i=0;i<MAX_USER;i++){
		if(mem_meta_data[i]==WAITING_MEMBER_SLOT){
			gotoxy(10, j+6); printf("��%d�� %s", WaitMem[i].rank, WaitMem[i].ID); gotoxy(68, j+6); printf("��");
			j++;
		}
	}
	gotoxy(10, j+6); printf("������������������������������������������������������������");
	gotoxy(10, j+7); printf("�� "); gotoxy(68, j+7); printf("��");
	gotoxy(10, j+8); printf("������������������������������������������������������������");
	gotoxy(12, j+9); printf("[ESC] : ���� ȭ��");
	gotoxy(27, j+7);
	
	getch();
	
	system("cls");
}

int CreateRoom(int sock){
	int ret;
	system("cls");
	gotoxy(3, 3); printf("����������������������������������������������������������������������������");
	gotoxy(3, 4); printf("���� ����"); gotoxy(77, 4); printf("��");
	gotoxy(3, 5); printf("����������������������������������������������������������������������������");
	gotoxy(3, 6); printf("���� ����	: "); gotoxy(77, 6); printf("��");
	gotoxy(3, 7); printf("����������������������������������������������������������������������������");
	gotoxy(3, 8); printf("[ESC] ���� ȭ������");
	gotoxy(19, 6); show_cursor(1); ret=InputMsg(Room_Name, sizeof(Room_Name)); show_cursor(0);
	
	return ret;
}

void EnterRoomNumber(int sock){
	system("cls");
	show_cursor(1);
	gotoxy(10, 3); printf("������������������������������������������������������������");
	gotoxy(10, 4); printf("���� ��ȣ�� ����"); gotoxy(68, 4); printf("��");
	gotoxy(10, 5); printf("������������������������������������������������������������");
	gotoxy(10, 6); printf("���� ��ȣ	: "); gotoxy(68, 6); printf("��");
	gotoxy(10, 7); printf("������������������������������������������������������������");
	gotoxy(27, 6); InputNum(char_move_room_number, sizeof(char_move_room_number));
	show_cursor(0);
	system("cls");
}

void Show_MyInfo(int sock){
	int Acion;
	int compare_flag=0;
	
	system("cls");
	show_cursor(0);
	gotoxy(10, 3); printf("������������������������������������������������������������");
	gotoxy(10, 4); printf("���� ���� [���� : %d�� %d��, %d��]", mem.win, mem.lose, mem.rank); gotoxy(68, 4); printf("��");
	gotoxy(10, 5); printf("������������������������������������������������������������");
	gotoxy(10, 6); printf("��");
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
	printf("�Ϻ� ������ �������� �ʽ��ϴ�."); 
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
	gotoxy(12, 21); printf("[Enter] : ���� ����");
	gotoxy(12, 22); printf("[ESC] : ���� ȭ��");
	
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
		
		gotoxy(27, 18); InputValue(mem.email_address, sizeof(mem.email_address));

		MsgType=MODIFY_REQUEST;
		send(sock, &MsgType, 1, 0);
		send(sock, &mem.mem_number, 4, 0);
		send(sock, &mem.pass, sizeof(mem.pass), 0);
		send(sock, &mem.email_address, sizeof(mem.email_address), 0);

		gotoxy(12, 23); printf("ȸ�� ������ ���ŵǾ����ϴ�. [Ȯ��]");
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
	gotoxy(10, 2); printf("������������������������������������������������������������");
	gotoxy(10, 3); printf("������"); gotoxy(68, 3); printf("��");
	gotoxy(10, 4); printf("������������������������������������������������������������");
	gotoxy(10, 5); printf("��"); gotoxy(68, 5); printf("��");
	gotoxy(10, 6); printf("��"); gotoxy(68, 6); printf("��");
	gotoxy(10, 7); printf("��"); gotoxy(68, 7); printf("��");
	gotoxy(10, 8); printf("��"); gotoxy(68, 8); printf("��");
	gotoxy(10, 9); printf("��"); gotoxy(68, 9); printf("��");
	gotoxy(10, 10); printf("��"); gotoxy(68, 10); printf("��");
	gotoxy(10, 11); printf("��"); gotoxy(68, 11); printf("��");
	gotoxy(10, 12); printf("��"); gotoxy(68, 12); printf("��");
	gotoxy(10, 13); printf("��"); gotoxy(68, 13); printf("��");
	gotoxy(10, 14); printf("��"); gotoxy(68, 14); printf("��");
	gotoxy(10, 15); printf("��"); gotoxy(68, 15); printf("��");
	gotoxy(10, 16); printf("��"); gotoxy(68, 16); printf("��");
	gotoxy(10, 17); printf("��"); gotoxy(68, 17); printf("��");
	gotoxy(10, 18); printf("��"); gotoxy(68, 18); printf("��");
	gotoxy(10, 19); printf("��"); gotoxy(68, 19); printf("��");
	gotoxy(10, 20); printf("��"); gotoxy(68, 20); printf("��");
	gotoxy(10, 21); printf("��"); gotoxy(68, 21); printf("��");
	gotoxy(10, 22); printf("��"); gotoxy(68, 22); printf("��");
	gotoxy(10, 23); printf("������������������������������������������������������������");
	gotoxy(12, 24); printf("[ESC] : ���� ȭ��");
	
	
	gotoxy(12, 5); printf("1. ������ �� ����� ����� ȭ�鿡 �ǽð����� �ݿ����� ��");
	gotoxy(12, 6); printf("����, ��/�� Ű�� �̿��ϸ� �� ����� ���ŵ˴ϴ�.");
	
	gotoxy(12, 8); printf("2. �뱹 ���� �غ� ���� �������� ���� ���� �߻� ������ ��");
	gotoxy(12, 9); printf("�� �뱹 ���� ���� �� ���� �������� ������ �Ұ��մϴ�.");
	gotoxy(12, 10); printf("�뱹 ���� �� 3�� ������ ������ ������ ������ ��ġ�� ����");
	gotoxy(12, 11); printf("�Ƿ� �� ������ �뱹�� �����Ͻø� �˴ϴ�.");
	
	gotoxy(12, 13); printf("3. ������ IP�� ȣ��Ʈ�� ���ÿ� ���� ���� �� ���߿� ����");
	gotoxy(12, 14); printf("�� ���� ������������ �۵��ϰ� �˴ϴ�.");
	gotoxy(12, 15); printf("���� IP ������ �����Ѵٸ� ������ �ذ�˴ϴٸ� ��Ȱ�� ��");
	gotoxy(12, 16); printf("����Ʈ �׽�Ʈ�� ���� �� �ذ� ����� ������� �ʾҽ��ϴ�.");
	
	gotoxy(12, 18); printf("4. ���� ��� Ȯ���� ���� �±� ������ �¸� �� �±�, �й�");
	gotoxy(12, 19); printf("�� ����Ǵ� ������ ����� ���ϰ� �ֽ��ϴ�.");
	
	gotoxy(12, 21); printf("5. �뱹 ���� �� �������� ���� ����� ���ŵǳ� Ŭ���̾�Ʈ");
	gotoxy(12, 22); printf("�� ȭ�鿡�� ����� ��� �ݿ����� �ʽ��ϴ�.");
}
#include "ConsoleHdr.h"
#include "ConnectHdr.h"
#include "ClntHdr.h"

void WaitingRoom(int sock){
	hThread=(HANDLE)_beginthread(Recv_Thread, 0, sock);
	
	MsgType=ROOM_CHANGE_REQUEST;
	send(sock, &MsgType, 1, 0);
	MsgType=ROOM_MINUS_REQUEST;
	send(sock, &MsgType, 1, 0);//殮濰ж濠葆濠 寞 偃撲 跡煙擊 轎溘ж晦 嬪и 蹂羶.

	InitializeCriticalSection(&WRoomCS);
	while(1){
		other_func=0;
		
		MsgType=RE_REQUEST;
		send(sock, &MsgType, 1, 0);
		send(sock, &MyNumber, 4, 0);//渠措 謙猿 �� 渠晦褒煎 釭諮擊 衛 �蛾� 薑爾 偵褐擊 嬪и 蹂羶
		
		//罹晦憮睡攪 渠晦褒 �飛�
		system("cls");
		Action=0;
		EnterCriticalSection(&WRoomCS);
		printf("忖忙式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖忙\n");
		printf("弛弛Room Number	: %d", room.room_number); gotoxy(75, 2); printf("弛弛\n");
		printf("弛弛Subject	: %s", room.room_name); gotoxy(75, 3); printf("弛弛\n");
		printf("弛弛Host Info.	: %s :: %d晝", room.leader_ID, room.leader_rank); gotoxy(75, 4); printf("弛弛\n");
		printf("戎戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎戌\n");
		printf("８８		謝/辦 酈諦 蝶む檜蝶 酈煎 寞擊 摹鷗м棲棻.		  ⅠⅠ\n");
		LeaveCriticalSection(&WRoomCS);
		
		EnterCriticalSection(&WRoomCS);
		WRoomChatWindow();
		LeaveCriticalSection(&WRoomCS);
		
		while (1) {//F1~12縑 嬴蝶酈 囀萄陛 й渡腎雖 彊嬴憮 鏃ж朝 褻纂.
			Action = getch();
			if(Action!=0)
				break;
		}
		
		if(Action==75 || Action==77){//寞 跡煙 檜翕
			MsgType=ROOM_CHANGE_REQUEST;
			send(sock, &MsgType, 1, 0);
			if(Action==75){
				gotoxy(1, 6); printf("９９９");
				MsgType=ROOM_MINUS_REQUEST;
				send(sock, &MsgType, 1, 0);
			}
			if(Action==77){
				gotoxy(73, 6); printf("ⅡⅡⅡ");
				MsgType=ROOM_PLUS_REQUEST;
				send(sock, &MsgType, 1, 0);
			}
		}else if(Action==32 || Action==63){//寞 霤陛. space / F6
			other_func=1;
			if(Action==63){
				EnterRoomNumber(sock);
				room.room_number=atoi(char_move_room_number);
			}
			
			MsgType=ENTER_ROOM_REQUEST;//褒衛除 �飛橉� 嬴棲嘎煎 п渡 寞 薑爾陛 嶸�褲� 雖 だ學й в蹂陛 氈擠. 詭顫 等檜攪蒂 輿堅 嫡嬴 �挫恉牉� 腆蛭 談棻.
			send(sock, &MsgType, 1, 0);
			send(sock, &Wait_index, 4, 0);//餌辨濠蒂 啪歜 醞戲煎 撲薑ж晦 嬪л.
			send(sock, &room.room_number, 4, 0);//п渡 寞擊 啪歜 醞戲煎 撲薑ж晦 嬪л.
			
			while(MsgType!=ENTER_ROOM_ACK && MsgType!=ENTER_ROOM_DENY){}
			Sleep(10);
			
			if(MsgType==ENTER_ROOM_ACK){
				system("cls");
				/////////////////////////////////
				/////////啪歜 詭檣 ��轎./////////
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
				gotoxy(19, 3); printf("[摹鷗脹 寞擎 渦 檜鼻 嶸�褲狫� 彊擎 寞殮棲棻.]\t\t");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				getch();
			}
			
		}else if(Action==13){//瓣た
			EnterCriticalSection(&WRoomCS);
			Send_Chat(sock);
			LeaveCriticalSection(&WRoomCS);
			
		}else if(Action==59){//F1 : 紫遺蜓
			system("cls");
			other_func=1;
			Help();
			getch();
			
		}else if(Action==60){//F2 : 頂 薑爾
			system("cls");
			other_func=1;
			MsgType=MY_INFO_REQUEST;
			send(sock, &MsgType, 1, 0);
			send(sock, &ID, sizeof(ID), 0);
			printf("%s", ID);
			Show_MyInfo(sock);
			
		}else if(Action==61){//F3 : 寞 偃撲
			other_func=1;
			if(!CreateRoom(sock)){//ESC蒂 援腦賊 input л熱陛 1擊 奩�納牊Й� 紫醞縑 鏃模й 熱 氈紫煙 л.
				GenPortNumber();//んお 廓�� 儅撩.
				MsgType=CREATE_ROOM_REQUEST;
				Data_size=sizeof(Room_Name);
				send(sock, &MsgType, 1, 0);
				send(sock, &Data_size, 4, 0);
				send(sock, &Room_Name, Data_size, 0);
				send(sock, &Wait_index, 4, 0);
				send(sock, &HostPort, 6, 0);
				Sleep(10);
				/////////////////////////////////
				/////////啪歜 詭檣 ��轎./////////
				/////////////////////////////////
				GameMain(1, ID, MyRank, NULL, NULL, 0, sock, HostPort, room.room_number);
				Sleep(10);
				MsgType=BREAK_ROOM_REQUEST;
				send(sock, &MsgType, 1, 0);
				send(sock, &Wait_index, 4, 0);
				send(sock, &Room_index, 4, 0);
			}
			
		}else if(Action==62){//F4 : 渠晦濠 葬蝶お
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
	int i;//瞪羹 �蛾� 熱 蘋遴お
	int j=0;//蕾樓 �蛾� 熱 蘋遴お. �飛� ル衛蒂 嬪п 餌辨.
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
	
	gotoxy(10, 3); printf("忙式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖");
	gotoxy(10, 4); printf("弛渠晦濠 貲欽"); gotoxy(68, 4); printf("弛");
	gotoxy(10, 5); printf("戍式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣");
	for(i=0;i<MAX_USER;i++){
		if(mem_meta_data[i]==WAITING_MEMBER_SLOT){
			gotoxy(10, j+6); printf("弛%d晝 %s", WaitMem[i].rank, WaitMem[i].ID); gotoxy(68, j+6); printf("弛");
			j++;
		}
	}
	gotoxy(10, j+6); printf("戍式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣");
	gotoxy(10, j+7); printf("弛 "); gotoxy(68, j+7); printf("弛");
	gotoxy(10, j+8); printf("戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎");
	gotoxy(12, j+9); printf("[ESC] : 檜瞪 �飛�");
	gotoxy(27, j+7);
	
	getch();
	
	system("cls");
}

int CreateRoom(int sock){
	int ret;
	system("cls");
	gotoxy(3, 3); printf("忙式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖");
	gotoxy(3, 4); printf("弛寞 儅撩"); gotoxy(77, 4); printf("弛");
	gotoxy(3, 5); printf("戍式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣");
	gotoxy(3, 6); printf("弛寞 薯跡	: "); gotoxy(77, 6); printf("弛");
	gotoxy(3, 7); printf("戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎");
	gotoxy(3, 8); printf("[ESC] 檜瞪 �飛橉虞�");
	gotoxy(19, 6); show_cursor(1); ret=InputMsg(Room_Name, sizeof(Room_Name)); show_cursor(0);
	
	return ret;
}

void EnterRoomNumber(int sock){
	system("cls");
	show_cursor(1);
	gotoxy(10, 3); printf("忙式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖");
	gotoxy(10, 4); printf("弛寞 廓�ㄦ� 殮濰"); gotoxy(68, 4); printf("弛");
	gotoxy(10, 5); printf("戍式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣");
	gotoxy(10, 6); printf("弛寞 廓��	: "); gotoxy(68, 6); printf("弛");
	gotoxy(10, 7); printf("戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎");
	gotoxy(27, 6); InputNum(char_move_room_number, sizeof(char_move_room_number));
	show_cursor(0);
	system("cls");
}

void Show_MyInfo(int sock){
	int Acion;
	int compare_flag=0;
	
	system("cls");
	show_cursor(0);
	gotoxy(10, 3); printf("忙式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖");
	gotoxy(10, 4); printf("弛頂 薑爾 [瞪瞳 : %d蝓 %dぬ, %d晝]", mem.win, mem.lose, mem.rank); gotoxy(68, 4); printf("弛");
	gotoxy(10, 5); printf("戍式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣");
	gotoxy(10, 6); printf("弛");
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
	printf("橾睡 薑爾朝 奢偃腎雖 彊蝗棲棻."); 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	
	gotoxy(68, 6); printf("弛");
	gotoxy(10, 7); printf("弛"); gotoxy(68, 7); printf("弛");
	gotoxy(10, 8); printf("弛ID		: "); gotoxy(68, 8); printf("弛");
	gotoxy(10, 9); printf("弛"); gotoxy(68, 9); printf("弛");
	gotoxy(10, 10); printf("弛Password	: "); gotoxy(68, 10); printf("弛");
	gotoxy(10, 11); printf("弛"); gotoxy(68, 11); printf("弛");
	gotoxy(10, 12); printf("弛Password �挫�: "); gotoxy(68, 12); printf("弛");
	gotoxy(10, 13); printf("弛"); gotoxy(68, 13); printf("弛");
	gotoxy(10, 14); printf("弛檜葷		: "); gotoxy(68, 14); printf("弛");
	gotoxy(10, 15); printf("弛"); gotoxy(68, 15); printf("弛");
	gotoxy(10, 16); printf("弛輿團蛔煙廓��	: "); gotoxy(34, 16); printf("-"); gotoxy(68, 16); printf("弛");
	gotoxy(10, 17); printf("弛"); gotoxy(68, 17); printf("弛");
	gotoxy(10, 18); printf("弛E-mail 輿模	: "); gotoxy(68, 18); printf("弛");
	gotoxy(10, 19); printf("弛"); gotoxy(68, 19); printf("弛");
	gotoxy(10, 20); printf("戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎");
	gotoxy(12, 21); printf("[Enter] : 薑爾 熱薑");
	gotoxy(12, 22); printf("[ESC] : 檜瞪 �飛�");
	
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
		
		/* ぬ蝶錶萄 橾纂 匐餌 */
		while(!compare_flag){
			gotoxy(27, 10); InputPass(mem.pass, sizeof(mem.pass));
			gotoxy(27, 12); InputPass(Config_Pass, sizeof(Config_Pass));
			if(strcmp(mem.pass, Config_Pass)){
				compare_flag=0;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				show_cursor(0);
				gotoxy(27, 10); printf("Password陛 橾纂ж雖 彊蝗棲棻. [�挫垚");
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

		gotoxy(12, 23); printf("�蛾� 薑爾陛 偵褐腎歷蝗棲棻. [�挫垚");
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
	gotoxy(10, 2); printf("忙式式式式式式式式式式式式式式式式式式式式式式式式式式式式忖");
	gotoxy(10, 3); printf("弛紫遺蜓"); gotoxy(68, 3); printf("弛");
	gotoxy(10, 4); printf("戍式式式式式式式式式式式式式式式式式式式式式式式式式式式式扣");
	gotoxy(10, 5); printf("弛"); gotoxy(68, 5); printf("弛");
	gotoxy(10, 6); printf("弛"); gotoxy(68, 6); printf("弛");
	gotoxy(10, 7); printf("弛"); gotoxy(68, 7); printf("弛");
	gotoxy(10, 8); printf("弛"); gotoxy(68, 8); printf("弛");
	gotoxy(10, 9); printf("弛"); gotoxy(68, 9); printf("弛");
	gotoxy(10, 10); printf("弛"); gotoxy(68, 10); printf("弛");
	gotoxy(10, 11); printf("弛"); gotoxy(68, 11); printf("弛");
	gotoxy(10, 12); printf("弛"); gotoxy(68, 12); printf("弛");
	gotoxy(10, 13); printf("弛"); gotoxy(68, 13); printf("弛");
	gotoxy(10, 14); printf("弛"); gotoxy(68, 14); printf("弛");
	gotoxy(10, 15); printf("弛"); gotoxy(68, 15); printf("弛");
	gotoxy(10, 16); printf("弛"); gotoxy(68, 16); printf("弛");
	gotoxy(10, 17); printf("弛"); gotoxy(68, 17); printf("弛");
	gotoxy(10, 18); printf("弛"); gotoxy(68, 18); printf("弛");
	gotoxy(10, 19); printf("弛"); gotoxy(68, 19); printf("弛");
	gotoxy(10, 20); printf("弛"); gotoxy(68, 20); printf("弛");
	gotoxy(10, 21); printf("弛"); gotoxy(68, 21); printf("弛");
	gotoxy(10, 22); printf("弛"); gotoxy(68, 22); printf("弛");
	gotoxy(10, 23); printf("戌式式式式式式式式式式式式式式式式式式式式式式式式式式式式戎");
	gotoxy(12, 24); printf("[ESC] : 檜瞪 �飛�");
	
	
	gotoxy(12, 5); printf("1. 偃撲脹 寞 跡煙擎 餌辨濠 �飛橦� 褒衛除戲煎 奩艙腎雖 彊");
	gotoxy(12, 6); printf("戲貊, 謝/辦 酈蒂 檜辨ж賊 寞 跡煙檜 偵褐腌棲棻.");
	
	gotoxy(12, 8); printf("2. 渠措 衛濛 遽綠 醞曖 黴濰戲煎 檣и 僥薯 嫦儅 寞雖蒂 嬪");
	gotoxy(12, 9); printf("п 渠措 衛濛 霜瞪 塽 謙猿 霜瞪縑朝 黴濰檜 碳陛м棲棻.");
	gotoxy(12, 10); printf("渠措 衛濛 �� 3熱 檜瞪曖 黴濰擎 瞪瞳縑 艙щ擊 郭纂雖 彊戲");
	gotoxy(12, 11); printf("嘎煎 檜 衛薄縑 渠措擊 謙猿ж衛賊 腌棲棻.");
	
	gotoxy(12, 13); printf("3. 翕橾и IP曖 �ˊ瘋扇� 翕衛縑 寞擊 偃撲 衛 釭醞縑 偃撲");
	gotoxy(12, 14); printf("脹 寞擎 綠薑鼻瞳戲煎 濛翕ж啪 腌棲棻.");
	gotoxy(12, 15); printf("翕橾 IP 蕾樓擊 離欽и棻賊 僥薯陛 п唸腌棲棻虜 錳�勒� Щ");
	gotoxy(12, 16); printf("煎薛お 纔蝶お蒂 嬪п 斜 п唸 寞徹擎 餌辨ж雖 彊懊蝗棲棻.");
	
	gotoxy(12, 18); printf("4. 緒艇 唸婁 �挫怹� 嬪п 蝓晝 薯紫朝 蝓葬 衛 蝓晝, ぬ寡");
	gotoxy(12, 19); printf("衛 鬼蛔腎朝 除欽и 寞衝擊 鏃ж堅 氈蝗棲棻.");
	
	gotoxy(12, 21); printf("5. 渠措 謙猿 衛 憮幗縑朝 瞪瞳 晦煙檜 偵褐腎釭 贗塭檜樹お");
	gotoxy(12, 22); printf("曖 �飛橦●� 晦煙檜 闊衛 奩艙腎雖 彊蝗棲棻.");
}
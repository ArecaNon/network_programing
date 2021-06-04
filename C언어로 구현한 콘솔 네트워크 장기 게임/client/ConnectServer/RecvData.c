#include "ConsoleHdr.h"
#include "ConnectHdr.h"
#include "ClntHdr.h"

void Send_Chat(int sock){
	InitializeCriticalSection(&WRoomChatSendCS);
	
	EnterCriticalSection(&WRoomChatSendCS);
	show_cursor(1);
	gotoxy(33, 21); printf("\r│입력 : \t\t\t");
	gotoxy(10, 21); InputMsg(EchoString, sizeof(EchoString));
	LeaveCriticalSection(&WRoomChatSendCS);
	
	MsgType=CHAT_REQUEST;
	IDSize=sizeof(ID);
	Msg_size=sizeof(EchoString);
	
	send(sock, &MsgType, 1, 0);
	send(sock, &IDSize, 4, 0);
	send(sock, &ID, IDSize, 0);
	send(sock, &Msg_size, 4, 0);
	send(sock, &EchoString, Msg_size, 0);
	show_cursor(0);
	DeleteCriticalSection(&WRoomChatSendCS);
}

void Recv_Thread(void* sock){
	int ret_thread=65535;
	
	memset(EchoBuf, 0, sizeof(EchoBuf));//버퍼 초기화.
	
	InitializeCriticalSection(&WRoomChatRecvCS);
	
	while(ret_thread!=INVALID_SOCKET || ret_thread!=SOCKET_ERROR){
		
		ret_thread=recv(sock, &MsgType, 1, 0);
		
		/* 서버의 ACK에 대한 반응*/
		if(MsgType==RE_ACK){//정보 갱신
			Recv_Re(sock);
		}
		if(MsgType==CHAT_ACK){//채팅
			Recv_Chat(sock);
		}
		if(MsgType==ROOM_CHANGE_ACK){//방 이동
			Recv_Room_Str(sock);
		}
		if(MsgType==ENTER_ROOM_ACK){//방 진입
			recv(sock, &room, sizeof(room), 0);
		}
		if(MsgType==CREATE_ROOM_ACK){//방 개설
			Recv_RoomNumber(sock);
		}
		if(MsgType==MY_INFO_ACK){//내 정보
			recv(sock, &mem, sizeof(mem), 0);
		}
		if(MsgType==WAIT_LIST_ACK){//대기자 목록
			RecvWaitingList(sock);
		}
	}
	DeleteCriticalSection(&WRoomChatRecvCS);
	//작업 완료 소켓 무효화.
	WaitForSingleObject(hMutex,100L);
	ReleaseMutex(hMutex);
	
	return;
}

void Recv_Re(int sock){
	recv(sock, &mem, sizeof(mem), 0);
	MyRank=mem.rank;
}

void Recv_Chat(int sock){
	int i;
	
	CONSOLE_SCREEN_BUFFER_INFO buffInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&buffInfo);//recv가 오기 전까지의 콘솔 커서를 저장하기 위한 구조체 초기화.
	
	memset(Full_Msg, 0, sizeof(Full_Msg));//버퍼 초기화.
	recv(sock, &Full_Msg, sizeof(Full_Msg), 0);
	/* 채팅 메시지 정렬 */
	EnterCriticalSection(&WRoomChatRecvCS);
	for(i=0;i<BASIC_DATA_SIZE+CHAT_BUF_SIZE+3;i++){
		EchoBuf[11][i]=EchoBuf[10][i];
		EchoBuf[10][i]=EchoBuf[9][i];
		EchoBuf[9][i]=EchoBuf[8][i];
		EchoBuf[8][i]=EchoBuf[7][i];
		EchoBuf[7][i]=EchoBuf[6][i];
		EchoBuf[6][i]=EchoBuf[5][i];
		EchoBuf[5][i]=EchoBuf[4][i];
		EchoBuf[4][i]=EchoBuf[3][i];
		EchoBuf[3][i]=EchoBuf[2][i];
		EchoBuf[2][i]=EchoBuf[1][i];
		EchoBuf[1][i]=EchoBuf[0][i];
	}
	LeaveCriticalSection(&WRoomChatRecvCS);
	
	EnterCriticalSection(&WRoomChatRecvCS);
	for(i=0;i<BASIC_DATA_SIZE+CHAT_BUF_SIZE+3;i++){
		EchoBuf[0][i]=Full_Msg[i];
	}
	LeaveCriticalSection(&WRoomChatRecvCS);
	
	cur_x=buffInfo.dwCursorPosition.X;
	cur_y=buffInfo.dwCursorPosition.Y;//recv가 오기 전까지의 커서 좌표로 복귀하기 위해 현재 좌표 저장.
	
	if(!other_func){
		EnterCriticalSection(&WRoomChatRecvCS);
		WRoomChatClear();
		WRoomChatWindow();
		LeaveCriticalSection(&WRoomChatRecvCS);
	}
	gotoxy(cur_x+1, cur_y+1);
}

void Recv_Room_Str(int sock){
	memset(&room, 0, sizeof(room));
	recv(sock, &room, sizeof(room), 0);
}

void Recv_RoomNumber(int sock){
	recv(sock, &Room_index, 4, 0);
	recv(sock, &room, sizeof(room), 0);
}

void RecvWaitingList(int sock){
	memset(mem_meta_data, 0, sizeof(mem_meta_data));
	memset(WaitMem, 0, sizeof(WaitMem));
	
	recv(sock, &mem_meta_data, sizeof(mem_meta_data), 0);
	
	recv(sock, &WaitMem, sizeof(WaitMem), 0);
}

void WRoomChatWindow(){
	gotoxy(1, 7);
	printf("┌─────────────────────────────────────┐\n");
	printf("│%s", EchoBuf[11]); gotoxy(77, 8); printf("│\n");
	printf("│%s", EchoBuf[10]); gotoxy(77, 9); printf("│\n");
	printf("│%s", EchoBuf[9]); gotoxy(77, 10); printf("│\n");
	printf("│%s", EchoBuf[8]); gotoxy(77, 11); printf("│\n");
	printf("│%s", EchoBuf[7]); gotoxy(77, 12); printf("│\n");
	printf("│%s", EchoBuf[6]); gotoxy(77, 13); printf("│\n");
	printf("│%s", EchoBuf[5]); gotoxy(77, 14); printf("│\n");
	printf("│%s", EchoBuf[4]); gotoxy(77, 15); printf("│\n");
	printf("│%s", EchoBuf[3]); gotoxy(77, 16); printf("│\n");
	printf("│%s", EchoBuf[2]); gotoxy(77, 17); printf("│\n");
	printf("│%s", EchoBuf[1]); gotoxy(77, 18); printf("│\n");
	printf("│%s", EchoBuf[0]); gotoxy(77, 19); printf("│\n");
	printf("├─────────────────────────────────────┤\n");
	printf("│입력 : "); gotoxy(77, 21); printf("│\n");
	printf("└─────────────────────────────────────┘\n");
	printf("[F1] : 도움말		[F2] : 내 정보		[F3] : 방 생성\n");
	printf("[F4] : 대기자 명단	[F5] : 방 번호로 입장");
}

void WRoomChatClear(){
	gotoxy(1, 7);
	printf("┌─────────────────────────────────────┐\n");
	printf("│\t\t\t\t\t\t\t\t"); gotoxy(77, 8); printf("│\n");
	printf("│\t\t\t\t\t\t\t\t"); gotoxy(77, 9); printf("│\n");
	printf("│\t\t\t\t\t\t\t\t"); gotoxy(77, 10); printf("│\n");
	printf("│\t\t\t\t\t\t\t\t"); gotoxy(77, 11); printf("│\n");
	printf("│\t\t\t\t\t\t\t\t"); gotoxy(77, 12); printf("│\n");
	printf("│\t\t\t\t\t\t\t\t"); gotoxy(77, 13); printf("│\n");
	printf("│\t\t\t\t\t\t\t\t"); gotoxy(77, 14); printf("│\n");
	printf("│\t\t\t\t\t\t\t\t"); gotoxy(77, 15); printf("│\n");
	printf("│\t\t\t\t\t\t\t\t"); gotoxy(77, 16); printf("│\n");
	printf("│\t\t\t\t\t\t\t\t"); gotoxy(77, 17); printf("│\n");
	printf("│\t\t\t\t\t\t\t\t"); gotoxy(77, 18); printf("│\n");
	printf("│\t\t\t\t\t\t\t\t"); gotoxy(77, 19); printf("│\n");
}

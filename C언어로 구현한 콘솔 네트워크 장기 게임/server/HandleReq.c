#include "ConnectHdr.h"
#include "ServerHdr.h"

void ChangeRoom(int sock){
	int i;
	int deadlock=0;
	MsgType=ROOM_CHANGE_ACK;
	send(sock, &MsgType, 1, 0);
	
	recv(sock, &MsgType, 1, 0);

	printf("\n%d ", Room_counter);
	printf("Room Slot : ");
	for(i=0;i<MAX_USER;i++)
		printf("%d ", room_meta_data[i], MakedRoom[i].room_name, MakedRoom[i].IP);
	
	if(Wait_Room_counter>0){//개설된 방이 하나 이상 존재할 경우에는 방 정보를 검색해서 전달.
		if(MsgType==ROOM_MINUS_REQUEST){
			Room_index--;
			if(Room_index==-1)
				Room_index=MAX_USER-1;
			while(1){
				deadlock++;//무한대기 방지
				if(room_meta_data[Room_index]==WAITING_ROOM_SLOT || deadlock==MAX_USER)
					break;
				Room_index--;
				if(Room_index==-1)
					Room_index=MAX_USER-1;
			}
		}
		if(MsgType==ROOM_PLUS_REQUEST){
			Room_index++;
			if(Room_index==MAX_USER)
				Room_index=0;
			while(1){
				deadlock++;//무한대기 방지
				if(room_meta_data[Room_index]==WAITING_ROOM_SLOT || deadlock==MAX_USER)
					break;
				Room_index++;
				if(Room_index==MAX_USER)
					Room_index=0;
			}
		}
		send(sock, &MakedRoom[Room_index], sizeof(MakedRoom[Room_index]), 0);
	}else{
		//memset(&room, 0, sizeof(room));
		room.IP[0]='\0';
		room.leader_ID[0]='\0';
		room.leader_rank=-1;
		room.leader_sock=-1;
		room.room_name[0]='\0';
		room.room_number=-1;
		strcpy(room.room_name, "현재 입장 가능한 방이 개설되어 있지 않습니다.");
		room.leader_rank=0;
		room.room_number=0;
		send(sock, &room, sizeof(room), 0);
	}
	
}

void EnterRoom(int sock){
	recv(sock, &Wait_index, 4, 0);//사용자를 게임 중으로 설정하기 위함.
	recv(sock, &room.room_number, 4, 0);//해당 방을 게임 중으로 설정하기 위함.
	
	if(room_meta_data[room.room_number]==WAITING_ROOM_SLOT){
		MsgType=ENTER_ROOM_ACK;
		send(sock, &MsgType, 1, 0);
		send(sock, &MakedRoom[room.room_number], sizeof(MakedRoom[room.room_number]), 0);
		Wait_Room_counter--;
		mem_meta_data[Wait_index]=GAMING_MEMBER_SLOT;//해당 회원을 게임 중으로 설정.
		room_meta_data[room.room_number]=GAMING_ROOM_SLOT;//방의 메타 데이터를 게임 중으로 설정
	}else{
		MsgType=ENTER_ROOM_DENY;
		send(sock, &MsgType, 1, 0);
	}
}

void ExitRoom(int sock){
	Wait_Room_counter++;
	recv(sock, &Wait_index, 4, 0);
	recv(sock, &room.room_number, 4, 0);
	if(room_meta_data[room.room_number]==GAMING_ROOM_SLOT)
		room_meta_data[room.room_number]=WAITING_ROOM_SLOT;//방장이 먼저 나와서 방을 break한 경우에는 메타 데이터를 수정하지 않음.
	
	mem_meta_data[Wait_index]=WAITING_MEMBER_SLOT;
}

void CreateRoom(int sock){
	recv(sock, &Data_size, 4, 0);
	recv(sock, &Room_Name, Data_size, 0);
	recv(sock, &Wait_index, 4, 0);
	recv(sock, &HostPort, 6, 0);
	printf("%d번 회원이 방 제목 %s으로 방 만들기 신청", Wait_index, Room_Name);
	
	Room_counter++;
	Wait_Room_counter++;//방 개설 후 대기 중인 방의 숫자를 ++;
	
	if(room_meta_data[Room_index]==EMPTY_ROOM_SLOT){//메타 데이터가 해당 슬롯이 비었음을 알리면 슬롯에 방 정보 적재.
		room_meta_data[Room_index]=WAITING_ROOM_SLOT;//메타 데이터가 해당 인덱스의 방 자리가 빈 슬롯이 아님을 표시.
		
		MakedRoom[Room_index].room_number=Room_index;
		strcpy(MakedRoom[Room_index].IP, WaitMem[Wait_index].IP);
		strcpy(MakedRoom[Room_index].leader_ID, WaitMem[Wait_index].ID);
		strcpy(MakedRoom[Room_index].room_name, Room_Name);
		MakedRoom[Room_index].leader_rank=WaitMem[Wait_index].rank;
		MakedRoom[Room_index].leader_sock=WaitMem[Wait_index].mysock;
		strcpy(MakedRoom[Room_index].host_port, HostPort);
		
		mem_meta_data[Wait_index]=GAMING_MEMBER_SLOT;//해당 회원은 게임 중인 것으로 설정.
	}
	else if(room_meta_data[Room_index]==WAITING_ROOM_SLOT || room_meta_data[Room_index]==GAMING_ROOM_SLOT){//해당 슬롯이 사용 중일 경우 빈 슬롯을 검색하여 빈 슬롯에 방 정보 적재.
		while(room_meta_data[Room_index]==WAITING_ROOM_SLOT || room_meta_data[Room_index]==GAMING_ROOM_SLOT){
			Room_index++;
			if(Room_index==MAX_USER)
				Room_index=0;
		}
		room_meta_data[Room_index]=WAITING_ROOM_SLOT;//메타 데이터가 해당 인덱스의 방 자리가 빈 슬롯이 아님을 표시.
		
		MakedRoom[Room_index].room_number=Room_index;
		strcpy(MakedRoom[Room_index].IP, WaitMem[Wait_index].IP);
		strcpy(MakedRoom[Room_index].leader_ID, WaitMem[Wait_index].ID);
		strcpy(MakedRoom[Room_index].room_name, Room_Name);
		MakedRoom[Room_index].leader_rank=WaitMem[Wait_index].rank;
		MakedRoom[Room_index].leader_sock=WaitMem[Wait_index].mysock;
		
		mem_meta_data[Wait_index]=GAMING_MEMBER_SLOT;//해당 회원은 게임 중인 것으로 설정.
	}
	
	MsgType=CREATE_ROOM_ACK;
	send(sock, &MsgType, 1, 0);
	send(sock, &Room_index, 4, 0);
	send(sock, &MakedRoom[Room_index], sizeof(MakedRoom[Room_index]), 0);
}

void BreakRoom(int sock){
	recv(sock, &Wait_index, 4, 0);
	recv(sock, &Room_index, 4, 0);
	Room_counter--;
	Wait_Room_counter--;

	MakedRoom[Room_index].IP[0]='\0';
	MakedRoom[Room_index].leader_ID[0]='\0';
	MakedRoom[Room_index].leader_rank=-1;
	MakedRoom[Room_index].leader_sock=-1;
	MakedRoom[Room_index].room_name[0]='\0';
	MakedRoom[Room_index].room_number=-1;

	mem_meta_data[Wait_index]=WAITING_MEMBER_SLOT;//해당 회원은 게임 중이 아닌 것으로 설정.
	room_meta_data[Room_index]=EMPTY_ROOM_SLOT;//방의 슬롯을 비어있는 것으로 설정.
}

void SendWaitList(int sock){
	int i=0;
	printf("\n%d Member Slot : ", Wait_counter);
	for(i=0;i<MAX_USER;i++)
		printf("%d ", mem_meta_data[i]);

	MsgType=WAIT_LIST_ACK;

	send(sock, &MsgType, 1, 0);
	send(sock, mem_meta_data, sizeof(mem_meta_data), 0);
	send(sock, WaitMem, sizeof(WaitMem), 0);
}

void SendInfo(int sock){
	int i;
	LoadMemberList();
	for(i=0;i<TotalMem_Counter;i++){
		if(!strcmp(ID, TotalMem[i].id))
			break;
	}
	MsgType=MY_INFO_ACK;
	send(sock, &MsgType, 1, 0);
	send(sock, &TotalMem[i], sizeof(TotalMem[i]), 0);
}

void ModInfo(int sock){
	int i;
	int mem_number;
	char pass[BASIC_DATA_SIZE+1];
	char email_address[EMAIL_ADD_SIZE+1];
	CRITICAL_SECTION FileCS2;
	FILE* out;

	LoadMemberList();
	recv(sock, &mem_number, 4, 0);
	recv(sock, &pass, sizeof(pass), 0);
	recv(sock, &email_address, sizeof(email_address), 0);

	strcpy(TotalMem[mem_number].pass, pass);
	strcpy(TotalMem[mem_number].email_address, email_address);

		InitializeCriticalSection(&FileCS2);
	EnterCriticalSection(&FileCS2);

	out=fopen("member.dat", "w");

	for(i=0;i<TotalMem_Counter;i++){
		fprintf(out, "%d %s %s %s %s %s %s %d %d %d", TotalMem[i].mem_number, TotalMem[i].id, TotalMem[i].pass, TotalMem[i].name,
			TotalMem[i].civil_number1, TotalMem[i].civil_number2, TotalMem[i].email_address,
			TotalMem[i].rank, TotalMem[i].win, TotalMem[i].lose);
		fprintf(out, "\n");
	}
	
	free(TotalMem);
	fclose(out);

	LeaveCriticalSection(&FileCS2);
	DeleteCriticalSection(&FileCS2);

}

void Win(int sock){
	int i;
	int User1_Num=-1;
	int User2_Num=-1;
	int User1_WaitNum=-1;
	int	User2_WaitNum=-1;

	FILE *out;

	CRITICAL_SECTION FileCS2;

	recv(sock, &User1, sizeof(User1), 0);
	recv(sock, &User2, sizeof(User2), 0);
	recv(sock, &win_flag, 4, 0);

	LoadMemberList();

	for(i=0;i<TotalMem_Counter;i++){
		if(strcmp(User1, TotalMem[i].id)==0){
			User1_Num=i;
		}
		if(strcmp(User2, TotalMem[i].id)==0){
			User2_Num=i;
		}
	}

	printf("[%d] %s vs [%d] %s = %d", TotalMem[User1_Num].rank, TotalMem[User1_Num].id, TotalMem[User2_Num].rank, TotalMem[User2_Num].id, win_flag);

	if(win_flag==BLUE_WIN){
		TotalMem[User1_Num].win++;
		TotalMem[User2_Num].lose++;

		if(TotalMem[User1_Num].rank>1)
			TotalMem[User1_Num].rank--;

		if(TotalMem[User2_Num].rank<16)
			TotalMem[User2_Num].rank++;	//간략한 급수 변동 구현
	}

	if(win_flag==RED_WIN){
		TotalMem[User2_Num].win++;
		TotalMem[User1_Num].lose++;

		if(TotalMem[User2_Num].rank>1)
			TotalMem[User2_Num].rank--;

		if(TotalMem[User1_Num].rank<16)
			TotalMem[User1_Num].rank++;	//간략한 급수 변동 구현
	}

	/* 로그인 회원 정보 역시 갱신 */
	for(i=0;i<MAX_USER;i++){
		if(strcmp(User1, WaitMem[i].ID)==0){
			User1_WaitNum=i;
		}
		if(strcmp(User2, WaitMem[i].ID)==0){
			User2_WaitNum=i;
		}
	}
	if(User1_WaitNum!=-1)
		WaitMem[User1_WaitNum].rank=TotalMem[User1_Num].rank;
	if(User2_WaitNum!=-1)
		WaitMem[User2_WaitNum].rank=TotalMem[User2_Num].rank;

	/* 변경 사항 파일에 적용 */
	InitializeCriticalSection(&FileCS2);
	EnterCriticalSection(&FileCS2);

	out=fopen("member.dat", "w");

	for(i=0;i<TotalMem_Counter;i++){
		fprintf(out, "%d %s %s %s %s %s %s %d %d %d", TotalMem[i].mem_number, TotalMem[i].id, TotalMem[i].pass, TotalMem[i].name,
			TotalMem[i].civil_number1, TotalMem[i].civil_number2, TotalMem[i].email_address,
			TotalMem[i].rank, TotalMem[i].win, TotalMem[i].lose);
		fprintf(out, "\n");
	}
	
	free(TotalMem);
	fclose(out);

	LeaveCriticalSection(&FileCS2);
	DeleteCriticalSection(&FileCS2);
}
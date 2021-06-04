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
	
	if(Wait_Room_counter>0){//������ ���� �ϳ� �̻� ������ ��쿡�� �� ������ �˻��ؼ� ����.
		if(MsgType==ROOM_MINUS_REQUEST){
			Room_index--;
			if(Room_index==-1)
				Room_index=MAX_USER-1;
			while(1){
				deadlock++;//���Ѵ�� ����
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
				deadlock++;//���Ѵ�� ����
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
		strcpy(room.room_name, "���� ���� ������ ���� �����Ǿ� ���� �ʽ��ϴ�.");
		room.leader_rank=0;
		room.room_number=0;
		send(sock, &room, sizeof(room), 0);
	}
	
}

void EnterRoom(int sock){
	recv(sock, &Wait_index, 4, 0);//����ڸ� ���� ������ �����ϱ� ����.
	recv(sock, &room.room_number, 4, 0);//�ش� ���� ���� ������ �����ϱ� ����.
	
	if(room_meta_data[room.room_number]==WAITING_ROOM_SLOT){
		MsgType=ENTER_ROOM_ACK;
		send(sock, &MsgType, 1, 0);
		send(sock, &MakedRoom[room.room_number], sizeof(MakedRoom[room.room_number]), 0);
		Wait_Room_counter--;
		mem_meta_data[Wait_index]=GAMING_MEMBER_SLOT;//�ش� ȸ���� ���� ������ ����.
		room_meta_data[room.room_number]=GAMING_ROOM_SLOT;//���� ��Ÿ �����͸� ���� ������ ����
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
		room_meta_data[room.room_number]=WAITING_ROOM_SLOT;//������ ���� ���ͼ� ���� break�� ��쿡�� ��Ÿ �����͸� �������� ����.
	
	mem_meta_data[Wait_index]=WAITING_MEMBER_SLOT;
}

void CreateRoom(int sock){
	recv(sock, &Data_size, 4, 0);
	recv(sock, &Room_Name, Data_size, 0);
	recv(sock, &Wait_index, 4, 0);
	recv(sock, &HostPort, 6, 0);
	printf("%d�� ȸ���� �� ���� %s���� �� ����� ��û", Wait_index, Room_Name);
	
	Room_counter++;
	Wait_Room_counter++;//�� ���� �� ��� ���� ���� ���ڸ� ++;
	
	if(room_meta_data[Room_index]==EMPTY_ROOM_SLOT){//��Ÿ �����Ͱ� �ش� ������ ������� �˸��� ���Կ� �� ���� ����.
		room_meta_data[Room_index]=WAITING_ROOM_SLOT;//��Ÿ �����Ͱ� �ش� �ε����� �� �ڸ��� �� ������ �ƴ��� ǥ��.
		
		MakedRoom[Room_index].room_number=Room_index;
		strcpy(MakedRoom[Room_index].IP, WaitMem[Wait_index].IP);
		strcpy(MakedRoom[Room_index].leader_ID, WaitMem[Wait_index].ID);
		strcpy(MakedRoom[Room_index].room_name, Room_Name);
		MakedRoom[Room_index].leader_rank=WaitMem[Wait_index].rank;
		MakedRoom[Room_index].leader_sock=WaitMem[Wait_index].mysock;
		strcpy(MakedRoom[Room_index].host_port, HostPort);
		
		mem_meta_data[Wait_index]=GAMING_MEMBER_SLOT;//�ش� ȸ���� ���� ���� ������ ����.
	}
	else if(room_meta_data[Room_index]==WAITING_ROOM_SLOT || room_meta_data[Room_index]==GAMING_ROOM_SLOT){//�ش� ������ ��� ���� ��� �� ������ �˻��Ͽ� �� ���Կ� �� ���� ����.
		while(room_meta_data[Room_index]==WAITING_ROOM_SLOT || room_meta_data[Room_index]==GAMING_ROOM_SLOT){
			Room_index++;
			if(Room_index==MAX_USER)
				Room_index=0;
		}
		room_meta_data[Room_index]=WAITING_ROOM_SLOT;//��Ÿ �����Ͱ� �ش� �ε����� �� �ڸ��� �� ������ �ƴ��� ǥ��.
		
		MakedRoom[Room_index].room_number=Room_index;
		strcpy(MakedRoom[Room_index].IP, WaitMem[Wait_index].IP);
		strcpy(MakedRoom[Room_index].leader_ID, WaitMem[Wait_index].ID);
		strcpy(MakedRoom[Room_index].room_name, Room_Name);
		MakedRoom[Room_index].leader_rank=WaitMem[Wait_index].rank;
		MakedRoom[Room_index].leader_sock=WaitMem[Wait_index].mysock;
		
		mem_meta_data[Wait_index]=GAMING_MEMBER_SLOT;//�ش� ȸ���� ���� ���� ������ ����.
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

	mem_meta_data[Wait_index]=WAITING_MEMBER_SLOT;//�ش� ȸ���� ���� ���� �ƴ� ������ ����.
	room_meta_data[Room_index]=EMPTY_ROOM_SLOT;//���� ������ ����ִ� ������ ����.
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
			TotalMem[User2_Num].rank++;	//������ �޼� ���� ����
	}

	if(win_flag==RED_WIN){
		TotalMem[User2_Num].win++;
		TotalMem[User1_Num].lose++;

		if(TotalMem[User2_Num].rank>1)
			TotalMem[User2_Num].rank--;

		if(TotalMem[User1_Num].rank<16)
			TotalMem[User1_Num].rank++;	//������ �޼� ���� ����
	}

	/* �α��� ȸ�� ���� ���� ���� */
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

	/* ���� ���� ���Ͽ� ���� */
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
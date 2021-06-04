#include "ConnectHdr.h"
#include "ServerHdr.h"

int AcceptLogin(int sock){
	int i;
	int Allow=0;
	int AlreadyLogin=0;
	int ThisMember=-1;
	char FileName[15];
	char Temp[10];
	
	CRITICAL_SECTION FileCS2;
	FILE* log;
	
	time_t ltime;
    struct tm *today;
    time(&ltime);
    today = localtime(&ltime);//�ð� ����.

	LoadMemberList();//���ԵǾ� �ִ� ȸ���� ��� ������ �迭�� ����.
	
	recv(sock, &IDSize, 4, 0);
	recv(sock, &ID, IDSize, 0);
	
	recv(sock, &PassSize, 4, 0);
	recv(sock, &pass, PassSize, 0);
	
	//���� ID ���� �α��ο� ���� ����.
	for(i=0;i<MAX_USER;i++){
		if(!strcmp(ID, WaitMem[i].ID))
			AlreadyLogin=1;
	}
	
	//�ߺ� �����ǿ� ���� ����.
	/*for(i=0;i<MAX_USER;i++){
		if(!strcmp(login_mem.IP, WaitMem[i].IP))
		AlreadyLogin=1;
	}*/
	
	for(i=0;i<TotalMem_Counter;i++){
		if( !strcmp(ID, TotalMem[i].id) && !strcmp(pass, TotalMem[i].pass) && !AlreadyLogin){
			printf("%s", TotalMem[i].id);
			ThisMember=i;
			Allow=1;
		}
	}
	
	/* Ŭ���̾�Ʈ�� ���� ���� */
	if(Allow==1)
		MsgType=LOGIN_ACCEPT;
	if(Allow==0)
		MsgType=LOGIN_DENY;
	printf(" ���� -> %d ", MsgType);
	send(sock, &MsgType, 1, 0);
	
	/* �α��� �Ϸ� ������ ���� */
	if(Allow==1){
		Wait_counter++;//�α��� �� ��� ���� ����� ���ڸ� ++;
		if(mem_meta_data[Wait_index]==EMPTY_MEMBER_SLOT){//��Ÿ �����Ͱ� �ش� ������ ������� �˸��� ���Կ� ȸ�� ���� �����ϰ� �ε��� ����.
			mem_meta_data[Wait_index]=WAITING_MEMBER_SLOT;//��Ÿ �����Ͱ� �ش� �ε����� ��� ȸ�� �ڸ��� �� ������ �ƴ��� ǥ��.
			
			WaitMem[Wait_index].number=TotalMem[ThisMember].mem_number;
			strcpy(WaitMem[Wait_index].IP, login_mem.IP);
			strcpy(WaitMem[Wait_index].ID, ID);
			WaitMem[Wait_index].rank=TotalMem[ThisMember].rank;
			WaitMem[Wait_index].mysock=login_mem.mysock;
		}
		else if(mem_meta_data[Wait_index]==WAITING_MEMBER_SLOT || mem_meta_data[Wait_index]==GAMING_MEMBER_SLOT){//�ش� ������ ��� ���� ��� �� ������ �˻��Ͽ� �� ���Կ� ȸ�� ���� ����.
			while(mem_meta_data[Wait_index]==WAITING_MEMBER_SLOT && Wait_index<MAX_USER){
				Wait_index++;
				if(Wait_index==MAX_USER)
					Wait_index=0;
			}
			mem_meta_data[Wait_index]=WAITING_MEMBER_SLOT;//��Ÿ �����Ͱ� �ش� �ε����� ��� ȸ�� �ڸ��� �� ������ �ƴ��� ǥ��.
			
			WaitMem[Wait_index].number=TotalMem[ThisMember].mem_number;
			strcpy(WaitMem[Wait_index].IP, login_mem.IP);
			strcpy(WaitMem[Wait_index].ID, ID);
			WaitMem[Wait_index].rank=TotalMem[ThisMember].rank;
			WaitMem[Wait_index].mysock=login_mem.mysock;
		}
		printf("\n%s%d. %s : %d���� �α����� %s %d��", asctime(today), Wait_index, WaitMem[Wait_index].IP, WaitMem[Wait_index].mysock, WaitMem[Wait_index].ID, WaitMem[Wait_index].rank);
	}
	
	send(sock, &Wait_index, 4, 0);//�� ����� � �ʿ��� ȸ�� �ε����� Ŭ���̾�Ʈ ��ü�� ������ �ֵ��� ��.
	send(sock, &WaitMem[Wait_index].rank, 4, 0);
	send(sock, &TotalMem[ThisMember].mem_number, 4, 0);
	
	/* �α��� ���� ���� �뺸�� ���� ���ϰ� ���� */
	if(Allow==1){
		///////////////
		/* �α� ��� */
		///////////////
		//log/��¥.log���Ϸ� ���� �̸� �����
		mkdir("log");
		strcpy(FileName, "log\\");
		itoa(today->tm_year-100, Temp, 10);
		strcat(FileName, Temp);
		itoa(today->tm_mon+1, Temp, 10);
		strcat(FileName, Temp);
		itoa(today->tm_mday, Temp, 10);
		strcat(FileName, Temp);
		strcat(FileName, ".log");
		printf("%s", FileName);
		
		InitializeCriticalSection(&FileCS2);
		EnterCriticalSection(&FileCS2);
		
		log=fopen(FileName, "a");
		
		fprintf(log, "%d��%d��%d��%d��%d�� %s %s", today->tm_year-100, today->tm_mon+1, today->tm_mday, today->tm_hour, today->tm_min, WaitMem[Wait_index].IP, WaitMem[Wait_index].ID);
		fprintf(log, "\n");
		
		fclose(log);
		LeaveCriticalSection(&FileCS2);
		DeleteCriticalSection(&FileCS2);

		return 1;
	}
	if(Allow==0)
		return 0;
}

void AcceptJoin(int sock){
	int i;
	int ID_Exist=1;
	int CN_Exist=1;
	
	FILE* out;
	
	LoadMemberList();//���ԵǾ� �ִ� ȸ���� ��� ������ �迭�� ����.
	
	/* ID �ߺ� �˻� */
	while(ID_Exist){
		recv(sock, &Data_size, 4, 0);
		recv(sock, &mem.id, Data_size, 0);
		ID_Exist=0;
		for(i=0;i<TotalMem_Counter;i++){
			if(!strcmp(mem.id, TotalMem[i].id)){
				ID_Exist=1;
			}
		}
		if(ID_Exist==1)
			MsgType=JOIN_DENY;
		if(!ID_Exist)
			MsgType=JOIN_ACCEPT;
		send(sock, &MsgType, 1, 0);
	}
	
	/* �ֹ� ��ȣ �ߺ� �˻� */
	while(CN_Exist){
		recv(sock, &Data_size, 4, 0);
		recv(sock, &mem.civil_number1, Data_size, 0);
		recv(sock, &Data_size, 4, 0);
		recv(sock, &mem.civil_number2, Data_size, 0);
		CN_Exist=0;
		for(i=0;i<TotalMem_Counter;i++){
			if( !strcmp(mem.civil_number1, TotalMem[i].civil_number1) && !strcmp(mem.civil_number2, TotalMem[i].civil_number2) ){
				CN_Exist=1;
			}
		}
		if(CN_Exist)
			MsgType=JOIN_DENY;
		if(!CN_Exist)
			MsgType=JOIN_ACCEPT;
		send(sock, &MsgType, 1, 0);
	}
	
	
	recv(sock, &Data_size, 4, 0);
	recv(sock, &mem, sizeof(mem), 0);
	
	mem.mem_number=TotalMem_Counter;

	InitializeCriticalSection(&FileCS);
	EnterCriticalSection(&FileCS);
	out=fopen("member.dat", "a");
	
	fprintf(out, "%d %s %s %s %s %s %s %d %d %d", mem.mem_number, mem.id, mem.pass, mem.name,
		mem.civil_number1, mem.civil_number2, mem.email_address,
		mem.rank, mem.win, mem.lose);
	fprintf(out, "\n");
	
	free(TotalMem);
	fclose(out);
	LeaveCriticalSection(&FileCS);
	DeleteCriticalSection(&FileCS);
}

void FindIDPass(int sock){
	int i;
	
	MsgType=FIND_DENY;
	
	recv(sock, &mem.civil_number1, sizeof(mem.civil_number1), 0);
	recv(sock, &mem.civil_number2, sizeof(mem.civil_number2), 0);
	recv(sock, &mem.email_address, sizeof(mem.email_address), 0);
	
	LoadMemberList();
	
	for(i=0;i<TotalMem_Counter;i++){
		if( !strcmp(mem.civil_number1, TotalMem[i].civil_number1) && !strcmp(mem.civil_number2, TotalMem[i].civil_number2) && !strcmp(mem.email_address, TotalMem[i].email_address) ){
			MsgType=FIND_ACCEPT;
			break;
		}
	}
	
	send(sock, &MsgType, 1, 0);
	
	if(MsgType==FIND_ACCEPT){
		send(sock, &TotalMem[i].id, sizeof(TotalMem[i].id), 0);
		send(sock, &TotalMem[i].pass, sizeof(TotalMem[i].pass), 0);
	}
}

void LoadMemberList(){
	FILE *in;
	
	int i;
	InitializeCriticalSection(&FileCS);
	EnterCriticalSection(&FileCS);
	TotalMem_Counter=MemberCount("member.dat");
	TotalMem=(Member*)malloc(TotalMem_Counter*sizeof(Member));

	in=fopen("member.dat", "r");
	for(i=0;i<TotalMem_Counter;i++){
		fscanf(in, "%d %s %s %s %s %s %s %d %d %d", &TotalMem[i].mem_number, TotalMem[i].id, TotalMem[i].pass, TotalMem[i].name,
			TotalMem[i].civil_number1, TotalMem[i].civil_number2, TotalMem[i].email_address,
			&TotalMem[i].rank, &TotalMem[i].win, &TotalMem[i].lose);
	}

	fclose(in);
	
	LeaveCriticalSection(&FileCS);
	DeleteCriticalSection(&FileCS);
}

int MemberCount(char *name){
	FILE *fp;
	int line=0;
	char c;
	fp=fopen(name, "r");
	while ((c=fgetc(fp))!=EOF){
		if(c=='\n')
			line++;
	}
	fclose(fp);
	return line;
}

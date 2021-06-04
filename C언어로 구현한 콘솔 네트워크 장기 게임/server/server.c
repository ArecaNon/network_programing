#include <stdio.h>
#include <process.h>
#include <winsock2.h>
#include <windows.h>

#include "ConnectHdr.h"
#include "ServerHdr.h"

#pragma comment(lib, "wsock32.lib")	
#define PORT 5000			// �����Ʈ�� 5000
#define ALLOW 65535			// �ִ� ���� ���� ���� ��ȣ 65535

void recv_client(void *sock);	// ������ �Լ� ������ Ÿ��

SOCKET ClntSock[ALLOW];

int main(){
	WSADATA wsd;
	SOCKET ServerSock;
	
	int addrsize, ret;
	int i;
	SOCKADDR_IN servAddr;
	SOCKADDR_IN clntAddr;
	
	memset(MakedRoom, 0, sizeof(MakedRoom));
	memset(WaitMem, 0, sizeof(Login_member));//�α��� ȸ��, ���� �� ���� �迭 �ʱ�ȭ.
	
	for(i=0;i<MAX_USER;i++){//��Ÿ ������ �ʱ�ȭ.
		mem_meta_data[i]=EMPTY_MEMBER_SLOT;
		room_meta_data[i]=EMPTY_ROOM_SLOT;
	}
	
	Wait_index=0;
	Wait_counter=0;
	
	Room_index=0;
	Room_counter=0;
	Wait_Room_counter=0;
	
	Client_Counter=0;
	Client_Num=0;
	
	hMutex=CreateMutex(NULL, FALSE, NULL);
	
	WSAStartup(MAKEWORD(1, 1), &wsd);
	
	ServerSock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(ServerSock==INVALID_SOCKET)
		ErrorHandling("socket() error");
	
	memset(&servAddr, 0, sizeof(servAddr)); //0�� ����Ʈ�ϴ� �޸𸮸� �� sercaddr�� sizeof���� ä���.
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_family=AF_INET;
	servAddr.sin_port=htons(PORT);
	
	if(bind(ServerSock, (SOCKADDR*)&servAddr, sizeof(servAddr))==SOCKET_ERROR)
		ErrorHandling("bind() error");
	
	printf("\n���� ���� �ο� : %d\n", MAX_USER-Client_Counter);
	
	if(listen(ServerSock, 10)==SOCKET_ERROR)
		ErrorHandling("listen() error");
	
	addrsize=sizeof(clntAddr);
	
	while(1){
		ClntSock[Client_Num]=accept(ServerSock, (SOCKADDR*)&clntAddr, &addrsize);
		
		if(Client_Counter<MAX_USER){//Ŭ���̾�Ʈ �� ���ѿ� �������� �ʾ��� �� ���� ����.
			if(ClntSock[Client_Num]!=INVALID_SOCKET || ClntSock[Client_Num]!=SOCKET_ERROR){}
			login_mem.mysock=ClntSock[Client_Num];
			strcpy(login_mem.IP, inet_ntoa(clntAddr.sin_addr));
			
			_beginthread(recv_client, 0, &ClntSock[Client_Num]);
			Sleep(10);
			printf("%d�� Ŭ���̾�Ʈ %s : %d ���� ����\n", Client_Num, inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));
		}
		
		else{//Ŭ���̾�Ʈ �� ���ѿ� �������� �� ���� �ź�.
			addrsize=sizeof(clntAddr);
			if (ClntSock[Client_Num]==INVALID_SOCKET){
				printf("���� ���� �ο��� �ʰ��߽��ϴ�.\n");
				closesocket(ClntSock[Client_Num]);
				closesocket(ServerSock);
				WSACleanup();
				return 1;
			}
			
			MsgType=ACCESS_DENY;
			ret=send(ClntSock[Client_Num], &MsgType, 1, 0);
			closesocket(ClntSock[Client_Num]);
			
		}
	}
	return 0;
}

void recv_client(void *sock){
	int ret, i;
	int wait_flag=0;
	int ImHost=0;
	int ImGuest=0;
	int EnterRoomNum=-1;
	int mysock=*(SOCKET*)sock;
	int MemNumber=-1;
	
	WaitForSingleObject(hMutex,INFINITE);
	
	Client_Counter++;//Ŭ���̾�Ʈ �� ����.
	Client_Num++;//Ŭ���̾�Ʈ ��ȣ ����.
	
	printf("\n���� ���� �ο� : %d\n", MAX_USER-Client_Counter);
	
	ReleaseMutex(hMutex);
	
	MsgType=ACCESS_ACCEPT;
	ret=send(*(SOCKET*)sock, &MsgType, 1, 0);//����
	
	while(ret!=SOCKET_ERROR || ret!=INVALID_SOCKET){
		MsgType=0;
		ret=recv(*(SOCKET*)sock, &MsgType, 1, 0);
		
		if(MsgType!=0)
			printf("\n%d ��û -> ", MsgType);
		
		/* �޽��� Ÿ�Կ� ���� ���� */
		/* �α��� */
		if(MsgType==LOGIN_REQUEST){
			wait_flag=AcceptLogin(*(SOCKET*)sock);
		}
		
		/* ȸ�� ���� */
		if(MsgType==JOIN_REQUEST)
			AcceptJoin(*(SOCKET*)sock);
		
		if(MsgType==FIND_REQUEST)
			FindIDPass(*(SOCKET*)sock);
		
		/* ȸ�� ���� ���� */
		if(MsgType==RE_REQUEST){
			recv(*(SOCKET*)sock, &MemNumber, 4, 0);
			MsgType=RE_ACK;
			send(*(SOCKET*)sock, &MsgType, 1, 0);
			send(*(SOCKET*)sock, &TotalMem[MemNumber], sizeof(TotalMem[MemNumber]), 0);
		}
		
		/* ä�� */
		if(MsgType==CHAT_REQUEST){
			HandleChating(*(SOCKET*)sock);
			for(i=0;i<Client_Num;i++){//ä�� �޽��� ���� �� �̸� broadcast.
				WaitForSingleObject(hMutex, INFINITE);
				MsgType=CHAT_ACK;
				send(ClntSock[i], &MsgType, 1, 0);
				send(ClntSock[i], Full_Msg, sizeof(Full_Msg), 0);
				ReleaseMutex(hMutex);
			}
			ID[0]='\0';
			//memset(ID, 0, IDSize);
			EchoString[0]='\0';
			//memset(EchoString, 0, Msg_size);
		}
		
		/* �� �̵� */
		if(MsgType==ROOM_CHANGE_REQUEST){
			ChangeRoom(*(SOCKET*)sock);
		}
		
		/* �� ���� */
		if(MsgType==ENTER_ROOM_REQUEST){
			ImGuest=1;
			Wait_counter--;
			wait_flag=0;
			EnterRoom(*(SOCKET*)sock);
			EnterRoomNum=room.room_number;
			printf("%d", Wait_Room_counter);
		}
		
		/* �� ������ */
		if(MsgType==EXIT_ROOM_REQUEST){
			ImGuest=0;
			EnterRoomNum=-1;
			Wait_counter++;
			wait_flag=1;
			ExitRoom(*(SOCKET*)sock);
		}
		
		/* �� ���� */
		if(MsgType==MY_INFO_REQUEST){
			recv(*(SOCKET*)sock, &ID, sizeof(ID), 0);
			SendInfo(*(SOCKET*)sock);
		}

		/* ���� ���� */
		if(MsgType==MODIFY_REQUEST){
			ModInfo(*(SOCKET*)sock);
		}
		
		/* �� ����� */
		if(MsgType==CREATE_ROOM_REQUEST){
			ImHost=1;
			Wait_counter--;
			wait_flag=0;
			CreateRoom(*(SOCKET*)sock);
			EnterRoomNum=room.room_number;
		}
		
		/* �� ���� */
		if(MsgType==BREAK_ROOM_REQUEST){
			ImHost=0;
			Wait_counter++;
			wait_flag=1;
			BreakRoom(*(SOCKET*)sock);
		}
		
		/* ���� ������ ��� */
		if(MsgType==WAIT_LIST_REQUEST){
			SendWaitList(*(SOCKET*)sock);
		}
		
		/* ���� ��� �뺸 */
		if(MsgType==REPORT_WIN){
			Win(*(SOCKET*)sock);
		}
		
	}
	
	/* ���ӵ� ������ ���� ���� �� */
	WaitForSingleObject(hMutex,INFINITE);
	
	for(i=0;i<MAX_USER;i++){//����� ���� ����.
		if(WaitMem[i].mysock==mysock){
			if(mem_meta_data[i]==GAMING_MEMBER_SLOT && ImGuest){
				Wait_Room_counter++;
				room_meta_data[EnterRoomNum]=WAITING_ROOM_SLOT;
			}
			if(ImHost && room_meta_data[EnterRoomNum]!=WAITING_ROOM_SLOT){
				Wait_Room_counter--;
				printf("\ndecrease Room counter\n");
			}
			printf("delete Mem info\n");
			mem_meta_data[i]=EMPTY_MEMBER_SLOT;
			WaitMem[i].ID[0]='\0';
			WaitMem[i].IP[0]='\0';
			WaitMem[i].mysock=-1;
			WaitMem[i].number=-1;
			WaitMem[i].rank=-1;
			break;
		}
	}
	
	if(wait_flag==1){
		Wait_counter--;
	}
	
	for(i=0;i<MAX_USER;i++){//�� ���� ����.
		if(MakedRoom[i].leader_sock==mysock){//���� ���� ���¿��� ���� ���� �� ����.
			Room_counter--;
			if(room_meta_data[i]==WAITING_ROOM_SLOT)
				Wait_Room_counter--;
			
			room_meta_data[i]=EMPTY_ROOM_SLOT;
			printf("delete Room info\n");
			MakedRoom[i].IP[0]='\0';
			MakedRoom[i].leader_ID[0]='\0';
			MakedRoom[i].leader_rank=-1;
			MakedRoom[i].leader_sock=-1;
			MakedRoom[i].room_name[0]='\0';
			MakedRoom[i].room_number=-1;
			break;
		}
	}
	
	Client_Counter--;
	printf("\n%d�� Ŭ���̾�Ʈ���� ���� ����\n���� ���� �ο� : %d\n", Client_Num, MAX_USER - Client_Counter);
	ReleaseMutex(hMutex);
	
	closesocket(*(int*)sock);
	
	return;
}

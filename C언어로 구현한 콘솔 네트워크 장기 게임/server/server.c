#include <stdio.h>
#include <process.h>
#include <winsock2.h>
#include <windows.h>

#include "ConnectHdr.h"
#include "ServerHdr.h"

#pragma comment(lib, "wsock32.lib")	
#define PORT 5000			// 사용포트는 5000
#define ALLOW 65535			// 최대 생성 가능 소켓 번호 65535

void recv_client(void *sock);	// 스레드 함수 프로토 타입

SOCKET ClntSock[ALLOW];

int main(){
	WSADATA wsd;
	SOCKET ServerSock;
	
	int addrsize, ret;
	int i;
	SOCKADDR_IN servAddr;
	SOCKADDR_IN clntAddr;
	
	memset(MakedRoom, 0, sizeof(MakedRoom));
	memset(WaitMem, 0, sizeof(Login_member));//로그인 회원, 개설 방 정보 배열 초기화.
	
	for(i=0;i<MAX_USER;i++){//메타 데이터 초기화.
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
	
	memset(&servAddr, 0, sizeof(servAddr)); //0이 포인트하는 메모리를 값 sercaddr로 sizeof개로 채운다.
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_family=AF_INET;
	servAddr.sin_port=htons(PORT);
	
	if(bind(ServerSock, (SOCKADDR*)&servAddr, sizeof(servAddr))==SOCKET_ERROR)
		ErrorHandling("bind() error");
	
	printf("\n여유 수용 인원 : %d\n", MAX_USER-Client_Counter);
	
	if(listen(ServerSock, 10)==SOCKET_ERROR)
		ErrorHandling("listen() error");
	
	addrsize=sizeof(clntAddr);
	
	while(1){
		ClntSock[Client_Num]=accept(ServerSock, (SOCKADDR*)&clntAddr, &addrsize);
		
		if(Client_Counter<MAX_USER){//클라이언트 수 제한에 도달하지 않았을 시 접속 승인.
			if(ClntSock[Client_Num]!=INVALID_SOCKET || ClntSock[Client_Num]!=SOCKET_ERROR){}
			login_mem.mysock=ClntSock[Client_Num];
			strcpy(login_mem.IP, inet_ntoa(clntAddr.sin_addr));
			
			_beginthread(recv_client, 0, &ClntSock[Client_Num]);
			Sleep(10);
			printf("%d번 클라이언트 %s : %d 에서 접속\n", Client_Num, inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));
		}
		
		else{//클라이언트 수 제한에 도달했을 시 접속 거부.
			addrsize=sizeof(clntAddr);
			if (ClntSock[Client_Num]==INVALID_SOCKET){
				printf("수용 가능 인원을 초과했습니다.\n");
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
	
	Client_Counter++;//클라이언트 수 증가.
	Client_Num++;//클라이언트 번호 증가.
	
	printf("\n여유 수용 인원 : %d\n", MAX_USER-Client_Counter);
	
	ReleaseMutex(hMutex);
	
	MsgType=ACCESS_ACCEPT;
	ret=send(*(SOCKET*)sock, &MsgType, 1, 0);//전송
	
	while(ret!=SOCKET_ERROR || ret!=INVALID_SOCKET){
		MsgType=0;
		ret=recv(*(SOCKET*)sock, &MsgType, 1, 0);
		
		if(MsgType!=0)
			printf("\n%d 요청 -> ", MsgType);
		
		/* 메시지 타입에 따른 동작 */
		/* 로그인 */
		if(MsgType==LOGIN_REQUEST){
			wait_flag=AcceptLogin(*(SOCKET*)sock);
		}
		
		/* 회원 가입 */
		if(MsgType==JOIN_REQUEST)
			AcceptJoin(*(SOCKET*)sock);
		
		if(MsgType==FIND_REQUEST)
			FindIDPass(*(SOCKET*)sock);
		
		/* 회원 정보 갱신 */
		if(MsgType==RE_REQUEST){
			recv(*(SOCKET*)sock, &MemNumber, 4, 0);
			MsgType=RE_ACK;
			send(*(SOCKET*)sock, &MsgType, 1, 0);
			send(*(SOCKET*)sock, &TotalMem[MemNumber], sizeof(TotalMem[MemNumber]), 0);
		}
		
		/* 채팅 */
		if(MsgType==CHAT_REQUEST){
			HandleChating(*(SOCKET*)sock);
			for(i=0;i<Client_Num;i++){//채팅 메시지 수신 시 이를 broadcast.
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
		
		/* 방 이동 */
		if(MsgType==ROOM_CHANGE_REQUEST){
			ChangeRoom(*(SOCKET*)sock);
		}
		
		/* 방 참가 */
		if(MsgType==ENTER_ROOM_REQUEST){
			ImGuest=1;
			Wait_counter--;
			wait_flag=0;
			EnterRoom(*(SOCKET*)sock);
			EnterRoomNum=room.room_number;
			printf("%d", Wait_Room_counter);
		}
		
		/* 방 나오기 */
		if(MsgType==EXIT_ROOM_REQUEST){
			ImGuest=0;
			EnterRoomNum=-1;
			Wait_counter++;
			wait_flag=1;
			ExitRoom(*(SOCKET*)sock);
		}
		
		/* 내 정보 */
		if(MsgType==MY_INFO_REQUEST){
			recv(*(SOCKET*)sock, &ID, sizeof(ID), 0);
			SendInfo(*(SOCKET*)sock);
		}

		/* 정보 변경 */
		if(MsgType==MODIFY_REQUEST){
			ModInfo(*(SOCKET*)sock);
		}
		
		/* 방 만들기 */
		if(MsgType==CREATE_ROOM_REQUEST){
			ImHost=1;
			Wait_counter--;
			wait_flag=0;
			CreateRoom(*(SOCKET*)sock);
			EnterRoomNum=room.room_number;
		}
		
		/* 방 깨기 */
		if(MsgType==BREAK_ROOM_REQUEST){
			ImHost=0;
			Wait_counter++;
			wait_flag=1;
			BreakRoom(*(SOCKET*)sock);
		}
		
		/* 현재 접속자 목록 */
		if(MsgType==WAIT_LIST_REQUEST){
			SendWaitList(*(SOCKET*)sock);
		}
		
		/* 게임 결과 통보 */
		if(MsgType==REPORT_WIN){
			Win(*(SOCKET*)sock);
		}
		
	}
	
	/* 접속된 소켓이 연결 해제 시 */
	WaitForSingleObject(hMutex,INFINITE);
	
	for(i=0;i<MAX_USER;i++){//사용자 슬롯 비우기.
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
	
	for(i=0;i<MAX_USER;i++){//방 슬롯 비우기.
		if(MakedRoom[i].leader_sock==mysock){//방을 만든 상태에서 강제 종료 시 적용.
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
	printf("\n%d번 클라이언트에서 연결 해제\n여유 수용 인원 : %d\n", Client_Num, MAX_USER - Client_Counter);
	ReleaseMutex(hMutex);
	
	closesocket(*(int*)sock);
	
	return;
}

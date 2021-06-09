#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32.lib")
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<process.h>
#include<winsock2.h>
#include<windows.h>

#define BUF_SIZE 104
#define READ 3
#define WRITE 5
#define MAX_CLNT 256
#define MAFIA1 1 
#define MAFIA2 2
#define CITIZEN 3
#define TRUE 1
#define FALSE 0
#define MAX_PLAYER 6 //플레이어 수
#define MAX_MAFIA 2 //마피아 수
#define MAX_CITIZEN 4  //시민 수
#define MAFIA_WIN 1 //마피아 승리
#define CITIZEN_WIN 0 //시민 승리
#define DAY_CONFERENCE_TIME 20 //회의시간
#define DAY_VOTE_TIME 30 //투표시간

//socket info
typedef struct {
	SOCKET hClntSock;
	SOCKADDR_IN clntAdr;
} PER_HANDLE_DATA, * LPPER_HANDLE_DATA;

//buffer info
typedef struct {
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];
	int rwMode;
} PER_IO_DATA, * LPPER_IO_DATA;

void setRoomInfoMsg(char*);
void setConnectedMsg(char* msg, int clntNum);
void setDisconnectedMsg(char* msg, int clntNum);
void setChatMsg(char* msg, int clntNum, char* chatMsg);
void setGameStartMsg(char* msg, int role, int clntNum);
void setDayTrigger(char* msg, int result);
void setNightTrigger(char* msg, int result);
void setGameEndTrigger(char* msg, int result);

int isGameEnd(int roomNum);
int checkRoomNum(SOCKET clnt);
void sendMsgToAll(char* message, int roomNum, DWORD len);
void errorHandling(char* message);

DWORD WINAPI gameManageThread();
DWORD WINAPI mainThread(LPVOID  pComPort);

int clntCnt; //클라이언트 수
SOCKET clntSocks[MAX_CLNT]; //클라이언트 소켓
int clntCntOfRoom[2]; //현재 방 인원수
SOCKET clntSocksOfRoom[2][MAX_PLAYER]; //방에 들어가있는 클라이언트 소켓
int mafiaIndex[2][MAX_MAFIA]; //마피아 인덱스
int mafiaCnt[2]; //마피아 숫자
int citizenCnt[2]; //시민 숫자
int gameStart[2]; //게임시작 여부
int vote[2][MAX_PLAYER]; //투표 값
int voteCnt[2]; //투표횟수
CRITICAL_SECTION cs[3];

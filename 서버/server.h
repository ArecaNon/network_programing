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
#define MAX_PLAYER 6 //�÷��̾� ��
#define MAX_MAFIA 2 //���Ǿ� ��
#define MAX_CITIZEN 4  //�ù� ��
#define MAFIA_WIN 1 //���Ǿ� �¸�
#define CITIZEN_WIN 0 //�ù� �¸�
#define DAY_CONFERENCE_TIME 20 //ȸ�ǽð�
#define DAY_VOTE_TIME 30 //��ǥ�ð�

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

int clntCnt; //Ŭ���̾�Ʈ ��
SOCKET clntSocks[MAX_CLNT]; //Ŭ���̾�Ʈ ����
int clntCntOfRoom[2]; //���� �� �ο���
SOCKET clntSocksOfRoom[2][MAX_PLAYER]; //�濡 ���ִ� Ŭ���̾�Ʈ ����
int mafiaIndex[2][MAX_MAFIA]; //���Ǿ� �ε���
int mafiaCnt[2]; //���Ǿ� ����
int citizenCnt[2]; //�ù� ����
int gameStart[2]; //���ӽ��� ����
int vote[2][MAX_PLAYER]; //��ǥ ��
int voteCnt[2]; //��ǥȽ��
CRITICAL_SECTION cs[3];

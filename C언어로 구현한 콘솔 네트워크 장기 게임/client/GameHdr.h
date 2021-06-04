#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

#define CHAT_BUF_SIZE		53
#define USER_ID_SIZE		12

char MsgType;				//�޽��� Ÿ��

/* �޽��� Ÿ�� �������� */
#define GAME_READY			10//10����� ���� �غ� ����
#define GAME_START			11
#define MOVE_PIECE			20//20����� ���� ���� ����
#define REQUEST_UNDO		21
#define ACCEPT_UNDO			22
#define DENY_UNDO			23
#define NOTICE_WIN			24
#define REQUEST_DRAW		25
#define ACCEPT_DRAW			26
#define DENY_DRAW			27
#define EXIT				28
#define SEND_CHAT_MSG		30//30����� ä�� ����
#define REPORT_WIN			99

/* �� �ν� */
#define BLUE				100
#define RED					200

/* �º� �̰���/�� ��/�� ��/���º� */
#define YET_WIN				0
#define BLUE_WIN			1
#define RED_WIN				2
#define DRAW_WIN			3

/* �⹰ ��ȣ */
#define EMPTY				999

#define BLUE_KING			1001
#define BLUE_SCHOLAR		1002
#define BLUE_CHARIOT		1003
#define BLUE_CANNON			1004
#define BLUE_KNIGHT			1005
#define BLUE_ELEPHANT		1006
#define BLUE_PRIVATE		1007

#define RED_KING			2001
#define RED_SCHOLAR			2002
#define RED_CHARIOT			2003
#define RED_CANNON			2004
#define RED_KNIGHT			2005
#define RED_ELEPHANT		2006
#define RED_PRIVATE			2007

/* ��/�� ��ġ ���� */
#define EKEK				3434
#define EKKE				3443
#define KEKE				4343
#define KEEK				4334

int ImHost;					//ȣ��Ʈ, �Խ�Ʈ ������.

int Host_Board_Type;		//ȣ��Ʈ ���� ��/�� ��ġ ����.
int Guest_Board_Type;		//�Խ�Ʈ ���� ��/�� ��ġ ����.

int myturn;					//�ڽ��� �� �ν�.
int Turn;					//�� �ν�.
int win_flag;				//�¸� ���� �ν�.
int chat_flag;				//ä�� �Է� ��� �ν�.

int Board[9][10];			//����� �迭.
int undo_Board[9][10];		//������ ����� ���� ����� �迭.
int x, y;					//������� �ε���.
int Move_Count;				//������� �ְ� ���� ��.

int cursor_x, cursor_y;		//Ŀ�� ��ǥ.
int selected;				//���õ� �⹰�� ����.
int selected_x, selected_y;	//���õ� �⹰�� ���� ��ǥ.
int moved_x, moved_y;		//���õ� �⹰�� �̵��ϰ� �� ��ǥ.

int All_continue;
int Do_Game;				//���� ���� ���θ� ǥ��.
int Wait_Guest;				//���ǿ����� ���� ���Ḧ ���� ����Ǵ� �Խ�Ʈ ��� �������� ���� �÷���.

char Send_EchoString[CHAT_BUF_SIZE+1];	//���� ä�� �޽���.
int Send_ChatMsgSize;		//���� ä�� �޽����� ũ��.
char Recv_EchoString[CHAT_BUF_SIZE+1];	//���� ä�� �޽���.
int Recv_ChatMsgSize;		//���� ä�� �޽����� ũ��.
char GameEchoBuf[6][CHAT_BUF_SIZE+1];		//ä�� ������ 6������ ����.
char UserBuf[6][USER_ID_SIZE+1];

int Action;					//Ű �Է�.
int Lock;					//���� ��.
int kill_chat;				//ä�� ������ break ����.
int exit_flag;				//�Ϲ��� ���� ���� �÷���.
int confirm_flag;			//Ŭ���̾�Ʈ ���� Ȯ��â ��� �� ����.

char User1[12];
char User2[12];
int User_Rank1;
int User_Rank2;

char EchoString[58];	//���� ä�� �޽���.
int ChatMsgSize;		//���� ä�� �޽����� ũ��.

int ServSock;
int RoomNum;

/* �Լ� */
int Host(char* gamePort);
int Guest(char* IP_Add, char* gamePort);
void Connect_Thread();
void Report_Server();

void InitGame();
void ShowBoard(int x, int y);

void Wait_Host();
void ReadyGame_Host(int sock, int ChatSock);
void ReadyGame_Guest(int sock, int ChatSock);
int SelectBoardType(int sock);

void PlayGame(int sock);
void WaitTurn(int sock);
int Victory(int sock);
void ErrorHandling(char *message);

void Selected(int selected);
void PrintManual(int selected);
int MovePiece();

void ShowUser();
int InputString(char* Msg, int size);
void GameChatWindow();
void SendChat(int sock);
void RecvChat(int sock);
void Game_Recv_Thread(void* sock);
void Game_Send_Thread(void* sock);//�ڽ��� ���ʰ� �ƴ� ��� ä�� �޽����� ������ ���� ���Ǵ� ������.
void GameChatClear();//ä�� â �����.

/* �ڵ鷯 �� ������ ID, ũ��Ƽ�� ���� */
HANDLE  hThreadSend;
HANDLE  hThreadRecv;
DWORD   dwThreadIDSend;
DWORD   dwThreadIDRecv;

HANDLE	hThreadCon;
DWORD	dwThreadIDCon;

CRITICAL_SECTION ChatCS;
CRITICAL_SECTION BoardCS;

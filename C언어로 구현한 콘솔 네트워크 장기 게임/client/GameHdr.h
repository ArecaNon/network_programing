#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

#define CHAT_BUF_SIZE		53
#define USER_ID_SIZE		12

char MsgType;				//메시지 타입

/* 메시지 타입 프로토콜 */
#define GAME_READY			10//10번대는 게임 준비 과정
#define GAME_START			11
#define MOVE_PIECE			20//20번대는 게임 진행 과정
#define REQUEST_UNDO		21
#define ACCEPT_UNDO			22
#define DENY_UNDO			23
#define NOTICE_WIN			24
#define REQUEST_DRAW		25
#define ACCEPT_DRAW			26
#define DENY_DRAW			27
#define EXIT				28
#define SEND_CHAT_MSG		30//30번대는 채팅 관련
#define REPORT_WIN			99

/* 턴 인식 */
#define BLUE				100
#define RED					200

/* 승부 미결정/초 승/한 승/무승부 */
#define YET_WIN				0
#define BLUE_WIN			1
#define RED_WIN				2
#define DRAW_WIN			3

/* 기물 번호 */
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

/* 상/마 배치 설정 */
#define EKEK				3434
#define EKKE				3443
#define KEKE				4343
#define KEEK				4334

int ImHost;					//호스트, 게스트 구분자.

int Host_Board_Type;		//호스트 측의 상/마 배치 상태.
int Guest_Board_Type;		//게스트 측의 상/마 배치 상태.

int myturn;					//자신의 턴 인식.
int Turn;					//턴 인식.
int win_flag;				//승리 여부 인식.
int chat_flag;				//채팅 입력 모드 인식.

int Board[9][10];			//장기판 배열.
int undo_Board[9][10];		//무르기 기능을 위한 백업용 배열.
int x, y;					//장기판의 인덱스.
int Move_Count;				//현재까지 주고 받은 수.

int cursor_x, cursor_y;		//커서 좌표.
int selected;				//선택된 기물의 종류.
int selected_x, selected_y;	//선택된 기물의 현재 좌표.
int moved_x, moved_y;		//선택된 기물이 이동하게 될 좌표.

int All_continue;
int Do_Game;				//게임 속행 여부를 표시.
int Wait_Guest;				//대기실에서의 게임 종료를 위해 수행되는 게스트 대기 스레드의 종료 플래그.

char Send_EchoString[CHAT_BUF_SIZE+1];	//보낼 채팅 메시지.
int Send_ChatMsgSize;		//보낼 채팅 메시지의 크기.
char Recv_EchoString[CHAT_BUF_SIZE+1];	//보낼 채팅 메시지.
int Recv_ChatMsgSize;		//보낼 채팅 메시지의 크기.
char GameEchoBuf[6][CHAT_BUF_SIZE+1];		//채팅 내역을 6개까지 저장.
char UserBuf[6][USER_ID_SIZE+1];

int Action;					//키 입력.
int Lock;					//스핀 락.
int kill_chat;				//채팅 스레드 break 여부.
int exit_flag;				//일방적 종료 여부 플래그.
int confirm_flag;			//클라이언트 측에 확인창 띄울 지 여부.

char User1[12];
char User2[12];
int User_Rank1;
int User_Rank2;

char EchoString[58];	//보낼 채팅 메시지.
int ChatMsgSize;		//보낼 채팅 메시지의 크기.

int ServSock;
int RoomNum;

/* 함수 */
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
void Game_Send_Thread(void* sock);//자신의 차례가 아닐 경우 채팅 메시지를 보내기 위해 사용되는 스레드.
void GameChatClear();//채팅 창 지우기.

/* 핸들러 및 스레드 ID, 크리티컬 섹션 */
HANDLE  hThreadSend;
HANDLE  hThreadRecv;
DWORD   dwThreadIDSend;
DWORD   dwThreadIDRecv;

HANDLE	hThreadCon;
DWORD	dwThreadIDCon;

CRITICAL_SECTION ChatCS;
CRITICAL_SECTION BoardCS;

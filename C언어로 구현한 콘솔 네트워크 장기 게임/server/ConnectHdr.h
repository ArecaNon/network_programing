#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>

/* 프로토콜 */
char MsgType;

#define ACCESS_ACCEPT		8
#define ACCESS_DENY			9
#define LOGIN_REQUEST		10
#define LOGIN_ACCEPT		11
#define LOGIN_DENY			12
#define JOIN_REQUEST		20
#define JOIN_ACCEPT			21
#define JOIN_DENY			22
#define FIND_REQUEST		23
#define FIND_ACCEPT			24
#define FIND_DENY			25
#define RE_REQUEST			26
#define RE_ACK				27
#define MODIFY_REQUEST		28
#define CHAT_REQUEST		30
#define CHAT_ACK			31
#define WAIT_LIST_REQUEST	32
#define WAIT_LIST_ACK		33
#define CREATE_ROOM_REQUEST 34
#define CREATE_ROOM_ACK		35
#define BREAK_ROOM_REQUEST	36
#define MY_INFO_REQUEST		37
#define MY_INFO_ACK			38
#define ROOM_CHANGE_REQUEST	40
#define ROOM_MINUS_REQUEST	41
#define ROOM_PLUS_REQUEST	42
#define ROOM_CHANGE_ACK		43
#define ENTER_ROOM_REQUEST	44
#define ENTER_ROOM_ACK		45
#define ENTER_ROOM_DENY		46
#define	EXIT_ROOM_REQUEST	47
#define REPORT_WIN			99

#define EMPTY_MEMBER_SLOT	0
#define WAITING_MEMBER_SLOT	1
#define GAMING_MEMBER_SLOT	2

#define EMPTY_ROOM_SLOT		0
#define WAITING_ROOM_SLOT	1
#define GAMING_ROOM_SLOT	2
#define UNBROKEN_ROOM_SLOT	3

#define CHAT_BUF_SIZE		58
#define CHAT_BUF_NUM		12
#define BASIC_DATA_SIZE		12
#define CIVIL_NUMBER1_SIZE	6
#define CIVIL_NUMBER2_SIZE	7
#define EMAIL_ADD_SIZE		40

#define MAX_USER			5
#define ROOM_NAME_SIZE		58

struct Member{
	int mem_number;
	char id[BASIC_DATA_SIZE+1];
	char pass[BASIC_DATA_SIZE+1];
	char name[BASIC_DATA_SIZE+1];
	char civil_number1[CIVIL_NUMBER1_SIZE+1];
	char civil_number2[CIVIL_NUMBER2_SIZE+1];
	char email_address[EMAIL_ADD_SIZE+1];
	int rank;
	int win;
	int lose;
} mem;

struct Room{
	int room_number;
	char IP[16];
	char room_name[CHAT_BUF_SIZE+1];
	char leader_ID[BASIC_DATA_SIZE+1];
	int leader_rank;
	int leader_sock;//방장 강제 종료 시 방 초기화를 위함.
	char host_port[6];
} room;

typedef struct Room Room;
Room MakedRoom[MAX_USER];
int Room_index;
int Room_counter;
int Wait_Room_counter;

struct Login_member{
	int number;
	int mysock;
	char ID[BASIC_DATA_SIZE+1];
	int rank;
	char IP[16];
} login_mem;

typedef struct Login_member Login_member;
Login_member WaitMem[MAX_USER];
int Wait_index;
int Wait_counter;

int mem_meta_data[MAX_USER];
int room_meta_data[MAX_USER];

char ID[12];
char pass[12];

int IDSize;
int PassSize;
int Data_size;
int Msg_size;

int Action;
int Login_flag;
char Config_Pass[BASIC_DATA_SIZE+1];

char EchoString[CHAT_BUF_SIZE+1];
char Full_Msg[BASIC_DATA_SIZE+CHAT_BUF_SIZE+3];
char EchoBuf[CHAT_BUF_NUM][BASIC_DATA_SIZE+CHAT_BUF_SIZE+3];

char Room_Name[CHAT_BUF_SIZE+1];

char char_move_room_number[5];
int move_room_number;

char HostPort[6];
char TempPort[6];//연속 방 개설을 위한 포트 넘버 생성 배열.
/* 미 사용 분량 */
char* name_request_fight;


void ErrorHandling(char *message);

/* 핸들 */
HANDLE hMutex;
HANDLE hThread;

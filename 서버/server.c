#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32.lib")
#include<stdio.h>
#include<stdlib.h>
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
#define MAX_PLAYER 6
#define MAX_MAFIA 2
#define MAX_CITIZEN 4 
#define MAFIA_WIN 0
#define CITIZEN_WIN 1
#define DAY_CONFERENCE_TIME 30
#define DAY_VOTE_TIME 10
#define NIGHT_CONFERENCE_TIME 20
#define NIGHT_VOTE_TIME 10

//socket info
typedef struct {
	SOCKET hClntSock;
	SOCKADDR_IN clntAdr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

//buffer info
typedef struct {
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];
	int rwMode;
} PER_IO_DATA, *LPPER_IO_DATA;

DWORD WINAPI gameManageThread();
DWORD WINAPI threadMain(LPVOID completionPortIO);
void sendMsgToAll(char* message, int roomNum, DWORD len);
void errorHandling(char* message);

int clntCnt = 0;
SOCKET clntSocks[MAX_CLNT]; 
int clntCntOfRoom[2]; //추가
SOCKET clntSocksOfRoom[2][MAX_PLAYER]; //추가
int mafiaIndex[2][MAX_MAFIA]; //마피아 번호
int mafiaCnt[2]; //마피아 수
int citizenCnt[2]; //시민수
int gameStart[2];
int vote[2][MAX_PLAYER];
int voteCnt[2];
CRITICAL_SECTION cs[3];

void setRoomInfoMsg(char *msg) {
	msg[0] = 'J';
	msg[1] = 2;
	msg[2] = clntCntOfRoom[0];
	msg[3] = clntCntOfRoom[1];
	msg[4] = 0;
}

void setConnectedMsg(char* msg, int clntNum) {
	msg[0] = 'H';
	msg[1] = 1;
	msg[2] = clntNum;
	msg[3] = 0;
}

void setDisconnectedMsg(char* msg, int clntNum) {
	msg[0] = 'I';
	msg[1] = 1;
	msg[2] = clntNum;
	msg[3] = 0;
}

void setChatMsg(char* msg, int clntNum, char* chatMsg) {
	msg[0] = 'B';
	msg[1] = strlen(chatMsg) + 2;
	msg[2] = clntNum;
	strcpy(msg + 3, chatMsg);
}

void setGameStartMsg(char* msg, int role, int clntNum) {
	msg[0] = 'A';
	msg[1] = 2;
	msg[2] = clntNum;
	msg[3] = role;
	msg[4] = 0;
}

void setDayTrigger(char* msg, int result) {
	msg[0] = 'C';
	msg[1] = 5;
	msg[2] = result;
	msg[3] = DAY_CONFERENCE_TIME;
	msg[4] = DAY_VOTE_TIME;
	msg[5] = 0;
}

void setNightTrigger(char* msg, int result) {
	msg[0] = 'D';
	msg[1] = 5;
	msg[2] = result;
	msg[3] = NIGHT_CONFERENCE_TIME;
	msg[4] = NIGHT_VOTE_TIME;
	msg[5] = 0;
}

void setGameEndTrigger(char* msg, int result) {
	msg[0] = 'G';
	msg[1] = 3;
	msg[2] = result;
	msg[3] = 0;
}

int isGameEnd(int roomNum) {
	if (mafiaCnt[roomNum] == 0 || mafiaCnt[roomNum] >= citizenCnt[roomNum]) {
		return TRUE;
	}
	else return FALSE;
}

int checkRoomNum(SOCKET clnt) {
	int i;
	int roomNum = 1;

	for (i = 0; i < clntCntOfRoom[0]; i++) {
		if (clnt == clntSocksOfRoom[0][i]) {
			roomNum = 0;
			break;
		}
	}

	return roomNum;
}

int main() {
	WSADATA wsaData;
	HANDLE hComPort;
	SYSTEM_INFO sysInfo;
	LPPER_IO_DATA ioInfo;
	LPPER_HANDLE_DATA handleInfo;

	SOCKET hServSock;
	SOCKADDR_IN servAdr;
	char input;
	int recvBytes, i, flags = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		errorHandling("WSAStartup() error!");
	}

	InitializeCriticalSection(&cs[0]);
	InitializeCriticalSection(&cs[1]);
	InitializeCriticalSection(&cs[2]);
	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	GetSystemInfo(&sysInfo);
	for (i = 0; i < sysInfo.dwNumberOfProcessors; i++) {
		_beginthreadex(NULL, 0, threadMain, (LPVOID)hComPort, 0, NULL);
	}

	_beginthreadex(NULL, 0, gameManageThread, NULL, 0, NULL);

	hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi("50000"));

	bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr));
	listen(hServSock, 5);

	while (1) {
		SOCKET hClntSock;
		SOCKADDR_IN clntAdr;
		char message[BUF_SIZE];
		int addrLen = sizeof(clntAdr);

		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);
		EnterCriticalSection(&cs[0]);
		clntSocks[clntCnt++] = hClntSock;
		LeaveCriticalSection(&cs[0]);

		handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
		handleInfo->hClntSock = hClntSock;
		memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);

		CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (DWORD)handleInfo, 0);
		
		setRoomInfoMsg(message);
		ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
		ioInfo->wsaBuf.len = strlen(message);
		ioInfo->wsaBuf.buf = message;
		ioInfo->rwMode = WRITE;
		WSASend(hClntSock, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);

		ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
		ioInfo->wsaBuf.len = BUF_SIZE;
		ioInfo->wsaBuf.buf = ioInfo->buffer;
		ioInfo->rwMode = READ;
		WSARecv(handleInfo->hClntSock, &(ioInfo->wsaBuf), 1, &recvBytes, &flags, &(ioInfo->overlapped), NULL);
	}

	return 0;
}

// 6명 채워지면 역할 부여 후 게임 시작시켜줌
DWORD WINAPI gameManageThread() {
	int i, mafia1, mafia2;
	LPPER_IO_DATA ioInfo;

	while (1) {
		if (!gameStart[0] && clntCntOfRoom[0] == MAX_PLAYER) {
			gameStart[0] = TRUE;
			mafiaCnt[0] = MAX_MAFIA;
			citizenCnt[0] = MAX_CITIZEN;
			voteCnt[0] = 0;
			memset(vote[0], 0, sizeof(int) * MAX_PLAYER);
			
			mafia1 = rand() % MAX_PLAYER;
			mafia2 = rand() % MAX_PLAYER;

			while (mafia1 == mafia2) {
				mafia2 = rand() % MAX_PLAYER;
			}
			mafiaIndex[0][0] = mafia1;
			mafiaIndex[0][1] = mafia2;

			for (i = 0; i < MAX_PLAYER; i++) {
				char message[BUF_SIZE];

				ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));

				if (i == mafia1) {
					setGameStartMsg(message, MAFIA1, i + 1);
					sendMsgToAll(message, 0, strlen(message));
				}
				else if (i == mafia2) {
					setGameStartMsg(message, MAFIA2, i + 1);
					sendMsgToAll(message, 0, strlen(message));
				}
				else {
					setGameStartMsg(message, CITIZEN, i + 1);
					sendMsgToAll(message, 0, strlen(message));
				}
			}
		}
		else if (!gameStart[1] && clntCntOfRoom[1] == MAX_PLAYER) {
			gameStart[1] = TRUE;
			mafiaCnt[1] = MAX_MAFIA;
			citizenCnt[1] = MAX_CITIZEN;
			voteCnt[1] = 0;
			memset(vote[1], 0, sizeof(int) * MAX_PLAYER);

			mafia1 = rand() % MAX_PLAYER;
			mafia2 = rand() % MAX_PLAYER;

			while (mafia1 == mafia2) {
				mafia2 = rand() % MAX_PLAYER;
			}
			mafiaIndex[1][0] = mafia1;
			mafiaIndex[1][1] = mafia2;

			for (i = 0; i < MAX_PLAYER; i++) {
				char message[BUF_SIZE];

				ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));

				if (i == mafia1) {
					setGameStartMsg(message, MAFIA1, i + 1);
					sendMsgToAll(message, 1, strlen(message));
				}
				else if (i == mafia2) {
					setGameStartMsg(message, MAFIA2, i + 1);
					sendMsgToAll(message, 1, strlen(message));
				}
				else {
					setGameStartMsg(message, CITIZEN, i + 1);
					sendMsgToAll(message, 1, strlen(message));
				}
			}
		}
	}
	return 0;
}

DWORD WINAPI threadMain(LPVOID pComPort) {
	HANDLE hComPort = (HANDLE)pComPort;
	SOCKET sock;
	DWORD bytesTrans;
	LPPER_HANDLE_DATA handleInfo;
	LPPER_IO_DATA ioInfo;
	DWORD flags;
	char message[BUFSIZ];
	int i, j, state, index, clntNum, currentClntNum, deathPerson, isEqual, max;
	int roomNum, result;

	flags = 0;

	while (1) {
		GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPWORD)&handleInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);
		sock = handleInfo->hClntSock;
		roomNum = checkRoomNum(sock);

		if (ioInfo->rwMode == READ) {
			if (bytesTrans == 0) { //EOF
				for (i = 0; i < clntCnt; i++)   //remove disconnected client
				{
					if (sock == clntSocks[i])
					{
						EnterCriticalSection(&cs[0]);
						while (i++ < clntCnt - 1) {
							clntSocks[i] = clntSocks[i + 1];
						}
						for (j = 0; j < clntCntOfRoom[0]; j++) {
							if (sock == clntSocksOfRoom[roomNum][j]) {
								setDisconnectedMsg(message, j + 1);
								while (j++ < clntCntOfRoom[roomNum] - 1) {
									clntSocksOfRoom[roomNum][j] = clntSocksOfRoom[roomNum][j + 1];
								}
							}
						}
						clntCntOfRoom[roomNum]--;
						LeaveCriticalSection(&cs[0]);
						sendMsgToAll(message, roomNum, strlen(message));
						break;
					}
				}
				clntCnt--;

				closesocket(sock);
				free(handleInfo);
				free(ioInfo);

				continue;
			}
			state = ioInfo->buffer[0];

			switch (state) {
			case 'B':
				clntNum = ioInfo->buffer[2];
				ioInfo->buffer[ioInfo->buffer[1] + 2] = 0;
				strcpy(message, ioInfo->buffer + 3);
				setChatMsg(ioInfo->wsaBuf.buf, clntNum, message);
				sendMsgToAll(ioInfo->wsaBuf.buf, roomNum, strlen(ioInfo->wsaBuf.buf));
				break;
			case 'E':
				currentClntNum = mafiaCnt[roomNum] + citizenCnt[roomNum];
				index = ioInfo->buffer[2] - 1;
				EnterCriticalSection(&cs[1]);
				vote[roomNum][index]++;
				voteCnt[roomNum]++;
				LeaveCriticalSection(&cs[1]);
				if (voteCnt[roomNum] == currentClntNum) {
					max = 0;
					deathPerson = -1;
					isEqual = FALSE;
					for (i = 0; i < MAX_PLAYER; i++) {
						if (vote[roomNum][i] > max) {
							max = vote[roomNum][i];
							deathPerson = i + 1;
							isEqual = FALSE;
						}
						else if (vote[roomNum][i] == max) {
							isEqual = TRUE;
						}
						vote[roomNum][i] = 0;
					}

					if (isEqual) {
						deathPerson = -1;
					}

					if (deathPerson - 1 == mafiaIndex[roomNum][0] || deathPerson - 1 == mafiaIndex[roomNum][1]) {
						mafiaCnt[roomNum]--;
					}
					else if (deathPerson != -1) {
						citizenCnt[roomNum]--;
					}

					if (isGameEnd(roomNum)) {
						if (mafiaCnt[roomNum] == 0) {
							result = CITIZEN_WIN;
						}
						else {
							result = MAFIA_WIN;
						}
						setGameEndTrigger(ioInfo->wsaBuf.buf, result);
						gameStart[roomNum] = FALSE;
					}

					else {
						setNightTrigger(ioInfo->wsaBuf.buf, deathPerson);
					}
					sendMsgToAll(ioInfo->wsaBuf.buf, roomNum, strlen(ioInfo->wsaBuf.buf));

					memset(vote[roomNum], 0, sizeof(int) * MAX_PLAYER);
					voteCnt[roomNum] = 0;
				}
				break;
			case 'F':
				citizenCnt[roomNum]--;
				if (isGameEnd(roomNum)) {
					if (mafiaCnt[roomNum] == 0) {
						result = CITIZEN_WIN;
					}
					else {
						result = MAFIA_WIN;
					}
					setGameEndTrigger(ioInfo->wsaBuf.buf, result);
					gameStart[roomNum] = FALSE;
				}
				else {
					deathPerson = ioInfo->buffer[2];
					setDayTrigger(ioInfo->wsaBuf.buf, deathPerson);
				}
				sendMsgToAll(ioInfo->wsaBuf.buf, roomNum, strlen(ioInfo->wsaBuf.buf));
				break;
			case 'J':
				roomNum = ioInfo->buffer[2] - 1;
				EnterCriticalSection(&cs[2]);
				clntSocksOfRoom[roomNum][clntCntOfRoom[roomNum]++] = sock;
				setConnectedMsg(message, clntCntOfRoom[roomNum]);
				LeaveCriticalSection(&cs[2]);
				sendMsgToAll(message, roomNum, strlen(message));
				break;
			}

			ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len = BUF_SIZE;
			ioInfo->wsaBuf.buf = ioInfo->buffer;
			ioInfo->rwMode = READ;
			WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
		}
		else {
			free(ioInfo);
		}
	}

	return 0;
}

void sendMsgToAll(char* message, int roomNum, DWORD len) {
	WSABUF wsaBuf;
	LPPER_IO_DATA ioInfo;
	int i, recvBytes;

	for (i = 0; i < MAX_PLAYER; i++) {
		ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
		ioInfo->wsaBuf.len = len;
		ioInfo->wsaBuf.buf = message;
		ioInfo->rwMode = WRITE;
		WSASend(clntSocksOfRoom[roomNum][i], &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
	}
}

void errorHandling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
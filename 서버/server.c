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

DWORD WINAPI threadMain(LPVOID completionPortIO);
void sendMsgToAll(char* message, DWORD len);
void errorHandling(char* message);

int mafia1, mafia2;
int clntCnt = 0;
int mafiaCnt = 2;
int citizenCnt = 4;
int vote[MAX_CLNT] = { 0 };
int voteCount;
SOCKET clntSocks[MAX_CLNT];
CRITICAL_SECTION cs1, cs2;

void setConnectedMsg(char* msg) {
	msg[0] = 'H';
	msg[1] = 1;
	msg[2] = clntCnt;
	msg[3] = 0;
}

void setDisconnectedMsg(char* msg) {
	msg[0] = 'I';
	msg[1] = 1;
	msg[2] = clntCnt;
	msg[3] = 0;
}

void setChatMsg(char* msg, int clntNum, char* chatMsg) {
	msg[0] = 'B';
	msg[1] = strlen(chatMsg) + 2;
	msg[2] = clntNum;
	strcpy(msg + 3, chatMsg);
}

void setGameStartMsg(char* msg, char role, char clntNum) {
	msg[0] = 'A';
	msg[1] = 2;
	msg[2] = clntNum;
	msg[3] = role;
	msg[4] = 0;
}

void setDayTrigger(char* msg, char result) {
	msg[0] = 'C';
	msg[1] = 5;
	msg[2] = result;
	msg[3] = 30;
	msg[4] = 10;
	msg[5] = 0;
}

void setNightTrigger(char* msg, char result) {
	msg[0] = 'D';
	msg[1] = 5;
	msg[2] = result;
	msg[3] = 20;
	msg[4] = 5;
	msg[5] = 0;
}

void setGameEndTrigger(char* msg) {
	msg[0] = 'G';
	msg[1] = 3;
	msg[2] = 1;
	msg[3] = 0;
}

int isGameEnd() {
	printf("%d %d\n", mafiaCnt, citizenCnt);
	if (mafiaCnt == 0 || mafiaCnt >= citizenCnt) {
		return TRUE;
	}
	else return FALSE;
}

int main() {
	WSADATA wsaData;
	HANDLE hComPort;
	SYSTEM_INFO sysInfo;
	LPPER_IO_DATA ioInfo;
	LPPER_HANDLE_DATA handleInfo;

	SOCKET hServSock;
	SOCKADDR_IN servAdr;
	int recvBytes, i, flags;

	flags = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		errorHandling("WSAStartup() error!");
	}

	InitializeCriticalSection(&cs1);
	InitializeCriticalSection(&cs2);
	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	GetSystemInfo(&sysInfo);
	for (i = 0; i < sysInfo.dwNumberOfProcessors; i++) {
		_beginthreadex(NULL, 0, threadMain, (LPVOID)hComPort, 0, NULL);
	}

	hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi("50000"));

	bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr));
	listen(hServSock, 5);

	while (clntCnt < 6) {
		SOCKET hClntSock;
		SOCKADDR_IN clntAdr;
		char message[BUF_SIZE];
		int addrLen = sizeof(clntAdr);

		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);
		EnterCriticalSection(&cs1);
		clntSocks[clntCnt++] = hClntSock;
		LeaveCriticalSection(&cs1);

		handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
		handleInfo->hClntSock = hClntSock;
		memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);

		CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (DWORD)handleInfo, 0);

		setConnectedMsg(message);
		sendMsgToAll(message, strlen(message));

		ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
		ioInfo->wsaBuf.len = BUF_SIZE;
		ioInfo->wsaBuf.buf = ioInfo->buffer;
		ioInfo->rwMode = READ;
		WSARecv(handleInfo->hClntSock, &(ioInfo->wsaBuf), 1, &recvBytes, &flags, &(ioInfo->overlapped), NULL);
	}
	EnterCriticalSection(&cs1);//테스트용 크리키컬 섹션
	Sleep(3000);
	printf("게임 시작!");
	mafia1 = (rand() % clntCnt);
	mafia2 = (rand() % clntCnt);

	while (mafia1 == mafia2) {
		mafia2 = rand() % clntCnt;
	}
	printf("마피아는 %d %d\n", mafia1, mafia2);
	
	for (i = 0; i < clntCnt; i++) {
		char message[BUF_SIZE];

		ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));

		if (i == mafia1) {
			setGameStartMsg(message, MAFIA1, i + 1);
			sendMsgToAll(message, strlen(message));
		}
		else if (i == mafia2) {
			setGameStartMsg(message, MAFIA2, i + 1);
			sendMsgToAll(message, strlen(message));
		}
		else {
			setGameStartMsg(message, CITIZEN, i + 1);
			sendMsgToAll(message, strlen(message));
		}

	}
	LeaveCriticalSection(&cs1);

	while (clntCnt > 0);

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
	int i, state, index, clntNum, currentClntNum, deathPerson = -1, isEqual = 0;

	flags = 0;

	while (1) {
		GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPWORD)&handleInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);
		sock = handleInfo->hClntSock;

		if (ioInfo->rwMode == READ) {
			if (bytesTrans == 0) { //EOF
				for (i = 0; i < clntCnt; i++)   //remove disconnected client
				{
					if (sock == clntSocks[i])
					{
						setDisconnectedMsg(message);
						sendMsgToAll(message, strlen(message));
						EnterCriticalSection(&cs1);
						while (i++ < clntCnt - 1)
							clntSocks[i] = clntSocks[i + 1];
						LeaveCriticalSection(&cs1);
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
				sendMsgToAll(ioInfo->wsaBuf.buf, strlen(ioInfo->wsaBuf.buf));
				break;
			case 'E':
				currentClntNum = mafiaCnt + citizenCnt;
				index = ioInfo->buffer[2] - 1;
				//printf("투표로 %d가 투표되었습니다  ", index);
				EnterCriticalSection(&cs2);
				vote[index]++;
				voteCount++;
				//printf("%d : %d\n", vote[index], voteCount);
				LeaveCriticalSection(&cs2);
				if (voteCount == currentClntNum) {
					int max = 0;
					for (i = 0; i < clntCnt; i++) {
						if (vote[i] > max) {
							max = vote[i];
							deathPerson = i+1;
							isEqual = 0;
						}
						else if (vote[i] == max) {
							isEqual = 1;
						}
						vote[i] = 0;
					}

					if (isEqual) {
						deathPerson = -1;
					}

					if (deathPerson-1 == mafia1 || deathPerson-1 == mafia2) {
						mafiaCnt--;
					}
					else if (deathPerson != -1) {
						citizenCnt--;
					}

					if (isGameEnd()) {
						setGameEndTrigger(ioInfo->wsaBuf.buf);
					}

					else {
						setNightTrigger(ioInfo->wsaBuf.buf, (char)deathPerson);
					}
					//printf("투표로 %d가 죽었습니다", deathPerson);
					sendMsgToAll(ioInfo->wsaBuf.buf, strlen(ioInfo->wsaBuf.buf));

					memset(vote, 0, BUF_SIZE);
					voteCount = 0;
				}
				break;
			case 'F':
				citizenCnt--;
				if (isGameEnd()) {
					setGameEndTrigger(ioInfo->wsaBuf.buf);
				}
				else {
					deathPerson = ioInfo->buffer[2];
					printf("마피아가 %d를 죽임\n", deathPerson);
					setDayTrigger(ioInfo->wsaBuf.buf, (char)deathPerson);
				}
				sendMsgToAll(ioInfo->wsaBuf.buf, strlen(ioInfo->wsaBuf.buf));
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

void sendMsgToAll(char* message, DWORD len) {
	WSABUF wsaBuf;
	LPPER_IO_DATA ioInfo;
	int i, recvBytes, a;

	for (i = 0; i < clntCnt; i++) {
		ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
		ioInfo->wsaBuf.len = len;
		ioInfo->wsaBuf.buf = message;
		ioInfo->rwMode = WRITE;
		WSASend(clntSocks[i], &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
	}
}

void errorHandling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
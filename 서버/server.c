#define _CRT_SECURE_NO_WARNINGS
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
SOCKET clntSocks[MAX_CLNT];

void setConnectedMsg(char* msg) {
	msg[0] = 'H';
	msg[1] = 1;
	msg[2] = clntCnt - 1;
	msg[3] = 0;
}

void setChatMsg(char* msg, char clntNum, char* chatMsg) {
	msg[0] = 'B';
	msg[1] = strlen(chatMsg) + 2;
	msg[2] = clntNum;
	strcpy(msg + 3, chatMsg);
}

void setGameStartMsg(char* msg, char role) {
	msg[0] = 'A';
	msg[1] = 1;
	msg[2] = role;
	msg[3] = 0;
}

void setDayTrigger(char *msg, char result) {
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
	if (mafiaCnt == 0 || mafiaCnt >= citizenCnt) {
		return TRUE;
	}
	else {
		return FALSE;
	}
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
		errorHandling("WSAStartup() erroor!");
	}

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

	for (i = 0; i < 6; i++) {
		SOCKET hClntSock;
		SOCKADDR_IN clntAdr;
		char message[BUF_SIZE];
		int addrLen = sizeof(clntAdr);

		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);
		clntSocks[clntCnt++] = hClntSock;

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

	mafia1 = rand() % clntCnt;
	mafia2 = rand() % clntCnt;

	while (mafia1 == mafia2) {
		mafia2 = rand() % clntCnt;
	}

	for (i = 0; i < clntCnt; i++) {
		char message[BUF_SIZE];

		ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));

		if (i == mafia1) {
			setGameStartMsg(message, MAFIA1);
			ioInfo->wsaBuf.len = strlen(message);
			ioInfo->wsaBuf.buf = message;
			ioInfo->rwMode = WRITE;;
			WSASend(clntSocks[i], &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
		}
		else if (i == mafia2) {
			setGameStartMsg(message, MAFIA2);
			ioInfo->wsaBuf.len = strlen(message);
			ioInfo->wsaBuf.buf = message;
			ioInfo->rwMode = WRITE;;
			WSASend(clntSocks[i], &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
		}
		else {
			setGameStartMsg(message, CITIZEN);
			ioInfo->wsaBuf.len = strlen(message);
			ioInfo->wsaBuf.buf = message;
			ioInfo->rwMode = WRITE;;
			WSASend(clntSocks[i], &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
		}
	}

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
	char temp[BUFSIZ];
	int i, state, index, clntNum, currentClntNum = clntCnt, count = 0, deathPerson = -1, isEqual = 0;
	int countVote[MAX_CLNT] = { 0, };


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
						while (i++ < clntCnt - 1)
							clntSocks[i] = clntSocks[i + 1];
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
				strcpy(temp, ioInfo->buffer + 3);
				setChatMsg(ioInfo->wsaBuf.buf, clntNum, temp);
				sendMsgToAll(ioInfo->wsaBuf.buf, strlen(ioInfo->wsaBuf.buf));
				break;
			case 'E':
				index = atoi(ioInfo->buffer[2]);
				countVote[index]++;
				count++;
				if (count == currentClntNum) {
					int max = 0;
					for (i = 0; i < clntCnt; i++) {
						if (countVote[i] > max) {
							max = countVote[i];
							deathPerson = i;
							isEqual = 0;
						}
						else if (countVote[i] == max) {
							isEqual = 1;
						}
						countVote[i] = 0;
					}

					if (isEqual) {
						deathPerson = -1;
					}

					if (deathPerson == mafia1 || deathPerson == mafia2) {
						currentClntNum--;
						mafiaCnt--;
					}
					else if (deathPerson != -1) {
						currentClntNum--;
						citizenCnt--;
					}

					if (isGameEnd()) {
						setGameEndTrigger(ioInfo->wsaBuf.buf);
					}

					else {
						setNightTrigger(ioInfo->wsaBuf.buf, (char)deathPerson);
					}
					sendMsgToAll(ioInfo->wsaBuf.buf, strlen(ioInfo->wsaBuf.buf));

					count = 0;
				}
				break;
			case 'F':
				citizenCnt--;
				if (isGameEnd()) {
					setGameEndTrigger(ioInfo->wsaBuf.buf);
				}
				else {
					deathPerson = atoi(&(ioInfo->buffer[2]));
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
			if (ioInfo->wsaBuf.buf[0] == 'A') {
				setDayTrigger(ioInfo->wsaBuf.buf, -2);
				ioInfo->wsaBuf.len = strlen(ioInfo->wsaBuf.buf);
				ioInfo->rwMode = WRITE;;
				WSASend(clntSocks[i], &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
			}
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
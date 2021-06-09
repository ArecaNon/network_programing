#include "server.h"

int main() {
	WSADATA wsaData;
	HANDLE hComPort;
	SYSTEM_INFO sysInfo;
	LPPER_IO_DATA ioInfo; //buffer info
	LPPER_HANDLE_DATA handleInfo; //socket info

	SOCKET hServSock;
	SOCKADDR_IN servAdr;
	char input;
	int recvBytes, i, flags = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		errorHandling("WSAStartup() error!");
	}

	//cs 초기화
	InitializeCriticalSection(&cs[0]);
	InitializeCriticalSection(&cs[1]);
	InitializeCriticalSection(&cs[2]);

	srand(time(NULL)); //seed값 설정

	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	GetSystemInfo(&sysInfo);
	//thread 실행
	for (i = 0; i < sysInfo.dwNumberOfProcessors; i++) {
		_beginthreadex(NULL, 0, mainThread, (LPVOID)hComPort, 0, NULL);
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
		
		//방 현재인원 정보 클라이언트로 전송
		setRoomInfoMsg(message);
		ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
		ioInfo->wsaBuf.len = 4;
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

// 6명 채워지면 역할 부여 후 게임 시작
DWORD WINAPI gameManageThread() {
	int i, mafia1, mafia2;
	LPPER_IO_DATA ioInfo;

	while (1) {
		if (!gameStart[0] && clntCntOfRoom[0] == MAX_PLAYER) { //1번방 게임시작전이고 인원수가 채워졌을 때
			//게임시작전 초기화
			gameStart[0] = TRUE; 
			mafiaCnt[0] = MAX_MAFIA;
			citizenCnt[0] = MAX_CITIZEN;
			voteCnt[0] = 0;
			memset(vote[0], 0, sizeof(int) * MAX_PLAYER);
			
			//마피아 설정
			mafia1 = rand() % MAX_PLAYER;
			mafia2 = rand() % MAX_PLAYER;

			//마피아 index가 같을때
			while (mafia1 == mafia2) {
				mafia2 = rand() % MAX_PLAYER;
			}
			mafiaIndex[0][0] = mafia1;
			mafiaIndex[0][1] = mafia2;
			Sleep(1000);

			//1번방 유저들에게 게임시작 메세지 전송
			for (i = 0; i < MAX_PLAYER; i++){
				char message[BUF_SIZE];

				ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
				memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
				Sleep(100);
				
				if (i == mafia1) {
					setGameStartMsg(message, MAFIA1, i + 1);
					ioInfo->wsaBuf.len = strlen(message);
					ioInfo->wsaBuf.buf = message;
					ioInfo->rwMode = WRITE;
					WSASend(clntSocksOfRoom[0][i], &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
				}
				else if (i == mafia2) {
					setGameStartMsg(message, MAFIA2, i + 1);
					ioInfo->wsaBuf.len = strlen(message);
					ioInfo->wsaBuf.buf = message;
					ioInfo->rwMode = WRITE;
					WSASend(clntSocksOfRoom[0][i], &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
				}
				else {
					setGameStartMsg(message, CITIZEN, i + 1);
					ioInfo->wsaBuf.len = strlen(message);
					ioInfo->wsaBuf.buf = message;
					ioInfo->rwMode = WRITE;
					WSASend(clntSocksOfRoom[0][i], &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
				}
			}
		}
		else if (!gameStart[1] && clntCntOfRoom[1] == MAX_PLAYER) { //2번방 게임시작전이고 인원수가 채워졌을 때
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
			Sleep(1000);

			//2번방 유저들에게 게임시작 메세지 전송
			for (i = 0; i < MAX_PLAYER; i++) {
				char message[BUF_SIZE];

				ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
				memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
				Sleep(100);

				if (i == mafia1) {
					setGameStartMsg(message, MAFIA1, i + 1);
					ioInfo->wsaBuf.len = strlen(message);
					ioInfo->wsaBuf.buf = message;
					ioInfo->rwMode = WRITE;
					WSASend(clntSocksOfRoom[1][i], &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
				}
				else if (i == mafia2) {
					setGameStartMsg(message, MAFIA2, i + 1);
					ioInfo->wsaBuf.len = strlen(message);
					ioInfo->wsaBuf.buf = message;
					ioInfo->rwMode = WRITE;
					WSASend(clntSocksOfRoom[1][i], &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
				}
				else {
					setGameStartMsg(message, CITIZEN, i + 1);
					ioInfo->wsaBuf.len = strlen(message);
					ioInfo->wsaBuf.buf = message;
					ioInfo->rwMode = WRITE;
					WSASend(clntSocksOfRoom[1][i], &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
				}
			}
		}
	}
	return 0;
}

DWORD WINAPI mainThread(LPVOID pComPort) {
	HANDLE hComPort = (HANDLE)pComPort;
	SOCKET sock;
	DWORD bytesTrans;
	LPPER_HANDLE_DATA handleInfo;
	LPPER_IO_DATA ioInfo;
	DWORD flags;
	char message[BUF_SIZE];
	int i, j, state, index, clntNum, currentClntNum, deathPerson, isEqual, max;
	int roomNum, result;

	flags = 0;

	while (1) {
		GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPWORD)&handleInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);
		sock = handleInfo->hClntSock;
		roomNum = checkRoomNum(sock); //몇번방 클라이언트인지 확인

		if (ioInfo->rwMode == READ) {
			if (bytesTrans == 0) { //EOF
				for (i = 0; i < clntCnt; i++)   //연결해제된 클라이언트 삭제
				{
					if (sock == clntSocks[i])
					{
						EnterCriticalSection(&cs[0]);
						while (i++ < clntCnt - 1) {
							clntSocks[i] = clntSocks[i + 1];
						}
						//클라이언트가 있던방에서 클라이언트 삭제
						for (j = 0; j < MAX_PLAYER; j++) {
							if (sock == clntSocksOfRoom[roomNum][j]) {
								clntSocksOfRoom[roomNum][j] = 0;
								setDisconnectedMsg(message, j + 1);
								if (clntCntOfRoom[roomNum] > 0) {
									clntCntOfRoom[roomNum]--;
								}
							}
						}
						//게임이 시작되고 클라이언트 수가 0이되면 게임종료로 설정
						if (gameStart[roomNum] && !clntCntOfRoom[roomNum]) {
							gameStart[roomNum] = FALSE;
						}
						LeaveCriticalSection(&cs[0]);
						//연결해제 메세지 전송
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
			case 'B': //채팅 메세지 전송
				clntNum = ioInfo->buffer[2];
				ioInfo->buffer[ioInfo->buffer[1] + 2] = 0;
				strcpy(message, ioInfo->buffer + 3);
				setChatMsg(ioInfo->wsaBuf.buf, clntNum, message);
				sendMsgToAll(ioInfo->wsaBuf.buf, roomNum, strlen(ioInfo->wsaBuf.buf));
				break;
			case 'E': //투표 정보 받아와서 결과 전송
				currentClntNum = mafiaCnt[roomNum] + citizenCnt[roomNum]; //현재 살아있는 유저 수
				index = ioInfo->buffer[2] - 1; //투표 정보
				EnterCriticalSection(&cs[1]);
				vote[roomNum][index]++;
				voteCnt[roomNum]++;
				LeaveCriticalSection(&cs[1]);
				if (voteCnt[roomNum] == currentClntNum) { //투표가 완료되면
					max = 0;
					deathPerson = -1;
					isEqual = FALSE;
					//투표로 죽은사람 확인
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
					//투표가 동률인 경우 무효처리
					if (isEqual) {
						deathPerson = -1;
					}
					//결과가 마피아인 경우
					if (deathPerson - 1 == mafiaIndex[roomNum][0] || deathPerson - 1 == mafiaIndex[roomNum][1]) {
						mafiaCnt[roomNum]--;
					}
					//결과가 시민인 경우
					else if (deathPerson != -1) {
						citizenCnt[roomNum]--;
					}

					//게임종료시 결과 전송
					if (isGameEnd(roomNum)) {
						if (mafiaCnt[roomNum] == 0) {
							result = CITIZEN_WIN;
						}
						else {
							result = MAFIA_WIN;
						}
						setGameEndTrigger(ioInfo->wsaBuf.buf, result);
					}
					//밤 트리거 메세지 전송
					else {
						setNightTrigger(ioInfo->wsaBuf.buf, deathPerson);
					}
					sendMsgToAll(ioInfo->wsaBuf.buf, roomNum, strlen(ioInfo->wsaBuf.buf));

					memset(vote[roomNum], 0, sizeof(int) * MAX_PLAYER);
					voteCnt[roomNum] = 0;
				}
				break;
			case 'F': //마피아가 죽인대상 확인후 결과 전송
				citizenCnt[roomNum]--;
				//게임 종료시 결과 전송
				if (isGameEnd(roomNum)) {
					if (mafiaCnt[roomNum] == 0) {
						result = CITIZEN_WIN;
					}
					else {
						result = MAFIA_WIN;
					}
					setGameEndTrigger(ioInfo->wsaBuf.buf, result);
				}
				//낮 트리거 메세지 전송
				else {
					deathPerson = ioInfo->buffer[2];
					setDayTrigger(ioInfo->wsaBuf.buf, deathPerson);
				}
				sendMsgToAll(ioInfo->wsaBuf.buf, roomNum, strlen(ioInfo->wsaBuf.buf));
				break;
			case 'K': //몇번방 입장할건지 확인후 입장
				roomNum = ioInfo->buffer[2] - 1; //방 번호
				EnterCriticalSection(&cs[2]);
				//해당하는 방에 입장
				for (i = 0; i < MAX_PLAYER; i++) {
					if (!clntSocksOfRoom[roomNum][i]) {
						setConnectedMsg(message, i + 1);
						break;
					}
				}
				clntSocksOfRoom[roomNum][i] = sock;
				clntCntOfRoom[roomNum]++;
				LeaveCriticalSection(&cs[2]);
				//입장 메세지 전송
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

//게임시작 메세지 설정
void setGameStartMsg(char* msg, int role, int clntNum) {
	msg[0] = 'A';
	msg[1] = 4;
	msg[2] = clntNum;
	msg[3] = role;
	msg[4] = DAY_CONFERENCE_TIME;
	msg[5] = DAY_VOTE_TIME;
	msg[6] = 0;
}

//채팅 메세지 설정
void setChatMsg(char* msg, int clntNum, char* chatMsg) {
	msg[0] = 'B';
	msg[1] = strlen(chatMsg) + 1;
	msg[2] = clntNum;
	strcpy(msg + 3, chatMsg);
}

//낮 트리거 메세지 설정
void setDayTrigger(char* msg, int result) {
	msg[0] = 'C';
	msg[1] = 1;
	msg[2] = result;
	msg[3] = 0;
}

//밤 트리거 메세지 설정
void setNightTrigger(char* msg, int result) {
	msg[0] = 'D';
	msg[1] = 1;
	msg[2] = result;
	msg[3] = 0;
}

//게임 종료 메세지 설정
void setGameEndTrigger(char* msg, int result) {
	msg[0] = 'G';
	msg[1] = 1;
	msg[2] = result;
	msg[3] = 0;
}

//연결 메세지 설정
void setConnectedMsg(char* msg, int clntNum) {
	msg[0] = 'H';
	msg[1] = 1;
	msg[2] = clntNum;
	msg[3] = 0;
}

//연결해제 메세지 설정
void setDisconnectedMsg(char* msg, int clntNum) {
	msg[0] = 'I';
	msg[1] = 1;
	msg[2] = clntNum;
	msg[3] = 0;
}

//현재 방인원 정보 메세지 설정
void setRoomInfoMsg(char* msg) {
	msg[0] = 'J';
	msg[1] = 2;
	msg[2] = clntCntOfRoom[0];
	msg[3] = clntCntOfRoom[1];
	msg[4] = 0;
}

//게임이 종료됐는지 확인
int isGameEnd(int roomNum) {
	if (mafiaCnt[roomNum] == 0 || mafiaCnt[roomNum] >= citizenCnt[roomNum]) { //마피아가 다죽거나 마피아수가 시민수 이상인 경우
		return TRUE;
	}
	else return FALSE;
}

//몇번방인지 확인
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

//해당방에 입장해있는 클라이언트들에게 메세지 전송
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

//에러처리
void errorHandling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
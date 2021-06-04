void ShowMain(int sock);
int Login(int sock);
int Join(int sock);
void FindIDPass(int sock);

int InputValue(char* Msg, int size);
int InputPass(char* Msg, int size);
int InputCivilNum(char* Msg, int size);
int InputMsg(char* Msg, int size);
int InputNum(char* Msg, int size);

void Show_MyInfo(int sock);

int GameMain(int Arg, char* ID, int Rank, char* IP, char* EnemyID, int EnemyRank, int sock, char* Port, int RecvRoomNum);
void WaitingRoom(int sock);
int CreateRoom(int sock);
void WaitingList();
void EnterRoomNumber(int sock);
void GenPortNumber();
void Help();

void Send_Chat(int sock);
void Recv_Thread(void* sock);

void Recv_Chat(int sock);
void WRoomChatWindow();
void WRoomChatClear();

void Recv_Re(int sock);
void RecvWaitingList(int sock);
void Recv_Room_Str(int sock);
void Recv_RoomNumber(int sock);

int other_func;

int cur_x, cur_y;

int MyRank;
int MyNumber;

CRITICAL_SECTION WRoomCS;
CRITICAL_SECTION WRoomChatSendCS;
CRITICAL_SECTION WRoomChatRecvCS;

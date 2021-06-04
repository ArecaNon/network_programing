int AcceptLogin(int sock);
void AcceptJoin(int sock);
void FindIDPass(int sock);
void HandleChating(int sock);
int MemberCount(char *name);
void LoadMemberList();

void ChangeRoom(int sock);
void EnterRoom(int sock);
void ExitRoom(int sock);

void CreateRoom(int sock);
void BreakRoom(int sock);
void SendWaitList(int sock);
void SendInfo(int sock);
void ModInfo(int sock);
void Win(int sock);

typedef struct Member Member;

int TotalMem_Counter;

int Client_Counter;
int Client_Num;

/* 승패 결정 시 사용 */
char User1[12];
char User2[12];
int win_flag;
#define BLUE_WIN			1
#define RED_WIN				2

Member* TotalMem;

CRITICAL_SECTION FileCS;

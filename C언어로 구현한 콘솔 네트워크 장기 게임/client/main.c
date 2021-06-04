#include "GameHdr.h"
#include "ConsoleHdr.h"

int GameMain(int Arg, char* ID, int Rank, char* IP, char* EnemyID, int EnemyRank, int sock, char* Port, int RecvRoomNum){
	//char Port[10];
	ServSock=sock;
	RoomNum=RecvRoomNum;
	exit_flag=0;

	if(Arg==1){
		strcpy(User1, ID);
		User_Rank1=Rank;
		User_Rank2=EnemyRank;
	}
	if(Arg==2){
		strcpy(User2, ID);
		User_Rank2=Rank;
		strcpy(User1, EnemyID);
		User_Rank1=EnemyRank;
	}
	
	init_console();
    show_cursor(0);
	
	All_continue=1;
	
	system("cls");
	while(All_continue){
		if(Arg*100==RED){
			Guest(IP, Port);//Guest("192.168.233.128", "9190", "5560");//
		}
		else if(Arg*100==BLUE){
			Host(Port);
		}
	}
/*
	TerminateThread(hThreadRecv, dwThreadIDCon);
	WaitForSingleObject(hThreadRecv, INFINITE);
	TerminateThread(hThreadSend, dwThreadIDCon);
	WaitForSingleObject(hThreadSend, INFINITE);*/

	if(ImHost)
		Sleep(100);
	
	return 0;
}

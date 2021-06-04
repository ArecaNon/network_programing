#include "GameHdr.h"
#include "ConsoleHdr.h"

void ShowUser(){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY	);
	printf("�� : %d�� %s\n", User_Rank1, User1);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY	);
	printf("�� : %d�� %s\n", User_Rank2, User2);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

int InputString(char* Msg, int size){
	int i;
	
	Msg[0]='a';
	gotoxy(10, 22);
	for(i=0;i<size-1;i++){
		Action=Msg[i]=getch();
		
		if(Action!=8)
			printf("%c", Msg[i]);
		if(Action==8){
			if(i>0){
				Msg[i]='\0';
				printf("\b \b");;
				i=i-2;
			}
			else{
				i=-1;
			}
		}
		if(Action==13 && i!=0){
			break;
		}
		if(Action==13 && i==0){
			return 1;
		}
	}
	
	return 0;
}

void GameChatWindow(){
	//int i;
	
	gotoxy(1, 14);
	printf("������������������������������������������������������������������������������\n");
	printf("��%s : %s", UserBuf[5], GameEchoBuf[5]); gotoxy(77, 15); printf("��\n");
	printf("��%s : %s", UserBuf[4], GameEchoBuf[4]); gotoxy(77, 16); printf("��\n");
	printf("��%s : %s", UserBuf[3], GameEchoBuf[3]); gotoxy(77, 17); printf("��\n");
	printf("��%s : %s", UserBuf[2], GameEchoBuf[2]); gotoxy(77, 18); printf("��\n");
	printf("��%s : %s", UserBuf[1], GameEchoBuf[1]); gotoxy(77, 19); printf("��\n");
	printf("��%s : %s", UserBuf[0], GameEchoBuf[0]); gotoxy(77, 20); printf("��\n");
	printf("������������������������������������������������������������������������������\n");
	printf("���Է� : "); gotoxy(77, 22); printf("��\n");
	printf("������������������������������������������������������������������������������\n");
	if(myturn)
		printf("[Enter] Chat\t[F1] ������\t[F2] ���º�\t[F5] �׺�\t[F6] ����");
	if(!myturn)
		printf("[Enter] Chat");
}

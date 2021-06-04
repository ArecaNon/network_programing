#include "GameHdr.h"
#include "ConsoleHdr.h"

void InitGame(){
	int i;
	int j;

	for(i=0;i<6;i++){
		for(j=0;j<CHAT_BUF_SIZE;j++)
			GameEchoBuf[i][j]='\0';
	}
	
	Turn=BLUE;//ÃÊ°¡ ¼±¼ö¸¦ Àâ´Â´Ù.
	
	selected_x=-1;
	selected_y=-1;//¼¿·ºÆ® »óÅÂ ÃÊ±âÈ­
	
	cursor_x=5;
	cursor_y=5;//Ä¿¼­ ÃÊ±â À§Ä¡
	
	Move_Count=0;
	win_flag=YET_WIN;//±â¹° ÀÌµ¿ È½¼ö ¹× ½Â¸® ÇÃ·¡±× ÃÊ±âÈ­.
	
	system("cls");
	for(y=0;y<10;y++){
		for(x=0;x<9;x++){
			Board[x][y]=EMPTY;
		}
	}
	
	/* Red init */
	Board[0][9]=RED_CHARIOT;
	if(Guest_Board_Type==EKEK||Guest_Board_Type==EKKE){
		Board[1][9]=RED_ELEPHANT;
		Board[2][9]=RED_KNIGHT;
	}
	if(Guest_Board_Type==KEEK||Guest_Board_Type==KEKE){
		Board[1][9]=RED_KNIGHT;
		Board[2][9]=RED_ELEPHANT;
	}
	Board[3][9]=RED_SCHOLAR;
	Board[5][9]=RED_SCHOLAR;
	if(Guest_Board_Type==EKEK||Guest_Board_Type==KEEK){
		Board[6][9]=RED_ELEPHANT;
		Board[7][9]=RED_KNIGHT;
	}
	if(Guest_Board_Type==EKKE||Guest_Board_Type==KEKE){
		Board[6][9]=RED_KNIGHT;
		Board[7][9]=RED_ELEPHANT;
	}
	Board[8][9]=RED_CHARIOT;
	
	Board[4][8]=RED_KING;
	
	Board[1][7]=RED_CANNON;
	Board[7][7]=RED_CANNON;
	
	Board[0][6]=RED_PRIVATE;
	Board[2][6]=RED_PRIVATE;
	Board[4][6]=RED_PRIVATE;
	Board[6][6]=RED_PRIVATE;
	Board[8][6]=RED_PRIVATE;
	
	/* Blue init */
	Board[0][0]=BLUE_CHARIOT;
	if(Host_Board_Type==EKEK||Host_Board_Type==EKKE){
		Board[1][0]=BLUE_ELEPHANT;
		Board[2][0]=BLUE_KNIGHT;
	}
	if(Host_Board_Type==KEEK||Host_Board_Type==KEKE){
		Board[1][0]=BLUE_KNIGHT;
		Board[2][0]=BLUE_ELEPHANT;
	}
	Board[3][0]=BLUE_SCHOLAR;
	Board[5][0]=BLUE_SCHOLAR;
	if(Host_Board_Type==EKEK||Host_Board_Type==KEEK){
		Board[6][0]=BLUE_ELEPHANT;
		Board[7][0]=BLUE_KNIGHT;
	}
	if(Host_Board_Type==EKKE||Host_Board_Type==KEKE){
		Board[6][0]=BLUE_KNIGHT;
		Board[7][0]=BLUE_ELEPHANT;
	}
	Board[8][0]=BLUE_CHARIOT;
	
	Board[4][1]=BLUE_KING;
	
	Board[1][2]=BLUE_CANNON;
	Board[7][2]=BLUE_CANNON;
	
	Board[0][3]=BLUE_PRIVATE;
	Board[2][3]=BLUE_PRIVATE;
	Board[4][3]=BLUE_PRIVATE;
	Board[6][3]=BLUE_PRIVATE;
	Board[8][3]=BLUE_PRIVATE;
	
	for(y=0;y<10;y++){//±â¹° ÃÊ±â »óÅÂ ¹é¾÷.
		for(x=0;x<9;x++){
			undo_Board[x][y]=Board[x][y];
		}
	}
}

void ShowBoard(int x, int y){
	int i;
	gotoxy(22, 1); printf("¦®¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¯\n");
	for(i=0;i<10;i++){
		gotoxy(22, i+2); printf("¦­");	gotoxy(60, i+2); printf("¦­\n");
	}
	//printf("\n");
	gotoxy(22, i+2); printf("¦±¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦°\n");
	for(y=0;y<10;y++){
		gotoxy(x+15, y+2);
		for(x=0;x<9;x++){
			if(selected_x==x && selected_y==y)
				printf("[");
			else
				printf("-");
			
			switch(Board[x][y]){			
			case EMPTY : 
				if( (x==4 && y==1) || (x==4 && y==8) )
					printf("><");
				else
					printf("¦«");
				break;
				
				/*Red Han*/
			case RED_KING :
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				printf("ùÓ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			case RED_SCHOLAR :
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				printf("ÞÍ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);	
				break;
			case RED_CHARIOT : 
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				printf("ó³");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			case RED_CANNON :
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				printf("øÐ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			case RED_KNIGHT : 
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				printf("Ø©");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			case RED_ELEPHANT :
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				printf("ßÚ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			case RED_PRIVATE : 
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				printf("Ü²");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
				
				/*Blue Cho*/
			case BLUE_KING :
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				printf("õ¢");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			case BLUE_SCHOLAR :
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				printf("ÞÍ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			case BLUE_CHARIOT : 
				
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				printf("ó³");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			case BLUE_CANNON :
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				printf("øÐ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			case BLUE_KNIGHT : 
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				printf("Ø©");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			case BLUE_ELEPHANT :
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				printf("ßÚ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			case BLUE_PRIVATE : 
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				printf("ðï");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			}
			if(selected_x==x && selected_y==y)
				printf("]");
			else
				printf("-");
			if((x+1)%9==0)
				printf("\n");
		}
	}
}

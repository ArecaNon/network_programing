#include "ConsoleHdr.h"

/* 콘솔 초기화 */
void init_console(){
	DWORD fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT; 
	
	/* 입출력 장치 핸들 취득 */
	COUT = GetStdHandle(STD_OUTPUT_HANDLE); 
	CIN  = GetStdHandle(STD_INPUT_HANDLE);
	
	/* 화면 크기 정보 */
	GetConsoleScreenBufferInfo(COUT,&CSBI);
	WORLD = CSBI.srWindow;
}

/* 커서 화면 출력 여부 */
void show_cursor(WORD show){
	CONSOLE_CURSOR_INFO ConsoleCursor;
	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(COUT, &ConsoleCursor);
}

/* 해당 좌표로 이동 */
void gotoxy(int x, int y){
	COORD Cur={x-1,y-1};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

#include "ConsoleHdr.h"

HANDLE COUT;   // �ܼ� ��� �ڵ� 
HANDLE CIN;// �ܼ� �Է� �ڵ� 

CONSOLE_SCREEN_BUFFER_INFO CSBI;
SMALL_RECT WORLD;

/* �ܼ� �ʱ�ȭ */
void init_console(){
	DWORD fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT; 
	
	/* ����� ��ġ �ڵ� ��� */
	COUT = GetStdHandle(STD_OUTPUT_HANDLE); 
	CIN  = GetStdHandle(STD_INPUT_HANDLE);
	
	/* ȭ�� ũ�� ���� */
	GetConsoleScreenBufferInfo(COUT,&CSBI);
	WORLD = CSBI.srWindow;
}

/* Ŀ�� ��� ���� */
void show_cursor(WORD show){
	CONSOLE_CURSOR_INFO ConsoleCursor;
	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(COUT, &ConsoleCursor);
}

/* �ش� ��ǥ�� �̵� */
void gotoxy(int x, int y){
	COORD Cur={x-1,y-1};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

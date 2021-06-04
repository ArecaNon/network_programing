#include <windows.h>
#include <stdio.h>
#include <conio.h>

HANDLE COUT;   // 콘솔 출력 핸들 
HANDLE CIN;// 콘솔 입력 핸들 

CONSOLE_SCREEN_BUFFER_INFO CSBI;
SMALL_RECT WORLD;

void init_console();
void show_cursor(WORD show);
void gotoxy(int x, int y);

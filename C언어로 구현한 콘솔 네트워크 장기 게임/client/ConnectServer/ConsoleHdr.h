#include <windows.h>
#include <stdio.h>
#include <conio.h>

HANDLE COUT;   // �ܼ� ��� �ڵ� 
HANDLE CIN;// �ܼ� �Է� �ڵ� 

CONSOLE_SCREEN_BUFFER_INFO CSBI;
SMALL_RECT WORLD;

void init_console();
void show_cursor(WORD show);
void gotoxy(int x, int y);

#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <process.h> 
#define TRUE 1
#define FALSE 0
#define BUF_SIZE 103
#define MSG_SIZE 100
#define NAME_SIZE 20
#define MAX_USER 6
#define MAX_MAFIA 2

enum Role { Null, Mafia1, Mafia2, Citizen };

char GameEchoBuf[6][MSG_SIZE+1];		//채팅 내역을 6개까지 저장.
char UserBuf[6][MSG_SIZE + 1];
char msg[MSG_SIZE]; // 전송및 받는 메세지
int currtime, confTime, voteTime, personalRole, personalIdx, anotherMafiaIdx;// Int
int mafia, mafiaCanUseAbility, checkMafiaAbilityUse, checkConfTime, checkVoteTime, checkAlive, checkMorning, gameStart, gameEnd; // Bool
int deathIdx[MAX_USER];
CRITICAL_SECTION ChatCS;

void ErrorHandling(char *msg);
unsigned WINAPI SendMsg(void * arg);
unsigned WINAPI RecvMsg(void * arg);
unsigned WINAPI GameManager(void * arg);
void GameChatWindow();
void GameChatClear();
void gotoxy(int x, int y);
void setMessage(char *recvMsg, char* user);
char* manufactureSendMsg(char *sendMsg,char* result);
#pragma once

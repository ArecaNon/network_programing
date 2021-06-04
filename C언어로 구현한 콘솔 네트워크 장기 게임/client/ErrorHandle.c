#include "GameHdr.h"
#include "ConsoleHdr.h"

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

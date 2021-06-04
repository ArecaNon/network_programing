#include "ConnectHdr.h"
#include "ClntHdr.h"

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

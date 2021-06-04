#include "ConnectHdr.h"
#include "ServerHdr.h"

void HandleChating(int sock){
	char* colon=" : ";
	Full_Msg[0]='\0';
	//memset(Full_Msg, 0, 40);
	
	recv(sock, &IDSize, 4, 0);
	recv(sock, &ID, IDSize, 0);
	recv(sock, &Msg_size, 4, 0);
	recv(sock, &EchoString, Msg_size, 0);

	strcat(Full_Msg, ID);
	strcat(Full_Msg, colon);
	strcat(Full_Msg, EchoString);

	printf("%s", Full_Msg);
}

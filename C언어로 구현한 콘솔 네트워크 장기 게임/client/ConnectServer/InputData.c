#include "ConsoleHdr.h"
#include "ConnectHdr.h"
#include "ClntHdr.h"

int InputValue(char* Msg, int size){
	int i=0;
	size=size-1;

	Msg[0]='\0';

	while(1){
		Action=getch();

		if(Action==13 || Action==9){
			if(i>=1)
				break;
		}

		if(i<size && Action!=8 && Action!=13 && Action!=9 && Action!=32){
			Msg[i]=Action;
			printf("%c", Msg[i]);
			i++;
		}
		if(Action==8){
			if(!i){}
			else{
				Msg[i]='\0';
				printf("\b \b");
				i--;
			}
		}
	}
	Msg[i]='\0';
	
	return 0;
}

int InputPass(char* Msg, int size){
	int i=0;
	size=size-1;

	Msg[0]='\0';

	while(1){
		Action=getch();

		if(Action==13 || Action==9){
			if(i>=1)
				break;
		}

		if(i<size && Action!=8 && Action!=13 && Action!=9 && Action!=32){
			Msg[i]=Action;
			printf("*");
			i++;
		}
		if(Action==8){
			if(!i){}
			else{
				Msg[i]='\0';
				printf("\b \b");
				i--;
			}
		}
	}
	Msg[i]='\0';
	
	return 0;
}

int InputCivilNum(char* Msg, int size){
	int i=0;
	size=size-1;

	Msg[0]='\0';

	while(1){
		Action=getch();

		if(Action==13 || Action==9){
			if(i>=1)
				break;
		}

		if(i<size && Action!=8 && Action!=13 && Action!=9 && Action!=32 && !(Action<48) && !(Action>57)){
			Msg[i]=Action;
			printf("*");
			i++;
		}
		if(Action==8){
			if(!i){}
			else{
				Msg[i]='\0';
				printf("\b \b");
				i--;
			}
		}
	}
	Msg[i]='\0';
	
	return 0;
}

int InputMsg(char* Msg, int size){
	int i=0;
	size=size-1;

	Msg[0]='\0';

	while(1){
		Action=getch();

		if(Action==13 || Action==9){
			if(i>=1)
				break;
		}

		if(Action==27)
			return 1;

		if(i<size && Action!=8 && Action!=13 && Action!=9){
			Msg[i]=Action;
			printf("%c", Msg[i]);
			i++;
		}
		if(Action==8){
			if(!i){}
			else{
				Msg[i]='\0';
				printf("\b \b");
				i--;
			}
		}
	}
	Msg[i]='\0';
	
	return 0;
}

int InputNum(char* Msg, int size){
	int i=0;
	size=size-1;

	Msg[0]='\0';

	while(1){
		Action=getch();

		if(Action==13 || Action==9){
			if(i>=1)
				break;
		}

		if(i<size && Action!=8 && Action!=13 && Action!=9 && Action!=32 && !(Action<48) && !(Action>57)){
			Msg[i]=Action;
			printf("%c", Msg[i]);
			i++;
		}
		if(Action==8){
			if(!i){}
			else{
				Msg[i]='\0';
				printf("\b \b");
				i--;
			}
		}
	}
	Msg[i]='\0';
	
	return 0;
}
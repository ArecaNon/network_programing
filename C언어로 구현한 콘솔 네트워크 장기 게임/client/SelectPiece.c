#include "GameHdr.h"
#include "ConsoleHdr.h"

void PrintManual(int selected){
	gotoxy(22, 0);
	if(selected==EMPTY && selected_x==-1 && selected_y==-1){
		printf("\t\t\t�⹰�� �����Ϸ��� SpaceŰ�� �����ʽÿ�.");
	}
	else
		printf("\t\t\t������ �����Ϸ��� ESCŰ�� �����ʽÿ�.");	
}

int MovePiece(){
	int i;
	int CANNON_flag;//���� ���� �� �����ϴ� �⹰�� ����.
	switch(selected%1000){//�� ���ο����� �����ӿ� ���� ó�� �ʿ�.(Ư�� �������� �밢 �̵�)
	case 1 ://��
	case 2 ://��. �հ� ��� ������ �̵� �˰����� ���. 
		if(selected/10==BLUE){//�̵� ������ �� ������ �� ���η� ����.
			if( (moved_x<3 || moved_x>5) || moved_y>2 )
				return 0;
		}
		if(selected/10==RED){
			if( (moved_x<3 || moved_x>5) || moved_y<7 )
				return 0;
		}
		if( (moved_x==selected_x+1 && moved_y==selected_y) || (moved_x==selected_x && moved_y==selected_y+1) ||
			(moved_x==selected_x-1 && moved_y==selected_y) || (moved_x==selected_x && moved_y==selected_y-1) ){//�¿� �̵��� ���� ó��
			return 1;
		}
		/* �� ���ο����� �밢�� �̵� ó�� */
		if( ( (selected_x==3 || selected_x==5) && (selected_y==0 || selected_y==2) ) ||
			( (selected_x==3 || selected_x==5) && (selected_y==7 || selected_y==9) ) ){//�� �����ڸ������� �밢�� �̵��� ���� ó��
			if( (moved_x==4 && moved_y==1) || (moved_x==4 && moved_y==8) )
				return 1;
		}
		if( (selected_x==4 && selected_y==1) || (selected_x==4 && selected_y==8) ){//�� �߾ӿ����� �밢�� �̵��� ���� ó��
			return 1;
		}
		break;
	case 3 ://��
		if( selected_x==moved_x ){//���� �̵�.
			if(selected_y<moved_y){
				for(i=selected_y+1;i<moved_y;i++){
					if(Board[moved_x][i]!=EMPTY)
						return 0;
				}
			}else{
				for(i=selected_y-1;i>moved_y;i--){
					if(Board[moved_x][i]!=EMPTY)
						return 0;
				}
			}
			return 1;
		}
		if( selected_y==moved_y ){//���� �̵�.
			if(selected_x<moved_x){
				for(i=selected_x+1;i<moved_x;i++){
					if(Board[i][moved_y]!=EMPTY)
						return 0;
				}
			}else{
				for(i=selected_x-1;i>moved_x;i--){
					if(Board[i][moved_y]!=EMPTY)
						return 0;
				}
			}
			return 1;
		}
		/* �� ���ο����� �밢�� �̵� ó�� */
		if( ( (selected_x==3 || selected_x==5) && (selected_y==0 || selected_y==2) ) ||
			( (selected_x==3 || selected_x==5) && (selected_y==7 || selected_y==9) ) ){//�� �����ڸ������� �밢�� �̵��� ���� ó��
			if( (moved_x==4 && moved_y==1) || (moved_x==4 && moved_y==8) )
				return 1;
			if( ( (moved_x==3 || moved_x==5) && (moved_y==0 || moved_y==2) ) ){
				if(Board[4][1]==EMPTY)
					return 1;
			}
			if( ( (moved_x==3 || moved_x==5) && (moved_y==7 || moved_y==9) ) ){
				if(Board[4][8]==EMPTY)
					return 1;
			}
		}
		if( (selected_x==4 && selected_y==1) || (selected_x==4 && selected_y==8) ){//�� �߾ӿ����� �밢�� �̵��� ���� ó��
			if( ( (moved_x==3 || moved_x==5) && (moved_y==0 || moved_y==2) ) ||
				( (moved_x==3 || moved_x==5) && (moved_y==7 || moved_y==9) ) ){
				return 1;
			}
		}
		break;
	case 4 ://��
		CANNON_flag=0;
		if(Board[moved_x][moved_y]%1000==selected%1000)//���� ���� ���� �� ����.
			return 0;
		if( selected_x==moved_x ){//���� �̵�.
			if(selected_y<moved_y){
				for(i=selected_y+1;i<moved_y;i++){//���� �� �����ϴ� �⹰ ���¸� �˻�.
					if(Board[moved_x][i]!=EMPTY)//���� �� �����ϴ� �⹰�� ������ count.
						CANNON_flag++;
					if(Board[moved_x][i]%1000==selected%1000)//���� ���� �� �����ϸ� �̵� �Ұ���.
						return 0;
				}
				if(CANNON_flag==1){
					return 1;
				}
			}else{
				for(i=selected_y-1;i>moved_y;i--){
					if(Board[moved_x][i]!=EMPTY)
						CANNON_flag++;
					if(Board[moved_x][i]%1000==selected%1000)
						return 0;
				}
				if(CANNON_flag==1)
					return 1;
			}
		}
		else if( selected_y==moved_y ){//���� �̵�.
			if(selected_x<moved_x){
				for(i=selected_x+1;i<moved_x;i++){
					if(Board[i][moved_y]!=EMPTY)
						CANNON_flag++;
					if(Board[i][moved_y]%1000==selected%1000)
						return 0;
				}
				if(CANNON_flag==1)
					return 1;
			}else{
				for(i=selected_x-1;i>moved_x;i--){
					if(Board[i][moved_y]!=EMPTY)
						CANNON_flag++;
					if(Board[i][moved_y]%1000==selected%1000)
						return 0;
				}
				if(CANNON_flag==1)
					return 1;
			}
		}
		/* �� ���ο����� �밢�� �̵� ó�� */
		else if( ( (selected_x==3 || selected_x==5) && (selected_y==0 || selected_y==2) ) ||
			( (selected_x==3 || selected_x==5) && (selected_y==7 || selected_y==9) ) ){//�� �����ڸ������� �밢�� �̵��� ���� ó��
			if( ( (moved_x==3 || moved_x==5) && (moved_y==0 || moved_y==2) ) ){
				if(Board[4][1]!=EMPTY && Board[4][1]%1000!=selected%1000)
					return 1;
			}
			if( ( (moved_x==3 || moved_x==5) && (moved_y==7 || moved_y==9) ) ){
				if(Board[4][8]!=EMPTY && Board[4][1]%1000!=selected%1000)
					return 1;
			}
		}
		//���� ��� �� �߾ӿ����� �밢�� �̵��� ���� ó���� ���ʿ�.
		break;
	case 5 ://��
		if( ( Board[selected_x+1][selected_y]==EMPTY && moved_x==selected_x+2 && ( moved_y==selected_y+1 || moved_y==selected_y-1 ) ) || 
			( Board[selected_x-1][selected_y]==EMPTY && moved_x==selected_x-2 && ( moved_y==selected_y+1 || moved_y==selected_y-1 ) ) ||
			( Board[selected_x][selected_y+1]==EMPTY && moved_y==selected_y+2 && ( moved_x==selected_x+1 || moved_x==selected_x-1 ) ) ||
			( Board[selected_x][selected_y-1]==EMPTY && moved_y==selected_y-2 && ( moved_x==selected_x+1 || moved_x==selected_x-1 ) ) ) {
			return 1;
		}
		break;
	case 6 ://��
		if( ( Board[selected_x+1][selected_y]==EMPTY && Board[selected_x+2][selected_y+1]==EMPTY && moved_x==selected_x+3 && moved_y==selected_y+2 ) ||
			( Board[selected_x+1][selected_y]==EMPTY && Board[selected_x+2][selected_y-1]==EMPTY && moved_x==selected_x+3 && moved_y==selected_y-2 ) ||
			( Board[selected_x-1][selected_y]==EMPTY && Board[selected_x-2][selected_y+1]==EMPTY && moved_x==selected_x-3 && moved_y==selected_y+2 ) ||
			( Board[selected_x-1][selected_y]==EMPTY && Board[selected_x-2][selected_y-1]==EMPTY && moved_x==selected_x-3 && moved_y==selected_y+2 ) ||
			( Board[selected_x][selected_y+1]==EMPTY && Board[selected_x+1][selected_y+2]==EMPTY && moved_y==selected_y+3 && moved_x==selected_x+2 ) ||
			( Board[selected_x][selected_y+1]==EMPTY && Board[selected_x-1][selected_y+2]==EMPTY && moved_y==selected_y+3 && moved_x==selected_x-2 ) ||
			( Board[selected_x][selected_y-1]==EMPTY && Board[selected_x+1][selected_y-2]==EMPTY && moved_y==selected_y-3 && moved_x==selected_x+2 ) ||
			( Board[selected_x][selected_y-1]==EMPTY && Board[selected_x-1][selected_y-2]==EMPTY && moved_y==selected_y-3 && moved_x==selected_x-2 ) ) {
			return 1;
		}
		break;
	case 7 ://��
		if(selected==BLUE_PRIVATE){
			if( (moved_x==selected_x+1 && moved_y==selected_y) || (moved_x==selected_x-1 && moved_y==selected_y) ||
				(moved_x==selected_x && moved_y==selected_y+1) )
				return 1;
		}
		if(selected==RED_PRIVATE){
			if( (moved_x==selected_x+1 && moved_y==selected_y) || (moved_x==selected_x-1 && moved_y==selected_y) ||
				(moved_x==selected_x && moved_y==selected_y-1) )
				return 1;
		}
		/* �� ���ο����� �밢�� �̵� ó�� */
		if( (selected_x==3 || selected_x==5) && selected_y==7 ){//�� �����ڸ������� �밢�� �̵��� ���� ó��(��)
			if(moved_x==4 && moved_y==8)
				return 1;
		}
		if( (selected_x==3 || selected_x==5) && selected_y==2 ){//�� �����ڸ������� �밢�� �̵��� ���� ó��(��)
			if(moved_x==4 && moved_y==1)
				return 1;
		}
		if(selected_x==4 && selected_y==8){//�� �߾ӿ����� �밢�� �̵��� ���� ó��(��)
			if( (moved_y==selected_y+1) && (moved_x==3 || moved_x==4 || moved_x==5) )
				return 1;
		}
		if(selected_x==4 && selected_y==1){//�� �߾ӿ����� �밢�� �̵��� ���� ó��(��)
			if( (moved_y==selected_y-1) && (moved_x==3 || moved_x==4 || moved_x==5) )
				return 1;
		}
		break;
	}
	return 0;
}

#include "GameHdr.h"
#include "ConsoleHdr.h"

void PrintManual(int selected){
	gotoxy(22, 0);
	if(selected==EMPTY && selected_x==-1 && selected_y==-1){
		printf("\t\t\t기물을 선택하려면 Space키를 누르십시오.");
	}
	else
		printf("\t\t\t선택을 해제하려면 ESC키를 누르십시오.");	
}

int MovePiece(){
	int i;
	int CANNON_flag;//포의 진로 상에 존재하는 기물의 갯수.
	switch(selected%1000){//궁 내부에서의 움직임에 대한 처리 필요.(특정 지점에서 대각 이동)
	case 1 ://왕
	case 2 ://사. 왕과 사는 동일한 이동 알고리즘을 사용. 
		if(selected/10==BLUE){//이동 범위를 각 진영의 궁 내부로 제한.
			if( (moved_x<3 || moved_x>5) || moved_y>2 )
				return 0;
		}
		if(selected/10==RED){
			if( (moved_x<3 || moved_x>5) || moved_y<7 )
				return 0;
		}
		if( (moved_x==selected_x+1 && moved_y==selected_y) || (moved_x==selected_x && moved_y==selected_y+1) ||
			(moved_x==selected_x-1 && moved_y==selected_y) || (moved_x==selected_x && moved_y==selected_y-1) ){//좌우 이동에 대한 처리
			return 1;
		}
		/* 궁 내부에서의 대각선 이동 처리 */
		if( ( (selected_x==3 || selected_x==5) && (selected_y==0 || selected_y==2) ) ||
			( (selected_x==3 || selected_x==5) && (selected_y==7 || selected_y==9) ) ){//궁 가장자리에서의 대각선 이동에 대한 처리
			if( (moved_x==4 && moved_y==1) || (moved_x==4 && moved_y==8) )
				return 1;
		}
		if( (selected_x==4 && selected_y==1) || (selected_x==4 && selected_y==8) ){//궁 중앙에서의 대각선 이동에 대한 처리
			return 1;
		}
		break;
	case 3 ://차
		if( selected_x==moved_x ){//세로 이동.
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
		if( selected_y==moved_y ){//가로 이동.
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
		/* 궁 내부에서의 대각선 이동 처리 */
		if( ( (selected_x==3 || selected_x==5) && (selected_y==0 || selected_y==2) ) ||
			( (selected_x==3 || selected_x==5) && (selected_y==7 || selected_y==9) ) ){//궁 가장자리에서의 대각선 이동에 대한 처리
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
		if( (selected_x==4 && selected_y==1) || (selected_x==4 && selected_y==8) ){//궁 중앙에서의 대각선 이동에 대한 처리
			if( ( (moved_x==3 || moved_x==5) && (moved_y==0 || moved_y==2) ) ||
				( (moved_x==3 || moved_x==5) && (moved_y==7 || moved_y==9) ) ){
				return 1;
			}
		}
		break;
	case 4 ://포
		CANNON_flag=0;
		if(Board[moved_x][moved_y]%1000==selected%1000)//포는 포를 잡을 수 없음.
			return 0;
		if( selected_x==moved_x ){//세로 이동.
			if(selected_y<moved_y){
				for(i=selected_y+1;i<moved_y;i++){//진로 상에 존재하는 기물 상태를 검사.
					if(Board[moved_x][i]!=EMPTY)//진로 상에 존재하는 기물의 갯수를 count.
						CANNON_flag++;
					if(Board[moved_x][i]%1000==selected%1000)//포가 진로 상에 존재하면 이동 불가능.
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
		else if( selected_y==moved_y ){//가로 이동.
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
		/* 궁 내부에서의 대각선 이동 처리 */
		else if( ( (selected_x==3 || selected_x==5) && (selected_y==0 || selected_y==2) ) ||
			( (selected_x==3 || selected_x==5) && (selected_y==7 || selected_y==9) ) ){//궁 가장자리에서의 대각선 이동에 대한 처리
			if( ( (moved_x==3 || moved_x==5) && (moved_y==0 || moved_y==2) ) ){
				if(Board[4][1]!=EMPTY && Board[4][1]%1000!=selected%1000)
					return 1;
			}
			if( ( (moved_x==3 || moved_x==5) && (moved_y==7 || moved_y==9) ) ){
				if(Board[4][8]!=EMPTY && Board[4][1]%1000!=selected%1000)
					return 1;
			}
		}
		//포의 경우 궁 중앙에서의 대각선 이동에 대한 처리는 불필요.
		break;
	case 5 ://마
		if( ( Board[selected_x+1][selected_y]==EMPTY && moved_x==selected_x+2 && ( moved_y==selected_y+1 || moved_y==selected_y-1 ) ) || 
			( Board[selected_x-1][selected_y]==EMPTY && moved_x==selected_x-2 && ( moved_y==selected_y+1 || moved_y==selected_y-1 ) ) ||
			( Board[selected_x][selected_y+1]==EMPTY && moved_y==selected_y+2 && ( moved_x==selected_x+1 || moved_x==selected_x-1 ) ) ||
			( Board[selected_x][selected_y-1]==EMPTY && moved_y==selected_y-2 && ( moved_x==selected_x+1 || moved_x==selected_x-1 ) ) ) {
			return 1;
		}
		break;
	case 6 ://상
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
	case 7 ://졸
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
		/* 궁 내부에서의 대각선 이동 처리 */
		if( (selected_x==3 || selected_x==5) && selected_y==7 ){//궁 가장자리에서의 대각선 이동에 대한 처리(졸)
			if(moved_x==4 && moved_y==8)
				return 1;
		}
		if( (selected_x==3 || selected_x==5) && selected_y==2 ){//궁 가장자리에서의 대각선 이동에 대한 처리(병)
			if(moved_x==4 && moved_y==1)
				return 1;
		}
		if(selected_x==4 && selected_y==8){//궁 중앙에서의 대각선 이동에 대한 처리(졸)
			if( (moved_y==selected_y+1) && (moved_x==3 || moved_x==4 || moved_x==5) )
				return 1;
		}
		if(selected_x==4 && selected_y==1){//궁 중앙에서의 대각선 이동에 대한 처리(병)
			if( (moved_y==selected_y-1) && (moved_x==3 || moved_x==4 || moved_x==5) )
				return 1;
		}
		break;
	}
	return 0;
}

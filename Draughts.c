#include "Draughts.h"
//Define macros:
#define IS_WHITE(x) (((x) == (WHITE_K))||((x) == (WHITE_M)))
#define IS_KING(x) (((x) == (WHITE_K))||((x) == (BLACK_K)))
int MINIMAX_DEPTH = 1;
int GAME = 0;
int SET = 0;
int PLAYER_TURN = 0;
int PLAYER_WHITE = 1;
char board[BOARD_SIZE][BOARD_SIZE];

// Board init & print methods
void print_line(){
	int i;
	printf("  |");
	for (i = 1; i < BOARD_SIZE*4; i++){
		printf("-");
	}
	printf("|\n");
}


void print_board(char board[BOARD_SIZE][BOARD_SIZE]) {
	int i,j;
	print_line();
	for (j = BOARD_SIZE-1; j >= 0 ; j--)
	{
		printf((j < 9 ? " %d" : "%d"), j+1);
		for (i = 0; i < BOARD_SIZE; i++){
			printf("| %c ", board[i][j]);
		}
		printf("|\n");
		print_line();
	}
	printf("   ");
	for (j = 0; j < BOARD_SIZE; j++){
		printf(" %c  ", (char)('a' + j));
	}
	printf("\n");
}


void init_board(char board[BOARD_SIZE][BOARD_SIZE]){
	int i,j;
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			if ((i + j) % 2 == 0){
				if (j <= 3){
					board[i][j] = WHITE_M;
				}
				else if (j >= 6){
					board[i][j] = BLACK_M;
				}
				else{
					board[i][j] = EMPTY;
				}
			}
			else{
				board[i][j] = EMPTY;
			}
		}
	}
}


// setters
void set_minmax_depth(int x){
  if (x > 0 && x < 7){
    MINIMAX_DEPTH = x;
  } else {
    printf("%s", WRONG_MINIMAX_DEPTH);
  }
}


void set_user_color(char *s){
  if (str.cmp(s, "black")== 0){
    PLAYER_WHITE = 0;
 }
}


void quit(void){
  exit(0);
}


void clear(){
  for (int i = 0; i < BOARD_SIZE; i++){
    for ( int j = 0; j < BOARD_SIZE; j++){
      board[i][j] = EMPTY;
    }
  }
}


void remove_location(location l){
  if (is_legal_location(l)){
    board[l.row][l.column] = EMPTY;
  }else{
    printf("%s", WRONG_POSITION);
  }
}


void set_location(location l, char disk){
  if (is_legal_location(l)){
    board[l.row][l.column] = disk;
  }else{
    printf("%s", WRONG_POSITION);
  }
}
  
  
// infrastructure

typedef struct //define a location 
{
  int column;	//0-9 (a-j)
  int row;	//0-9 (1-10)
} location;


typedef struct //define a disc 
{
  location address;
  move *next;	//(WHITE_M, WHITE_K, BLACK_M, BLACK_K)
} move;


int is_legal_location(location l){
  if ( location.row > BOARD_SIZE - 1  ||location.row < 0 ||\ 
       location.column > BOARD_SIZE - 1  || location.column < 0){
    return 0;	//location is outside the board
  }
  if ( !(location.row + location.column)%2 ){ 
    return 0;	//this location is a white square
  }
  return 1;	//this location is legal (black and on the board)
}


//tests :
int test1(void){
	char temp_board[BOARD_SIZE][BOARD_SIZE];
	init_board(temp_board);
	print_board(temp_board);
	print_message(WRONG_MINIMAX_DEPTH);
	perror_message("TEST");
	return 1;
}

int main(){
	char *input;
	printf("%s",WELCOME_TO_DRAUGHTS)
	// start setting part
	printf("%s",ENTER_SETTINGS)
	test1();
	return 0;
}
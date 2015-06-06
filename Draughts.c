#include "Draughts.h"
#include <string.h>
#include <stdlib.h>


// infrastructure

typedef struct  //define a location 
{
  int column;	//0-9 (a-j)
  int row;	//0-9 (1-10)
} location;


typedef struct 
{
  location address;
  struct move *next;	
} move;


// func headers

int is_legal_location(location l);
void clear();
void set_minmax_depth(int x);
void set_user_color(char *s);
void quit(void);
void remove_location(location l);
void set_location(location l, int white, int man);
int check_settings(void);
location str_to_location(char* locus);

//Define macros:
#define IS_WHITE(x) (((x) == (WHITE_K))||((x) == (WHITE_M)))
#define IS_KING(x) (((x) == (WHITE_K))||((x) == (BLACK_K)))
int MINIMAX_DEPTH = 1;
int GAME = 0;
int SETINGS = 1;
int WHITE_TURN = 1; // 0 - black, 1- white
int PLAYER_WHITE = 1;
char board[BOARD_SIZE][BOARD_SIZE];
int NUM_WHITE_M = 0;
int NUM_WHITE_K = 0;
int NUM_BLACK_M = 0;
int NUM_BLACK_K = 0;

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
  if (strcmp(s, "black")== 0){
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


int is_legal_location(location l){
  if ( l.row > BOARD_SIZE - 1  ||l.row < 0 ||\
       l.column > BOARD_SIZE - 1  || l.column < 0){
    return 0;	//location is outside the board
  }
  if ( ((l.row + l.column)%2) ){ 
    return 0;	//this location is a white square
  }
  return 1;	//this location is legal (black and on the board)
}


void set_location(location l, int white, int man){
	char disc = WHITE_M;
	if ( !(white) ) {
		if ( !(man) ){
			disc = BLACK_K;
		}
		else {
			disc = BLACK_M;
		}
	} 
	else {
		if ( !(man) ){
			disc = WHITE_K;
		}
	}
  if (is_legal_location(l)){
    board[l.row][l.column] = disc;
  }else{
    printf("%s", WRONG_POSITION);
  }
}
  
  
void quit_allcation_error(void){
	perror("Failed to allocate memory.");
    exit(1);
}

// change phase - from now on settings is disable, game is enable.   
void start_game(void){
	SETINGS = 0;
	GAME = 1;
}

/** 
 * reads the input from user by dynamically allocationg
 * memory. On the go minimizes spaces to a max of one
 * space between words. Output is the whole string submitted by the user.
 */ 
char* read_input(void){
	char* input;
	int i = 0;
	if ((input = malloc(1*sizeof(char))) == NULL){
		quit_allcation_error();
	}
	int c = getchar();
	while ( c == ' ' ){
		c = getchar();
	}	
	while(1){

		if(c == '\n'){
			break;
		}
		if( c == ' '){
			input[i] = c;
			i += 1;
			if ((input = realloc(input, (i+1)*sizeof(char))) == NULL){
				quit_allcation_error();
			}
			while( c == ' ' ){
				c = getchar();
			}
		}
		input[i] = c;
		i += 1;
		if ((input = realloc(input, (i+1)*sizeof(char))) == NULL){
			quit_allcation_error();
		}
		c = getchar();
	}
	input[i] = '\0';
	return input;
}

void parse_input_settings(char* input){

	char *words; // will be a copy of the input.
	char *word;
	if((words = malloc((strlen(input)+1)*sizeof(*words))) == NULL){
		quit_allcation_error();
	}
	strcpy(words, input); 
	words[strlen(input)] = '\0';
	word = strtok(words, " ");

	if ( strcmp(word, "minimax_depth") == 0){
		word = strtok(NULL, " ");
		int x = atoi(word); //unsigned?????
		set_minmax_depth(x);		
	}
	else if ( strcmp(word, "user_color") == 0){
		word = strtok(NULL, " ");
		set_user_color(word);
	}
	else if ( strcmp(input, "clear") == 0){
		clear();
	}
	else if ( strcmp(word, "rm") == 0){
		word = strtok(NULL, " ");
		location l = str_to_location(word); //????
		remove_location(l);	
	}
	else if ( strcmp(word, "set") == 0){
		int is_white = 1;
		int is_man = 1;
		
		word = strtok(NULL, " ");
		location l = str_to_location(word); //????
		word = strtok(NULL, " ");
		if ( strcmp(word, "black") == 0 ){
			is_white = 0;
		}
		word = strtok(NULL, " ");
		if ( strcmp(word, "k") == 0 ){
			is_man = 0;
		}
		set_location( l, is_white, is_man );	
	}
	else if ( strcmp(input, "print") == 0){
		print_board(board);	
	}
	else if ( strcmp(input, "start") == 0){
		if ( check_settings() ){
			start_game();
		}	
	}
	else {
		print_message( ILLEGAL_COMMAND );
	}
	free(words);
}

int check_settings(void){
	for ( int i = 0; i < BOARD_SIZE; i++ ){
		for ( int j = 0; j < BOARD_SIZE; j++ ){
			switch (board[i][j]){
				case WHITE_M:
					NUM_WHITE_M++;
					break;
				case WHITE_K:
					NUM_WHITE_K++;
					break;					
				case BLACK_M:
					NUM_BLACK_M++;
					break;
				case BLACK_K:
					NUM_BLACK_K++;
					break;
			} 
		}
	}
	if ( NUM_WHITE_M+NUM_WHITE_K > 20 || NUM_BLACK_M+NUM_BLACK_K > 20 ){
		print_message(WROND_BOARD_INITIALIZATION);
		return 0;
	}
	if ( NUM_WHITE_M+NUM_WHITE_K == 0 || NUM_BLACK_M+NUM_BLACK_K == 0){
		print_message(WROND_BOARD_INITIALIZATION);
		return 0;
	}
	return 1;
}

location str_to_location(char* locus){
	location l;
	int row = locus[1] - 'a';
	int column = 0;
	for ( int i = 3 ; locus[i] != '>' ; i++ ){
		column = 10 * column + (locus[i] - '0');
	} 
	l.row = row;
	l.column = column-1;
	return l;
}

void declare_winner(void){
	WHITE_TURN = (WHITE_TURN + 1)%2;
	char* winner = WHITE_TURN ? "white" : "black";
	printf("%s player wins!\n", winner);
}

//tests :
int test1(void){
	char temp_board[BOARD_SIZE][BOARD_SIZE];
	init_board(temp_board);
	print_board(temp_board);
	print_message(WRONG_MINIMAX_DEPTH);
	perror_message("TEST 1");
	return 1;
}
int test2(void){
	GAME = 0 ;
	SETINGS = 0;
	return 1;
}
int test3(void){ //print settings
	printf("MINIMAX_DEPTH = %d\n",MINIMAX_DEPTH);
	printf("PLAYER_WHITE = %d\n",PLAYER_WHITE);
	printf("WHITE_TURN = %d\n",WHITE_TURN);
	return 1;
}
int main(){
	char *input;
	init_board(board);
	printf("%s",WELCOME_TO_DRAUGHTS);
	// start setting part
	printf("%s",ENTER_SETTINGS);
	while(1){
		input = read_input();
		if(strcmp(input,"\0") == 0){ // verify input isn't empty.
			free(input);
			continue;
		}
		if (strcmp(input, "quit") == 0){
			free(input);
			quit();
		}
		else if(SETINGS){
			parse_input_settings(input);
		}
		else if(GAME){
			test3();
			test2();
			
			WHITE_TURN = (WHITE_TURN + 1)%2;
		}
		if(!GAME && !SETINGS){
			declare_winner();
			free(input);
			quit();
		}
		free(input);
	}
	test1();
	return 0;
}
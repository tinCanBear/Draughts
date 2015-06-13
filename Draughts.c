#include "Draughts.h"
#include <string.h>
#include <stdlib.h>


// infrastructure

struct location_st   //define a location 
{
  int column;	//0-9 (a-j)
  int row;	//0-9 (1-10)
  struct location_st *next; //linked list implementation
};

typedef struct location_st location;

struct move_st   //define a move
{
  int eats; //how good is this move 
  location *step; //will be a linked list of steps within this move	
  struct move_st *next;	//linked list of moves
};

typedef struct move_st move;

// functions headers

void set_minmax_depth(int x);
void set_user_color(char *s);
void quit(void);
void clear(void);
void remove_location(location l);
int is_legal_location(location l);
void set_location(location l, int white, int man);
void quit_allcation_error(void);
void start_game(void);
char* read_input(void);
void parse_input_settings(char* input);
int check_settings(void);
location str_to_location(char* locus);
void declare_winner(void);
move* get_disc_moves(char a_board[BOARD_SIZE][BOARD_SIZE], location *l);
move* get_moves(char a_board[BOARD_SIZE][BOARD_SIZE], int is_white_turn); // all the available moves
move *get_eating_moves(int row ,int column, char a_board[BOARD_SIZE][BOARD_SIZE]);
move* link_moves(move *moves, move *disc_moves);
int same_color(char a, char b);
void print_move(move *m);
void print_all_moves(move *m);
int is_legal_move(move* m);// will check if move is legal

//??? to be implemented:
void parse_input_game(char* input); //similar to that of the settings phase.
 
//???

//Define macros:

#define IS_WHITE(x) (((x) == (WHITE_K))||((x) == (WHITE_M)))
#define IS_KING(x) (((x) == (WHITE_K))||((x) == (BLACK_K)))
int MINIMAX_DEPTH = 1;
int GAME = 0;
int SETTINGS = 1;
int WHITE_TURN = 1; // 0 - black, 1- white
int PLAYER_WHITE = 1;
char board[BOARD_SIZE][BOARD_SIZE];
int NUM_WHITE_M = 0;
int NUM_WHITE_K = 0;
int NUM_BLACK_M = 0;
int NUM_BLACK_K = 0;
int DEBUGGING = 0;  
/* 
if (DEBUGGING){
	printf("\n");
	print_all_moves(moves);
	fflush(stdout);
}
 */
// 																		*************** General methods ****************

void quit(void){
  exit(0);
}
void quit_allcation_error(void){
	perror("Failed to allocate memory.");
    exit(1);
}

/** create location. */
location *create_location(int row, int column){
	location *l;
	if ((l = (location *)malloc(1*sizeof(location))) == NULL){
		quit_allcation_error();
	}
 
	l->row = row;
	l->column = column;
	return l;
}

/** create move. if row==(-1) will create empty move.*/
move *create_move(int row, int column){
	move *m;
	if ((m = (move *)malloc(1*sizeof(move))) == NULL){
		quit_allcation_error();
	}
	m->eats = 0;
	m->next = NULL;
	if (row == -1){
		m->step = NULL; 
	}else{
		location *l = create_location(row , column);
		m->step = l;
	}
	return m;
}

/** frees location (as a link list). */
/* void free_location(location *l){
	if ( l != NULL ){
		free_location(l->next);
		free(l);
	}
} */

/** frees move (as a link list). */
/* void free_move(move *m){
	if ( m != NULL ){
		free_move(m->next);
		free_location(m->step);
		free(m);
	}
} 
 */
//																		 *************** Board init & print methods ****************

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



// 																		************************* setters ***************************

void set_minmax_depth(int x){ //set the minimax_depth
  if (x > 0 && x < 7){
    MINIMAX_DEPTH = x;
  } else { // x is illegal 
    printf("%s", WRONG_MINIMAX_DEPTH);
  }
}


void set_user_color(char *s){ // default is white 
  if (strcmp(s, "black")== 0){ 
    PLAYER_WHITE = 0;
 }
}

//																	 	******************* settings phase functions ***********************


/** clears the board (all EMPTY) */
void clear(void){ 
  for (int i = 0; i < BOARD_SIZE; i++){
    for ( int j = 0; j < BOARD_SIZE; j++){
      board[i][j] = EMPTY;
    }
  }
}

/** sets the l location on the board to EMPTY if l is legal */
void remove_location(location l){ 
  if (is_legal_location(l)){
    board[l.column][l.row] = EMPTY;
  }else{
    printf("%s", WRONG_POSITION);
  }
}

/** checks if l is valid location in the board
  * returns '1' if l is a legal location
  * else, returns '0'. */
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

/** sets a disc on the board at location 'l' if 'l' is legal.
  * 'white' is '1' if the disc should be white (otherwise, black) 
  * 'man' is '1' if the disc should be a man (otherwise, a king)
  * if 'l' is an illegal location prints an appropriate message. */
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
    board[l.column][l.row] = disc;
  }else{
    printf("%s", WRONG_POSITION);
  }
}
  
/** change phase - from now on settings is disable, 
  * game is enable. */  
void start_game(void){
	SETTINGS = 0;
	GAME = 1;
}

/** reads the input from user by dynamically allocating
  * memory. On the go minimizes spaces to a max of one
  * space between words. 
  * Output is the whole string (char *) submitted by the user. */
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

/** receives a char* 'input'.
  * if input contains valid, setting phase, instructions;
  * executes them. 
  * otherwise, prints ILLEGAL_COMMAND. */
void parse_input_settings(char* input){

	char *words; // will be a copy of the input.
	char *word;
	if((words = malloc((strlen(input)+1)*sizeof(*words))) == NULL){
		quit_allcation_error();
	}
	strcpy(words, input); 
	words[strlen(input)] = '\0';
	word = strtok(words, " ");
	// check if 'word' matches a legal (settings) command: 
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
		location l = str_to_location(word);
		remove_location(l);	
	}
	else if ( strcmp(word, "set") == 0){
		int is_white = 1;
		int is_man = 1;
		
		word = strtok(NULL, " ");
		location l = str_to_location(word); 
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
	else { // 'word' doesn't match any legal (settings) command 
		print_message( ILLEGAL_COMMAND );
	}
	free(words);
}

/** returns '1' if the board set-up is legal.
  * '0' otherwise.
  * counts the numbers of each instrument. */
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
	if ( NUM_WHITE_M+NUM_WHITE_K > 20 || NUM_BLACK_M+NUM_BLACK_K > 20 ){ // too many black/white discs on board
		print_message(WROND_BOARD_INITIALIZATION);
		NUM_WHITE_K = 0;
		NUM_BLACK_K = 0;
		NUM_WHITE_M = 0;
		NUM_BLACK_M = 0;
		return 0;
	}
	if ( NUM_WHITE_M+NUM_WHITE_K == 0 || NUM_BLACK_M+NUM_BLACK_K == 0){ // too little (none) black/white discs on board
		print_message(WROND_BOARD_INITIALIZATION);
		NUM_WHITE_K = 0;
		NUM_BLACK_K = 0;
		NUM_WHITE_M = 0;
		NUM_BLACK_M = 0;
		return 0;
	}
	return 1;
}

/** converts locus into a location. */
location str_to_location(char* locus){
	location l;
	int column = locus[1] - 'a'; // convert the row to integer
	int row = 0;
	for ( int i = 3 ; locus[i] != '>' ; i++ ){ // calculate column value (could be '10')
		row = 10 * row + (locus[i] - '0');
	} 
	l.column = column; 
	l.row = row-1; // normalize to range starting from 0
	return l;
}

/** decides who the winner is (by who played last),
  * print out the winning message. */
void declare_winner(void){
	WHITE_TURN = (WHITE_TURN + 1)%2; // assumes the turn has already been updated, so adds 1 to get beck the last player.
	char* winner = WHITE_TURN ? "white" : "black";
	printf("%s player wins!\n", winner);
}

// 																	******************* game phase functions ***********************
move* get_moves(char a_board[BOARD_SIZE][BOARD_SIZE], int is_white_turn){
	
	// initialize a move linked list:
	move *moves = NULL; 
	location *l;
	// find out who's turn it is
	char man = is_white_turn ? WHITE_M : BLACK_M;
	char king = is_white_turn ? WHITE_K : BLACK_K;

	for ( int i = 0; i < BOARD_SIZE; i++ ){
		for ( int j = 0; j < BOARD_SIZE; j++ ){
			if (  a_board[j][i] == man || a_board[j][i] == king ){
				move *disc_moves;
				l = create_location(i,j);
				disc_moves = get_disc_moves(a_board, l); // get the moves for the discs in this location
				if (DEBUGGING){
					print_all_moves(disc_moves);
					fflush(stdout);
				}
				moves = link_moves(moves,disc_moves); // concatenate the linked lists
				
			}
		}
	}
	return moves;
}

move* get_disc_moves(char a_board[BOARD_SIZE][BOARD_SIZE], location *l){
	char disc = a_board[l->column][l->row];
	move *moves = NULL;
	if (DEBUGGING){
		printf("******* get_disc_moves: <%c,%d> ********** \n",l->column+'a',l->row+1);
		printf("is %c white disk: %d\n",a_board[l->column][l->row] ,IS_WHITE(disc));
		printf("is cond0: %d\n",(l->row - 1 >= 0 && l->column + 1 <= BOARD_SIZE-1));
		printf("is cond1: %d\n",!(same_color(a_board[l->column + 1][l->row - 1], disc)));
		printf("is cond2: %d\n",a_board[l->column + 1][l->row - 1] == EMPTY);
		printf("is cond3: %d\n",(!IS_WHITE(disc)));
		print_board(a_board);
		fflush(stdout);
	}
	
		// A king 
	if ( disc == BLACK_K || disc == WHITE_K ){
		int k = -1;
		int n = 1;
		while ( l->row + k >= 0 && l->column + n <= BOARD_SIZE-1 ){ // location is inside the board (upper right)
			if ( !(same_color(a_board[l->column + n][l->row  + k], disc)) ){ // enemy or empty 
				if ( a_board[l->column + n][l->row + k] == EMPTY ){
					moves = link_moves(moves, create_move( l->row + k, l->column + n ));					
				}
				else { // enemy!!!
					if ( (l->row + k-1  >= 0 && l->column + n+1 <= BOARD_SIZE-1)  && a_board[l->column + n+1][l->row + k-1] == EMPTY){ // EATING! :O
						char a_board_copy[BOARD_SIZE][BOARD_SIZE];
						memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
						a_board_copy[l->column + n+1][l->row + k-1] = disc; // disc to new spot*
						a_board_copy[l->column][l->row] = EMPTY; // remove disc from previous location
						a_board_copy[l->column + n][l->row + k] = EMPTY; // remove enemy disc 
						moves = link_moves(moves, get_eating_moves(l->row + k-1, l->column + n+1, a_board_copy )); // 
						
					}	
					break;
				}
			}
			k--;
			n++;
		}
		k = -1;
		n = -1;
		while ( l->row + k >= 0 && l->column + n >= 0 ){ // location is inside the board (upper left)
			if ( !(same_color(a_board[l->column + n][l->row  + k], disc)) ){ // enemy or empty 
				if ( a_board[l->column + n][l->row + k] == EMPTY ){
					moves = link_moves(moves, create_move(l->row + k, l->column + n));					
				}
				else { // enemy!!!
					if ( (l->row + k-1  >= 0 && l->column + n-1 >= 0)  && a_board[l->column + n-1][l->row + k-1] == EMPTY){ // EATING! :O
						char a_board_copy[BOARD_SIZE][BOARD_SIZE];
						memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
						a_board_copy[l->column + n-1][l->row + k-1] = disc; // disc to new spot*
						a_board_copy[l->column][l->row] = EMPTY; // remove disc from previous location
						a_board_copy[l->column + n][l->row + k] = EMPTY; // remove enemy disc 
						moves = link_moves(moves, get_eating_moves(l->row + k-1, l->column + n-1, a_board_copy )); // 
						
					}	
					break;
				}
			}
			k--;
			n--;
		}
		k = 1;
		n = 1;
		while ( l->row + k <= BOARD_SIZE-1 && l->column + n <= BOARD_SIZE-1 ){ // location is inside the board (lower right)
			if ( !(same_color(a_board[l->column + n][l->row  + k], disc)) ){ // enemy or empty 
				if ( a_board[l->column + n][l->row + k] == EMPTY ){
					moves = link_moves(moves, create_move( l->row + k, l->column + n));					
				}
				else { // enemy!!!
					if ( (l->row + k+1  >= 0 && l->column + n+1 <= BOARD_SIZE-1)  && a_board[l->column + n+1][l->row + k+1] == EMPTY){ // EATING! :O
						char a_board_copy[BOARD_SIZE][BOARD_SIZE];
						memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
						a_board_copy[l->column + n+1][l->row + k+1] = disc; // disc to new spot*
						a_board_copy[l->column][l->row] = EMPTY; // remove disc from previous location
						a_board_copy[l->column + n][l->row + k] = EMPTY; // remove enemy disc 
						moves = link_moves(moves, get_eating_moves(l->row + k+1, l->column + n+1, a_board_copy )); // 
						
					}	
					break;
				}
			}
			k++;
			n++;
		}
		k = 1;
		n = -1;
		while ( l->row + k <= BOARD_SIZE-1 && l->column + n >= 0 ){ // location is inside the board (lower left)
			if ( !(same_color(a_board[l->column + n][l->row  + k], disc)) ){ // enemy or empty 
				if ( a_board[l->column + n][l->row + k] == EMPTY ){
					moves = link_moves(moves, create_move( l->row + k, l->column + n ));	
				}
				else { // enemy!!!
					if ( (l->row + k+1 <= BOARD_SIZE-1 && l->column + n-1 >= 0)  && a_board[l->column + n-1][l->row + k+1] == EMPTY){ // EATING! :O
						char a_board_copy[BOARD_SIZE][BOARD_SIZE];
						memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
						a_board_copy[l->column + n-1][l->row + k+1] = disc; // disc to new spot*
						a_board_copy[l->column][l->row] = EMPTY; // remove disc from previous location
						a_board_copy[l->column + n][l->row + k] = EMPTY; // remove enemy disc 
						moves = link_moves(moves, get_eating_moves(l->row + k+1, l->column + n-1, a_board_copy )); // 
					}
					break;
				}
			}
			k++;
			n--;
		}

	}
		// A man
	else {	
		if ( (l->row - 1 >= 0 && l->column + 1 <= BOARD_SIZE-1) ){ // location is inside the board (upper right)
			if ( !(same_color(a_board[l->column + 1][l->row - 1], disc)) ){ // enemy or empty 
				if ( a_board[l->column + 1][l->row - 1] == EMPTY ){
					if ( (!IS_WHITE(disc)) ){
						moves = link_moves(moves, create_move( l->row -1, l->column + 1));					
					}
					
				}
				else { // enemy!!!
					if ( (l->row - 2 >= 0 && l->column + 2 <= BOARD_SIZE-1)  && a_board[l->column + 2][l->row - 2] == EMPTY){ // EATING! :O
						char a_board_copy[BOARD_SIZE][BOARD_SIZE];
						memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
						a_board_copy[l->column+2][l->row-2] = disc; // disc to new spot*
						a_board_copy[l->column][l->row] = EMPTY; // remove disc from previous location
						a_board_copy[l->column + 1][l->row - 1] = EMPTY; // remove enemy disc 
						moves = link_moves(moves, get_eating_moves(l->row - 2, l->column  + 2, a_board_copy )); // 
					}	
				}
			}
		}
		if ( (l->row - 1 >= 0 && l->column - 1  >= 0) ){ // location is inside the board (upper left)
			if ( !(same_color(a_board[l->column - 1][l->row - 1], disc)) ){ // enemy or empty 
				if ( a_board[l->column - 1][l->row - 1] == EMPTY ){
					if ( !(IS_WHITE(disc)) ){
						moves = link_moves(moves, create_move( l->row -1, l->column - 1));					
					}					
				}
				else { // enemy!!!
					if ( (l->row - 2 >= 0 && l->column - 2 >= 0 )  && a_board[l->column - 2][l->row - 2] == EMPTY){ // EATING! :O
						char a_board_copy[BOARD_SIZE][BOARD_SIZE];
						memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
						a_board_copy[l->column-2][l->row-2] = disc; // disc to new spot*
						a_board_copy[l->column][l->row] = EMPTY; // remove disc from previous location
						a_board_copy[l->column - 1][l->row - 1] = EMPTY; // remove enemy disc 
						moves = link_moves(moves, get_eating_moves(l->row - 2, l->column  - 2, a_board_copy )); // 
					}	
				}
			}
		}
		if ( (l->row + 1 <= BOARD_SIZE-1 && l->column + 1 <= BOARD_SIZE-1) ){ // location is inside the board (lower right)
			if ( !(same_color(a_board[l->column + 1][l->row + 1], disc)) ){ // enemy or empty 
				if ( a_board[l->column + 1][l->row + 1] == EMPTY ){
					if ( IS_WHITE(disc) ){
						moves = link_moves(moves, create_move( l->row + 1, l->column + 1));	
					}
				}
				else { // enemy!!!
					if ( (l->row + 2 <= BOARD_SIZE-1 && l->column + 2 <= BOARD_SIZE-1)  && a_board[l->column + 2][l->row + 2] == EMPTY){ // EATING! :O
						char a_board_copy[BOARD_SIZE][BOARD_SIZE];
						memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
						a_board_copy[l->column+2][l->row+2] = disc; // disc to new spot*
						a_board_copy[l->column][l->row] = EMPTY; // remove disc from previous location
						a_board_copy[l->column + 1][l->row + 1] = EMPTY; // remove enemy disc 
						moves = link_moves(moves, get_eating_moves(l->row + 2, l->column  + 2, a_board_copy )); // 
					}	
				}
			}
		}
		if ( (l->row + 1 <= BOARD_SIZE-1 && l->column - 1 >= 0) ){ // location is inside the board (lower left)
			if ( !(same_color(a_board[l->column - 1][l->row + 1], disc)) ){ // enemy or empty 
				if ( a_board[l->column - 1][l->row + 1] == EMPTY ){
					if ( IS_WHITE(disc) ){
						moves = link_moves(moves, create_move( l->row + 1, l->column - 1));
					}				
				}
				else { // enemy!!!
					if ( (l->row + 2 <= BOARD_SIZE-1 && l->column - 2 >= 0)  && a_board[l->column - 2][l->row + 2] == EMPTY){ // EATING! :O
						char a_board_copy[BOARD_SIZE][BOARD_SIZE];
						memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
						a_board_copy[l->column-2][l->row+2] = disc; // disc to new spot*
						a_board_copy[l->column][l->row] = EMPTY; // remove disc from previous location
						a_board_copy[l->column - 1][l->row + 1] = EMPTY; // remove enemy disc 
						moves = link_moves(moves, get_eating_moves(l->row + 2, l->column  - 2, a_board_copy )); // 
					}	
				}
			}
		}
	}	
	move * temp_move = moves;
	location *current_loc;
	while(temp_move != NULL){
		current_loc = create_location(l->row,l->column);
		temp_move->eats++;
		current_loc->next = temp_move->step;
		temp_move->step = current_loc;
		temp_move = temp_move->next;
	}
	if (DEBUGGING){
		print_all_moves(moves);
		fflush(stdout);
	}
	//free_location(l);
	return moves;
}

move *get_eating_moves(int row ,int column, char a_board[BOARD_SIZE][BOARD_SIZE]){
	if (DEBUGGING){
		printf("******* get_eating_moves: <%c,%d> ********** \n",column+'a',row+1);
		print_board(a_board);
		fflush(stdout);
	}
	move *moves = NULL;
	move *temp_move = NULL;
	location *current_loc;
	location *l = create_location(row,column);
	char disc = a_board[l->column][l->row];
	if ( (l->row - 1 >= 0 && l->column + 1 <= BOARD_SIZE-1) ){ // location is inside the board (upper right)
		if ( !(same_color(a_board[l->column + 1][l->row - 1], disc)) ){ // enemy or empty 
			if ( !(a_board[l->column + 1][l->row - 1] == EMPTY) ){ // enemy!!!
				if ( (l->row - 2 >= 0 && l->column + 2 <= BOARD_SIZE-1)  && a_board[l->column + 2][l->row - 2] == EMPTY){ // EATING! :O
					char a_board_copy[BOARD_SIZE][BOARD_SIZE];
					memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
					a_board_copy[l->column+2][l->row-2] = disc; // disc to new spot*
					a_board_copy[l->column][l->row] = EMPTY; // remove disc from previous location
					a_board_copy[l->column + 1][l->row - 1] = EMPTY; // remove enemy disc 
					moves = link_moves(moves, get_eating_moves(l->row - 2, l->column  + 2, a_board_copy )); // 
				}	
			}
		}
	}
	if ( (l->row - 1 >= 0 && l->column - 1  >= 0) ){ // location is inside the board (upper left)
		if ( !(same_color(a_board[l->column - 1][l->row - 1], disc)) ){ // enemy or empty 
			if ( !(a_board[l->column - 1][l->row - 1] == EMPTY )){ // enemy!!!
				if ( (l->row - 2 >= 0 && l->column - 2 >= 0 )  && a_board[l->column - 2][l->row - 2] == EMPTY){ // EATING! :O
					char a_board_copy[BOARD_SIZE][BOARD_SIZE];
					memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
					a_board_copy[l->column-2][l->row-2] = disc; // disc to new spot*
					a_board_copy[l->column][l->row] = EMPTY; // remove disc from previous location
					a_board_copy[l->column - 1][l->row - 1] = EMPTY; // remove enemy disc 
					moves = link_moves(moves, get_eating_moves(l->row - 2, l->column  - 2, a_board_copy )); // 
				}	
			}
		}
	}
	if ( (l->row + 1 <= BOARD_SIZE-1 && l->column + 1 <= BOARD_SIZE-1) ){ // location is inside the board (lower right)
		if ( !(same_color(a_board[l->column + 1][l->row + 1], disc)) ){ // enemy or empty 
			if ( !(a_board[l->column + 1][l->row + 1] == EMPTY) ){ // enemy!!!
				if ( (l->row + 2 <= BOARD_SIZE-1 && l->column + 2 <= BOARD_SIZE-1)  && a_board[l->column + 2][l->row + 2] == EMPTY){ // EATING! :O
					char a_board_copy[BOARD_SIZE][BOARD_SIZE];
					memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
					a_board_copy[l->column+2][l->row+2] = disc; // disc to new spot*
					a_board_copy[l->column][l->row] = EMPTY; // remove disc from previous location
					a_board_copy[l->column + 1][l->row + 1] = EMPTY; // remove enemy disc 
					moves = link_moves(moves, get_eating_moves(l->row + 2, l->column  + 2, a_board_copy )); // 
				}	
			}
		}
	}
	if ( (l->row + 1 <= BOARD_SIZE-1 && l->column - 1 >= 0) ){ // location is inside the board (lower left)
		if ( !(same_color(a_board[l->column - 1][l->row + 1], disc)) ){ // enemy or empty 
			if ( !(a_board[l->column - 1][l->row + 1] == EMPTY )){ // enemy!!!
				if ( (l->row + 2 <= BOARD_SIZE-1 && l->column - 2 >= 0)  && a_board[l->column - 2][l->row + 2] == EMPTY){ // EATING! :O
					char a_board_copy[BOARD_SIZE][BOARD_SIZE];
					memcpy(a_board_copy, a_board, sizeof(a_board_copy)); //copy the board
					a_board_copy[l->column-2][l->row+2] = disc; // disc to new spot*
					a_board_copy[l->column][l->row] = EMPTY; // remove disc from previous location
					a_board_copy[l->column - 1][l->row + 1] = EMPTY; // remove enemy disc 
					moves = link_moves(moves, get_eating_moves(l->row + 2, l->column  - 2, a_board_copy )); // 
				}	
			}
		}
	}
	//free_location(l);
	if (moves == NULL){ // end of the recursion.
		moves = create_move(-1, -1);// create empty move.
	}
	temp_move = moves;
	while(temp_move != NULL){
		current_loc = create_location(row,column);
		temp_move->eats++;
		current_loc->next = temp_move->step;
		temp_move->step = current_loc;
		temp_move = temp_move->next;
	}
	return moves;
	//
	
	
	
	
	
}
int same_color(char a, char b){
	if ((a == EMPTY && b != EMPTY) || (b == EMPTY && a != EMPTY) ){
		return 0;
	}
	if ( IS_WHITE(a) == IS_WHITE(b) ){
		return 1;
	}
	return 0;
}


move* link_moves(move *moves, move *disc_moves){
	move *temp = disc_moves;
	if ( moves == NULL  && disc_moves == NULL ){
		return NULL;
	}
	
	if ( moves == NULL ){ // these are the first moves added
		moves = disc_moves;
		return moves;
	}
	
	if ( disc_moves == NULL ){ // Nothing new to add
		return moves;
	}
	
	if ( temp->eats < moves->eats ) { // moves are better, dump new moves 
		//free_move(disc_moves); 
		return moves;
	}
	
	if ( temp->eats > moves->eats ) { // disc_moves is better, dump previous moves 
		//free_move(moves); 
		moves = disc_moves; //???pointer still exists?....
		return moves;
	}
	// else: (sizes are equal)
	while ( temp->next != NULL ){ // reach last move of disc_moves
		temp = temp->next;
	}
	temp->next = moves;
	moves = disc_moves;
	return moves;
}

void print_move(move *m){
	location *temp = m->step;
	printf("<%c,%d> to ",(temp->column+'a'),temp->row+1);
	while ( (temp = temp->next) != NULL){
		printf("<%c,%d>",(temp->column+'a'),temp->row+1);
	}
	printf("\n");
}

void print_all_moves(move *m){
	while(m != NULL){
		print_move(m);
		m = m->next;
	}
}

int is_legal_move(move* m){
	location *temp_loc1; //  pointer for the pattern moves
	location *temp_loc2; // pointer for the user move
	move *moves = get_moves(board, WHITE_TURN ); //get pattern moves (legal)
	move * temp_moves = moves;
	while(temp_moves != NULL){ // check if one of the pattern moves is the same as the user move
		temp_loc1 = temp_moves->step;
		temp_loc2 = m->step;
		while(temp_loc1 != NULL  && temp_loc2 != NULL){
			if (temp_loc1->row != temp_loc2->row || temp_loc1->column != temp_loc2->column ){
				break;
			}
			temp_loc1 = temp_loc1->next;
			temp_loc2 = temp_loc2->next;
		}
		if (temp_loc1 == NULL  && temp_loc2 == NULL){ // the user move is the same as a legal move
			return 1;
		}
		temp_moves = temp_moves->next;
	}
	//free_move(moves);
	return 0;
}
/* void parse_input_game(char* input){
	//??? something??
	char *words; // will be a copy of the input.
	char *word;
	if((words = malloc((strlen(input)+1)*sizeof(*words))) == NULL){
		quit_allcation_error();
	}
	strcpy(words, input); 
	words[strlen(input)] = '\0';
	word = strtok(words, " ");
	// check if 'word' matches a legal (game) command: 
	if ( strcmp(word, "move") == 0){
		//???
	}
	else if ( strcmp(word, "get_moves") == 0){
		//???
	}
	else { // 'word' doesn't match any legal (game) command 
		print_message( ILLEGAL_COMMAND );
	}
	free(words);
}
 */



// 																		*********************** tests ************************
int test1(void){
	char temp_board[BOARD_SIZE][BOARD_SIZE];
	init_board(temp_board);
	print_board(temp_board);
	print_message(WRONG_MINIMAX_DEPTH);
	perror_message("TEST 1");
	return 1;
}
int test2(void){ // END THE GAME
	printf("*************** test2 ******************\n");
	printf("END THE GAME\n");
	GAME = 0 ;
	SETTINGS = 0;
	return 1;
}
int test3(void){ //print settings
	printf("*************** test3 ******************\n");
	printf("print settings\n");
	printf("MINIMAX_DEPTH = %d\n",MINIMAX_DEPTH);
	printf("PLAYER_WHITE = %d\n",PLAYER_WHITE);
	printf("WHITE_TURN = %d\n",WHITE_TURN);
	return 1;
}
int test4(void){ //print all first turn moves + board
	printf("*************** test4 ******************\n");
	print_board(board);
	printf("WHITE_TURN = %d\n",0);
	move *moves = get_moves(board, 0);
	printf("got moves! \n printing moves: \n");
	fflush(stdout);
	print_all_moves(moves);
	//free_move(moves);
	return 1;
}
int test5(void){ //print all moves(black) + board
	printf("*************** test5 ******************\n");
	char temp_board[BOARD_SIZE][BOARD_SIZE];
	for (int i = 0; i < BOARD_SIZE; i++){
		for ( int j = 0; j < BOARD_SIZE; j++){
			temp_board[i][j] = EMPTY;
		}
	}
	temp_board[6][0] = BLACK_K;
	temp_board[9][1] = BLACK_M;
	temp_board[8][2] = WHITE_M;
	temp_board[8][4] = WHITE_M;
	temp_board[8][6] = WHITE_M;
	temp_board[8][8] = WHITE_M;
	temp_board[6][6] = WHITE_M;
	temp_board[4][4] = WHITE_M;
	print_board(temp_board);
	printf("WHITE_TURN = %d\n",0);
	move *moves = get_moves(temp_board, 0);
	printf("got moves! \n printing moves: \n");
	fflush(stdout);
	print_all_moves(moves);
	//free_move(moves);
	return 1;
}
int test6(void){ //print all moves(white) + board
	printf("*************** test6 ******************\n");
	char temp_board[BOARD_SIZE][BOARD_SIZE];
	temp_board[6][6] = WHITE_K;
	temp_board[4][4] = BLACK_M;
	temp_board[3][3] = BLACK_M;
	print_board(temp_board);
	printf("WHITE_TURN = %d\n",1);
	move *moves = get_moves(temp_board, 1);
	printf("got moves! \n printing moves: \n");
	fflush(stdout);
	print_all_moves(moves);
	//free_move(moves);
	return 1;
}
/** the main function. */
int main(){
	char *input;
	init_board(board);
	printf("%s",WELCOME_TO_DRAUGHTS);

	// start the settings phase
	printf("%s",ENTER_SETTINGS);
	while(1){
		input = read_input();///read input here???? maybe inside the phases? because if it's the computer's turn...
		if(strcmp(input,"\0") == 0){ // verify input isn't empty.
			free(input);
			continue;
		}
		if (strcmp(input, "quit") == 0){
			free(input);
			quit();
		}
		else if(SETTINGS){ // settings phase
			parse_input_settings(input);
		}
		else if(GAME){ // game time
/* 			if ( (PLAYER_WHITE && WHITE_TURN) || (!PLAYER_WHITE && !WHITE_TURN) ){ //user's turn???maybe make different logic. 
				printf("%s", ENTER_YOUR_MOVE);//should be here???
				parse_input_game(input);
			}
			else { // computer's turn
				// do something???
			} */
			test3();
			test4();
			test2();
			
			WHITE_TURN = (WHITE_TURN + 1)%2;
		}
		if(!GAME && !SETTINGS){  // game's over
			declare_winner();
			free(input);
			quit();
		}
		free(input);
	}
	test1();
	return 0;
}
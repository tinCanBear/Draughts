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

struct move_st   //define a move
{
  int eats; //how good is this move 
  location *step; //will be a linked list of steps within this move	
  struct move_st *next;	//linked list of moves
};

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
int DEBUGGING2 = 0;

// 																		*************** General methods ****************

/** create location. */
location *create_location(int row, int column){
	location *l;
	if ((l = (location *)malloc(1*sizeof(location))) == NULL){
		quit_allcation_error();
	}
 
	l->row = row;
	l->column = column;
	l->next = NULL;
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
void free_location(location *l){
	if ( l != NULL ){
		free_location(l->next);
		free(l);
	}
}

/** frees move (as a link list). */
void free_move(move *m){
	if ( m != NULL ){
		free_move(m->next);
		free_location(m->step);
		free(m);
	}
}

void quit(void){
  exit(0);
}

void quit_allcation_error(void){
	perror("Failed to allocate memory.");
    exit(1);
} 

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
			if ( c == '\n' ){
				break;
			}
		}
		input[i] = c;
		i += 1;
		if ((input = realloc(input, (i+1)*sizeof(char))) == NULL){
			quit_allcation_error();
		}
		c = getchar();
	}
	if(i!=0 && input[i-1] == ' ' ){
		if ((input = realloc(input, (i)*sizeof(char))) == NULL){
			quit_allcation_error();
		}
		i--;
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
		int x = atoi(word);
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
	char* winner = WHITE_TURN ? "White" : "Black";
	printf("%s player wins!\n", winner);
}

// 																	******************* game phase functions ***********************

/** returns a linked list of moves comprising
 * all available legal moves of the given player. */
move* get_moves(char a_board[BOARD_SIZE][BOARD_SIZE], int is_white_turn){
	if (DEBUGGING){
		printf("in get_moves\n");
		print_board(board);
		fflush(stdout);
	}
	// initialize a move linked list:
	move *moves = NULL; 
	location *l;
	move *disc_moves;
	// find out who's turn it is
	char man = is_white_turn ? WHITE_M : BLACK_M;
	char king = is_white_turn ? WHITE_K : BLACK_K;

	for ( int i = 0; i < BOARD_SIZE; i++ ){
		for ( int j = 0; j < BOARD_SIZE; j++ ){
			if (  a_board[j][i] == man || a_board[j][i] == king ){
				disc_moves = NULL;
				l = create_location(i,j);
				disc_moves = get_disc_moves(a_board, l); // get the moves for the discs in this location
				if (DEBUGGING){
					print_all_moves(disc_moves);
					fflush(stdout);
				}
				moves = link_moves(moves,disc_moves); // concatenate the linked lists
				free_location(l);
				l = NULL;
				
			}
		}
	}
	return moves;
}

/**  returns a move pointer which is the entire move for the disc in location l. */
move* get_disc_moves(char a_board[BOARD_SIZE][BOARD_SIZE], location *l){
	char disc = a_board[l->column][l->row];
	move *moves = NULL;
	if (DEBUGGING){
		printf("******* get_disc_moves: <%c,%d> ********** \n",l->column+'a',l->row+1);
		printf("is %c white disk: %d\n",a_board[l->column][l->row] ,IS_WHITE(disc));
		printf("is cond0: %d\n",(l->row - 1 >= 0 && l->column - 1 <= BOARD_SIZE-1));
		printf("is cond1: %d\n",!(same_color(a_board[l->column - 1][l->row - 1], disc)));
		printf("is cond2: %d\n",a_board[l->column - 1][l->row - 1] == EMPTY);
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
				if (DEBUGGING){
					printf("in get_disc_moves in the correct 'if' (-1,-1)(upper left)\n");
					fflush(stdout);
				}		
				if ( a_board[l->column - 1][l->row - 1] == EMPTY ){
					if ( !(IS_WHITE(disc)) ){
						if (DEBUGGING){
							printf("in get_disc_moves in the correct 'if' (-1,-1)(upper left)(just before link)\n");
							fflush(stdout);
						}
						moves = link_moves(moves, create_move( l->row -1, l->column - 1));
						if (DEBUGGING){
							printf("in get_disc_moves in the correct 'if' (-1,-1)(upper left)(just AFTER link)\n");
							fflush(stdout);
						}								
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
	move *temp_move = moves;
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

/** a recursive function. returns a move which begins after the first eat.*/
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
	
	if ( (disc == BLACK_M && row == 0) || (disc == WHITE_M && row == BOARD_SIZE-1) ){ // end of the recursion( men--->king ).
		moves = create_move(-1, -1);
		moves->step = l;
		moves->eats++;
		return moves;
	}
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
	if (moves == NULL){ // end of the recursion, can't eat.
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
	free_location(l);
	return moves;	
}

/** returns '1' if a is the same colour as b (EMPTY doesn't count),
  * '0' otherwise. */
int same_color(char a, char b){
	if ((a == EMPTY && b != EMPTY) || (b == EMPTY && a != EMPTY) ){
		return 0;
	}
	if ( IS_WHITE(a) == IS_WHITE(b) ){
		return 1;
	}
	return 0;
}

/** links disc_moves to moves.
  * (linked list).*/
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
		free_move(disc_moves); 
		return moves;
	}
	
	if ( temp->eats > moves->eats ) { // disc_moves is better, dump previous moves 
		free_move(moves); 
		moves = disc_moves; 
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

/** prints a single move;
  * all associated locations.*/ 
void print_move(move *m){
	location *temp = m->step;
	printf("<%c,%d> to ",(temp->column+'a'),temp->row+1);
	while ( (temp = temp->next) != NULL){
		printf("<%c,%d>",(temp->column+'a'),temp->row+1);
	}
	printf("\n");
}

/** prints all  moves within m.*/ 
void print_all_moves(move *m){
	while(m != NULL){
		print_move(m);
		m = m->next;
	}
}

/** return '1' of m is a legal move,
  * '0' otherwise. */
int is_legal_move(move* m){
	location *temp_loc1; //  pointer for the pattern moves
	location *temp_loc2; // pointer for the user move
	move *moves = get_moves(board, WHITE_TURN); //get pattern moves (legal)
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
			free_move(moves);
			return 1;
		}
		temp_moves = temp_moves->next;
	}
	free_move(moves);
	return 0;
}

/** perfomes all the steps in move *m on a_board.*/ 
void do_move(char a_board[BOARD_SIZE][BOARD_SIZE],move* m){
	char disc = a_board[m->step->column][m->step->row];
	location *from = m->step;
	location *to = m->step->next;
	a_board[from->column][from->row] = EMPTY;
	if ( m->eats == 0 ){
		a_board[to->column][to->row] = disc;
		from = to;
		to = from->next;
	}else{
		int up;
		int right;
		while(to != NULL){
			up = to->row > from->row ? 1 : -1;
			right = to->column > from->column ? 1 : -1;
			a_board[from->column][from->row] = EMPTY; //delete previous location 
			a_board[to->column - right][to->row - up] = EMPTY; //delete eaten disc
			a_board[to->column][to->row] = disc;
			from = to;
			to = from->next;
		}
	}
	if ( (disc == BLACK_M) && (from->row == 0) ){
		a_board[from->column][from->row] = BLACK_K;
	}
	else if( (disc == WHITE_M) && (from->row == 9) ){
		a_board[from->column][from->row] = WHITE_K;
	}
}

/** returns an int '1' or '0'. '1' if user's input is wrong
  * in some way that requires another input receiving
  * and turn skipping. the function receives the input char*
  * and assumes "game phase" type commands. */
int parse_input_game(char* input){
	
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
		free(words);
		move *user_move = create_move(-1,-1); // create empty move.
		location *l = NULL;
		location *last_l = NULL; 
		int column;
		int row;
		for(int i = 0; input[i] != '\0'; i++){
			if ( input[i]== '<' ){
				column = input[i+1] - 'a'; // convert the row to integer
				row = 0;
				for ( int j = i+3 ; input[j] != '>' ; j++ ){ // calculate column value (could be '10')
					row = 10 * row + (input[j] - '0');
					i = j; 
				} 
				l = create_location(row-1,column);
				if ( user_move->step == NULL ){
					user_move->step = l;
					last_l = l;
				}
				else{
				last_l->next = l;
				last_l = last_l->next;
				}
				if (l != NULL && !is_legal_location(*l)){
					print_message(WRONG_POSITION);
					free_move(user_move);
					return 1;
				}
			}

		}
	
		if (user_move->step != NULL){
			char disc = board[user_move->step->column][user_move->step->row];
			if (PLAYER_WHITE && ((disc == BLACK_K) || (disc == BLACK_M) || (disc == EMPTY))){
				print_message(NO_DICS);
				free_move(user_move);
				return 1;
			}
			if ((!PLAYER_WHITE) && ((disc == WHITE_K) || (disc == WHITE_M) || (disc == EMPTY))){
				print_message(NO_DICS);
				free_move(user_move);
				return 1;
			}			
		}
		if (is_legal_move(user_move)){
			int next_col = user_move->step->next->column;
			int next_row = user_move->step->next->row;
			int this_col = user_move->step->column;
			int this_row = user_move->step->row;
			int up;
			int right;
			char disc = board[this_col][this_row];
			up = next_row > this_row ? 1 : -1;
			right = next_col > this_col ? 1 : -1;
			if ( board[next_col - right][next_row - up] != disc ){
				user_move->eats = 1;
			}
			do_move(board, user_move);
			free_move(user_move);
			return 0;
		}
		else{ // move is illegal
			print_message(ILLEGAL_MOVE);
			print_move(user_move); 
			free_move(user_move);
			return 1;			
		}
	}
	else if ( strcmp(word, "get_moves") == 0){
		free(words);
		move *user_moves = get_moves(board, PLAYER_WHITE);
		print_all_moves(user_moves);
		free_move(user_moves);
		return 1;
	}
	else { // 'word' doesn't match any legal (game) command 
		print_message( ILLEGAL_COMMAND );
		free(words);
		return 1;
	}
}
 
 /** returns an int which is the score of the current board with
   * accordance to the white_player (which player to calculate the score). */
int score_board(char a_board[BOARD_SIZE][BOARD_SIZE],int white_player){
	int black_score = 0;
	int white_score = 0;
	int white_can_move = 0;
	int black_can_move = 0;
	for( int i = 0 ; i < BOARD_SIZE ; i++ ){
		for( int j = 0 ; j < BOARD_SIZE ; j++ ){
			if ( a_board[i][j] != EMPTY ){
				if(a_board[i][j] == BLACK_M){
					black_score++;
				}
				if(a_board[i][j] == BLACK_K){
					black_score = black_score + 3;
				}
				if(a_board[i][j] == WHITE_M){
					white_score++;
				}
				if(a_board[i][j] == WHITE_K){
					white_score = white_score + 3;
				}
				if( i+1 < BOARD_SIZE && j+1 < BOARD_SIZE ){
					if(a_board[i+1][j+1] == EMPTY ){
						if(IS_WHITE(a_board[i][j])){
							white_can_move = 1;
						}
						if( a_board[i][j] == BLACK_K){
							black_can_move = 1;
						}
					}
					else{
						if( i+2 < BOARD_SIZE && j+2 < BOARD_SIZE ){
							if(a_board[i+2][j+2] == EMPTY){
								if(IS_WHITE(a_board[i][j]) && IS_BLACK(a_board[i+1][j+1])){
									white_can_move = 1;
								}
								if(IS_WHITE(a_board[i+1][j+1]) && IS_BLACK(a_board[i][j])){
									black_can_move = 1;
								}
							}
						}
					}
				}
				if ( i-1 >= 0 && j+1 < BOARD_SIZE ){
					if(a_board[i-1][j+1] == EMPTY ){
						if(IS_WHITE(a_board[i][j])){
							white_can_move = 1;
						}
						if( a_board[i][j] == BLACK_K){
							black_can_move = 1;
						}
					}
					else{
						if( i-2 >= 0 && j+2 < BOARD_SIZE ){
							if(a_board[i-2][j+2] == EMPTY){
								if(IS_WHITE(a_board[i][j]) && IS_BLACK(a_board[i-1][j+1])){
									white_can_move = 1;
								}
								if(IS_WHITE(a_board[i-1][j+1]) && IS_BLACK(a_board[i][j])){
									black_can_move = 1;
								}
							}
						}
					}
				}
				if ( i+1 < BOARD_SIZE && j-1 >= 0 ){
					if(a_board[i+1][j-1] == EMPTY ){
						if(IS_BLACK(a_board[i][j])){
							black_can_move = 1;
						}
						if( a_board[i][j] == WHITE_K){
							white_can_move = 1;
						}
					}
					else{
						if( i+2 < BOARD_SIZE && j-2 >= 0 ){
							if(a_board[i+2][j-2] == EMPTY){
								if(IS_WHITE(a_board[i][j]) && IS_BLACK(a_board[i+1][j-1])){
									white_can_move = 1;
								}
								if(IS_WHITE(a_board[i+1][j-1]) && IS_BLACK(a_board[i][j])){
									black_can_move = 1;
								}
							}
						}
					}
				}
				if ( i-1 >= 0 && j-1 >= 0 ){
					if(a_board[i-1][j-1] == EMPTY ){
						if(IS_BLACK(a_board[i][j])){
							black_can_move = 1;
						}
						if( a_board[i][j] == WHITE_K){
							white_can_move = 1;
						}
					}
					else{
						if( i-2 >= 0 && j-2 >= 0 ){
							if(a_board[i-2][j-2] == EMPTY){
								if(IS_WHITE(a_board[i][j]) && IS_BLACK(a_board[i-1][j-1])){
									white_can_move = 1;
								}
								if(IS_WHITE(a_board[i-1][j-1]) && IS_BLACK(a_board[i][j])){
									black_can_move = 1;
								}
							}
						}
					}
				}
			}
		}
	}
	if( ((!white_can_move) && white_player) || ((!black_can_move) && (!white_player)) ){
		return -100;
	}
	if( ((!black_can_move) && white_player) || ((!white_can_move) && (!white_player)) ) {
		return 100;
	}
	if( white_player ){
		return white_score - black_score;
	}
	return black_score - white_score;
}

/** returns an int representing the score for for the previously
  * executed move (that caused the a_board configuration),
  * according to the scoring function and depending on the depth.
  * this is a recursive function. */
int minmax(char a_board[BOARD_SIZE][BOARD_SIZE], int maxi, int depth){
	
	int next_color = maxi ? PLAYER_WHITE : !PLAYER_WHITE;
	move *moves = NULL;
	move *temp = NULL;
	int mini_score = 101;
	int maxi_score = -101;
	// recursion ends
	if (depth == MINIMAX_DEPTH){
		return score_board(a_board, !PLAYER_WHITE);
	}
	moves = get_moves(a_board, next_color);	//get the moves of the other player
	if ( moves == NULL ){
		if(maxi){
			return 100;
		}
		else{
			return -100;
		}
	}
	temp = moves;	//initialize temp
	while ( temp != NULL ){
		int temp_score;
		char board_copy[BOARD_SIZE][BOARD_SIZE];
		memcpy(board_copy, a_board, sizeof(board_copy));
		do_move(board_copy, temp); // now the board copy is updated 
		temp_score = minmax(board_copy, !maxi, depth+1);
		
		if ( temp_score > maxi_score ){
			maxi_score = temp_score;
		}
		if ( temp_score < mini_score ){
			mini_score = temp_score;
		}
		temp = temp->next;
	}
	free_move(moves);
	if ( !maxi ){
		return maxi_score;
	}
	else{
		return mini_score;
	}
}

/** returns the max score move for the computer's turn.
  * uses calls to minmax (recursive function). */
move *get_move_minmax(void){
	move *moves = get_moves(board, WHITE_TURN); // our options.
	move *max_move = NULL;
	move *prev_max_move = NULL;
	move *temp = moves;
	move *prev_temp = NULL;
	int max_score = -101;
	int current_score;
	char board_copy[BOARD_SIZE][BOARD_SIZE];

	while ( temp != NULL ){
		
		// copy the board
		memcpy(board_copy, board, sizeof(board_copy));
		
		//do move 
		do_move(board_copy, temp); // now the board copy is updated 

		// get score for the move, using minmax
		current_score = minmax(board_copy, 1, 1);
		
		// update max if necessary 
		if ( current_score > max_score ){
			max_score = current_score;
			max_move = temp;
			prev_max_move = prev_temp;
		}
		if(DEBUGGING2){
				printf("check move :\n");
				print_move(temp);
				print_board(board_copy);
				printf("current_score: %d\n",current_score );
				printf("max_score: %d\n",max_score );
				fflush(stdout);
		}
		prev_temp = temp;
		temp = temp->next;
	}
	if (prev_max_move != NULL){
		prev_max_move->next = NULL;
		free_move(moves);
	}
	return max_move;
}

/** the main function. */
int main(){
	int repeat = 0;
	char *input;
	move *comp_move;
	init_board(board);
	printf("%s",WELCOME_TO_DRAUGHTS);

	// start the settings phase
	printf("%s",ENTER_SETTINGS);
	while(1){
		if ( (PLAYER_WHITE && WHITE_TURN) || (!PLAYER_WHITE && !WHITE_TURN)  || (SETTINGS)){
			if (GAME){
				print_message(ENTER_YOUR_MOVE);
			}
			input = read_input();
			if(strcmp(input,"\0") == 0){ // verify input isn't empty.
				free(input);
				continue;
			}
			if (strcmp(input, "quit") == 0){
				free(input);
				quit();
			}
		}
		if(SETTINGS){ // settings phase
			parse_input_settings(input);
		}
		else if(GAME){ // game time
 			if ( (PLAYER_WHITE && WHITE_TURN) || (!PLAYER_WHITE && !WHITE_TURN) ){ //user's turn 
				if ( (repeat = parse_input_game(input)) ){ //'1' if user's input was wrong in some way, need another input
					WHITE_TURN = (WHITE_TURN + 1)%2;
				} 
			}
			else { // computer's turn
				comp_move = get_move_minmax();
				do_move(board, comp_move);
				printf("Computer: move ");
				print_move(comp_move);
				free_move(comp_move);
			}  
			WHITE_TURN = (WHITE_TURN + 1)%2;
			if(!repeat){
				print_board(board);
			}
			if( score_board(board,WHITE_TURN) == -100){ // game's over
				GAME = 0;
			}
		}
		if(!GAME && !SETTINGS){  // end the game
			declare_winner();
			free(input);
			quit();
		}
		if ( (PLAYER_WHITE && WHITE_TURN) || (!PLAYER_WHITE && !WHITE_TURN)  || (SETTINGS)){
			free(input);
		}
	}
	return 0;
}
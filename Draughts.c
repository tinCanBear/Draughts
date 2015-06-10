#include "Draughts.h"
#include <string.h>
#include <stdlib.h>


// infrastructure

typedef struct  //define a location 
{
  int column;	//0-9 (a-j)
  int row;	//0-9 (1-10)
  struct location *next; //linked list implementation
} location;


typedef struct 
{
  int eats; //how good is this move 
  location step; //will be a linked list of steps within this move	
  struct move *next;	//linked list of moves
  struct move *last;	//last move linked list of moves
	
} move;


// functions headers

void set_minmax_depth(int x);
void set_user_color(char *s);
void quit(void);
void clear();
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
move* get_disc_moves(char* a_board, location l);
move* get_moves(int color, char* cpy_brd); // all the available moves
move* link_moves(move *moves, move *disc_moves);
int same_color(char a, char b);
void print_move(move *m);
void print_all_move(move *m);
int is_legal_move(move* m);// will check if move is legal

//??? to be implemented:

move* get_first_move(location l); // the first move of the instrument, will call get_move...
move* get_move(location l, char* cpy_brd); // don't need to make distinctions between men and kings (i think)
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


// 																		*************** General methods ****************

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
void clear(){ 
  for (int i = 0; i < BOARD_SIZE; i++){
    for ( int j = 0; j < BOARD_SIZE; j++){
      board[i][j] = EMPTY;
    }
  }
}

/** sets the l location on the board to EMPTY if l is legal */
void remove_location(location l){ 
  if (is_legal_location(l)){
    board[l.row][l.column] = EMPTY;
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
    board[l.row][l.column] = disc;
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
		return 0;
	}
	if ( NUM_WHITE_M+NUM_WHITE_K == 0 || NUM_BLACK_M+NUM_BLACK_K == 0){ // too little (none) black/white discs on board
		print_message(WROND_BOARD_INITIALIZATION);
		return 0;
	}
	return 1;
}

/** converts locus into a location. */
location str_to_location(char* locus){
	location l;
	int row = locus[1] - 'a'; // convert the row to integer
	int column = 0;
	for ( int i = 3 ; locus[i] != '>' ; i++ ){ // calculate column value (could be '10')
		column = 10 * column + (locus[i] - '0');
	} 
	l.row = row; 
	l.column = column-1; // normalize to range starting from 0
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
move* get_moves(char *a_board, int is_white_turn){
	
	// initialize a move linked list:
	move moves = NULL; 
	location *l;
	if (( l = malloc(sizeof *l)) == NULL ){
		quit_allcation_error();
	}
	// find out who's turn it is
	char man = is_white_turn ? WHITE_M : BLACK_M;
	char king = is_white_turn ? WHITE_K : BLACK_K;

	for ( int i = 0; i < BOARD_SIZE; i++ ){
		for ( int j = 0; j < BOARD_SIZE; j++ ){
			if (  board[i][j] == man || board[i][j] == king ){
				move disc_moves;
				l.row = i; //???maybe we want to transfer just the i,j?
				l.column = j;
				disc_moves = get_disc_moves(a_board, l); // get the moves for the discs in this location
				moves = link_moves(moves,disc_moves); // concatenate the linked lists
			}
		}
	}
	return moves;
}

move* get_disc_moves(char* a_board, location l){
	char disc = a_board[l.row][l.column];
	move moves = NULL;
	int eat; // it is possible to eat (first step)
	if ( disc == BLACK_K || disc == WHITE_K ){
		int k = -1;
		int l = 1;
		while ( l.row + k >= 0 && l.column + l <= BOARD_SIZE-1 ){ // location is inside the board (upper right)
			if ( !(same_color(a_board[l.row  + k][l.column + l], disc)){ // enemy or empty 
				if ( a_board[l.row + k][l.column + l] == EMPTY ){
					moves = link_moves(moves, create_move( l.row + k, l.column + l ));					
				}
				else { // enemy!!!
					if ( (l.row + k-1  >= 0 && l.column + l+1 <= BOARD_SIZE-1)  && a_board[l.row + k-1][l.column + l+1] == EMPTY){ // EATING! :O
						char a_board_copy;// copy the board to and change it  ????
						moves = link_moves(moves, get_eating_moves(l.row + k-1, l.column + l+1, a_board_copy ))
						break;
						
					}	
				}
			}
			k--;
			l++;
		}
		k = -1;
		l = -1;
		while ( l.row + k >= 0 && l.column + l >= 0 ){ // location is inside the board (upper left)
			if ( !(same_color(a_board[l.row  + k][l.column + l], disc)){ // enemy or empty 
				if ( a_board[l.row + k][l.column + l] == EMPTY ){
					moves = link_moves(moves, create_move(l.row, l.column, l.row + k, l.column + l));					
				}
				else { // enemy!!!
					if ( (l.row + k-1  >= 0 && l.column + l-1 >= 0)  && a_board[l.row + k-1][l.column + l-1] == EMPTY){ // EATING! :O
						//???
						break;
						
					}	
				}
			}
			k--;
			l--;
		}
		k = 1;
		l = 1;
		while ( l.row + k <= BOARD_SIZE-1 && l.column + l <= BOARD_SIZE-1 ){ // location is inside the board (lower right)
			if ( !(same_color(a_board[l.row  + k][l.column + l], disc)){ // enemy or empty 
				if ( a_board[l.row + k][l.column + l] == EMPTY ){
					moves = link_moves(moves, create_move(l.row, l.column, l.row + k, l.column + l));					
				}
				else { // enemy!!!
					if ( (l.row + k+1  >= 0 && l.column + l+1 <= BOARD_SIZE-1)  && a_board[l.row + k+1][l.column + l+1] == EMPTY){ // EATING! :O
						//???
						break;
						
					}	
				}
			}
			k++;
			l++;
		}
		k = 1;
		l = -1;
		while ( l.row + k <= BOARD_SIZE-1 && l.column + l >= 0 ){ // location is inside the board (lower left)
			if ( !(same_color(a_board[l.row  + k][l.column + l], disc)){ // enemy or empty 
				if ( a_board[l.row + k][l.column + l] == EMPTY ){
					moves = link_moves(moves, create_move(l.row, l.column, l.row + k, l.column + l));					
				}
				else { // enemy!!!
					if ( (l.row + k+1 <= BOARD_SIZE-1 && l.column + l-1 >= 0)  && a_board[l.row + k+1][l.column + l-1] == EMPTY){ // EATING! :O
						//???
						break;
						
					}	
				}
			}
			k++;
			l--;
		}

	}
	else {
		if ( (l.row - 1 >= 0 && l.column + 1 <= BOARD_SIZE-1) ){ // location is inside the board (upper right)
			if ( !(same_color(a_board[l.row - 1][l.column + 1], disc)){ // enemy or empty 
				if ( a_board[l.row - 1][l.column + 1] == EMPTY ){
					if ( !IS_WHITE(disc) ){
						moves = link_moves(moves, create_move(l.row, l.column, l.row -1, l.column + 1));					
					}
					
				}
				else { // enemy!!!
					if ( (l.row - 2 >= 0 && l.column + 2 <= BOARD_SIZE-1)  && a_board[l.row - 2][l.column + 2] == EMPTY){ // EATING! :O
						//???
					}	
				}
			}
		}
		if ( (l.row - 1 >= 0 && l.column - 1  >= 0) ){ // location is inside the board (upper left)
			if ( !(same_color(a_board[l.row - 1][l.column - 1], disc)){ // enemy or empty 
				if ( a_board[l.row - 1][l.column - 1] == EMPTY ){
					if ( !IS_WHITE(disc) ){
						moves = link_moves(moves, create_move(l.row, l.column, l.row -1, l.column - 1));					
					}					
				}
				else { // enemy!!!
					if ( (l.row - 2 >= 0 && l.column - 2 >= 0 )  && a_board[l.row - 2][l.column - 2] == EMPTY){ // EATING! :O
						//???
					}	
				}
			}
		}
		if ( (l.row + 1 <= BOARD_SIZE-1 && l.column + 1 <= BOARD_SIZE-1) ){ // location is inside the board (lower right)
			if ( !(same_color(a_board[l.row + 1][l.column + 1], disc)){ // enemy or empty 
				if ( a_board[l.row + 1][l.column + 1] == EMPTY ){
					if ( IS_WHITE(disc) ){
						moves = link_moves(moves, create_move(l.row, l.column, l.row + 1, l.column + 1));					
					}
				}
				else { // enemy!!!
					if ( (l.row + 2 <= BOARD_SIZE-1 && l.column + 2 <= BOARD_SIZE-1)  && a_board[l.row + 2][l.column + 2] == EMPTY){ // EATING! :O
						//???
					}	
				}
			}
		}
		if ( (l.row + 1 <= BOARD_SIZE-1 && l.column - 1 >= 0) ){ // location is inside the board (lower left)
			if ( !(same_color(a_board[l.row + 1][l.column - 1], disc)){ // enemy or empty 
				if ( a_board[l.row + 1][l.column - 1] == EMPTY ){
					if ( IS_WHITE(disc) ){
						moves = link_moves(moves, create_move(l.row, l.column, l.row + 1, l.column - 1));					
					}				
				}
				else { // enemy!!!
					if ( (l.row + 2 <= BOARD_SIZE-1 && l.column - 2 >= 0)  && a_board[l.row + 2][l.column - 1] == EMPTY){ // EATING! :O
						//???
					}	
				}
			}
		}
	}	
}

int same_color(char a, char b){
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
	
	if ( temp.size < moves.size ) { // moves are better, dump new moves 
		// ???free disc_moves here 
		return moves;
	}
	
	if ( temp.size > moves.size ) { // disc_moves is better, dump previous moves 
		// ???free moves here 
		moves = disc_moves;
		return moves;
	}
	// else: (sizes are equal)
	while ( temp.next != NULL ){ // reach last move of disc_moves
		temp = temp.next;
	}
	if ( temp.size <= )
	temp.next = moves;
	moves = disc_moves;
	return moves;
}

void print_move(move *m){
	location *temp = m.step;
	printf("<%d,%c> to ", temp.row,(temp.column+'a'));
	while ( (temp = temp.next) != NULL){
		printf("<%d,%c>", temp.row,(temp.column+'a'));
	}
	printf("\n")
}

void print_all_moves(move *m){
	while(m != NULL){
		print_move(m);
		m = m.next;
	}
}

int is_legal_move(move* m){
	location *temp_loc1; //  pointer for the pattern moves
	location *temp_loc2; // pointer for the user move
	move *moves = get_moves(WHITE_TURN, board ); // copy the board ????      //get pattern moves (legal)
	move * temp_moves = moves;
	while(temp_moves != NULL){ // check if one of the pattern moves is the same as the user move
		temp_loc1 = temp_moves.step;
		temp_loc2 = m.step;
		while(temp_loc1 != NULL  && temp_loc2 != NULL){
			if (temp_loc1.row != temp_loc2.row || temp_loc1.column != temp_loc2.column ){
				break;
			}
			temp_loc1 = temp_loc1.next;
			temp_loc2 = temp_loc2.next;
		}
		if (temp_loc1 == NULL  && temp_loc2 == NULL){ // the user move is the same as a legal move
			return 1;
		}
		temp_moves = temp_moves.next;
	}
	//free moves ????
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
	GAME = 0 ;
	SETTINGS = 0;
	return 1;
}
int test3(void){ //print settings
	printf("MINIMAX_DEPTH = %d\n",MINIMAX_DEPTH);
	printf("PLAYER_WHITE = %d\n",PLAYER_WHITE);
	printf("WHITE_TURN = %d\n",WHITE_TURN);
	return 1;
}
int test4(void){ //print all this turn moves + board
	print_board(board);
	printf("WHITE_TURN = %d",WHITE_TURN);
	move *moves = get_moves(board, WHITE_TURN);
	print_all_moves(moves);
	//free moves ????
	return 1
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
			if ( PLAYER_WHITE && WHITE_TURN || !PLAYER_WHITE && !WHITE_TURN ){ //user's turn???maybe make different logic. 
				printf("%s", ENTER_YOUR_MOVE);//should be here???
				parse_input_game(input);
			}
			else { // computer's turn
				// do something???
			}
			test3();
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
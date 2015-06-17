#ifndef DRAUGHTS_
#define DRAUGHTS_

#include<stdio.h>


#define WHITE_M 'm'
#define WHITE_K 'k'
#define BLACK_M 'M'
#define BLACK_K 'K'
#define EMPTY ' '

#define BOARD_SIZE 10

typedef char** board_t;
#define WELCOME_TO_DRAUGHTS "Welcome to Draughts!\n"
#define ENTER_SETTINGS "Enter game settings:\n" 
#define WRONG_MINIMAX_DEPTH "Wrong value for minimax depth. The value should be between 1 to 6\n"
#define WRONG_POSITION "Invalid position on the board\n"
#define NO_DICS "The specified position does not contain your piece\n"
#define ILLEGAL_COMMAND "Illegal command, please try again\n"
#define ILLEGAL_MOVE "Illegal move\n"
#define WROND_BOARD_INITIALIZATION "Wrong board initialization\n"
 
#define ENTER_YOUR_MOVE "Enter your move:\n" 
#define perror_message(func_name) (fprintf(stderr, "Error: standard function %s has failed\n", func_name))
#define print_message(message) (printf("%s", message));



void print_board(char board[BOARD_SIZE][BOARD_SIZE]);
void init_board(char board[BOARD_SIZE][BOARD_SIZE]);
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
int minmax(char a_board[BOARD_SIZE][BOARD_SIZE], int maxi, int depth);
move* get_disc_moves(char a_board[BOARD_SIZE][BOARD_SIZE], location *l);
move* get_moves(char a_board[BOARD_SIZE][BOARD_SIZE], int is_white_turn); 
void do_move(char a_board[BOARD_SIZE][BOARD_SIZE],move* m);
move *get_eating_moves(int row ,int column, char a_board[BOARD_SIZE][BOARD_SIZE]);
move *get_move_minmax(void);
move* link_moves(move *moves, move *disc_moves);
int score_board(char a_board[BOARD_SIZE][BOARD_SIZE],int white_player);
int same_color(char a, char b);
void print_move(move *m);
void print_all_moves(move *m);
int is_legal_move(move* m);
int parse_input_game(char* input); 
 

#endif  

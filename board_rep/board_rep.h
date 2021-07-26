#ifndef BOARD_REP_H_
#define BOARD_REP_H_

#include <stdint.h>

/*
				FILE
			    black
	   A   B   C   D   E   F   G   H
	 ---------------------------------- 
   8 | 56  57  58  59  60  61  62  63 |
   7 | 48  49  50  51  52  53  54  55 |
R  6 | 40  41  42  43  44  45  46  47 |
A  5 | 32  33  34  35  36  37  38  39 |
N  4 | 24  25  26  27  28  29  30  31 |
K  3 | 16  17  18  19  20  21  22  23 |
   2 | 8   9   10  11  12  13  14  15 |
   1 | 0   1   2   3   4   5   6   7  |
	 ----------------------------------
				white
 
uint64 := {0 .. 63}
*/

// 72 bytes (64 bits = 8 bytes * 9 items)
typedef struct {
	uint64_t all;
	uint64_t white;
	uint64_t black;
	uint64_t pawns;
	uint64_t knights;
	uint64_t bishops;
	uint64_t rooks;
	uint64_t queens;
	uint64_t kings;
} Board;

#define WHITE 	0x00
#define BLACK 	0x01

int move_count;

// allocates and zeroes a new board
Board* empty_board();

// allocates and initializes a board for a new game 
Board* new_game_board();

// initializes a board for a new game
void init_new_game(Board* board);

// for testing
Board* test_game_board();

// copy 
Board* copy_board(const Board * board);

// move piece
void move_piece(Board* board, char color, char taken, uint64_t old_pos, uint64_t new_pos);
void unset(Board* board, char color, uint64_t position);

// next board state
void next_board(Board* board, uint64_t old_pos, uint64_t new_pos);

// prints in a fancy helpful manner, good for debugging
void print_board_and_moves(Board * board, uint64_t position, uint64_t moves);

// simple colorized binary 0/1 print
void print_num(uint64_t num);

// finds piece's type and generates moves
uint64_t moves 			(Board* board, uint64_t position, char color);

// generates moves for specific types
uint64_t knight_moves	(Board* board, uint64_t position, char color);
uint64_t pawn_moves		(Board* board, uint64_t position, char color);
uint64_t king_moves 	(Board* board, uint64_t position, char color);
uint64_t queen_moves 	(Board* board, uint64_t position, char color);
uint64_t bishop_moves 	(Board* board, uint64_t position, char color); 
uint64_t rook_moves 	(Board* board, uint64_t position, char color); 

// if you just need the slides, i do, aww thats what she said
uint64_t slideUp		(uint64_t position, uint64_t other, uint64_t nsame);
uint64_t slideDown		(uint64_t position, uint64_t other, uint64_t nsame);
uint64_t slideLeft		(uint64_t position, uint64_t other, uint64_t nsame);
uint64_t slideRight		(uint64_t position, uint64_t other, uint64_t nsame);
uint64_t slideUpRight	(uint64_t position, uint64_t other, uint64_t nsame);
uint64_t slideUpLeft	(uint64_t position, uint64_t other, uint64_t nsame);
uint64_t slideDownRight	(uint64_t position, uint64_t other, uint64_t nsame);
uint64_t slideDownLeft	(uint64_t position, uint64_t other, uint64_t nsame);

// evaluate board state
int evaluate_board(Board* board, char color);

#endif
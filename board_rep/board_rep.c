#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board_rep.h"

// allocates and zeroes a new board
Board* empty_board() {
	Board * board = (Board*) malloc(sizeof(Board));
	memset(board, 0, sizeof(Board));
	return board;
}

// void init_new_game(Board* board) {
// 	// printf("Initializing\n");
// 	board->all = 		0x0308040000000000;
// 	board->black = 		0x0308000000000000;
// 	board->white = 		0x0000040000000000;
// 	board->pawns = 		0x0008000000000000;
// 	board->rooks =		0x0100000000000000;
// 	board->bishops =	0x0000000000000000;
// 	board->knights =	0x0200040000000000;
// 	board->queens =		0x0000000000000000;
// 	board->kings = 		0x0000000000000000;

// 	move_count=0;
// }

void init_new_game(Board* board) {
	// printf("Initializing\n");
	board->all = 		0xffff00000000ffff;
	board->white = 		0xffff000000000000;
	board->black = 		0x000000000000ffff;
	board->pawns = 		0x00ff00000000ff00;
	board->rooks =		0x8100000000000081;
	board->bishops =	0x2400000000000024;
	board->knights =	0x4200000000000042;
	board->queens =		0x1000000000000010;
	board->kings = 		0x0800000000000008;

	move_count=0;
}

Board* new_game_board() {

	Board * board = empty_board();
	
	init_new_game(board);

	return board;
}

Board* test_game_board() {
	Board * board = empty_board();
	
	board->all = 		0x0002010400000000;
	board->white = 		0x0002000400000000;
	board->black = 		0x0000010000000000;
	board->pawns = 		0x0000000000000000;
	board->rooks =		0x0000010000000000;
	board->bishops =	0x0002000400000000;
	board->knights =	0x0000000000000000;
	board->queens =		0x0000000000000000;
	board->kings = 		0x0000000000000000;

	return board;
}

Board* copy_board(const Board * board) {

	// dynamically allocate
	Board * cpy = (Board*) malloc(sizeof(Board));

	// copy data
	memcpy(cpy, board, sizeof(Board));

	return cpy;
}
// stub
void unset(Board* board, char color, uint64_t position);

/*
 *	taken = {
 *		1 => if new position takes a piece
 *		0 => doesn't take a piece
 *	}
 */
void move_piece(Board* board, char color, char taken, uint64_t old_pos, uint64_t new_pos) {
	board->all &= ~old_pos;
	board->all |= new_pos;

	if(taken) {
		unset(board, !color, new_pos);
	}

	// set new position
	if(color) {
		board->black &= ~old_pos;
		board->black |= new_pos;

	} else {
		board->white &= ~old_pos;
		board->white |= new_pos;
	}

	if (old_pos & board->pawns) {
		board->pawns &= ~old_pos;
		board->pawns |= new_pos; 
	}
	else if (old_pos & board->rooks) {
		board->rooks &= ~old_pos;
		board->rooks |= new_pos; 
	}
	else if (old_pos & board->knights) {
		board->knights &= ~old_pos;
		board->knights |= new_pos; 
	}
	else if (old_pos & board->bishops) {
		board->bishops &= ~old_pos;
		board->bishops |= new_pos; 
	}
	else if (old_pos & board->queens) {
		board->queens &= ~old_pos;
		board->queens |= new_pos; 
	}
	else if (old_pos & board->kings) {
		board->kings &= ~old_pos;
		board->kings |= new_pos; 
	}
}

void next_board(Board* board, uint64_t old_pos, uint64_t new_pos) {
	
	board->all &= ~old_pos;
	board->all |= new_pos;

	if(board->black & old_pos){
		if(board->white & new_pos){
			unset(board, 0, new_pos);
		}

		board->black &= ~old_pos;
		board->black |= new_pos;
	}
	else{
		if(board->black & new_pos){
			unset(board, 1, new_pos);
		}

		board->white &= ~old_pos;
		board->white |= new_pos;
	}




	// Find piece type and move piece
	if (old_pos & board->pawns) {
		board->pawns &= ~old_pos;
		board->pawns |= new_pos;
		return;
	}
	else if (old_pos & board->rooks) {
		board->rooks &= ~old_pos;
		board->rooks |= new_pos; 
		return;
	}
	else if (old_pos & board->knights) {
		board->knights &= ~old_pos;
		board->knights |= new_pos; 
		return;
	}
	else if (old_pos & board->bishops) {
		board->bishops &= ~old_pos;
		board->bishops |= new_pos; 
		return;
	}
	else if (old_pos & board->queens) {
		board->queens &= ~old_pos;
		board->queens |= new_pos; 
		return;
	}
	else if (old_pos & board->kings) {
		board->kings &= ~old_pos;
		board->kings |= new_pos; 
		return;
	}
	return;
}

void unset(Board* board, char color, uint64_t position) {

	if(color) {
		board->black &= ~position;
	} else {
		board->white &= ~position;
	}

	if (position & board->pawns) {
		board->pawns &= ~position;
		return;
	}
	else if (position & board->rooks) {
		board->rooks &= ~position;
		return;
	}
	else if (position & board->knights) {
		board->knights &= ~position;
		return;
	}
	else if (position & board->bishops) {
		board->bishops &= ~position;
		return;
	}
	else if (position & board->queens) {
		board->queens &= ~position;
		return;
	}
	else if (position & board->kings) {
		board->kings &= ~position;
		return;
	}
}

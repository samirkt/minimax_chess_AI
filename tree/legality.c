

#include <stdlib.h>
#include <stdint.h>
#include <string.h> 
#include <assert.h>

#include "board_rep/board_rep.h"
#include "bits/bitscan.h"
#include "structures.h"
#include "tree.h"
#include "tree_helper.h"

// stubs
static uint8_t checkSlides(Status * status, uint64_t king,uint64_t same, uint64_t other);
static uint8_t checkPawn(Status * status, uint64_t king, uint64_t other);
static uint8_t checkKing(Status * status, uint64_t king, uint64_t other);
static uint8_t checkKnight(Status * status, uint64_t king, uint64_t other);

// takes new node with move made, before turn change
uint8_t is_legal(Status * status){

	assert(status);

	uint8_t val = 1;

	uint64_t same;
	uint64_t other;

	if (status->color) {
		same = status->board.black;
		other = status->board.white;
	} else {
		same = status->board.white;
		other = status->board.black;
	}

	uint64_t king = status->board.kings & same;

	// all must be legal (return 1)
	val &= checkSlides(status, king, ~same, other);
	val &= checkPawn(status, king, other);
	val &= checkKing(status, king, other);
	val &= checkKnight(status, king, other);

	return val;
}

uint8_t castle_is_legal(Status * status, uint8_t direction) {

	int i;
	uint8_t ret = 1;

	// make copy of status
	Status stat = *status;

	// find king
	uint64_t king = stat.board.kings & (stat.color ? stat.board.black : stat.board.white);

	// castle left
	if(direction & CASTLE_L) {

		// if checked in initial condition
		ret &= is_legal(&stat);

		// if passing through check
		for(i = 0; i < 2 && ret; ++i){

			// move and check
			move_piece(&stat.board, stat.color,  0, king, king << 1);
			king <<= 1;
			ret &= is_legal(&stat);
		}
	}

	// castle right
	else {

		// if checked in initial condition
		ret &= is_legal(&stat);

		// if passing through check
		for(i = 0; i < 2 && ret; ++i){
			
			// move and check
			move_piece(&stat.board, stat.color,  0, king, king >> 1);
			king >>= 1;
			ret &= is_legal(&stat);
		}
	}

	return ret;
}

#define START_R	0x0000000000000001UL
#define START_F 0x8000000000000000UL

static uint8_t checkSlides(Status * status, uint64_t king, uint64_t nsame, uint64_t other){

	uint64_t cClockwise[8];
	int idx;
	uint64_t pos;

	// calculate sliding moves from king's position

	// can check trailing zeros
	cClockwise[0] = slideRight(king, other, nsame);
	cClockwise[1] = slideUpRight(king, other, nsame);
	cClockwise[2] = slideUp(king, other, nsame);
	cClockwise[3] = slideUpLeft(king, other, nsame);
	
	// can check leading zeros
	cClockwise[4] = slideLeft(king, other, nsame);
	cClockwise[5] = slideDownLeft(king, other, nsame);
	cClockwise[6] = slideDown(king, other, nsame);
	cClockwise[7] = slideDownRight(king, other, nsame);

	int i;

	for(i = 0; i < 4; ++i) {

		// there are moves in this direction
		if(cClockwise[i]){

			// find last bit in direction
			reverse_bit_scan(idx, cClockwise[i]);
			pos = START_R << idx;

			// odd
			if(i & 1){
				// if matches enemy, return not legal
				if((pos & other) && (pos & status->board.queens || pos & status->board.bishops))
					return 0;
			} 

			// even
			else {
				// if matches enemy, return not legal
				if((pos & other) && (pos & status->board.queens || pos & status->board.rooks))
					return 0;
			}
		}
	}

	for(;i < 8; ++i) {

		// there are moves in this direction
		if(cClockwise[i]){

			// find last bit in direction
			forward_bit_scan(idx, cClockwise[i]);
			pos = START_F >> idx;

			// odd
			if(i & 1){
				// if matches enemy, return not legal
				if((pos & other) && (pos & status->board.queens || pos & status->board.bishops))
					return 0;
			} 

			// even
			else {
				// if matches enemy, return not legal
				if((pos & other) && (pos & status->board.queens || pos & status->board.rooks))
					return 0;
			}
		}
	}

	// everything checks out
	return 1;
}

static uint8_t checkPawn(Status * status, uint64_t king, uint64_t other){

	uint64_t moves = pawn_moves(&status->board, king, status->color);

	if(moves & other & status->board.pawns)
		return 0;

	return 1;
}

static uint8_t checkKing(Status * status, uint64_t king, uint64_t other){

	uint64_t moves = king_moves(&status->board, king, status->color);

	if(moves & other & status->board.kings)
		return 0;

	return 1;
}

static uint8_t checkKnight(Status * status, uint64_t king, uint64_t other){

	uint64_t moves = knight_moves(&status->board, king, status->color);

	if(moves & other & status->board.knights)
		return 0;

	return 1;
}


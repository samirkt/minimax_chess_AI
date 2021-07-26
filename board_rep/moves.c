#include "board_rep.h"

#include <stdlib.h>
#include <stdint.h>

#define UNARY_OR(x)		(0 != (x))
#define SIDES 		0x8181818181818181
#define N_LEFT		0x7f7f7f7f7f7f7f7f
#define N_RIGHT		0xfefefefefefefefe
#define N2_LEFT		0x3f3f3f3f3f3f3f3f
#define N2_RIGHT	0xfcfcfcfcfcfcfcfc

uint64_t pawn_moves(Board* board, uint64_t position, char color){

	uint64_t generated = 0;

	// black piece
	if(color) {
		
		// can take white piece to the right
		if((position << 7) & (board->white) & N_RIGHT){
			generated |= position << 7;
		}
		
		// can take piece to the left 
		if((position << 9) & (board->white) & N_LEFT){
			generated |= position << 9;
		}
		
		// not blocked 
		if((position << 8) & ~(board->all)){
			generated |= position << 8;
		
			// haven't moved yet and not blocked 2 up
			if( (position & 0x000000000000ff00) && ((position << 16) & ~board->all)){
		
				generated |= position << 16;
			}

		}
		
		// TODO: check bottom edge for transformation
	}

	// white piece
	else {
 		
 		// can take black piece to the left
		if((position >> 7) & board->black & N_LEFT){
			generated |= position >> 7;
		}

 		// can take piece to the right
		if((position >> 9) & (board->black) & N_RIGHT){
			generated |= position >> 9;
		}

		// not blocked
		if((position >> 8) & ~(board->all)){
			generated |= position >> 8;

			// haven't moved yet and not blocked 2 up
			if( (position & 0x00ff000000000000) && ((position >> 16) & ~board->all)){
		
				generated |= position >> 16;
			}
		}

		// TODO check topedge for transformation
	}

	return generated;
}

uint64_t king_moves(Board* board, uint64_t position, char color) {
	
	uint64_t generated = 0;

	generated |= position >> 8;
	generated |= position << 8;

	if(position & N_LEFT) {

		generated |= position >> 7;
		generated |= position << 1;
		generated |= position << 9;
	}

	if(position & N_RIGHT) {

		generated |= position << 7;
		generated |= position >> 1;
		generated |= position >> 9;
	}

	if(color) {
		generated &= ~board->black;
	} else {
		generated &= ~board->white;
	}

	return generated;
}

uint64_t knight_moves(Board* board, uint64_t position, char color) {

	uint64_t generated = 0;

	if(N2_LEFT & position) {
		generated |= position >> 6;
		generated |= position << 10;
	}

	if(N_LEFT & position) {
		generated |= position >> 15;
		generated |= position << 17;
	}

	if(N2_RIGHT & position) {
		generated |= position >> 10;
		generated |= position << 6;
	}

	if(N_RIGHT & position) {
		generated |= position >> 17;
		generated |= position << 15;
	}

	if(color) {
		generated &= ~board->black;
	} else {
		generated &= ~board->white;
	}

	return generated;
}

// function stubs
static uint64_t slideDiagonal(Board* board, uint64_t position, char color);
static uint64_t slideStraight(Board* board, uint64_t position, char color);

uint64_t queen_moves(Board* board, uint64_t position, char color) {
	return slideDiagonal(board, position, color) | slideStraight(board, position, color);
}

uint64_t bishop_moves (Board* board, uint64_t position, char color){
	return slideDiagonal(board, position, color);
}

uint64_t rook_moves(Board* board, uint64_t position, char color) {
	return slideStraight(board, position, color);
}

static uint64_t slideDiagonal(Board* board, uint64_t position, char color) {
	
	uint64_t generated;
	uint64_t other;
	uint64_t nsame;	

	// if black
	if(color) {
		other = board->white;
		nsame = ~board->black;
	} else{
		other = board->black;
		nsame = ~board->white;
	}

	generated = slideUpRight(position,other,nsame)
				| slideUpLeft(position,other,nsame)
				| slideDownRight(position,other,nsame)
				| slideDownLeft(position,other,nsame);

	return generated;
}

static uint64_t slideStraight(Board* board, uint64_t position, char color) {

	uint64_t generated = 0;
	uint64_t other;
	uint64_t nsame;	

	// if black
	if(color) {
		other = board->white;
		nsame = ~board->black;
	} else{
		other = board->black;
		nsame = ~board->white;
	}

	
	generated = slideUp(position, other, nsame)
				| slideDown(position, other, nsame)
				| slideLeft(position, other, nsame)
				| slideRight(position, other, nsame);

	return generated;
}

uint64_t moves(Board* board, uint64_t position, char color) {
	
	if(board->pawns & position) 
		return pawn_moves(board,position,color);
	else if (board->rooks & position)
		return rook_moves(board,position,color);
	else if (board->knights & position)
		return knight_moves(board,position,color);
	else if (board->bishops & position)
		return bishop_moves(board,position,color);
	else if (board->queens & position)
		return queen_moves(board,position,color);
	else if (board->kings & position)
		return king_moves(board,position,color);

	return 0;
}

uint64_t slideUp(uint64_t position, uint64_t other, uint64_t nsame){
	uint64_t generated = 0;
	uint64_t idx;

	for(idx = position >> 8; idx & nsame; idx >>= 8) {
		generated |= idx;
		if(idx & other) break;
	}

	return generated; 
}

uint64_t slideDown(uint64_t position, uint64_t other, uint64_t nsame){
	uint64_t generated = 0;
	uint64_t idx;

	for(idx = position << 8; idx & nsame; idx <<= 8) {
		generated |= idx;
		if(idx & other) break;
	}

	return generated; 
}

uint64_t slideLeft(uint64_t position, uint64_t other, uint64_t nsame){
	uint64_t generated = 0;
	uint64_t idx;

	if(N_LEFT & position) {
		for(idx = position << 1; idx & nsame; idx <<= 1) {
			
			generated |= idx;
			if(idx & other || idx & SIDES) break;
		}
	}

	return generated; 
}

uint64_t slideRight(uint64_t position, uint64_t other, uint64_t nsame){
	uint64_t generated = 0;
	uint64_t idx;

	if(N_RIGHT & position) {
		for(idx = position >> 1; idx & nsame; idx >>= 1) {
			
			generated |= idx;
			if(idx & other || idx & SIDES) break;
		}
	}

	return generated; 
}

uint64_t slideUpRight(uint64_t position, uint64_t other, uint64_t nsame){

	uint64_t generated = 0;
	uint64_t idx;

	if(position & N_RIGHT){
		for(idx = position >> 9; idx & nsame ; idx >>= 9) {
			
			generated |= idx;

			if(idx & SIDES || idx & other) break;
		}
	}

	return generated;
}

uint64_t slideUpLeft(uint64_t position, uint64_t other, uint64_t nsame){
	uint64_t generated = 0;
	uint64_t idx;

	if(position & N_LEFT){
		for(idx = position >> 7; idx & nsame ; idx >>= 7) {

			generated |= idx;

			if(idx & SIDES || idx & other) break;
		}
	}
	return generated;
}

uint64_t slideDownRight(uint64_t position, uint64_t other, uint64_t nsame){
	uint64_t generated = 0;
	uint64_t idx;

	if(position & N_RIGHT){
		for(idx = position << 7; idx & nsame ; idx <<= 7) {
			
			generated |= idx;

			if(idx & SIDES || idx & other) break;
		}
	}

	return generated;
}

uint64_t slideDownLeft(uint64_t position, uint64_t other, uint64_t nsame){
	uint64_t generated = 0;
	uint64_t idx;

	if(position & N_LEFT){
		for(idx = position << 9; idx & nsame ; idx <<= 9) {

			generated |= idx;

			if(idx & SIDES || idx & other) break;
		}
	}

	return generated;
}



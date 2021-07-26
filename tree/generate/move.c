#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "board_rep/board_rep.h"
#include "bits/bitscan.h"
#include "tree/structures.h"
#include "tree/tree.h"
#include "tree/tree_helper.h"
#include "structs/hash_table/hash.h"

#define SIDES 		0x8181818181818181
#define N_LEFT		0x7f7f7f7f7f7f7f7f
#define N_RIGHT		0xfefefefefefefefe
#define N2_LEFT		0x3f3f3f3f3f3f3f3f
#define N2_RIGHT	0xfcfcfcfcfcfcfcfc
#define TOP 		0x00000000000000ff
#define BOTTOM 		0xff00000000000000
#define B_FILTER	0x000000ff00000000
#define W_FILTER	0x00000000ff000000

// prototypes
static void move_pawn		(Node_s* search, MvHelp * helper, uint8_t p_idx);
static void move_king 		(Node_s* search, MvHelp * helper, uint8_t p_idx);
static void move_knight 	(Node_s* search, MvHelp * helper, uint8_t p_idx);
static void move_queen 		(Node_s* search, MvHelp * helper, uint8_t p_idx);
static void move_bishop 	(Node_s* search, MvHelp * helper, uint8_t p_idx);
static void move_rook 		(Node_s* search, MvHelp * helper, uint8_t p_idx);
static void slideDiagonal	(Node_s* search, MvHelp * helper, uint8_t p_idx);
static void slideStraight	(Node_s* search, MvHelp * helper, uint8_t rook, uint8_t p_idx);

void move(Node_s* search, MvHelp * helper) {
	
	Node_i * iterator = &search->stack[search->s_ptr];
	Board * board = &iterator->node->status.board;

	// position needs to point to a piece
	assert(helper->same & iterator->position);

	if (board->pawns & iterator->position) 
		move_pawn(search, helper, helper->h_idx_color + H_PAWN);
	else if (board->rooks & iterator->position)
		move_rook(search, helper, helper->h_idx_color + H_ROOK);
	else if (board->knights & iterator->position)
		move_knight(search, helper, helper->h_idx_color + H_KNIGHT);
	else if (board->bishops & iterator->position)
		move_bishop(search, helper, helper->h_idx_color + H_BISHOP);
	else if (board->queens & iterator->position)
		move_queen(search, helper, helper->h_idx_color + H_QUEEN);
	else if (board->kings & iterator->position)
		move_king(search, helper, helper->h_idx_color + H_KING);
}

static void move_pawn(Node_s* search, MvHelp * helper, uint8_t p_idx){

	// setup
	Status * status = &search->stack[search->s_ptr].node->status;
	uint64_t position = search->stack[search->s_ptr].position;

	uint64_t pos = status->passant;

	uint64_t right_move;
	uint64_t left_move;
	uint64_t forward;
	uint64_t forward_2;
	uint8_t nright = UNARY_OR(position & N_RIGHT);
	uint8_t nleft = UNARY_OR(position & N_LEFT);

	uint8_t flags = NOOP;

	// black
	if(status->color){
		left_move = position << 9;
		right_move = position << 7;
		forward = position << 8;
		forward_2 = position << 16;
	} 

	// white
	else {
		left_move = position >> 7;
		right_move = position >> 9;
		forward = position >> 8;
		forward_2 = position >> 16;
	}

	// right before piece reaches other side
	if(forward & helper->boundary){
		flags |= XFORM;
	}

	// in position for passant capture
	if(pos && position & helper->filter){
		if(right_move & pos && nright){
			add_move(search, 0, position, right_move, TAKE_PASSANT, p_idx);
		}

		if(left_move & pos && nleft){
			add_move(search, 0, position, left_move, TAKE_PASSANT, p_idx);
		}
	}

	// can take white piece to the right
	if(right_move & helper->other && nright){
		add_move(search, 1, position, right_move, flags, p_idx);
	}

	// can take piece to the left 
	if(left_move & helper->other && nleft){
		add_move(search, 1, position, left_move, flags, p_idx);
	}

	// not blocked forward
	if(forward & ~(status->board.all)){
		add_move(search, 0, position, forward, flags, p_idx);

		// haven't moved yet and not blocked 2 up
		if( (position & helper->start) && (forward_2 & ~status->board.all)){
			add_move(search, 0, position, forward_2, ADD_PASSANT, p_idx);
		}
	}
}

static void move_king(Node_s* search, MvHelp * helper, uint8_t p_idx) {

	Status * status = &search->stack[search->s_ptr].node->status;
	uint64_t position = search->stack[search->s_ptr].position;

	uint64_t temp;
	uint8_t flags = NOOP;
	flags |= status->WCL ? REVOKE_L : NOOP;
	flags |= status->WCR ? REVOKE_R : NOOP;

	// can move up
	if((temp = position >> 8) & helper->nsame){
		add_move(search, (temp & helper->other), position, temp, flags, p_idx);
	}

	// can move down
	if((temp = position << 8) & helper->nsame){
		add_move(search, (temp & helper->other), position, temp, flags, p_idx);
	}

	if(position & N_LEFT) {

		// can move up left
		if((temp = position >> 7) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, flags, p_idx);
		}

		// can move left
		if((temp = position << 1) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, flags, p_idx);
		}

		// can move down left
		if((temp = position << 9) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, flags, p_idx);
		}
	}

	if(position & N_RIGHT) {

		// can move down right, constant checks for 
		if((temp = position << 7) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, flags, p_idx);
		}

		// can move right
		if((temp = position >> 1) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, flags, p_idx);
		}

		// can move up right
		if((temp = position >> 9) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, flags, p_idx);
		}
	}

	// all checks see if we have rights, the path is clear, and it is legal to castle

	// black
	if(status->color) {
		if(status->BCL  && !(status->board.all & 0x0000000000000070) && castle_is_legal(status, CASTLE_L)){
			add_move(search,0,0,0, flags | CASTLE_L, p_idx);
		}

		if(status->BCR && !(status->board.all & 0x0000000000000006) && castle_is_legal(status, CASTLE_R)){
			add_move(search,0,0,0, flags | CASTLE_R, p_idx);
		}
	}

	// white
	else {
		if(status->WCL  && !(status->board.all & 0x7000000000000000) && castle_is_legal(status, CASTLE_L)){
			add_move(search,0,0,0, flags | CASTLE_L, p_idx);
		}

		if(status->WCR && !(status->board.all & 0x0600000000000000) && castle_is_legal(status, CASTLE_R)){
			add_move(search,0,0,0, flags | CASTLE_R, p_idx);
		}
	}
}

static void move_knight(Node_s* search, MvHelp * helper, uint8_t p_idx) {

	uint64_t position = search->stack[search->s_ptr].position;
	uint64_t temp;

	if(N2_LEFT & position) {
		if((temp = position >> 6) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, NOOP, p_idx);
		}

		if((temp = position << 10) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, NOOP, p_idx);
		}
	}

	if(N_LEFT & position) {
		if((temp = position >> 15) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, NOOP, p_idx);
		}

		if((temp = position << 17) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, NOOP, p_idx);
		}
	}

	if(N2_RIGHT & position) {
		if((temp = position >> 10) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, NOOP, p_idx);
		}

		if((temp = position << 6) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, NOOP, p_idx);
		}
	}

	if(N_RIGHT & position) {
		if((temp = position >> 17) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, NOOP, p_idx);
		}

		if((temp = position << 15) & helper->nsame){
			add_move(search, (temp & helper->other), position, temp, NOOP, p_idx);
		}
	}
}

static void move_queen(Node_s* search, MvHelp * helper, uint8_t p_idx) {
	slideDiagonal(search, helper, p_idx);
	slideStraight(search, helper, 0, p_idx);
}

static void move_bishop (Node_s* search, MvHelp * helper, uint8_t p_idx){
	slideDiagonal(search, helper, p_idx);
}

static void move_rook(Node_s* search, MvHelp * helper, uint8_t p_idx) {
	slideStraight(search, helper, 1, p_idx);
}

static void slideDiagonal(Node_s* search, MvHelp * helper, uint8_t p_idx) {

	uint64_t position = search->stack[search->s_ptr].position;

	uint64_t idx;
	uint64_t taken;

	// right
	if(position & N_RIGHT){

		// up right
		for(idx = position >> 9; idx & helper->nsame; idx >>= 9){

			taken = (idx & helper->other);

			add_move(search, taken, position, idx, NOOP, p_idx);

			if(taken || idx & SIDES)
				break;
		}

		// down right
		for(idx = position << 7; idx & helper->nsame; idx <<= 7){

			taken = (idx & helper->other);

			add_move(search, taken, position, idx, NOOP, p_idx);

			if(taken || idx & SIDES)
				break;
		}
	}
	
	// left
	if(position & N_LEFT){

		// up left
		for(idx = position >> 7; idx & helper->nsame; idx >>= 7){

			taken = (idx & helper->other);

			add_move(search, taken, position, idx, NOOP, p_idx);
			
			if(taken || idx & SIDES)
				break;
		}

		// down left
		for(idx = position << 9; idx & helper->nsame; idx <<= 9){

			taken = (idx & helper->other);

			add_move(search, taken, position, idx, NOOP, p_idx);
			
			if(taken || idx & SIDES)
				break;
		}
	}
}

static void slideStraight(Node_s* search, MvHelp * helper, uint8_t rook, uint8_t p_idx) {

	uint64_t position = search->stack[search->s_ptr].position;
	Status * status = &search->stack[search->s_ptr].node->status;

	uint64_t idx;
	uint64_t taken = 0;
	uint8_t flags = NOOP;

	
	if(rook){
		if((status->BCL || status->WCL) && position & 0x8000000000000080)
			flags |= REVOKE_L;
		if((status->BCR || status->WCR) && position & 0x0100000000000001)
			flags |= REVOKE_R;
	}
	
	// up
	for(idx = position >> 8; idx & helper->nsame && !taken; idx >>= 8){

		taken = (idx & helper->other);

		add_move(search, taken, position, idx, flags, p_idx);
	}	

	taken = 0;

	// down
	for(idx = position << 8; idx & helper->nsame && !taken; idx <<= 8){

		taken = (idx & helper->other);

		add_move(search, taken, position, idx, flags, p_idx);
	}

	taken = 0;

	// left
	if(N_LEFT & position) {

		for(idx = position << 1; idx & helper->nsame && !taken; idx <<= 1){

			taken = (idx & helper->other);

			add_move(search, taken, position, idx, flags, p_idx);

			if(idx & SIDES)
				break;
		}
	}

	taken = 0;

	// right
	if(N_RIGHT & position) {

		for(idx = position >> 1; idx & helper->nsame && !taken; idx >>= 1){

			taken = (idx & helper->other);

			add_move(search, taken, position, idx, flags, p_idx);

			if(idx & SIDES)
				break;
		}
	}
}

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "board_rep/board_rep.h"
#include "bits/bitscan.h"
#include "tree/structures.h"
#include "tree/tree.h"
#include "tree/tree_helper.h"
#include "tree/priority/queue.h"
#include "structs/hash_table/hash.h"

// stubs
static void mv(Node_o* node, uint64_t taken, uint64_t old, uint64_t new, uint8_t p_idx);
static void add_move_xform(Node_s* search, char taken, uint64_t old, uint64_t new, uint8_t p_idx);
static Node_o* new_child_node(Node_s* search);
static void unset_taken_hash(Node_o * node, uint64_t new);
static char* translate_comp(uint16_t move_made);
static void print_move(Node_o * node, uint64_t new);
static char piece_value(Board * board, uint64_t pos);

// allocates space for new node and copies over Status
Node_o* new_child_node(Node_s* search) {

	assert(search);

	Node_o * new_node = (Node_o *) malloc(sizeof(Node_o));

	Node_i * parent = &search->stack[search->s_ptr];

	// copy over node
	new_node->node = *parent->node;

	// copy over hash and init move_to_make
	new_node->hash = parent->obj->hash;

	// undone for now to expose other issues
	// new_node->node.move_to_make = 0;

	// s_ptr points to parent
	new_node->node.depth_searched = search->to_depth - (search->s_ptr + 1);

	return new_node;
}

static void unset_taken_hash(Node_o * node, uint64_t new){
	Status * status = &node->node.status;
	uint8_t p_idx;

	if(status->color)
		p_idx = H_WHITE;
	else
		p_idx = H_BLACK;

	if(status->board.pawns & new)
		p_idx += H_PAWN;
	else if(status->board.rooks & new)
		p_idx += H_ROOK;
	else if(status->board.bishops & new)
		p_idx += H_BISHOP;
	else if(status->board.knights & new)
		p_idx += H_KNIGHT;
	else if(status->board.queens & new)
		p_idx += H_QUEEN;
	else if(status->board.kings & new)
		p_idx += H_KING;

	node->hash = hash_piece(node->hash, new, p_idx);
}

// makes proper function call to move piece, taken can have any bit set
static void mv(Node_o* node, uint64_t taken, uint64_t old, uint64_t new, uint8_t p_idx) {

	assert(node);
	assert(old);
	assert(new);

	uint8_t tk = UNARY_OR(taken);

	node->hash = hash_move_piece(node->hash, old, new, p_idx);

	if(tk){
		unset_taken_hash(node, new);
	}

	move_piece(&node->node.status.board, node->node.status.color,  tk, old, new);

	node->move_made = 0;
	node->move_made |= get_column(new);
	node->move_made |= (get_row(new) << 3);
	node->move_made |= (get_column(old) << 6);
	node->move_made |= (get_row(old) << 9);

}

void remove_passant_capture(Node_o* node, uint64_t new) {

	assert(node);
	assert(new);

	// remove captured piece
	if(node->node.status.color) {
		node->node.status.board.pawns &= ~(new >> 8);
		node->node.status.board.white &= ~(new >> 8);
		node->hash = hash_piece(node->hash, new >> 8, WPAWN);
	} else {
		node->node.status.board.pawns &= ~(new << 8);
		node->node.status.board.black &= ~(new << 8);
		node->hash = hash_piece(node->hash, new << 8, BPAWN);
	}
}

// checks for and revokes castling rights
void piece_moved(Node_o* node, uint8_t flags) {
	
	assert(node);

	assert(flags & (REVOKE_L | REVOKE_R));
	
	if(node->node.status.color){
		if(flags & REVOKE_R && node->node.status.BCR){
			node->node.status.BCR = 0;
			node->hash = hash_castle(node->hash, BCASTLER);
		}
		if (flags & REVOKE_L && node->node.status.BCL){
			node->node.status.BCL = 0;
			node->hash = hash_castle(node->hash, BCASTLEL);
		}	
	}

	else {
		if(flags & REVOKE_R && node->node.status.WCR){
			node->node.status.WCR = 0;
			node->hash = hash_castle(node->hash, WCASTLER);
		}
		if (flags & REVOKE_L && node->node.status.WCL){
			node->node.status.WCL = 0;
			node->hash = hash_castle(node->hash, WCASTLEL);
		}
	}
}

void castle(Node_o* node, uint8_t direction) {

	assert(node);
	assert(direction & (CASTLE_R | CASTLE_L));

	// move order is important for generating proper
	// move tag

	// black
	if(node->node.status.color) {

		// remove any existing castling rights for color
		if(node->node.status.BCL)
			node->hash = hash_castle(node->hash, BCASTLEL);
		if(node->node.status.BCR)
			node->hash = hash_castle(node->hash, BCASTLER);

		if(direction & CASTLE_R) {
			mv(node, 0, 0x0000000000000001,0x0000000000000004, BROOK); // rook
			mv(node, 0, 0x0000000000000008,0x0000000000000002, BKING); // king
		}

		else {
			mv(node, 0, 0x0000000000000080,0x0000000000000010, BROOK); // rook
			mv(node, 0, 0x0000000000000008,0x0000000000000020, BKING); // king, seems like an error
		}
	}

	// white
	else {
			
		// remove any existing castling rights for color
		if(node->node.status.WCL)
			node->hash = hash_castle(node->hash, WCASTLEL);
		if(node->node.status.WCR)
			node->hash = hash_castle(node->hash, WCASTLER);

		if(direction & CASTLE_R) {
			mv(node, 0, 0x0100000000000000,0x0400000000000000, WROOK);
			mv(node, 0, 0x0800000000000000,0x0200000000000000, WKING);
		}

		else {
			mv(node, 0, 0x8000000000000000,0x1000000000000000, WROOK);
			mv(node, 0, 0x0800000000000000,0x2000000000000000, WKING);
		}
	}
}

// basically the switchboard for everything
void add_move(Node_s* search, uint64_t taken, uint64_t old, uint64_t new, uint8_t flags, uint8_t p_idx) {

	assert(search);

	// check if pawn transformation
	if(flags & XFORM){
		add_move_xform(search, taken, old, new, p_idx);
		return;
	}

	// make copy and add as a child
	Node_o * new_node = new_child_node(search);

	// check for castle moves
	if(flags & (CASTLE_R | CASTLE_L)){
		castle(new_node, flags); // generates p_idx on the spot
	}

	// normal move, updates move_made
	else {
		mv(new_node, taken, old, new, p_idx);
	}

	// if taking via passant
	if(flags & TAKE_PASSANT) {
		remove_passant_capture(new_node, new);
	}

	// if adding passant bits, otherwise clear
	if(flags & ADD_PASSANT) {
		// add passant
		uint8_t new_passant = 0x80 >> get_column(new);
		new_node->hash = hash_passant_change(new_node->hash, new_node->node.status.passant, new_passant);
		new_node->node.status.passant = new_node->node.status.passant;
	} else {
		// clear passant
		new_node->hash = hash_passant_change(new_node->hash, new_node->node.status.passant, 0);
		new_node->node.status.passant = 0;
	}

	// adjust castling rights
	if(flags & (REVOKE_R | REVOKE_L)) 
		piece_moved(new_node, flags);

	// if not legal
	if(!is_legal(&new_node->node.status)){
		
		free(new_node);
		return;
	} 	

	// change turn for new node
	new_node->node.status.color = !new_node->node.status.color;
	new_node->hash = hash_change_turn(new_node->hash);

	// set check bits, is legal calcs different value since turn has changed
	if(new_node->node.status.color){
		new_node->node.status.BCheck = !is_legal(&new_node->node.status);
	} else {
		new_node->node.status.WCheck = !is_legal(&new_node->node.status);
	}

	compute_scores(new_node, search->stack[search->s_ptr].obj, old, new, flags);

	if(r_print)
		print_move(new_node, new);

	pq_push(search->stack[search->s_ptr].queue, new_node);
}

static void add_move_xform(Node_s* search, char taken, uint64_t old, uint64_t new, uint8_t p_idx) {

	assert(search);
	assert(old & 0x00ff00000000ff00);
	assert(new & 0xff000000000000ff);

	Node_o * new_node[4];

	int i;
	for(i = 0; i < 4; ++i){
		new_node[i] = new_child_node(search);	

		// move pawn to new location
		mv(new_node[i], taken, old, new, p_idx);

		// clear passant
		new_node[i]->node.status.passant = 0;

	}

	// p_idx is being updated to reflect xform
	// all the same color, use first
	p_idx = (new_node[0]->node.status.color ? H_WHITE : H_BLACK);

	new_node[0]->node.status.board.pawns &= ~new;
	new_node[0]->node.status.board.queens |= new;
	new_node[0]->hash = hash_piece(new_node[0]->hash, new, p_idx + H_PAWN);
	new_node[0]->hash = hash_piece(new_node[0]->hash, new, p_idx + H_QUEEN);
	new_node[0]->move_made |= 04 << 12;

	new_node[1]->node.status.board.pawns &= ~new;
	new_node[1]->node.status.board.bishops |= new;
	new_node[1]->hash = hash_piece(new_node[1]->hash, new, p_idx + H_PAWN);
	new_node[1]->hash = hash_piece(new_node[1]->hash, new, p_idx + H_BISHOP);
	new_node[1]->move_made |= 05 << 12;

	new_node[2]->node.status.board.pawns &= ~new;
	new_node[2]->node.status.board.rooks |= new;
	new_node[2]->hash = hash_piece(new_node[2]->hash, new, p_idx + H_PAWN);
	new_node[2]->hash = hash_piece(new_node[2]->hash, new, p_idx + H_ROOK);
	new_node[2]->move_made |= 07 << 12;

	new_node[3]->node.status.board.pawns &= ~new;
	new_node[3]->node.status.board.knights |= new;
	new_node[3]->hash = hash_piece(new_node[3]->hash, new, p_idx + H_PAWN);
	new_node[3]->hash = hash_piece(new_node[3]->hash, new, p_idx + H_KNIGHT);
	new_node[3]->move_made |= 06 << 12;

	for(i = 0; i < 4; ++i){

		// if not legal
		if(!is_legal(&new_node[i]->node.status)){
			
			// undo
			free(new_node[i]);

			continue;
		}
		
		// change turn for new node
		new_node[i]->node.status.color = !new_node[i]->node.status.color;
		new_node[i]->hash = hash_change_turn(new_node[i]->hash);
		
		// set check bits, is legal calcs different value since turn has changed
		if(new_node[i]->node.status.color){
			new_node[i]->node.status.BCheck = !is_legal(&new_node[i]->node.status);
		} else {
			new_node[i]->node.status.WCheck = !is_legal(&new_node[i]->node.status);
		}

		if(r_print)
			print_move(new_node[i], new);

		pq_push(search->stack[search->s_ptr].queue, new_node[i]);
	}
}

static void print_move(Node_o * node, uint64_t new){
	char * string;

	string = translate_comp(node->move_made);

	printf("%c: %s\n", piece_value(&node->node.status.board, new), string);

	free(string);
}

// weights are kinda arbitrary since its only for ordering 
static char piece_value(Board * board, uint64_t pos){

	if(board->pawns & pos)
		return 'P';
	else if(board->knights & pos)
		return 'N';
	else if(board->bishops & pos)
		return 'B';
	else if(board->rooks & pos)
		return 'R';
	else if(board->queens & pos)
		return 'Q';
	else if(board->kings & pos)
		return 'K';
	else 
		return 0;
}

// Translate move to format "move x#y#"
static char* translate_comp(uint16_t move_made){
	
	char * response = malloc(sizeof(char) * 6);

	// octal format rocks
	response[2] = (move_made & 07) + 'a';
	response[3] = ((move_made & 070) >> 3) + '1';
	response[0] = ((move_made & 0700) >> 6) + 'a';
	response[1] = ((move_made & 07000) >> 9) + '1';
	response[4] = (move_made & 070000) >> 12;
	
	switch(response[4]){
		case 0:
			response[4] = '\0';
			break;
		case 4:
			response[4] = 'q';
			break;
		case 5:
			response[4] = 'b';
			break;
		case 6:
			response[4] = 'n';
			break;
		case 7:
			response[4] = 'r';
			break;
	}

	response[5] = '\0';

	return response;
}
/*
#ifndef TEST_H_
#define TEST_H_

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "board_rep/board_rep.h"
#include "tree/tree.h"
#include "tree/tree_helper.h"
#include "xboard/xboard.h"

#define START1	0x2000000000000000
#define END1	0x0800000000000000
#define START2 	0x0400000000000000
#define END2	0x0100000000000000

void print_node(Node* node){
	print_board_and_moves(&node->status.board,node->status.position,0);
}

void print_node_check(Node* node) {
	printf("\nColor: %s\nIn check: %s\n", (node->status.color? "Black" : "White"), (read_check_status(&node->status) ? "Yes":"No"));
	print_board_and_moves(&node->status.board,0,0);
}

void test_castle() {
	// move things around 
	Board* board = new_game_board();

	uint64_t idx = START1;

	// move pieces out of way for castle testing
	for(idx = START1; idx != END1; idx >>= 1) {
		move_piece(board, WHITE, 0, idx, idx >> 16);
	}
	for(idx = START2; idx != END2; idx >>= 1){
		move_piece(board, WHITE, 0, idx, idx >> 16);
	}

	// build tree and print 
	new_game_tree();

	// don't make a habit of this, is essentially a memcpy
	root->status.board = *board;

	init_stat(&root->status);

	expand_leaf(root);

	for_each_child(root, print_node);
}

void remove_pieces(Board * board, uint64_t pieces) {
	board->all &= ~pieces;
	board->white &= ~pieces;
	board->black &= ~pieces;
	board->pawns &= ~pieces;
	board->rooks &= ~pieces;
	board->knights &= ~pieces;
	board->bishops &= ~pieces;
	board->queens &= ~pieces;
	board->kings &= ~pieces;
}

void test_castle_legality(){

	// move things around 
	Board* board = new_game_board();

	uint64_t idx = START1;

	// move pieces out of way for castle testing
	for(idx = START1; idx != END1; idx >>= 1) {
		move_piece(board, WHITE, 0, idx, idx >> 16);
	}
	for(idx = START2; idx != END2; idx >>= 1){
		move_piece(board, WHITE, 0, idx, idx >> 16);
	}

	remove_pieces(board, 0x00ffff0000000000);

	move_piece(board, BLACK, 0, 0x0000000000000001, 0x0000000000040000);

	printf("Initial Conditions:\n");
	print_board_and_moves(board,0,0);

	// build tree and print 
	new_game_tree();

	root->status.board = *board;

	init_stat(&root->status);

	expand_leaf(root);

	printf("\npossible moves:\n");

	for_each_child(root, print_node);	
}

void test_en_passant(){
	new_game_tree();

	expand_leaf(root);

	Node * base2 = root->first_child->next_sibling;

	expand_leaf(base2);

	Node * base3 = base2->first_child;

	int i; 
	for(i = 0; i < 7; i++){
		base3 = base3->next_sibling;
	}

	expand_leaf(base3);

	Node * base4 = base3->first_child->next_sibling;
	change_turn(base4);
	base4->status.b_passant = 0x02;
	expand_leaf(base4);

	for_each_child(base4, print_node);
}

void testCheckDetection() {

	// initialize game tree
	new_game_tree();

	expand_leaf(root);

	Node * base2 = root->first_child;

	int i; 
	for(i = 0; i < 6; i++){
		base2 = base2->next_sibling;
	}

	expand_leaf(base2);

	Node * base3 = base2->first_child;

	for(i = 0; i < 8; i++){
		base3 = base3->next_sibling;
	}

	expand_leaf(base3);

	Node * base4 = base3->first_child;

	for(i = 0; i < 27; i++){
		base4 = base4->next_sibling;
	}

	expand_leaf(base4);

	Node * base5 = base4->first_child;

	expand_leaf(base5);

	Node * base6 = base5->first_child;

	print_node_check(root);
	print_node_check(base2);
	print_node_check(base3);
	print_node_check(base4);
	print_node_check(base5);
	print_node_check(base6);

}

void test_legality() {

	// move things around
	Board* board = new_game_board();

	// move bishop into place
	move_piece(board, BLACK, 0, 0x0000000000000004, 0x0000000100000000);

	// move pawn out of place
	move_piece(board, WHITE, 0, 0x0004000000000000, 0x0000040000000000);

	printf("initial condition:\n");

	// print initial condition
	print_board_and_moves(board,0,0);

	printf("\nsolutions:\n");

	// initialize game tree
	new_game_tree();

	// don't make a habit of this, is essentially a memcpy, slow
	root->status.board = *board;

	init_stat(&root->status);

	expand_leaf(root);

	for_each_child(root, print_node);
}

#endif
*/
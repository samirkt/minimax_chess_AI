
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "board_rep/board_rep.h"
#include "tree/structures.h"
#include "tree/tree_helper.h"
#include "queue.h"

#define SPECIAL	0
#define VICTIM 	1
#define ATTAKER	2

static int cmpfunc(const void * a, const void * b){
	return (*(Node_o**) a)->scores - (*(Node_o**)b)->scores;
}

void pq_free(PriorityQ * q){
	int i;
	
	if(!q)
		return;

	for(i = 0; i <= q->idx; ++i){
		free(q->table[i]);
	}

	q->idx = -1;
}

void pq_push (PriorityQ * q, Node_o * node){

	assert(q->idx < 126);
	assert(q->dirty); // can change to := 1 after testing

	q->nothing_generated = 0;

	// pre-increment
	q->table[++q->idx] = node;
}

Node_o* pq_pop (PriorityQ * q){

	if(q->idx < 0)
		return NULL;

	// make sure its sorted (aka not dirty)
	if(q->dirty){
		qsort(q->table, q->idx + 1, sizeof(Node_o*), cmpfunc);
		q->dirty = 0;
	}
	
	// decrements after indexing
	return q->table[q->idx--];
}

PriorityQ* pq_new (){
	PriorityQ * q = (PriorityQ *) malloc(sizeof(PriorityQ));
	q->idx = -1;
	q->new_search = 1;
	q->nothing_generated = 1;
	q->dirty = 1;
	return q;
}

void pq_init (PriorityQ * q){
	q->idx = -1;
	q->new_search = 1;
	q->nothing_generated = 1;
	q->dirty = 1;
}

void pq_destroy	(PriorityQ * q){
	pq_free(q);
	free(q);
}

uint8_t pq_size (PriorityQ * q){
	return q->idx + 1;
}

// weights are kinda arbitrary since its only for ordering 
static int16_t piece_value(Board * board, uint64_t pos){

	if(board->pawns & pos)
		return 1;
	else if(board->knights & pos)
		return 2;
	else if(board->bishops & pos)
		return 3;
	else if(board->rooks & pos)
		return 4;
	else if(board->queens & pos)
		return 5;
	else if(board->kings & pos)
		return 6;
	else 
		return 0;
}

void compute_scores(Node_o * node, Node_o * parent, uint64_t old, uint64_t new, uint8_t flags){

	uint64_t p_other;

	node->scores = 0;

	if(flags & (CASTLE_L | CASTLE_R | XFORM))
		node->scores |= 0100;

	if(node->node.status.color)
		p_other = parent->node.status.board.white;
	else
		p_other = parent->node.status.board.black;

	// if has victim
	if(new & p_other){
		node->scores |= piece_value(&parent->node.status.board, new) << 3;
		node->scores |= 7 - piece_value(&parent->node.status.board, old);
	}

	// passant take
	else if(flags & TAKE_PASSANT) {
		node->scores |= 010;
		node->scores |= 06;
	}
}


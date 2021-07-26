
#include <stdlib.h>
#include <stdint.h>

#include "board_rep/board_rep.h"
#include "bits/bitscan.h"
#include "tree/structures.h"
#include "tree/tree.h"
#include "tree/tree_helper.h"
#include "structs/hash_table/hash.h"

// prototypes
static void pre_compute(Status * status, MvHelp * helper);


void generate_moves(Node_s * search){

	Node_i * current = &search->stack[search->s_ptr];
	MvHelp helper;

	// precompute some widely re-used variables
	pre_compute(&current->node->status , &helper);

	uint64_t pieces = helper.same;

	while(pieces){

		uint64_t idx;

		reverse_n_squash(idx, pieces);
		
		current->position = 0x1UL << idx;

		move(search, &helper);
	}
}

#define TOP 		0x00000000000000ff
#define BOTTOM 		0xff00000000000000
#define B_FILTER	0x000000ff00000000
#define W_FILTER	0x00000000ff000000

static void pre_compute(Status * status, MvHelp * helper){

	helper->pos = status->passant;

	if(status->color){
		helper->pos <<= 48;
		helper->same = status->board.black;
		helper->other = status->board.white;
		helper->boundary = BOTTOM;
		helper->filter = B_FILTER;
		helper->start = 0x000000000000ff00;
		helper->h_idx_color = H_BLACK;
	}
	else {
		helper->pos <<= 16;
		helper->same = status->board.white;
		helper->other = status->board.black;
		helper->boundary = TOP;
		helper->filter = W_FILTER;
		helper->start = 0x00ff000000000000;
		helper->h_idx_color = H_WHITE;
	}

	helper->nsame = ~helper->same;
}
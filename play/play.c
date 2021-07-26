#include <stdlib.h>
#include <inttypes.h>
#include "../board_rep/board_rep.h"
#include "../tree/structures.h"
#include "../tree/tree.h"

#include "play.h"

uint16_t make_move(){

	uint16_t new = fixed_mem_AB_search(INF_N, INF_P, TREE_DEPTH, root);

	apply_move(new);
	move_count++;

	return new;
}


void new_game() {
	
	if(root)
		free(root);

	new_game_tree(TREE_DEPTH);
}


void do_move(uint16_t move_made){

	apply_move(move_made);
	move_count++;
}

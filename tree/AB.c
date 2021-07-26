
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "board_rep/board_rep.h"
#include "bits/bitscan.h"

#include "structures.h"
#include "tree.h"
#include "tree_helper.h"
#include "priority/queue.h"
#include "structs/hash_table/hash.h"
#include "x-pos/htable.h"

// stubs
static Node_s * init_search	(Node * start, uint8_t to_depth, int16_t alpha, int16_t beta);
static void 	init_iterator		(Node_s * search, Node_o * node);
static void 	evaluate_current	(Node_s * search);
static void 	returnUp			(Node_s * search);
static void 	update_AB			(Node_s * search);
static void 	free_child			(Node_s * search);
static void 	clean_up			(Node_s * search);
static int8_t	check_hash			(Node_s * search, uint8_t isleaf);
static void 	update_hash			(Node_s * search);
static void		end_of_game			(Node_s * search);
static Node* 	validate_hash		(Node_s * search, uint8_t isleaf);
static void 	decrement_ptr		(Node_s * search);

/*
 *  Function is called at the beginning of every new game.
 * 	It allocates a Node to act as root, and initializes the Board
 *  and parameters for a new game. Sets root.
 */
void new_game_tree(uint8_t depth) {

	// allocate root node
	root = (Node*) malloc(sizeof(Node));

	// initialize pieces
	init_new_game(&root->status.board);

	// initialize status bits
	root->status.passant = 0;
	root->status.WCL = 1;
	root->status.WCR = 1;
	root->status.BCL = 1;
	root->status.BCR = 1;
	root->status.WCheck = 0;
	root->status.BCheck = 0;
	root->status.color = 0;

	// initialize root node variables
	root->alpha = INF_N;
	root->beta = INF_P;
	root->move_to_make = 0;
	root->depth_searched = depth;

	htable_init(1024);
}

/*
 *  Called once at the beginning of every fixed_mem_AB_search(), it
 *  allocates a Node_s (search object), and within that an arrasy of Node_i (node iterators)
 * 	to act as the search stack. It then initializes the stack pointer, copies starting node
 *	into the base of the search stack, and initializes its iterator 
 */
static Node_s * init_search(Node * start, uint8_t to_depth, int16_t alpha, int16_t beta){

	// allocate space, for search object and search stack
	Node_s * search = (Node_s *) malloc(sizeof(Node_s));
	search->stack = (Node_i*) malloc(sizeof(Node_i)*(to_depth+1));
	search->to_depth = to_depth;

	int i;
	// skip last depth, leaves don't need priority queue
	for(i = 0; i < to_depth; ++i){
		search->stack[i].queue = pq_new();
	}
	search->stack[to_depth].queue = NULL;

	// initialize stack ptr
	search->s_ptr = 0;

	// allocate space for node_o and initialize pointers
	search->stack[0].obj = (Node_o *) malloc(sizeof(Node_o));
	search->stack[0].node = &search->stack[0].obj->node;

	// copy over start node
	memcpy(search->stack[0].node, start, sizeof(Node));

	// initialize root hash
	search->stack[0].obj->hash = hash_gen(&search->stack[0].node->status);

	// initialize start node
	search->stack[0].alpha = alpha;
	search->stack[0].beta = beta;

	// return search object
	return search;
}


/*
 *	Initializes a new search iterator on the search stack,
 * 	assumes that s_ptr now points to new slot, inherits A & B
 *	parameters from parent (s_ptr - 1).
 * 
 * 	This is called everytime we select a new child move to explore,
 * 	it initializes the Node_i for the new move.
 */
static void init_iterator(Node_s * search, Node_o * node){

	Node_i * iterator = &search->stack[search->s_ptr];
	Node_i * parent = &search->stack[search->s_ptr - 1];

	iterator->node = &node->node;
	iterator->obj = node;

	iterator->alpha = parent->alpha;
	iterator->beta = parent->beta;

	// black
	if(iterator->node->status.color){
		// alpha is bound, beta is min
		iterator->node->alpha = iterator->alpha;
		iterator->node->beta = INF_P; // used for true min in x-pos table
	}

	// white
	else{
		// beta is bound, alpha is max
		iterator->node->beta = iterator->beta;
		iterator->node->alpha = INF_N; // used for true min in x-pos table
	}

	// leaf nodes don't have queues to initialize
	if(iterator->queue){
		pq_init(iterator->queue);
	}
}

/*
 *	assumes s_ptr points at node to be evaluated, sets score in
 * 	s_ptr - 1  =  eval(s_ptr).
 * 	
 * 	The function is called on leaf nodes at search depth,
 * 	i.e. s_ptr = to_depth
 */
static void evaluate_current(Node_s * search){
	
	Node_i * parent = &search->stack[search->s_ptr - 1];
	Node_i * child = &search->stack[search->s_ptr];

	uint16_t temp = evaluate_board(
		&child->node->status.board,
		 child->node->status.color
	);

	parent->temp = temp;
	parent->temp_move = child->obj->move_made;

	if(child->node->status.color){

		child->node->beta = temp;
		
		// no bound for leaf
		child->node->alpha = INF_N; 

	} else{

		child->node->alpha = temp;

		// no bound for leaf
		child->node->beta = INF_P;
	}
}


/*
 *	This function is called once all child nodes/leaves have been searched
 * 	and the node is ready to return it's A/B up one level to its parent.
 * 
 *	Assumes s_ptr points at child node, and s_ptr - 1 is parent. 
 */
static void returnUp(Node_s * search){

	Node_i *current = &search->stack[search->s_ptr];
	Node_i *parent = &search->stack[search->s_ptr - 1];

	parent->temp_move = current->obj->move_made;

	// no moves from here
	if(current->queue && current->queue->nothing_generated){

		// TODO: differentiate between losing and stalemate
		// by checking if we're in check
		// right now it drives for both

		// black, minimizing, lost
		if(current->node->status.color){
			parent->temp = INF_P;
		}

		// white, maximizing, lost
		else {
			parent->temp = INF_N;
		}

		return;
	}

	// black, minimizing
	if(current->node->status.color){
		parent->temp = current->beta;
	}

	// white, maximizing
	else {
		parent->temp = current->alpha;
	}

}

/*	
 * 	This function is called after a child has returned a score
 * 	and assumes that s_ptr points at parent node
 */
static void update_AB(Node_s * search){

	Node_i *current = &search->stack[search->s_ptr];

	// black == minimizing
	if(current->node->status.color){

		// fail hard alpha-cutoff
		if( current->temp <= current->alpha && search->s_ptr > 0){
			decrement_ptr(search);
			return;
		}

		// beta acts like min in MiniMax
		if( current->temp < current->beta ){
			current->beta = current->temp;
			current->node->move_to_make = current->temp_move;
		}

		// true min for x-pos table score
		if(current->temp < current->node->beta){
			current->node->beta = current->temp;
		}
	}

	// white == maximizing
	else {

		// fail hard beta-cutoff
		if( current->temp >= current->beta && search->s_ptr > 0){
			decrement_ptr(search);
			return;
		}

		// alpha acts like max in MiniMax
		if( current->temp > current->alpha ){
			current->alpha = current->temp;
			current->node->move_to_make = current->temp_move;
		}

		// true max for x-pos table score
		if(current->temp > current->node->alpha){
			current->node->alpha = current->temp;
		}
	}
}

/*
 * 	This is called after update_AB, s_ptr points to parent
 * 	s_ptr - 1 is valid child that has completed search. This
 * 	function deallocates move ordering node (Node_o) 
 */
static void free_child(Node_s * search) {
	Node_i *child = &search->stack[search->s_ptr + 1];
	free(child->obj);
}

static void clean_up(Node_s * search){
	
	// deallocate all priority queues
	int i;
	for(i = 0; i < search->to_depth; ++i){
		pq_destroy(search->stack[i].queue);
	}

	free(search->stack[0].obj);
	
	// deallocate search stack and search object
	free(search->stack);
	free(search);
}

// short circuiting and smart ordering should make this fast
inline static uint8_t equal_status(Status * a, Status * b){
	return 
		a->board.all == b->board.all &&
		a->color == b->color &&
		a->board.white == b->board.white &&
		a->board.black == b->board.black;
}

uint64_t found = 0;
uint64_t collisions = 0;

static Node* validate_hash(Node_s * search, uint8_t isleaf){

	Node * node;

	Node_i * current = &search->stack[search->s_ptr];
	uint8_t depth_needed = current->node->depth_searched;

	// entry at index
	if(!(node = htable_find(current->obj->hash)))
		return NULL;

	// collision!
	if(!equal_status(&node->status, &current->node->status)){
		collisions++;
		return NULL;
	}
	found++;

	// too shallow a search
	if(node->depth_searched < depth_needed)
		return NULL;

	// skip bounds check if leaf node
	if(isleaf)
		return node;

	// check bounds aren't too strict

	// black
	if(node->status.color){
		if(node->alpha < current->node->alpha)
			return NULL;
	}

	// white
	else {
		if(node->beta > current->node->beta)
			return NULL;
	}

	return node;
}

/*
 *	Checks if current Node_i is in the x-position table, has
 * 	been searched to sufficient depth, and that the search wasn't too
 * 	restrictive for our current search. 
 *	if its a leaf we node we don't care about bounds for lookup, we
 * 	need to pass this info along to validate_hash() call
 *	If so, it can:
 *		hit cutoff 				: return -1
 *		update alpha and beta 	: return 1
 * 		else 					: return 0.
 */
static int8_t check_hash(Node_s * search, uint8_t isleaf){

	assert(search);

	Node_i * current = &search->stack[search->s_ptr];
	Node * node = validate_hash(search, isleaf);

	if(!node)
		return 0;

	// VALID ENTRY FOUND:

	// leaves don't have moves
	if(!isleaf){

		// assert(node->move_to_make); // TODO
		current->queue->nothing_generated = 0;
		current->node->move_to_make = node->move_to_make;
	}

	if(node->status.color){
		if(node->beta < current->alpha) // cutoff
			return -1;

		if(node->beta < current->beta)
			current->beta = node->beta;

	} else {
		if(node->alpha > current->beta) // cutoff
			return -1;

		if(node->alpha > current->alpha)
			current->alpha = node->alpha;
	}

	return 1;
}

/*
 *	Assumes s_ptr points at node to insert into table
 *	current assumption is that this is called only on nodes
 *	that were computed, indicating that the entry did not exist
 *	in the hash table or was not deep/general enough.
 *	thus we can insert into hash table without any checks
 */
static void update_hash(Node_s * search){

	Node_i * current = &search->stack[search->s_ptr];

	htable_insert(current->obj->hash, current->node);
}

static void decrement_ptr(Node_s * search){

	// free any elements left in priority q
	pq_free(search->stack[search->s_ptr].queue);

	// free current obj pointed to in iterator
	free(search->stack[search->s_ptr--].obj);
}

uint64_t hits = 0;
uint64_t misses = 0;

/*
 *	Iterative implementation of AB search algorithm using fixed size memory stack.
 * 	Note, heaps used for move ordering have a fixed size of 127 * sizeof(Node_o).
 *  Maximum # of possible moves in real gameplay ever recorded = 79. This is still
 *	a little risky. Safer to use 256, but this conserves x2 memory
 */
uint16_t fixed_mem_AB_search(int16_t alpha, int16_t beta, uint8_t to_depth, Node * start){

	hits = 0;
	found = 0;
	misses = 0;
	collisions = 0;

	// initialize search object
	Node_s * search = init_search(start, to_depth, alpha, beta);
	Node_i * current;

	while(1){

		// end of depth
		if(search->s_ptr == to_depth){
			
 			// check for value in x-pos table, 1 -> leaf node
 			int8_t m = check_hash(search, 1);

 			switch(m){
 				case -1: // cutoff
 					hits++;
 					decrement_ptr(search);
					break;
 				case 0: // notfound
 					misses++;
 					evaluate_current(search);
 					update_hash(search);
					decrement_ptr(search);
					update_AB(search);
					break;
 				case 1: // found
 					hits++;
 					returnUp(search);
					decrement_ptr(search);
					update_AB(search);
					break;
 			}
		}

		current = &search->stack[search->s_ptr];

		// moves haven't been generated yet
		if(current->queue->new_search){

			// unset new_search
			current->queue->new_search = 0;

			// check for value in x-pos table, 0 -> not leaf
			int8_t m = check_hash(search, 0);

			switch(m){

				case -1: 	// cutoff
					hits++;
					decrement_ptr(search);
					continue;
		
				case 0: 	// not found
					// sets/unsets nothing_generated flag
					misses++;
					generate_moves(search);
					break;
		
				case 1: 	// ok
					hits++;
					returnUp(search);
					decrement_ptr(search);
					update_AB(search);
					continue;
			}
		}

		Node_o * next_move;

		// there are moves left
		if((next_move = pq_pop(current->queue))){
			search->s_ptr++;
			init_iterator(search, next_move);
			continue;
		}

		// no moves left & not root
		if(search->s_ptr){
			update_hash(search);
			returnUp(search);
			decrement_ptr(search);
			update_AB(search);
			continue;
		}

		// no moves left & root & nothing generated
		if(current->queue->nothing_generated){
			end_of_game(search);
		}

		// no moves left, root, and moves were generated
		uint16_t ret = search->stack[search->s_ptr].node->move_to_make;
		printf("h: %lx, m: %lx, f: %lx, c: %lx\n", hits, misses, found, collisions);
		clean_up(search);
		return ret; 
	}
}

/* 
 * TODO: update how we handle end of game, integrate with
 * xboard end of game
 */ 
static void	end_of_game(Node_s * search){

	Node_i * current = &search->stack[search->s_ptr];

	// black
	if(current->node->status.color){

		// checkmate
		if(current->node->status.BCheck){
			printf("\nGame Over. Black lost.\n");
		}

		// stalemate
		else {
			printf("\nGame Over. Stalemate.\n");
		}

	} 

	// white
	else{

		// checkmate
		if(current->node->status.WCheck){
			printf("\nGame Over. White lost.\n");
		}

		// stalemate
		else {
			printf("\nGame Over. Stalemate.\n");
		}						
	}

	exit(0);
}
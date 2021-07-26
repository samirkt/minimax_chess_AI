
#ifndef TREE_TREE_H_
#define TREE_TREE_H_

#include <stdint.h>
#include "structures.h"

#define UNARY_OR(x)		(0 != (x))
#define INF_P	INT16_MAX
#define INF_N	INT16_MIN

Node* root;

uint8_t r_print;

// initializes a new tree and sets root variable
void new_game_tree(uint8_t depth);

// searches with fixed mem = to_depth * sizeof(Node_i)
// use INF_P/INF_N defines as initial beta/alpha
uint16_t fixed_mem_AB_search(int16_t alpha, int16_t beta, uint8_t to_depth, Node * start);

void htable_delete();

#endif

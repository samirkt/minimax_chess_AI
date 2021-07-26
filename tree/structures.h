
#ifndef TREE_STRUCTS_H_
#define TREE_STRUCTS_H_

#include <stdint.h>
#include "board_rep/board_rep.h"

// number of things that we'll be ordering by
#define ORDER_PARAMS	3

// data used for representing a board and game status
typedef struct {
	Board board;
	uint8_t passant;

	// 7 bits -> 1 byte, yay bitfields
	uint8_t WCL : 1;	// White Castle Left
	uint8_t WCR : 1;	// White Castle Right
	uint8_t BCL : 1;	// Black Castle Left
	uint8_t BCR : 1;	// Black Castle Right
	uint8_t WCheck : 1;	// White in Check
	uint8_t BCheck : 1;	// Black in Check
	uint8_t color : 1;	// Whose turn 0 -> white, 1 -> black
} Status;

// data used for x-pos tables
// alpha/beta are start bound and true max/min
typedef struct {
	Status status;
	int16_t alpha;
	int16_t beta;
	uint16_t move_to_make;
	uint8_t depth_searched;
} Node;

// data used for move generation and ordering
typedef struct {
	Node node;
	uint64_t hash;
	uint16_t move_made;
	uint8_t scores;
} Node_o;

typedef struct {
	Node_o *table[127];  // array of pointers
	int16_t idx;
	uint8_t new_search : 1;
	uint8_t nothing_generated : 1;
	uint8_t dirty : 1;
} PriorityQ;

// data used for AB algorithm
typedef struct {
	Node * node;
	Node_o * obj;
	PriorityQ * queue;
	uint64_t position;
	int16_t alpha;
	int16_t beta;
	int16_t temp;
	int16_t temp_move;
} Node_i;

// used for holding AB search stack
typedef struct {
	Node_i * stack;
	uint8_t s_ptr;
	uint8_t to_depth;
} Node_s;

#endif
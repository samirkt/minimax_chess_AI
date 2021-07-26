
#ifndef STRUCTS_HASH_TABLE_HASH_H_
#define STRUCTS_HASH_TABLE_HASH_H_

#include "tree/structures.h"

// piece indices
#define WPAWN	0
#define WROOK	1
#define WKNIGHT	2
#define WBISHOP	3
#define WQUEEN	4
#define WKING 	5
#define BPAWN	6
#define BROOK	7
#define BKNIGHT	8
#define BBISHOP	9
#define BQUEEN	10
#define BKING 	11

// additive indices, color + piece = index
#define H_WHITE		0
#define H_BLACK		6
#define H_PAWN		0
#define H_ROOK		1
#define H_KNIGHT	2
#define H_BISHOP	3
#define H_QUEEN		4
#define H_KING 		5

// castle directions
#define WCASTLEL	0
#define WCASTLER	1
#define BCASTLEL	2
#define BCASTLER	3

void 	 hash_init  		();
uint64_t hash_gen 			(Status * status);

// for incremental updates
uint64_t hash_piece 		(uint64_t hash, uint64_t pos, uint8_t type);
uint64_t hash_piece_idx		(uint64_t hash, uint8_t idx, uint8_t type) ;
uint64_t hash_move_piece	(uint64_t hash, uint64_t old, uint64_t new, uint8_t type);
uint64_t hash_castle		(uint64_t hash, uint8_t castle_dir);
uint64_t hash_passant 		(uint64_t hash, uint8_t passant);
uint64_t hash_passant_change(uint64_t hash, uint8_t old, uint8_t new);
uint64_t hash_change_turn	(uint64_t hash);

#endif
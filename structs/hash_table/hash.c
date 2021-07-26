#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "board_rep/board_rep.h"
#include "bits/bitscan.h"
#include "rand/rand.h"
#include "tree/structures.h"
#include "tree/tree.h"
#include "hash.h"

// prototypes
static void add_idx(int idx, void * hlpr);

// globals
static uint64_t key_table[12][64];
static uint64_t isblack;
static uint64_t castle_key[4];
static uint64_t passant_col[8];

// TODO: store seed to file for debug

typedef struct {
	uint64_t hash;
	uint8_t piece;
} HashHelper;

void hash_init(){	

	// initialize key_table
	int i, j;
	for(i = 0; i < 12; ++i){
		for(j = 0; j < 64; ++j){
			key_table[i][j] = rand64();			
		}
	}

	isblack = rand64();

	for(i = 0; i < 3; ++i){
		castle_key[i] = rand64();
	}

	for(i = 0; i < 8; ++i){
		passant_col[i] = rand64();		
	}
}

static void add_idx(int idx, void * hlpr){

	// typecast
	HashHelper * helper = hlpr;

	// xor in random value for piece and pos
	helper->hash ^= key_table[helper->piece][idx];
}

#define WLC		1
#define WRC		2
#define BLC 	4
#define BRC		8

uint64_t hash_gen(Status * status){

	HashHelper helper;
	uint64_t color;
	uint8_t count;

	helper.hash = 0;
	helper.piece = WPAWN;

	// white and black
	for(count = 0; count < 2; ++count){

		// set mask for white or black pieces
		if(count)
			color = status->board.black;
		else
			color = status->board.white;

		// iterate over sets of pieces
		for_each_bit_r(color & status->board.pawns, &helper, add_idx);
		helper.piece++;

		for_each_bit_r(color & status->board.rooks, &helper, add_idx);
		helper.piece++;

		for_each_bit_r(color & status->board.knights, &helper, add_idx);
		helper.piece++;

		for_each_bit_r(color & status->board.bishops, &helper, add_idx);
		helper.piece++;

		for_each_bit_r(color & status->board.queens, &helper, add_idx);
		helper.piece++;

		for_each_bit_r(color & status->board.kings, &helper, add_idx);
		helper.piece++;
	}

	int idx;

	// black
	if(status->color){
		helper.hash ^= isblack;
	}

	if(status->passant){
		reverse_bit_scan(idx,status->passant);
		helper.hash ^= passant_col[idx];
	}
	
	if(status->WCL) {
		helper.hash ^= castle_key[WCASTLEL];
	}

	if(status->WCR) {
		helper.hash ^= castle_key[WCASTLER];
	}

	if(status->BCL) {
		helper.hash ^= castle_key[BCASTLEL];
	}

	if(status->BCR) {
		helper.hash ^= castle_key[BCASTLER];
	}

	return helper.hash;
}

uint64_t hash_piece(uint64_t hash, uint64_t pos, uint8_t type){
	uint8_t idx;
	reverse_bit_scan(idx, pos);
	hash ^= key_table[type][idx];

	return hash;
}

uint64_t hash_piece_idx(uint64_t hash, uint8_t idx, uint8_t type) {
	return hash ^ key_table[type][idx];
}

uint64_t hash_move_piece(uint64_t hash, uint64_t old, uint64_t new, uint8_t type){
	uint8_t idx;

	reverse_bit_scan(idx, old);
	hash ^= key_table[type][idx];

	reverse_bit_scan(idx, new);
	return hash ^ key_table[type][idx];
}

uint64_t hash_castle(uint64_t hash, uint8_t castle_dir){
	return hash ^ castle_key[castle_dir];
}

uint64_t hash_passant(uint64_t hash, uint8_t passant){
	int idx;

	if(passant){
		reverse_bit_scan(idx,passant);
		hash ^= passant_col[idx];
	}

	return hash;	
}

uint64_t hash_passant_change(uint64_t hash, uint8_t old, uint8_t new){

	int idx;

	if(old){
		reverse_bit_scan(idx,old);
		hash ^= passant_col[idx];
	}

	if(new){
		reverse_bit_scan(idx,new);
		hash ^= passant_col[idx];	
	}

	return hash;
}

uint64_t hash_change_turn(uint64_t hash){
	return hash ^ isblack;
}
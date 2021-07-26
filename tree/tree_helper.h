
#ifndef TREE_HELPER_H_
#define TREE_HELPER_H_

#include "tree.h"

/*
 * intermediate header for this folder
 * defines functions and macros used across
 * this folder that we don't want/need public in tree.h
 */ 

//flags, do not edit
#define NOOP			0x00
#define ADD_PASSANT 	0x01
#define TAKE_PASSANT	0x02
#define XFORM 			0x04
#define REVOKE_R		0x08
#define REVOKE_L		0x10
#define CASTLE_L		0x20
#define CASTLE_R 		0x40

// xform type, might be used in xboard?
#define X_QUEEN			0x08
#define X_BISHOP		0x10
#define X_KNIGHT		0x20
#define X_ROOK			0x40

// check status
#define NO_CHECK	0x00
#define CHECK		0x01

// used in generate folder
typedef struct {
	uint64_t same;
	uint64_t nsame;
	uint64_t other;
	uint64_t pos;
	uint64_t boundary;
	uint64_t filter;
	uint64_t start;
	uint8_t h_idx_color;
} MvHelp;

// generate/validate.c
void add_move(Node_s* search, uint64_t taken, uint64_t old, uint64_t new, uint8_t flags, uint8_t p_idx);
// generate/generate.c
void generate_moves(Node_s * search);

// generate/move.c
void move(Node_s* search, MvHelp * helper);

// legality.c
uint8_t is_legal(Status *status);
uint8_t castle_is_legal(Status * status, uint8_t direction);

#endif
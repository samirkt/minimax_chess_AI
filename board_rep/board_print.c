
#include "board_rep.h"

#include <stdio.h>

/*
    A  B  C  D  E  F  G  H  
  -------------------------- 
8 | O  O  O  O  O  O  O  O |
7 | O  O  O  O  O  O  O  O |
6 | O  O  O  O  O  O  O  O |
5 | O  O  O  O  O  O  O  O |
4 | O  O  O  O  O  O  O  O |
3 | O  O  O  O  O  O  O  O |
2 | O  O  O  O  O  O  O  O |
1 | O  O  O  O  O  O  O  O |
  --------------------------
*/

// colors should work on windows powershell and most unix terminals

// i know too many colors, but it might be useful
// plus this won't be part of end product

#define RESET 	0
#define WHT 	1
#define RED 	2
#define GRN 	3
#define BLU 	4
#define CYN 	5
#define YEL 	6
#define LGRY 	7
#define DGRY 	8
#define LRED 	9
#define LGRN 	10
#define LBLU 	11
#define LMAG 	12
#define LCYN 	13
#define HGRNW 	14
#define HGRNB	15

#define C_RESET 	"\x1B[0m"
#define C_WHT		"\x1B[97m" 
#define C_RED		"\x1B[31m" 
#define C_GRN		"\x1B[32m" 
#define C_BLU		"\x1B[34m" 
#define C_CYN		"\x1B[36m" 
#define C_YEL		"\x1B[93m" 
#define C_LGRY		"\x1B[37m" 
#define C_DGRY		"\x1B[90m" 
#define C_LRED		"\x1B[91m" 
#define C_LGRN		"\x1B[92m" 
#define C_LBLU		"\x1B[94m" 
#define C_LMAG		"\x1B[95m" 
#define C_LCYN		"\x1B[96m"
#define C_HGW		"\x1B[41m"
#define C_HGB		"\x1B[41m\x1B[30m" 

static char *colors[17] = {C_RESET,C_WHT,C_RED,C_GRN,C_BLU,C_CYN,C_YEL,C_LGRY,C_DGRY,C_LRED,C_LGRN,C_LBLU,C_LMAG,C_LCYN,C_HGW,C_HGB};

// return variable :
// lower byte for piece character
// upper byte to index into colors
uint16_t find_type(Board * board, uint64_t idx, uint64_t position, uint64_t moves) {

	uint16_t type = 0;
	char white = 0 || (board->white & position);

	// current position
	if(white && (idx & position)) type |= HGRNW;
	else if (!white && (idx & position)) type |= HGRNB;

	// can take current piece
	else if ( (white && (idx & board->black & moves)) || (!white && (idx & board->white & moves)) ) 
		type |= LRED;
	
	// normal piece 
	else if (idx & board->white) type |= WHT;
	else if (idx & board->black) type |= LMAG;

	// valid move
	else if (idx & moves) type |= LGRN;

	// empty place
	else type |= CYN;


	type = type << 8;

	// find type
	if 		(idx & board->pawns) 	type |= 'P';
	else if (idx & board->rooks)  	type |= 'R';
	else if (idx & board->knights)  type |= 'N';
	else if (idx & board->bishops)  type |= 'B';
	else if (idx & board->queens)  	type |= 'Q';
	else if (idx & board->kings)  	type |= 'K';
	else 							type |= '-';
		
	return type;
}

// extracts piece character from return value of find_type()
char get_piece_character(uint16_t piece_type){
	return (char) piece_type;
}

// extracts color index from return value of find_type()
uint8_t get_piece_color(uint16_t piece_type){
	return (uint8_t) (piece_type >> 8);
}

// prints out board and colorizes
void print_board_and_moves(Board * board, uint64_t position, uint64_t moves) {
	uint64_t init = 0x8000000000000000;	
	uint64_t idx;
	int i;
	int j;

	uint16_t type;

	// header
	printf("  --------------------------\n");


	// rows
	for(i = 8; i > 0; i--) {

		idx = init >> (8 * (i -1));

		printf("%d |", i);

		// columns
		for(j = 0; j < 8; j++) {
			
			type = find_type(board, idx, position, moves);

			printf("%s %c %s", colors[get_piece_color(type)], get_piece_character(type), C_RESET);

			idx = idx >> 1;
		}

		printf("|\n");
		idx = idx << 15;
	}

	// footer
	printf("  --------------------------\n    A  B  C  D  E  F  G  H  \n");

}

// prints out representation and colorizes 1's 
void print_num(uint64_t num) {
	uint64_t init = 0x8000000000000000;
	uint64_t idx;
	int i;
	int j; 

	printf("--------------------------\n");

	// rows
	for(i = 8; i > 0; i--) {

		idx = init >> (8 * (i -1));

		printf("|");

		// columns
		for(j = 0; j < 8; j++) {
			
			if(idx & num) printf("\x1B[31m" " 1 " "\x1B[0m");
			else printf(" 0 ");  

			if(idx >> 1) idx = idx >> 1;
		}

		printf("|\n");
	}

	// footer
	printf("--------------------------\n");
}
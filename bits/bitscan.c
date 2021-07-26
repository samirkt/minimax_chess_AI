
#include "bitscan.h"

void for_each_bit_f(uint64_t bits, void* pass_along, void (*fcn)(int, void*)) {

	int idx;

	while(bits){

		forward_n_squash(idx,bits);
		
		fcn(idx, pass_along);
	}
}

void for_each_bit_r(uint64_t bits, void* pass_along, void (*fcn)(int, void*)) {

	int idx;

	while(bits){

		reverse_n_squash(idx,bits);
		
		fcn(idx, pass_along);
	}
}

uint8_t get_row(uint64_t pos){
	uint8_t idx;

	// find bit index
	forward_bit_scan(idx, pos);

	// could possibly make this idx >> 3;
	return (idx & 0x38) >> 3;
}

uint8_t get_column(uint64_t pos){
	uint8_t idx;

	forward_bit_scan(idx,pos);

	return (idx & 0x07);
}

uint64_t build_pos(uint8_t row, uint8_t col) {

	uint64_t num = 0x8000000000000000;

	// mult row by 8 and shift
	num >>= (row << 3);
	num >>= col;

	return num;
}

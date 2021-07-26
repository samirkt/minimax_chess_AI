
#ifndef BITSCAN_H_
#define BITSCAN_H_

#include <stdint.h>

#define BT_STRT1	0x8000000000000000UL

// reverse is faster on intel
// forward is probably better supported on ARM

// these are simpler, and defined by compiler, and better type handled
#define forward_bit_scan(ret,val)		(ret = __builtin_clzl(val))
#define reverse_bit_scan(ret,val)		(ret = __builtin_ctzl(val))

#define reverse_n_squash(ret,val)		{reverse_bit_scan(ret,val); val &= val - 1;}
#define forward_n_squash(ret,val)		{forward_bit_scan(ret,val); val &= ~(BT_STRT1 >> ret);}

// forward / reverse : calls given function with index of next bit and optional pointer to pass along for parameters
void for_each_bit_f(uint64_t bits, void* pass_along, void (*fcn)(int, void*));
void for_each_bit_r(uint64_t bits, void* pass_along, void (*fcn)(int, void*));

// retreives row and column efficiently
uint8_t get_row(uint64_t pos);
uint8_t get_column(uint64_t pos);

// returns position from row and column indices
uint64_t build_pos(uint8_t row, uint8_t col);

#endif
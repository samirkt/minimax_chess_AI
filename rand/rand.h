
#ifndef STRUCT_RAND_RAND_H_
#define STRUCT_RAND_RAND_H_

#include <stdint.h>

/* 
 * ensures everything uses the same random seed
 * and that it is initialized once
 */

uint32_t rand32();
uint64_t rand64();

#endif
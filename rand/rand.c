
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "rand.h"

uint32_t init = 0;


static void init_rand_seed(){
	uint32_t seed = time(NULL);
	srandom(seed);
}

uint32_t rand32(){

	if(!init){
		init_rand_seed();
		init = 1;
	}
	
	return random();
}

uint64_t rand64(){
	
	if(!init){
		init_rand_seed();
		init = 1;
	}

	uint64_t num = 0;
	num = random() << 32;
	num |= random();
	return num;
}

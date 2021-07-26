#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "board_rep/board_rep.h"
#include "tree/tree.h"
#include "tree/tree_helper.h"

#include "xboard/xboard.h"

#include "test.h"

// put your tests in test.h
// or test_name.h
// and put function calls here
// keeps things cleaner
// && less conflicts
void lg(void);

int main (int argc, char *argv[]) {

	is_print = 0;

	if(argc > 1){
		if(!strcmp(argv[1], "-cli")){
			is_print = 1;
		}
	}

	xboard_prot();

    return 0;
}

void lg(){
	char* buffer;
	size_t buffersize = 32;
	buffer = (char*)malloc(buffersize * sizeof(char));
	getline(&buffer,&buffersize,stdin);
	FILE* fd = fopen("temp","w");
	fprintf(fd,"%sL",buffer);
	fclose(fd);
}

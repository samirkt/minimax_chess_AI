#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "../board_rep/board_rep.h"
#include "../bits/bitscan.h"
#include "../play/play.h"
#include "tree/tree.h"
#include "xboard.h"

// globals
static char response[6];

//stubs 
static char* translate_comp(uint16_t move_made);
static uint16_t translate_user(char* move);
static int is_letter(char c);
static int is_num(char c);

uint32_t count = 0;

void xboard_prot(){

	// If force asserted, computer never plays
	int force = 0; 
	uint16_t mv;

	new_game();

	char buffer[100];
	int l = 0;
	remove("test");

	//strcpy(buffer, "go");

	// Wait for instructions
	while(1){

		// if(count < 1){
		// 	count++;
		// } else {
		// 	break;
		// }

		scanf("%s", buffer);
		printf("\"%s\"\n",buffer);
		// fflush(stdout);
		// continue;
		l = strlen(buffer);

		FILE* fd = fopen("test","a");
		fprintf(fd,"%s\n",buffer);
		fclose(fd);

		if(!strcmp(buffer,"xboard")){
			fprintf(stdout,"feature sigint=0 sigterm=0 done=1\n");
		}
		else if(!strcmp(buffer,"go")){
			force = 0;

			mv = make_move();
			char* cm = translate_comp(mv);

			fprintf(stdout,"move %s\n", cm);
			fflush(stdout);

			if(is_print)
				print_board_and_moves(&root->status.board,0,0);
		}

		else if(!strcmp(buffer,"force")){
			force = 1;
		}

		// QUIT
		else if(!strcmp(buffer,"quit")){
			break;
		}

		// OPPONENT MOVE
		else if( (l == 4 || l == 5) && 
				is_letter(buffer[0]) &&
				is_num(buffer[1]) &&
				is_letter(buffer[2]) &&
				is_num(buffer[3]) &&
				(is_letter(buffer[5]) || buffer[5] == '\0')
				){

			do_move(translate_user(buffer));

			//print_board_and_moves(board,0,0);//&root->status.board,0,0);
			if(is_print)
				print_board_and_moves(&root->status.board,0,0);

			// respond
			if(!force){
				mv = make_move();

				char* cm = translate_comp(mv);
				fprintf(stdout,"move %s\n", cm);//make_move()));
				fflush(stdout);
				
				if(is_print)
					print_board_and_moves(&root->status.board,0,0);
			}
		}
	}

	htable_delete();

	free(root);
}

static int is_letter(char c){
	return (c >= 'a' && c <= 'z');
}

static int is_num(char c) {
	return (c >= '0' && c <= '9');
}

// Translate move to format "move x#y#"
static char* translate_comp(uint16_t move_made){
	
	// octal format rocks
	response[2] = (move_made & 07) + 'a';
	response[3] = ((move_made & 070) >> 3) + '1';
	response[0] = ((move_made & 0700) >> 6) + 'a';
	response[1] = ((move_made & 07000) >> 9) + '1';
	response[4] = (move_made & 070000) >> 12;
	
	switch(response[4]){
		case 0:
			response[4] = '\0';
			break;
		case 4:
			response[4] = 'q';
			break;
		case 5:
			response[4] = 'b';
			break;
		case 6:
			response[4] = 'n';
			break;
		case 7:
			response[4] = 'r';
			break;
	}

	response[5] = '\0';

	return response;
}



static uint16_t translate_user(char* move){
	int len = strlen(move);
	uint16_t old_file = move[0] - 'a';
	uint16_t old_rank = move[1] - '1';
	uint16_t new_file = move[2] - 'a';
	uint16_t new_rank = move[3] - '1';
	uint16_t promotion = 0;

	// handles promotion
	if(len == 5) {
		switch(move[4]){
			case 'q':
				promotion = 040000;
				break;
			case 'b':
				promotion = 050000;
				break;
			case 'n':
				promotion = 060000;
				break;
			case 'r':
				promotion = 070000;
				break;
			default:
				assert(0);
				break;
		}
	}

	uint16_t ret = 0;
	ret |= new_file;
	ret |= new_rank << 3;
	ret |= old_file << 6;
	ret |= old_rank << 9;
	ret |= promotion;


	return ret; 
}

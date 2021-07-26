#include "board_rep.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define END 0x8000000000000000

struct Phase{
	const int KnightTable[64];
	const int PawnTable[64];
	const int BishopTable[64];
	const int RookTable[64];
	const int QueenTable[64];
	const int KingTable[64];

};

// TODO: MAKE THESE POINTERS
static const struct Phase midGame = {
	// Knight
	{	-50,-40,-30,-30,-30,-30,-40,-50, 
		-40,-20,  0,  0,  0,  0,-20,-40,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30, 
		-30,  0, 15, 20, 20, 15,  0,-30, 
		-30,  5, 10, 15, 15, 10,  5,-30, 
		-40,-20,  0,  5,  5,  0,-20,-40, 
		-50,-40,-20,-30,-30,-20,-40,-50
	},

	// Pawn
	{	 0,  0,  0,  0,  0,  0,  0,  0, 
		50, 50, 50, 50, 50, 50, 50, 50, 
		10, 10, 20, 30, 30, 20, 10, 10,  
		 5,  5, 10, 27, 27, 10,  5,  5,  
		 0,  0,  0, 25, 25,  0,  0,  0,  
		 5, -5,-10,  0,  0,-10, -5,  5,  
		 5, 10, 10,-25,-25, 10, 10,  5,  
		 0,  0,  0,  0,  0,  0,  0,  0
	},

	// Bishop
	{	-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-10,-10,-10,-10,-10,-20
	},

	// Rook
	{	 0,  0,  0,  0,  0,  0,  0,  0,
	  	 5, 10, 10, 10, 10, 10, 10,  5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	  	 0,  0,  0,  5,  5,  0,  0,  0
	},

	// Queen
	{	-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
	 	 -5,  0,  5,  5,  5,  5,  0, -5,
	  	  0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	},

	// King
	{	-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		 20, 20,  0,  0,  0,  0, 20, 20,
		 20, 30, 10,  0,  0, 10, 30, 20
	}

};

// TODO: MODIFY END AND BEGINNING GAME TABLES
static const struct Phase endGame = {
	// Knight
	{	-50,-40,-30,-30,-30,-30,-40,-50, 
		-40,-20,  0,  0,  0,  0,-20,-40,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30, 
		-30,  0, 15, 20, 20, 15,  0,-30, 
		-30,  5, 10, 15, 15, 10,  5,-30, 
		-40,-20,  0,  5,  5,  0,-20,-40, 
		-50,-40,-20,-30,-30,-20,-40,-50
	},

	// Pawn
	{	 0,  0,  0,  0,  0,  0,  0,  0, 
		50, 50, 50, 50, 50, 50, 50, 50, 
		10, 10, 20, 30, 30, 20, 10, 10,  
		 5,  5, 10, 27, 27, 10,  5,  5,  
		 0,  0,  0, 25, 25,  0,  0,  0,  
		 5, -5,-10,  0,  0,-10, -5,  5,  
		 5, 10, 10,-25,-25, 10, 10,  5,  
		 0,  0,  0,  0,  0,  0,  0,  0
	},

	// Bishop
	{	-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-10,-10,-10,-10,-10,-20
	},

	// Rook
	{	 0,  0,  0,  0,  0,  0,  0,  0,
	  	 5, 10, 10, 10, 10, 10, 10,  5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	  	 0,  0,  0,  5,  5,  0,  0,  0
	},

	// Queen
	{	-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
	 	 -5,  0,  5,  5,  5,  5,  0, -5,
	  	  0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	},

	// King
	{	-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		 20, 20,  0,  0,  0,  0, 20, 20,
		 20, 30, 10,  0,  0, 10, 30, 20
	}
};

static const struct Phase beginGame = {
	// Knight
	{	-50,-40,-30,-30,-30,-30,-40,-50, 
		-40,-20,  0,  0,  0,  0,-20,-40,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30, 
		-30,  0, 15, 20, 20, 15,  0,-30, 
		-30,  5, 10, 15, 15, 10,  5,-30, 
		-40,-20,  0,  5,  5,  0,-20,-40, 
		-50,-40,-20,-30,-30,-20,-40,-50
	},

	// Pawn
	{	 0,  0,  0,  0,  0,  0,  0,  0, 
		50, 50, 50, 50, 50, 50, 50, 50, 
		10, 10, 20, 30, 30, 20, 10, 10,  
		 5,  5, 10, 27, 27, 10,  5,  5,  
		 0,  0,  0, 25, 25,  0,  0,  0,  
		 5, -5,-10,  0,  0,-10, -5,  5,  
		 5, 10, 10,-25,-25, 10, 10,  5,  
		 0,  0,  0,  0,  0,  0,  0,  0
	},

	// Bishop
	{	-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-10,-10,-10,-10,-10,-20
	},

	// Rook
	{	 0,  0,  0,  0,  0,  0,  0,  0,
	  	 5, 10, 10, 10, 10, 10, 10,  5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	 	-5,  0,  0,  0,  0,  0,  0, -5,
	  	 0,  0,  0,  5,  5,  0,  0,  0
	},

	// Queen
	{	-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
	 	 -5,  0,  5,  5,  5,  5,  0, -5,
	  	  0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	},

	// King
	{	-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		 20, 20,  0,  0,  0,  0, 20, 20,
		 20, 30, 10,  0,  0, 10, 30, 20
	}
};



int popcount (uint64_t b);
int mobility(Board *board, uint64_t pos, char color);
int material(Board* board);
int position(Board* board, uint64_t pos, int indx, const struct Phase phase, char color);
int pawn_structure(Board* board, uint64_t pos,char color);
extern int move_count;

int evaluate_board(Board *board, char color){
	int value = 0;
	uint64_t pos;
	int indx = 0;
	struct Phase phase;
	
	if (move_count <= 20){
		memcpy(&phase, &beginGame, sizeof(struct Phase));
	} else if((popcount((board->white)^((board->pawns)&(board->white)))<=3) && (popcount((board->white)^((board->pawns)&(board->white))) <=3)){
		memcpy(&phase, &endGame, sizeof(struct Phase));
	} else {
		memcpy(&phase, &midGame,sizeof(struct Phase));
	}

	// Iterate through pieces on board
	for(pos = 1; pos <= END; pos <<= 1){
		

		// Call piece-dependent evaluation functions
		if(pos & board->black){
			value -= mobility(board, pos, 1);
			value -= position(board,pos,indx,phase,1)*-1;
			value -= pawn_structure(board, pos,1);
			
// /////////////////////////////////////////////////
// 			if((pos & board->black)&(board->kings)){
// 				if()
// 			}
// ////////////////////////////////////////////////
		}
		else if(pos & board->white){

			value += mobility(board, pos, 0);
			value += position(board,pos,indx,phase,0);
			value += pawn_structure(board, pos,0);
			// printf("%d",pawn_structure(board,pos,0));
		}

		indx++;
		if(pos == END){
			break;
		}
	}

	value += material(board);

	// Flip value for black
	if(color)
		return value * -1;
	else 
		return value;
	

}



int position(Board* board, uint64_t position, int indx, const struct Phase phase, char color){

	if(color){
		indx = 63 - indx;
	}


	if(board->pawns & position) 
		return phase.PawnTable[indx];
	else if (board->rooks & position)
		return phase.RookTable[indx];
	else if (board->knights & position)
		return phase.KnightTable[indx];
	else if (board->bishops & position)
		return phase.BishopTable[indx];
	else if (board->queens & position)
		return phase.QueenTable[indx];
	else if (board->kings & position)
		return phase.KingTable[indx];

	return 0;
}

int mobility(Board *board, uint64_t pos, char color){
	uint64_t same;
	uint64_t mvs_old;
	uint64_t mvs;
	int count = 0;

	// TODO: Make this more efficient

	if(color) {
		same = board->black;
	} else{
		same = board->white;
	}

	// printf("%016llx\n",same);
	

	// Caclulate mobility score
	if(pos & same){
		mvs = moves(board,pos,color);

		// printf("%016llx\n",mvs);
		// print_board_and_moves(board, pos, mvs)

		// Count bits set and add to total moves possible
		while(mvs){
			mvs_old = mvs;
			mvs -= 1;
			mvs &= mvs_old;
			count++;
		}
	}

	return count;
}



int popcount (uint64_t b) {
   	int count = 0;
    while (b) {
   		count++;
    	b &= (b - 1); // reset LS1B
    	
	}
	// printf("count: %d\n",count);
	return count;
}


int material(Board* board){
	

	int king_score= popcount((board->white) & (board->kings)) - popcount((board->black) & (board->kings));
	int queen_score= popcount((board->white) & (board->queens)) - popcount((board->black) & (board->queens));
	int rook_score= popcount((board->white) & (board->rooks)) - popcount((board->black) & (board->rooks));
	int bishop_score= popcount((board->white) & (board->bishops)) - popcount((board->black) & (board->bishops));
	int knight_score= popcount((board->white) & (board->knights)) - popcount((board->black) & (board->knights));
	int pawn_score= popcount((board->white) & (board->pawns)) - popcount((board->black) & (board->pawns));

	int material_value= 3000*(king_score) + 900*(queen_score) + 500*(rook_score) + 325*(bishop_score) + 300*(knight_score) + 100*(pawn_score);

	return material_value;
}
#define N_LEFT		0x7f7f7f7f7f7f7f7f
#define N_RIGHT		0xfefefefefefefefe
#define EDGE		0xff818181818181ff


int pawn_structure(Board* board,uint64_t pos, char color){
	//white
	int structure_score=0;
	if (pos & (~color & (board -> pawns))){
		// checking if in center of board
		
		if (pos & ~EDGE){
			//checks neighbors left and right
			if((((pos << 1) | (pos >> 1))  & (~color & (board -> pawns)))){
				structure_score = structure_score + 2;
			}
			//checks upleft upright downleft downright neighbors
			if((((pos << 7) | (pos << 9) | (pos >> 7) | (pos >> 9))) & ( ~color & (board -> pawns))){
				structure_score = structure_score + 1;
			}
			//checks above & below neighbors
			// if( ((pos >> 8) /*| (pos << 8 )*/) & ( ~color & (board -> pawns)) ){
			// 	structure_score = structure_score - 30;
			// }


		//checking if on right side
		} else if(pos & (~N_RIGHT)){ 
			//checks left neighbors
			if((pos << 1) & ( ~color & (board -> pawns))){
				structure_score = structure_score + 2;
			}
			if(((pos << 9)& (pos >> 7)) & ( ~color & (board -> pawns))){
				structure_score = structure_score + 1;
			}
		//checks if on left edge 
		} else if(pos & (~N_LEFT)){
			//checks right neigbors
			if((pos >> 1) & ( ~color & (board -> pawns))){
				structure_score = structure_score + 2;
			}
			if(((pos >> 9)| (pos << 7)) & ( ~color & (board -> pawns))){
				structure_score = structure_score + 1;
			}

		}
		if( ((pos >> 8) /*| (pos << 8 )*/) & ( ~color & (board -> pawns)) ){
			structure_score = structure_score - 3;
		}

		///todo: verify if works proplerly
	}else if(pos & (color & (board->pawns))){

		if (pos & ~EDGE){
			if((((pos << 1) | (pos >> 1))  & (color & (board -> pawns)))){
				structure_score = structure_score - 2;
			}
			if((((pos << 7) | (pos << 9) | (pos >> 7) | (pos >> 9))) & ( ~color & (board -> pawns))){
				structure_score = structure_score - 1;
			}
		}else if(pos & (~N_RIGHT)){ 
			//checks left neighbors
			if((pos << 1) & ( color & (board -> pawns))){
				structure_score = structure_score - 2;
			}
			if(((pos << 9)& (pos >> 7)) & ( color & (board -> pawns))){
				structure_score = structure_score - 1;
			}
		//checks if on left edge 
		} else if(pos & (~N_LEFT)){
			//checks right neigbors
			if((pos >> 1) & ( color & (board -> pawns))){
				structure_score = structure_score - 2;
			}
			if(((pos >> 9)| (pos << 7)) & ( color & (board -> pawns))){
				structure_score = structure_score - 1;
			}

		}
		if( ((pos<<  8) /*| (pos << 8 )*/) & ( color & (board -> pawns)) ){
			structure_score = structure_score + 3;
		}

	}


	// if (structure_score == 0){
	// 	return -10;
	// }else{
	return structure_score;
	//}
}

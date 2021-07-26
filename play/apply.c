
#include <stdlib.h>
#include <inttypes.h>
#include "../bits/bitscan.h"
#include "../board_rep/board_rep.h"
#include "../tree/structures.h"
#include "../tree/tree.h"
#include "../tree/tree_helper.h"

#include "play.h"

#define CAPTURE 	0x80

// only sensitive to REVOKE_R, REVOKE_L
static void init_stat(Status * status, uint8_t flags, uint8_t passant){

	// color change
	status->color = !status->color;
	status->passant = passant;

	// black
	if(status->color){

		status->BCheck = !is_legal(status); // applies after color change

		// flags setting is color reversed -> applies to pre color change
		if(flags & REVOKE_R)
			status->WCR = 0;
		if(flags & REVOKE_L)
			status->WCL = 0;

	// white
	} else {

		status->WCheck = !is_legal(status); // applies afer color change

		// flags setting is color reversed -> applies to pre color change
		if(flags & REVOKE_R)
			status->BCR = 0;
		if(flags & REVOKE_L)
			status->BCL = 0;
	}
}

void apply_move(uint16_t move){

	uint8_t new_row = (move & 070) >> 3;
	uint8_t new_col = (move & 07);
	uint8_t old_row = (move & 07000) >> 9;
	uint8_t old_col = (move & 0700) >> 6;
	uint8_t xform = (move & 070000) >> 12;

	uint64_t new = build_pos(new_row, new_col);
	uint64_t old = build_pos(old_row, old_col);

	uint64_t same, other;

	if(root->status.color){
		same  = root->status.board.black;
		other = root->status.board.white;
	} else {
		same = root->status.board.white;
		other = root->status.board.black;
	}

	// pawn
	if(same & root->status.board.pawns & old){

		// passant take
		if(old_col != new_col && new & ~other){

			move_piece(&root->status.board, root->status.color,  0, old, new);

			// remove captured piece
			if(root->status.color) {
				root->status.board.pawns &= ~(new >> 8);
				root->status.board.white &= ~(new >> 8);
			} else {
				root->status.board.pawns &= ~(new << 8);
				root->status.board.black &= ~(new << 8);
			}

			init_stat(&root->status, 0, 0);
		}

		// add passant bits
		else if(old_row + 2 == new_row || old_row - 2 == new_row){
			move_piece(&root->status.board, root->status.color,  0, old, new);
			init_stat(&root->status, 0, 0x80 >> new_col);
		}

		// xform
		else if(xform){

			move_piece(&root->status.board, root->status.color,  UNARY_OR(other & new), old, new);

			switch(xform){
				case 4:
					root->status.board.pawns &= ~new;
					root->status.board.queens |= new;
				break;
				case 5:
					root->status.board.pawns &= ~new;
					root->status.board.bishops |= new;
				break;
				case 6:
					root->status.board.pawns &= ~new;
					root->status.board.knights |= new;
				break;
				case 7:
					root->status.board.pawns &= ~new;
					root->status.board.rooks |= new;
				break;

			}
			
			init_stat(&root->status, 0, 0);
		}

		else {
			move_piece(&root->status.board, root->status.color,  UNARY_OR(other & new), old, new);
			init_stat(&root->status, 0, 0);
		}
	}

	// king
	else if(same & root->status.board.kings & old){
		
		// castle right
		if(old_col + 2 == new_col) {

			if(root->status.color){
				move_piece(&root->status.board, root->status.color, 0, 0x0000000000000001,0x0000000000000004);
				move_piece(&root->status.board, root->status.color, 0, 0x0000000000000008,0x0000000000000002);
			} else {
				move_piece(&root->status.board, root->status.color, 0, 0x0100000000000000,0x0400000000000000);
				move_piece(&root->status.board, root->status.color, 0, 0x0800000000000000,0x0200000000000000);
			}

			init_stat(&root->status, REVOKE_L | REVOKE_R, 0);
		}

		// castle left
		else if(old_col - 2 == new_col){

			if(root->status.color){
				move_piece(&root->status.board, root->status.color, 0, 0x0000000000000080,0x0000000000000010);
				move_piece(&root->status.board, root->status.color, 0, 0x0000000000000008,0x0000000000000020);
			} else {
				move_piece(&root->status.board, root->status.color, 0, 0x8000000000000000,0x1000000000000000);
				move_piece(&root->status.board, root->status.color, 0, 0x0800000000000000,0x2000000000000000);
			}

			init_stat(&root->status, REVOKE_L | REVOKE_R, 0);
		}

		// normal move
		else {
			move_piece(&root->status.board, root->status.color,  UNARY_OR(other & new), old, new);
			init_stat(&root->status, REVOKE_L | REVOKE_R, 0);
		}
	}

	// rook
	else if(same & root->status.board.rooks & old){
		move_piece(&root->status.board, root->status.color,  UNARY_OR(other & new), old, new);

		uint8_t flag = 0;

		if(root->status.color){
			if(root->status.BCL && (old & 0x0000000000000080))
				flag = REVOKE_L;
			if(root->status.BCR && (old & 0x0000000000000001))
				flag = REVOKE_R;
		} else {
			if(root->status.WCL && (old & 0x8000000000000000))
				flag = REVOKE_L;
			if(root->status.WCR && (old & 0x0100000000000000))
				flag = REVOKE_R;
		}

		init_stat(&root->status, flag, 0);
	}

	// rest of pieces aka normal moves
	else {
		move_piece(&root->status.board, root->status.color,  UNARY_OR(other & new), old, new);
		init_stat(&root->status, 0, 0);
	}
}
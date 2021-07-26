#ifndef PLAY_H_
#define PLAY_H_

#define TREE_DEPTH		5

// clears and initializes tree and expands to depth
void new_game();

uint16_t make_move();

// rebases tree and maintains tree depth
void do_move(uint16_t move_made);

void apply_move(uint16_t move);

#endif 


#ifndef X_POS_HTABLE_H_
#define X_POS_HTABLE_H_

#include "tree/structures.h"

void 	htable_init	(uint32_t size_mb);
Node * 	htable_find	(uint64_t hash);
void 	htable_insert(uint64_t hash, Node * node);
void	htable_delete();

#endif
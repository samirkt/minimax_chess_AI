
#ifndef TREE_PRIORITY_QUEUE_H_
#define TREE_PRIORITY_QUEUE_H_

#include <stdint.h>
#include "tree/structures.h"

// see tree.h, stuctures for board rep, status, x-pos table, and AB alg.

void 		pq_push		(PriorityQ * q, Node_o * node);
Node_o* 	pq_pop		(PriorityQ * q);

PriorityQ* 	pq_new		();
void 		pq_init		(PriorityQ * q);
void 		pq_destroy	(PriorityQ * q);
uint8_t 	pq_size		(PriorityQ * q);

void 		pq_free		(PriorityQ * q);

void compute_scores(Node_o * node, Node_o * parent, uint64_t old, uint64_t new, uint8_t flags);

#endif
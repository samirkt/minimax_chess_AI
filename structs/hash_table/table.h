
#ifndef STRUCTS_HASH_TABLE_TABLE_H_
#define STRUCTS_HASH_TABLE_TABLE_H_

#include "tree/structures.h"

typedef struct Table Table;

/*
 *	Table only stores pointers to your data, 
 *	you need to make sure youre data is allocated
 *	on the heap (malloc) and take this into consideration
 *	when thinking of total structural size.
 *
 * 	entries_in_size is provided to help calculate the # of 
 *	entries that fit into a given size in MB. It assumes
 *	every entry is occupied (max size) and that you know 
 *	the size of each entry on the heap in Bytes. It ensures
 *	the total size doesn't exceed size_mb
 */

uint32_t num_entries_in_size(uint32_t size_mb, uint32_t size_struct);

// all pointers are initialized to NULL
Table* 	new_hash_table	(uint32_t num_entries);

// return the pointer that corresponds to the given hash
// may be NULL
void * 	table_find		(Table * table, uint64_t hash);

// over writes position corresponding to hash with data
void 	table_insert	(Table * table, uint64_t hash, void * data);

// this destroys the table, dealloc function should deallocate 
// your data. Needs to handle receiving NULL pointers graciously.
void 	table_dealloc	(Table * table, void (*dealloc)(void *));

#endif
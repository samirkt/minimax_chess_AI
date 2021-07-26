
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "tree/structures.h"
#include "tree/tree.h"
#include "table.h"

struct Table {
	uint32_t entries;
	void* array[];
};

uint32_t num_entries_in_size(uint32_t size_mb, uint32_t size_struct){
	uint32_t true_size = size_struct + sizeof(void*);
	size_mb = size_mb*1024*1024 - sizeof(Table);

	return size_mb / true_size;
}

Table* 	new_hash_table(uint32_t num_entries){

	Table * table = (Table*) malloc(sizeof(Table));
	table->entries = num_entries;
	
	// glorified malloc, needs to return 0 for success
	assert(!
		posix_memalign((void**) &table->array, sizeof(void*), sizeof(void*) * table->entries)
	);

	assert(table->entries);

	return table;
}


void* table_find(Table * table, uint64_t hash) {

	assert(table);
	assert(table->array);

	return table->array[hash % table->entries];
}

void table_insert(Table* table, uint64_t hash, void * data) {

	assert(table);
	assert(table->array);

	table->array[hash & table->entries] = data;
}

void table_dealloc(Table * table, void (*dealloc)(void *)){

	assert(table);
	assert(table->array);

	uint32_t i;
	for(i = 0; i < table->entries; ++i){
		dealloc(table->array[i]);
	}

	free(table->array);
	free(table);
}


#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "tree/structures.h"
#include "tree/tree.h"
#include "htable.h"
#include "structs/hash_table/hash.h"

typedef struct {
	Node node;
	uint64_t hash;
} TableEntry;

static uint32_t entries;
static uint32_t bytes;
static TableEntry * table;

void htable_init(uint32_t size_mb) {
	hash_init();

	entries = (size_mb * 1024 * 1024) / sizeof(TableEntry);
	bytes = sizeof(TableEntry) * entries;

	table = (TableEntry *) malloc(bytes);
	memset(table, 0, bytes);
}

Node * htable_find(uint64_t hash) {

	assert(table);

	uint32_t index = hash % entries;

	if(table[index].hash == hash){
		return &table[index].node;
	}

	else 
		return NULL;
}

void htable_insert(uint64_t hash, Node * node) {

	assert(table);

	uint32_t index = hash % entries;

	table[index].node = *node;
	table[index].hash = hash;
}

void htable_delete(){

	assert(table);

	free(table);
}

#ifndef STRUCTS_LINKED_LIST_LIST_H_
#define STRUCTS_LINKED_LIST_LIST_H_

#include <stdint.h>

typedef struct List_i List_i;
typedef struct List List;

void list_rand_init();

List* empty_list();
void list_push(List * list, void * data);
void* list_get_rand(List * list);
void* list_index(List * list, uint32_t idx);
void list_delete(List * list);

#endif

#include <stdlib.h>
#include <stdint.h>
#include "list.h"

#include "../../rand/rand.h"

struct List_i {
	void * data;
	struct List_i * next;
};

struct List {
	uint32_t length;
	List_i * head;
	List_i * tail;
};

static List_i* index(List * list, uint32_t idx);

List* empty_list(){
	List* list = (List*) malloc(sizeof(List));

	list->head = NULL;
	list->tail = NULL;
	list->length = 0;

	return list;
}

void list_push(List * list, void * data){

	List_i * item = (List_i *) malloc(sizeof(List_i));

	item->next = NULL;
	item->data = data;

	if(list->length == 0){
		list->head = item;
		list->tail = item;
		list->length = 1;
		return;
	}

	list->tail->next = item;
	list->tail = item;
	list->length++;
}

void* list_get_rand(List * list){
	if(list->length)
		return list_index(list, rand32() % list->length);
	else 
		return NULL;
}

void list_delete(List * list){
	List_i * item = list->head;
	List_i * temp;

	free(list);

	while(item){
		temp = item->next;
		free(item);
		item = temp;
	}
}

void* list_index(List * list, uint32_t idx){
	List_i * item = index(list, idx);
	return item->data;
}

static List_i* index(List * list, uint32_t idx){
	uint32_t i;
	List_i * item;

	// iterate through
	for(i = 0, item = list->head; i < idx && item; ++i, item = item->next);

	return item;
}
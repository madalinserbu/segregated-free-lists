#pragma once
#include "utils.h"
#include <stdlib.h>

typedef struct allocated {
	int id, address, size;
	char *v;
} allocated;

typedef struct dll_node_t {
	void *data;
	struct dll_node_t *prev, *next;
} dll_node_t;

typedef struct doubly_linked_list_t {
	dll_node_t *head;
	int size;
} doubly_linked_list_t;

typedef struct segregatedfreelist {
	doubly_linked_list_t **v;
	int size;
} segregatedfreelist;

#define SIZE_ALLOCATED sizeof(struct allocated)
#define SIZE_DLL_NODE sizeof(struct dll_node_t)
#define SIZE_DOUBLY_LINKED_LIST sizeof(struct doubly_linked_list_t)
#define SIZE_SEGREGATED sizeof(struct segregatedfreelist)
#define SIZE_ALLOCATED_PTR sizeof(struct allocated *)
#define SIZE_DLL_NODE_PTR sizeof(struct dll_node_t *)
#define SIZE_DOUBLY_LINKED_LIST_PTR sizeof(struct doubly_linked_list_t *)
#define SIZE_SEGREGATED_PTR sizeof(struct segregatedfreelist *)
#define SIZE_CHAR 1

void *create_data(int id, int address, int size, char *v)
{
	allocated *data = malloc(SIZE_ALLOCATED);
	// DIE(!data, "Memory allocation failed!\n");
	data->size = size;
	data->address = address;
	data->id = id;
	data->v = v;
	return data;
}

dll_node_t *create_node(void *data)
{
	dll_node_t *new_node = malloc(SIZE_DLL_NODE);
	DIE(!new_node, "Memory allocation failed!\n");
	new_node->prev = NULL;
	new_node->next = NULL;
	new_node->data = data;
	return new_node;
}

doubly_linked_list_t *create_dll(void)
{
	doubly_linked_list_t *list = malloc(SIZE_DOUBLY_LINKED_LIST);
	DIE(!list, "Memory allocation failed!\n");
	list->size = 0;
	list->head = NULL;
	return list;
}

void delete_list(doubly_linked_list_t *list)
{
	dll_node_t *curr_node = list->head;
	int i = 0;
	while (i < list->size) {
		dll_node_t *next_node = curr_node->next;
		allocated *data = curr_node->data;
		char *v = data->v;
		free(curr_node->data);
		free(curr_node);
		curr_node = next_node;
		free(v);
		++i;
	}
	free(list);
}

void dll_pop_front(doubly_linked_list_t *list)
{
	if (list->head->next)
		list->head->next->prev = NULL;
	list->size--;
	list->head = list->head->next;
}

dll_node_t *dll_lower_bound(doubly_linked_list_t *list, int address)
{
	if (!list->head || address < ((allocated *)list->head->data)->address)
		return NULL;
	dll_node_t *aux = list->head;
	while (aux->next && address >= ((allocated *)aux->next->data)->address)
		aux = aux->next;
	return aux;
}

void dll_erase(doubly_linked_list_t *list, dll_node_t *node)
{
	list->size--;
	if (node->next)
		node->next->prev = node->prev;
	if (node->prev)
		node->prev->next = node->next;
	if (list->head == node)
		list->head = node->next;
}

void dll_insert(doubly_linked_list_t *list, dll_node_t *node)
{
	dll_node_t *aux = dll_lower_bound(list, ((allocated *)node->data)->address);
	list->size++;
	node->prev = aux ? aux : NULL;
	node->next = aux ? aux->next : list->head;
	if (!aux) {
		if (list->head)
			list->head->prev = node;
		list->head = node;
		return;
	}
	if (aux->next)
		aux->next->prev = node;
	aux->next = node;
}

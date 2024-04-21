#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structures.h"
#include "utils.h"

#define error_value 1
#define succes_value 0

int write_command(doubly_linked_list_t *alloc_list, int address,
				  char str[MAX_STRING_LENGTH], int no_bytes);
int read_command(doubly_linked_list_t *alloc_list, int address, int no_bytes);
void initialise_heap(stats *info, segregatedfreelist *sfl, int start_address,
					 int no_lists, int list_size);
void destroy_heap(segregatedfreelist *sfl, doubly_linked_list_t *alloc_list);
void malloc_command(stats *info, segregatedfreelist *sfl,
					doubly_linked_list_t *alloc_list, int no_bytes);
void dump_memory(stats *info, segregatedfreelist *sfl,
				 doubly_linked_list_t *alloc_list);
void free_command(stats *info, segregatedfreelist *sfl,
				  doubly_linked_list_t *alloc_list, int address);
int parse_command(char command[MAX_STRING_LENGTH], stats *info,
				  segregatedfreelist *sfl, doubly_linked_list_t *alloc_list)
{
	if (strcmp(command, "INIT_HEAP") == 0) {
		int start_address, no_lists, list_size;
		if (scanf("%x %d %d %d", &start_address, &no_lists,
				  &list_size, &info->reconstruction) != 4)
			return error_value; // Error reading input
		initialise_heap(info, sfl, start_address, no_lists, list_size);
	} else if (strcmp(command, "MALLOC") == 0) {
		int no_bytes;
		if (scanf("%d", &no_bytes) != 1)
			return error_value; // Error reading input
		malloc_command(info, sfl, alloc_list, no_bytes);
	} else if (strcmp(command, "FREE") == 0) {
		int address;
		if (scanf("%x", &address) != 1)
			return error_value; // Error reading input
		if (!address)
			return succes_value; // Return without error if address is 0
		free_command(info, sfl, alloc_list, address);
	} else if (strcmp(command, "READ") == 0) {
		int address, no_bytes;
		if (scanf("%x %d", &address, &no_bytes) != 2)
			return error_value; // Error reading input
		if (read_command(alloc_list, address, no_bytes)) {
			printf("Segmentation fault (core dumped)\n");
			dump_memory(info, sfl, alloc_list);
			destroy_heap(sfl, alloc_list);
			return error_value;
		}
	} else if (strcmp(command, "WRITE") == 0) {
		int address, no_bytes;
		char data[MAX_STRING_LENGTH];
		if (scanf("%x \"%[^\"]\" %d", &address, data, &no_bytes) != 3)
			return error_value; // Error reading input
		if (write_command(alloc_list, address, data, no_bytes)) {
			printf("Segmentation fault (core dumped)\n");
			dump_memory(info, sfl, alloc_list);
			destroy_heap(sfl, alloc_list);
			return error_value;
		}
	} else if (strcmp(command, "DUMP_MEMORY") == 0) {
		dump_memory(info, sfl, alloc_list);
	} else if (strcmp(command, "DESTROY_HEAP") == 0) {
		destroy_heap(sfl, alloc_list);
		return error_value;
	}
	return succes_value; // Return success
}

void initialise_heap(stats *info, segregatedfreelist *sfl, int start_address,
					 int no_lists, int list_size)
{
	info->total_memory = list_size * no_lists;
	sfl->size = (8 << (no_lists - 1));
	int max_size = sfl->size;
	sfl->v = (doubly_linked_list_t **)
		malloc((max_size + 1) * SIZE_DOUBLY_LINKED_LIST_PTR);
	DIE(!sfl, "Memory allocation failed!\n");
	int i = 0;
	while (i < max_size) {
		sfl->v[i + 1] = create_dll();
		++i;
	}
	int id = 0, address = start_address;
	for (int sz = 8; sz <= max_size; sz <<= 1) {
		dll_node_t *last_node = NULL;
		i = 0;
		while (i < list_size / sz) {
			void *data = create_data(id, address, sz, 0);
			dll_node_t *new_node = create_node(data);
			if (last_node)
				last_node->next = new_node;
			if (!sfl->v[sz]->head)
				sfl->v[sz]->head = new_node;
			new_node->prev = last_node;
			sfl->v[sz]->size++;
			address += sz;
			last_node = new_node;
			info->free_blocks++;
			++id;
			info->free_memory += sz;
			++i;
		}
	}
}

void dump_memory(stats *info, segregatedfreelist *sfl,
				 doubly_linked_list_t *alloc_list)
{
	PRINT_MEMORY_INFO(info);
	int sz = 1;
	while (sz <= sfl->size) {
		if (sfl->v[sz]->size == 0) {
			sz++;
			continue;
		}
		dll_node_t *node = sfl->v[sz]->head;
		printf("Blocks with %d bytes - %d free block(s) :", sz,
			   sfl->v[sz]->size);

		while (node) {
			allocated *data = node->data;
			node = node->next;
			printf(" 0x%x", data->address);
		}
		printf("\n");
		++sz;
	}
	printf("Allocated blocks :");
	dll_node_t *node = alloc_list->head;
	while (node) {
		allocated *data = node->data;
		node = node->next;
		printf(" (0x%x - %d)", data->address, data->size);
	}
	printf("\n-----DUMP-----\n");
}

void malloc_command(stats *info, segregatedfreelist *sfl,
					doubly_linked_list_t *alloc_list, int no_bytes)
{
	if (no_bytes <= sfl->size)
		if (sfl->v[no_bytes]->size) {
			dll_node_t *old_node = sfl->v[no_bytes]->head;
			if (sfl->v[no_bytes]->head->next)
				sfl->v[no_bytes]->head->next->prev = NULL;
			sfl->v[no_bytes]->head = sfl->v[no_bytes]->head->next;
			sfl->v[no_bytes]->size--;

			char *v = (char *)malloc(no_bytes * SIZE_CHAR);
			int id = ((allocated *)old_node->data)->id;
			int addr = ((allocated *)old_node->data)->address;
			void *data = create_data(id, addr, no_bytes, v);
			dll_node_t *new_node = create_node(data);
			dll_insert(alloc_list, new_node);
			info->allocated_memory += no_bytes;
			info->allocated_blocks++;
			info->free_memory -= no_bytes;
			info->malloc_calls++;
			info->free_blocks--;
			free(old_node->data);
			free(old_node);

			return;
		}
	int sz;
	sz = no_bytes + 1;
	while (sz <= sfl->size) {
		if (sfl->v[sz]->size == 0) {
			++sz;
			continue;
		}

		char *v = (char *)malloc(no_bytes * SIZE_CHAR);
		dll_node_t *front = sfl->v[sz]->head;
		dll_pop_front(sfl->v[sz]);
		int id = ((allocated *)front->data)->id;
		int addr = ((allocated *)front->data)->address;
		void *allocated_data = create_data(id, addr, no_bytes, v);
		dll_node_t *allocated_node = create_node(allocated_data);
		dll_insert(alloc_list, allocated_node);
		void *free_data = create_data(id, addr + no_bytes, sz - no_bytes, 0);
		dll_node_t *free_node = create_node(free_data);
		dll_insert(sfl->v[sz - no_bytes], free_node);

		free(front->data);
		free(front);
		info->allocated_memory += no_bytes;
		info->fragmentations++;
		info->malloc_calls++;
		info->allocated_blocks++;
		info->free_memory -= no_bytes;
		return;
	}
	printf("Out of memory\n");
}

dll_node_t *find_node(segregatedfreelist *sfl, int address)
{
	dll_node_t *node = NULL;
	int best_address = 0;
	int sz;
	sz = 1;
	while (sz <= sfl->size) {
		if (sfl->v[sz]->size == 0) {
			sz++;
			continue;
		}
		dll_node_t *aux = dll_lower_bound(sfl->v[sz], address);
		if (aux && ((allocated *)aux->data)->address > best_address) {
			allocated *data = aux->data;
			node = aux;
			best_address = data->address;
		}
	}
	return node;
}

void free_command(stats *info, segregatedfreelist *sfl,
				  doubly_linked_list_t *alloc_list, int address)
{
	dll_node_t *node = dll_lower_bound(alloc_list, address);
	int size = 0;
	char *v;
	if (!node) {
		printf("Invalid free\n");
		return;
	}

	allocated *data = node->data;
	if (data->address != address) {
		printf("Invalid free\n");
		return;
	}
	if (alloc_list->head == node)
		alloc_list->head = node->next;
	if (node->prev)
		node->prev->next = node->next;
	if (node->next)
		node->next->prev = node->prev;
	alloc_list->size--;
	v = ((allocated *)node->data)->v;
	free(v);
	size = ((allocated *)node->data)->size;
	info->allocated_memory -= size;
	info->free_memory += size;
	info->free_calls++;
	info->allocated_blocks--;
	void *new_data = create_data(((allocated *)node->data)->id,
								 address, size, 0);
	free(node->data);
	free(node);
	dll_node_t *new_node = create_node(new_data);
	dll_insert(sfl->v[size], new_node);
	info->free_blocks++;
}

bool check_seg_fault(doubly_linked_list_t *alloc_list, int address,
					 int no_bytes)
{
	// Find the node containing or starting at the provided address
	dll_node_t *node = dll_lower_bound(alloc_list, address);

	// Check if the node exists or if its end address is before the
	// provided address
	if (!node || (((allocated *)node->data)->address +
				  ((allocated *)node->data)->size) <= address)
		return true; // Indicate segmentation fault

	// Calculate the true size of the segment starting from the provided
	// address
	int true_size = (((allocated *)node->data)->address +
					 ((allocated *)node->data)->size) -
					address;

	// Check if the provided address covers the whole segment
	if (true_size >= no_bytes)
		return false; // No segmentation fault

	// Iterate over subsequent nodes to check for additional segments
	for (int left = no_bytes - true_size; left > 0 && node->next;) {
		// Check if there is a gap between segments
		if ((((allocated *)node->data)->address +
			 ((allocated *)node->data)->size) !=
			((allocated *)node->next->data)->address)
			return true; // Indicate segmentation fault

		// Move to the next node and update the remaining bytes to check
		node = node->next;
		left -= ((allocated *)node->data)->size;
	}

	// No segmentation fault found
	return false;
}

int read_command(doubly_linked_list_t *alloc_list, int address, int no_bytes)
{
	if (check_seg_fault(alloc_list, address, no_bytes))
		return error_value;

	dll_node_t *node = dll_lower_bound(alloc_list, address);
	int true_size = ((allocated *)node->data)->address +
					((allocated *)node->data)->size - address;
	int left = no_bytes - true_size;
	int pos = 0;
	char *v = ((allocated *)node->data)->v;
	char *str = malloc((no_bytes + 1) * SIZE_CHAR);
	memset(str, 0, (no_bytes + 1) * SIZE_CHAR);
	int start_addr = address - ((allocated *)node->data)->address;
	for (int i = start_addr;
		 i < ((allocated *)node->data)->size && pos < no_bytes; i++, pos++)
		str[pos] = v[i];
	while (left > 0) {
		node = node->next;
		v = ((allocated *)node->data)->v;
		for (int i = 0;
			 i < ((allocated *)node->data)->size && pos < no_bytes; i++, pos++)
			str[pos] = v[i];
		left -= ((allocated *)node->data)->size;
	}
	printf("%s\n", str);
	free(str);
	return succes_value;
}

int write_command(doubly_linked_list_t *alloc_list, int address,
				  char str[MAX_STRING_LENGTH], int no_bytes)
{
	if (check_seg_fault(alloc_list, address, no_bytes))
		return error_value;

	dll_node_t *node = dll_lower_bound(alloc_list, address);
	int true_size = ((allocated *)node->data)->address +
					((allocated *)node->data)->size - address;
	int left = no_bytes - true_size;
	int pos = 0;
	char *v = ((allocated *)node->data)->v;
	int start_addr = address - ((allocated *)node->data)->address;
	for (int i = start_addr;
		 i < ((allocated *)node->data)->size && pos < no_bytes; i++, pos++)
		v[i] = str[pos];
	while (left > 0) {
		node = node->next;
		v = ((allocated *)node->data)->v;
		for (int i = 0;
			 i < ((allocated *)node->data)->size && pos < no_bytes; i++, pos++)
			v[i] = str[pos];
		left -= ((allocated *)node->data)->size;
	}
	return succes_value;
}

void destroy_heap(segregatedfreelist *sfl, doubly_linked_list_t *alloc_list)
{
	for (int sz = 1; sz <= sfl->size; sz++) {
		dll_node_t *curr_node = sfl->v[sz]->head;
		for (int i = 0; i < sfl->v[sz]->size; i++) {
			dll_node_t *next_node = curr_node->next;
			char *v = ((allocated *)curr_node->data)->v;
			free(v);
			free(curr_node->data);
			free(curr_node);
			curr_node = next_node;
		}
		free(sfl->v[sz]);
	}
	free(sfl->v);

	dll_node_t *curr_node = alloc_list->head;
	for (int i = 0; i < alloc_list->size; i++) {
		dll_node_t *next_node = curr_node->next;
		char *v = ((allocated *)curr_node->data)->v;
		free(v);
		free(curr_node->data);
		free(curr_node);
		curr_node = next_node;
	}
	free(alloc_list);
}

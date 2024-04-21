#pragma once

#include <errno.h>
#include <stdio.h>

typedef struct stats {
	int allocated_memory;
	int free_memory;
	int reconstruction;
	int free_blocks;
	int allocated_blocks;
	int malloc_calls;
	int free_calls;
	int fragmentations;
	int total_memory;
} stats;

#define MAX_STRING_LENGTH 1000
#define PRINT_MEMORY_INFO(info)                                                \
	do {                                                                       \
		printf("+++++DUMP+++++\n");                                            \
		stats *inf = (info);                                                   \
		printf("Total memory: %d bytes\n", (inf)->total_memory);               \
		printf("Total allocated memory: %d bytes\n", (inf)->allocated_memory); \
		printf("Total free memory: %d bytes\n", (inf)->free_memory);           \
		printf("Free blocks: %d\n", (inf)->free_blocks);                       \
		printf("Number of allocated blocks: %d\n", (inf)->allocated_blocks);   \
		printf("Number of malloc calls: %d\n", (inf)->malloc_calls);           \
		printf("Number of fragmentations: %d\n", (inf)->fragmentations);       \
		printf("Number of free calls: %d\n", (inf)->free_calls);               \
	} while (0)

#define DIE(assertion, call_description)  \
	do {                                  \
		if (assertion) {                  \
			fprintf(stderr, "(%s, %d): ", \
					__FILE__, __LINE__);  \
			perror(call_description);     \
			exit(errno);                  \
		}                                 \
	} while (0)

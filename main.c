#include "commands.h"
#include "structures.h"
#include "utils.h"
#include <stdio.h>

int main(void)
{
	stats info;
	info.free_blocks = 0;
	info.free_memory = 0;
	info.allocated_blocks = 0;
	info.free_calls = 0;
	info.malloc_calls = 0;
	info.free_calls = 0;
	info.fragmentations = 0;
	info.allocated_memory = 0;
	info.total_memory = 0;
	segregatedfreelist sfl;
	char command[MAX_STRING_LENGTH];
	doubly_linked_list_t *alloc_list = create_dll();
	int end_program = 0;
	do {
		scanf("%s", command);
	} while (!parse_command(command, &info, &sfl, alloc_list));
	return 0;
}

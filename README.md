**Name: Serbu Ovidiu-Madalin**
**Class: 324CA**

## Segregated Free Lists, Tema 1

## Contents

*structures.h*
    - all structures and operations on lists
    - I used doubly linked lists as my structure for allocated blocks

*commands.h*
    - contains majoritarily the implementation of the allocator

*utils.h*
    - contains helper functions like DIE and PRINT_MEMORY_DUMP
    - also contains structures with informations needed for MEMORY_DUMP

*main.c*
    - where the commands are being read and parsed

### Description:

*Segregated Free List*
The allocator employs a segregated free list data structure, where blocks of free memory are organized into separate lists based on their sizes. This allows for efficient allocation and deallocation operations, as it reduces the search space for available memory blocks.

*Doubly Linked List*
Allocated memory blocks are stored in a doubly linked list. This data structure enables fast insertion and removal of memory blocks while maintaining their sequential order in memory.

*Memory Allocation Strategy*
When allocating memory, the allocator searches for a suitable block in the segregated free lists. If a block of the exact size is found, it is allocated directly. Otherwise, the allocator may split a larger block to satisfy the requested size, reducing fragmentation.

*Memory Deallocation Strategy*
When deallocating memory, the allocator searches for the corresponding block in the allocated memory list and frees it. If necessary, adjacent free blocks are merged to prevent fragmentation.

*Segmentation Fault Handling*
The allocator includes checks to prevent segmentation faults when accessing allocated memory. These checks ensure that memory accesses remain within the bounds of allocated regions and detect attempts to access unallocated or freed memory.

*Memory Dumping*
The allocator provides a functionality to dump the contents of the memory, including information about allocated and free memory blocks. This feature aids in debugging and understanding the memory layout during runtime.

*Error Handling*
The allocator includes error handling mechanisms to detect and report errors such as invalid memory accesses, failed memory allocations, and out-of-memory conditions. Proper error messages are displayed to assist users in diagnosing and resolving issues.

*Memory Destruction*
Upon program termination or explicit destruction of the heap, all allocated memory is deallocated, and the heap data structures are freed to prevent memory leaks and ensure clean resource management.

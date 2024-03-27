#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "hashmap.h"
void myMalloc()
{
    return;
}
// adresa o citesc ca string si o convertesc cu strtol in numar, daca sunt probleme vezi schibma citirea sa citesti direct numar
// si ai grija la hashmap cand adaugi si cand cauti, eu cheia o am adresa si valoare e blocu cua dresa aia

int calculateFreeMemory(LinkedList **freeList, int sizeArray)
{
    int nrFree = 0;
    for (int i = 0; i < sizeArray; i++)
    {
        nrFree += freeList[i]->nrBlocks * freeList[i]->sizeBlock;
    }
    return nrFree;
}

int calculateNrFreeBlocks(LinkedList **freeList, int sizeArray)
{
    int nrFreeBlocks = 0;
    for (int i = 0; i < sizeArray; i++)
    {
        nrFreeBlocks += freeList[i]->nrBlocks;
    }
    return nrFreeBlocks;
}

int main()
{
    int memoryOfList;
    int sizeArray;
    char startAddress[10];
    char command[20];
    int nrMallocs = 0;
    int nrFrees = 0;
    int typeReconst;
    int nrTotalBlocks = 0;
    HashMap *allocatedBlocks = createHashMap();
    LinkedList **freeList = NULL;
    while (1)
    {
        scanf("%s", command);
        // printf("%s\n", command);
        if (strcmp(command, "INIT_HEAP") == 0)
        {
            scanf("%s%d%d%d", startAddress, &sizeArray, &memoryOfList, &typeReconst);
            // printf("%s%d%d%d\n", startAddress, sizeArray, memoryOfList, typeReconst);
            freeList = allocateArrayOfLinkedLists(sizeArray, memoryOfList, startAddress);
            nrTotalBlocks = calculateNrFreeBlocks(freeList, sizeArray);
        }
        else if (strcmp(command, "DUMP_MEMORY") == 0)
        {

            printf("1");
            fflush(stdout);
            printf("Total memory: %d bytes\n", memoryOfList * sizeArray);
            // SCHIMBA CALCUL MEMORIE ALOCARE PT FRAGMENTARE
            printf("1");
            fflush(stdout);
            printf("Total allocated memory: %d bytes\n", memoryOfList * sizeArray - calculateFreeMemory(freeList, sizeArray));
            printf("2");
            fflush(stdout);
            printf("Total free memory: %d bytes\n", calculateFreeMemory(freeList, sizeArray));
            printf("3");
            fflush(stdout);
            printf("Number of free blocks: %d\n", calculateNrFreeBlocks(freeList, sizeArray));
            printf("4");
            fflush(stdout);
            // SCHIMBA CALCUL BLOCURI ALOCARE PT FRAGMENTARE
            printf("Number of allocated blocks: %d\n", nrTotalBlocks - calculateNrFreeBlocks(freeList, sizeArray));
            printf("5");
            fflush(stdout);
            printf("Number of malloc calls: %d\n", nrMallocs);
            printf("6");
            fflush(stdout);
            printf("Number of fragmentations: %d\n", 0);
            printf("7");
            fflush(stdout);
            printf("Number of free calls: %d\n", nrFrees);
            printf("8");
            fflush(stdout);
        }
        else if (strcmp(command, "MALLOC") == 0)
        {
            int sizeWanted;
            scanf("%d", &sizeWanted);
            int ok = 0;
            for (int i = 0; i < sizeArray; i++)
            {
                if (8 << i == sizeWanted)
                {
                    if (freeList[i]->nrBlocks > 0)
                    {
                        ok = 1;
                        nrMallocs++;
                        Node *allocatedNode = removeNodeAtIndex(freeList[i], 0);
                        insert(allocatedBlocks, allocatedNode->data, allocatedNode);
                        break;
                    }
                }
                if (8 << i > sizeWanted)
                {
                    // fragmentare
                    printf("ce pula mea asteptari ai de la mine fă mărie!\n");
                    break;
                }
            }
            if (ok == 0)
            {
                printf("Bagamias pula\n");
            }
        }
        else if (strcmp(command, "ZZZ") == 0)
        {
        }
        else if (strcmp(command, "") == 0)
        {
        }
        else if (strcmp(command, "") == 0)
        {
        }
        else if (strcmp(command, "") == 0)
        {
        }
        else if (strcmp(command, "") == 0)
        {
        }
        else if (strcmp(command, "DESTROY_HEAP") == 0)
        {
            break;
        }
        else
        {
            // printf("Unknown Command!\n");
            char c = 'a';
            while (c != '\n')
            {
                scanf("%c", &c);
            }
        }
    }
}

/*
int main()
{

    LinkedList list;
    initializeList(&list);

    // Example data
    int data1 = 10;
    int data2 = 20;
    int data3 = 30;

    // Add nodes at specific indices in the list
    appendNodeAtIndex(&list, &data1, 0); // Append at index 0
    appendNodeAtIndex(&list, &data2, 0); // Append at index 0
    appendNodeAtIndex(&list, &data3, 2); // Append at index 2
    printList(&list);

    // Removing a node at index 1
    removeNodeAtIndex(&list, 0);

    // Print the contents of the list
    printList(&list);

    // Freeing memory allocated to the list
    freeList(&list);

    return 0;
}
*/

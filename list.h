// #include <stdlib.h>
//  Define the structure for a node in the linked list
typedef struct Node
{
    struct Node *prev;
    struct Node *next;
    char isOccupied;
    void *data;
} Node;

// Define the structure for the linked list
typedef struct
{
    Node *head;
    Node *tail;
    int nrBlocks;
    int sizeBlock;
} LinkedList;

void initializeList(LinkedList *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->nrBlocks = 0;
}

void initializeListWithSize(LinkedList *list, int size)
{
    initializeList(list);
    list->sizeBlock = size;
}

void appendNodeAtIndex(LinkedList *list, void *data, int index);
// Function to allocate an array of linked lists
LinkedList **allocateArrayOfLinkedLists(int n, int size, char startAddress[10])
{
    LinkedList **array = (LinkedList **)malloc(n * sizeof(LinkedList *));
    if (array == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    char *ptr; // Pointer to character to be used with strtol
    long int startaddress;

    // Convert hexadecimal string to long int
    startaddress = strtol(startAddress, &ptr, 0);

    // Check for errors during conversion
    if (*ptr != '\0')
    {
        printf("Error: Invalid hexadecimal string.\n");
        return NULL;
    }
    // Initialize each linked list in the array
    for (int i = 0; i < n; i++)
    {
        array[i] = (LinkedList *)malloc(sizeof(LinkedList));
        if (array[i] == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        int blockSize = 8 << i;
        int calculatedSize = startaddress;
        initializeListWithSize(array[i], blockSize);
        for (int j = 0; j < size / blockSize; j++)
        {
            appendNodeAtIndex(array[i], &calculatedSize, j);
        }
        startaddress += blockSize;
    }

    return array;
}
// Function to add a node to a sorted linked list
void addNodeSorted(LinkedList *list, void *data) {
    int value = *(int *)data; // Assuming data is of type int*

    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->isOccupied = 1;

    if (list->head == NULL) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        Node *current = list->head;
        while (current != NULL && (*(int *)(current->data)) < value) {
            current = current->next;
        }
        if (current == NULL) {
            // Add to the end
            newNode->prev = list->tail;
            newNode->next = NULL;
            list->tail->next = newNode;
            list->tail = newNode;
        } else if (current == list->head) {
            // Add at the beginning
            newNode->prev = NULL;
            newNode->next = list->head;
            list->head->prev = newNode;
            list->head = newNode;
        } else {
            // Add in the middle
            newNode->next = current;
            newNode->prev = current->prev;
            current->prev->next = newNode;
            current->prev = newNode;
        }
    }
    list->nrBlocks++;
}

// Function to add a node at a specific index in the list
void appendNodeAtIndex(LinkedList *list, void *data, int index)
{
    if (index < 0 || index > list->nrBlocks)
    {
        fprintf(stderr, "Index out of bounds\n");
        exit(EXIT_FAILURE);
    }

    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->isOccupied = 1;

    if (list->head == NULL)
    {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    }
    else if (index == 0)
    {
        newNode->prev = NULL;
        newNode->next = list->head;
        list->head->prev = newNode;
        list->head = newNode;
    }
    else if (index == list->nrBlocks)
    {
        newNode->prev = list->tail;
        newNode->next = NULL;
        list->tail->next = newNode;
        list->tail = newNode;
    }
    else
    {
        Node *current = list->head;
        for (int i = 0; i < index - 1; i++)
        {
            current = current->next;
        }
        newNode->prev = current;
        newNode->next = current->next;
        current->next->prev = newNode;
        current->next = newNode;
    }
    list->nrBlocks++;
}

// Function to remove a node at a specific index in the list
Node *removeNodeAtIndex(LinkedList *list, int index)
{
    if (index < 0 || index >= list->nrBlocks)
    {
        fprintf(stderr, "Index out of bounds\n");
        exit(EXIT_FAILURE);
    }

    Node *current = list->head;
    for (int i = 0; i < index; i++)
    {
        current = current->next;
    }

    if (current == list->head)
    {
        list->head = current->next;
    }
    if (current == list->tail)
    {
        list->tail = current->prev;
    }

    if (current->prev != NULL)
    {
        current->prev->next = current->next;
    }
    if (current->next != NULL)
    {
        current->next->prev = current->prev;
    }
    list->nrBlocks--;
    return current;
}

// Function to print the contents of the linked list
void printList(LinkedList *list)
{
    Node *current = list->head;
    printf("Linked List contents: ");
    while (current != NULL)
    {
        printf("%d ", *(int *)(current->data)); // Assuming the data is of int type
        current = current->next;
    }
    printf("\n");
}

// Function to free memory allocated to the list
void freeList(LinkedList *list)
{
    Node *current = list->head;
    while (current != NULL)
    {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
}

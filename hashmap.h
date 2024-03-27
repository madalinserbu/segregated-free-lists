#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHSIZE 100

//SCHELET, TREBUIE SCHIMBAT PT CERINTE

// Define a structure for a NodeHashmap in the hashmap
typedef struct NodeHashmap {
    void* key;
    void* value;
    struct NodeHashmap* next;
    char adddress[10];
} NodeHashmap;

// Define a structure for the hashmap
typedef struct {
    NodeHashmap* table[HASHSIZE];
} HashMap;

// Hash function to calculate the index for a given key
unsigned int hash(void* key) {
    // Custom hash function can be implemented based on the type of keys
    // Here, we cast the key pointer to unsigned integer for simplicity
    unsigned long int hashval = (unsigned long int)key;
    return hashval % HASHSIZE;
}

// Function to create a new NodeHashmap
NodeHashmap* createNodeHashmap(void* key, void* value) {
    NodeHashmap* newNodeHashmap = (NodeHashmap*)malloc(sizeof(NodeHashmap));
    if (newNodeHashmap == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    newNodeHashmap->key = key;
    newNodeHashmap->value = value;
    newNodeHashmap->next = NULL;
    return newNodeHashmap;
}

// Function to initialize the hashmap
HashMap* createHashMap() {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (map == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < HASHSIZE; i++)
        map->table[i] = NULL;
    return map;
}

// Function to insert a key-value pair into the hashmap
void insert(HashMap* map, void* key, void* value) {
    unsigned int index = hash(key);
    NodeHashmap* newNodeHashmap = createNodeHashmap(key, value);
    newNodeHashmap->next = map->table[index];
    map->table[index] = newNodeHashmap;
}

// Function to search for a key in the hashmap
NodeHashmap* search(HashMap* map, void* key) {
    unsigned int index = hash(key);
    NodeHashmap* currentNodeHashmap = map->table[index];
    while (currentNodeHashmap != NULL) {
        if (currentNodeHashmap->key == key)
            return currentNodeHashmap;
        currentNodeHashmap = currentNodeHashmap->next;
    }
    return NULL;
}

// Function to delete a key from the hashmap
void deleteKey(HashMap* map, void* key) {
    unsigned int index = hash(key);
    NodeHashmap* currentNodeHashmap = map->table[index];
    NodeHashmap* prevNodeHashmap = NULL;
    while (currentNodeHashmap != NULL) {
        if (currentNodeHashmap->key == key) {
            if (prevNodeHashmap == NULL)
                map->table[index] = currentNodeHashmap->next;
            else
                prevNodeHashmap->next = currentNodeHashmap->next;
            free(currentNodeHashmap);
            return;
        }
        prevNodeHashmap = currentNodeHashmap;
        currentNodeHashmap = currentNodeHashmap->next;
    }
}

// Function to print all key-value pairs in the hashmap
void printHashMap(HashMap* map) {
    printf("HashMap Contents:\n");
    for (int i = 0; i < HASHSIZE; i++) {
        NodeHashmap* currentNodeHashmap = map->table[i];
        while (currentNodeHashmap != NULL) {
            printf("(Key: %p, Value: %p) -> ", currentNodeHashmap->key, currentNodeHashmap->value);
            currentNodeHashmap = currentNodeHashmap->next;
        }
        printf("NULL\n");
    }
}

// Function to free the memory allocated for the hashmap
void freeHashMap(HashMap* map) {
    for (int i = 0; i < HASHSIZE; i++) {
        NodeHashmap* currentNodeHashmap = map->table[i];
        while (currentNodeHashmap != NULL) {
            NodeHashmap* temp = currentNodeHashmap;
            currentNodeHashmap = currentNodeHashmap->next;
            free(temp);
        }
    }
    free(map);
}

/*
int main() {
    HashMap* map = createHashMap();

    // Example usage with integer keys and values
    int key1 = 42;
    int value1 = 100;
    insert(map, &key1, &value1);

    // Example usage with string keys and integer values
    char* key2 = "hello";
    int value2 = 200;
    insert(map, key2, &value2);

    // Printing hashmap contents
    printHashMap(map);

    // Searching for a key
    int keyToSearch = 42;
    NodeHashmap* result = search(map, &keyToSearch);
    if (result != NULL)
        printf("Value for key '%d' is: %d\n", keyToSearch, *(int*)result->value);
    else
        printf("Key '%d' not found\n", keyToSearch);

    // Freeing the hashmap memory 
    freeHashMap(map);

    return 0;
}
*/

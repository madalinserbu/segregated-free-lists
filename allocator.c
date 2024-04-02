// Include directives
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structura pentru nodurile listei dublu înlănțuite
typedef struct Node
{
    void *address;
    size_t size;
    struct Node *next;
    struct Node *prev;
} Node;

// Structura pentru arborele binar de căutare
typedef struct TreeNode
{
    void *address;
    size_t size;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

// Structura pentru allocatorul de memorie
typedef struct
{
    size_t num_lists;             // Numărul de liste dublu înlănțuite
    size_t bytes_per_list;        // Numărul de octeți pentru fiecare listă
    Node *free_list_head;         // Capul listei dublu înlănțuite pentru blocurile libere
    TreeNode *root;               // Rădăcina arborelui binar de căutare pentru blocurile alocate
    size_t total_allocated_bytes; // Numărul total de octeți alocați
    size_t num_malloc_calls;      // Numărul total de apeluri malloc
    size_t num_free_calls;        // Numărul total de apeluri free
    size_t num_fragmentations;    // Numărul total de fragmentări
} MemoryAllocator;

// Prototipuri de funcții
TreeNode *createTreeNode(void *address, size_t size);
TreeNode *insertTreeNode(TreeNode *root, void *address, size_t size);
TreeNode *searchTreeNode(TreeNode *root, void *address);
TreeNode *deleteTreeNode(TreeNode *root, void *address);
void appendNodeToList(Node **head_ref, void *address, size_t size);
void deleteNodeFromList(Node **head_ref, Node *del);
void init_heap(MemoryAllocator *allocator, void *heap_base, size_t num_lists, size_t bytes_per_list, int tip_reconstituire);
void *my_malloc(MemoryAllocator *allocator, size_t size);
void my_free(MemoryAllocator *allocator, void *address);
void read_memory(MemoryAllocator *allocator, void *address, size_t size);
void write_memory(MemoryAllocator *allocator, void *address, const char *data, size_t size);
void dump_memory(MemoryAllocator *allocator);
void process_commands(MemoryAllocator *allocator);

// Funcția principală
int main()
{
    MemoryAllocator allocator;
    process_commands(&allocator);
    return 0;
}

// Funcție pentru eliberarea memoriei alocate pentru un arbore binar
void destroyTree(TreeNode *root)
{
    if (root != NULL)
    {
        destroyTree(root->left);
        destroyTree(root->right);
        free(root);
    }
}

// Funcție pentru crearea unui nod de arbore
TreeNode *createTreeNode(void *address, size_t size)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->address = address;
    node->size = size;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Funcție pentru inserarea unui nod în arbore
TreeNode *insertTreeNode(TreeNode *root, void *address, size_t size)
{
    if (root == NULL)
    {
        return createTreeNode(address, size);
    }
    if (address < root->address)
    {
        root->left = insertTreeNode(root->left, address, size);
    }
    else
    {
        root->right = insertTreeNode(root->right, address, size);
    }
    return root;
}

// Funcție pentru căutarea unui nod în arbore
TreeNode *searchTreeNode(TreeNode *root, void *address)
{
    if (root == NULL || root->address == address)
    {
        return root;
    }
    if (address < root->address)
    {
        return searchTreeNode(root->left, address);
    }
    return searchTreeNode(root->right, address);
}

// Funcție pentru ștergerea unui nod din arbore
TreeNode *deleteTreeNode(TreeNode *root, void *address)
{
    if (root == NULL)
    {
        return root;
    }
    if (address < root->address)
    {
        root->left = deleteTreeNode(root->left, address);
    }
    else if (address > root->address)
    {
        root->right = deleteTreeNode(root->right, address);
    }
    else
    {
        if (root->left == NULL)
        {
            TreeNode *temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL)
        {
            TreeNode *temp = root->left;
            free(root);
            return temp;
        }
        TreeNode *temp = root->right;
        while (temp->left != NULL)
        {
            temp = temp->left;
        }
        root->address = temp->address;
        root->size = temp->size;
        root->right = deleteTreeNode(root->right, temp->address);
    }
    return root;
}

// Funcție pentru adăugarea unui nod la sfârșitul unei liste dublu înlănțuite
void appendNodeToList(Node **head_ref, void *address, size_t size)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->address = address;
    new_node->size = size;
    new_node->next = NULL;
    if (*head_ref == NULL)
    {
        new_node->prev = NULL;
        *head_ref = new_node;
        return;
    }
    Node *last = *head_ref;
    while (last->next != NULL)
    {
        last = last->next;
    }
    last->next = new_node;
    new_node->prev = last;
}

// Funcție pentru ștergerea unui nod dintr-o listă dublu înlănțuită
void deleteNodeFromList(Node **head_ref, Node *del)
{
    if (*head_ref == NULL || del == NULL)
    {
        return;
    }
    if (*head_ref == del)
    {
        *head_ref = del->next;
    }
    if (del->next != NULL)
    {
        del->next->prev = del->prev;
    }
    if (del->prev != NULL)
    {
        del->prev->next = del->next;
    }
    free(del);
}

// Funcție pentru inițializarea allocatorului de memorie
void init_heap(MemoryAllocator *allocator, void *heap_base, size_t num_lists, size_t bytes_per_list, int tip_reconstituire)
{
    allocator->num_lists = num_lists;
    allocator->bytes_per_list = bytes_per_list;
    allocator->free_list_head = NULL;
    allocator->root = NULL;
    allocator->total_allocated_bytes = 0;
    allocator->num_malloc_calls = 0;
    allocator->num_free_calls = 0;
    allocator->num_fragmentations = 0;

    // Inițializare listele dublu înlănțuite pentru blocurile libere
    size_t block_size = 8;
    for (size_t i = 0; i < num_lists; i++)
    {
        for (size_t j = 0; j < bytes_per_list / block_size; j++)
        {
            Node *node = (Node *)malloc(sizeof(Node));
            node->address = (char *)heap_base + j * block_size;
            node->size = block_size;
            node->prev = NULL;
            node->next = allocator->free_list_head;
            if (allocator->free_list_head != NULL)
            {
                allocator->free_list_head->prev = node;
            }
            allocator->free_list_head = node;
        }
        block_size *= 2;
    }
}

// Funcție pentru alocarea de memorie
void *my_malloc(MemoryAllocator *allocator, size_t size)
{
    allocator->num_malloc_calls++;
    Node *current = allocator->free_list_head;
    while (current != NULL)
    {
        if (current->size >= size)
        {
            Node *next_block = current->next;
            deleteNodeFromList(&(allocator->free_list_head), current);
            allocator->root = insertTreeNode(allocator->root, current->address, size);
            allocator->total_allocated_bytes += size;
            if (current->size > size)
            {
                appendNodeToList(&(allocator->free_list_head), (char *)current->address + size, current->size - size);
                allocator->num_fragmentations++;
            }
            return current->address;
        }
        current = current->next;
    }
    printf("Nu există suficient spațiu liber pentru alocare.\n");
    return NULL;
}

// Funcție pentru eliberarea de memorie
void my_free(MemoryAllocator *allocator, void *address)
{
    allocator->num_free_calls++;
    TreeNode *node = searchTreeNode(allocator->root, address);
    if (node == NULL || (char *)address + node->size > (char *)node->address + node->size)
    {
        printf("Segmentation fault (core dumped)\n");
        return;
    }
    appendNodeToList(&(allocator->free_list_head), address, node->size);
    allocator->root = deleteTreeNode(allocator->root, address);
    allocator->total_allocated_bytes -= node->size;
}

// Funcție pentru afișarea conținutului unei zone de memorie
void read_memory(MemoryAllocator *allocator, void *address, size_t size)
{
    TreeNode *node = searchTreeNode(allocator->root, address);
    if (node == NULL || (char *)address + size > (char *)node->address + node->size)
    {
        printf("Segmentation fault (core dumped)\n");
        return;
    }
    printf("Conținutul memoriei la adresa %p:\n", address);
    for (size_t i = 0; i < size; i++)
    {
        printf("%c ", *((char *)address + i));
    }
    printf("\n");
}

// Funcție pentru scrierea de date într-o zonă de memorie
void write_memory(MemoryAllocator *allocator, void *address, const char *data, size_t size)
{
    TreeNode *node = searchTreeNode(allocator->root, address);
    if (node == NULL || (char *)address + size > (char *)node->address + node->size)
    {
        printf("Segmentation fault (core dumped)\n");
        return;
    }
    memcpy(address, data, size);
}

// Funcție pentru afișarea informațiilor despre starea memoriei
void dump_memory(MemoryAllocator *allocator)
{
    size_t total_free_bytes = 0;
    Node *current = allocator->free_list_head;
    while (current != NULL)
    {
        total_free_bytes += current->size;
        current = current->next;
    }
    size_t total_memory = allocator->num_lists * allocator->bytes_per_list;
    size_t total_allocated_bytes = allocator->total_allocated_bytes;
    size_t num_free_blocks = 0;
    current = allocator->free_list_head;
    while (current != NULL)
    {
        num_free_blocks++;
        current = current->next;
    }
    size_t num_allocated_blocks = allocator->num_malloc_calls - allocator->num_free_calls;
    size_t num_fragmentations = allocator->num_fragmentations;

    printf("+++++DUMP+++++\n");
    printf("Total memory: %zu bytes\n", total_memory);
    printf("Total allocated memory: %zu bytes\n", total_allocated_bytes);
    printf("Total free memory: %zu bytes\n", total_free_bytes);
    printf("Free blocks: %zu\n", num_free_blocks);
    printf("Number of allocated blocks: %zu\n", num_allocated_blocks);
    printf("Number of malloc calls: %zu\n", allocator->num_malloc_calls);
    printf("Number of fragmentations: %zu\n", num_fragmentations);
    printf("Number of free calls: %zu\n", allocator->num_free_calls);

    // Afișăm toate blocurile libere
    size_t block_size = 8;
    for (size_t i = 0; i < allocator->num_lists; i++)
    {
        size_t num_free_blocks_x = 0;
        current = allocator->free_list_head;
        while (current != NULL)
        {
            if (current->size == block_size)
                num_free_blocks_x++;
            current = current->next;
        }
        printf("Blocks with %zu bytes - %zu free block(s) : ", block_size, num_free_blocks_x);
        current = allocator->free_list_head;
        while (current != NULL)
        {
            if (current->size == block_size)
                printf("%p ", current->address);
            current = current->next;
        }
        printf("\n");
        block_size *= 2;
    }

    // Afișăm blocurile alocate
    printf("Allocated blocks : ");
    TreeNode *root = allocator->root;
    while (root != NULL)
    {
        printf("(%p - %zu) ", root->address, root->size);
        root = root->right;
    }
    printf("\n");
    printf("-----DUMP-----\n");
}

// Funcție pentru procesarea comenzilor
void process_commands(MemoryAllocator *allocator)
{
    char command[20];
    void *address;
    size_t size;
    size_t num_lists;
    size_t bytes_per_list;
    int tip_reconstituire;

    while (1)
    {
        scanf("%s", command);

        if (strcmp(command, "INIT_HEAP") == 0)
        {
            scanf("%p %zu %zu %d", &address, &num_lists, &bytes_per_list, &tip_reconstituire);
            init_heap(allocator, address, num_lists, bytes_per_list, tip_reconstituire);
        }

        else if (strcmp(command, "MALLOC") == 0)
        {
            scanf("%zu", &size);
            void *ptr = my_malloc(allocator, size);
            if (ptr == NULL)
            {
                return;
            }
        }
        else if (strcmp(command, "FREE") == 0)
        {
            scanf("%p", &address);
            my_free(allocator, address);
        }
        else if (strcmp(command, "READ") == 0)
        {
            scanf("%p %zu", &address, &size);
            read_memory(allocator, address, size);
        }
        else if (strcmp(command, "WRITE") == 0)
        {
            char data[100];
            scanf("%p %s", &address, data);
            size = strlen(data);
            write_memory(allocator, address, data, size);
        }
        else if (strcmp(command, "DUMP_MEMORY") == 0)
        {
            dump_memory(allocator);
        }
        else if (strcmp(command, "DESTROY_HEAP") == 0)
        {
            // Eliberarea întregii memorii alocate
            Node *current = allocator->free_list_head;
            while (current != NULL)
            {
                Node *next = current->next;
                free(current);
                current = next;
            }
            allocator->free_list_head = NULL;

            destroyTree(allocator->root); // Eliberarea arborelui
            allocator->root = NULL;
            exit(0); // Terminarea programului
        }
        else
        {
            printf("Comandă invalidă!\n");
        }
    }
}

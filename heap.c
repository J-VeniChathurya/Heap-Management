#include <stdio.h>
#include <stdlib.h>

#define MAX_FIB_SIZE 20 // Maximum Fibonacci sequence size

// Structure for heap block
typedef struct Block
{
    int size;
    int free; // 1 if free, 0 if allocated
    struct Block *next;
} Block;

// Fibonacci sequence array for block sizes
int fibonacci[MAX_FIB_SIZE];

// Free lists based on Fibonacci sizes
Block *free_lists[MAX_FIB_SIZE] = {NULL};

// Function to generate Fibonacci series
void generate_fibonacci()
{
    fibonacci[0] = 1;
    fibonacci[1] = 2;
    for (int i = 2; i < MAX_FIB_SIZE; i++)
    {
        fibonacci[i] = fibonacci[i - 1] + fibonacci[i - 2];
    }
}

// Find the Fibonacci index of a given size
int find_fib_index(int size)
{
    for (int i = 0; i < MAX_FIB_SIZE; i++)
    {
        if (fibonacci[i] >= size){
            return i;
        }
    }
    return -1;
}

// Insert block into its respective free list
void insert_free_block(Block *block)
{
    int index = find_fib_index(block->size);
    if (index == -1)
        return;

    block->free = 1;
    block->next = free_lists[index];
    free_lists[index] = block;
}

// Find best-fit free block
Block *find_best_fit(int size, int *index)
{
    for (int i = find_fib_index(size); i < MAX_FIB_SIZE; i++)
    {
        if (free_lists[i])
        {
            *index = i;
            return free_lists[i];
        }
    }
    return NULL;
}
// Allocate memory block
void *alloc(int size)
{
    int fib_index = find_fib_index(size);
    if (fib_index == -1)
        return NULL;

    int best_index;
    Block *best_fit = find_best_fit(size, &best_index);

    if (!best_fit)
        return NULL;

    // Remove block from free list
    free_lists[best_index] = best_fit->next;
    best_fit->free = 0;

    best_fit->next = NULL; // Ensure it's detached from the list

    // Split the block while it’s still too large
    while (best_fit->size > size && best_index >= 2)
    {
        int split_size = fibonacci[best_index - 2];  // Fibonacci-2 size
        int remaining_size = best_fit->size - split_size;

        if (split_size >= size)
        {
            // Create a new block for the remaining part
            Block *split_block = (Block *)((char *)best_fit+ sizeof(Block) + split_size);
            split_block->size = remaining_size;
            split_block->free = 1;
            split_block->next = NULL;

            // Remove original block from free list before inserting new ones
            insert_free_block(split_block);
              // Place the remaining part into the free list
            best_fit->size = split_size;
        }
        // Update the best-fit block to the new split size
        best_index -= 2;
    }

    return (void *)(best_fit + 1);
}


void free_mem(void *ptr)
{
    if (!ptr)
        return;

    Block *block = (Block *)ptr - 1;
    block->free = 1;

    int index = find_fib_index(block->size);

    // Try merging continuously
    while (index + 1 < MAX_FIB_SIZE)  // Use index+1, not index-2
    {
        int buddy_size = fibonacci[index + 1]; // Correct buddy size

        Block *prev = NULL, *curr = free_lists[index + 1];
        Block *buddy = NULL;

        // Find a free, adjacent buddy
        while (curr)
        {
            if (curr->size == buddy_size && curr->free &&
                ((char *)curr == (char *)block + sizeof(Block) + block->size || 
                 (char *)block == (char *)curr + sizeof(Block) + buddy_size))
            {
                buddy = curr;
                break;
            }
            prev = curr;
            curr = curr->next;
        }

        if (!buddy)
            break; // No buddy found, stop merging

        // Remove buddy from the free list
        if (prev)
            prev->next = buddy->next;
        else
            free_lists[index + 1] = buddy->next;

        // Merge blocks: Ensure lower memory address is the base
        if ((char *)block > (char *)buddy)
            block = buddy;

        block->size = fibonacci[index + 2]; // Merge into larger block

        // Move to the next Fibonacci size
        index += 2;
    }

    // Insert the final merged block into the correct free list **only once**
    insert_free_block(block);
}


// Initialize heap memory
void initialize_heap()
{
    generate_fibonacci();

    // Create an initial large block of memory
    Block *initial_block = (Block *)malloc(sizeof(Block) + fibonacci[MAX_FIB_SIZE - 1]);
    initial_block->size = fibonacci[MAX_FIB_SIZE - 1];
    initial_block->free = 1;
    insert_free_block(initial_block);
}

// Print free lists for debugging
void print_free_lists()
{
    printf("Free Lists:\n");
    for (int i = 0; i < MAX_FIB_SIZE; i++)
    {
        printf("Size %d: ", fibonacci[i]);
        Block *temp = free_lists[i];
        while (temp)
        {
            printf("[%p, %d] -> ",(void*)temp, temp->size);
            temp = temp->next;
        }
        printf("NULL\n");
    }
}

// Main function for testing
int main()
{
    initialize_heap();

    printf("Before Allocation:\n");
    print_free_lists();

    int n1;
    printf("enter the size you want\n");
    scanf("%d",&n1);
    if(n1 > 0 && n1 < 10946){

    void *ptr1 = alloc(n1);

    printf("\nAfter Allocation:\n");
    print_free_lists();

    void *ptr2 = alloc(20);

    printf("\nAfter Allocation:\n");
    print_free_lists();

    free_mem(ptr1);

    printf("\nAfter Deallocation:\n");
    print_free_lists();
    }
    else{
        printf("enter positive size and size within range");
    }

    return 0;
}
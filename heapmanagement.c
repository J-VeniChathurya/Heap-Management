#include <stdio.h>
#include <stdlib.h>

#define FIB_SIZE 16  // Define Fibonacci sequence size

// Structure for a memory block
typedef struct Block {
    int size;
    int is_free;
    struct Block *next;
} Block;

Block *heap = NULL;  // Head of the heap list


// Generate Fibonacci series with 16 elements
void generate_fibonacci(int fib[]) {
    fib[0] = 1;
    fib[1] = 2;
    for (int i = 2; i < FIB_SIZE; i++) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }
}


// Initialize heap with Fibonacci-sized blocks
void initialize_heap() {
    int fib[FIB_SIZE];
    generate_fibonacci(fib);
    
    heap = NULL;
    for (int i = FIB_SIZE - 1; i >= 0; i--) {  // Largest blocks first
        Block *new_block = (Block *)malloc(sizeof(Block));
        new_block->size = fib[i];
        new_block->is_free = 1;  // Initially, all blocks are free
        new_block->next = heap;
        heap = new_block;
    }
}


// Allocate memory of requested size
void *allocate(int size) {
    Block *curr = heap;
    Block *best_fit = NULL;

    while (curr) {
        if (curr->is_free && curr->size >= size) {
            if (!best_fit || curr->size < best_fit->size) {
                best_fit = curr;
            }
        }
        curr = curr->next;
    }

    if (best_fit) {
        best_fit->is_free = 0;  // Mark as allocated
        printf("Allocated block at %p (Size: %d)\n", best_fit, best_fit->size);
        return (void *)best_fit;
    } else {
        printf("Memory allocation failed. No suitable block found.\n");
        return NULL;
    }
}


// Check if two numbers are consecutive Fibonacci numbers
int is_fibonacci_pair(int a, int b) {
    int fib[16] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597};
    for (int i = 0; i < 15; i++) {
        if ((fib[i] == a && fib[i + 1] == b) || (fib[i] == b && fib[i + 1] == a)) {
            return 1;
        }
    }
    return 0;
}




void merge_adjacent_blocks(Block *block) {
    if (!block || !block->is_free) return;

    // Try merging with next block
    if (block->next && block->next->is_free && is_fibonacci_pair(block->size, block->next->size)) {
        printf("Merging [%d] and [%d]\n", block->size, block->next->size);
        block->size += block->next->size;

        // Remove next block
        Block *to_remove = block->next;
        block->next = to_remove->next;
        free(to_remove);
    }

    // Try merging with previous block
    Block *prev = heap;
    while (prev && prev->next != block) {
        prev = prev->next;
    }

    if (prev && prev->is_free && is_fibonacci_pair(prev->size, block->size)) {
        printf("Merging [%d] and [%d]\n", prev->size, block->size);
        prev->size += block->size;

        // Remove current block
        prev->next = block->next;
        free(block);
    }
}


// Free allocated memory
void free_memory(void *ptr) {
    if (!ptr) return;
    Block *block = (Block *)ptr  ;
    block->is_free = 1; // Mark block as free
    printf("Freed block at %p (Size: %d)\n", block, block->size);
    merge_adjacent_blocks(block); // Merge adjacent free blocks
}

// Display heap status (One block per line)
void display_heap() {
    Block *curr = heap;
    printf("\nHeap Status:\n");
    while (curr) {
        printf("[Size: %d, %s]\n", curr->size, curr->is_free ? "Free" : "Allocated");
        curr = curr->next;
    }
    printf("NULL\n");
}




int main() {
    initialize_heap();
    display_heap();

    // Allocate memory blocks
    void *p1 = allocate(10);
    display_heap();

    void *p2 = allocate(50);
    display_heap();

    // Free a memory block
    free_memory(p1);
    display_heap();

    // Allocate another block
    void *p3 = allocate(100);
    display_heap();

    // Free more blocks and merge
    free_memory(p2);
    display_heap();
    free_memory(p3);
    display_heap();

    return 0;
}

#include <stdio.h>
#include <stdlib.h>

#define HEAP_SIZE (10 * 1024 * 1024)  // 10 MB
#define STATIC_SIZE (5 * 1024 * 1024) // 5 MB
#define STACK_SIZE (1 * 1024 * 1024)  // 1 MB

// Global variable for static allocation
char static_memory[STATIC_SIZE]; // Allocated in the static segment

int main() {
  // Allocating 10 MB on the heap
  char *heap_memory = malloc(HEAP_SIZE);
  if (heap_memory == NULL) {
    fprintf(stderr, "Heap memory allocation failed\n");
    return 1; // Exit if allocation fails
  }
  printf("Heap memory allocated: %d bytes\n", HEAP_SIZE);

  // Allocating 1 MB on the stack
  char stack_memory[STACK_SIZE]; // Allocated on the stack
  printf("Stack memory allocated: %d bytes\n", STACK_SIZE);

  // Static memory allocation
  printf("Static memory allocated: %d bytes\n", STATIC_SIZE);

  // When the program exits:
  // - Heap memory is returned to the operating system when `free()` is called
  // - Stack memory is automatically deallocated when the function returns
  // - Static memory is returned to the operating system when the program exits

  // Freeing the heap memory
  free(heap_memory);
  printf("Heap memory deallocated\n");

  return 0;
}

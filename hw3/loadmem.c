#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 100

// Define the dyn_block structure
typedef struct {
  int *data;
  int size;
} dyn_block;

// Function to allocate a dyn_block
dyn_block *alloc_dyn_block(int size) {
  dyn_block *block = (dyn_block *)malloc(sizeof(dyn_block));
  if (block == NULL) {
    fprintf(stderr, "Error: Could not allocate memory for dyn_block\n");
    exit(1);
  }
  block->data = (int *)malloc(size * sizeof(int));
  if (block->data == NULL) {
    fprintf(stderr, "Error: Could not allocate memory for data array\n");
    free(block);
    exit(1);
  }
  block->size = size;
  return block;
}

// Function to store an array of integers in a dyn_block
void store_mem_blk(dyn_block *block, int *data, int size) {
  if (block == NULL || block->data == NULL) {
    fprintf(stderr, "Error: dyn_block is not properly allocated\n");
    return;
  }
  for (int i = 0; i < size; i++) {
    block->data[i] = data[i];
  }
  block->size = size; // Update the size
}

// Function to print the content of a dyn_block
void print_block(dyn_block *block) {
  if (block == NULL || block->data == NULL) {
    printf("Empty block\n");
    return;
  }
  printf("Block size: %d\nData: ", block->size);
  for (int i = 0; i < block->size; i++) {
    printf("%d ", block->data[i]);
  }
  printf("\n");
}

// Function to read integers from a file and store them in dynamic blocks
void load_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Error: Could not open file %s\n", filename);
    exit(1);
  }

  char line[1024]; // Buffer to hold each line of input
  int data[BLOCK_SIZE];
  int num, index;

  while (fgets(line, sizeof(line), file)) {
    index = 0;
    char *ptr = line;
    while (sscanf(ptr, "%d", &num) == 1) {
      data[index++] = num;
      while (*ptr != ' ' && *ptr != '\0')
        ptr++; // Move to next number or end of line
      if (*ptr == ' ')
        ptr++;
    }

    // Allocate a dynamic block and store the line of integers
    dyn_block *block = alloc_dyn_block(index);
    store_mem_blk(block, data, index);
    print_block(block);

    free(block->data); 
    free(block);
  }

  fclose(file);
}

int main() {
  const char *filename = "blocks.data";
  load_file(filename);
  return 0;
}

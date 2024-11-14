#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to generate random number using LCG algorithm
int genRand(int min, int max) {
  static unsigned long seed = 123456789;
  unsigned long a = 1664525;
  unsigned long c = 1013904223;
  unsigned long m = 4294967296; // 2^32

  // LCG formula: Xn+1 = (a * Xn + c) % m
  seed = (a * seed + c) % m;

  // Scale the result to the desired range
  return (seed % (max - min + 1)) + min;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <number_of_random_numbers> <file_name> [-a]\n", argv[0]);
    return 1;
  }

  // Get the number of random numbers to generate
  int numRand = atoi(argv[1]);
  if (numRand <= 0) {
    printf("Please enter a valid positive integer for the number of random "
           "numbers.\n");
    return 1;
  }

  // Get the file name
  const char *fileName = argv[2];

  // Check if the third argument is '-a' (append mode)
  int appendMode = (argc == 4 && strcmp(argv[3], "-a") == 0);

  // Open file in append or write mode
  FILE *file = fopen(fileName, appendMode ? "a" : "w");
  if (file == NULL) {
    printf("Error opening file %s\n", fileName);
    return 1;
  }

  // Generate and write random numbers to the file
  for (int i = 0; i < numRand; i++) {
    int randNum = genRand(1, 100); // Generate random number in range [1, 100]
    fprintf(file, "%d\n", randNum);
  }

  // Close the file
  fclose(file);

  printf("%d random numbers generated and written to %s\n", numRand, fileName);
  return 0;
}

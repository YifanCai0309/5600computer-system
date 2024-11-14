#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CHARSET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define MODULUS 2147483647 // A prime number (2^31 - 1)
#define MULTIPLIER 1664525
#define INCREMENT 1013904223
#define MIN_STRINGS 10000 // Minimum number of strings

// Custom random number generator using Linear Congruential Generator (LCG)
unsigned long lcg_rand(unsigned long *seed) {
  *seed = (MULTIPLIER * (*seed) + INCREMENT) % MODULUS;
  return *seed;
}

// Function to generate random strings of specified length
void genRandString(char *str, int length, unsigned long *seed) {
  int charsetSize = strlen(CHARSET); // Size of the character set
  for (int i = 0; i < length; i++) {
    str[i] = CHARSET[lcg_rand(seed) %
                     charsetSize]; // Pick a random character from the charset
  }
  str[length] = '\0'; // Null-terminate the string
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <number_of_strings> <file_name> [-a]\n", argv[0]);
    return 1;
  }

  // Ensure the number of strings is at least 10,000
  int numStrings = atoi(argv[1]); // Number of strings to generate
  if (numStrings < MIN_STRINGS) {
    printf("Number of strings must be at least %d. Generating %d strings.\n",
           MIN_STRINGS, MIN_STRINGS);
    numStrings = MIN_STRINGS; // Set to minimum if less
  }

  const char *fileName = argv[2]; // Output file name
  int appendMode =
      (argc == 4 && strcmp(argv[3], "-a") == 0); // Check if '-a' option is used

  FILE *file;
  if (appendMode) {
    file = fopen(fileName, "a"); // Open file in append mode
  } else {
    file = fopen(fileName, "w"); // Open file in write mode (overwrite)
  }

  if (file == NULL) {
    printf("Error opening file.\n");
    return 1;
  }

  // Seed random number generator using the current time
  unsigned long seed = (unsigned long)time(NULL);

  char buffer[11]; // Adjust for strings between 5-10 characters
  for (int i = 0; i < numStrings; i++) {
    int stringLength =
        (lcg_rand(&seed) % 6) + 5; // Random length between 5 and 10
    genRandString(buffer, stringLength, &seed);
    fprintf(file, "%s\n", buffer); // Write the random string to the file
  }

  fclose(file); // Close the file
  printf("Generated %d strings and written to %s\n", numStrings, fileName);

  return 0;
}

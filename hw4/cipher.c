#include "polybius.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: cipher -e \"plaintext\" or cipher -d \"ciphertext\"\n");
    return 1;
  }

  polybius_table_t table;
  initTable(&table);

  char result[256]; // Buffer to hold the result

  if (strcmp(argv[1], "-e") == 0) {
    pbEncode(argv[2], table, result);
    printf("Encoded: %s\n", result);
  } else if (strcmp(argv[1], "-d") == 0) {
    pbDecode(argv[2], table, result);
    printf("Decoded: %s\n", result);
  } else {
    printf("Invalid option. Use -e for encode or -d for decode.\n");
  }

  return 0;
}

#include "polybius.h"
#include <stdio.h>
#include <string.h>

void pbEncode(const char *plaintext, char *encodedText,
              const char table[5][5]) {
  char c;
  int row, col;

  for (int i = 0, j = 0; plaintext[i] != '\0'; i++) {
    c = plaintext[i];

    // Convert character to uppercase and handle 'J' as 'I'
    if (c >= 'a' && c <= 'z')
      c -= 'a' - 'A';
    if (c == 'J')
      c = 'I';

    // Handle alphabetic characters
    if (c >= 'A' && c <= 'Z') {
      int found = 0;
      for (row = 0; row < 5 && !found; row++) {
        for (col = 0; col < 5 && !found; col++) {
          if (table[row][col] == c) {
            encodedText[j++] = '1' + row; // Row (1-5)
            encodedText[j++] = '1' + col; // Column (1-5)
            found = 1;
          }
        }
      }
    } else if (c == ' ') {
      encodedText[j++] = ' ';
    }
  }
  encodedText[strlen(encodedText)] = '\0'; // Null-terminate the encoded string
}

int main() {
  char buffer[256];
  char encodedText[512]; // Enough space for encoding
  char table[5][5] = {{'A', 'B', 'C', 'D', 'E'},
                      {'F', 'G', 'H', 'I', 'K'}, // I/J combined
                      {'L', 'M', 'N', 'O', 'P'},
                      {'Q', 'R', 'S', 'T', 'U'},
                      {'V', 'W', 'X', 'Y', 'Z'}};

  // Read input from stdin (pipe from parent)
  while (fgets(buffer, sizeof(buffer), stdin)) {
    pbEncode(buffer, encodedText, table);
    printf("%s\n", encodedText); // Output the encoded result
  }

  return 0;
}

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "polybius.h"

// Initialize the Polybius square, excluding 'J', combining it with 'I'
void initTable(polybius_table_t *table) {
    char alphabet[] = "ABCDEFGHIKLMNOPQRSTUVWXYZ"; // 'I' and 'J' share the same position
    int idx = 0;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            table->grid[row][col] = alphabet[idx++];
        }
    }
}

// Encoding function: encodes the plaintext to Polybius ciphertext, preserving spaces
void pbEncode(const char *plaintext, polybius_table_t table, char *ciphertext) {
    int length = strlen(plaintext);
    int cipherIdx = 0;

    for (int i = 0; i < length; i++) {
        char ch = toupper(plaintext[i]);

        // If the character is a space, add it directly to the ciphertext
        if (ch == ' ') {
            ciphertext[cipherIdx++] = ' ';
            continue;
        }

        // Skip non-alphabetic characters
        if (ch < 'A' || ch > 'Z' || ch == 'J') {
            continue;
        }

        // Treat 'J' as 'I'
        if (ch == 'J') {
            ch = 'I';
        }

        // Find the character in the table and convert it to its grid position
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 5; col++) {
                if (table.grid[row][col] == ch) {
                    ciphertext[cipherIdx++] = '1' + row; // Row number
                    ciphertext[cipherIdx++] = '1' + col; // Column number
                    break;
                }
            }
        }
    }

    ciphertext[cipherIdx] = '\0'; // Null-terminate the ciphertext
}

// Decoding function: decodes the Polybius ciphertext back to plaintext, preserving spaces
void pbDecode(const char *ciphertext, polybius_table_t table, char *plaintext) {
    int length = strlen(ciphertext);
    int plainIdx = 0;

    for (int i = 0; i < length; i++) {
        // If the character is a space, copy it directly to the plaintext
        if (ciphertext[i] == ' ') {
            plaintext[plainIdx++] = ' ';
            continue;
        }

        // Convert pairs of digits back to the corresponding letter in the table
        int row = ciphertext[i] - '1';
        int col = ciphertext[++i] - '1';

        plaintext[plainIdx++] = table.grid[row][col];
    }

    plaintext[plainIdx] = '\0'; // Null-terminate the plaintext
}

#ifndef POLYBIUS_H
#define POLYBIUS_H

// Define a table structure for the Polybius cipher
typedef struct {
  char grid[5][5]; // 5x5 grid for alphabet (A-Z)
} polybius_table_t;

// Function prototypes
void pbEncode(const char *plaintext, polybius_table_t table, char *ciphertext);
void pbDecode(const char *ciphertext, polybius_table_t table, char *plaintext);
void initTable(polybius_table_t *table);

#endif

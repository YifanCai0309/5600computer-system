#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to split a line into words and add them to the queue
void enqueue_words(queue_t *queue, char *line) {
  char *word = strtok(line, " \n");
  while (word != NULL) {
    // Dynamically allocate memory for each word
    char *wordCopy = (char *)malloc(strlen(word) + 1);
    strcpy(wordCopy, word);

    // Add word to the queue
    push_queue(queue, wordCopy);

    // Get the next word
    word = strtok(NULL, " \n");
  }
}

int main() {
  queue_t queue;
  initialize_queue(&queue);

  FILE *file = fopen("output.txt", "r");
  if (file == NULL) {
    printf("Error opening file.\n");
    return 1;
  }

  char line[256];
  while (fgets(line, sizeof(line), file)) {
    enqueue_words(&queue, line);
  }

  fclose(file);

  // Display the contents of the queue
  printf("Words in the queue:\n");
  while (get_queue_size(&queue) > 0) {
    char *word = (char *)pop_queue(&queue);
    printf("%s\n", word);
    free(word); // Free the memory after printing
  }

  return 0;
}

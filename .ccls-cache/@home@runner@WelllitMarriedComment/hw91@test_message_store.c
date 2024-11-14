#include "message_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_MESSAGES 160
#define ACCESS_COUNT 1000

int main() {
  // Initialize cache
  init_cache();

  // Seed random number generator
  srand(time(NULL));

  // Create and store messages
  printf("Storing messages...\n");
  for (int i = 0; i < TOTAL_MESSAGES; i++) {
    char sender[20];
    char receiver[20];
    char content[100];

    sprintf(sender, "Sender%d", i);
    sprintf(receiver, "Receiver%d", i);
    sprintf(content, "This is message number %d.", i);

    Message *msg = create_msg(sender, receiver, content);
    if (store_msg(msg) != 0) {
      printf("Failed to store message ID %d\n", msg->id);
      free(msg);
    }
    // No need to free msg here as it's managed by the cache
  }

  // Set replacement policy
  // set_replacement_policy(LRU_REPLACEMENT);
  set_replacement_policy(RANDOM_REPLACEMENT);
  // replacement

  // Metrics
  int cache_hits = 0;
  int cache_misses = 0;

  printf("Accessing messages...\n");
  for (int i = 0; i < ACCESS_COUNT; i++) {
    int msg_id = rand() % TOTAL_MESSAGES + 1;

    // Check if message is in cache before retrieval
    if (cache_get(msg_id)) {
      cache_hits++;
    } else {
      cache_misses++;
    }

    // Retrieve the message (loads into cache if not present)
    Message *msg = retrieve_msg(msg_id);
    if (!msg) {
      printf("Message ID %d not found.\n", msg_id);
    }
  }

  // Calculate and display metrics
  printf("Cache Hits: %d\n", cache_hits);
  printf("Cache Misses: %d\n", cache_misses);
  double hit_ratio = (double)cache_hits / ACCESS_COUNT;
  printf("Cache Hit Ratio: %.2f%%\n", hit_ratio * 100);

  // Clean up
  free_cache();
  return 0;
}

#include "cache.h"
#include "message.h"
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
  // Uncomment one of the following lines to set the desired policy
  set_replacement_policy(LRU_REPLACEMENT);
  // set_replacement_policy(RANDOM_REPLACEMENT);

  // Reset metrics before accessing
  reset_metrics();

  // Access messages randomly
  printf("Accessing messages...\n");
  for (int i = 0; i < ACCESS_COUNT; i++) {
    int msg_id = rand() % TOTAL_MESSAGES + 1;

    // Retrieve the message (loads into cache if not present)
    Message *msg = retrieve_msg(msg_id);
    if (!msg) {
      printf("Message ID %d not found.\n", msg_id);
    }
  }

  // Get and display metrics
  CacheMetrics metrics = get_metrics();
  printf("Cache Hits: %d\n", metrics.cache_hits);
  printf("Cache Misses: %d\n", metrics.cache_misses);
  double hit_ratio = (metrics.cache_hits + metrics.cache_misses) > 0
                         ? ((double)metrics.cache_hits /
                            (metrics.cache_hits + metrics.cache_misses)) *
                               100
                         : 0.0;
  printf("Cache Hit Ratio: %.2f%%\n", hit_ratio);

  // Clean up
  free_cache();
  return 0;
}

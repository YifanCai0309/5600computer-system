#include "message_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_MESSAGES 160
#define ACCESS_COUNT 1000

int main() {
  Cache cache;
  init_cache(&cache, LRU_REPLACEMENT);

  printf("Storing messages...\n");
  for (int i = 0; i < TOTAL_MESSAGES; i++) {
    char sender[20];
    char receiver[20];
    char content[100];

    sprintf(sender, "Sender%d", i);
    sprintf(receiver, "Receiver%d", i);
    sprintf(content, "This is message number %d.", i);

    Message *msg = create_msg(sender, receiver, content);
    if (store_msg(msg, &cache) != 0) {
      printf("Failed to store message ID %d\n", msg->id);
      free(msg);
    }
  }

  int cache_hits = 0;
  int cache_misses = 0;

  printf("Accessing messages...\n");
  for (int i = 0; i < ACCESS_COUNT; i++) {
    int msg_id = rand() % TOTAL_MESSAGES + 1;

    if (cache_get(&cache, msg_id)) {
      cache_hits++;
    } else {
      cache_misses++;
    }

    Message *msg = retrieve_msg(msg_id, &cache);
    if (!msg) {
      printf("Message ID %d not found.\n", msg_id);
    }
  }

  printf("Cache Hits: %d\n", cache_hits);
  printf("Cache Misses: %d\n", cache_misses);
  double hit_ratio = (double)cache_hits / ACCESS_COUNT;
  printf("Cache Hit Ratio: %.2f%%\n", hit_ratio * 100);

  free_cache(&cache);
  return 0;
}

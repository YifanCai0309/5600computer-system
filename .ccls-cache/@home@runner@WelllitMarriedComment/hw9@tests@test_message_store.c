#include "cache.h"
#include "message_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_MESSAGES 160
#define ACCESS_COUNT 1000

int main() {
  init_cache();
  srand(time(NULL));

  for (int i = 0; i < TOTAL_MESSAGES; i++) {
    char sender[20], receiver[20], content[100];
    sprintf(sender, "Sender%d", i);
    sprintf(receiver, "Receiver%d", i);
    sprintf(content, "Message %d", i);

    Message *msg = create_msg(sender, receiver, content);
    store_msg(msg);
  }

  int hits = 0, misses = 0;
  for (int i = 0; i < ACCESS_COUNT; i++) {
    int id = rand() % TOTAL_MESSAGES + 1;
    if (cache_get(id)) {
      hits++;
    } else {
      retrieve_msg(id);
      misses++;
    }
  }

  printf("Hits: %d, Misses: %d, Hit Ratio: %.2f%%\n", hits, misses,
         (double)hits / ACCESS_COUNT * 100);

  free_cache();
  return 0;
}

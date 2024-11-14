#include "message_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_MESSAGES 160  // 5% cache  16/160
#define ACCESS_COUNT 1000

// Compare RANDOM_REPLACEMENT / LRU_REPLACEMENT
void compare_cache_policies() {
    Cache cache;

    // Test RANDOM_REPLACEMENT policy
    printf("Testing with RANDOM_REPLACEMENT policy...\n");
    init_cache(&cache, RANDOM_REPLACEMENT);

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

    printf("Cache Hits with RANDOM_REPLACEMENT: %d\n", cache_hits);
    printf("Cache Misses with RANDOM_REPLACEMENT: %d\n", cache_misses);
    double hit_ratio = (double)cache_hits / ACCESS_COUNT;
    printf("Cache Hit Ratio with RANDOM_REPLACEMENT: %.2f%%\n", hit_ratio * 100);

    free_cache(&cache);

    // Test LRU_REPLACEMENT policy
    printf("\nTesting with LRU_REPLACEMENT policy...\n");
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

    cache_hits = 0;
    cache_misses = 0;

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

    printf("Cache Hits with LRU_REPLACEMENT: %d\n", cache_hits);
    printf("Cache Misses with LRU_REPLACEMENT: %d\n", cache_misses);
    hit_ratio = (double)cache_hits / ACCESS_COUNT;
    printf("Cache Hit Ratio with LRU_REPLACEMENT: %.2f%%\n", hit_ratio * 100);

    free_cache(&cache);
}

int main() {
    srand(time(NULL)); // Set random seed every time the program runs
    compare_cache_policies(); // Compare RANDOM_REPLACEMENT and LRU_REPLACEMENT
    return 0;
}

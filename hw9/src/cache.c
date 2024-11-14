#include "cache.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CACHE_SIZE 16 // Number of messages the cache can hold

// Structure for cache entries
typedef struct CacheEntry {
  Message *msg;
  time_t last_accessed;
} CacheEntry;

// Global cache
static CacheEntry cache_entries[CACHE_SIZE];

// Current replacement policy
static int current_policy = LRU_REPLACEMENT;

// Metrics
static CacheMetrics metrics = {0, 0};

// Initialize cache
void init_cache() {
  for (int i = 0; i < CACHE_SIZE; i++) {
    cache_entries[i].msg = NULL;
    cache_entries[i].last_accessed = 0;
  }
  reset_metrics();
}

// Free cache resources
void free_cache() {
  for (int i = 0; i < CACHE_SIZE; i++) {
    if (cache_entries[i].msg != NULL) {
      free(cache_entries[i].msg);
      cache_entries[i].msg = NULL;
    }
  }
}

// Set replacement policy
void set_replacement_policy(int policy) {
  if (policy == RANDOM_REPLACEMENT || policy == LRU_REPLACEMENT) {
    current_policy = policy;
  }
}

// Get message from cache
Message *cache_get(int msg_id) {
  for (int i = 0; i < CACHE_SIZE; i++) {
    if (cache_entries[i].msg && cache_entries[i].msg->id == msg_id) {
      cache_entries[i].last_accessed = time(NULL);
      metrics.cache_hits++;
      return cache_entries[i].msg;
    }
  }
  metrics.cache_misses++;
  return NULL;
}

// Select a cache index based on the current replacement policy
static int select_replacement_index() {
  if (current_policy == RANDOM_REPLACEMENT) {
    return rand() % CACHE_SIZE;
  } else if (current_policy == LRU_REPLACEMENT) {
    time_t oldest_time = LONG_MAX;
    int oldest_index = 0;
    for (int i = 0; i < CACHE_SIZE; i++) {
      if (cache_entries[i].last_accessed < oldest_time) {
        oldest_time = cache_entries[i].last_accessed;
        oldest_index = i;
      }
    }
    return oldest_index;
  }
  // Default to random if policy is unknown
  return rand() % CACHE_SIZE;
}

// Put message into cache
void cache_put(Message *msg) {
  if (!msg)
    return;

  // Check for empty slot
  for (int i = 0; i < CACHE_SIZE; i++) {
    if (cache_entries[i].msg == NULL) {
      cache_entries[i].msg = msg;
      cache_entries[i].last_accessed = time(NULL);
      return;
    }
  }

  // No empty slot; apply replacement policy
  int replace_index = select_replacement_index();

  // Replace the existing message
  if (cache_entries[replace_index].msg != NULL) {
    free(cache_entries[replace_index].msg);
  }

  cache_entries[replace_index].msg = msg;
  cache_entries[replace_index].last_accessed = time(NULL);
}

// Reset metrics
void reset_metrics() {
  metrics.cache_hits = 0;
  metrics.cache_misses = 0;
}

// Get current metrics
CacheMetrics get_metrics() { return metrics; }

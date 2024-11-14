#ifndef CACHE_H
#define CACHE_H

#include "message.h"

// Replacement policies
#define RANDOM_REPLACEMENT 1
#define LRU_REPLACEMENT 2

// Initialize and free cache
void init_cache();
void free_cache();

// Cache operations
Message *cache_get(int msg_id);
void cache_put(Message *msg);

// Set replacement policy
void set_replacement_policy(int policy);

// Metrics
typedef struct CacheMetrics {
  int cache_hits;
  int cache_misses;
} CacheMetrics;

void reset_metrics();
CacheMetrics get_metrics();

#endif // CACHE_H

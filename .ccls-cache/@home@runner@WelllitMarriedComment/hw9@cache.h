#ifndef CACHE_H
#define CACHE_H

#include "message_store.h"

#define CACHE_SIZE 16
#define RANDOM_REPLACEMENT 1
#define LRU_REPLACEMENT 2

void init_cache();
void free_cache();
Message *cache_get(int msg_id);
void cache_put(Message *msg);
void set_replacement_policy(int policy);

#endif // CACHE_H

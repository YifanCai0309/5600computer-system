#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>
#include <time.h>

#define MAX_SENDER_LEN 50
#define MAX_RECEIVER_LEN 50
#define MAX_CONTENT_LEN 900 // Fixed message content size
#define CACHE_SIZE 16       // Cache capacity

// Replacement policies
#define RANDOM_REPLACEMENT 1
#define LRU_REPLACEMENT 2

#define HASH_TABLE_SIZE 256 // Size of the hash table for cache nodes

typedef struct Message {
  int id;
  time_t time_sent;
  char sender[MAX_SENDER_LEN];
  char receiver[MAX_RECEIVER_LEN];
  char content[MAX_CONTENT_LEN];
  bool delivered;
} Message;

typedef struct CacheNode {
  Message *msg;
  struct CacheNode *prev;
  struct CacheNode *next;
  struct CacheNode *hash_next; // For collision handling in hash table
} CacheNode;

typedef struct Cache {
  CacheNode *head;
  CacheNode *tail;
  int size;
  int policy;
  CacheNode *hash_table[HASH_TABLE_SIZE]; // Hash table for quick access
} Cache;

// Cache function declarations
void init_cache(Cache *cache, int policy);
void free_cache(Cache *cache);
Message *cache_get(Cache *cache, int msg_id);
void cache_put(Cache *cache, Message *msg);

#endif

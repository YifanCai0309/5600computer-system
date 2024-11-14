#include "cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Hash function to map message IDs to hash table indices
unsigned int hash_function(int msg_id) { return msg_id % HASH_TABLE_SIZE; }

// Initialize cache
void init_cache(Cache *cache, int policy) {
  if (!cache) {
    fprintf(stderr, "Error: Cache is NULL.\n");
    return;
  }
  cache->head = NULL;
  cache->tail = NULL;
  cache->size = 0;
  cache->policy = policy;
  memset(cache->hash_table, 0, sizeof(cache->hash_table));
}

// Free cache
void free_cache(Cache *cache) {
  if (!cache) {
    fprintf(stderr, "Error: Cache is not initialized.\n");
    return;
  }
  CacheNode *current = cache->head;
  while (current) {
    CacheNode *temp = current;
    current = current->next;
    free(temp->msg);
    free(temp);
  }
  cache->head = cache->tail = NULL;
  cache->size = 0;
  memset(cache->hash_table, 0, sizeof(cache->hash_table));
}

CacheNode* get_node_at_index(Cache *cache, int index) {
    if (index < 0 || index >= cache->size) {
        return NULL;
    }

    CacheNode *current = cache->head;
    for (int i = 0; i < index; i++) {
        if (current != NULL) {
            current = current->next;
        }
    }
    return current;
}
// Detach a node from the cache linked list without freeing it
void detach_node(Cache *cache, CacheNode *node) {
  if (!cache || !node)
    return;

  // Remove from linked list
  if (node->prev) {
    node->prev->next = node->next;
  } else {
    cache->head = node->next;
  }

  if (node->next) {
    node->next->prev = node->prev;
  } else {
    cache->tail = node->prev;
  }

  cache->size--;
  node->prev = NULL;
  node->next = NULL;
}

// Remove a node from cache and free it
void remove_node(Cache *cache, CacheNode *node) {
  if (!cache || !node)
    return;

  // Detach from linked list
  detach_node(cache, node);

  // Remove from hash table
  unsigned int index = hash_function(node->msg->id);
  CacheNode *current = cache->hash_table[index];
  CacheNode *prev = NULL;
  while (current) {
    if (current == node) {
      if (prev) {
        prev->hash_next = current->hash_next;
      } else {
        cache->hash_table[index] = current->hash_next;
      }
      break;
    }
    prev = current;
    current = current->hash_next;
  }

  // Free memory
  free(node->msg);
  free(node);
}

// Add a node to the head of the cache linked list
void add_to_head(Cache *cache, CacheNode *node) {
  if (!cache || !node)
    return;

  node->next = cache->head;
  node->prev = NULL;

  if (cache->head) {
    cache->head->prev = node;
  }
  cache->head = node;

  if (!cache->tail) {
    cache->tail = node;
  }

  cache->size++;
}

// Insert a node into the hash table
void hash_table_insert(Cache *cache, CacheNode *node) {
  unsigned int index = hash_function(node->msg->id);
  node->hash_next = cache->hash_table[index];
  cache->hash_table[index] = node;
}

// Get a message from the cache using the hash table
Message *cache_get(Cache *cache, int msg_id) {
  if (!cache)
    return NULL;

  unsigned int index = hash_function(msg_id);
  CacheNode *node = cache->hash_table[index];
  while (node) {
    if (node->msg && node->msg->id == msg_id) {
      if (cache->policy == LRU_REPLACEMENT && node != cache->head) {
        detach_node(cache, node);
        add_to_head(cache, node);
      }
      return node->msg;
    }
    node = node->hash_next;
  }
  return NULL; // Cache miss
}

// Add a message to the cache
void cache_put(Cache *cache, Message *msg) {
  if (!cache)
    return;

  if (cache->size >= CACHE_SIZE) {
//    CacheNode *node_to_remove =
//        cache->policy == LRU_REPLACEMENT ? cache->tail : cache->head;
//    remove_node(cache, node_to_remove);
      CacheNode *node_to_remove=NULL;;
      // LRU_REPLACEMENT
      if (cache->policy == LRU_REPLACEMENT) {
          node_to_remove = cache->tail;
      }
      // RANDOM_REPLACEMENT
      else if (cache->policy == RANDOM_REPLACEMENT) {
          int random_index = rand() % cache->size;
          node_to_remove = get_node_at_index(cache, random_index);
      }
      if (node_to_remove) {
          remove_node(cache, node_to_remove);
      }
  }

  CacheNode *new_node = (CacheNode *)malloc(sizeof(CacheNode));
  if (!new_node) {
    fprintf(stderr, "Error: Failed to allocate memory for CacheNode.\n");
    return;
  }

  new_node->msg = msg;
  new_node->prev = NULL;
  new_node->next = NULL;
  new_node->hash_next = NULL;

  // Insert into linked list and hash table
  add_to_head(cache, new_node);
  hash_table_insert(cache, new_node);
}

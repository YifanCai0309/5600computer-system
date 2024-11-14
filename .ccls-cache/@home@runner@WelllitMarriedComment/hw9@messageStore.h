#ifndef MESSAGE_STORE_H
#define MESSAGE_STORE_H

#include <time.h>
#include <stdbool.h>

#define MAX_SENDER_LEN 50
#define MAX_RECEIVER_LEN 50
#define MAX_CONTENT_LEN 900  // Adjusted to fit within fixed message size

#define MESSAGE_SIZE 1024  // Fixed size for each message
#define CACHE_SIZE 16      // Number of messages the cache can hold

typedef struct Message {
    int id;
    time_t time_sent;
    char sender[MAX_SENDER_LEN];
    char receiver[MAX_RECEIVER_LEN];
    char content[MAX_CONTENT_LEN];
    bool delivered;
} Message;

// Function prototypes
Message* create_msg(const char* sender, const char* receiver, const char* content);
int store_msg(Message* msg);
Message* retrieve_msg(int msg_id);

// Cache management functions
void init_cache();
void free_cache();
Message* cache_get(int msg_id);
void cache_put(Message* msg);
void set_replacement_policy(int policy);

#endif  // MESSAGE_STORE_H

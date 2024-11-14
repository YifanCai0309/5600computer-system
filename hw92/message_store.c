#include "message_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_STORE_FILE "message_store.dat"

// Generate unique message ID
static int generate_unique_id() {
  static int id_counter = 0;
  return ++id_counter;
}

// Create a new message
Message *create_msg(const char *sender, const char *receiver,
                    const char *content) {
  if (!sender || !receiver || !content) {
    return NULL;
  }


  Message *msg = (Message *)malloc(sizeof(Message));
  if (!msg) {
    return NULL;
  }

  msg->id = generate_unique_id();
  msg->time_sent = time(NULL);
  strncpy(msg->sender, sender, MAX_SENDER_LEN - 1);
  msg->sender[MAX_SENDER_LEN - 1] = '\0';
  strncpy(msg->receiver, receiver, MAX_RECEIVER_LEN - 1);
  msg->receiver[MAX_RECEIVER_LEN - 1] = '\0';
  strncpy(msg->content, content, MAX_CONTENT_LEN - 1);
  msg->content[MAX_CONTENT_LEN - 1] = '\0';
  msg->delivered = false;

  return msg;
}

// Store a message on disk
int store_msg(Message *msg, Cache *cache) {
  if (!msg) {
    return -1;
  }

  FILE *fp = fopen(MESSAGE_STORE_FILE, "ab");
  if (!fp) {
    fprintf(stderr, "Error opening message store file.\n");
    return -1;
  }

  size_t write_count = fwrite(msg, sizeof(Message), 1, fp);
  fclose(fp);

  if (write_count != 1) {
    fprintf(stderr, "Error writing message to disk.\n");
    return -1;
  }

  cache_put(cache, msg);
  return 0;
}

// Retrieve a message from disk
Message *retrieve_msg_from_disk(int msg_id) {
  FILE *fp = fopen(MESSAGE_STORE_FILE, "rb");
  if (!fp) {
    fprintf(stderr, "Error: Could not open message store file.\n");
    return NULL;
  }

  Message *msg = (Message *)malloc(sizeof(Message));
  if (!msg) {
    fclose(fp);
    return NULL;
  }

  while (fread(msg, sizeof(Message), 1, fp) == 1) {
    if (msg->id == msg_id) {
      fclose(fp);
      return msg;
    }
  }

  free(msg);
  fclose(fp);
  return NULL;
}

// Retrieve a message
Message *retrieve_msg(int msg_id, Cache *cache) {
  if (!cache) {
    fprintf(stderr, "Error: Cache is not initialized.\n");
    return NULL;
  }

  Message *msg = cache_get(cache, msg_id);
  if (msg) {
    return msg;
  }

  msg = retrieve_msg_from_disk(msg_id);
  if (msg) {
    cache_put(cache, msg);
  }

  return msg;
}

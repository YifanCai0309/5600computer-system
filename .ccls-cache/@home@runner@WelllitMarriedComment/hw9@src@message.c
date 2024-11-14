#include "message.h"
#include "cache.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define storage files
#define MESSAGE_STORE_FILE "message_store.dat"
#define INDEX_FILE "message_index.dat"

// Structure for index entries
typedef struct IndexEntry {
  int msg_id;
  long file_offset;
} IndexEntry;

// Function to generate unique message IDs
static int generate_unique_id() {
  static int id_counter = 0;
  return ++id_counter;
}

// Helper function to write index entry
static int write_index_entry(IndexEntry *index_entry) {
  FILE *index_fp = fopen(INDEX_FILE, "ab");
  if (!index_fp) {
    return -1;
  }

  size_t write_count = fwrite(index_entry, sizeof(IndexEntry), 1, index_fp);
  fclose(index_fp);

  if (write_count != 1) {
    return -1;
  }

  return 0;
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

// Helper function to load index entries
static IndexEntry *load_index_entries(int *count) {
  FILE *index_fp = fopen(INDEX_FILE, "rb");
  if (!index_fp) {
    *count = 0;
    return NULL;
  }

  fseek(index_fp, 0, SEEK_END);
  long size = ftell(index_fp);
  fseek(index_fp, 0, SEEK_SET);

  *count = size / sizeof(IndexEntry);
  IndexEntry *entries = (IndexEntry *)malloc(size);
  if (!entries) {
    fclose(index_fp);
    return NULL;
  }

  size_t read_count = fread(entries, sizeof(IndexEntry), *count, index_fp);
  fclose(index_fp);

  if (read_count != (size_t)(*count)) {
    // Read error
    free(entries);
    return NULL;
  }

  return entries;
}

// Retrieve message from disk
static Message *retrieve_msg_from_disk(int msg_id) {
  int index_count = 0;
  IndexEntry *index_entries = load_index_entries(&index_count);
  if (!index_entries) {
    return NULL;
  }

  long offset = -1;
  for (int i = 0; i < index_count; i++) {
    if (index_entries[i].msg_id == msg_id) {
      offset = index_entries[i].file_offset;
      break;
    }
  }
  free(index_entries);

  if (offset == -1) {
    return NULL;
  }

  FILE *fp = fopen(MESSAGE_STORE_FILE, "rb");
  if (!fp) {
    return NULL;
  }

  if (fseek(fp, offset, SEEK_SET) != 0) {
    fclose(fp);
    return NULL;
  }

  Message *msg = (Message *)malloc(sizeof(Message));
  if (!msg) {
    fclose(fp);
    return NULL;
  }

  size_t read_count = fread(msg, sizeof(Message), 1, fp);
  fclose(fp);

  if (read_count != 1) {
    // Read error
    free(msg);
    return NULL;
  }

  return msg;
}

// Store message to disk
int store_msg(Message *msg) {
  if (!msg) {
    return -1;
  }

  FILE *fp = fopen(MESSAGE_STORE_FILE, "ab");
  if (!fp) {
    return -1;
  }

  long offset = ftell(fp);
  size_t write_count = fwrite(msg, sizeof(Message), 1, fp);
  fclose(fp);

  if (write_count != 1) {
    return -1;
  }

  // Write to index file
  IndexEntry index_entry = {msg->id, offset};
  if (write_index_entry(&index_entry) != 0) {
    return -1;
  }

  // Place the message in the cache
  cache_put(msg);

  return 0;
}

// Retrieve message by ID
Message *retrieve_msg(int msg_id) {
  // Check cache first
  Message *msg = cache_get(msg_id);
  if (msg) {
    return msg; // Cache hit
  }

  // Cache miss; retrieve from disk
  msg = retrieve_msg_from_disk(msg_id);
  if (msg) {
    // Place in cache
    cache_put(msg);
  }

  return msg;
}

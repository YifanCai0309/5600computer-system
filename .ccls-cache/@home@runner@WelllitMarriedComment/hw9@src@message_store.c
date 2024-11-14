#include "message_store.h"
#include "cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_STORE_FILE "message_store.dat"
#define INDEX_FILE "message_index.dat"

static int generate_unique_id() {
  static int id_counter = 0;
  return ++id_counter;
}

Message *create_msg(const char *sender, const char *receiver,
                    const char *content) {
  if (!sender || !receiver || !content) {
    return NULL;
  }

  Message *msg = malloc(sizeof(Message));
  if (!msg) {
    return NULL;
  }

  msg->id = generate_unique_id();
  msg->time_sent = time(NULL);
  strncpy(msg->sender, sender, MAX_SENDER_LEN - 1);
  strncpy(msg->receiver, receiver, MAX_RECEIVER_LEN - 1);
  strncpy(msg->content, content, MAX_CONTENT_LEN - 1);
  msg->delivered = false;

  return msg;
}

int store_msg(Message *msg) {
  if (!msg)
    return -1;

  FILE *fp = fopen(MESSAGE_STORE_FILE, "ab");
  if (!fp)
    return -1;

  long offset = ftell(fp);
  fwrite(msg, sizeof(Message), 1, fp);
  fclose(fp);

  cache_put(msg);
  return 0;
}

Message *retrieve_msg(int msg_id) {
  Message *msg = cache_get(msg_id);
  if (msg)
    return msg;

  FILE *fp = fopen(MESSAGE_STORE_FILE, "rb");
  if (!fp)
    return NULL;

  Message temp;
  while (fread(&temp, sizeof(Message), 1, fp)) {
    if (temp.id == msg_id) {
      fclose(fp);
      msg = malloc(sizeof(Message));
      memcpy(msg, &temp, sizeof(Message));
      cache_put(msg);
      return msg;
    }
  }

  fclose(fp);
  return NULL;
}

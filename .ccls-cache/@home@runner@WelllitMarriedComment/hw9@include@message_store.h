#ifndef MESSAGE_STORE_H
#define MESSAGE_STORE_H

#include <stdbool.h>
#include <time.h>

#define MAX_SENDER_LEN 50
#define MAX_RECEIVER_LEN 50
#define MAX_CONTENT_LEN 900

typedef struct Message {
  int id;
  time_t time_sent;
  char sender[MAX_SENDER_LEN];
  char receiver[MAX_RECEIVER_LEN];
  char content[MAX_CONTENT_LEN];
  bool delivered;
} Message;

Message *create_msg(const char *sender, const char *receiver,
                    const char *content);
int store_msg(Message *msg);
Message *retrieve_msg(int msg_id);

#endif // MESSAGE_STORE_H

#ifndef MESSAGE_STORE_H
#define MESSAGE_STORE_H

#include "cache.h"

// Message store function declarations
Message *create_msg(const char *sender, const char *receiver,
                    const char *content);
int store_msg(Message *msg, Cache *cache);
Message *retrieve_msg(int msg_id, Cache *cache);

#endif

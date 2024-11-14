#ifndef QUEUE_H
#define QUEUE_H

// Process structure
typedef struct {
  int identifier;
  char *name;
  long runtime;
  int priority;
} process_t;

// Queue node structure
typedef struct queue_node {
  void *data;
  struct queue_node *next;
  struct queue_node *prev;
} queue_node_t;

// Queue structure
typedef struct {
  queue_node_t *head;
  queue_node_t *tail;
  int size;
} queue_t;

// Function prototypes
void push_queue(queue_t *queue, void *element);
void *pop_queue(queue_t *queue);
process_t *remove_process(queue_t *queue);
int get_queue_size(queue_t *queue);
void initialize_queue(queue_t *queue);

#endif 

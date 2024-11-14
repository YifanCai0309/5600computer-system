#ifndef QUEUE_H
#define QUEUE_H

// Define process_t to represent a process
typedef struct process {
  int id;
  char *name;
  long run_time;
  int priority;
} process_t;

// Define node_t for use in the queue
typedef struct node {
  void *data;
  struct node *next;
  struct node *prev;
} node_t;

// Define queue_t as a doubly linked list
typedef struct queue {
  node_t *front;
  node_t *rear;
  int size;
} queue_t;

// Function declarations
queue_t *create_queue();
void push_queue(queue_t *queue, void *element);
void *pop_queue(queue_t *queue);
process_t *remove_process(queue_t *queue);
int get_queue_size(queue_t *queue);
void destroy_queue(queue_t *queue);

#endif

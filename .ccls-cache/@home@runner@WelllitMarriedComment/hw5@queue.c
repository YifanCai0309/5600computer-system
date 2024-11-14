#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

// Function to create a new queue
queue_t *create_queue() {
  queue_t *queue = (queue_t *)malloc(sizeof(queue_t));
  queue->front = queue->rear = NULL;
  queue->size = 0;
  return queue;
}

// Function to push an element into the queue
void push_queue(queue_t *queue, void *element) {
  node_t *new_node = (node_t *)malloc(sizeof(node_t));
  new_node->data = element;
  new_node->next = NULL;
  new_node->prev = queue->rear;

  if (queue->rear) {
    queue->rear->next = new_node;
  } else {
    queue->front = new_node;
  }

  queue->rear = new_node;
  queue->size++;
}

// Function to pop an element from the front of the queue
void *pop_queue(queue_t *queue) {
  if (queue->size == 0)
    return NULL;

  node_t *front_node = queue->front;
  void *data = front_node->data;

  queue->front = front_node->next;

  if (queue->front) {
    queue->front->prev = NULL;
  } else {
    queue->rear = NULL;
  }

  free(front_node);
  queue->size--;
  return data;
}

// Function to remove the process with the highest priority
process_t *remove_process(queue_t *queue) {
  if (queue->size == 0)
    return NULL;

  node_t *current = queue->front;
  node_t *highest_priority_node = current;
  process_t *highest_priority_process = (process_t *)current->data;

  // Find the node with the highest priority
  while (current) {
    process_t *current_process = (process_t *)current->data;
    if (current_process->priority < highest_priority_process->priority) {
      highest_priority_process = current_process;
      highest_priority_node = current;
    }
    current = current->next;
  }

  // Remove the highest priority node
  if (highest_priority_node->prev) {
    highest_priority_node->prev->next = highest_priority_node->next;
  } else {
    queue->front = highest_priority_node->next;
  }

  if (highest_priority_node->next) {
    highest_priority_node->next->prev = highest_priority_node->prev;
  } else {
    queue->rear = highest_priority_node->prev;
  }

  free(highest_priority_node);
  queue->size--;
  return highest_priority_process;
}

// Function to get the size of the queue
int get_queue_size(queue_t *queue) { return queue->size; }

// Function to destroy the queue and free all its nodes
void destroy_queue(queue_t *queue) {
  while (queue->size > 0) {
    pop_queue(queue);
  }
  free(queue);
}

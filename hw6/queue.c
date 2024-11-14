#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

// Initialize the queue
void initialize_queue(queue_t *queue) {
  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;
}

// Add element to the end of the queue
void push_queue(queue_t *queue, void *element) {
  queue_node_t *newNode = (queue_node_t *)malloc(sizeof(queue_node_t));
  newNode->data = element;
  newNode->next = NULL;
  newNode->prev = queue->tail;

  if (queue->tail) {
    queue->tail->next = newNode;
  }
  queue->tail = newNode;

  if (queue->head == NULL) {
    queue->head = newNode;
  }

  queue->size++;
}

// Remove and return element from the front of the queue
void *pop_queue(queue_t *queue) {
  if (queue->size == 0) {
    return NULL;
  }

  queue_node_t *node = queue->head;
  void *data = node->data;

  queue->head = node->next;
  if (queue->head) {
    queue->head->prev = NULL;
  } else {
    queue->tail = NULL;
  }

  free(node);
  queue->size--;

  return data;
}

// Function to remove the highest priority process (specific to your previous
// assignment)
process_t *remove_process(queue_t *queue) {
  if (queue->size == 0) {
    return NULL;
  }

  queue_node_t *current = queue->head;
  queue_node_t *highestPriorityNode = current;
  while (current != NULL) {
    process_t *currentProcess = (process_t *)current->data;
    process_t *highestPriorityProcess = (process_t *)highestPriorityNode->data;

    if (currentProcess->priority < highestPriorityProcess->priority) {
      highestPriorityNode = current;
    }

    current = current->next;
  }

  // Remove the node with the highest priority process
  if (highestPriorityNode->prev) {
    highestPriorityNode->prev->next = highestPriorityNode->next;
  } else {
    queue->head = highestPriorityNode->next;
  }

  if (highestPriorityNode->next) {
    highestPriorityNode->next->prev = highestPriorityNode->prev;
  } else {
    queue->tail = highestPriorityNode->prev;
  }

  process_t *highestPriorityProcess = (process_t *)highestPriorityNode->data;
  free(highestPriorityNode);
  queue->size--;

  return highestPriorityProcess;
}

// Get the size of the queue
int get_queue_size(queue_t *queue) { return queue->size; }

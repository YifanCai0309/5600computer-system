#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to create a process
process_t *create_process(int id, const char *name, long run_time,
                          int priority) {
  process_t *process = (process_t *)malloc(sizeof(process_t));
  process->id = id;
  process->name = strdup(name);
  process->run_time = run_time;
  process->priority = priority;
  return process;
}

int main() {
  // Create a queue
  queue_t *queue = create_queue();

  // Add some processes to the queue
  process_t *p1 = create_process(1, "Process1", 100, 3);
  process_t *p2 = create_process(2, "Process2", 200, 2);
  process_t *p3 = create_process(3, "Process3", 300, 1);

  printf("Adding processes to the queue...\n");
  push_queue(queue, p1);
  push_queue(queue, p2);
  push_queue(queue, p3);
  printf("Queue size after adding: %d\n", get_queue_size(queue));

  // Pop a process from the queue
  process_t *front_process = (process_t *)pop_queue(queue);
  printf("Popped process: %s (ID: %d, Priority: %d)\n", front_process->name,
         front_process->id, front_process->priority);
  printf("Queue size after popping: %d\n", get_queue_size(queue));

  // Remove the highest priority process
  process_t *highest_priority_process = remove_process(queue);
  printf("Removed highest priority process: %s (ID: %d, Priority: %d)\n",
         highest_priority_process->name, highest_priority_process->id,
         highest_priority_process->priority);
  printf("Queue size after removing highest priority process: %d\n",
         get_queue_size(queue));

  // Clean up
  destroy_queue(queue);
  free(front_process->name);
  free(front_process);
  free(highest_priority_process->name);
  free(highest_priority_process);

  return 0;
}

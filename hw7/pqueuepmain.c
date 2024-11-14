#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 15     // Increased buffer size to 15
#define MAX_ITEMS 30    // Maximum number of items to produce and consume
#define NUM_PRODUCERS 2 // Number of producer threads
#define NUM_CONSUMERS 2 // Number of consumer threads

int buffer[BUF_SIZE]; // Shared buffer
int num = 0;          // Current number of items in buffer
pthread_mutex_t mut;  // Mutex for synchronizing access to buffer and counters
pthread_cond_t cond;  // Condition variable for signaling

// Counters to track total produced and consumed items
int total_produced = 0;
int total_consumed = 0;

// Function prototypes
void *producer(void *param);
void *consumer(void *param);

int main(int argc, char *argv[]) {
  pthread_t producers[NUM_PRODUCERS];
  pthread_t consumers[NUM_CONSUMERS];
  int i;

  // Initialize mutex
  if (pthread_mutex_init(&mut, NULL) != 0) {
    perror("pthread_mutex_init");
    exit(1);
  }

  // Initialize condition variable
  if (pthread_cond_init(&cond, NULL) != 0) {
    perror("pthread_cond_init");
    exit(1);
  }

  // Create producer threads
  for (i = 0; i < NUM_PRODUCERS; i++) {
    if (pthread_create(&producers[i], NULL, producer, NULL) != 0) {
      perror("pthread_create for producer");
      exit(1);
    }
  }

  // Create consumer threads
  for (i = 0; i < NUM_CONSUMERS; i++) {
    if (pthread_create(&consumers[i], NULL, consumer, NULL) != 0) {
      perror("pthread_create for consumer");
      exit(1);
    }
  }

  // Wait for all producer threads to finish
  for (i = 0; i < NUM_PRODUCERS; i++) {
    pthread_join(producers[i], NULL);
  }

  // Wait for all consumer threads to finish
  for (i = 0; i < NUM_CONSUMERS; i++) {
    pthread_join(consumers[i], NULL);
  }

  // Destroy mutex and condition variable
  pthread_mutex_destroy(&mut);
  pthread_cond_destroy(&cond);

  printf("WE ARE DONE\n"); // Final message after all threads have finished
  return 0;
}

void *producer(void *param) {
  int item = 0; // Item to be produced

  while (1) {
    pthread_mutex_lock(&mut); // Lock mutex before accessing shared variables

    // Check if production limit has been reached
    if (total_produced >= MAX_ITEMS) {
      pthread_mutex_unlock(&mut);
      printf("Producer thread %ld exiting after producing %d items.\n",
             pthread_self(), total_produced);
      pthread_exit(NULL); // Exit thread gracefully
    }

    // Wait while buffer is full and production limit not reached
    while (num == BUF_SIZE && total_produced < MAX_ITEMS) {
      pthread_cond_wait(&cond, &mut);
    }

    // Double-check if production limit has been reached after waking up
    if (total_produced >= MAX_ITEMS) {
      pthread_mutex_unlock(&mut);
      printf("Producer thread %ld exiting after producing %d items.\n",
             pthread_self(), total_produced);
      pthread_exit(NULL);
    }

    // Produce an item and add it to the buffer
    buffer[num] = item;
    num++;
    total_produced++;
    printf("Producer thread %ld produced item %d. Buffer size: %d\n",
           pthread_self(), item, num);
    item++;

    // Signal consumers that a new item is available
    pthread_cond_broadcast(&cond);

    pthread_mutex_unlock(&mut); // Unlock mutex
    sleep(1);                   // Simulate time taken to produce an item
  }
}

void *consumer(void *param) {
  int item;

  while (1) {
    pthread_mutex_lock(&mut); // Lock mutex before accessing shared variables

    // Check if consumption limit has been reached
    if (total_consumed >= MAX_ITEMS) {
      pthread_mutex_unlock(&mut);
      printf("Consumer thread %ld exiting after consuming %d items.\n",
             pthread_self(), total_consumed);
      pthread_exit(NULL); // Exit thread gracefully
    }

    // Wait while buffer is empty and consumption limit not reached
    while (num == 0 && total_consumed < MAX_ITEMS) {
      pthread_cond_wait(&cond, &mut);
    }

    // Double-check if consumption limit has been reached after waking up
    if (total_consumed >= MAX_ITEMS) {
      pthread_mutex_unlock(&mut);
      printf("Consumer thread %ld exiting after consuming %d items.\n",
             pthread_self(), total_consumed);
      pthread_exit(NULL);
    }

    // Consume an item from the buffer
    item = buffer[num - 1];
    num--;
    total_consumed++;
    printf("Consumer thread %ld consumed item %d. Buffer size: %d\n",
           pthread_self(), item, num);

    // Signal producers that space is available in the buffer
    pthread_cond_broadcast(&cond);

    pthread_mutex_unlock(&mut); // Unlock mutex
    sleep(3);                   // Simulate time taken to consume an item
  }
}

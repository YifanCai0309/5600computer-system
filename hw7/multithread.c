#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRAY_SIZE 10

// Global array to be shared by threads
int shared_array[ARRAY_SIZE];

// Worker function for thread X (adds data to shared array)
void *worker_x(void *data) {
  pthread_t tid = pthread_self(); // Get the current thread ID
  printf("Thread X (ID: %lu) is writing data...\n", tid);
  for (int i = 0; i < ARRAY_SIZE; i++) {
    usleep(50000);           // Sleep for 50 milliseconds
    shared_array[i] = i + 1; // Write data to the shared array
    printf("Thread X writes shared_array[%d] = %d\n", i, shared_array[i]);
  }
  printf("Thread X done writing! (ID: %lu)\n", tid);
  return NULL;
}

// Worker function for thread Y (reads data from shared array)
void *worker_y(void *data) {
  pthread_t tid = pthread_self(); // Get the current thread ID
  printf("Thread Y (ID: %lu) is reading data...\n", tid);
  for (int i = 0; i < ARRAY_SIZE; i++) {
    usleep(50000); // Sleep for 50 milliseconds
    printf("Thread Y reads shared_array[%d] = %d\n", i,
           shared_array[i]); // Read data
  }
  printf("Thread Y done reading! (ID: %lu)\n", tid);
  return NULL;
}

int main(void) {
  pthread_t th1, th2;

  // Create thread X (writer) and thread Y (reader)
  pthread_create(&th1, NULL, worker_x, NULL);
  pthread_create(&th2, NULL, worker_y, NULL);

  // Sleep for 1 second and cancel thread Y
  sleep(1);
  printf("====> Cancelling Thread Y!! (Thread ID: %lu)\n", th2);
  pthread_cancel(th2); // Cancel thread Y

  // Sleep for another 100 milliseconds and cancel thread X
  usleep(100000);
  printf("====> Cancelling Thread X!! (Thread ID: %lu)\n", th1);
  pthread_cancel(th1); // Cancel thread X

  // Wait for threads to complete before exiting
  pthread_join(th1, NULL);
  pthread_join(th2, NULL);

  printf("Exiting from main program\n");
  return 0;
}

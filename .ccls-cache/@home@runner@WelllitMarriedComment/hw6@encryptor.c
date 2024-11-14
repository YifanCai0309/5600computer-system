#include <fcntl.h> // For open()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_STRINGS 100
#define BUFFER_SIZE 256

// Function to spawn a child process and pass strings for encryption
void encrypt_strings(char strings[MAX_STRINGS][BUFFER_SIZE], int string_count) {
  static int file_counter = 0; // Static counter for unique file names
  char output_filename[BUFFER_SIZE];

  // Generate a unique output file name
  snprintf(output_filename, sizeof(output_filename), "cipher_output_%d.txt",
           file_counter++);

  int pipefd[2];
  pid_t pid;

  // Create a pipe
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  // Fork a child process
  pid = fork();

  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // ----- Child Process -----

    // Close the write end of the pipe in the child
    close(pipefd[1]);

    // Redirect stdin to the read end of the pipe
    if (dup2(pipefd[0], STDIN_FILENO) == -1) {
      perror("dup2 stdin");
      exit(EXIT_FAILURE);
    }
    close(pipefd[0]); // Close the original read end

    // Open the unique output file for writing
    int out_fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
      perror("open output file");
      exit(EXIT_FAILURE);
    }

    // Redirect stdout to the output file
    if (dup2(out_fd, STDOUT_FILENO) == -1) {
      perror("dup2 stdout");
      close(out_fd);
      exit(EXIT_FAILURE);
    }
    close(out_fd); // Close the original file descriptor

    // Execute the cipher program (e.g., polybius)
    execlp("./polybius", "./polybius", NULL);

    // If execlp fails
    perror("execlp");
    exit(EXIT_FAILURE);
  } else {
    // ----- Parent Process -----

    // Close the read end of the pipe in the parent
    close(pipefd[0]);

    // Write each string to the child process via the pipe
    for (int i = 0; i < string_count; i++) {
      size_t len = strlen(strings[i]);
      if (write(pipefd[1], strings[i], len) != (ssize_t)len) {
        perror("write string to pipe");
        close(pipefd[1]);
        exit(EXIT_FAILURE);
      }
      // Write a newline character after each string
      if (write(pipefd[1], "\n", 1) != 1) {
        perror("write newline to pipe");
        close(pipefd[1]);
        exit(EXIT_FAILURE);
      }
    }

    // Close the write end of the pipe after writing
    close(pipefd[1]);

    // Wait for the child process to finish
    if (wait(NULL) == -1) {
      perror("wait");
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char *argv[]) {
  // Check for correct usage
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Open the input file for reading
  FILE *input_file = fopen(argv[1], "r");
  if (input_file == NULL) {
    perror("Error opening input file");
    exit(EXIT_FAILURE);
  }

  char strings[MAX_STRINGS][BUFFER_SIZE];
  int string_count = 0;
  char line[BUFFER_SIZE];

  // Read lines from the input file
  while (fgets(line, sizeof(line), input_file)) {
    // Remove the newline character if present
    line[strcspn(line, "\n")] = '\0';

    // Safely copy the line into the strings array using snprintf
    snprintf(strings[string_count], BUFFER_SIZE, "%s", line);
    string_count++;

    // Once we have MAX_STRINGS, encrypt them
    if (string_count == MAX_STRINGS) {
      encrypt_strings(strings, string_count);
      string_count = 0; // Reset for the next batch
    }
  }

  // Encrypt any remaining strings
  if (string_count > 0) {
    encrypt_strings(strings, string_count);
  }

  // Close the input file
  fclose(input_file);

  return 0;
}

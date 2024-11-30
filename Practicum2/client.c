/*
 * client.c -- TCP Socket Client supporting WRITE, GET, RM commands
 *
 * Compilation:
 *   gcc -Wall -g client.c -o client
 *
 * Usage:
 *   ./client WRITE <local-file-path> <remote-file-path>
 *   ./client GET <remote-file-path> <local-file-path>
 *   ./client RM <remote-path>
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 8196

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s COMMAND [ARGS...]\n", argv[0]);
    printf("Commands:\n");
    printf("  WRITE <local-file-path> <remote-file-path>\n");
    printf("  GET <remote-file-path> <local-file-path>\n");
    printf("  RM <remote-path>\n");
    return -1;
  }

  char *command = argv[1];

  // Initialize variables for commands
  char local_file_path[BUFFER_SIZE];
  char remote_file_path[BUFFER_SIZE];
  FILE *fp = NULL;
  long file_size = 0;
  char *file_buffer = NULL;

  if (strcmp(command, "WRITE") == 0) {
    if (argc != 4) {
      printf("Usage: %s WRITE <local-file-path> <remote-file-path>\n", argv[0]);
      return -1;
    }
    strcpy(local_file_path, argv[2]);
    strcpy(remote_file_path, argv[3]);

    // Open local file
    fp = fopen(local_file_path, "rb");
    if (fp == NULL) {
      perror("Failed to open local file");
      return -1;
    }

    // Get file size
    if (fseek(fp, 0L, SEEK_END) != 0) {
      perror("Failed to seek to end of file");
      fclose(fp);
      return -1;
    }
    file_size = ftell(fp);
    if (file_size < 0) {
      perror("Failed to get file size");
      fclose(fp);
      return -1;
    }
    if (fseek(fp, 0L, SEEK_SET) != 0) {
      perror("Failed to seek to beginning of file");
      fclose(fp);
      return -1;
    }

    // Allocate buffer and read file
    file_buffer = malloc(file_size);
    if (file_buffer == NULL) {
      perror("Failed to allocate memory");
      fclose(fp);
      return -1;
    }

    size_t bytes_read = fread(file_buffer, 1, file_size, fp);
    if (bytes_read != file_size) {
      perror("Failed to read the complete file");
      free(file_buffer);
      fclose(fp);
      return -1;
    }
    fclose(fp);
  } else if (strcmp(command, "GET") == 0) {
    if (argc != 4) {
      printf("Usage: %s GET <remote-file-path> <local-file-path>\n", argv[0]);
      return -1;
    }
    strcpy(remote_file_path, argv[2]);
    strcpy(local_file_path, argv[3]);
  } else if (strcmp(command, "RM") == 0) {
    if (argc != 3) {
      printf("Usage: %s RM <remote-path>\n", argv[0]);
      return -1;
    }
    strcpy(remote_file_path, argv[2]);
  } else {
    printf("Unknown command: %s\n", command);
    return -1;
  }

  int socket_desc;
  struct sockaddr_in server_addr;
  char server_response[BUFFER_SIZE];

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0) {
    perror("Unable to create socket");
    if (file_buffer)
      free(file_buffer);
    return -1;
  }

  printf("Socket created successfully\n");

  // Set port and IP the same as server-side:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2000);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change as needed

  // Send connection request to server:
  if (connect(socket_desc, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) < 0) {
    perror("Unable to connect");
    close(socket_desc);
    if (file_buffer)
      free(file_buffer);
    return -1;
  }
  printf("Connected with server successfully\n");

  // Prepare and send the command
  if (strcmp(command, "WRITE") == 0) {
    char command_message[BUFFER_SIZE];
    // Ensure snprintf does not exceed buffer size
    if (snprintf(command_message, sizeof(command_message), "WRITE %s %ld\n",
                 remote_file_path, file_size) >= sizeof(command_message)) {
      printf("Command message too long.\n");
      close(socket_desc);
      free(file_buffer);
      return -1;
    }

    // Send the command message
    if (send(socket_desc, command_message, strlen(command_message), 0) < 0) {
      perror("Unable to send WRITE command");
      close(socket_desc);
      free(file_buffer);
      return -1;
    }
    printf("Sent WRITE command: %s", command_message);

    // Send the file data
    if (send(socket_desc, file_buffer, file_size, 0) < 0) {
      perror("Unable to send file data");
      close(socket_desc);
      free(file_buffer);
      return -1;
    }
    printf("Sent file data successfully.\n");
    free(file_buffer);
  } else if (strcmp(command, "GET") == 0) {
    char command_message[BUFFER_SIZE];
    // Ensure snprintf does not exceed buffer size
    if (snprintf(command_message, sizeof(command_message), "GET %s\n",
                 remote_file_path) >= sizeof(command_message)) {
      printf("Command message too long.\n");
      close(socket_desc);
      return -1;
    }

    // Send the GET command
    if (send(socket_desc, command_message, strlen(command_message), 0) < 0) {
      perror("Unable to send GET command");
      close(socket_desc);
      return -1;
    }
    printf("Sent GET command: %s", command_message);
  } else if (strcmp(command, "RM") == 0) {
    char command_message[BUFFER_SIZE];
    // Ensure snprintf does not exceed buffer size
    if (snprintf(command_message, sizeof(command_message), "RM %s\n",
                 remote_file_path) >= sizeof(command_message)) {
      printf("Command message too long.\n");
      close(socket_desc);
      return -1;
    }

    // Send the RM command
    if (send(socket_desc, command_message, strlen(command_message), 0) < 0) {
      perror("Unable to send RM command");
      close(socket_desc);
      return -1;
    }
    printf("Sent RM command: %s", command_message);
  }

  // Receive the server's response
  memset(server_response, '\0', sizeof(server_response));
  int recv_size =
      recv(socket_desc, server_response, sizeof(server_response) - 1, 0);
  if (recv_size < 0) {
    perror("Error while receiving server's response");
    close(socket_desc);
    return -1;
  }

  if (strcmp(command, "WRITE") == 0) {
    printf("Server's response: %s\n", server_response);
  } else if (strcmp(command, "GET") == 0) {
    if (strncmp(server_response, "FILE_SIZE", 9) == 0) {
      long recv_file_size;
      if (sscanf(server_response, "FILE_SIZE %ld", &recv_file_size) != 1) {
        printf("Invalid FILE_SIZE response from server.\n");
        close(socket_desc);
        return -1;
      }
      printf("Receiving file of size %ld bytes.\n", recv_file_size);

      // Open local file for writing
      fp = fopen(local_file_path, "wb");
      if (fp == NULL) {
        perror("Failed to open local file for writing");
        close(socket_desc);
        return -1;
      }

      long bytes_received = 0;
      char buffer[BUFFER_SIZE];
      while (bytes_received < recv_file_size) {
        int bytes = recv(socket_desc, buffer, sizeof(buffer), 0);
        if (bytes < 0) {
          perror("Failed to receive file data");
          fclose(fp);
          close(socket_desc);
          return -1;
        } else if (bytes == 0) {
          printf("Connection closed unexpectedly.\n");
          fclose(fp);
          close(socket_desc);
          return -1;
        }
        fwrite(buffer, sizeof(char), bytes, fp);
        bytes_received += bytes;
      }

      fclose(fp);
      printf("File received and saved to %s successfully.\n", local_file_path);
    } else {
      printf("Server's response: %s\n", server_response);
    }
  } else if (strcmp(command, "RM") == 0) {
    printf("Server's response: %s\n", server_response);
  }

  // Close the socket:
  close(socket_desc);

  return 0;
}

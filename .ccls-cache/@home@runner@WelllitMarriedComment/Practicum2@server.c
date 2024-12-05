/*
 * server.c -- TCP Socket Server supporting WRITE, GET, RM commands
 *
 * Compilation:
 *   gcc -Wall -g server.c -o server
 *
 * Usage:
 *   ./server
 */

#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 8196
#define ROOT_DIR "server_root"

// Function to handle WRITE command
int handle_write(int client_sock, char *remote_path, long file_size) {
  char full_path[BUFFER_SIZE];
  // Prevent buffer overflow by ensuring full_path is null-terminated
  if (snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_DIR, remote_path) >=
      sizeof(full_path)) {
    char *error_msg = "WRITE_FAIL Path too long.";
    send(client_sock, error_msg, strlen(error_msg), 0);
    return -1;
  }

  // Prevent directory traversal
  if (strstr(remote_path, "..") != NULL) {
    char *error_msg = "WRITE_FAIL Invalid path.";
    send(client_sock, error_msg, strlen(error_msg), 0);
    return -1;
  }

  // Create directories if they don't exist
  char *dir_end = strrchr(full_path, '/');
  if (dir_end != NULL) {
    *dir_end = '\0';
    // Use mkdir with appropriate permissions
    if (mkdir(full_path, 0700) != 0 && errno != EEXIST) {
      perror("Failed to create directories");
      char *error_msg = "WRITE_FAIL Unable to create directories.";
      send(client_sock, error_msg, strlen(error_msg), 0);
      *dir_end = '/';
      return -1;
    }
    *dir_end = '/';
  }

  FILE *fp = fopen(full_path, "wb");
  if (fp == NULL) {
    perror("Failed to open file for writing");
    char *error_msg = "WRITE_FAIL Unable to create file.";
    send(client_sock, error_msg, strlen(error_msg), 0);
    return -1;
  }

  long bytes_received = 0;
  char buffer[BUFFER_SIZE];
  while (bytes_received < file_size) {
    int bytes = recv(client_sock, buffer, sizeof(buffer), 0);
    if (bytes <= 0) {
      perror("Failed to receive file data");
      fclose(fp);
      char *error_msg = "WRITE_FAIL Error receiving data.";
      send(client_sock, error_msg, strlen(error_msg), 0);
      return -1;
    }
    fwrite(buffer, sizeof(char), bytes, fp);
    bytes_received += bytes;
  }

  fclose(fp);
  printf("File written to %s successfully.\n", full_path);
  char *success_msg = "WRITE_SUCCESS";
  if (send(client_sock, success_msg, strlen(success_msg), 0) < 0) {
    perror("Failed to send WRITE_SUCCESS");
    return -1;
  }
  return 0;
}

// Function to handle GET command
int handle_get(int client_sock, char *remote_path) {
  char full_path[BUFFER_SIZE];
  // Prevent buffer overflow by ensuring full_path is null-terminated
  if (snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_DIR, remote_path) >=
      sizeof(full_path)) {
    char *error_msg = "GET_FAIL Path too long.";
    send(client_sock, error_msg, strlen(error_msg), 0);
    return -1;
  }

  // Prevent directory traversal
  if (strstr(remote_path, "..") != NULL) {
    char *error_msg = "GET_FAIL Invalid path.";
    send(client_sock, error_msg, strlen(error_msg), 0);
    return -1;
  }

  FILE *fp = fopen(full_path, "rb");
  if (fp == NULL) {
    perror("Failed to open file for reading");
    char *error_msg = "GET_FAIL File not found.";
    send(client_sock, error_msg, strlen(error_msg), 0);
    return -1;
  }

  // Get file size
  fseek(fp, 0L, SEEK_END);
  long file_size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  // Send FILE_SIZE message
  char size_message[BUFFER_SIZE];
  // Ensure that snprintf does not exceed buffer size
  if (snprintf(size_message, sizeof(size_message), "FILE_SIZE %ld\n",
               file_size) >= sizeof(size_message)) {
    char *error_msg = "GET_FAIL File size too large.";
    send(client_sock, error_msg, strlen(error_msg), 0);
    fclose(fp);
    return -1;
  }
  if (send(client_sock, size_message, strlen(size_message), 0) < 0) {
    perror("Failed to send file size");
    fclose(fp);
    return -1;
  }

  // Send the file data
  char buffer[BUFFER_SIZE];
  size_t bytes_read;
  while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
    if (send(client_sock, buffer, bytes_read, 0) < 0) {
      perror("Failed to send file data");
      fclose(fp);
      return -1;
    }
  }

  fclose(fp);
  printf("File %s sent successfully.\n", full_path);
  return 0;
}

// Function to remove files or directories recursively
int remove_path(const char *path) {
  struct stat st;
  if (stat(path, &st) != 0) {
    return -1;
  }

  if (S_ISDIR(st.st_mode)) {
    // Directory: remove contents recursively
    DIR *dir = opendir(path);
    if (dir == NULL) {
      return -1;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      }
      char full_path[BUFFER_SIZE];
      if (snprintf(full_path, sizeof(full_path), "%s/%s", path,
                   entry->d_name) >= sizeof(full_path)) {
        closedir(dir);
        return -1;
      }
      if (remove_path(full_path) != 0) {
        closedir(dir);
        return -1;
      }
    }
    closedir(dir);
    // Remove the directory itself
    return rmdir(path);
  } else {
    // File: remove it
    return unlink(path);
  }
}

// Function to handle RM command
int handle_rm(int client_sock, char *remote_path) {
  char full_path[BUFFER_SIZE];
  // Prevent buffer overflow by ensuring full_path is null-terminated
  if (snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_DIR, remote_path) >=
      sizeof(full_path)) {
    char *error_msg = "RM_FAIL Path too long.";
    send(client_sock, error_msg, strlen(error_msg), 0);
    return -1;
  }

  // Prevent directory traversal
  if (strstr(remote_path, "..") != NULL) {
    char *error_msg = "RM_FAIL Invalid path.";
    send(client_sock, error_msg, strlen(error_msg), 0);
    return -1;
  }

  if (remove_path(full_path) == 0) {
    char *success_msg = "RM_SUCCESS";
    if (send(client_sock, success_msg, strlen(success_msg), 0) < 0) {
      perror("Failed to send RM_SUCCESS");
      return -1;
    }
    printf("Removed %s successfully.\n", full_path);
    return 0;
  } else {
    char error_msg[BUFFER_SIZE];
    if (snprintf(error_msg, sizeof(error_msg), "RM_FAIL %s", strerror(errno)) >=
        sizeof(error_msg)) {
      // Truncate error message if too long
      strncpy(error_msg, "RM_FAIL Unknown error.", sizeof(error_msg) - 1);
      error_msg[sizeof(error_msg) - 1] = '\0';
    }
    send(client_sock, error_msg, strlen(error_msg), 0);
    perror("Failed to remove path");
    return -1;
  }
}

int main(void) {
  int socket_desc, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char client_message[BUFFER_SIZE];

  // Create root directory if it doesn't exist
  struct stat st = {0};
  if (stat(ROOT_DIR, &st) == -1) {
    if (mkdir(ROOT_DIR, 0700) != 0) {
      perror("Failed to create root directory");
      return -1;
    }
  }

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0) {
    perror("Error while creating socket");
    return -1;
  }
  printf("Socket created successfully\n");

  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2000);
  server_addr.sin_addr.s_addr =
      inet_addr("0.0.0.0"); // Listen on all interfaces

  // Bind to the set port and IP:
  if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("Couldn't bind to the port");
    close(socket_desc);
    return -1;
  }
  printf("Done with binding\n");

  // Listen for clients:
  if (listen(socket_desc, 5) < 0) {
    perror("Error while listening");
    close(socket_desc);
    return -1;
  }
  printf("\nListening for incoming connections.....\n");

  // Server loop to accept multiple connections
  while (1) {
    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    client_sock =
        accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

    if (client_sock < 0) {
      perror("Can't accept");
      continue; // Continue to accept next connection
    }
    printf("Client connected from IP: %s and port: %i\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Receive client's message (command)
    memset(client_message, '\0', sizeof(client_message));
    int read_size =
        recv(client_sock, client_message, sizeof(client_message) - 1, 0);
    if (read_size < 0) {
      perror("Couldn't receive");
      close(client_sock);
      continue;
    }

    // Ensure null-terminated string
    client_message[read_size] = '\0';
    printf("Received message: %s\n", client_message);

    // Parse the command
    char command[10], remote_path[BUFFER_SIZE];
    long file_size = 0;
    sscanf(client_message, "%s %s %ld", command, remote_path, &file_size);

    if (strcmp(command, "WRITE") == 0) {
      printf("Handling WRITE command for %s with size %ld bytes\n", remote_path,
             file_size);
      handle_write(client_sock, remote_path, file_size);
    } else if (strcmp(command, "GET") == 0) {
      printf("Handling GET command for %s\n", remote_path);
      handle_get(client_sock, remote_path);
    } else if (strcmp(command, "RM") == 0) {
      printf("Handling RM command for %s\n", remote_path);
      handle_rm(client_sock, remote_path);
    } else {
      char *unknown_cmd = "UNKNOWN_COMMAND";
      if (send(client_sock, unknown_cmd, strlen(unknown_cmd), 0) < 0) {
        perror("Failed to send UNKNOWN_COMMAND");
      }
      printf("Unknown command received.\n");
    }

    // Closing the client socket:
    close(client_sock);
    printf("Client disconnected.\n\n");
  }

  // Closing the socket (unreachable code in this loop):
  close(socket_desc);

  return 0;
}

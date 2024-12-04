#include <pthread.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define BUFFER_SIZE 8196
#define ROOT_DIR "server_root"

int socket_desc; // Server socket descriptor
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for file operations

// Signal handler to gracefully shut down the server
void signal_handler(int signum) {
    printf("\nReceived signal %d, shutting down server...\n", signum);
    close(socket_desc);
    exit(0);
}

// Function for XOR encryption and decryption
void xor_encrypt_decrypt(char *data, size_t size, char key) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key; // XOR each byte with the key
    }
}

// Function to handle WRITE command
int handle_write(int client_sock, char *remote_path, long file_size) {
    char full_path[BUFFER_SIZE];
    char encryption_key = 0xAA; // XOR encryption key

    snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_DIR, remote_path);

    // Prevent directory traversal
    if (strstr(remote_path, "..") != NULL) {
        char *error_msg = "WRITE_FAIL Invalid path.";
        send(client_sock, error_msg, strlen(error_msg), 0);
        return -1;
    }

    // Create directories if they do not exist
    char *dir_end = strrchr(full_path, '/');
    if (dir_end != NULL) {
        *dir_end = '\0';
        mkdir(full_path, 0700);
        *dir_end = '/';
    }

    // Open file for writing
    FILE *fp = fopen(full_path, "wb");
    if (fp == NULL) {
        perror("Failed to open file for writing");
        char *error_msg = "WRITE_FAIL Unable to create file.";
        send(client_sock, error_msg, strlen(error_msg), 0);
        return -1;
    }

    pthread_mutex_lock(&file_mutex); // Lock for thread safety

    long bytes_received = 0;
    char buffer[BUFFER_SIZE];

    while (bytes_received < file_size) {
      int bytes = recv(client_sock, buffer, sizeof(buffer), 0);
      if (bytes < 0) {
          perror("Failed to receive file data");
          fclose(fp);
          char *error_msg = "WRITE_FAIL Error receiving data.";
          send(client_sock, error_msg, strlen(error_msg), 0);
          pthread_mutex_unlock(&file_mutex); //unlock
          return -1;
      } else if (bytes == 0) {
          break;
      }

        // Encrypt the data before writing
        xor_encrypt_decrypt(buffer, bytes, encryption_key);
      if (fwrite(buffer, sizeof(char), bytes, fp) != bytes) {
          perror("Failed to write encrypted data to file");
          fclose(fp);
          char *error_msg = "WRITE_FAIL Error writing to file.";
          send(client_sock, error_msg, strlen(error_msg), 0);
          pthread_mutex_unlock(&file_mutex); // unlock
          return -1;
      }
        bytes_received += bytes;
    }

    pthread_mutex_unlock(&file_mutex); // Unlock
    fclose(fp);

    char *success_msg = "WRITE_SUCCESS";
    send(client_sock, success_msg, strlen(success_msg), 0);
    return 0;
}


// Function to handle GET command
int handle_get(int client_sock, char *remote_path) {
    char full_path[BUFFER_SIZE];
    char encryption_key = 0xAA; // XOR decryption key

    snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_DIR, remote_path);

    // Prevent directory traversal
    if (strstr(remote_path, "..") != NULL) {
        char *error_msg = "GET_FAIL Invalid path.";
        send(client_sock, error_msg, strlen(error_msg), 0);
        return -1;
    }

    // Open file for reading
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
    snprintf(size_message, sizeof(size_message), "FILE_SIZE %ld\n", file_size);
    send(client_sock, size_message, strlen(size_message), 0);

    // Read and decrypt the file data before sending
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        xor_encrypt_decrypt(buffer, bytes_read, encryption_key);
        send(client_sock, buffer, bytes_read, 0);
    }

    fclose(fp);
    return 0;
}

// Function to recursively remove files and directories
int remove_path(const char *path) {
    struct stat st;

    if (stat(path, &st) != 0) {
        return -1;
    }

    if (S_ISDIR(st.st_mode)) {
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
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            remove_path(full_path);
        }

        closedir(dir);
        return rmdir(path); // Remove directory itself
    } else {
        return unlink(path); // Remove file
    }
}

// Function to handle RM command
int handle_rm(int client_sock, char *remote_path) {
    char full_path[BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_DIR, remote_path);

    // Prevent directory traversal
    if (strstr(remote_path, "..") != NULL) {
        char *error_msg = "RM_FAIL Invalid path.";
        send(client_sock, error_msg, strlen(error_msg), 0);
        return -1;
    }

    if (remove_path(full_path) == 0) {
        char *success_msg = "RM_SUCCESS";
        send(client_sock, success_msg, strlen(success_msg), 0);
        return 0;
    } else {
        char *error_msg = "RM_FAIL Unable to remove.";
        send(client_sock, error_msg, strlen(error_msg), 0);
        return -1;
    }
}

// Function to list all files in a directory (non-recursive)
int handle_list(int client_sock, char *remote_path) {
    char full_path[BUFFER_SIZE];
  if (remote_path == NULL || strcmp(remote_path, "") == 0 || strcmp(remote_path, "/") == 0) {
    snprintf(full_path, sizeof(full_path), "%s", ROOT_DIR);  // Direct ROOT_DIR
  } else {
    snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_DIR, remote_path);
  }

    // Prevent directory traversal
    if (strstr(remote_path, "..") != NULL) {
        char *error_msg = "LIST_FAIL Invalid path.";
        send(client_sock, error_msg, strlen(error_msg), 0);
        return -1;
    }

    // Open the directory
    DIR *dir = opendir(full_path);
    if (dir == NULL) {
        perror("Failed to open directory");
        char *error_msg = "LIST_FAIL Directory not found.";
        send(client_sock, error_msg, strlen(error_msg), 0);
        return -1;
    }

    struct dirent *entry;
    char file_list[BUFFER_SIZE];
    file_list[0] = '\0';  // Initialize to empty string

    // Read the directory entries and build the file list
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            strcat(file_list, entry->d_name);
            strcat(file_list, "\n");
        }
    }

    closedir(dir);

    // Send the file list to the client
    if (strlen(file_list) > 0) {
        send(client_sock, file_list, strlen(file_list), 0);
    } else {
        char *empty_msg = "No files found.\n";
        send(client_sock, empty_msg, strlen(empty_msg), 0);
    }

    return 0;
}

// Client handler function
void *client_handler(void *sock) {
    int client_sock = *(int *)sock;
    char client_message[BUFFER_SIZE];

    int read_size = recv(client_sock, client_message, sizeof(client_message) - 1, 0);
    if (read_size < 0) {
        perror("Couldn't receive");
        close(client_sock);
        return NULL;
    }

    client_message[read_size] = '\0';
    printf("Received message: %s\n", client_message);

    char command[10], remote_path[BUFFER_SIZE];
    long file_size = 0;

    sscanf(client_message, "%s %s %ld", command, remote_path, &file_size);

    if (strcmp(command, "WRITE") == 0) {
        handle_write(client_sock, remote_path, file_size);
    } else if (strcmp(command, "GET") == 0) {
        handle_get(client_sock, remote_path);
    } else if (strcmp(command, "RM") == 0) {
        handle_rm(client_sock, remote_path);
    } else if (strcmp(command, "LIST") == 0) {
        handle_list(client_sock, remote_path); 
    }
    else {
        char *unknown_cmd = "UNKNOWN_COMMAND";
        send(client_sock, unknown_cmd, strlen(unknown_cmd), 0);
    }

    close(client_sock);
    printf("Client disconnected.\n");
    return NULL;
}

int main(void) {
  int client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  // Register signal handler for SIGINT (Ctrl+C)
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  // Create root directory if it doesn't exist
  mkdir(ROOT_DIR, 0700);
  // Create socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc < 0) {
  perror("Error while creating socket");
  return -1;
  }
  // Set port and IP
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2000);
  server_addr.sin_addr.s_addr = inet_addr("0.0.0.0"); // Listen on all interfaces
  // Bind to the set port and IP
  if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
  perror("Couldn't bind to the port");
  close(socket_desc);
  return -1;
  }
  // Listen for clients
  if (listen(socket_desc, 5) < 0) {
  perror("Error while listening");
  close(socket_desc);
  return -1;
  }
  printf("\nListening for incoming connections.....\n");
  
  // Server loop to accept multiple connections
  while (1) {
  client_size = sizeof(client_addr);
  client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);
  if (client_sock < 0) {
  perror("Can't accept");
  continue; // Continue to accept next connection
  }
  printf("Client connected from IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
  // Create a new thread for each client
  pthread_t client_thread;
  if (pthread_create(&client_thread, NULL, client_handler, (void *)&client_sock) != 0) {
  perror("Failed to create thread");
  }
  pthread_detach(client_thread); // Detach the thread
  }
  close(socket_desc);
  return 0;

}
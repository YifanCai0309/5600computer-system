# Define variables
CC = gcc
CFLAGS = -Wall -g
SERVER_SRC = server.c
CLIENT_SRC = client.c
SERVER_TARGET = server
CLIENT_TARGET = client

# Default target
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Compile server
$(SERVER_TARGET): $(SERVER_SRC)
	$(CC) $(CFLAGS) $< -o $@

# Compile client
$(CLIENT_TARGET): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $< -o $@

# clean target to remove generated files
.PHONY: clean
clean:
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET)


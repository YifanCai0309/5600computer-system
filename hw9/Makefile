# Makefile for Hierarchical Memory Simulation Project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

# Directories
INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj

# Target executable
TARGET = memory_sim

# Source files
SRCS = $(SRC_DIR)/message.c \
			 $(SRC_DIR)/cache.c \
			 $(SRC_DIR)/main.c

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Default rule
all: $(TARGET)

# Create object directory if it doesn't exist
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Clean up generated files
clean:
	rm -rf $(OBJ_DIR) $(TARGET) message_store.dat message_index.dat

.PHONY: all clean

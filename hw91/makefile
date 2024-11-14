# Makefile for compiling message store program

CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Target executable
TARGET = message_store

# Source files
SRCS = message_store.c test_message_store.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.c message_store.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJS) $(TARGET) message_store.dat message_index.dat

.PHONY: all clean


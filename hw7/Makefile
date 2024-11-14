# Makefile for compiling pqueuepmain.c and multithread.c

# Compiler to use
CC = gcc

# Compiler flags:
# -Wall       : Enable all compiler's warning messages
# -pthread    : Link with the pthread library for multithreading
# -g          : Include debugging information
CFLAGS = -Wall -pthread -g

# Executable names
TARGETS = pqueuepmain multithread

# Default target to build all executables
all: $(TARGETS)

# Target to build pqueuepmain executable
pqueuepmain: pqueuepmain.c
	$(CC) $(CFLAGS) -o pqueuepmain pqueuepmain.c

# Target to build multithread executable
multithread: multithread.c
	$(CC) $(CFLAGS) -o multithread multithread.c

# Clean target to remove compiled binaries
clean:
	rm -f $(TARGETS)

# Phony targets to prevent conflicts with files named 'all' or 'clean'
.PHONY: all clean

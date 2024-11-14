CC = gcc
CFLAGS = -Wall

# Default target to build both the cipher and genrand executables
all: cipher genrand

# Rule to build the cipher executable
cipher: cipher.o polybius.o
	$(CC) $(CFLAGS) -o cipher cipher.o polybius.o

# Rule to build the genrand executable from genrand.c
genrand: genrand.o
	$(CC) $(CFLAGS) -o genrand genrand.o

# Rule to compile cipher.o from cipher.c
cipher.o: cipher.c polybius.h
	$(CC) $(CFLAGS) -c cipher.c

# Rule to compile polybius.o from polybius.c
polybius.o: polybius.c polybius.h
	$(CC) $(CFLAGS) -c polybius.c

# Rule to compile genrand.o from genrand.c
genrand.o: genrand.c
	$(CC) $(CFLAGS) -c genrand.c

# Clean target to remove compiled files
clean:
	rm -f cipher.o polybius.o genrand.o cipher genrand



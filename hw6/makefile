CC = gcc
CFLAGS = -Wall -g  # Enable warnings and debugging symbols

# Executables
all: encryptor polybius randString storeWord

# Compile encryptor (which uses polybius executable)
encryptor: encryptor.o
	$(CC) $(CFLAGS) -o encryptor encryptor.o

# Compile polybius (the executable)
polybius: polybius.o
	$(CC) $(CFLAGS) -o polybius polybius.o

# Compile randString
randString: randString.o
	$(CC) $(CFLAGS) -o randString randString.o

# Compile storeWord (depends on queue)
storeWord: storeWord.o queue.o
	$(CC) $(CFLAGS) -o storeWord storeWord.o queue.o

# Object files
encryptor.o: encryptor.c
	$(CC) $(CFLAGS) -c encryptor.c

polybius.o: polybius.c polybius.h
	$(CC) $(CFLAGS) -c polybius.c

randString.o: randString.c
	$(CC) $(CFLAGS) -c randString.c

storeWord.o: storeWord.c queue.h
	$(CC) $(CFLAGS) -c storeWord.c

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

# Clean up object files and executables
clean:
	rm -f *.o encryptor polybius randString storeWord

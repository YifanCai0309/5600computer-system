CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJ = allocmem.o loadmem.o

all: allocmem loadmem

allocmem: allocmem.o
	$(CC) $(CFLAGS) -o allocmem allocmem.o

loadmem: loadmem.o
	$(CC) $(CFLAGS) -o loadmem loadmem.o

allocmem.o: allocmem.c
	$(CC) $(CFLAGS) -c allocmem.c

loadmem.o: loadmem.c
	$(CC) $(CFLAGS) -c loadmem.c

clean:
	rm -f *.o allocmem loadmem

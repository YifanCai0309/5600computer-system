CC = gcc
CFLAGS = -Wall -g
TARGET = queuetest
OBJS = queue.o queuetest.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

queuetest.o: queuetest.c queue.h
	$(CC) $(CFLAGS) -c queuetest.c

clean:
	rm -f $(OBJS) $(TARGET)

# Variable definitions
CC = gcc
CFLAGS = -Wall

# Object files and executable file definitions
LEARNC_OBJ = learnc.o mpg2km.o
LEARNC_EXEC = learnc
ARGER_EXEC = arger

# Default target
all: $(LEARNC_EXEC) $(ARGER_EXEC)

# Generate the learnc executable
$(LEARNC_EXEC): $(LEARNC_OBJ)
	$(CC) $(CFLAGS) -o $(LEARNC_EXEC) $(LEARNC_OBJ)

# Generate the arger executable
$(ARGER_EXEC): arger.o
	$(CC) $(CFLAGS) -o $(ARGER_EXEC) arger.o

# Generate the learnc.o object file
learnc.o: learnc.c mpg2km.h
	$(CC) $(CFLAGS) -c learnc.c

# Generate the mpg2km.o object file
mpg2km.o: mpg2km.c mpg2km.h
	$(CC) $(CFLAGS) -c mpg2km.c

# Generate the arger.o object file
arger.o: arger.c
	$(CC) $(CFLAGS) -c arger.c

# Clean up object files and executables
clean:
	rm -f *.o $(LEARNC_EXEC) $(ARGER_EXEC)



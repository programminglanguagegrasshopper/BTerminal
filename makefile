# Variables for compiler and flags
CC = gcc
CFLAGS = -Wall
LDFLAGS = -e main

# Default target
all: btrm

# Rule to compile the project
btrm: btrm.o
	$(CC) $(CFLAGS) -o btrm btrm.o $(LDFLAGS)

# Rule to compile the object file
btrm.o: btrm.c
	$(CC) $(CFLAGS) -c btrm.c

# Clean rule to remove compiled files
clean:
	rm -f *.o btrm

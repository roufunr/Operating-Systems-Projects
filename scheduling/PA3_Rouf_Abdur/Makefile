# Makefile for compiling and running a C program

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g
LINKER = -pthread

# Source file and executable name
SOURCE = sched.c
EXECUTABLE = sched.out

# Default target: build and run the program
all: build

# Build the program
build: $(SOURCE)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCE) $(LINKER)

# Run the program
# run: $(EXECUTABLE)
# 	./$(EXECUTABLE)

# Clean up generated files
clean:
	rm -f $(EXECUTABLE)

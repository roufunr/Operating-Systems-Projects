# Makefile for compiling and running a C program

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g

# Source file and executable name
SOURCE = mymalloc.c
EXECUTABLE = mymalloc

# Default target: build and run the program
all: build

# Build the program
build: $(SOURCE)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCE)

# Run the program
# run: $(EXECUTABLE)
# 	./$(EXECUTABLE)

# Clean up generated files
clean:
	rm -f $(EXECUTABLE)

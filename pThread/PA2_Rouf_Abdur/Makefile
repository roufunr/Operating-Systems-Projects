# Makefile for compiling and running a C program

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g
LINKER = -lpthread

# Source file and executable name
SOURCE = threaded_sum.c
EXECUTABLE = threaded_sum

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

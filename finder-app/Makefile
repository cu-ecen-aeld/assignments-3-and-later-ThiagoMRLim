# Variables
CC := $(CROSS_COMPILE)gcc
CFLAGS := -Wall -Wextra -g
TARGET := writer
SRC := writer.c
OBJ := $(SRC:.c=.o)

# Default target: Build the application
.PHONY: all
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target: Remove built files
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ)

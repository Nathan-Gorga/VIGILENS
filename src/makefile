# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -O2

# Target executable
TARGET = main

# Source files
SRCS = main.c THREADFUNCTIONS/DATAINTAKE/dataintake.c
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(TARGET)
	@make clean
	@./$(TARGET)

# Linking the final binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compiling .c to .o
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS)

# Convenience
.PHONY: all clean

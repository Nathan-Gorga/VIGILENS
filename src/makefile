# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra

# Target executable
TARGET = VIGILENCE

# Source files
SRCS = main.c \
	RINGBUFFER/ringbuffer.c \
	EVENTDATASTRUCTURES/eventdatastructure.c \
	THREADFUNCTIONS/DATAINTAKE/dataintake.c
	
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(TARGET)
	@make clean
	@./$(TARGET)

# Linking the final binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -lpthread -o $@ $^

# Compiling .c to .o
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS)

# Convenience
.PHONY: all clean

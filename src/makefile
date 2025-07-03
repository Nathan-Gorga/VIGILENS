# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -D_POSIX_C_SOURCE=200809L -pthread -pedantic

# Target executable
TARGET = VIGILENCE

# Source files
SRCS = main.c \
	RINGBUFFER/ringbuffer.c \
	EVENTDATASTRUCTURES/eventdatastructure.c \
	THREADFUNCTIONS/DATAPROCESSING/dataprocessing.c \
	THREADFUNCTIONS/DATAINTAKE/dataintake.c
	
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(TARGET)
	@make clean
	@./$(TARGET)

# Linking the final binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=200809L -o $@ $^

# Compiling .c to .o
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS)

# Convenience
.PHONY: all clean

# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -pedantic -D_POSIX_C_SOURCE=200809L -O2
LDFLAGS = -pthread -lm
PROF_FLAGS = #-pg


# Target executable
TARGET = VIGILENCE

# Source files
SRCS = main.c \
	RINGBUFFER/ringbuffer.c \
	EVENTDATASTRUCTURES/eventdatastructure.c \
	THREADFUNCTIONS/DATAPROCESSING/dataprocessing.c \
	THREADFUNCTIONS/DATAINTAKE/dataintake.c \
	ALGOS/algos.c \
	LOG/log.c \
	UART/uart.c \
	MOCKUART/mockuart.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(TARGET)
	@make clean
	@./$(TARGET)
	@gprof ./$(TARGET) gmon.out > analysis.txt

# Link the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(PROF_FLAGS)

# Compile each .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean object files and the target
clean:
	rm -rf $(OBJS)

.PHONY: all clean

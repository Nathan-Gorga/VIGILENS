 # Compiler and flags
CC = clang
CFLAGS = -g -Wall -Wextra -pedantic #-D_POSIX_C_SOURCE=200809L 
LDFLAGS = -pthread -lm -lgsl -lgpiod -lcsv -lcjson
PROF_FLAGS = #-pg

# Target executable
TARGET = VIGILENCE

# Source files
SRCS = main.c \
	RINGBUFFER/ringbuffer.c \
	EVENTDATASTRUCTURES/eventdatastructure.c \
	THREADFUNCTIONS/PROCESSING/dataprocessing.c \
	THREADFUNCTIONS/INTAKE/dataintake.c \
	LOG/log.c \
	UART/uart.c \
	MOCKUART/mockuart.c \
	ALGOS/THRESHOLD/threshold_algos.c \
	ALGOS/FILTER/filter.c \
	LED/led.c \
	ALGOS/RANDOM_FOREST/build_forest.c \
	ALGOS/RANDOM_FOREST/csv_stuff.c \
	ALGOS/RANDOM_FOREST/event_feature.c \
	ALGOS/RANDOM_FOREST/predict_forest.c \
	ALGOS/RANDOM_FOREST/test_forest.c 
# 	ALGOS/RANDOM_FOREST/random_forest.c \

# Object files
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(TARGET)
	@make clean
	@sudo ./$(TARGET)

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

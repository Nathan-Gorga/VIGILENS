// THE LOGGING SYSTEM IS DESIGNED TO HANDLE ROUGH CRASHES AND INTERRUPTS
#ifndef LOG_H
#define LOG_H

#include "../globaldefinition.h"

// toggle to enable/disable logging
// #define LOG_ENABLED

typedef struct {
    THREAD_ID thread_id;
    LOG_TYPE log_type;
    char * message;
}LOG_PARAM;

int initLoggingSystem(void);

int logEntry(const THREAD_ID thread_id, const LOG_TYPE log_type, char * message);

int closeLoggingSystem(void);

static FILE * log_file;

static struct timespec begin;

extern pthread_mutex_t log_mutex;

#endif




#ifndef LOG_H
#define LOG_H

#include "../globaldefinition.h"

typedef enum{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
}LOG_TYPE;

typedef enum{
    NONE,
    THREAD_MASTER,
    THREAD_DATA_INTAKE,
    THREAD_DATA_PROCESSING
}THREAD_ID;


#define LOG_FILE_NAME "vigilence.log"

int initLoggingSystem(void);

int log(const THREAD_ID thread_id, const LOG_TYPE log_type, char * message);

int _log(const THREAD_ID thread_id, const LOG_TYPE log_type, char * message);

int closeLoggingSystem(void);


static FILE * log_file;

static struct timespec begin;

extern pthread_mutex_t log_mutex;

#endif




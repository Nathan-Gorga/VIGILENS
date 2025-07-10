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


//TODO : creating a logging system

//TODO : init logging file
int initLoggingSystem(void);

//TODO : write to logging file
int logEntry(const THREAD_ID thread_id, const LOG_TYPE log_type, char * message);


//TODO : close logging file
int closeLoggingSystem(void);


//TODO : mutex around logging


static FILE * log_file;

static time_t begin;


#endif




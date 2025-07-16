#ifndef LOG_H
#define LOG_H

#include "../globaldefinition.h"

// #define LOG_ENABLED//comment out this line to turn off logging


typedef enum{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
}LOG_TYPE;

typedef enum{
    THREAD_MASTER,
    THREAD_DATA_INTAKE,
    THREAD_DATA_PROCESSING,
    NONE,
}THREAD_ID;

typedef struct {
    THREAD_ID thread_id;
    LOG_TYPE log_type;
    char * message;
}LOG_PARAM;

#define LOG_FILE_NAME "vigilence.log"



/**
 * @brief Initializes the logging system by opening a log file and recording the start time.
 *
 * @details Attempts to open the log file with write access. If successful, records the 
 *          current time as the logging system's start time. Logs a message indicating 
 *          system initialization. If the log file cannot be opened, an error is printed 
 *          and the function returns an error code.
 *
 * @return 0 on successful initialization, -1 if the log file cannot be opened.
 */int initLoggingSystem(void);



/**
 * @brief Logs a message with a specified thread and log type.
 *
 * @param thread_id The identifier of the thread generating the log.
 * @param log_type The type of log message (e.g., debug, info, warning, error).
 * @param message The message to be logged.
 *
 * @return 0 on success, -1 on failure (e.g., memory allocation failure or thread creation error).
 *
 * @details This function creates a log entry in a separate thread to ensure
 *          non-blocking logging. The log entry includes the thread ID, log type,
 *          and the message. The log parameters are allocated on the heap to
 *          extend their lifetime beyond the function scope. The function
 *          returns immediately after creating the log thread, which handles
 *          the actual logging operation.
 */int logEntry(const THREAD_ID thread_id, const LOG_TYPE log_type, char * message);

static void * _logEntry(void * param);

static int __logEntry(const THREAD_ID thread_id, const LOG_TYPE log_type, char * message);



/**
 * @brief Closes the logging system.
 *
 * @details Closes the log file if it is not NULL.
 *
 * @return 0 on success, -1 on failure.
 */int closeLoggingSystem(void);


static FILE * log_file;

static struct timespec begin;

extern pthread_mutex_t log_mutex;

#endif




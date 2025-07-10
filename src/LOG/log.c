#include "log.h"

pthread_mutex_t log_mutex;

//there are multiple because different threads may fire these at the same time, create the same thread at the same time (no bueno)
pthread_t log_thread[3];//0 : master, 1 : data intake, 2 : data processing


int initLoggingSystem(void){//TESTME

    printf("in logging system\n");

    log_file = fopen(LOG_FILE_NAME, "w");
    
    if(log_file == NULL){
    
        (void)printf(RED"ERROR in %s:%d\n : Could not open log file.\n"RESET, __FILE__, __LINE__);
    
        return -1;
    }

    (void)clock_gettime(CLOCK_REALTIME, &begin);//don't change begin from now on

    printf("Logging system initialized\n");
    
    return log(NONE, LOG_INFO, "LOGGING SYSTEM INITIALIZED");
}

int log(const THREAD_ID thread_id, const LOG_TYPE log_type, char * message){
    
    LOG_PARAM log_param = {thread_id, log_type, message};

    if(pthread_create(log_thread[thread_id], NULL, _log, &log_param) != 0) return -1;

    pthread_detach(log_thread[thread_id]);

    return 0;
}


void * _log(void * param){

    LOG_PARAM * log_param = (LOG_PARAM *)param;

    const THREAD_ID thread_id = log_param->thread_id;
    const LOG_TYPE log_type = log_param->log_type;
    char * message = log_param->message;

    MUTEX_LOCK(&log_mutex);

    const int ret = __log(thread_id, log_type, message);

    MUTEX_UNLOCK(&log_mutex);

    return ret;
}



int __log(const THREAD_ID thread_id, const LOG_TYPE log_type, char * message){//TODO : find a way to close before a crash, or at least close and save periodically
    
    if(log_file == NULL){
    
        (void)printf(RED"ERROR in %s:%d\n : logging file was not accessible\n"RESET, __FILE__, __LINE__);
    
        return -1;
    }

    char thread_name[255];

    char log_type_name[255];

    switch(thread_id){
        case THREAD_MASTER:
            strcpy(thread_name, GREEN"MASTER");
            break;
        
        case THREAD_DATA_INTAKE:
            strcpy(thread_name, YELLOW"INTAKE");
            break;
        
        case THREAD_DATA_PROCESSING:
            strcpy(thread_name, BLUE"PROC");
            break;
        
        case NONE:
        default:
            strcpy(thread_name, "");
            break;
    }

    switch(log_type){
        case LOG_DEBUG:
            strcpy(log_type_name, PURPLE"DEBUG"RESET);
            break;
        case LOG_INFO:
            strcpy(log_type_name, CYAN"INFO"RESET);
            break;
        case LOG_WARNING:
            strcpy(log_type_name, YELLOW"WARNING"RESET);
            break;
        case LOG_ERROR:
            strcpy(log_type_name, RED"ERROR"RESET);
            break;
        default:
            strcpy(log_type_name, "UNKNOWN");
            break;
    }


    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    time_t seconds = now.tv_sec - begin.tv_sec;
    long nanoseconds = now.tv_nsec - begin.tv_nsec;

    if (nanoseconds < 0) {
        seconds--;
        nanoseconds += 1000000000L;
    }

    (void)fprintf(log_file, "%s - %s(%ld.%06ld): %s\n"RESET, log_type_name,thread_name,seconds, (long long)(nanoseconds * 0.001f), message);

    return 0;
}



int closeLoggingSystem(void){//TESTME
    printf("closing logging system\n");
    if (log_file != NULL) {
    
        fclose(log_file);

        return 0;
    }

    return -1;
}




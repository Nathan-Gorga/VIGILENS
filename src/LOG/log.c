#include "log.h"


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



int log(const THREAD_ID thread_id, const LOG_TYPE log_type, char * message){//TODO : find a way to close before a crash, or at least close and save periodically
    
    if(log_file == NULL){
    
        (void)printf(RED"ERROR in %s:%d\n : logging file was not accessible\n"RESET, __FILE__, __LINE__);
    
        return -1;
    }

    char thread_name[255];

    char log_type_name[255];

    switch(thread_id){
        case THREAD_MASTER:
            strcpy(thread_name, "MASTER");
            break;
        
        case THREAD_DATA_INTAKE:
            strcpy(thread_name, "INTAKE");
            break;
        
        case THREAD_DATA_PROCESSING:
            strcpy(thread_name, "PROC");
            break;
        
        case NONE:
        default:
            strcpy(thread_name, "");
            break;
    }

    switch(log_type){
        case LOG_DEBUG:
            strcpy(log_type_name, "DEBUG");
            break;
        case LOG_INFO:
            strcpy(log_type_name, "INFO");
            break;
        case LOG_WARNING:
            strcpy(log_type_name, "WARNING");
            break;
        case LOG_ERROR:
            strcpy(log_type_name, "ERROR");
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

    (void)fprintf(log_file, "%s(%ld.%06ld) - %s : %s\n",thread_name,seconds, (long long)(nanoseconds * 0.001f), log_type_name, message);

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




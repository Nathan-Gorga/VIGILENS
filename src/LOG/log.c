#include "log.h"


int initLoggingSystem(void){//TESTME

    log_file = fopen(LOG_FILE_NAME, "w");

    if(log_file == NULL){
    
        (void)printf(RED"ERROR in %s:%d\n : Could not open log file.\n"RESET, __FILE__, __LINE__);
    
        return -1;
    }
    
    return 0;
}



int logEntry(const THREAD_ID thread_id, const LOG_TYPE log_type, char * message){
    

}



int closeLoggingSystem(void){//TESTME
    
    if (log_file != NULL) {
    
        fclose(log_file);

        return 0;
    }

    return -1;
}




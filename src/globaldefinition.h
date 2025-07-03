#ifndef GLOBALDEFINITION_H
#define GLOBALDEFINITION_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>
#include <signal.h>
#include <bits/types/sigset_t.h>
#include <bits/sigaction.h>




#define BLACK "\e[0;30m"
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define YELLOW "\e[0;33m"
#define BLUE "\e[0;34m"
#define PURPLE "\e[0;35m"
#define CYAN "\e[0;36m"
#define WHITE "\e[0;37m"

#define RESET "\e[0m"


#define DEBUG

#ifdef DEBUG
    #define PRINTF_DEBUG do{printf(PURPLE"DEBUG : %s - %s:%d\n"RESET, __FILE__, __func__,__LINE__); fflush(stdout);}while(0);
#else
    #define PRINTF_DEBUG
#endif


#define MUTEX_LOCK(mutex) if(pthread_mutex_lock(mutex) != 0){ \
                                (void)printf("ERROR in %s:%d\n : You did something you shouldn't have...\n", __FILE__, __LINE__);\
                                pthread_exit(NULL);\
                           }

#define MUTEX_UNLOCK(mutex) if(pthread_mutex_unlock(mutex) != 0){ \
                                (void)printf("ERROR in %s:%d\n : You did something you shouldn't have...\n", __FILE__, __LINE__);\
                                pthread_exit(NULL);\
                            }

#endif


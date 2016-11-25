#ifndef	__2016_IS_MUTEX__
#define __2016_IS_MUTEX__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

extern pthread_mutex_t fd_mutx;
int get_symbol_id();

#endif

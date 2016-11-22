#ifndef __2016_IS_HANDLING__
#define __2016_IS_HANDLING__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

//#include "mutex.h"

#define MSG_SIZE	(4096)

void * handle_clnt(void * arg);
int get_http_size(char * msg, int recv_len, int clnt_sock);
void close_all(int serv_sock, int clnt_sock);

#endif

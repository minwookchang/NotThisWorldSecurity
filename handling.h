#ifndef __2016_IS_HANDLING__
#define __2016_IS_HANDLING__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define MSG_SIZE	(2048)
#define SERVER_ADDRESS	("127.0.0.1")
#define SERVER_PORT	(80)

void * handle_clnt(void * arg);
int get_http_size(char * msg, int recv_len);

#endif

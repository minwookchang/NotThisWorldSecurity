#ifndef __2016_IS_HANDLING__
#define __2016_IS_HANDLING__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "list_finder.h"
#include "bcrypt.h"

#include "mutex.h"

#define MSG_SIZE	(4096)
#define MSG_RECEIVE_SIZE	(2048)
#define SALT		("$2y$10$ThisIsNotRealWorldSalt")
#define AREA_LOCATION	("/TEMP/")
#define AREA_NAMESIZE	(6)
#define SYMBOL_SIZE		(19)

//ERROR TYPE
#define ERROR_TYPE_NO_ERROR	(0)
#define ERROR_TYPE_EXIT	(1)
//HTTP ERROR CODE
#define ERROR_404	(404)
//custom server socket ERROR CODE
#define ERROR_S_SOCK_CREATION		(800)
#define ERROR_S_CONNECTION	(801)
#define ERROR_S_OPTION		(802)
#define ERROR_S_TIMEOUT		(803)
#define ERROR_S_SEND		(804)
//custom client socket ERROR CODE
#define ERROR_C_OPTION		(902)
#define ERROR_C_TIMEOUT		(903)
#define ERROR_C_SEND		(904)

void * handle_clnt(void * arg);
int get_http_size(char * msg, int recv_len, int clnt_sock);
void close_all(int serv_sock, int clnt_sock);
/* TODO two function below */
int act(char *msg, int *recv_len, int *ERROR_CODE, char *symbol_path);
int error_proc(int serv_sock, int clnt_sock, int *ERROR_CODE, char *symbol_path);
int hash_load(char *msg, char *uri, char *uri_file_start, char *uri_file_pivot, char *uri_file_end, int *recv_len, char *symbol_path);

#endif

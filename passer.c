#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

//#include "mutex.h"
#include "handling.h"

#define MAX_CLNT 64

// from handling.c
extern in_addr_t SERVER_IP_ADDRESS;
extern int SERVER_PORT_NO;

void error_handling(char * msg);

int main(int argc, char *argv[]) {
	int pass_sock, clnt_sock;
	struct sockaddr_in pass_addr, clnt_addr;
	int clnt_addr_sz;

	pthread_t t_id[MAX_CLNT];
	//pthread_mutex_init(&fd_mutx, NULL);
	if(argc!=4) {
		printf("Usage : %s <port> <server address> <server port>\n", argv[0]);
		exit(1);
	}
	SERVER_IP_ADDRESS=inet_addr(argv[2]);
	SERVER_PORT_NO=atoi(argv[3]);
	if(SERVER_IP_ADDRESS==-1) {
		printf("Invalid Address\n");
		exit(1);
	}
	if(SERVER_PORT_NO==0) {
		printf("Invalid Server Port No\n");
		exit(1);
	}
	
	pass_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&pass_addr, 0, sizeof(pass_addr));
	pass_addr.sin_family=AF_INET;
	pass_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	pass_addr.sin_port=htons(atoi(argv[1]));

	if(bind(pass_sock, (struct sockaddr*) &pass_addr, sizeof(pass_addr))==-1)
		error_handling("bind() error");
	if(listen(pass_sock, 5)==-1)
		error_handling("listen() error");

	while(1) {
		clnt_addr_sz=sizeof(clnt_addr);
		//pthread_mutex_lock(&fd_mutx);
		clnt_sock=accept(pass_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_sz);
		//pthread_mutex_unlock(&fd_mutx);

		pthread_create(&t_id[clnt_sock], NULL, handle_clnt, (void*) &clnt_sock);
		pthread_detach(t_id[clnt_sock]);
		printf("Connected client IP: %s \n", inet_ntoa(clnt_addr.sin_addr));
	}
	close(pass_sock);
	return 0;
}

void error_handling(char * msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

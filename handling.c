#include "handling.h"

void * handle_clnt(void * arg) {
	int clnt_sock=*(int *)arg;
	char msg[MSG_SIZE];
	int recv_len=recv(clnt_sock, msg, MSG_SIZE-1, 0);
	if(recv_len<0) {
		fputs("recv() error\n",stderr);
		close(clnt_sock);
		return NULL;
	}
	msg[recv_len]='\0';
	printf("%s\n",msg);

	//server connection
	int serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock<0) {
		fputs("server socket() error\n",stderr);
		close(clnt_sock);
		return NULL;
	}
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(SERVER_ADDRESS);
	serv_addr.sin_port=htons(SERVER_PORT);

	if(connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))==-1) {
		fputs("server connect() error\n", stderr);
		close(clnt_sock);
		return NULL;
	}
	int pass_len=send(serv_sock, msg, strlen(msg), 0);
	if(pass_len<strlen(msg)) {
		fputs("server send() error\n", stderr);
		close(clnt_sock);
		close(serv_sock);
		return NULL;
	}

	char serv_msg[MSG_SIZE];
	while(1) {
		int serv_recv_len=recv(serv_sock, serv_msg, MSG_SIZE-1, 0);
		if(serv_recv_len>0) {
			serv_msg[serv_recv_len]='\0';
			int send_len=send(clnt_sock, serv_msg, strlen(serv_msg), 0);
			if(send_len<strlen(serv_msg)) {
				fputs("send() error\n",stderr);
				close(clnt_sock);
				close(serv_sock);
				return NULL;
			}
		} else if(serv_recv_len<0) {
			fputs("server recv() error\n",stderr);
			close(clnt_sock);
			close(serv_sock);
			return NULL;
		} else {
			//ends normally
			break;
		}
	}



	close(clnt_sock);
	printf("socket closed.\n");
	return NULL;
}

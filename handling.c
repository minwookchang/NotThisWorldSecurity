#include "handling.h"

void * handle_clnt(void * arg) {
	int clnt_sock=*(int *)arg;

	//server connection
	int serv_sock;
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock < 0) {
		fputs("server socket() error\n", stderr);
		close(serv_sock);
		close(clnt_sock);
		return NULL;
	}
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
	serv_addr.sin_port = htons(SERVER_PORT);
	if (connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		fputs("server connect() error\n", stderr);
		close(serv_sock);
		close(clnt_sock);
		return NULL;
	}

	//requesting loop
	printf("fd#%d requesting loop start\n", clnt_sock);
	char msg[MSG_SIZE];
	int start_flag = 1;
	int total_size = 0;
	int current_size = 0;
	while (1) {
		int recv_len = recv(clnt_sock, msg, MSG_SIZE - 1, 0);
		printf("fd#%d - %d received\n", clnt_sock, recv_len);
		if (start_flag==1) {
			total_size = get_http_size(msg, recv_len);
			start_flag = 0;
		}
		current_size += recv_len;
		if (recv_len < 0) {
			fputs("recv() error\n", stderr);
			close(serv_sock);
			close(clnt_sock);
			return NULL;
		} else if (recv_len > 0) {
			int pass_len = send(serv_sock, msg, recv_len, 0);
			printf("fd#%d - %d passed\n", clnt_sock, pass_len);
			if (pass_len < recv_len) {
				fputs("server send() error\n", stderr);
				close(serv_sock);
				close(clnt_sock);
				return NULL;
			} else if (current_size == total_size) {
				break;
			}
		} else {
			break;
		}
	}

	//responsing loop
	printf("fd#%d responsing loop start\n", clnt_sock);
	start_flag = 1;
	total_size = 0;
	current_size = 0;
	char serv_msg[MSG_SIZE];
	while (1) {
		int serv_recv_len = recv(serv_sock, serv_msg, MSG_SIZE - 1, 0);
		printf("fd#%d - %d responsed\n", clnt_sock, serv_recv_len);
		if (start_flag == 1) {
			total_size = get_http_size(serv_msg, serv_recv_len);
			start_flag = 0;
		}
		current_size += serv_recv_len;
		if (serv_recv_len < 0) {
			fputs("server recv() error\n", stderr);
			close(serv_sock);
			close(clnt_sock);
			return NULL;
		} else if (serv_recv_len > 0) {
			int send_len = send(clnt_sock, serv_msg, serv_recv_len, 0);
			printf("fd#%d - %d passed to client\n", clnt_sock, send_len);
			if (send_len < serv_recv_len) {
				fputs("send() error\n", stderr);
				close(serv_sock);
				close(clnt_sock);
				return NULL;
			} else if (current_size == total_size) {
				break;
			}
		} else {
			break;
		}
	}

	close(serv_sock);
	close(clnt_sock);
	printf("fd#%d socket closed.\n", clnt_sock);
	return NULL;
}

int get_http_size(char * msg, int recv_len) {
	char buffer[2048];
	strncpy(buffer, msg, recv_len);
	printf("%s\n", msg);
	char *end = strstr(buffer, "\r\n\r\n");
	if (end == NULL) {
		fputs("invalid header",stderr);
		return -1;
	}
	int header_len = end-buffer+4;
	buffer[header_len] = '\0';

	int content_len = 0;
	int i=0;	
	while(i<header_len) {
		if(strncmp(&buffer[i], "\r\nContent-Length: ", 18)==0) {
			if(sscanf(&buffer[i+18], "%d", &content_len)>0) {
				printf("total size : %d\n", header_len+content_len);
				return header_len+content_len;
			} else {
				printf("invalid header format\n");
				return -1;
			}
		}
		i++;
	}
	printf("total size(header only) : %d\n", header_len);
	return header_len;
}

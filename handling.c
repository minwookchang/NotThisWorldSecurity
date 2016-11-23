#include "handling.h"

in_addr_t SERVER_IP_ADDRESS;
int SERVER_PORT_NO;
void * handle_clnt(void * arg) {
	int clnt_sock=*(int *)arg;
	struct timeval tv_clnt2pass = {120, 0};	// it can vary
	struct timeval tv_serv2pass = {30, 0};	// it can vary
	/* TODO
		ERROR CODE implemented
	*/
	int ERROR_CODE=0;

	//server connection
	int serv_sock;
	//pthread_mutex_lock(&fd_mutx);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	//pthread_mutex_unlock(&fd_mutx);
	if (serv_sock < 0) {
		fputs("server socket() error\n", stderr);
		ERROR_CODE = ERROR_S_SOCK_CREATION;
		if (error_proc(serv_sock, clnt_sock, &ERROR_CODE) == ERROR_TYPE_EXIT)
			return NULL;
	}
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = SERVER_IP_ADDRESS;
	serv_addr.sin_port = htons(SERVER_PORT_NO);
	if (connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		fputs("server connect() error\n", stderr);
		ERROR_CODE = ERROR_S_CONNECTION;
		if (error_proc(serv_sock, clnt_sock, &ERROR_CODE) == ERROR_TYPE_EXIT)
			return NULL;
	}

	//requesting loop
	printf("fd#%d requesting loop start\n", clnt_sock);
	char msg[MSG_SIZE];
	int start_flag = 1;
	int total_size = 0;
	int current_size = 0;
	if(setsockopt(clnt_sock, SOL_SOCKET, SO_RCVTIMEO, &tv_clnt2pass, sizeof(tv_clnt2pass))==-1) {
		printf("fd#%d socket option failed\n", clnt_sock);
		ERROR_CODE = ERROR_C_OPTION;
		if (error_proc(serv_sock, clnt_sock, &ERROR_CODE) == ERROR_TYPE_EXIT)
			return NULL;
	}
	while (1) {
		memset(msg, 0, sizeof(msg));
		int recv_len = recv(clnt_sock, msg, MSG_SIZE - 1, 0);
		if (recv_len < 0) {
			printf("fd#%d recv() error or timeout\n", clnt_sock);
			ERROR_CODE = ERROR_C_TIMEOUT;
			if (error_proc(serv_sock, clnt_sock, &ERROR_CODE) == ERROR_TYPE_EXIT)
				return NULL;
		}
		printf("fd#%d - %d received\n", clnt_sock, recv_len);
		if (start_flag==1) {
			if(recv_len==0) {
				printf("fd#%d socket closed(cache usage estimated)\n", clnt_sock);
				close_all(serv_sock, clnt_sock);
				return NULL;
			}

			/* TODO
				create METHOD variable
				create URI variable
				parse a line and save it to a string variable
				parse METHOD and URI
				  allocate METHOD, get METHOD, save it (for next todo)
				  allocate URI, get URI, save it (for next todo)
			*/
			/* TODO
				when method is GET:
				  1. find URI in the whitelist file.
				    if there's line
				      end process.
				    if not
				      modify URI to hashed URI
				      find hashed URI from directory
				      if there's file
				        create symbolic link to safe area
				        modify hashed URI with safe area
				        modify recv_len with hashed URI
				      if not
				        error
				when method is POST or METHOD:
				  1. find URI in the whitelist file.
				    if there's line
				      end process
				    if not
				      error
			*/
			/* TODO
				free method variable
				free URI variable
			*/
			act(msg, &recv_len, &ERROR_CODE);
			/* TODO
				process error code from act
			*/
			if (error_proc(serv_sock, clnt_sock, &ERROR_CODE) == ERROR_TYPE_EXIT)
				return NULL;

			total_size = get_http_size(msg, recv_len, clnt_sock);
			start_flag = 0;
		}
		current_size += recv_len;
		if (recv_len > 0) {
			int pass_len = send(serv_sock, msg, recv_len, 0);
			printf("fd#%d - %d passed\n", clnt_sock, pass_len);
			if (pass_len < recv_len) {
				fputs("server send() error\n", stderr);
				ERROR_CODE = ERROR_S_SEND;
				if (error_proc(serv_sock, clnt_sock, &ERROR_CODE) == ERROR_TYPE_EXIT)
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
	if(setsockopt(serv_sock, SOL_SOCKET, SO_RCVTIMEO, &tv_serv2pass, sizeof(tv_serv2pass))==-1) {
		printf("fd#%d server socket option failed\n", clnt_sock);
		ERROR_CODE = ERROR_S_OPTION;
		if (error_proc(serv_sock, clnt_sock, &ERROR_CODE) == ERROR_TYPE_EXIT)
			return NULL;
	}
	while (1) {
		memset(serv_msg, 0, sizeof(serv_msg));
		int serv_recv_len = recv(serv_sock, serv_msg, MSG_SIZE - 1, 0);
		if (serv_recv_len < 0) {
			printf("fd#%d server recv() error or timeout\n", clnt_sock);
			ERROR_CODE = ERROR_S_TIMEOUT;
			if (error_proc(serv_sock, clnt_sock, &ERROR_CODE) == ERROR_TYPE_EXIT)
				return NULL;
		}
		printf("fd#%d - %d responsed\n", clnt_sock, serv_recv_len);
		if (start_flag == 1) {
			if(serv_recv_len==0) {
				printf("server return no data\n");
				close_all(serv_sock, clnt_sock);
				return NULL;
			}
			total_size = get_http_size(serv_msg, serv_recv_len, clnt_sock);
			start_flag = 0;
		}
		current_size += serv_recv_len;
		if (serv_recv_len > 0) {
			int send_len = send(clnt_sock, serv_msg, serv_recv_len, 0);
			printf("fd#%d - %d passed to client\n", clnt_sock, send_len);
			if (send_len < serv_recv_len) {
				fputs("send() error\n", stderr);
				ERROR_CODE = ERROR_C_SEND;
				if (error_proc(serv_sock, clnt_sock, &ERROR_CODE) == ERROR_TYPE_EXIT)
					return NULL;
			} else if (current_size == total_size) {
				break;
			}
		} else {
			break;
		}
	}

	close_all(serv_sock, clnt_sock);
	printf("fd#%d socket closed.\n", clnt_sock);
	return NULL;
}

int get_http_size(char * msg, int recv_len, int clnt_sock) {
	char buffer[2048];
	strncpy(buffer, msg, recv_len);
	printf("%s\n", msg);
	char *end = strstr(buffer, "\r\n\r\n");
	if (end == NULL) {
		printf("fd#%d invalid header\n", clnt_sock);
		return -1;
	}
	int header_len = end-buffer+4;
	buffer[header_len] = '\0';

	int content_len = 0;
	int i=0;	
	while(i<header_len) {
		if(strncmp(&buffer[i], "\r\nContent-Length: ", 18)==0) {
			if(sscanf(&buffer[i+18], "%d", &content_len)>0) {
				printf("fd#%d total size : %d\n", clnt_sock, header_len+content_len);
				return header_len+content_len;
			} else {
				printf("fd#%d invalid header format\n", clnt_sock);
				return -1;
			}
		}
		i++;
	}
	printf("fd#%d total size(header only) : %d\n", clnt_sock, header_len);
	return header_len;
}


void close_all(int serv_sock, int clnt_sock) {
	//pthread_mutex_lock(&fd_mutx);
	close(serv_sock);
	close(clnt_sock);
	//pthread_mutex_unlock(&fd_mutx);
	return;
}

int act(char *msg, int *recv_len, int *ERROR_CODE) {
	char *method, *uri, *version, *first_line_end;
	int method_len, uri_len;
	char * delimeter = "\r\n";

	/* TODO
		create METHOD variable
		create URI variable
		parse a line and save it to a string variable
		parse METHOD and URI
			allocate METHOD, get METHOD, save it (for next todo)
			allocate URI, get URI, save it (for next todo)
	*/
	method = msg;
	if (method == NULL) {
		*ERROR_CODE = 404;
		return -1;
	}
	first_line_end = strstr(msg, delimeter);
	if (first_line_end == NULL) {
		*ERROR_CODE = 404;
		return -1;
	}
	uri = strchr(method, ' ');
	if (uri == NULL || first_line_end - uri <= 0) {
		*ERROR_CODE = 404;
		return -1;
	}
	method_len = uri - method;
	uri++;
	version = strchr(uri, ' ');
	if (version == NULL || first_line_end - version <= 0) {
		*ERROR_CODE = 404;
		return -1;
	}
	uri_len = version - uri;
	version++;
	char *method_str = (char *)malloc(sizeof(char) * (method_len + 1));
	method_str[method_len] = '\0';
	strncpy(method_str, method, method_len);
	char *uri_str = (char *)malloc(sizeof(char) * (uri_len + 1));
	uri_str[uri_len] = '\0';
	strncpy(uri_str, uri, uri_len);
	printf("method : %s, URI : %s\n", method_str, uri_str);	//for testring purpose


	/* TODO
		when method is GET:
			1. find URI in the whitelist file.
				if there's line
					end process.
				if not
					modify URI to hashed URI
					find hashed URI from directory
						if there's file
							create symbolic link to safe area
							modify hashed URI with safe area
							modify recv_len with hashed URI
						if not
							error
		when method is POST or METHOD:
			1. find URI in the whitelist file.
				if there's line
					end process
				if not
					error
	*/
	/*
	if (strcmp(method_str, "GET") == 0) {
		char * uri_file_start;
		char * uri_file_end;
		int hash_size = 61;		//bcrypt hashspace

	}
	else {

	}
	*/

	/* TODO
	don't forget that in each return, free the allocated variables!!
		free method variable
		free URI variable
	*/
	free(method_str);
	free(uri_str);
	return 0;
}

/* TODO
	check ERROR_CODE
		if ERROR_CODE is in some domain of error
			check domain of error (ex) error that is ignorable, error that should be close both of serv_sock... (just for generalize))
			do some action for such types of action (ex) send client to 404 NOT FOUND ERROR ...)
			close socket(this can be depending on error type)
			return ERROR_TYPE (ERROR that should cut the connection function, or just resume connection....) : most of the case is just cut the connection
		if not
			return ERROR_TYPE_NO_ERROR
*/
int error_proc(int serv_sock, int clnt_sock, int *ERROR_CODE) {
	if (*ERROR_CODE > 799) {
		close_all(serv_sock, clnt_sock);
		*ERROR_CODE = 0;
		return ERROR_TYPE_EXIT;
	}
	 else if (*ERROR_CODE > 399) {
		if (*ERROR_CODE == 404) {
			// TODO some action(sending error packet to client etc...)
			close_all(serv_sock, clnt_sock);
			*ERROR_CODE = 0;
			return ERROR_TYPE_EXIT;
		}
		/*
		else if (if ERROR CODE is in some domain) {

		}
		else if (if ERROR CODE is in some domain) {

		}//...
		*/
	}
	else {
		return ERROR_TYPE_NO_ERROR;
	}
}
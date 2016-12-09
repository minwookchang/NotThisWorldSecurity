#include "handling.h"

in_addr_t SERVER_IP_ADDRESS;
int SERVER_PORT_NO;
void * handle_clnt(void * arg) {
	int clnt_sock=*(int *)arg;
	struct timeval tv_clnt2pass = {120, 0};	// client to proxy timeout set to 120 sec
	struct timeval tv_serv2pass = {30, 0};	// server to proxy timeout set to 30 sec

	int ERROR_CODE=0;
	char symbol_path[SYMBOL_SIZE + 10];
	memset(symbol_path, 0, SYMBOL_SIZE + 10);

	//server connection
	int serv_sock;
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock < 0) {
		fputs("server socket() error\n", stderr);
		ERROR_CODE = ERROR_S_SOCK_CREATION;
		if (error_proc(serv_sock, clnt_sock, &ERROR_CODE, symbol_path) == ERROR_TYPE_EXIT)
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
		if (error_proc(serv_sock, clnt_sock, &ERROR_CODE, symbol_path) == ERROR_TYPE_EXIT)
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
		if (error_proc(serv_sock, clnt_sock, &ERROR_CODE, symbol_path) == ERROR_TYPE_EXIT)
			return NULL;
	}
	while (1) {
		memset(msg, 0, sizeof(msg));
		int recv_len = recv(clnt_sock, msg, MSG_RECEIVE_SIZE - 1, 0);
		if (recv_len < 0) {
			printf("fd#%d recv() error or timeout\n", clnt_sock);
			ERROR_CODE = ERROR_C_TIMEOUT;
			if (error_proc(serv_sock, clnt_sock, &ERROR_CODE, symbol_path) == ERROR_TYPE_EXIT)
				return NULL;
		}
		printf("fd#%d - %d received\n", clnt_sock, recv_len);
		//get packets & measure packet length

		if (start_flag==1) {
			if(recv_len==0) {
				printf("fd#%d socket closed(cache usage estimated)\n", clnt_sock);
				close_all(serv_sock, clnt_sock);
				return NULL;
			}
			act(msg, &recv_len, &ERROR_CODE, symbol_path);
			if (error_proc(serv_sock, clnt_sock, &ERROR_CODE, symbol_path) == ERROR_TYPE_EXIT)
				return NULL;

			total_size = get_http_size(msg, recv_len, clnt_sock);
			start_flag = 0;
		}
		current_size += recv_len;
		// pass packet to server
		if (recv_len > 0) {
			int pass_len = send(serv_sock, msg, recv_len, 0);
			printf("fd#%d - %d passed\n", clnt_sock, pass_len);
			if (pass_len < recv_len) {
				fputs("server send() error\n", stderr);
				ERROR_CODE = ERROR_S_SEND;
				if (error_proc(serv_sock, clnt_sock, &ERROR_CODE, symbol_path) == ERROR_TYPE_EXIT)
					return NULL;
			} else if (current_size == total_size) {
				break;
			}
		} else {
			break;
		}
	}

	printf("msg\n%s\n", msg);
	//responsing loop
	printf("fd#%d responsing loop start\n", clnt_sock);
	start_flag = 1;
	total_size = 0;
	current_size = 0;
	char serv_msg[MSG_SIZE];
	if(setsockopt(serv_sock, SOL_SOCKET, SO_RCVTIMEO, &tv_serv2pass, sizeof(tv_serv2pass))==-1) {
		printf("fd#%d server socket option failed\n", clnt_sock);
		ERROR_CODE = ERROR_S_OPTION;
		if (error_proc(serv_sock, clnt_sock, &ERROR_CODE, symbol_path) == ERROR_TYPE_EXIT)
			return NULL;
	}
	//pass server response to client
	while (1) {
		memset(serv_msg, 0, sizeof(serv_msg));
		int serv_recv_len = recv(serv_sock, serv_msg, MSG_SIZE - 1, 0);
		if (serv_recv_len < 0) {
			printf("fd#%d server recv() error or timeout\n", clnt_sock);
			ERROR_CODE = ERROR_S_TIMEOUT;
			if (error_proc(serv_sock, clnt_sock, &ERROR_CODE, symbol_path) == ERROR_TYPE_EXIT)
				return NULL;
		}
		printf("fd#%d - %d responsed\n", clnt_sock, serv_recv_len);
		if (start_flag == 1) {
			printf("serv_msg\n%s\n", serv_msg);
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
				if (error_proc(serv_sock, clnt_sock, &ERROR_CODE, symbol_path) == ERROR_TYPE_EXIT)
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

//get http total size (header + data)
int get_http_size(char * msg, int recv_len, int clnt_sock) {
	char buffer[MSG_SIZE];
	strncpy(buffer, msg, recv_len);
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
	close(serv_sock);
	close(clnt_sock);
	return;
}

int act(char *msg, int *recv_len, int *ERROR_CODE, char *symbol_path) {
	char *method, *uri, *version, *first_line_end;
	int method_len, uri_len;
	char * delimeter = "\r\n";
	//get uri & http method
	method = msg;
	printf("msg1 : %s\n", msg);
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
	printf("uri1 : %s\n", uri);
	version = strchr(uri, ' ');
	if (version == NULL || first_line_end - version <= 0) {
		*ERROR_CODE = 404;
		return -1;
	}
	uri_len = version - uri;
	version++;
	printf("version1 : %s\n", version);
	char *method_str = (char *)malloc(sizeof(char) * (method_len + 1));
	method_str[method_len] = '\0';
	strncpy(method_str, method, method_len);
	char *uri_str = (char *)malloc(sizeof(char) * (uri_len + 1));
	uri_str[uri_len] = '\0';
	strncpy(uri_str, uri, uri_len);
	printf("method : %s\nuri : %s\n", method_str, uri_str);

	/*
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
	char * uri_file_start = NULL;	//the left of the start is directory, right is file
	char * uri_file_pivot = NULL;	//the left of the pivot is filename, right is extensions(not hashed)
	char * uri_file_end;	//the left of the end is file, right is GET extensions
	char record_bit = 0;
	//whitelisting from uri to uri_file_end(not included), hashing from uri_file_start(not included) to uri_file_pivot(not included).
	int i = uri_len - 1;
	while (i > -1) {
		if (uri[i] == '/') {
			uri_file_start = &uri[i];
			break;
		}
		i--;
	}
	if (uri_file_start == NULL) {
		*ERROR_CODE = 404;
		free(method_str);
		free(uri_str);
		return -1;
	}
	printf("uri_len : %d\nuri_file_start : %s\n", uri_len, uri_file_start);
	uri_file_end = strchr(uri_file_start, '?');
	if (uri_file_end == NULL) {
		uri_file_end = &uri[uri_len];
	} else if(uri_file_end - uri > uri_len) {
		uri_file_end = &uri[uri_len];
	}
	char * a = uri_file_end;
	while(a > uri_file_start) {
		if (*a == '.') {
			uri_file_pivot = a;
			break;
		}
		a--;
	}
	printf("uri_file_end : %s\na : %s\n", uri_file_end, a);
	printf("3\n");
	if (uri_file_pivot == NULL) {
		uri_file_pivot = uri_file_end;
	}

	record_bit = *uri_file_end;
	*uri_file_end = '\0';
	int list_flag = find(uri);
	printf("uri after find : %s\nlist_flag : %d\n", uri, list_flag);
	*uri_file_end = record_bit;
	if (list_flag == FILE_MISMATCH) {
		printf("4\n");
		if (hash_load(msg, uri, uri_file_start, uri_file_pivot, uri_file_end, recv_len, symbol_path) == -1) {
			*ERROR_CODE = 404;
			free(method_str);
			free(uri_str);
			return -1;
		}
		printf("5\n");
		
	}

	free(method_str);
	free(uri_str);
	return 0;
}

/* 
	check ERROR_CODE
		if ERROR_CODE is in some domain of error
			check domain of error (ex) error that is ignorable, error that should be close both of serv_sock... (just for generalize))
			do some action for such types of action (ex) send client to 404 NOT FOUND ERROR ...)
			close socket(this can be depending on error type)
			return ERROR_TYPE (ERROR that should cut the connection function, or just resume connection....) : most of the case is just cut the connection
		if not
			return ERROR_TYPE_NO_ERROR
*/
int error_proc(int serv_sock, int clnt_sock, int *ERROR_CODE, char *symbol_path) {
	if (*ERROR_CODE > 799) {
		char *ERROR_MSG = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
		send(clnt_sock, ERROR_MSG, strlen(ERROR_MSG), 0);
		close_all(serv_sock, clnt_sock);
		*ERROR_CODE = 0;
		if (strlen(symbol_path) != 0) {
			remove(symbol_path);
		}
		return ERROR_TYPE_EXIT;
	}
	 else if (*ERROR_CODE > 399) {
		if (*ERROR_CODE == 404) {
			char *ERROR_MSG = "HTTP/1.1 404 Not Found\r\n\r\n";
			send(clnt_sock, ERROR_MSG, strlen(ERROR_MSG), 0);
			close_all(serv_sock, clnt_sock);
			*ERROR_CODE = 0;
			if (strlen(symbol_path) != 0) {
				remove(symbol_path);
			}
			return ERROR_TYPE_EXIT;
		}
	}
	else {
		return ERROR_TYPE_NO_ERROR;
	}
}

//get filename & hash it, use it as new uri.
//with new uri, check directory whether new uri exists.
//if exists, make symbolic link by symbol_num. and modify request packet with TEMP/symbolic_link file.
int hash_load(char *msg, char *uri, char *uri_file_start, char *uri_file_pivot, char *uri_file_end, int *recv_len, char *symbol_path) {
	char *location;
	if (uri_file_start+1<uri_file_pivot) {
		location = (char *)malloc(sizeof(char)*(13 + uri_file_end - uri - (uri_file_pivot - uri_file_start - 1) + BCRYPT_HASHSIZE + 1));
		memset(location, 0, 13 + uri_file_end - uri - (uri_file_pivot - uri_file_start - 1) + BCRYPT_HASHSIZE + 1);
		strncat(location, "/var/www/html", 13);
		strncat(location, uri, uri_file_pivot - uri);
		char hash[BCRYPT_HASHSIZE];
		printf("location : %s\n", location);
		bcrypt_hashpw(&location[13+uri_file_start - uri + 1], SALT, hash);
		printf("hash : %s\n", hash);
		memcpy(&location[13 + uri_file_start - uri + 1], hash, BCRYPT_HASHSIZE);
		strncat(location, uri_file_pivot, uri_file_end - uri_file_pivot);
		printf("location2 : %s\n", location);
		struct stat buf;
		if (stat(location, &buf) == 0) {

			int symbol_id = get_symbol_id();
			sprintf(symbol_path, "/var/www/html/TEMP/%d", symbol_id);
			remove(symbol_path);
			// remove symbolic link
			if (symlink(location, symbol_path) == -1) {
				printf("FAIL to CREATE SYMBOLIC LINK\n");
			}
			// make symbolic link

			int origin_len = uri_file_end - uri;
			int finish_len = strlen(symbol_path) - 13;
			int changed_len = finish_len - origin_len;
			if (changed_len > 0) {
				char *a = &msg[*recv_len];
				while (a > uri_file_end - 1) {
					*(a + changed_len) = a[0];
					a--;
				}
			}
			else if (changed_len < 0) {
				char *a = uri_file_end;
				while (a < &msg[*recv_len + 1]) {
					*(a + changed_len) = a[0];
					a++;
				}
			}
			memcpy(uri, &symbol_path[13], strlen(symbol_path)-13);
			*recv_len = *recv_len + changed_len;
			free(location);
			return 0;
		}
		else {
			free(location);
			return -1;
		}
	}

	printf("location : %s\n", location);
	
	free(location);
	return -1;
}

#include "list_finder.h"

int find(char * URI) {
	int fd = open("./whitelist", O_RDONLY);
	if (fd < 1) {
		return FILE_MISMATCH;
	}
	char buffer[FILE_SIZE_LIMIT_APPROX];
	memset(buffer, 0, FILE_SIZE_LIMIT_APPROX);
	read(fd, buffer, FILE_SIZE_LIMIT_APPROX - 1);
	char * token = strtok(buffer, "\n");
	while (token) {
		if (token[strlen(token) - 1] == '/') {
			if (strlen(token) == 1) {
				if (strcmp(token, URI) == 0) {
					close(fd);
					return FILE_MATCH;
				}
			}
			else {
				if (strncmp(token, URI, strlen(token)) == 0) {
					close(fd);
					return FILE_MATCH;
				}
			}
		}
		else {
			if (strcmp(token, URI) == 0) {
				close(fd);
				return FILE_MATCH;
			}
		}
		token = strtok(NULL, "\n");
	}
	close(fd);
	return FILE_MISMATCH;
}
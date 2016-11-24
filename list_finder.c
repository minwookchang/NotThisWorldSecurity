#include "list_finder.h"

int find(char * URI) {
	int fd = open("./whitelist", O_RDONLY);
	if (fd < 1) {
		return FILE_MISMATCH;
	}
	char buffer[FILE_SIZE_LIMIT_APPROX];
	memset(buffer, 0, FILE_SIZE_LIMIT_APPROX);
	read(fd, buffer, FILE_SIZE_LIMIT_APPROX - 1);
	printf("buffer : %s\n", buffer);
	char * token = strtok(buffer, "\n");
	while (token) {
		if (strcmp(token, URI) == 0) {
			close(fd);
			return FILE_MATCH;
		}
		token = strtok(NULL, "\n");
	}
	close(fd);
	return FILE_MISMATCH;
}
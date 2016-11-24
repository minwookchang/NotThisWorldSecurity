#ifndef	__2016_IS_LIST_FINDER__
#define __2016_IS_LIST_FINDER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

// whitelist file should approximately less than 4096 byte for test situation
#define FILE_SIZE_LIMIT_APPROX	(4096)

#define FILE_MISMATCH	(0)
#define FILE_MATCH		(1)

int find(char * URI);

#endif

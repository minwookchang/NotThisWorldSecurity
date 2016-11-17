#include "EngineOff.h"
#include <cstdio>
#define CHARPERLINE 500
#define CHARPERWORD 100

EngineOff::EngineOff()
{
}


EngineOff::~EngineOff()
{
}


int EngineOff::exec(const char* _path)
{
	//file read
	pFile = fopen(_path, "rwb");

	long totnum = 0;
	long curnum = 0;
	int flag = 0;

	_fseeki64(pFile, 0, SEEK_END);
	long lSize = ftell(pFile);
	_fseeki64(pFile, 0, SEEK_SET);

	char* buf = (char*)malloc(sizeof(char)*lSize + 1);
	char* buf2 = (char*)malloc(sizeof(char) * CHARPERLINE);
	char* buf3 = (char*)malloc(sizeof(char) * CHARPERWORD);
	while (curnum = fread(&buf[totnum], sizeof(char), lSize - totnum, pFile) > 0) {
		totnum += curnum;
	}

	buf2 = strtok(buf, "\n");
	
	while (buf2 = NULL) {
		if (strcmp(buf2, "<Directory /var/www/html/" + *_path) == 0) {
			flag = 1;
		}
		if (flag == 1) {
			if (strcmp(buf2, "php_admin_value engine off") == 0) {
				return 0;
			}
		}
		buf2 = strtok(NULL, "\n");
	}

	//file read


	free(buf);
	free(buf2);
	return 0;
}


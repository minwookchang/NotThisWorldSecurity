#include "EngineOff.h"
#include <cstdio>


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

	_fseeki64(pFile, 0, SEEK_END);
	long lSize = ftell(pFile);
	_fseeki64(pFile, 0, SEEK_SET);

	char* buf = (char*)malloc(sizeof(char)*lSize + 1);
	while (curnum = fread(&buf[totnum], sizeof(char), lSize - totnum, pFile) > 0) {
		totnum += curnum;
	}




	return 0;
}


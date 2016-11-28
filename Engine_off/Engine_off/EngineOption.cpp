#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <fstream>
#include <iostream>
#include "EngineOption.h"

#define CHARPERLINE 500
#define CHARPERWORD 100
//#define FILEPATH "/etc/apache2/apache2.conf"
//for testing
#define FILEPATH "apache2.conf"

// 지우고 on 기능 추가하기
// - 1개만 있으면 3줄 지우고
// - 여러개 있으면 line을 지우고

EngineOption::EngineOption()
{
}


EngineOption::~EngineOption()
{
}


int EngineOption::exec(char* _path)
{

	string str = "\n<Directory /var/www/html/" + (string)_path + (string)">";
	string strend = "</Directory>\0";
	string strOption = "\tOptions FollowSymLinks\0";

	//file read
	pFile = fopen(FILEPATH, ("rb"));
	if (pFile == NULL) {
		cout << "Not open" << endl;
		return 0;
	}
	long totnum = 0;
	long curnum = 0;
	int flag = 0;

	fseek(pFile, 0, SEEK_END);
	long lSize = ftell(pFile);
	cout << lSize << endl;
	fseek(pFile, 0, SEEK_SET);

	char* buf = (char*)malloc(sizeof(char)*lSize);
	char* buf2 = (char*)malloc(sizeof(char) * CHARPERLINE);
	char* buf3 = (char*)malloc(sizeof(char) * CHARPERWORD);

	while (curnum = fread(&buf[totnum], sizeof(char), lSize - totnum, pFile) > 0) {
		totnum += curnum;
	}

	buf2 = strtok(buf, "\n");

	//if already exists
	while (buf2) {

		if (strncmp(buf2, str.c_str(), strlen(str.c_str())) == 0) {
			flag = 1;
		}
		if (flag == 1) {
			if (strncmp(buf2, strOption.c_str(), strlen(strOption.c_str())) == 0) {
				flag = 2;
				break;
			}
			else if (strncmp(buf2, strend.c_str(), strlen(strend.c_str())) == 0) {
				flag = 0;
			}
		}
		buf2 = strtok(NULL, "\n");
	}

	//there is already engine Option
	if (flag == 2) {
		cout << "Already Option set" << endl;
		return 0;

		free(buf);
		free(buf2);

	}
	else if (flag == 0) {

		fclose(pFile);

		pFile = fopen(FILEPATH, "a");
		if (pFile == NULL) {
			cout << "Not open" << endl;
			return 0;
		}
		fprintf(pFile, "%s\n%s\n%s", str.c_str(), strOption.c_str(), strend.c_str());

		free(buf);
		free(buf2);

		return 1;
	}

	else {
		cout << "Something wrong" << endl;

		free(buf);
		free(buf2);

		return 0;

	}
}
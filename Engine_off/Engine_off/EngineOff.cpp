/*
2016-2 IS Term Project
Autor : Minwook Chang
*/

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <fstream>
#include <iostream>
#include "EngineOff.h"

#define CHARPERLINE 500
#define CHARPERWORD 100
//file 권한은 이 곳에서 관리
#define FILEPATH "/etc/apache2/apache2.conf"

EngineOff::EngineOff()
{
}


EngineOff::~EngineOff()
{
}


int EngineOff::exec(char* _path)
{
	//default file path
	string str = "<Directory /var/www/html/"+ (string)_path + (string)">";
	string strend = "</Directory>\0";
	string stroff = "\tphp_admin_value engine off\0";

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
	fseek(pFile, 0, SEEK_SET);

	char* buf = (char*)malloc(sizeof(char)*lSize);
	char* buf2 = (char*)malloc(sizeof(char) * CHARPERLINE);
	char* buf3 = (char*)malloc(sizeof(char) * CHARPERWORD);

	while (curnum = fread(&buf[totnum], sizeof(char), lSize - totnum, pFile) > 0) {
		totnum += curnum;
	}

	buf2 = strtok(buf,"\n");
	//if already exists
	while (buf2) {
		// 해당 디렉토리에 대한 문구가 있을 경우 flag = 1
		if (strncmp(buf2,str.c_str(),strlen(str.c_str())) == 0) {
			flag = 1;
		}
		//해당 디렉토리에 이미 동일한 옵션이 있을 경우 flag = 2
		if (flag == 1) {
			if (strncmp(buf2, stroff.c_str(), strlen(stroff.c_str())) == 0) {
				flag = 2;
				break;
			}
			//해당 디렉토리에 대한 동일한 옵션이 없이 끝나는 경우 다시 flag = 0
			else if (strncmp(buf2, strend.c_str(), strlen(strend.c_str())) == 0) {
				flag = 0;
			}
		}
		buf2 = strtok(NULL, "\n");
	}

	//there is already engine off
	if (flag == 2){
		cout << "Already engine off" << endl;
		fclose(pFile);
		return 0;

		free(buf);
		free(buf2);

	}
	//flag이 0으로 끝났다는 것은, 해당 디렉토리에 대한 engine off 옵션이 없다는 것.
	else if (flag == 0) {

		fclose(pFile);
		pFile = fopen(FILEPATH, "a");
		if (pFile == NULL) {
			cout << "Not open" << endl;
			return 0;
		}
		//폴더에 engine off 옵션을 기록한다.
		fprintf(pFile, "\n%s\n%s\n%s",str.c_str(),stroff.c_str(),strend.c_str());
		
		free(buf);
		free(buf2);
		fclose(pFile);
		return 1;
	}

	//에러 처리
	else {
		cout << "Something wrong" << endl;
		free(buf);
		free(buf2);
		fclose(pFile);
		return 0;
	}

}
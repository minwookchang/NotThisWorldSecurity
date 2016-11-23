#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <vector>

using namespace std;

class EngineOff
{
private: 
	FILE* pFile;

public:
	EngineOff();
	~EngineOff();
	int exec(char* _path);
};



#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <vector>

using namespace std;

class EngineOption
{
private:
	FILE* pFile;

public:
	EngineOption();
	~EngineOption();
	int exec(char* _path);
};
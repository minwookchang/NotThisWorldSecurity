#pragma once
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
	int exec(const char* _path);
};



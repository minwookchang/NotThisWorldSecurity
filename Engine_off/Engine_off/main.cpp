#include <iostream>
#include "EngineOff.h"
#include <cstring>
#define _CRT_SECURE_NO_WARNINGS
#define LENGTH 100
using namespace std;


void main() {

	EngineOff engineoff;
		
	char s[LENGTH];
	gets_s(s);

	int result = engineoff.exec(s);

	if (result == 1) {
		cout << "Engine off Success!" << endl;
	}
	else {
		cout << "dir already off" << endl;
	}

}
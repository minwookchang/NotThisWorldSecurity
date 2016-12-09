/*
2016-2 IS Term Project
Autor : Minwook Chang
*/
#include <iostream>
#include "EngineOff.h"
#include "EngineOption.h"
#include <cstring>
#define _CRT_SECURE_NO_WARNINGS
#define LENGTH 100
using namespace std;

int main() {
		
	char s[LENGTH];
	int result = 0;
	//UI
	while (1) {
		
		// 1: for engine off
		// 2: for dir index option
		// q: for quit

		cout << "\n**Press the button**\nEngine off : press 1\nOptions FollowSymLinks : 2\nExit : q" << endl;
		gets_s(s);
		
		//engine off
		if (strcmp(s,"1") == 0) {

			//get dir path
			cout << "Input the dir path" << endl;
			gets_s(s);

			//exec engine off
			EngineOff engineoff;
			result = engineoff.exec(s);
			if (result == 1) {
				cout << "Engine off Success!" << endl;
			}

		}
		//dir index option
		else if (strcmp(s, "2") == 0) {

			//get dir path
			cout << "Input the dir path" << endl;
			gets_s(s);

			//exec option
			EngineOption engineoption;
			result = engineoption.exec(s);

			if (result == 1) {
				cout << "Option Success!" << endl;
			}
		}
		else if (strcmp(s, "q") == 0) {
			return 0;
		}

		//execption
		else {
			cout << "error input" << endl;
		}
	}
	return 0;
}
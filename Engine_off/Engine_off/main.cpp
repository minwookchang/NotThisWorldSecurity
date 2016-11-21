#include <iostream>
#include "EngineOff.h"
#define _CRT_SECURE_NO_WARNINGS

using namespace std;


void main() {


	 EngineOff engineoff;
	
	 int result = engineoff.exec("test.txt");

	 if (result == 1) {
		 cout << "Engine off Success!" << endl;
	 }
	 else {
		 cout << "dir already off" << endl;
	 }

}
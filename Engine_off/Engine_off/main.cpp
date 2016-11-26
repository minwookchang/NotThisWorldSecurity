#include <iostream>
#include "EngineOff.h"
#include <cstring>
#define _CRT_SECURE_NO_WARNINGS
#define LENGTH 100
using namespace std;

/* 
<Directory /var/www/*>
Options FollowSymLinks
</Directory>
*/
void main() {
		
	char s[LENGTH];
	int result = 0;
	while (1) {
		cout << "\n**Press the button**\nEngine off : press 1\nOptions FollowSymLinks : 2\nExit : q" << endl;
		gets_s(s);
		//engine off
		if (strcmp(s,"1") == 0) {

			cout << "Input the dir path" << endl;
			gets_s(s);

			EngineOff engineoff;
			result = engineoff.exec(s);
			if (result == 1) {
				cout << "Engine off Success!" << endl;
			}
		}
		else if (strcmp(s, "2") == 0) {

			cout << "Input the dir path" << endl;
			gets_s(s);

			
			if (result == 1) {
				cout << "Option Success!" << endl;
			}
		}
		else if (strcmp(s, "q") == 0) {
			return;
		}
		else {
			cout << "error input" << endl;
		}


	}

}
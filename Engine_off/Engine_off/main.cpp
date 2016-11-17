#include <iostream>
using namespace std;

void main(char* argv[]) {


	 EngineOff engineoff;
	
	 int result = engineoff.exec(argv[]);

	 if (result == 1) {
		 cout << "Engine off Success!" << endl;
	 }
	 else {
		 cout << "dir already off" << endl;
	 }

}
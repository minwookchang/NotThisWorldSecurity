#include "mutex.h"

pthread_mutex_t fd_mutx;

static int symbol_no = 0;

//get symbolic number
int get_symbol_id() {
	int result;
	//lock mutex
	pthread_mutex_lock(&fd_mutx);
	result = symbol_no;
	//if symbol_no is too large, set to 0
	if (symbol_no == 1073741824)
		symbol_no = 0;
	symbol_no++;
	pthread_mutex_unlock(&fd_mutx);
	// return symbol_no
	return result;
}

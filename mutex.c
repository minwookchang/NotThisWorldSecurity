#include "mutex.h"

pthread_mutex_t fd_mutx;

static int symbol_no = 0;

int get_symbol_id() {
	int result;
	pthread_mutex_lock(&fd_mutx);
	result = symbol_no;
	if (symbol_no == 1073741824)
		symbol_no = 0;
	symbol_no++;
	pthread_mutex_unlock(&fd_mutx);
	return result;
}
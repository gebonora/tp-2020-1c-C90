#include "main.h"

int main(){
	init_config();
	init_queues();
	init_semaphores();
	init_server();
	return 0;
}

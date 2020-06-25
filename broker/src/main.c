#include "main.h"

int main(){
	init_config();
	init_queues();
	init_semaphores();
	init_context();
	init_threads();
	//init_memory();
	init_dump();
	init_server();
	return 0;
}

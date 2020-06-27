#include "dump.h"

static void _create_dump();

void dump_handler(int signum) {
	if(signum == SIGUSR1) {
		log_info(LOGGER, "Received SIGUSR1");
		_create_dump();
		log_info(LOGGER, "Dump file created at: %s", DUMP_PATH);
	}
}

/** PRIVATE FUNCTIONS **/

// TODO: mostrar la posta
static void _create_dump() {
    FILE * fp;
	fp = fopen(DUMP_PATH, "w+");
	fprintf(fp, "Dump: %s\n", current_date_time_as_string());
	fprintf(fp, "Particion 1: ...");
	fclose(fp);
}

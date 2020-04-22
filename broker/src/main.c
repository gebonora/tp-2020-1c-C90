#include "main.h"

int main(){

	t_config* config = config_create("brokerconfig.config");

	IP = config_get_string_value(config, "IP_BROKER");
	PUERTO = config_get_string_value(config, "PUERTO_BROKER");
	TAMANO_MEMORIA = config_get_int_value(config, "TAMANO_MEMORIA");
	TAMANO_MINIMO_PARTICION = config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
	ALGORITMO_MEMORIA = config_get_string_value(config, "ALGORITMO_MEMORIA");
	ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	FRECUENCIA_COMPACTACION = config_get_int_value(config, "FRECUENCIA_COMPACTACION");

	iniciar_servidor();
	return 0;
}

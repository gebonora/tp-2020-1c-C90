#include "mem.h"
/*
void add (void* dato_que_termina_en_cache, Message message)
	if(strcmp(ALGORITMO_MEMORIA, "BS")) add_buddy_system();
	else add_dynamic_partitions();
}

void add_dynamic_partitions() {
	Partition* particion = find_partition(tamanioQueQuieroGuardar);

	if(partition != NULL) {
		guardar_dato(dato);
	} else {
		if(FRECUENCIA_COMPACTACION != -1) {
			incrementar_busquedas_fallidas();
			if(cantidad_de_fallidas == FRECUENCIA_COMPACTACION) {
				compactar();
				 particion = find_partition(tamanioQueQuieroGuardar);
				 if(partition != NULL) {
					 guardar_dato(dato);
				 } else {
					 incrementar_busquedas_fallidas();
					 choose_victim();
				 }
			} else {
				incrementar_busquedas_fallidas();
				 choose_victim();
			}

		} else {
			choose_victim();
			add_dynamic_partitions();
		}
	}
}

// cuando guardamos el dato, hay que setearle el access_time a la particion
// devolver puntero a la particion
guardar_dato(dato, particion) {

}

// 1) filtramos las particiones libres que satisfacen el tamañoAGuardar
// 2) buscamos la particion en base al algoritmo FF / BF
// 3) si encontre una, la rompo si hace falta en 2 particiones (1 del tamanioQueVoyAGuardar)
//  y la otra de lo que le quedo libre
//  Ej: guardo 30, tengo libre una de 80 (nro 7)
//  me queda -> particion a usar (nro 7) tamanio 30 + particion (nro8) tamanio 50
//  // TODO: ver lo del tamanioMinioDeParticion
// 4) devolvemos la particion encontrada o un NULL si no encontre ninguna

// first . filter(free && tamanio>=tamanioABuscar)
Partition* find_partition(tamanioABuscar) {

	t_list* partitionss = list_filter(partitions, estaLireYTamanioMayorIgualQue(tamanioABuscar));

	if(partitionss->elements_count > 0) {
		if(strcmp(ALGORITMO_PARTICION_LIBRE, "BF")) {
			list_sort(partitionss, menorTamanio);
		}
		// ROMPER LA PARTICION
		// tamanio particion encontrada == tamanioAGuardar -> devuelvo asi
		// tamanio particion encontrada >  tamanioAGuardar -> trunco particion encontrada y genero una nueva con el excedente
		return list_get(partitionss, 0);
	} else {
		return NULL;
	}
}

bool estaLibreYTamanioMayorIgualQue( , tamanioABuscar) {

}

bool menorTamanio(Partition* partition_1, Partition* partition_2) {
	return partition_1->partition_size < partition_2->partition_size;
}

// 1) calculo la potencia de 2 mas cercana (hacia arriba) al valor de tamanioDeMiDato
//  2) busco una particion libre de tamanio = pot2
//   	   2.a) si la encuentro guardo el dato
//   	   2.b.i) si no la encuentro, busco la particion libre mas cercana a la pot2 calculada
//   	   2.b.ii) genero n buddys particionando hasta que me queda el tamanio del buddy = pot2 calculadass
//   3) si no encontre ninguna particion, elijo una victima y vuelvo a lanzar la busqueda
void add_buddy_system(dato) {

	int pot2 = calcular_potencia_2(tamanioAGuardar); // me tiene que dar el max entre pot2 mas cercana a mi tamanio y TAMANIO_MINIO_PARTICION
	Particion* partition = find_partition_buddy(pot2);

	if(partition != NULL) {
		guardar_dato_buddy(dato, partition);
	} else {
		choose_victim_buddy();
		compactar_buddy_si_puedo();
		add_buddy_system(dato);
	}
}

// para compactar tiene que cumplir: el tipo esta libre + su buddy esta libre + su buddy es del mismo tamanio
// y hacer recursivo hasta que no puedo compactar ma
// para calcular la direccion del buddy uso el XOR con la direccion del tipo que estoy analizando
// XOR -> (miDireccionDeMemoria ^ miTamanioDeMemoria)
compactar_buddy_si_puedo(){

}

// siempre que mato una particion, tengo que consolidar, siempre
choose_victims() {
	// usar strcmp?
	if(strcmp(ALGORITMO_REEMPLAZO, "FIFO")) {
		fifo();
	} else {
		lru();
	}
}

// si nos confirman en el foro que siempre es "BF", podemos reutilizar el find para dinamicas y buddy
// por configuracion de archivo siempre vamos a tener "BF"
Partition* find_partition_buddys(potenciaDe2ABuscar) {

	t_list* partitionss = list_filter(partitions, estaLireYTamanioMayorIgualQue(tamanioABuscar));

	if(partitionss->elements_count > 0) {
		list_sort(partitionss, menorTamanio);
		// ROMPER LA PARTICION en Buddys
		// tamanio particion encontrada == tamanioAGuardar -> devuelvo asi
		// tamanio particion encontrada >  tamanioAGuardar -> divido_en_2 hasta que tengo el buddy de la pot2 buscada
		Partition* elegida = list_get(partitionss, 0);

		// mientras no tenga el buddy de tamanio = miPotenciade2 sigo rompiendo 1 de las mitades
		while(elegida->partition_size != potenciaDe2ABuscar) {
			elegida = divido_en_2(elegida); // divido_en_2 me tiene que dar uno de los 2 buddys generados
		}
		return elegida;
	} else {
		return NULL;
	}
}


// ver como trackear a la siguiente victimas
void fifo(){

}
*/

Partition* lru(t_list* partitions) {
	t_list* occupied_partitions = list_filter(partitions, &is_occupied);

	if(occupied_partitions->elements_count > 0) {
		list_sort(occupied_partitions, &less_access_time);
		return list_get(occupied_partitions, 0);
	}
	return NULL;
}

Partition* memory_lru() {
	t_list* occupied_partitions = list_sorted(list_filter(memory->partitions, &is_occupied), &less_access_time);

	if(occupied_partitions->elements_count > 0) {
		return list_get(occupied_partitions, 0);
	}
	return NULL;
}

t_list* get_occupied_partitions() {
	return list_filter(memory->partitions, &is_occupied);
}

t_list* get_sorted_partitions() {
	t_list* partitions = list_filter(memory->partitions, &is_occupied);
	list_sort(partitions, &less_access_time);
	return partitions;
}

bool less_access_time(Partition* partition_a, Partition* partition_b) {
	return partition_a->access_time < partition_b->access_time;
}

bool is_occupied(Partition* partition) {
	return !partition->free;
}

Message* create_message(Operation operation, uint32_t message_id, uint32_t correlational_id, uint32_t data_size) {
	Message* message = malloc(sizeof(Message));
	message->operation_code = operation;
	message->message_id = message_id;
	message->correlational_id = correlational_id;
	message->data_size = data_size;
	return message;
}

Partition* create_partition(uint32_t partition_number, uint32_t partition_size, uint32_t* partition_start, uint32_t position, Message* message) {
	Partition* partition = malloc(sizeof(Partition));
	partition->access_time = (int) ahoraEnTimeT() - partition_number; // borrar la resta
	partition->free = partition_number % 2 == 0 ? true : false;
	partition->number = partition_number;
	partition->size = partition_size;
	partition->start = partition_start;
	partition->position = position;
	partition->message = message;
	return partition;
}

void show_partitions(t_list* partitions) {
	log_info(LOGGER, "--------------------------------");
	log_info(LOGGER, "Partitions size: %d", memory->partitions->elements_count);
	list_iterate(partitions, &show_partition);
}

void show_memory_partitions() {
	log_info(LOGGER, "--------------------------------");
	log_info(LOGGER, "Partitions size: %d", memory->partitions->elements_count);
	list_iterate(memory->partitions, &show_partition);
}

void show_partition(Partition* partition) {
	log_info(LOGGER, "Partition #%d", partition->number);
	log_info(LOGGER, "Free: %s", partition->free ? "true" : "false");
	log_info(LOGGER, "Start: %d - %06p", partition->position, partition->start);
	log_info(LOGGER, "Size: %d", partition->size);
	log_info(LOGGER, "Buddy: %d - %06p", xor(partition->position, partition->size), xor(partition->start, partition->size));
	log_info(LOGGER, "Last access: %d", partition->access_time);
	show_message(partition->message);
	log_info(LOGGER, "--------------------------------");
}

void show_message(Message* message) {
	log_info(LOGGER, "Message Queue: %s", get_operation_by_value(message->operation_code));
	log_info(LOGGER, "Message ID: %d", message->message_id);
	log_info(LOGGER, "Correlative ID: %d", message->correlational_id);
	log_info(LOGGER, "Message Size: %d", message->data_size);
}

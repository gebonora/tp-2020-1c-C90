#include "dynamic.h"

static bool _is_free(Partition*);
static t_link_element* _list_find_element(t_list *self, bool(*condition)(void*), int* index);

// cuando guardamos el dato, hay que setearle el access_time a la particion
// devolver puntero a la particion
void guardar_dato(dato, particion) {

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

Partition* find_partition_dynamic(uint32_t tamanioABuscar) {
/*
	t_list* partitions = find_partition(tamanioABuscar);

	if(partitions->elements_count > 0) {
		if(strcmp(ALGORITMO_PARTICION_LIBRE, "BF")) {
			list_sort(partitions, &menorTamanio);
		}
		t_link_element* elegida = _list_get_element(partitions, 0);
		t_link_element* next_partition = elegida->next;

		int nuevo_tamanio_particion = MAX_PARTITION_SIZE(tamanioABuscar);

		Partition* elegidaPartition = elegida->data;
		int tamanio_viejo = elegidaPartition->size;

		elegidaPartition->size = nuevo_tamanio_particion;
		elegidaPartition->free = false;
		elegidaPartition->access_time = (int)ahoraEnTimeT();
		// TODO: ver access time,
		 // ver metodo de seba nueva particion


		Partition* new_partition = malloc(sizeof(Partition));
		new_partition->free = true;
		new_partition->size = tamanio_viejo - nuevo_tamanio_particion;
		new_partition->start = elegidaPartition->start + nuevo_tamanio_particion;
		new_partition->position = elegidaPartition->position + nuevo_tamanio_particion;

		t_link_element* new_element = malloc(sizeof(t_link_element));
		new_element->data = new_partition;
		new_element->next = next_partition;
		elegida->next = new_element;


		// ROMPER LA PARTICION
		// tamanio particion encontrada == tamanioAGuardar -> devuelvo asi
		// tamanio particion encontrada >  tamanioAGuardar -> trunco particion encontrada y genero una nueva con el excedente
		return elegidaPartition;
	} else {
		return NULL;
	}
	*/
}


void save_to_cache_dynamic_partitions(void* data, Message* message) {
	int partitions_killed = 0;
	Partition* partition = NULL;

	while(partition == NULL){
		partition = find_partition(message->data_size);
		if (partition != NULL) {
			guardar_dato(data);
		} else {
			if(FRECUENCIA_COMPACTACION == partitions_killed) {//casos igual o 0
				compactar();
			} else {
				choose_victim();//que tambien la mata
				consolidate();
			}
			partitions_killed++;
		}
	}
}

/*Busco la primera libre.
 * Me fijo si la siguiente esta libre. Si es asi, la absorbo.
 * Por las dudas tengo que fijarme si la siguiente tambien esta libre (caso en el que borro una particion que esta rodeada de dos libres).
 * Si es asi, la absorbo.
 */
void consolidate(){

	t_link_element* first_free = _list_find_element(memory->partitions, &_is_free, NULL);

	while(first_free->next != NULL){

	}

}

void compactar() {

}

/** PRIVATE FUNCTIONS **/

static bool _is_free(Partition* partition) {
	return !partition->free;
}

static t_link_element* _list_find_element(t_list *self, bool(*condition)(void*), int* index) {
	t_link_element *element = self->head;
	int position = 0;

	while (element != NULL && !condition(element->data)) {
		element = element->next;
		position++;
	}

	if (index != NULL) {
		*index = position;
	}

	return element;
}


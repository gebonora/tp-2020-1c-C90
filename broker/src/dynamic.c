#include "dynamic.h"

static void _free_suscriber(Suscriber*);
static void _consolidate();

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

Partition* find_partition_dynamic(uint32_t sizeOfData) {

	t_link_element* partition_element = find_partition(sizeOfData);

	if(partition_element != NULL) {

		t_link_element* next_partition = partition_element->next;

		int new_size = MAX_PARTITION_SIZE(sizeOfData);

		Partition* partition = partition_element->data;
		int old_size = partition->size;

		partition->size = new_size;
		partition->free = false;
		int now = (int) ahoraEnTimeT();
		partition->access_time = now;
		partition->creation_time = now;
		// TODO: ver access time,
		 // ver metodo de seba nueva particion


		Partition* new_partition = malloc(sizeof(Partition));
		new_partition->free = true;
		new_partition->size = old_size - new_size;
		new_partition->start = partition->start + new_size;
		new_partition->position = partition->position + new_size;

		t_link_element* new_element = malloc(sizeof(t_link_element));
		new_element->data = new_partition;
		new_element->next = next_partition;
		partition_element->next = new_element;


		// ROMPER LA PARTICION
		// tamanio particion encontrada == tamanioAGuardar -> devuelvo asi
		// tamanio particion encontrada >  tamanioAGuardar -> trunco particion encontrada y genero una nueva con el excedente
		return partition;
	} else {
		return NULL;
	}
}


void save_to_cache_dynamic_partitions(void* data, Message* message) {
	int partitions_killed = 0;
	Partition* partition = NULL;

	while(partition == NULL){
		partition = find_partition_dynamic(message->data_size);
		if (partition != NULL) {
			guardar_dato(data);
		} else {
			if(FRECUENCIA_COMPACTACION == partitions_killed) {//casos igual o 0
				compactar();
			} else {
				choose_victim();//que tambien la mata
				_consolidate();
			}
			partitions_killed++;
		}
	}
}


void compactar() {

}

/** PRIVATE FUNCTIONS **/

static void _free_suscriber(Suscriber* suscriber) {
	free(suscriber);
}

/*Busco la primera libre.
 * Me fijo si la siguiente esta libre. Si es asi, la absorbo.
 * Por las dudas tengo que fijarme si la siguiente tambien esta libre (caso en el que borro una particion que esta rodeada de dos libres).
 * Si es asi, la absorbo.
 */
static void _consolidate(){

	t_link_element* left_element;

	for(int i = 0; i < memory->partitions->elements_count; i++){
		t_link_element* element = list_get_element(memory->partitions, i);
		if(((Partition*)(element->data))->free && ((Partition*)(element->next->data))->free && element->next->data != NULL){
			left_element = element;
			break;
		}
	}
	//ver de liberar los mensajes, listas etc para que quede limpita


	if(left_element != NULL){
		Partition* left_partition = left_element->data;
		t_link_element* middle_element = left_element->next;
		Partition* middle_partition = middle_element->data;
		t_link_element* left_partition_next;

		left_partition->size = left_partition->size + middle_partition->size;
		left_partition_next = middle_element->next;

		t_link_element* right_element;
		if(middle_element->next !=NULL && ((Partition*)(middle_element->next->data))->free){
			right_element = middle_element->next;
			Partition* right_partition = right_element->data;
			left_partition->size = left_partition->size + right_partition->size;
			left_partition_next = right_element->next;

			free(right_partition->message);
			list_destroy_and_destroy_elements(right_partition->notified_suscribers, &_free_suscriber);
			free(right_element);
		}

		left_element->next = left_partition_next;
		free(middle_partition->message);
		list_destroy_and_destroy_elements(middle_partition->notified_suscribers, &_free_suscriber);
		free(middle_partition);
		free(middle_element);
	}
}

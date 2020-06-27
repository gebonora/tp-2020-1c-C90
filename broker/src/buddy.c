#include "buddy.h"

static void _consolidate_buddy(Partition*);
static bool _partition_at_position(uintptr_t, uintptr_t);
static uint32_t _buddy_address(Partition*);
static Partition* _buddy_of(Partition*);
static Partition* _find_partition_buddys();
static Partition* _break_buddys(Partition*, uint32_t);

// 1) calculo la potencia de 2 mas cercana (hacia arriba) al valor de tamanioDeMiDato
//  2) busco una particion libre de tamanio = pot2
//   	   2.a) si la encuentro guardo el dato
//   	   2.b.i) si no la encuentro, busco la particion libre mas cercana a la pot2 calculada
//   	   2.b.ii) genero n buddys particionando hasta que me queda el tamanio del buddy = pot2 calculadass
//   3) si no encontre ninguna particion, elijo una victima y vuelvo a lanzar la busqueda
void save_to_cache_buddy_system(void* data, Message* message) {
	/*
	int desired_size = MAX_PARTITION_SIZE(next_power_of_2(message->data_size));

	Partition* partition = find_partition_buddy(desired_size);

	while(partition == NULL) {
		Partition* victim = choose_victim_buddy();
		_consolidate_buddy(victim);
		partition = find_partition_buddy(desired_size);

	}
	Partition* where_to_save_data = break_buddys(partition, message->data_size);

	// guardo el data con el memcpy
	// guardo el message
	partition->message = message;
	*/

}

/** PRIVATE FUNCTIONS **/

// para compactar tiene que cumplir: el tipo esta libre + su buddy esta libre + su buddy es del mismo tamanio
// y hacer recursivo hasta que no puedo compactar ma
// para calcular la direccion del buddy uso el XOR con la direccion del tipo que estoy analizando
// XOR -> (miDireccionDeMemoria ^ miTamanioDeMemoria)
static void _consolidate_buddy(Partition* partition) {
	/*
	Partition* buddy = _buddy_of(partition);
	Partition* survivor;

	// si puedo consolidar, arranco, sino no hago nada
	while(buddy->free && buddy->size == partition->size) {
		// consolido
		// obtengo la particion mas al izquierda, seria la ede menor posicion de memoria
		if(partition->start < buddy->start) {
			survivor = partition;
		} else {
			survivor = buddy;
		}

		// creo la nueva particion
		// TODO: crear una funcion para esto
		uint32_t now = (int) ahoraEnTimeT();
		survivor->size = partition->size + buddy->size;
		survivor->creation_time = now;
		survivor->access_time = now;

		// reemplazo la particion por esta nueva y elimino la data vieja
		//list_replace_and_destroy_element();
		// elimino el buddy que sobro, ya que fue absorbido
		//list_remove_and_destroy_by_condition(memory_partitions, &_funcion_para_obtener el buddy);

		buddy = _buddy_of(survivor);
		partition = survivor;
	}
	*/
}

static Partition* _break_buddys(Partition* partition_to_break, uint32_t data_size) {
/*
	// mientras la particion actual sea mayor a mi data_size (que ya es pot2), entonces sigo rompiendo
	while(partition_to_break->size != data_size){
		//divido partition en 2 mitades iguales y me quedo con la de la izquierd?, es lo mismo
		make_to_buddys(partition_to_break);

		Partition* left = malloc(sizeof(Partition));
		Partition* right = malloc(sizeof(Partition));

		// la de la izquierda arranca donde arrancaba la que rompi
		left->start = partition_to_break->start;
		// y tiene un size igual a la mitad de la que rompi
		left->size = partition_to_break->size/2;

		// la de la derecha arrnaca done empieza la izq + el size de la izq
		right->start = left->start + left->size;
		// y tiene un size igual al de la izquierda
		right->size = left->size;

		//reemplazo partition_to_break con left
		//list_replace(memory->partitions, indice_de_partition_to_break, left);

		// agrego la de la derecha al lado de la de la izq
		//list_add_in_index(memory->partitions, indice_de_partition_to_break, right);

		// libero partition_to_break??

		//ahora continuo viendo si tengo que romper con la de la izq
		partition_to_break = left;
	}

	// cuando ya no tengo que romper mas, devuelvo la uqe me quedo
	return partition_to_break;
	*/
}

// si nos confirman en el foro que siempre es "BF", podemos reutilizar el find para dinamicas y buddy
// por configuracion de archivo siempre vamos a tener "BF"
static Partition* _find_partition_buddys(potenciaDe2ABuscar) {

	/*
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
	*/
}

static Partition* _buddy_of(Partition* partition) {
	bool _inline_partition_at_position(Partition* partition) {
		return _partition_at_position(_buddy_address(partition), partition->start);
	}
	return list_find(memory->partitions, &_inline_partition_at_position);
}

static uintptr_t _buddy_address(Partition* partition) {
	return xor_pointer_and_int(partition->start, partition->size);
}

static bool _partition_at_position(uintptr_t position_to_compare, uintptr_t actual_position) {
	return position_to_compare == actual_position;
}

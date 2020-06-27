#include "buddy.h"

static bool _partition_at_position(uintptr_t, Partition*);
static uint32_t buddy_of(Partition*);

// 1) calculo la potencia de 2 mas cercana (hacia arriba) al valor de tamanioDeMiDato
//  2) busco una particion libre de tamanio = pot2
//   	   2.a) si la encuentro guardo el dato
//   	   2.b.i) si no la encuentro, busco la particion libre mas cercana a la pot2 calculada
//   	   2.b.ii) genero n buddys particionando hasta que me queda el tamanio del buddy = pot2 calculadass
//   3) si no encontre ninguna particion, elijo una victima y vuelvo a lanzar la busqueda
void save_to_cache_buddy_system(void* data, Message* message) {
	int desired_size = MAX_PARTITION_SIZE(next_power_of_2(message->data_size));

	Partition* partition = find_partition_buddy(desired_size);

	if(partition != NULL) {
		add_data_buddy(dato, partition);
	} else {
		choose_victim_buddy();
		consolidate_if_necessary();
		add_data_buddy(dato);
	}
}

Partition* get_buddy(Partition* partition) {
	bool _inline_partition_at_position(Partition* partition) {
		return _partition_at_position(buddy_of(partition), partition);
	}
	return list_find(memory->partitions, &_inline_partition_at_position);
}

// para compactar tiene que cumplir: el tipo esta libre + su buddy esta libre + su buddy es del mismo tamanio
// y hacer recursivo hasta que no puedo compactar ma
// para calcular la direccion del buddy uso el XOR con la direccion del tipo que estoy analizando
// XOR -> (miDireccionDeMemoria ^ miTamanioDeMemoria)
consolidate_if_necessary(){

}

/*
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

 */

/** PRIVATE FUNCTIONS **/

static uintptr_t buddy_of(Partition* partition) {
	return xor_pointer_and_int(partition->start, partition->size);
}

static bool _partition_at_position(uintptr_t position, Partition* partition) {
	return partition->start == position;
}

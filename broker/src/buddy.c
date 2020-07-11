#include "buddy.h"

static void _consolidate_buddy(Partition*);
static uint32_t _buddy_address(Partition*);
static Partition* _buddy_of(Partition*);
static Partition* _break_buddys(Partition*, uint32_t);

// 1) calculo la potencia de 2 mas cercana (hacia arriba) al valor de tamanioDeMiDato
//  2) busco una particion libre de tamanio = pot2
//   	   2.a) si la encuentro guardo el dato
//   	   2.b.i) si no la encuentro, busco la particion libre mas cercana a la pot2 calculada
//   	   2.b.ii) genero n buddys particionando hasta que me queda el tamanio del buddy = pot2 calculadass
//   3) si no encontre ninguna particion, elijo una victima y vuelvo a lanzar la busqueda
void save_to_cache_buddy_system(void* data, Message* message) {
	int desired_size = MAX_PARTITION_SIZE(next_power_of_2(message->data_size));

	// busco una particion libre
	t_link_element* target_element = find_partition(desired_size);

	// busco una particion libre y sino elimino alguna ocupada y consolido
	while(target_element == NULL) {
		Partition* victim = choose_victim();
		_consolidate_buddy(victim);
		target_element = find_partition(desired_size);
	}

	// rompo la particion libre elegida, hasta la minima pot2 posible
	Partition* target_partition = _break_buddys(target_element->data, message->data_size);
	target_partition->message = message;

	// guardo el data con el memcpy
	memcpy(target_partition->start, data, message->data_size);
}

/** PRIVATE FUNCTIONS **/

static void _consolidate_buddy(Partition* partition) {
	Partition* left_partition;
	Partition* right_partition;
	Partition* buddy = _buddy_of(partition);

	// si puedo consolidar, arranco, sino no hago nada
	while(buddy->free && buddy->size == partition->size) {

		// obtengo la particion mas al izquierda, seria la de menor posicion de memoria
		if(partition->start < buddy->start) {
			left_partition = partition;
			right_partition = buddy;
		} else {
			left_partition = buddy;
			right_partition = partition;
		}

		// creo una nueva particion sumando los tamanios y manteniendo el start
		Partition* replacement = create_partition(left_partition->start, left_partition->size + right_partition->size);

		// reemplazo la left partition por esta nueva que ya esta consolidada
		replace_partition_at(left_partition->start, replacement);

		// elimino el buddy/right_partition que sobro, ya que fue absorbido
		remove_partition_at(right_partition->start);

		// obtengo el buddy de la nueva particion consolidada
		buddy = _buddy_of(replacement);

		// reasigno para volver a chequear si puedo consolidar esta nueva particion mas grande
		partition = replacement;
	}
}

static Partition* _break_buddys(Partition* partition_to_break, uint32_t data_size) {
	// mientras la particion actual sea mayor a mi data_size (que ya es pot2), entonces sigo rompiendo
	while(partition_to_break->size != data_size){

		// izq -> size = a la que rompo dividido 2, start = start de la que rompo
		Partition* left_partition = create_partition(partition_to_break->start, HALF(partition_to_break->size));

		// der -> size = al de la iz, start = start izq + size
		Partition* right_partition = create_partition(left_partition->start, left_partition->size);

		// reemplazo partition_to_break con la nueva left_partition
		replace_partition_at(partition_to_break->start, left_partition);

		// agrego la de la derecha al lado de la de la izq
		add_partition_next_to(left_partition->start, right_partition);

		// ahora veo si tengo que seguir rompiendo la de la izq
		partition_to_break = left_partition;
	}

	// cuando ya no tengo que romper mas, devuelvo la que me quedo, que es del tamanio deseado
	return partition_to_break;
}

static Partition* _buddy_of(Partition* partition) {
	return find_partition_at(_buddy_address(partition));
}

static uintptr_t _buddy_address(Partition* partition) {
	return xor_pointer_and_int(partition->start, partition->size);
}

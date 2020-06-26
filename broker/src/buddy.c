#include "buddy.h"

void save_to_cache_buddy_system(void* data, Message* message) {

}

Partition* get_buddy(Partition* partition) {
	return list_find(memory->partitions, starts_with(buddy_of(partition)));
}

Partition* buddy_of(Partition* partition) {
	return xor(partition->start, partition->size);
}

uint32_t xor(uint32_t position, uint32_t size) {
	return (uint32_t) (unsigned) position ^ (unsigned) size;
}

uint32_t* starts_with(Partition* partition) {
	return partition->start;
}

/*

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

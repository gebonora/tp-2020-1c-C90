#include "dynamic.h"


/*
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
*/

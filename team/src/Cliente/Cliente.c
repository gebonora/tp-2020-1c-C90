#include "cliente/ClienteBroker.h"

uint32_t *VectorIDPendientesDelCatch;

void atenderCaughtRecibido(Caught* unCaught,uint32_t idMensaje){
	  /*Al recibir uno de los mismos deberá realizar los siguientes pasos:
	 *
	 * 1)Validar si el id de mensaje correlativo del mensaje corresponde a uno
	 *  pendiente de respuesta generado por la la instrucción CATCH_POKEMON antes descrita.
	 *  Si no corresponde a ninguno, ignorar el mensaje.
	 *
	 * 2) En caso que corresponda se deberá validar si el resultado del mensaje es afirmativo (se atrapó el Pokémon).
	 *   Si es así se debe asignar al entrenador bloqueado el Pokémon y habilitarlo a poder volver operar.
	 *  */
	int i = 0;
	int idMatch = 0;

	while(VectorIDPendientesDelCatch[i]!= NULL && idMatch == 0 ){
		if(VectorIDPendientesDelCatch[i] == idMensaje){
			idMatch = 1;
		}
		i++;
	}

	if(idMatch == 1){
		if(unCaught->result == 1){
			// asignar pokemon al entrenador y desbloquearlo.
		}
	}

}



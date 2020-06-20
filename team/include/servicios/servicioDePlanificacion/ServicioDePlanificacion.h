//
// Created by Alan Zhao on 29/05/2020.
//

#ifndef TEAM_SERVICIODEPLANIFICACION_H
#define TEAM_SERVICIODEPLANIFICACION_H

/**
 * Esta clase es conocedora de que implicancias a nivel planificacion tienen los eventos del sistema.
 *
 * Problemas:
 *  - Alguien tiene que persistir los ids, ¿Sera que puedo modelar los eventos y guardarlos ahi?
 *      Modelar eventos esperables y registrarlos en el event-handler.
 *  - ¿Esto deberia soportar la posibilidad de procesamiento asincronico?
 *      Si. Vamos a tener que armar una cola de trabajo y correr en un hilo aparte.
 *  - Llego hasta tener al entrenador bloqueado esperando la respuesta del CATCH, ¿ya hago return void?
 *      Registro en el ManejadorDeEventos que necesito un CAUGHT y retorno.
 *
 * Responsabilidades:
 *  - Tiene conocimiento de las reglas de movimiento de los entrenadores en los diferentes estados.
 *  - Colaborar con la resolucion de deadlock facilitando mover entrenadores y realizar intercambios.
 *
 * Ejemplo de flujo CATCH-CAUGHT:
 *      El servidor recibe el evento "LOCALIZED SQUIRTLE 4 4", por lo que debe notificar al servicio eso.
 *          Aclaracion:
 *              Si vienen N ejemplares pero necesito M < N, mando M entrenadores, NO mas!!!
 *              Si fallo la captura, puedo probar con los otros ejemplares en el mapa.
 *              Si vienen N ejemplares y necesito N, mando N entrenadores (el algoritmo dira quien va pri).
 *      El servicio resuelve qué entrenador/es está mas cerca y le dice al planificador que lo mande a READY.
 *      El algoritmo selecciona a uno y lo manda a EXEC
 *      El entrenador en EXEC gasta un ciclo de CPU (c/u dura N segundos) por cada desplazamiento hasta llegar.
 *      El proceso llama al cliente Broker y envia "CATCH SQUIRTLE 4 4".
 *      El cliente recibe un ID de correlatividad.
 *      El entrenador pasa a BLOCKED a la espera de un evento con el ID de correlatividad persistido.
 *      El event handler detecto que hubo un caught con un id requerido por un entrenador, le avisa al planificador.
 *      El entrenador se adjudica el pokemon (lo sacamos del mapa compartido) y se determina su futuro.
 *          Si cumple su objetivo, EXIT.
 *          Sino
 *              Si ya no puede seguir atrapando por capacidad maxima, BLOCKED (saldra por deadlock)
 *              Sino
 *                  Si hay pokemones para atrapar, vuelve a READY para que pueda seguir capturando.
 *                  Si no pasa a BLOCKED (saldra por evento LOCALIZED)
 *      El servicio queda a la expera de nuevos eventos, pero el planificador debe seguir trabajando con las capturas.
 */

typedef struct ServicioDePlanificacion {
    // Interfaz publica
} ServicioDePlanificacion;

#endif //TEAM_SERVICIODEPLANIFICACION_H

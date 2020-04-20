/*
 * server.c
 *
 *  Created on: 19 abr. 2020
 *      Author: GONZALO BONORA
 */


/* Se conecta con BROKER a las colas de mensajes.
 * Abre un socket de solo escucha para un gameboy
 *
 * Le llegan mensajes de cualquiera de los dos, procesa y trata de responder al BROKER, si no puede logea error y sigue.
 *
 * Entonces para broker necesito un server?
 * Para el gameboy sí, usar un server.
 *
 * Tengo que estar tratando de levantar la conexion si se cae -> un hilo para eso.
 * Tengo que atender en principio a un broker y un gamecard (o varios?).
 *
 */

/* Recibe NEW. Responde APPEARED.
 *
 * Recibe CATCH. Responde CAUGTH.
 *
 * Recibe GET. Responde LOCALIZED.
 */



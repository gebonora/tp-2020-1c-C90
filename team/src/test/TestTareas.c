//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDeTareas() {
	t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestTareas", 1, LOG_LEVEL_INFO);

	log_info(testLogger, "Testeando la generacion de una tarea de captura");

	log_info(testLogger, "Sobreescribimos temporalmente el servicio de metricas global");
	ServicioDeMetricas * servicioDeMetricasOriginal = servicioDeMetricasProcesoTeam;
	servicioDeMetricasProcesoTeam = ServicioDeMetricasConstructor.new();
	log_info(testLogger, "Sobreescribimos temporalmente el servicio de captura global");
	ServicioDeCaptura * servicioDeCapturaOriginal;
	servicioDeCapturaOriginal = servicioDeCapturaProcesoTeam;

	ServicioDeMetricas* metricasTest = ServicioDeMetricasConstructor.new();
	ServicioDeResolucionDeDeadlocks* deadlocksTest = ServicioDeResolucionDeDeadlocksConstructor.new(metricasTest);

	ServicioDePlanificacion * servicioDePlanificacion = ServicioDePlanificacionConstructor.new(metricasTest, deadlocksTest);
	Mapa mapita = MapaConstructor.new();
	servicioDeCapturaProcesoTeam = ServicioDeCapturaConstructor.new(mapita, servicioDePlanificacion);
	Entrenador * entrenador = EntrenadorConstructor.new("4|1", "A", "B");
	registrarEnMapaPosicionEntrenador(&mapita, entrenador);
	Coordinate posicionPokemon = (Coordinate ) { .pos_x = 5, .pos_y = 1 };
	TareaPlanificable * tareaCaptura = generarTareaDeCaptura(entrenador, "A", posicionPokemon);

	assert(list_size(tareaCaptura->instrucciones) == 2);
	assert(tareaCaptura->totalInstrucciones == 2);
	assert(tareaCaptura->cantidadInstruccionesEjecutadas(tareaCaptura)==0);
	assert(tareaCaptura->estado == PENDIENTE_DE_EJECUCION);

	Instruccion * instruccion1 = tareaCaptura->proximaInstruccion(tareaCaptura);
	assert(tareaCaptura->estado == EJECUTANDO);

	tareaCaptura->notificarEjecucion(tareaCaptura, 0);
	assert(tareaCaptura->estado == PENDIENTE_DE_EJECUCION);
	assert(tareaCaptura->cantidadInstruccionesEjecutadas(tareaCaptura)==1);

	Instruccion * instruccion2 = tareaCaptura->proximaInstruccion(tareaCaptura);
	assert(tareaCaptura->estado == EJECUTANDO);

	tareaCaptura->notificarEjecucion(tareaCaptura, 1);
	assert(tareaCaptura->estado == FINALIZADA);
	assert(tareaCaptura->cantidadInstruccionesEjecutadas(tareaCaptura)==2);
	assert(tareaCaptura->contadorDeInstrucciones + 1 == tareaCaptura->totalInstrucciones);

	assert(tareaCaptura->proximaInstruccion(tareaCaptura) == NULL);
	assert(tareaCaptura->estado == FINALIZADA);

	tareaCaptura->notificarEjecucion(tareaCaptura, 1203);
	assert(tareaCaptura->estado == ABORTADA);

	assert(tareaCaptura->proximaInstruccion(tareaCaptura) == NULL);
	assert(tareaCaptura->estado == ABORTADA);

	servicioDePlanificacion->destruir(servicioDePlanificacion);
	servicioDeMetricasProcesoTeam->destruir(servicioDeMetricasProcesoTeam);
	servicioDeCapturaProcesoTeam->destruir(servicioDeCapturaProcesoTeam);
	log_info(testLogger, "Restauramos el servicio de metricas global original");
	servicioDeMetricasProcesoTeam = servicioDeMetricasOriginal;
	log_info(testLogger, "Restauramos el servicio de captura global original");
	servicioDeCapturaProcesoTeam = servicioDeCapturaOriginal;
	tareaCaptura->destruir(tareaCaptura);
	entrenador->destruir(entrenador);
	mapita.destruir(&mapita);
	metricasTest->destruir(metricasTest);
	deadlocksTest->destruir(deadlocksTest);
	log_destroy(testLogger);
}

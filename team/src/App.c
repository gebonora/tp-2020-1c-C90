#include "app/App.h"

int main() {
	// Precalentamiento
	warmUp();
	log_info(INTERNAL_LOGGER, "========================= Inicio de ejecución ============================");

	// Config
	log_debug(INTERNAL_LOGGER, "Levantando configuracion...");
	servicioDeConfiguracion = ServicioDeConfiguracionConstructor.new(TEAM_CONFIG_FILE, TEAM_INTERNAL_LOG_FILE);

	// Logger obligatorio
	log_debug(INTERNAL_LOGGER, "Configurando logger obligatorio...");
	char * mandatoryLogPath = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, LOG_FILE);
	MANDATORY_LOGGER = log_create(mandatoryLogPath, "LogObligatorio", 1, LOG_LEVEL_INFO);

	// Setup de los componentes del sistema
	log_info(INTERNAL_LOGGER, "Inicializando componentes del sistema...");
	inicializarComponentesDelSistema();

	// Reservado a pruebas de integracion. Si logica de negocio rompe, explota el programa aca.
	if (CORRER_TESTS) {
		log_info(INTERNAL_LOGGER, "--------------------- Realizando pruebas de integracion... ---------------------");
		testDeIntegracion();
		log_info(INTERNAL_LOGGER, "--------------------- Pruebas de integracion finalizadas con exito -------------");
	}

	// Estado inicial del proceso Team
	log_info(INTERNAL_LOGGER, "Configurando el estado inicial del proceso Team...");
	configurarEstadoInicialProcesoTeam();

	// Server
	log_info(INTERNAL_LOGGER, "Levantando el server...");
	configurarServer();
	if (ESPERAR_OBJETIVO_GLOBAL) {
		atenderConexiones(); //TODO: Hacer apagable por si logramos cumplir con el objetivo.
	}

	// Por cada pokemon del objetivo global, enviar un GET [POKEMON].
	log_info(INTERNAL_LOGGER, "Solicitando la ubicacion de los pokemones objetivo para comenzar...");
	objetivoGlobalProcesoTeam.solicitarUbicacionPokemonesNecesitados(&objetivoGlobalProcesoTeam);

	// Cuando se complete el objetivo global, podremos finalizar el proceso y liberar los recursos.
	if (ESPERAR_OBJETIVO_GLOBAL) {
		log_info(INTERNAL_LOGGER, "Esperando a que se complete el objetivo global...");
		sem_wait(&semaforoObjetivoGlobalCompletado);
	}

	// Mostramos las metricas para comparar contra otros procesos team
	servicioDeMetricasProcesoTeam->imprimirMetricas(servicioDeMetricasProcesoTeam);

	// Liberacion
	log_info(INTERNAL_LOGGER, "Finalizando proceso Team...");
	liberarRecursos();
	log_info(INTERNAL_LOGGER, "============================ Fin de ejecución ============================");
	log_destroy(INTERNAL_LOGGER);
	return 0;
}

void warmUp() {
	INTERNAL_LOGGER = log_create(TEAM_INTERNAL_LOG_FILE, "Team.app", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
	mostrarTitulo(INTERNAL_LOGGER);
	if (SHOW_INTERNAL_CONSOLE) {
		log_warning(INTERNAL_LOGGER, "Logs de uso interno por consola activado. Recordar desactivarlo para la entrega.");
	}
	log_warning(INTERNAL_LOGGER, "Nivel de log interno configurado: %s", log_level_as_string(INTERNAL_LOG_LEVEL));
	if (CORRER_TESTS) {
		log_warning(INTERNAL_LOGGER, "Pruebas de integracion: ACTIVADAS");
	}
	if (!ESPERAR_OBJETIVO_GLOBAL) {
		log_warning(INTERNAL_LOGGER, "Objetivo global es necesario para finalizar el proceso: NO");
	}
	if (!ACTIVAR_RETARDO_CPU) {
		log_warning(INTERNAL_LOGGER, "Retardo de CPU: DESACTIVADO");
	}
	sem_init(&semaforoPokemone, 0, 0);
	sem_init(&semaforoReady, 0, 0);
	sem_init(&semaforoTrabajar2, 0, 0);
	sem_init(&semaforoDeadlock, 0, 0);
	sem_init(&semaforoCaptura, 0, 1);
	pthread_mutex_init(&mtxBlock, NULL);
	pthread_mutex_init(&mtxExec, NULL);
	pthread_mutex_init(&mtxExit, NULL);
	pthread_mutex_init(&mtxReady, NULL);
	pthread_mutex_init(&mtxNew, NULL);
	pthread_mutex_init(&messi,NULL);
}

void mostrarTitulo(t_log * logger) {
	char *title = "\n"
			"====================================\n"
			"████████╗███████╗ █████╗ ███╗   ███╗\n"
			"╚══██╔══╝██╔════╝██╔══██╗████╗ ████║\n"
			"   ██║   █████╗  ███████║██╔████╔██║\n"
			"   ██║   ██╔══╝  ██╔══██║██║╚██╔╝██║\n"
			"   ██║   ███████╗██║  ██║██║ ╚═╝ ██║\n"
			"   ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝\n"
			"====================================";

	log_info(logger, title);
}

void inicializarComponentesDelSistema() {
	srandom(time(NULL));
	pthread_mutex_init(&MTX_INTERNAL_LOG, NULL); //TODO: por ahi conviene moverlo a configurarServer()

	log_debug(INTERNAL_LOGGER, "Creando el cliente para comunicarse con el broker...");
	clienteBrokerV2ProcesoTeam = ClienteBrokerV2Constructor.new();

	log_debug(INTERNAL_LOGGER, "Preparando los algoritmos de planificacion...");
	inicializarAlgoritmosDePlanificacion();

	log_debug(INTERNAL_LOGGER, "Creando el mapa de coordenadas...");
	mapaProcesoTeam = MapaConstructor.new();

	log_debug(INTERNAL_LOGGER, "Creando el servicio de métricas...");
	servicioDeMetricasProcesoTeam = ServicioDeMetricasConstructor.new();

	log_debug(INTERNAL_LOGGER, "Creando el servicio de resolucion de deadlocks...");
	servicioDeResolucionDeDeadlocksProcesoTeam = ServicioDeResolucionDeDeadlocksConstructor.new(servicioDeMetricasProcesoTeam);

	log_debug(INTERNAL_LOGGER, "Creando el servicio de planificacion...");
	servicioDePlanificacionProcesoTeam = ServicioDePlanificacionConstructor.new(servicioDeMetricasProcesoTeam, servicioDeResolucionDeDeadlocksProcesoTeam);

	log_debug(INTERNAL_LOGGER, "Creando el servicio de captura...");
	servicioDeCapturaProcesoTeam = ServicioDeCapturaConstructor.new(mapaProcesoTeam);

	log_debug(INTERNAL_LOGGER, "Creando estructuras para eventos...");
	registradorDeEventosProcesoTeam = RegistradorDeEventosConstructor.new();
	manejadorDeEventosProcesoTeam = ManejadorDeEventosConstructor.new(servicioDeCapturaProcesoTeam, registradorDeEventosProcesoTeam);

	log_debug(INTERNAL_LOGGER, "Inicializando semanforo de fin de proceso...");
	sem_init(&semaforoObjetivoGlobalCompletado, 0, 0);
}

/**
 * Instanciar a los entrenadores a partir de la config - OK
 * Armar el objetivo global - OK
 * Armar los hilos de entrenador planificables - OK
 * Enviar a los entrenadores a new - OK
 * Finalmente, con el cliente broker a mano: Por cada pokemon del objetivo global, enviar un GET \[POKEMON\] - OK
 */
void configurarEstadoInicialProcesoTeam() {
	printf("entre a configurar\n");
	log_debug(INTERNAL_LOGGER, "Instanciando entrenadores y armando el equipo...");
	equipoProcesoTeam = crearEquipoPorConfiguracion();
	log_debug(INTERNAL_LOGGER, "Calculando el objetivo global...");
	objetivoGlobalProcesoTeam = ObjetivoGlobalConstructor.new(equipoProcesoTeam, clienteBrokerV2ProcesoTeam, registradorDeEventosProcesoTeam);
	manejadorDeEventosProcesoTeam->setObjetivoGlobal(manejadorDeEventosProcesoTeam, objetivoGlobalProcesoTeam);
	manejadorDeEventosProcesoTeam->registrarpokemonsNecesarios(manejadorDeEventosProcesoTeam);

	servicioDePlanificacionProcesoTeam->servicioDeCaptura = servicioDeCapturaProcesoTeam;
	servicioDePlanificacionProcesoTeam->objetivoGlobal = objetivoGlobalProcesoTeam;

	log_debug(INTERNAL_LOGGER, "Registrando al equipo en el mapa...");
	void registrarEquipo(Entrenador * entrenador) {
		registrarEnMapaPosicionEntrenador(&mapaProcesoTeam, entrenador);
	}
	list_iterate(equipoProcesoTeam, (void (*)(void *)) registrarEquipo);
	log_debug(INTERNAL_LOGGER, "Agregando equipo a la planificacion...");
	servicioDePlanificacionProcesoTeam->asignarEquipoAPlanificar(servicioDePlanificacionProcesoTeam, equipoProcesoTeam);
	sem_post(&servicioDePlanificacionProcesoTeam->semaforoEjecucionHabilitada);
	sem_post(&semaforoTrabajar2);
	sem_post(&servicioDePlanificacionProcesoTeam->semaforoEjecucionHabilitada3);
}

void liberarRecursos() {
	// Server
	log_debug(INTERNAL_LOGGER, "Apagando server...");
	apagarServer(); //Cierra hilos server y elimina config server con sus variables.

	// Logger
	log_debug(INTERNAL_LOGGER, "Liberando logger obligatorio...");
	log_destroy(MANDATORY_LOGGER);

	// Proceso Team
	log_debug(INTERNAL_LOGGER, "Liberando participantes del proceso Team...");
	destruirEquipo(equipoProcesoTeam);
	objetivoGlobalProcesoTeam.destruirObjetivoGlobal(&objetivoGlobalProcesoTeam);

	// Servicios
	log_debug(INTERNAL_LOGGER, "Liberando servicios...");
	servicioDeConfiguracion.destruir(&servicioDeConfiguracion);
	servicioDeCapturaProcesoTeam->destruir(servicioDeCapturaProcesoTeam);
	servicioDePlanificacionProcesoTeam->destruir(servicioDePlanificacionProcesoTeam);
	servicioDeResolucionDeDeadlocksProcesoTeam->destruir(servicioDeResolucionDeDeadlocksProcesoTeam);
	servicioDeMetricasProcesoTeam->destruir(servicioDeMetricasProcesoTeam);

	// Componentes
	log_debug(INTERNAL_LOGGER, "Liberando componentes del sistema...");
	manejadorDeEventosProcesoTeam->destruir(manejadorDeEventosProcesoTeam);
	pthread_mutex_destroy(&MTX_INTERNAL_LOG);
	sem_destroy(&semaforoObjetivoGlobalCompletado);
	registradorDeEventosProcesoTeam->destruir(registradorDeEventosProcesoTeam);
	destruirAlgoritmosDePlanificacion();
	mapaProcesoTeam.destruir(&mapaProcesoTeam);
	clienteBrokerV2ProcesoTeam->destruir(clienteBrokerV2ProcesoTeam);
}

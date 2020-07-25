# CATCH - CAUGHT

Este flujo resulta necesario para concretar el objetivo global mediante la captura de los pokemones.

## Detonante

El servidor recibe el evento "LOCALIZED SQUIRTLE 4 4", por lo que debe llama al ManejadorDeEventos para avisarle
de la existencia de un nuevo pokemon. 
Esto implica registrar al pokemon en el _Mapa_ y notificarle al _ServicioDePlanificacion_ 
para que envíe a un _Entrenador_ a capturarlo.

### Manejador de eventos
Filtra los mensajes por id de correlatividad y por repetido
en el caso de que me lleguen 2 localized de la misma especie pokemon.

Si llega otro LOCALIZED SQUIRTLE, no le tengo que dar bola. Solo uno por especie, y si llego un caught, el localized  
de esa especie tambien debe ser descartado.  

Si bien me pueden llegar mas ubicaciones en el localized, la planificación toma la primera.

El resto de los pokemones se guardan en el mapa por si falla la captura.

Para facilitar la tarea,  tanto appeared como localized seran tratados de de la misma manera por el sistema como un 
evento "nuevoPokemon".

#### Aclaracion:
> - Si vienen N ejemplares pero necesito M < N, mando M entrenadores, ¡¡¡NO mas!!!  
> - Si fallo la captura, puedo probar con los otros ejemplares en el mapa.
> - Si vienen N ejemplares y necesito N, mando N entrenadores (el algoritmo dira quien va a **EXEC**). 

## Servicio de planificación

Esta clase será la encargada de generarle trabajo al planificador, manejando la logica de negocio y dejándole
solo las responsabilidades propias de la planificación, como por ejemplo, mover elementos entre colas,
llamar a los algoritmos, etc.  

Tanto el servicio de planificacion como el planificador interactuan con los entrenadores indirectamente
a traves del wrapper _HiloEntrenadorPlanificable_. Cuando hablamos de entrenadores, nos referimos al planificable.

### Procesamiento
Como pueden llegar varios eventos mientras se ejecuta, o bien no llegar ninguno, necesito correrlo en un hilo aparte.

Para procesar el trabajo asincrónicamente, disponemos de una cola de trabajo, que utilizaremos como dicta el modelo 
**Productor-Consumidor** (el manejador de eventos producirá, y el servicio consumirá).

Mientras haya algo que hacer, con la ayuda de un **semáforo** habilitará el consumo de trabajo.

### Algo para hacer

Consideramos algo para hacer a la presencia de trabajo en la cola, y que todavia haya pokemones en el mapa que necesite 
para completar el objetivo global.

### Consumiendo el trabajo LOCALIZED

El _ServicioDePlanificacion_ determina que hay que procesar un **LOCALIZED**, asi que lo primero que tenemos que hacer
es ver que entrenador/es estan mas cerca del pokemon a capturar (puede haber empate) y mandarlos a **READY**.

#### Aclaración
> La transicion de **NEW**/**BLOCKED** a **READY** se hace por cercania (solo 1). En caso de empate, el primero.
> Por regla general, voy a querer planificar al entrenador mas cercano libre, eso quiere decir que si tengo uno en 
> BLOCKED a la espera de pokemones, y otro en en NEW (poco problable que ocurra), 
> tengo que mandar a **READY** al mas cercano.


Nos valemos de la ayuda del _Mapa_ para calcular distancias.

Con la lista de entrenadores en READY (pueden entrar entrenadores por otras razones, como necesitar ir a la posicion 
de otro entrenador para intercambiar pokemones), le decimos al _Planificador_ 
que seleccione a uno para mandarlo a **EXEC**.

Esta clase notifica del su nuevo estado al _HiloEntrenadorPlanificable_,
por lo que este va a poder levantar su semaforo, dando lugar al ciclo de ejecucion del entrenador.

## Planificador

Maneja las colas de los distintos estados. Al principio tiene a todos los entrenadores en **NEW**.

No interactua con los objetos Entrenador directamente, sino que lo hace mediante el wrapper HiloEntrenadorPlanificable,
ya que es necesario que el entrenador corra en un hilo y además sea activable con el signal que le proporcionara
el pase a **EXEC**.

TODO: Definir la arquitectura para que la llegada de eventos corra separado de la seleccion de los varios entrenadores.
Osea, en mi cola de trabajo puedo dejar un localized, que si lo proceso me va a dejar 1 entrenador en READY.
A su vez un entrenador puede querer ir a intercambiar, ambos deben competir por llegar a EXEC.

> Idea 1
> * Tal vez mientras tengo en exec a un entrenador, me lleno mientras de cosas para hacer, las puedo procesar en bache, y
> finalmente volver a **armar mi conjunto ready**.  

> Idea 2
> * Sigo teniendo una cola de tareas a planificar, la idea es que no haga solo un pop, sino que me lleve todos los que 
> haya en ese momento para asi generar todos los candidatos a **READY**, de esta forma pueden competir uno que 
> vaya a cazar y otro que quiera ir a resolver un deadlock. 
> * Como solo uno va a ganar, necesito que los que perdieron sigan ahi.
> * La clave de esta idea es que haya un "while (algo en ready)" corre planificacion.
> * En consecuencia, necesito que mi _HiloEntrenadorPlanificable_ pueda guardar la causa por la que fue planificado, asi
> lo puedo seleecionar en otra vuelta, tambien de paso para que sepa que hacer cuando le confirmen la ejecución.

### Algoritmo de planificacion

Con la ayuda del algoritmo de planificación configurado, vamos a seleccionar a uno de los entrenadores en **READY**
y mandarlo a **EXEC**.

> Parte de la responsabilidad del algoritmo es realizar las tareas posteriores a mandar a ejecutar. Por ejemplo 
> el Round Robin si no terminaste, te manda a READY nuevamente. SJF con desalojo también lo hace. Ambos replanifican.

Detallaremos lo que ocurre en el caso de que sea un entrenador que va a capturar a un pokemon.

### Ciclos de CPU

El entrenador debe moverse hacia la posicion del pokemon. Esto conlleva gastar un **ciclo de CPU** (duran 1 segundo)
por cada pasito hasta llegar a destino.

Dependiendo el algoritmo, es probable que necesitemos avisarle al _HiloEntrenadorPlanificable_ de cuanto GAS tiene.

## Entrenador

Esta clase tiene conocimiento de como efectuar las acciones propias de un entrenador.  

TODO: Averiguar si me pueden cortar la ejecucion mientras estoy haciendo algo (RR?).
> Posiblemente tenga que modelar el hilo de tal forma que sabiendo cuantos ciclos se le permite correr
> pueda realizar esas acciones atomicamente. Lo importante es que termine consistente como 
> para que se pueda retomar en una proxima planificación.

> Idea 1
> * Solo el movimiento y el envio de mensaje al Broker ratonean ciclos de CPU.
> * Ambas son acciones atomicas.
> * Sabiendo eso puedo garantizar que el entrenador va a quedar en un estado consistente si al menos
> le doy 1 ciclo de CPU.
> * Para concretar esta idea necesito separar las acciones que consumen CPU del resto.
> * Voy a manejar la idea de una "tarea parcialmente completada", asi puedo retomar desde donde la dejé.
> * Al tener guardada como atributo su tarea, puede dejar a un costado anotado donde quedó, para retomar de ahí.
> * La tarea pueden ser modelada como una lista de cosas para hacer.
> * Usaria funciones aplicables parcialmente #jojoó_pdp_feelings.
> * La cantidad de ciclos de cpu determina cuanto avanzo el puntero de instrucciones y ademas si lo guardo
> puedo trackear cuanto me falta.
> * Un entrenador con tarea asignada no puede calificar para otras cosas, a.k.a no esta libre.

### Cliente Broker

Necesitamos hacerle llegar al Broker que queremos atrapar al pokemon.  

El entrenador dispondrá del cliente broker para realizar el envio del mensaje CATCH.

Enviamos un "CATCH SQUIRTLE 4 4" desde el _Entrenador_.

Recibimos el id de correlatividad.

### Bloqueo por espera de evento

Con el id de correlatividad en mano, **registro** en el _ManejadorDeEventos_ que espero un CAUGHT con ese id.

Desde el _ServicioDePlanificacion_ mando al entrenador a **BLOCKED**, waiteando su semaforo para que no siga ejecutando.

## Llega el evento mas esperado, el CAUGHT

El event handler detectó que hubo un **CAUGHT** con un id requerido por un entrenador. 

Se le avisa al ServicioDePlanificacion se determine su futuro.

### Los caminos de la vida

De acuerdo al resultado del **CAUGHT** y otras variables, podemos decidir que hacer con el entrenador.

Si llegó CAUGHT SI:

* Volamos al pokemon del mapa y registramos la captura en el entrenador.
* Evaluamos el estado del entrenador:
    - Si su objetivo fue completado, le decimos al planificador que lo mande a **EXIT**. Fin.
    - Sino,
        - Si ya no puede seguir atrapando pokemones por ya tener su capacidad maxima repleta,
            lo mandamos a **BLOCKED** a la espera de un **DEADLOCK**.
        - Sino,
            - Si hay pokemones para atrapar, hay que hacer que este entrenador pueda formar parte del grupo de seleccion.
            - Sino, pasa a **BLOCKED** a la espera del evento **LOCALIZED** (hay que registrarlo).

Si llegó CAUGHT NO:

Por suerte los localized pueden traen varios pokemones, por lo que puedo probar suerte con otro en el mapa.
- Muevo el entrenador a READY para que pueda ser seleccionado en futuras planificaciones.

## Finalmente

El servicio queda a la expera de nuevos eventos, pero el planificador debe seguir trabajando con las capturas.

Si todavia hay pokemones en el mapa producto del localized, no tengo que esperar a otro evento.
Una de las condiciones del servicio de planificador para correr además de que haya trabajo en cola, 
es ver si hay pokemones disponibles que former parte del objetivo global.

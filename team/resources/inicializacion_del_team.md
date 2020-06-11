# Inicializacion del Team

### Intro

Al levantar la app, requerimos dejar al sistema en un estado listo para recibir mensajes del broker.  

* Tener todos los entes que formen parte del flujo de planificacion debidamente creados
* Levantar el servidor
* Instanciar a los entrenadores a partir de la config
* Armar el objetivo global
* Crear el mapa y setear la posicion de cada entrenador
* Armar los hilos de entrenador planificables
* Enviar a los entrenadores a new
* Finalmente, con el cliente broker a mano: Por cada pokemon del objetivo global, enviar un GET \[POKEMON\]
    

## Sistema arriba

Verificar que esten iniciados:
   * Servicio de configuracion
   * ServicioDeSemaforos
   * Manejador de eventos
   * Servicio de planificacion
   * Planificador
   * Servidor
   
## Crear entrenadores

En base a la config tendremos que armar a los entrenadores.

Un _Entrenador_ tiene una posicion inicial y dos listas de pokemon: atrapados y objetivo.

La lista de objetivos es inmutable, mientras que la lista de atrapados puede mutar por intercambios o capturas.

Se obtendrá una **lista** de entrenadores.

## Objetivo global

El _ObjetivoGlobal_ va a estar modelado con una clase que se construye
a partir de la información de todos los entrenadores.

Nos va a ser util para responder preguntas como qué especies pokemon necesitamos.

## Mapa

Vamos a optar por que cada clase "posicionable" delegue en el mapa la responsabilidad de saber donde está.  

Para ello, en el constructor de cada "posicionable" deberemos llamar al mapa para registrar en el sistema la posicion.  

El _Mapa_ guardará en su estado interno todas las posiciones, tanto de entrenadores como de pokemones, utilizando un UUID.  

El identificador unico o UUID debera ser persistido por la clase "posicionable".

## Hilos entrenadores

Los entrenadores manejan el concepto de ejecución planificada. Esto quiere decir que van a haber momentos en concreto
donde puedan llevar a cabo sus acciones.  

Decidimos separar la logica de planificacion de las responsabilidades del entrenador utilizando un wrapper.  

Existe la clase _Entrenador**Planificable**_, cuyo objetivo es manejar un hilo al que "pausaremos" con semaforos.
Dentro de la misma se podrá llamar a los metodos del entrenador al que envuelve.

Por cada entrenador existira un _EntrenadorPlanificable_.  
Cuando el planificador decida mandar al entrenador a **NEW**, estara interactuando con un _EntrenadorPlanificable_, 
enviando por detrás un **signal** al semaforo correspondiente.  

Dentro del entrenador habrá un ciclo while habilitado por el semáforo. Adentro del mismo se chequeará si hay
"cosas para hacer" (ver mas detalles en la documentación de la arquitectura entrenador) y mientras no haya un **wait**
se ejecutarán.

Es el _EntrenadorPlanificable_ quien se mueve entre los estados de planificacion.

## Entrenadores a NEW

Una vez creados los _EntrenadorPlanificable_, se los insertará en NEW con la ayuda del _ServicioDePlanificacion_,
una clase que sabe como interactuar con el _Planificador_ en base a las implicancias de los eventos del sistema.


## GET Pokemon

Es una precondición que el servidor esté levantado y atento a los mensajes del broker.  

Con nuestro _ObjetivoGlobal_ podremos obtener una lista de las especies de pokemon necesarias.

Por cada especie, hay que mandar un GET al broker.  

Nos valdremos de un _ClienteBroker_ para enviar todos los mensajes del proceso Team.



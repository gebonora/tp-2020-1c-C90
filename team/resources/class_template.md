# Como hacer clases en C (?

A continuacion se detalla como armar una pseudo-clase en C llamada EjemploPiola.
Es necesario crear un header con las definiciones y un .c con el código de la implementacion.

## EjemploPiola.h

```C
typedef struct EjemploPiola {
    // Propiedades-Atributos de la clase
    t_log *logger;
    int atributo;
    char * identificador;
    // Metodos de clase: El primer argumento de todos los metodos de clase es un puntero a si mismo.
    // Metodos privados
    void (*foo)(struct EjemploPiola *this, bool argumento);
    // Interfaz publica
    int (*bar)(struct EjemploPiola *this);
    void (*destruir)(struct EjemploPiola *this); // Este metodo es necesario si se alocó memoria
} EjemploPiola;

extern const struct EjemploPiolaClass {
    EjemploPiola (*new)(char * argConstruccion); // Si queremos un puntero agregar el * - EjemploPiola *(*new)(...);
} EjemploPiolaConstructor;

EjemploPiola ejemploPiola; // Agregar Si es que quiero hacerlo global (ej. servicios)s

```

## EjemploPiola.c

Primero escribo los metodos (tanto publicos como privados)

```C
static void foo(EjemploPiola *this, bool argumento) {
    if (argumento)
        log_info(this->logger, "foo");
}


static int bar(EjemploPiola *this) {
    return this->atributo * 302;
}

static void destruir(EjempoPiola *this) {
    // hacer free de lo que haya alocado para crear esta clase
    log_destroy(this->logger);
    free(this->atributoQueRequirioMalloc);
    // si la instanciacion devuelve un puntero (opc. B), liberarlo.
    free(this);
}
```

Luego, dependiendo de si el constructor devuelve un puntero o no, debo utilizar una de estas opciones.

### Opcion A - No aloca memoria.

```C
static EjemploPiola new(char * argConstruccion) {
    return (EjemploPiola) {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "EjemploPiola", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO),
            .atributo = 100,
            .identificador = argConstruccion,
            .identificador = "Ejemplo",
            // Listar los metodos en el orden en el que fueron declarados en el .h (MUCHO MUY IMPORTANTE)
            &foo,
            &bar,
            &destruir
    };
}
```

### Opcion B - Devuelve un puntero, recordar hacerle free.

```C
static EjemploPiola *new(char * argConstruccion) {
    EjemploPiola *ejemploPiola = malloc(sizeof(EjemploPiola));

    // El orden de las asignaciones no importa
    ejemploPiola->identificador = argConstruccion;
    ejemploPiola->atributo = 100;
    ejemploPiola->logger = logger = log_create(TEAM_INTERNAL_LOG_FILE, "EjemploPiola", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    // Interfaz publica
    ejemploPiola->foo = &foo;
    ejemploPiola->destruir = &destruir;
    // Metodos privados
    ejemploPiola->bar = &bar;

    return ejemploPiola;
}
```

Por ultimo, escribir esto, necesario para instanciar la clase donde la necesite con ConstructorX.new(...)

```C
const struct EjemploPiolaClass EjemploPiolaConstructor = {.new=&new};
```
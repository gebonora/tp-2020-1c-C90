# UTN Sistemas Operativos 1C 2020
# Proyecto Delibird - Grupo C90

## Clonar el proyecto en la VM

```bash
git clone https://github.com/sisoputnfrba/tp-2020-1c-C90.git
```

Luego nos posicionamos en el root del proyecto, donde se encuentra la mayoría de los scripts

```bash
cp tp-2020-1c-C90
```

## Instalar CMake y las commons

```bash
./delibird.sh -i
```

## Compilar todos los módulos

```bash
./delibird.sh
```

## Configurar módulos para pruebas integrales

### Primera Prueba

```bash
./configuracion-entrega.sh IP_BROKER IP_GAMECARD IP_TEAM gamecard 1ra
```

### Segunda Prueba

```bash
./configuracion-entrega.sh IP_BROKER IP_GAMECARD IP_TEAM gamecard 2da
```

## Configurar módulos para pruebas individuales

### Team

```bash
cd team/deploy/config
cp archivo_prueba_puntual.config team.config (ej: cp fifo.config team.config)
editar dentro de team.config IP_GAMEBOY_TEAM por la IP del team de la vm
```

### Broker

```bash
cd broker/config
cp archivo.config broker.config (ej: cp buddy_lru.config team.config)
editar dentro de broker.config IP_BROKER por la IP del broker de la vm
```

## Editar archivos desde terminal

### Con Vim

```bash
vim nombre_archivo.config
apretamos "i" debería aparecer el texto "INSERTAR" abajo a la izquierda
editamos la línea que queremos
apretamos escape "Esc"
escribimos ":wq" y apretamos enter
```

### Con Nano

```bash
nano nombre_archivo.config
editamos la línea que queremos
apretamos "Ctrl" + "X"
nos pregunta si queremos guardar, apretamos "Y"
apretamos enter
```
## Matar procesos que se encuentran ejecutando

### Matar todos los módulos

```bash
nos posicionamos en el root del proyecto
./kill.sh all
```

### Matar módulo individual

```bash
nos posicionamos en el root del proyecto
./kill.sh ${módulo}
```

> módulo puede tomar los siguientes valores: broker|gamecard|team|team2

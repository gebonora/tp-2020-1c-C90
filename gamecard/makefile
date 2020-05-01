# Directorios e implementacion de funciones propias
IDIR = include
SDIR = src
ODIR = obj
LIBSDIR = ../libs/

define convert_obj_to_src_path
	$(subst obj/,src/,$(patsubst %.o, %.c, $(1)))
endef

# Template tuneado

PROJECT = gameCard
TARGET = $(PROJECT) # Definimos la tarea que va a ejecutar make por defecto Y el nombre del binario final....
LIBS = -lreadline -lcommons -pthread # Incluimos las bibliotecas que necesitamos
CC = gcc # Vamos a usar gcc como compilador, porque nos encanta!
CFLAGS = -g -Wall # Compilamos con los flags -g (con simbolos de debug) y -Wall (que nos muestre todos los warnings)
CFLAGS += -I$(IDIR) -I$(LIBSDIR)

.PHONY: default all clean

default: create-dirs clean $(TARGET)
all: default

HEADERS = $(shell find ./include -name "*.h")
SOURCES = $(shell find ./src -name "*.c")
SOURCES += $(shell find ../libs -name "*.c")
OBJECTS = $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(SOURCES))

%.o:
	$(CC) $(CFLAGS) -c $(call convert_obj_to_src_path, $@) -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS) $(SOURCES) $(HEADERS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

clean:
	-rm -f $(ODIR)/*.o
	-rm -f $(TARGET)

check:
	@echo ------begin---------
	@echo objects: $(OBJECTS)
	@echo ---------------
	@echo sources: $(SOURCES)
	@echo ---------------
	@echo headers: $(HEADERS)
	@echo ------end-----------

# Creacion de directorio
create-dirs:
	mkdir -p logs
	mkdir -p obj
Library delibird.

Instalar:
 	sh ./install.txt

Desinstalar:
	sh ./uninstall.txt

Nota: no borrar "install_manifest.txt", es necesario para desinstalar luego.

En el Makefile agregarlo a libs linkeadas como "-ldelibird"

En los .h incluir como:

	#include "delibird/elPuntoHQueQuiero.h"

O bien:

	#include <delibird/elPuntoHQueQuiero.h>

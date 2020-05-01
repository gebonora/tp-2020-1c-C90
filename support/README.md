Library support.

Instalar:
 	sh ./install.txt

Desinstalar:
	sh ./uninstall.txt

Nota: no borrar "install_manifest.txt", es necesario para desinstalar luego.

En el Makefile agregarlo a libs linkeadas como "-lsupport"

En los .h incluir como:

	#include "support/elPuntoHQueQuiero.h"

O bien:

	#include <support/elPuntoHQueQuiero.h>

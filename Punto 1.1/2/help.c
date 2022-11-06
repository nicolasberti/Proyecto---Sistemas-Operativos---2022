//help
#include "constantes.h"
int main (int argc, char *argv[])
{ // OK
			char *ayuda = "~$ Comandos disponibles\n- Crear un directorio: mkdir [ubicacion] [nombre]\n- Remover un directorio (debe estar vacio): rmdir [ubicacion] [nombre]\n- Crear un archivo: mkfile [ubicacion] [nombre.formato]\n- Listar el contenido de un directorio: ldir [ubicacion]\n- Listar el contenido de un archivo: lfile [ubicacion]\nIMPORTANTE! Las ubicaciones deben terminar con /";
			printf("%s\n", ayuda);
			exit(0);
      }
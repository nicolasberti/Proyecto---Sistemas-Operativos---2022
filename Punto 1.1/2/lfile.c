//lfile
#include "constantes.h"
int main (int argc, char *argv[]){
int retorno = 0;
FILE* fichero;
			char ubicacion[MAXIMO_CHAR*2];
			char contenido[MAXIMO_CHAR*2];
			strcpy(ubicacion, "");
			strcat(ubicacion, argv[1]); // el primer parametro tiene que terminar con /
			strcat(ubicacion, argv[2]);
			fichero = fopen(ubicacion, "rt");
			if(fichero == NULL) { retorno = -1; }
			else {
			  while(fgets (contenido, MAXIMO_CHAR*2, fichero))
				printf("%s", contenido);
			  fclose(fichero);
			}
			exit(retorno);
      } 
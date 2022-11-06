//mkfile
#include "constantes.h"
int main (int argc, char *argv[]){
	int retorno = 0;
			FILE *fp;
			char ubicacion[MAXIMO_CHAR*2];
			strcpy(ubicacion, "");
			strcat(ubicacion, argv[1]); // el primer parametro tiene que terminar con /
			strcat(ubicacion, argv[2]);
			fp = fopen(ubicacion, "w");
			if(fp == NULL) 
				retorno = -1;
			else 
				fclose(fp); 
			exit(retorno);
}
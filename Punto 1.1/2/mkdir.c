//mkdir
#include "constantes.h"
int main (int argc, char *argv[]){
    int retorno = 0;
    char ubicacion[MAXIMO_CHAR*2];
    strcpy(ubicacion, "");
    strcat(ubicacion, argv[1]); // el primer parametro tiene que terminar con /
    strcat(ubicacion, argv[2]);
    retorno = mkdir(ubicacion, 0777);
    exit(retorno);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ftw.h>
#include <dirent.h>

/* Errores */
#define ERROR_PIPE					    	1
#define ERROR_FORK              			2

/* Errores pero para imprimir por consola */
#define ERROR_LIMITE 				     	printf("~$ Limite de comandos");
#define	ERROR_DECOMANDO				     	printf("~$ El comando no pudo ser ejecutado");
#define ERROR_INVALIDO 				     	printf("~$ Comando invalido");

#define CANTIDAD_COMANDOS					6

#define	PARAMETROS_MAXIMOS			     	2
#define	TOTAL_INDICES				        PARAMETROS_MAXIMOS+2
#define MAXIMO_CHAR					      	256

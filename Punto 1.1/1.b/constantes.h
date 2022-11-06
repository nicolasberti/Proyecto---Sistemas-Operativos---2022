#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define ERROR_FORK 3

#define KEY_cola ((key_t) (1234))

struct basuramsg {
	long tipo;
  	char msg;
	/*
		Tipos de basura
		1: Plástico sin clasificar
		2: Vidrio sin clasificar
		3: Aluminio sin clasificar
		4: Carton sin clasificar

   		 5: Plástico clasificado
    		6: Vidrio clasificado
    		7: Aluminio clasificado
    		8: Cartón clasificado

	*/
};
#include "constantes.h"

int main(int argc, char *argv[]){
    int i = atoi(argv[0]);
    int qid_cola = msgget(KEY_cola, 0666);
    if(qid_cola == -1)
        printf("CLA: Error recuperando cola en reciclador%i\n", qid_cola);
    while(1){
        struct basuramsg m;
    	if(msgrcv(qid_cola, &m, (sizeof(struct basuramsg) - sizeof(long)), -4, 0) == -1)
            printf("\nCLA: Error al leer de la cola.\n");
        switch(m.tipo){
            case 1: { printf("\n[Contador %i] Clasificador recibe Plastico\n", i); m.tipo = 5;
						if(msgsnd(qid_cola, &m, ( sizeof(struct basuramsg) - sizeof(long)), 0) == -1)
							printf("\nCLA: Error al enviar mensaje en writer.\n");	break; } //Plastico
            case 2: { printf("\n[Contador %i] Clasificador recibe Vidrio\n", i); m.tipo = 6;
						if(msgsnd(qid_cola, &m, ( sizeof(struct basuramsg) - sizeof(long)), 0) == -1)
							printf("\nCLA: Error al enviar mensaje en writer.\n");	break; } //Vidrio
            case 3: { printf("\n[Contador %i] Clasificador recibe Aluminio\n", i); m.tipo = 7;
						if(msgsnd(qid_cola, &m, ( sizeof(struct basuramsg) - sizeof(long)), 0) == -1)
							printf("\nCLA: Error al enviar mensaje en writer.\n");	break; } //Aluminio
            case 4: { printf("\n[Contador %i] Clasificador recibe Carton\n", i); m.tipo = 8;
						if(msgsnd(qid_cola, &m, ( sizeof(struct basuramsg) - sizeof(long)), 0) == -1)
							printf("\nCLA: Error al enviar mensaje en writer.\n");	break; } //Carton
        }
    sleep(5);
	}
    exit(0);
}

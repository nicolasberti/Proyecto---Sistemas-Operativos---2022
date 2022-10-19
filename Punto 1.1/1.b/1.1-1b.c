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


// Función para generar basura random.
char generarBasura(){
    char basura;
    int r = rand() % 4;
        switch(r){
            case 0: { basura = 'P'; break; }
            case 1: { basura = 'C'; break; }
            case 2: { basura = 'A'; break; }
            case 3: { basura = 'V'; break; }
        }
    return basura;
}

int main(){

    pid_t pid;
    int qid_cola = msgget(KEY_cola, IPC_CREAT|0666);
    if(qid_cola == -1)
        printf("\nError en creacion de cola principal %i\n", qid_cola);

  // Código de los recolectores
    for(int i = 0; i < 3; i++){
        pid = fork();
        if(pid < 0) exit(ERROR_FORK);
        else if(pid == 0) { // Código del reciclador
         srand( time(NULL)+(i*52*37*96)); // Semilla del rand, hago que sean distintas
            while(1){
                char basura = generarBasura();
                int reciclado;
                switch(basura){
                    case 'P': { reciclado = 1; break; }
                    case 'V': { reciclado = 2; break; }
                    case 'A': { reciclado = 3; break; }
                    case 'C': { reciclado = 4 ; break; }
                }
								struct basuramsg m;
								m.tipo = reciclado;
              	m.msg = basura;
              	int qid_cola = msgget(KEY_cola, 0666);
              	if(qid_cola == -1)
                  printf("Error recuperando cola en recolector %i\n", qid_cola);
              
								if(msgsnd(qid_cola, &m, ( sizeof(struct basuramsg) - sizeof(long)), 0) == -1)
									printf("\nError al enviar mensaje en writer.\n");
								printf("\nRecolectar %i mete: %c\n", i, basura);
                sleep(5); // Producen cada 5 segundo, para no sobrecargar la shell
            }
         	exit(0);
        } else continue;
    }

    for(int i = 0; i < 2; i++){
        pid = fork();
        if(pid < 0) exit(ERROR_FORK);
        else if(pid == 0) { // Código del reciclador
            while(1){
              			struct basuramsg m;
              			int qid_cola = msgget(KEY_cola, 0666);
              			if(qid_cola == -1)
                  		printf("Error recuperando cola en reciclador%i\n", qid_cola);
    								if(msgrcv(qid_cola, &m, (sizeof(struct basuramsg) - sizeof(long)), -4, 0) == -1)
                      	printf("\nError al leer de la cola.\n");
                    switch(m.tipo){
                            case 1: { printf("\n[Contador %i] Clasificador recibe Plastico\n", i); m.tipo = 5;
																			if(msgsnd(qid_cola, &m, ( sizeof(struct basuramsg) - sizeof(long)), 0) == -1)
																				printf("\nError al enviar mensaje en writer.\n");	break; } //Plastico
                            case 2: { printf("\n[Contador %i] Clasificador recibe Vidrio\n", i); m.tipo = 6;
																			if(msgsnd(qid_cola, &m, ( sizeof(struct basuramsg) - sizeof(long)), 0) == -1)
																				printf("\nError al enviar mensaje en writer.\n");	break; } //Vidrio
                            case 3: { printf("\n[Contador %i] Clasificador recibe Aluminio\n", i); m.tipo = 7;
																			if(msgsnd(qid_cola, &m, ( sizeof(struct basuramsg) - sizeof(long)), 0) == -1)
																				printf("\nError al enviar mensaje en writer.\n");	break; } //Aluminio
                            case 4: { printf("\n[Contador %i] Clasificador recibe Carton\n", i); m.tipo = 8;
																			if(msgsnd(qid_cola, &m, ( sizeof(struct basuramsg) - sizeof(long)), 0) == -1)
																				printf("\nError al enviar mensaje en writer.\n");	break; } //Carton
                        }
                    sleep(5);
            }
            exit(0);
        } else continue;
    }



for(int i = 0; i < 4; i++){
    pid = fork();
    if(pid < 0) exit(ERROR_FORK);
    else if(pid == 0) {

        int contador;
        int j = i;
        int reciclarOtro = 0;
        srand( time(NULL)+(i*123*54+123*5));
        int qid_cola = msgget(KEY_cola, 0666);
      	if(qid_cola == -1)
                  printf("Error recuperando cola en reciclador%i\n", qid_cola);
      	
            while(1){
                switch(j){
                        case 0: { // Vidrio clasificado = 6
                          struct basuramsg m;
													if(msgrcv(qid_cola, &m, (sizeof(struct basuramsg) - sizeof(long)), 6, IPC_NOWAIT) > -1){
                                if(reciclarOtro == 0)
                                    printf("Reciclo lo mio - ");
                                else printf("Reciclo lo de otro - ");
                                printf("Se recicló una unidad de vidrio.\n");
                                reciclarOtro = 0; j = i;
                          } else reciclarOtro = 1;  break; }
                        case 1: {
                          struct basuramsg m;
                          // Vidrio clasificado = 5
													if(msgrcv(qid_cola, &m, (sizeof(struct basuramsg) - sizeof(long)), 5, IPC_NOWAIT) > -1){
                                if(reciclarOtro == 0)
                                	printf("Reciclo lo mio - ");
                                else printf("Reciclo lo de otro - ");
                                printf("Se recicló una unidad de plastico.\n");
                                reciclarOtro = 0; j = i;
                            } else reciclarOtro = 1;  break; }
                        case 2: {// Aluminio clasificado = 7
                          struct basuramsg m;
													if(msgrcv(qid_cola, &m, (sizeof(struct basuramsg) - sizeof(long)), 7, IPC_NOWAIT) > -1){
                                if(reciclarOtro == 0)
                                	printf("Reciclo lo mio - ");
                                else printf("Reciclo lo de otro - ");
                                printf("Se recicló una unidad de aluminio.\n");
                                reciclarOtro = 0; j = i;
                            } else reciclarOtro = 1;  break; }
                        case 3: {// Cartón clasificado = 8
                          struct basuramsg m;
													if(msgrcv(qid_cola, &m, (sizeof(struct basuramsg) - sizeof(long)),8, IPC_NOWAIT) > -1){
                                if(reciclarOtro == 0)
                                	printf("Reciclo lo mio - ");
                                else printf("Reciclo lo de otro - ");
                                printf("Se recicló una unidad de carton.\n");
                                reciclarOtro = 0;
                                j = i;
                            } else reciclarOtro = 1; break; }
                    }

                        if(reciclarOtro == 1) {
                            int r = rand() % 2;
                            int h;
                            if(r == 0) { // Ayuda, si no, se va a tomar mate
                                do {
                                h = rand() % 4;
                                } while ( j != h );
                                j = h;
                                printf(" Ayudo a otro\n");
                            } else { printf(" Me voy a tomar mate\n"); reciclarOtro = 0; }
                        }
                        sleep(5);
              }
                    exit(0);
                } else continue;
            }
  
  	for(int i = 0; i < 9; i++)
      wait(NULL);

    msgctl(qid_cola, IPC_RMID, 0);

    return 0;
}

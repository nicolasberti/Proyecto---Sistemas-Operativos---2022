#include "constantes.h"

int main(int argc, char *argv[]){
    int i = atoi(argv[0]);
    int j = i;
    int reciclarOtro = 0;
    int qid_cola = msgget(KEY_cola, 0666);
        if(qid_cola == -1)
        printf("RECICLADOR: Error recuperando cola en reciclador%i\n", qid_cola);
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
}

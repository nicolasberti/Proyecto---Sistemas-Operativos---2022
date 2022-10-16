#include<stdio.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define ERROR_FORK 3

#define KEY_clasificador ((key_t) (1222377))

#define SEGSIZE sizeof(struct message_buffer) /* longitud del segmento */

struct message_buffer{
    long msg_type;
    int msg; // 1 a 4 recolectada, 5 a 9 clasificada.
};

//Recibe el id de una cola de mensajes y un mensaje a enviar y lo envia por esa cola.
void writer(int msg_id, int message){
    struct message_buffer m;
    m.msg_type= 1;
    m.msg = message;
    if(msgsnd(msg_id, &m, ( sizeof(struct message_buffer) - sizeof(long)), IPC_NOWAIT) == -1)
        printf("\nError al enviar mensaje en writer.\n");
}


//Recibe el id de una cola de mensajes, lee el mensaje que esta adentro y lo devuelve.
int reader(int msg_id){
    struct message_buffer m;
    if(msgrcv(msg_id, &m, (sizeof(struct message_buffer) - sizeof(long)), 0, IPC_NOWAIT) == -1)
        printf("\nError al recibir mensaje en reader\n");
    return m.msg;
}


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

  /*
   * Tipos de basura
   * P: Plástico
   * C: Cartón
   * V: Vidrio
   * A: Aluminio
  */

    pid_t pid;

    // En el caso de que la cola ya exista la recupero, la elimino y la vuelvo a crear.
    int qid_clas1 = msgget(KEY_clasificador, IPC_CREAT|0666);

    msgctl(qid_clas1, IPC_RMID, 0);

    int qid_clas = msgget(KEY_clasificador, IPC_CREAT|0666);
    if(qid_clas == -1)
        printf("\nError en creacion de cola principal\n");


  // Código de los recolectores
    for(int i = 0; i < 3; i++){
        pid = fork();
        if(pid < 0) exit(ERROR_FORK);
        else if(pid == 0) { // Código del reciclador
         srand( time(NULL)+(i*52*37*96)); // Semilla del rand, hago que sean distintas

            while(1){
                char basura = generarBasura();
                int reciclado;
                printf("\nRecolectar %i mete: %c\n", i, basura);
                switch(basura){
                    case 'P': { reciclado = 1; break; }
                    case 'V': { reciclado = 2; break; }
                    case 'A': { reciclado = 3; break; }
                    case 'C': { reciclado = 4 ; break; }
                }

                //Se obtiene la cola de mensajes.
                int msg_id = msgget(KEY_clasificador, 1);
                if(msg_id == -1){
                    printf("\nError en creacion de cola de recolectores\n");}
                else
                    writer(msg_id, reciclado);

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
                    //Se obtiene la cola de mensajes.
                    int basura;
                    int msg_id = msgget(KEY_clasificador, IPC_NOWAIT|0666);
                    if(msg_id == -1)
                        printf("\nError en cola para recibir de recolectores\n");
                    else{
                        //Se recibe el mensaje.
                        basura = reader(msg_id);

                        //Envia en la cola de mensajes el mensaje correspondiente.
                        switch(basura){
                            case 1: {printf("\n[Contador %i] Clasificador recibe Plastico\n", i); basura = 5; writer(msg_id, basura); break; } //Plastico
                            case 2: {printf("\n[Contador %i] Clasificador recibe Vidrio\n", i); basura = 6; writer(msg_id, basura); break; } //Vidrio
                            case 3: {printf("\n[Contador %i] Clasificador recibe Aluminio\n", i); basura = 7; writer(msg_id, basura); break; } //Aluminio
                            case 4: {printf("\n[Contador %i] Clasificador recibe Carton\n", i); basura = 8; writer(msg_id, basura); break; } //Carton
                            //default: {writer(msg_id, basura); break;} Tengo que volver a escribir lo que leí
                        }
                    }
                    sleep(5);
            }
            exit(0);
        } else continue;
    }


/*
for(int i = 0; i < 4; i++){
    pid = fork();
    if(pid < 0) exit(ERROR_FORK);
    else if(pid == 0) {

        int contador;
        int j = i;
        int reciclarOtro = 0;
        srand( time(NULL)+(i*123*54+123*5));
        int basura;
        int msg_id = msgget(KEY_clasificador, IPC_NOWAIT|0666);

        if(msg_id == -1)
            printf("\nError en cola recicladores\n");
        else{
                while(1){

                basura = reader(msg_id);

                switch(j){
                        case 0: {

                            if(basura == 6) {
                                if(reciclarOtro == 0)
                                    printf("Reciclo lo mio - ");
                                else printf("Reciclo lo de otro - ");
                                printf("Se recicló una unidad de vidrio.\n");
                                reciclarOtro = 0; j = i;
                            } else reciclarOtro = 1;  break; }
                        case 1: {

                            if(basura == 5) {
                                if(reciclarOtro == 0)
                                printf("Reciclo lo mio - ");
                                else printf("Reciclo lo de otro - ");
                                printf("Se recicló una unidad de plastico.\n");
                                reciclarOtro = 0; j = i;
                            } else reciclarOtro = 1;  break; }
                        case 2: {

                            if(basura == 7) {
                                if(reciclarOtro == 0)
                                printf("Reciclo lo mio - ");
                                else printf("Reciclo lo de otro - ");
                                printf("Se recicló una unidad de aluminio.\n");
                                reciclarOtro = 0; j = i;
                            } else reciclarOtro = 1;  break; }
                        case 3: {

                            if(basura == 8) {
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
                } else continue;
            }
*/
    msgctl(qid_clas, IPC_RMID, 0);

    return 0;
}

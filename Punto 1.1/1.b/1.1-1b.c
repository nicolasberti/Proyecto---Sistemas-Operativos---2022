#include<stdio.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define	ERROR_FORK 3

#define KEY_clasificador 	((key_t) (1230777))

#define KEY_vidrio 			((key_t) (123177))
#define KEY_aluminio	 	((key_t) (123277))
#define KEY_plastico 		((key_t) (123377))
#define KEY_carton	 		((key_t) (123477))

#define SEGSIZE sizeof(struct message_buffer) /* longitud del segmento */

struct message_buffer{
    long msg_type;
    char msg;
};

//Recibe el id de una cola de mensajes y un mensaje a enviar y lo envia por esa cola.
void writer(int msg_id, char message){
    struct message_buffer m;
    m.msg_type= 1;
    m.msg = message;
    if(msgsnd(msg_id, &m, ( sizeof(struct message_buffer) - sizeof(long) ), 0) == -1)
        printf("\nError al enviar mensaje.\n");
    printf("\nEstoy en writer meti %c\n", m.msg);
}

//Recibe el id de una cola de mensajes, lee el mensaje que esta adentro y lo devuelve.
char reader(int msg_id){
    struct message_buffer m;
    if(msgrcv(msg_id, &m, (sizeof(struct message_buffer) - sizeof(long)), 0, 0) == -1)
        printf("\nError al recibir mensaje\n");
    printf("\nEstoy en reader recibi %c\n", (m.msg));
    return (m.msg);
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

	// Creación de las colas
	int qid_clas = msgget(KEY_clasificador, IPC_CREAT|0666);

	int qid_vidrio = msgget(KEY_vidrio, IPC_CREAT|0666);
	int qid_aluminio = msgget(KEY_aluminio, IPC_CREAT|0666);
	int qid_carton = msgget(KEY_carton,  IPC_CREAT|0666);
	int qid_plastico = msgget(KEY_plastico, IPC_CREAT|0666);

  // Código de los recolectores
	for(int i = 0; i < 3; i++){
		pid = fork();
		if(pid < 0) exit(ERROR_FORK);
		else if(pid == 0) { // Código del reciclador
		  srand( time(NULL)+(i*52*37*96)); // Semilla del rand, hago que sean distintas

		  while(1){
			char basura = generarBasura();
			printf("\nRecolectar %i mete: %c\n", i, basura);

			//Creacion de cola de mensajes de recolectores.
			//int msg_id = shmget(KEY_clasificador, SEGSIZE, 0);
			int msg_id = msgget(KEY_clasificador, 0666);
            if(msg_id == -1)
                printf("\nError en creacion de cola de recolectores\n");
            //Se envia el mensaje,
            writer(msg_id, basura);

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
                //Se obtiene la cola de mensajes de los recolectores.
				char basura;
                //int msg_id = shmget(KEY_clasificador, SEGSIZE, 0);
                int msg_id = msgget(KEY_clasificador, 0666);
                if(msg_id == -1)
                    printf("\nError en cola para recibir de recolectores\n");

                //Se recibe el mensaje.
                basura = reader(msg_id);

				printf("\n[Contador %i] Clasificador recibe %c\n", i, basura);

                //Cola de mensajes para vidrio
				//int msg_id_vidrio = shmget(KEY_vidrio, SEGSIZE, 0);
				int msg_id_vidrio = msgget(KEY_vidrio, 0666);
                if(msg_id_vidrio == -1)
                    printf("\nError en cola de vidrio\n");

                //Cola de mensajes para plastico
                //int msg_id_plastico = shmget(KEY_plastico, SEGSIZE, 0);
                int msg_id_plastico = msgget(KEY_plastico,0666 );

                if(msg_id_plastico == -1)
                    printf("\nError en cola de plastico\n");

                //Cola de mensajes para aluminio
                //int msg_id_aluminio = shmget(KEY_aluminio, SEGSIZE, 0);
                int msg_id_aluminio = msgget(KEY_aluminio, 0666 );
                if(msg_id_aluminio == -1)
                    printf("\nError en cola de aluminio\n");

                //Cola de mensajes para carton
                //int msg_id_carton = shmget(KEY_carton, SEGSIZE, 0);
                int msg_id_carton = msgget(KEY_carton, 0666);
                if(msg_id_carton == -1)
                    printf("\nError en cola de carton\n");

                //Envia en la cola de mensajes correspondiente.
				switch(basura){
					case 'P': { writer(msg_id_plastico, basura); break; }
					case 'V': { writer(msg_id_vidrio, basura); break; }
					case 'A': { writer(msg_id_aluminio, basura); break; }
					case 'C': { writer(msg_id_carton, basura); break; }
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
			int nread;
			int j = i;
			int reciclarOtro = 0;
			srand( time(NULL)+(i*123*54+123*5));


			while(1){
				char basura;
				switch(j){
					case 0: {
                        //Se obtiene la cola de mensajes para vidrio.
                        //int msg_id_vidrio = shmget(KEY_vidrio, SEGSIZE, 0);
                        int msg_id_vidrio = msgget(KEY_vidrio, 0666);
                        if(msg_id_vidrio == -1)
                            printf("\nError en cola para recibir vidrio\n");
                        //Se recibe el mensaje.
						if(reader(msg_id_vidrio) == 'V') {
							if(reciclarOtro == 0)
								printf("Reciclo lo mio - ");
							else printf("Reciclo lo de otro - ");
							printf("Se recicló una unidad de vidrio.\n");
							reciclarOtro = 0; j = i;
						} else reciclarOtro = 1;  break; }
					case 1: {
                        //Se obtiene la cola de mensajes para plastico.
                        //int msg_id_plastico = shmget(KEY_plastico, SEGSIZE, 0);
                        int msg_id_plastico = msgget(KEY_plastico,0666 );
                        if(msg_id_plastico == -1)
                            printf("\nError en cola para recibir plastico\n");
                        //Se recibe el mensaje.
						if(reader(msg_id_plastico) == 'P') {
							if(reciclarOtro == 0)
								printf("Reciclo lo mio - ");
							else printf("Reciclo lo de otro - ");
							printf("Se recicló una unidad de plastico.\n");
							reciclarOtro = 0; j = i;
						} else reciclarOtro = 1;  break; }
					case 2: {
                        //Se obtiene la cola de mensajes para aluminio.
                        //int msg_id_aluminio = shmget(KEY_aluminio, SEGSIZE, 0);
                        int msg_id_aluminio = msgget(KEY_aluminio, 0666 );
                        if(msg_id_aluminio == -1)
                                printf("\nError en cola para recibir aluminio\n");
                        //Se recibe el mensaje.
						if(reader(msg_id_aluminio) == 'A') {
							if(reciclarOtro == 0)
								printf("Reciclo lo mio - ");
							else printf("Reciclo lo de otro - ");
							printf("Se recicló una unidad de aluminio.\n");
							reciclarOtro = 0; j = i;
						} else reciclarOtro = 1;  break; }
					case 3: {
                        //Se obtiene la cola de mensajes para carton.
                        //int msg_id_carton = shmget(KEY_carton, SEGSIZE, 0);
                        int msg_id_carton = msgget(KEY_carton, 0666);
                        if(msg_id_carton == -1)
                            printf("\nError en cola para recibir carton\n");
                        //Se recibe el mensaje.
						if(reader(msg_id_carton) == 'C') {
							if(reciclarOtro == 0)
								printf("Reciclo lo mio - ");
							else printf("Reciclo lo de otro - ");
							printf("Se recicló una unidad de carton.\n");
							reciclarOtro = 0; j = i;
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

    msgctl(qid_clas, IPC_RMID, 0);
    msgctl(qid_aluminio, IPC_RMID, 0);
    msgctl(qid_carton, IPC_RMID, 0);
    msgctl(qid_plastico, IPC_RMID, 0);
    msgctl(qid_vidrio, IPC_RMID, 0);

	return 0;
}

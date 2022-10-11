#include<stdio.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define	ERROR_FORK 3
#define MAX 2


struct message_buffer{
    long msg_type;
    char msg;
};

//Recibe el id de una cola de mensajes y un mensaje a enviar y lo envia por esa cola.
void writer(int msg_id, char message){
    struct message_buffer m;
    m.msg_type= 1;
    m.msg = message;
    if(msgsnd(msg_id, &m, strlen(&message), 0) == -1)
        printf("Error al enviar mensaje.");
}

//Recibe el id de una cola de mensajes, lee el mensaje que esta adentro y lo devuelve.
char reader(int msg_id){

    struct message_buffer m;

    if(msgrcv(msg_id, &m, MAX, 0, 0) == -1)
        printf("Error al recibir mensaje");

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
			int msg_id;
            key_t key;
            key = ftok("/tmp", 'a');
            msg_id = msgget(key, IPC_CREAT | 0666);
            if(msg_id == -1)
                printf("Error en creacion de cola");
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
                int msg_id;
                key_t key;
                key = ftok("/tmp", 'a');
                msg_id = msgget(key, IPC_CREAT | 0666);
                if(msg_id == -1)
                    printf("Error en cola");

                //Se recibe el mensaje.
                basura = reader(msg_id);

				printf("\n[Contador %i] Clasificador recibe %c\n", i, basura);

                //Cola de mensajes para vidrio
                key_t key_vidrio;
                key_vidrio = ftok("/tmp", 'b');
                int msg_id_vidrio;
                msg_id_vidrio = msgget(key_vidrio, IPC_CREAT | 0666);
                if(msg_id_vidrio == -1)
                    printf("Error en cola");

                //Cola de mensajes para plastico
                key_t key_plastico;
                key_plastico = ftok("/tmp", 'c');
                int msg_id_plastico;
                msg_id_plastico = msgget(key_plastico, IPC_CREAT | 0666);
                if(msg_id_plastico == -1)
                    printf("Error en cola");

                //Cola de mensajes para aluminio
                key_t key_aluminio;
                key_aluminio = ftok("/tmp", 'd');
                int msg_id_aluminio;
                msg_id_aluminio = msgget(key_aluminio, IPC_CREAT | 0666);
                if(msg_id_aluminio == -1)
                    printf("Error en cola");

                //Cola de mensajes para carton
                key_t key_carton;
                key_carton = ftok("/tmp", 'e');
                int msg_id_carton;
                msg_id_carton = msgget(key_carton, IPC_CREAT | 0666);
                if(msg_id_carton == -1)
                    printf("Error en cola");

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
                        key_t key_vidrio;
                        key_vidrio = ftok("/tmp", 'b');
                        int msg_id_vidrio;
                        msg_id_vidrio = msgget(key_vidrio, IPC_CREAT | 0666);
                        if(msg_id_vidrio == -1)
                            printf("Error en cola");
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
                        key_t key_plastico;
                        key_plastico = ftok("/tmp", 'c');
                        int msg_id_plastico;
                        msg_id_plastico = msgget(key_plastico, IPC_CREAT | 0666);
                        if(msg_id_plastico == -1)
                            printf("Error en cola");
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
                        key_t key_aluminio;
                        key_aluminio = ftok("/tmp", 'd');
                        int msg_id_aluminio;
                        msg_id_aluminio = msgget(key_aluminio, IPC_CREAT | 0666);
                        if(msg_id_aluminio == -1)
                                printf("Error en cola");
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
                        key_t key_carton;
                        key_carton = ftok("/tmp", 'e');
                        int msg_id_carton;
                        msg_id_carton = msgget(key_carton, IPC_CREAT | 0666);
                        if(msg_id_carton == -1)
                            printf("Error en cola");
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


	return 0;
}


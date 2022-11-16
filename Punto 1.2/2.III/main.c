#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>

// El modelo (algoritmo general) de como funciona el código está explicado en el informe como se pidió.

// IMPORTANTE: Si se aborta el programa, las colas quedan guardadas.
// Hice otro script que se llama "clear" para ejecutarlo y borrarlo si se quiere ejecutar de nuevo esto.
// Sin embargo, lo que hicimos es que previamente ya que se da esta situación, si hay una cola existente en memoria, se borre.

// Cola espera: contiene la llave (tipo=1) y los autos (tipo=getpid() del proceso auto + 2) que están esperando a cruzar el puente
// Cola pasar: contiene los autos habilitados a cruzar el puente en un momento de tiempo (tipo=getpid() del auto +2)

#define KEY_colaEspera 					((key_t) (1234))
#define KEY_colaPasar 					((key_t) (1235))

struct vehiculo{
    long pid;
    int sentido;
};

/* -- Código del vehículo -- */
void autoEntidad(int sent, int i, int qid_espera, int qid_pasar){
	
  char *sentido;
  sentido = malloc(6 * sizeof(char));
  if(sent == 1)
    strcpy(sentido, "Norte");
  else if(sent == 2)
    strcpy(sentido, "Sur");

  /* Sección de entrada: Un vehículo llega al puente. */
  struct vehiculo ae;
  ae.pid = (long) (getpid()+2);
  ae.sentido = sent;
  msgsnd(qid_espera, &ae, (sizeof(struct vehiculo) - sizeof(long)), 0); // Escribe el auto en la cola de espera.
  printf("auto%s %i: esperando a cruzar el puente (%i - %i)\n", sentido, i, ae.sentido, ae.pid);
  struct vehiculo auto_pasar;
  msgrcv(qid_pasar, &auto_pasar, (sizeof(struct vehiculo) - sizeof(long)), (getpid()+2), 0); // Lee el auto de la cola pasar.
  
  /* Cruzando el puente */
  printf("auto%s %i: pasando el puente...\n", sentido, i);
  msgsnd(qid_pasar, &auto_pasar, (sizeof(struct vehiculo) - sizeof(long)), 0); // Me agrego nuevamente para indicar que estoy pasando el puente.
  sleep(5);
  
  /* Sección de salida */
  printf("auto%s %i: pase el puente\n", sentido, i);
	  // Comprueba si fue el último. Esto lo puede hacer un auto a la vez, entonces agrego un mutex (mutex esta en qid_pasar y es el pid=1);
	  struct vehiculo mutex;
	  msgrcv(qid_pasar, &mutex, (sizeof(struct vehiculo) - sizeof(long)), 1, 0);
	  
		msgrcv(qid_pasar, &auto_pasar, (sizeof(struct vehiculo) - sizeof(long)), (getpid()+2), 0); // Me saco, porque ya pase el puente
		struct vehiculo auto_aux; // ¿Hay alguien ademas de mi cruzando el puente?
		if(msgrcv(qid_pasar, &auto_aux, (sizeof(struct vehiculo) - sizeof(long)), 0, IPC_NOWAIT) > -1){ 
		  msgsnd(qid_pasar, &auto_aux, (sizeof(struct vehiculo) - sizeof(long)), 0);
		  printf("auto%s %i: hay autos en el puente\n", sentido, i); 
		}
		else { 
		  msgsnd(qid_espera, &mutex, (sizeof(struct vehiculo) - sizeof(long)), 0); // Como el mutex y la llave comparten el mismo "pid", meto el mutex pero a la cola qid_espera
		  // mutex y llave es como un token único que solo lo puede tener una entidad a la vez.
		  printf("auto%s %i: soy el ultimo. puente disponible\n", sentido, i); 
		}
	  
	  msgsnd(qid_pasar, &mutex, (sizeof(struct vehiculo) - sizeof(long)), 0); // libero el mutex
    
  exit(0);
}
/* -- Fin código vehículo -- */

/* -- Código generador de vehículos -- */
void generadorAutos(int qid_espera, int qid_pasar){
	// Creación de autos
	pid_t pid;
	int i = 0;
	while(1){
		pid = fork();
		if(pid == 0){
			srand(time(NULL) * i); 
		  int random = rand() % 2;
		  if(random == 0)
			{ autoEntidad(1, i, qid_espera, qid_pasar); }
		  else 
			{ autoEntidad(2, i, qid_espera, qid_pasar); }
		} 
		else { i++; sleep(1); }
	}
}
/* -- Fin código generador de vehículos -- */


int main(){
  
  /* En caso de que anteriormente se haya abortado el programa (como nunca finaliza) se borran las colas si es que quedaron en memoria*/
    int qid_espera = msgget(KEY_colaEspera, 0666);
    int qid_pasar = msgget(KEY_colaPasar, 0666);
	msgctl(qid_espera, IPC_RMID, 0);
	msgctl(qid_pasar, IPC_RMID, 0);
  /* ------------------------------------------------------------------------------------------------------------------------------- */
  
  // Creación de las colas.
  qid_espera = msgget(KEY_colaEspera, IPC_CREAT|0666);
  qid_pasar = msgget(KEY_colaPasar, IPC_CREAT|0666);
  int sentidoActual = 0; // Variable local que usa el intermediario para saber que sentido se encuentra.
  
  // El que genera los vehículos es un proceso aparte.
 
  pid_t pid;
  pid = fork();
  if(pid == 0){
	generadorAutos(qid_espera, qid_pasar);
  }
  
  sleep(1);
  
  struct vehiculo mutex;
  mutex.pid = 1;
  msgsnd(qid_pasar, &mutex, (sizeof(struct vehiculo) - sizeof(long)), 0);
  
  // -- "Operario" que indica que autos deben pasar. -> Se repite infinitamente.
  while(1){
    struct vehiculo m;
    msgrcv(qid_espera, &m, (sizeof(struct vehiculo) - sizeof(long)), 0, 0); 
    // Si llegase a leer la llave (pid==1) continua el ciclo agregando la llave.
    if(m.pid == 1) {
        msgsnd(qid_espera, &m, (sizeof(struct vehiculo) - sizeof(long)), 0);
    }
    else {
   	 printf("Vehiculo leido: [%i - %i]\n", m.sentido, m.pid);
   	 if(sentidoActual == 0){
  	  	sentidoActual = m.sentido;
        printf("Comienza con sentido: ");
        if(sentidoActual == 1)
            printf("Norte\n");
        else 
            printf("Sur\n");
  	  }
  	  if(m.sentido == sentidoActual){
  	   	msgsnd(qid_pasar, &m, (sizeof(struct vehiculo) - sizeof(long)), 0);
  	    printf("Ingreso a cola pasar [%i - %i]\n", m.sentido, m.pid);
  	  } else {
   	   struct vehiculo llave;
    	  msgrcv(qid_espera, &llave, (sizeof(struct vehiculo) - sizeof(long)), 1, 0); // Lee la llave
          sentidoActual = m.sentido;
          printf("Se cambio el sentido a: ");
          if(sentidoActual == 1)
            printf("Norte\n");
          else 
            printf("Sur\n");
     	 msgsnd(qid_pasar, &m, (sizeof(struct vehiculo) - sizeof(long)), 0);
   	 }
   }
  }
  
  msgctl(qid_espera, IPC_RMID, 0);
  msgctl(qid_pasar, IPC_RMID, 0);
  printf("\n--FIN--\n");
  exit(0);
}

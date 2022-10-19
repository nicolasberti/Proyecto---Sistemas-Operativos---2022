#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

// ## MUY IMPORTANTE: Si se aborta el programa, las colas quedan guardadas. hice otro script que se llama "clear" para ejecutarlo y borrarlo si se quiere ejecutar de nuevo esto.

// Cola espera: contiene la llave (tipo=1) y los autos (tipo=getpid() del proceso auto + 2) que están esperando a cruzar el puente
// Cola pasar: contiene los autos habilitados a cruzar el puente en un momento de tiempo (tipo=getpid() del auto +2)

// en cola pasra se podria agregar el tipo 1 para mutex, y getpid()+1 los autos

#define KEY_colaEspera 					((key_t) (1234))
#define KEY_colaPasar 					((key_t) (1235))

#define CANTIDAD_AUTOS 6

struct vehiculo{
    long pid;
    int sentido;
};

void autoEntidad(int sent, int i, int qid_espera, int qid_pasar){
  char *sentido;
  sentido = malloc(6 * sizeof(char));
  if(sent == 1)
    strcpy(sentido, "Norte");
  else if(sent == 2)
    strcpy(sentido, "Sur");
  struct vehiculo ae;
  ae.pid = (long) (getpid()+2);
  ae.sentido = sent;
  msgsnd(qid_espera, &ae, (sizeof(struct vehiculo) - sizeof(long)), 0); // Escribe el auto en la cola de espera.
  printf("auto%s %i: ingrese a COLA ESPERA... (%i - %i)\n", sentido, i, ae.sentido, ae.pid);
  struct vehiculo auto_pasar;
  msgrcv(qid_pasar, &auto_pasar, (sizeof(struct vehiculo) - sizeof(long)), (getpid()+2), 0); // Lee el auto de la cola pasar.
  printf("auto%s %i: pasando el puente...\n", sentido, i);
  msgsnd(qid_pasar, &auto_pasar, (sizeof(struct vehiculo) - sizeof(long)), 0); // Me agrego nuevamente para indicar que estoy pasando el puente.
  sleep(5);
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
      msgsnd(qid_espera, &mutex, (sizeof(struct vehiculo) - sizeof(long)), 0); // COmo el mutex y la llave comparten el mismo "pid", meto el mutex pero a la cola qid_espera
      // mutex y llave es como un token único que solo lo puede tener una entidad a la vez.
      printf("auto%s %i: puente disponible\n", sentido, i); 
    }
  
  msgsnd(qid_pasar, &mutex, (sizeof(struct vehiculo) - sizeof(long)), 0); // libero el mutex
    
  exit(0);
}

int main(){
  
  // Creación de la cola.
  int qid_espera = msgget(KEY_colaEspera, IPC_CREAT|0666);
  int qid_pasar = msgget(KEY_colaPasar, IPC_CREAT|0666);
  int sentidoActual = 0;
  // Creación de autos
  pid_t pid;
  for(int i = 0; i < CANTIDAD_AUTOS; i++){
    pid = fork();
    if(pid == 0){
    	srand(time(NULL) * i); 
      int random = rand() % 2;
      if(random == 0)
        { printf("N\n"); autoEntidad(1, i, qid_espera, qid_pasar); }
      else 
        { printf("S\n"); autoEntidad(2, i, qid_espera, qid_pasar); }
    } else continue;
  }
  sleep(2);
  printf("\n\n");
  
  struct vehiculo mutex;
  mutex.pid = 1;
  msgsnd(qid_pasar, &mutex, (sizeof(struct vehiculo) - sizeof(long)), 0);
  
  // -- "Operario" que indica que autos deben pasar.
  while(1){
    struct vehiculo m;
    msgrcv(qid_espera, &m, (sizeof(struct vehiculo) - sizeof(long)), 0, 0); 
    // Hacer un caso si llegase a leer la llave (pid==1) que continue el ciclo agregando la llave.
    
    // hay que compilar con gcc -D_GNU_SOURCE [el resto de los parametros]
    printf("Vehiculo leido: [%i - %i]\n", m.sentido, m.pid);
    if(sentidoActual == 0){
    	sentidoActual = m.sentido;
      printf("sentidoActual = %i", m.sentido);
    }
    if(m.sentido == sentidoActual){
     	msgsnd(qid_pasar, &m, (sizeof(struct vehiculo) - sizeof(long)), 0);
      printf("Ingreso a cola pasar [%i - %i]\n", m.sentido, m.pid);
    } else {
      struct vehiculo llave;
      msgrcv(qid_espera, &llave, (sizeof(struct vehiculo) - sizeof(long)), 1, 0); // Lee la llave
      sentidoActual = m.sentido;
      msgsnd(qid_pasar, &m, (sizeof(struct vehiculo) - sizeof(long)), 0);
    }
  }
  
  msgctl(qid_espera, IPC_RMID, 0);
	msgctl(qid_pasar, IPC_RMID, 0);
	printf("\n--FIN--\n");
	exit(0);
  
  
}

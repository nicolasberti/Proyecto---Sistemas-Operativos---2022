#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY_colaespera 					((key_t) (123177))
#define KEY_colapasar	 					((key_t) (123277))
#define KEY_colapuentelibre 		((key_t) (123377))

#define CANTIDAD_AUTOS 20

#define SEGSIZE sizeof(struct message_buffer)

struct message_buffer{
    long pid;
    int sentido;
};

// ------
void writer(int msg_id, struct message_buffer m){
    if(msgsnd(msg_id, &m, ( sizeof(struct message_buffer) - sizeof(long) ), 0) == -1)
        printf("\nError al enviar mensaje.\n");
}

struct message_buffer readerPrimero(int msg_id){
    struct message_buffer m;
    if(msgrcv(msg_id, &m, (sizeof(struct message_buffer) - sizeof(long)), 0, 0) == -1)
        printf("\nError al recibir mensaje\n");
    return m;
}

struct message_buffer readerPID(int msg_id, long pid){
    struct message_buffer m;
    if(msgrcv(msg_id, &m, (sizeof(struct message_buffer) - sizeof(long)), pid, 0) == -1)
        printf("\nError al recibir mensaje\n");
    return m;
}

void autoNorte(int i){
  int qid_espera = msgget(KEY_colaespera, 0666);
  int qid_pasar = msgget(KEY_colapasar, 0666);
  int qid_puentelibre = msgget(KEY_colapuentelibre, 0666);
  struct message_buffer m;
  m.pid = getpid();
  m.sentido = 1;
  writer(qid_espera, m); // Estoy a la espera de pasar el puente
  readerPID(qid_pasar, getpid()); // Cuando esté en la cola de pasar, paso.
  printf("autoNorte %i: pasando el puente...\n", i);
  sleep(5);
  printf("autoNorte %i: pase el puente\n", i);
  // ¿cola vacía?
  struct message_buffer mAux;
  if(msgrcv(qid_pasar, &mAux, (sizeof(struct message_buffer) - sizeof(long)), 0, IPC_NOWAIT) != -1)
    // Cola no vacía
    { writer(qid_pasar, mAux); printf("autoNorte %i: hay autos a la espera de pasar de mi mismo sentido\n", i); }
  else { writer(qid_puentelibre, m); printf("autoNorte %i: puente disponible\n", i); }
  exit(0);
}

void autoSur(int i){
  int qid_espera = msgget(KEY_colaespera, 0666);
  int qid_pasar = msgget(KEY_colapasar, 0666);
  int qid_puentelibre = msgget(KEY_colapuentelibre, 0666);
  struct message_buffer m;
  m.pid = getpid();
  m.sentido = 1;
  writer(qid_espera, m); // Estoy a la espera de pasar el puente
  readerPID(qid_pasar, getpid()); // Cuando esté en la cola de pasar, paso.
  printf("autoSur %i: pasando el puente...\n", i);
  sleep(5);
  printf("autoSur %i: pase el puente\n", i);
  // ¿cola vacía?
  struct message_buffer mAux;
  if(msgrcv(qid_pasar, &mAux, (sizeof(struct message_buffer) - sizeof(long)), 0, IPC_NOWAIT) != -1)
    // Cola no vacía
    { writer(qid_pasar, mAux); printf("autoSur %i: hay autos a la espera de pasar de mi mismo sentido\n", i); }
  else { writer(qid_puentelibre, m); printf("autoSur %i: puente disponible\n", i); }
  exit(0);
}


int main(){
  
  // Creación de colas.
  int qid_espera = msgget(KEY_colaespera, IPC_CREAT|0666);
  int qid_pasar = msgget(KEY_colapasar, IPC_CREAT|0666);
  int qid_puentelibre = msgget(KEY_colapuentelibre, IPC_CREAT|0666);
  int sentidoActual = 1; // 1=norte, 2=sur
  
  // Creación de autos
  pid_t pid;
  for(int i = 0; i < CANTIDAD_AUTOS; i++){
    pid = fork();
    if(pid == 0){
    	srand(time(NULL) * i); 
      switch(rand() % 2){
      case 0: { printf("N\n"); autoNorte(i); break; }
      case 1: { printf("S\n"); autoSur(i); break; }
    	}
    } else continue;
  }
  printf("\n\n");
  // -- "Operario" que indica que autos deben pasar.
  while(1){
    struct message_buffer m;
    m = readerPrimero(qid_espera);
    if(m.sentido == sentidoActual){
      writer(qid_pasar, m);
    } else {
      /*struct message_buffer mAux;
      mAux = */readerPrimero(qid_puentelibre);
      sentidoActual = m.sentido;
      writer(qid_pasar, m);
    }
    sleep(2);
  }
  
	printf("\n--FIN--\n");
	exit(0);
}

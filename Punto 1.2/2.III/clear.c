#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY_colaEspera 					((key_t) (1234))
#define KEY_colaPasar 					((key_t) (1235))

int main(int argc, char *argv[]) {
	int qid_espera = msgget(KEY_colaEspera, 0666);
	int qid_pasar = msgget(KEY_colaPasar, 0666);
	
	msgctl(qid_espera, IPC_RMID, 0);
	msgctl(qid_pasar, IPC_RMID, 0);
	printf("\nColas borradas\n");
	exit(0);
}

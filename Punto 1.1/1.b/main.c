#include "constantes.h"
int main(){
    pid_t pid;
    int qid_cola = msgget(KEY_cola, IPC_CREAT|0666);
    if(qid_cola == -1)
        printf("\nError en creacion de cola principal %i\n", qid_cola);

    for(int i = 0; i < 3; i++){
	pid = fork();
	if(pid < 0) exit(ERROR_FORK);
	else if(pid == 0) {
	    execvp("./recolector", NULL);
	}
    }
	
	for(int i = 0; i < 2; i++){
        pid = fork();
	char str[50];
	snprintf(str, sizeof(str), "%i", i);
        if(pid < 0) exit(ERROR_FORK);
        else if(pid == 0) {
			execlp("./clasificador", str); // i? lo manda bien? 
		}
	}
	
	for(int i = 0; i < 4; i++){
        pid = fork();
	char str[50];
	snprintf(str, sizeof(str), "%i", i);
        if(pid < 0) exit(ERROR_FORK);
        else if(pid == 0) {
			execlp("./reciclador", str);
		}
	}
	
  	for(int i = 0; i < 9; i++)
	    wait(NULL);


	exit(0);
}

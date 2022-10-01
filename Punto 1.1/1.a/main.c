#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define ERROR_PIPE 1
#define	ERROR_FCNTL 2
#define	ERROR_FORK 3

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
	int pipe_recicladorTOclasificador[2];

  int pipe_vidrio[2];
  int pipe_carton[2];
  int pipe_aluminio[2];
  int pipe_plastico[2];
  
  if(pipe(pipe_recicladorTOclasificador) < 0) exit(ERROR_PIPE);
  if(pipe(pipe_vidrio) < 0) exit(ERROR_PIPE);
  if(pipe(pipe_carton) < 0) exit(ERROR_PIPE);
  if(pipe(pipe_aluminio) < 0) exit(ERROR_PIPE);
  if(pipe(pipe_plastico) < 0) exit(ERROR_PIPE);
  
  // Estos pipes son no bloqueantes para tener la posibilidad de optar por ayudar o ir a tomar mate (desde el punto de vista del reciclador)
  if (fcntl(pipe_vidrio[0], F_SETFL, O_NONBLOCK) < 0) exit(ERROR_FNCTL);
  if (fcntl(pipe_carton[0], F_SETFL, O_NONBLOCK) < 0) exit(ERROR_FNCTL);
  if (fcntl(pipe_aluminio[0], F_SETFL, O_NONBLOCK) < 0) exit(ERROR_FNCTL);
  if (fcntl(pipe_plastico[0], F_SETFL, O_NONBLOCK) < 0) exit(ERROR_FNCTL);
  
  // Código de los recolectores
	for(int i = 0; i < 3; i++){
		pid = fork();
		if(pid < 0) exit(ERROR_FORK);
		else if(pid == 0) { // Código del reciclador
		  srand( time(NULL)+(i*52*37*96)); // Semilla del rand, hago que sean distintas 
		  /* Cierro los pipes que no se usan */
		  close(pipe_recicladorTOclasificador[0]);
		  close(pipe_vidrio[0]);
		  close(pipe_vidrio[1]);
		  close(pipe_carton[0]);
		  close(pipe_carton[1]);
		  close(pipe_plastico[0]);
		  close(pipe_plastico[1]);
		  close(pipe_aluminio[0]);
		  close(pipe_aluminio[1]);
		  while(1){
			char basura = generarBasura(); // Se mete una basura random al pipe
			// printf("Recolectar %i mete: %c\n", i, basura);
			write(pipe_recicladorTOclasificador[1], &basura, sizeof(char));
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
				close(pipe_recicladorTOclasificador[1]);
				close(pipe_vidrio[0]);
				close(pipe_carton[0]);
				close(pipe_plastico[0]);
				close(pipe_aluminio[0]);
				char basura;
				read(pipe_recicladorTOclasificador[0], &basura, sizeof(char));
				/* La escribe en el pipe correspondiente */
				//printf("[Contador %i] Clasificador recibe %c\n", i, basura);
				switch(basura){
					case 'P': { write(pipe_plastico[1], &basura, sizeof(char)); break; }
					case 'V': { write(pipe_vidrio[1], &basura, sizeof(char)); break; }
					case 'A': { write(pipe_aluminio[1], &basura, sizeof(char)); break; }
					case 'C': { write(pipe_carton[1], &basura, sizeof(char)); break; }
				}
				sleep(5);
			}
			exit(0);
		} else continue;
	}
  
  /*
	Algoritmo - pseudocódigo de los recicladores
	Leo del pipe
	if(tengo algo)
		lo proceso
	else
		si no me fui a tomar mate, leo de otro pipe al azar
		if(tengo algo)
			proceso
  (Repito el proceso)
  */
  
	for(int i = 0; i < 4; i++){
		pid = fork();
		if(pid < 0) exit(ERROR_FORK);
		else if(pid == 0) {
			close(pipe_recicladorTOclasificador[0]);
			close(pipe_recicladorTOclasificador[1]);
			close(pipe_vidrio[1]);
			close(pipe_carton[1]);
			close(pipe_plastico[1]);
			close(pipe_aluminio[1]);
			int contador;
			int nread;
			int j = i;
			int reciclarOtro = 0;
			srand( time(NULL)+(i*123*54+123*5)); 
			while(1){
				char basura;
				switch(j){
					case 0: {
						nread = read(pipe_vidrio[0], &basura, sizeof(char));
						if(nread == 1) {
							if(reciclarOtro == 0)
								printf("Reciclo lo mio - ");
							else printf("Reciclo lo de otro - ");
							printf("Se recicló una unidad de vidrio.\n");
							reciclarOtro = 0; j = i;
						} else reciclarOtro = 1; printf("No tengo para reciclar. Que hago?"); break; }
					case 1: {
						nread = read(pipe_plastico[0], &basura, sizeof(char));
						if(nread == 1) {
							if(reciclarOtro == 0)
								printf("Reciclo lo mio - ");
							else printf("Reciclo lo de otro - ");
							printf("Se recicló una unidad de plastico.\n");
							reciclarOtro = 0; j = i;
						} else reciclarOtro = 1; printf("No tengo para reciclar. Que hago?"); break; }
					case 2: {
						nread = read(pipe_aluminio[0], &basura, sizeof(char));
						if(nread == 1) {
							if(reciclarOtro == 0)
								printf("Reciclo lo mio - ");
							else printf("Reciclo lo de otro - ");
							printf("Se recicló una unidad de aluminio.\n");
							reciclarOtro = 0; j = i;
						} else reciclarOtro = 1; printf("No tengo para reciclar. Que hago?"); break; }
					case 3: {
						nread = read(pipe_carton[0], &basura, sizeof(char));
						if(nread == 1) {
							if(reciclarOtro == 0)
								printf("Reciclo lo mio - ");
							else printf("Reciclo lo de otro - ");
							printf("Se recicló una unidad de carton.\n");
							reciclarOtro = 0; j = i;
						} else reciclarOtro = 1; printf("No tengo para reciclar. Que hago?"); break; }
				}
				if(reciclarOtro == 1) {
					int r = rand() % 2;
					int h;
					if(r == 0) { // El reciclar decide que hacer, si ayudar o irse a tomar mate.
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
  
	/* Fin */
	exit(0);
}

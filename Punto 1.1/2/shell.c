#include "constantes.h"


/* 
 * 
 * Sintaxis de los comandos
 * Crear un directorio: mkdir [ubicacion] [nombre]
 * Remover un directorio: rmdir [ubicacion] [nombre]
 * Crear un archivo: mkfile [ubicacion] [nombre.formato]
 * Listar el contenido de un directorio: ldir [ubicacion]
 * Listar el contenido de un archivo: lfile [ubicacion] [nombre.formato] 
 * Mostrar una ayuda con los comandos posibles: help
 * 
*/

int main()
{
	char buf[1024];
	char *token;
	char *comandos[TOTAL_INDICES];
	comandos[0] = malloc(sizeof(char) * MAXIMO_CHAR);
	comandos[1] = malloc(sizeof(char) * MAXIMO_CHAR);
	comandos[2] = malloc(sizeof(char) * MAXIMO_CHAR);
	strcpy(comandos[0], "");
	strcpy(comandos[1], "");
	strcpy(comandos[2], "");
	printf("~$ shell> ");
	int errorLimite = 0;
	int count = 0;
	char *comando;
	comando = malloc(sizeof(char) * 25);
	pid_t pid;
	int status;
	while (fgets(buf, 1024, stdin)) {
		count = 0;
		errorLimite = 0;
		token = strtok(buf, " ");
		while (token != 0 && strlen(token) > 1 && errorLimite == 0) {
			if(count >= TOTAL_INDICES) {
				errorLimite = 1;
			}
			else {
				strcpy(comandos[count], token);
				count++;
				token = strtok(0, " ");
			}	
		}
		*(comandos[count-1]+(strlen(comandos[count-1])-1)) = '\0'; // El ultimo string contendrá un '\n' al final, se cambia por el caracter finalizador: '\0'
		if(count > TOTAL_INDICES) { ERROR_LIMITE }
		else {
			pid = fork();
			if(pid == 0){
				strcpy(comando, "./");
				strcat(comando, comandos[0]);
				comandos[3] = NULL;
				if(execvp(comando,comandos) == -1)
					exit(-1);
			} else {
				waitpid(pid, &status, 0);
				if(WEXITSTATUS(status)!=0) {
					ERROR_DECOMANDO
					printf("\n~$ shell> ");
				} else printf("~$ shell> ");
  
			} 
		}
		
	}
  exit(0);
}

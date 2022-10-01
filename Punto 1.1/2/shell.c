#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ftw.h>
#include <dirent.h>

/* Errores */
#define ERROR_PIPE					    	1
#define ERROR_FORK              			2


#define CANTIDAD_COMANDOS					6
/* Errores pero para imprimir por consola */
#define ERROR_LIMITE 				     	printf("~$ Limite de comandos");
#define	ERROR_DECOMANDO				     	printf("~$ El comando no pudo ser ejecutado [revise los parametros]");
#define ERROR_INVALIDO 				     	printf("~$ Comando invalido");


#define	PARAMETROS_MAXIMOS			     	2
#define	TOTAL_INDICES				        PARAMETROS_MAXIMOS+1
#define MAXIMO_CHAR					      	256

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


int rmdir_notempty(const char *path) { // Remueve un directorio que no está vacío -> Si usamos rmdir solo se restringue a directorios vacíos.
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;
   if (d) {
      struct dirent *p;
      r = 0;
      while (!r && (p=readdir(d))) {
          int r2 = -1;
          char *buf;
          size_t len;
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
             continue;
          len = path_len + strlen(p->d_name) + 2; 
          buf = malloc(len);
          if (buf) {
             struct stat statbuf;
             snprintf(buf, len, "%s/%s", path, p->d_name);
             if (!stat(buf, &statbuf)) {
                if (S_ISDIR(statbuf.st_mode))
                   r2 = remove_directory(buf);
                else
                   r2 = unlink(buf);
             }
             free(buf);
          }
          r = r2;
      }
      closedir(d);
   }
   if (!r)
      r = rmdir(path);
   return r;
}

int ejecutar(char comandos[TOTAL_INDICES][MAXIMO_CHAR]){
  	pid_t pid;
    int fd[2];
    int retorno = 0;
    if(pipe(fd) < 0) exit(ERROR_PIPE);
    
    pid = fork();
    if(pid < 0) exit(ERROR_FORK);
    else if(pid == 0){
      if(strcmp(comandos[0], "mkdir") == 0){ // OK
			char ubicacion[MAXIMO_CHAR*2];
			strcpy(ubicacion, "");
			strcat(ubicacion, comandos[1]); // el primer parametro tiene que terminar con /
			strcat(ubicacion, comandos[2]);
			retorno = mkdir(ubicacion, 0777);
      } else if(strcmp(comandos[0], "rmdir") == 0){ // OK.
			char ubicacion[MAXIMO_CHAR*2];
			strcpy(ubicacion, "");
			strcat(ubicacion, comandos[1]); // el primer parametro tiene que terminar con /
			strcat(ubicacion, comandos[2]);
			retorno = rmdir_notempty(ubicacion);
      } else if(strcmp(comandos[0], "mkfile") == 0){ // OK
			FILE *fp;
			char ubicacion[MAXIMO_CHAR*2];
			strcpy(ubicacion, "");
			strcat(ubicacion, comandos[1]); // el primer parametro tiene que terminar con /
			strcat(ubicacion, comandos[2]);
			fp = fopen(ubicacion, "w");
			if(fp == NULL) 
				retorno = -1;
			else 
				fclose(fp); 
      } else if(strcmp(comandos[0], "ldir") == 0){ // OK.
			const char *dir_name;            
			DIR *dir_ptr = NULL;
			struct dirent *dirent_ptr;
			dir_name = comandos[1];
			if ((dir_ptr = opendir(dir_name)) == NULL) { retorno = -1; }
			else {
				while ((dirent_ptr = readdir(dir_ptr)) != NULL) {
					if(strcmp(dirent_ptr -> d_name, ".") != 0 && strcmp(dirent_ptr -> d_name, "..") != 0)
					printf("%s\n", dirent_ptr -> d_name);
				}
				if(dir_ptr != NULL) 
					closedir(dir_ptr);
			}
        } else if(strcmp(comandos[0], "lfile") == 0){ // OK
			FILE* fichero;
			char ubicacion[MAXIMO_CHAR*2];
			char contenido[MAXIMO_CHAR*2];
			strcpy(ubicacion, "");
			strcat(ubicacion, comandos[1]); // el primer parametro tiene que terminar con /
			strcat(ubicacion, comandos[2]);
			fichero = fopen(ubicacion, "rt");
			if(fichero == NULL) { retorno = -1; }
			else {
			  while(fgets (contenido, MAXIMO_CHAR*2, fichero))
				printf("%s", contenido);
			  fclose(fichero);
			}
      } else if(strcmp(comandos[0], "help") == 0){ // OK
			char *ayuda = "~$ Comandos disponibles\n- Crear un directorio: mkdir [ubicacion] [nombre]\n- Remover un directorio (debe estar vacio): rmdir [ubicacion] [nombre]\n- Crear un archivo: mkfile [ubicacion] [nombre.formato]\n- Listar el contenido de un directorio: ldir [ubicacion]\n- Listar el contenido de un archivo: lfile [ubicacion]\nIMPORTANTE! Las ubicaciones deben terminar con /";
			printf("%s", ayuda);
      } else retorno = -2; // No coincide con ningún comando, por ende, no es un comando válido.
	  close(fd[0]);
	  write(fd[1], &retorno, sizeof(int)); // Mando al padre como terminó la ejecución del comando que enviaron
	  close(fd[1]);
	  exit(0); // Finalizan los procesos
	} else {		
		close(fd[1]);
		read(fd[0], &retorno, sizeof(int)); // No es necesario un wait ya que con este read hace una espera ocupada el padre.
		close(fd[0]);
		/*
			OBS. -> En el enunciado no especifica si se deben ejecutar en background los comandos, entonces asumimos que es todo en foreground.
		*/
	}
  return retorno;    
}

int main()
{
	char buf[1024];
	char *token;
	char comandos[TOTAL_INDICES][MAXIMO_CHAR];
	strcpy(comandos[0], "");
	strcpy(comandos[1], "");
	strcpy(comandos[2], "");
	printf("~$ shell> ");
	int errorLimite = 0;
	int retornoComandos;
	int count = 0;
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
    	comandos[count-1][strlen(comandos[count-1])-1] = '\0'; // El ultimo string contendrá un '\n' al final, se cambia por el caracter finalizador: '\0'
        if(count > TOTAL_INDICES) { ERROR_LIMITE }
        else {
            if(count > 0){
              	retornoComandos = ejecutar(comandos);
				if(retornoComandos == -2) ERROR_INVALIDO
					else if(retornoComandos == -1) ERROR_DECOMANDO
            } 
		}
		printf("\n~$ shell> ");
  }
  exit(0);
}
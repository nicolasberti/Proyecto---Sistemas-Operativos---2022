#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANTIDAD_COMANDOS		6
#define ERROR_PIPE					1
#define ERROR_LIMITE 				printf("~$ Limite de comandos");
#define	ERROR_DECOMANDO			printf("~$ El comando no pudo ser ejecutado [revise los parametros]");
#define ERROR_INVALIDO 			printf("~$ Comando invalido");
#define	PARAMETROS_MAXIMOS	2
#define	TOTAL_INDICES				PARAMETROS_MAXIMOS+1
#define MAXIMO_CHAR					256

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

/*
static char *comandos_posibles[] = { "mkdir", "rmdir", "mkfile", "ldir", "lfile", "help" };
static int parametros_posibles = { 2, 2, 2, 1, 2, 0 }; // ¿es necesario?
*/																							
/*
 * Retorna 0 si es valido, -1 en caso contrario.
 * 
 */
/*int comandoValido(char comandos[100]){ // ¿De esto se podria encargar otro proceso?
  int retorno = -1;
  for(int i = 0; i < CANTIDAD_COMANDOS && retorno == -1; i++){
    if(strcmp(comandos, comandos_posibles[i]) == 0)
      retorno = 0;
  }
  return retorno;
}*/

int ejecutar(char comandos[TOTAL_INDICES][MAXIMO_CHAR]){
  /*	pid_t pid;
  	int fd[2];
  	if(pipe(fd) < 0) exit(ERROR_PIPE);*/
    int retorno = 0;
      if(strcmp(comandos[0], "mkdir") == 0){
        
        	char ubicacion[MAXIMO_CHAR*2];
        	strcpy(ubicacion, "");
        	strcat(ubicacion, comandos[1]); // el primer parametro tiene que terminar con /
        	strcat(ubicacion, comandos[2]);
        	if(mkdir(ubicacion, S_IRWXU) < 0){
            retorno = -1;
          }
        
      } else if(strcmp(comandos[0], "rmdir") == 0){

        	char ubicacion[MAXIMO_CHAR*2];
        	strcpy(ubicacion, "");
        	strcat(ubicacion, comandos[1]); // el primer parametro tiene que terminar con /
        	strcat(ubicacion, comandos[2]);
        	if(remove(ubicacion) < 0){
            retorno = -1;
          }
        
      } else if(strcmp(comandos[0], "mkfile") == 0){
        
        FILE *fp;
        char ubicacion[MAXIMO_CHAR*2];
        strcpy(ubicacion, "");
        strcat(ubicacion, comandos[1]); // el primer parametro tiene que terminar con /
        strcat(ubicacion, comandos[2]);
        fp = fopen(ubicacion, "w");
        if(fp == NULL) { retorno = -1; }
        else {
         fclose(fp); 
        }
        
      } else if(strcmp(comandos[0], "ldir") == 0){
        // https://foro.chuidiang.org/ccplusplus-linux/listar-contenido-de-un-directorio-usando-funciones-posix-de-c/
        const char * dir_name;            
				DIR * dir_ptr = NULL;
				struct dirent * dirent_ptr;
        int count = 0;
        
        dir_name = comandos[1];
        if ((dir_ptr = opendir(dir_name)) == NULL) {
					//printf( "No existe o no se pudo abrir el directorio '%s'\n", dir_name );
					return -1;
				}
        
        /* ahora listamos el directorio */
				while (count < 100 && (dirent_ptr = readdir(dir_ptr)) != NULL) {
					printf("[%03d] %s\n", ++count, dirent_ptr -> d_name);
				}
	
				/* cierra el directorio */
				if ( dir_ptr != NULL ) 
          closedir( dir_ptr );
        
      } else if(strcmp(comandos[0], "lfile") == 0){
        
        FILE* fichero;
        char ubicacion[MAXIMO_CHAR*2];
        char contenido[MAXIMO_CHAR*2];
        strcpy(ubicacion, "");
        strcat(ubicacion, comandos[1]); // el primer parametro tiene que terminar con /
        strcat(ubicacion, comandos[2]);
    		fichero = fopen(ubicacion, "rt");
        if(fichero == NULL) { retorno = -1; }
        else {
          fgets (contenido, MAXIMO_CHAR*2, fichero);
          fclose(fichero);
          printf("%s", contenido);
        }
      } else if(strcmp(comandos[0], "help") == 0){
        
        char *ayuda = "~$ Comandos disponibles\n\nCrear un directorio: mkdir [ubicacion] [nombre]\nRemover un directorio: rmdir [ubicacion] [nombre]\nCrear un archivo: mkfile [ubicacion] [nombre.formato]
          \nListar el contenido de un directorio: ldir [ubicacion]\nListar el contenido de un archivo: lfile [ubicacion]";
        printf("%s", ayuda);
        
      } else retorno = -2; // No coincide con ningún comando, por ende, no es un comando válido.
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
	while (fgets(buf, 1024, stdin)) {
    		errorLimite = 0;
        token = strtok(buf, " ");
        int count = 0;
        while (token != 0 && strlen(token) > 1 && errorLimite == 0) {
          	if(count >= TOTAL_INDICES)
              errorLimite = 1;
          	else {
              strcpy(comandos[count], token);
              count++;
              token = strtok(0, " ");
            }
        }
    		comandos[count-1][strlen(comandos[count-1])-1] = '\0'; // El ultimo string contendrá un '\n' al final, se cambia por el caracter finalizador: '\0'
        if(count >= TOTAL_INDICES) ERROR_LIMITE
        else {
            if(count > 0){
              	retornoComandos = ejecutar(comandos);
              	if(retornoComandos == 0){
                   //printf("valido");
                } else if(retornoComandos == -1) ERROR_INVALIDO
                } else if(retornoComandos == -2) ERROR_DECOMANDO
            } 
        }
		printf("\n~$ shell> ");
    }
    return 0;
}
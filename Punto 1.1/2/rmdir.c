//rmdir.c
#include "constantes.h"

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
                   r2 = rmdir(buf); //remove_directory
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

int main (int argc, char *argv[]){
	int retorno = 0;
	char ubicacion[MAXIMO_CHAR*2];
	strcpy(ubicacion, "");
	strcat(ubicacion, argv[1]); // el primer parametro tiene que terminar con /
	strcat(ubicacion, argv[2]);
	retorno = rmdir_notempty(ubicacion);
	exit(retorno);
}
//ldir
#include "constantes.h"
int main (int argc, char *argv[]){
	int retorno = 0;
const char *dir_name;            
			DIR *dir_ptr = NULL;
			struct dirent *dirent_ptr;
			dir_name = argv[1];
			if ((dir_ptr = opendir(dir_name)) == NULL) { retorno = -1; }
			else {
				while ((dirent_ptr = readdir(dir_ptr)) != NULL) {
					if(strcmp(dirent_ptr -> d_name, ".") != 0 && strcmp(dirent_ptr -> d_name, "..") != 0)
					printf("%s\n", dirent_ptr -> d_name);
				}
				if(dir_ptr != NULL) 
					closedir(dir_ptr);
			}
			exit(retorno);
		}
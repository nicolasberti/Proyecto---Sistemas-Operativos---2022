#include "constantes.h"
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
    int qid_cola = msgget(KEY_cola, 0666);
    if(qid_cola == -1)
            printf("RECOLECTOR: Error recuperando cola\n");
    int i = 0;
    while(1){
		srand( time(NULL)+(i)); // Semilla del rand, hago que sean distintas
        char basura = generarBasura();
        int reciclado;
        switch(basura){
            case 'P': { reciclado = 1; break; }
            case 'V': { reciclado = 2; break; }
            case 'A': { reciclado = 3; break; }
            case 'C': { reciclado = 4 ; break; }
        }
		struct basuramsg m;
		m.tipo = reciclado;
        m.msg = basura;
        if(msgsnd(qid_cola, &m, ( sizeof(struct basuramsg) - sizeof(long)), 0) == -1)
			printf("\nRECOLECTOR: Error al enviar mensaje en writer.\n");
		printf("\nRECOLECTOR: Recolector mete: %c\n", basura);
        sleep(5); // Producen cada 5 segundo, para no sobrecargar la shell
    }
    exit(0);
} 

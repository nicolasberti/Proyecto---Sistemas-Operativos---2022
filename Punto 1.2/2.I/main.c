#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

// Para simular un while(1) habria que poner un numero grande
// o cambiar el for por un while(1).
#define CANTIDAD_AUTOS 6

sem_t puenteDisponible, autosNorte, autosSur;
pthread_mutex_t  mutexNorte, mutexSur;

void *autoNorte(void *data){
	int *id= (int *)data;
	/* Sección de entrada 
		1. Puedo entrar si hay alguien de mi sentido en el puente.
		2. Puedo entrar como primero, esto es, no hay nadie cruzando el puente.
	*/ 
	pthread_mutex_lock(&mutexNorte);	
	if( sem_trywait(&autosNorte) != -1 ) { // Un auto proveniente del norte está circulando sobre el puente.
		printf("autoNorte %i: hay alguien pasando, paso con el\n", *id);
		sem_post(&autosNorte); // Indico que estoy pasando el puente
		sem_post(&autosNorte); // Agrego al que saqué
	pthread_mutex_unlock(&mutexNorte);  
	} else { // Caso en el que soy el primero en pasar
		sem_wait(&puenteDisponible);	
		// No libero el mutex hasta que pase como "primero". Si no se puede dar el caso de que el auto de atras pase como primero y no me siga.
		printf("autoNorte %i: soy el primero en pasar\n", *id);
		sem_post(&autosNorte);
		pthread_mutex_unlock(&mutexNorte);
	}
	
	/* Cruzando el puente */
    printf("autoNorte %i: pasando el puente...\n", *id);
	sleep(5);
	
	/* Sección de salida */
	printf("autoNorte %i: pase el puente\n", *id);
	/* Compruebo si hay alguien atrás mio (i.e soy el último?) */
	pthread_mutex_lock(&mutexNorte); // Solo se puede comprobar de un vehiculo a la vez.
		sem_wait(&autosNorte); // Me bajo del puente
		if( sem_trywait(&autosNorte) == -1 ) {
			sem_post(&puenteDisponible); // No hay nadie sobre el puente (i.e era el último), puedo liberar.
			printf("autoNorte %i: nadie me siguio\n", *id);
		} else { sem_post(&autosNorte); printf("autoNorte %i: me siguieron\n", *id); }
	pthread_mutex_unlock(&mutexNorte);
	
	pthread_exit(NULL);	
}

void *autoSur(void *data){
	int *id= (int *)data;
	/* Sección de entrada 
		1. Puedo entrar si hay alguien de mi sentido en el puente.
		2. Puedo entrar como primero, esto es, no hay nadie cruzando el puente.
	*/ 
	pthread_mutex_lock(&mutexSur);	
	if( sem_trywait(&autosSur) != -1 ) { // Un auto proveniente del sur está circulando sobre el puente.
		printf("autoSur %i: hay alguien pasando, paso con el\n", *id);
		sem_post(&autosSur); // Indico que estoy pasando el puente
		sem_post(&autosSur); // Agrego al que saqué
	pthread_mutex_unlock(&mutexSur);  
	} else { // Caso en el que soy el primero en pasar
		sem_wait(&puenteDisponible);	
		// No libero el mutex hasta que pase como "primero". Si no se puede dar el caso de que el auto de atras pase como primero y no me siga.
		printf("autoSur %i: soy el primero en pasar\n", *id);
		sem_post(&autosSur);
		pthread_mutex_unlock(&mutexSur);
	}
	
	/* Cruzando el puente */
    printf("autoSur %i: pasando el puente...\n", *id);
	sleep(5);
	
	/* Sección de salida */
	printf("autoSur %i: pase el puente\n", *id);
	/* Compruebo si hay alguien atrás mio (i.e soy el último?) */
	pthread_mutex_lock(&mutexSur); // Solo se puede comprobar de un vehiculo a la vez.
		sem_wait(&autosSur); // Me bajo del puente
		if( sem_trywait(&autosSur) == -1 ) {
			sem_post(&puenteDisponible); // No hay nadie sobre el puente (i.e era el último), puedo liberar.
			printf("autoSur %i: nadie me siguio\n", *id);
		} else { sem_post(&autosSur); printf("autoSur %i: me siguieron\n", *id); }
	pthread_mutex_unlock(&mutexSur);
	
	pthread_exit(NULL);	
}


/* Función principal */
int main(){
	pthread_mutex_t  mutexNorte = PTHREAD_MUTEX_INITIALIZER;  
	pthread_mutex_t  mutexSur = PTHREAD_MUTEX_INITIALIZER;  
	//sem_init(&mutexNorte, 0, 1);
  	//sem_init(&mutexSur, 0, 1);
	sem_init(&puenteDisponible, 0, 1);
	sem_init(&autosNorte, 0, 0);
	sem_init(&autosSur, 0, 0);
	
	pthread_t hilos[CANTIDAD_AUTOS];
  for(int i = 0; i < CANTIDAD_AUTOS; i++){
  	int *numero;
    numero = malloc(sizeof(int));
    *numero = i;
  	time_t t;
    srand((unsigned) time(&t)); 
    switch(rand() % 2){
      case 0: { printf("PADRE: auto NORTE id %i generado\n\n", *numero); pthread_create(&hilos[i], NULL, autoNorte, (void *)(numero)); break; }
      case 1: { printf("PADRE: auto SUR id %i generado\n\n", *numero); pthread_create(&hilos[i], NULL, autoSur, (void *)(numero)); break; }
    }
    sleep(2);
  }
	
	for(int i = 0; i < CANTIDAD_AUTOS; i++)
		pthread_join(hilos[i], NULL);
		
	printf("\n--FIN--\n");
	exit(0);
}

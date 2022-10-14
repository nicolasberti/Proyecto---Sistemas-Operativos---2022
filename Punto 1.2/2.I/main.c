#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define CANTIDAD_AUTOS 6

sem_t puenteDisponible, autosNorte, autosSur, mutexNorte, mutexSur; 

void *autoNorte(void *data){
	int *id= (int *)data;
	sem_wait(&mutexNorte); // Exclusión mutua para fijarse si hay alguien pasando
	if( sem_trywait(&autosNorte) != -1 ) { // Un auto proveniente del norte está circulando sobre el puente.
		printf("autoNorte %i: hay alguien pasando, paso con el\n", *id);
		sem_post(&autosNorte); // Indico que estoy pasando el puente
		sem_post(&autosNorte); // Agrego al que saqué
    sem_post(&mutexNorte);
		
    printf("autoNorte %i: pasando el puente...\n", *id);
		sleep(5);
		printf("autoNorte %i: pase el puente\n", *id);
    
		sem_wait(&mutexNorte);
		sem_wait(&autosNorte); // me bajo del puente
		if( sem_trywait(&autosNorte) == -1 ) {
			sem_post(&puenteDisponible); // No hay nadie sobre el puente (i.e era el último), puedo liberar.
			printf("autoNorte %i: nadie me siguio\n", *id);
		} else { sem_post(&autosNorte); printf("autoNorte %i: me siguieron\n", *id); }
		sem_post(&mutexNorte);
    
		pthread_exit(NULL);	
	}
	sem_wait(&puenteDisponible);	
  printf("autoNorte %i: soy el primero en pasar\n", *id);
	sem_post(&autosNorte);
  sem_post(&mutexNorte);
  
  printf("autoNorte %i: pasando el puente...\n", *id);
  sleep(5);
  printf("autoNorte %i: pase el puente\n", *id);
    
  	sem_wait(&mutexNorte);
		sem_wait(&autosNorte); // me bajo del puente
		if( sem_trywait(&autosNorte) == -1 ) {
			sem_post(&puenteDisponible); // No hay nadie sobre el puente (i.e era el último), puedo liberar.
			printf("autoNorte %i: nadie me siguio\n", *id);
		} else { sem_post(&autosNorte); printf("autoNorte %i: me siguieron\n", *id); }
		sem_post(&mutexNorte);
    
	pthread_exit(NULL);
}

void *autoSur(void *data){
	int *id= (int *)data;
	sem_wait(&mutexSur); // Exclusión mutua para fijarse si hay alguien pasando
	if( sem_trywait(&autosSur) != -1 ) { // Un auto proveniente del norte está circulando sobre el puente.
		printf("autoSur %i: hay alguien pasando, paso con el\n", *id);
		sem_post(&autosSur); // Indico que estoy pasando el puente
		sem_post(&autosSur); // Agrego al que saqué
    sem_post(&mutexSur);
		
    printf("autoSur %i: pasando el puente...\n", *id);
		sleep(5);
		printf("autoSur %i: pase el puente\n", *id);
    
		sem_wait(&mutexSur);
  sem_wait(&autosSur); // me bajo del puente
	if( sem_trywait(&autosSur) == -1 ) {
			sem_post(&puenteDisponible); // No hay nadie sobre el puente (i.e era el último), puedo liberar.
			printf("autoSur %i: nadie me siguio\n", *id);
	} else { sem_post(&autosSur); printf("autoSur %i: me siguieron\n", *id); }
	sem_post(&mutexSur);
    
		pthread_exit(NULL);	
	}
	sem_wait(&puenteDisponible);	
  printf("autoSur %i: soy el primero en pasar\n", *id);
	sem_post(&autosSur);
  sem_post(&mutexSur);
  
  printf("autoSur %i: pasando el puente...\n", *id);
  sleep(5);
  printf("autoSur %i: pase el puente\n", *id);
    
  sem_wait(&mutexSur);
  sem_wait(&autosSur); // me bajo del puente
	if( sem_trywait(&autosSur) == -1 ) {
			sem_post(&puenteDisponible); // No hay nadie sobre el puente (i.e era el último), puedo liberar.
			printf("autoSur %i: nadie me siguio\n", *id);
	} else { sem_post(&autosSur); printf("autoSur %i: me siguieron\n", *id); }
	sem_post(&mutexSur);
    
	pthread_exit(NULL);
}

int main(){
	sem_init(&mutexNorte, 0, 1);
  sem_init(&mutexSur, 0, 1);
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

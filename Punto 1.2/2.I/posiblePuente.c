#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// Falta testear... No se si funciona...

sem_t puenteDisponible, autosNorte, autosSur, mutex; // Tendria que haber un mutexSur y un mutexNorte ??

void *autoNorte(void *data){
	int * id= (int *) data;
	wait(mutex); // Me fijo si hay alguien pasando el puente
	if( waitTry(autosNorte) != -1 ) { // Un auto proveniente del norte está circulando sobre el puente.
		printf("autoNorte %i: hay alguien pasando, paso con el\n", *id);
		// "Paso con él"
		signal(autoNorte); // Indico que estoy pasando el puente
		signal(mutex);
		// Terminé de pasar. ¿Alguien me siguió?
		sleep(2);
		printf("autoNorte %i: pase el puente\n", *id);
		wait(mutex);
		if( waitTry(autosNorte) != -1 ) {
			signal(puenteDisponible); // Entro si nadie me siguió, i.e soy el último.
			printf("autoNorte %i: nadie me siguio\n", *id);
		} else printf("autoNorte %i: me siguieron\n", *id);
		signal(mutex);
		pthread_exit(NULL);	
	}
	signal(mutex);
	wait(puenteDisponible);	
	printf("autoNorte %i: soy el primero en pasar\n", *id);
	// Paso el puente, doy la señal que estoy pasando
	signal(autosNorte);
	wait(mutex);
	if( waitTry(autosNorte) != -1 ) {
			signal(puenteDisponible); // Entro si nadie me siguió, i.e soy el último.
			printf("autoNorte %i: nadie me siguio\n", *id);
	} else printf("autoNorte %i: me siguieron\n", *id);
	signal(mutex);
	pthread_exit(NULL);
}

void *autoSur(void *data){
	int * id= (int *) data;
	wait(mutex); // Me fijo si hay alguien pasando el puente
	if( waitTry(autosSur) != -1 ) { // Un auto proveniente del norte está circulando sobre el puente.
		printf("autoSur %i: hay alguien pasando, paso con el\n", *id);
		// "Paso con él"
		signal(autosSur); // Indico que estoy pasando el puente
		signal(mutex);
		// Terminé de pasar. ¿Alguien me siguió?
		sleep(2);
		printf("autoSur %i: pase el puente\n", *id);
		wait(mutex);
		if( waitTry(autosSur) != -1 ) {
			signal(puenteDisponible); // Entro si nadie me siguió, i.e soy el último.
			printf("autoSur %i: nadie me siguio\n", *id);
		} else printf("autoSur %i: me siguieron\n", *id);
		signal(mutex);
		pthread_exit(NULL);	
	}
	signal(mutex);
	wait(puenteDisponible);	
	printf("autoSur %i: soy el primero en pasar\n", *id);
	// Paso el puente, doy la señal que estoy pasando
	signal(autosSur);
	wait(mutex);
	if( waitTry(autosSur) != -1 ) {
			signal(puenteDisponible); // Entro si nadie me siguió, i.e soy el último.
			printf("autoSur %i: nadie me siguio\n", *id);
	} else printf("autoSur %i: me siguieron\n", *id);
	signal(mutex);
	pthread_exit(NULL);
}

int main(){
	sem_init(&mutex, 0, 1);
	sem_init(&puenteDisponible, 0, 1);
	sem_init(&autosNorte, 0, 0);
	sem_init(&autosSur, 0, 0);
	
	pthread_t hilos[7];
	int *numero;
    numero = malloc(6*sizeof(int));
	// N N N S S N S
	*numero = 1;
	pthread_create(&hilos[0], NULL, autoNorte, (void *)numero);
	*numero = 2;
	pthread_create(&hilos[1], NULL, autoNorte, (void *)numero);
	*numero = 3;
	pthread_create(&hilos[2], NULL, autoNorte, (void *)numero);
	
	*numero = 4;
	pthread_create(&hilos[3], NULL, autoSur, (void *)numero);
	*numero = 5;
	pthread_create(&hilos[4], NULL, autoSur, (void *)numero);
	
	*numero = 6;
	pthread_create(&hilos[5], NULL, autoNorte, (void *)numero);
	
	*numero = 7;
	pthread_create(&hilos[6], NULL, autoSur, (void *)numero);
	
	for(int i = 0; i < 7; i++)
		pthread_join(hilos[i], NULL);
		
	printf("\n--OK FIN--\n");
	exit(0);
}
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

// Secuencia: .. AB(DoE)AC(DoE)F .. ( Se repite -> while(1) )

// Semaforos
sem_t sem_A, sem_B, sem_C, sem_DE, sem_F, sem_FA;

	
// Agregar wait and signal según la foto de wpp 
// Agregar ciclos while
void * A(){
	while(1){
		sem_wait(&sem_FA);
		sem_wait(&sem_A);
		printf("A");
		sem_post(&sem_B);
		sem_post(&sem_C);
	}
}
void * B(){
	while(1){
		sem_wait(&sem_B);
		sem_wait(&sem_B);
		printf("B");
		sem_post(&sem_DE);
	}
}

void * D(){
	while(1){
		sem_wait(&sem_DE); // D y E compiten por este semaforo
		printf("D");
		sem_post(&sem_A);
		sem_post(&sem_F);
	}
}
void * E(){
	while(1){
		sem_wait(&sem_DE);
		printf("E");
		sem_post(&sem_A);
		sem_post(&sem_F);
	}
}

void * C(){
	while(1){
		sem_wait(&sem_C);
		sem_wait(&sem_C);
		printf("C");
		sem_post(&sem_DE);
	}
}

void * F(){
	while(1){
		sem_wait(&sem_F);
		sem_wait(&sem_F);
		printf("F\n");
  		sleep(2); // Ponemos este sleep para que se vayan generando las secuencias y no se sobrecargue la terminal
		sem_post(&sem_FA);
		sem_post(&sem_FA);
	}
}

int main(){
	sem_init(&sem_A, 1, 1);
	sem_init(&sem_B, 1, 1);
	sem_init(&sem_C, 1, 0);
	sem_init(&sem_DE, 1, 0);
	sem_init(&sem_FA, 1, 2);
	sem_init(&sem_F, 1, 0);
	
	pthread_t hilos[6];
	pthread_create(&(hilos[0]), NULL, A, NULL);
	pthread_create(&(hilos[1]), NULL, B, NULL);
	pthread_create(&(hilos[2]), NULL, C, NULL);
	pthread_create(&(hilos[3]), NULL, D, NULL);
	pthread_create(&(hilos[4]), NULL, E, NULL);
	pthread_create(&(hilos[5]), NULL, F, NULL);
	for(int i = 0; i < 6; i++)
		pthread_join(hilos[i], NULL);
	exit(0);
}

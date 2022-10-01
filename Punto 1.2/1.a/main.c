#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

// Semaforos

// Secuencia: .. AB(DoE)AC(DoE)F .. ( Se repite -> while(1) )
	
// Agregar wait and signal según la foto de wpp 
// Agregar ciclos while
void A(){
	printf("A");
}
void B(){
	
	printf("B");
}
void C(){
	
	printf("C");
}
void D(){
	printf("D");
}
void E(){
	printf("E");
}
void F(){
	printf("F\n");
}

// Crear hilos
int main(){
	// Para limitar se podría hacer 5 veces la secuencia con un contador (asi no se sobrecarga la shell)
	exit(0);
}
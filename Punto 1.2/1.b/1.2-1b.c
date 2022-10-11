#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/wait.h>

// Secuencia: .. AB(DoE)AC(DoE)F .. ( Se repite -> while(1) )

int main(){
    int semA[2], semB[2], semC[2], semDE[2], semF[2], semFA[2];

    if(pipe(semA) < 0) exit(1);
    if(pipe(semB) < 0) exit(1);
    if(pipe(semC)< 0) exit(1);
    if(pipe(semDE)< 0) exit(1);
    if(pipe(semF)< 0) exit(1);
    if(pipe(semFA)< 0) exit(1);

    int numero;

    pid_t pid;
    pid = fork();
    if(pid == 0){
        close(semFA[1]);
        close(semA[1]);
        close(semB[0]);
        close(semC[0]);
        close(semDE[0]);
        close(semDE[1]);
        close(semF[0]);
        close(semF[1]);
        while(1){
            read(semFA[0], &numero, sizeof(numero));
            read(semA[0], &numero, sizeof(numero));
            printf("A\n");
            write(semB[1], &numero, sizeof(numero));
            write(semC[1], &numero, sizeof(numero));
        }
        exit(0);
    }

    pid = fork();
    if(pid == 0){
        close(semB[1]);
        close(semA[0]);
        close(semA[1]);
        close(semC[0]);
        close(semC[1]);
        close(semDE[0]);
        close(semF[0]);
        close(semF[1]);
        close(semFA[0]);
        close(semFA[1]);
        while(1){
            read(semB[0], &numero, sizeof(numero));
            read(semB[0], &numero, sizeof(numero));
            printf("B\n");
            write(semDE[1], &numero, sizeof(numero));
        }
        exit(0);
    }

    pid = fork();
    if(pid == 0){
        close(semB[0]);
        close(semB[1]);
        close(semA[0]);
        close(semA[1]);
        close(semC[1]);
        close(semDE[0]);
        close(semF[0]);
        close(semF[1]);
        close(semFA[0]);
        close(semFA[1]);
        while(1){
            read(semC[0], &numero, sizeof(numero));
            read(semC[0], &numero, sizeof(numero));
            printf("C\n");
            write(semDE[1], &numero, sizeof(numero));
        }
        exit(0);
    }

    pid = fork();
    if(pid == 0){
        close(semB[0]);
        close(semB[1]);
        close(semA[0]);
        close(semC[0]);
        close(semC[1]);
        close(semDE[1]);
        close(semF[0]);
        close(semFA[0]);
        close(semFA[1]);
        while(1){
            read(semDE[0], &numero, sizeof(numero));
            printf("D\n");
            write(semA[1], &numero, sizeof(numero));
            write(semF[1], &numero, sizeof(numero));
        }
        exit(0);
    }

    pid = fork();
    if(pid == 0){
        close(semB[0]);
        close(semB[1]);
        close(semA[0]);
        close(semC[0]);
        close(semC[1]);
        close(semDE[1]);
        close(semF[0]);
        close(semFA[0]);
        close(semFA[1]);
        while(1){
            read(semDE[0], &numero, sizeof(numero));
            printf("E\n");
            write(semA[1], &numero, sizeof(numero));
            write(semF[1], &numero, sizeof(numero));
        }
        exit(0);
    }

    pid = fork();
    if(pid == 0){
        close(semB[0]);
        close(semB[1]);
        close(semA[0]);
        close(semA[1]);
        close(semC[0]);
        close(semC[1]);
        close(semDE[0]);
        close(semDE[1]);
        close(semF[1]);
        close(semFA[0]);
        while(1){
            read(semF[0], &numero, sizeof(numero));
            read(semF[0], &numero, sizeof(numero));
            printf("F\n");
            sleep(2);
            write(semFA[1], &numero, sizeof(numero));
            write(semFA[1], &numero, sizeof(numero));
        }
        exit(0);
    }

    /* Inicializo los "semaforos" -> Le mando algo por el pipe */

    close(semDE[0]);
    close(semDE[1]);
    close(semF[0]);
    close(semF[1]);
    close(semC[0]);
    close(semC[1]);

    close(semFA[0]);
    close(semA[0]);
    close(semB[0]);

    write(semFA[1], &numero, sizeof(int));
    write(semFA[1], &numero, sizeof(int));
    close(semFA[1]);

    write(semA[1], &numero, sizeof(int));
    close(semA[1]);

    write(semB[1], &numero, sizeof(int));
    close(semB[1]);

	for(int i=0;i<6;i++)
		wait(NULL);

    exit(0);

}


#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h> 
#include <time.h> 
#include "inicializador.h"


#define CELDA_SIZE 45
#define SNAME "rw_mutex"

u_int16_t MEMORY_SIZE;
key_t KEY = 54609;

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Se debe especificar la cantidad de celdas que tendra la memoria:\n");
        printf("./inicializador <Cantidad_Celdas> \n");
        exit(1);
    }
    else if((MEMORY_SIZE = atoi(argv[1])) < 1){
        printf("La cantidad de celdas debe ser un numero positivo. \n");
        exit(1);
    }
    crearMemComp();
    
    crearSemaforos();
    return 0;
}

void crearMemComp(){
    int size_total = CELDA_SIZE*MEMORY_SIZE;
    int shmid = shmget(KEY, size_total, IPC_CREAT | 0666);
    if(shmid < 0 ){
        perror("shmget");
        exit(1);
    }
    printf("el id es %d \n", shmid);
    char* shm = shmat(shmid,NULL,0);
    if(shm == (char *) -1){
        perror("shmat");
        exit(1);
    }
    char* s = shm+size_total-1;
    *s = '$';
}

void crearSemaforos(){
    sem_t *rw_mutex;
    if((rw_mutex = sem_open(SNAME, O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror ("sem_open"); 
        exit (1);
    }
    /*if (sem_unlink (SNAME) == -1) {
        perror ("sem_unlink"); 
        exit (1);
    }*/
}
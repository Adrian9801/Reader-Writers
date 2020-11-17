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
#include "writers.h"

#define CELDA_SIZE 45
#define SNAME "rw_mutex"

u_int16_t MEMORY_SIZE;
key_t KEY = 54609;
sem_t *rw_mutex;
int  cantProcesos;
int  tiempoDurmiendo;
int  tiempoEscribiendo;
char* shm;

int main(int argc, char const *argv[])
{
    if (argc < 4)
    {
        printf("Se debe especificar la cantidad de procesos, tiempo escribiendo y tiempo durmiendo:\n");
        printf("./writers <Cantidad_Procesos> <Tiempo_Escribiendo> <Tiempo_Durmiendo> \n");
        exit(1);
    }
    cantProcesos = atoi(argv[1]);
    tiempoEscribiendo = atoi(argv[2]);
    tiempoDurmiendo = atoi(argv[3]);
    obtenerSemaforo();
    obtenerMemComp();
    for (size_t i = 0; i < cantProcesos; i++)
    {
        pthread_t hilo;
        pthread_create(&hilo, NULL, correr, NULL);

    }
    while(true);
    return 0;
}


void *correr(){
    pthread_t hilo;
    pthread_create(&hilo, NULL, crearProceso, NULL);
    pthread_join(hilo,NULL);
}

void *crearProceso(){
    Process* process = createProcess(1,"Ni idea");
    escribir(process);
    process->state = "Durmiendo.";
    sleep(tiempoDurmiendo);
}

void escribir(Process* process){
    sem_wait (rw_mutex);
    char text[44];
    int count = 0;
    printf("Nuevo proceso entrando... \n");
    for (char* s = shm; *s != '$';)
    {
        if(s != shm)
            s++;
        if(*s != '-'){
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char procesoChar[22];
            char timeChar[22];
            sprintf(procesoChar, "-PID: %d, linea: %d, ", process->pid, count);
            strftime(timeChar, sizeof(timeChar)-1,"%Y/%m/%d %H:%M:%S*", t);
            strcpy(text, procesoChar);
            strcat(text, timeChar);
            printf("%s \n", text);
            memcpy(s,text,sizeof(text));
            break;
        }
        else{
            printf("Un F \n");
        }
        count++;
        s+= CELDA_SIZE-1;
    }
    sleep(tiempoEscribiendo);
    sem_post (rw_mutex);
}

void obtenerSemaforo(){
    if((rw_mutex = sem_open(SNAME, 0)) == SEM_FAILED) {
        perror ("sem_open"); 
        exit (1);
    }
}

void obtenerMemComp(){
    int shmid = shmget(KEY, 0, 0666);
    if(shmid < 0 ){
        perror("shmget");
        exit(1);
    }
    shm = shmat(shmid,NULL,0);
    if(shm == (char *) -1){
        perror("shmat");
        exit(1);
    }
}
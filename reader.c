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
#include "reader.h"


#define CELDA_SIZE 45
#define SNAME "rw_mutex"

u_int16_t MEMORY_SIZE;
key_t KEY = 54609;
sem_t *rw_mutex;
int  cantProcesos;
int  tiempoLeyendo;
int  tiempoDurmiendo;
int readCount=0;
int PID=50;
char* shm;
pthread_mutex_t lock; 

int main(int argc, char const *argv[])
{
    if (argc < 4)
    {
        printf("Se debe especificar la cantidad de readers, tiempo leyendo y tiempo durmiendo:\n");
        printf("./reader <Cantidad_Procesos> <Tiempo_Leyendo> <Tiempo_Durmiendo> \n");
        exit(1);
    }
    cantProcesos = atoi(argv[1]);
    tiempoLeyendo = atoi(argv[2]);
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
    Process* process = createProcess(PID,"Esperando");
    PID++;
    leer(process);
    process->state = "Durmiendo.";
    sleep(tiempoDurmiendo);
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
void leer(Process* process){
    
    pthread_mutex_lock(&lock); 
    readCount++;
    if(readCount==1){
        sem_wait (rw_mutex);
    }
    pthread_mutex_unlock(&lock);
    //Lectura
    for (char* s = shm; *s != '$';)
    {
        if(s != shm)
            s++;
        if(*s=='-'){
            char linea[400];
            while(*s!='*'){
                //strcat(linea,s);
                int len = strlen(linea);
                linea[len] = *s;
                //linea[len+1] = '\0';
                s++;
            }
            printf("Leyendo PID Reader: %d \n", process->pid);
            printf("%s \n",linea);
            sleep(tiempoLeyendo);
        }
    }
    obtenerMemComp();

    pthread_mutex_lock(&lock); 
    readCount--;
    if(readCount==0){
        sem_post (rw_mutex);
    }
    pthread_mutex_unlock(&lock); 
    
    
}
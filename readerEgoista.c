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
#include "readerEgoista.h"


#define CELDA_SIZE 50
#define SNAME "rw_mutex"
#define SNAMEE "re_mutex"

u_int16_t MEMORY_SIZE;
key_t KEY = 54609;
sem_t *rw_mutex;
int  cantProcesos;
int  tiempoLeyendo;
int  tiempoDurmiendo;
int readCount = 0;
int PID = 100;
int maxRand = 1;
char* shm;
int contador = 0;
bool completo = false;
pthread_mutex_t lockEgoista; 
pthread_mutex_t lockEgoista2; 
pthread_mutex_t lockPID; 
sem_t *re_mutex;
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
    time_t t;
    srand((unsigned) time(&t));
    re_mutex = sem_open(SNAMEE, O_CREAT, 0644, 3);
    obtenerSemaforo();
    obtenerMemComp();
    for (size_t i = 0; i < cantProcesos; i++)
    {
        pthread_t hilo;
        pthread_create(&hilo, NULL, correr, NULL);

    }
    sem_t *process_sem = sem_open("process_sema", 0);
    sem_wait (process_sem);
    sem_post (process_sem);
    return 0;
}
void *correr(){
    pthread_t hilo;
    pthread_create(&hilo, NULL, crearProceso, NULL);
    pthread_join(hilo,NULL);
}

void *crearProceso(){
    pthread_mutex_lock(&lockPID); 
    Process* process = createProcess(PID,"Esperando.");
    PID++;
    pthread_mutex_unlock(&lockPID); 
    while(true){
        leer(process);
        process->state = "Durmiendo.";
        sleep(tiempoDurmiendo);
    }
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
    process->state = "Esperando.";
    sem_wait (re_mutex);
    sem_wait (rw_mutex); 
    pthread_mutex_lock(&lockEgoista);
    contador++;
    pthread_mutex_unlock(&lockEgoista);

    //Lectura
    if(maxRand == 1){
        char* se = shm;
        se += 49;
        while(*se != '$'){
            maxRand++;
            se += 50;
        }
    }
    int archLinea = (rand() % maxRand)*50;
    char* s = shm;
    s += archLinea;
    if(*s=='-'){
        *s = 0;
        s++;
        char linea[50];
        memset(linea, 0, 50);
        while(*s!='*'){
            int len = strlen(linea);
            linea[len] = *s;
            *s = 0;
            s++;
        }
        *s = 0;
        printf("Leyendo PID Reader Egoista: %d \n", process->pid);
        printf("%s \n",linea);
        sleep(tiempoLeyendo);
        memset(linea, 0, 50);
    }
    else{
        printf("Leyendo PID Reader Egoista: %d \n", process->pid);
        printf("La linea estaba vacia.\n");
        sleep(tiempoLeyendo);
    }
    printf("Contador: %d\n",contador);
    sem_post (rw_mutex);
    pthread_mutex_lock(&lockEgoista2);
    if(contador == 3){
        contador = 0;
        sem_post (re_mutex);
        sem_post (re_mutex);
        sem_post (re_mutex);
    }
    pthread_mutex_unlock(&lockEgoista2);
}
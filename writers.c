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

#define CELDA_SIZE 50
#define SNAME "rw_mutex"

u_int16_t MEMORY_SIZE;
key_t KEY = 54609;
key_t KEYPROCESOS = 54608;
int sizeMemory = 300;
sem_t *rw_mutex;
sem_t *wr_mutex;
int  cantProcesos;
int  tiempoDurmiendo;
int  tiempoEscribiendo;
pthread_mutex_t lock; 
pthread_mutex_t lock2; 
pthread_mutex_t lockEstado; 
char* shm;
char* shmp;
int pID = 1;
char* sh;

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
    wr_mutex = sem_open("wr_mutex", O_CREAT, 0644, 3);
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
    sem_wait (wr_mutex); 
    Process* process = createProcess(pID,"Esperando.");
    pID++;
    char procesoChar[10];
    sprintf(procesoChar, "W:%d,%d", process->pid,0);
    memcpy(sh,procesoChar,sizeof(procesoChar));
    sh+=10;
    sem_post (wr_mutex);
    while(true){
        escribir(process);
        process->state = "Durmiendo.";
        pthread_mutex_lock(&lockEstado);
        cambiarEstado(process->pid, '2');
        pthread_mutex_unlock(&lockEstado);
        sleep(tiempoDurmiendo);
    }
}

void escribir(Process* process){
    process->state = "Esperando.";
    pthread_mutex_lock(&lockEstado);
    cambiarEstado(process->pid, '0');
    pthread_mutex_unlock(&lockEstado);
    sem_wait (rw_mutex);
    process->state = "Ejecutando.";
    pthread_mutex_lock(&lockEstado);
    cambiarEstado(process->pid, '1');
    pthread_mutex_unlock(&lockEstado);
    char text[50];
    int count = 0;
    for (char* s = shm; *s != '$';)
    {
        int info = 0;
        if(s != shm)
            s++;
        if(*s != '-'){
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char procesoChar[25];
            char timeChar[25];
            printf("Escribiendo PID Writer: %d \n", process->pid);
            sprintf(procesoChar, "-PID: %d, linea: %d, ", process->pid, count);
            strftime(timeChar, sizeof(timeChar)-1,"%Y/%m/%d %H:%M:%S*", t);
            strcpy(text, procesoChar);
            strcat(text, timeChar);
            printf("%s \n", text);
            memcpy(s,text,sizeof(text));
            break;
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
    int shmidp = shmget(KEYPROCESOS, 0, 0666);
    shmp = shmat(shmidp,NULL,0);
    sh = shmp;
    while(*sh == 'R' || *sh == 'E'){
        sh += 10;
    }
}

void cambiarEstado(int pId, char pState){
    char* ss = shmp;
    while(*ss == 'R' || *ss == 'E'){
        ss += 10;
    }
    while(*ss == 'W'){
        char idProceso[3];
        memset(idProceso, 0, 3);
        int info = 3;
        ss+=2;
        while(*ss != ','){
            int len = strlen(idProceso);
            idProceso[len] = *ss;
            ss++;
            info++;
            if(info > 5)
                break;
        }
        ss++;
        int id = atoi(idProceso);
        if(id == pId){
            *ss = pState;
            break;
        }
        while(*ss != 'W'){
            ss++;
        }
        memset(idProceso, 0, 3);
    }
}
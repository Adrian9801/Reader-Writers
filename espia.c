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

u_int16_t MEMORY_SIZE;
key_t KEY = 54609;
key_t KEYPROCESOS = 54608;
sem_t *mc_mutex;
int option = 0;
char* shm;
char* shmp;
char *sh;
int main(int argc, char const *argv[])
{
    obtenerMemComp();
    mc_mutex = sem_open("memoria_compartida", 0);
    do{
        printf("\nEscoge el la opcion deseada: \n");
        printf("0. Estado del archivo\n");
        printf("1. Estado de los writers\n");
        printf("2. Estado de los readers\n");
        printf("3. Estado de los readers egoista\n");
        printf("4. Salir\n");
        printf("Digite el numero de la accion deseada: ");
        scanf("%i", &option);

        if(option == 0){
            mostrarEstadoArchivo();
        }
        else if(option == 1){
            mostrarEstadoWriters();
        }
        else if(option == 2){
            mostrarEstadoReader();
        }
        else if(option == 3){
            mostrarEstadoReaderEgoista();
        }
        else if(option != 4)
        {
            printf("La opcion digitada no existe.\n");
        }
    }while(option != 4);

    return 0;
}

void mostrarEstadoArchivo(){
    int numLinea = 0;
    for (char* s = shm; *s != '$';)
    {
        int info = 0;
        printf("Linea: %d\n",numLinea);
        if(s != shm)
            s++;
        if(*s=='-'){
            s++;
            info = 1;
            char linea[50];
            memset(linea, 0, 50);
            while(*s != '*'){
                int len = strlen(linea);
                linea[len] = *s;
                s++;
                info++;
            }
            printf("%s \n",linea);
            memset(linea, 0,50);
            s-= info;
        }
        else
            printf("Esta vacia \n");
        numLinea++;
        s+= 49;
    }
}

void mostrarEstadoWriters(){
    sh = shmp;
    while(*sh == 'R' || *sh == 'E'){
        sh += 10;
    }
    char* states[3] = {"Esperando","Escribiendo","Durmiendo"};
    while(*sh == 'W'){
        char idProceso[3];
        memset(idProceso, 0, 3);
        while(*sh != ','){
            int len = strlen(idProceso);
            idProceso[len] = *sh;
            sh++;
        }
        sh++;
        char state = *sh;
        int numero =  state - '0';
        printf("El poceso con PID Writer: %s esta: %s\n", idProceso,states[numero]);
        while(*sh != 'W'){
            sh--;
        }
        sh+=10;
        memset(idProceso, 0, 3);
    }
}

void mostrarEstadoReader(){
    sh = shmp;
    while(*sh == 'W' || *sh == 'E'){
        sh += 10;
    }
    char* states[3] = {"Esperando","Leyendo","Durmiendo"};
    sem_wait (mc_mutex);
    while(*sh == 'R'){
        char idProceso[3];
        memset(idProceso, 0, 3);
        sh+=2;
        int info = 3;
        while(*sh != ','){
            int len = strlen(idProceso);
            idProceso[len] = *sh;
            sh++;
            info++;
        }
        sh++;
        char state = *sh;
        int numero =  state - '0';
        printf("El poceso con PID Reader: %s esta: %s\n", idProceso,states[numero]);
        while(*sh != 'R'){
            sh--;
        }
        sh+=10;
        memset(idProceso, 0, 3);
    }
    sem_post (mc_mutex);

}

void mostrarEstadoReaderEgoista(){
    sh = shmp;
    while(*sh == 'W' || *sh == 'R'){
        sh += 10;
    }
    char* states[3] = {"Esperando","Leyendo","Durmiendo"};
    sem_wait (mc_mutex);
    while(*sh == 'E'){
        char idProceso[3];
        memset(idProceso, 0, 3);
        sh+=2;
        int info = 3;
        while(*sh != ','){
            int len = strlen(idProceso);
            idProceso[len] = *sh;
            sh++;
            info++;
        }
        sh++;
        char state = *sh;
        int numero =  state - '0';
        printf("El poceso con PID Reader Egoista: %s esta: %s\n", idProceso,states[numero]);
        while(*sh != 'E'){
            sh--;
        }
        sh+=10;
        memset(idProceso, 0, 3);
    }
    sem_post (mc_mutex);

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
}
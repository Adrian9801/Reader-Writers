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
int option = 0;
char* shm;

int main(int argc, char const *argv[])
{
    obtenerMemComp();
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
            
        }
        else if(option == 2){
            
        }
        else if(option == 3){

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
        printf("Linea: %d\n",numLinea);
        if(s != shm)
            s++;
        if(*s=='-'){
            char linea[45];
            memset(linea, 0, 45);
            while(*s != '*'){
                int len = strlen(linea);
                linea[len] = *s;
                s++;
            }
            printf("%s \n",linea);
            memset(linea, 0, 45);
        }
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
#include "finalizador.h"

void deleteSemaforos(){

    sem_t *process_sem = sem_open("process_sema", 0);
    sem_post (process_sem);
    for (int i = 0; i < 4; i++)
    {    
        sem_unlink(semaphores[i]);
    }
    printf("Se detuvieron todos los semaforos\n");
}

void deleteShareMemory(){
    system("ipcrm -M 54609");
    system("ipcrm -M 54608");
};


int main(int argc, char const *argv[]){
    
    deleteSemaforos();
    deleteShareMemory();
    
    return 0;

}
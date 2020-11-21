#include "finalizador.h"

void crearSemaforos(){
    sem_t *rw_mutex;
    if((rw_mutex = sem_open(SNAME, O_CREAT, 0644, 1)) == SEM_FAILED) {
        printf("AS");
        perror ("sem_open"); 
        exit (1);
    }

}

// CREAR ARREGLO Y PUM 
void deleteSemaforos(){
    sem_unlink (SNAME);
}

void deleteShareMemory(){
    system("ipcrm -M 54609");
};


int main(int argc, char const *argv[]){

    crearSemaforos();
    deleteSemaforos();
    //deleteShareMemory();
    return 0;
}
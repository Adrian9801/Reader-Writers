#ifndef _H_READER
#define _H_READER
#include "process.c"

void *crearProceso(void);
void *correr(void);
void obtenerSemaforo(void);
void obtenerMemComp(void);
void leer(Process* process);
#endif
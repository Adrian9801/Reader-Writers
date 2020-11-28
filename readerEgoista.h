#ifndef _H_READEREGOISTA
#define _H_READEREGOISTA
#include "process.c"

void *crearProceso(void);
void *correr(void);
void obtenerSemaforo(void);
void obtenerMemComp(void);
void leer(Process* process);
#endif
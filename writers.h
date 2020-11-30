#ifndef _H_WRITERS
#define _H_WRITERS
#include "process.c"

void *crearProceso(void);
void escribir(Process* process);
void *correr(void);
void obtenerSemaforo(void);
void obtenerMemComp(void);
void cambiarEstado(int pId, char pState);
void abrirArchivo(char *pText,int pId);

#endif
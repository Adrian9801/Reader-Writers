#include "process.h"


Process * createProcess(int pid,char *state){
    Process* process = (Process * )malloc(sizeof(Process));
    process->pid = pid;
    process->state = state;
    return process;
}


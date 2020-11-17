#ifndef PROCESSSTRUCT
#define PROCESSSTRUCT

typedef struct Process
{   
    int pid;
    char *state;
}Process;

Process * createProcess(int pid,char *state );


#endif

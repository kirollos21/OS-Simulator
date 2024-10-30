// protect from multiple compiling
#ifndef SIMULATOR_H
#define SIMULATOR_H

// header files
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "configops.h"
#include "metadataops.h"
#include "datatypes.h"
#include "simtimer.h"






// Fivce state data structure for processes
typedef enum { NEW,
               READY,
               RUNNING,
               BLOCKED,
               EXIT } ProcessState;

typedef struct ProcessControlBoard
{
    ProcessState state;
    int pid;
    int counter;
    int timeLeft;
    int memUpperBound, memLowerBound;
    int headMemAddress;
    OpCodeType *headOfOpCode;
    struct MemAddress *memAddressList; // Use 'struct MemAddress' here
    struct ProcessControlBoard *nextProcess; // Use 'struct ProcessControlBoard' here
} PCB;

typedef enum {
   EMPTY,
   DEQUEUE,
   ENQUEUE
} OperationType;

typedef struct QueueNode {
    PCB *process;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue {
    QueueNode *front, *rear;
} Queue;

// Define your custom types
typedef struct FileString
{
    char currentStr[MAX_STR_LEN];
    struct FileString *nextStr; // Use 'struct FileString' here
} FileStr;

typedef struct MemoryAddress
{
    int address;
    int size;
    struct MemoryAddress *nextAddress; // Use 'struct MemoryAddress' here
} MemAddress;

// prototype functions
FileStr *addToFileList( char *newLine, FileStr *headPtr );
MemAddress *addToMemList(int address, int size, MemAddress *headPtr);
PCB *addToProcessList(int pid, OpCodeType *opCodePtr, PCB *headPtr);
PCB *changeProcessState(PCB *currentPCB, ProcessState newState, char *outStr);
MemAddress *createMemAddress(int address, int size);
PCB *createPCB(int pid, OpCodeType *OpCodePtr);
void decodeOpCode(OpCodeType *currentCode, char *decodedString, const char *opCodeStatus);
void delay(int milliseconds);
PCB *dequeue(Queue *queue);
FileStr *displayOutput(ConfigDataType *configPtr, char *message, FileStr *fileList);
void enqueue(Queue *queue, PCB *process);
bool isQueueEmpty(Queue *queue);
FileStr *freeFileList(FileStr *fileList);
MemAddress *freeMemList(MemAddress *memList);
PCB *freePCBList(PCB *PCBList);
OpCodeType *getNextOpCode(PCB *headPtr);
PCB *getNextProc(ConfigDataType *configPtr, PCB *headPtr);
PCB *getNextProcFCFS(Queue *queue);
void initQueue(Queue *queue);
void logToFile(ConfigDataType *configPtr, FileStr *FileStrList);
int roundRobinQueue(int operation, int processID);
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr );
PCB *searchProcessId(PCB *headptr, int pid);

#endif // SIMULATOR_H

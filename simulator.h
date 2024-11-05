
#ifndef SIMULATOR_H
#define SIMULATOR_H

// header files//
#include <stdlib.h>
#include <pthread.h>
#include "datatypes.h"
#include "simtimer.h"
#include "StringUtils.h"

// Data structures //

//current prototype stack
typedef struct StackStruct
{
    void* data;
    struct StackStruct* prevData;

    int StackSize;
    int StackCapacity;
} Stack;

//current prototype for heap
typedef struct HeapStruct
{
    void* data;
    struct HeapStruct* leftChild;
    struct HeapStruct* rightChild;
} Heap;

typedef struct memoryStruct
{
    int memoryBaseAddress;
    int memoryAllocated;

    struct memoryStruct* nextMemory;
    struct memoryStruct* prevMemory;

} memoryAllocationBlock;

//current type to handle PCB
typedef struct ProcessControlBlockStruct
{
    int PID;
    ProcessState state;
    Stack stack;
    Heap heap;
    void* data;
    void* text;
    int burstTimeCPU;
    int burstTimeIO;
    OpCodeType* programStartNode;

} ProcessControlBlock;

//List for holding pointers to all PCB's
typedef struct ProcessesListStruct
{
    ProcessControlBlock* CurrentProcess;
    struct ProcessesListStruct* prevProcess;
    struct ProcessesListStruct* nextProcess;
} ProcessesList;

//List for holding all strings to be outputted to file
typedef struct stringListStruct
{
    char string[MAX_OUTPUT_STRING_LENGTH];

    struct stringListStruct* nextString;
    struct stringListStruct* prevString;
} stringList;

// functions //
/*
    This function takes a ConfigDataType and OpCodeType,
    Then it finds all the processes present in the OpCode
    and creates a PCB for each process, logging all os
    operations. Once all processes are found, run each process and
    once ended end the simulation and print logs to file
*/
void runSim(ConfigDataType *configData, OpCodeType *metaData);

/*
    Create a new process control block which points to a OpCodeType as where the process starts
    along with unique PID and any other data needed for an initial process, such as finding total
    burst time, allocates memory for all needed
    returns new PCB
*/
ProcessControlBlock* createProcess(ProcessControlBlock *process, OpCodeType *metaData, ConfigDataType *configData);

/*
    creates a memory block for a process and return a pointer to the block
*/
memoryAllocationBlock* createMemoryBlock(int memoryBase, int memoryOffset);

/*
    deallocates memory from memory block
*/
void freeProcessMemory(memoryAllocationBlock* memory);

/*
    check if memory is available in a process
*/
bool checkIfMemoryAvailable(memoryAllocationBlock *memory, memoryAllocationBlock* testMemory, ConfigDataType* config, memoryAllocationBlock** outMem);

/*
    Adds new memory block to end of process memory linked list
*/
memoryAllocationBlock* addMemoryBlock(memoryAllocationBlock *memory, memoryAllocationBlock* newMemory);

/*
    Check if a memory block has been reserved and is accessible by the membase and offset
*/
bool checkIfMemoryExists(memoryAllocationBlock* memory, int memBase, int memOffset, memoryAllocationBlock** outMem);

/*
    Goes through all metaData and finds each unique process, adding it to a process list
*/
ProcessesList* findAndCreateProcesses(ProcessesList* processes, OpCodeType *metaDataNode, ConfigDataType *configData);

/*
    This function gets the list of processes and depending on the scheduling selected for processes,
    orders the process to conform and run as intended
*/
ProcessesList* orderProcesses(ProcessesList* processes, ConfigDataType *configData);

/*
    Runs a given process, running as long as dictated by the config file, along with
    outputting any info to log gile or monitor. Uses POSIX threads for IO
*/
stringList* runProcess(ProcessControlBlock* process, ConfigDataType* config, stringList* strings, memoryAllocationBlock* memory);

/*
    Recursively goes through processes and frees allocated memory
*/
void freeProcesses(ProcessesList* processes);

/*
    Adds new string node to string list to be outputted to file
*/
stringList* addNewStringNode(stringList* strings, char* buffer);

/*
    Handles printing to monitor or saving to string list for file output
*/
stringList* printOrAddtoList(stringList* tempStrPtr, char buffer[], ConfigDataType* config);

/*
    Recursively Free string list of allocated memory
*/
void freeStrings(stringList* strings);

/*
    Take all simulator logs and output to a log file as
    specified in the config file, takes in config file
    and stringList
*/
void printToFile(stringList* strOutput, ConfigDataType* config);

/*
    When output is only going to file, runs a loading symbol 
    as a POSIX thread until the Simulator is finished, where 
    this function as a thread should be cancelled
*/
void* printLoadingSymbol(void* timeUntilSymbolChange);

#endif
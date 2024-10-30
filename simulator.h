//  File: simulator.h
//  Project: Sim02
//  Secret ID: 708996
//  Date: 09/30/2024

#ifndef simulator_h
#define simulator_h
#include "datatypes.h"
#include "StandardConstants.h"
#include "metadataops.h"
#include "configops.h"
#include "simtimer.h"
#include "time.h"
#include <pthread.h>
#include <stdio.h>

typedef struct MemoryBlock {
    int base;
    int offset;
    int pid;
    struct MemoryBlock *next;
} MemoryBlock;

/*
Name: runSim
process: primary simulation driver
Function Input/Parameters:  configuration data (ConfigDataType *),
                            metadata (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: tbd
*/
void runSim(ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr);

/*
Name: createNewPCBNode
Process: Creates a NEW_STATE PCB node with specified process ID 
and metadata pointer
Function Input/Parameters: Process ID, pointer to metadata
Function Output/Parameters: None
Function Output/Returned: Newly created PCB node pointer
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
PCB *createNewPCBNode(int pid, OpCodeType *mdPtr);

/*
Name: createPCB_List
Process: Creates a linked list of PCB nodes based on the 
configuration and metadata
Function Input/Parameters: Configuration pointer, Metadata 
pointer
Function Output/Parameters: None
Function Output/Returned: Head pointer of the created PCB 
linked list
Device Input/File: None
Device Output/Device: None
Dependencies: createNewNode function
*/
PCB *createPCB_List(ConfigDataType *configPtr, OpCodeType *metaData);

/*
Name: displayProcessState
Process: Displays the current state of the process based on the configuration settings
Function Input/Parameters: Configuration pointer, Process pointer, Lap time, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
void displayProcessState(ConfigDataType *config, PCB *process, double lapTime, FILE* file);

/*
Name: displayToMonitor
Process: Displays the current state of the process to the monitor
Function Input/Parameters: Process pointer, Lap time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: Monitor
Dependencies: None
*/
void displayToMonitor(PCB *process, double lapTime);

/*
Name: displayToFile
Process: Displays the current state of the process to the file
Function Input/Parameters: Process pointer, Lap time, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: File
Dependencies: None
*/
void displayToFile(PCB *process, double lapTime, FILE* file);

/*
Name: getNextProcess
Process: Retrieves the next process in the linked list of processes
 based on the current process and metadata
Function Input/Parameters: Current process pointer, Metadata pointer
Function Output/Parameters: None
Function Output/Returned: Next process pointer
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
PCB *getNextProcess(PCB *currentProcess, OpCodeType *metaData);

/*
Name: printTitle
Process: Prints the title and the start time of the simulation based 
on the configuration settings
Function Input/Parameters: Configuration pointer, File pointer, Time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
void printTitle(ConfigDataType *config, FILE *fileName, double time);

/*
Name: getTimer
Process: Retrieves lap time using a timer thread
Function Input/Parameters: Pointer to PCB node
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: runTimer
*/
void getTimer(PCB *processPtr);

/*
Name: isEndOfProcess
Process: Checks if the metadata node marks the end of a process
Function Input/Parameters: Pointer to metadata node
Function Output/Parameters: None
Function Output/Returned: Boolean value indicating if it's the end of a process
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
bool isEndOfProcess(OpCodeType *metaPtr);

/*
Name: printMemInitial
Process: Prints memory initialization information
Function Input/Parameters: Pointer to configuration data type,
pointer to PCB node, lap time, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void printMemInitial(ConfigDataType *config, PCB *process, double timer,FILE *outputFile);

/*
Name: printOpCode
Process: Prints the operation code details
Function Input/Parameters: File pointer, pointer to 
configuration data type, pointer to PCB node
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void printOpCode(FILE* outputFile, ConfigDataType *config, PCB* process);

/*
Name: printReadyRunning
Process: Prints the ProcessState transition from READY_STATE to RUNNING_STATE
Function Input/Parameters: File pointer, pointer to 
configuration data type, pointer to PCB node, lap time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void printReadyRunning(FILE* outputFile, ConfigDataType *config, 
                                       PCB* process ,double time);

/*
Name: printStartSim
Process: Prints the start of the simulation
Function Input/Parameters: Pointer to configuration data type,
pointer to PCB node, lap time, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void printStartSim(ConfigDataType *config, PCB *process, double timer,FILE *outputFile);

// Declaration of displayOpCode function
void displayOpCode(ConfigDataType *configPtr, OpCodeType *mdPtr, PCB *pcb, FILE *file, double *elapsedTime);

// Declare the memory-related functions
MemoryBlock *initializeMemory(ConfigDataType *configPtr);
bool allocateMemoryBlock(MemoryBlock **head, int pid, int size);
void freeMemoryBlock(MemoryBlock **head, int pid);
void displayMemoryBlocks(MemoryBlock *head);


#endif
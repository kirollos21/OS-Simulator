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

// Five state data structure for processes
typedef enum {
    NEW_STATE,
    READY_STATE,
    RUNNING_STATE,
    BLOCKED_STATE,
    EXIT_STATE
} ProcessState;


typedef struct PCB
{
    int pid;
    int time;
    ProcessState currentState;
    OpCodeType *mdPtr;
    struct PCB *nextNode;

}PCB;

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
Name: createNewNode
Process: Allocates memory for a new PCB node and initializes
its nextNode pointer to NULL
Function Input/Parameters: pointer to an existing PCB node
Function Output/Parameters: Newly created PCB node pointer
Function Output/Returned: Newly created PCB node pointer
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
PCB *createNewNode(PCB *pointer);

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
Name: displayOpCode
Process: Displays the operation codes based on the configuration 
settings and updates the process state
Function Input/Parameters: Configuration pointer, Metadata pointer,
Process pointer, File pointer, Time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: getOpCode, displayState functions
*/
void displayOpCode(ConfigDataType *configPtr, OpCodeType *metaData, PCB *process, FILE *fileName, double *elapsedTime);

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
Name: getOpCode
Process: Retrieves and displays the operation codes from metadata 
based on the configuration settings
Function Input/Parameters: Metadata pointer, Time stamp, Process ID, 
Configuration pointer, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
// void getOpCode(OpCodeType *metaDataPtr, double timeStamp, int pid, ConfigDataType *config, FILE *file);

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

#endif
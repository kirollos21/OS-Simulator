// protect from multiple compiling
#ifndef SIMULATOR_H
#define SIMULATOR_H

// header files
#include "metadataops.h"
#include "configops.h"
#include "simtimer.h"
#include "time.h"
#include "datatypes.h"
#include <pthread.h>
typedef struct {
    pthread_mutex_t lock;
    int lap;
} TimerData;
typedef enum
{
    NEW,
    READY,
    RUNNING,
    EXITING
}STATE;

typedef enum
{
    NOVALUE,
    TITLE,
    SIMSTART,
    NEWTOREADY,
    MEMINTIAL,
    READYRUNNING,
    OPDISPLAY
}DISPLAY;

//data structure
typedef struct PCB
{
    int pid;
    int remainingtime; //in milliseconds
    STATE currentState;
    OpCodeType *mdPtr;
    struct PCB *nextNode;
}PCB;

// Function prototypes

/*
Name: calculateRemainingTime
Process: Calculates the remaining time for a PCB based 
on configuration and metadata
Function Input/Parameters: Pointer to PCB node, pointer 
to configuration data type
Function Output/Parameters: None
Function Output/Returned: Remaining time for the PCB
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
double calculateRemainingTime(PCB *pcbNode, ConfigDataType *configPtr);

/*
Name: createNewPCBNode
Process: Creates a new PCB node with specified process ID 
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
Process: Creates a PCB linked list based on configuration and metadata
Function Input/Parameters: Pointer to configuration data type,
 pointer to metadata, pointer to total process counter
Function Output/Parameters: None
Function Output/Returned: Head pointer of the created PCB linked list
Device Input/File: None
Device Output/Device: None
Dependencies: createNewPCBNode, calculateRemainingTime, isEndOfProcess
*/
PCB *createPCB_List(ConfigDataType *configPtr, OpCodeType *metaData, int *totProc);

/*
Name: displayMem
Process: Displays memory details
Function Input/Parameters: Pointer to PCB node, pointer to 
configuration data type, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void displayMem(PCB *process, ConfigDataType *configPtr, FILE *outputFile);

/*
Name: displayProcessState
Process: Displays the current state of the process based on the 
configuration settings
Function Input/Parameters: Pointer to configuration data type, 
pointer to PCB node, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void displayProcessState(ConfigDataType *config, PCB *process, FILE *outputFile);

/*
Name: displayToMonitor
Process: Displays the current state of the process to the monitor
Function Input/Parameters: Pointer to configuration data type, 
pointer to PCB node, display type, lap time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: Monitor
Dependencies: None
*/
void displayToMonitor(ConfigDataType *config, PCB *process, int displayType, double time);

/*
Name: displayToFile
Process: Displays the current state of the process to the file
Function Input/Parameters: File pointer, pointer to PCB node, 
pointer to configuration data type, display type, lap time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: File
Dependencies: None
*/
void displayToFile(FILE *outputFile, PCB *process, ConfigDataType *config, int displayType, double time);

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
Name: getNextProcess
Process: Retrieves the next process in the linked list based on the current
process and metadata
Function Input/Parameters: CPU scheduler type, pointer to current process,
head pointer of the process list
Function Output/Parameters: None
Function Output/Returned: Next process pointer
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
PCB *getNextProcess(int cpuScheduler, PCB *processPtr);

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
void printMemInitial(ConfigDataType *config, PCB *process, double timer, FILE *outputFile);

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
void printOpCode(FILE *outputFile, ConfigDataType *config, PCB* process);

/*
Name: printReadyRunning
Process: Prints the state transition from READY to RUNNING
Function Input/Parameters: File pointer, pointer to 
configuration data type, pointer to PCB node, lap time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void printReadyRunning(FILE *outputFile, ConfigDataType *config, PCB* process, double time);

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
void printStartSim(ConfigDataType *config, PCB *process, double timer, FILE *outputFile);

/*
Name: printTitle
Process: Prints the title and the start time of 
the simulation based on the configuration settings
Function Input/Parameters: Pointer to configuration data type, 
pointer to PCB node, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void printTitle(ConfigDataType *config, PCB *process, FILE *outputFile);

/*
Name: runSim
Process: Runs the simulation based on the provided configuration
and metadata
Function Input/Parameters: Pointer to configuration data type, 
pointer to metadata
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: createPCB_List, displayState, getNextProcess,
displayOpCode, printTitle
*/
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr );

#endif  // SIMULATOR_H

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "configops.h"  
#include "metadataops.h"
#include "simtimer.h"
#include "OS_SimDriver.h" 
#include "StandardConstants.h"
#include "StringUtils.h"
#include "datatypes.h"
#include <pthread.h>
#include <stdbool.h>


// Function prototypes

#define READY_STATE 1
#define NEW_STATE 0

#define START 0  // Start flag
#define RUN 1    // Run flag
typedef int Boolean;   // Define Boolean as an integer type

typedef struct PCBType
{
    pthread_t PID;  // Process ID (int)
    pthread_t thread;  // Thread handle (pthread_t)
    int PCBStatus;  // PCB state
    int opEndTime;  // Operation end time
    OpCodeType *appStart;  // Pointer to process start
    struct PCBType *nextPCB;  // Pointer to next PCB
} PCBType;

typedef struct memHolder {
    int start;               // Start of the memory range
    int end;                 // End of the memory range
    int memoryUsed;          // Amount of memory used within the range
    struct memHolder *next;  // Pointer to the next memory block
} memHolder;


typedef struct logToFile
{
    char filePiece[STD_STR_LEN];  // Holds a line of the log to be written to file
    struct logToFile *next;       // Pointer to the next log entry
} logToFile;

// Global mutex for synchronization
pthread_mutex_t stateMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t interruptMutex = PTHREAD_MUTEX_INITIALIZER;

// Interrupt queue for managing I/O completion
typedef struct Interrupt {
    int processID;
    struct Interrupt *next;
} Interrupt;

// Function Prototypes

/*
Name: addLine
Process: recursively creates new fileHolder for dumping at the end of the display
Function Input/Parameters: pointer to fileHolder head (logToFile *),
                           string to add (char *)
Function Output/Parameters: none
Function Output/Returned: returns pointer to fileHolder
Device Input/device: none
Device Output/device: none
Dependencies: copyString
*/
logToFile *addLine(logToFile *fileHolder, char *lineAdd);

/*
Name: addPCB
Process: recursively creates new PCB with data from the config & metadata files
Function Input/Parameters: pointer to config data structure (ConfigDataType *),
                           pointer to metadata structure (OpCodeType *),
                           PID (int),
                           pointer to PCB head (PCBType *)
Function Output/Parameters: none
Function Output/Returned: returns pointer to PCB head
Device Input/device: none
Device Output/device: none
Dependencies: tbd
*/
PCBType *addPCB( ConfigDataType *configDataPtr, OpCodeType *metaDataPtr, int PID,
                 PCBType *PCBHeadPtr );

/*
Name: calculateOpTime
Process: Calculate process time for PCB process
Function Input/Parameters: pointer to metadata app start head (OpCodeType *),
                           pointer to config head (ConfigDataType *)
Function Output/Parameters: none
Function Output/Returned: Total calculated operation time
Device Input/device: none
Device Output/device: none
Dependencies: accessTimer
*/
int calculateOpTime(OpCodeType *MDPtr, ConfigDataType *configPtr);

/*
Name: dumpFile
Process: dumps all the lines into a file at the end
Function Input/Parameters: pointer to fileHolder head (logToFile *),
                           pointer to file (FILE *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: fprintf
*/
void dumpFile(logToFile *fileHolder, FILE *fileName);

/*
Name: runSim
Process: primary simulation driver
Function Input/Parameters: configuration data (ConfigDataType *),
                           metadata (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: tbd
*/
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr );

/*
Name: setState
Process: loops over the PCB list setting the state and displaying appropriately
Function Input/Parameters: pointer to PCB head (PCBType *),
                           State to set to (int),
                           LogTo code (int),
                           pointer to fileHolder (logToFile *),
                           pointer to log file (FILE *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: Displays to monitor and/or file
Dependencies: accessTimer
*/
void setState(PCBType *headPtr, int state, int logCode,
              logToFile* fileHolder, FILE* logFile);

/*
Name: startPCB
Process: starts building the PCB linked list
Function Input/Parameters: pointer to metadata (OpCodeType *),
                           pointer to config data (ConfigDataType *)
Function Output/Parameters: none
Function Output/Returned: pointer to head of PCB linked list
Device Input/device: none
Device Output/device: none
Dependencies: addPCB
*/
PCBType *startPCB(OpCodeType *metaDataPtr, ConfigDataType *configPtr);

/*
Name: displayPCB
Process: Displays the PCB information.
Function Input/Parameters: pointer to PCB head (PCBType *),
                           pointer to log file (FILE *),
                           Display flag (int),
                           timer data (char *),
                           pointer to file holder (logToFile *),
                           pointer to config data (ConfigDataType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: Displays to monitor and/or file
Dependencies: accessTimer
*/
void displayPCB(PCBType *pcbHeadPtr, FILE *logFile, int dispFlag, char *timer, 
                logToFile *fileHolder, ConfigDataType *configDataPtr); 

/*
Name: checkIdle
Process: Checks if a process is idle and logs it.
Function Input/Parameters: pointer to PCB head (PCBType *),
                           pointer to file holder (logToFile *),
                           timer data (char *),
                           LogTo code (int)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: Displays to monitor and/or file
Dependencies: accessTimer
*/
void checkIdle(PCBType *PCBHeadHolder, logToFile *fileHolder, char *timer, int logCode);


void *simulateProcess(void *arg);

/*
Name: memSim
Process: simulates the memory part of the sim. 
Function Input/Parameters:
Function Output/Parameters: none
Function Output/Returned: none 
Device Input/device: none
Device Output/device: none
Dependencies: fprintf
*/
bool memSim( OpCodeType *memPtr, int logCode, int PID, logToFile *fileHolder, 
                        int flag, ConfigDataType *configPtr, memHolder *memUsed);

memHolder *freeMem(memHolder *usedMem);

memHolder *addMem(memHolder *usedMem, int startData, int endData);


#endif // SIMULATOR_H
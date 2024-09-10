//  File: datatypes.h
//  Project: Sim01
//  Name: Kirollos Zikry
//  Date: 09/07/2024

#ifndef datatypes_h
#define datatypes_h
#include <stdio.h>
#include <stdbool.h>
#include "StandardConstants.h"

// GLOBAL CONSTANTS - may be used in other files

typedef enum {
    CMD_STR_LEN = 5,
    IO_ARG_STR_LEN = 5,
    STR_ARG_LEN = 15
} OpCodeArrayCapacity;

// Five state data structure for processes
typedef enum {
    NEW_STATE,
    READY_STATE,
    RUNNING_STATE,
    BLOCKED_STATE,
    EXIT_STATE
} ProcessState;

typedef struct ConfigDataType
{
    double version;
    char metaDataFileName[ LARGE_STR_LEN ];
    int cpuSchedCode;
    int quantumCycles;
    bool memDisplay;
    int memAvailable;
    int procCycleRate;
    int ioCycleRate;
    int logToCode;
    char logToFileName[ LARGE_STR_LEN ];
} ConfigDataType;

typedef struct OpCodeType
{
    int pid; // pid, added when PCB is created
    char command[ CMD_STR_LEN ]; // three letter command quantity
    char inOutArg[ IO_ARG_STR_LEN ]; // for device in/out
    char strArg1[ STR_ARG_LEN ]; // arg 1 descriptor, up to 12 chars
    int intArg2; // cycles or memory, assumes 4 byte
    int intArg3; // memory, assumes 4 byte int
                // also non/premption indicator
    double opEndTime; // size of time string returned from accessTimer
    struct OpCodeType *nextNode; // pointer to next node as needed
} OpCodeType;

#endif
// protect from multiple compiling
#ifndef DATATYPES_H
#define DATATYPES_H

// file headers
#include <stdio.h>
#include <stdbool.h>
#include "StandardConstants.h"


// Gloable constants - may be used in other files
typedef enum { 	CMD_STR_LEN = 5,
                IO_ARG_STR_LEN = 5,
                STR_ARG_LEN = 15 } OpCodeArrayCapacity;


typedef struct ConfigDataType
   {
    double version;
    char metaDataFileName[ STD_STR_LEN ];
    int cpuSchedCode;
    int quantumCycles;
    bool memDisplay;
    int memAvailable;
    int procCycleRate;
    int ioCycleRate;
    int logToCode;
    char logToFileName[ STD_STR_LEN ];
   } ConfigDataType;

typedef struct OpCodeType
   {
    int pid;
    char command[ CMD_STR_LEN ];
    char inOutArg[ IO_ARG_STR_LEN ];
    char strArg1[ STR_ARG_LEN ];
    int intArg2;
    int intArg3;
    double opEndTime;
    struct OpCodeType *nextNode;
   } OpCodeType;


#endif // DATATYPES_H

// protect from multiple compiling
#ifndef DATATYPES_H
#define DATATYPES_H

#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "StandardConstants.h"

// global constants - may be used in other files


// TEMP, CHANGE THIS
typedef struct ConfigDataTypeStruct
{
   double version;
   char metaDataFileName[ LARGE_STR_LEN ];
   int quantumCycles, memAvailable;
   int procCycleRate, ioCycleRate;
   int cpuSchedCode, logToCode;
   bool memDisplay;
   char logToFileName[ LARGE_STR_LEN ];

} ConfigDataType;


// TEMP, CHANGE THIS
typedef struct OpCodeTypeStruct
{
   int pid, intArg2, intArg3;
   char command[ MIN_STR_LEN ];
   char inOutArg[ MIN_STR_LEN ];
   char strArg1[ MIN_STR_LEN ];
   double opEndTime;

   struct OpCodeTypeStruct *nextNode;

} OpCodeType;


#endif   // DATATYPES_H

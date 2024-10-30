// protect from multiple compiling
#ifndef SIM_01_H
#define SIM_01_H

// header files
#include <stdio.h>
#include "StringUtils.h"
#include "datatypes.h"
#include "configops.h"
#include "metadataops.h"
#include "simulator.h"


// Program constants
typedef enum { MIN_NUM_ARGS = 3, LAST_FOUR_LETTERS = 4} PRGRM_CONSTANTS;

// Command line struct storing command line switch settings
typedef struct CmdLineDataStruct
   {
    bool programRunFlag;
    bool configDisplayFlag;
    bool mdDisplayFlag;
    bool runSimFlag;
    
    char fileName[STD_STR_LEN];
   } CmdLineData;
   
// function prototypes
void clearCmdLineStruct( CmdLineData *clDataPtr);
bool processCmdLine( int numArgs, char **strVector, CmdLineData *clDataPtr);
void showCommandLineFormat();

#endif // SIM_01_h

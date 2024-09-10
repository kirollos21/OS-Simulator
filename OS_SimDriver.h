//
//  OS_SimDriver_S12.h
//  Sim01
//
//  Created by Youssef Elmahdy on 05/09/2024.
//

// protect from multiple compiling
#ifndef OS_SimDriver_h
#define OS_SimDriver_h

// header files
#include <stdio.h>
#include "datatypes.h"
#include "configops.h"
#include "metadataops.h"
#include "simulator.h"
#include "StringUtils.h"

// Program Constants
typedef enum { MIN_NUM_ARGS = 3, LAST_FOUR_LETTERS = 4} PRGRM_CONSTANTS;

// Command Line struct for string command line switch settings
typedef struct CmdLineDataStruct
{
    bool programRunFlag;
    bool configDisplayFlag;
    bool mdDisplayFlag;
    bool runSimFlag;
    
    char fileName[ STD_STR_LEN ];
} CmdLineData;


// Function Prototypes
bool processCmdLine( int numArgs, char **strVector, CmdLineData *clDataPtr );

void showCommandLineFormat();

#endif /* OS_SimDriver_h */

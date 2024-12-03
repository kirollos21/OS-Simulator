// protect from multipl compiling
#ifndef OS_SIM_DRIVER_H
#define OS_SIM_DRIVER_H

// header files 
#include <stdio.h>
#include "datatypes.h"
#include "StringUtils.h"
#include "configops.h"
#include "metadataops.h"
#include "simulator.h"


// Constants
typedef enum { MIN_NUM_ARGS = 3, LAST_FOUR_LETTERS = 4 } PRGRM_CONSTANTS;

// Command line struct for storing command line switch settings
typedef struct CmdLineDataStruct
   {
    bool programRunFlag;
    bool configDisplayFlag;
    bool mdDisplayFlag;
    bool runSimFlag;
                                                
    char fileName[ STD_STR_LEN ];
   } CmdLineData;

// Function prototypes

/*
 Name: clearCmdLineStruct
 Process: sets command line structure data to defaults
 Function Input/Parameters: pointer to command line structure (CmdLineData *)
 Function Output/Parameters: pointer to command line structure (CmdLineData *)
 Function Output/Returned: none
*/
void clearCmdLineStruct(CmdLineData *clDataPtr);

/*
 Name: processCmdLine
 Process: checks for at least two arguments, sets Booleans depending on command line switches,
          and captures the config file name
 Function Input/Parameters: number of arguments (int),
                            vector of arguments (char **)
 Function Output/Parameters: pointer to command line structure (CmdLineData *)
 Function Output/Returned: Boolean result of argument capture (bool)
*/
bool processCmdLine(int numArgs, char **strVector, CmdLineData *clDataPtr);

/*
 Name: showCommandLineFormat
 Process: displays command line format to the user
 Function Input/Parameters: none
 Function Output/Parameters: none
 Function Output/Returned: none
*/
void showCommandLineFormat();


#endif 
//  File: OS_SimDriver.h
//  Project: Sim01
//  Secret ID: 708996
//  Date: 09/07/2024 & 09/08/2024

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
#include <stdbool.h>

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

/*
Name: clearCmdLineStruct
Process: sets command line structure data to defaults
         Booleans to false, fileName to empty string
Function Input/Parameters: pointer to command line structure (CmdLineData *)
Function Output/Parameters: pointer to command line structure (CmdLineData *)
                            with updated members
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: none
*/
void clearCmdLineStruct(CmdLineData *clDataPtr);

/*
 Name: processCmdLine
 Process:   checks for at least two arguments,
            then sets Booleans depending on command Line switches
            which can be in any order,
            also captures config file name which must be the last argument
 Function Input/Parameters: number of arguments (int),
                            vector of arguments (char **o<
 Function Output/Parameters:    pointer to command Line structure (CmdLineData *)
                                with updated members,
                                set to default values if failure
                                to capture arguments
 Function Output/Returned:  Boolean result of argument capture,
                            true if at least one switch and config file name,
                            false otherwise
 Device Input/device: none
 Device Output/device: none
 Dependencies: tbd
 */
bool processCmdLine(int numArgs, char **strVector, CmdLineData *clDataPtr);

/*
Name: showCommandLineFormat
Process: displays command Line format as assistance to user
Function Input/Parameters: none
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/monitor: data displayed as specified
Dependencies: printf
*/
void showCommandLineFormat();

#endif
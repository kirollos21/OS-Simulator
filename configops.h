
#ifndef CONFIG_OPS_H
#define CONFIG_OPS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "datatypes.h"
#include "StringUtils.h"

/*
Name: getConfigData
Process: Goes through config file and finds filenames
Function input/parameters: filename
Function output/parameters: configData, endStateMsg
Function output/returned: bool for success or failure
Device input/---: none
Device output/---: none
Dependencies: none
*/
bool getConfigData(const char* filename, ConfigDataType **configData,
    char* endStateMsg);

/*
Name: getCpuSchedCode
Process: Determine which Cpu schedule code is in config
Function input/parameters: string
Function output/parameters: none
Function output/returned: corresponding config code
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
ConfigDataCodes getCpuSchedCode(const char *lowerCaseCodeStr);

/*
Name: getDataLineCode
Process: Determine which Cpu data code is in config
Function input/parameters: string
Function output/parameters: none
Function output/returned: corresponding data code
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
ConfigCodeMessages getDataLineCode(const char* dataBuffer);

/*
Name: getLogToCode
Process: Determine which log code is in config
Function input/parameters: string
Function output/parameters: none
Function output/returned: corresponding log code
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
ConfigDataCodes getLogToCode(const char *lowerCaseLogToStr);

/*
Name: stripTrailingSpaces
Process: Replaces all trailing spaces of a string with NULL chars
Function input/parameters: string
Function output/parameters: stripped string
Function output/returned: none
Device input/---: none
Device output/---: none
Dependencies: getStringLength
*/
void stripTrailingSpaces(char *str);

/*
Name: valueInRange
Process: Determines if a certain value is in range for a 
        certain code
Function input/parameters: codes
Function output/parameters: none
Function output/returned: result if value 
                          is in correct range for code
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
bool valueInRange(int lineCode, int intVal, 
                  double doubleVal, const char *lowerCaseStringVal);

/*
Name: displayConfigData
Process: Displays config file to the user
Function input/parameters: config data
Function output/parameters: prints out config data
Function output/returned: none 
Device input/---: none
Device output/---: none
Dependencies: printf
*/
void displayConfigData(ConfigDataType *configData);

/*
Name: configCodeToString
Process: Determines if a certain value is in range for a 
        certain code
Function input/parameters: codes
Function output/parameters: none
Function output/returned: result if value 
                          is in correct range for code
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
void configCodeToString(int code, char *outString);

/*
Name: clearConfigData
Process: empties configData struct for future use
Function input/parameters: ConfigDataType
Function output/parameters: none
Function output/returned: empty ConfigDataType
Device input/---: none
Device output/---: none
Dependencies: free
*/
ConfigDataType* clearConfigData(ConfigDataType *configData);

#endif
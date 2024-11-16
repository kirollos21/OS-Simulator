//  File: configops.h
//  Project: Sim04
//  Secret ID: 708996

#ifndef configops_h
#define configops_h
#include <stdio.h>
#include <stdbool.h>
#include "StandardConstants.h"
#include "datatypes.h"
#include "StringUtils.h"

/*
Name: displayConfigData
Process: screen dump/display of all config data
Function Input/Parameters: pointer to config data structure (ConfigDataType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: displayed as specified
Dependencies: tbd
*/
void displayConfigData(ConfigDataType *configData);

/*
Name: clearConfigData
Process: frees dynamically allocated config data structure
         if it has not already been freed
Function Input/Parameters: pointer to config data structure (ConfigDataType *)
Function Output/Parameters: none
Function Output/Returned: NULL (ConfigDataType *)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: tbd
*/
ConfigDataType *clearConfigData(ConfigDataType *configData);

/*
Name: getConfigData
Process: Driver function for capturing configuration data from a config file
Function Input/Parameters: file name (const char *)
Function Output/Parameters: pointer to config data pointer (ConfigDataType **),
                            end/result state message pointer (char *)
Function Output/Returned: Boolean result of data access operation (bool)
Device Input/Keyboard: config data uploaded
Device Output/Monitor: none
Dependencies: tbd
*/
bool getConfigData(const char *fileName, ConfigDataType **configData, char *endStateMsg);

/*
Name: getCpuSchedCode
Process: converts cpu schedule string to code (all scheduling possibilities)
Function Input/Parameters: lower case code string (const char *)
Function Output/Parameters: none
Function Output/Returned: cpu schedule code (ConfigDataCodes)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: compareString
*/
ConfigDataCodes getCPUSchedCode(const char *lowerCaseCodeStr);

/*
Name: configCodeToString
Process: utility function converts configuration code numbers
         to the string they represent
Function Input/Parameters: configuration code (int)
Function Output/Parameters: resulting output string (char *)
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: copyString
*/
void configCodeToString(int code, char *outString);

/*
Name: stripTrailingSpaces
Process: removes trailing spaces from input config leader lines
Function Input/Parameters: config leader line string (char *)
Function Output/Parameters: updated config leader line string (char *)
Function Output/Returned: configuration data code value (ConfigDataCodes)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringLength
*/
void stripTrailingSpaces(char *str);

/*
Name: getLogToCode
Process: converts "Log to" text to configuration data code
         (three log to strings)
Function Input/Parameters: lower case Log to string (const char *)
Function Output/Parameters: none
Function Output/Returned: configuration data code value (ConfigDataCodes)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: compareString
*/
ConfigDataCodes getLogToCode(const char *lowerCaseLogToStr);

/*
Name: getDataLineCode
Process: converts leader lines string to configuration code value
         (all config file leader lines)
Function Input/Parameters: config leader line string (const char *)
Function Output/Parameters: none
Function Output/Returned: configuration code value (ConfigCodeMessages)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: compareString
*/
ConfigCodeMessages getDataLineCode(const char *dataBuffer);

/*
Name: valueInRange
Process: checks for config data values in range, including string values
         (all config data values)
Function Input/Parameters: line code number for specific config value (int),
                           integer value, as needed (int),
                           double value, as needed (double),
                           string value, as needed (const char *)
Function Output/Parameters: none
Function Output/Returned: Boolean result of range test (bool)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringLength
*/
bool valueInRange(int lineCode, int intVal, double doubleVal, const char *lowerCaseStringVal);

#endif


#include "configops.h"

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
    char* endStateMsg)
{
    //set constant number of data lines
    const int NUM_DATA_LINES = 10;

    //set read only constant
    const char READ_ONLY_FLAG[] = "r";

    //create pointer for data input
    ConfigDataType *tempData;

    //declare other variables
    FILE *fileAccessPtr;
    char dataBuffer[MAX_STR_LEN], lowerCaseDataBuffer[MAX_STR_LEN];
    int intData, dataLineCode, lineCtr = 0;
    double doubleData;

    //set endStateMsg to success
    char testMsg[] = "Configuration file upload success";
    copyString(endStateMsg, testMsg);

    //initialize config file data pointer in case of return error
    *configData = NULL;

    //open file
    fileAccessPtr = fopen(filename, READ_ONLY_FLAG);

    //check if file open failure
    if (fileAccessPtr == NULL)
    {
        char testMsg[] = "Config file access error";
        copyString(endStateMsg, testMsg);
        
        return false;
    }
    
    if (!getStringToDelimiter (fileAccessPtr, COLON, dataBuffer)
        || compareStrings(dataBuffer, "Start Simulator Configuration File")
        != STR_EQ)
    {
        fclose(fileAccessPtr);

        copyString(endStateMsg, "Corrupt config leader line error");
    }

    tempData = (ConfigDataType*) malloc (sizeof(ConfigDataType));

    while (lineCtr < NUM_DATA_LINES)
    {
        if (!getStringToDelimiter(fileAccessPtr, COLON, dataBuffer))
        {
            //free creates data
            free (tempData);

            //close file
            fclose(fileAccessPtr);

            //copy error msg
            copyString(endStateMsg, "Config data leader line capture error");

            //return incomplete file error
            return false;
        }

        //strip trailing spaces of config leader lines
        //for use in other functions
            //function: strip trailing spaces
        stripTrailingSpaces(dataBuffer);


        //find correct data line code number from string
            //function: getDataLineCode
        dataLineCode = getDataLineCode(dataBuffer);

        //check for data line found
        if (dataLineCode != CFG_CORRUPT_PROMPT_ERR)
        {
            //get data value

            //check for version number
            if (dataLineCode == CFG_VERSION_CODE)
            {
                //get version number
                    //function: fscanf
                fscanf(fileAccessPtr, "%lf", &doubleData);
            }

            //otherwise, check for metaData, file names,
            //CPU scheduling names, or memory display setting
            else if (dataLineCode == CFG_MD_FILE_NAME_CODE ||
                     dataLineCode == CFG_LOG_FILE_NAME_CODE ||
                     dataLineCode == CFG_CPU_SCHED_CODE ||
                     dataLineCode == CFG_LOG_TO_CODE ||
                     dataLineCode == CFG_MEM_DISPLAY_CODE)
            {
                //get string input
                    //function: fscanf
                fscanf(fileAccessPtr, "%s", dataBuffer);

                //set string to lower case for testing in value in range
                    //function:setStrToLowerCase
                setStrToLowerCase(lowerCaseDataBuffer, dataBuffer);
            }
            else
            {
                //get integer input
                    //function: fscanf
                fscanf(fileAccessPtr, "%d", &intData);
            }
            

            //check for data value in range
                //function: valueInRange
            if (valueInRange(dataLineCode, intData,
                 doubleData, lowerCaseDataBuffer))
            {
                //assign to data pointer depending on config item
                //all config possibilities
                    //function: copyString, getCpuSchedCode, 
                    // compareString, getLogToCode, as needed
                switch (dataLineCode)
                {
                    case CFG_VERSION_CODE:
                        tempData->version = doubleData;
                        break;
                    case CFG_MD_FILE_NAME_CODE:
                        copyString(tempData->metaDataFilename, dataBuffer);
                        break;
                    case CFG_CPU_SCHED_CODE:
                        tempData->cpuSchedCode = 
                            getCpuSchedCode(lowerCaseDataBuffer);
                        break;
                    case CFG_QUANT_CYCLES_CODE:
                        tempData->quantumCycles = intData;
                        break;
                    case CFG_MEM_DISPLAY_CODE:
                        tempData->memDisplay = 
                        compareStrings(lowerCaseDataBuffer, "on") == STR_EQ;
                        break;   
                    case CFG_MEM_AVAILABLE_CODE:
                        tempData->memAvailable = intData;
                        break;    
                    case CFG_PROC_CYCLES_CODE:
                        tempData->procCycleRate = intData; 
                        
                        break;    
                    case CFG_IO_CYCLES_CODE:
                        tempData->ioCycleRate = intData; 
                        break;
                    case CFG_LOG_TO_CODE:
                        tempData->logToCode =
                         getLogToCode(lowerCaseDataBuffer);
                        break;
                    case CFG_LOG_FILE_NAME_CODE:
                        copyString(tempData->logToFileName, dataBuffer);
                        break;
                }
                //end of switch

            }
            else
            {
                //free temp struct memory
                    //function: free
                free(tempData);

                //close file access
                    //function fclose
                fclose(fileAccessPtr);

                //set end state message to configuration data out of range
                    //function: copyString
                copyString(endStateMsg, "Configuration item out of range");

                //return data out of range
                return false;
            }
        }
        else
        {
            //free temp struct memory
                //function: free
            free(tempData);

            //close file access
                //function fclose
            fclose(fileAccessPtr);

            //set end state message to Corrupted configuration prompt
                //function: copyString
            copyString(endStateMsg, "Corrupted configuration prompt");

            //return corrupt file code
            return false;
        }   
        //increment line counter
        lineCtr++;
    }

    //acquired end of sim config string
        //function: getStringToDelimiter, compareString
    if (!getStringToDelimiter(fileAccessPtr, PERIOD, dataBuffer)
        || compareStrings(dataBuffer, "End Simulator Configuration File")
        != STR_EQ)
    {
        //free temp struct memory
            //function: free
        free(tempData);

        //close file access
            //function fclose
        fclose(fileAccessPtr);

        //set end state message to Configuration end line capture error
            //function: copyString
        copyString(endStateMsg, "Configuration end line capture error");

        //return corrupt file data
        return false;  
    }

    //test for "file only" output so memory diagnostics do not display
    tempData->memDisplay = tempData->memDisplay 
    && tempData->logToCode != LOGTO_FILE_CODE;

    //assign temporary pointer to parameter return pointer
    *configData = tempData;

    //close file access
        //function: fclose
    fclose(fileAccessPtr);

    //return no error codes 
    return true;
}

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
ConfigDataCodes getCpuSchedCode(const char *lowerCaseCodeStr)
{
    //initialize function/variables
        //set default return to FCFS-N
    ConfigDataCodes returnVal = CPU_SCHED_FCFS_N_CODE;

    //check for SJF-N
        //function: compareString
    if (compareStrings(lowerCaseCodeStr, "sjf-n") == STR_EQ)
    {
        //set SJF-N code
        returnVal = CPU_SCHED_SJF_N_CODE;
    }

    //check for SRTF-P
        //function: compareString
    if (compareStrings(lowerCaseCodeStr, "srtf-p") == STR_EQ)
    {
        //set SRTF-P code
        returnVal = CPU_SCHED_SRTF_P_CODE;
    }

    //check for FCFS-P
        //function: compareString
    if (compareStrings(lowerCaseCodeStr, "fcfs-p") == STR_EQ)
    {
        //set FCFS-P code
        returnVal = CPU_SCHED_FCFS_P_CODE;
    }

    //check for RR-P
        //function: compareString
    if (compareStrings(lowerCaseCodeStr, "rr-p") == STR_EQ)
    {
        //set SJF-N code
        returnVal = CPU_SCHED_RR_P_CODE;
    }

    //return selected val
    return returnVal;
}

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
ConfigCodeMessages getDataLineCode(const char* dataBuffer)
{
    //return appropriate code depending on prompt string provided
    if (compareStrings(dataBuffer, "Version/Phase") == STR_EQ)
    {
        return CFG_VERSION_CODE;
    }

    if (compareStrings(dataBuffer, "File Path") == STR_EQ)
    {
        return CFG_MD_FILE_NAME_CODE;
    }

    if (compareStrings(dataBuffer, "CPU Scheduling Code") == STR_EQ)
    {
        return CFG_CPU_SCHED_CODE;
    }

    if (compareStrings(dataBuffer, "Quantum Time (cycles)") == STR_EQ)
    {
        return CFG_QUANT_CYCLES_CODE;
    }

    if (compareStrings(dataBuffer, "Memory Available (KB)") == STR_EQ)
    {
        return CFG_MEM_AVAILABLE_CODE;
    }

    if (compareStrings(dataBuffer, "Memory Display (On/Off)") == STR_EQ)
    {
        return CFG_MEM_DISPLAY_CODE;
    }

    if (compareStrings(dataBuffer, "Processor Cycle Time (msec)") == STR_EQ)
    {
        return CFG_PROC_CYCLES_CODE;
    }

    if (compareStrings(dataBuffer, "I/O Cycle Time (msec)") == STR_EQ)
    {
        return CFG_IO_CYCLES_CODE;
    }

    if (compareStrings(dataBuffer, "Log To") == STR_EQ)
    {
        return CFG_LOG_TO_CODE;
    }

    if (compareStrings(dataBuffer, "Log File Path") == STR_EQ)
    {
        return CFG_LOG_FILE_NAME_CODE;
    }

    //return corrupt leader line error code
    return CFG_CORRUPT_PROMPT_ERR;
}

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
ConfigDataCodes getLogToCode(const char *lowerCaseLogToStr)
{
    //initialize variables
        //set default to log to monitor
        ConfigDataCodes returnVal = LOGTO_MONITOR_CODE;

    //check for BOTH
        //function compareString
    if (compareStrings(lowerCaseLogToStr, "both") == STR_EQ)
    {
        //set return value to both code
        returnVal = LOGTO_BOTH_CODE;
    }

    //check for FILE
        //function: compareString
    if (compareStrings(lowerCaseLogToStr, "file") == STR_EQ)
    {
        //set return value to file code
        returnVal = LOGTO_FILE_CODE;
    }

    //return values
    return returnVal;
}

/*
Name: stripTrailingSpaces
Process: Replaces all trailing spaces of a string with NULL chars
Function input/parameters: string
Function output/parameters: stripped string
Function output/returned: none
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
void stripTrailingSpaces(char *str)
{
    //initialize index ot length of string -1 to get highest array index
    int index = getStringLength(str) - 1;

    //loop while space is found at end of string
    while (str[index] == SPACE)
    {
        str[index] = NULL_CHAR;
        index--;
    }
    //end loop from end of string
}

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
                  double doubleVal, const char *lowerCaseStringVal)
{
    //initialize variables

        //set result to true, all tests are to find false
        bool result = true;

    switch (lineCode)
    {
        //for version code
        case CFG_VERSION_CODE:
            if (doubleVal < 0.00 || doubleVal > 10.00)
            {
                result = false;
            }
            break;
        case CFG_CPU_SCHED_CODE:
            //check for not finding one of the shceduling strings
                //function: compareString
            if (compareStrings(lowerCaseStringVal, "fcfs-n") != STR_EQ
                && compareStrings(lowerCaseStringVal, "sjf-n") != STR_EQ
                && compareStrings(lowerCaseStringVal, "srtf-p") != STR_EQ
                && compareStrings(lowerCaseStringVal, "fcfs-p") != STR_EQ
                && compareStrings(lowerCaseStringVal, "rr-p") != STR_EQ)
                {
                    result = false;
                }
            break;
        case CFG_QUANT_CYCLES_CODE:
            if (intVal < 0 || intVal > 100)
            {
                result = false;
            }
            break;
        case CFG_MEM_DISPLAY_CODE:
            //check for not finding either "on" or "off"
                //function: compareStrings
             if (compareStrings(lowerCaseStringVal, "on") != STR_EQ
                && compareStrings(lowerCaseStringVal, "off") != STR_EQ)
            {
                result = false;
            }
            break;   
        case CFG_MEM_AVAILABLE_CODE:
            //check for not under 1024 and not over 102400
            if (intVal < 1024 || intVal > 102400)
            {
                result = false;
            }
            break;    
        case CFG_PROC_CYCLES_CODE:
            //check for not under 1 and not over 100
            if (intVal < 1 || intVal > 100)
            {
                result = false;
            }
            break;    
        case CFG_IO_CYCLES_CODE:
            //check for not under 1 and not over 1000
            if (intVal < 1 || intVal > 1000)
            {
                result = false;
            }
            break;
        case CFG_LOG_TO_CODE:
            //check for not finding one of the log to strings
                //function: compareString 
            if (compareStrings(lowerCaseStringVal, "both") != STR_EQ
                && compareStrings(lowerCaseStringVal, "monitor") != STR_EQ
                && compareStrings(lowerCaseStringVal, "file") != STR_EQ)
            {
                result = false;
            }
            break;
    }
    return result;
}

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
void displayConfigData(ConfigDataType *configData)
{
    //intialize string for data
    char displayStirng[STD_STR_LEN];

    //print lines of display for all member values
    printf("Config File Display\n");
    printf("-------------------\n");
    printf("Version                : %3.2f\n", configData->version);
    printf("Program file name      : %s\n", configData->metaDataFilename);
    configCodeToString(configData->cpuSchedCode, displayStirng);
    printf("CPU schedule selection : %s\n", displayStirng);
    printf("Quantum time           : %d\n", configData->quantumCycles);
    printf("Memory Display         : ");
    if (configData->memDisplay)
    {
        printf("On\n");
    }
    else
    {
        printf("Off\n");
    }
    printf("Memory Available       : %d\n", configData->memAvailable);
    printf("Process cycle rate     : %d\n", configData->procCycleRate);
    printf("I/O cycle rate         : %d\n", configData->ioCycleRate);
    configCodeToString(configData->logToCode, displayStirng);
    printf("Log to selection       : %s\n", displayStirng);
    printf("Log file name          : %s\n\n", configData->logToFileName);
}

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
void configCodeToString(int code, char *outString)
{
    //define array with eight items and short (10) lengths
    char displayStrings[8][10] = {"SJF-N", "SRTF-P", "FCFS-P",
    "RR-P", "FCFS-N", "Monitor", "File", "Both"};

    //copy string to return parameter
    copyString(outString, displayStrings[code]);
}

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
ConfigDataType* clearConfigData(ConfigDataType *configData)
{

    if (configData != NULL)
    {
        free (configData);
        configData = NULL;
    }

    return NULL;
}
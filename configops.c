/**
 * Configuration Data Operations
 */
#include "configops.h"

/**
 * Display configuration data
 */
void displayConfigData(ConfigDataType *configData)
{
  char displayString[STD_STR_LEN];
  printf("Config File Display\n");
  printf("-------------------\n");
  printf("Version                : %3.2f\n", configData->version);
  printf("Program File Name      : %s\n", configData->metaDataFileName);
  configCodeToString(configData->cpuSchedCode, displayString);
  printf("CPU Schedule Selection : %s\n", displayString);
  printf("Quantum Time           : %d\n", configData->quantumCycles);
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
  printf("Process Cycle Rate     : %d\n", configData->procCycleRate);
  printf("I/O Cycle Rate         : %d\n", configData->ioCycleRate);
  configCodeToString(configData->logToCode, displayString);
  printf("Log to Selection       : %s\n", displayString);
  printf("Log File Name          : %s\n\n", configData->logToFileName);
}

/**
 * Clear config data struct
 */
ConfigDataType *clearConfigData(ConfigDataType *configData)
{
  if (configData != NULL)
  {
    free(configData);
    configData = NULL;
  }
  return NULL;
}

/**
 * Retrieves configuration data described in a .cnf file
 */
_Bool getConfigData(const char *fileName, ConfigDataType **configData,
                    char *endStateMsg)
{
  const int NUM_DATA_LINES = 10;
  const char READ_ONLY_FLAG[] = "r";
  ConfigDataType *tempData;
  FILE *fileAccessPtr;
  char dataBuffer[MAX_STR_LEN], lowerCaseDataBuffer[MAX_STR_LEN];
  int intData, dataLineCode, lineCtr = 0;
  double doubleData;
  char testMsg[] = "Configuration file upload successful";

  copyString(endStateMsg, testMsg);
  *configData = NULL;

  fileAccessPtr = fopen(fileName, READ_ONLY_FLAG);
  if (fileAccessPtr == NULL)
  {
    char testMsg[] = "Configuration file access error";
    copyString(endStateMsg, testMsg);
    return false;
  }

  if (!getStringToDelimiter(fileAccessPtr, COLON, dataBuffer) ||
      compareString(dataBuffer, "Start Simulator Configuration File") != STR_EQ)
  {
    fclose(fileAccessPtr);
    copyString(endStateMsg, "Corrupt configuration leader line error");
    return false;
  }
  tempData = (ConfigDataType *)malloc(sizeof(ConfigDataType));

  while (lineCtr < NUM_DATA_LINES)
  {
    if (!getStringToDelimiter(fileAccessPtr, COLON, dataBuffer))
    {
      free(tempData);
      fclose(fileAccessPtr);
      copyString(endStateMsg, "Configuration data leader line capture error");
      return false;
    }
    stripTrailingSpaces(dataBuffer);
    dataLineCode = getDataLineCode(dataBuffer);

    if (dataLineCode != CFG_CORRUPT_PROMPT_ERR)
    {
      if (dataLineCode == CFG_VERSION_CODE)
      {
        fscanf(fileAccessPtr, "%lf", &doubleData);
      }
      else if (dataLineCode == CFG_MD_FILE_NAME_CODE ||
               dataLineCode == CFG_LOG_FILE_NAME_CODE ||
               dataLineCode == CFG_CPU_SCHED_CODE ||
               dataLineCode == CFG_LOG_TO_CODE ||
               dataLineCode == CFG_MEM_DISPLAY_CODE)
      {
        fscanf(fileAccessPtr, "%s", dataBuffer);
        setStrToLowerCase(lowerCaseDataBuffer, dataBuffer);
      }
      else
      {
        fscanf(fileAccessPtr, "%d", &intData);
      }

      // Check for data value in range
      if (valueInRange(dataLineCode, intData, doubleData, lowerCaseDataBuffer))
      {
        // Assign to data pointer depending on config item
        switch (dataLineCode)
        {
        case CFG_VERSION_CODE:
          tempData->version = doubleData;
          break;

        case CFG_MD_FILE_NAME_CODE:
          copyString(tempData->metaDataFileName, dataBuffer);
          break;

        case CFG_CPU_SCHED_CODE:
          tempData->cpuSchedCode = getCPUSchedCode(lowerCaseDataBuffer);
          break;

        case CFG_QUANT_CYCLES_CODE:
          tempData->quantumCycles = intData;
          break;

        case CFG_MEM_DISPLAY_CODE:
          tempData->memDisplay = compareString(lowerCaseDataBuffer, "on") == 0;
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
          tempData->logToCode = getLogToCode(lowerCaseDataBuffer);
          break;

        case CFG_LOG_FILE_NAME_CODE:
          copyString(tempData->logToFileName, dataBuffer);
        }
      }
      // assume data value not in range
      else
      {
        free(tempData);
        fclose(fileAccessPtr);
        copyString(endStateMsg, "Configuration item out of range");

        return false;
      }
    }
    // Otherwise, assume data not found
    else
    {
      free(tempData);
      fclose(fileAccessPtr);
      copyString(endStateMsg, "Corrupted configuration prompt");
      return false;
    }
    lineCtr++;
  }

  if (!getStringToDelimiter(fileAccessPtr, PERIOD, dataBuffer) ||
      compareString(dataBuffer, "End Simulator Configuration File") != STR_EQ)
  {
    free(tempData);
    fclose(fileAccessPtr);
    copyString(endStateMsg, "Configuration end line capture error");
    return false;
  }
  tempData->memDisplay =
      tempData->memDisplay && tempData->logToCode != LOGTO_FILE_CODE;

  *configData = tempData;
  fclose(fileAccessPtr);
  return true;
}

/**
 * Config code to string
 */
void configCodeToString(int code, char *outString)
{
  char displayStrings[8][10] = {"SJF-N",  "SRTF-P",  "FCFS-P", "RR-P",
                                "FCFS-N", "Monitor", "File",   "Both"};

  copyString(outString, displayStrings[code]);
}

/**
 * Retrieves the CPU Schedular Code
 */
ConfigDataCodes getCPUSchedCode(const char *lowerCaseCodeStr)
{
  ConfigDataCodes returnVal = CPU_SCHED_FCFS_N_CODE;

  if (compareString(lowerCaseCodeStr, "sjf-n") == STR_EQ)
  {
    returnVal = CPU_SCHED_SJF_N_CODE;
  }
  if (compareString(lowerCaseCodeStr, "srtf-p") == STR_EQ)
  {
    returnVal = CPU_SCHED_SRTF_P_CODE;
  }
  if (compareString(lowerCaseCodeStr, "fcfs-p") == STR_EQ)
  {
    returnVal = CPU_SCHED_FCFS_P_CODE;
  }
  if (compareString(lowerCaseCodeStr, "rr-p") == STR_EQ)
  {
    returnVal = CPU_SCHED_RR_P_CODE;
  }
  return returnVal;
}

/**
 * strip spaces
 */
void stripTrailingSpaces(char *str)
{
  int index = getStringLength(str) - 1;
  while (str[index] == SPACE)
  {
    str[index] = NULL_CHAR;
    index--;
  }
}

/**
 * get line code
 */
ConfigCodeMessages getDataLineCode(const char *dataBuffer)
{
  if (compareString(dataBuffer, "Version/Phase") == STR_EQ)
  {
    return CFG_VERSION_CODE;
  }
  if (compareString(dataBuffer, "File Path") == STR_EQ)
  {
    return CFG_MD_FILE_NAME_CODE;
  }
  if (compareString(dataBuffer, "CPU Scheduling Code") == STR_EQ)
  {
    return CFG_CPU_SCHED_CODE;
  }
  if (compareString(dataBuffer, "Quantum Time (cycles)") == STR_EQ)
  {
    return CFG_QUANT_CYCLES_CODE;
  }
  if (compareString(dataBuffer, "Memory Display (On/Off)") == STR_EQ)
  {
    return CFG_MEM_DISPLAY_CODE;
  }
  if (compareString(dataBuffer, "Memory Available (KB)") == STR_EQ)
  {
    return CFG_MEM_AVAILABLE_CODE;
  }
  if (compareString(dataBuffer, "Processor Cycle Time (msec)") == STR_EQ)
  {
    return CFG_PROC_CYCLES_CODE;
  }
  if (compareString(dataBuffer, "I/O Cycle Time (msec)") == STR_EQ)
  {
    return CFG_IO_CYCLES_CODE;
  }
  if (compareString(dataBuffer, "Log To") == STR_EQ)
  {
    return CFG_LOG_TO_CODE;
  }
  if (compareString(dataBuffer, "Log File Path") == STR_EQ)
  {
    return CFG_LOG_FILE_NAME_CODE;
  }
  return CFG_CORRUPT_PROMPT_ERR;
}

ConfigDataCodes getLogToCode(const char *lowerCaseLogToStr)
{
  ConfigDataCodes returnVal = LOGTO_MONITOR_CODE;

  if (compareString(lowerCaseLogToStr, "both") == STR_EQ)
  {
    returnVal = LOGTO_BOTH_CODE;
  }
  if (compareString(lowerCaseLogToStr, "file") == STR_EQ)
  {
    returnVal = LOGTO_FILE_CODE;
  }
  return returnVal;
}

/**
 * function to parse config data values in a given interval
 */
_Bool valueInRange(int lineCode, int intVal, double doubleVal,
                   const char *lowerCaseStringVal)
{
  _Bool result = true;
  switch (lineCode)
  {
  case CFG_VERSION_CODE:
    if (doubleVal < 0.00 || doubleVal > 10.00)
    {
      result = false;
    }
    break;
  case CFG_CPU_SCHED_CODE:
    if (compareString(lowerCaseStringVal, "fcfs-n") != STR_EQ &&
        compareString(lowerCaseStringVal, "sjf-n") != STR_EQ &&
        compareString(lowerCaseStringVal, "srtf-p") != STR_EQ &&
        compareString(lowerCaseStringVal, "fcfs-p") != STR_EQ &&
        compareString(lowerCaseStringVal, "rr-p") != STR_EQ)
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
    if (compareString(lowerCaseStringVal, "on") != STR_EQ &&
        compareString(lowerCaseStringVal, "off") != STR_EQ)
    {
      result = false;
    }
    break;
  case CFG_MEM_AVAILABLE_CODE:
    if (intVal < 1024 || intVal > 102400)
    {
      result = false;
    }
    break;
  case CFG_PROC_CYCLES_CODE:
    if (intVal < 1 || intVal > 100)
    {
      result = false;
    }
    break;
  case CFG_IO_CYCLES_CODE:
    if (intVal < 1 || intVal > 1000)
    {
      result = false;
    }
    break;
  case CFG_LOG_TO_CODE:
    if (compareString(lowerCaseStringVal, "both") != STR_EQ &&
        compareString(lowerCaseStringVal, "file") != STR_EQ &&
        compareString(lowerCaseStringVal, "monitor") != STR_EQ)
    {
      result = false;
    }
    break;
  }
  return result;
}

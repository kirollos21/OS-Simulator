#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdio.h>
#include <stdbool.h>
#include "StandardConstants.h"

//GLOBAL CONSTANTS - used in other files

//Op code enum
typedef enum {
    CMD_STR_LEN = 5,
    IO_ARG_STR_LEN = 5,
    STR_ARG_LEN = 15
} OpCodeArrayCapacity;

//Five state data structure for processes
typedef enum {
    NEW_STATE,
    READY_STATE,
    RUNNING_STATE,
    BLOCKED_STATE,
    EXIT_STATE
} ProcessState;

//Program constants
typedef enum {MIN_NUM_ARGS = 3, LAST_FOUR_LETTERS = 4} PRGRM_CONSTANTS;

//command line struct for storing data
typedef struct CmdLineDataStruct
{
    bool programRunFlag;
    bool configDisplayFlag;
    bool mdDisplayFlag;
    bool runSimFlag;
    
    char fileName [STD_STR_LEN];
} CmdLineData;

//struct defnition for Config data
typedef struct ConfigDataTypeStruct
{
    float version;
    char metaDataFilename [STD_STR_LEN];
    int cpuSchedCode;
    char fileName [STD_STR_LEN];
    int quantumCycles;
    bool memDisplay;
    int memAvailable;
    int procCycleRate;
    int ioCycleRate;
    int logToCode;
    char logToFileName [STD_STR_LEN];
} ConfigDataType;

//struct definition for Meta data
typedef struct MetaDataTypeStruct
{
    int pid;
    char command[STD_STR_LEN];
    char inOutArg[STD_STR_LEN];
    char strArg1[STD_STR_LEN];
    int intArg2;
    int intArg3;
    double opEndTime;
    struct MetaDataTypeStruct* nextNode;

} OpCodeType;

//defines enum for config messages
typedef enum {
    CFG_FILE_ACCESS_ERR,
    CFG_CORRUPT_DESCRIPTION_ERR,
    CFG_DATA_OUT_OF_RANGE_ERR,
    CFG_CORRUPT_PROMPT_ERR,
    CFG_VERSION_CODE,
    CFG_MD_FILE_NAME_CODE,
    CFG_CPU_SCHED_CODE,
    CFG_QUANT_CYCLES_CODE,
    CFG_MEM_DISPLAY_CODE,
    CFG_MEM_AVAILABLE_CODE,
    CFG_PROC_CYCLES_CODE,
    CFG_IO_CYCLES_CODE,
    CFG_LOG_TO_CODE,
    CFG_LOG_FILE_NAME_CODE 
} ConfigCodeMessages;

//defines enum for config data messages
typedef enum {
    CPU_SCHED_SJF_N_CODE,
    CPU_SCHED_SRTF_P_CODE,
    CPU_SCHED_FCFS_P_CODE,
    CPU_SCHED_RR_P_CODE,
    CPU_SCHED_FCFS_N_CODE,
    LOGTO_MONITOR_CODE,
    LOGTO_FILE_CODE,
    LOGTO_BOTH_CODE,
    NON_PREEMPTIVE_CODE,
    PREEMPTIVE_CODE
} ConfigDataCodes;

#endif
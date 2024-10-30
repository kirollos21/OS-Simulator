/**
 * Custom data types needed for sim01
 */
#ifndef TYPES_H
#define TYPES_H

#include "constants.h"
#include <stdio.h>

// enum representing numbers to avoid using numeric literals
// that dont correspond to existing enums
typedef enum Numbers
{
  ZERO = 0,
  ONE = 1,
  TWO = 2,
  THREE = 3
} Numbers;

// boolean enum for timer code from leverington
typedef enum
{
  True,
  False
} Boolean;

typedef enum OpCodeArrayCapacity
{
  CMD_STR_LEN = 5,
  IO_ARG_STR_LEN = 5,
  STR_ARG_LEN = 15
} OpCodeArrayCapacity;

typedef struct ConfigDataTypeStruct
{
  char metaDataFileName[STD_STR_LEN];
  int cpuSchedCode;
  int ioCycleRate;
  int memAvailable;
  int logToCode;
  char logToFileName[STD_STR_LEN];
  _Bool memDisplay;
  int procCycleRate;
  double version;
  int quantumCycles;
} ConfigDataType;

typedef enum ConfigDataCodes
{
  CPU_SCHED_SJF_N_CODE = 0,
  CPU_SCHED_SRTF_P_CODE = 1,
  CPU_SCHED_FCFS_P_CODE = 2,
  CPU_SCHED_RR_P_CODE = 3,
  CPU_SCHED_FCFS_N_CODE = 4,
  LOGTO_MONITOR_CODE = 5,
  LOGTO_FILE_CODE = 6,
  LOGTO_BOTH_CODE = 7,
  NON_PREEMPTIVE_CODE = 8,
  PREEMPTIVE_CODE = 9,
  WAIT = -505,
  EXIT = -606,
  NULL_PID = -1
} ConfigDataCodes;

typedef struct OpCodeTypeStruct
{
  int pid;
  char command[CMD_STR_LEN];
  char inOutArg[IO_ARG_STR_LEN];
  char strArg1[STR_ARG_LEN];
  int intArg2;
  int intArg3;
  int op_time;
  double opEndTime;
  struct OpCodeTypeStruct *next_ptr;
} OpCodeType;

typedef enum OpCodeMessages
{
  BAD_ARG_VAL = -1,
  NO_ACCESS_ERR = 0,
  MD_FILE_ACCESS_ERR = 1,
  MD_CORRUPT_DESCRIPTOR_ERR = 2,
  OPCMD_ACCESS_ERR = 3,
  CORRUPT_OPCMD_ERR = 4,
  CORRUPT_OPCMD_ARG_ERR = 5,
  UNBALANCED_START_END_ERR = 6,
  COMPLETE_OPCMD_FOUND_MSG = 7,
  LAST_OPCMD_FOUND_MSG = 8

} OpCodeMessages;

typedef enum ConfigCodeMessages
{
  CFG_FILE_ACCESS_ERR,
  CFG_CORRUPT_DATA_DESCRIPTOR_ERR,
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

#endif

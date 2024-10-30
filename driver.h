#ifndef DRIVER_H
#define DRIVER_h

#include "StringUtils.h"
#include "configops.h"
#include "metadataops.h"
#include "simulator.h"
#include "types.h"
#include <stdio.h>

typedef enum
{
  MIN_NUM_ARGS = 3,
  LAST_FOUR_LETTERS = 4
} PRGRM_CONSTANTS;

/**
 * Command Line Data struct
 */
typedef struct CmdLineDataStruct
{
  _Bool programRunFlag;
  _Bool configDisplayFlag;
  _Bool mdDisplayFlag;
  _Bool runSimFlag;
  char fileName[STD_STR_LEN];
} CmdLineData;

/**
 * clear CMD Line struct
 */
void clearCmdLineStruct(CmdLineData *clDataPtr);

/**
 * Parse CMD Line
 */
_Bool processCmdLine(int numArgs, char **strVector, CmdLineData *clDataPtr);

/**
 * Command Line Usage
 */
void usage();

#endif

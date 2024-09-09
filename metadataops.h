#ifndef METADATA_OPS_H
#define METADATA_OPS_H

#include "StandardConstants.h"

typedef struct OpCodeType {
    int pid;
    char command[CMD_STR_LEN];
    char inOutArg[IO_ARG_STR_LEN];
    char strArg1[STR_ARG_LEN];
    int intArg2;
    int intArg3;
    double opEndTime;
    struct OpCodeType *nextNode;
} OpCodeType;

int readMetaDataFile(const char *fileName, OpCodeType **opCodeData);

#endif

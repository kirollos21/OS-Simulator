#ifndef CONFIG_OPS_H
#define CONFIG_OPS_H

#include "StandardConstants.h"

typedef struct ConfigDataType {
    double version;
    char metaDataFileName[LARGE_STR_LEN];
    int cpuSchedCode;
    int quantumCycles;
    bool memDisplay;
    int memAvailable;
    int procCycleRate;
    int ioCycleRate;
    int logToCode;
    char logToFileName[LARGE_STR_LEN];
} ConfigDataType;

int readConfigFile(const char *fileName, ConfigDataType *configData);

#endif

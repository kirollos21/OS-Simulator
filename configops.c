#include "configops.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int readConfigFile(const char *fileName, ConfigDataType *configData) {
    FILE *filePtr = fopen(fileName, "r");
    char line[LARGE_STR_LEN];
    if (filePtr == NULL) {
        return -1;
    }

    while (fgets(line, LARGE_STR_LEN, filePtr) != NULL) {
        if (strncmp(line, "Version/Phase", 13) == 0) {
            sscanf(line, "Version/Phase : %lf", &configData->version);
        } else if (strncmp(line, "File Path", 9) == 0) {
            sscanf(line, "File Path : %s", configData->metaDataFileName);
        } else if (strncmp(line, "CPU Scheduling Code", 19) == 0) {
            char schedCode[LARGE_STR_LEN];
            sscanf(line, "CPU Scheduling Code : %s", schedCode);
            if (strcmp(schedCode, "FCFS-N") == 0) {
                configData->cpuSchedCode = 0; // Example code for FCFS-N
            } else if (strcmp(schedCode, "SJF-N") == 0) {
                configData->cpuSchedCode = 1; // Example code for SJF-N
            } else if (strcmp(schedCode, "SRTF-P") == 0) {
                configData->cpuSchedCode = 2; // Example code for SRTF-P
            } else if (strcmp(schedCode, "FCFS-P") == 0) {
                configData->cpuSchedCode = 3; // Example code for FCFS-P
            } else if (strcmp(schedCode, "RR-P") == 0) {
                configData->cpuSchedCode = 4; // Example code for RR-P
            }
        } else if (strncmp(line, "Quantum Time (cycles)", 21) == 0) {
            sscanf(line, "Quantum Time (cycles) : %d", &configData->quantumCycles);
        } else if (strncmp(line, "Memory Available", 16) == 0) {
            sscanf(line, "Memory Available (KB) : %d", &configData->memAvailable);
        } else if (strncmp(line, "Memory Display", 14) == 0) {
            char displayOpt[LARGE_STR_LEN];
            sscanf(line, "Memory Display (On/Off) : %s", displayOpt);
            configData->memDisplay = (strcmp(displayOpt, "On") == 0) ? true : false;
        } else if (strncmp(line, "Processor Cycle Time", 20) == 0) {
            sscanf(line, "Processor Cycle Time (msec) : %d", &configData->procCycleRate);
        } else if (strncmp(line, "I/O Cycle Time", 14) == 0) {
            sscanf(line, "I/O Cycle Time (msec) : %d", &configData->ioCycleRate);
        } else if (strncmp(line, "Log To", 6) == 0) {
            char logOpt[LARGE_STR_LEN];
            sscanf(line, "Log To : %s", logOpt);
            if (strcmp(logOpt, "Monitor") == 0) {
                configData->logToCode = 0; // Example code for Monitor
            } else if (strcmp(logOpt, "File") == 0) {
                configData->logToCode = 1; // Example code for File
            } else if (strcmp(logOpt, "Both") == 0) {
                configData->logToCode = 2; // Example code for Both
            }
        } else if (strncmp(line, "Log File Path", 13) == 0) {
            sscanf(line, "Log File Path : %s", configData->logToFileName);
        }
    }

    fclose(filePtr);
    return 0;
}
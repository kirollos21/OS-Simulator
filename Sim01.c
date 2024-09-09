#include "configops.h"
#include "metadataops.h"
#include "simulator.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    ConfigDataType configData;
    OpCodeType *opCodeData = NULL;

    // Check if the correct number of arguments is provided
    if (argc < 3) {
        printf("Usage: %s <-dc|-dm|-rs> <config_file.cnf>\n", argv[0]);
        return 1;
    }

    // Read configuration file
    if (readConfigFile(argv[2], &configData) == -1) {
        printf("Error reading configuration file: %s\n", argv[2]);
        return 1;
    }

    // Handle the -dc, -dm, and -rs arguments
    if (strcmp(argv[1], "-dc") == 0) {
        // Display configuration data
        printf("Config File Display\n");
        printf("-------------------\n");
        printf("Version: %.2f\n", configData.version);
        printf("Meta-Data File Name: %s\n", configData.metaDataFileName);
        printf("CPU Scheduling Code: %d\n", configData.cpuSchedCode);
        printf("Quantum Cycles: %d\n", configData.quantumCycles);
        printf("Memory Display: %s\n", configData.memDisplay ? "On" : "Off");
        printf("Memory Available: %d KB\n", configData.memAvailable);
        printf("Processor Cycle Rate: %d\n", configData.procCycleRate);
        printf("I/O Cycle Rate: %d\n", configData.ioCycleRate);
        printf("Log To Code: %d\n", configData.logToCode);
        printf("Log To File Name: %s\n", configData.logToFileName);
    } 
    else if (strcmp(argv[1], "-dm") == 0) {
        // Read and display meta-data file
        if (readMetaDataFile(configData.metaDataFileName, &opCodeData) == -1) {
            printf("Error reading meta-data file: %s\n", configData.metaDataFileName);
            return 1;
        }

        // Display meta-data file content
        printf("Meta-Data File Display\n");
        printf("----------------------\n");
        OpCodeType *currentOp = opCodeData;
        while (currentOp != NULL) {
            printf("Op Code: /pid: %d/cmd: %s/io: %s\n",
                   currentOp->pid, currentOp->command, currentOp->inOutArg);
            currentOp = currentOp->nextNode;
        }
    } 
    else if (strcmp(argv[1], "-rs") == 0) {
        // Run simulation
        runSim();
    } 
    else {
        printf("Invalid argument. Use -dc to display config, -dm to display meta-data, or -rs to run simulator.\n");
    }

    return 0;
}
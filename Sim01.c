#include "configops.h"
#include "metadataops.h"
#include "simulator.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    ConfigDataType configData;
    OpCodeType *opCodeData = NULL;

    // Read configuration file
    if (readConfigFile("configN.cnf", &configData) == -1) {
        printf("Error reading configuration file\n");
        return 1;
    }

    // Read meta-data file
    if (readMetaDataFile(configData.metaDataFileName, &opCodeData) == -1) {
        printf("Error reading meta-data file\n");
        return 1;
    }

    // Run the simulator
    runSim();

    return 0;
}

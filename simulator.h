//  File: simulator.h
//  Project: Sim01
//  Name: Kirollos Zikry
//  Date: 09/08/2024

#ifndef simulator_h
#define simulator_h
#include "datatypes.h"
#include "StandardConstants.h"
#include <stdio.h>

/*
Name: runSim
process: primary simulation driver
Function Input/Parameters:  configuration data (ConfigDataType *),
                            metadata (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: tbd
*/
void runSim(ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr);

#endif
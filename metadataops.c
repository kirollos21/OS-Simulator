#include "metadataops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int readMetaDataFile(const char *fileName, OpCodeType **opCodeData) {
    FILE *filePtr = fopen(fileName, "r");
    char line[STR_ARG_LEN];
    OpCodeType *newNode, *currentNode;
    
    if (filePtr == NULL) {
        return -1; // Error opening file
    }

    // Initialize the linked list
    *opCodeData = NULL;

    // Parse the file contents line by line
    while (fgets(line, STR_ARG_LEN, filePtr) != NULL) {
        newNode = (OpCodeType *)malloc(sizeof(OpCodeType));
        if (newNode == NULL) {
            fclose(filePtr);
            return -1; // Memory allocation error
        }

        // Parse each line into the OpCodeType structure
        sscanf(line, "/pid: %d/cmd: %s/io: %s/arg1: %s/arg2: %d/arg3: %d", 
                &newNode->pid, 
                newNode->command, 
                newNode->inOutArg, 
                newNode->strArg1, 
                &newNode->intArg2, 
                &newNode->intArg3);

        newNode->nextNode = NULL;

        // Add the new node to the linked list
        if (*opCodeData == NULL) {
            *opCodeData = newNode; // First node
        } else {
            currentNode = *opCodeData;
            while (currentNode->nextNode != NULL) {
                currentNode = currentNode->nextNode; // Traverse to the end
            }
            currentNode->nextNode = newNode; // Add the new node to the end
        }
    }

    fclose(filePtr);
    return 0; // Success
}

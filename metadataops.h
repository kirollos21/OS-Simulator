
#ifndef METADATA_OPS_H
#define METADATA_OPS_H


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "datatypes.h"
#include "StandardConstants.h"
#include "StringUtils.h"

//define enum for Op codes
typedef enum {
        BAD_ARG_VAL = -1,
        NO_ACCESS_ERR,
        MD_FILE_ACCESS_ERR,
        MD_CORRUPT_DESCRIPTOR_ERR,
        OPCMD_ACCESS_ERR,
        CORRUPT_OPCMD_ERR,
        CORRUPT_OPCMD_ARG_ERR,
        UNBALANCED_START_END_ERR,
        COMPLETE_OPCMD_FOUND_MSG,
        LAST_OPCMD_FOUND_MSG 
} OpCodeMessages;

/*
Name: addNode
Process: adds a Node to the meta data linked list
Function input/parameters: pointer to OpCodeType and new node
Function output/parameters: none
Function output/returned: pointer to new node added
Device input/---: none
Device output/---: none
Dependencies: malloc, copyString
*/
OpCodeType *addNode(OpCodeType *localPtr, OpCodeType *newNode);

/*
Name: displayMetaData
Process: displays metadata to user
Function input/parameters: pointer to metadata linked list
Function output/parameters: none
Function output/returned: none
Device input/---: none
Device output/---: none
Dependencies: printf
*/
void displayMetaData(const OpCodeType *localPtr);

/*
Name: clearMetaDataList
Process: clears metadata linkedlist
Function input/parameters: metadata linked list
Function output/parameters: none
Function output/returned: none
Device input/---: none
Device output/---: none
Dependencies: clearMetaDataList, free
*/
OpCodeType* clearMetaDataList(OpCodeType *localPtr);

/*
Name: getCommand
Process: gets first command in a given string
Function input/parameters: input string and index
Function output/parameters: command str
Function output/returned: int of cmd index
Device input/---: none
Device output/---: none
Dependencies: none
*/
int getCommand(char* cmd, const char* inputStr, int index);

/*
Name: getMetaData
Process: reads file of meta data and 
Function input/parameters: Filename, metadate linked list
Function output/parameters: endstatestring
Function output/returned: bool for success or failure
Device input/---: none
Device output/---: none
Dependencies: fopen, copyString, compareStrings, getStringToDelimiter
              fclose, malloc, getOpCommand, updateStartCount, 
              updateEndCount, clearMetaDataList, free
*/
bool getMetaData(const char* filename, OpCodeType **opcodeDataHead,
    char* endStateMsg);

/*
Name: getOpCommand
Process: gdetermine what commands were gotten and verify it as valid
Function input/parameters: file and opCodeType
Function output/parameters: command str
Function output/returned: int of cmd index
Device input/---: none
Device output/---: none
Dependencies: getStringToDelimiter, getCommand, copyString
              verifyValidCommand, compareStrings, getStringArg,
              getNumArg
*/
OpCodeMessages getOpCommand(FILE* filePtr, OpCodeType *inData);

/*
Name: getNumberArg
Process: find number of arguments passed in
Function input/parameters: int number, input string, index
Function output/parameters: none
Function output/returned: int of arguments
Device input/---: none
Device output/---: none
Dependencies: isDigit
*/
int getNumberArg(int* number, const char* inputStr, int index);

/*
Name: getStringArg
Process: get argument of string
Function input/parameters: char* strArg, inputStr, int index
Function output/parameters: none
Function output/returned: int of arguments
Device input/---: none
Device output/---: none
Dependencies: isDigit
*/
int getStringArg(char *strArg, const char *inputStr, int index);

/*
Name: isDigit
Process: tests if a char is a digit
Function input/parameters: test character
Function output/parameters: none
Function output/returned: true for digit, false for not
Device input/---: none
Device output/---: none
Dependencies: none
*/
bool isDigit(char testChar);

/*
Name: updateEndCount
Process: updates where the end index of a string is
Function input/parameters: current count, test string
Function output/parameters: none
Function output/returned: change in count
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
int updateEndCount(int count, const char *opString);

/*
Name: updateStartCount
Process: updates where the start index of a string is
Function input/parameters: current count, test string
Function output/parameters: none
Function output/returned: change in count
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
int updateStartCount(int count, const char *opString);

/*
Name: verifyFirstStringArg
Process: tests if first string argument is a valid input
Function input/parameters: string
Function output/parameters: none
Function output/returned: bool if valid or not
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
bool verifyFirstStringArg(const char *strArg);

/*
Name: verifyFirstStringArg
Process: tests if command string argument is a valid input
Function input/parameters: string
Function output/parameters: none
Function output/returned: bool if valid or not
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
bool verifyValidCommand(char *testCmd);


#endif
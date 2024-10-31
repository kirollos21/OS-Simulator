//  File: metadataops.h
//  Project: Sim03
//  Secret ID: 708996

#ifndef metadataops_h
#define metadataops_h

#include "StringUtils.h"
#include "StandardConstants.h"
#include "datatypes.h"

/*
Name: addNode
Process: adds metadata node to linked list recursively,
         handles empty list condition
Function Input/Parameters: pointer to head or next linked node (OpCodeType *)
                           pointer to new node (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: pointer to previous node, or head node (OpCodeType *)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: malloc, copyString
*/
OpCodeType *addNode(OpCodeType *localPtr, OpCodeType *newNode);

/*
Name: displayMetaData
Process: data dump/display of all op code items
Function Input/Parameters: pointer to head of
                           op code/metadata list (const OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: displayed as specified
Dependencies: printf, compareString
*/
void displayMetaData(const OpCodeType *local_ptr);

/*
Name: getMetaData
Process: main driver function to upload, parse, and store list
         of op code commands in a linked list
Function Input/Parameters: file name (const char *)
Function Output/Parameters: pointer to op code linked list
                            head pointer (OpCodeType **),
                            result message of function state
                            after completion (char *)
Function Output/Returned: Boolean result of operation (bool)
Device Input/File: op code list uploaded
Device Output/Device: none
Dependencies: copyString, fopen, getStringToDelimiter, compareString, fclose,
              malloc, getOpCommand, updateStartCount, updateEndCount,
              clearMetaDataList, free, addNode
*/
bool getMetaData(const char *fileName, OpCodeType **opCodeDataHead, char *endStateMsg);

/*
Name: clearMetaData
Process: recursively traverses list, frees dynamically allocated nodes
Function Input/Parameters: node op code (const OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: NULL (OpCodeType *)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: tbd
*/
OpCodeType *clearMetaDataList(OpCodeType *localPtr);

/*
Name: getOpCommand
Process: acquires one op command line from a previously opened file,
         parses it, and sets various struct members according
         to the three letter command
Function Input/Parameters: pointer to open file handle (FILE *)
Function Output/Parameters: pointer to one op code struct (OpCodeType *)
Function Output/Returned: coded result of operation (OpCodeMessages)
Device Input/File: op code line uploaded
Device Output/Device: none
Dependencies: getStringToDelimiter, getCommand, copyString, verifyValidCommand,
              compareString, getStringArg, verifyFirstStringArg, getNumberArg
*/
int getCommand(char *cmd, const char *inputStr, int index);

/*
Name: getOpCommand
Process: acquires one op command line from a previously opened file,
         parses it, and sets various struct members according
         to the three letter command
Function Input/Parameters: pointer to open file handle (FILE *)
Function Output/Parameters: pointer to one op code struct (OpCodeType *)
Function Output/Returned: coded result of operation (OpCodeMessages)
Device Input/File: op code line uploaded
Device Output/Device: none
Dependencies: getStringToDelimiter, getCommand, copyString, verifyValidCommand,
              compareString, getStringArg, verifyFirstStringArg, getNumberArg
*/
OpCodeMessages getOpCommand(FILE *filePtr, OpCodeType *inData);

/*
Name: getStringArg
Process: starts at given index, captures and assembles string argument,
         and returns as parameter
Function Input/Parameters: input string (const char *), starting index (int)
Function Output/Parameters: pointer to captured string argument (char *)
Function Output/Returned: updated index for next function start
Device Input/File: none
Device Output/Device: none
Dependencies: none
*/
int getStringArg(char *strArg, const char *inputStr, int index);

/*
Name: getNumberArg
Process: starts at given index, captures and assembles integer argument,
         and returns as parameter
Function Input/Parameters: input string (const char *), starting index (int)
Function Output/Parameters: pointer to captured integer value
Function Output/Returned: updated index for next function start
Device Input/File: none
Device Output/Device: none
Dependencies: isDigit
*/
int getNumberArg(int *number, const char *inputStr, int index);

/*
Name: verifyFirstStringArg
Process: checks for all possibilities of first argument string of op command
Function Input/Parameters: test string (const char *)
Function Output/Parameters: none
Function Output/Returned: Boolean result of test (bool)
Device Input/File: none
Device Output/Device: none
Dependencies: compareString
*/
bool verifyFirstStringArg(const char *strArg);

/*
Name: verifyValidCommand
Process: checks for all possibilities of three-letter op command
Function Input/Parameters: test string for command (char *)
Function Output/Parameters: none
Function Output/Returned: Boolean result of test (bool)
Device Input/File: none
Device Output/Device: none
Dependencies: compareString
*/
bool verifyValidCommand(char *testCmd);

/*
Name: isDigit
Process: tests character parameter for digit, returns true if is digit,
         false otherwise
Function Input/Parameters: test character (char)
Function Output/Parameters: none
Function Output/Returned: Boolean result of test (bool)
Device Input/File: none
Device Output/Device: none
Dependencies: none
*/
bool isDigit(char testChar);

/*
Name: updateStartCount
Process: manages count of "start" arguments to be compared at end
         of process input
Function Input/Parameters: initial count (int)
                           test string for "start" (const char *)
Function Output/Parameters: none
Function Output/Returned: updated count, if "start" string found,
                          otherwise no change
Device Input/File: none
Device Output/Device: none
Dependencies: captureString
*/
int updateStartCount(int count, const char *opString);

/*
Name: updateEndCount
Process: manages count of "end" arguments to be compared at end
         of process input
Function Input/Parameters: initial count (int)
                           test string for "end" (const char *)
Function Output/Parameters: none
Function Output/Returned: updated count, if "end" string found,
                          otherwise no change
Device Input/File: none
Device Output/Device: none
Dependencies: captureString
*/
int updateEndCount(int count, const char *opString);

#endif

// protect from multiple compiling 
#ifndef METADATAOPS_H
#define METADATAOPS_H

// header files
#include "datatypes.h"
#include "StandardConstants.h"
#include "StringUtils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// constants

typedef struct OpCodeType
{
    int pid;
    char command[MAX_STR_LEN];
    char inOutArg[MAX_STR_LEN];
    char strArg1[MAX_STR_LEN];
    int intArg2;
    int intArg3;
    double opEndTime;
    struct OpCodeType *nextNode;
} OpCodeType;


typedef enum { BAD_ARG_VAL = -1, 
               NO_ACCESS_ERR,
               MD_FILE_ACCESS_ERR,
               MD_CORRUPT_DESCRIPTOR_ERR,
               OPCMD_ACCESS_ERR,
               CORRUPT_OPCMD_ERR,
               CORRUPT_OPCMD_ARG_ERR,
               UNBALANCED_START_END_ERR,
               COMPLETE_OPCMD_FOUND_MSG,
               LAST_OPCMD_FOUND_MSG } OpCodeMessages;
 

// Function prototypes

/*
 Name: addNode
 Process: adds metadata node to linked list recursively, handles empty list condition
 Function Input/Parameters: pointer to head or next linked node (OpCodeType *),
                            pointer to new node (OpCodeType *)
 Function Output/Parameters: none
 Function Output/Returned: pointer to previous node, or head node (OpCodeType *)
 */
OpCodeType *addNode( OpCodeType *localPtr, OpCodeType *newNode );

/*
 Name: clearMetaDataList
 Process: recursively traverses list, frees dynamically allocated nodes
 Function Input/Parameters: node op code (const OpCodeType *)
 Function Output/Parameters: none
 Function Output/Returned: NULL (OpCodeType *)
 */
OpCodeType *clearMetaDataList( OpCodeType *localPtr );

/*
 Name: displayMetaData
 Process: data dump/display of all op code items
 Function Input/Parameters: pointer to head of op code/metadata list (const OpCodeType *)
 Function Output/Parameters: none
 Function Output/Returned: none
 */
void displayMetaData( const OpCodeType *localPtr );

/*
 Name: getCommand
 Process: parses three-letter command part of op code string
 Function Input/Parameters: input op code string (const char *),
                            starting index (int)
 Function Output/Parameters: parsed command (char *)
 Function Output/Returned: updated starting index for use by calling function (int)
 */
int getCommand( char *cmd, const char *inputStr, int index );

/*
 Name: getMetaData
 Process: main driver function to upload, parse, and store list of op code commands in a linked list
 Function Input/Parameters: file name (const char *)
 Function Output/Parameters: pointer to op code linked list head pointer (OpCodeType **),
                             result message of function state after completion (char *)
 Function Output/Returned: Boolean result of operation (bool)
 */
bool getMetaData( const char *fileName, OpCodeType **opCodeDataHead, char *endStateMsg );

/*
 Name: getOpCommand
 Process: acquires one op command line from a previously opened file, parses it,
          and sets various struct members according to the three-letter command
 Function Input/Parameters: pointer to open file handle (FILE *),
                            pointer to one op code struct (OpCodeType *)
 Function Output/Returned: code result of operation (OpCodeMessages)
 */
OpCodeMessages getOpCommand( FILE *filePtr, OpCodeType *inData );

/*
 Name: getNumberArg
 Process: starts at given index, captures and assembles integer argument,
          and returns as parameter
 Function Input/Parameters: input string (const char *),
                            starting index (int)
 Function Output/Returned: updated index for next function start (int)
 */
int getNumberArg( int *number, const char *inputStr, int index );

/*
 Name: getStringArg
 Process: starts at given index, captures and assembles string argument,
          and returns as parameter
 Function Input/Parameters: input string (const char *),
                            starting index (int)
 Function Output/Returned: updated index for next function start (int)
 */
int getStringArg( char *strArg, const char *inputStr, int index );

/*
 Name: isDigit
 Process: tests character parameter for digit, returns true if it is a digit, false otherwise
 Function Input/Parameters: test character (char)
 Function Output/Returned: Boolean result of test (bool)
 */
bool isDigit( char testChar );

/*
 Name: updateEndCount
 Process: manages count of "end" arguments to be compared at end of process input
 Function Input/Parameters: initial count (int),
                            test string for "end" (const char *)
 Function Output/Returned: updated count, if "end" string found, otherwise no change (int)
 */
int updateEndCount( int count, const char *opString );

/*
 Name: updateStartCount
 Process: manages count of "start" arguments to be compared at end of process input
 Function Input/Parameters: initial count (int),
                            test string for "start" (const char *)
 Function Output/Returned: updated count, if "start" string found, otherwise no change (int)
 */
int updateStartCount( int count, const char *opString );

/*
 Name: verifyFirstStringArg
 Process: checks for all possibilities of the first argument string of op command
 Function Input/Parameters: test string (const char *)
 Function Output/Returned: Boolean result of test (bool)
 */
bool verifyFirstStringArg( const char *strArg );

/*
 Name: verifyValidCommand
 Process: checks for all possibilities of the three-letter op code command
 Function Input/Parameters: test string (const char *)
 Function Output/Returned: Boolean result of test (bool)
 */
bool verifyValidCommand( char *testCmd );

#endif // METADATA_OPS_H
/**
 * Meta Data Operations
 */
#ifndef METADATAOPS_H
#define METADATAOPS_H

#include "StringUtils.h"
#include "constants.h"
#include "types.h"
#include <stdio.h>

/**
 * add note to LL
 */
OpCodeType *addNode(OpCodeType *localPtr, OpCodeType *newNode);

/**
 * display metadata information
 */
void displayMetaData(const OpCodeType *local_ptr);

/**
 * retrieve metadata information
 */
_Bool getMetaData(const char *fileName, OpCodeType **opCodeDataHead,
                  char *endStateMsg);
/**
 * clear metadata ll
 */
OpCodeType *clearMetaDataList(OpCodeType *localPtr);

int getCommand(char *cmd, const char *inputStr, int index);

OpCodeMessages getOpCommand(FILE *filePtr, OpCodeType *inData);

int getStringArg(char *strArg, const char *inputStr, int index);

int getNumberArg(int *number, const char *inputStr, int index);

_Bool verifyFirstStringArg(const char *strArg);

_Bool verifyValidCommand(char *testCmd);

_Bool isDigit(char testChar);

int updateStartCount(int count, const char *opString);

int updateEndCount(int count, const char *opString);

#endif

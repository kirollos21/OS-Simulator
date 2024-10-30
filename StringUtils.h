// Preprocessor directive
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// header files
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "StandardConstants.h"

// Prototypes
int compareString( const char *oneStr, const char *otherStr );
void concatenateString( char *destStr, const char *sourceStr );
void copyString( char *destStr, const char *sourceStr );
int findSubString( const char *testStr, const char *searchSubStr );
bool getStringConstrained( FILE *inStream, bool clearLeadingNonPrintable, 
                               bool clearLeadingSpace, bool stopAtNonPrintable,
                                         char delimiter, char *capturedString);
int getStringLength( const char *string );
void getSubString( char *destStr, const char *sourceStr,
                                                 int startIndex, int endIndex);
bool getStringToDelimiter( FILE *inStream, char delimiter,
                                                        char *capturedString );
bool getStringToLineEnd( FILE *inStream, char *captureString );
void setStrToLowerCase(char *destStr, const char * sourceStr );
char toLowerCase( char testChar );
#endif // STRING_UTILS_H

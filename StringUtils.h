#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdbool.h>
#include <stdio.h>

// Function prototypes

/*
 Name: compareString
 Process: compares two strings and returns:
          - less than zero if left string is less than right string,
          - greater than zero if left string is greater than right string,
          - zero if both strings are equal.
 Function Input/Parameters: two c-style strings (char *)
 Function Output/Returned: result as specified (int)
 */
int compareString(const char *oneStr, const char *otherStr);

/*
 Name: concatenateString
 Process: appends one string onto another.
 Function Input/Parameters: destination and source c-style strings (char *)
 Function Output/Returned: none
 */
void concatenateString(char *destStr, const char *sourceStr);

/*
 Name: copyString
 Process: copies a string into another, overwriting destination string.
 Function Input/Parameters: destination and source c-style strings (char *)
 Function Output/Returned: none
 */
void copyString(char *destStr, const char *sourceStr);

/*
 Name: findSubString
 Process: searches for a substring within a larger string.
 Function Input/Parameters: test string (char *), search substring (char *)
 Function Output/Returned: index of the found substring or SUBSTRING_NOT_FOUND.
 */
int findSubString(const char *testStr, const char *searchSubStr);

/*
 Name: getStringConstrained
 Process: captures a string from the input stream with various constraints.
 Function Input/Parameters: input stream (FILE *), control booleans and delimiter.
 Function Output/Returned: success of operation (bool).
 */
bool getStringConstrained(FILE *inStream, bool clearLeadingNonPrintable, bool clearLeadingSpace, bool stopAtNonPrintable, char delimiter, char *capturedString);

/*
 Name: getStringLength
 Process: finds the length of a string by counting characters up to the NULL_CHAR.
 Function Input/Parameters: c-style string (char *)
 Function Output/Returned: length of the string (int).
 */
int getStringLength(const char *testStr);

/*
 Name: getStringtoDelimiter
 Process: captures a string from the input stream up to a specified delimiter.
 Function Input/Parameters: input stream (FILE *), delimiter (char)
 Function Output/Returned: success of operation (bool).
 */
bool getStringToDelimiter(FILE *inStream, char delimiter, char *capturedString);

/*
 Name: setStrToLowerCase
 Process: sets all upper-case letters in a string to lower-case.
 Function Input/Parameters: destination and source c-style strings (char *)
 Function Output/Returned: none.
 */
void setStrToLowerCase(char *destStr, const char *sourceStr);

/*
 Name: toLowerCase
 Process: converts a character to lower-case if it is upper-case.
 Function Input/Parameters: a character (char).
 Function Output/Returned: the converted lower-case character.
 */
char toLowerCase(char testChar);

/*
 Name: getStringToLineEnd
 Process: captures a string from the input stream up to the end of the line.
 Function Input/Parameters: input stream (FILE *), captured string (char *)
 Function Output/Returned: success of operation (bool).
 */
bool getStringToLineEnd(FILE *inStream, char *capturedString);

/*
 Name: getSubString
 Process: extracts a substring from a larger string between two indices.
 Function Input/Parameters: destination and source strings (char *), start and end indices (int).
 Function Output/Returned: none.
 */
void getSubString(char *destStr, const char *sourceStr, int startIndex, int endIndex);

#endif // STRING_UTILS_H
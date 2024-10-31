//  File: StringUtils.c
//  Project: Sim03
//  Secret ID: 708996

#include "StringUtils.h"

/*
Name: compareString
Process: compares two strings with the following results:
         if left string less than right string, returns less than zero
         if left string greater than right string, returns greater than zero
         if left string equals right string, returns zero
Function Input/Parameters: c-style left and right strings (char *)
Function Output/Parameters: none
Function Output/Returned: result as specified (int)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringLength
*/
int compareString(const char *leftStr, const char *rightStr)
{
  int diff, index = 0;
  while (leftStr[index] != NULL_CHAR && rightStr[index] != NULL_CHAR &&
         index < MAX_STR_LEN)
  {
    diff = leftStr[index] - rightStr[index];

    if (diff != 0)
    {
      return diff;
    }

    index++;
  }
  return (getStringLength(leftStr) - getStringLength(rightStr));
}

/*
Name: concatenateString
Process: appends one string onto another
Function Input/Parameters: c-style string (char *)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringLength
*/
void concatenateString(char *destStr, const char *sourceStr)
{
  int destIndex = getStringLength(destStr);
  int sourceStrLen = getStringLength(sourceStr);
  char *tempStr;
  int sourceIndex = 0;
  tempStr = (char *)malloc(sourceStrLen + 1);
  copyString(tempStr, sourceStr);
  while (tempStr[sourceIndex] != NULL_CHAR && destIndex < MAX_STR_LEN)
  {
    destStr[destIndex] = tempStr[sourceIndex];
    destIndex++;
    sourceIndex++;
    destStr[destIndex] = NULL_CHAR;
  }
  free(tempStr);
}

/*
Name: copyString
Process: copies one string into another overwriting
         data in the destination string
Function Input/Parameters: c-style string (char *)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringLength
*/
void copyString(char *destStr, const char *sourceStr)
{
  int index = 0;
  if (destStr != sourceStr)
  {
    while (sourceStr[index] != NULL_CHAR && index < MAX_STR_LEN)
    {
      destStr[index] = sourceStr[index];
      index++;
      destStr[index] = NULL_CHAR;
    }
  }
}

/*
Name: findSubString
Process: linear search for given substring within another string
Function Input/Parameters: c-style source test string (char *)
                           c-style source search string (char *)
Function Output/Parameters: none
Function Output/Returned: index of found substring, or
                          SUBSTRING_NOT_FOUND constant if string not found
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringLength
*/
int findSubString(const char *testStr, const char *searchSubStr)
{
  int testStrLen = getStringLength(testStr);
  int masterIndex = 0;
  int searchIndex, internalIndex;
  while (masterIndex < testStrLen)
  {
    internalIndex = masterIndex;
    searchIndex = 0;
    while (internalIndex <= testStrLen &&
           testStr[internalIndex] == searchSubStr[searchIndex])
    {
      internalIndex++;
      searchIndex++;
      if (searchSubStr[searchIndex] == NULL_CHAR)
      {
        return masterIndex;
      }
    }
    masterIndex++;
  }
  return SUBSTRING_NOT_FOUND;
}

/*
Name: getStringConstrained
Process: captures a string from the input stream
         with various constraints
Function Input/Parameters: input stream (FILE *)
                           clears leading non printable (bool),
                           clears leading space (bool),
                           stops at non printable (bool),
                           stops at specified delimiter (char)
                           Notes: consumes delimiter
Function Output/Parameters: string returned (char *)
Function Output/Returned: success of operation (bool)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: fgetc
*/
bool getStringConstrained(FILE *inStream, bool clearLeadingNonPrintable, bool clearLeadingSpace, bool stopAtNonPrintable, char delimiter, char *capturedString)
{
  int intChar = EOF, index = 0;
  capturedString[index] = NULL_CHAR;
  intChar = fgetc(inStream);
  while ((intChar != EOF) &&
         ((clearLeadingNonPrintable && intChar < (int)SPACE) ||
          (clearLeadingSpace && intChar == (int)SPACE)))
  {
    intChar = fgetc(inStream);
  }

  if (intChar == EOF)
  {
    return false;
  }
  while ((intChar != EOF && index < MAX_STR_LEN - 1) &&
         ((stopAtNonPrintable && intChar >= (int)SPACE) ||
          (!stopAtNonPrintable)) &&
         (intChar != (char)delimiter))
  {
    capturedString[index] = (char)intChar;
    index++;
    capturedString[index] = NULL_CHAR;
    intChar = fgetc(inStream);
  }
  return true;
}

/*
Name: getStringLength
Process: finds the length of a string by
         counting characters up to the NULL_CHAR character
Function Input/Parameters: c-style string (char *)
Function Output/Parameters: none
Function Output/Returned: length of string (int)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: none
*/
int getStringLength(const char *testStr)
{
  int index = 0;
  while (index < STD_STR_LEN && testStr[index] != NULL_CHAR)
  {
    index++;
  }
  return index;
}

/*
Name: getStringToDelimiter
Process: captures a string from the input stream
         to a specified delimiter;
         Note: consumes delimiter
Function Input/Parameters: input stream (FILE *)
                           stops at specified delimiter (char *)
Function Output/Parameters: string returned (char *)
Function Output/Returned: success of operation (bool)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringConstrained
*/
bool getStringToDelimiter(FILE *inStream, char delimiter, char *capturedString)
{
  return getStringConstrained(inStream, true, true, true, delimiter, capturedString);
}

/*
Name: getStringToLineEnd
Process: captures a string from the input stream
         to the end of the line
Function Input/Parameters: input stream (FILE *)
Function Output/Parameters: string returned (char *)
Function Output/Returned: success of operation (bool)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringConstrained
*/
bool getStringToLineEnd(FILE *inStream, char *capturedString)
{
  return getStringConstrained(inStream, true, true, true, NON_PRINTABLE_CHAR, capturedString);
}

/*
Name: getSubString
Process: compares sub string within larger string between two inclusive indices
         returns empty string if either index is out of range,
         assumes enough memory in destination string
Function Input/Parameters: c-style source string (char *)
                           start and end indices (int)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringLength, malloc, copyString, free
*/
void getSubString(char *destStr, const char *sourceStr, int startIndex, int endIndex)
{
  int sourceStrLen = getStringLength(sourceStr);
  int destIndex = 0;
  int sourceIndex = startIndex;
  char *tempStr;
  if (startIndex >= 0 && startIndex <= endIndex && endIndex < sourceStrLen)
  {
    tempStr = (char *)malloc(sourceStrLen + 1);
    copyString(tempStr, sourceStr);
    while (sourceIndex <= endIndex)
    {
      destStr[destIndex] = tempStr[sourceIndex];
      destIndex++;
      sourceIndex++;
      destStr[destIndex] = NULL_CHAR;
    }
    free(tempStr);
  }
}

/*
Name: setStrToLowerCase
Process: iterates through string, sets any upper case letter
         to lower case; no effect on other letters
Function Input/Parameters: c-style source string (char *)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: toLowerCase
*/
void setStrToLowerCase(char *destStr, const char *sourceStr)
{
  int sourceStrLen = getStringLength(sourceStr);
  char *tempStr = NULL;
  int index = 0;
  tempStr =
      (char *)malloc((char)sizeof(sourceStrLen) + (char)sizeof(NULL_CHAR) * 3);
  copyString(tempStr, sourceStr);
  while (tempStr[index] != NULL_CHAR && index < MAX_STR_LEN)
  {
    destStr[index] = toLowerCase(tempStr[index]);
    index++;
    destStr[index] = NULL_CHAR;
  }
  free(tempStr);
}

/*
Name: toLowerCase
Process: if character is upper case, sets it to lower case;
         otherwise returns character unchanged
Function Input/Parameters: test character (char)
Function Output/Parameters: none
Function Output/Returned: character to set to lower case, if appropriate
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: none
*/
int toLowerCase(char testChar)
{
  if (testChar >= 'A' && testChar <= 'Z')
  {
    return testChar - 'A' + 'a';
  }
  return testChar;
}

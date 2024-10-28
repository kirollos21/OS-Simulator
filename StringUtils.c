// header files 
#include "StringUtils.h"

// function implementations 

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
int compareString( const char *oneStr, const char *otherStr )
{
   // initialize variables
   int index = 0, diff;

   // loop to end of shortest string, with overrun protection
   while( index < MAX_STR_LEN && oneStr[index] != NULL_CHAR &&
                                                otherStr[index] != NULL_CHAR )
   {
      // get difference in characters
      diff = oneStr[index] - otherStr[index];

      // check for difference between characters
      if( diff != 0 )
      {
         // return difference
         return diff;
      }
         
      // increment index
      index++;
   }
   // end loop

   // return difference in lengths
      // function: getStringLength
   return getStringLength( oneStr ) - getStringLength( otherStr );
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
void concatenateString( char *destStr, const char *sourceStr )
{ 
   // initialize variables
      // set destination index
         // function: getStringLength
      int destIndex = getStringLength( destStr );

      // get source string length
         // function: getStringLength
      int sourceStrLen = getStringLength( sourceStr );

      // create temporay string pointer
      char *tempStr;

      // create other variables
      int sourceIndex = 0;

   // copy source string in case of aliasing
      // functions: malloc, copyString
   tempStr = (char *)malloc( sizeof( sourceStrLen + 1 ) );
   copyString( tempStr, sourceStr );

   // loop to end of source string
   while( tempStr[sourceIndex] != NULL_CHAR && destIndex < MAX_STR_LEN )
   {
      // assign characters to end of destination string
      destStr[ destIndex ] = tempStr[ sourceIndex ];

      // update indicies
      destIndex++;
      sourceIndex++;

      // set temporary end of destination string
      destStr[ destIndex ] = NULL_CHAR;
   }
   // end loop

   // free the temp string
      // function: free
   free( tempStr );

   // no return
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
void copyString( char *destStr, const char *sourceStr )
{
   // initialize variables
   int index = 0;

   // check for source and destination not the same (aliasing)
   if( destStr != sourceStr )
   {
      // loop to end of source string
      while( sourceStr[ index ] != NULL_CHAR && index < MAX_STR_LEN )
      {
         // assign characters to destination string
         destStr[index] =  sourceStr[ index ];

         // update index
         index++;

         // set temporary end of destination string
         destStr[ index ] = NULL_CHAR;
      }
      // end loop
   }
      
   // no return
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
int findSubString( const char *testStr, const char *searchSubStr )
{
   // initialize variables
      // initialize test string length
         // function: getStringLength
      int testStrLen = getStringLength( testStr );
      // initialize master index - location of sub string start point
      int masterIndex = 0;
      // initialize other variables
      int searchIndex, internalIndex;

   // loop across test string
   while( masterIndex < testStrLen )
   {
      // set internal loop index to currect test string index
      internalIndex = masterIndex;

      // set interal search index to zero
      searchIndex = 0;

      // loop to end of test string while test string/sub string chars are same
      while( internalIndex <= testStrLen && 
               testStr[ internalIndex ] == searchSubStr[ searchIndex ] )
      {
         // increment test string, substring indicies
         internalIndex++;
         searchIndex++;

         // check for end of sub string (search completed)
         if( searchSubStr[searchIndex] == NULL_CHAR )
         {
            // return beginning location of sub string
            return masterIndex;
         } 
      }    
      // end internal comparison loop

      // increment current beginning location index
      masterIndex++;
   }
   // end loop across test string

   // assume test have failed at this point, return SUBSTRING_NOT_FOUND (0)
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
bool getStringConstrained( FILE *inStream, bool clearLeadingNonPrintable,
                            bool clearLeadingSpace, bool stopAtNonPrintable,
                                       char delimiter, char *capturedString )
{
   // initialize variables
   int intChar = EOF, index = 0;

   // initialize output string
   capturedString[ index ] = NULL_CHAR;

   // capture first value in stream
      // function: fgetc
   intChar = fgetc( inStream );

   // loop to clear non-printable or space, if indicated
   while( ( intChar != EOF )
            && ( ( clearLeadingNonPrintable && intChar < (int)SPACE )
                          || ( clearLeadingSpace && intChar == (int)SPACE ) )

        )
   {
      // get next character
         // function: fgetc
      intChar = fgetc( inStream );
   }
   // end clear non-printable/space loop

   // check for end of file found
   if( intChar == EOF )
   {
      // return failed operation
      return false;
   }
      

   // loop to capture input
   // continues if not at end of file and max string length not reached
   // AND
   // continues if not printable flag set and characters are printable
   // OR continues if not printable flag not set
   // AND continues if specified delimiter is not found
   while( 
         ( intChar != EOF && index < MAX_STR_LEN - 1 )
         && ( ( stopAtNonPrintable && intChar >= (int)SPACE )
                || ( !stopAtNonPrintable ) )
         && ( intChar != (char)delimiter )
        
        )
   {
      // place character in array element
      capturedString[ index ] = (char)intChar;

      // increment array index
      index++;

      // set next element to null character / end of c-string
      capturedString[ index ] = NULL_CHAR;

      // get next character as integer
         // function: fgetc
      intChar = fgetc( inStream );
   }
   // end loop

   // return successful operation
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
int getStringLength( const char *testStr )
{
   // initialize variables
   int index = 0;

   // loop to end of string, protect from overflow
   while( index < STD_STR_LEN && testStr[ index ] != NULL_CHAR  )
   {
      // increment index
      index++;
   }

   // return index ie. string size
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
bool getStringToDelimeter( FILE *inStream, char delimiter, 
                                                  char *capturedString )
{
   // call engine function with delimiter
      // function: getStringConstrained
   return getStringConstrained( 
                  inStream,          // file stream pointer
                  true,              // clears leading non-printable character
                  true,              // bool clearLeadingSpace
                  true,              // stops at non-printable
                  delimiter,         // stops at delimiter
                  capturedString );  // return string
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
bool getStringToLineEnd( FILE *inStream, char *capturedString )
{
   // call engine function with specified constraints
      // function: getStringConstrained
   return getStringConstrained( 
               inStream,              // file stream pointer
               true,                  // clears leading non-printable character
               true,                  // bool clearLeadingSpace
               true,                  // stops at non-printable
               NON_PRINTABLE_CHAR,    // non-printable delimiter
               capturedString );      // returns string
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
void getSubString( char *destStr, const char * sourceStr,
                                         int startIndex, int endIndex )
{
   // initialize variables
      // set length of source string
         // function: getStringLength
      int sourceStrLen = getStringLength( sourceStr );
      // initialize the destination index to zero
      int destIndex = 0;
      // intialize source index to start index (parameter)
      int sourceIndex = startIndex;
      // create pointer for temp string
      char *tempStr;


   // check for indices within limits
   if( startIndex >= 0 && startIndex <= endIndex && endIndex < sourceStrLen )
   {
      // create temporary string ( + 1 for NULL char )
         // function: malloc, copyString
      tempStr = (char *)malloc( sourceStrLen + 1 );
      copyString( tempStr, sourceStr );

      // loop across requested substring (indices)
      while( sourceIndex <= endIndex )
      {
         // assign source character to destination element
         destStr[ destIndex ] = tempStr[ sourceIndex ];
         // increment indices
         destIndex++;
         sourceIndex++;
         // set temporary end of destination string
         destStr[ destIndex ] = NULL_CHAR;
      }
      // end loop

      // return memory for temp string
         // function: free
      free( tempStr );
   }
   
      
   // no return
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
void setStrToLowerCase( char *destStr, const char *sourceStr )
{
   // initialize variables
      // get source string length
         // function: getStringLength
      int sourceStrLen = getStringLength( sourceStr );

      // create temp string pointer
      char *tempStr;

      // create other variables
      int index = 0;

   // copy source string in case of aliasing
      // function: malloc, copyString
   tempStr = (char *)malloc( sourceStrLen + 1 );
   copyString( tempStr, sourceStr );

   // loop across source string
   while( tempStr[ index ] != NULL_CHAR && index < MAX_STR_LEN )
   {
      // set individual character to lower case as needed,
      // assign to destination string
      destStr[ index ] = toLowerCase( tempStr[index] );

      // update index
      index++;

      // set temp end of destination string
      destStr[ index ] = NULL_CHAR;
   }
   // end loop

   // release memory used for temp string
      // function: free
   free( tempStr );

   // no return
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
char toLowerCase( char testChar )
{
   // check for upper case letter
   if( testChar >= 'A' && testChar <= 'Z' )
   {
      // return lower case letter
      return testChar - 'A' + 'a';
   }

   // otherwise, assume no upper case letter,
   // return character unchanged
   return testChar;
}

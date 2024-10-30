//header files
#include "OS_SimDriver.h"

// global constants
  //none

// main program

/*
Name: main
Process: default
Function Input/Parameters: number of arguments (int)
                           vector of arguments (char **)
Function Output/Parameters: none
Function Output/Returned: function/program success (0)
Device Input/Keyboard: none
Device Output/Monitor: result displayed
Dependencies: stdio tools
*/

int main( int argc, char **argv )
   {
    // initialize program
      
      // initalize variables
      ConfigDataType *configDataPtr = NULL;
      OpCodeType *metaDataPtr = NULL;
      char errorMessage[ MAX_STR_LEN ];
      CmdLineData cmdLineData;
      bool configUploadSuccess = false;
      
      //show title
        // function: printf
      printf("\nSimulator Program\n");
      printf("=================\n\n");

    // process comand line, check for program run
    // at least one correct switch and config file name (at end) verified
      // function: processCmdLine
    if( processCmdLine( argc, argv, &cmdLineData ) )
       {
        // upload config file, check for success
          // function: getConfigData
        if( getConfigData( cmdLineData.fileName, &configDataPtr,
                                                                errorMessage) )
           {
            // check config display flag
            if( cmdLineData.configDisplayFlag )
               {
                // display config data
                  // function: displayConfigData
                displayConfigData( configDataPtr );
               }
               
            // setconfig upload success flag
            configUploadSuccess = true;
           }
        // otherwise, assume config file upload failure
        else
           {
            // show error message, end program
              // function: printf
            printf("\nConfig Upload Error: %s, program aborted\n\n",
                                                                errorMessage );
           }

        // check for config success and need for metadata
        if( configUploadSuccess && ( cmdLineData.mdDisplayFlag || 
                                                      cmdLineData.runSimFlag) )
           {
            // upload meta data display flag
              // function: getMetaData
            if( getMetaData( configDataPtr->metaDataFileName, &metaDataPtr,
                                                               errorMessage ) )
               {
                if( cmdLineData.mdDisplayFlag )
                   {
                    // display meta data
                      // function: displayMetaData
                    displayMetaData(metaDataPtr);
                   }
                 
                // check run simulator flag
                if( cmdLineData.runSimFlag)
                   {
                    // run simulator
                      // function: runSim
                    runSim( configDataPtr, metaDataPtr );
                   }
             }

          // otherwise assume meta data upload failure
          else
             {
              // show error mesage, end program
                // function: printf
              printf("\nMetadata Upload Error: %s, program aborted\n", 
                                                                errorMessage );
             }
         }
      
        // clean up config data as needed
          // function: clearConfigData
        configDataPtr = clearConfigData( configDataPtr );
      
        // clean up metadata as needed
          // function: clearMetaDataList
        metaDataPtr = clearMetaDataList( metaDataPtr );
       }
    // end check for good command line
    
    // otherwise, assume command line failure
    else
       {
        // show command argument requirements
          // function: showCommandLineFormat
        showCommandLineFormat();
       }
    // display program end
      // function: printf
    printf("\nSimulator Program End.\n");
        
    // return program success 
    return 0;
   }
   
/*
Name: clearCmdLineStruct
Process: sets command line structure data to defaults
         Booleans to false, fileName to empty string
Function Input/Parameters: pointer to command Line Struct ( CmdLineData * )
Function Output/Parameters: pointer to command Line Struct ( CmdLineData * )
                            with updated members
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: none
*/
void clearCmdLineStruct( CmdLineData *clDataPtr)
   {
    // reset struct members to default
    clDataPtr->programRunFlag = false;
    clDataPtr->configDisplayFlag = false;
    clDataPtr->mdDisplayFlag = false;
    clDataPtr->runSimFlag = false;
    clDataPtr->fileName[0] = NULL_CHAR;
   }

/*
Name: processCmdLine
Process: checks for at least two arguments,
         then sets Booleans depending on command line switches
         which can be in any order,
         also captures config file name which must be the last argument
Function Input/Parameters: number of arguments (int)
                           vector of arguments (char **)
Function Output/Parameters: pointer to command line structure (CmdLineData*)
                            with updated members,
                            set to default values if failure
                            to capture arguments
Function Output/Returned: Boolean result of argument capture,
                          true if at least one switch and config file name,
                          false otherwise
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: clearCmdLineStruct, compareString, getStringLength,
              findSubString, copyString
*/
bool processCmdLine( int numArgs, char **strVector, CmdLineData *clDataPtr)
   {
   // initalize function/variables
   
     // initalize structure to default
       // function: clearCmdLineStruct
     clearCmdLineStruct( clDataPtr );
     
     // initalize success flag to false
     bool atLeastOneSwitchFlag = false;
     bool correctConfigFileFlag = false;
     
     // initalize first arg index to one
     int argIndex = 1;
     
     // declare other variables
     int fileStrLen, fileStrSubLoc;
     
   // must have prog name, at least one switch, and config file name, minimum
   if( numArgs >= MIN_NUM_ARGS )
      {
       // loop across args (starting at 1) and program run flag
       while( argIndex < numArgs )
          {
           // check for -dc (display config flag)
           if( compareString( strVector[argIndex], "-dc") == STR_EQ)
              {
               // set config display flag
               clDataPtr->configDisplayFlag = true;
               
               // set at least one switch flag
               atLeastOneSwitchFlag = true;
              }
              
           // otherwise, check for -dm (display metadata flag)
           else if( compareString( strVector[argIndex], "-dm") == STR_EQ)
              {  
               // set metadata display flag
               clDataPtr->mdDisplayFlag = true;
               
               // set at least one switch flag
               atLeastOneSwitchFlag = true;
              }
              
           // otherwise, check for -rs (run simulator)
           else if( compareString( strVector[argIndex], "-rs") == STR_EQ)
              {  
               // set run sim flag
               clDataPtr->runSimFlag = true;
               
               // set at least one switch flag
               atLeastOneSwitchFlag = true;
              }
              
           // otherwise, check for filename, ending in .cfg
           // must be last four
           else
              {  
               // find lengths to verify file name
               // file name must be last arg, and have ".cnf" extension
                 // function: getStringLength, findSubString
               fileStrLen = getStringLength( strVector[numArgs - 1] );
               fileStrSubLoc = findSubString( strVector[numArgs - 1], ".cnf" );
               
               // check that file exists, and correct file format
               if( fileStrSubLoc != SUBSTRING_NOT_FOUND && 
                              fileStrSubLoc == fileStrLen - LAST_FOUR_LETTERS )
                  {
                   // set file name to variable
                   copyString( clDataPtr->fileName, strVector[numArgs - 1] );
                   
                   // set success flag to true
                   correctConfigFileFlag = true;
                  }
                  
              // otherwise, assume bad config file name
              else
                 {
                  // reset struct, correct config file flag stays false
                    // function: clearCmdLineStruct
                  clearCmdLineStruct( clDataPtr );
                 } 
              }
                    
           // update arg index
           argIndex++;
          }
        // end arg loop
      }   
    // return verification of at least one switch and correct file name
    return atLeastOneSwitchFlag && correctConfigFileFlag;
   }

/*
Name: showCommandLineFormat
Process: displays command line format as assistance to user
Function Input/Parameters: none
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: data displayed as specified
Dependencies: printf
*/
void showCommandLineFormat()
   {
    // print title 
      // function: printf
    printf("Command Line Format:\n");
    
    // display formate
      // function: printf
    printf("\tsim_0x [-dc] [-dm] [-rs] <config file name>\n");
    
    // show what -dc does
      // function: printf
    printf("\t-dc [optional] display configuration data\n");

    // show what -dm does
      // function: printf
    printf("\t-dm [optional] display meta data\n");
    
    // shows what -rs does
      // function: printf
    printf("\t-rs [optional] runs simulator\n");
    
    // display requirement
      // function: printf
    printf("\trequired config file name\n");
   }

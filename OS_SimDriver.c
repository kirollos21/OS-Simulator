//  File: OS_SimDriver.c
//  Project: Sim03
//  Secret ID: 708996

#include "OS_SimDriver.h"

/*
Name: showCommandLineFormat
Process: displays command Line format as assistance to user
Function Input/Parameters: none
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/monitor: data displayed as specified
Dependencies: printf
*/
void showCommandLineFormat()
{
  printf("Command Line Format:\n");
  printf("     sim_0x [-dc] [-dm] [-rs] <config file name>\n");
  printf("     -dc [optional] displays configuration data\n");
  printf("     -dm [optional] displays meta data\n");
  printf("     -rs [optional] runs simulator\n");
  printf("     required config file name\n");
}

/*
Name: clearCmdLineStruct
Process: sets command line structure data to defaults
         Booleans to false, fileName to empty string
Function Input/Parameters: pointer to command line structure (CmdLineData *)
Function Output/Parameters: pointer to command line structure (CmdLineData *)
                            with updated members
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: none
*/
void clearCmdLineStruct(CmdLineData *clDataPtr)
{
  clDataPtr->programRunFlag = false;
  clDataPtr->configDisplayFlag = false;
  clDataPtr->mdDisplayFlag = false;
  clDataPtr->runSimFlag = false;
  clDataPtr->fileName[0] = NULL_CHAR;
}

/*
 Name: processCmdLine
 Process:   checks for at least two arguments,
            then sets Booleans depending on command Line switches
            which can be in any order,
            also captures config file name which must be the last argument
 Function Input/Parameters: number of arguments (int),
                            vector of arguments (char **o<
 Function Output/Parameters:    pointer to command Line structure (CmdLineData *)
                                with updated members,
                                set to default values if failure
                                to capture arguments
 Function Output/Returned:  Boolean result of argument capture,
                            true if at least one switch and config file name,
                            false otherwise
 Device Input/device: none
 Device Output/device: none
 Dependencies: tbd
 */
bool processCmdLine(int numArgs, char **strVector, CmdLineData *clDataPtr)
{
  clearCmdLineStruct(clDataPtr);
  bool atLeastOneSwitchFlag = false;
  bool correctConfigFileFlag = false;
  int argIndex = 1;
  int fileStrLen;
  int fileStrSubLoc;

  if (numArgs >= MIN_NUM_ARGS)
  {
    while (argIndex < numArgs)
    {
      // DISPLAY CONFIG
      if (compareString(strVector[argIndex], "-dc") == STR_EQ)
      {
        clDataPtr->configDisplayFlag = true;
        atLeastOneSwitchFlag = true;
      }
      // DISPLAY METADATA
      else if (compareString(strVector[argIndex], "-dm") == STR_EQ)
      {
        clDataPtr->mdDisplayFlag = true;
        atLeastOneSwitchFlag = true;
      }
      // RUN SIMULATOR
      else if (compareString(strVector[argIndex], "-rs") == STR_EQ)
      {
        clDataPtr->runSimFlag = true;
        atLeastOneSwitchFlag = true;
      }
      // READING .cnf FILE
      else
      {
        fileStrLen = getStringLength(strVector[numArgs - 1]);
        fileStrSubLoc = findSubString(strVector[numArgs - 1], ".cnf");

        if (fileStrSubLoc != SUBSTRING_NOT_FOUND &&
            fileStrSubLoc == fileStrLen - LAST_FOUR_LETTERS)
        {
          copyString(clDataPtr->fileName, strVector[numArgs - 1]);
          correctConfigFileFlag = true;
        }
        else
        {
          clearCmdLineStruct(clDataPtr);
        }
      }
      argIndex++;
    }
  }
  return atLeastOneSwitchFlag && correctConfigFileFlag;
}

int main(int argc, char **argv)
{
  /* SIM0x MAIN ENTRY POINT */
  ConfigDataType *configDataPtr = NULL;
  OpCodeType *metaDataPtr = NULL;
  char errorMessage[MAX_STR_LEN];
  CmdLineData cmdLineData;
  bool configUploadSuccess = false;

  printf("Simulator Program\n");
  printf("=================\n\n");
  if (processCmdLine(argc, argv, &cmdLineData))
  {
    if (getConfigData(cmdLineData.fileName, &configDataPtr, errorMessage))
    {
      if (cmdLineData.configDisplayFlag)
      {
        displayConfigData(configDataPtr);
      }

      configUploadSuccess = true;
    }
    else
    {
      printf("\nConfig Upload Error: %s, program aborted.\n\n", errorMessage);
    }

    if (configUploadSuccess &&
        (cmdLineData.mdDisplayFlag || cmdLineData.runSimFlag))
    {
      if (getMetaData(configDataPtr->metaDataFileName, &metaDataPtr,
                      errorMessage))
      {
        if (cmdLineData.mdDisplayFlag)
        {
          displayMetaData(metaDataPtr);
        }

        if (cmdLineData.runSimFlag)
        {
          runSim(configDataPtr, metaDataPtr);
        }
      }
      else
      {
        printf("\nMetadata Upload Error: %s, program aborted.\n", errorMessage);
      }
    }

    configDataPtr = clearConfigData(configDataPtr);

    metaDataPtr = clearMetaDataList(metaDataPtr);
  }
  else
  {
    showCommandLineFormat();
  }

  printf("\n\nSimulator Program End.\n\n");
  return 0;
}

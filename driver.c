/**
 * Command Line Parser driver for sim01, sim.c contains the driver for
 * running the sim.
 */
#include "driver.h"

/**
 * Function to return the usage of sim01
 */
void usage()
{
  printf("Command Line Format:\n");
  printf("     sim_0x [-dc] [-dm] [-rs] <config file name>\n");
  printf("     -dc [optional] displays configuration data\n");
  printf("     -dm [optional] displays meta data\n");
  printf("     -rs [optional] runs simulator\n");
  printf("     required config file name\n");
}

/**
 * clears the cmd line struct
 */
void clearCmdLineStruct(CmdLineData *clDataPtr)
{
  clDataPtr->programRunFlag = false;
  clDataPtr->configDisplayFlag = false;
  clDataPtr->mdDisplayFlag = false;
  clDataPtr->runSimFlag = false;
  clDataPtr->fileName[0] = NULL_CHAR;
}

/**
 * process cmd line args
 */
_Bool processCmdLine(int numArgs, char **strVector, CmdLineData *clDataPtr)
{
  clearCmdLineStruct(clDataPtr);
  _Bool atLeastOneSwitchFlag = false;
  _Bool correctConfigFileFlag = false;
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
  _Bool configUploadSuccess = false;

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
    usage();
  }

  printf("\n\nSimulator Program End.\n\n");
  return 0;
}

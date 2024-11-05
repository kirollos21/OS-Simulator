//Header files
#include "datatypes.h"
#include "simulator.h"
#include "metadataops.h"
#include "configops.h"
#include "StringUtils.h"
#include <stdio.h>

bool processCmdLine (int numArgs, char** strVector, CmdLineData* cmdDataPtr);

void showComandLineFormat();

void clearCmdLineStruct(CmdLineData* clDataPtr);

int main(int argc, char **argv)
{
    //initialize program variables
    ConfigDataType* configDataPtr = NULL;
    OpCodeType* metaDataPtr = NULL;
    char errorMessage[MAX_STR_LEN];
    CmdLineData cmdLineData;
    bool configUploadSuccess = false;


    //show title
    printf("\nSimulator Program");
    printf("\n=================\n\n");


    //process command line, check for program run

    //at least one correct switch and config file name
    if (processCmdLine(argc, argv, &cmdLineData))
    {

        //upload config file, check for success
        if (getConfigData(cmdLineData.fileName, &configDataPtr, errorMessage))
        {

            //check config display flag
            if (cmdLineData.configDisplayFlag)
            {
                //display config file
                    //displayConfigData()
                displayConfigData(configDataPtr);
            }
            //set config upload success flag
            configUploadSuccess = true;
        }
        //otherwise assume config file upload failure
        else
        {
            printf("\nConfig Upload error: %s, program aborted\n\n", 
                errorMessage);
        }

        //check for config success and need for metadata
        //both config and metadata file success
        if (configUploadSuccess == true &&
        (cmdLineData.mdDisplayFlag || cmdLineData.runSimFlag))
        {
            //upload metadata file, check for success
            if (getMetaData(configDataPtr->metaDataFilename,
                    &metaDataPtr, errorMessage))
            {
                //check metadata display flag
                if (cmdLineData.mdDisplayFlag)
                {
                    //display metadata file
                        //displayMetaData()
                    displayMetaData(metaDataPtr);
                }

                //check run simulator flag
                if (cmdLineData.runSimFlag)
                {
                    runSim(configDataPtr, metaDataPtr);
                }
            }

            //otherwise assume metadata file upload failure
            else
            {
                printf("\nConfig Upload error: %s, program aborted\n\n", 
                    errorMessage);
            }
        }

        configDataPtr = clearConfigData(configDataPtr);

        metaDataPtr = clearMetaDataList(metaDataPtr);
        

    }
    //end check for command line arguments
    else
    {
        showComandLineFormat();
    }

    printf("\nSimulator Program End.\n\n");

    //return success
    return 0; 
}

//check if command line has all required arguments 
//and sets the CmdLineData to reflect that
bool processCmdLine (int numArgs, char** strVector, CmdLineData* cmdDataPtr)
{
    //clear struct for use
    clearCmdLineStruct(cmdDataPtr);

    //intialize variables
    bool atleastOneSwitchFlag = false;
    bool correctConfigFileFlag = false;

    int argIndex = 1;

    int fileStrLen, fileStrSubLock;

    //check if number of arguments needed is matched
    if (numArgs >= MIN_NUM_ARGS)
    {
        //loops until all arguments checked
        while (argIndex < numArgs)
        {   
            //test if atleast one flag is found
            if (compareStrings(strVector[argIndex],
            "-dc") == STR_EQ)
            {
                cmdDataPtr->configDisplayFlag = true;
                atleastOneSwitchFlag = true;
            }
            else if (compareStrings(strVector[argIndex],
            "-dm") == STR_EQ)
            {
                cmdDataPtr->mdDisplayFlag = true;
                atleastOneSwitchFlag = true;
            }
            else if (compareStrings(strVector[argIndex],
            "-rs") == STR_EQ)
            {
                cmdDataPtr->runSimFlag = true;
                atleastOneSwitchFlag = true;
            }
            else
            {
                fileStrLen = getStringLength(strVector[numArgs-1]);
                fileStrSubLock = findSubString(strVector[numArgs-1], ".cnf");
                if (fileStrSubLock != 0 &&
                    fileStrSubLock == fileStrLen - LAST_FOUR_LETTERS)
                {
                    copyString(cmdDataPtr->fileName, strVector[numArgs-1]);

                    correctConfigFileFlag = true;
                }
                else
                {
                    clearCmdLineStruct(cmdDataPtr);
                }
            }

            //updare arg index
            argIndex++;
        }
        //end arg loop
    }
    //end test for min number of command line arguments
    return atleastOneSwitchFlag && correctConfigFileFlag;
}

//Displays the format that the user should input
void showComandLineFormat()
{
    //display command line format
        //function: printf
    printf("Command Line Format:\n");
    printf("sim [-dc] [-dm] [-rs] config file name\n");
    printf("    -dc [optional] display configuration data\n");
    printf("    -dm [optional] display meta data\n");
    printf("    -rs [optional] run simulator\n");
    printf("    required config file name");

}

//clears the CmdLineStruct for future use
void clearCmdLineStruct(CmdLineData* clDataPtr)
{
    //set all struct members to default
    clDataPtr->programRunFlag = false;
    clDataPtr->configDisplayFlag = false;
    clDataPtr->mdDisplayFlag = false;
    clDataPtr->runSimFlag = false;
    clDataPtr->fileName[0] = NULL_CHAR;
}
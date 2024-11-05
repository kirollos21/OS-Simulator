#include "metadataops.h"

/*
Name: addNode
Process: adds a Node to the meta data linked list
Function input/parameters: pointer to OpCodeType and new node
Function output/parameters: none
Function output/returned: pointer to new node added
Device input/---: none
Device output/---: none
Dependencies: malloc, copyString
*/
OpCodeType *addNode(OpCodeType *localPtr, OpCodeType *newNode)
{
    //check for local pointer assigned to null
    if (localPtr == NULL)
    {
        //access memory for new link/node
        localPtr = (OpCodeType*)malloc(sizeof(OpCodeType));

        //assign all three values to newly created node
        //assign next pointer to null
            //function copyString
        localPtr->pid = newNode->pid;
        copyString(localPtr->command, newNode->command);
        copyString(localPtr->inOutArg, newNode->inOutArg);
        copyString(localPtr->strArg1, newNode->strArg1);
        localPtr->intArg2 = newNode->intArg2; 
        localPtr->intArg3 = newNode->intArg3;
        localPtr->opEndTime = newNode->opEndTime;

        localPtr->nextNode = NULL; 

        //return current local pointer
        return localPtr;
    }

    //assume end of list not found yet
    //assign recursive function to current's next link
        //function: addNode
    localPtr->nextNode = addNode(localPtr->nextNode, newNode);

    //return current pointer
    return localPtr;
}


void displayMetaData(const OpCodeType *localPtr)
{
    //display title
    printf("Meta-Data File Display\n");
    printf("----------------------\n\n");

    //loop to end of linked list
        //functions in loop: printf, compareStrings
    while (localPtr != NULL)
    {
        //print leader
        printf("Op code: ");

        //print op code pid
        printf("/pid: %d", localPtr->pid);

        //print op code cmd
        printf("/cmd: %s", localPtr->command);

        //check for dev op
        if (compareStrings(localPtr->command, "dev") == STR_EQ)
        {
            printf("/io: %s", localPtr->inOutArg);
        }
        else 
        {
            printf("/io: NA");
        }
        
        //print first string argument
        printf("\n\t/arg1: %s", localPtr->strArg1);

        //print first and second int argument
            //function: printf
        printf("/arg 2: %d", localPtr->intArg2);
        printf("/arg 3: %d", localPtr->intArg3);

        //print op end time
        printf("/op end time: %8.6f", localPtr->opEndTime);

        //print end line
        printf("\n\n");

        //assign local pointer to next node
        localPtr = localPtr->nextNode;
    }


}

/*
Name: clearMetaDataList
Process: clears metadata linkedlist
Function input/parameters: metadata linked list
Function output/parameters: none
Function output/returned: none
Device input/---: none
Device output/---: none
Dependencies: clearMetaDataList, free
*/
OpCodeType* clearMetaDataList(OpCodeType *localPtr)
{
    if (localPtr != NULL)
    {
        //call recursive function with next pointer
            //function: clearMetaDataList
        clearMetaDataList(localPtr->nextNode);

        //after recursive calls finish release memory
            //frunction:free
        free(localPtr);

        //set given pointer to null
        localPtr = NULL;
    }
    //returns null to calling function
    return NULL;
}

int getCommand(char* cmd, const char* inputStr, int index)
{
    //intialize variable
    int lengthOfCommand = 3;

    //loop across command length
    while (index < lengthOfCommand)
    {
        //assign character from input string to buffer string
        cmd[index] = inputStr[index];

        //increment index
        index++;

        //set next Charcter to null char
        cmd[index] = NULL_CHAR;
    }

    //return current index
    return index;
}

/*
Name: getMetaData
Process: reads file of meta data and 
Function input/parameters: Filename, metadate linked list
Function output/parameters: endstatestring
Function output/returned: bool for success or failure
Device input/---: none
Device output/---: none
Dependencies: fopen, copyString, compareStrings, getStringToDelimiter
              fclose, malloc, getOpCommand, updateStartCount, 
              updateEndCount, clearMetaDataList, free
*/
bool getMetaData(const char* filename, OpCodeType **opCodeDataHead,
                 char* endStateMsg)
{
    //intialize read only constant
    const char READ_ONLY_FLAG[] = "r";

    //initialize variables
    int accessResult, startCount = 0, endCount = 0;
    char dataBuffer[MAX_STR_LEN];
    bool returnState = true;
    OpCodeType *newNodePtr;
    OpCodeType *localHeadPtr = NULL;
    FILE* fileAccessPtr;

    //initialize op code data pointer in case of return error
    *opCodeDataHead = NULL;

    //initialize end state message
        //function: copyString
    copyString(endStateMsg, "Metadata file upload successful");

    //open file for reading
        //function: fopem
    fileAccessPtr = fopen(filename, READ_ONLY_FLAG);

    //check for file open failure
    if (fileAccessPtr == NULL)
    {
        //set end message to err
        copyString(endStateMsg, "Metadata file access error");

        //return false
        return false;
    }

    //check first line for correct leader
        //function: getStringToDelimiter, compareString
    if (!getStringToDelimiter(fileAccessPtr, COLON, dataBuffer)
        || compareStrings(dataBuffer, "Start Program Meta-Data Code") != STR_EQ)
    {
        //close file
            //function: fclose
        fclose(fileAccessPtr);

        //set end state message
            //function: copyString
        copyString(endStateMsg, "Corrupt metadata leader line error");

        //return corrupt desc error
        return false;
    }

    //allocate memory for temporary data structure
        //function: malloc
    newNodePtr = (OpCodeType*)malloc(sizeof(OpCodeType));

    //get the first op command
        //function: getOpCommand
    accessResult = getOpCommand(fileAccessPtr, newNodePtr);

    //get start and end counts for later comparison
        //function: updateStartCount, updateEndCount
    startCount = updateStartCount(startCount, newNodePtr->strArg1);
    endCount = updateEndCount(endCount, newNodePtr->strArg1);

    //check for failure of first complete op command
    if (accessResult != COMPLETE_OPCMD_FOUND_MSG)
    {
        //close file
            //function: fclose
        fclose(fileAccessPtr );

        //clear data from structure list
            //function: clearMetaDataList
        *opCodeDataHead = clearMetaDataList(localHeadPtr);

        //free temp structure memory
            //function: free
        free(newNodePtr);

        //set end state message
            //functions: copyString
        copyString(endStateMsg, "Metadata incomplete first op command not found");

        //return result of operation
        return false;
    }

    //loop across all remaining op commands
    while (accessResult == COMPLETE_OPCMD_FOUND_MSG)
    {
        //add the new op command to the linked list
            //functions: addNode
        localHeadPtr = addNode(localHeadPtr, newNodePtr);

        //get a new op command
            //function: getopCommand
        accessResult = getOpCommand(fileAccessPtr, newNodePtr);

        //update start and end counts for later comparison
            //function: updateStartCount updateEndCount
        startCount = updateStartCount(startCount, newNodePtr->strArg1);
        endCount = updateEndCount(endCount, newNodePtr->strArg1);
    }

    //check for end
    if (accessResult == LAST_OPCMD_FOUND_MSG)
    {
        //check for start and end op code counts equal
        if (startCount == endCount)
        {
            //add the last node to the linked list
                //function: addNode
            localHeadPtr = addNode(localHeadPtr, newNodePtr);

            //set access result to no error for later operation
            accessResult = NO_ACCESS_ERR;

            //check last line for incorrect end descriptor
                //function: getStringToDelimiter, compareString
            if (!getStringToDelimiter(fileAccessPtr, PERIOD, dataBuffer)
                || compareStrings(dataBuffer, "End Program Meta-Data Code") 
                != STR_EQ)
            {
                //set access result to corrupted descriptor error
                accessResult = MD_CORRUPT_DESCRIPTOR_ERR;
                
                //set end state message
                    //function
                copyString(endStateMsg, "Metadata corrupted descriptor error");
            }
        }
        else
        {
            //close file
                //function: fclose
            fclose(fileAccessPtr);

            //clear data from the structure list
                //function: clearMetaDataList
            *opCodeDataHead = clearMetaDataList(localHeadPtr);

            //free temp struct memory
                //function: free
            free(newNodePtr);

            //set access result to error
            accessResult = UNBALANCED_START_END_ERR;

            //set end state message
                //function: copyString
            copyString(endStateMsg, 
            "Unbalanced start and end arguments in metadata");

            //return result
            return false;
        }
    }

    //otherwise assume didn't find end
    else
    {
        //set end state message
            //function: copyString
        copyString(endStateMsg, "Corrupted metadata op code");

        //unset return state
        returnState = false;
    }

    //check for any errors found (not no error)
    if (accessResult != NO_ACCESS_ERR)
    {
        //clear the op command list
            //function: cleanMetaDataList
        localHeadPtr = clearMetaDataList(localHeadPtr);
    }

    //close access file
        //function: fclose
    fclose(fileAccessPtr);

    //release temporary structure memory
        //functions: free
    free(newNodePtr);

    //assign temporary local head pointer to parameter return pointer
    *opCodeDataHead = localHeadPtr;

    //return access result
    return returnState;
} 

/*
Name: getOpCommand
Process: gdetermine what commands were gotten and verify it as valid
Function input/parameters: file and opCodeType
Function output/parameters: command str
Function output/returned: int of cmd index
Device input/---: none
Device output/---: none
Dependencies: getStringToDelimiter, getCommand, copyString
              verifyValidCommand, compareStrings, getStringArg,
              getNumArg
*/
OpCodeMessages getOpCommand(FILE* filePtr, OpCodeType *inData)
{
    //initialize local constants
    const int MAX_CMD_LENGTH = 5;
    const int MAX_ARG_STR_LENGTH = 15;

    //initialize other variables
    int numBuffer = 0;
    char strBuffer[STD_STR_LEN];
    char cmdBuffer[MAX_CMD_LENGTH];
    char argStrBuffer[MAX_ARG_STR_LENGTH];
    int runningStringIndex = 0;
    bool arg2FailureFlag = false;
    bool arg3FailureFlag = false;

    //get whole op command as string, check for successful access
        //function: getStringToDelimiter
    if (getStringToDelimiter(filePtr, SEMICOLON, strBuffer))
    {
        //get three-letter command
            //function: getCommand
        runningStringIndex = getCommand(cmdBuffer,
            strBuffer, runningStringIndex);

        //assign op command to node
            //function: copystring
        copyString(inData->command, cmdBuffer);
    }
    //otherwise, assume unsuccessful access
    else
    {
        //set pointer to data structure to null
        inData = NULL;

        //return op command access failure
        return OPCMD_ACCESS_ERR;
    }

    //verify op command
    if (!verifyValidCommand(cmdBuffer))
    {
        //return op command error
        return CORRUPT_OPCMD_ERR;
    }

    //set all struct values that may not be initialized to defaults
    inData->pid = 0;
    inData->inOutArg[0] = NULL_CHAR;
    inData->intArg2 = 0;
    inData->intArg3 = 0;
    inData->opEndTime = 0.0;
    inData->nextNode = NULL;
    
    //check for device command
    if (compareStrings(cmdBuffer, "dev") == STR_EQ)
    {
        //get in/out argument
        runningStringIndex = getStringArg(argStrBuffer,
        strBuffer, runningStringIndex);

        //set device in/out argument
        copyString(inData->inOutArg, argStrBuffer);

        //check correct argument
        if (compareStrings(argStrBuffer, "in") != STR_EQ
            && compareStrings(argStrBuffer, "out") != STR_EQ)
        {
            return CORRUPT_OPCMD_ARG_ERR;
        }
    }

    //get first string arg
    runningStringIndex = getStringArg(argStrBuffer, 
                         strBuffer, runningStringIndex);

    //set device in/out argument
    copyString(inData->strArg1, argStrBuffer);

    //check for legitimate first string org
    if (!verifyFirstStringArg(argStrBuffer))
    {
        //return argument error
        return CORRUPT_OPCMD_ARG_ERR;
    }

    //checks for last op command found
    if (compareStrings(inData->command, "sys") == STR_EQ
        && compareStrings(inData->strArg1, "end") == STR_EQ)
    {
        return LAST_OPCMD_FOUND_MSG;
    }

    //check for app start seconds argument
    if (compareStrings(inData->command, "app") == STR_EQ
        && compareStrings(inData->strArg1, "start") == STR_EQ)
    {
        //get number argument
            //function: getNumberArg
        runningStringIndex = getNumberArg(&numBuffer,
                             strBuffer, runningStringIndex);

        if (numBuffer <= BAD_ARG_VAL)
        {
            //set failure flag
            arg2FailureFlag = true;
        }

        //set first int argument to number
        inData->intArg2 = numBuffer;
    }
    //check for cpu cycle time
    else if (compareStrings(inData->command, "cpu") == STR_EQ)
    {
        //get number argument
            //function: getNumberArg
        runningStringIndex = getNumberArg(&numBuffer,
                              strBuffer, runningStringIndex);

        if (numBuffer <= BAD_ARG_VAL)
        {
            //set failure flag
            arg2FailureFlag = true;
        }

        //set first into argument to number
        inData->intArg2 = numBuffer;
    }
    //check for device cycle time
    else if (compareStrings(inData->command, "dev") == STR_EQ)
    {
        //get number argument
            //function: getNumberArg
        runningStringIndex = getNumberArg(&numBuffer,
                            strBuffer, runningStringIndex);

        //check for failed number access
        if (numBuffer <= BAD_ARG_VAL)
        {
            arg2FailureFlag = true;
        }

        //set first int argument to number
        inData->intArg2 = numBuffer;
    }
    
    //check for memory base and offset
    else if (compareStrings(inData->command, "mem") == STR_EQ)
    {
        //get number argument for base
            //function: getNumberArg
        runningStringIndex = getNumberArg(&numBuffer, 
                              strBuffer, runningStringIndex);

        //check for failed number access
        if (numBuffer <= BAD_ARG_VAL)
        {
            //set failure flag
            arg2FailureFlag = true;
        }

        //set first int argument to number
        inData->intArg2 = numBuffer;

        //get number argument for offset
            //function: getNumberArg
        runningStringIndex = getNumberArg(&numBuffer,
                             strBuffer, runningStringIndex);

        //check for failed number access
        if (numBuffer <= BAD_ARG_VAL)
        {
            //set failure flag
            arg3FailureFlag = true;
        }

        //set second int argument
        inData->intArg3 = numBuffer;
    }

    //check int args for upload failure
    if (arg2FailureFlag || arg3FailureFlag)
    {
        //return corrupt op command error error
        return CORRUPT_OPCMD_ARG_ERR;
    }

    return COMPLETE_OPCMD_FOUND_MSG;
}

/*
Name: getNumberArg
Process: find number of arguments passed in
Function input/parameters: int number, input string, index
Function output/parameters: none
Function output/returned: int of arguments
Device input/---: none
Device output/---: none
Dependencies: isDigit
*/
int getNumberArg(int* number, const char* inputStr, int index)
{
    //initialize variables
    bool foundDigit = false;
    *number = 0;
    int multiplier = 1;

    //loop to skip white space
    while(inputStr[index] <= SPACE || inputStr[index] == COMMA)
    {
        index++;
    }

    //loop across string length
    while (isDigit(inputStr[index]) == true 
           && inputStr[index] != NULL_CHAR)
    {
        //set digit found flag
        foundDigit = true;

        //assign digit to output
        (*number) = (*number) *multiplier + inputStr[index] - '0';

        //increment index and multiplier
        index++;
        multiplier = 10;
    }

    if (!foundDigit)
    {
        *number = BAD_ARG_VAL;
    }

    //return current index
    return index;
}

/*
Name: getStringArg
Process: get argument of string
Function input/parameters: char* strArg, inputStr, int index
Function output/parameters: none
Function output/returned: int of arguments
Device input/---: none
Device output/---: none
Dependencies: isDigit
*/
int getStringArg(char *strArg, const char *inputStr, int index)
{
    //initialize variables
    int localIndex = 0;

    //loop to skip white space
    while (inputStr[index] <= SPACE || inputStr[index] == COMMA)
    {
        index++;
    }

    //loop across string length
    while (inputStr[index] != COMMA && inputStr[index] != NULL_CHAR)
    {
        //assign character from input string to buffer string
        strArg[localIndex] = inputStr[index];

        //increment index
        index++;
        localIndex++;

        //set next character to null character
        strArg[localIndex] = NULL_CHAR;
    }

    return index;
}

/*
Name: isDigit
Process: tests if a char is a digit
Function input/parameters: test character
Function output/parameters: none
Function output/returned: true for digit, false for not
Device input/---: none
Device output/---: none
Dependencies: none
*/
bool isDigit(char testChar)
{
    //check for test character between characters '0' - '9', return
    return (testChar >= '0' && testChar <= '9');
}

/*
Name: updateStartCount
Process: updates where the end index of a string is
Function input/parameters: current count, test string
Function output/parameters: none
Function output/returned: change in count
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
int updateEndCount(int count, const char *opString)
{
    //check for "end" in op string
        //function: compareString
    if (compareStrings(opString, "end") == STR_EQ)
    {
        //return incremented end count
        return count + 1;
    }
    //return unchanged end count
    return count;
}

/*
Name: updateStartCount
Process: updates where the start index of a string is
Function input/parameters: current count, test string
Function output/parameters: none
Function output/returned: change in count
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
int updateStartCount(int count, const char *opString)
{
    //check for "end" in op string
        //function: compareString
    if (compareStrings(opString, "start") == STR_EQ)
    {
        //return incremented end count
        return count + 1;
    }
    //return unchanged end count
    return count;
}

/*
Name: verifyFirstStringArg
Process: tests if first string argument is a valid input
Function input/parameters: string
Function output/parameters: none
Function output/returned: bool if valid or not
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
bool verifyFirstStringArg(const char *strArg)
{
    //checl for string holding correct first argument, return
        //function: compareString

    return (compareStrings(strArg, "access") == STR_EQ
         || compareStrings(strArg, "allocate") == STR_EQ
         || compareStrings(strArg, "end") == STR_EQ
         || compareStrings(strArg, "ethernet") == STR_EQ
         || compareStrings(strArg, "hard drive") == STR_EQ
         || compareStrings(strArg, "keyboard") == STR_EQ
         || compareStrings(strArg, "monitor") == STR_EQ
         || compareStrings(strArg, "printer") == STR_EQ
         || compareStrings(strArg, "process") == STR_EQ
         || compareStrings(strArg, "serial") == STR_EQ
         || compareStrings(strArg, "sound signal") == STR_EQ
         || compareStrings(strArg, "start") == STR_EQ
         || compareStrings(strArg, "usb") == STR_EQ
         || compareStrings(strArg, "video signal") == STR_EQ);
}

/*
Name: verifyFirstStringArg
Process: tests if command string argument is a valid input
Function input/parameters: string
Function output/parameters: none
Function output/returned: bool if valid or not
Device input/---: none
Device output/---: none
Dependencies: compareStrings
*/
bool verifyValidCommand(char *testCmd)
{
    //check for string holding three-letter op code command return
        //function: compareString
    return (compareStrings(testCmd, "sys") == STR_EQ
         || compareStrings(testCmd, "app") == STR_EQ
         || compareStrings(testCmd, "cpu") == STR_EQ
         || compareStrings(testCmd, "mem") == STR_EQ
         || compareStrings(testCmd, "dev") == STR_EQ);
}







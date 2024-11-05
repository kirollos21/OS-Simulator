
#include "simulator.h"

/*
    This function takes a ConfigDataType and OpCodeType,
    Then it finds all the processes present in the OpCode
    and creates a PCB for each process, logging all os
    operations. Once all processes are found, run each process and
    once ended end the simulation and print logs to file
*/
void runSim(ConfigDataType *configData, OpCodeType *metaData)
{
    //print title
    printf("Simulator Run\n");
    printf("-------------\n");

    //initialize variables
    char timerString[MIN_STR_LEN];
    char buffer[MAX_OUTPUT_STRING_LENGTH];
    accessTimer(ZERO_TIMER, timerString);

    pthread_t loadtid;  /* the thread identifier */
    pthread_attr_t loadattr; /* set of thread attributes */
    pthread_attr_init(&loadattr); /* Set thread */
    int loadTimeBuffer = 100; /* Buffer for loading symbol */

    /*
        Create main memory list
    */
    memoryAllocationBlock* mainMemory = NULL;

    //set buffer for initial output
    sprintf(buffer, "%s, OS: Simulator start\n", timerString);
    //print initial output
    /*
        This kind of block will appear when deciding whether to print to monitor or not
    */
    if (configData->logToCode == LOGTO_BOTH_CODE || configData->logToCode == LOGTO_MONITOR_CODE)
    {
        printf("%s", buffer);
    }
    else
    {
        printf("Outputting to File only: ");
        pthread_create(&loadtid, &loadattr, printLoadingSymbol, &loadTimeBuffer);
    }
    
    //create pointer to processes
    ProcessesList* Processes = NULL;
    ProcessesList* tempProcessPtr = NULL;
    ProcessesList* clearProcessPtr = NULL;

    //initialize string list;
    stringList* strings = (stringList*)malloc(sizeof(stringList));
    copyString(strings->string, buffer);
    strings->nextString = NULL;
    stringList* tempStrPtr = strings;
    stringList* clearStrPtr = strings;

    //find all processes and creates the PCB's for them
    Processes = findAndCreateProcesses(Processes, metaData, configData);
    tempProcessPtr = Processes;
    clearProcessPtr = Processes;


    //set Processes to READY once processes are found
    /*
        THIS IS THE CURRENT ITERATION FOR THE MASTER LOOP
        RUNS ALL PROCESSES AND ENDS ONCE ALL OF THEM ARE
        COMPLETED
    */
    while (tempProcessPtr != NULL)
    {

        //set process to ready state
        tempProcessPtr->CurrentProcess->state = READY_STATE;
        
        //// GET TIME
        accessTimer(LAP_TIMER, timerString);

        //// Process string prep start

        sprintf(buffer, "%s, OS: Process %d set to READY state from NEW state\n",
                timerString, tempProcessPtr->CurrentProcess->PID);

        tempStrPtr = printOrAddtoList(tempStrPtr, buffer, configData);

        //// Process string prep end

        tempProcessPtr = tempProcessPtr->nextProcess;
    }

    if (configData->memDisplay && 
       (configData->logToCode == LOGTO_BOTH_CODE || configData->logToCode == LOGTO_MONITOR_CODE))
    {
        printf("--------------------------------------------------\n");
        printf("After memory initialization\n");
        printf("0 [ Open, P#: x, 0-0 ] %d\n", configData->memAvailable - 1);
        printf("--------------------------------------------------\n");
    }

    //orders processes for the scheduling algorithm
    Processes = orderProcesses(Processes, configData);

    //Run master loop
    //Runs process in the order they appear in the process list
    while (Processes != NULL)
    {
        ///////// display initial OS actions prior to process start

        accessTimer(LAP_TIMER, timerString);
        sprintf(buffer, "%s, OS: Process %d selected with %d ms remaining\n",
                timerString, Processes->CurrentProcess->PID,
                (Processes->CurrentProcess->burstTimeIO +
                 Processes->CurrentProcess->burstTimeCPU));

        tempStrPtr = printOrAddtoList(tempStrPtr, buffer, configData);

        /////Tell user that process is set to running and start Process

        Processes->CurrentProcess->state = RUNNING_STATE;

        sprintf(buffer, "%s, OS: Process %d set from READY to RUNNING\n",
                timerString, Processes->CurrentProcess->PID);

        tempStrPtr = printOrAddtoList(tempStrPtr, buffer, configData);

        ////////run a process

        tempStrPtr = runProcess(Processes->CurrentProcess, configData, tempStrPtr, mainMemory);

        /*
            free process
        */
        freeProcessMemory(mainMemory);

        ////////End process output

        //set process to exit state
        Processes->CurrentProcess->state = EXIT_STATE;

        ////////Start Print OS end Process 

        //inform process ended

        accessTimer(LAP_TIMER, timerString);
        sprintf(buffer, "%s, OS: Process %d ended\n",
                timerString, Processes->CurrentProcess->PID);

        tempStrPtr = printOrAddtoList(tempStrPtr, buffer, configData);

        if (configData->memDisplay && 
           (configData->logToCode == LOGTO_BOTH_CODE || configData->logToCode == LOGTO_MONITOR_CODE))
        {
            printf("--------------------------------------------------\n");
            printf("After clear process %d success\n", Processes->CurrentProcess->PID);
            printf("0 [ Open, P#: x, 0-0 ] %d\n", configData->memAvailable - 1);
            printf("--------------------------------------------------\n");
        }

        //inform process set to exit

        accessTimer(LAP_TIMER, timerString);
        sprintf(buffer, "%s, OS: Process %d set to EXIT\n",
                timerString, Processes->CurrentProcess->PID);

        tempStrPtr = printOrAddtoList(tempStrPtr, buffer, configData);

        ////////End Print OS end Process 

        //move to next process
        Processes = Processes->nextProcess;
    }

    //display end of memory configuration
    if (configData->memDisplay && 
       (configData->logToCode == LOGTO_BOTH_CODE || configData->logToCode == LOGTO_MONITOR_CODE))
    {
        printf("--------------------------------------------------\n");
        printf("After clear all process success\n");
        printf("No memory configured\n");
        printf("--------------------------------------------------\n");
    }

    //end timer
    accessTimer(STOP_TIMER, timerString);

    //Inform OS ended simulation
    sprintf(buffer, "%s, OS: Simulator End\n", timerString);
    
    tempStrPtr = addNewStringNode(tempStrPtr, buffer);

    //if print to file only, print out upload was finished
    if (configData->logToCode == LOGTO_FILE_CODE)
    {
        sprintf(buffer, "Finished, outputted to: %.*s\n", STD_STR_LEN, configData->logToFileName);
        //pthread only cancelled if logging to file only, dont take out of if block
        pthread_cancel(loadtid);
    }
    printf("%s", buffer);
    
    //create log file and print ot log file if config file specifies to do so
    if (configData->logToCode == LOGTO_BOTH_CODE || configData->logToCode == LOGTO_FILE_CODE)
    {
        printToFile(strings, configData);
    }

    //free processes and strings memory
    freeProcesses(clearProcessPtr);
    freeStrings(clearStrPtr);
}

/*
    Create a new process control block which points to a OpCodeType as where the process starts
    along with unique PID and any other data needed for an initial process, such as finding total
    burst time, allocates memory for all needed
    returns new PCB
*/
ProcessesList* findAndCreateProcesses(ProcessesList* processes, OpCodeType *metaDataPtr, ConfigDataType *configData)
{
    //create temporary pointer for process addition
    ProcessesList* tempListPtr = NULL;
    ProcessesList* firstProcessPtr = NULL;
    OpCodeType* metaDataNode = metaDataPtr;

    //go through all metadata
    while (metaDataNode != NULL)
    {
        //if metadata is process start, create new process
        if (compareStrings(metaDataNode->command,"app") == STR_EQ && 
            compareStrings(metaDataNode->strArg1,"start") == STR_EQ)
        {
            if (processes == NULL)
            {
                processes = (ProcessesList*)malloc(sizeof(ProcessesList));
                processes->CurrentProcess = NULL;
                processes->nextProcess = NULL;
                processes->prevProcess = tempListPtr;
                tempListPtr = processes;
                if (processes->prevProcess != NULL)
                {
                    processes->prevProcess->nextProcess = processes;
                }
                if (firstProcessPtr == NULL)
                {
                    firstProcessPtr = processes;
                }
                //create new process once it is found and assign to list block
                processes->CurrentProcess = createProcess(processes->CurrentProcess, metaDataNode, configData);

                processes = processes->nextProcess;
            }

        }
        //assign current metadata pointer to nextNode
        metaDataNode = metaDataNode->nextNode;
    }

    //return first Process pointer
    return firstProcessPtr;
}

/*
    This function gets the list of processes and depending on the scheduling selected for processes,
    orders the process to conform and run as intended
*/
ProcessesList* orderProcesses(ProcessesList* processes, ConfigDataType *configData)
{
    //initialize needed pointers
    ProcessesList* firstProcessPtr = processes;
    ProcessesList* tempProcessPtr = processes;
    ProcessControlBlock* tempPCBPtr = NULL;

    //finds the process code
    if (configData->cpuSchedCode == CPU_SCHED_SJF_N_CODE)
    {
        //implements selection sort for setting SJF sched
        while (processes != NULL)
        {
            //set initial minimum process   
            ProcessesList* min = processes;
            //loops through all unsorted processes
            while(tempProcessPtr != NULL)
            {
                //if process found is shorter then min, it is set to new min
                if (tempProcessPtr->CurrentProcess->burstTimeCPU + 
                    tempProcessPtr->CurrentProcess->burstTimeIO < 
                    min->CurrentProcess->burstTimeCPU + 
                    min->CurrentProcess->burstTimeIO)
                {
                    min = tempProcessPtr;
                }

                //moves to next process
                tempProcessPtr = tempProcessPtr->nextProcess;
            }

            //swaps processes
            tempProcessPtr = min;
            tempPCBPtr = min->CurrentProcess;
            min->CurrentProcess = processes->CurrentProcess;
            processes->CurrentProcess = tempPCBPtr;

            //move to next process and set tempProcessPointer
            processes = processes->nextProcess;
            tempProcessPtr = processes;
        }
    }
    else if (configData->cpuSchedCode == CPU_SCHED_SRTF_P_CODE)
    {
        /* stub if block for future shceduling */
    }
    else if (configData->cpuSchedCode == CPU_SCHED_FCFS_P_CODE)
    {
        /* stub if block for future shceduling */
    }
    else if (configData->cpuSchedCode == CPU_SCHED_RR_P_CODE)
    {
        /* stub if block for future shceduling */
    }
    
    return firstProcessPtr;
}


/*
    Goes through all metaData and finds each unique process, adding it to a process list
*/
ProcessControlBlock* createProcess(ProcessControlBlock* process, OpCodeType *metaData, ConfigDataType *configData)
{
    //set pointer for function use
    OpCodeType* processDataPtr = metaData;

    //initialize static int to keep track of PIDs
    static int PID_Number = 0;
    if (process == NULL)
    {
        process = (ProcessControlBlock*)malloc(sizeof(ProcessControlBlock));
    }
    
    //set process PID
    process->PID = PID_Number;

    //add PID to metadata start node
    metaData->pid = PID_Number++;

    //set process to new
    process->state = NEW_STATE;
    
    //set start program node to passed in node
    process->programStartNode = processDataPtr;

    //set initial burstTime until total time is found
    process->burstTimeCPU = 0;
    process->burstTimeIO = 0;

    //initialize data/text/memory
    process->data = NULL;
    process->text = NULL;


    //loop through process until CPU/IO burst time is found
    while (processDataPtr != NULL && (compareStrings(processDataPtr->command,"app") != STR_EQ || 
            compareStrings(processDataPtr->strArg1,"end") != STR_EQ))
    {
        if (compareStrings(processDataPtr->command, "dev") == STR_EQ)
        {
            process->burstTimeIO += processDataPtr->intArg2 * configData->ioCycleRate; 
        }
        else if (compareStrings(processDataPtr->command, "cpu") == STR_EQ)
        {
            process->burstTimeCPU += processDataPtr->intArg2 * configData->procCycleRate; 
        }

        processDataPtr = processDataPtr->nextNode;
    }

    //return current process PID
    return process;
}

/*
    Runs a given process, running as long as dictated by the config file, along with
    outputting any info to log gile or monitor. Uses POSIX threads for IO
*/
stringList* runProcess(ProcessControlBlock* process, ConfigDataType* config, 
                       stringList* strings, memoryAllocationBlock* memory)
{
    //declare/initialize variables
    char timerString[MIN_STR_LEN];
    char buffer[MAX_OUTPUT_STRING_LENGTH];
    int waitSeconds = 0;
    void* param;
    
    //get start of program 
    OpCodeType* programPtr = process->programStartNode;

    //create thread variables for future use
    pthread_t IOtid;  /* the thread identifier */
    pthread_attr_t attr; /* set of thread attributes */
    pthread_attr_init(&attr); /* Set thread */

    //loop through metaData until either end of process or empty ptr
    while (programPtr != NULL && (compareStrings(programPtr->command, "app") != STR_EQ
    || compareStrings(programPtr->strArg1, "end") != STR_EQ))
    {
        //check if this is a command that is to be outputted
        if (compareStrings(programPtr->inOutArg, "in") == STR_EQ
        || compareStrings(programPtr->inOutArg, "out") == STR_EQ)
        {
            ////START OPERATION
            //prints out a operations has started
            accessTimer(LAP_TIMER, timerString);
            sprintf(buffer, "%s, Process: %d, %s %s %sput Operation start\n",
                timerString, process->PID, programPtr->command, programPtr->strArg1,
                programPtr->inOutArg);
                
            strings = printOrAddtoList(strings, buffer, config);

            /*
                Test which operation category is being executed 
                dev: IO
                cpu: cpu
                and run timerthread for IO operations
            */
            if (compareStrings(programPtr->command, "cpu") == STR_EQ)
            {
                waitSeconds = programPtr->intArg2 * config->ioCycleRate;
                param = &waitSeconds;
                runTimer(param);
            }
            else if (compareStrings(programPtr->command, "dev") == STR_EQ)
            {
                waitSeconds = programPtr->intArg2 * config->ioCycleRate;
                param = &waitSeconds;
                pthread_create(&IOtid, &attr, runTimer, param);
                pthread_join(IOtid, NULL);
            }

            ////print out operation end
            accessTimer(LAP_TIMER, timerString);
            sprintf(buffer, "%s, Process: %d, %s %s %sput Operation end\n",
                timerString, process->PID, programPtr->command, programPtr->strArg1,
                programPtr->inOutArg);

            strings = printOrAddtoList(strings, buffer, config);
            ////END OPERATION
        }

        /*
            BLOCK FOR mem COMMAND
        */
        else if (compareStrings(programPtr->command, "mem") == STR_EQ)
        {
            /*
                if block for mem allocate command
            */
            //memblock pointer for output
            memoryAllocationBlock* outputMem = NULL;

            if (compareStrings(programPtr->strArg1, "allocate") == STR_EQ)
            {
                //print code
                accessTimer(LAP_TIMER, timerString);
                sprintf(buffer, "%s, Process: %d, %s allocate request (%d, %d)\n",
                timerString, process->PID, programPtr->command, programPtr->intArg2,
                programPtr->intArg3);

                strings = printOrAddtoList(strings, buffer, config);

                //end of print code

                //create a memory block and allocate memory to it
                memoryAllocationBlock* newMemory = createMemoryBlock(programPtr->intArg2, programPtr->intArg3);

                //check if the selected memory for the newMemory is available, if yes add it
                //if not free the newMemory and end the process
                if (checkIfMemoryAvailable(memory, newMemory, config, &outputMem))
                {
                    //add the new memory to the memory linked list
                    memory = addMemoryBlock(memory, newMemory);
                    freeProcessMemory(newMemory);

                    if (config->memDisplay && 
                       (config->logToCode == LOGTO_BOTH_CODE || config->logToCode == LOGTO_MONITOR_CODE))
                    {
                        printf("--------------------------------------------------\n");                    
                        printf("After allocate success\n");
                        printf("0 [ Used, P#: %d, %d-%d ] %d\n", process->PID,
                        newMemory->memoryBaseAddress, newMemory->memoryBaseAddress + programPtr->intArg3 - 1,
                        newMemory->memoryAllocated - 1);
                        printf("%d [ Open, P#: x, 0-0 ] %d\n", newMemory->memoryAllocated, config->memAvailable - 1);
                        printf("--------------------------------------------------\n");
                    }

                    //print code
                    accessTimer(LAP_TIMER, timerString);
                    sprintf(buffer, "%s, Process: %d, successful mem allocate request\n",
                    timerString, process->PID);

                    strings = printOrAddtoList(strings, buffer, config);
                }
                //if memory overlaps with already allocated memory, or out of range
                else
                {
                    if (config->memDisplay && 
                       (config->logToCode == LOGTO_BOTH_CODE || 
                       config->logToCode == LOGTO_MONITOR_CODE))
                    {
                        printf("--------------------------------------------------\n");
                        printf("After allocate overlap failure\n");
                        printf("0 [ Used, P#: %d, %d-%d ] %d\n", process->PID, outputMem->memoryBaseAddress, 
                        outputMem->memoryBaseAddress + outputMem->memoryAllocated - 1, outputMem->memoryAllocated - 1);
                        printf("0 [ Open, P#: x, 0-0 ] %d\n", config->memAvailable - 1);
                        printf("--------------------------------------------------\n");
                    }
                    
                    //free the new memory
                    freeProcessMemory(newMemory);
                    newMemory = NULL;

                    //print code
                    accessTimer(LAP_TIMER, timerString);
                    sprintf(buffer, "%s, Process: %d, failed mem allocate request\n",
                    timerString, process->PID);

                    strings = printOrAddtoList(strings, buffer, config);

                    //print code
                    accessTimer(LAP_TIMER, timerString);
                    sprintf(buffer, "%s, OS: Segmentation fault, Process %d ended\n",
                    timerString, process->PID);

                    strings = printOrAddtoList(strings, buffer, config);

                    return strings;
                }

            }
            /*
                if block for mem access command
            */
            else if (compareStrings(programPtr->strArg1, "access") == STR_EQ)
            {
                //print code
                accessTimer(LAP_TIMER, timerString);
                sprintf(buffer, "%s, Process: %d, %s access request (%d, %d)\n",
                timerString, process->PID, programPtr->command, programPtr->intArg2,
                programPtr->intArg3);

                strings = printOrAddtoList(strings, buffer, config);

                //end of print code

                //check if the memory being accesses is allocated and able to be accesses
                if (checkIfMemoryExists(memory, programPtr->intArg2, programPtr->intArg3, &outputMem))
                {

                    //print out memory operation results
                    if (config->memDisplay && 
                       (config->logToCode == LOGTO_BOTH_CODE || 
                        config->logToCode == LOGTO_MONITOR_CODE))
                    {
                        printf("--------------------------------------------------\n");                    
                        printf("After access success\n");
                        printf("0 [ Used, P#: %d, %d-%d ] %d\n", process->PID,
                        outputMem->memoryBaseAddress, outputMem->memoryBaseAddress + outputMem->memoryAllocated - 1,
                        outputMem->memoryAllocated - 1);
                        printf("%d [ Open, P#: x, 0-0 ] %d\n", outputMem->memoryAllocated, config->memAvailable - 1);
                        printf("--------------------------------------------------\n");
                    }

                    //print code
                    accessTimer(LAP_TIMER, timerString);
                    sprintf(buffer, "%s, Process: %d, successful mem access request\n",
                    timerString, process->PID);

                    strings = printOrAddtoList(strings, buffer, config);
                }
                //if memory access is out of range or invalid
                else
                {
                    if (config->memDisplay && 
                       (config->logToCode == LOGTO_BOTH_CODE || 
                        config->logToCode == LOGTO_MONITOR_CODE))
                    {
                        printf("--------------------------------------------------\n");
                        printf("After access failure\n");
                        printf("0 [ Used, P#: %d, %d-%d ] %d\n", process->PID,
                        programPtr->intArg2, programPtr->intArg2 + programPtr->intArg3 - 1,
                        programPtr->intArg3 - 1);
                        printf("%d [ Open, P#: x, 0-0 ] %d\n", programPtr->intArg3, config->memAvailable - 1);
                        printf("--------------------------------------------------\n");
                    }

                    //print code
                    accessTimer(LAP_TIMER, timerString);
                    sprintf(buffer, "%s, Process: %d, failed mem access request\n",
                    timerString, process->PID);

                    strings = printOrAddtoList(strings, buffer, config);

                    //print code
                    accessTimer(LAP_TIMER, timerString);
                    sprintf(buffer, "%s, OS: Segmentation fault, Process %d ended\n",
                    timerString, process->PID);

                    strings = printOrAddtoList(strings, buffer, config);

                    return strings;
                }
            }
        }
        
        //move to next operation
        programPtr = programPtr->nextNode;
    }

    return strings;
}

/*
    creates a memory block for a process and return a pointer to the block
*/
memoryAllocationBlock* createMemoryBlock(int memoryBase, int memoryOffset)
{
    //allocate memory
    memoryAllocationBlock* memory = (memoryAllocationBlock*)malloc(sizeof(memoryAllocationBlock));

    memory->memoryBaseAddress = memoryBase;
    memory->memoryAllocated = memoryOffset;

    memory->nextMemory = NULL;
    memory->prevMemory = NULL;

    return memory;
}

/*
    check if memory is available in a process
*/
bool checkIfMemoryAvailable(memoryAllocationBlock *memory, memoryAllocationBlock* testMemory, ConfigDataType* config, memoryAllocationBlock** outMem)
{
    memoryAllocationBlock* tempPtr = memory;

    int testMemEndAddress;
    int tempMemEndAddress;

    while (tempPtr != NULL)
    {
        testMemEndAddress = testMemory->memoryBaseAddress + testMemory->memoryAllocated;
        tempMemEndAddress = tempPtr->memoryBaseAddress + tempPtr->memoryAllocated;

        if (//Tests if new memory start is in already allocated memory
            (testMemory->memoryBaseAddress > tempPtr->memoryBaseAddress &&
            testMemory->memoryBaseAddress < tempMemEndAddress) ||
            //Tests if new memory end is in already allocated memory
            (testMemEndAddress > tempPtr->memoryBaseAddress &&
            testMemEndAddress < tempMemEndAddress) ||
            //check if memory allocation is within config permitted bounds
            (testMemEndAddress > config->memAvailable))
        {
            *outMem = tempPtr; 
            return false;
        }

        tempPtr = tempPtr->nextMemory;
    }

    return true;
}

/*
    Check if a memory block has been reserved and is accessible by the membase and offset
*/
bool checkIfMemoryExists(memoryAllocationBlock* memory, int memBase, int memOffset, memoryAllocationBlock** outMem)
{
    memoryAllocationBlock* tempMemPtr = memory;

    while (tempMemPtr != NULL)
    {
        //check if access is within bounds of a memory block
        if (tempMemPtr->memoryBaseAddress <= memBase && 
        tempMemPtr->memoryBaseAddress + tempMemPtr->memoryAllocated
        >= memBase + memOffset)
        {
            *outMem = tempMemPtr;
            return true;
        }

        tempMemPtr = tempMemPtr->nextMemory;
    }
    return false;
}

/*
    Adds new memory block to end of process memory linked list
*/
memoryAllocationBlock* addMemoryBlock(memoryAllocationBlock *memory, memoryAllocationBlock* newMemory)
{
    memoryAllocationBlock* firstMemPtr = memory;

    //if first memory allocation, sets memory
    if (memory == NULL)
    {
        firstMemPtr = newMemory;
    }
    else
    {   
        //loops through memory until next empty spot is found
        while (memory->nextMemory != NULL)
        {
            memory = memory->nextMemory;
        }
        memory->nextMemory = newMemory;
        newMemory->prevMemory = memory;
    }

    return firstMemPtr;
}

/*
    deallocates memory from memory block
*/
void freeProcessMemory(memoryAllocationBlock* memory)
{
    if (memory != NULL)
    {
        freeProcessMemory(memory->nextMemory);
        free(memory);
        memory = NULL;
    }
}

/*
    Recursively goes through processes and frees allocated memory
*/
void freeProcesses(ProcessesList* processes)
{
    if (processes != NULL)
    {
        freeProcesses(processes->nextProcess);
        free(processes->CurrentProcess);
        free(processes);
    }
}

/*
    Adds new string node to string list to be outputted to file
*/
stringList* addNewStringNode(stringList* strings, char* buffer)
{
    //allocate memory
    strings->nextString = (stringList*)malloc(sizeof(stringList));

    //copy string into new stringList and set initial data
    copyString(strings->nextString->string, buffer);
    strings->nextString->nextString = NULL;
    strings->nextString->prevString = strings;
    return strings->nextString;
}

/*
    Handles printing to monitor or saving to string list for file output
*/
stringList* printOrAddtoList(stringList* tempStrPtr, char buffer[], ConfigDataType* config)
{
    if (config->logToCode == LOGTO_BOTH_CODE || config->logToCode == LOGTO_MONITOR_CODE)
    {   
        printf("%s", buffer);
    }

    return addNewStringNode(tempStrPtr, buffer);;
}


/*
    Recursively Free string list of allocated memory
*/
void freeStrings(stringList* strings)
{
    if (strings != NULL)
    {
        freeStrings(strings->nextString);
        free(strings);
        strings = NULL;
    }
}

/*
    Take all simulator logs and output to a log file as
    specified in the config file, takes in config file
    and stringList
*/
void printToFile(stringList* strOutput, ConfigDataType* config)
{
    //Create and set file pointer
    FILE* outputFilePtr;

    outputFilePtr = fopen(config->logToFileName, "w");


    //loop through each line in stringList and outputting to file
    while(strOutput != NULL)
    {

        fprintf(outputFilePtr, "%s", strOutput->string);
        
        strOutput = strOutput->nextString;

    }

    //CLOSE THE FILE
    fclose(outputFilePtr);
}

/*
    When output is only going to file, runs a loading symbol 
    as a POSIX thread until the Simulator is finished, where 
    this function as a thread should be cancelled
*/
void* printLoadingSymbol(void* timeUntilSymbolChange)
{
    //initialize symbols
    static int pos = 0;
    char cursor[4] = {'/','-','\\','|'};
    while (true)
    {
        //print out current symbol
        printf("%c\b", cursor[pos]);
        //flush output for new symbol
        fflush(stdout);
        //move to next symbol
        pos = (pos+1) % 4;
        //set a buffer for the change of symbol
        runTimer(timeUntilSymbolChange);
    }
}


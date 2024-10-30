//  File: simulator.c
//  Project: Sim02
//  Secret ID: 708996
//  Date: 09/30/2024

#include "simulator.h"

/*
Name: runSim
process: primary simulation driver
Function Input/Parameters:  configuration data (ConfigDataType *),
                            metadata (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: initializeMemory, allocateMemoryBlock, freeMemoryBlock
*/
void runSim(ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr)
{
    // Initialize variables
    OpCodeType *localMetaPtr = metaDataMstrPtr;
    PCB *newPCBList = createPCB_List(configPtr, localMetaPtr);
    PCB *wkgPtrPCB = newPCBList;
    char timeStr[10];
    double elapsedTime = 0.0;   // Global elapsed time tracker
    FILE *file = NULL;

    // Initialize memory for the simulation
    MemoryBlock *memoryList = initializeMemory(configPtr);
    if (!memoryList) {
        printf("Error: Memory initialization failed.\n");
        return;
    }

    // Open the log file if logging to a file is required
    if (configPtr->logToCode == LOGTO_FILE_CODE || configPtr->logToCode == LOGTO_BOTH_CODE)
    {
        file = fopen(configPtr->logToFileName, "w");
        if (file == NULL)
        {
            printf("Error: Unable to open log file\n");
            return;
        }
    }

    // Start the simulation timer and ensure the first timestamp is 0.000000
    accessTimer(ZERO_TIMER, timeStr);
    elapsedTime = 0.000000;  // Set the initial time to zero explicitly

    // Print the title
    printTitle(configPtr, file, elapsedTime);

    // Immediately update elapsed time after timer starts for accurate state transitions
    elapsedTime += accessTimer(LAP_TIMER, timeStr);

    // Loop through the PCB list to set each process to READY state
    while (wkgPtrPCB != NULL)
    {
        wkgPtrPCB->currentState = READY_STATE;

        // Log the state transition for each process
        displayProcessState(configPtr, wkgPtrPCB, elapsedTime, file);

        // Move to the next process in the list
        wkgPtrPCB = wkgPtrPCB->nextNode;
    }

    // Reset working PCB pointer to the start of the list
    wkgPtrPCB = newPCBList;

    // Loop through all the processes for execution
    while (wkgPtrPCB != NULL)
    {
        // Get the next process
        wkgPtrPCB = getNextProcess(wkgPtrPCB, localMetaPtr);

        // Set process state to RUNNING
        wkgPtrPCB->currentState = RUNNING_STATE;

        // Update elapsed time before logging
        elapsedTime += accessTimer(LAP_TIMER, timeStr);  // Accumulate global time

        // Print process selection and transition to running state
        if (configPtr->logToCode != LOGTO_FILE_CODE)
        {
            printf("%1.6f, OS: Process %d selected with %d ms remaining\n", elapsedTime, wkgPtrPCB->pid, wkgPtrPCB->time);
            printf("%1.6f, OS: Process %d set from READY to RUNNING\n", elapsedTime, wkgPtrPCB->pid);
        }

        if (file != NULL)
        {
            fprintf(file, "%1.6f, OS: Process %d selected with %d ms remaining\n", elapsedTime, wkgPtrPCB->pid, wkgPtrPCB->time);
            fprintf(file, "%1.6f, OS: Process %d set from READY to RUNNING\n", elapsedTime, wkgPtrPCB->pid);
        }

        // Handle process operations
        displayOpCode(configPtr, wkgPtrPCB->mdPtr, wkgPtrPCB, file, &elapsedTime);

        // Memory allocation for processes requesting memory (example with 1024KB):
        if (allocateMemoryBlock(&memoryList, wkgPtrPCB->pid, 1024))
        {
            printf("Memory allocated for Process %d\n", wkgPtrPCB->pid);
        }
        else
        {
            printf("Failed to allocate memory for Process %d\n", wkgPtrPCB->pid);
        }

        // Memory deallocation for processes after execution
        freeMemoryBlock(&memoryList, wkgPtrPCB->pid);

        // Print the process exit status
        wkgPtrPCB->currentState = EXIT_STATE;
        displayProcessState(configPtr, wkgPtrPCB, elapsedTime, file);  // Log EXIT state

        // Display current memory blocks for logging
        displayMemoryBlocks(memoryList);

        // Move to the next process
        wkgPtrPCB = wkgPtrPCB->nextNode;
    }

    // Print the system stop message
    elapsedTime += accessTimer(LAP_TIMER, timeStr);  // Final time accumulation

    if (configPtr->logToCode != LOGTO_FILE_CODE)
    {
        printf("%1.6f, OS: System stop\n", elapsedTime);
    }

    if (file != NULL)
    {
        fprintf(file, "%1.6f, OS: System stop\n", elapsedTime);
        fflush(file);
    }

    elapsedTime += accessTimer(LAP_TIMER, timeStr);  // Accumulate time again

    if (configPtr->logToCode != LOGTO_FILE_CODE)
    {
        printf("%1.6f, OS: Simulation end\n", elapsedTime);
    }

    if (file != NULL)
    {
        fprintf(file, "%1.6f, OS: Simulation end\n", elapsedTime);
        fflush(file);
        fclose(file);
    }

    if (configPtr->logToCode != LOGTO_FILE_CODE)
    {
        printf("\nSimulator Program End.\n\n");
    }

    if (file != NULL)
    {
        fprintf(file, "\nSimulator Program End.\n\n");
        fflush(file);
    }

    // Stop the timer
    accessTimer(STOP_TIMER, timeStr);
}

/*
Name: createNewPCBNode
Process: Creates a NEW_STATE PCB node with specified process ID 
and metadata pointer
Function Input/Parameters: Process ID, pointer to metadata
Function Output/Parameters: None
Function Output/Returned: Newly created PCB node pointer
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
PCB *createNewPCBNode(int pid, OpCodeType *mdPtr) 
{
    // Allocate memory for the NEW_STATE PCB node
    PCB *newNode = (PCB *)malloc(sizeof(PCB));
    //check if memory allocation failed
    if (newNode == NULL) 
    {
        // Memory allocation failed
        return NULL;
    }

    // Set PCB attributes
    newNode->pid = pid;
    newNode->mdPtr = mdPtr;
    newNode->time = 0.0;
    newNode->currentState = NEW_STATE;
    newNode->nextNode = NULL;
    //return NEW_STATE node
    return newNode;
}

/*
Name: createPCB_List
Process: Creates a linked list of PCB nodes based on the configuration
and metadata
Function Input/Parameters: Configuration pointer, Metadata pointer
Function Output/Parameters: None
Function Output/Returned: Head pointer of the created PCB linked list
Device Input/File: None
Device Output/Device: None
Dependencies: createNewNode function
*/
PCB *createPCB_List ( ConfigDataType *configPtr, OpCodeType *metaData)
{
	//intialize variables
	OpCodeType *metaPtr = metaData;
	int pid = 0;
	PCB *headPtrProcess = NULL;
	PCB *wkgPtrProcess = NULL;
	//compute data
	//loop across the metaData until sys end
	while(metaPtr != NULL)
	{
		//check for app start
		//function: compareString
		if (compareString(metaPtr->command, "app"
		                 ) == STR_EQ && compareString(
		            metaPtr->strArg1, "start") == STR_EQ)
		{
			//check if the we are at the beginning
			if (pid == 0)
			{
				//set the first node to the pcb head ptr
				headPtrProcess = (PCB *)malloc(sizeof(PCB));

				//set values of the headptr - createnodefunction
				headPtrProcess->nextNode = NULL;

				//set working ptr to head
				wkgPtrProcess = headPtrProcess;
			}
			//otherwise we are not at the beginning but it is a new process
			else
			{
				//create new node
				//function: createNewNode
				wkgPtrProcess->nextNode = createNewNode(wkgPtrProcess);

				//move to next ptr
				wkgPtrProcess = wkgPtrProcess->nextNode;
			}

			//set PCB nodes
			//set pid
			metaPtr->pid = pid;
			wkgPtrProcess->pid = pid;
			//advance the pid
			pid++;

			//set time remaining to zero
			wkgPtrProcess->time = 0.0;

			//set state = NEW
			wkgPtrProcess->currentState = NEW_STATE;

			//set pointer to the current md node
			wkgPtrProcess->mdPtr = metaPtr;

			//loop to the end of the proces
			while( compareString(wkgPtrProcess->mdPtr->command,
			                     "app")!= STR_EQ || compareString(
			            wkgPtrProcess->mdPtr->strArg1, "end")!= STR_EQ)
			{
				//move the metadata node to the next node
				metaPtr = metaPtr->nextNode;

				//Milliseconds = config time * the cycle rate (in arg2)
				//check for CPU/Process
				//function: compareString
				if(compareString(wkgPtrProcess->mdPtr->command, "cpu")==STR_EQ)
				{
					//add the calculated cpu time to remain time
					wkgPtrProcess->time += (
					                           wkgPtrProcess->mdPtr->intArg2 * configPtr->procCycleRate);

				}
				//check for dev /io op
				//function: compareString
				if(compareString(wkgPtrProcess->mdPtr->command,"dev") == STR_EQ)
				{
					//add calculated IO time to remain time
					wkgPtrProcess->time += (
					                           wkgPtrProcess->mdPtr->intArg2 * configPtr->ioCycleRate);

				}
				//move wkgptr to the next node
				wkgPtrProcess->mdPtr = wkgPtrProcess->mdPtr->nextNode;
			}
		}
		//advance the pointer
		metaPtr = metaPtr->nextNode;
	}

	//return list
	return headPtrProcess;
}

/*
Name: displayProcessState
Process: Displays the current state of the process based on the configuration settings
Function Input/Parameters: Configuration pointer, Process pointer, Lap time, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
void displayProcessState(ConfigDataType *config, PCB *process, double lapTime, FILE* file)
{
	// Check logToCode value for monitor
	if (config->logToCode == LOGTO_MONITOR_CODE || config->logToCode == LOGTO_BOTH_CODE)
	{
		// Display process state to monitor
		displayToMonitor(process, lapTime);
	}

	// Check logToCode value for file
	if (config->logToCode == LOGTO_FILE_CODE || config->logToCode == LOGTO_BOTH_CODE)
	{
		// Display process state to file
		displayToFile(process, lapTime, file);
	}
}

/*
Name: displayToMonitor
Process: Displays the current state of the process to the monitor
Function Input/Parameters: Process pointer, Lap time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: Monitor
Dependencies: None
*/
void displayToMonitor(PCB *process, double lapTime)
{
	//depending on the currentState - monitor
	switch (process->currentState)
	{
	//READY
	case READY_STATE:
		//print ready statment
		//function: printf
		printf("%1.6f, OS: Process %d set to READY state from NEW state\n", lapTime, process->pid);
		break;

	//RUNNING
	case RUNNING_STATE:
		//print runnning statment
		//function: printf
		printf("%1.6f, OS: Process %d selected with %d ms remaining\n", lapTime, process->pid, process->time);
		printf("%1.6f, OS: Process %d set from READY to RUNNING\n\n", lapTime, process->pid);
		printf("\n");
		break;

	//EXITING
	case EXIT_STATE:
		//print exiting statment
		//function: printf
		printf("\n");
		printf("%1.6f, OS: Process %d ended\n", lapTime, process->pid);
		printf("%1.6f, OS: Process %d set to EXIT\n", lapTime, process->pid);
		break;
	//ERROR
	default:
		//print error statment
		//function: printf
		printf("Error: Invalid process state\n");
		break;
	}
}

/*
Name: displayToFile
Process: Displays the current state of the process to the file
Function Input/Parameters: Process pointer, Lap time, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: File
Dependencies: None
*/
void displayToFile(PCB *process, double lapTime, FILE* file)
{
	//depending on the currentState - file
	switch (process->currentState)
	{
	//READY
	case READY_STATE:
		//print ready statment tofile
		//function: fprintf
		fprintf(file, "%1.6f, OS: Process %d set to READY state from NEW state\n", lapTime, process->pid);
		fflush(file);
		break;
	//Running
	case RUNNING_STATE:
		//print RUNNING statment tofile
		//function: fprintf
		fprintf(file, "%1.6f, OS: Process %d selected with %d ms remaining\n", lapTime, process->pid, process->time);
		fflush(file);
		fprintf(file, "%1.6f, OS: Process %d set from READY to RUNNING\n\n", lapTime, process->pid);
		fflush(file);
		fprintf(file, "\n");
		fflush(file);
		break;

	//Exiting
	case EXIT_STATE:
		//print EXITING statment to file
		//function: fprintf
		fprintf(file, "\n");
		fprintf(file, "%1.6f, OS: Process %d ended\n", lapTime, process->pid);
		fflush(file);
		fprintf(file, "%1.6f, OS: Process %d set to EXIT\n", lapTime, process->pid);
		fflush(file);
		break;
	//ERROR
	default:
		//print error statment to file
		//function: fprintf
		fprintf(file, "Error: Invalid process state\n");
		fflush(file);
		break;
	}
}

/*
Name: getNextProcess
Process: Retrieves the next process in the linked list of
 processes based on the current process and metadata
Function Input/Parameters: Current process pointer, Metadata pointer
Function Output/Parameters: None
Function Output/Returned: Next process pointer
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
PCB *getNextProcess(PCB *currentProcess,OpCodeType *metaData)
{
	//initialize variables
	PCB *wrkPtr = currentProcess;
	OpCodeType *localMeta = metaData;

	//compute data
	//see if we are at the end of the process
	if (compareString(
	            wrkPtr->mdPtr->command, "app")== STR_EQ && compareString(
	            wrkPtr->mdPtr->strArg1, "end")==STR_EQ)
	{
		//if we are then loop through the metadata
		while (localMeta != NULL)
		{
			//check if they are at the same pid as the metadata
			if (localMeta->pid == wrkPtr->pid)
			{
				//if we are at the very beginning of the file
				if(compareString(localMeta->command, "sys")== STR_EQ)
				{
					//move to the next one
					localMeta = localMeta->nextNode;
				}
				//move to the next one
				wrkPtr->mdPtr = localMeta;
				//return the wrkptr
				return wrkPtr;
			}
			//move to the next MetaData
			localMeta = localMeta->nextNode;
		}
	}
	//return NULL if we are not at the end
	return NULL;
}

/*
Name: printTitle
Process: Prints the title and the start time of the
simulation based on the configuration settings
Function Input/Parameters: Configuration pointer,
File pointer, Time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
void printTitle(ConfigDataType *config, FILE* fileName, double time)
{
	if (config->logToCode==LOGTO_MONITOR_CODE)
	{
		//display title
		//print title
		//function: printf
		printf("Simulator Run\n");
		printf("-------------\n\n");

		//display TIME, OS: Simulator start
		//function: printf
		printf("%1.6f, OS: Simulator start\n", time);

	}
	else if (config->logToCode==LOGTO_FILE_CODE)
	{
		//display title
		//print title
		//function: printf
		fprintf(fileName, "Simulator Run\n");
		fprintf(fileName, "-------------\n\n");
		//display TIME, OS: Simulator start
		//function: printf
		fprintf(fileName,"%1.6f, OS: Simulator start\n", time);
	}
	else if (config->logToCode== LOGTO_BOTH_CODE)
	{
		//display title
		//print title
		//function: printf
		printf("Simulator Run\n");
		printf("-------------\n\n");
		//display TIME, OS: Simulator start
		//function: printf
		printf("%1.6f, OS: Simulator start\n", time);

		//display title
		//print title
		//function: printf
		fprintf(fileName, "Simulator Run\n");
		fprintf(fileName, "-------------\n\n");
		//display TIME, OS: Simulator start
		//function: printf
		fprintf(fileName,"%1.6f, OS: Simulator start\n", time);

	}
}

MemoryBlock *initializeMemory(ConfigDataType *configPtr) {
    // Allocate memory for the head block
    MemoryBlock *headBlock = (MemoryBlock *)malloc(sizeof(MemoryBlock));
    if (headBlock == NULL) {
        return NULL; // Handle allocation failure if necessary
    }
    headBlock->base = 0;
    headBlock->offset = configPtr->memAvailable;
    headBlock->pid = -1;   // Indicates free memory
    headBlock->next = NULL;

    return headBlock;
}

bool allocateMemoryBlock(MemoryBlock **head, int pid, int size) {
    MemoryBlock *current = *head;
    while (current != NULL) {
        if (current->pid == -1 && current->offset >= size) { // Check for free block
            // Allocate the block to the process
            current->pid = pid;
            int originalOffset = current->offset;
            current->offset = size;

            // If there's remaining space, split the block
            if (originalOffset > size) {
                MemoryBlock *newBlock = (MemoryBlock *)malloc(sizeof(MemoryBlock));
                newBlock->base = current->base + size;
                newBlock->offset = originalOffset - size;
                newBlock->pid = -1;  // Free block
                newBlock->next = current->next;
                current->next = newBlock;
            }
            return true; // Allocation successful
        }
        current = current->next;
    }
    return false; // No suitable block found
}

void freeMemoryBlock(MemoryBlock **head, int pid) {
    MemoryBlock *current = *head;
    MemoryBlock *prev = NULL;

    while (current != NULL) {
        if (current->pid == pid) { // Block owned by this process
            current->pid = -1; // Mark as free

            // Merge with next block if it’s free
            if (current->next != NULL && current->next->pid == -1) {
                current->offset += current->next->offset;
                MemoryBlock *temp = current->next;
                current->next = current->next->next;
                free(temp);
            }
            // Merge with previous block if it’s free
            if (prev != NULL && prev->pid == -1) {
                prev->offset += current->offset;
                prev->next = current->next;
                free(current);
                current = prev; // Set current back to prev after merging
            }
        }
        prev = current;
        current = current->next;
    }
}

void displayMemoryBlocks(MemoryBlock *head) {
    MemoryBlock *current = head;
    printf("Memory Layout:\n");
    while (current != NULL) {
        printf("Base: %d, Offset: %d, PID: %d\n", current->base, current->offset, current->pid);
        current = current->next;
    }
    printf("Available Memory: %d KB\n", configPtr->memAvailable);
}

bool accessMemory(int base, int offset) {
    MemoryBlock *block = findBlockByRange(base, offset);
    return block != NULL; // Return success if block is found
}

/*
Name: displayMem
Process: Displays memory details
Function Input/Parameters: Pointer to PCB node, pointer to 
configuration data type, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void displayMem(PCB *process, ConfigDataType *configPtr, FILE *outputFile)
{
    // Declare/initialize variables
    int memStart = 0;
    int memBase = process->mdPtr->intArg2;
    int memOffset = process->mdPtr->intArg3;
    int memRange = memOffset - 1;
    int memEndRange = process->mdPtr->intArg2 + memRange;
    int memBaseRequest = process->mdPtr->intArg2;
    int memOffsetRequest = memOffset - 1;
    double time;
    
    // Get the current time
    time = accessTimer(LAP_TIMER, NULL); 

    //check if log to == monitor or both
    if (configPtr->logToCode == LOGTO_MONITOR_CODE || 
                     configPtr->logToCode == LOGTO_BOTH_CODE)
    {
        //then display to monitor
        printf("%1.6f, Process: %d, %s %s request (%d, %d)\n", 
        time, process->pid, process->mdPtr->command,  
        process->mdPtr->strArg1, memBase, memOffset);
        //display memory details
        printf("--------------------------------------------------\n");
        //check if the request is allocate
        if (compareString(process->mdPtr->strArg1, "allocate") == STR_EQ)
        {
            //display success
            printf("After %s success\n", process->mdPtr->strArg1);
            //display memory details
            printf("%d [ Used, P#: %d, %d-%d ] %d\n", memStart, process->pid, 
                                             memBase, memEndRange, memRange);
            //display memory details
            printf("%d [ Open, P#: x, 0-0 ] %d\n", memBase, 
                                                   configPtr->memAvailable);
        }
        //otherwise if the request is access
        else if (compareString(process->mdPtr->strArg1, "access") == STR_EQ)
        {
            //check if the memory range is greater than the offset request
            if (memRange >= memOffsetRequest && memBase == memBaseRequest)
            {
                //display success
                printf("After %s success\n", process->mdPtr->strArg1);
                //display memory details
                printf("%d [ Used, P#: %d, %d-%d ] %d\n", memStart, process->pid, 
                                             memBaseRequest, memEndRange, memRange);
                //display memory details
                printf("%d [ Open, P#: x, 0-0 ] %d\n", memBaseRequest, 
                                                   configPtr->memAvailable);
                //display success
                printf("%1.6f, Process: %d, successful %s %s request\n", 
                time, process->pid, process->mdPtr->command, 
                process->mdPtr->strArg1);
            }
            //otherwise
            else
            {
                //display failure
                printf("After %s failure\n", process->mdPtr->strArg1);
            }
        }
        //display line
        printf("--------------------------------------------------\n");
    }

    //if log to == file or both 
    if (configPtr->logToCode == LOGTO_FILE_CODE || 
                                configPtr->logToCode == LOGTO_BOTH_CODE)
    {
        //then display to file
        fprintf(outputFile, 
        "%1.6f, Process: %d, %s %s request (%d, %d)\n", 
        time, process->pid, process->mdPtr->command,
         process->mdPtr->strArg1, memBase, memOffset);
        fprintf(outputFile,
         "--------------------------------------------------\n");
        //check if the request is allocate
        if (compareString(process->mdPtr->strArg1, "allocate") == STR_EQ)
        {
            //display success
            fprintf(outputFile, "After %s success\n", 
                                  process->mdPtr->strArg1);
            //display memory details
            fprintf(outputFile, "%d [ Used, P#: %d, %d-%d ] %d\n", 
                      memStart, process->pid, memBase, memEndRange, memRange);
            //display memory details
            fprintf(outputFile, "%d [ Open, P#: x, 0-0 ] %d\n", 
                                            memBase, configPtr->memAvailable);
        }
        //otherwise if the request is access
        else if (compareString(process->mdPtr->strArg1, "access") == STR_EQ)
        {
            //check if the memory range is greater than the offset request
            if (memRange >= memOffsetRequest && memBase == memBaseRequest)
            {
                //display success
                fprintf(outputFile, "After %s success\n", 
                                                   process->mdPtr->strArg1);
                //display memory details
                fprintf(outputFile, "%d [ Used, P#: %d, %d-%d ] %d\n", 
                memStart, process->pid, memBaseRequest, memEndRange, memRange);
                //display memory details
                fprintf(outputFile, "%d [ Open, P#: x, 0-0 ] %d\n", 
                memBaseRequest, configPtr->memAvailable);
                //display success
                fprintf(outputFile, 
                "%1.6f, Process: %d, successful %s %s request\n", 
                                  time, process->pid, process->mdPtr->command,
                                                     process->mdPtr->strArg1);
            }
            //otherwise
            else
            {
                //display failure
                fprintf(outputFile, "After %s failure\n", 
                                                   process->mdPtr->strArg1);
            }
        }
        //display line
        fprintf(outputFile,
                      "--------------------------------------------------\n");
    }
    //return nothing
}

/*
Name: getTimer
Process: Retrieves lap time using a timer thread
Function Input/Parameters: Pointer to PCB node
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: runTimer
*/
void getTimer(PCB *processPtr)
{
   //initialize variables
    pthread_t timer;
    TimerData timerData = { .lap = 0 };

    // Create the timer thread
    pthread_create(&timer, NULL, runTimer, (void*)&timerData);
    
    // Join the timer thread
    pthread_join(timer, NULL);
}

/*
Name: isEndOfProcess
Process: Checks if the metadata node marks the end of a process
Function Input/Parameters: Pointer to metadata node
Function Output/Parameters: None
Function Output/Returned: Boolean value indicating if it's the end of a process
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
bool isEndOfProcess(OpCodeType *metaPtr) 
{
   //return true if the metadata node marks the end of a process
      //function: compareString
    return compareString(metaPtr->strArg1, "end") == STR_EQ && 
           compareString(metaPtr->command, "app") == STR_EQ;
}

/*
Name: printMemInitial
Process: Prints memory initialization information
Function Input/Parameters: Pointer to configuration data type,
pointer to PCB node, lap time, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void printMemInitial(ConfigDataType *config, PCB *process, 
                                        double timer,FILE *outputFile)
{
   //initialize variables

   //if the log to == monitor
   if (config->logToCode == LOGTO_MONITOR_CODE)
   {
      //run monitor display
         //function: displayToMonitor
      displayToMonitor(config, process, MEMINTIAL, timer);
   }
   //if the log to == file
   else if (config->logToCode == LOGTO_FILE_CODE)
   {
      //run file display
         //function: displayToFile
      displayToFile(outputFile, process, config, MEMINTIAL, timer);
   }
   //if the log to == BOTH
   else if (config->logToCode == LOGTO_BOTH_CODE)
   {
      //run both file and monitor display
         //function: displayToFile, displayToMonitor
      displayToFile(outputFile, process, config, MEMINTIAL, timer);

      displayToMonitor(config, process, MEMINTIAL, timer);

   }
   else if (outputFile == NULL)
   {
      //display no output file message
         //function: printf
      printf("There is no output file detected.\n");
   }
   //otherwise
   else
   {
      //print to screen error statement
         //function: printf
      printf("Something went wrong at PRINTING MEM INITAL\n");
   }
   //return nothing
}

/*
Name: printOpCode
Process: Prints the operation code details
Function Input/Parameters: File pointer, pointer to 
configuration data type, pointer to PCB node
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void printOpCode(FILE* outputFile, ConfigDataType *config, PCB* process)
{
   //initialize variables
   char timeString[MAX_STR_LEN];
   double time = accessTimer(LAP_TIMER, timeString);

   //if the log to == monitor
   if (config->logToCode == LOGTO_MONITOR_CODE)
   {
      //run monitor display
         //function: displayToMonitor
      displayToMonitor(config, process, OPDISPLAY, time);
   }
   //if the log to == file
   else if (config->logToCode == LOGTO_FILE_CODE)
   {
      //run file display
         //function: displayToFile
      displayToFile(outputFile, process, config, OPDISPLAY, time);
   }
   //if the log to == BOTH
   else if (config->logToCode == LOGTO_BOTH_CODE)
   {
      //run both file and monitor display
         //function: displayToFile, displayToMonitor
      displayToFile(outputFile, process, config, OPDISPLAY, time);

      displayToMonitor(config, process, OPDISPLAY, time);
   }
   //if there is no output file
   else if (outputFile == NULL)
   {
      //display no output file message
         //function: printf
      printf("There is no output file detected.\n");
   }
   //otherwise
   else
   {
      //print to screen error statement
         //function: printf
      printf("Something went wrong at PRINTING IO DISPLAY\n");
   }

}

/*
Name: printReadyRunning
Process: Prints the ProcessState transition from READY_STATE to RUNNING_STATE
Function Input/Parameters: File pointer, pointer to 
configuration data type, pointer to PCB node, lap time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void printReadyRunning(FILE* outputFile, ConfigDataType *config, 
                                       PCB* process ,double time)
{
   //initialize variables

   //if the log to == monitor
   if (config->logToCode == LOGTO_MONITOR_CODE)
   {
      //run monitor display
         //function: displayToMonitor
      displayToMonitor(config, process, READYRUNNING, time);
   }
   //if the log to == file
   else if (config->logToCode == LOGTO_FILE_CODE)
   {
      //run file display
         //function: displayToFile
      displayToFile(outputFile, process, config, READYRUNNING, time);
   }
   //if the log to == BOTH
   else if (config->logToCode == LOGTO_BOTH_CODE)
   {
      //run both file and monitor display
         //function: displayToFile, displayToMonitor
      displayToFile(outputFile, process, config, READYRUNNING, time);

      displayToMonitor(config, process, READYRUNNING, time);
   }
   //if there is no output file
   else if (outputFile == NULL)
   {
      //display no output file message
         //function: printf
      printf("There is no output file detected.\n");
   }
   //otherwise
   else
   {
      //print to screen error statement
         //function: printf
      printf("Something went wrong at PRINTING MEM INITAL\n");
   }

   //return nothing
}

/*
Name: printStartSim
Process: Prints the start of the simulation
Function Input/Parameters: Pointer to configuration data type,
pointer to PCB node, lap time, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void printStartSim(ConfigDataType *config, PCB *process,
                                         double timer,FILE *outputFile)
{
 //initialize variables
   //if the log to == monitor
   if (config->logToCode == LOGTO_MONITOR_CODE)
   {
      //run monitor display
         //function: displayToMonitor
      displayToMonitor(config, process, SIMSTART, timer);
   }
   //if the log to == file
   else if (config->logToCode == LOGTO_FILE_CODE)
   {
      //run file display
         //function: displayToFile
      displayToFile(outputFile, process, config, SIMSTART, timer);
   }
   //if the log to == BOTH
   else if (config->logToCode == LOGTO_BOTH_CODE)
   {
      //run both file and monitor display
         //function: displayToFile, displayToMonitor
      displayToFile(outputFile, process, config, SIMSTART, timer);
      displayToMonitor(config, process, SIMSTART, timer);

   }
   //if there is no output file
   else if (outputFile == NULL)
   {
      //display no output file message
         //function: printf
      printf("There is no output file detected.\n");
   }
   //otherwise
   else
   {
      //print to screen error statement
         //function: printf
      printf("Something went wrong at PRINTING SIM START\n");
   }
   //return nothing
}


/*
Name: printTitle
Process: Prints the title and the start time of 
the simulation based on the configuration settings
Function Input/Parameters: Pointer to configuration data type, 
pointer to PCB node, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void printTitle(ConfigDataType *config, PCB *process, FILE *outputFile)
{
   //initialize variables
   //if the log to == monitor
   if (config->logToCode == LOGTO_MONITOR_CODE)
   {
      //run monitor display
         //function: displayToMonitor
      displayToMonitor(config, process, TITLE, NOVALUE);
   }
   //if the log to == file
   else if (config->logToCode == LOGTO_FILE_CODE)
   {
      //run file display
         //function: displayToFile
      displayToFile(outputFile, process, config, TITLE, NOVALUE);
   }
   //if the log to == BOTH
   else if (config->logToCode == LOGTO_BOTH_CODE)
   {
      //run both file and monitor display
         //function: displayToFile, displayToMonitor
      displayToFile(outputFile, process, config, TITLE, NOVALUE);
      displayToMonitor(config, process, TITLE, NOVALUE);

   }
   //if there is no output file
   else if (outputFile == NULL)
   {
      //display no output file message
         //function: printf
      printf("There is no output file detected.\n");
   }
   //otherwise
   else
   {
      //print to screen error statement
         //function: printf
      printf("Something went wrong at PRINT TITLE\n");
   }
   //return nothing
}
//  File: simulator.c
//  Project: Sim02
//  Secret ID: 708996
//  Date: 09/30/2024

#include "simulator.h"

#define MAX_MEMORY_BLOCKS 100  // Adjust size as needed
#define MEMORY_BLOCK_SIZE 1024 // Block size in KB or as needed

MemoryBlock memory[MAX_MEMORY_BLOCKS]; // Declare the memory array globally

/*
Name: runSim
Process: primary simulation driver
Function Input/Parameters: configuration data (ConfigDataType *),
                           metadata (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: createPCB_List, accessTimer, displayProcessState, 
              allocateMemory, memoryAccess, clearProcessMemory, 
              displayOpCode, displayMemoryState
*/
void runSim(ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr)
{
    // Initialize variables
    OpCodeType *localMetaPtr = metaDataMstrPtr;
    PCB *newPCBList = createPCB_List(configPtr, localMetaPtr);
    PCB *wkgPtrPCB = newPCBList;
    char timeStr[10];
    double elapsedTime = 0.0;
    FILE *file = NULL;

    // Open log file if needed
    if (configPtr->logToCode == LOGTO_FILE_CODE || configPtr->logToCode == LOGTO_BOTH_CODE)
    {
        file = fopen(configPtr->logToFileName, "w");
        if (file == NULL)
        {
            printf("Error: Unable to open log file\n");
            return;
        }
    }

    // Start simulation timer
    accessTimer(ZERO_TIMER, timeStr);
    elapsedTime = 0.000000;

    // Print title and initial simulation message
    printTitle(configPtr, file, elapsedTime);
    elapsedTime += accessTimer(LAP_TIMER, timeStr);
    fprintf(file ? file : stdout, "%1.6f, OS: Simulator start\n", elapsedTime);

    // Initialize all processes to READY state
    while (wkgPtrPCB != NULL)
    {
        wkgPtrPCB->currentState = READY_STATE;
        displayProcessState(configPtr, wkgPtrPCB, elapsedTime, file);
        wkgPtrPCB = wkgPtrPCB->nextNode;
    }

    // Log initial memory state
    displayMemoryState(configPtr, file, elapsedTime, "After memory initialization");

    // Reset working PCB pointer to the start of the list
    wkgPtrPCB = newPCBList;

    // Process loop
    while (wkgPtrPCB != NULL)
    {
        // Set process to RUNNING and log state
        wkgPtrPCB->currentState = RUNNING_STATE;
        displayProcessState(configPtr, wkgPtrPCB, elapsedTime, file);

        // Process operations
        OpCodeType *opCode = wkgPtrPCB->mdPtr;
        while (opCode != NULL)
        {
            // Log each operation's start
            if (strcmp(opCode->command, "A") == 0 && strcmp(opCode->strArg1, "allocate") == 0)
            {
                // Attempt memory allocation
                if (allocateMemory(wkgPtrPCB->pid, opCode->intArg2, opCode->intArg3))
                {
                    displayMemoryState(configPtr, file, elapsedTime, "After allocate success");
                }
                else
                {
                    displayMemoryState(configPtr, file, elapsedTime, "After allocate failure");
                    wkgPtrPCB->currentState = EXIT_STATE;
                    displayProcessState(configPtr, wkgPtrPCB, elapsedTime, file);
                    clearProcessMemory(wkgPtrPCB->pid);
                    break;
                }
            }
            else if (strcmp(opCode->command, "A") == 0 && strcmp(opCode->strArg1, "access") == 0)
            {
                // Attempt memory access
                if (memoryAccess(wkgPtrPCB->pid, opCode->intArg2, opCode->intArg3))
                {
                    displayMemoryState(configPtr, file, elapsedTime, "After access success");
                }
                else
                {
                    displayMemoryState(configPtr, file, elapsedTime, "After access failure");
                    wkgPtrPCB->currentState = EXIT_STATE;
                    displayProcessState(configPtr, wkgPtrPCB, elapsedTime, file);
                    clearProcessMemory(wkgPtrPCB->pid);
                    break;
                }
            }
            else
            {
                // Log other operations
                displayOpCode(configPtr, opCode, wkgPtrPCB, file, &elapsedTime);
            }

            // Log operation end and move to next
            elapsedTime += accessTimer(LAP_TIMER, timeStr);
            opCode = opCode->nextNode;
        }

        // Check if process exited prematurely due to memory issue
        if (wkgPtrPCB->currentState != EXIT_STATE)
        {
            // Log normal process exit
            wkgPtrPCB->currentState = EXIT_STATE;
            displayProcessState(configPtr, wkgPtrPCB, elapsedTime, file);
            clearProcessMemory(wkgPtrPCB->pid);
            displayMemoryState(configPtr, file, elapsedTime, "After clear process success");
        }

        // Move to next process
        wkgPtrPCB = wkgPtrPCB->nextNode;
    }

    // Log simulation stop and memory clearing
    elapsedTime += accessTimer(LAP_TIMER, timeStr);
    fprintf(file ? file : stdout, "%1.6f, OS: System stop\n", elapsedTime);
    displayMemoryState(configPtr, file, elapsedTime, "After clear all process success");
    fprintf(file ? file : stdout, "%1.6f, OS: Simulation end\n", elapsedTime);

    // Final cleanup and close file if needed
    if (file != NULL)
    {
        fclose(file);
    }

    // Stop the timer
    accessTimer(STOP_TIMER, timeStr);
    printf("\nSimulator Program End.\n\n");
}

/*
Name: createNewNode
Process: Allocates memory for a new PCB node and initializes its
nextNode pointer to NULL
Function Input/Parameters: pointer to an existing PCB node
Function Output/Parameters: Newly created PCB node pointer
Function Output/Returned: Newly created PCB node pointer
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
PCB *createNewNode(PCB *pointer)
{
	//initialize variables

	//compute data
	//allocate memory
	pointer->nextNode = (PCB *)malloc(sizeof(PCB));
	//move to next pointer
	pointer = pointer->nextNode;
	//set next node to null
	pointer->nextNode = NULL;

	//return pointer
	return pointer;
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
Name: displayOpCode
Process: Displays the operation codes based on the configuration
settings and updates the process state
Function Input/Parameters: Configuration pointer, Metadata pointer,
Process pointer, File pointer, Time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: getOpCode, displayState functions
*/
void displayOpCode(ConfigDataType *configPtr, OpCodeType *metaData, PCB *process, FILE *file, double *elapsedTime)
{
	double operationTime = 0.0;  // Time for each operation

	// Loop through each operation in the metadata for the current process
	while (compareString(metaData->strArg1, "end") != STR_EQ)
	{
		// Determine the type of operation (CPU or I/O)
		if (compareString(metaData->command, "cpu") == STR_EQ)
		{
			// CPU process operation: adjust timing according to CPU cycle rate
			operationTime = (metaData->intArg2 * configPtr->procCycleRate) / 1000.0;

			// Log CPU process operation start
			if (configPtr->logToCode != LOGTO_FILE_CODE)
			{
				printf("%1.6f, Process: %d, cpu process operation start\n", *elapsedTime, process->pid);
			}
			
			if (file != NULL)
			{
				fprintf(file, "%1.6f, Process: %d, cpu process operation start\n", *elapsedTime, process->pid);
			}
		}
		else if (compareString(metaData->inOutArg, "in") == STR_EQ)
		{
			// Input device operation: adjust timing according to IO cycle rate
			operationTime = (metaData->intArg2 * configPtr->ioCycleRate) / 1000.0;

			// Log input device operation start
			if (configPtr->logToCode != LOGTO_FILE_CODE)
			{
				printf("%1.6f, Process: %d, %s input operation start\n", *elapsedTime, process->pid, metaData->strArg1);
			}

			if (file != NULL)
			{
				fprintf(file, "%1.6f, Process: %d, %s input operation start\n", *elapsedTime, process->pid, metaData->strArg1);
			}
		}
		else if (compareString(metaData->inOutArg, "out") == STR_EQ)
		{
			// Output device operation: adjust timing according to IO cycle rate
			operationTime = (metaData->intArg2 * configPtr->ioCycleRate) / 1000.0;

			// Log output device operation start
			if (configPtr->logToCode != LOGTO_FILE_CODE)
			{
				printf("%1.6f, Process: %d, %s output operation start\n", *elapsedTime, process->pid, metaData->strArg1);
			}
			
			if (file != NULL)
			{
				fprintf(file, "%1.6f, Process: %d, %s output operation start\n", *elapsedTime, process->pid, metaData->strArg1);
			}
		}

		// Update elapsed time for operation duration
		*elapsedTime += operationTime;

		// Log end of operation
		if (compareString(metaData->command, "cpu") == STR_EQ)
		{
			if (configPtr->logToCode != LOGTO_FILE_CODE)
			{
				printf("%1.6f, Process: %d, cpu process operation end\n", *elapsedTime, process->pid);
			}

			if (file != NULL)
			{
				fprintf(file, "%1.6f, Process: %d, cpu process operation end\n", *elapsedTime, process->pid);
			}
		}
		else if (compareString(metaData->inOutArg, "in") == STR_EQ)
		{
			if (configPtr->logToCode != LOGTO_FILE_CODE)
			{
				printf("%1.6f, Process: %d, %s input operation end\n", *elapsedTime, process->pid, metaData->strArg1);
			}

			if (file != NULL)
			{
				fprintf(file, "%1.6f, Process: %d, %s input operation end\n", *elapsedTime, process->pid, metaData->strArg1);
			}
		}
		else if (compareString(metaData->inOutArg, "out") == STR_EQ)
		{
			if (configPtr->logToCode != LOGTO_FILE_CODE)
			{
				printf("%1.6f, Process: %d, %s output operation end\n", *elapsedTime, process->pid, metaData->strArg1);
			}
			
			if (file != NULL)
			{
				fprintf(file, "%1.6f, Process: %d, %s output operation end\n", *elapsedTime, process->pid, metaData->strArg1);
			}
		}

		// Move to the next operation in the metadata
		metaData = metaData->nextNode;
	}

	// After processing all operations, set the process state to EXIT
	process->currentState = EXIT_STATE;
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

void initializeMemory(int totalMemory) {
    // Initialize memory blocks as open, with totalMemory defining the size.
    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        memory[i].base = i * MEMORY_BLOCK_SIZE;
        memory[i].offset = MEMORY_BLOCK_SIZE;
        memory[i].processID = -1;  // No process assigned
        memory[i].isAllocated = false;
    }
    // Print initial memory layout
    printMemoryState();
}

bool allocateMemory(int processID, int base, int offset) {
    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        if (!memory[i].isAllocated && memory[i].base == base && memory[i].offset >= offset) {
            memory[i].isAllocated = true;
            memory[i].processID = processID;
            printMemoryState();
            return true;
        }
    }
    printf("Allocation failed for Process %d\n", processID);
    return false;
}

bool memoryAccess(int processID, int base, int offset) {
    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        if (memory[i].isAllocated && memory[i].processID == processID &&
            memory[i].base <= base && (memory[i].base + memory[i].offset) >= (base + offset)) {
            printMemoryState();
            return true;
        }
    }
    printf("Segmentation fault, Process %d\n", processID);
    return false;
}

void clearProcessMemory(int processID) {
    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        if (memory[i].processID == processID) {
            memory[i].isAllocated = false;
            memory[i].processID = -1;
        }
    }
    printMemoryState();
}

void printMemoryState() {
    printf("--------------------------------------------------\n");
    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        printf("%d [ %s, P#: %d, %d-%d ] %d\n", i,
               memory[i].isAllocated ? "Used" : "Open",
               memory[i].processID,
               memory[i].base,
               memory[i].base + memory[i].offset - 1,
               MEMORY_BLOCK_SIZE);
    }
    printf("--------------------------------------------------\n");
}

void displayMemoryState(ConfigDataType *configPtr, FILE *file, double elapsedTime, const char *message) {
    printf("%1.6f, %s\n", elapsedTime, message);
    if (file != NULL) {
        fprintf(file, "%1.6f, %s\n", elapsedTime, message);
    }
}

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
Dependencies: tbd
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

    // Store a constant timestamp for the READY state transitions
    double readyTimestamp = elapsedTime;

    // Loop through the PCB list and set to READY state, using the same timestamp for all processes
    while (wkgPtrPCB != NULL)
    {
        wkgPtrPCB->currentState = READY_STATE;

        // Log the state transition for each process with the same timestamp
        displayProcessState(configPtr, wkgPtrPCB, readyTimestamp, file);

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
        printf("%1.6f, OS: Process %d selected with %d ms remaining\n", elapsedTime, wkgPtrPCB->pid, wkgPtrPCB->time);
        printf("%1.6f, OS: Process %d set from READY to RUNNING\n", elapsedTime, wkgPtrPCB->pid);

        if (file != NULL)
        {
            fprintf(file, "%1.6f, OS: Process %d selected with %d ms remaining\n", elapsedTime, wkgPtrPCB->pid, wkgPtrPCB->time);
            fprintf(file, "%1.6f, OS: Process %d set from READY to RUNNING\n", elapsedTime, wkgPtrPCB->pid);
        }

        // Handle process operations (displayOpCode also updates the elapsed time)
        displayOpCode(configPtr, wkgPtrPCB->mdPtr, wkgPtrPCB, file, &elapsedTime);

        // Print the process exit status
        wkgPtrPCB->currentState = EXIT_STATE;
        displayProcessState(configPtr, wkgPtrPCB, elapsedTime, file);  // Log EXIT state

        // Move to the next process
        wkgPtrPCB = wkgPtrPCB->nextNode;
    }

    // Print the system stop message
    elapsedTime += accessTimer(LAP_TIMER, timeStr);  // Final time accumulation
    printf("%1.6f, OS: System stop\n", elapsedTime);

    if (file != NULL)
    {
        fprintf(file, "%1.6f, OS: System stop\n", elapsedTime);
        fflush(file);
        fclose(file);
    }

    // Stop the timer
    accessTimer(STOP_TIMER, timeStr);
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
            // Calculate time for CPU operation (based on cycles and procCycleRate)
            operationTime = (metaData->intArg2 * configPtr->procCycleRate) / 1000.0;
        }
        else if (compareString(metaData->command, "dev") == STR_EQ)
        {
            // Calculate time for I/O operation (based on cycles and ioCycleRate)
            operationTime = (metaData->intArg2 * configPtr->ioCycleRate) / 1000.0;
        }

        // Add the operation time to the total elapsed time
        *elapsedTime += operationTime;

        // Log start of operation
        if (compareString(metaData->command, "cpu") == STR_EQ)
        {
            printf("  %1.6f, Process: %d, cpu process operation start\n", *elapsedTime, process->pid);
            if (file != NULL)
            {
                fprintf(file, "  %1.6f, Process: %d, cpu process operation start\n", *elapsedTime, process->pid);
            }
        }
        else if (compareString(metaData->strArg1, "input") == STR_EQ)
        {
            printf("  %1.6f, Process: %d, %s %s operation start\n", *elapsedTime, process->pid, metaData->strArg1, metaData->command);
            if (file != NULL)
            {
                fprintf(file, "  %1.6f, Process: %d, %s %s operation start\n", *elapsedTime, process->pid, metaData->strArg1, metaData->command);
            }
        }
        else if (compareString(metaData->strArg1, "output") == STR_EQ)
        {
            printf("  %1.6f, Process: %d, %s %s operation start\n", *elapsedTime, process->pid, metaData->strArg1, metaData->command);
            if (file != NULL)
            {
                fprintf(file, "  %1.6f, Process: %d, %s %s operation start\n", *elapsedTime, process->pid, metaData->strArg1, metaData->command);
            }
        }

        // Simulate end of operation based on the operation time added
        *elapsedTime += operationTime;

        // Log end of operation
        if (compareString(metaData->command, "cpu") == STR_EQ)
        {
            printf("  %1.6f, Process: %d, cpu process operation end\n", *elapsedTime, process->pid);
            if (file != NULL)
            {
                fprintf(file, "  %1.6f, Process: %d, cpu process operation end\n", *elapsedTime, process->pid);
            }
        }
        else if (compareString(metaData->strArg1, "input") == STR_EQ)
        {
            printf("  %1.6f, Process: %d, %s %s operation end\n", *elapsedTime, process->pid, metaData->strArg1, metaData->command);
            if (file != NULL)
            {
                fprintf(file, "  %1.6f, Process: %d, %s %s operation end\n", *elapsedTime, process->pid, metaData->strArg1, metaData->command);
            }
        }
        else if (compareString(metaData->strArg1, "output") == STR_EQ)
        {
            printf("  %1.6f, Process: %d, %s %s operation end\n", *elapsedTime, process->pid, metaData->strArg1, metaData->command);
            if (file != NULL)
            {
                fprintf(file, "  %1.6f, Process: %d, %s %s operation end\n", *elapsedTime, process->pid, metaData->strArg1, metaData->command);
            }
        }

        // Move to the next operation in the metadata
        metaData = metaData->nextNode;
    }

    // After processing all operations, set the process state to EXIT
    process->currentState = EXIT_STATE;

    // Display the process exit status
    printf("  %1.6f, OS: Process %d ended\n", *elapsedTime, process->pid);
    if (file != NULL)
    {
        fprintf(file, "  %1.6f, OS: Process %d ended\n", *elapsedTime, process->pid);
    }
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
            break;

       //EXITING
        case EXIT_STATE:
            //print exiting statment
               //function: printf
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
            break;

       //Exiting
        case EXIT_STATE:
            //print EXITING statment to file
            //function: fprintf
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
Name: getOpCode
Process: Retrieves and displays the operation codes from 
metadata based on the configuration settings
Function Input/Parameters: Metadata pointer, Time stamp,
 Process ID, Configuration pointer, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: None
void getOpCode(OpCodeType *metaDataPtr, double timeStamp, int pid, ConfigDataType *config, FILE *file)
{
    // Initialize variables
    metaDataPtr = metaDataPtr->nextNode;

    // Compute data

    // Loop through until we get to the correct portions of data
    while (metaDataPtr->pid != pid)
    {
        // Move to the next node until we are at the right place
        metaDataPtr = metaDataPtr->nextNode;
    }

    // If we are at the right place
    if (metaDataPtr->pid == pid)
    {
        // Loop through the ops
        while (compareString(metaDataPtr->strArg1, "end") != STR_EQ)
        {
            // Check logToCode configuration
            if (config->logToCode == LOGTO_MONITOR_CODE || config->logToCode == LOGTO_BOTH_CODE)
            {
                // Display the ops to the monitor
                printf("%1.6f, Process: %d, %s %s operation start\n", timeStamp, pid, metaDataPtr->strArg1, metaDataPtr->command);

                // Display the end of the ops to the monitor
                printf("%1.6f, Process: %d, %s %s operation end\n", timeStamp, pid, metaDataPtr->strArg1, metaDataPtr->command);
            }

            // Check logToCode configuration
            if (config->logToCode == LOGTO_FILE_CODE || config->logToCode == LOGTO_BOTH_CODE)
            {
                // Display the ops to the file
                fprintf(file, "%1.6f, Process: %d, %s %s operation start\n", timeStamp, pid, metaDataPtr->strArg1, metaDataPtr->command);

                // Display the end of the ops to the file
                fprintf(file, "%1.6f, Process: %d, %s %s operation end\n", timeStamp, pid, metaDataPtr->strArg1, metaDataPtr->command);
            }

            // Go to the next MetaData
            metaDataPtr = metaDataPtr->nextNode;
        }
    }

    // Print another line for beauty to both monitor and file
    printf("\n");
    fprintf(file, "\n");
}
*/

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
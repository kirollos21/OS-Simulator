// header files
#include "simulator.h"


/*
Name: calculateRemainingTime
Process: Calculates the remaining time for a PCB based 
on configuration and metadata
Function Input/Parameters: Pointer to PCB node, pointer 
to configuration data type
Function Output/Parameters: None
Function Output/Returned: Remaining time for the PCB
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
double calculateRemainingTime(PCB *pcbNode, ConfigDataType *configPtr) 
{
   //initialize variables
    double remainingTime = 0.0;
    int totIO = 0;
    int totProc = 0;
    int totMem = 0;
    OpCodeType *metaPtr = pcbNode->mdPtr;

    // Traverse the metadata nodes for the current process
       //function: isEndOfProcess
    while (metaPtr != NULL && !isEndOfProcess(metaPtr)) 
    {
       // check for I/O process
         //function: compareString
       if( compareString(pcbNode->mdPtr->command, "dev") == STR_EQ )
         {
         // add current task time in meta-data to total for I/O
         totIO += metaPtr->intArg2;
         }
      
      // otherwise task is processing
         //function: compareString
      if( compareString(pcbNode->mdPtr->command, "cpu") == STR_EQ )
         {
         // ad current task time in meta-data to total for processes
         totProc += metaPtr->intArg2;
         }
      // otherwise task is memory
      else 
         {
            totMem += metaPtr->intArg2;
         }  
      // find estimated time for given process
      remainingTime = (configPtr->procCycleRate * totProc) + 
                        (configPtr->ioCycleRate * totIO) + totMem;

        // Move to the next metadata node
        metaPtr = metaPtr->nextNode;
    }
    //return the remaining time
    return remainingTime;
}


/*
Name: createNewPCBNode
Process: Creates a new PCB node with specified process ID 
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
    // Allocate memory for the new PCB node
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
    newNode->remainingtime = 0.0;
    newNode->currentState = NEW;
    newNode->nextNode = NULL;
    //return new node
    return newNode;
}

/*
Name: createPCB_List
Process: Creates a PCB linked list based on configuration and metadata
Function Input/Parameters: Pointer to configuration data type,
 pointer to metadata, pointer to total process counter
Function Output/Parameters: None
Function Output/Returned: Head pointer of the created PCB linked list
Device Input/File: None
Device Output/Device: None
Dependencies: createNewPCBNode, calculateRemainingTime, isEndOfProcess
*/
PCB *createPCB_List(ConfigDataType *configPtr, OpCodeType *metaData,
                                                         int *totProc)
{
    // Initialize variables
    OpCodeType *metaPtr = metaData;
    int pid = 0;
    PCB *headPtrProcess = NULL;
    PCB *wkgPtrProcess = NULL;

    // Create PCB nodes
    while (metaPtr != NULL) 
    {
        // Check for app start
           //function: compareString
        if (compareString(metaPtr->strArg1, "start") == STR_EQ && 
            compareString(metaPtr->command, "app") == STR_EQ) 
        {
            // Create new node
               //funciton: createNewPCBNode
            PCB *newNode = createNewPCBNode(pid, metaPtr);
            // check if memory allocation failed
            if (headPtrProcess == NULL) 
            {
               //set head pointer to new node
                headPtrProcess = newNode;
                //set working pointer to new node
                wkgPtrProcess = headPtrProcess;

            } 
            //otherwise
            else
            {
               //set next node to new node
                wkgPtrProcess->nextNode = newNode;
               //set working pointer to new node
                wkgPtrProcess = newNode;
            }

            // Set PCB attributes
               //function: calculateRemainingTime
            wkgPtrProcess->remainingtime = calculateRemainingTime(
                                             wkgPtrProcess, configPtr);
            //increment pid
            *totProc = pid;
            // Advance the pid
            pid++;
        }
        // Move to the next metadata node
        metaPtr = metaPtr->nextNode;
    }
    // Return the head pointer of the created PCB linked list
    return headPtrProcess;
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
Name: displayProcessState
Process: Displays the current state of the process based on the 
configuration settings
Function Input/Parameters: Pointer to configuration data type, 
pointer to PCB node, File pointer
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: displayToMonitor, displayToFile
*/
void displayProcessState(ConfigDataType *config,PCB *process, FILE *outputFile)
{
   //initlize function
   double time = accessTimer(LAP_TIMER, NULL);

   //compute data
      //loop through all the processes and display
      while(process != NULL)
      {
         //if the log to == monitor
         if (config->logToCode == LOGTO_MONITOR_CODE)
         {
            //run monitor display
               //function: displayToMonitor
            displayToMonitor(config, process, NEWTOREADY, time);
         }
         //if the log to == file
         else if (config->logToCode == LOGTO_FILE_CODE)
         {
            //run file display
               //function: displayToFile
            displayToFile(outputFile, process, config, NEWTOREADY, time);
         }
         //if the log to == BOTH
         else if (config->logToCode == LOGTO_BOTH_CODE)
         {
            //run both file and monitor display
               //function: displayToFile, displayToMonitor
            displayToFile(outputFile, process, config, NEWTOREADY, time) ;
            displayToMonitor(config, process, NEWTOREADY, time);
         
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
            printf("Something went wrong at PRINTING NEW TO READY\n");
         }
         //move to next process
         process = process->nextNode;
      }
   //return nothing
}

/*
Name: displayToMonitor
Process: Displays the current state of the process to the monitor
Function Input/Parameters: Pointer to configuration data type, 
pointer to PCB node, display type, lap time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: Monitor
Dependencies: None
*/
void displayToMonitor(ConfigDataType *config, PCB *process,
                                   int displayType, double time)
{
   //display depending on the display type
   switch (displayType)
   {
      //if the display type is title
      case TITLE:
         //print title
            //function: printf
         printf("Simulator Run\n-------------\n\n");
         break;
      //if the display type is sim start
      case SIMSTART:
         //print simulator start
            //function: printf
         printf( "%1.6f, OS: Simulator start\n", time);
         break;
      //if the display type is new to ready
      case NEWTOREADY:
         //print new to ready
            //function: printf
         printf("%1.6f, OS: Process %d set to READY state from NEW state\n",
                                                         time, process->pid);
         process->currentState = READY;
         break;
      //if the display type is memory initialization
      case MEMINTIAL:
         //print memory initialization
            //function: printf
         printf("--------------------------------------------------\n");
         printf("After memory initialization\n");
         printf("0 [ Open, P#: x, 0-0 ] %d\n", process->pid);
         printf("--------------------------------------------------\n\n");
         break;
      //if the display type is ready to running
      case READYRUNNING:
         //print ready to running
            //function: printf
         printf("%1.6f, OS: Process %d selected with %d ms remaining\n",
                             time, process->pid, process->remainingtime );
         printf("%1.6f, OS: Process %d set from READY to RUNNING\n\n",
                                                      time, process->pid);
         break;
      //if the display type is operation display
      case OPDISPLAY:
         //if the process is input
         if (compareString(process->mdPtr->inOutArg,"in")==STR_EQ)
         {
            //print input operation
               //function: printf
            printf("%1.6f, Process: %d, %s input operation start\n", 
                                 time, process->pid, process->mdPtr->strArg1);
            printf("%1.6f, Process: %d, %s input operation end\n", 
                                 time, process->pid, process->mdPtr->strArg1);
         }
         //if the process is output
         else if(compareString(process->mdPtr->inOutArg, "out")==STR_EQ)
         {
            //print output operation
               //function: printf
            printf("%1.6f, Process: %d, %s output operation start\n", 
                                 time, process->pid, process->mdPtr->strArg1);
            printf("%1.6f, Process: %d, %s output operation end\n", 
                                 time, process->pid, process->mdPtr->strArg1);
         }     
         //if the process is cpu
         else if(compareString(process->mdPtr->command, "cpu")==STR_EQ)
         {
            //print cpu operation
               //function: printf
            printf("%1.6f, Process: %d, %s %s operation start\n", 
         time, process->pid, process->mdPtr->command, process->mdPtr->strArg1);
            printf("%1.6f, Process: %d, %s %s operation end\n",
         time, process->pid, process->mdPtr->command, process->mdPtr->strArg1);
         }
         break;
      //default
      default:
         break;
      }
   //return nothing
}


/*
Name: displayToFile
Process: Displays the current state of the process to the file
Function Input/Parameters: File pointer, pointer to PCB node, 
pointer to configuration data type, display type, lap time
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: File
Dependencies: None
*/
void displayToFile(FILE *outputFile, PCB *process, ConfigDataType *config,
                                               int displayType, double time)
{
   //display depending on the display type
   switch (displayType)
   {
      //if the display type is title
      case TITLE:
         //print title
            //function: fprintf
         fprintf(outputFile,"Simulator Run\n-------------\n\n");
         break;
      //if the display type is sim start
      case SIMSTART:
         //print simulator start
            //function: fprintf
         fprintf(outputFile, "%1.6f, OS: Simulator start\n", time);
         break;
      //if the display type is new to ready
      case NEWTOREADY:
         //print new to ready
            //function: fprintf
         fprintf(outputFile, 
          "%1.6f, OS: Process %d set to READY state from NEW state\n",
                                                    time, process->pid);
         break;
      //if the display type is memory initialization
      case MEMINTIAL:
         //print memory initialization
            //function: fprintf
         fprintf(outputFile,
                "--------------------------------------------------\n");
         fprintf(outputFile,"After memory initialization\n");
         fprintf(outputFile,"0 [ Open, P#: x, 0-0 ] %d\n", 
                                                  config->memAvailable);
         fprintf(outputFile,
              "--------------------------------------------------\n\n");
         break;
      //if the display type is ready to running
      case READYRUNNING:
         //print ready to running
            //function: fprintf
         fprintf(outputFile,
          "%1.6f, OS: Process %d selected with %d ms remaining\n",
                            time, process->pid, process->remainingtime);
         fprintf(outputFile, 
         "%1.6f, OS: Process %d set from READY to RUNNING\n\n",
                                                   time, process->pid);
         break;
      //if the display type is operation display
      case OPDISPLAY:
         //if the process is input
            //function: compareString
         if (compareString(process->mdPtr->inOutArg,"in")==STR_EQ)
         {
            //print input operation
               //function: fprintf
            fprintf(outputFile,
            "%1.6f, Process: %d, %s input operation start\n",
                       time, process->pid, process->mdPtr->strArg1);
            fprintf(outputFile,
            "%1.6f, Process: %d, %s input operation end\n",
                         time, process->pid, process->mdPtr->strArg1);
         }
         //if the process is output
            //function: compareString
         else if(compareString(process->mdPtr->inOutArg, "out")==STR_EQ)
         {
            //print output operation
               //function: fprintf
            fprintf(outputFile,
            "%1.6f, Process: %d, %s output operation start\n",
                       time, process->pid, process->mdPtr->strArg1);
            fprintf(outputFile,
            "%1.6f, Process: %d, %s output operation end\n",
                         time, process->pid, process->mdPtr->strArg1);
         }
         //if the process is cpu
            //function: compareString
         else if(compareString(process->mdPtr->command, "cpu")==STR_EQ)
         {
            //print cpu operation
               //function: fprintf
            fprintf(outputFile,
              "%1.6f, Process: %d, %s %s operation start\n",
               time, process->pid, process->mdPtr->command, 
               process->mdPtr->strArg1);
            fprintf(outputFile, 
               "%1.6f, Process: %d, %s %s operation end\n",
                time, process->pid, process->mdPtr->command,
                process->mdPtr->strArg1);
               
         }
         break;
      //default
      default:
         break;
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
Name: getNextProcess
Process: Retrieves the next process in the linked list based on the current
process and metadata
Function Input/Parameters: CPU scheduler type, pointer to current process,
head pointer of the process list
Function Output/Parameters: None
Function Output/Returned: Next process pointer
Device Input/File: None
Device Output/Device: None
Dependencies: None
*/
PCB *getNextProcess(int cpuScheduler, PCB *processPtr)
{
    // Declare/initialize variables
    int shortestJobTime = processPtr->remainingtime + 1;
    PCB *wkgPtr = processPtr;
    PCB *selectedProcess = (PCB *)malloc(sizeof(PCB));

    // If the CPU scheduler is FCFS
    if (cpuScheduler == CPU_SCHED_FCFS_N_CODE)
    {
        // Get the next process
        selectedProcess = wkgPtr->nextNode;
    }
    // If the CPU scheduler is SJF
    else if (cpuScheduler == CPU_SCHED_SJF_N_CODE)
    {
         // if the current stat is exiting
         while (wkgPtr->currentState == EXITING)
         {
            if (wkgPtr->nextNode == NULL)
            {
               printf("WORKING..\n");
               wkgPtr = processPtr;
            }
            //move to the next process
            wkgPtr = wkgPtr->nextNode;
            //set the selected process to wkgPtr
            selectedProcess = wkgPtr;
            //set the shortest job time to the remaining time
            shortestJobTime = wkgPtr->remainingtime;
         }
        
        // Loop through the processes
        while (wkgPtr != NULL)
        {
            // If the other node has less remaining time
            if (wkgPtr->currentState == READY && 
                             wkgPtr->remainingtime < shortestJobTime)
            {
                //Update the shortest job time and the next process pointer
                shortestJobTime = wkgPtr->remainingtime;
                
                //set the selected process to wkgPtr
                selectedProcess = wkgPtr;
            }
            // Move to the next process
            wkgPtr = wkgPtr->nextNode;
        }
        
      }
     // Return the next Process
    return selectedProcess;
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
Process: Prints the state transition from READY to RUNNING
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


/*
Name: runSim
Process: Runs the simulation based on the provided configuration
and metadata
Function Input/Parameters: Pointer to configuration data type, 
pointer to metadata
Function Output/Parameters: None
Function Output/Returned: None
Device Input/File: None
Device Output/Device: None
Dependencies: createPCB_List, displayState, getNextProcess,
displayOpCode, printTitle
*/
void runSim(ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr)
{
    // Initialize variables
    double elapsedTime = 0.0;
    int processCounter = 0;
    int totProc = 0;
    int atBeginning = 0;
    OpCodeType *localMetaPtr = metaDataMstrPtr;
    PCB *newPCBList = createPCB_List(configPtr, localMetaPtr, &totProc);
    PCB *wkgPCBPtr = newPCBList;
    FILE *outputFile = NULL;

    // Open log file if needed
    if (configPtr->logToCode == LOGTO_FILE_CODE || configPtr->logToCode == LOGTO_BOTH_CODE)
    {
        outputFile = fopen(configPtr->logToFileName, "w");
        if (outputFile == NULL)
        {
            printf("Error: Unable to open log file\n");
            return;
        }
    }

    // Log initial state and print title
    printTitle(configPtr, wkgPCBPtr, outputFile);

    // Start the simulation timer and set the initial timestamp
    elapsedTime = accessTimer(ZERO_TIMER, NULL);
    printStartSim(configPtr, wkgPCBPtr, elapsedTime, outputFile);
    wkgPCBPtr->currentState = READY;
    displayProcessState(configPtr, wkgPCBPtr, outputFile);
    printMemInitial(configPtr, wkgPCBPtr, elapsedTime, outputFile);

    // MASTER LOOP
    while (processCounter <= totProc || compareString(wkgPCBPtr->mdPtr->strArg1, "end") != STR_EQ)
    {
        // Get the next process based on CPU scheduling type
        if (configPtr->cpuSchedCode == CPU_SCHED_SJF_N_CODE && processCounter == atBeginning)
        {
            wkgPCBPtr = getNextProcess(configPtr->cpuSchedCode, wkgPCBPtr);
        }
        else if (processCounter != atBeginning)
        {
            wkgPCBPtr = getNextProcess(configPtr->cpuSchedCode, newPCBList);
        }

        // Increment process counter and set process state
        processCounter++;
        wkgPCBPtr->currentState = RUNNING;

        // Update and log elapsed time for process selection and transition to RUNNING
        elapsedTime += accessTimer(LAP_TIMER, NULL);
        printReadyRunning(outputFile, configPtr, wkgPCBPtr, elapsedTime);

        // Process each operation
        while (compareString(wkgPCBPtr->mdPtr->command, "app") != STR_EQ || compareString(wkgPCBPtr->mdPtr->strArg1, "end") != STR_EQ)
        {
            // Update elapsed time before each operation
            elapsedTime += accessTimer(LAP_TIMER, NULL);

            // Log I/O operations and handle memory if needed
            printOpCode(outputFile, configPtr, wkgPCBPtr);
            if (compareString(wkgPCBPtr->mdPtr->command, "mem") == STR_EQ)
            {
                displayMem(wkgPCBPtr, configPtr, outputFile);
            }

            // Move to the next operation node
            wkgPCBPtr->mdPtr = wkgPCBPtr->mdPtr->nextNode;
        }

        // Set the process to EXITING state and log final elapsed time for process
        wkgPCBPtr->currentState = EXITING;
        elapsedTime += accessTimer(LAP_TIMER, NULL);
        displayProcessState(configPtr, wkgPCBPtr, outputFile);

        // End of MASTER LOOP
    }

    // Log the end of the simulation
    elapsedTime += accessTimer(LAP_TIMER, NULL);
    if (configPtr->logToCode == LOGTO_FILE_CODE || configPtr->logToCode == LOGTO_BOTH_CODE)
    {
        fprintf(outputFile, "%1.6f, OS: Simulator End\n", elapsedTime);
        fclose(outputFile);
    }
    else
    {
        printf("%1.6f, OS: Simulator End\n", elapsedTime);
    }
}
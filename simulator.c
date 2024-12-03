#include "simulator.h"

/*
Name: runSim
Process: primary simulation driver
Function Input/Parameters: configuration data (ConfigDataType *),
                           metadata (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: tbd
*/
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr )
   {
    // initialize variables
       // mark for start end or processing
       int dispFlag = START;

       // holder for log code
       int logCode = configPtr->logToCode;
       
       // temp pointer for metadata looping
       OpCodeType *metaDataHolder = metaDataMstrPtr;
        
       // set PCBhead for data storage to NULL
       PCBType *PCBHead = startPCB( metaDataHolder, configPtr );
        
       // set temp PCB head for looping to NULL
       PCBType *PCBHeadHolder = NULL;
        
       // Variable for logto file
       FILE *logFile = NULL;
    
       // Variable for logging to file at end
       logToFile *fileHolder = (logToFile *)malloc(sizeof(logToFile));
        
        
    if(logCode == LOGTO_BOTH_CODE || logCode == LOGTO_FILE_CODE)
       {
        logFile = fopen( configPtr->logToFileName, "w" );
       }

      // variable that holds timer data
      char timer[STD_STR_LEN];
      char fileLine[STD_STR_LEN];
    
    
   // initialize display
      // function: displayPCB
   displayPCB( PCBHead, logFile, dispFlag, timer, fileHolder, configPtr, 
               usedMem );

   // initialize memory 
      // function: memSim
   memSim(NULL, logCode, dispFlag, fileHolder, START, configPtr, usedMem);
    
   // set all PCBs to ready
      // function: setState
   setState( PCBHead, READY_STATE, logCode, fileHolder, logFile );

    // set display flag to run state
    dispFlag = RUN;
    
    // Start threads for each PCB (process)
    while (PCBHeadHolder != NULL) 
       {
        pthread_create(&(PCBHeadHolder->PID), NULL, 
                    simulateProcess, (void *)PCBHeadHolder );
        PCBHeadHolder = PCBHeadHolder->nextPCB;
       }

    // Wait for all threads to finish
    PCBHeadHolder = PCBHead;
    while (PCBHeadHolder != NULL) 
       {
        pthread_join(PCBHeadHolder->PID, NULL);
        PCBHeadHolder = PCBHeadHolder->nextPCB;
       }

    // Finish simulation 
       // function: accessTimer
    accessTimer(STOP_TIMER, timer);
    snprintf(fileLine, sizeof(fileLine), "%.8s, OS: Simulation End\n", timer);
    if (logCode == LOGTO_MONITOR_CODE || logCode == LOGTO_BOTH_CODE) 
       {
        printf("%s", fileLine);
       }
    if (logCode == LOGTO_FILE_CODE || logCode == LOGTO_BOTH_CODE)
       {
        fileHolder = addLine(fileHolder, fileLine);
        dumpFile(fileHolder, logFile);
       }
   }

/*
Name: simulateProcess
Process: simulates threading processes
Function Input/Parameters: configuration data (ConfigDataType *),
                           metadata (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: accessTimer, snprintf, printf, pthread_exit
*/
void *simulateProcess(void *arg) 
{
    PCBType *pcb = (PCBType *)arg;
    char timer[STD_STR_LEN];
    char fileLine[256];

    // Access the timer for this process
    accessTimer(LAP_TIMER, timer);

    // Simulate the process (this is a placeholder for your simulation logic)
    printf("%s, OS: Process: %lu is simulating...\n", timer, pcb->PID);

    // Simulate process task completion
    accessTimer(LAP_TIMER, timer);
    snprintf(fileLine, sizeof(fileLine), "%.8s, OS: Process: %lu finished execution\n", timer, pcb->PID);

    // You can log this to a file or console depending on your config
    // For now, we'll print it
    printf("%s", fileLine);

    pthread_exit(NULL);  // Exit thread when done
}



/*
Name: addLine
Process: recursively creates new fileHolder for dumping at the end of the display
Function Input/Parameters: pointer to fileHolder head

Function Output/Parameters: none
Function Output/Returned: returns pointer to fileHolder
Device Input/device: none
Device Output/device: none
Dependencies: copyString
*/
logToFile *addLine(logToFile *fileHolder, char *lineAdd) {
    // Check if fileHolder is NULL
    if (fileHolder == NULL) 
    {
        // Allocate memory for a new logToFile node
        fileHolder = (logToFile *)malloc(sizeof(logToFile));
        if (fileHolder == NULL) 
        {
            printf("Memory allocation failed\n");
            return NULL;  // Return if memory allocation fails
        }

        // Copy the line to filePiece using the copyString function
        copyString(fileHolder->filePiece, lineAdd);

        // Initialize next pointer
        fileHolder->next = NULL;

        return fileHolder;  // Return the newly created node
    }

    // Recursively add the line to the next node
    fileHolder->next = addLine(fileHolder->next, lineAdd);

    return fileHolder;
}


/*
Name: addPCB
Process: recursively creates new PCB with data from the config & metadata files
Function Input/Parameters: pointer to config data structure (ConfigDataType *)
                           Pointer to metadata structure (OpCodeType *)
                           pointer to PCB head (PCBType *)
Function Output/Parameters: none
Function Output/Returned: returns pointer to PCB head
Device Input/device: none
Device Output/device: none
Dependencies: tbd
*/
PCBType *addPCB( ConfigDataType *configDataPtr, OpCodeType *metaDataPtr, int PID,
                                                         PCBType *PCBHeadPtr)
{
    int endTime = calculateOpTime(metaDataPtr, configDataPtr);
    PCBType *tempHolder;
    
    //check if PCB head is null
    if( PCBHeadPtr == NULL)
    {
        //allocate the data
        PCBHeadPtr = (PCBType *)malloc(sizeof(PCBType));
        //set status new
        PCBHeadPtr->PCBStatus = NEW_STATE;
        //set the appstart
        PCBHeadPtr->appStart = metaDataPtr;
        //set next to null
        PCBHeadPtr->nextPCB = NULL;
        //calculate op time
        PCBHeadPtr->opEndTime = endTime;
        
        PCBHeadPtr->PID = PID;
        
        //return head
        return PCBHeadPtr;
    }
    
    if(configDataPtr->cpuSchedCode == CPU_SCHED_SJF_N_CODE || 
                    configDataPtr->cpuSchedCode == CPU_SCHED_SRTF_P_CODE)
    {
        if(endTime < PCBHeadPtr->opEndTime)
        {
            //set new data
            tempHolder = (PCBType *)malloc(sizeof(PCBType));
            tempHolder->PCBStatus = NEW_STATE;
            tempHolder->appStart = metaDataPtr;
            tempHolder->opEndTime = endTime;

            //set current PTR to the next because it is bigger
            tempHolder->nextPCB = PCBHeadPtr;
            //give the smaller PID to the new block
            tempHolder->PID = PCBHeadPtr->PID;
            //update the old ID to be more
            PCBHeadPtr->PID = PID;
            //return the new PTR
            return tempHolder;
        }
    }

    //set head next to recusive call with next
    PCBHeadPtr->nextPCB = addPCB( configDataPtr, metaDataPtr, PID,
                                                        PCBHeadPtr->nextPCB);

    //return head
    return PCBHeadPtr;
}


/*
Name: startPCB
Process: starts building the PCB 
Function Input/Pananeters: configuration data (ConfigDataType *),
                           metadata (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: pointer to head of PCB Linked list
Device Input/device: none
Device Output/device: none
Dependencies: addPCB
*/
PCBType *startPCB(OpCodeType *metaDataPtr, ConfigDataType *configPtr)
{
    PCBType *PCBHead = NULL;
    int PID = 0;
    
    //loop over temp metadata until end of data
    while( metaDataPtr != NULL)
    {
        //look for metaData app start
        if( compareString(metaDataPtr->command, "app") == STR_EQ &&
           compareString(metaDataPtr->strArg1, "start") == STR_EQ )
        {
            //add to PCB
            PCBHead =  addPCB( configPtr, metaDataPtr, PID, PCBHead);

            PID++;

        }
        
        //move meta data
        metaDataPtr = metaDataPtr->nextNode;
    }
    
    return PCBHead;
}

pthread_mutex_t stateMutex = PTHREAD_MUTEX_INITIALIZER;


/*
Name: setState
Process: loops over the PCB list setting the state and displaying appropriately
Function Input/Parameters: pointer to PCB head (PCBType *)
                           State to set to (int)
                           LogTo code (int)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: Displays to monitor and/or file
Dependencies: accessTimer
*/
pthread_mutex_t stateMutex = PTHREAD_MUTEX_INITIALIZER;

void setState(PCBType *headPtr, int state, int logCode,
                                logToFile* fileHolder, FILE* logFile)
{

    PCBType *pcbHolder = headPtr;
    char timer[STD_STR_LEN];
    char fileLine[256];
    
    // check for holder not null
    while(pcbHolder != NULL)
    {
        // lock the state chaange 
        pthread_mutex_lock( &stateMutex ); 

        pcbHolder->PCBStatus = state;

        pthread_mutex_unlock( &stateMutex);

        // lap timer
           // function: accessTimer
        accessTimer(LAP_TIMER, timer);
        
        // check for monitor or both
        if(logCode == LOGTO_MONITOR_CODE || logCode == LOGTO_BOTH_CODE)
           {
            // print process and remaining time
               // function: printf
            printf("%s, OS: Process: %lu set to READY state from NEW state\n",
                                                        timer, pcbHolder->PID);
           }
        
        // check for file or both now
        if(logCode == LOGTO_FILE_CODE || logCode == LOGTO_BOTH_CODE)
        {

            // print to file 
               // function: printf
            snprintf(fileLine, STD_STR_LEN, "%.8s, OS: Process: %lu set to READY state from NEW state\n", 
                        timer, pcbHolder->PID);
            
            fileHolder = addLine(fileHolder, fileLine);

        }
        
        // update PCB holder
        pcbHolder = pcbHolder->nextPCB;
    }

}


/*
Name: displayPCB
Process: recursively creates new PCB with data from the config & metadata files
Function Input/Parameters: pointer to PCB head (PCBType *)
                           LogTo Code (int)
                           LogTo File (FILE*)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: Displays to monitor and/or file
Dependencies: accessTimer, compareString
*/
void displayPCB( PCBType *PCBHead, FILE *logFile, int dispFlag, char *timer, logToFile *fileHolder, ConfigDataType *configPtr )
   {
    // Iterate through all PCBs and display their status based on the dispFlag
    pthread_mutex_lock(&stateMutex);  // Lock to ensure safe access

    PCBType *currentPCB = PCBHead;

    // Loop through each PCB and display its state
    while( currentPCB != NULL )
    {
        // Prepare the log message
        char logMessage[STD_STR_LEN];

        // Check the display flag and log the state transition
        if( dispFlag == START )
           {
            snprintf(logMessage, STD_STR_LEN, 
                    "%s, OS: Process %lu set to READY state from NEW state\n", 
                    timer, currentPCB->PID);
           }
        else if( dispFlag == RUN )
           {
            snprintf(logMessage, STD_STR_LEN, 
                    "%s, OS: Process %lu set from READY to RUNNING\n", 
                    timer, currentPCB->PID);
           }
        else if( dispFlag == EXIT_STATE )
           {
            snprintf(logMessage, STD_STR_LEN, 
                    "%s, OS: Process %lu set to EXIT\n", 
                    timer, currentPCB->PID);
           }

        // Display to monitor if configured
        if(configPtr->logToCode == LOGTO_MONITOR_CODE || 
                configPtr->logToCode == LOGTO_BOTH_CODE)
           {
            printf("%s", logMessage);
           }

        // Log to file if configured
        if(configPtr->logToCode == LOGTO_FILE_CODE || 
                configPtr->logToCode == LOGTO_BOTH_CODE)
           {
            fprintf(logFile, "%s", logMessage);
           }

        // Move to the next PCB in the list
        currentPCB = currentPCB->nextPCB;
       }
    // unlock after display success
    pthread_mutex_unlock( &stateMutex );  
   }

/*
Name: dumpFile
Process: dumps all the lines into a file at the end
Function Input/Parameters: pointer to fileHolder head and file  
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: fprintf
*/
void dumpFile( logToFile *fileHolder, FILE *fileName )
{
    logToFile *tempPtr = fileHolder;
    while( tempPtr != NULL )
       {
        fprintf(fileName, "%s", tempPtr->filePiece);
        tempPtr = tempPtr->next;
       }
}


/*
Name: checkIdle
Process: checks for pause in between threading
Function Input/Parameters: pointer to fileHolder head and file  
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: fprintf
*/
void checkIdle( PCBType *pcbPtr, logToFile *fileHolder, char *timer, int logCode )
   {
    PCBType *nextPtr = NULL;
    char fileLine[STD_STR_LEN];
    if(pcbPtr->nextPCB != NULL)
       {
        nextPtr = pcbPtr->nextPCB;
        if(pcbPtr->PCBStatus != BLOCKED_STATE && nextPtr->PCBStatus == BLOCKED_STATE)
           {
            accessTimer(LAP_TIMER, timer);
            if(logCode == LOGTO_MONITOR_CODE || logCode == LOGTO_BOTH_CODE)
               {
                //print process and remaining time
                printf("%s, OS: CPU idle, all active processes blocked\n", timer);

                printf("%s, OS: CPU interrupt, end idle\n", timer);
               }
            
            //check for file or both
            if(logCode == LOGTO_FILE_CODE || logCode == LOGTO_BOTH_CODE)
               {
                //print to file
                snprintf(fileLine, STD_STR_LEN, "%.8s, OS: CPU idle, all active processes blocked\n", timer);

                fileHolder = addLine(fileHolder, fileLine);

                snprintf(fileLine, STD_STR_LEN, "%.8s, OS: CPU interrupt, end idle\n", timer);

                fileHolder = addLine(fileHolder, fileLine);
               }
           }
       }
   }

/*
Name: calculateOpTime
Process: Calculate Process time for PCB process
Function Input/Parameters: pointer to metadata app start head (OpCpdeType *)
                           Pointer to config head (configDataType *)
Function Output/Parameters: none
Function Output/Returned: Total calculated operation time
Device Input/device: none
Device Output/device: none
Dependencies: accessTimer
*/
int calculateOpTime(OpCodeType *MDPtr, ConfigDataType *configPtr)
   {
    //init vars
    int runTime = 0;
    OpCodeType *tempPtr = MDPtr->nextNode;
    
    // loop through
    while(tempPtr != NULL && (compareString(tempPtr->command,"app") != STR_EQ))
       {
        // check for cpu run time
        if(compareString(tempPtr->command, "cpu") == STR_EQ)
           {
            // calc run time
            runTime = runTime + ((tempPtr->intArg2) * 
                                                    ( configPtr->proCycleRate ));
           }

        // check for dev run time
        if( compareString ( tempPtr->command, "dev" ) == STR_EQ )
           {
            // calc run time
            runTime = runTime + (( tempPtr->intArg2 ) * ( configPtr->ioCycleRate ));
           }

        // update tempPtr
        tempPtr = tempPtr->nextNode;
       }
    
    //return total
    return runTime;
   }

/*
Name: addfromPCB
Process: Calculate Process time for PCB process
Function Input/Parameters: pointer to metadata app start head (OpCpdeType *)
                           Pointer to config head (configDataType *)
Function Output/Parameters: none
Function Output/Returned: Total calculated operation time
Device Input/device: none
Device Output/device: none
Dependencies: accessTimer
*/
PCBType *addfromPCB( PCBType *pcbPtr, int state )
   {
    PCBType *newPCB = malloc(sizeof(PCBType));
    
    newPCB->appStart = pcbPtr->appStart;
    newPCB->opEndTime = pcbPtr->opEndTime;
    newPCB->PCBStatus = state;
    newPCB->PID = pcbPtr->PID;
    newPCB->nextPCB = NULL;

    return newPCB;
   }

/*
Name: memSim
Process: simulates the memory part of the sim. 
Function Input/Parameters:
Function Output/Parameters: none
Function Output/Returned: none 
Device Input/device: none
Device Output/device: none
Dependencies: fprintf
*/
bool memSim( OpCodeType *memPtr, int logCode, int PID, logToFile *fileHolder, 
                        int flag, ConfigDataType *configPtr, memHolder *memUsed)
   {
    // initialize variables
    bool memAcq = false;
    memHolder *tempMem = memUsed;
    int dataEdge = 0;
    char fileLine[STD_STR_LEN];
 
    // check for monitor or both
    if( logCode == LOGTO_MONITOR_CODE || logCode == LOGTO_BOTH_CODE )
       {
        // print process and remaining time
           // function: printf
        printf("--------------------------------------------------\n");
       }
        
        //check for file or both
    if( logCode == LOGTO_FILE_CODE || logCode == LOGTO_BOTH_CODE )
       {
        // print to file
           // function: sprintf
        sprintf(fileLine, 
                        "--------------------------------------------------\n");

        fileHolder = addLine(fileHolder, fileLine);
       }
    
    if( flag == START)
       {
        if(logCode == LOGTO_MONITOR_CODE || logCode == LOGTO_BOTH_CODE )
           {
            // print process and remaining time 
               // function: printf
            printf("After memory initialization\n");
            printf("0 [ Open, P#: x, 0-0 ] %d\n", (configPtr->memAvailable));
           }
        
        // check for file or both
        if( logCode == LOGTO_FILE_CODE || logCode == LOGTO_BOTH_CODE )
           {
            //print to file
            sprintf(fileLine, "After memory initializatio\n");

            fileHolder = addLine(fileHolder, fileLine);

            sprintf(fileLine, "0 [ Open, P#: x, 0-0 ] %d\n", 
                                                    (configPtr->memAvailable));

            fileHolder = addLine(fileHolder, fileLine);
           } 
       }
    
    if( flag == RUN )
       {
        dataEdge = memPtr->intArg2 + memPtr->intArg3 - 1;
        if( compareString(memPtr->strArg1, "access") == STR_EQ )
           {
            while( tempMem->next != NULL && memAcq != true )
               {
                if((tempMem->start < memPtr->intArg2) && 
                     (memPtr->intArg2 < tempMem->end) &&
                          (tempMem->start < dataEdge) &&
                              (dataEdge < tempMem->end))
                   {
                    memAcq = true;
                   }
                   else
                    { 
                    tempMem = tempMem->next;
                   }
               }
            memAcq = ((tempMem->start < memPtr->intArg2) && 
                                        (memPtr->intArg2 < tempMem->end) &&
                                            (tempMem->start < dataEdge) &&
                                                    (dataEdge < tempMem->end));
       }
        else
        {
            
            memAcq = true;

            if(memUsed == NULL)
            {
                memUsed = addMem(memUsed, memPtr->intArg2, dataEdge);
            }
            else
            {
                while (tempMem != NULL && memAcq != false)
                {
                    if((tempMem->start < memPtr->intArg2 && 
                                        tempMem->start < tempMem->end) ||
                                                (tempMem->start < dataEdge && 
                                                tempMem->start < tempMem->end))
                    {
                        memAcq = false;
                    }
                    else
                    {
                        tempMem = tempMem->next;
                    }
                }
                if(memAcq)
                {
                    memUsed = addMem(memUsed, memPtr->intArg2, dataEdge);
                }
            }
        }
        if(memAcq)
        {  
            if(compareString(memPtr->strArg1, "access") == STR_EQ)
            {
                //check for monitor or both
                if(logCode == LOGTO_MONITOR_CODE || logCode == LOGTO_BOTH_CODE)
                {
                    //print process and remaining time
                    printf("After access success\n");
                    printf("0 [ Used, P#: %d, %d-%d ] %d\n ", PID, 
                                                tempMem->start, tempMem->end,
                                            (tempMem->end - tempMem->start));
                    printf(" %d [ Open, P#: x, 0-0 ] %d\n", (tempMem->end + 1), 
                                                    (configPtr->memAvailable));
                }
                        
                        //check for file or both
                if(logCode == LOGTO_FILE_CODE || logCode == LOGTO_BOTH_CODE)
                {
                    //print to file
                    sprintf(fileLine, "After allocate success\n");
                    fileHolder = addLine(fileHolder, fileLine);
                    sprintf(fileLine, "0 [ Used, P#: %d, %d-%d ] %d\n", PID, 
                                                tempMem->start, tempMem->end,
                                            (tempMem->end - tempMem->start));
                    fileHolder = addLine(fileHolder, fileLine);
                    sprintf(fileLine, "%d [ Open, P#: x, 0-0 ] %d\n", 
                                                            (tempMem->end + 1),
                                                    (configPtr->memAvailable));
                    
                    fileHolder = addLine(fileHolder, fileLine);
                }
            }
            else
            {
                //check for monitor or both
                if(logCode == LOGTO_MONITOR_CODE || logCode == LOGTO_BOTH_CODE)
                {
                    //print process and remaining time
                    printf("After allocate success\n");
                    printf("0 [ Used, P#: %d, %d-%d ] %d\n ", PID, 
                                                    memPtr->intArg2, dataEdge,
                                                (dataEdge - memPtr->intArg2));
                    printf(" %d [ Open, P#: x, 0-0 ] %d\n", memPtr->intArg3, 
                                                    (configPtr->memAvailable));
                }
                        
                        //check for file or both
                if(logCode == LOGTO_FILE_CODE || logCode == LOGTO_BOTH_CODE)
                {
                    //print to file
                    sprintf(fileLine, "After allocate success\n");
                    fileHolder = addLine(fileHolder, fileLine);
                    sprintf(fileLine, "0 [ Used, P#: %d, %d-%d ] %d\n", 
                                                PID, memPtr->intArg2, dataEdge,
                                                (dataEdge - memPtr->intArg2));
                    fileHolder = addLine(fileHolder, fileLine);
                    sprintf(fileLine, "%d [ Open, P#: x, 0-0 ] %d\n", 
                                                                memPtr->intArg3,
                                                    (configPtr->memAvailable));
                    fileHolder = addLine(fileHolder, fileLine);
                }
            }
        }
        else
        {
            if(compareString(memPtr->strArg1, "access") == STR_EQ)
            {
                if(logCode == LOGTO_MONITOR_CODE || logCode == LOGTO_BOTH_CODE)
                {
                        //print process and remaining time
                    printf("After access failure\n");
                    printf("0 [ Used, P#: %d, %d-%d ] %d\n ", PID, 
                                                tempMem->start, tempMem->end,
                                            (tempMem->end - tempMem->start));
                    printf(" %d [ Open, P#: x, 0-0 ] %d\n", 
                                (tempMem->end + 1), (configPtr->memAvailable));
                }
                        
                        //check for file or both
                if(logCode == LOGTO_FILE_CODE || logCode == LOGTO_BOTH_CODE)
                {
                            //print to file
                    sprintf(fileLine, "After access failure\n");
                    fileHolder = addLine(fileHolder, fileLine);
                    sprintf(fileLine, "0 [ Used, P#: %d, %d-%d ] %d\n ", 
                                            PID, tempMem->start, tempMem->end,
                                            (tempMem->end - tempMem->start));
                    fileHolder = addLine(fileHolder, fileLine);
                    sprintf(fileLine, "%d [ Open, P#: x, 0-0 ] %d\n", 
                            (tempMem->end + 1), (configPtr->memAvailable - 1));
                    fileHolder = addLine(fileHolder, fileLine);
                }
            }
            else
            {
            if(logCode == LOGTO_MONITOR_CODE || logCode == LOGTO_BOTH_CODE)
            {
                    //print process and remaining time
                printf("After allocate Overlap failure\n");
                printf("0 [ Used, P#: %d, %d-%d ] %d\n ", PID, tempMem->start, 
                                tempMem->end,(tempMem->end - tempMem->start));
                
                printf(" %d [ Open, P#: x, 0-0 ] %d\n", (tempMem->end + 1),
                                                (configPtr->memAvailable - 1));
            }
                    
                    //check for file or both
            if(logCode == LOGTO_FILE_CODE || logCode == LOGTO_BOTH_CODE)
            {
                        //print to file
                sprintf(fileLine, "After allocate Overlap failure\n");
                fileHolder = addLine(fileHolder, fileLine);
                sprintf(fileLine, "0 [ Used, P#: %d, %d-%d ] %d\n ", PID, 
                                                tempMem->start, tempMem->end,
                                            (tempMem->end - tempMem->start));
                fileHolder = addLine(fileHolder, fileLine);
                sprintf(fileLine, "%d [ Open, P#: x, 0-0 ] %d\n", 
                        (tempMem->end + 1), (configPtr->memAvailable - 1));
                fileHolder = addLine(fileHolder, fileLine);
            }
            }
        }

    }

    if(flag == CLEAR)
    {
        freeMem(memUsed);
        if(logCode == LOGTO_MONITOR_CODE || logCode == LOGTO_BOTH_CODE)
        {
                //print process and remaining time
            printf("After clear process %d success\n", PID);
            printf("0 [ Open, P#: x, 0-0 ] %d\n", 
                                                (configPtr->memAvailable - 1));
        }
        
        //check for file or both
        if(logCode == LOGTO_FILE_CODE || logCode == LOGTO_BOTH_CODE)
        {
                //print to file
            sprintf(fileLine, "After clear process %d success\n", PID);

            fileHolder = addLine(fileHolder, fileLine);

            sprintf(fileLine, "0 [ Open, P#: x, 0-0 ] %d\n", 
                                                (configPtr->memAvailable - 1));

            fileHolder = addLine(fileHolder, fileLine);
        }
    

    }
    
    if(logCode == LOGTO_MONITOR_CODE || logCode == LOGTO_BOTH_CODE)
    {
            //print process and remaining time
        printf("--------------------------------------------------\n");
    }
        
        //check for file or both
    if(logCode == LOGTO_FILE_CODE || logCode == LOGTO_BOTH_CODE)
    {
            //print to file
        sprintf
            (fileLine, "--------------------------------------------------\n");

        fileHolder = addLine(fileHolder, fileLine);
    }

    return memAcq;   
}

/*
Name: addMem
Process: Recursively adds used memory
Function Input/Pananeters: memHolder * (pointer to used memory)
                            int starting bound of data
                            int ending bound of data
Function Output/Parameters: none
Function Output/Returned: returns pointer to head of used memory
Device Input/device: none
Device Output/device: none
Dependencies: addMem
*/
memHolder *addMem(memHolder *usedMem, int startData, int endData)
{
    //check if PCB head is null
    if( usedMem == NULL)
    {
        usedMem = (memHolder *)malloc(sizeof(memHolder));
        usedMem->start = startData;

        usedMem->end = endData;
        
        usedMem->next = NULL;

        //return head
        return usedMem;
    }
    
    //set head next to recusive call with next
    usedMem->next= addMem(usedMem, startData, endData);
    
    //return head
    return usedMem;
}

/*
Name: freeMem
Process: Recursively frees used memory
Function Input/Pananeters: memHolder * (pointer to used memory)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: free
*/
memHolder *freeMem(memHolder *usedMem)
{
    if(usedMem == NULL)
    {
        return NULL;
    }

    freeMem(usedMem->next);

    free(usedMem);

    return NULL;
}


/*
Name: enqueueInterrupt
Process: Recursively frees used memory
Function Input/Pananeters: memHolder * (pointer to used memory)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: free
*/
void enqueueInterrupt( int processID ) 
   {
    pthread_mutex_lock( &interruptMutex );
    Interrupt *newInterrupt = ( Interrupt * )malloc( sizeof( Interrupt ) );
    newInterrupt->processID = processID;
    newInterrupt->next = NULL;

    if ( interruptQueue == NULL ) 
       {
        interruptQueue = newInterrupt;
       } 
    else 
       {
        Interrupt *temp = interruptQueue;
        while ( temp->next != NULL ) 
           {
            temp = temp->next;
           }
        temp->next = newInterrupt;
       }
    pthread_mutex_unlock( &interruptMutex );
}


/*
Name: dequeueInterrupt
Process: Recursively frees used memory
Function Input/Pananeters: memHolder * (pointer to used memory)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: free
*/
int dequeueInterrupt() 
   {
    pthread_mutex_lock( &interruptMutex );
    if (interruptQueue == NULL) 
       {
        pthread_mutex_unlock(&interruptMutex);
        return -1;
       }

    Interrupt *temp = interruptQueue;
    int processID = temp->processID;
    interruptQueue = interruptQueue->next;
    free(temp);
    pthread_mutex_unlock(&interruptMutex);
    return processID;
}


/*
Name: simulateIO
Process: Recursively frees used memory
Function Input/Pananeters: memHolder * (pointer to used memory)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: free
*/
// Function to simulate I/O operation
void *simulateIO(void *arg) 
   {
    PCBType *pcb = (PCBType *)arg;
    char timer[STD_STR_LEN];
    accessTimer(LAP_TIMER, timer);
    printf("%s, OS: Process %lu performing I/O operation\n", 
                                timer, pcb->PID);

    // Simulate I/O time
    runTimer(pcb->ioTime);

    // Add to interrupt queue when done
    enqueueInterrupt(pcb->PID);
    accessTimer(LAP_TIMER, timer);
    printf("%s, OS: Process %lu completed I/O\n", timer, pcb->PID);

    pthread_exit(NULL);
}


/*
Name: scheduleProcesses
Process: Recursively frees used memory
Function Input/Pananeters: memHolder * (pointer to used memory)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: free
*/
// Preemptive scheduling: FCFS-P, SRTF-P, RR-P
void scheduleProcesses(ConfigDataType *config, PCBType **readyQueue, int schedulingCode) 
   {
    PCBType *current = NULL;
    char timer[STD_STR_LEN];

    while (*readyQueue != NULL) 
       {
        // Select process based on scheduling strategy
        if (schedulingCode == CPU_SCHED_FCFS_P_CODE) 
           {
            // First Come First Serve (Preemptive)
            current = *readyQueue;
           } 
        else if (schedulingCode == CPU_SCHED_SRTF_P_CODE) 
           {
            // Shortest Remaining Time First (Preemptive)
            current = *readyQueue;
            PCBType *temp = current->nextPCB;
            while (temp != NULL) 
               {
                if (temp->cycles < current->cycles) 
                   {
                    current = temp;
                   }
                temp = temp->nextPCB;
               }
           } 
        else if (schedulingCode == CPU_SCHED_RR_P_CODE) 
           {
            // Round Robin (Preemptive)
            current = *readyQueue;  // Take the first process in the queue
           }

        // Remove the selected process from the ready queue
        *readyQueue = current->nextPCB;
        current->nextPCB = NULL;

        // Run the selected process
        current->PCBStatus = RUNNING_STATE;
        accessTimer(LAP_TIMER, timer);
        printf("%s, OS: Process %lu moved to RUNNING state\n", timer, current->PID);

        int remainingCycles = current->cycles;
        int quantum = config->quantumCycles;  // Use quantum time for Round Robin
        bool processCompleted = false;


        while (remainingCycles > 0) 
           {
            int cyclesToRun = (schedulingCode == CPU_SCHED_RR_P_CODE && remainingCycles > quantum)
                                  ? quantum
                                  : remainingCycles;

            for (int cycle = 0; cycle < cyclesToRun; cycle++) 
               {
                runTimer(config->proCycleRate);

                // Check for interrupts
                int interruptID;
                while ((interruptID = dequeueInterrupt()) != -1) 
                   {
                    printf("%s, OS: Process %d interrupted\n", timer, interruptID);
                    // Handle interrupts (e.g., move interrupted process back to READY state)
                   }
               }

            remainingCycles -= cyclesToRun;

            // If preempted (RR-P or interrupt), move process back to the ready queue
            if (remainingCycles > 0 && schedulingCode == CPU_SCHED_RR_P_CODE) 
               {
                current->cycles = remainingCycles;
                current->PCBStatus = READY_STATE;
                accessTimer(LAP_TIMER, timer);
                printf("%s, OS: Process %lu preempted, %d cycles remaining\n", 
                                        timer, current->PID, remainingCycles);

                // Add process back to the ready queue
                PCBType *tempQueue = *readyQueue;
                if (tempQueue == NULL) 
                   {
                    *readyQueue = current;
                   } 
                else 
                   {
                    while (tempQueue->nextPCB != NULL) 
                       {
                        tempQueue = tempQueue->nextPCB;
                       }
                    tempQueue->nextPCB = current;
                   }

                // Exit inner loop by setting a flag
                processCompleted = true;
               }
           }

        if (remainingCycles <= 0) 
           {
            // Process completes execution
            current->PCBStatus = EXIT_STATE;
            accessTimer(LAP_TIMER, timer);
            printf("%s, OS: Process %lu moved to EXIT state\n", 
                                        timer, current->PID);
           }
       }
    }



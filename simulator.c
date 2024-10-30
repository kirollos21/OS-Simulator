// header file
#include "simulator.h"

/*
Name: addToFileList
Process: adds a new line to the file list
Function Input/Parameters: 
    - newLine: pointer to the new line to be added (char *)
    - headPtr: pointer to the head of the file list (FileStr *)
Function Output/Parameters: none
Function Output/Returned: pointer to the head of the updated file list (FileStr *)
Dependencies:
*/
FileStr *addToFileList( char *newLine, FileStr *headPtr )
{
  //intilize avairbles 
    FileStr *wkgPtr = headPtr;

    //check if the wkg pointer is null
    if( wkgPtr == NULL )
    {
        //if it is empty then create a new file line
          // function: malloc
        FileStr *fileS = (FileStr *)malloc( sizeof( FileStr ) );

        // set the string
        copyString(fileS->currentStr, "");

        //set the next string to null
        fileS->nextStr = NULL;

        wkgPtr = fileS;
        //set the head pointer to the working pointer
        headPtr = wkgPtr;
    }

    //otherwise it's full with goodies
    else
    {
        //look through the list until the end
        while( wkgPtr->nextStr != NULL )
        {
            //set the working pointer to the next string
            //moving over one
            wkgPtr = wkgPtr->nextStr;
        }
        //if it is end of the line then create a new file line
          // function: malloc
        FileStr *fileS = (FileStr *)malloc( sizeof( FileStr ) );

        // set the string
        copyString(fileS->currentStr, "");
        
        //set the next string to null
        fileS->nextStr = NULL;

        wkgPtr = fileS;
    }
    //return the head pointer
    return headPtr;
}


/*
Name: addToMemList
Process: adds a new memory address node to the memory address list
Function Input/Parameters: 
    - address: the memory address to be added (int)
    - size: the size of the memory block (int)
    - headPtr: pointer to the head of the memory address list (MemAddress *)
Function Output/Parameters: none
Function Output/Returned: pointer to the head of the updated memory address list (MemAddress *)
Dependencies: createMemAddress
*/
MemAddress *addToMemList(int address, int size, MemAddress *headPtr)
{
    //intilize avairbles
    MemAddress *wkgPtr = headPtr;
     
    //check if the working pointer is null
    if (wkgPtr == NULL)
    {
        //if it is, then create a new memory address
        headPtr = createMemAddress(address, size);
    }

    //otherwise it's full with goodies
    else
    {
        //loop thru the list until the end
        while (wkgPtr->nextAddress != NULL)
        {
            //set the working pointer to the next address
            wkgPtr = wkgPtr->nextAddress;
        }
        //if we are at the end of the list then create a new memory address
        wkgPtr->nextAddress = createMemAddress(address, size);
    }
    //return the head pointer
    return headPtr;
}

/*
Name: addToProcessList
Process: adds a new process control block (PCB) to the process list
Function Input/Parameters: 
    - pid: process ID of the new PCB (int)
    - opCodePtr: pointer to the metadata for the new PCB (OpCodeType *)
    - headPtr: pointer to the head of the process list (PCB *)
Function Output/Parameters: none
Function Output/Returned: pointer to the head of the updated process list (PCB *)
Dependencies: createPCB
*/
PCB *addToProcessList(int pid, OpCodeType *opCodePtr, PCB *headPtr)
{

    //intilize avairbles
    PCB *wkgPtr = headPtr;
    
    //check if the working pointer is null
    if (wkgPtr == NULL)
    {
        //if it is, then create a new process control block
        headPtr = createPCB(pid, opCodePtr);
    }
    //otherwise it's full with goodies
    else
    {
        //loop thru the list until the end
        while (wkgPtr->nextProcess != NULL)
        {
            //set the working pointer to the next process
            wkgPtr = wkgPtr->nextProcess;
        }
        //if we are at the end of the list then create a new process control block
        wkgPtr->nextProcess = createPCB(pid, opCodePtr);
    }
    //return the head pointer
    return headPtr;
}

/*
Name: changeProcessState
Process: changes the state of a process control block (PCB)
Function Input/Parameters: 
    - currentPCB: pointer to the PCB whose state is to be changed (PCB *)
    - newState: the new state to which the PCB's state is to be changed (ProcessState)
    - outStr: output string to record the state change (char *)
Function Output/Parameters: none
Function Output/Returned: pointer to the updated PCB (PCB *)
Dependencies: none
*/
PCB *changeProcessState(PCB *currentPCB, ProcessState newState, char *outStr)
{
    // Initialize variables
    char currentStateStr[MIN_STR_LEN], newStateStr[MIN_STR_LEN];

    // Determine the current state
    //if the current state is new
    if (currentPCB->state == NEW)
    {
        // Copy the current state to the current state string
        copyString(currentStateStr, "NEW");
        //save the message neeeded
        sprintf(outStr, ", OS: Process %d set to %s state from %s state\n", currentPCB->pid, newStateStr, currentStateStr);
    }
    //check if the current state is running
    else if (currentPCB->state == RUNNING)
    {
        // Copy the current state to the current state string
        copyString(currentStateStr, "RUNNING");
    }
    //check if the current state is ready
    else if (currentPCB->state == READY)
    {
        // Copy the current state to the current state string
        copyString(currentStateStr, "READY");
    }
    //check if the current state is blocked
    else if (currentPCB->state == BLOCKED)
    {
        // Copy the current state to the current state string
        copyString(currentStateStr, "BLOCKED");
    }
    //otherwise the current state is exit
    else if (currentPCB->state == EXIT)
    {
        // Copy the current state to the current state string
        copyString(currentStateStr, "EXIT");
    }

    // Determine the new state
    //if the new state is new
    if (newState == NEW)
    {
        // Copy the new state to the new state string
        copyString(newStateStr, "NEW");
    }
    //check if the new state is running
    else if (newState == RUNNING)
    {
      // Copy the new state to the new state string
        copyString(newStateStr, "RUNNING");
    }
    //check if the new state is ready
    else if (newState == READY)
    {
      // Copy the new state to the new state string
        copyString(newStateStr, "READY");
    }
    //check if the new state is blocked
    else if (newState == BLOCKED)
    {
      // Copy the new state to the new state string
        copyString(newStateStr, "BLOCKED");
    }
    //otherwise the new state is exit
    else if (newState == EXIT)
    {
      // Copy the new state to the new state string
        copyString(newStateStr, "EXIT");
        //save the message neeeded
        sprintf(outStr, ", OS: Process %d set to %s\n", currentPCB->pid, newStateStr);
    }
    //save the message neeeded
    sprintf(outStr, ", OS: Process %d set from %s to %s\n\n", currentPCB->pid, currentStateStr, newStateStr);


    // Change current state
    currentPCB->state = newState;

    // Return updated pcb
    return currentPCB;
}

/*
Name: createMemAddress
Process: creates a new memory address node
Function Input/Parameters: 
    - address: memory address value (int)
    - size: size of memory block (int)
Function Output/Parameters: none
Function Output/Returned: pointer to the newly created memory address node (MemAddress *)
Dependencies: none
*/
MemAddress *createMemAddress(int address, int size)
{
    // initialize variables
    MemAddress *memNode = (MemAddress *)malloc(sizeof(MemAddress));

    // Set node's address and size
    memNode->address = address;
    memNode->size = size;

    // Set next node to null
    memNode->nextAddress = NULL;

    // Return new memory node
    return memNode;
}

/*
Name: createPCB
Process: creates a new Process Control Block (PCB) node
Function Input/Parameters: 
    - pid: process ID (int)
    - OpCodePtr: pointer to the metadata (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: pointer to the newly created PCB node (PCB *)
Dependencies: none
*/
PCB *createPCB(int pid, OpCodeType *OpCodePtr)
{
    // Initialize a new process control board
    PCB *newProcess = (PCB *)malloc(sizeof(PCB));

    // Set process ID to input PID
    newProcess->pid = 0;

    // Set next process to null
    newProcess->nextProcess = NULL;

    // Set the head of the opcode
    newProcess->headOfOpCode = NULL;

    // Set the status to NEW
    newProcess->state = NEW;

    // Set program counter and time left to zero
    newProcess->counter = 0;
    newProcess->timeLeft = 0;

    // Set memory bounds to zero
    newProcess->memUpperBound = 0;
    newProcess->memLowerBound = 0;

    // Set list of used memory address to defaults
    newProcess->headMemAddress = 0;
    newProcess->memAddressList = NULL;

    // Return the new PCB
    return newProcess;
}



/*
Name: createQueueNode
Process: creates a new node for a process queue
Function Input/Parameters: 
    - process: pointer to the PCB of the process (PCB *)
Function Output/Parameters: none
Function Output/Returned: pointer to the newly created queue node (QueueNode *)
Dependencies: none
*/
QueueNode* createQueueNode(PCB *process) 
{
    // Initialize a new queue node
    QueueNode *newNode = (QueueNode*)malloc(sizeof(QueueNode));
    // Check if node was created
    if (newNode != NULL) 
    {
        // Set the process and next node
        newNode->process = process;
        // Set the next node to null
        newNode->next = NULL;
    }
    // Return the new node
    return newNode;
}


/*
Name: decodeOpCode
Process: decodes an operation code into a human-readable string
Function Input/Parameters: 
    - currentCode: pointer to the OpCodeType containing the operation code details (OpCodeType *)
    - decodedString: pointer to the buffer where the decoded string will be stored (char *)
    - opCodeStatus: status of the operation code (const char *)
Function Output/Parameters: decodedString
Function Output/Returned: none
Dependencies: compareString, sprintf
*/
void decodeOpCode(OpCodeType *currentCode, char *decodedString, const char *opCodeStatus)
{
    // Initialize variables
    char formatedStr[STD_STR_LEN];

    // Check if code is for device I/O
    if (compareString(currentCode->command, "dev") == STR_EQ)
    {
        // Format the operation string based on input/output
        sprintf(formatedStr, "%s %s%s", currentCode->strArg1, 
                      (compareString(currentCode->inOutArg, "in") == STR_EQ) ? "input" : "output", 
                                                               opCodeStatus);
    }
    // Check if code is for CPU process
    else if (compareString(currentCode->command, "cpu") == STR_EQ)
    {
        // Format the operation string for CPU process
        sprintf(formatedStr, "cpu process operation %s", opCodeStatus);
    }

    // Format the decoded string
    sprintf(decodedString, ", Process: %d, %s\n", currentCode->pid, formatedStr);
}


/*
Name: delay
Process: introduces a delay for a specified number of milliseconds
Function Input/Parameters: 
    - milliseconds: the duration of the delay in milliseconds (int)
Function Output/Parameters: none
Function Output/Returned: none
Dependencies: clock
*/
void delay(int milliseconds) 
{
    // Get the start time
    clock_t start_time = clock();
    // Loop until the time has elapsed
    while ((clock() - start_time) * 1000 / CLOCKS_PER_SEC < milliseconds);

}

/*
Name: dequeue
Process: removes and returns the front PCB from the queue
Function Input/Parameters: 
    - queue: a pointer to the queue from which to dequeue (Queue *)
Function Output/Parameters: none
Function Output/Returned: 
    - a pointer to the dequeued PCB, or NULL if the queue is empty (PCB *)
Dependencies: isQueueEmpty
*/
PCB* dequeue(Queue *queue) 
{
    // Check if queue is empty
    if (isQueueEmpty(queue)) 
    {
        // Return null if queue is empty
        return NULL;
    } 
    // Otherwise, remove and return the front PCB
    else 
    {
        // Initialize variables
        QueueNode *temp = queue->front;
        PCB *process = temp->process;

        // Move front to the next node
        queue->front = queue->front->next;
        // Check if front is null
        if (queue->front == NULL) 
        {
            // Set rear to null if front is null
            queue->rear = NULL;
        }
        // Free the temp node
        free(temp);
        // Return the dequeued PCB
        return process;
    }
}



/*
Name: displayOutput
Process: displays the output message and adds it to the file list if configured to log to file
Function Input/Parameters: 
    - configPtr: a pointer to the configuration data (ConfigDataType *)
    - message: the message to be displayed (char *)
    - fileList: a pointer to the file list (FileStr *)
Function Output/Parameters: none
Function Output/Returned: 
    - a pointer to the updated file list (FileStr *)
Dependencies: accessTimer, addToFileList
*/
FileStr *displayOutput(ConfigDataType *configPtr, char *message, FileStr *fileList)
{
    // Initialize variables
    char outMessage[MAX_STR_LEN], timering[MIN_STR_LEN];

    // Get current time
    accessTimer(LAP_TIMER, timering);

    // Format the message
    sprintf(outMessage, " %s%s", timering, message);

    // Check if output to screen or both
    if (configPtr->logToCode == LOGTO_MONITOR_CODE || configPtr->logToCode == LOGTO_BOTH_CODE)
    {
        // Print to screen
        printf("%s", outMessage);
    }
    // Check if output to file or both
    if (configPtr->logToCode == LOGTO_FILE_CODE || configPtr->logToCode == LOGTO_BOTH_CODE)
    {
        // Add to file line list
        fileList = addToFileList(outMessage, fileList);
    }

    // Return updated file pointer
    return fileList;
}


/*
Name: enqueue
Process: adds a PCB process to the end of the queue
Function Input/Parameters: 
    - queue: a pointer to the queue structure (Queue *)
    - process: a pointer to the PCB process to be enqueued (PCB *)
Function Output/Parameters: none
Function Output/Returned: none
Dependencies: createQueueNode, isQueueEmpty
BROKEN
*/
void enqueue(Queue *queue, PCB *process) 
{
    // Create a new queue node
    QueueNode *newNode = createQueueNode(process);
    // Check if node was created
    if (newNode != NULL) 
    {
        // Check if queue is empty
        if (isQueueEmpty(queue)) 
        {
            // Set front and rear to the new node
            queue->front = queue->rear = newNode;
        } 
        // Otherwise, add the node to the rear
        else 
        {
            // Set the next node of the rear to the new node
            queue->rear->next = newNode;
            // Set the rear to the new node
            queue->rear = newNode;
        }
    }
}

/*
Name: isQueueEmpty
Process: checks if the queue is empty
Function Input/Parameters: queue: a pointer to the queue structure (Queue *)
Function Output/Parameters: none
Function Output/Returned: a boolean value indicating whether the queue is empty (bool)
Dependencies: none
*/
bool isQueueEmpty(Queue *queue)
{
    // Return true if front is null
    return queue->front == NULL;
}


/*
Name: freeFileList
Process: frees memory allocated for a linked list of FileStr structures
Function Input/Parameters: fileList: a pointer to the head of the FileStr linked list (FileStr *)
Function Output/Parameters: none
Function Output/Returned: a null pointer
Dependencies: freeFileList
*/
FileStr *freeFileList(FileStr *fileList)
{
    // Check if the list is empty
    if (fileList == NULL) 
    {
        // Return null if list is empty
        return NULL;
    }
    //otherwise
    // Free the next node
    freeFileList(fileList->nextStr);
    // Free the current node
    free(fileList);
    // Return null
    return NULL;
}

/*
Name: freeMemList
Process: frees memory allocated for a linked list of MemAddress structures
Function Input/Parameters: memList: a pointer to the head of the MemAddress linked list (MemAddress *)
Function Output/Parameters: none
Function Output/Returned: a null pointer
Dependencies: freeMemList
*/

MemAddress *freeMemList(MemAddress *memList)
{
    // Check if the list is empty
    if (memList == NULL) 
    {
        // Return null if list is empty
        return NULL;
    }
    //otherwise
    // Free the next node
    freeMemList(memList->nextAddress);
    // Free the current node
    free(memList);
    // Return null
    return NULL;
}

/*
Name: freePCBList
Process: frees memory allocated for a linked list of PCB structures
Function Input/Parameters: PCBList: a pointer to the head of the PCB linked list (PCB *)
Function Output/Parameters: none
Function Output/Returned: a null pointer
Dependencies: freePCBList
*/
PCB *freePCBList(PCB *PCBList)
{
    // Check if the list is empty
    if (PCBList == NULL) 
    {
        // Return null if list is empty
        return NULL;
    }
    //otherwise
    // Free the next node
    freePCBList(PCBList->nextProcess);
    // Free the current node
    free(PCBList);
    return NULL;
}


/*
Name: getNextOpCode
Process: retrieves the next OpCodeType structure from the linked list of OpCodeType structures associated with a PCB
Function Input/Parameters: headPtr: a pointer to the PCB structure (PCB *)
Function Output/Parameters: none
Function Output/Returned: a pointer to the next OpCodeType structure (OpCodeType *)
Dependencies: none
*/
OpCodeType *getNextOpCode(PCB *headPtr)
{
    // Initialize variables
    OpCodeType *tempPtr = headPtr->headOfOpCode;
    int index = 0;
    // Loop through the list until the end
    while (index < headPtr->counter) 
    {
        // Move to the next node
        tempPtr = tempPtr->nextNode;
        // Increment the index
        index++;
    }
    // Return the next OpCodeType structure
    return tempPtr;
}


/*
Name: getNextProc
Process: retrieves the next PCB based on the CPU scheduling algorithm specified in the configuration data
Function Input/Parameters: configPtr: pointer to the configuration data (ConfigDataType *),
                           headPtr: pointer to the head of the PCB linked list (PCB *)
Function Output/Parameters: none
Function Output/Returned: pointer to the next PCB selected based on the CPU scheduling algorithm (PCB *)
Dependencies: roundRobinQueue
*/
PCB *getNextProc(ConfigDataType *configPtr, PCB *headPtr)
{
    // Initialize variables
    PCB *wkgPtr = headPtr;
    PCB *selected = NULL;
    int index = wkgPtr->pid, smallestVal = wkgPtr->timeLeft;
    int currentTime, shortestTime = 0;
    int processID;

    // Check the CPU scheduling code
    //if the CPU scheduling code is SJF-N
    if (configPtr->cpuSchedCode == CPU_SCHED_SJF_N_CODE)
    {
        // Loop through the list until the end (+ its not running)
        while (wkgPtr->nextProcess != NULL && wkgPtr->state != RUNNING)
        {
            // Check if the time left is less than the smallest value
            if ((smallestVal == 0 || wkgPtr->timeLeft < smallestVal) &&
                wkgPtr->state != EXIT)
            {
                // Set the smallest value and index
                smallestVal = wkgPtr->timeLeft;
                // Set the index to the process ID
                index = wkgPtr->pid;
            }
            // Move to the next node
            wkgPtr = wkgPtr->nextProcess;
        }
        // if rghe smallest value is not zero
        if (smallestVal != 0)
        {
            // Loop through the list until the end
            if (wkgPtr->state == RUNNING || (wkgPtr->timeLeft < smallestVal && wkgPtr->state != EXIT))
            {
                // Set the index to the process ID
                index = wkgPtr->pid;
            }
            // Move to the next node
            while (wkgPtr->pid != index)
            {
                // Move to the next node
                wkgPtr = wkgPtr->nextProcess;
            }
        }
    }
    //if the CPU scheduling code is SJF-P
    else if (configPtr->cpuSchedCode == CPU_SCHED_SRTF_P_CODE)
    {
        // Loop through the list until the end
        while (wkgPtr != NULL)
        {
            // Check if the state is ready
            if (wkgPtr->state == READY)
            {
                // Set the current time
                currentTime = wkgPtr->timeLeft;
                // Check if the current time is less than the shortest time
                if (currentTime < shortestTime || shortestTime == 0)
                {
                    // Set the shortest time and selected process
                    shortestTime = currentTime;
                    selected = wkgPtr;
                }
            }
            // Move to the next node
            wkgPtr = wkgPtr->nextProcess;
        }
    }
    // if the CPU scheduling code is RR-P
    else if (configPtr->cpuSchedCode == CPU_SCHED_RR_P_CODE)
    {
        // Initialize static variable
        static bool initialized = false;

        // Check if the queue is initialized
        if (!initialized) 
        {
            // Loop through the list until the end
            PCB *tempProcess = headPtr;
            // Loop through the list until the end
            while (tempProcess != NULL) 
            {
                // Add the process to the queue
                roundRobinQueue(ENQUEUE, tempProcess->pid);
                // Move to the next node
                tempProcess = tempProcess->nextProcess;
            }
            // Set the initialized flag
            initialized = true;
        }
        // Get the next process ID from the queue
        processID = roundRobinQueue(DEQUEUE, EMPTY);
        // Check if the process ID is empty
        if (processID == EMPTY) 
        {
            // Return null if the process ID is empty
            return NULL;
        }
        // Loop through the list until the end
        while (headPtr != NULL && headPtr->pid != processID) 
        {
            // Move to the next node
            headPtr = headPtr->nextProcess;
        }
        // Return the selected process
        wkgPtr = headPtr;
    }
    //if the CPU scheduling code is FCFS-N or FCFS-P
    else if (configPtr->cpuSchedCode == CPU_SCHED_FCFS_P_CODE ||
               configPtr->cpuSchedCode == CPU_SCHED_FCFS_N_CODE)
    {
        // Loop through the list until the end
        while (wkgPtr != NULL && wkgPtr->state == EXIT)
        {
            // Move to the next node
            wkgPtr = wkgPtr->nextProcess;
        }
    }
    // Return the selected process
    return wkgPtr;
}

/*
Name: getNextProcFCFS
Process: retrieves the next PCB from the front of the queue (First-Come, First-Served)
Function Input/Parameters: queue: pointer to the queue structure (Queue *)
Function Output/Parameters: none
Function Output/Returned: pointer to the next PCB from the front of the queue (PCB *)
Dependencies: dequeue
*/
PCB* getNextProcFCFS(Queue *queue) 
{
    // Return the dequeued PCB
    return dequeue(queue);
}


/*
Name: initQueue
Process: initializes the queue by setting front and rear pointers to NULL
Function Input/Parameters: queue: pointer to the queue structure (Queue *)
Function Output/Parameters: none
Function Output/Returned: none
Dependencies: none
*/
void initQueue(Queue *queue)
{
    // Set front and rear pointers to null
    queue->front = queue->rear = NULL;
}


/*
Name: logToFile
Process: writes the contents of FileStrList to a log file specified in the configPtr
Function Input/Parameters: configPtr: pointer to the configuration data (ConfigDataType *)
                           FileStrList: pointer to the list of file strings (FileStr *)
Function Output/Parameters: none
Function Output/Returned: none
Dependencies: fopen, fprintf, fclose
*/
void logToFile(ConfigDataType *configPtr, FileStr *FileStrList)
{
    // Initialize variables
    const char WRITE_FLAG[] = "w";
    FILE *file;
    FileStr *tempListPtr = FileStrList;

    // Open the log file
    file = fopen(configPtr->logToFileName, WRITE_FLAG);
    // Check if the file is null
    if (file == NULL)
    {
        // Print an error message
        printf("\nOutput Error\n");
    }
    // Otherwise, write the contents of the file list to the log file
    else
    {
        // Loop through the list until the end
        while (tempListPtr != NULL)
        {
            // Write the current string to the file
            fprintf(file, "%s", tempListPtr->currentStr);
            //  Move to the next node
            tempListPtr = tempListPtr->nextStr;
        }
        //  Close the file
        fclose(file);
    }
}



/*
Name: roundRobinQueue
Process: performs enqueue and dequeue operations on a circular queue
Function Input/Parameters: operation: type of operation (ENQUEUE or DEQUEUE)
                           processID: process ID to be enqueued or dequeued
Function Output/Parameters: none
Function Output/Returned: process ID (int) if dequeued, 
EMPTY if queue is full or empty, or if operation is invalid
Dependencies: printf
Doesn't work Madge
*/
int roundRobinQueue(int operation, int processID) 
{
    // Initialize variables
    int MAX_QUEUE_SIZE = 100;
    int queue[MAX_QUEUE_SIZE];
    static int front = -1, rear = -1; 

    // Check operation type if enqueue
    if (operation == ENQUEUE) 
    {
        // Check if queue is full
        if ((front == EMPTY && rear == MAX_QUEUE_SIZE - 1) || 
                     (rear == (front - 1) % (MAX_QUEUE_SIZE - 1))) 
        {
            // Print error message if queue is full
            printf("Queue is full\n");
            // Return EMPTY if queue is full
            return EMPTY;
        }
        // Check if the queue is initially empty
        else if (front == EMPTY -1)
        {
            // Set front and rear to 0
            front = rear = 0;
            // Add process to the front of the queue
            queue[rear] = processID;
        }
        // Otherwise, add process to the rear of the queue
        else 
        {
            // Move rear to the next element
            rear = (rear + 1) % MAX_QUEUE_SIZE;
            // Add process to the rear of the queue
            queue[rear] = processID;
        }
        // Return the process ID
        return processID;
    } 

    //otherwise if the operation is dequeue
    else if (operation == DEQUEUE) 
    {
        // Check if queue is empty
        if (front == EMPTY -1) 
        {
            // Print error message if queue is empty
            printf("Queue is empty\n");
            // Return EMPTY if queue is empty
            return EMPTY;
        }
        // gett and remove the front element from the queue
        int processID = queue[front];
        // Check if only one element is present
        if (front == rear) 
        {
            // Set front and rear to -1
            front = rear = -1;
        }
        // Otherwise, move front to the next element
        else 
        {
            // Move front to the next element
            front = (front + 1) % MAX_QUEUE_SIZE;
        }
        // Return the process ID
        return processID;
    }
    // Return EMPTY if operation is WRONG llllll
    return EMPTY; 
}


/*
Name: runSim
Process: primary simulation driver
Function Input/Parameters: configPtr: pointer to the configuration data (ConfigDataType)
         metaDataMstrPtr: pointer to the metadata (OpCodeType)
Function Output/Parameters: none
Function Output/Returned: none
Dependencies: alot
*/
void runSim(ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr)
{
    // Initialize variables
    int timeToRun, currentPid = 0;
    PCB *processList = NULL, *wkgProcess = NULL;
    OpCodeType *mtData = metaDataMstrPtr;
    char timer[MIN_STR_LEN];
    char getMessage[STD_STR_LEN];
    FileStr *fPtr = NULL;
    bool notBrokenFlag = false;
    Queue *processQueue = NULL;
    initQueue(processQueue);
    pthread_t timerThread;


    // display start Simulator message
    printf("Simulator Run\n");
    printf("-------------\n\n");
    //start da timerr
    accessTimer(ZERO_TIMER, timer);
    //copy the message
    copyString(getMessage, ", OS: Simulator start\n");
    //display the message to the correct location
    fPtr = displayOutput(configPtr, getMessage, fPtr);


    //MASTER  LOOOP
    // Loop through the metadata until the end
    while (compareString(mtData->command, "sys") != STR_EQ ||
         compareString(mtData->strArg1, "end") != STR_EQ)
    {
        // Get the next process
        //in the queue
        //DOESN'T WORK
        wkgProcess = getNextProcFCFS(processQueue);
        //loop thru the metadata
            while (compareString(mtData->command, "sys") != STR_EQ ||
                 compareString(mtData->strArg1, "end") != STR_EQ)
            {
                // Check if the current code is an application start
                mtData->pid = currentPid;

                // Check if the current code is an application start
                if (compareString(mtData->command, "app") == STR_EQ &&
                    compareString(mtData->strArg1, "start") == STR_EQ)
                {
                    // Change the process state to ready
                    processList = addToProcessList(currentPid, mtData, processList);

                    // Add the process to the queue for Preemptive
                    enqueue( processQueue, processList);
                    // Get the next process depeding on the CPU scheduling code
                    wkgProcess = getNextProc(configPtr, processList);
                }

                // Check if the current code is at CPU
                if (compareString(mtData->command, "cpu") == STR_EQ)
                {
                    // Add the time to the process
                    wkgProcess->timeLeft += mtData->intArg2 * configPtr->procCycleRate;
                }
                // Check if the current code is at I/O
                if (compareString(mtData->command, "dev") == STR_EQ)
                {
                    // Add the time to the process
                    wkgProcess->timeLeft += mtData->intArg2 * configPtr->ioCycleRate;
                }
                //if we are at the end
                if (compareString(mtData->command, "app") == STR_EQ &&
                    compareString(mtData->strArg1, "end") == STR_EQ)
                {
                    // Change the process state to ready
                    wkgProcess = changeProcessState(wkgProcess, READY, getMessage);

                    // incremement the piddy
                    currentPid++;
                    // display the message acordingly
                    fPtr = displayOutput(configPtr, getMessage, fPtr);
                }
                // Get the next OpCodeType structure
                mtData = mtData->nextNode;
            }

        // Get the next Process structure
        wkgProcess = getNextProc(configPtr, processList);

        // Get the next OpCodeType structure
        mtData = getNextOpCode(wkgProcess);
        // if we are at the start of the process
        if (compareString(mtData->command, "app") == STR_EQ &&
            compareString(mtData->strArg1, "start") == STR_EQ)
        {
            //single to the systemt hat we are not broken yet
            notBrokenFlag = false;
            //save the needed messages
            sprintf(getMessage,
                ", OS: Process %d selected with %d ms remaining\n",
                wkgProcess->pid, wkgProcess->timeLeft);
            //take that message and display it to the correct location
            fPtr = displayOutput(configPtr, getMessage, fPtr);
            //change the process state to running
            wkgProcess = changeProcessState(wkgProcess, RUNNING, getMessage);
            //display the message to the correct location
            fPtr = displayOutput(configPtr, getMessage, fPtr);
            //if it is ONYL TO THE FILE
            if (configPtr->logToCode == LOGTO_FILE_CODE)
            {
                //dispaly the message to the correct location
                printf("Simulator running for output to file only.\n");
            }
        }
        //if the current code is at the end and it's NOT broken
        else if ((compareString(mtData->command, "app") == STR_EQ &&
                 compareString(mtData->strArg1, "end") == STR_EQ) ||
                notBrokenFlag)
        {
            //print and endline for pretty 
            printf("\n");
            //if the log to code is both or file
            if (configPtr->logToCode == LOGTO_BOTH_CODE ||
            configPtr->logToCode == LOGTO_FILE_CODE)
            {
                //add a new line to the file list
                fPtr = addToFileList("\n", fPtr);
            }
            //if the system is nor broken
            if (notBrokenFlag)
            {
                //now it is broken
                notBrokenFlag = false;

                //display the message to the correct location
                sprintf(getMessage, ", OS: Segmentation fault, Process %d ended\n",
                    wkgProcess->pid);
                //display the message to the correct location
                fPtr = displayOutput(configPtr, getMessage, fPtr);
            }
            //otherwise it is safe
            else
            {
                //grab the message
                sprintf(getMessage, ", OS: Process %d ended\n", wkgProcess->pid);
            }
            //display the message to the correct location
            fPtr = displayOutput(configPtr, getMessage, fPtr);
            //change the process state to exit
            wkgProcess = changeProcessState(wkgProcess, EXIT, getMessage);
            //display the message to the correct location
            fPtr = displayOutput(configPtr, getMessage, fPtr);
        }
        //if we are in the op codes and it's in out
        else if (compareString(mtData->command, "dev") == STR_EQ &&
             wkgProcess->state != EXIT)
        {
            //decode the op code
            decodeOpCode(mtData, getMessage, "start");
            //display the message to the correct location
            fPtr = displayOutput(configPtr, getMessage, fPtr);
            //set the time to run
            timeToRun = mtData->intArg2 * configPtr->ioCycleRate;
            //fake a wait time
            pthread_create(&timerThread, NULL, runTimer, &timeToRun);
            //THIS USE TO BE PTHREAD_JOIN BUT NOW IT'S DETACHED
            pthread_detach(timerThread);
            //this runs the time and pretends to wait
            delay(timeToRun);

            //decode the op code
            decodeOpCode(mtData, getMessage, "end");
            //display the message to the correct location
            fPtr = displayOutput(configPtr, getMessage, fPtr);
            //set the time left
            wkgProcess->timeLeft -= timeToRun;
        }
        //if we are in the op codes and it's in cpu
        else if (compareString(mtData->command, "cpu") == STR_EQ &&
             wkgProcess->state != EXIT)
        {
            //decode the op code
            decodeOpCode(mtData, getMessage, "start");
            //display the message to the correct location
            fPtr = displayOutput(configPtr, getMessage, fPtr);
            //set the time to run
            timeToRun = mtData->intArg2 * configPtr->procCycleRate;
            //fake a wait time
            pthread_create(&timerThread, NULL, runTimer, &timeToRun);
            //THIS USE TO BE PTHREAD_JOIN BUT NOW IT'S DETACHED
            pthread_detach(timerThread);
            //this runs the time and pretends to wait
            delay(timeToRun);

            //decode the op code
            decodeOpCode(mtData, getMessage, "end");
            //display the message to the correct location
            fPtr = displayOutput(configPtr, getMessage, fPtr);
            //set the time left
            wkgProcess->timeLeft -= timeToRun;
        }
        // if we are at the end
        else if (compareString(mtData->command, "sys") == STR_EQ &&
             compareString(mtData->strArg1, "end") == STR_EQ)
        {
            //get the message to stop the system
            copyString(getMessage, ", OS: System stop\n");
            //display the message to the correct location
            fPtr = displayOutput(configPtr, getMessage, fPtr);
        }
        // increase the counter
        wkgProcess->counter++;
    }
    //get the message to stop the simulation
    copyString(getMessage, ", OS: Simulation end\n");
    //display the message to the correct location
    fPtr = displayOutput(configPtr, getMessage, fPtr);

    //if the log to code is both or monitor
    if (configPtr->logToCode == LOGTO_BOTH_CODE ||
    configPtr->logToCode == LOGTO_FILE_CODE)
    {
        //log the messages to file
        logToFile(configPtr, fPtr);
    }
    //STAHP the timer
    accessTimer(STOP_TIMER, timer);
    //free all the lists 
    fPtr = freeFileList(fPtr);
    processList = freePCBList(processList);
}

/*
Name: searchProcessId
Process: Searches for a process with the given PID in a linked list of PCBs.
Function Input/Parameters: headptr: Pointer to the head of the PCB linked list (PCB *)
                           pid: Process ID to search for (int)
Function Output/Parameters: None
Function Output/Returned: Pointer to the PCB with the given PID if found, otherwise NULL (PCB *)
Dependencies: None
*/
PCB *searchProcessId(PCB *headptr, int pid)
{
    // Initialize variables
    PCB *wkgPtr = headptr;
    // Loop through the list until the end
    while (wkgPtr != NULL)
    {
        // Check if the current PID matches the input PID
        if (wkgPtr->pid == pid)
        {
            // Return the PCB with the given PID
            return wkgPtr;
        }
        // Move to the next node
        wkgPtr = wkgPtr->nextProcess;
    }
    // Return null if the PID is not found
    return NULL;
}

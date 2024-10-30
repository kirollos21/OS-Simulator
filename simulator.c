/**
 * Driver code for running the simulator
 */

#include "simulator.h"

/**
 * Copies the contents of a source ConfigDataType struct to a destination
 * ConfigDataType struct
 * @param dest Pointer to the destination ConfigDataType struct
 * @param src Pointer to the source ConfigDataType struct
 * @return void
 */
void copyConfigData(ConfigDataType *dest, ConfigDataType *src)
{
  dest->version = src->version;
  copyString(dest->metaDataFileName, src->metaDataFileName);
  dest->cpuSchedCode = src->cpuSchedCode;
  dest->quantumCycles = src->quantumCycles;
  dest->memDisplay = src->memDisplay;
  dest->memAvailable = src->memAvailable;
  dest->procCycleRate = src->procCycleRate;
  dest->ioCycleRate = src->ioCycleRate;
  dest->logToCode = src->logToCode;
  copyString(dest->logToFileName, src->logToFileName);
}

/**
 * Add a new LOGnode to a linked list of LOGnodes.
 * @param local_ptr Pointer to the current node in the linked list.
 * @param txt_input String containing the log data to add to the new node.
 * @return LOGnode* Pointer to the updated linked list
 */
LOGnode *LOGnode_add(LOGnode *local_ptr, char *txt_input)
{
  LOGnode *new_ptr;
  if (local_ptr)
  {
    local_ptr->next_ptr = LOGnode_add(local_ptr->next_ptr, txt_input);
    return local_ptr;
  }
  else
  {
    new_ptr = (LOGnode *)malloc(sizeof(LOGnode));
    copyString(new_ptr->LOG_out, txt_input);
    new_ptr->next_ptr = NULL;
    return new_ptr;
  }
}

/**
 * Deletes the linked list of LOG nodes recursively.
 * @param local_ptr The pointer to the first LOG node in the linked list.
 * @return Returns a NULL pointer after deleting all nodes in the linked list.
 */
LOGnode *LOGnode_del(LOGnode *local_ptr)
{
  if (!local_ptr)
  {
    return ZERO;
  }
  else
  {
    LOGnode_del(local_ptr->next_ptr);
  }

  free(local_ptr);
  return ZERO;
}

/**
 * Dumps the log information to a file or monitor.
 * @param trigger Integer value that represents the trigger point of the log
 * operation.
 * @param config_dataptr Pointer to a ConfigDataType structure that contains the
 * configuration data for the simulation.
 * @param txt_input Pointer to a character string that contains the text to be
 * logged.
 * @return void
 */
void LOGdump(int trigger, ConfigDataType *config_dataptr, char *txt_input)
{
  char outputString[MAX_STR_LEN];
  char timeString[MIN_STR_LEN];
  char WRITE_FLAG[TWO];
  int temp_trigger;
  FILE *outFilePtr;
  static LOGnode *log_ptr = NULL;
  LOGnode *wkg_log_ptr = NULL;

  copyString(WRITE_FLAG, "w");
  // check if start of line = OS:
  getSubString(outputString, txt_input, ZERO, TWO);
  copyString(outputString, "\n ");

  if (trigger == INIT_LOG)
  {
    log_ptr = NULL;
    accessTimer(LAP_TIMER, timeString);
    temp_trigger = ONE;

    if (config_dataptr->logToCode == LOGTO_FILE_CODE)
    {
      printf("Simulator running with output to file\n");
    }
  }
  else
  {
    accessTimer(LAP_TIMER, timeString);
    temp_trigger = trigger;
  }

  if (temp_trigger == ADD_LOG)
  {
    concatenateString(outputString, timeString);
    concatenateString(outputString, ", ");
    concatenateString(outputString, txt_input);
    if (config_dataptr->logToCode == LOGTO_FILE_CODE ||
        config_dataptr->logToCode == LOGTO_BOTH_CODE)
    {
      log_ptr = LOGnode_add(log_ptr, outputString);
    }

    if (config_dataptr->logToCode == LOGTO_BOTH_CODE ||
        config_dataptr->logToCode == LOGTO_MONITOR_CODE)
    {
      printf(outputString);
    }
  }
  else
  {
    if (config_dataptr->logToCode == LOGTO_FILE_CODE ||
        config_dataptr->logToCode == LOGTO_BOTH_CODE)
    {
      wkg_log_ptr = log_ptr;
      outFilePtr = fopen(config_dataptr->logToFileName, WRITE_FLAG);
      fprintf(outFilePtr,
              "\n==================================================\n");
      fprintf(outFilePtr, "Simulator Log File Header\n\n");
      fprintf(outFilePtr, "File Name                       : %s\n",
              config_dataptr->metaDataFileName);
      configCodeToString(config_dataptr->cpuSchedCode, outputString);
      fprintf(outFilePtr, "CPU Scheduling                  : %s\n",
              outputString);
      fprintf(outFilePtr, "Quantum Cycles                  : %d\n",
              config_dataptr->quantumCycles);
      fprintf(outFilePtr, "Memory Available (KB)           : %d\n",
              config_dataptr->memAvailable);
      fprintf(outFilePtr, "Processor Cycle Rate (ms/cycle) : %d\n",
              config_dataptr->procCycleRate);
      fprintf(outFilePtr, "I/O Cycle Rate (ms/cycle)       : %d\n\n",
              config_dataptr->ioCycleRate);
      fprintf(outFilePtr, "================\n");
      fprintf(outFilePtr, "Begin Simulation\n\n");

      while (wkg_log_ptr)
      {
        fprintf(outFilePtr, "%s", wkg_log_ptr->LOG_out);
        wkg_log_ptr = wkg_log_ptr->next_ptr;
      }

      fprintf(outFilePtr, "\nEnd Simulation - Complete\n");
      fprintf(outFilePtr, "=========================\n\n");

      fclose(outFilePtr);
      log_ptr = LOGnode_del(log_ptr);
    }
  }
}

/*
 * Adds a new PCB node to the linked list in ascending order of the start time
 * of its OpCodeList.
 * @param local_ptr A pointer to the head of the linked list to add the new node
 * to.
 * @param new_ptr A pointer to the new PCB node to add to the linked list.
 * @return A pointer to the head of the updated linked list.
 */
PCBdata *PCBnode_add(PCBdata *local_ptr, PCBdata *new_ptr)
{
  PCBdata *wkg_ptr;

  if (local_ptr && new_ptr->OClist->intArg2 >= local_ptr->OClist->intArg2)
  {
    for (wkg_ptr = local_ptr;
         wkg_ptr->next_ptr &&
         new_ptr->OClist->intArg2 >= wkg_ptr->next_ptr->OClist->intArg2;
         wkg_ptr = wkg_ptr->next_ptr)
    {
      ;
    }
    new_ptr->next_ptr = wkg_ptr->next_ptr;
    wkg_ptr->next_ptr = new_ptr;
  }
  else
  {
    new_ptr->next_ptr = local_ptr;
    return new_ptr;
  }
  return local_ptr;
}

/**
 * Deletes all the nodes in the linked list pointed to by local_ptr.
 * @param local_ptr A pointer to the head of the linked list to be deleted.
 * @return PCBdata* A null pointer is returned.
 */
PCBdata *PCBnode_del(PCBdata *local_ptr)
{
  if (local_ptr)
  {
    PCBnode_del(local_ptr->next_ptr);
    free(local_ptr);
  }
  return ZERO;
}

/**
 * Finds a PCB node with the given PID.
 * @param local_ptr Pointer to the head node of the PCB linked list.
 * @param PCB_PID The PID to search for in the PCB linked list.
 * @return A pointer to the PCB node with the specified PID, or NULL if not
 * found.
 */
PCBdata *PCBnode_pid(PCBdata *local_ptr, int PCB_PID)
{
  while (local_ptr)
  {
    if (local_ptr->pid == PCB_PID)
    {
      return local_ptr;
    }
    local_ptr = local_ptr->next_ptr;
  }
  return ZERO;
}

/**
 * Creates a Process Control Block (PCB) for each application in the given
 * metadata.
 * @param config_dataptr A pointer to the configuration data.
 * @param md_head_ptr A pointer to the metadata for the current run.
 * @return A pointer to the head of the PCB list.
 */
PCBdata *PCBcreate(ConfigDataType *config_dataptr, OpCodeType *md_head_ptr)
{
  PCBdata *new_ptr;
  PCBdata *head_ptr = NULL;
  int op_time = ZERO;
  int pidSet = NEW_STATE;

  if (compareString(md_head_ptr->command, "sys") ||
      compareString(md_head_ptr->strArg1, "start"))
  {
    return ZERO;
  }

  for (md_head_ptr = md_head_ptr->next_ptr;
       compareString(md_head_ptr->command, "sys") &&
       compareString(md_head_ptr->strArg1, "end");
       md_head_ptr = md_head_ptr->next_ptr)
  {
    if (!compareString(md_head_ptr->command, "app") &&
        !compareString(md_head_ptr->strArg1, "start"))
    {
      new_ptr = (PCBdata *)malloc(sizeof(PCBdata) + 1);
      new_ptr->pid = pidSet;
      new_ptr->state = NEW_STATE;
      new_ptr->time_left = ZERO;
      new_ptr->time_start = md_head_ptr->intArg2;
      new_ptr->quant_time = true;
      new_ptr->OCcurr = NULL;
      new_ptr->OClist = md_head_ptr;
      new_ptr->next_ptr = NULL;

      while (compareString(md_head_ptr->command, "app") ||
             compareString(md_head_ptr->strArg1, "end"))
      {
        if (compareString(md_head_ptr->command, "mem"))
        {
          if (compareString(md_head_ptr->command, "dev"))
          {
            op_time = md_head_ptr->intArg2 * config_dataptr->procCycleRate;
          }

          else
          {
            op_time = md_head_ptr->intArg2 * config_dataptr->ioCycleRate;
          }

          new_ptr->time_left = new_ptr->time_left + op_time;

          if (config_dataptr->cpuSchedCode &&
              config_dataptr->cpuSchedCode != CPU_SCHED_FCFS_N_CODE)
          {
            md_head_ptr->intArg3 = PREEMPTIVE_CODE;
          }

          else
          {
            md_head_ptr->intArg3 = NON_PREEMPTIVE_CODE;
          }
        }
        md_head_ptr->pid = pidSet;
        md_head_ptr = md_head_ptr->next_ptr;
      }
      head_ptr = PCBnode_add(head_ptr, new_ptr);
      ++pidSet;
    }
  }
  return head_ptr;
  free(new_ptr);
}

/**
 * Parse the PCB linked list to find the next process ID to execute based on the
 * scheduling algorithm
 * @param config_dataptr A pointer to the configuration data struct
 * @param local_ptr A pointer to the head of the PCB linked list
 * @return int The PID of the next process to execute
 */
int PCBparse(ConfigDataType *config_dataptr, PCBdata *local_ptr)
{
  int PCBid = ZERO;
  int minTime = ZERO;
  int minId = NULL_PID;
  int loopCount = ZERO;
  int newBlockedCount = ZERO;

  if (config_dataptr->cpuSchedCode == CPU_SCHED_FCFS_P_CODE ||
      config_dataptr->cpuSchedCode == CPU_SCHED_FCFS_N_CODE)
  {
    while (local_ptr)
    {
      if (local_ptr->state == READY_STATE)
      {
        return local_ptr->pid;
      }
      if (local_ptr->state == BLOCKED_STATE || !local_ptr->state)
      {
        ++newBlockedCount;
      }
      local_ptr = local_ptr->next_ptr;
    }
  }
  else if (config_dataptr->cpuSchedCode == CPU_SCHED_RR_P_CODE)
  {
    local_ptr = PCBnode_pid(local_ptr, PCBid);
    if (local_ptr->state != BLOCKED_STATE && local_ptr->state != EXIT_STATE &&
        !local_ptr->quant_time)
    {
      local_ptr->quant_time = true;
      return local_ptr->pid;
    }
    local_ptr->quant_time = true;

    if (local_ptr->next_ptr)
      local_ptr = local_ptr->next_ptr;

    else
      local_ptr = local_ptr;

    PCBid = local_ptr->pid;

    while (loopCount < TWO)
    {
      local_ptr = PCBnode_pid(local_ptr, PCBid);

      if (local_ptr->state == READY_STATE)
      {
        return local_ptr->pid;
      }
      if (local_ptr->state == BLOCKED_STATE || !local_ptr->state)
        ++newBlockedCount;

      if (local_ptr->next_ptr)
      {
        local_ptr = local_ptr->next_ptr;
      }
      else
      {
        ++loopCount;
      }
      PCBid = local_ptr->pid;
    }
  }
  else
  {
    while (local_ptr && local_ptr->state != READY_STATE)
    {
      if (!local_ptr->state || local_ptr->state == BLOCKED_STATE)
      {
        ++newBlockedCount;
      }

      local_ptr = local_ptr->next_ptr;
    }

    if (local_ptr)
    {
      minId = local_ptr->pid;
      minTime = local_ptr->time_left;
      local_ptr = local_ptr->next_ptr;
    }

    while (local_ptr)
    {
      if (local_ptr->state == READY_STATE && local_ptr->time_left < minTime)
      {
        minId = local_ptr->pid;
        minTime = local_ptr->time_left;
      }
      else if (local_ptr->state == BLOCKED_STATE || !local_ptr->state)
      {
        ++newBlockedCount;
      }
      local_ptr = local_ptr->next_ptr;
    }
  }

  if (minId < ZERO)
  {
    if (newBlockedCount < ONE)
    {
      minId = EXIT;
    }
    else
    {
      minId = WAIT;
    }
  }

  return minId;
}

/**
 * Updates the state of all PCBs in a linked list that are in the NEW_STATE to
 * READY_STATE if their start time has passed.
 * @param config_dataptr A pointer to the configuration data struct.
 * @param local_ptr A pointer to the head of the linked list of PCBs.
 * @return void.
 */
void PCBstate(ConfigDataType *config_dataptr, PCBdata *local_ptr)
{
  char timeString[MAX_STR_LEN];
  char reportString[MAX_STR_LEN];
  double sim_time = 0.0;
  PCBdata *PCB_ptr;

  for (PCB_ptr = local_ptr; PCB_ptr != NULL; PCB_ptr = PCB_ptr->next_ptr)
  {
    if (PCB_ptr->state == NEW_STATE)
    {
      sim_time = accessTimer(LAP_TIMER, timeString);
      if (PCB_ptr->time_start <= sim_time)
      {
        PCB_ptr->state = READY_STATE;
        sprintf(reportString,
                "OS: Process %d set to READY state from NEW state",
                PCB_ptr->pid);
        LOGdump(ADD_LOG, config_dataptr, reportString);
      }
    }
  }
}

/**
 * Displays the contents of a linked list of Process Control Blocks (PCBs).
 * @param head_ptr A pointer to the head of the linked list of PCBs.
 * @return void.
 */
void PCBdisplay(PCBdata *head_ptr)
{
  PCBdata *wkg_ptr;

  if (head_ptr)
  {
    while (wkg_ptr)
    {
      printf("PCB pid: %d, state: %d, time remaining: %d,\n", wkg_ptr->pid,
             wkg_ptr->state, wkg_ptr->time_left);

      printf("\tcurrent command: %s, ", wkg_ptr->OClist->command);

      printf("\tcurrent op code: %s, \n", wkg_ptr->OClist->strArg1);

      printf("\tfirst int arg: %d, second int arg: %d\n\n",
             wkg_ptr->OClist->intArg2, wkg_ptr->OClist->intArg3);

      wkg_ptr = wkg_ptr->next_ptr;
    }
  }
  else
  {
    printf("\n[-] INVALID PCB LIST\n");
  }
}

/**
 * Executes a process according to its cycle time and quantum time.
 * @param CNF_ptr A pointer to the configuration data structure.
 * @param OPC_ptr A pointer to the operation code data structure.
 * @param PCB_ptr A pointer to the process control block data structure.
 */
void PROCthread(ConfigDataType *CNF_ptr, OpCodeType *OPC_ptr, PCBdata *PCB_ptr)
{
  char reportString[MAX_STR_LEN];
  PCBdata *PCB_wkg_ptr;
  int oneCycle, cyclesToRun, quantumCount;
  _Bool isPreemptive, cont;
  _Bool interrupt = false;

  oneCycle = CNF_ptr->procCycleRate;
  quantumCount = CNF_ptr->quantumCycles;
  cyclesToRun = OPC_ptr->intArg2;
  cont = true;
  isPreemptive = (OPC_ptr->intArg3 == PREEMPTIVE_CODE);
  PCB_wkg_ptr = PCBnode_pid(PCB_ptr, OPC_ptr->pid);

  while (cont)
  {
    runTimer(oneCycle);
    PCB_wkg_ptr->time_left -= oneCycle;
    --cyclesToRun;
    --quantumCount;
    interrupt = interruptMNGR(INTERRUPT_CHECK, OPC_ptr, PCB_ptr, CNF_ptr);
    if ((!cyclesToRun || isPreemptive) && (!quantumCount || interrupt))
    {
      cont = false;
    }
  }

  if (cyclesToRun > ZERO && isPreemptive && !interrupt)
  {
    PCB_wkg_ptr->quant_time = true;
    sprintf(reportString,
            "OS: Process %d quantum time out, cpu process operation end",
            OPC_ptr->pid);
    LOGdump(ADD_LOG, CNF_ptr, reportString);
  }
  else
  {
    sprintf(reportString, "Process: %d, %s %s operation end", OPC_ptr->pid,
            OPC_ptr->command, OPC_ptr->strArg1);
    LOGdump(ADD_LOG, CNF_ptr, reportString);
  }

  OPC_ptr->intArg2 = cyclesToRun;
}

/**
 * Runs I/O operation on a process based on its operation type.
 * @param config_dataptr Pointer to configuration data
 * @param OPC_ptr Pointer to the OpCodeType struct for the process
 * @param PCB_ptr Pointer to the PCBdata struct for the process
 * @return void
 */
void IOthread(ConfigDataType *config_dataptr, OpCodeType *OPC_ptr,
              PCBdata *PCB_ptr)
{
  char reportString[MAX_STR_LEN];
  PCBdata *local_ptr;

  if (OPC_ptr->intArg3 == NON_PREEMPTIVE_CODE)
  {
    runTimer(OPC_ptr->intArg2);
    sprintf(reportString, "Process: %d, %s %sput operation end", OPC_ptr->pid,
            OPC_ptr->strArg1, OPC_ptr->inOutArg);
    LOGdump(ADD_LOG, config_dataptr, reportString);
  }
  else
  {
    sprintf(reportString, "OS: Process %d blocked for %sput operation",
            OPC_ptr->pid, OPC_ptr->inOutArg);
    LOGdump(ADD_LOG, config_dataptr, reportString);

    local_ptr = PCBnode_pid(PCB_ptr, OPC_ptr->pid);
    local_ptr->state = BLOCKED_STATE;

    sprintf(reportString, "OS: Process %d set from RUNNING to BLOCKED",
            OPC_ptr->pid);
    LOGdump(ADD_LOG, config_dataptr, reportString);
    interruptMNGR(SET_IO_START, OPC_ptr, PCB_ptr, config_dataptr);
  }
}

/**
 * A wrapper function that is passed to the pthread_create function to create an
 * IO thread.
 * @param arg A pointer to an array of three void pointers containing
 * ConfigDataType, OpCodeType and PCBdata structs in that order.
 * @return void* This function does not return anything.
 */
void *IOthread_wrapper(void *arg)
{
  // refer to IOargs in simulator.h for definitions on thread args
  void **args = (void **)arg;
  ConfigDataType *config_dataptr = (ConfigDataType *)args[IO_ARG_ONE];
  OpCodeType *OPC_ptr = (OpCodeType *)args[IO_ARG_TWO];
  PCBdata *PCB_ptr = (PCBdata *)args[IO_ARG_THREE];

  IOthread(config_dataptr, OPC_ptr, PCB_ptr);

  pthread_exit(NULL);
}

/**
 * Copies the content of one OpCodeType struct to another.
 * @param destination A pointer to the destination OpCodeType struct.
 * @param source A pointer to the source OpCodeType struct.
 * @return void
 */
void OPCcopy(OpCodeType *destination, OpCodeType *source)
{
  destination->pid = source->pid;
  copyString(destination->command, source->command);
  copyString(destination->inOutArg, source->inOutArg);
  copyString(destination->strArg1, source->strArg1);
  destination->intArg2 = source->intArg2;
  destination->intArg3 = source->intArg3;
  destination->opEndTime = source->opEndTime;
  destination->next_ptr = source->next_ptr;
  return;
}

/**
 * Adds a new node with interrupt OpCodeType to the end of the linked list.
 * @param local_ptr Pointer to the head of the linked list of OpCodeType.
 * @param new_ptr Pointer to the new OpCodeType node to be added.
 * @return OpCodeType* Pointer to the head of the updated linked list of
 * OpCodeType.
 */
OpCodeType *addInterrupt(OpCodeType *local_ptr, OpCodeType *new_ptr)
{
  OpCodeType *addedNode;

  if (local_ptr)
  {
    local_ptr->next_ptr = addInterrupt(local_ptr->next_ptr, new_ptr);
    return local_ptr;
  }
  else
  {
    addedNode = (OpCodeType *)malloc(sizeof(OpCodeType));
    OPCcopy(addedNode, new_ptr);
    addedNode->next_ptr = NULL;
    return addedNode;
  }
}

/**
 * Removes a node from a linked list of OpCodeType nodes.
 * @param headPtr Pointer to the head node of the linked list of OpCodeType
 * nodes.
 * @param removedPtr Pointer to the node to be removed.
 * @return OpCodeType* Pointer to the head node of the updated linked list.
 */
OpCodeType *removeOpCodeNode(OpCodeType *headPtr, OpCodeType *removedPtr)
{
  OpCodeType *wkgPtr;

  wkgPtr = headPtr;
  if (removedPtr == headPtr)
  {
    headPtr = headPtr->next_ptr;
  }
  else
  {
    for (; wkgPtr->next_ptr != removedPtr; wkgPtr = wkgPtr->next_ptr)
    {
    }
    wkgPtr->next_ptr = wkgPtr->next_ptr->next_ptr;
    free(removedPtr);
  }
  return headPtr;
}

/**
 * Manages interrupts and performs necessary operations.
 * @param CTRL_ptr Pointer to an Interrupts object.
 * @param OPC_ptr Pointer to an OpCodeType object.
 * @param PCB_ptr Pointer to a PCBdata object.
 * @param config_dataptr Pointer to a ConfigDataType object.
 * @return Boolean value indicating whether an interrupt was handled.
 */
_Bool interruptMNGR(Interrupts CTRL_ptr, OpCodeType *OPC_ptr, PCBdata *PCB_ptr,
                    ConfigDataType *config_dataptr)
{
  char reportString[MAX_STR_LEN];
  double currentTime = 0.0;
  PCBdata *local_ptr = NULL;
  _Bool returnVal = false;
  OpCodeType *delOPC = NULL;
  OpCodeType *wkgOPC_ptr = NULL;

  delOPC = NULL;
  returnVal = false;

  if (CTRL_ptr)
  {
    switch (CTRL_ptr)
    {
    case INIT_MNGR:
      wkgOPC_ptr = NULL;
      returnVal = true;

    case INTERRUPT_CHECK:
      wkgOPC_ptr = OPC_ptr;
      while (wkgOPC_ptr)
      {
        local_ptr = PCBnode_pid(PCB_ptr, wkgOPC_ptr->pid);
        currentTime = accessTimer(LAP_TIMER, reportString);
        if (currentTime > wkgOPC_ptr->opEndTime)
        {
          if (local_ptr->state != EXIT_STATE)
          {
            returnVal = true;
          }
          delOPC = wkgOPC_ptr;
        }
        if (delOPC)
        {
          wkgOPC_ptr = removeOpCodeNode(wkgOPC_ptr, delOPC);
          delOPC = NULL;
        }
        else
        {
          wkgOPC_ptr = wkgOPC_ptr->next_ptr;
        }
      }
      break;

    case SET_IO_START:
      currentTime = accessTimer(LAP_TIMER, reportString);
      OPC_ptr->opEndTime = OPC_ptr->intArg2 / 1000.0 + currentTime;
      wkgOPC_ptr = addInterrupt(wkgOPC_ptr, OPC_ptr);
      break;

    case RESOLVE_INTERRUPTS:
      wkgOPC_ptr = OPC_ptr;

      while (wkgOPC_ptr)
      {
        local_ptr = PCBnode_pid(PCB_ptr, wkgOPC_ptr->pid);
        currentTime = accessTimer(LAP_TIMER, reportString);

        if (currentTime > wkgOPC_ptr->opEndTime)
        {
          if (local_ptr->state != EXIT_STATE)
          {
            sprintf(reportString,
                    "OS: Interrupted by process %d, %s %sput operation",
                    wkgOPC_ptr->pid, wkgOPC_ptr->strArg1, wkgOPC_ptr->inOutArg);
            LOGdump(ADD_LOG, config_dataptr, reportString);
            local_ptr->state = READY_STATE;
            sprintf(reportString, "OS: Process %d set from BLOCKED to READY",
                    wkgOPC_ptr->pid);
            LOGdump(ADD_LOG, config_dataptr, reportString);
          }
          delOPC = wkgOPC_ptr;
        }

        if (delOPC)
        {
          wkgOPC_ptr = removeOpCodeNode(wkgOPC_ptr, delOPC);
          delOPC = NULL;
        }

        else
        {
          wkgOPC_ptr = wkgOPC_ptr->next_ptr;
        }
      }
      returnVal = true;
      break;
    }
  }

  else
  {
    wkgOPC_ptr = NULL;
    return true;
  }

  return returnVal;
}

/**
 * Allocates memory for a new MEMnode and initializes its fields.
 * @param physStart The starting physical address of the memory block.
 * @param physEnd The ending physical address of the memory block.
 * @param memState The state of the memory block: 0 for used, 1 for open.
 * @param procNum The number of the process using the memory block.
 * @param logStart The starting logical address of the memory block.
 * @param logStop The ending logical address of the memory block.
 * @return A pointer to the newly allocated MEMnode.
 */
MEMnode *MEMnode_add(int physStart, int physEnd, int memState, int procNum,
                     int logStart, int logStop)
{
  MEMnode *result = (MEMnode *)malloc(sizeof(MEMnode));
  result->physicalStart = physStart;
  result->physicalStop = physEnd;
  result->memBlockState = memState;
  result->processNumber = procNum;
  result->logicalStart = logStart;
  result->logicalStop = logStop;
  result->next_ptr = NULL;
  return result;
}

/**
 * @brief Recycles a memory node with the provided information.
 * @param tempNode The memory node to recycle.
 * @param memState The state of the memory block after recycling.
 * @param procNum The process number associated with the memory block.
 * @param phyStart The starting physical address of the memory block.
 * @param phyStop The ending physical address of the memory block.
 * @param logStart The starting logical address of the memory block.
 * @param logStop The ending logical address of the memory block.
 * @return void
 */
void MEMnode_recycle(MEMnode *tempNode, int memState, int procNum, int phyStart,
                     int phyStop, int logStart, int logStop)
{
  tempNode->memBlockState = memState;
  tempNode->processNumber = procNum;
  tempNode->physicalStart = phyStart;
  tempNode->physicalStop = phyStop;
  tempNode->logicalStart = logStart;
  tempNode->logicalStop = logStop;
}

/**
 * @brief Repairs the memory blocks in the linked list by combining adjacent
 * free memory blocks
 * @param MEM_ptr Pointer to the head of the linked list
 * @return void
 */
void MEMrepair(MEMnode *MEM_ptr)
{
  MEMnode *removePtr;

  while (MEM_ptr)
  {
    if (MEM_ptr->memBlockState == READY_STATE && MEM_ptr->next_ptr &&
        MEM_ptr->next_ptr->memBlockState == READY_STATE)
    {
      MEMnode_recycle(MEM_ptr, READY_STATE, NULL_PID, MEM_ptr->physicalStart,
                      MEM_ptr->next_ptr->physicalStop, ZERO, ZERO);
      removePtr = MEM_ptr->next_ptr;
      MEM_ptr->next_ptr = removePtr->next_ptr;
      free(removePtr);
    }
    else
    {
      MEM_ptr = MEM_ptr->next_ptr;
    }
  }
}

/**
 * @brief Displays information about a linked list of memory blocks
 * @param MEM_ptr Pointer to the head node of the linked list of memory blocks
 * @param output_str String to be displayed as a header before displaying memory
 * information
 */
void MEMdisplay(MEMnode *MEM_ptr, char *output_str, _Bool output_flag)
{
  char output_string[MAX_STR_LEN];
  MEMnode *wkgMemPtr;

  if (output_flag != false)
  {
    puts("\n--------------------------------------------------");
    puts(output_str);
    wkgMemPtr = MEM_ptr;
    if (MEM_ptr == NULL)
    {
      puts("No memory configured");
    }
    for (; wkgMemPtr != NULL; wkgMemPtr = wkgMemPtr->next_ptr)
    {
      if (wkgMemPtr->memBlockState == MEM_OPEN)
      {
        copyString(output_string, "Open");
      }
      else
      {
        copyString(output_string, "Used");
      }
      if (wkgMemPtr->processNumber == NULL_PID)
      {
        printf("%d [ %s, P#: x, ", wkgMemPtr->physicalStart, output_string);
      }
      else
      {
        printf("%d [ %s, P#: %d, ", wkgMemPtr->physicalStart, output_string,
               wkgMemPtr->processNumber);
      }
      printf("%d-%d ] %d\n", wkgMemPtr->logicalStart, wkgMemPtr->logicalStop,
             wkgMemPtr->physicalStop);
    }
    puts("--------------------------------------------------");
  }
}

/**
 * Manages memory operations such as allocation, access and clearing
 * @param config_dataptr A pointer to ConfigDataType struct containing the
 * configuration data
 * @param OPC_ptr A pointer to OpCodeType struct containing the operation code
 * data
 * @return _Bool Returns true if the memory operation was successful, otherwise
 * false
 */
_Bool MMU(ConfigDataType *config_dataptr, OpCodeType *OPC_ptr)
{
  char displayStr[MAX_STR_LEN];
  MEMnode *tempNodePtr;
  static MEMnode *head_ptr = NULL;
  int highMemLoc;
  int lowMemLoc;
  int processId;
  int logicalHigh;
  int requestedMemory;
  _Bool displayFlag = config_dataptr->memDisplay;
  int highestMemLoc = config_dataptr->memAvailable - ONE;
  int lowestMemLoc = ZERO;
  MEMnode *wkgMemPtr;
  MEMnode *trlgPtr;
  int logicalOffset;
  int logicalLow;

  if (head_ptr == NULL)
  {
    head_ptr = MEMnode_add(lowestMemLoc, highestMemLoc, MEM_OPEN, NULL_PID,
                           MEM_INIT, MEM_INIT);
    copyString(displayStr, "After memory initialization");
    MEMdisplay(head_ptr, displayStr, displayFlag);
    return true;
  }

  if (compareString(OPC_ptr->strArg1, "clearAll") &&
      compareString(OPC_ptr->strArg1, "clearOne"))
  {
    logicalLow = OPC_ptr->intArg2;
    logicalOffset = OPC_ptr->intArg3;
  }
  else
  {
    logicalLow = ONE;
    logicalOffset = ONE;
  }
  requestedMemory = logicalOffset;
  logicalHigh = logicalOffset + logicalLow - ONE;
  processId = OPC_ptr->pid;

  if ((!compareString(OPC_ptr->strArg1, "allocate") ||
       !compareString(OPC_ptr->strArg1, "access")) &&
      (logicalLow < ZERO || logicalOffset <= ZERO ||
       config_dataptr->memAvailable <= logicalLow ||
       config_dataptr->memAvailable <= logicalHigh))
  {
    copyString(displayStr, "After limits out of range");
    MEMdisplay(head_ptr, displayStr, displayFlag);
    return false;
  }
  if (compareString(OPC_ptr->strArg1, "allocate"))
  {
    if (!compareString(OPC_ptr->strArg1, "access"))
    {
      for (wkgMemPtr = head_ptr; wkgMemPtr != NULL;
           wkgMemPtr = wkgMemPtr->next_ptr)
      {
        if (wkgMemPtr->processNumber == processId &&
            wkgMemPtr->logicalStart <= logicalLow &&
            wkgMemPtr->logicalStop >= logicalLow &&
            wkgMemPtr->logicalStart <= logicalHigh &&
            wkgMemPtr->logicalStop >= logicalHigh)
        {
          copyString(displayStr, "After access success");
          MEMdisplay(head_ptr, displayStr, displayFlag);
          return true;
        }
      }
      copyString(displayStr, "After access failure");
      MEMdisplay(head_ptr, displayStr, displayFlag);
      return false;
    }

    if (compareString(OPC_ptr->strArg1, "clearOne"))
    {
      wkgMemPtr = head_ptr;
      while (wkgMemPtr)
      {
        tempNodePtr = wkgMemPtr;
        wkgMemPtr = wkgMemPtr->next_ptr;
        free(tempNodePtr);
      }
      head_ptr = NULL;
      copyString(displayStr, "After clear all process success");
      MEMdisplay(head_ptr, displayStr, displayFlag);
      return true;
    }

    else
    {
      for (wkgMemPtr = head_ptr; wkgMemPtr != NULL;
           wkgMemPtr = wkgMemPtr->next_ptr)
      {
        if (wkgMemPtr->processNumber == processId)
          MEMnode_recycle(wkgMemPtr, MEM_OPEN, NULL_PID,
                          wkgMemPtr->physicalStart, wkgMemPtr->physicalStop,
                          ZERO, ZERO);
      }
      MEMrepair(head_ptr);
      sprintf(displayStr, "After clear process %d success", processId);
      MEMdisplay(head_ptr, displayStr, displayFlag);
      return true;
    }
  }

  else
  {
    for (wkgMemPtr = head_ptr; wkgMemPtr != NULL;
         wkgMemPtr = wkgMemPtr->next_ptr)
    {
      if (wkgMemPtr->processNumber == processId &&
          ((wkgMemPtr->logicalStart <= logicalLow &&
            wkgMemPtr->logicalStop >= logicalLow) ||
           (wkgMemPtr->logicalStart <= logicalHigh &&
            wkgMemPtr->logicalStop >= logicalHigh)))
      {
        copyString(displayStr, "After allocate overlap failure");
        MEMdisplay(head_ptr, displayStr, displayFlag);
        return false;
      }
    }

    wkgMemPtr = head_ptr;
    trlgPtr = head_ptr;

    while (wkgMemPtr &&
           (wkgMemPtr->memBlockState == MEM_USED ||
            wkgMemPtr->physicalStop - wkgMemPtr->physicalStart + ONE <
                requestedMemory))
    {
      trlgPtr = wkgMemPtr;
      wkgMemPtr = wkgMemPtr->next_ptr;
    }

    if (wkgMemPtr->physicalStop - wkgMemPtr->physicalStart + ONE ==
        requestedMemory)
    {
      MEMnode_recycle(wkgMemPtr, MEM_USED, processId, wkgMemPtr->physicalStart,
                      wkgMemPtr->physicalStop, logicalLow, logicalHigh);
      copyString(displayStr, "After allocate failure");
      MEMdisplay(head_ptr, displayStr, displayFlag);
      return false;
    }
    lowMemLoc = wkgMemPtr->physicalStart;
    highMemLoc = lowMemLoc + requestedMemory - ONE;
    tempNodePtr = MEMnode_add(lowMemLoc, highMemLoc, MEM_USED, processId,
                              logicalLow, logicalHigh);

    if (wkgMemPtr == head_ptr)
    {
      head_ptr = tempNodePtr;
    }

    else
    {
      trlgPtr->next_ptr = tempNodePtr;
    }
    wkgMemPtr->physicalStart = highMemLoc + 1;
    tempNodePtr->next_ptr = wkgMemPtr;
    copyString(displayStr, "After allocate success");
    MEMdisplay(head_ptr, displayStr, displayFlag);

    return true;
  }
}

/**
 * CPUidle - function to simulate idle state of CPU when all processes are
 * blocked
 * @param config_dataptr pointer to configuration data structure
 * @param PCB_ptr pointer to process control block data structure
 * @return void
 */
void CPUidle(ConfigDataType *config_dataptr, PCBdata *PCB_ptr)
{
  char reportString[MAX_STR_LEN];
  OpCodeType *temp_ptr = NULL;
  int oneCycle = config_dataptr->procCycleRate;
  _Bool interruptFound = false;

  copyString(reportString, "OS: CPU idle, all active processes blocked");
  LOGdump(ADD_LOG, config_dataptr, reportString);
  while (!interruptFound)
  {
    runTimer(oneCycle);
    interruptFound =
        interruptMNGR(INTERRUPT_CHECK, temp_ptr, PCB_ptr, config_dataptr);
  }
  copyString(reportString, "OS: CPU interrupt, end idle");
  LOGdump(ADD_LOG, config_dataptr, reportString);
}

/**
 * Runs the simulation using the given configuration and meta data.
 * @param config_dataptr Pointer to the configuration data.
 * @param meta_data_ptr Pointer to the meta data.
 */
void runSim(ConfigDataType *config_dataptr, OpCodeType *meta_data_ptr)
{
  char reportString[MAX_STR_LEN];
  char timeString[MAX_STR_LEN];
  PCBdata *PCB_ptr;
  PCBdata *PCB_wkg;
  PCBdata *ID_ptr;
  OpCodeType *OPC_ptr = NULL;

  int currentPID = NULL_PID;
  _Bool isPreemptive;
  int lastPid = NULL_PID;
  _Bool runFlag = true;
  // IO thread arguments
  void *IO_args[IO_ARGS];
  // IO thread
  pthread_t IO;
  // IO pthread functions return code for create and join
  int io_init = ZERO;

  printf("Simulator Run");
  printf("\n-------------\n");
  accessTimer(ZERO_TIMER, timeString);

  LOGdump(INIT_LOG, config_dataptr, "OS: Simulator start");
  PCB_ptr = PCBcreate(config_dataptr, meta_data_ptr);

  isPreemptive = meta_data_ptr->next_ptr->intArg3 == PREEMPTIVE_CODE;
  // process interrupt requests
  interruptMNGR(INIT_MNGR, OPC_ptr, PCB_ptr, config_dataptr);
  // change state of PCB
  PCBstate(config_dataptr, PCB_ptr);

  // call Memory Mgmt Unit for processing requests
  MMU(config_dataptr, meta_data_ptr);

  // enter this loop while runFlag is = to true, this is set to true since
  // we entered this function the needed flag was already called (-rs)
  do
  {
    currentPID = PCBparse(config_dataptr, PCB_ptr);

    if (currentPID == EXIT)
    {
      LOGdump(ADD_LOG, config_dataptr, "OS: System stop");
      runFlag = false;
    }

    else if (currentPID == WAIT)
    {
      // CPUidle based on preemptive scheduling
      CPUidle(config_dataptr, PCB_ptr);
    }

    else
    {
      // get the next opcode
      PCBstate(config_dataptr, PCB_ptr);

      ID_ptr = PCBnode_pid(PCB_ptr, currentPID);

      if (ID_ptr->OCcurr != NULL)
      {
        if (ID_ptr->OCcurr->intArg2 == 0)
        {
          OPC_ptr = ID_ptr->OCcurr->next_ptr;
        }

        else
        {
          OPC_ptr = ID_ptr->OCcurr;
        }
      }

      else
      {
        OPC_ptr = ID_ptr->OClist;
      }

      ID_ptr->OCcurr = OPC_ptr;

      if ((compareString(OPC_ptr->command, "app") ||
           compareString(OPC_ptr->strArg1, "start")) &&
          isPreemptive && currentPID != lastPid)
      {
        PCB_wkg = PCBnode_pid(PCB_ptr, currentPID);

        sprintf(reportString, "OS: Process %d selected with %d ms remaining",
                currentPID, PCB_wkg->time_left);

        LOGdump(ADD_LOG, config_dataptr, reportString);

        sprintf(reportString, "OS: Process %d set from READY to RUNNING\n",
                currentPID);

        LOGdump(ADD_LOG, config_dataptr, reportString);
      }

      // these tested statements take into account possible commands and
      // arguments and call necessary function based on such conditions
      if (compareString(OPC_ptr->command, "app") ||
          compareString(OPC_ptr->strArg1, "start"))
      {
        if (compareString(OPC_ptr->command, "app") ||
            compareString(OPC_ptr->strArg1, "end"))
        {
          if (compareString(OPC_ptr->command, "mem"))
          {
            if (compareString(OPC_ptr->command, "dev"))
            {
              if (!compareString(OPC_ptr->command, "cpu"))
              {
                sprintf(reportString, "Process: %d, %s %s operation start",
                        OPC_ptr->pid, OPC_ptr->command, OPC_ptr->strArg1);

                LOGdump(ADD_LOG, config_dataptr, reportString);

                PROCthread(config_dataptr, OPC_ptr, PCB_ptr);
              }
            }

            else
            {
              sprintf(reportString, "Process: %d, %s %sput operation start",
                      OPC_ptr->pid, OPC_ptr->strArg1, OPC_ptr->inOutArg);

              LOGdump(ADD_LOG, config_dataptr, reportString);
              OPC_ptr->intArg2 *= config_dataptr->ioCycleRate;

              IO_args[IO_ARG_ONE] = (void *)config_dataptr;
              IO_args[IO_ARG_TWO] = (void *)OPC_ptr;
              IO_args[IO_ARG_THREE] = (void *)PCB_ptr;

              io_init = pthread_create(&IO, NULL, IOthread_wrapper, IO_args);

              if (io_init != ZERO)
              {
                fprintf(stderr, "Error: Failed to create IO thread\n");
              }
              io_init = pthread_join(IO, NULL);

              if (io_init != ZERO)
              {
                fprintf(stderr, "Error: Failed to join IO thread\n");
              }

              PCB_wkg = PCBnode_pid(PCB_ptr, currentPID);
              PCB_wkg->time_left -= OPC_ptr->intArg2;
              OPC_ptr->intArg2 = ZERO;
            }
          }
          else
          {
            sprintf(reportString, "Process: %d, mem %s request (%d, %d)",
                    OPC_ptr->pid, OPC_ptr->strArg1, OPC_ptr->intArg2,
                    OPC_ptr->intArg3);

            LOGdump(ADD_LOG, config_dataptr, reportString);

            if (MMU(config_dataptr, OPC_ptr))
            {
              sprintf(reportString, "Process: %d, successful mem %s request",
                      OPC_ptr->pid, OPC_ptr->strArg1);

              LOGdump(ADD_LOG, config_dataptr, reportString);

              OPC_ptr->intArg2 = ZERO;
            }
            else
            {
              sprintf(reportString, "Process: %d, failed mem %s request\n",
                      OPC_ptr->pid, OPC_ptr->strArg1);

              LOGdump(ADD_LOG, config_dataptr, reportString);

              sprintf(reportString, "OS: Segmentation fault, Process % d ended",
                      OPC_ptr->pid);

              LOGdump(ADD_LOG, config_dataptr, reportString);

              copyString(OPC_ptr->strArg1, "clearOne");

              MMU(config_dataptr, OPC_ptr);

              PCB_wkg->state = EXIT_STATE;

              sprintf(reportString, "OS: Process %d set to EXIT", currentPID);

              LOGdump(ADD_LOG, config_dataptr, reportString);
            }
          }
        }
        else
        {
          sprintf(reportString, "OS: Process %d ended\n", currentPID);

          LOGdump(ADD_LOG, config_dataptr, reportString);

          OPC_ptr->pid = currentPID;

          copyString(reportString, "clearOne");

          copyString(OPC_ptr->strArg1, reportString);

          MMU(config_dataptr, OPC_ptr);

          PCB_wkg = PCBnode_pid(PCB_ptr, currentPID);

          PCB_wkg->state = EXIT_STATE;

          sprintf(reportString, "OS: Process %d set to EXIT", currentPID);

          LOGdump(ADD_LOG, config_dataptr, reportString);
        }
      }
      else
      {
        PCB_wkg = PCBnode_pid(PCB_ptr, currentPID);

        sprintf(reportString, "OS: Process %d selected with %d ms remaining",
                currentPID, PCB_wkg->time_left);

        LOGdump(ADD_LOG, config_dataptr, reportString);

        sprintf(reportString, "OS: Process %d set from READY to RUNNING\n",
                currentPID);

        LOGdump(ADD_LOG, config_dataptr, reportString);

        OPC_ptr->intArg2 = ZERO;
      }
    }
    // interruptMNGR will get called again here for handling blocks for
    // preemptive scheduling
    lastPid = currentPID;

  }

  while (runFlag); // dirty trick for a do while loop

  PCB_ptr = PCBnode_del(PCB_ptr);

  copyString(OPC_ptr->strArg1, "clearAll");

  MMU(config_dataptr, OPC_ptr);

  LOGdump(ADD_LOG, config_dataptr, "OS: Simulation end\n");

  LOGdump(DUMP_LOG, config_dataptr, reportString);

  accessTimer(STOP_TIMER, reportString);
}

//  File: simulator.h
//  Project: Sim03
//  Secret ID: 708996

#ifndef simulator_h
#define simulator_h

#include "StringUtils.h"
#include "configops.h"
#include "metadataops.h"
#include "StandardConstants.h"
#include "simtimer.h"
#include "time.h"
#include "datatypes.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void copyConfigData(ConfigDataType *dest, ConfigDataType *src);

LOGnode *LOGnode_add(LOGnode *local_ptr, char *txt_input);

LOGnode *LOGnode_del(LOGnode *local_ptr);

void LOGdump(int trigger, ConfigDataType *config_dataptr, char *txt_input);

PCBdata *PCBnode_add(PCBdata *local_ptr, PCBdata *new_ptr);

PCBdata *PCBnode_del(PCBdata *local_ptr);

PCBdata *PCBnode_pid(PCBdata *local_ptr, int PCB_PID);

PCBdata *PCBcreate(ConfigDataType *config_dataptr, OpCodeType *md_head_ptr);

int PCBparse(ConfigDataType *config_dataptr, PCBdata *local_ptr);

void PCBstate(ConfigDataType *config_dataptr, PCBdata *local_ptr);

void PCBdisplay(PCBdata *head_ptr);

void IOthread(ConfigDataType *config_dataptr, OpCodeType *OPC_ptr, PCBdata *PCB_ptr);

void *IOthread_wrapper(void *arg);

void PROCthread(ConfigDataType *CNF_ptr, OpCodeType *OPC_ptr, PCBdata *PCB_ptr);

OpCodeType *addInterrupt(OpCodeType *local_ptr, OpCodeType *new_ptr);

OpCodeType *removeOpCodeNode(OpCodeType *headPtr, OpCodeType *removedPtr);

OpCodeType *getNextOpCode(PCBdata *PCB_ptr, int PCB_pid);

void OPCcopy(OpCodeType *destination, OpCodeType *source);

bool interruptMNGR(Interrupts CTRL_ptr, OpCodeType *OPC_ptr, PCBdata *PCB_ptr, ConfigDataType *config_dataptr);

MEMnode *MEMnode_add(int physStart, int physEnd, int memState, int procNum, int logStart, int logStop);

void MEMnode_recycle(MEMnode *tempNode, int memState, int procNum, int phyStart, int phyStop, int logStart, int logStop);

void MEMrepair(MEMnode *MEM_ptr);

void MEMdisplay(MEMnode *MEM_ptr, char *output_str, bool output_flag);

bool MMU(ConfigDataType *config_dataptr, OpCodeType *OPC_ptr);

void CPUidle(ConfigDataType *config_dataptr, PCBdata *PCB_ptr);

void runSim(ConfigDataType *config_dataptr, OpCodeType *meta_data_ptr);

#endif

#ifndef __NASCENTER_JS_JOBMASTER_H__
#define __NASCENTER_JS_JOBMASTER_H__

/*****************************************************************************
 *
 * NASCenter_JS_JobMaster.h
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *      
 *
 *
 * description:
 *      NASCenter_JS_JobMaster.h provides common includes,macros,and external
 *              declarations/definitions for users of NASCenter_JS_JobMaster.c
 *
 * $Id: NASCenter_JS_Linux.h,v 1.1 2003/12/02 06:33:57 thiagav Exp $
 *
 ****************************************************************************/

 /********************************** includes *******************************/
#include <stdlib.h>
#include "ctypes.h"
#include "NASCenter_JS_Linux_List.h"

 /********************************** macros *********************************/
#define JS_ADD 1
#define JS_MODIFY 2
#define JS_DELETE 3 

#define JS_RULES_EXCEEDED -1

#define MAX_OID_LEN 128
#define JS_MASTER_ID 1
#define JS_MASTER_NAME 2
#define JS_PERIODIC_INTERVAL 3
#define JS_START_TIME 4
#define JS_FINISH_TIME 5
#define JS_CPUUSAGE 6
#define JS_MEMORYUSAGE 7
#define JS_ROW_STATUS 8

#define JS_START_RULE_NUM 1
#define JS_MAX_ELEMENT 14
#define JS_FIELD_LEN 14
#define JS_NO_RECORD_FOUND 99
#define  C_MAX_SIZE 1000000
#define JS_DAY_CNT 7	

UI8 Days[7][5]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};


 /******************************* exported types ****************************/

typedef struct 
{
    UI16 iJobMasIndex;       /* Index ID of the table*/
    UI8 *pcJobMasId;   /*unique id of schedule*/
    UI8 *pcJobMasName; /*Schedule Name */
    I16 iJobMasInterval;   /*Schedulear calling period of time*/
    UI8 *pcJobMasStartTime;     /*Job Starting time*/
    UI8 *pcJobMasFinishTime;     /*Job End time*/
    UI32 uiEndTimeinNum;
    I16 iJobMasMemoryUsage;     /*average memory usage*/
    I16 iJobMasCPUUsage;        /*Average CPU usage during schedular execution*/    
    I16 iJobMasStatus;       /*Row status of the record */
}stJobMaster;

oid agentInfoLogFileJobProcess_variables_oid[] = {1,3,6,1,4,1,3204,1,3,36,1,20,0};
oid agentInfoLogFileTransferIP_variables_oid[] ={1,3,6,1,4,1,3204,1,3,36,1,13,0};
oid agentInfoLogFilePortNo_variables_oid[] = {1,3,6,1,4,1,3204,1,3,36,1,14,0};


typedef struct
{
   struct snmp_session  stSession;
}SSNMPSession;

typedef netsnmp_pdu *QueueEntry ;

typedef struct StJobMaster
{
    UI8 *pcMasterId;
    UI8 *pcMasterScriptFile;
    
}stJobMasterSched;

typedef struct CBuffer  
{
  pthread_mutex_t queue_lock;
  pthread_cond_t  queue_not_full;
  pthread_cond_t  queue_not_empty;
  I16 iQRear;
  I16 iQFront;
  QueueEntry  queueEntry[JS_MAX_ELEMENT];
}SCBuffer;



/************************* exported function prototypes ********************/
/*extern stList JS_MakeEmpty(stList);
extern void JS_Insert( ElementType ,ElementType, ElementType ,  stList , struct ip_fwnew *);
extern stRetStruct *  JS_GetstructForMod(I16 , stList,I16);
extern I16 JS_DelRuleNumFromList(I16 , stList );
extern I16  JS_ModifyList(stList ,  struct ip_fwnew *,I16 );
extern I16  JS_DeleteFromList(stList , I16,I16 );*/

/***************************** exported globals ****************************/
extern I8 iVal;
extern I32 iLen;
UI32 INTERVAL= 60;

UI8 gcNASSACommunity[]="public";
UI8 gcAgentSessPeerName[] = "localhost";
I16 giNASSAPort=161;
UI8 JS_CURRENT_PATH_LOGFILE[] ="/home/thiagav/cvs/Project/Development/Group2/Unix/JobSched/src";

UI8 cSrcMask[]="255.255.255.255";
UI8 cDstMask[]="255.255.255.255";
UI8 cDstAddr[]="192.168.165.101";
UI8 MemUsage[]="1.3.6.1.4.1.3204.1.3.34.1.1.1.12";
UI8 CPUUsage[]="1.3.6.1.4.1.3204.1.3.34.1.1.1.11";

UI8 JS_DEST_PATH[]="/home/thiagav";


#endif /* __NASCENTER_JS_JOBMASTER_H__ */
/* end of  NASCenter_JS_JOBMASTER.h */

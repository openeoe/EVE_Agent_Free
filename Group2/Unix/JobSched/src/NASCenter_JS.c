/*****************************************************************************
 *
 * NASCenter_JS.c
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      
 *
 * description :
 *
 * TODO:
 *
 *
 *
 *  CM Details:
 *
 * $Author: thiagav $
 * $Log: NASCenter_JS.c,v $
 * Revision 1.46  2004/01/19 11:39:03  thiagav
 * Comments added and header is added
 *
 * Revision 1.45  2004/01/13 13:23:36  thiagav
 * Memroy leaks fixed by using purify tool
 *
 * Revision 1.44  2004/01/12 03:43:30  thiagav
 * Code updated after memory leaks check in bounce checker
 *
 * Revision 1.43  2004/01/08 12:44:58  baravind
 * *** empty log message ***
 *
 * Revision 1.42  2004/01/07 06:20:27  vijaige
 * errno not supported in windows
 *
 * Revision 1.41  2004/01/07 04:39:00  karthis
 * changed the sleep from 1000 to 100 in Get CPUUtilization for windows
 *
 * Revision 1.40  2004/01/06 05:47:04  thiagav
 * All time structure is initialized to NULL
 *
 * Revision 1.39  2004/01/06 05:30:43  thiagav
 * Fix done for Unix ware for receiving 1 hour less
 *
 * Revision 1.38  2004/01/05 12:15:25  thiagav
 * Fix done for Start Time
 *
 * Revision 1.36  2004/01/05 09:59:14  thiagav
 * Added after freeing the pstjob pointer after performing the Job scheduling
 *
 * Revision 1.35  2004/01/05 09:14:25  thiagav
 * *** empty log message ***
 *
 * Revision 1.34  2004/01/05 05:20:00  vijaige
 * Close handle done for create process
 *
 * Revision 1.33  2004/01/02 06:44:15  thiagav
 * newTaskId has been replaced with lTaskID
 *
 * Revision 1.32  2003/12/25 04:28:54  thiagav
 * Added memset in the log file
 *
 * Revision 1.31  2003/12/24 08:59:54  thiagav
 * Added after fixing the check for Detail table row status incative
 *
 * Revision 1.30  2003/12/23 06:41:23  thiagav
 * Compiled and bugs for unixware is fixed
 *
 * Revision 1.29  2003/12/23 06:36:49  thiagav
 * Updated along with unixware
 *
 * Revision 1.28  2003/12/23 03:21:44  thiagav
 * Updated by Thiagu   
 * 
 * Revision 1.27  2003/12/22 10:15:46  thiagav
 * Fix done for unistd
 *
 * Revision 1.26  2003/12/22 10:11:36  thiagav
 * Fix done for more than one detail table fetch
 *
 * Revision 1.23  2003/12/21 08:20:08  thiagav
 * Added unistd.h
 *
 * Revision 1.22  2003/12/21 07:54:52  thiagav
 * Added bug fixes on row status
 *
 * Revision 1.20  2003/12/20 12:31:05  thiagav
 * Added after putting return NASC_SUCCESS after JS_DEBUG message
 *
 * Revision 1.19  2003/12/20 04:31:59  thiagav
 * Code modified to long with respect to long value
 *
 * Revision 1.18  2003/12/19 17:58:17  thiagav
 * Added after adding finish time check and checking script is available or not,
 *
 * Revision 1.17  2003/12/16 07:14:51  thiagav
 * Added for Interim Release
 *
 * Revision 1.16  2003/12/15 12:26:48  vijaige
 * after changed JS_FillTimeStruct
 *
 * Revision 1.15  2003/12/15 11:05:30  vijaige
 * iResult has been added for win32
 *
 * Revision 1.14  2003/12/15 10:17:07  thiagav
 * *** empty log message ***
 *
 * Revision 1.13  2003/12/15 04:37:52  thiagav
 * Added after testing
 *
 * Revision 1.12  2003/12/15 03:11:22  thiagav
 * Added after final testing
 *
 * Revision 1.10  2003/12/13 05:58:01  thiagav
 * added configuration paramaters
 *
 * Revision 1.9  2003/12/06 09:38:36  thiagav
 * Added after testing
 *
 * Revision 1.8  2003/12/04 03:49:39  vijaige
 * after porting for windows
 *
 * Revision 1.6  2003/12/02 11:01:06  thiagav
 * *** empty log message ***
 *
 * Revision 1.5  2003/12/02 08:16:04  thiagav
 * Added after modification
 *
 * Revision 1.4  2003/12/02 06:58:56  thiagav
 * Added
 *
 * Revision 1.3  2003/12/02 06:35:55  thiagav
 * Added after name validation
 *
 * Revision 1.2  2003/12/02 05:59:19  thiagav
 * Added
 *
 * Revision 1.13  2003/12/02 05:57:05  thiagav
 * Added
 *
 * Revision 1.1  2003/12/02 05:56:14  thiagav
 * Added
 *
 * Revision 1.12  2003/12/02 05:52:46  thiagav
 * Added after ifdef with all OS
 *
 * Revision 1.11  2003/12/02 05:13:02  thiagav
 * Added after ifdef with solaris
 *
 * Revision 1.10  2003/11/25 11:18:18  thiagav
 * Added after modified
 *
 * Revision 1.9  2003/11/21 07:28:41  thiagav
 * Added in to cvs after testing
 *
 * Revision 1.8  2003/11/21 06:47:27  thiagav
 * Added after basic testing
 *
 * Revision 1.6  2003/11/20 10:50:19  thiagav
 * Added after 30% of testing
 *
 * Revision 1.5  2003/11/19 15:26:17  thiagav
 * Added after basic testing
 *
 * Revision 1.1  2003/11/18 05:34:43  thiagav
 * Added after compilation
 *
 *
 * $Id: NASCenter_JS.c,v 1.46 2004/01/19 11:39:03 thiagav Exp $
 *
 ****************************************************************************/


/********************************* includes *********************************/
#include <stdio.h>
#ifdef HP_UX_JS
#include <sys/pstat.h>
#endif

#ifndef WIN32
#include<unistd.h>
#endif  

#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#ifndef WIN32
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include <windows.h>
#include <process.h>
#include <psapi.h>
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/mib_api.h>
#include <net-snmp/net-snmp-includes.h>
#include "NASCenter_JS.h"
#include "NASCenter_Errors.h"
#include "NASCenter_Scheduler.h"
#include "jobProcessMasterTable.h"
#include "JobProcessDetailTable.h"
#include "NASCenter_FileTrans.h"
#include "AgentInfo.h" 

#define MAX_REPETITIONS 6

/****************************** local macros *********************************
*
* (Macros shareable with rest of the modules are present in http_sample.h file)
*/

/********************* local structure/union definitions ********************/

/********************* prototypes for local functions ***********************/
I16 JS_StartJobSchedProcess( ) ;
I16 JS_FillStruct(UI8  **,  netsnmp_variable_list *);
I16 JS_GetJobProcessMasterTableDetails();
I16 JS_GetAgentInfoMIB();
I16 JS_UpdateJobToScheduler(stJobMaster  *, I16 );
I16 JS_CheckTable(netsnmp_variable_list *);
I16 JS_CheckField( netsnmp_variable_list *);
I16 JS_AddJobToSched(stJobMaster  *);
I16 JS_ModifyJobToSched(stJobMaster  *);
I16 JS_DeleteFromSched(stJobMaster  *);
I16 JS_SpawnJobSched(void *);
I16 JS_GetIndex(netsnmp_variable_list *pstCurrVars);
I16 JS_GetDay(time_t );
I16 JS_CheckScriptAvail(UI8 *);
I16 JS_GetCPUUtilization(stJobMaster **, stJobMasterSched *);
I16 JS_DoSNMPSet(char *, I16 , I16 );
I16 JS_GetDayNum(UI8 *pcDay);
I16 JS_ScheduleLogInfo();
UI32  JS_FillTimeStruct(struct tm *, char *) ; 	    
UI32 JS_GetCurrentTime ();
BOOL  JS_IsQueueEmpty(SCBuffer *);
BOOL  JS_IsQueue_Full(SCBuffer *);
void JS_CallJobLogFile(void *args);
void JS_SendLogFile();
void JS_FreeJob(stJobMasterSched  *);
void JS_PerformJobSchedAction(void *);
void JS_Read_Packets(void *);
void JS_DoJobSched(stJobMasterSched *, stJobMaster *);
void JS_InitSession();
void JS_InitCBuffer(SCBuffer *);
void JS_ProcessTrapMsg(void *);
void JS_ParseTrap( struct snmp_pdu *);
void JS_Log(UI8 *logMsg);
void JS_FreeJobMaster( stJobMaster *);
int JS_ListenTrap(int id, struct snmp_session *sess, int req, netsnmp_pdu *pdu, void *arg);
int readoid(char *, oid *, size_t *) ;
void FreeMemAndCPU(char *pcCPUPer, char *pcMemPer );

/************************ globals within this module*************************/

SCBuffer stCB;
stList stListPtr;
struct snmp_session *agentSess;
void *tSess;
char charBuff[C_MAX_SIZE];   
FILE *fpForJS;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t job_mutex = PTHREAD_MUTEX_INITIALIZER;
/*Agent information  */
I16 JobSchedServerPort;
UI8 JobSchedServerIP[16];
UI8 JobSchedLogFileName[80];
I16 backupInterval=0;
long lTotalSize;
#ifdef JS_DEBUG
FILE *ifdForJS;
#endif

/*****************************************************************************
 *
 * name             :   JS_StartJobSchedProcess 
 * description      :   Starting function of Job Process application 
 *                      This function creates the log file,  
 *                      fetch the agentInfo table and starts the scheduler 
 * input parameters :   - 
 * return type      :   - 
 * global variables :   ifdForJS -> File Descriptor for logging the debug log
 *                      information 
 * calls            :   JS_GetAgentInfoMIB
 *                      SCH_StartScheduler
 *                      JS_ScheduleLogInfo
 *                      JS_GetJobProcessMasterTableDetails
 *                      JS_ProcessTrapMsg
*****************************************************************************/
I16 JS_StartJobSchedProcess() 
{
   I16 iResult; 	
   pthread_t readThread;
   
   /*Call Scheduler function */
   SCH_StartScheduler();
   
   /*Get Agent Info Table details for logging the log information */
   iResult=0;
   iResult = JS_GetAgentInfoMIB();
   if(iResult == NASC_FAILURE )
   {
       printf("Error in Retrieving agent Info  table records \n");	
       printf("Please initialize the Agent Info table for Job Scheduling\n");	
       printf("Job Scheduler is exiting now..\n");	
       return NASC_FAILURE;
   }
   pthread_mutex_lock(&log_mutex);	     
   /*Open the file decriptor for log file*/
   fpForJS = fopen(JobSchedLogFileName , "a+");
   if(fpForJS == NULL)
   {
        pthread_mutex_unlock(&log_mutex);	     
        return NASC_FAILURE;	   
   }
   pthread_mutex_unlock(&log_mutex);	     
 
   //JS_Log("Log file name %s \n",JobSchedLogFileName );   
   iResult=0;
   /*Assign the task to scheduler */
   iResult = JS_ScheduleLogInfo();  
   if(iResult == NASC_FAILURE)
   {
	   
#ifdef JS_DEBUG	   
         JS_Log("Error in adding log file transfer activity to scheduler \n");	   
#endif	 
   }
   
   iResult=0;
   /*Get JobSched Master table details */
   iResult = JS_GetJobProcessMasterTableDetails();
   if(!iResult)
   {
#ifdef JS_DEBUG	   
       JS_Log("Error in Retrieving Job Master table records \n");	
#endif       
   }
    
   /*Initialize the circular queue*/
   JS_InitCBuffer(&stCB) ;
   /*Create a thread for processing the trap message  */
   if(pthread_create(&readThread , NULL , (void *) JS_ProcessTrapMsg ,
                 NULL) != 0)
   {
          return NASC_FAILURE;	   
   }
    return NASC_SUCCESS;
}//end of JS_StartJobSchedProcess


/*****************************************************************************
 *
 * name             :   JS_InitSession 
 * description      :   Initialize the SNMP Session with NASCenter sub agent 
 * input parameters :   - 
 * return type      :   - struct snmp_session * - Poniter to the snmp session 
 * global variables :   - gcAgentSessPeerName  - peer name 
 *                      - giNASSAPort - port number used for opening the snmp
 *                        session
 *                      - gcNASSACommunity - Community name for snmp session
 *                        
 * calls	    :   -JS_ListenTrap  
 *                      
 *
*****************************************************************************/
void JS_InitSession()
{
   struct snmp_session tempSess;
   struct snmp_session trapSess;
   UI8 cPeerName[30];

#ifdef  JS_UNIQUE_PORT
   UI8 localAddr[30]; 
#endif   

   /* For Session Initiation */
   snmp_sess_init(&tempSess);
   tempSess.version = SNMP_VERSION_2c;

#ifdef  JS_UNIQUE_PORT
   memset(localAddr,'\0',30); 
   sprintf(localAddr,"%s:%d",gIPAddr,gAgentPort);
   tempSess.peername = localAddr;
#else
   tempSess.peername = gcAgentSessPeerName;
#endif

   tempSess.community = gcNASSACommunity;
   tempSess.community_len = strlen(gcNASSACommunity);
   tempSess.callback_magic = NULL;

   snmp_sess_init(&trapSess);
   trapSess.version = SNMP_VERSION_2c;
   memset(cPeerName,'\0',30);
   sprintf(cPeerName,"%s:%d","localhost",cTrapPort);
   trapSess.peername = cPeerName;
   trapSess.local_port = cTrapPort;
   trapSess.community = gcNASSACommunity;
   trapSess.community_len = strlen(gcNASSACommunity);
   trapSess.callback = JS_ListenTrap;
   trapSess.callback_magic = NULL;
 

   SOCK_STARTUP;
   agentSess = snmp_open(&tempSess);
   SOCK_STARTUP;
   tSess = snmp_sess_open(&trapSess);

   if(agentSess == NULL) 
   {
	   
#ifdef JS_DEBUG	   
       JS_Log("Error opening Session\n");
#endif       
       snmp_perror("NASCenterJS Snmp Open: ");
       exit(1);
   }
   
}

/*****************************************************************************
 *
 * name             :   JS_GetAgentInfoMIB
 * description      :   Get the Agent Info Mib information
 * input parameters :   - 
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :   
 *                        
 * calls	    :   - 
 *                      
 *
*****************************************************************************/
I16 JS_GetAgentInfoMIB()
{
    struct snmp_pdu *pstreqPDU;
    struct snmp_pdu *pstrespPDU= NULL;
    netsnmp_variable_list *pstCurrVars;

    I16 status=0;
    I16 iLen=0;
    memset(JobSchedLogFileName,0,80);
               
       pstreqPDU = snmp_pdu_create(SNMP_MSG_GET);
         
      /*Get the Relevant Fields*/
       /*Get the Log file name */
       iLen=sizeof(agentInfoLogFileJobProcess_oid)/sizeof(oid); 
       snmp_add_null_var(pstreqPDU,agentInfoLogFileJobProcess_oid,iLen);
      
      /*Get the IP Address */ 
       iLen=sizeof(agentInfoFileTransferIP_oid)/sizeof(oid); 
       snmp_add_null_var(pstreqPDU,agentInfoFileTransferIP_oid,iLen);

       /*Get the port number*/
       iLen=sizeof(agentInfoFileTransferPortNo_oid)/sizeof(oid); 
       snmp_add_null_var(pstreqPDU,agentInfoFileTransferPortNo_oid,iLen);

       /*Get the file transfer interval period */
       iLen=sizeof(agentInfoFileTransferInterval_oid)/sizeof(oid); 
       snmp_add_null_var(pstreqPDU,agentInfoFileTransferInterval_oid,iLen);

       /*Get the response in the respPDU*/
       status = snmp_synch_response(agentSess,pstreqPDU,&pstrespPDU);	
       if(status==STAT_SUCCESS)
       {
	   if(pstrespPDU->errstat==SNMP_ERR_NOERROR && 
           pstrespPDU->errindex==0 && 
           pstrespPDU->variables->type != ASN_NULL &&
           pstrespPDU->variables->type != SNMP_NOSUCHINSTANCE &&
           pstrespPDU->variables->type != SNMP_NOSUCHOBJECT )
	   {
#ifdef JS_DEBUG	   
	          JS_Log("sync response success \n");	   
#endif		  
	   }else
	   {
#ifdef JS_DEBUG	   
	          JS_Log("sync response failure \n");	   
#endif		  
                  return NASC_FAILURE;		   
	   }
           if(pstrespPDU == NULL)	        
	   {
#ifdef JS_DEBUG	   
                JS_Log("Error in sync response \n");		   
#endif		
		return NASC_FAILURE;
	   }
       } else 
       {
#ifdef JS_DEBUG	   
           JS_Log("Error in snmp_synch_response \n");
#endif	   
           snmp_perror("Error ");   	   
               return NASC_FAILURE;		   
       }

       pstCurrVars = pstrespPDU->variables;
       /*Get the Job Process Log file */
       iLen =0;
       iLen =  pstCurrVars->val_len;
       if(pstCurrVars != NULL)
       {
            if(pstCurrVars->val.string != NULL)  	       
	    {
                 pthread_mutex_lock(&log_mutex);	     
                 memset(JobSchedLogFileName,0,80);
                 memcpy(JobSchedLogFileName,pstCurrVars->val.string,iLen);       
                 pthread_mutex_unlock(&log_mutex);	     
		 //JS_Log("Log file name %s \n ",JobSchedLogFileName);
	    }
	    else
    	         return NASC_FAILURE; 	    
       }
       pstCurrVars = pstCurrVars->next_variable;

       /*Get the IP Address*/
       iLen =0;
       iLen =  pstCurrVars->val_len;
       if(pstCurrVars != NULL)
       {
              	       
            if(pstCurrVars->val.string != NULL)  	       
            {		    
                 pthread_mutex_lock(&log_mutex);	     
                 memcpy(JobSchedServerIP,pstCurrVars->val.string,iLen);       
                 pthread_mutex_unlock(&log_mutex);	     
            }
	    else
    	         return NASC_FAILURE; 	    
       }
       pstCurrVars = pstCurrVars->next_variable;
       /*Get the port number */
       if(pstCurrVars != NULL)
       {
             if(pstCurrVars->val.integer != NULL)  	       
             {
                 pthread_mutex_lock(&log_mutex);	     
                 JobSchedServerPort= (I16) *(pstCurrVars->val.integer);
                 pthread_mutex_unlock(&log_mutex);	     
		 //JS_Log("Server Port Number  %d \n ",JobSchedServerPort);
             }
	     else
    	         return NASC_FAILURE; 	    
       }
       pstCurrVars = pstCurrVars->next_variable;
       /*Get the Log file transfer interval */
       if(pstCurrVars != NULL)
       {
             if(pstCurrVars->val.integer != NULL)  	       
             {
                 pthread_mutex_lock(&log_mutex);	     
                 backupInterval = (I16) *(pstCurrVars->val.integer);
                 pthread_mutex_unlock(&log_mutex);	     
             }
	     else
    	         return NASC_FAILURE; 	    
	     if(backupInterval ==0)
    	         return NASC_FAILURE; 	    
       }
       /* Free the respPDU */
       if(pstrespPDU != NULL)
       {
            snmp_free_pdu(pstrespPDU); 	       
	    pstrespPDU = NULL;
       }

       return NASC_SUCCESS;
}//end of JS_GetAgentInfoMIB

/*****************************************************************************
 *
 * name             :   JS_GetJobProcessMasterTableDetails
 * description      :   Fetch the Job Process Master Table Details 
 * input parameters :   - 
 * return type      :   NASC_SUCCESS / NASC_FAILURE
 * global variables :   
 * calls	    :   
 *                      
 *
*****************************************************************************/
I16 JS_GetJobProcessMasterTableDetails()
{
    struct snmp_pdu *pstreqPDU;
    struct snmp_pdu *pstrespPDU= NULL;
    netsnmp_variable_list *vars;
    netsnmp_variable_list *pstCurrVars;
    I16 iCheckTable;
    oid *tableId;
    
    oid *nextMasterId;
    I16 nextMasterIdLen;
    oid *nextMasterName;
    I16 nextMasterNameLen;
    oid *nextMasterInterval;
    I16 nextMasterIntervalLen;
    oid *nextMasterStartTime;
    I16 nextMasterStartTimeLen;
    oid *nextMasterFinishTime;
    I16 nextMasterFinishTimeLen;
    oid *nextMasterCPUUsage;
    I16 nextMasterCPUUsageLen;
    oid *nextMasterMemoryUsage;
    I16 nextMasterMemoryUsageLen;
    oid *nextMasterStatus;
    I16 nextMasterStatusLen;
    
    I16 iProcessCount;
    
    I16 status=0;
    I16 iLen;
    I16 itableLen;
    stJobMaster *pstJobMasterTable;
              
    /*Get the Relevant Fields*/
    
    nextMasterId=jobProcessMasterID_variables_oid;
    nextMasterIdLen=sizeof(jobProcessMasterID_variables_oid)/sizeof(oid); 
    
    nextMasterName=jobProcessMasterName_variables_oid;   
    nextMasterNameLen=sizeof(jobProcessMasterName_variables_oid)/sizeof(oid); 
    
    nextMasterInterval=jobProcessMasterPeriodicInterval_variables_oid;   
    nextMasterIntervalLen=sizeof(jobProcessMasterPeriodicInterval_variables_oid)/sizeof(oid); 
          
    nextMasterStartTime=jobProcessMasterStartTime_variables_oid;
    nextMasterStartTimeLen=sizeof(jobProcessMasterStartTime_variables_oid)/sizeof(oid); 
    
    nextMasterFinishTime=jobProcessMasterFinishTime_variables_oid;   
    nextMasterFinishTimeLen=sizeof(jobProcessMasterFinishTime_variables_oid)/sizeof(oid); 
    
    nextMasterCPUUsage=jobProcessMasterCPUPercentUsage_variables_oid;   
    nextMasterCPUUsageLen=sizeof(jobProcessMasterCPUPercentUsage_variables_oid)/sizeof(oid); 
    
    nextMasterMemoryUsage=jobProcessMasterMemoryPercentUsage_variables_oid;
    nextMasterMemoryUsageLen=sizeof(jobProcessMasterMemoryPercentUsage_variables_oid)/sizeof(oid); 
    
    nextMasterStatus=jobProcessMasterStatus_variables_oid;   
    nextMasterStatusLen=sizeof(jobProcessMasterStatus_variables_oid)/sizeof(oid); 
    
    tableId=nextMasterId;
    itableLen=nextMasterIdLen;
        
    while(1) 
    {
       pstreqPDU = snmp_pdu_create(SNMP_MSG_GETBULK);
       pstreqPDU->non_repeaters = 0;
       /*Set the number of records to be fetched */
       pstreqPDU->max_repetitions = MAX_REPETITIONS;
    
       snmp_add_null_var(pstreqPDU,nextMasterId,nextMasterIdLen);
       snmp_add_null_var(pstreqPDU,nextMasterName,nextMasterNameLen);
       snmp_add_null_var(pstreqPDU,nextMasterInterval,nextMasterIntervalLen);
       snmp_add_null_var(pstreqPDU,nextMasterStartTime,nextMasterStartTimeLen);
       snmp_add_null_var(pstreqPDU,nextMasterFinishTime,nextMasterFinishTimeLen);
       snmp_add_null_var(pstreqPDU,nextMasterCPUUsage,nextMasterCPUUsageLen);
       snmp_add_null_var(pstreqPDU,nextMasterMemoryUsage,nextMasterMemoryUsageLen);
       snmp_add_null_var(pstreqPDU,nextMasterStatus,nextMasterStatusLen);

       /* Free the respPDU */
       if(pstrespPDU != NULL)
       {
            snmp_free_pdu(pstrespPDU); 	       
	    pstrespPDU = NULL;
       }

       /*Get the response in the respPDU*/
       status = snmp_synch_response(agentSess,pstreqPDU,&pstrespPDU);	
       if(status==STAT_SUCCESS)
       {
	   if(pstrespPDU->errstat==SNMP_ERR_NOERROR && 
           pstrespPDU->errindex==0 && 
           pstrespPDU->variables->type != ASN_NULL &&
           pstrespPDU->variables->type != SNMP_NOSUCHINSTANCE &&
           pstrespPDU->variables->type != SNMP_NOSUCHOBJECT  &&
	   pstrespPDU->variables->type !=  SNMP_ENDOFMIBVIEW )
	   {
#ifdef JS_DEBUG	   
	          JS_Log("sync response success \n");	   
#endif		  
	   }else
	   {
#ifdef JS_DEBUG	   
	          JS_Log("sync response failure \n");	   
#endif		  
                  return NASC_FAILURE;		   
	   }
           if(pstrespPDU == NULL)	        
	   {
#ifdef JS_DEBUG	   
                JS_Log("Error in sync response \n");		   
#endif		
		return NASC_FAILURE;
	   }
       } else 
       {
#ifdef JS_DEBUG	   
           JS_Log("Error in snmp_synch_response \n");
#endif	   
           snmp_perror("Error ");   	   
               return NASC_FAILURE;		   
       }

        vars = pstrespPDU->variables;
       pstCurrVars = vars;

        
       /*Flag for checking if the fetched table details belongs to 
	* required table*/ 
       iCheckTable = TRUE;
       for(iProcessCount =0; iProcessCount < MAX_REPETITIONS;
             iProcessCount++ ){
	       
	      if((pstCurrVars == NULL))
              { 
                    JS_FreeJobMaster(pstJobMasterTable);
                    return NASC_FAILURE;
              }

              /*Check the fetched record belongs to respective table */
              if(netsnmp_oid_is_subtree(tableId,
                  itableLen, pstCurrVars->name, pstCurrVars->name_length))
	       { 
		       iCheckTable = FALSE;
                    break;
               }//if

               pstJobMasterTable = malloc(sizeof(stJobMaster)); 
               if(pstJobMasterTable == NULL)
               {
#ifdef JS_DEBUG	   
                   JS_Log("Memory allocation failed \n "); 			
#endif
                   return NASC_INSUFFICIENT_MEMORY;
               }//if

	       /*Set the default value */
               pstJobMasterTable->iJobMasInterval = -1;                       
              /*Get the MasterID */
              if(JS_FillStruct((&pstJobMasterTable->pcJobMasId),pstCurrVars)==0)
              {
                      pstJobMasterTable->pcJobMasId = NULL;		      
              }
              /*Get the Index of the table*/ 
              nextMasterId       = pstCurrVars->name;
              nextMasterIdLen    = pstCurrVars->name_length;

	      iLen=0;
              iLen = pstCurrVars->name_length-1;
              pstJobMasterTable->iJobMasIndex = (I16) pstCurrVars->name[iLen];
              
              /*Get the Schedule name */
              pstCurrVars = pstCurrVars->next_variable;
	      if(pstCurrVars == NULL )
              {
                      JS_FreeJobMaster(pstJobMasterTable);
		      return NASC_FAILURE;
              }

              if(JS_FillStruct((&pstJobMasterTable->pcJobMasName),pstCurrVars)==0)
	      {
                     pstJobMasterTable->pcJobMasName = NULL;
	      }
                  
              nextMasterName      = pstCurrVars->name;
              nextMasterNameLen   = pstCurrVars->name_length;
              
              /*Get the Periodic Interval */
              pstCurrVars = pstCurrVars->next_variable;
	      if(pstCurrVars == NULL )
              {
                      JS_FreeJobMaster(pstJobMasterTable);
		      return NASC_FAILURE;
              }
              pstJobMasterTable->iJobMasInterval = (I16)(*pstCurrVars->val.integer);  
              nextMasterInterval    =  pstCurrVars->name;
              nextMasterIntervalLen = pstCurrVars->name_length;
              
              /*Get the Start Time */
              pstCurrVars = pstCurrVars->next_variable;
	      if(pstCurrVars == NULL )
              {
                      JS_FreeJobMaster(pstJobMasterTable);
		      return NASC_FAILURE;
              }
              if((JS_FillStruct((&pstJobMasterTable->pcJobMasStartTime),
                   pstCurrVars))== NASC_INSUFFICIENT_MEMORY )
                  return NASC_FAILURE;
              
              nextMasterStartTime    =  pstCurrVars->name;
              nextMasterStartTimeLen = pstCurrVars->name_length;
              
              /*Get the Finish Time */
              pstCurrVars = pstCurrVars->next_variable;
	      if(pstCurrVars == NULL )
              {
                      JS_FreeJobMaster(pstJobMasterTable);
		      return NASC_FAILURE;
              }
              if((JS_FillStruct((&pstJobMasterTable->pcJobMasFinishTime),
                    pstCurrVars))== NASC_INSUFFICIENT_MEMORY)
                  return NASC_FAILURE;
              
              nextMasterFinishTime    =  pstCurrVars->name;
              nextMasterFinishTimeLen = pstCurrVars->name_length;
              
              /*Get the CPU Usage */
              pstCurrVars = pstCurrVars->next_variable;
	      if(pstCurrVars == NULL )
              {
                      JS_FreeJobMaster(pstJobMasterTable);
		      return NASC_FAILURE;
              }
              pstJobMasterTable->iJobMasCPUUsage = (I16)(*pstCurrVars->val.integer);  
              nextMasterCPUUsage    =  pstCurrVars->name;
              nextMasterCPUUsageLen = pstCurrVars->name_length;
              
              /*Get the Memory Usage */
              pstCurrVars = pstCurrVars->next_variable;
	      if(pstCurrVars == NULL )
              {
                      JS_FreeJobMaster(pstJobMasterTable);
		      return NASC_FAILURE;
              }
              pstJobMasterTable->iJobMasMemoryUsage = (I16)(*pstCurrVars->val.integer);  
              nextMasterMemoryUsage    =  pstCurrVars->name;
              nextMasterMemoryUsageLen = pstCurrVars->name_length;

              /*Get the row status */
              pstCurrVars = pstCurrVars->next_variable;
	      if(pstCurrVars == NULL )
              {
                      JS_FreeJobMaster(pstJobMasterTable);
		      return NASC_FAILURE;
              }
              pstJobMasterTable->iJobMasStatus = (I16)(*pstCurrVars->val.integer);  
              nextMasterStatus    =  pstCurrVars->name;
              nextMasterStatusLen = pstCurrVars->name_length;
   
	      if(pstJobMasterTable->iJobMasStatus ==2) 
	      {
	         /*Move the Pointer to the next location*/  
                  pstCurrVars = pstCurrVars->next_variable;
#ifdef JS_DEBUG
                 JS_Log(" Recieved not in service records , so continue \n");		  
#endif		  
                 continue;
	      }
		      
	      JS_UpdateJobToScheduler(pstJobMasterTable, JS_ADD);
             
	         /*Move the Pointer to the next location*/  
              pstCurrVars = pstCurrVars->next_variable;
	      
              
       }//for iProcessCount
       
       if(iCheckTable == FALSE)
       {
            break; 		   
       }		   
     } // end of while(1)
     /* Free the respPDU */
     if(pstrespPDU != NULL)
     {
          snmp_free_pdu(pstrespPDU); 	       
          pstrespPDU = NULL;
     }
    return NASC_SUCCESS;
}//end of JS_GetJobProcessMasterTableDetails


/*****************************************************************************
 *
 * name             :   JS_FreeJobMaster 
 * description      :   Free Job Process Master Table Details 
 * input parameters :   pstJSMaster Pointer to the stJobMaster structure 
 * return type      :   - 
 * global variables :   
 * calls	    :   
 *                      
*****************************************************************************/
void JS_FreeJobMaster( stJobMaster *pstJSMaster)
{
      if(pstJSMaster == NULL) 	
      {
          return;	      
      }
      /*Check for other fields */
       if(pstJSMaster->pcJobMasId!= NULL )
       { 
            free(pstJSMaster->pcJobMasId); 
	    pstJSMaster->pcJobMasId=NULL;
       }
       if(pstJSMaster->pcJobMasName!= NULL )
       { 
            free(pstJSMaster->pcJobMasName); 
	    pstJSMaster->pcJobMasName = NULL;
       }
       if(pstJSMaster->pcJobMasStartTime!= NULL )
       { 
            free(pstJSMaster->pcJobMasStartTime); 
	    pstJSMaster->pcJobMasStartTime = NULL;
       }
       if(pstJSMaster->pcJobMasFinishTime!= NULL )
       { 
            free(pstJSMaster->pcJobMasFinishTime); 
	    pstJSMaster->pcJobMasFinishTime = NULL;
       }
       free(pstJSMaster);
}

/*****************************************************************************
 *
 * name             :   JS_FillStruct 
 * description      :   Alocates memorry for length and copy the value
 * input parameters :   pcDst -> pointer to be char (Name will be copied)
 *                      pstCurrVars -> Pointer to the netsnmp_variable_list list
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :   - 
 * calls	    :   - 
 *                      
*****************************************************************************/
I16 JS_FillStruct(UI8  **pcDst,  netsnmp_variable_list *pstCurrVars)
{
        I16 iLen;	
        iLen =0;
        iLen =  pstCurrVars->val_len;
	if(iLen ==0)
	{
             return NASC_FAILURE;		
	}
        *pcDst  = malloc(iLen+1);
        if(*pcDst == NULL)
        {
#ifdef JS_DEBUG	   
           JS_Log("Memory allocation failed \n ");	
#endif	   
           return NASC_INSUFFICIENT_MEMORY;
        }
        memset(*pcDst,0,(iLen+1));
        memcpy(*pcDst,pstCurrVars->val.string,iLen);
	return NASC_SUCCESS;
}
/*****************************************************************************
 *
 * name             :   JS_UpdateJobToScheduler  
 * description      :   Adds the Job the scheduler 
 * input parameters :   pstJSStruct -> pointer to the stJobMaster table
 *                      iDecisionFlag -> JS_ADD / JS_MODIFY / JS_DEL values
 * return type      :   JS_SUCCESS / JS_FAILURE / JS_NO_RECORD_FOUND
 * global variables :   - 
 * calls	    :   
 * 		        JS_AddJobToSched
 *                      JS_ModifyJobToSched
 * 		        IM_ModifyRule
 * 		        JS_DeleteFromSched
*****************************************************************************/
I16 JS_UpdateJobToScheduler( stJobMaster  *pstJSStruct, I16 iDecisionFlag)
{
    
    I16 iResult;

    if(pstJSStruct == NULL)
    {
#ifdef JS_DEBUG	   
         JS_Log("No record found \n");	    
#endif	 
         return NASC_FAILURE;   				
    }
    
    switch(iDecisionFlag)
    {
        case JS_ADD:
             iResult =0; 		
             iResult = JS_AddJobToSched(pstJSStruct); 		
	     if(!iResult)
             {
#ifdef JS_DEBUG	   
             JS_Log("Error : Error occured while adding task to scheduler \n");		     
#endif	     
             }
             break;		
        case JS_MODIFY:
             iResult =0; 		
             iResult = JS_ModifyJobToSched(pstJSStruct); 		
	     if(!iResult)
             {
#ifdef JS_DEBUG	   
                  JS_Log("Error : Error occured while modifying task to scheduler \n");		     
#endif		  
             }
	     
             break;		
        case JS_DELETE:
             iResult =0; 		
             iResult = JS_DeleteFromSched(pstJSStruct); 		
	     if(!iResult)
             {
#ifdef JS_DEBUG	   
                  JS_Log("Task ID not found in the list \n");		     
#endif		  
             }
             break;		
    }	    
    return 1;
}

/*****************************************************************************
 * name             :   JS_ScheduleLogInfo
 * description      :   Add the Log file transfer to the schedule 
 * input parameters :   
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :   - 
 * calls	    :   
 * 		        SCH_AddTask
*****************************************************************************/ 
I16 JS_ScheduleLogInfo()	   
{

    stTask task; 
    static long lTaskId=-1;
    struct tm stTm={0};
    struct timeval stTv;
    static I16 iInterVal=0;
    I16 iMinute=0;
    I16 iHour =0;

    if(backupInterval == iInterVal)
    {
#ifdef JS_DEBUG
    JS_Log(" Interval  matches .. No need to Add the task again \n");
#endif	    
    return NASC_SUCCESS;
    }
    iInterVal= backupInterval;
#ifdef JS_DEBUG
    JS_Log("JS_ScheduleLogInfo... Entering \n");
#endif
    gettimeofday(&stTv, (struct timezone *)0);
    memcpy(&stTm, localtime((time_t *) & stTv.tv_sec), sizeof(stTm));

    iHour = (stTm.tm_min + 2) / 60; 
    iMinute = (stTm.tm_min + 2) % 60; 
    /*Fill the task structure for current date and time*/
    task.StartTime.ui16Hour   =   stTm.tm_hour + iHour ;
    task.StartTime.ui16Minute =   iMinute;
    task.StartTime.ui16Second =   stTm.tm_sec;
    task.StartDate.ui16Day    =   stTm.tm_mday;
    task.StartDate.ui16Month  =   stTm.tm_mon;
    task.StartDate.ui16Year   =   stTm.tm_year +1900; 


    /*Here the task should be periodic*/
    task.eType  = SCH_PERIODIC; 
    task.ePCity = SCH_SEC_MIN_HOUR;

    pthread_mutex_lock(&log_mutex);	     
    task.ui32Interval = backupInterval;
    pthread_mutex_unlock(&log_mutex);	     
    
    task.eWeekDay = SCH_ALL_DAYS; 
    
    task.fp = JS_CallJobLogFile;

    if(lTaskId == -1)
    {
#ifdef JS_DEBUG
         JS_Log("No need to delete the task from scheduler\n ");	    
#endif
    }else
    {
	  if(SCH_DeleteTask(lTaskId) == NASC_SUCCESS)   
          {
#ifdef JS_DEBUG
         JS_Log("Task has been deleted \n ");	    
#endif
	  }
    }

    /*Add to the scheduler */
    lTaskId = SCH_AddTask(task);           	
#ifdef JS_DEBUG    
    printf("Task ID is %ld \n",lTaskId );
#endif    

    if(lTaskId == -1)
    {
#ifdef JS_DEBUG
         JS_Log("Error occured while adding the task to scheduler \n ");	    
#endif
    }
#ifdef JS_DEBUG
    JS_Log("JS_ScheduleLogInfo... Leaving\n");
#endif
    return NASC_SUCCESS;

}

/*****************************************************************************
 * name             :   JS_CallJobLogFile
 * description      :   Create a SendLogfile thread for sending log file to 
 *                      remote log file server    
 * input parameters :   
 * return type      :   - 
 * global variables :   - 
 * calls	    :   
 * 		        JS_SendLogFile 
*****************************************************************************/
void JS_CallJobLogFile(void *args)
{
     pthread_t logThread;	
     if(pthread_create(&logThread,NULL,(void*)JS_SendLogFile,NULL) != 0)
     {
	  return;    
     }
}

/*****************************************************************************
 * name             :   JS_SendLogFile
 * description      :   Sends the log file to the remote server 
 * input parameters :   - 
 * return type      :   - 
 * global variables :   - 
 * calls	    :   
 * 		        JS_GetAgentInfoMIB
 * 		        FileTrans_SendFile
*****************************************************************************/
void JS_SendLogFile()
{
      struct sockaddr_in sock_addr;
      I16 iError;
      I16 iResult;
      char buff[16];
     
#ifdef JS_DEBUG	   
      JS_Log("SendLogFile function .. Entering\n "); 
#endif      
      sock_addr.sin_family = AF_INET;
      pthread_mutex_lock(&log_mutex);	     
      if(fpForJS != NULL)
      {
          fprintf(fpForJS, "%s",charBuff);       	  
          fclose(fpForJS);
	  fpForJS = NULL;
      }

      /*Send the log file to the manager */
      sock_addr.sin_port = htons(JobSchedServerPort);
	  memset(buff,0,16);
      sprintf(buff,"%d.%d.%d.%d",(0xff& JobSchedServerIP[0]),(0xff & 
      JobSchedServerIP[1]), (0xff & JobSchedServerIP[2]),
          (0xff& JobSchedServerIP[3]));   
      pthread_mutex_unlock(&log_mutex);	     
      sock_addr.sin_addr.s_addr =inet_addr(buff);
      memset(charBuff, '\0',C_MAX_SIZE); 
      
#ifdef JS_DEBUG	   
      JS_Log("Calling FileTrans_SendFile function \n");
#endif      
      iError = FileTrans_SendFile (JobSchedLogFileName, &sock_addr
          ,NULL, 512);             
#ifdef JS_DEBUG	   
      JS_Log("FileTrans_SendFile function exit \n");
#endif      
      if(iError == NASC_SUCCESS )
      {
#ifdef JS_DEBUG	   
          JS_Log("Log file is transferred to Server\n");
#endif	  
      }else
      {
#ifdef JS_DEBUG	   
          JS_Log("Error in sending log file to server  \n");
#endif	  
      }

      /*Call the GetAgentInfoMIB to get the Mib values */
      iResult = JS_GetAgentInfoMIB();
      if(iResult == NASC_FAILURE)
      {
#ifdef JS_DEBUG	   
	      JS_Log("Error in retrieving log file info \n");
#endif	      
            pthread_detach(pthread_self());
            pthread_exit(NULL);
      }
      iResult=0;
      /*Assign the task to scheduler */
      iResult = JS_ScheduleLogInfo();  
      if(iResult == NASC_FAILURE)
      {
#ifdef JS_DEBUG	   
         JS_Log("Error in adding log file transfer activity to scheduler \n");	   
#endif	 
      }
      /*Open the file decriptor for log file*/

      pthread_mutex_lock(&log_mutex);	     
      fpForJS = fopen(JobSchedLogFileName , "w");
      if(fpForJS == NULL)
      {	      
             pthread_mutex_unlock(&log_mutex);	     
             pthread_detach(pthread_self());
             pthread_exit(NULL);
      }
      pthread_mutex_unlock(&log_mutex);	     
      pthread_detach(pthread_self());
      pthread_exit(NULL);
#ifdef JS_DEBUG	   
      JS_Log("SendLogFile function .. Returning\n "); 
#endif      
}


/*****************************************************************************
 * name             :   JS_AddJobToSched
 * description      :   Adds the filter rule 
 * input parameters :   pstJSStruct -> pointer to the stJobMaster table
 * return type      :   NASC_SUCCESS / NASC_FAILURE  
 * global variables :   - 
 * calls	    :   
 * 		        SCH_AddTask
*****************************************************************************/
I16 JS_AddJobToSched(stJobMaster  *pstJSStruct)
{

    long lTaskId;
    UI32 ulTime;
    struct tm pstTmStart ={0};
    struct tm pstTmEnd ={0};
    stTask task; 
    I16 iRetCount;

    
    if(pstJSStruct == NULL)
    {
#ifdef JS_DEBUG	   
         JS_Log("No record found \n");	    
#endif	 
         return NASC_FAILURE;   				
    }
    

    /*fill the task structure */
    if((pstJSStruct->pcJobMasStartTime == NULL))
    {
#ifdef JS_DEBUG	   
          JS_Log("Start time is NULL ...\n");	    
	  JS_Log("Job is not added in to the scheduler \n");
#endif	  
          return NASC_FAILURE;	    
    }

    if(pstJSStruct->pcJobMasStartTime != NULL)
    {
          ulTime =0;
          ulTime = JS_FillTimeStruct(&pstTmStart,pstJSStruct->pcJobMasStartTime);  	    
         if(ulTime == 0)
         { 
#ifdef JS_DEBUG	   
              JS_Log("No Start time has been set \n"); 		 
#endif	      
              return NASC_FAILURE; 		  
         }
         /*Update the task structure */
         task.StartTime.ui16Hour   =   pstTmStart.tm_hour ;
         task.StartTime.ui16Minute =   pstTmStart.tm_min ;
         task.StartTime.ui16Second =   pstTmStart.tm_sec;
         task.StartDate.ui16Day    =   pstTmStart.tm_mday;
	 //JS_Log("Day of the month %d \n", task.StartDate.ui16Day);
         task.StartDate.ui16Month  =   pstTmStart.tm_mon;
         task.StartDate.ui16Year   =   pstTmStart.tm_year; 
	 iRetCount= JS_GetDay(ulTime);
	 //JS_Log("Day count %d \n",iRetCount);
    }


    /*Check the end time */
    if((pstJSStruct->pcJobMasFinishTime== NULL) && pstJSStruct->iJobMasInterval != 0)
    {
#ifdef JS_DEBUG	   
          JS_Log("Job will not be added to scheduler\n");	     
          JS_Log("End time is NULL ...\n");	    
#endif	  
          return NASC_FAILURE;	    
    }

    /**/
    if(pstJSStruct->iJobMasInterval != 0)
    {
         ulTime =0;
         ulTime = JS_FillTimeStruct(&pstTmEnd ,pstJSStruct->pcJobMasFinishTime);  	    
         if(ulTime == 0)
         {
#ifdef JS_DEBUG	   
              JS_Log("No Finish time has been set \n"); 		 
#endif	      
              return NASC_FAILURE; 		  
         }

         /*Fill the end time */
         pstJSStruct->uiEndTimeinNum= ulTime;
         task.EndTime.ui16Hour   =   pstTmEnd.tm_hour ;
         task.EndTime.ui16Minute =   pstTmEnd.tm_min ;
         task.EndTime.ui16Second =   pstTmEnd.tm_sec;
         task.EndDate.ui16Day    =   pstTmEnd.tm_mday;
         task.EndDate.ui16Month  =   pstTmEnd.tm_mon;
         task.EndDate.ui16Year   =   pstTmEnd.tm_year; 
    }

    /*Check if the interval is not specified */
    if(pstJSStruct->iJobMasInterval == -1)
    {
#ifdef JS_DEBUG	   
         JS_Log("Job interval is not specified ,Job can not be added to scheduler \n");	    
#endif	 
         return NASC_FAILURE;    		 
	 
    }

    /*Update the interval */
    switch(pstJSStruct->iJobMasInterval)
    {
         case 0:
               task.eType              =   SCH_ONE_SHOT; 
               task.ePCity             =   SCH_DAY; 
               task.eWeekDay           =   SCH_ALL_DAYS; 
               break;
         case 1:
               task.eType              =   SCH_PERIODIC; 
               task.ePCity             =   SCH_DAY; 
               task.eWeekDay           =   SCH_ALL_DAYS; 
               task.EndDate.ui16Day    =   pstTmEnd.tm_mday;
               task.EndDate.ui16Month  =   pstTmEnd.tm_mon;
               task.EndDate.ui16Year   =   pstTmEnd.tm_year; 
               break;
         case 2:
               task.eType              =   SCH_PERIODIC; 
               task.ePCity             =   SCH_WEEK; 
               task.eWeekDay           =   iRetCount; 
               task.EndDate.ui16Day    =   pstTmEnd.tm_mday;
               task.EndDate.ui16Month  =   pstTmEnd.tm_mon;
               task.EndDate.ui16Year   =   pstTmEnd.tm_year; 
               break;
         case 3:
               task.eType              =   SCH_PERIODIC; 
               task.ePCity             =   SCH_MONTH; 
               task.eWeekDay           =   iRetCount; 
               task.EndDate.ui16Day    =   pstTmEnd.tm_mday;
               task.EndDate.ui16Month  =   pstTmEnd.tm_mon;
               task.EndDate.ui16Year   =   pstTmEnd.tm_year; 
               break;
	 default:
#ifdef JS_DEBUG	   
               JS_Log("Job Master interval is Wrong !! \n"); 		 
	       JS_Log("Job Can not be added to scheduler \n");
#endif	       
	       break;
    }

    task.vpArg = (void*)pstJSStruct;
    task.fp = JS_PerformJobSchedAction;

    /*Add to the scheduler */
#ifdef JS_DEBUG	   
    JS_Log("Adding Task to scheduler \n ");
#endif    
    lTaskId = SCH_AddTask(task);           	
#ifdef JS_DEBUG	   
    JS_Log("Task has been added \n ");
    printf("Task ID is %ld \n",lTaskId );
#endif    
    //JS_Log("Task ID %ld \n",lTaskId);
    if(lTaskId == -1)
    {
#ifdef JS_DEBUG	   
         JS_Log("Error occured while adding the task to scheduler \n ");	    
#endif	 
	 return NASC_FAILURE;
    }
#ifdef JS_DEBUG	   
    JS_Log("Adding Task to scheduler \n ");
#endif    
    JS_AddNodeInJobList(lTaskId ,pstJSStruct->iJobMasIndex, stListPtr);
#ifdef JS_DEBUG	   
    JS_Log("Adding Task to scheduler \n ");
#endif    
    return NASC_SUCCESS;
}

/*****************************************************************************
 * name             :   JS_FillTimeStruct
 * description      :   Fill the time structure and returns 
 * input parameters :   pstTm -> pointer to the time struct 
 *                      pstTime -> pointer to the character
 * return type      :   time in long value 
 * global variables :   - 
 * calls	    :   
*****************************************************************************/
UI32  JS_FillTimeStruct(struct tm *pstTm, char *pstTime)  	    
{
    unsigned long iTime= 0;
    UI8 szStr[80] = { 0 };
    int iYear=0;

    strcpy (szStr, pstTime);

    /* Convert DateAndTime format into unsigned long format */
    iYear = szStr[0];
    iYear = (iYear << 8) | (szStr[1] & 0xff);
    pstTm->tm_year = iYear - 1900;

    pstTm->tm_mon  = (int) (szStr[2] & 0xff) - 1;
    pstTm->tm_mday = (int) (szStr[3] & 0xff);
    pstTm->tm_hour = (int) (szStr[4] & 0xff);
    pstTm->tm_min =  (int) (szStr[5] & 0xff);
    pstTm->tm_sec =  (int) (szStr[6] & 0xff);

#ifndef WIN32
    //pstTm->tm_year = iYear;
#endif
    iTime = mktime(pstTm); 

    pstTm->tm_year = iYear;


    return iTime;
}	

/*****************************************************************************
 * name             :   JS_ModifyJobToSched
 * description      :   Modify the task structure
 * input parameters :   pstJSStruct  -> pointer to the stJobMaster table  
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :   - 
 * calls	    :   JS_GetTaskID
 *                      JS_AddNodeInJobList 
 *                      SCH_AddTask
*****************************************************************************/
I16 JS_ModifyJobToSched(stJobMaster  *pstJSStruct)
{
    long lTaskId;
    long oldTaskID=0;
    long newTaskID;
    UI32 ulTime;
    struct tm pstTmStart = {0};
    struct tm pstTmEnd = {0};
    stTask *pstModTask;
    stJobMaster  *pstJSMod;
    I16 iDayCount;
    I16 iResult;

    /*Get the task ID from the list*/
    lTaskId = JS_GetTaskID(pstJSStruct->iJobMasIndex, stListPtr);
    if(lTaskId == -1)
    {
#ifdef JS_DEBUG	   
          JS_Log("No task ID found in the List \n ");		
#endif	  
          JS_FreeJobMaster(pstJSStruct);
	  return NASC_FAILURE;
    }
    /*Allocate memory for pstModTask struct*/
    pstModTask= malloc(sizeof(stTask));

    /*Get the task structure */
    iResult = SCH_GetTask(lTaskId, &pstModTask);
    if(iResult == NASC_FAILURE)
    {
#ifdef JS_DEBUG	   
          JS_Log("Error in Getting the task struct \n");	    
#endif	  
	  return NASC_FAILURE; 
    }

    if(pstJSStruct->pcJobMasStartTime != NULL) 
    {
         /*fill the task structure */
         ulTime = JS_FillTimeStruct(&pstTmStart,pstJSStruct->pcJobMasStartTime);  	    
         if(ulTime == 0)
         {
#ifdef JS_DEBUG	   
               JS_Log("No Start time has been set \n"); 		 
#endif	       
               return NASC_FAILURE; 		  
         }
         /*Update the task structure */
	 iDayCount= JS_GetDay(ulTime);
         pstModTask->StartTime.ui16Hour   =   pstTmStart.tm_hour ;
         pstModTask->StartTime.ui16Minute =   pstTmStart.tm_min ;
         pstModTask->StartTime.ui16Second =   pstTmStart.tm_sec;
         pstModTask->StartDate.ui16Day    =   pstTmStart.tm_mday;
         pstModTask->StartDate.ui16Month  =   pstTmStart.tm_mon;
         pstModTask->StartDate.ui16Year   =   pstTmStart.tm_year; 
    }

    /*Check the end time */
    if((pstJSStruct->pcJobMasFinishTime != NULL))
    {
         ulTime = JS_FillTimeStruct(&pstTmEnd ,pstJSStruct->pcJobMasFinishTime);  	    
         if(ulTime == 0)
         {
#ifdef JS_DEBUG	   
               JS_Log("No Start time has been set \n"); 		 
#endif	       
               return NASC_FAILURE; 		  
         }

         pstJSStruct->uiEndTimeinNum= ulTime;
         /*Fill the end time */
         pstModTask->EndTime.ui16Hour   =   pstTmEnd.tm_hour ;
         pstModTask->EndTime.ui16Minute =   pstTmEnd.tm_min ;
         pstModTask->EndTime.ui16Second =   pstTmEnd.tm_sec;
         pstModTask->EndDate.ui16Day    =   pstTmEnd.tm_mday;
         pstModTask->EndDate.ui16Month  =   pstTmEnd.tm_mon;
         pstModTask->EndDate.ui16Year   =   pstTmEnd.tm_year; 

	 if(pstJSStruct->iJobMasInterval != -1)
	 {
              /*Update the interval */
              switch(pstJSStruct->iJobMasInterval)
              {
		      
              case 0:
                    pstModTask->eType              =   SCH_ONE_SHOT; 
                    pstModTask->ePCity             =   SCH_DAY; 
                    pstModTask->eWeekDay           =   SCH_ALL_DAYS; 
                    break;
              case 1:
                    pstModTask->eType              =   SCH_PERIODIC; 
                    pstModTask->ePCity             =   SCH_DAY; 
                    pstModTask->eWeekDay           =   SCH_ALL_DAYS; 
                    pstModTask->EndDate.ui16Day    =   pstTmEnd.tm_mday;
                    pstModTask->EndDate.ui16Month  =   pstTmEnd.tm_mon;
                    pstModTask->EndDate.ui16Year   =   pstTmEnd.tm_year; 
                    break;
              case 2:
                    pstModTask->eType              =   SCH_PERIODIC; 
                    pstModTask->ePCity             =   SCH_WEEK; 
                    pstModTask->eWeekDay           =   iDayCount; 
                    pstModTask->EndDate.ui16Day    =   pstTmEnd.tm_mday;
                    pstModTask->EndDate.ui16Month  =   pstTmEnd.tm_mon;
                    pstModTask->EndDate.ui16Year   =   pstTmEnd.tm_year; 
                    break;
              case 3:
                    pstModTask->eType              =   SCH_PERIODIC; 
                    pstModTask->ePCity             =   SCH_MONTH; 
                    pstModTask->eWeekDay           =   iDayCount; 
                    pstModTask->EndDate.ui16Day    =   pstTmEnd.tm_mday;
                    pstModTask->EndDate.ui16Month  =   pstTmEnd.tm_mon;
                    pstModTask->EndDate.ui16Year   =   pstTmEnd.tm_year; 
                    break;
              default:
#ifdef JS_DEBUG	   
                    JS_Log("Job Master interval is Wrong !! \n"); 		 
	            JS_Log("Job Can not be added to scheduler \n");
#endif		    
                    break;
            }
	 }
    }

    /*Modify the Master ID with respect to Modified name*/ 

    /*Get the void pointer struct*/
    pstJSMod = pstModTask->vpArg;
    
    if(pstJSStruct->pcJobMasId != NULL)
    {
         free(pstJSMod->pcJobMasId);

 	 pstJSMod->pcJobMasId = malloc(strlen(pstJSStruct->pcJobMasId)+1);
	 if(pstJSMod->pcJobMasId == NULL)
               return NASC_FAILURE;		 
	 memset(pstJSMod->pcJobMasId,0,strlen(pstJSStruct->pcJobMasId)+1);
	 strcpy(pstJSMod->pcJobMasId,pstJSStruct->pcJobMasId);
    }

    /*Modify the Master name*/
    if(pstJSStruct->pcJobMasName!= NULL)
    {
	 free(pstJSMod->pcJobMasName);
 	 pstJSMod->pcJobMasName = malloc(strlen(pstJSStruct->pcJobMasName)+1);
	 if(pstJSMod->pcJobMasName == NULL)
             return NASC_FAILURE; 		 
	 memset(pstJSMod->pcJobMasName, 0, strlen(pstJSStruct->pcJobMasName)+1);
	 strcpy(pstJSMod->pcJobMasName,pstJSStruct->pcJobMasName);
    }

    /*Copy the Start Time */
    if(pstJSStruct->pcJobMasStartTime != NULL)
    {
	 free(pstJSMod->pcJobMasStartTime);
 	 pstJSMod->pcJobMasStartTime = malloc(strlen(pstJSStruct->pcJobMasStartTime)+1);
	 if(pstJSMod->pcJobMasStartTime== NULL)
             return NASC_FAILURE; 		 
	 memset(pstJSMod->pcJobMasStartTime, 0, strlen(pstJSStruct->pcJobMasStartTime)+1);
	 memcpy(pstJSMod->pcJobMasStartTime,pstJSStruct->pcJobMasStartTime,
              strlen(pstJSStruct->pcJobMasStartTime) );
    }

    /*Copy the End Time */
    if(pstJSStruct->pcJobMasFinishTime != NULL)
    {
	 free(pstJSMod->pcJobMasFinishTime);
 	 pstJSMod->pcJobMasFinishTime = malloc(strlen(pstJSStruct->pcJobMasFinishTime)+1);
	 if(pstJSMod->pcJobMasFinishTime== NULL)
             return NASC_FAILURE; 		 
	 memset(pstJSMod->pcJobMasFinishTime, 0, strlen(pstJSStruct->pcJobMasFinishTime)+1);
	 memcpy(pstJSMod->pcJobMasFinishTime,pstJSStruct->pcJobMasFinishTime,
              strlen(pstJSStruct->pcJobMasFinishTime));
    }

    
    /*Comparing the End time interval */ 
    pstJSMod->uiEndTimeinNum   = pstJSStruct->uiEndTimeinNum;	    

    pstModTask->vpArg = (void*)pstJSMod;
	 
    pstModTask->fp = JS_PerformJobSchedAction;
    /*Add to the scheduler */
    newTaskID =0;
    oldTaskID = lTaskId;
    newTaskID = SCH_ModifyTask(lTaskId, *pstModTask);           	
    //JS_Log("JS_ModifyJobToSched: Modified Task Id %ld \n",newTaskID );

    if(newTaskID != -1)
    {

    /*delete the old task from the list*/  	    
    iResult = JS_DeleteNodeFromList(stListPtr,pstJSStruct->iJobMasIndex ,oldTaskID);
    if(iResult ==0)
    {
#ifdef JS_DEBUG	   
          JS_Log("Error in deleting the task ID from the list \n");   		 
#endif	  
          return NASC_FAILURE;

    }else
    {
#ifdef JS_DEBUG	   
         // JS_Log("Node deleted from the list \n");   		 
#endif	  
    }
  
    /*Add the returned node in the List*/  	    
    JS_AddNodeInJobList(newTaskID,pstJSStruct->iJobMasIndex ,stListPtr); 

    }else
    {
#ifdef JS_DEBUG	   
          JS_Log("Error adding the task to scheduler \n");   		 
#endif	  
  	    
    }
    
    JS_FreeJobMaster(pstJSStruct);
    return NASC_SUCCESS;
}

/*****************************************************************************
 * name             :   JS_DeleteFromSched
 * description      :   Delete from the scheduler  
 * input parameters :   pstJSStruct -> pointer to the stJobMaster table
 * return type      :   NASC_SUCCESS / NASC_FAILURE  
 * global variables :   - 
 * calls	    :   JS_GetTaskID
 *                      JS_DeleteNodeFromList
 *                      SCH_DeleteTask
*****************************************************************************/
I16 JS_DeleteFromSched(stJobMaster  *pstJSStruct)
{
    long lDelTaskId;
    UI16 iResult;
    stTask *pstModTask;
    stJobMaster  *pstJSMod;

    if(pstJSStruct == NULL)
    {
#ifdef JS_DEBUG	   
         JS_Log("No record found \n");	    
#endif	 
         JS_FreeJobMaster(pstJSStruct);
         return NASC_FAILURE;   				
    }
    /*Get the task ID*/ 	    
    lDelTaskId = JS_GetTaskID(pstJSStruct->iJobMasIndex,stListPtr );    
    if(lDelTaskId == -1)
    {
#ifdef JS_DEBUG	   
         JS_Log("No Such record to be deleted \n");	    
#endif	   
         JS_FreeJobMaster(pstJSStruct);
	 return NASC_FAILURE;
    }

    /*Allocate memory for pstModTask struct*/
    pstModTask= malloc(sizeof(stTask));

    /*Get the task structure */
    iResult = SCH_GetTask(lDelTaskId, &pstModTask);
    if(iResult == NASC_FAILURE)
    {
#ifdef JS_DEBUG	   
          JS_Log("Error in Getting the task struct \n");	    
#endif	  
	  return NASC_FAILURE; 
    }

    /*Store the vp arg pointer for freeing*/
    /*Get the void pointer struct*/
    pstJSMod = pstModTask->vpArg;

    iResult = SCH_DeleteTask(lDelTaskId);

    if(iResult ==0)
    {
#ifdef JS_DEBUG	   
   	    JS_Log("Error in deleting the task \n");   		 
#endif	    
	  return NASC_FAILURE;
    }else
    {
#ifdef JS_DEBUG	   
          JS_Log("Task has been deleted from the list successfully \n");   		 
#endif	   
          if(pstJSMod != NULL )
          { 
             JS_FreeJobMaster(pstJSMod); 
             pstJSMod = NULL;
          }
          if(pstModTask != NULL)
          {
                   free(pstModTask);
                   pstModTask = NULL; 
          }
          
    }
    iResult = JS_DeleteNodeFromList(stListPtr,pstJSStruct->iJobMasIndex ,lDelTaskId);
    if(iResult ==0)
    {
#ifdef JS_DEBUG	   
          JS_Log("Error in deleting the task ID from the list \n");   		 
#endif	  
    }else
    {
#ifdef JS_DEBUG	   
          JS_Log("Node deleted from the list \n");   		 
#endif	  
          JS_FreeJobMaster(pstJSStruct);
          return NASC_SUCCESS;
    }
    return NASC_SUCCESS;
}

/*****************************************************************************
 * name             :   JS_GetDay 
 * description      :   Get the Day (Sun... Sat) of the week 
 * input parameters :   iTime -> struct variable for time structure
 * return type      :   NASC_SUCCESS / NASC_FAILURE  
 * global variables :   - 
 * calls	    :   JS_GetDayNum 
*****************************************************************************/
I16 JS_GetDay(time_t iTime )
{
       UI8 cDay[4];
       char cTimeBuff[100];
       UI8 cOthers[30];

       sprintf(cTimeBuff,"%s",ctime(&iTime));
       //JS_Log("Time is %s \n",cTimeBuff);
       sscanf(cTimeBuff,"%s %s",cDay,cOthers );
       //JS_Log("Day is %s \n",cDay);

       return JS_GetDayNum(cDay);
}

/*****************************************************************************
 * name             :   JS_GetDayNum
 * description      :   Get the Day (0..6) (Sun... Sat) of the week 
 * input parameters :   pcDay -> pointer to string "sun,Mon,...Sat" 
 * return type      :   Returns 0..6 numbers respect to Sun...Sat 
 * global variables :   - 
 * calls	    :   NASC_FAILURE /  day count
*****************************************************************************/
I16 JS_GetDayNum(UI8 *pcDay)
{
     I16 iCount;	
     for(iCount=0; iCount< JS_DAY_CNT;iCount++)	
     {
         if(strcmp(pcDay,Days[iCount])==0)	
         {
                 return iCount;		 
         }
     }
     return NASC_FAILURE; 
}


/*****************************************************************************
 *
 * name             :   JS_InitCBuffer  
 * description      :   Initialize the circular queue 
 * input parameters :   pstBuffer pointer to the circular buffer queue 
 * return type      :   void 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
void JS_InitCBuffer(SCBuffer *pstCBuffer)
{
  /*Initialize the circular queue*/	
  pstCBuffer->iQRear = -1;
  pstCBuffer->iQFront = 0;
  pthread_mutex_init(&(pstCBuffer->queue_lock) , NULL);
  pthread_cond_init(&(pstCBuffer->queue_not_full) , NULL);
  pthread_cond_init(&(pstCBuffer->queue_not_empty) , NULL);
}

/*****************************************************************************
 *
 * name             :   JS_IsQueueEmpty  
 * description      :   Check if the circular queue is empty 
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   BOOL 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
BOOL  JS_IsQueueEmpty(SCBuffer *pstCBuffer)
{
  /*Check Queue is empty*/	
  BOOL status = FALSE;
  if(((pstCBuffer->iQRear + 1) % (JS_MAX_ELEMENT)) == pstCBuffer->iQFront)
  {
    status = TRUE;
  }
  return status;
}

/*****************************************************************************
 *
 * name             :   JS_IsQueue_Full  
 * description      :   Check if the circular queue is full 
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   BOOL 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
BOOL  JS_IsQueue_Full(SCBuffer *pstCBuffer)
{
  /*Check queue is full*/	
  BOOL status = FALSE;
  if(((pstCBuffer->iQRear + 2) % (JS_MAX_ELEMENT )) == pstCBuffer->iQFront)
  {
    status = TRUE;
  }
  return status;

}
/*****************************************************************************
 *
 * name             :   JS_ProcessTrapMsg 
 * description      :   Copy the trap PDU and process the same  
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   void 
 * global variables :   - 
 * calls	    :   JS_IsQueueEmpty
 *                      JS_ParseTrap   
 * 		        
*****************************************************************************/
void JS_ProcessTrapMsg(void *args)
{
  netsnmp_pdu *trapPDU;
  netsnmp_pdu *pstClonePDU;
  
  
  /*Thread function to process the trap message*/
  while(1)
  {
#ifdef JS_DEBUG	   
       JS_Log("Before Mutex lock \n");	  
#endif       
       pthread_mutex_lock(&(stCB.queue_lock));
#ifdef JS_DEBUG	   
       JS_Log("Enter into queue \n");
#endif       
       if(JS_IsQueueEmpty(&stCB))
       {
#ifdef JS_DEBUG	   
           JS_Log("Queue is Empty \n");	  
           JS_Log("JS_ProcessTrapMsg Queue Empty.. waiting for signal \n");
#endif	   
           pthread_cond_wait(&(stCB.queue_not_empty) ,
               &(stCB.queue_lock));
#ifdef JS_DEBUG	   
	   JS_Log ("Got signal..\n");
#endif	   
       }

#ifdef JS_DEBUG	   
       JS_Log("Reading from queue \n");
#endif       
       trapPDU =  stCB.queueEntry[stCB.iQFront];
#ifdef JS_DEBUG	   
       JS_Log("Doing SNMP Clone \n");
#endif       
       pstClonePDU= snmp_clone_pdu(trapPDU);
       snmp_free_pdu(trapPDU);  
       stCB.queueEntry[stCB.iQFront]= NULL;

       stCB.iQFront = ++(stCB.iQFront) % (JS_MAX_ELEMENT); 
       pthread_mutex_unlock(&(stCB.queue_lock));
       /*Call function to parse the retrieved trap message */     
#ifdef JS_DEBUG	   
       JS_Log("Calling JS_Parse Trap \n");
#endif       
       JS_ParseTrap(pstClonePDU);
  }
}

/*****************************************************************************
 *
 * name             :   JS_ListenTrap 
 * description      :   Listens the trap message and copy the same in
 *                      to the circular buffer 
 * input parameters :   
 *                      id-> number
 *                      sess -> pointer to the snmp_session
 *                      pdu -> pointer to the nsetsnmp_pdu structure 
 *                      arg -> pointer to the void
 *                      req -> request number
 * return type      :   NASC_SUCCESS 
 * global variables :   - 
 * calls	    :   JS_IsQueue_Full 
 * 		        
*****************************************************************************/
int JS_ListenTrap(int id, struct snmp_session *sess, int req, netsnmp_pdu *pdu, void *arg)
{
    netsnmp_pdu *pstPDUClone;

    /* Call back function to write the trap message in to circular queue */ 	

#ifdef JS_DEBUG	   
    JS_Log("Received Trap message \n ");
#endif    

    pthread_mutex_lock(&(stCB.queue_lock));
    if(JS_IsQueue_Full(&stCB))
    {
#ifdef JS_DEBUG	   
      JS_Log("Dropping the Trap Msg \n");
#endif      
      pthread_mutex_unlock(&(stCB.queue_lock));
      /*Error should be logged in the syslog */
    }
#ifdef JS_DEBUG	   
    JS_Log("Before incrementing rear \n ");
#endif    
    stCB.iQRear = ++(stCB.iQRear) % (JS_MAX_ELEMENT);
    pstPDUClone = snmp_clone_pdu(pdu);
    stCB.queueEntry[stCB.iQRear] = pstPDUClone;
#ifdef JS_DEBUG	   
    JS_Log("Signalling  \n");
#endif    
    pthread_cond_signal(&(stCB.queue_not_empty)); 
    pthread_mutex_unlock(&(stCB.queue_lock));
    return 1;
                                                                    
}

/*****************************************************************************
 *
 * name             :   JS_ParseTrap 
 * description      :   Parse the trap message and calls the function for 
 *                      adding the task to scheduler 
 * input parameters :   respPDU -> pointer to the snmp_pdu structure 
 * return type      :   void 
 * global variables :   - 
 * calls	    :   
 *                      JS_CheckTable
 *                      JS_GetIndex
 *                      JS_FillStruct
 *                      JS_UpdateJobToScheduler
*****************************************************************************/
void JS_ParseTrap( struct snmp_pdu *respPDU)
{
    I16 iOperFlag;
    I16 iDecisionFlag;
    netsnmp_variable_list *vars;
    netsnmp_variable_list *pstCurrVars;
    I16 iFieldFlag= -1 ;
    stJobMaster *pstJSMaster;
    I16 iResult=0;

    
    vars = respPDU->variables;
    pstCurrVars = vars;

    
    iOperFlag = JS_CheckTable(pstCurrVars);
    if(iOperFlag == -1)
    {
#ifdef JS_DEBUG	   
         JS_Log("Not belong to this table \n");	    
#endif	 
         return;
    }

    /*Allocate Memory for Generic table structure*/
    pstJSMaster=malloc(sizeof(stJobMaster));
    if(pstJSMaster == NULL)
    {
#ifdef JS_DEBUG	   
         JS_Log("Memory allocation failed \n "); 			
#endif	 
         return ;
    }
    if(pstCurrVars != NULL)
    {
         /*Get the Index ID of the table*/
         pstJSMaster->iJobMasIndex = JS_GetIndex(pstCurrVars);
    }else
    {
         if(pstJSMaster != NULL ) free(pstJSMaster); 	    
#ifdef JS_DEBUG	   
         JS_Log("No record exists \n");	    
#endif	 
	 return ;
    }
    
    /*Make default entry as modify, If add / delete trap comes this field 
     * will be modified inside this while loop */
    pstJSMaster->iJobMasStatus = 0;
    pstJSMaster->iJobMasInterval = -1;
    pstJSMaster->pcJobMasId = NULL;
    pstJSMaster->pcJobMasName = NULL;
    pstJSMaster->pcJobMasStartTime = NULL;
    pstJSMaster->pcJobMasFinishTime =  NULL;
    

    while(pstCurrVars != NULL) 
    {
           iFieldFlag = JS_CheckField(pstCurrVars);  	    
	   switch(iFieldFlag)
           {
             case JS_MASTER_ID:
                   /*Get the Master ID */
          	   if(JS_FillStruct((&pstJSMaster->pcJobMasId),pstCurrVars)
                        == NASC_INSUFFICIENT_MEMORY) 
		   {
                      JS_FreeJobMaster(pstJSMaster);
                      return ;		      
		   }
		   break;
             case JS_MASTER_NAME:
                   /*Get the Master Name */
          	   if(JS_FillStruct((&pstJSMaster->pcJobMasName),pstCurrVars)
                        == NASC_INSUFFICIENT_MEMORY)
                   {
                      JS_FreeJobMaster(pstJSMaster);
                      return ;		      
                   }
                   break;		     
             case JS_PERIODIC_INTERVAL:
                   /*Get the Interval */
                   pstJSMaster->iJobMasInterval= (I16)(*pstCurrVars->val.integer);
                   break;
             case JS_START_TIME:
                   /*Get the Start Time*/
                   if(JS_FillStruct((&pstJSMaster->pcJobMasStartTime),pstCurrVars)
                        == NASC_INSUFFICIENT_MEMORY)
                   {
                      JS_FreeJobMaster(pstJSMaster);
                      return ;		      
                   }
                   break;
             case JS_FINISH_TIME:
                   /*Get the End Time*/
          	   if(JS_FillStruct((&pstJSMaster->pcJobMasFinishTime),pstCurrVars) 
                        == NASC_INSUFFICIENT_MEMORY)
                   {
                      JS_FreeJobMaster(pstJSMaster);
                      return ;		      
                   }
                   break;
             case JS_ROW_STATUS:
                   /*Get the row status */
                   pstJSMaster->iJobMasStatus = (I16)(*pstCurrVars->val.integer);  
                   break;

             default :
#ifdef WIN32
					break;
#endif
#ifdef JS_DEBUG	   
                   JS_Log("Received other records so continue..\n");
#endif		   
		   
           }

	    /*Move to the next record */ 
	    pstCurrVars = pstCurrVars->next_variable;
    }

 
    /*Fine the decision flag*/
    /*Check record is for Add / Modify / Del */
    switch(pstJSMaster->iJobMasStatus)
    {
          case 4:
          case 1:
                 iDecisionFlag = JS_ADD;
                 break;
          case 6:
          case 2:
                 iDecisionFlag = JS_DELETE;
                 break;
          default:
                 iDecisionFlag = JS_MODIFY;
                 break;
    }
   
#ifdef JS_DEBUG	   
    JS_Log("Calling JS_UpdateJobToScheduler \n"); 
#endif    
    iResult = JS_UpdateJobToScheduler(pstJSMaster, iDecisionFlag);
#ifdef JS_DEBUG	   
    JS_Log("JS_UpdateJobToScheduler over \n"); 
#endif    
     /* Free the respPDU */
     if(respPDU != NULL)
     {
          snmp_free_pdu(respPDU); 	       
          respPDU = NULL;
     }
    return ;
    
}

/*****************************************************************************
 *
 * name             :   JS_GetIndex
 * description      :   Get the index 
 * input parameters :   pstCurrVars-> pointer to the netsnmp_variable_list structure 
 * return type      :   Index number 
 * global variables :   - 
 * calls	    :   -
*****************************************************************************/
I16 JS_GetIndex(netsnmp_variable_list *pstCurrVars)
{
       I16 iLen=0;
       I16 iIndex=-1;
       iLen = pstCurrVars->name_length-1;
       /*Get the Index ID of the table*/
       iIndex = (I16)pstCurrVars->name[iLen];
       return  iIndex;
}
/*****************************************************************************
 *
 * name             :   JS_CheckTable  
 * description      :   Function finds pstCurrVars is belong to the table 
 * input parameters :   pstCurrVars -> pointer to the netsnmp_variable_list
 * return type      :   Return -1 if the fetched record is not belonging to 
 *                      the Job process Master table Else returns 1 
 * global variables :    
 * calls	    :    
*****************************************************************************/
I16 JS_CheckTable(netsnmp_variable_list *pstCurrVars)
{
     I16 iOperFlag; 
     I16 iLen;

     iOperFlag =-1;
    /*Check the fetched record is pertaining to Job Master Table */

     iLen =0;
     iLen = sizeof(jobProcessMasterTable_variables_oid) / sizeof(oid);
     if(!netsnmp_oid_is_subtree(jobProcessMasterTable_variables_oid,
         iLen, pstCurrVars->name, pstCurrVars->name_length))
     { 
         iOperFlag = 1;
     }

    return iOperFlag;
}

/*****************************************************************************
 * name             :   JS_CheckField
 * description      :   Function finds the field type in Job Process Master table 
 * input parameters :   pstSNMPPDU -> Pointer to the netsnmp_variable_list
 * return type      :   JS_MASTER_ID   / JS_MASTER_NAME / JS_PERIODIC_INTERVAL
 *                      JS_START_TIME  / JS_FINISH_TIME / JS_CPUUSAGE 
 *                      JS_MEMORYUSAGE / JS_ROW_STATUS  
 * global variables :   - 
 * calls	    :   - 
*****************************************************************************/
I16 JS_CheckField( netsnmp_variable_list *pstSNMPPDU )
{
     I16 iLen;
     I16 iRetValue=-1;

     iLen=sizeof(jobProcessMasterID_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(jobProcessMasterID_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue= JS_MASTER_ID;	     
     }
     iLen=sizeof(jobProcessMasterName_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(jobProcessMasterName_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =JS_MASTER_NAME;	     
     }
     
     iLen=sizeof(jobProcessMasterPeriodicInterval_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(jobProcessMasterPeriodicInterval_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =JS_PERIODIC_INTERVAL;	     
     }

     iLen=sizeof(jobProcessMasterStartTime_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(jobProcessMasterStartTime_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =JS_START_TIME;	     
     }
     
     iLen=sizeof(jobProcessMasterFinishTime_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(jobProcessMasterFinishTime_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =JS_FINISH_TIME;	     
     }
     iLen=sizeof(jobProcessMasterCPUPercentUsage_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(jobProcessMasterCPUPercentUsage_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =JS_CPUUSAGE;	     
     }
     iLen=sizeof(jobProcessMasterMemoryPercentUsage_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(jobProcessMasterMemoryPercentUsage_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {

          iRetValue =JS_MEMORYUSAGE;	     
     }
     iLen=sizeof(jobProcessMasterStatus_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(jobProcessMasterStatus_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =JS_ROW_STATUS;	     
     }
     return iRetValue;
}

/*****************************************************************************
 * name             :   JS_PerformJobSchedAction
 * description      :   Create a thread for perform Job action 
 * input parameters :   args -> Pointer to the void
 * return type      :   void 
 * global variables :    
 * calls	    :    
*****************************************************************************/
void JS_PerformJobSchedAction(void *args)
{
     pthread_t jobThread;

     /*Create a thread to perform the Job action*/
     if((pthread_create(&jobThread,NULL,(void*)JS_SpawnJobSched,args)) != 0)
     {
         return ;	     
     }
    
}

/*****************************************************************************
 * name             :   JS_SpawnJobSched
 * description      :   Performs Job action 
 * input parameters :   pvjobPtr-> Pointer to the void
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :    
 * calls	    :   JS_DoJobSched
 *                      JS_FillStruct     
 *                      JS_FreeJob 
*****************************************************************************/
I16 JS_SpawnJobSched(void *pvjobPtr)
{
    
    struct snmp_pdu *reqPDU;
    struct snmp_pdu *respPDU;
    netsnmp_variable_list *vars;
    netsnmp_variable_list *pstCurrVars;
    UI32 uiCurrentTime;	
    oid *pstDet1;
    I16 iDetLen1; 
    oid *pstDet2;
    I16 iDetLen2; 
    oid *pstStatus;
    I16 iStatLen;
    
    oid *tableID;
    I16 tableLen;
    oid *pstField;
    I16 iDetLen; 
    I16 iProcessCount;
    I16 status=0;
    I16 iLen;
    
    stJobMasterSched *pstjob;
    stJobMaster *pstJobMaster;
    
    I16 iCheckTable; 
    I16 iResult; 

    /*Fetch the Detail table records  */  
    pstJobMaster = (stJobMaster *)pvjobPtr;
    
      uiCurrentTime = JS_GetCurrentTime(); 
     /*Check the current time with finish time 
      * Dont check for one shot */         
     if((pstJobMaster->uiEndTimeinNum < uiCurrentTime))  
     {
#ifdef JS_DEBUG	   
          JS_Log("Current Time is exceeded the end time \n");	     
#endif	  
          pthread_detach(pthread_self());
          pthread_exit(NULL);
	  return NASC_FAILURE;
     }
	
    tableID = JobProcessDetailTable_variables_oid;
    tableLen = sizeof(JobProcessDetailTable_variables_oid)/ sizeof(oid);
    
    pstDet1  = jobProcessDetailID_variables_oid;
    iDetLen1 = sizeof(jobProcessDetailID_variables_oid)/ sizeof(oid); 

    pstField = pstDet1;
    iDetLen =  iDetLen1; 
	    
    pstDet2  = jobProcessDetailScriptFile_variables_oid;
    iDetLen2 = sizeof(jobProcessDetailScriptFile_variables_oid)/ sizeof(oid);

    pstStatus = jobProcessDetailStatus_variables_oid;
    iStatLen = sizeof(jobProcessDetailStatus_variables_oid)/ sizeof(oid);

    while(1) 
    {
       reqPDU = snmp_pdu_create(SNMP_MSG_GETBULK);
       reqPDU->non_repeaters = 0;
       /*Set the number of records to be fetched */
       reqPDU->max_repetitions = MAX_REPETITIONS;
       
       /*Allocate memory for stJobMasterSched structure */
       /*pstjob = malloc(sizeof(stJobMasterSched));
       if(pstjob == NULL)
       {
             return NASC_FAILURE;	       
       }
       pstjob->pcMasterId=  NULL;
       pstjob->pcMasterScriptFile =  NULL;*/

       /*Form the Job master ID */	    
       iLen =0; 
       snmp_add_null_var(reqPDU,pstDet1,iDetLen1);
       snmp_add_null_var(reqPDU,pstDet2,iDetLen2);
       snmp_add_null_var(reqPDU,pstStatus,iStatLen);
      
       /*Get the response in the respPDU*/
       status = snmp_synch_response(agentSess,reqPDU,&respPDU);	
       if(status==STAT_SUCCESS)
       {
	   if(respPDU->errstat==SNMP_ERR_NOERROR && 
           respPDU->errindex==0 && 
           respPDU->variables->type != ASN_NULL &&
           respPDU->variables->type != SNMP_NOSUCHINSTANCE &&
           respPDU->variables->type != SNMP_NOSUCHOBJECT  &&
	   respPDU->variables->type !=  SNMP_ENDOFMIBVIEW )
	   {
#ifdef JS_DEBUG	   
	          JS_Log("sync response success \n");	   
#endif		  
	   }else
	   {
                  //JS_FreeJob(pstjob); 		   
#ifdef JS_DEBUG	   
	          JS_Log("sync response failure \n");	   
#endif		  
                  pthread_detach(pthread_self());
                  pthread_exit(NULL);
                  return NASC_FAILURE;		   
	   }
           if(respPDU == NULL)	        
	   {
                //JS_FreeJob(pstjob); 		   
#ifdef JS_DEBUG	   
                JS_Log("Error in sync response \n");		   
#endif		
                pthread_detach(pthread_self());
                pthread_exit(NULL);
		return NASC_FAILURE;
	   }
       } else 
       {
#ifdef JS_DEBUG	   
           JS_Log("Error in snmp_synch_response \n");
#endif	   
           snmp_perror("Error ");   	   
               pthread_detach(pthread_self());
               pthread_exit(NULL);
               return NASC_FAILURE;		   
       }

       
       vars = respPDU->variables;
       pstCurrVars = vars;

       /*Check the fetched record is belong to back up master table */
       if(netsnmp_oid_is_subtree(tableID,
          tableLen, pstCurrVars->name, pstCurrVars->name_length))
       { 
#ifdef JS_DEBUG	   
             JS_Log("Error : No record found for this table \n");
#endif	     
             //JS_FreeJob(pstjob); 		   
              pthread_detach(pthread_self());
              pthread_exit(NULL);
              return NASC_FAILURE;	       
       }
       
       /*Flag for checkiing whether the fetched table details belongs to 
	* required table*/ 
       iCheckTable = TRUE;
       for(iProcessCount =0; iProcessCount < MAX_REPETITIONS;
             iProcessCount++ )
       {
              /*Check the fetched record is belong to respective table */
              if(netsnmp_oid_is_subtree(pstField,
                 iDetLen, pstCurrVars->name, pstCurrVars->name_length))
	      { 
		       iCheckTable = FALSE;
                       break;
              }
	       /*Allocate memory for stJobMasterSched structure */
	       pstjob = malloc(sizeof(stJobMasterSched));
	       if(pstjob == NULL)
	       {
		     return NASC_FAILURE;	       
	       }
	       pstjob->pcMasterId=  NULL;
	       pstjob->pcMasterScriptFile =  NULL;
         
              /*Allocate memory and give the struct*/ 
	      
	      /*Get the Id */
	      if((JS_FillStruct((&pstjob->pcMasterId), pstCurrVars)) == 
                   NASC_INSUFFICIENT_MEMORY)
              {
                   JS_FreeJob(pstjob); 		   
                   pthread_detach(pthread_self());
                   pthread_exit(NULL);
                   return NASC_FAILURE;		      
              }
                              
	      pstDet1  = pstCurrVars->name;
              iDetLen1 = pstCurrVars->name_length;
	      /*Move the Pointer to the next location*/  
              pstCurrVars = pstCurrVars->next_variable;


              /*Get the Script file*/
	      iResult = JS_FillStruct((&pstjob->pcMasterScriptFile), pstCurrVars);
              if(iResult ==  NASC_INSUFFICIENT_MEMORY)
              {
                   JS_FreeJob(pstjob); 		   
                   pthread_detach(pthread_self());
                   pthread_exit(NULL);
                   return NASC_FAILURE;		      
              }
              
	       
              pstDet2  = pstCurrVars->name;
              iDetLen2 = pstCurrVars->name_length;
	      /*Move the Pointer to the next location*/  
              pstCurrVars = pstCurrVars->next_variable;

	      pstStatus = pstCurrVars->name;
	      iStatLen =  pstCurrVars->name_length;

              pstjob->iStatus =0;
	      /*Check the row status is inactive */ 
              if(pstCurrVars->val.integer != NULL)
                  pstjob->iStatus  = (I16) *(pstCurrVars->val.integer);	      
            
	      if((pstjob->iStatus == 2) || (pstjob->iStatus ==0)) 
	      {
#ifdef JS_DEBUG
               JS_Log("Received not in service records .. so continue \n");		      
#endif		      
               pstCurrVars = pstCurrVars->next_variable;
               continue;
	      }
	      if(pstjob->pcMasterId != NULL)
	      {
                  if((strcmp(pstjob->pcMasterId , pstJobMaster->pcJobMasId)) !=0 ) 	                        {
	              /*Move the Pointer to the next location*/  
#ifdef JS_DEBUG			  
                       JS_Log("Master ID Does not match .. so continue \n ");	     
#endif			  
                       pstCurrVars = pstCurrVars->next_variable;
		       continue;
                  }
	      }

	      /*Move the Pointer to the next location*/  
              pstCurrVars = pstCurrVars->next_variable;
              JS_DoJobSched(pstjob,pstJobMaster);
	      /*Free  */

       }
       if(iCheckTable == FALSE)
       {
           break; 		   
       }		   
    }
    pthread_detach(pthread_self());
    pthread_exit(NULL);
    return NASC_SUCCESS;
}

/*****************************************************************************
 * name             :   JS_FreeJob
 * description      :   Free stJobMasterSched structure 
 * input parameters :   pstjob-> Pointer to the stJobMasterSched strucutre 
 * return type      :   void 
 * global variables :    
 * calls	    :   
*****************************************************************************/
void JS_FreeJob(stJobMasterSched  *pstjob)
{
     /*Check for NULL*/	
     if(pstjob == NULL) 	
     {
#ifdef JS_DEBUG	   
          JS_Log("No need to free \n ");	     
#endif	  
	  return;
     }
     if(pstjob->pcMasterId != NULL) 	
     {
          free(pstjob->pcMasterId);	     
	  pstjob->pcMasterId = NULL;
     }
     if(pstjob->pcMasterScriptFile != NULL) 	
     {
          free(pstjob->pcMasterScriptFile);	     
	  pstjob->pcMasterScriptFile = NULL;
     }
     free(pstjob);
     pstjob = NULL;
}	

/*****************************************************************************
 * name             :   JS_DoJobSched 
 * description      :   Perform the Job action 
 * input parameters :   pstjob-> Pointer to the stJobMasterSched strucutre 
 *                      pstJobMaster->Pointer to the stJobMaster
 * return type      :   void 
 * global variables :    
 * calls	    :   JS_CheckScriptAvail
 *                      JS_GetCPUUtilization
 *                      FileTrans_SendFile
 *                      JS_DoSNMPSet
*****************************************************************************/
void JS_DoJobSched(stJobMasterSched *pstjob, stJobMaster *pstJobMaster)
{
      I16 iResult;
      char localBuff[10000];
      struct sockaddr_in sock_addr;
      I16 iError;
      char buff[15];
       struct tm pstTmStart = {0};
      UI32 ulTime;
      UI8 cStartDate[60];
      UI8 cEndDate[60];
      UI8 *cMastName; 
      I16 iTotLen;
      I16 iNoScriptFlg=0;


     /*Check the script file is available */	
      iNoScriptFlg=0;
      iResult = JS_CheckScriptAvail(pstjob->pcMasterScriptFile);  	
      if(iResult == NASC_FAILURE)
      {
             JS_FreeJob(pstjob); 		   
        
#ifdef JS_DEBUG	   
             JS_Log("No script is available in specified path \n");	      
#endif	     
	     iNoScriptFlg=1;
      }

      if(iNoScriptFlg != 1 )
      {
           if(JS_GetCPUUtilization(&pstJobMaster,pstjob )  == NASC_FAILURE)
           {
                 JS_FreeJob(pstjob); 		   
                 return;	      
           }
        
      
           /*Perform the Job processing */
#ifdef JS_DEBUG	   
           JS_Log(" Job Schdule was over \n");
#endif  
           memset(localBuff,0,10000) ;

/*Added for Solaris    */
  
              ulTime = JS_FillTimeStruct(&pstTmStart,pstJobMaster->pcJobMasStartTime);  	    
              if(ulTime == 0)
              {
#ifdef JS_DEBUG	   
                    JS_Log("No Start time has been set \n"); 		 
#endif	       
              }

              memset(cStartDate,0,60); 
             /*Add the Date format */	 
              sprintf(cStartDate, "Year %d Mon %d Day %d Hour%d Min%d Sec%d",
              (pstTmStart.tm_year), (pstTmStart.tm_mon+1), pstTmStart.tm_mday,
              pstTmStart.tm_hour ,pstTmStart.tm_min, pstTmStart.tm_sec);

              printf("Start Date %s\n",cStartDate);
	 
              ulTime = JS_FillTimeStruct(&pstTmStart,pstJobMaster->pcJobMasFinishTime);  	   
              if(ulTime == 0)
              {
#ifdef JS_DEBUG	   
                    JS_Log("No End time has been set \n"); 		 
#endif	       
              }

              memset(cEndDate,0,60); 
             /*Add the Date format */	 
              sprintf(cEndDate, "Year %d Mon %d Day %d Hour%d Min%d Sec%d",
              (pstTmStart.tm_year), (pstTmStart.tm_mon+1), pstTmStart.tm_mday,
              pstTmStart.tm_hour ,pstTmStart.tm_min, pstTmStart.tm_sec);

              printf("End Date %s\n",cEndDate);
              printf("ID %d \n",pstJobMaster->iJobMasIndex );
              printf("Master ID %s \n",pstJobMaster->pcJobMasId);
              printf("CPU usage %d \n",pstJobMaster->iJobMasCPUUsage);
              printf(" Master status %d \n",pstJobMaster->iJobMasStatus);
              
   

              /*Check Master Name and Master ID */
             if(pstJobMaster->pcJobMasName == NULL)
             {
                 cMastName = malloc(5);
                 memset(cMastName,0,5);
                 strcpy(cMastName,"None");
             }else
             {
                 cMastName = malloc(strlen(pstJobMaster->pcJobMasName)+1);
		 memset(cMastName,0,strlen(pstJobMaster->pcJobMasName)+1);
                 strcpy(cMastName, pstJobMaster->pcJobMasName);
             }
        
             sprintf(localBuff,"%s;%s;%d;%s;%s;%d;%d \n",cStartDate,
             cEndDate,pstJobMaster->iJobMasIndex, pstJobMaster->pcJobMasId,
             cMastName,pstJobMaster->iJobMasCPUUsage,pstJobMaster->iJobMasStatus);
      }else 
      {
             sprintf(localBuff, "No Script file is avaliable for %s master ID\n", 
                 pstJobMaster->pcJobMasId); 	      
      }
      pthread_mutex_lock(&log_mutex);	     
      strcat(charBuff,localBuff);
      iTotLen = strlen(charBuff);
      pthread_mutex_unlock(&log_mutex);	      


      if(strlen(charBuff) > (C_MAX_SIZE -200))
      {
           pthread_mutex_lock(&log_mutex);	     
	   if(fpForJS != NULL)
	   {
                fprintf(fpForJS, "%s",charBuff);       	  
                fclose(fpForJS);
		fpForJS = NULL;
	   }
            memset(charBuff, 0,C_MAX_SIZE); 
	   /* Transfer the file to FTP server */
           sock_addr.sin_family = AF_INET;
           sock_addr.sin_port = htons(JobSchedServerPort);
           sprintf(buff,"%d.%d.%d.%d",(0xff& JobSchedServerIP[0]),(0xff & 
           JobSchedServerIP[1]), (0xff & JobSchedServerIP[2]),
           (0xff& JobSchedServerIP[3]));
           pthread_mutex_unlock(&log_mutex);	     

           sock_addr.sin_addr.s_addr =inet_addr(buff);
           iError = FileTrans_SendFile (JobSchedLogFileName, &sock_addr
                 ,NULL, 512);             
           if(iError == NASC_SUCCESS )
           {
#ifdef JS_DEBUG	   
               JS_Log("Log file is transferred to Server\n");
#endif	       
           }else
           {
#ifdef JS_DEBUG	   
               JS_Log("Error in sending log file to server  \n");
#endif	       
           }
           
           /*Open the file decriptor for log file*/
           fpForJS = fopen(JobSchedLogFileName, "a+");
           if(fpForJS == NULL)
	   {
               if(cMastName != NULL)
               {
                  free(cMastName );
                  cMastName = NULL;   	  
               }
               return;	   
	   }
      }

      /*Check the flag and return */
      if(iNoScriptFlg ==1 )
      {
          if(cMastName != NULL)
          {
              free(cMastName );
              cMastName = NULL;   	  
          }
             return;	      
      }	      

      // pstJobMaster->iJobMasMemoryUsage = (int)((pstJobMaster->iJobMasMemoryUsage / lTotalSize) * 100);

       //JS_Log("Memory Utilization %d \n ",pstJobMaster->iJobMasMemoryUsage );
      /*Form the OID with the index */ 

      /*Form the OID with the index */ 
      JS_DoSNMPSet(CPUUsage,
            pstJobMaster->iJobMasCPUUsage,pstJobMaster->iJobMasIndex);
      
#ifdef JS_DEBUG
      printf("Memory utilization %d \n" ,pstJobMaster->iJobMasMemoryUsage );
#endif       
      JS_DoSNMPSet(MemUsage,
            pstJobMaster->iJobMasMemoryUsage,pstJobMaster->iJobMasIndex);
      /*Form SNMP Packet to set the values in MIB*/ 

      /*Check the end Time is crossed the current time */

      /*Free the  pstJob structure*/
      if(pstjob != NULL)
         JS_FreeJob(pstjob);

      if(cMastName != NULL)
      {
          free(cMastName );
          cMastName = NULL;   	  
      }
      
}

/*****************************************************************************
 * name             :   JS_DoSNMPSet
 * description      :   Set the Memory and CPU utilization in to MIB   
 * input parameters :   OID-> Pointer to the OID 
 *                      iIndex-> index variable
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :    
 * calls	    :   
*****************************************************************************/
I16 JS_DoSNMPSet(char *OID, I16 value, I16 iIndex)
{
	int status;
	//char type = 'i';
	struct snmp_pdu *pdu, *response = NULL;
	oid name[MAX_OID_LEN];
	size_t name_len = MAX_OID_LEN;
        char cOIDForSet[MAX_OID_LEN];	
	I16 iLen=0;
	char *pcOIDToSet;
        //unsigned long ltmp = 90;
	char cpValue[10];
	
	//init_snmp ("snmprocess2");

	memset(cOIDForSet,0,MAX_OID_LEN);
	sprintf(cOIDForSet,"%s.%d",OID,iIndex);

	iLen = strlen(cOIDForSet);
        pcOIDToSet= malloc(iLen+1);
        if(pcOIDToSet == NULL)
             return NASC_FAILURE;
        memset(pcOIDToSet,0,(iLen+1));
        memcpy(pcOIDToSet,cOIDForSet,iLen);

	
        if(readoid(pcOIDToSet, name, &name_len)==0)
        {
             pthread_mutex_unlock(&job_mutex);	     
             printf("Error in read_objid 1\n"); 
             return NASC_FAILURE;
         }       
	/* TODO: confirm we're setting the right OID ! */
	pdu = snmp_pdu_create(SNMP_MSG_SET);
	if (pdu == NULL)
	{
#ifdef JS_DEBUG	   
	  JS_Log("Not enough memory \n");
#endif	  
	  return NASC_FAILURE;
        }	

	memset(cpValue,0,10);
	sprintf(cpValue,"%d",value);
        cpValue[strlen(cpValue)] =0;	

	snmp_add_var(pdu,name,name_len,'i',cpValue);
        //setValue = value;
	/*snmp_pdu_add_variable(pdu,name ,name_len , ASN_INTEGER, 
				(unsigned char*)&value, sizeof(setValue));*/


        status = snmp_synch_response(agentSess, pdu, &response);
        if (status == STAT_SUCCESS) {
             if (response->errstat == SNMP_ERR_NOERROR) 
	     {
#ifdef JS_DEBUG	   
		JS_Log("Snmp Set success \n");
#endif	     
	     }
             else {
                    /*JS_Log("Error in packet.\nReason: %s\n",
                    snmp_errstring(response->errstat));*/
             if (response->errindex != 0) {
#ifdef JS_DEBUG		     
                    JS_Log("Failed object: \n");
#endif		    
            }
          }
	}
        else if (status == STAT_TIMEOUT) {
           /*JS_Log("Timeout: No Response from %s\n",
                agentSess->peername);*/
        } else {                    /* status == STAT_ERROR */
        snmp_sess_perror("snmpset", agentSess);
     }

    if (response)
        snmp_free_pdu(response); 

    if(pcOIDToSet != NULL)    
    {
         free(pcOIDToSet);
         pcOIDToSet = NULL;	 
    }
	
    return NASC_SUCCESS;
}


/*****************************************************************************
 * name             :   JS_CheckScriptAvail
 * description      :   Check script is available in the specified path 
 * input parameters :   pcScript-> Pointer to the script 
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :    
 * calls	    :   
*****************************************************************************/
I16 JS_CheckScriptAvail(UI8 *pcScript)
{
      struct stat stStat;
      if(stat(pcScript, &stStat) == 0)       
      {
            return NASC_SUCCESS;       
      }else
      {
            return NASC_FAILURE;	      
      }
}

/*****************************************************************************
 * name             :   JS_GetCPUUtilization 
 * description      :   Get the CPU utilization and memory utilization 
 * input parameters :   pstJobMaster-> Pointer to the stJobMaster structure
 *                      pstjob -> Pointer to the stJobMasterSched structure 
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :    
 * calls	    :   
*****************************************************************************/
#ifdef WIN32
I16 JS_GetCPUUtilization(stJobMaster **pstJobMaster , stJobMasterSched *pstjob)
{
 /*Do the Job Processing*/  
   

    I16 iLen=0;  
    
	DWORD exitstatus;
	PROCESS_MEMORY_COUNTERS stpmc;

    I16 iRetval;
	FILETIME KernalTime, UserTime;
	FILETIME Enter, Exit;
	LARGE_INTEGER sumKernal, sumUser;
	LARGE_INTEGER Kernal, User;
	long lProcMemsize=0;
	UI32 lcount=0;
	float fCurrentAmount=0;

	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo; 
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );  
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO); 
	sumKernal.QuadPart=0;
    sumUser.QuadPart=0;


    init_snmp ("snmprocess1");
    if (pstjob->pcMasterScriptFile == 0)
		return NASC_FAILURE;


	iRetval=CreateProcess(pstjob->pcMasterScriptFile, // name of executable module
						NULL,                      // command line string
						NULL, // SD
						NULL,  // SD
						FALSE,                      // handle inheritance option
						0,						
						NULL,                      // new environment block
						NULL,                // current directory name
						&siStartInfo,&piProcInfo
						);
	if(iRetval==0)
	{
		return NASC_FAILURE;
	}
	else
	{
		while(1)
		{
			Sleep(100);
			GetExitCodeProcess(piProcInfo.hProcess,&exitstatus);
			if(exitstatus==STILL_ACTIVE)
			{
				
				if(GetProcessTimes(piProcInfo.hProcess, &Enter, &Exit, &KernalTime, &UserTime)!=0)
				{
					if(GetProcessMemoryInfo(piProcInfo.hProcess, &stpmc, sizeof(stpmc)))
					{
						memcpy(&Kernal, &KernalTime, sizeof(FILETIME));
						memcpy(&User, &UserTime, sizeof(FILETIME));
						//Get a running total of the process times

						sumKernal.QuadPart+=Kernal.QuadPart;
						sumUser.QuadPart+=User.QuadPart;     
				  
						lcount ++;
						lProcMemsize = lProcMemsize + stpmc.PeakWorkingSetSize;
					}
				}
			}
			else
				break;
		}
		fCurrentAmount=(float)((sumKernal.QuadPart+sumUser.QuadPart)) 
												/ (float)(100000 * lcount);

		 if(lcount != 0)
		       lProcMemsize = lProcMemsize/lcount;
			
    //sprintf(buffer, "Processor Usage (%0.2f) from %d/%d", CurrentAmount, ProcCount, MaxCount);
    //Msg(buffer, 0, 0);
		if (fCurrentAmount < 0)
			 fCurrentAmount=0;
		if (fCurrentAmount > 100)
			 fCurrentAmount=100;
		
		
		(*pstJobMaster)->iJobMasCPUUsage  = (I16) fCurrentAmount;        
                

	  //JS_Log("CPU Utilization %d \n",(*pstJobMaster)->iJobMasCPUUsage  );

          
		(*pstJobMaster)->iJobMasMemoryUsage = (I16)((100 * lProcMemsize)/ lTotalSize);
		CloseHandle( piProcInfo.hProcess );
		CloseHandle( piProcInfo.hThread );

	  
	  //JS_Log("Memory Utilization %d \n",(*pstJobMaster)->iJobMasMemoryUsage);
	}
	    
		
      return NASC_SUCCESS;
}

#else
I16 JS_GetCPUUtilization(stJobMaster **pstJobMaster , stJobMasterSched *pstjob)
{
 /*Do the Job Processing*/



      char *argv[5];
      struct snmp_pdu *pstreqPDU;
      struct snmp_pdu *pstrespPDU;

#if defined(LINUX_JS) || defined(AIX_JS) || defined(HP_UX_JS) 
      pid_t pid;
#else
      long pid;
#endif      
      netsnmp_variable_list *pstCurrVars;
      extern char **environ;
      char oidCPUPercent[] = ".1.3.6.1.4.1.3204.1.3.33.1.1.15";
      char oidMemUtil[] = ".1.3.6.1.4.1.3204.1.3.33.1.1.26";
      char oidCPU[40];
      char oidMem[40];
      I16 iStatus;
      I16 iLen=0;
      char *pcCPUPer;
      char *pcMemPer;
      oid name[MAX_OID_LEN];
      size_t name_len = MAX_OID_LEN;
      int *status;
      //argv[0] = "sh";
      argv[0] = "/bin/sh";
      argv[1] = "-c";


      pthread_mutex_lock(&job_mutex);	     
      if (pstjob->pcMasterScriptFile == NULL)
      {
          pthread_mutex_unlock(&job_mutex);	     
          return NASC_FAILURE;
      }

      /*Get the PID */
      pid = fork();
      
      if (pid == -1)
      {
         pthread_mutex_unlock(&job_mutex);	     
         return NASC_FAILURE;
      }
      if (pid == 0) {
             argv[2] = pstjob->pcMasterScriptFile;
             printf(" Script %s \n", argv[2]);  
             argv[3] = 0;
             argv[4] = 0;
           
             if(execve("/bin/sh", argv, environ)== -1)
             {
                      exit(127);
             }
      }
      do {

/*In order to get the CPU utilzation sleep for 6 seconds */
            //sleep (6);
            memset(oidCPU,0,40);	      
            memset(oidMem,0,40);	      

#if defined(LINUX_JS) || defined(AIX_JS) || defined(HP_UX_JS) 
            /*Get the cpu and memory utilization */	     
            sprintf(oidCPU, "%s.%d",oidCPUPercent,pid);	      
            sprintf(oidMem, "%s.%d",oidMemUtil,pid);	      
#else
            sprintf(oidCPU, "%s.%ld",oidCPUPercent,pid);	      
            sprintf(oidMem, "%s.%ld",oidMemUtil,pid);
#endif

	    iLen = strlen(oidCPU);
          
	    pcCPUPer = malloc(iLen+1);
	    if(pcCPUPer == NULL)
            {
                    pthread_mutex_unlock(&job_mutex);	     
		    return NASC_FAILURE;
            }
	    memset(pcCPUPer,0,(iLen+1));
	    memcpy(pcCPUPer,oidCPU,iLen);

            iLen=0;
	    iLen = strlen(oidMem);
          
	    pcMemPer = malloc(iLen+1);
	    if(pcMemPer == NULL)
            {
                    pthread_mutex_unlock(&job_mutex);	     
		    return NASC_FAILURE;
             }
	    memset(pcMemPer,0,(iLen+1));
	    memcpy(pcMemPer,oidMem,iLen);
           
            memset(name,0,MAX_OID_LEN);
	    /*Added for CPU Utilization */
            
            name_len = MAX_OID_LEN;
	          
           
           
            if(readoid(pcCPUPer, name, &name_len)==0)
            {
                pthread_mutex_unlock(&job_mutex);	     
                printf("Error in read_objid 1\n"); 
	    	return NASC_FAILURE;
            }       
	    
	    pstreqPDU = snmp_pdu_create(SNMP_MSG_GET);
            pstreqPDU->non_repeaters = 0;
            
            snmp_add_null_var(pstreqPDU,name,name_len);

            memset(name,0,MAX_OID_LEN);
	    name_len = MAX_OID_LEN;

	    /*Added for CPU Utilization */
        
            if(readoid(pcMemPer, name, &name_len)==0)
            {
                pthread_mutex_unlock(&job_mutex);	     
                printf("Error in read_objid 2\n"); 
		FreeMemAndCPU(pcCPUPer,pcMemPer );
	    	return NASC_FAILURE;
            }       
	    
            snmp_add_null_var(pstreqPDU,name,name_len);

            iStatus = snmp_synch_response(agentSess,pstreqPDU,&pstrespPDU);	
           if(iStatus==STAT_SUCCESS)
           {
                if(pstrespPDU->errstat==SNMP_ERR_NOERROR && 
                 pstrespPDU->errindex==0 && 
                 pstrespPDU->variables->type != ASN_NULL &&
                 pstrespPDU->variables->type != SNMP_NOSUCHINSTANCE &&
                 pstrespPDU->variables->type != SNMP_NOSUCHOBJECT ) {
	        }else
                {
                      snmp_perror("Error :Error Valu 1 \n ");		    
                      pthread_mutex_unlock(&job_mutex);	     
		      FreeMemAndCPU(pcCPUPer,pcMemPer );
                      return NASC_FAILURE;		   
                }

                if(pstrespPDU == NULL)	        
	        {
                    snmp_perror("Error : Error value 2\n ");   	   
                    pthread_mutex_unlock(&job_mutex);	     
		    FreeMemAndCPU(pcCPUPer,pcMemPer );
                    return NASC_FAILURE;
                }
           } else 
           {
           snmp_perror("Error in retrieving CPU util value \n");   	   
              pthread_mutex_unlock(&job_mutex);	     
               FreeMemAndCPU(pcCPUPer,pcMemPer );
               return NASC_FAILURE;		   
           }


          /*Get the CPU utilization */
	    
          pstCurrVars = pstrespPDU->variables;

          if(pstCurrVars->val.integer != NULL)
          (*pstJobMaster)->iJobMasCPUUsage  = (I16) *(pstCurrVars->val.integer);
          pstCurrVars = pstCurrVars->next_variable;

          /*Get the Memory utilization */
          if(pstCurrVars->val.integer != NULL)
           (*pstJobMaster)->iJobMasMemoryUsage = (I16) *(pstCurrVars->val.integer);
	    
        
          if (waitpid(pid, status, 0) == -1) {
          if (errno != EINTR)
          {
              pthread_mutex_unlock(&job_mutex);	     
              FreeMemAndCPU(pcCPUPer,pcMemPer );
              return NASC_FAILURE;
          }
            } else 
          {
              pthread_mutex_unlock(&job_mutex);	     
              FreeMemAndCPU(pcCPUPer,pcMemPer );
              return NASC_SUCCESS;
          }
       } while(1);
       /* Free the respPDU */
       if(pstrespPDU != NULL)
       {
            snmp_free_pdu(pstrespPDU); 	       
	    pstrespPDU = NULL;
       }
     
      pthread_mutex_unlock(&job_mutex);	     
      return NASC_SUCCESS;

}
#endif


/*****************************************************************************
 * name             :   FreeMemAndCPU 
 * description      :   Free the pointers pcCPUPer , pcMemPer 
 * input parameters :   pcCPUPer ->pointer to char 
 * 		    :   pcMemPer->pointer to char 
 * output parameters:   -
 * return type      :   - 
 * global variables :   - 
 * calls            :   -
 *****************************************************************************/
void FreeMemAndCPU(char *pcCPUPer, char *pcMemPer )
{
	if(pcCPUPer != NULL)
	{
              free(pcCPUPer);		
	      pcCPUPer = NULL;
	}
	if(pcMemPer != NULL)
	{
              free(pcMemPer);		
	      pcMemPer = NULL;
	}
	return;
}

/*****************************************************************************
 * name             :   JS_GetCurrentTime
 * description      :   Get the current time
 * input parameters :   -
 * output parameters:   -
 * return type      :   time in long value 
 * global variables :
 * calls            :   
 *****************************************************************************/

UI32 JS_GetCurrentTime ()
{
    struct tm stTm ={0};
    struct timeval stTv;
    UI32 iTime;

    gettimeofday(&stTv, (struct timezone *) 0);
    memcpy(&stTm, localtime((time_t *) & stTv.tv_sec), sizeof(stTm));
    iTime = mktime(&stTm);

    return iTime;
}

/*****************************************************************************
 *
 * name             :   JS_Log
 * description      :   Writes the log message in to log file 
 * input parameters :   logMsg ->pointer to the log message
 * return type      :   void 
 * global variables :   ifdForJS-> File descriptor for storing the 
 *                      log message in to log file  
 * calls	    :   - 
 * 		        
*****************************************************************************/
void JS_Log(UI8 *logMsg)
{
#ifdef JS_DEBUG	   
/*   ifdForJS= fopen("JS_LOG_FILE","a+");
   if((ifdForJS == NULL))
   {
       JS_Log("Error in opening the file \n");		   
       return;
   }
   fprintf(ifdForJS,"%s\n",logMsg );	
   fclose(ifdForJS);*/
#ifdef JS_PRINT
   printf("%s",logMsg);	
#endif   
#endif       
       	
}

/*****************************************************************************
 * name             :   readoid 
 * description      :   Converts oid string notation to oid octet notation 
 * input parameters :   oidString->pointer to char 
 *                      name_len -> aximum oid Length
 * return type      :   Returns oid size 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
int readoid(char *oidString, oid *name, size_t *nameLen) {
    char *oidPtr;
    char temp[MAX_OID_LEN+1];
    size_t oidSize=0;
    int i=0;
    int j=0;
    int k=0;
    oidPtr = oidString;
    while(oidPtr[j]){
        if(oidPtr[j]=='.'){
            if(i!=0){
                temp[i]='\0';
                name[k++]=atoi(temp);
                oidSize++;
                i=0;
            }
        }else{
            temp[i++]=oidPtr[j];
            if(i>MAX_OID_LEN){
                return 0;
            }
        }
        j++;
    }
    temp[i]='\0';
    name[k]=atoi(temp);
    oidSize++;
    *nameLen = oidSize;
    return oidSize;
}

/*****************************************************************************
 *
 * name             :   main 
 * description      :   Start the Start Job process and calls the JS_InitSession
 *                      to initialize the snmp session
 * input parameters :  
 * return type      :   int 
 * global variables :   
 * calls	    :   JS_StartJobSchedProcess
 *                      JS_MakeEmpty
 *                      JS_InitSession
*****************************************************************************/
#ifdef WIN32
int main()
{
    pthread_t thID;

	MEMORYSTATUS stmemstat;
	I16 iResult=0;

    stListPtr = JS_MakeEmpty(NULL);
    /*Get the Total emory*/
	GlobalMemoryStatus(&stmemstat);
	lTotalSize =stmemstat.dwTotalPhys;
    
    //JS_Log("Memory %ld\n",lTotalSize );
    /*Open the session with the NASCenter sub agent*/	
	memset(charBuff,0,1000000); 
    JS_InitSession();
    if((pthread_create(&thID,NULL,(void*)JS_Read_Packets,NULL)) != 0)
    {
          return 0;     	     
    }
    iResult = JS_StartJobSchedProcess();
    if(iResult == NASC_FAILURE)
    {
         SCH_StopScheduler(); 	    
	 exit(0);
    }

    pthread_join(thID, NULL);
    return 0;
}

#else

int main()
{
    pthread_t thID;
    I16 iResult=0;

#if defined(LINUX_JS) || defined(SOLARIS_JS) || defined(AIX_JS) 
    long lPageSize;                  
    long lTotPages; 
#endif

#ifdef TRU_64_JS
    long buffer;
    unsigned long flag =0; 
#endif

#ifdef HP_UX_JS        
    struct pst_static stat_buf;        
#endif

    stListPtr = JS_MakeEmpty(NULL);

#if defined(LINUX_JS) || defined(SOLARIS_JS) || defined(AIX_JS) 
    /*Get the Total emory*/
    lPageSize = sysconf(_SC_PAGE_SIZE);
    lTotPages = sysconf(_SC_PHYS_PAGES);

    lTotalSize = (long)((lPageSize*lTotPages) /1024); 
    //JS_Log("Memory %ld\n",lTotalSize );
#endif

#ifdef UNIXWARE_JS
      lTotalSize = sysconf(_SC_TOTAL_MEMORY);
#endif    

#ifdef TRU_64_JS
     getsysinfo (GSI_PHYSMEM, buffer, sizeof(buffer), 0, NULL, flag);
     lTotalSize = buffer * 1024;
        
#endif

#ifdef HP_UX_JS
    pstat_getstatic(&stat_buf, sizeof(struct pst_static), 1, 0);
    printf("Physical memory = %ld bytes \n",stat_buf.physical_memory*4096); 
    lTotalSize = stat_buf.physical_memory*4096; 
#endif                                         

    memset(charBuff,0,1000000); 
    /*Open the session with the NASCenter sub agent*/	
    JS_InitSession();
    if((pthread_create(&thID,NULL,(void*)JS_Read_Packets,NULL)) != 0)
    {
          return 0;     	     
    }
    iResult =0;
    iResult = JS_StartJobSchedProcess();
    if(iResult == NASC_FAILURE)
    {
         SCH_StopScheduler(); 	    
	 exit(0);
    }
    pthread_join(thID, NULL);
    return 0;
}
#endif
/*****************************************************************************
 *
 * name             :   JS_Read_Packets
 * description      :   Read the trap information in the specified port 
 * input parameters :   args ->pointer to the coid 
 * return type      :   void 
 * global variables :   tSess -> void pointer to the session
 * calls	    :   
*****************************************************************************/
void JS_Read_Packets(void * arg){
   fd_set snmpSet;
   int snmpFds, blocking;
   struct timeval tv;
#ifdef JS_DEBUG	   
   JS_Log("Inside REad_Pkt......\n");
#endif   
   while(1){
        snmpFds=0;
        blocking=1;

#ifdef AIX_JS
          bzero(&snmpSet,sizeof(fd_set)); 
#else
       FD_ZERO(&snmpSet);   
#endif
        snmp_sess_select_info(tSess, &snmpFds,&snmpSet,&tv,&blocking);
        snmpFds = select(snmpFds,&snmpSet,0,0,0);
	if(snmpFds == -1)
	{
#ifndef WIN32
             if(errno == EBADF)
	     {
#ifdef JS_DEBUG	   
                   JS_Log("An invalid file descriptor \n ");
#endif		   
	     }else if(errno == EINTR)
	     {
#ifdef JS_DEBUG	   
                 JS_Log("A non blocked signal was caught \n");		     
#endif		 
	     }else if(errno == ENOMEM)		     
	     {
#ifdef JS_DEBUG	   
                 JS_Log("Select was unable to allocate memory non \n");		     
#endif		 
	     }
#endif
#ifdef JS_DEBUG	   
	     JS_Log("Select returned error \n");	
#endif	     
	     continue;
	}
        if(snmpFds){
#ifdef JS_DEBUG	   
            JS_Log("Inside  READ TRAP WHILE......\n");
#endif	    
            snmp_sess_read(tSess, &snmpSet);
        } else {
#ifdef JS_DEBUG	   
            JS_Log("No FileDS to READ......\n");
#endif	    
	
    } // end of while(1)
 }
}

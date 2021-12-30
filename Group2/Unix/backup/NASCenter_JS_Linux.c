/*****************************************************************************
 *
 * NASCenter_JS_Linux.c
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
 * $Log: NASCenter_JS_Linux.c,v $
 * Revision 1.1  2003/12/02 06:25:05  thiagav
 * added
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
 * $Id: NASCenter_JS_Linux.c,v 1.1 2003/12/02 06:25:05 thiagav Exp $
 *
 ****************************************************************************/


/********************************* includes *********************************/
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/mib_api.h>
#include <net-snmp/net-snmp-includes.h>
#include "NASCenter_JS_Linux.h"
#include "NASCenter_Errors.h"
#include "NASCenter_Scheduler.h"
#include "jobProcessMasterTable.h"
#include "JobProcessDetailTable.h"
#include "NASCenter_FileTrans_my.h"

#define MAX_REPETITIONS 6

/****************************** local macros *********************************
*
* (Macros shareable with rest of the modules are present in http_sample.h file)
*/

/********************* local structure/union definitions ********************/

/********************* prototypes for local functions ***********************/
void JS_StartJobSchedProcess( ) ;
void JS_InitCBuffer(SCBuffer *);
void JS_ProcessTrapMsg(void *);
void JS_ParseTrap( struct snmp_pdu *);
void JS_Log(UI8 *logMsg);
I16 JS_FillStruct(UI8  **,  netsnmp_variable_list *);
I16 JS_GetJobProcessMasterTableDetails();
void JS_FreeJobMaster( stJobMaster *);
I16 JS_GetAgentInfoMIB();
I16 JS_UpdateJobToScheduler(stJobMaster  *, I16 );
int JS_ListenTrap(int id, struct snmp_session *sess, int req, netsnmp_pdu *pdu, void *arg);
I16 JS_CheckTable(netsnmp_variable_list *);
I16 JS_CheckField( netsnmp_variable_list *);
I16 JS_AddJobToSched(stJobMaster  *);
I16 JS_ModifyJobToSched(stJobMaster  *);
I16 JS_DeteteFromSched(stJobMaster  *);
I16 JS_SpawnJobSched(void *);
BOOL  JS_IsQueueEmpty(SCBuffer *);
BOOL  JS_IsQueue_Full(SCBuffer *);
void JS_InitSession();
I16 JS_GetIndex(netsnmp_variable_list *pstCurrVars);
void JS_Read_Packets(void *);
I16  JS_FillTimeStruct(struct tm *, char *) ; 	    
void JS_DoJobSched(stJobMasterSched *, stJobMaster *);
void JS_PerformJobSchedAction(void *);
UI32 JS_GetCurrentTime ();
I16 BM_GetDay(time_t );
I16 JS_CheckScriptAvail(UI8 *);
I16 JS_GetCPUUtilization(stJobMaster **, stJobMasterSched *);
I16 JS_DoSNMPSet(char *, I16 , I16 );
I16 BM_GetDayNum(UI8 *pcDay);
I16 JS_ScheduleLogInfo();
void JS_CallJobLogFile();
void JS_SendLogFile();
void JS_FreeJob(stJobMasterSched  *);



/************************ globals within this module*************************/

SCBuffer stCB;
stList stListPtr;
struct snmp_session *agentSess;
void *tSess;
#ifdef JS_DEBUG
FILE *ifdForJS;
#endif
char charBuff[C_MAX_SIZE];   
FILE *fpForJS;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

/*Agent information  */
I16 JobSchedServerPort;
UI8 JobSchedServerIP[16];
UI8 JobSchedLogFileName[80];
long lTotalSize;


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
void JS_StartJobSchedProcess() 
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
#ifdef JS_DEBUG	   
       JS_Log("Error in Retrieving agent Info  table records \n");	
#endif       
       return;
   }
   

   /*Open the file decriptor for log file*/
   fpForJS = fopen(JobSchedLogFileName , "a+");
   if(fpForJS == NULL)
        return;	   
 
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
   pthread_create(&readThread , NULL , (void *) JS_ProcessTrapMsg ,
                 NULL);
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

   /* For Session Initiation */
   snmp_sess_init(&tempSess);
   tempSess.version = SNMP_VERSION_2c;
   tempSess.peername = gcAgentSessPeerName;    

#ifdef LINUX_JS
   tempSess.peername = "192.168.165.69:2310";
#endif
#ifdef SOLARIS_JS
   tempSess.peername = "192.168.165.49:2311";
#endif         
#ifdef UNIXWARE_JS
   tempSess.peername = "192.168.165.98:2312";
#endif      
#ifdef AIX_JS
   tempSess.peername = "192.168.165.59:2314";
#endif  
#ifdef HP_UX_JS
      tempSess.peername = "192.168.165.56:2315"; 
#ednif       

   tempSess.community = gcNASSACommunity;
   tempSess.community_len = strlen(gcNASSACommunity);
   tempSess.callback_magic = NULL;

   snmp_sess_init(&trapSess);
   trapSess.version = SNMP_VERSION_2c;
   trapSess.peername = "localhost:2001";
   trapSess.local_port = 2001;
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
    struct snmp_pdu *pstrespPDU;
    netsnmp_variable_list *pstCurrVars;

    I16 status=0;
    I16 iLen=0;
    memset(JobSchedLogFileName,0,80);
               
       pstreqPDU = snmp_pdu_create(SNMP_MSG_GET);
         
      /*Get the Relevant Fields*/
       /*Get the Log file name */
       iLen=sizeof(agentInfoLogFileJobProcess_variables_oid)/sizeof(oid); 
       snmp_add_null_var(pstreqPDU,agentInfoLogFileJobProcess_variables_oid,iLen);
      
      /*Get the IP Address */ 
       iLen=sizeof(agentInfoLogFileTransferIP_variables_oid)/sizeof(oid); 
       snmp_add_null_var(pstreqPDU,agentInfoLogFileTransferIP_variables_oid,iLen);

       /*Get the port number*/
       iLen=sizeof(agentInfoLogFilePortNo_variables_oid)/sizeof(oid); 
       snmp_add_null_var(pstreqPDU,agentInfoLogFilePortNo_variables_oid,iLen);

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
    struct snmp_pdu *pstrespPDU;
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
              pstJobMasterTable->iJobMasIndex = pstCurrVars->name[iLen];
              
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
              if(JS_FillStruct((&pstJobMasterTable->pcJobMasStartTime),pstCurrVars)==0)
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
              if(JS_FillStruct((&pstJobMasterTable->pcJobMasFinishTime),pstCurrVars)==0)
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
    
	      JS_UpdateJobToScheduler(pstJobMasterTable, JS_ADD);
             
	         /*Move the Pointer to the next location*/  
              pstCurrVars = pstCurrVars->next_variable;
	      
              
       }//for iProcessCount
       
       if(iCheckTable == FALSE)
       {
            break; 		   
       }		   
     } // end of while(1)
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
	    return;
       }
       if(pstJSMaster->pcJobMasName!= NULL )
       { 
            free(pstJSMaster->pcJobMasName); 
	    pstJSMaster->pcJobMasName = NULL;
	    return;
       }
       if(pstJSMaster->pcJobMasStartTime!= NULL )
       { 
            free(pstJSMaster->pcJobMasStartTime); 
	    pstJSMaster->pcJobMasStartTime = NULL;
	    return;
       }
       if(pstJSMaster->pcJobMasFinishTime!= NULL )
       { 
            free(pstJSMaster->pcJobMasFinishTime); 
	    pstJSMaster->pcJobMasFinishTime = NULL;
	    return;
       }
       free(pstJSMaster);
}

/*****************************************************************************
 *
 * name             :   JS_FillStruct 
 * description      :   Aloocates memorry for length and copy the value
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
 * 		        JS_DeteteFromSched
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
             iResult = JS_DeteteFromSched(pstJSStruct); 		
	     if(!iResult)
             {
#ifdef JS_DEBUG	   
                  JS_Log("Error : Error occured while deleting the task \n");		     
#endif		  
             }
             break;		
    }	    
    return 1;
}

/*****************************************************************************
 * name             :   JS_ScheduleLogInfo
 * description      :   Add the Lof file transfer to the schedule 
 * input parameters :   
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :   - 
 * calls	    :   
 * 		        SCH_AddTask
*****************************************************************************/
I16 JS_ScheduleLogInfo()	   
{

    stTask task; 
    UI32 ui32TaskId;
    struct tm stTm;
    struct timeval stTv;

#ifdef JS_DEBUG	   
    JS_Log("JS_ScheduleLogInfo... Entering \n");
#endif    
    gettimeofday(&stTv, (struct timezone *) 0);
    memcpy(&stTm, localtime((time_t *) & stTv.tv_sec), sizeof(stTm));
    

    /*Fill the task structure for current date and time*/
    task.StartTime.ui16Hour   =   stTm.tm_hour ;
    task.StartTime.ui16Minute =   stTm.tm_min ;
    task.StartTime.ui16Second =   stTm.tm_sec;
    task.StartDate.ui16Day    =   stTm.tm_mday;
    task.StartDate.ui16Month  =   stTm.tm_mon;
    task.StartDate.ui16Year   =   stTm.tm_year; 


    /*Here the task should be periodic*/
    task.eType  = SCH_PERIODIC; 
    task.ePCity = SCH_SEC_MIN_HOUR;
    task.ui32Interval = INTERVAL;
    task.fp = JS_CallJobLogFile;

    /*Add to the scheduler */
    ui32TaskId = SCH_AddTask(task);           	
    if(ui32TaskId ==0)
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
void JS_CallJobLogFile()
{
     pthread_t logThread;	
     pthread_create(&logThread,NULL,(void*)JS_SendLogFile,NULL);
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
      char buff[15];
     
#ifdef JS_DEBUG	   
      JS_Log("SendLogFile function .. Entering\n "); 
#endif      
      strcat(charBuff,"This is the sample log file transferred to server \n");
      sock_addr.sin_family = AF_INET;

      pthread_mutex_lock(&log_mutex);	     
      fprintf(fpForJS, "%s",charBuff);       	  
      fclose(fpForJS);

      /*Send the log file to the manager */
      sock_addr.sin_port = htons(JobSchedServerPort);
      sprintf(buff,"%d.%d.%d.%d",(0xff& JobSchedServerIP[0]),(0xff & 
      JobSchedServerIP[1]), (0xff & JobSchedServerIP[2]),
          (0xff& JobSchedServerIP[3]));   
      sock_addr.sin_addr.s_addr =inet_addr(buff);
      //inet_pton(AF_INET , buff, &sock_addr.sin_addr);
      pthread_mutex_unlock(&log_mutex);	     
      
#ifdef JS_DEBUG	   
      JS_Log("Calling FileTrans_SendFile function \n");
#endif      
      iError = FileTrans_SendFile (JobSchedLogFileName, &sock_addr
          ,JS_DEST_PATH, 512);             
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
              //pthread_exit(NULL);
              return;
      }
      /*Open the file decriptor for log file*/
      fpForJS = fopen(JobSchedLogFileName , "w");
      if(fpForJS == NULL)
      {	      
             //pthread_exit(NULL);
             return;	   
      }
#ifdef JS_DEBUG	   
      JS_Log("SendLogFile function .. Returning\n "); 
#endif      
      //pthread_exit(NULL);
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

    UI32 ui32TaskId;
    UI32 ulTime;
    struct tm pstTmStart;
    struct tm pstTmEnd;
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
	 iRetCount= BM_GetDay(ulTime);
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
    ui32TaskId = SCH_AddTask(task);           	
#ifdef JS_DEBUG	   
    JS_Log("Task has been added \n ");
#endif    
    //JS_Log("Task ID %ld \n",ui32TaskId);
    if(ui32TaskId ==0)
    {
#ifdef JS_DEBUG	   
         JS_Log("Error occured while adding the task to scheduler \n ");	    
#endif	 
	 return NASC_FAILURE;
    }
#ifdef JS_DEBUG	   
    JS_Log("Adding Task to scheduler \n ");
#endif    
    JS_AddNodeInJobList(ui32TaskId ,pstJSStruct->iJobMasIndex, stListPtr);
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
I16  JS_FillTimeStruct(struct tm *pstTm, char *pstTime)  	    
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

    iTime = mktime(pstTm);

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
    UI32 ui32TaskId;
    UI32 oldTaskID;
    UI32 newTaskID;
    UI32 ulTime;
    struct tm pstTmStart;
    struct tm pstTmEnd;
    stTask *pstModTask;
    stJobMaster  *pstJSMod;
    I16 iDayCount;
    I16 iResult;

    /*Get the task ID from the list*/
    ui32TaskId = JS_GetTaskID(pstJSStruct->iJobMasIndex, stListPtr);
    if(ui32TaskId == 0)
    {
#ifdef JS_DEBUG	   
          JS_Log("No task ID found in the List \n ");		
#endif	  
	  return NASC_FAILURE;
    }
    /*Allocate memory for pstModTask struct*/
    pstModTask= malloc(sizeof(stTask));

    /*Get the task structure */
    iResult = SCH_GetTask(ui32TaskId, &pstModTask);
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
	 iDayCount= BM_GetDay(ulTime);
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

 	 pstJSMod->pcJobMasId = malloc(strlen(pstJSStruct->pcJobMasId));
	 strcpy(pstJSMod->pcJobMasId,pstJSStruct->pcJobMasId);
    }

    /*Modify the Master name*/
    if(pstJSStruct->pcJobMasName!= NULL)
    {
	 free(pstJSMod->pcJobMasName);
 	 pstJSMod->pcJobMasName = malloc(strlen(pstJSStruct->pcJobMasName));
	 strcpy(pstJSMod->pcJobMasName,pstJSStruct->pcJobMasName);
    }
     
    	    
    pstModTask->vpArg = (void*)pstJSMod;
	 
    pstModTask->fp = JS_PerformJobSchedAction;
    /*Add to the scheduler */
    newTaskID =0;
    oldTaskID = ui32TaskId;
    newTaskID = SCH_ModifyTask(ui32TaskId, *pstModTask);           	
    //JS_Log("JS_ModifyJobToSched: Modified Task Id %ld \n",newTaskID );


    if(ui32TaskId ==0)
    {
#ifdef JS_DEBUG	   
         JS_Log("Error occured while adding the task to scheduler \n ");	    
#endif	 
	 return NASC_FAILURE;
    }

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
          JS_Log("Node deleted from the list \n");   		 
#endif	  
    }
  
    /*Add the returned node in the List*/  	    
    JS_AddNodeInJobList(ui32TaskId,pstJSStruct->iJobMasIndex ,stListPtr);
    return NASC_SUCCESS;
}

/*****************************************************************************
 * name             :   JS_DeteteFromSched
 * description      :   Delete from the scheduler  
 * input parameters :   pstJSStruct -> pointer to the stJobMaster table
 * return type      :   NASC_SUCCESS / NASC_FAILURE  
 * global variables :   - 
 * calls	    :   JS_GetTaskID
 *                      JS_DeleteNodeFromList
 *                      SCH_DeleteTask
*****************************************************************************/
I16 JS_DeteteFromSched(stJobMaster  *pstJSStruct)
{
    UI32 ui32DelTaskId;
    UI16 iResult;
    if(pstJSStruct == NULL)
    {
#ifdef JS_DEBUG	   
         JS_Log("No record found \n");	    
#endif	 
         return NASC_FAILURE;   				
    }
    /*Get the task ID*/ 	    
    ui32DelTaskId = JS_GetTaskID(pstJSStruct->iJobMasIndex,stListPtr );    
    if(ui32DelTaskId == -1)
    {
	 return NASC_FAILURE;
    }
    iResult = SCH_DeleteTask(ui32DelTaskId);
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
    }
    iResult = JS_DeleteNodeFromList(stListPtr,pstJSStruct->iJobMasIndex ,ui32DelTaskId);
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
          return NASC_SUCCESS;
    }
    return NASC_SUCCESS;
}

/*****************************************************************************
 * name             :   BM_GetDay 
 * description      :   Get the Day (Sun... Sat) of the week 
 * input parameters :   iTime -> struct variable for time structure
 * return type      :   NASC_SUCCESS / NASC_FAILURE  
 * global variables :   - 
 * calls	    :   BM_GetDayNum 
*****************************************************************************/
I16 BM_GetDay(time_t iTime )
{
       UI8 cDay[4];
       char cTimeBuff[100];
       UI8 cOthers[30];

       sprintf(cTimeBuff,"%s",ctime(&iTime));
       //JS_Log("Time is %s \n",cTimeBuff);
       sscanf(cTimeBuff,"%s %s",cDay,cOthers );
       //JS_Log("Day is %s \n",cDay);

       return BM_GetDayNum(cDay);
}

/*****************************************************************************
 * name             :   BM_GetDayNum
 * description      :   Get the Day (0..6) (Sun... Sat) of the week 
 * input parameters :   pcDay -> pointer to string "sun,Mon,...Sat" 
 * return type      :   Returns 0..6 numbers respect to Sun...Sat 
 * global variables :   - 
 * calls	    :   NASC_FAILURE /  day count
*****************************************************************************/
I16 BM_GetDayNum(UI8 *pcDay)
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
    pstJSMaster->iJobMasStatus = JS_MODIFY;
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
                 iDecisionFlag = JS_ADD;
                 break;
          case 6:
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
       iIndex = pstCurrVars->name[iLen];
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
     pthread_create(&jobThread,NULL,(void*)JS_SpawnJobSched,args);
    
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
     if((pstJobMaster->uiEndTimeinNum < uiCurrentTime)  
            && (pstJobMaster->iJobMasInterval != 0 ))
     {
#ifdef JS_DEBUG	   
          JS_Log("Current Time is exceeded the end time \n");	     
#endif	  
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

    while(1) 
    {
       reqPDU = snmp_pdu_create(SNMP_MSG_GETBULK);
       reqPDU->non_repeaters = 0;
       /*Set the number of records to be fetched */
       reqPDU->max_repetitions = MAX_REPETITIONS;
       
       /*Allocate memory for stJobMasterSched structure */
       pstjob = malloc(sizeof(stJobMasterSched));
       if(pstjob == NULL)
       {
             return NASC_FAILURE;	       
       }
       pstjob->pcMasterId=  NULL;
       pstjob->pcMasterScriptFile =  NULL;

       /*Form the Job master ID */	    
       iLen =0; 
       snmp_add_null_var(reqPDU,pstDet1,iDetLen1);
       snmp_add_null_var(reqPDU,pstDet2,iDetLen2);
      
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
                  JS_FreeJob(pstjob); 		   
#ifdef JS_DEBUG	   
	          JS_Log("sync response failure \n");	   
#endif		  
                  return NASC_FAILURE;		   
	   }
           if(respPDU == NULL)	        
	   {
                JS_FreeJob(pstjob); 		   
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

       
       vars = respPDU->variables;
       pstCurrVars = vars;

       /*Check the fetched record is belong to back up master table */
       if(netsnmp_oid_is_subtree(tableID,
          tableLen, pstCurrVars->name, pstCurrVars->name_length))
       { 
#ifdef JS_DEBUG	   
             JS_Log("Error : No record found for this table \n");
#endif	     
             JS_FreeJob(pstjob); 		   
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
          
	      
	      /*Get the Id */
	      if((JS_FillStruct((&pstjob->pcMasterId), pstCurrVars)) == 
                   NASC_INSUFFICIENT_MEMORY)
              {
                   JS_FreeJob(pstjob); 		   
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
                   return NASC_FAILURE;		      
              }
              
	       
              pstDet2  = pstCurrVars->name;
              iDetLen2 = pstCurrVars->name_length;
	      /*Move the Pointer to the next location*/  
              pstCurrVars = pstCurrVars->next_variable;

              JS_DoJobSched(pstjob,pstJobMaster);
		
       }
       if(iCheckTable == FALSE)
       {
           break; 		   
       }		   
    }
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
       struct tm pstTmStart;
      UI32 ulTime;
      UI8 cStartDate[60];
      UI8 cEndDate[60];
      UI8 *cMastName; 

     /*Check the script file is available */	
      iResult = JS_CheckScriptAvail(pstjob->pcMasterScriptFile);  	
      if(iResult == NASC_FAILURE)
      {
             JS_FreeJob(pstjob); 		   
#ifdef JS_DEBUG	   
             JS_Log("No script is available in specified path \n");	      
#endif	     
	     return; 
      }

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
         (pstTmStart.tm_year+1900), pstTmStart.tm_mon, pstTmStart.tm_mday,
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
         (pstTmStart.tm_year+1900), pstTmStart.tm_mon, pstTmStart.tm_mday,
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
            strcpy(cMastName,"None");
      }else
      {
            cMastName = pstJobMaster->pcJobMasName;
      }
        
      sprintf(localBuff,"%s %s %d %s %s %d %d \n",cStartDate,
      cEndDate,pstJobMaster->iJobMasIndex, pstJobMaster->pcJobMasId,
      cMastName,pstJobMaster->iJobMasCPUUsage,pstJobMaster->iJobMasStatus);


      strcat(localBuff,"Sample file");        
    
      strcat(charBuff,localBuff);
      if(strlen(charBuff) > (C_MAX_SIZE -200))
      {
           pthread_mutex_lock(&log_mutex);	     
           fprintf(fpForJS, "%s",charBuff);       	  
	   memset(charBuff, 0,C_MAX_SIZE); 
	   fclose(fpForJS);
	   /* Transfer the file to FTP server */
           sock_addr.sin_family = AF_INET;
           sock_addr.sin_port = htons(JobSchedServerPort);
           sprintf(buff,"%d.%d.%d.%d",(0xff& JobSchedServerIP[0]),(0xff & 
           JobSchedServerIP[1]), (0xff & JobSchedServerIP[2]),
           (0xff& JobSchedServerIP[3]));
           //inet_pton(AF_INET , buff, &sock_addr.sin_addr);
           sock_addr.sin_addr.s_addr =inet_addr(buff);
           iError = FileTrans_SendFile (JobSchedLogFileName, &sock_addr
                 ,JS_DEST_PATH, 512);             
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
               return;	   
           pthread_mutex_unlock(&log_mutex);	     
      }
       pstJobMaster->iJobMasMemoryUsage = (int)((pstJobMaster->iJobMasMemoryUsage / lTotalSize) * 100);
       //JS_Log("Memory Utilization %d \n ",pstJobMaster->iJobMasMemoryUsage );
      /*Form the OID with the index */ 
      JS_DoSNMPSet(CPUUsage,
            pstJobMaster->iJobMasCPUUsage,pstJobMaster->iJobMasIndex);
      JS_DoSNMPSet(MemUsage,
            pstJobMaster->iJobMasMemoryUsage,pstJobMaster->iJobMasIndex);
      /*Form SNMP Packet to set the values in MIB*/ 

      /*Check the end Time is crossed the current time */
      
 

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
	
	init_snmp ("snmprocess2");

	memset(cOIDForSet,0,MAX_OID_LEN);
	sprintf(cOIDForSet,"%s.%d",OID,iIndex);

	iLen = strlen(cOIDForSet);
        pcOIDToSet= malloc(iLen+1);
        if(pcOIDToSet == NULL)
             return NASC_FAILURE;
        memset(pcOIDToSet,0,(iLen+1));
        memcpy(pcOIDToSet,cOIDForSet,iLen);

	
	/*if (!read_objid(pcOIDToSet, name, &name_len)) {
		JS_Log("net_snmp_set: %s: %s",
			 pcOIDToSet, snmp_api_errstring(snmp_errno));
		return NASC_FAILURE;
	}*/
	if (snmp_parse_oid(pcOIDToSet, name, &name_len) == NULL) {
#ifdef JS_DEBUG	   
		JS_Log("Error in setting OID \n");
#endif		
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
I16 JS_GetCPUUtilization(stJobMaster **pstJobMaster , stJobMasterSched *pstjob)
{
 /*Do the Job Processing*/
      char *argv[4];
      struct snmp_pdu *pstreqPDU;
      struct snmp_pdu *pstrespPDU;
      pid_t pid;
      netsnmp_variable_list *pstCurrVars;
      char *environ[1];
      char oidCPUPercent[] = ".1.3.6.1.2.1.25.5.1.1.1";
      char oidMemUtil[] = ".1.3.6.1.2.1.25.5.1.1.2";
      char oidCPU[30];
      char oidMem[30];
      I16 iStatus;
      I16 iLen=0;
      char *pcCPUPer;
      char *pcMemPer;
      oid name[MAX_OID_LEN];
      size_t name_len = MAX_OID_LEN;
      int *status;
      argv[0] = "sh";
      argv[1] = "-c";


      init_snmp ("snmprocess1");
      if (pstjob->pcMasterScriptFile == 0)
          return NASC_FAILURE;
      /*Get the PID */
      pid = fork();
      //JS_Log("PID value %d \n", pid);
      
      if (pid == -1)
         return NASC_FAILURE;
      if (pid == 0) {
             argv[2] = pstjob->pcMasterScriptFile;
             argv[3] = 0;
             execve("/bin/sh", argv, environ);
             exit(127);
      }
      do {
            memset(oidCPU,0,30);	      
            memset(oidMem,0,30);	      
#ifdef LINUX_JS | AIX_JS | HP_UX_JS
            /*Get the cpu and memory utilization */	     
            sprintf(oidCPU, "%s.%d",oidCPUPercent,pid);	      
            sprintf(oidMem, "%s.%d",oidMemUtil,pid);	      
#endif

#ifdef SOLARIS_JS | UNIXWARE_JS
            sprintf(oidCPU, "%s.%ld",oidCPUPercent,pid);	      
            sprintf(oidMem, "%s.%ld",oidMemUtil,pid);
#endif

	    iLen = strlen(oidCPU);
	    pcCPUPer = malloc(iLen+1);
	    if(pcCPUPer == NULL)
		    return NASC_FAILURE;
	    memset(pcCPUPer,0,(iLen+1));
	    memcpy(pcCPUPer,oidCPU,iLen);

	    iLen = strlen(oidMem);
	    pcMemPer = malloc(iLen+1);
	    if(pcMemPer == NULL)
		    return NASC_FAILURE;
	    memset(pcMemPer,0,(iLen+1));
	    memcpy(pcMemPer,oidMem,iLen);

	    /*Added for CPU Utilization */
	    if (!read_objid(pcCPUPer, name, &name_len)) {
	    	/*JS_Log("net_snmp_set: %s: %s",
	    		 oidCPU, snmp_api_errstring(snmp_errno));*/
	    	return NASC_FAILURE;
            }
            
	    
	    pstreqPDU = snmp_pdu_create(SNMP_MSG_GET);
            pstreqPDU->non_repeaters = 0;
            
            snmp_add_null_var(pstreqPDU,name,name_len);

	    name_len = MAX_OID_LEN;

	    /*Added for CPU Utilization */
	    if (!read_objid(pcMemPer, name, &name_len)) {
	    	/*JS_Log("net_snmp_set: %s: %s",
	    		 oidMem, snmp_api_errstring(snmp_errno));*/
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
                      snmp_perror("Error \n ");		    
                      return NASC_FAILURE;		   
                }

                if(pstrespPDU == NULL)	        
	        {
                    snmp_perror("Error ");   	   
                    return NASC_FAILURE;
                }
           } else 
           {
           snmp_perror("Error ");   	   
               return NASC_FAILURE;		   
           }


          /*Get the CPU utilization */
	    
          pstCurrVars = pstrespPDU->variables;

          if(pstCurrVars != NULL)
          (*pstJobMaster)->iJobMasCPUUsage  = (I16) *(pstCurrVars->val.integer);
          pstCurrVars = pstCurrVars->next_variable;

	  //JS_Log("CPU Utilization %d \n",(*pstJobMaster)->iJobMasCPUUsage  );

          /*Get the Memory utilization */
          if(pstCurrVars != NULL)
           (*pstJobMaster)->iJobMasMemoryUsage = (I16) *(pstCurrVars->val.integer);
	  
	  //JS_Log("Memory Utilization %d \n",(*pstJobMaster)->iJobMasMemoryUsage);
	    
          if (waitpid(pid, status, 0) == -1) {
          if (errno != EINTR)
              return NASC_FAILURE;
            } else 
          return NASC_SUCCESS;
       } while(1);

      return NASC_SUCCESS;
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
    unsigned long ulValue=0;
    struct tm stTm;
    struct timeval stTv;

    gettimeofday(&stTv, (struct timezone *) 0);
    memcpy(&stTm, localtime((time_t *) & stTv.tv_sec), sizeof(stTm));
  
    ulValue = stTm.tm_hour * 60 * 60 + stTm.tm_min * 60 + stTm.tm_sec;
    return ulValue;
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
   ifdForJS= fopen("JS_LOG_FILE","a+");
   if((ifdForJS == NULL))
   {
       JS_Log("Error in opening the file \n");		   
       return;
   }
   fprintf(ifdForJS,"%s\n",logMsg );	
   fclose(ifdForJS);
#ifdef JS_PRINT
   printf("%s",logMsg);	
#endif   
#endif       
       	
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
int main()
{
    pthread_t thID;
    long lTotPages; 

#ifdef LINUX_JS | SOLARIS_JS | AIX_JS  
    long lPageSize;                  
#ebdif

    stListPtr = JS_MakeEmpty(NULL);

#ifdef HP_UX_JS        
    struct pst_static stat_buf;        
#ebdif

#ifdef LINUX_JS | SOLARIS_JS | AIX_JS
    /*Get the Total emory*/
    lPageSize = sysconf(_SC_PAGE_SIZE);
    lTotPages = sysconf(_SC_PHYS_PAGES);

    lTotalSize = (long)((lPageSize*lTotPages) /1024); 
    //JS_Log("Memory %ld\n",lTotalSize );
#endif

#ifdef UNIXWARE_JS
      lTotalSize = sysconf(_SC_TOTAL_MEMORY);
#endif    

#ifdef HP_UX_JS
    pstat_getstatic(&stat_buf, sizeof(struct pst_static), 1, 0);
    printf("Physical memory = %ld bytes \n",stat_buf.physical_memory*4096); 
    lTotalSize = stat_buf.physical_memory*4096; 
#endif                                         

    memset(charBuff,0,1000000); 
    /*Open the session with the NASCenter sub agent*/	
    JS_InitSession();
    pthread_create(&thID,NULL,(void*)JS_Read_Packets,NULL);
    JS_StartJobSchedProcess();
    pthread_join(thID, NULL);
    return 0;
}

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
             if(snmpFds == EBADF)
	     {
#ifdef JS_DEBUG	   
                   JS_Log("An invalid file descriptor \n ");
#endif		   
	     }else if(snmpFds == EINTR)
	     {
#ifdef JS_DEBUG	   
                 JS_Log("A non blocked signal was caught \n");		     
#endif		 
	     }else if(snmpFds == ENOMEM)		     
	     {
#ifdef JS_DEBUG	   
                 JS_Log("Select was unable to allocate memory non \n");		     
#endif		 
	     }
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

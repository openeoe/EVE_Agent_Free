/*****************************************************************************
          ieturn NASC_SUCCESS;
 *
 * NASCenter_BM_Linux.c
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Thiagarajan V.
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
 * $Log: NASCenter_BM_Linux.c,v $
 * Revision 1.1  2003/12/02 11:19:42  thiagav
 * Added
 *
 * Revision 1.8  2003/12/02 11:18:04  thiagav
 * Modified
 *
 * Revision 1.7  2003/11/29 14:29:07  guga
 * Added after unit testing
 *
 * Revision 1.5  2003/11/21 12:58:30  thiagav
 * Added after basic testing
 *
 * Revision 1.4  2003/11/17 06:08:55  thiagav
 * Added in to CVS after basic testing
 *
 *
 * $Id: NASCenter_BM_Linux.c,v 1.1 2003/12/02 11:19:42 thiagav Exp $
 *
 ****************************************************************************/


/********************************* includes *********************************/
#define REENTRANT
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include "NASCenter_BM_Linux.h"
#include "NASCenter_Scheduler.h"
#include "NASCenter_Errors.h"
#include "backupMasterTable.h"
#include "backupDetailTable.h"
#include "NASCenter_FileTrans_my.h"

#define MAX_REPETITIONS 6

/****************************** local macros *********************************
*
* (Macros shareable with rest of the modules are present in http_sample.h file)
*/

/********************* local structure/union definitions ********************/

/********************* prototypes for local functions ***********************/
void BM_StartBackupProcess( ) ;
void BM_InitCBuffer(SCBuffer *);
void BM_ProcessTrapMsg(void *);
void BM_ParseTrap( struct snmp_pdu *);
#ifdef BM_DEBUG 
extern void BM_Log(UI8 *logMsg);
#endif

I16 BM_FillStruct(UI8  **,  netsnmp_variable_list *);
I16 BM_GetBackupMaster();
I16 BM_GetAgentInfoMIB();
I16 BM_GetDay(time_t iTime );
I16 BM_GetDayNum(UI8 *pcDay);
I16 BM_UpdateBackupToScheduler(stBackupMaster  *, I16 );
int BM_ListenTrap(int id, struct snmp_session *sess, int req, netsnmp_pdu *pdu, void *arg);
I16 BM_CheckTable(netsnmp_variable_list *);
I16 BM_CheckField( netsnmp_variable_list *);
I16 BM_AddBackupToSched(stBackupMaster  *);
I16 BM_ModifyBackupToSched(stBackupMaster  *);
I16 BM_DeteteFromSched(stBackupMaster  *);
I16 BM_SpawnBackup(void *);
BOOL  BM_IsQueueEmpty(SCBuffer *);
BOOL  BM_IsQueue_Full(SCBuffer *);
void BM_InitSession();
I16 BM_FillBackupStruct( stBackupStruct *, stBackupMaster *);
I16 BM_GetIndex(netsnmp_variable_list *pstCurrVars);
void BM_Read_Packets(void *);
unsigned long BM_FillTimeStruct(struct tm *, char *) ; 	    
void BM_PerformBackupAction(void *);
void BM_DoBackup(void *);
UI32 BM_GetCurrentTime ();
I16 BM_ScheduleLogInfo();
void BM_FreeBackupMaster(    stBackupMaster *pstBMMaster);
void BM_CallBackLogFile();
void BM_SendLogFile();
void BM_Read_Packets_Info(void *args);



/************************ globals within this module*************************/

SCBuffer stCB;
stList stListPtr;
struct snmp_session *agentSess;
void *tSess;
#ifdef BM_DEBUG
FILE *ifdForBM;
#endif
char charBuff[C_MAX_SIZE];   
FILE *fpForBM;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

/*Agent information for log file transformation */
I16 backupServerPort;
UI8 backupServerIP[16];
UI8 backupLogFileName[80];


/*****************************************************************************
 *
 * name             :   BM_StartBackupProcess 
 * description      :   Starting function of User Meetering application 
 *                      This function retrieves the table values  
 *                      Creates a thread for processing the trap message 
 * input parameters :   - 
 * return type      :   - 
 * global variables :   ifdForBM -> File Descriptor for logging the debug log
 *                      information 
 * calls            :   BM_GetBackupMaster()
 *                  :   BM_ProcessTrapMsg()
*****************************************************************************/
void BM_StartBackupProcess() 
{
   I16 iResult; 	
   pthread_t readThread;
   
   
#ifdef BM_DEBUG
   BM_Log("Before Starting the scheduler \n");
#endif

   /*Start the scheduler */
   SCH_StartScheduler();

#ifdef BM_DEBUG
   BM_Log("Scheduler started \n");
#endif
     
   iResult=0;
   /*Get Agent Info Table details for loggin the log information */
   iResult = BM_GetAgentInfoMIB();

   if(iResult == NASC_FAILURE )
   {
#ifdef BM_DEBUG
       BM_Log("Error in Retrieving agent Info  table records \n");	
#endif
       return;
   }
   
   /*Open the file decriptor for log file*/
   fpForBM = fopen(backupLogFileName , "a+");
   if(fpForBM == NULL)
        return;	   
  
   iResult=0;
   /*Assign the task to scheduler */
   iResult = BM_ScheduleLogInfo();  
   if(iResult == NASC_FAILURE)
   {
#ifdef BM_DEBUG
         BM_Log("Error in adding log file transfer activity to scheduler \n");	   
#endif
   }
   
   iResult=0;
   /*Get Backup Master table details */
   iResult = BM_GetBackupMaster();
   if(iResult == NASC_FAILURE)
   {
#ifdef BM_DEBUG
       //BM_Log("Error in Retrieving Backup Master table records \n");	
#endif
   }
    
   /*Initialize the circular queue*/
   BM_InitCBuffer(&stCB) ;
   /*Create a thread for processing the trap message  */
   if((pthread_create(&readThread , NULL , (void *) BM_ProcessTrapMsg ,
                 NULL)) !=0)
   {
#ifdef BM_DEBUG
         BM_Log("Thread can not be created \n");	   
#endif
	 return ;
   }
}


/*****************************************************************************
 *
 * name             :   BM_InitSession 
 * description      :   Initialize the SNMP Session with NASCenter sub agent 
 * input parameters :   - 
 * return type      :   - struct snmp_session * - Poniter to the snmp session 
 * global variables :   -gcAgentSessPeerName  - peer name 
 *                      - giNASSAPort - port number used for opening the snmp
 *                        session
 *                      - gcNASSACommunity - Community name for snmp session
 *                        
 * calls	    :   -BM_ListenTrap  
 *                      -BM_AddMsgToLogFile
 *
*****************************************************************************/
void BM_InitSession()
{
   struct snmp_session tempSess;
   struct snmp_session trapSess;

   /* For Session Initiation */
   snmp_sess_init(&tempSess);
   tempSess.version = SNMP_VERSION_2c;
   tempSess.peername = "192.168.165.69:2120";
   //tempSess.local_port = 200;
   /*tempSess.remote_port = giNASSAPort;*/
   tempSess.community = gcNASSACommunity;
   tempSess.community_len = strlen(gcNASSACommunity);
   tempSess.callback_magic = NULL;


   snmp_sess_init(&trapSess);
   trapSess.version = SNMP_VERSION_2c;
   trapSess.peername = "localhost:2100";
   trapSess.local_port = 2100;
   trapSess.community = gcNASSACommunity;
   trapSess.community_len = strlen(gcNASSACommunity);
   trapSess.callback = BM_ListenTrap;
   trapSess.callback_magic = NULL;
 

   SOCK_STARTUP;
   agentSess = snmp_open(&tempSess);
   SOCK_STARTUP;
   tSess = snmp_sess_open(&trapSess);

   if(agentSess == NULL) 
   {
#ifdef BM_DEBUG
       BM_Log("Error opening Session\n");
#endif
       snmp_perror("NASCenterJS Snmp Open: ");
       exit(1);
   }
}

I16 BM_GetAgentInfoMIB()
{
    struct snmp_pdu *pstreqPDU;
    struct snmp_pdu *pstrespPDU;
    netsnmp_variable_list *pstCurrVars;

    I16 status=0;
    I16 iLen=0;
               
    pstreqPDU = snmp_pdu_create(SNMP_MSG_GET);
       
    /*Get the Relevant Fields*/
    
       /*Get the Log file name */
       iLen=sizeof(agentInfoLogFileBackup_variables_oid)/sizeof(oid); 
       snmp_add_null_var(pstreqPDU,agentInfoLogFileBackup_variables_oid,iLen);
      
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
#ifdef BM_DEBUG
	          BM_Log("sync response success \n");	   
#endif
	   }else
	   {
#ifdef BM_DEBUG
	          BM_Log("sync response failure \n");	   
#endif
                  return NASC_FAILURE;		   
	   }
           if(pstrespPDU == NULL)	        
	   {
#ifdef BM_DEBUG
                BM_Log("Error in sync response \n");		   
#endif
		return NASC_FAILURE;
	   }
       } else 
       {
#ifdef BM_DEBUG
           BM_Log("Error in snmp_synch_response \n");
#endif
           snmp_perror("Error ");   	   
               return NASC_FAILURE;		   
       }
       pstCurrVars = pstrespPDU->variables;
       /*Get the backup Log file */
       iLen =0;
       iLen =  pstCurrVars->val_len;

       if(pstCurrVars != NULL)
       {
            if(pstCurrVars->val.string != NULL)  	       
	    {
                 /*Copy the log file name*/ 		    
                 pthread_mutex_lock(&log_mutex);	     
                 memcpy(backupLogFileName,pstCurrVars->val.string,iLen);       
                 pthread_mutex_unlock(&log_mutex);	     
		 //BM_Log("Log file name %s \n ",backupLogFileName);
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
                 /*Copy the Ip address */ 		    
                 pthread_mutex_lock(&log_mutex);	     
                 memcpy(backupServerIP,pstCurrVars->val.string,iLen);       
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
                 backupServerPort= (I16) *(pstCurrVars->val.integer);
                 pthread_mutex_unlock(&log_mutex);	     
             }
	     else
    	         return NASC_FAILURE; 	    
       }

       return NASC_SUCCESS;
}//end of JS_GetAgentInfoLogFileBackupDetail

/*****************************************************************************
 *
 * name             :   BM_GetBackupMaster
 * description      :   Fetch the table rows for the backup Master table  
 * input parameters :   - 
 * return type      :   BM_SUCCESS / BM_FAILURE
 * global variables :   
 * calls	    :   
 *                      
 *
*****************************************************************************/

I16 BM_GetBackupMaster()
{
    struct snmp_pdu *reqPDU;
    struct snmp_pdu *respPDU;
    stBackupMaster *pstBMMaster;
    netsnmp_variable_list *vars;
    netsnmp_variable_list *pstCurrVars;
    I16 iCheckTable;

    oid *pstMasID;
    I16 iMasLen; 
    oid *pstMasName;
    I16 iMasNameLen; 
    oid *pstMasInterlval;
    I16 iMasInterLen; 
    oid *pstMasStartTime;
    I16 iMasSTLen; 
    oid *pstMasEndTime;
    I16 iMasEndLen; 
    oid *pstMasIPAddr;
    I16 iMasIPLen; 
    oid *pstMasDestPath;
    I16 iMasDestLen; 
    oid *pstStatus;
    I16 iMasStatLen; 
    oid *tableID;
    I16 tableLen;

    I16 iProcessCount;
    I16 status=0;
    I16 iLen;
	
    tableID = backupMasterTable_variables_oid;
    tableLen = sizeof(backupMasterTable_variables_oid)/ sizeof(oid);

    
    pstMasID = backupMasterID_variables_oid;
    iMasLen  = sizeof(backupMasterID_variables_oid )/ sizeof(oid); 

    pstMasName = backupMasterName_variables_oid;
    iMasNameLen= sizeof(backupMasterName_variables_oid)/ sizeof(oid);

    pstMasInterlval = backupMasterPeriodicInterval_variables_oid;
    iMasInterLen = sizeof(backupMasterPeriodicInterval_variables_oid)/ sizeof(oid);

    pstMasStartTime=backupMasterStartTime_variables_oid ;
    iMasSTLen= sizeof(backupMasterStartTime_variables_oid)/ sizeof(oid); 

    pstMasEndTime= backupMasterFinishTime_variables_oid;
    iMasEndLen= sizeof(backupMasterFinishTime_variables_oid)/ sizeof(oid);

    pstMasIPAddr=backupMasterDestinationIPAddress_variables_oid;
    iMasIPLen= sizeof(backupMasterDestinationIPAddress_variables_oid)/ sizeof(oid); 

    pstMasDestPath= backupMasterDestinationPath_variables_oid;
    iMasDestLen= sizeof(backupMasterDestinationPath_variables_oid)/ sizeof(oid);

    pstStatus = backupMasterStatus_variables_oid;
    iMasStatLen=sizeof(backupMasterStatus_variables_oid)/ sizeof(oid); 


    while(1) 
    {
       reqPDU = snmp_pdu_create(SNMP_MSG_GETBULK);
       reqPDU->non_repeaters = 0;
       /*Set the number of records to be fetched */
       reqPDU->max_repetitions = MAX_REPETITIONS;

       /*Form the backup master ID */	    
       iLen =0; 
       snmp_add_null_var(reqPDU,pstMasID,iMasLen);

       /*Get the backup  master name*/
       snmp_add_null_var(reqPDU,pstMasName,iMasNameLen);

       /*Get the backup  master periodic interval */
       snmp_add_null_var(reqPDU,pstMasInterlval,iMasInterLen);

       /*Get the backup  master Start  time*/
       snmp_add_null_var(reqPDU,pstMasStartTime,iMasSTLen);

       /*Get the backup  master finish time*/
       snmp_add_null_var(reqPDU,pstMasEndTime,iMasEndLen);

       /*Get the backup  master destination IP address*/
       snmp_add_null_var(reqPDU,pstMasIPAddr,iMasIPLen);

       /*Get the backup  master Destination path */
       snmp_add_null_var(reqPDU,pstMasDestPath,iMasDestLen);

       /*Get the backup  master row status*/
       snmp_add_null_var(reqPDU,pstStatus,iMasStatLen);

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
#ifdef BM_DEBUG
	          BM_Log("sync response success \n");	   
#endif
	   }else
	   {
#ifdef BM_DEBUG
	          BM_Log("sync response failure \n");	   
#endif
                  return NASC_FAILURE;		   
	   }
           if(respPDU == NULL)	        
	   {
#ifdef BM_DEBUG
                BM_Log("Error in sync response \n");		   
#endif
		return NASC_FAILURE;
	   }
#ifdef BM_DEBUG
           BM_Log("Request Succeeded\n");
#endif
       } else 
       {
#ifdef BM_DEBUG
           BM_Log("Error in snmp_synch_response \n");
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
             return NASC_FAILURE;	       
       }
       
       /*Flag for checkiing whether the fetched table details belongs to 
	* required table*/ 
       iCheckTable = TRUE;
       for(iProcessCount =0; iProcessCount < MAX_REPETITIONS;
             iProcessCount++ )
       {
              /*Check the fetched record is belong to respective table */
              if(netsnmp_oid_is_subtree(pstMasID,
                 iMasLen, pstCurrVars->name, pstCurrVars->name_length))
	      { 
		       iCheckTable = FALSE;
                       break;
              }

   	      pstBMMaster = malloc(sizeof(stBackupMaster)); 
              if(pstBMMaster == NULL)
              {
#ifdef BM_DEBUG
                   BM_Log("Memory allocation failed \n "); 			
#endif
                   return NASC_INSUFFICIENT_MEMORY;
              }

	      /*Initialize to NULL*/
              pstBMMaster->pcMasterID   = NULL;
              pstBMMaster->pcMasterName = NULL;
              pstBMMaster->pcStartTime 	= NULL;  
              pstBMMaster->pcEndTime    = NULL;
              pstBMMaster->pcIPAddr     = NULL;
              pstBMMaster->pcDestPath   = NULL;
	      
              /*Get the Master ID (Mandatory field )*/
	      if(BM_FillStruct(&(pstBMMaster->pcMasterID),pstCurrVars)
                   == NASC_INSUFFICIENT_MEMORY)
              {
                      BM_FreeBackupMaster(pstBMMaster);
		      return NASC_FAILURE;
              }
	      /*Set the default value */
	      pstBMMaster->iInterval =-1;

              pstMasID= pstCurrVars->name;
              iMasLen= pstCurrVars->name_length;
	      iLen=0;
	      iLen = pstCurrVars->name_length-1;
	      /*Get the Index ID of the table*/
              pstBMMaster->iIndex = pstCurrVars->name[iLen];
              
	      pstCurrVars = pstCurrVars->next_variable;
              if(pstCurrVars == NULL )
              {
                      BM_FreeBackupMaster(pstBMMaster);
		      return NASC_FAILURE;
              }

              /*Get the Master Name */
	      if(BM_FillStruct(&(pstBMMaster->pcMasterName),pstCurrVars)
                  == NASC_INSUFFICIENT_MEMORY)
	      {
                      BM_FreeBackupMaster(pstBMMaster);
		      return NASC_FAILURE;
	      }
              pstMasName= pstCurrVars->name;
              iMasNameLen= pstCurrVars->name_length;
	      pstCurrVars = pstCurrVars->next_variable;
              if(pstCurrVars == NULL )
	      {
                   BM_FreeBackupMaster(pstBMMaster);
                   return NASC_FAILURE;
	      }
                
	      /*Get the interval */
              if(pstCurrVars->val.integer != NULL)
                   pstBMMaster->iInterval= (I16)(*pstCurrVars->val.integer);
              pstMasInterlval = pstCurrVars->name;
              iMasInterLen = pstCurrVars->name_length;
	      pstCurrVars = pstCurrVars->next_variable;
              if(pstCurrVars == NULL )
	      {
                      BM_FreeBackupMaster(pstBMMaster);
		      return NASC_FAILURE;
	      }

              /*Get the Start Time*/
	      if(BM_FillStruct(&(pstBMMaster->pcStartTime),pstCurrVars)
                  == NASC_INSUFFICIENT_MEMORY)
              {
                      BM_FreeBackupMaster(pstBMMaster);
		      return NASC_FAILURE;
               }
              pstMasStartTime= pstCurrVars->name;
              iMasSTLen= pstCurrVars->name_length;
	      pstCurrVars = pstCurrVars->next_variable;
              if(pstCurrVars == NULL )
              {
                     BM_FreeBackupMaster(pstBMMaster);
                     return NASC_FAILURE;
              }


              /*Get the End Time*/
	      if(BM_FillStruct(&(pstBMMaster->pcEndTime),pstCurrVars)
                  == NASC_INSUFFICIENT_MEMORY )
	      {
                      BM_FreeBackupMaster(pstBMMaster);
                      return NASC_FAILURE;		      
	      }
              pstMasEndTime= pstCurrVars->name;
              iMasEndLen= pstCurrVars->name_length;
	      pstCurrVars = pstCurrVars->next_variable;
              if(pstCurrVars == NULL )
              { 
                    BM_FreeBackupMaster(pstBMMaster);
                    return NASC_FAILURE;
              }


              /*Get the IP Address*/
              if(BM_FillStruct(&(pstBMMaster->pcIPAddr),pstCurrVars)
                  == NASC_INSUFFICIENT_MEMORY)
	      {
                      BM_FreeBackupMaster(pstBMMaster);
                      return NASC_FAILURE;		      
	      }
              pstMasIPAddr= pstCurrVars->name;
              iMasIPLen= pstCurrVars->name_length;
	      pstCurrVars = pstCurrVars->next_variable;
              if(pstCurrVars == NULL )
              { 
                    BM_FreeBackupMaster(pstBMMaster);
                    return NASC_FAILURE;
              }


              /*Get the Destination path*/
	      if(BM_FillStruct(&(pstBMMaster->pcDestPath),pstCurrVars)
                   ==NASC_INSUFFICIENT_MEMORY)
	      {
                      BM_FreeBackupMaster(pstBMMaster);
                      return NASC_FAILURE;		      
	      }
              pstMasDestPath= pstCurrVars->name;
              iMasDestLen= pstCurrVars->name_length;
	      pstCurrVars = pstCurrVars->next_variable;
              if(pstCurrVars == NULL )
              { 
                    BM_FreeBackupMaster(pstBMMaster);
                    return NASC_FAILURE;
              }
	     
	       pstBMMaster->iStatus =-1; 
              /*Get the row status */
	      if(pstCurrVars->val.integer != NULL)
                   pstBMMaster->iStatus = (I16)(*pstCurrVars->val.integer);  
              pstStatus=  pstCurrVars->name;
              iMasStatLen= pstCurrVars->name_length;
              
               
	      /*Call shceduler to add to shceduler */
               if(BM_UpdateBackupToScheduler(pstBMMaster,BM_ADD)== NASC_MANDATORY_MISSING)
               {
                     pstCurrVars = pstCurrVars->next_variable;
                     /*Free the master record */
                     BM_FreeBackupMaster(pstBMMaster);
                     continue;
	       }		       
	      
	      /*Move the Pointer to the next localtion*/  
              pstCurrVars = pstCurrVars->next_variable;

              if((pstCurrVars == NULL) && (iProcessCount != (MAX_REPETITIONS-1)))
              { 
                    BM_FreeBackupMaster(pstBMMaster);
                    return NASC_FAILURE;
              }
       }
       if(iCheckTable == FALSE)
       {
            break; 		   
       }		   
     } // end of while(1)
    return NASC_SUCCESS;
}

void BM_FreeBackupMaster(    stBackupMaster *pstBMMaster)
{
      if(pstBMMaster == NULL) 	
      {
          return;	      
      }
      /*Check for other fields */
       if(pstBMMaster->pcMasterID != NULL )
       { 
            free(pstBMMaster->pcMasterID); 
	    return;
       }
       if(pstBMMaster->pcMasterName != NULL )
       { 
            free(pstBMMaster->pcMasterName); 
	    return;
       }
       if(pstBMMaster->pcStartTime != NULL )
       { 
            free(pstBMMaster->pcStartTime); 
	    return;
       }
       if(pstBMMaster->pcEndTime != NULL )
       { 
            free(pstBMMaster->pcEndTime); 
	    return;
       }
       if(pstBMMaster->pcIPAddr != NULL )
       { 
            free(pstBMMaster->pcIPAddr); 
	    return;
       }
       if(pstBMMaster->pcDestPath != NULL )
       { 
            free(pstBMMaster->pcDestPath); 
	    return;
       }
       free(pstBMMaster);
}

I16 BM_FillStruct(UI8  **pcDst,  netsnmp_variable_list *pstCurrVars)
{
        I16 iLen;	
        iLen =0;
        iLen =  pstCurrVars->val_len;
	if(iLen ==0)
	{
             *pcDst=NULL;
             return NASC_FAILURE;		
	     
	}
        *pcDst  = malloc(iLen+1);
        if(*pcDst == NULL)
        {
#ifdef BM_DEBUG
           BM_Log("Memory allocation failed \n ");	
#endif
           return NASC_INSUFFICIENT_MEMORY;
        }
        memset(*pcDst,0,(iLen+1));
        memcpy(*pcDst,pstCurrVars->val.string,iLen);
	return NASC_SUCCESS;
}

/*****************************************************************************
 *
 * name             :   BM_UpdateBackupToScheduler  
 * description      :   Adds the filter rule 
 * input parameters :   pstBMStruct -> pointer to the stBackupMaster table
 *                      iDecisionFlag -> BM_ADD / BM_MODIFY / BM_DEL values
 * return type      :   BM_SUCCESS / BM_FAILURE / BM_NO_RECORD_FOUND
 * global variables :   - 
 * calls	    :   BM_Log	
 * 		        BM_AddBackupToSched
 * 		        IM_ModifyRule
 * 		        BM_DeteteFromSched
*****************************************************************************/
I16 BM_UpdateBackupToScheduler( stBackupMaster  *pstBMStruct, I16 iDecisionFlag)
{
    
    I16 iResult;

#ifdef BM_DEBUG
    BM_Log("BM_UpdateBackupToScheduler called \n"); 
#endif
    if(pstBMStruct == NULL)
    {
#ifdef BM_DEBUG
         BM_Log("No record found \n");	    
#endif
         return NASC_FAILURE;   				
    }

    /*Check all mandatory fields are set for Addition  and Modification */
    if((pstBMStruct->pcMasterID== NULL) || (pstBMStruct->pcStartTime == NULL)
        || (pstBMStruct->pcEndTime==NULL) || ( pstBMStruct->iInterval== -1))
    {
          return  NASC_MANDATORY_MISSING;  	    
    }


    switch(iDecisionFlag)
    {
        case BM_ADD:
             iResult =0; 		
             iResult = BM_AddBackupToSched(pstBMStruct); 		
	     if(!iResult)
             {
#ifdef BM_DEBUG
                  BM_Log("Error : Error occured while adding task to scheduler \n");	
#endif
               	  return NASC_FAILURE;   
             }
             break;		
        case BM_MODIFY:
             iResult =0; 		
             iResult = BM_ModifyBackupToSched(pstBMStruct); 		
	     if(!iResult)
             {
#ifdef BM_DEBUG
                  BM_Log("Error : Error occured while modifying task to scheduler \n");		     
#endif
               	  return NASC_FAILURE;   
             }
	     
             break;		
        case BM_DELETE:
             iResult =0; 		
             iResult = BM_DeteteFromSched(pstBMStruct); 		
	     if(!iResult)
             {
#ifdef BM_DEBUG
                  BM_Log("Error : Error occured while deleting the task \n");		     
#endif
               	  return NASC_FAILURE;   
             }
             break;		
    }	    
    return 1;
}

I16 BM_ScheduleLogInfo()	   
{

    stTask task; 
    UI32 ui32TaskId;
    struct tm stTm;
    struct timeval stTv;

#ifdef BM_DEBUG
    BM_Log("BM_ScheduleLogInfo... Entering \n");
#endif
    gettimeofday(&stTv, (struct timezone *)0);
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
    task.fp = BM_CallBackLogFile;

    /*Add to the scheduler */
    ui32TaskId = SCH_AddTask(task);           	
    if(ui32TaskId ==0)
    {
#ifdef BM_DEBUG
         BM_Log("Error occured while adding the task to scheduler \n ");	    
#endif
    }
#ifdef BM_DEBUG
    BM_Log("BM_ScheduleLogInfo... Leaving\n");
#endif
    return NASC_SUCCESS;

}

void BM_CallBackLogFile()
{
     pthread_t logThread;	
          
     pthread_create(&logThread,NULL,(void*)BM_SendLogFile,NULL);
}

void BM_SendLogFile()
{
      struct sockaddr_in sock_addr;
      I16 iError;
      I16 iResult;
      char cBuff[15];
       
#ifdef BM_DEBUG
      BM_Log("Calling BM_SendLogFile.. \n"); 
#endif

      /*Shoud be removed from the code */
      memset(charBuff, '\0',C_MAX_SIZE); 
      memset(cBuff,'\0',15);
      strcat(charBuff,"Sample content to be transferred to backup log file \n");
      sock_addr.sin_family = AF_INET;
      pthread_mutex_lock(&log_mutex);	     
      fprintf(fpForBM, "%s",charBuff);       	  
      fclose(fpForBM);
      /*Send the log file to the manager */
      sock_addr.sin_port = htons(backupServerPort);
      sprintf(cBuff,"%d.%d.%d.%d",(0xff& backupServerIP[0]),(0xff & 
      backupServerIP[1]), (0xff & backupServerIP[2]),(0xff& backupServerIP[3]));
      pthread_mutex_unlock(&log_mutex);	     
      //inet_pton(AF_INET , cBuff, &sock_addr.sin_addr);
	  sock_addr.sin_addr.s_addr = inet_addr(cBuff);

      iError = FileTrans_SendFile (backupLogFileName, &sock_addr
          ,BM_DEST_PATH_DIR, 512);             
      if(iError == NASC_SUCCESS )
      {
#ifdef BM_DEBUG
          BM_Log("Log file is transferred to Server\n");
#endif
      }else
      {
#ifdef BM_DEBUG
          BM_Log("Error in sending log file to server  \n");
#endif
      }
      /*Call the GetAgentInfoMIB to get the Mib values */
      iResult = BM_GetAgentInfoMIB();
      if(iResult == NASC_FAILURE)
      {
#ifdef BM_DEBUG
	      BM_Log("Error in retrieving log file info");
#endif
              return;
      }
      /*Open the file decriptor for log file*/
      fpForBM = fopen(backupLogFileName , "a+");
      if(fpForBM == NULL)
             return;	   
#ifdef BM_DEBUG
      BM_Log("Ending BM_SendLogFile.. \n"); 
#endif
}


/*****************************************************************************
 * name             :   BM_AddBackupToSched
 * description      :   Adds the filter rule 
 * input parameters :   pstBMStruct -> pointer to the stBackupMaster table
 *                      stListPtr-> Pointer to the stList structure 
 * return type      :   BM_SUCCESS / BM_FAILURE  
 * global variables :   - 
 * calls	    :   BM_Log	
 * 		        IM_Insert 	
*****************************************************************************/
I16 BM_AddBackupToSched(stBackupMaster  *pstBMStruct)
{
    UI32 ui32TaskId;
    UI32 ulTime;
    struct tm pstTmStart;
    struct tm pstTmEnd;
    stTask task; 
    I16 iRetCount;

#ifdef BM_DEBUG
    BM_Log("BM_AddBackupToSched called \n"); 
#endif
    if(pstBMStruct == NULL)
    {
#ifdef BM_DEBUG
         BM_Log("No record found \n");	    
#endif
         return NASC_FAILURE;   				
    }
    
    if(pstBMStruct->iInterval == -1)
    {
#ifdef BM_DEBUG
           BM_Log("Invalid Interval \n");	    
#endif
	   return NASC_FAILURE;
    }

    /*fill the task structure */
    if((pstBMStruct->pcStartTime == NULL))
    {
#ifdef BM_DEBUG
          BM_Log("Start time is NULL ...\n");	    
#endif
          return NASC_FAILURE;	    
    }

    ulTime = BM_FillTimeStruct(&pstTmStart,pstBMStruct->pcStartTime);  	    
    if(ulTime == 0)
    { 
#ifdef BM_DEBUG
         BM_Log("No Start time has been set \n"); 		 
#endif
         return NASC_FAILURE; 		  
    }
    /*Update the task structure */
    task.StartTime.ui16Hour   =   pstTmStart.tm_hour ;
    //BM_Log("Hour %d \n", task.StartTime.ui16Hour );
    task.StartTime.ui16Minute =   pstTmStart.tm_min ;
    //BM_Log("Minute %d \n", task.StartTime.ui16Minute);
    task.StartTime.ui16Second =   pstTmStart.tm_sec;
    //BM_Log("Second %d \n", task.StartTime.ui16Second);
    task.StartDate.ui16Day    =   pstTmStart.tm_mday;
    //BM_Log("Day %d \n", task.StartDate.ui16Day);
    task.StartDate.ui16Month  =   pstTmStart.tm_mon;
    //BM_Log("Month %d \n", task.StartDate.ui16Month);
    task.StartDate.ui16Year   =   pstTmStart.tm_year; 
    //BM_Log("Year %d \n", task.StartDate.ui16Year);
    //BM_Log("Interval %d \n", task.ui32Interval);
    iRetCount= BM_GetDay(ulTime);
    //BM_Log("Day count %d \n",iRetCount);

    
    /*Check the end time */
    if((pstBMStruct->pcEndTime== NULL)  && (pstBMStruct->iInterval != 0))
    {
#ifdef BM_DEBUG
          BM_Log("End time is NULL ...\n");	    
#endif
          return NASC_FAILURE;	    
    }

    if(pstBMStruct->iInterval != 0)
    {
          ulTime = BM_FillTimeStruct(&pstTmEnd ,pstBMStruct->pcEndTime);  	    
          if(ulTime == 0)
          {
#ifdef BM_DEBUG
                BM_Log("No Start time has been set \n"); 		 
#endif
                return NASC_FAILURE; 		  
          }

          pstBMStruct->uiEndTimeinNum= ulTime;
          task.EndTime.ui16Hour   =   pstTmEnd.tm_hour ;
          task.EndTime.ui16Minute =   pstTmEnd.tm_min ;
          task.EndTime.ui16Second =   pstTmEnd.tm_sec;
          task.EndDate.ui16Day    =   pstTmEnd.tm_mday;
          task.EndDate.ui16Month  =   pstTmEnd.tm_mon;
          task.EndDate.ui16Year   =   pstTmEnd.tm_year; 
    }


    /*Update the interval */
    switch(pstBMStruct->iInterval)
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
#ifdef BM_DEBUG
               BM_Log("Job Master interval is Wrong !! \n"); 		 
	       BM_Log("Job Can not be added to scheduler \n");
#endif
	       break;
    }

    task.vpArg = (void*)pstBMStruct;
    task.fp = BM_PerformBackupAction;

    /*Add to the scheduler */
    ui32TaskId = SCH_AddTask(task);           	
    if(ui32TaskId ==0)
    {
#ifdef BM_DEBUG
         BM_Log("Error occured while adding the task to scheduler \n ");	    
#endif
    }
    BM_AddNodeInBackupList(ui32TaskId ,pstBMStruct->iIndex, stListPtr);
    return NASC_SUCCESS;
}

unsigned long BM_FillTimeStruct(struct tm *pstTm, char *pstTime)  	    
{
    unsigned long iTime= 0;
    UI8 szStr[80] = { 0 };
    int iYear=0;

    if(pstTime == NULL)
    {
#ifdef BM_DEBUG
               BM_Log("BM_FillTimeStruct Time is NULL\n");	    
#endif
	       return NASC_FAILURE;
    }
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
    pstTm->tm_year += 1900;

    return iTime;
}	

I16 BM_ModifyBackupToSched(stBackupMaster  *pstBMStruct)
{
    UI32 ui32TaskId;
    UI32 oldTaskID;
    time_t ulTime;
    I16 iDayCount;
    struct tm pstTmStart;
    struct tm pstTmEnd;
    stTask *pstModTask;
    stBackupMaster  *pstBMMod;
    I16 iResult;
//    time_t timeVal;

    /*Get the task ID from the list*/
    /*To be checked */
    ui32TaskId = BM_GetTaskID(pstBMStruct->iIndex, stListPtr);
    if(ui32TaskId == 0)
    {
#ifdef BM_DEBUG
          BM_Log("No task ID found in the List ");		
#endif
          return NASC_FAILURE;
    }
    /*Get the task structure */

    /*Allocate memory for pstModTask struct*/
    pstModTask= malloc(sizeof(stTask));
    
    iResult = SCH_GetTask(ui32TaskId, &pstModTask);
    if(iResult ==  NASC_FAILURE)
    {
#ifdef BM_DEBUG
        BM_Log("Error in retrieving Task Id from scheduler \n ");	    
#endif
	return NASC_FAILURE;
    }
    
    if(pstBMStruct->pcStartTime != NULL) 
    {
         /*fill the task structure */
         ulTime = BM_FillTimeStruct(&pstTmStart,pstBMStruct->pcStartTime);  	    
         if(ulTime == 0)
         {
#ifdef BM_DEBUG
               BM_Log("No Start time has been set \n"); 		 
#endif
               return NASC_FAILURE; 		  
         }
	 iDayCount= BM_GetDay(ulTime);

	 //BM_Log("Day count %d \n",iDayCount);
         /*Update the task structure */
         pstModTask->StartTime.ui16Hour   =   pstTmStart.tm_hour ;
         pstModTask->StartTime.ui16Minute =   pstTmStart.tm_min ;
         pstModTask->StartTime.ui16Second =   pstTmStart.tm_sec;
         pstModTask->StartDate.ui16Day    =   pstTmStart.tm_mday;
         pstModTask->StartDate.ui16Month  =   pstTmStart.tm_mon;
         pstModTask->StartDate.ui16Year   =   pstTmStart.tm_year; 

    }

    /*Check the end time */
    if((pstBMStruct->pcEndTime != NULL))
    {
         ulTime = BM_FillTimeStruct(&pstTmEnd ,pstBMStruct->pcEndTime);  	    
         if(ulTime == 0)
         {
#ifdef BM_DEBUG
               BM_Log("No Start time has been set \n"); 		 
#endif
               return NASC_FAILURE; 		  
         }

         pstBMStruct->uiEndTimeinNum= ulTime;
         pstModTask->EndTime.ui16Hour   =   pstTmEnd.tm_hour ;
         pstModTask->EndTime.ui16Minute =   pstTmEnd.tm_min ;
         pstModTask->EndTime.ui16Second =   pstTmEnd.tm_sec;
         pstModTask->EndDate.ui16Day    =   pstTmEnd.tm_mday;
         pstModTask->EndDate.ui16Month  =   pstTmEnd.tm_mon;
         pstModTask->EndDate.ui16Year   =   pstTmEnd.tm_year; 

	 if(pstBMStruct->iInterval != -1 )
         /*Update the interval */
         switch(pstBMStruct->iInterval)
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
#ifdef BM_DEBUG
                    BM_Log("Job Master interval is Wrong !! \n"); 		 
	            BM_Log("Job Can not be added to scheduler \n");
#endif
                    break;
        }
    }

    /*Modify the Master ID with respect to Modified name*/ 

    /*Get the void pointer struct*/
    pstBMMod = pstModTask->vpArg;
    

    if(pstBMMod->pcMasterID != NULL)
    {
         free(pstBMMod->pcMasterID);
 	 pstBMMod->pcMasterID = malloc(strlen(pstBMStruct->pcMasterID));
	 strcpy(pstBMMod->pcMasterID,pstBMStruct->pcMasterID);
    }

    /*Modify the Master name*/
    if(pstBMStruct->pcMasterName != NULL)
    {
	 free(pstBMMod->pcMasterName);
 	 pstBMMod->pcMasterName = malloc(strlen(pstBMStruct->pcMasterName));
	 strcpy(pstBMMod->pcMasterName,pstBMStruct->pcMasterName);
    }
     
    /*Modify the IP Address*/
    if(pstBMStruct->pcIPAddr != NULL)
    {
	 free(pstBMMod->pcIPAddr);
 	 pstBMMod->pcIPAddr = malloc(strlen(pstBMStruct->pcIPAddr));
	 strcpy(pstBMMod->pcIPAddr,pstBMStruct->pcIPAddr);
    }
    /*Modify the destination path*/ 
    if(pstBMStruct->pcDestPath != NULL)
    {
	 free(pstBMMod->pcDestPath);
 	 pstBMMod->pcDestPath = malloc(strlen(pstBMStruct->pcDestPath));
	 strcpy(pstBMMod->pcDestPath,pstBMStruct->pcDestPath);
    }
    
	    
    pstModTask->vpArg = (void*)pstBMStruct;
	 
    pstModTask->fp = BM_PerformBackupAction;
    /*Add to the scheduler */
    oldTaskID = ui32TaskId;
    ui32TaskId = SCH_ModifyTask(ui32TaskId, *pstModTask); 
    if(ui32TaskId ==0)
    {
#ifdef BM_DEBUG
         BM_Log("Error occured while adding the task to scheduler \n ");	    
#endif
	 return NASC_FAILURE;
    }
    

    /*delete the old task from the list*/  	    
    iResult = BM_DeleteNodeFromList(stListPtr,pstBMStruct->iIndex,oldTaskID);
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
     
    BM_AddNodeInBackupList(ui32TaskId ,pstBMStruct->iIndex ,stListPtr);
    return NASC_SUCCESS;
}

I16 BM_GetDay(time_t iTime )
{
       UI8 cDay[4];
       char cTimeBuff[100];	
       UI8 cOthers[30];
       sprintf(cTimeBuff,"%s",ctime(&iTime));
       //BM_Log("Time is %s \n",cTimeBuff);
       sscanf(cTimeBuff,"%s %s",cDay,cOthers );
       //BM_Log("Day is %s \n",cDay);

       return BM_GetDayNum(cDay);
}

I16 BM_GetDayNum(UI8 *pcDay)
{
     I16 iCount;	
     for(iCount=0; iCount< BM_DAY_CNT;iCount++)	
     {
         if(strcmp(pcDay,Days[iCount])==0)	
         {
                 return iCount;		 
         }
     }
     return NASC_FAILURE; 
}
/*****************************************************************************
 * name             :   BM_DeteteFromSched
 * description      :   Delete the filter rule 
 * input parameters :   pstBMStruct -> pointer to the stBackupMaster table
 *                      stListPtr-> Pointer to the stList structure 
 * return type      :   BM_SUCCESS / BM_FAILURE  
 * global variables :   - 
 * calls	    :   BM_DeteteFromSchedNumFromList
 *                      BM_DeleteFromList
*****************************************************************************/
I16 BM_DeteteFromSched(stBackupMaster  *pstBMStruct)
{
    UI32 ui32DelTaskId;
    UI16 iResult;
    if(pstBMStruct == NULL)
    {
#ifdef BM_DEBUG
         BM_Log("No record found \n");	    
#endif
         return NASC_FAILURE;   				
    }
    /*Get the task ID*/ 	    
    ui32DelTaskId = BM_GetTaskID(pstBMStruct->iIndex,stListPtr );    
    if(ui32DelTaskId == -1)
    {
	 return NASC_FAILURE;
    }
    iResult = SCH_DeleteTask(ui32DelTaskId);
    if(iResult ==0)
    {
#ifdef BM_DEBUG
          BM_Log("Error in deleting the task \n");   		 
#endif
    }else
    {
          return NASC_SUCCESS;
    }
    iResult = BM_DeleteNodeFromList(stListPtr,pstBMStruct->iIndex ,ui32DelTaskId);
    if(iResult ==0)
    {
#ifdef BM_DEBUG
          BM_Log("Error in deleting the task ID from the list \n");   		 
#endif
    }else
    {
#ifdef BM_DEBUG
          BM_Log("Node deleted from the list \n");   		 
#endif
          return NASC_SUCCESS;
    }
    return NASC_SUCCESS;
}

/*****************************************************************************
 *
 * name             :   BM_InitCBuffer  
 * description      :   Initialize the circular queue 
 * input parameters :   pstBuffer pointer to the circular buffer queue 
 * return type      :   void 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
void BM_InitCBuffer(SCBuffer *pstCBuffer)
{
  /*I16 iCount;	*/
  /*Initialize the circular queue*/	
  pstCBuffer->iQRear = -1;
  pstCBuffer->iQFront = 0;
  pthread_mutex_init(&(pstCBuffer->queue_lock) , NULL);
  pthread_cond_init(&(pstCBuffer->queue_not_full) , NULL);
  pthread_cond_init(&(pstCBuffer->queue_not_empty) , NULL);
  /*for(iCount =0;iCount <BM_MAX_ELEMENT;iCount++)
  {
     pstCBuffer->queueEntry[iCount] = NULL;
  }*/
}

/*****************************************************************************
 *
 * name             :   BM_IsQueueEmpty  
 * description      :   Check if the circular queue is empty 
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   BOOL 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
BOOL  BM_IsQueueEmpty(SCBuffer *pstCBuffer)
{
  /*Check Queue is empty*/	
  BOOL status = FALSE;
  if(((pstCBuffer->iQRear + 1) % (BM_MAX_ELEMENT)) == pstCBuffer->iQFront)
  {
    status = TRUE;
  }
  return status;
}


/*****************************************************************************
 *
 * name             :   BM_IsQueue_Full  
 * description      :   Check if the circular queue is full 
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   BOOL 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
BOOL  BM_IsQueue_Full(SCBuffer *pstCBuffer)
{
  /*Check queue is full*/	
  BOOL status = FALSE;
  if(((pstCBuffer->iQRear + 2) % (BM_MAX_ELEMENT )) == pstCBuffer->iQFront)
  {
    status = TRUE;
  }
  return status;

}

/*****************************************************************************
 *
 * name             :   BM_ProcessTrapMsg 
 * description      :   Copy the trap PDU and process the same  
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   void 
 * global variables :   - 
 * calls	    :   BM_IsQueueEmpty
 *                      BM_ParseTrap   
 * 		        
*****************************************************************************/
void BM_ProcessTrapMsg(void *args)
{
  netsnmp_pdu *trapPDU;
  netsnmp_pdu *pstClonePDU;
  
  
  /*Thread function to process the trap message*/
  while(1)
  {
       pthread_mutex_lock(&(stCB.queue_lock));
#ifdef BM_DEBUG
       BM_Log("Enter into queue \n");
#endif
       if(BM_IsQueueEmpty(&stCB))
       {
#ifdef BM_DEBUG
           BM_Log("BM_ProcessTrapMsg Queue Empty.. waiting for signal\n");
#endif
           pthread_cond_wait(&(stCB.queue_not_empty) ,
               &(stCB.queue_lock));
       }
       trapPDU =  stCB.queueEntry[stCB.iQFront]; 
       pstClonePDU= snmp_clone_pdu(trapPDU);
       snmp_free_pdu(trapPDU);  
       stCB.queueEntry[stCB.iQFront] = NULL;  

       stCB.iQFront = ++(stCB.iQFront) % (BM_MAX_ELEMENT); 
       pthread_mutex_unlock(&(stCB.queue_lock));
       /*Call function to parse the retrieved trap message */     
       BM_ParseTrap(pstClonePDU);
  }
}

/*****************************************************************************
 *
 * name             :   BM_ListenTrap 
 * description      :   Listens the trap message and copy the same in
 *                      to the circular buffer 
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   NASC_SUCCESS 
 * global variables :   - 
 * calls	    :   BM_IsQueue_Full 
 * 		        
*****************************************************************************/
int BM_ListenTrap(int id, struct snmp_session *sess, int req, netsnmp_pdu *pdu, void *arg)
{
    netsnmp_pdu *pstPDUClone;

    /* Call back function to write the trap message in to circular queue */ 	

#ifdef BM_DEBUG
    BM_Log("Received Trap message \n ");
#endif

    pthread_mutex_lock(&(stCB.queue_lock));
    if(BM_IsQueue_Full(&stCB))
    {
#ifdef BM_DEBUG
      BM_Log("Dropping the Trap Msg \n");
#endif
      pthread_mutex_unlock(&(stCB.queue_lock));
      /*Error should be logged in the syslog */
    }
    stCB.iQRear = ++(stCB.iQRear) % (BM_MAX_ELEMENT);
    pstPDUClone = snmp_clone_pdu(pdu);
    stCB.queueEntry[stCB.iQRear] = pstPDUClone;
    pthread_cond_signal(&(stCB.queue_not_empty)); 
    pthread_mutex_unlock(&(stCB.queue_lock));
    return 1;
                                                                    
}

/*****************************************************************************
 *
 * name             :   BM_ParseTrap 
 * description      :   Parse the trap message and calls the function for 
 *                      creating the rule  
 * input parameters :   respPDU -> pointer to the snmp_pus structure 
 * return type      :   void 
 * global variables :   - 
 * calls	    :   BM_Log
 *                      BM_UpdateBackupToScheduler
 *                      BM_CheckTable
*****************************************************************************/
void BM_ParseTrap( struct snmp_pdu *respPDU)
{
    I16 iOperFlag;
    I16 iDecisionFlag;
    netsnmp_variable_list *vars;
    netsnmp_variable_list *pstCurrVars;
    I16 iFieldFlag= -1 ;
    stBackupMaster *pstBMMaster;

    
    vars = respPDU->variables;
    pstCurrVars = vars;

#ifdef BM_DEBUG
    BM_Log("BM_ParseTrap called \n"); 
#endif
    iOperFlag = BM_CheckTable(pstCurrVars);
    if(iOperFlag == -1)
    {
#ifdef BM_DEBUG
         BM_Log("Not belong to this table \n");	    
#endif
         return;
    }

    /*Allocate Memory for Generic table structure*/
    pstBMMaster=malloc(sizeof(stBackupMaster));
    if(pstBMMaster == NULL)
    {
#ifdef BM_DEBUG
         BM_Log("Memory allocation failed \n "); 			
#endif
         return ;
    }
    if(pstCurrVars != NULL)
    {
         /*Get the Index ID of the table*/
         pstBMMaster->iIndex = BM_GetIndex(pstCurrVars);
    }else
    {
         free(pstBMMaster);
#ifdef BM_DEBUG
         BM_Log("No record exists \n");	    
#endif
	 return ;
    }

    pstBMMaster->iStatus =0;
    pstBMMaster->pcMasterID= NULL;
    pstBMMaster->pcMasterName= NULL;
    pstBMMaster->pcStartTime =NULL;
    pstBMMaster->pcEndTime =NULL;
    pstBMMaster->pcIPAddr= NULL;
    pstBMMaster->pcDestPath = NULL;
    pstBMMaster->iInterval = -1;

    while(pstCurrVars != NULL) 
    {
           iFieldFlag = BM_CheckField(pstCurrVars);  	    
	   switch(iFieldFlag)
           {
             case BM_MASTER_ID:
                   /*Get the Master ID */
          	   if(BM_FillStruct(&(pstBMMaster->pcMasterID),pstCurrVars)
                       == NASC_INSUFFICIENT_MEMORY)
	           {		   
                      BM_FreeBackupMaster(pstBMMaster);
                      return ;		      
		   }
		   break;
             case BM_MASTER_NAME:
                   /*Get the Master Name */
          	   if(BM_FillStruct(&(pstBMMaster->pcMasterName),pstCurrVars)
                       == NASC_INSUFFICIENT_MEMORY)
	           {		   
                      BM_FreeBackupMaster(pstBMMaster);
                      return ;		      
		   }
                   break;		     
             case BM_PERIODIC_INTERVAL:
                   /*Get the Interval */
		   if(pstCurrVars->val.integer != NULL)
                        pstBMMaster->iInterval= (I16)(*pstCurrVars->val.integer);
                   break;
             case BM_START_TIME:
                   /*Get the Start Time*/
                   if(BM_FillStruct(&(pstBMMaster->pcStartTime),pstCurrVars)
                       == NASC_INSUFFICIENT_MEMORY)
	           {		   
                      BM_FreeBackupMaster(pstBMMaster);
                      return ;		      
		   }
                   break;
             case BM_FINISH_TIME:
                   /*Get the End Time*/
          	   if(BM_FillStruct(&(pstBMMaster->pcEndTime),pstCurrVars)
                      == NASC_INSUFFICIENT_MEMORY)
	           {		   
                      BM_FreeBackupMaster(pstBMMaster);
                      return ;		   
		   }
                   break;
             case BM_DEST_IPADDR:
                  /*Get the IP Address*/
                   if(BM_FillStruct(&(pstBMMaster->pcIPAddr),pstCurrVars)
                       == NASC_INSUFFICIENT_MEMORY)
	           {		   
                      BM_FreeBackupMaster(pstBMMaster);
                      return ;		   
		   }
                   break;
             case BM_DEST_PATH:
                  /*Get the Master Destination path*/
                   if(BM_FillStruct(&(pstBMMaster->pcDestPath),pstCurrVars)
                      == NASC_INSUFFICIENT_MEMORY)
	           {		   
                      BM_FreeBackupMaster(pstBMMaster);
                      return ;		   
		   }
                   break;
             case BM_ROW_STATUS:
                   /*Get the row status */
		   if(pstCurrVars->val.integer != NULL)
                        pstBMMaster->iStatus = (I16)(*pstCurrVars->val.integer);  
                   break;

             default :
#ifdef BM_DEBUG
                   BM_Log("Received other records so continue..\n");
#endif
           }

	    /*Move to the next record */ 
	    pstCurrVars = pstCurrVars->next_variable;
    }
  
    /*Check any field is set */
    if((pstBMMaster->iStatus ==0) &&  (pstBMMaster->pcMasterID== NULL)
     && ( pstBMMaster->pcMasterName== NULL) && (pstBMMaster->pcStartTime ==NULL)
     &&(pstBMMaster->pcEndTime ==NULL ) && (pstBMMaster->pcIPAddr== NULL)
     &&(pstBMMaster->pcDestPath == NULL) && (pstBMMaster->iInterval == -1))
    {
#ifdef BM_DEBUG
        BM_Log("It is an Dummy record and return \n");	    
#endif
	return ;
    }

    switch(pstBMMaster->iStatus)
    {
          case 4:
                 iDecisionFlag = BM_ADD;
                 break;
          case 6:
                 iDecisionFlag = BM_DELETE;
                 break;
          default:
                 iDecisionFlag = BM_MODIFY;
                 break;
    }
    
    BM_UpdateBackupToScheduler(pstBMMaster, iDecisionFlag);
    return ;
}

I16 BM_GetIndex(netsnmp_variable_list *pstCurrVars)
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
 * name             :   BM_CheckTable  
 * description      :   Function finds the fetched table is Telnet or 
 *                      FTP or SMTP or POP3 table   
 * input parameters :   - 
 * return type      :   Protocol to be allowed ( Telnet / FTP/ SMTP /POP3 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
I16 BM_CheckTable(netsnmp_variable_list *pstCurrVars)
{
     I16 iOperFlag; 
     I16 iLen;

     iOperFlag =-1;
    /*Check the fetched record is pertaining to Backup Master Table */

     iLen =0;
     iLen = sizeof(backupMasterTable_variables_oid) / sizeof(oid);
     if(!netsnmp_oid_is_subtree(backupMasterTable_variables_oid,
         iLen, pstCurrVars->name, pstCurrVars->name_length))
     { 
         iOperFlag = 1;
     }

    return iOperFlag;
}

/*****************************************************************************
 *
 * name             :   
 * description      :   Function finds the field type in telnet 
 * input parameters :   pstSNMPPDU -> Pointer to the netsnmp_variable_list
 * return type      :   I16 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
I16 BM_CheckField( netsnmp_variable_list *pstSNMPPDU )
{
     I16 iLen;
     I16 iRetValue=-1;

     iLen=sizeof(backupMasterID_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(backupMasterID_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue= BM_MASTER_ID;	     
     }
     iLen=sizeof(backupMasterName_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(backupMasterName_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =BM_MASTER_NAME;	     
     }
     
     iLen=sizeof(backupMasterPeriodicInterval_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(backupMasterPeriodicInterval_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =BM_PERIODIC_INTERVAL;	     
     }

     iLen=sizeof(backupMasterStartTime_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(backupMasterStartTime_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =BM_START_TIME;	     
     }
     
     iLen=sizeof(backupMasterFinishTime_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(backupMasterFinishTime_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =BM_FINISH_TIME;	     
     }
     iLen=sizeof(backupMasterDestinationIPAddress_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(backupMasterDestinationIPAddress_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =BM_DEST_IPADDR;	     
     }
     iLen=sizeof(backupMasterDestinationPath_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(backupMasterDestinationPath_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =BM_DEST_PATH;	     
     }
     iLen=sizeof(backupMasterStatus_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(backupMasterStatus_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =BM_ROW_STATUS;	     
     }
     return iRetValue;
}

void BM_PerformBackupAction(void *args)
{
     pthread_t backupThread;

#ifdef BM_DEBUG
     BM_Log("BM_PerformBackupAction called \n");
#endif

     /*Create a thread to perform the backup action*/
     pthread_create(&backupThread,NULL,(void*)BM_SpawnBackup,args);
    

}

I16 BM_SpawnBackup(void *pvBkpPtr)
{
    pthread_t  Thread[5];
    struct snmp_pdu *reqPDU;
    struct snmp_pdu *respPDU;
    netsnmp_variable_list *vars;
    netsnmp_variable_list *pstCurrVars;
    oid *pstDet1;
    I16 iDetLen1; 
    oid *pstDet2;
    I16 iDetLen2; 
    oid *pstDet3;
    I16 iDetLen3; 
    oid *pstDet4;
    I16 iDetLen4; 
    oid *pstDet5;
    oid  *bkpMasterID;
    I16 bkpMasterLen;
    I16 iDetLen5; 
    oid *tableID;
    I16 tableLen;
    oid *pstField;
    I16 iDetLen; 
    I16 cMasterNameFlag;
    I16 cStrCmpflag;
    
    I16 iProcessCount;
    I16 status=0;
    UI8 *inputArray[5];
    I16 iRecCount=0;
    stBackupStruct *pstBkp;
    stBackupMaster *pstBackup;
    I16 iCount;
    I16 iCheckTable; 
    I16 iResult; 
    UI8 *pcMasterName;


    /*Fetch the Detail table records  */  
    pstBackup = (stBackupMaster *)pvBkpPtr;
    
/*     uiCurrentTime = BM_GetCurrentTime(); 
     if(pstBackup->uiEndTimeinNum < uiCurrentTime)
     {
          BM_Log("Current Time is exceeded the end time \n");	     
	  return NASC_FAILURE;
     }*/

     
    tableID = backupDetailTable_variables_oid;
    tableLen = sizeof(backupDetailTable_variables_oid)/ sizeof(oid);

    bkpMasterID  = backupDetailMasterID_variables_oid;
    bkpMasterLen = sizeof(backupDetailMasterID_variables_oid)/sizeof(oid);
    
    pstDet1  = backupDetailFileLocation1_variables_oid;
    iDetLen1 = sizeof(backupDetailFileLocation1_variables_oid)/ sizeof(oid); 

    pstField = backupDetailMasterID_variables_oid;
    iDetLen =  sizeof(backupDetailMasterID_variables_oid)/ sizeof(oid); 
	    
    pstDet2  = backupDetailFileLocation2_variables_oid;
    iDetLen2 = sizeof(backupDetailFileLocation2_variables_oid)/ sizeof(oid);

    pstDet3  = backupDetailFileLocation3_variables_oid;
    iDetLen3 = sizeof(backupDetailFileLocation3_variables_oid)/ sizeof(oid);

    pstDet4  =backupDetailFileLocation4_variables_oid;
    iDetLen4 = sizeof(backupDetailFileLocation4_variables_oid)/ sizeof(oid); 

    pstDet5  = backupDetailFileLocation5_variables_oid;
    iDetLen5 = sizeof(backupDetailFileLocation5_variables_oid)/ sizeof(oid);


    while(1) 
    {
       
       reqPDU = snmp_pdu_create(SNMP_MSG_GETBULK);
       reqPDU->non_repeaters = 0;
       /*Set the number of records to be fetched */
       reqPDU->max_repetitions = MAX_REPETITIONS;
       
        snmp_add_null_var(reqPDU, bkpMasterID ,bkpMasterLen );
   

       /*Form backup details */
       snmp_add_null_var(reqPDU,pstDet1,iDetLen1);
       snmp_add_null_var(reqPDU,pstDet2,iDetLen2);
       snmp_add_null_var(reqPDU,pstDet3,iDetLen3);
       snmp_add_null_var(reqPDU,pstDet4,iDetLen4);
       snmp_add_null_var(reqPDU,pstDet5,iDetLen5);


       /*Get the response in the respPDU*/
#ifdef BM_DEBUG
       BM_Log("Before calling sync response \n");
#endif
       status = snmp_synch_response(agentSess,reqPDU,&respPDU);	
#ifdef BM_DEBUG
       BM_Log("After calling sync response \n");
#endif
       if(status==STAT_SUCCESS)
       {
	   if(respPDU->errstat==SNMP_ERR_NOERROR && 
           respPDU->errindex==0 && 
           respPDU->variables->type != ASN_NULL &&
           respPDU->variables->type != SNMP_NOSUCHINSTANCE &&
           respPDU->variables->type != SNMP_NOSUCHOBJECT  &&
	   respPDU->variables->type !=  SNMP_ENDOFMIBVIEW )
	   {
#ifdef BM_DEBUG
	          BM_Log("sync response success \n");	   
#endif
	   }else
	   {
#ifdef BM_DEBUG
	          BM_Log("sync response failure \n");	   
#endif
                  return NASC_FAILURE;		   
	   }
           if(respPDU == NULL)	        
	   {
#ifdef BM_DEBUG
                BM_Log("Error in sync response \n");		   
#endif
		return NASC_FAILURE;
	   }
       } else 
       {
#ifdef BM_DEBUG
           BM_Log("Error in snmp_synch_response \n");
#endif
           snmp_perror("Error ");   	   
           return NASC_FAILURE;		   
       }
       
       vars = respPDU->variables;
       pstCurrVars = vars;

      
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
               /*Initialize all the array to NULL*/ 
               for(iCount=0 ;iCount<5;iCount++ )
               {
                   inputArray[iCount] = NULL ;
               }

	       if(pstCurrVars == NULL ) return NASC_FAILURE;
	       /*Get the Master ID Name */
	      iResult = BM_FillStruct(&pcMasterName, pstCurrVars);
	      if(iResult == NASC_INSUFFICIENT_MEMORY)
		      return NASC_FAILURE;

              if(iResult != NASC_FAILURE)
              {
#ifdef BM_DEBUG
                   BM_Log("Master Name found \n"); 		      
#endif
              }
              bkpMasterID = pstCurrVars->name;
              bkpMasterLen = pstCurrVars->name_length;
	      /*Move the Pointer to the next localtion*/  
              pstCurrVars = pstCurrVars->next_variable;
	      if(pstCurrVars == NULL ) return NASC_FAILURE;

              cMasterNameFlag = 0;    		      
	      if(pcMasterName != NULL)
	      {
                     cMasterNameFlag = 1;    		      
	      }
              cStrCmpflag=  0;

	      if(cMasterNameFlag ==1 )
	      {
	            /*Check the record belongs to this master ID*/ 
                   if(strcmp(pcMasterName, pstBackup->pcMasterID ) !=0 ) 
	           {
                       cStrCmpflag =1;   			   
		   }
	      }
	      if((cStrCmpflag == 1) || (cMasterNameFlag ==0))
	      {
                    /*Skip all the records and check for next set of records */		      
		     /*Move for remaining all the records*/ 
                     pstDet1  = pstCurrVars->name;
                     iDetLen1 = pstCurrVars->name_length;
                     pstCurrVars = pstCurrVars->next_variable; 
        
		     pstDet2  = pstCurrVars->name;
                     iDetLen2 = pstCurrVars->name_length;
                     /*Move the Pointer to the next localtion*/  
                     pstCurrVars = pstCurrVars->next_variable;
        
                     pstDet3  = pstCurrVars->name;
                     iDetLen3 = pstCurrVars->name_length;
               
                     /*Move the Pointer to the next localtion*/  
                     pstCurrVars = pstCurrVars->next_variable;
                     pstDet4  = pstCurrVars->name;
                     iDetLen4 = pstCurrVars->name_length;

                     /*Move the Pointer to the next localtion*/  
                     pstCurrVars = pstCurrVars->next_variable;
                     pstDet5  = pstCurrVars->name;
                     iDetLen5 = pstCurrVars->name_length;
              
                     /*Move the Pointer to the next localtion*/  
                     pstCurrVars = pstCurrVars->next_variable;
		     continue;
              }

	      /*Get the File location with path 1*/
	      iResult = BM_FillStruct(&(inputArray[0]), pstCurrVars);
	      if(iResult == NASC_INSUFFICIENT_MEMORY)
		      return NASC_FAILURE;
              if(iResult != NASC_FAILURE)
              {
		   iRecCount++;   
#ifdef BM_DEBUG
                   BM_Log("File Location 1 with path found \n"); 		      
#endif
              }
              pstDet1  = pstCurrVars->name;
              iDetLen1 = pstCurrVars->name_length;
	      /*Move the Pointer to the next localtion*/  
              pstCurrVars = pstCurrVars->next_variable;

	      iResult = BM_FillStruct(&(inputArray[1]), pstCurrVars);
	      if(iResult == NASC_INSUFFICIENT_MEMORY)
		      return NASC_FAILURE;
              if(iResult != NASC_FAILURE )
              {
		   iRecCount++;   
#ifdef BM_DEBUG
                   BM_Log("File Location 2 with path found \n"); 		      
#endif
              }

              pstDet2  = pstCurrVars->name;
              iDetLen2 = pstCurrVars->name_length;
	      /*Move the Pointer to the next localtion*/  
              pstCurrVars = pstCurrVars->next_variable;

	      iResult = BM_FillStruct(&(inputArray[2]), pstCurrVars);
	      if(iResult == NASC_INSUFFICIENT_MEMORY)
		      return NASC_FAILURE;
              if(iResult != NASC_FAILURE )
              {
		   iRecCount++;   
#ifdef BM_DEBUG
                   BM_Log("File Location 3 with path found \n"); 		      
#endif
              }
              pstDet3  = pstCurrVars->name;
              iDetLen3 = pstCurrVars->name_length;
               
	      /*Move the Pointer to the next localtion*/  
              pstCurrVars = pstCurrVars->next_variable;
	      iResult = BM_FillStruct(&(inputArray[3]), pstCurrVars);
	      if(iResult == NASC_INSUFFICIENT_MEMORY)
		      return NASC_FAILURE;
              if(iResult != NASC_FAILURE )
              {
		   iRecCount++;   
#ifdef BM_DEBUG
                   BM_Log("File Location 4 with path found \n"); 		      
#endif
              }
              pstDet4  = pstCurrVars->name;
              iDetLen4 = pstCurrVars->name_length;

	      /*Move the Pointer to the next localtion*/  
              pstCurrVars = pstCurrVars->next_variable;
	      iResult = BM_FillStruct(&(inputArray[4]), pstCurrVars);
	      if(iResult == NASC_INSUFFICIENT_MEMORY)
		      return NASC_FAILURE;
              if(iResult != NASC_FAILURE )
              {
		   iRecCount++;   
#ifdef BM_DEBUG
                   BM_Log("File Location 5 with path found \n"); 		      
#endif
              }
              pstDet5  = pstCurrVars->name;
              iDetLen5 = pstCurrVars->name_length;
              
	      /*Move the Pointer to the next localtion*/  
              pstCurrVars = pstCurrVars->next_variable;
	      
	      /*Create thread for iRecCount*/
	      for(iCount=0;iCount<5;iCount++)
              {
                   if(inputArray[iCount] != NULL) 		       
		   {
#ifdef BM_DEBUG
                         BM_Log("Before DoBackup function \n"); 		      
#endif
                        /*Allocate memory for pstBkp */                	
                         pstBkp = malloc(sizeof(stBackupStruct));			   
			 if(pstBkp == NULL)
				 return NASC_FAILURE; 
			
			 pstBkp->pcDestIP = NULL;
			 pstBkp->pcStartDate = NULL;
			 pstBkp->pcEndDate = NULL;
			 pstBkp->MasterID = NULL;
			 pstBkp->pcMasterName = NULL;
			 pstBkp->pcDestPath = NULL;
				 
			 
			if(BM_FillBackupStruct(pstBkp ,pstBackup )== NASC_FAILURE)
			{
                              return NASC_FAILURE;				
			}
#ifdef BM_DEBUG
                         BM_Log("pstBackup structure is copied \n"); 		      
#endif
			pstBkp->iMasterIndex = pstBackup->iIndex;

                        pstBkp->pcSourcePath= malloc(strlen(inputArray[iCount])+1);
			if(pstBkp->pcSourcePath == NULL)
				 return NASC_FAILURE; 
                        memset(pstBkp->pcSourcePath,'\0',(strlen(inputArray[iCount])+1));
		        strcpy(pstBkp->pcSourcePath,inputArray[iCount]);  
                        if ( (pthread_create(&Thread[iCount],NULL,(void*)BM_DoBackup,
                        (void*)pstBkp)) != 0 )
                        {
                              perror("Thread can not be created \n ");
                        }
		   }
              }
       }
       if(iCheckTable == FALSE)
       {
           break; 		   
       }		   
    }
    return NASC_SUCCESS;
}

I16 BM_FillBackupStruct( stBackupStruct *pstBkp , stBackupMaster *pstBackup )
{
         /*Fill the Destination IP*/
         pstBkp->pcDestIP = malloc(strlen(pstBackup->pcIPAddr));
         if(pstBkp->pcDestIP == NULL)
	       return NASC_FAILURE; 
         memset(pstBkp->pcDestIP,0,(strlen(pstBackup->pcIPAddr)));
         strcpy(pstBkp->pcDestIP ,pstBackup->pcIPAddr);

         /*Check the destination path is not null because as it is not mandatory*/    
	 if(pstBackup->pcDestPath != NULL)
	 {
               /*Fill the Destination Path*/
               pstBkp->pcDestPath= malloc(strlen(pstBackup->pcDestPath));
               if(pstBkp->pcDestPath == NULL)
	               return NASC_FAILURE; 
               memset(pstBkp->pcDestPath,0,(strlen(pstBackup->pcDestPath)));
               strcpy(pstBkp->pcDestPath,pstBackup->pcDestPath);
	 }
       
         /*Fill the Start Date */
         pstBkp->pcStartDate= malloc(strlen(pstBackup->pcStartTime));
         if(pstBkp->pcStartDate == NULL)
                return NASC_FAILURE; 
         memset(pstBkp->pcStartDate,0,(strlen(pstBackup->pcStartTime)));
         strcpy(pstBkp->pcStartDate,pstBackup->pcStartTime);


	 /*Fill the End Date*/
         pstBkp->pcEndDate = malloc(strlen(pstBackup->pcEndTime));
         if(pstBkp->pcEndDate  == NULL)
                return NASC_FAILURE; 
         memset(pstBkp->pcEndDate ,0,(strlen(pstBackup->pcEndTime)));
         strcpy(pstBkp->pcEndDate ,pstBackup->pcEndTime);
         
	 /*Fill the Master ID*/
         pstBkp->MasterID = malloc(strlen(pstBackup->pcMasterID));
         if(pstBkp->MasterID == NULL)
                return NASC_FAILURE; 
         memset(pstBkp->MasterID,0,(strlen(pstBackup->pcMasterID)));
         strcpy(pstBkp->MasterID,pstBackup->pcMasterID);

             	 
	 /*Fill the Master Name*/
         if(pstBackup->pcMasterName != NULL)  	 
	 {
              pstBkp->pcMasterName= malloc(strlen(pstBackup->pcMasterName));
              if(pstBkp->pcMasterName == NULL)
                     return NASC_FAILURE; 
              memset(pstBkp->pcMasterName,0,(strlen(pstBackup->pcMasterName)));
              strcpy(pstBkp->pcMasterName,pstBackup->pcMasterName);
	 }
	
	return NASC_SUCCESS; 
}

void BM_DoBackup(void *pvBKP)
{
     I16 iError;
     UI8 localBuff[1000];
     UI8 tempStr[100];
     stBackupStruct *pstBacup;
     struct sockaddr_in sock_addr;
     UI16 PORT= 2500;
     UI8 buff[15];
     struct tm pstTmStart;
     UI8 cStartDate[60];
     UI8 cEndDate[60];
     UI8 cTempDestPath[300];
     UI8 cTempMasName[60];
     UI32  ulTime;
     

     pstBacup = (stBackupStruct*)pvBKP ;
     printf("Address of pvBKP %p\n",pstBacup);

#ifdef BM_DEBUG
            BM_Log("BM_DoBackup Called \n");  	      
#endif

     /*Check if Destination IP and source path */
      if((pstBacup->pcDestIP == NULL) || (pstBacup->pcSourcePath == NULL)) 
      {
#ifdef BM_DEBUG
            BM_Log("IP or Path could be null \n");  	      
#endif
	    return;
      }

     sock_addr.sin_family = AF_INET;
     sock_addr.sin_port = htons(PORT);
    
     sprintf(buff,"%d.%d.%d.%d",(0xff& pstBacup->pcDestIP[0]),(0xff & pstBacup->pcDestIP[1]),
         (0xff & pstBacup->pcDestIP[2]),(0xff& pstBacup->pcDestIP[3]));
     
     //inet_pton(AF_INET , buff, &sock_addr.sin_addr);
	 sock_addr.sin_addr.s_addr = inet_addr(buff);

#ifdef BM_DEBUG
     BM_Log("Before Calling FileTrans_SendFile\n"); 		      
#endif
      //pthread_mutex_lock(&log_mutex);	     
     iError = FileTrans_SendFile (pstBacup->pcSourcePath , &sock_addr ,pstBacup->pcDestPath, 512);             
      //pthread_mutex_unlock(&log_mutex);	     
#ifdef BM_DEBUG
     BM_Log("After  Calling FileTrans_SendFile\n"); 		      
#endif

     if(iError == NASC_SUCCESS )
     {
#ifdef BM_DEBUG
           BM_Log("File Transfer Success \n"); 		      
#endif
	   strcpy(tempStr,"File Transfer SUCCESS");
     }else
     {
#ifdef BM_DEBUG
           BM_Log("File Transfer Failed\n"); 		      
#endif
	   strcpy(tempStr,"File Transfer FAILED");
     }
#ifdef BM_DEBUG
     BM_Log("Adding in to char buff \n"); 		      
#endif
     
     ulTime = BM_FillTimeStruct(&pstTmStart,pstBacup->pcStartDate);
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

     ulTime = BM_FillTimeStruct(&pstTmStart,pstBacup->pcEndDate);
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

     memset(cTempMasName,'\0',60);
      
     /*Check any of the field in null*/
     if(pstBacup->pcMasterName != NULL)
	     memcpy(cTempMasName,pstBacup->pcMasterName,strlen(pstBacup->pcMasterName));
     else
	     strcpy(cTempMasName,"None");
     
     memset(cTempDestPath,'\0',300);
     /*Check any of the field in null*/
     if(pstBacup->pcDestPath!= NULL)
	     memcpy(cTempDestPath,pstBacup->pcDestPath,strlen(pstBacup->pcDestPath));
     else
	     strcpy(cTempDestPath,"None");
     
	     
      pthread_mutex_lock(&log_mutex);	     

      sprintf(localBuff,"%s %s %d %s %s %s %s %s \n ",cStartDate,
      cEndDate,pstBacup->iMasterIndex,pstBacup->MasterID,
      cTempMasName,buff,cTempDestPath,tempStr);
      strcat(charBuff,localBuff);

      if(strlen(charBuff) > (C_MAX_SIZE - 200))
      {
           fprintf(fpForBM, "%s",charBuff);       	  
	   memset(charBuff, 0,C_MAX_SIZE); 
	   fclose(fpForBM);
	   /**/
           /*Open the file decriptor for log file*/
           fpForBM = fopen(backupLogFileName , "a+");
           if(fpForBM == NULL)
               return;	   
      }
      /*update in the log file*/   
      pthread_mutex_unlock(&log_mutex);	     
      /*update in the log file*/   
#ifdef BM_DEBUG
            BM_Log("BM_DoBackup Returning\n");  	      
#endif
}

/*****************************************************************************
 * name             :   RTC_Get_Current_Time
 * description      :   Return seconds from 00:00 hours for current day
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :   RTC_Calc_Num_Nodes_Interval, RTC_Add_Intervals_To_List
 *****************************************************************************/

UI32 BM_GetCurrentTime ()
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
 * name             :   BM_Log
 * description      :   Writes the log message in to log file 
 * input parameters :   logMsg ->pointer to the log message
 * return type      :   void 
 * global variables :   ifdForBM-> File descriptor for storing the 
 *                      log message in to log file  
 * calls	    :   - 
 * 		        
*****************************************************************************/
#ifdef BM_DEBUG
void BM_Log(UI8 *logMsg)
{
	
   /*Open the file descriptor for Log file */   
   /*ifdForBM= fopen("BM_Log_file.log","a+");
   if((ifdForBM == NULL))
   {
       BM_Log("Error in opening the file \n");		   
       return;
   }
   fprintf(ifdForBM,"%s",logMsg );	
   fclose(ifdForBM);*/

#ifdef BM_PRINT
   printf("%s",logMsg);   
#endif   

}
#endif

/*****************************************************************************
 *
 * name             :   main 
 * description      :   Initialize the ruleArray , initializes the pointer
 *                      for list  
 * input parameters :  
 * return type      :   int 
 * global variables :   
 * calls	    :   BM_StartBackupProcess
 *                      BM_MakeEmpty
*****************************************************************************/
int main()
{
    pthread_t thID;
    stListPtr = BM_MakeEmpty(NULL);

    //snmp_set_do_debugging(1);
    memset(backupLogFileName,'\0',80);

    memset(backupServerIP,'\0',16);
     
    /*Open the session with the NASCenter sub agent*/	
    BM_InitSession();
    if((pthread_create(&thID,NULL,(void*)BM_Read_Packets,NULL)) != 0 )
    {
#ifdef BM_DEBUG
          BM_Log("Thread can not be created \n");	    
#endif	  

    }
    BM_StartBackupProcess();
  
    //WaitForScheduler();
    pthread_join(thID, NULL);
    return 0;
}


void BM_Read_Packets(void * arg){
   fd_set snmpSet;
   int snmpFds, blocking;
   struct timeval tv;
#ifdef BM_DEBUG
   BM_Log("Inside REad_Pkt......\n");
#endif
   while(1){
        snmpFds=0;
        blocking=1;
        FD_ZERO(&snmpSet);
        snmp_sess_select_info(tSess, &snmpFds,&snmpSet,&tv,&blocking);
        snmpFds = select(snmpFds,&snmpSet,0,0,0);
	if(snmpFds == -1)
	{
             if(snmpFds == EBADF)
	     {
#ifdef BM_DEBUG
                   BM_Log("An invalid file descriptor \n ");
#endif
	     }else if(snmpFds == EINTR)
	     {
#ifdef BM_DEBUG
                 BM_Log("A non blocked signal was caught \n");		     
#endif
	     }else if(snmpFds == ENOMEM)		     
	     {
#ifdef BM_DEBUG
                 BM_Log("Select was unable to allocate memory non \n");		     
#endif
	     }
#ifdef BM_DEBUG
	     BM_Log("Select returned error \n");	
#endif
	     continue;
	}
        if(snmpFds){
#ifdef BM_DEBUG
            BM_Log("Inside  READ TRAP WHILE......\n");
#endif
            snmp_sess_read(tSess, &snmpSet);
        } else {
#ifdef BM_DEBUG
            BM_Log("No FileDS to READ......\n");
#endif
	
    } // end of while(1)
 }
}


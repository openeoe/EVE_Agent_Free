/*****************************************************************************
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
 * $Log: NASCenter_BM.c,v $
 * Revision 1.35  2004/01/19 11:39:31  thiagav
 * Comments added and Header footer is added
 *
 * Revision 1.34  2004/01/14 05:08:56  thiagav
 * Memroy leaks fixed by using purify tool
 *
 * Revision 1.32  2004/01/12 03:43:17  thiagav
 * Code updated after memory leaks check in bounce checker
 *
 * Revision 1.31  2004/01/07 06:15:58  vijaige
 * errno not supported in windows
 *
 * Revision 1.30  2004/01/06 09:07:31  thiagav
 * In the Receiving side change recv address part to INADDR_ANY
 *
 * Revision 1.29  2004/01/06 05:50:14  thiagav
 * All time structure is initialized to NULL
 *
 * Revision 1.28  2004/01/06 05:22:47  thiagav
 * Fix done for getting one hour less
 *
 * Revision 1.27  2004/01/05 12:37:24  thiagav
 * Checked after modifying the task.eType value for monthly activity
 *
 * Revision 1.26  2004/01/05 07:31:41  thiagav
 * Free the Backup structure
 *
 * Revision 1.25  2004/01/05 04:19:52  thiagav
 * Fix done for performing the backup in the destination path
 *
 * Revision 1.24  2004/01/02 09:52:10  thiagav
 * Code modified after code review
 *
 * Revision 1.23  2003/12/25 05:23:36  thiagav
 * Added after verifying the BM_DEBUG falg in code
 *
 * Revision 1.22  2003/12/21 06:39:07  thiagav
 * Bug fixed for row atatus active and inactive conditions
 *
 * Revision 1.21  2003/12/20 04:32:49  thiagav
 * Code modified to long with respect to long value
 *
 * Revision 1.20  2003/12/19 15:15:53  thiagav
 * Fixes are done for transfering the backup file to the destination path, checking the finish time, scheduler fixes
 *
 * Revision 1.19  2003/12/18 10:15:01  testteam
 * Multiple detail table records working fine
 *
 * Revision 1.18  2003/12/18 07:49:44  thiagav
 * Changed to 161 port configuration
 *
 * Revision 1.17  2003/12/18 05:27:33  thiagav
 * BM_InitSession modified
 *
 * Revision 1.16  2003/12/18 03:56:29  thiagav
 * Added fix for fpForBM Check
 *
 * Revision 1.15  2003/12/17 06:22:57  testteam
 * *** empty log message ***
 *
 * Revision 1.14  2003/12/16 07:15:40  thiagav
 * Added
 *
 * Revision 1.13  2003/12/15 14:05:18  vijaige
 * Changed FillTime function
 *
 * Revision 1.12  2003/12/15 13:53:56  thiagav
 * Added
 *
 * Revision 1.11  2003/12/15 03:37:32  thiagav
 * Added after final testing
 *
 * Revision 1.10  2003/12/11 03:37:38  thiagav
 * Added date fix
 *
 * Revision 1.8  2003/12/04 09:53:01  thiagav
 * added after modifications
 *
 * Revision 1.7  2003/12/04 08:23:40  vijaige
 * windows porting
 *
 * Revision 1.6  2003/12/04 07:11:02  thiagav
 * Added
 *
 * Revision 1.4  2003/12/04 04:23:22  thiagav
 * Added after testing
 *
 * Revision 1.2  2003/12/02 11:33:44  thiagav
 * Added after name modification
 *
 * Revision 1.1  2003/12/02 11:21:32  thiagav
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
 * $Id: NASCenter_BM.c,v 1.35 2004/01/19 11:39:31 thiagav Exp $
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
#ifndef WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/utsname.h>
#include <netdb.h> 
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include "NASCenter_BM.h"
#include "NASCenter_Scheduler.h"
#include "NASCenter_Errors.h"
#include "backupMasterTable.h"
#include "backupDetailTable.h"
#include "NASCenter_FileTrans.h"
#include "AgentInfo.h"

#define MAX_REPETITIONS 6

/****************************** local macros *********************************
*
* (Macros shareable with rest of the modules are present in http_sample.h file)
*/

/********************* local structure/union definitions ********************/

/********************* prototypes for local functions ***********************/
I16 BM_StartBackupProcess();
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
I16 BM_FillBackupStruct( stBackupStruct **, stBackupMaster *);
I16 BM_GetIndex(netsnmp_variable_list *pstCurrVars);
void BM_Read_Packets(void *);
unsigned long BM_FillTimeStruct(struct tm *, char *) ; 	    
void BM_PerformBackupAction(void *);
void BM_DoBackup(void *);
void BM_RecvProcess(void*);
UI32 BM_GetCurrentTime ();
I16 BM_ScheduleLogInfo();
void BM_FreeBackupMaster(    stBackupMaster *pstBMMaster);
void BM_CallBackLogFile(void *args);
void BM_SendLogFile();
void BM_Read_Packets_Info(void *args);
void BM_FreeBackup(stBackupStruct *pstBkpPtr);



/************************ globals within this module*************************/

SCBuffer stCB;
stList stListPtr;
struct snmp_session *agentSess;
void *tSess;
char charBuff[C_MAX_SIZE];   
FILE *fpForBM=NULL;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

/*Agent information for log file transformation */
I16 backupServerPort;
I16 backupInterval;
UI8 backupServerIP[20];
UI8 backupLogFileName[255];


/*****************************************************************************
 *
 * name             :   BM_StartBackupProcess 
 * description      :   Starting function of Backup application 
 *                      This function retrieves the table values  
 *                      Creates a thread for processing the trap message 
 * input parameters :   - 
 * return type      :   - 
 * global variables :   ifdForBM -> File Descriptor for logging the debug log
 *                      information 
 * calls            :   BM_GetBackupMaster()
 *                  :   BM_ProcessTrapMsg()
*****************************************************************************/
I16 BM_StartBackupProcess() 
{
   I16 iResult; 	
   pthread_t readThread;
   pthread_t recvThread;
   
    /*Create the receive thread*/ 
   /*Create a thread for processing the trap message  */
   if((pthread_create(&recvThread, NULL , (void *) BM_RecvProcess,
                 NULL)) !=0)
   {
#ifdef BM_DEBUG
         BM_Log("Thread can not be created for trap process \n");	   
#endif
	 return NASC_FAILURE;
   }
   
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
       printf("Error in Retrieving agent Info  table records \n");	
       printf("Please initialize the Agent Info Table Records \n");	
       printf("Backup Management Module is exiting\n");	
       
       return NASC_FAILURE;
   }
   
    pthread_mutex_lock(&log_mutex);	        
   /*Open the file decriptor for log file*/
   fpForBM = fopen(backupLogFileName , "a+");
   if(fpForBM == NULL)
   { 	   
	 pthread_mutex_unlock(&log_mutex);   
         return NASC_FAILURE;	   
   }
   pthread_mutex_unlock(&log_mutex);   
  
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
       BM_Log("Error in Retrieving Backup Master table records \n");	
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
	 return NASC_FAILURE;
   }

   return NASC_SUCCESS;
}

/*****************************************************************************
 *
 * name             :   BM_RecvProcess 
 * description      :   Receiver thread used to retrieve the backup files  
 * input parameters :   - 
 * return type      :   - 
 * global variables :   
 * calls            :   FileTrans_RecvFile()
*****************************************************************************/
void BM_RecvProcess(void *args)
{
    struct sockaddr_in sock_addr;

    memset(&sock_addr,0,sizeof(struct sockaddr_in));
    
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(BACKUP_PORT);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    while(1)
    {
        FileTrans_RecvFile (&sock_addr);
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
   UI8 cPeerName[30];

#ifdef BM_UNIQUE_PORT
   UI8 localAddr[30];
#endif   

   /* For Session Initiation */
   snmp_sess_init(&tempSess);
   tempSess.version = SNMP_VERSION_2c;
   
#ifdef  BM_UNIQUE_PORT
   memset(localAddr,'\0',30); 
   sprintf(localAddr,"%s:%d",gIPAddr,gAgentPort);
   printf(" Local Address %s \n", localAddr);
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
   trapSess.peername =  cPeerName;
   trapSess.local_port = cTrapPort;
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

/*****************************************************************************
 *
 * name             :   BM_GetAgentInfoMIB 
 * description      :   Retrieve the Agent Info values  
 * input parameters :   - 
 * return type      :   - 
 * global variables :   
 * calls            :   
*****************************************************************************/
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
       iLen=sizeof(agentInfoLogFileBackup_oid)/sizeof(oid); 
       snmp_add_null_var(pstreqPDU,agentInfoLogFileBackup_oid,iLen);
      
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
           if(pstrespPDU == NULL)	        
	   {
#ifdef BM_DEBUG
                BM_Log("Error in sync response \n");		   
#endif
		return NASC_FAILURE;
	   }
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
                 memset(backupLogFileName,'\0',255);
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
		 memset(backupServerIP,'\0',20);
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
	     if(backupServerPort ==0)
    	         return NASC_FAILURE; 	    

       }

       
       pstCurrVars = pstCurrVars->next_variable;
       /*Get the file transfer interval */
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
       /*Free the respPDU */
       if(pstrespPDU != NULL)
       {
           snmp_free_pdu(pstrespPDU);	       
	   pstrespPDU = NULL;
       }

       return NASC_SUCCESS;
}//

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
    struct snmp_pdu *respPDU = NULL;
    stBackupMaster *pstBMMaster;
    netsnmp_variable_list *vars;
    netsnmp_variable_list *pstCurrVars;
    I16 iCheckTable;

    oid *pstMasID;
    I16 iMasLen; 
    oid *pstMasIDCheck;
    I16 iMasLenCheck; 
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

    pstMasIDCheck = backupMasterID_variables_oid;
    iMasLenCheck  = sizeof(backupMasterID_variables_oid )/ sizeof(oid); 


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


       /*Free the respPDU */
       if(respPDU != NULL)
       {
           snmp_free_pdu(respPDU);	       
	   respPDU = NULL;
       }

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
              if(netsnmp_oid_is_subtree(pstMasIDCheck,
                 iMasLenCheck, pstCurrVars->name, pstCurrVars->name_length))
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
              /*Check if the row status in inactive*/         
	      if(pstBMMaster->iStatus == 2)
	      {
#ifdef BM_DEBUG
                BM_Log("Row status is inactive  continue..\n");
#endif		      
                pstCurrVars = pstCurrVars->next_variable;
                /*Free the master record */
                BM_FreeBackupMaster(pstBMMaster);
                continue;
	      }
               
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
     /*Free the respPDU */
     if(respPDU != NULL)
     {
          snmp_free_pdu(respPDU);	       
          respPDU = NULL;
     }
     return NASC_SUCCESS;
}

/*****************************************************************************
 *
 * name             :   BM_FreeBackupMaster 
 * description      :   Free the Backup Master structure 
 * input parameters :   
 *                      pstBMStruct -> pointer to the stBackupMaster table
 * return type      :   -
 * global variables :   - 
 * calls	    :   -
*****************************************************************************/
void BM_FreeBackupMaster(stBackupMaster *pstBMMaster)
{
      if(pstBMMaster == NULL) 	
      {
          return;	      
      }
      /*Check for other fields */
       if(pstBMMaster->pcMasterID != NULL )
       { 
            free(pstBMMaster->pcMasterID); 
	    pstBMMaster->pcMasterID = NULL;
       }
       if(pstBMMaster->pcMasterName != NULL )
       { 
            free(pstBMMaster->pcMasterName); 
	    pstBMMaster->pcMasterName =NULL;
       }
       if(pstBMMaster->pcStartTime != NULL )
       { 
            free(pstBMMaster->pcStartTime); 
	    pstBMMaster->pcStartTime = NULL; 
       }
       if(pstBMMaster->pcEndTime != NULL )
       { 
            free(pstBMMaster->pcEndTime); 
	    pstBMMaster->pcEndTime = NULL;
       }
       if(pstBMMaster->pcIPAddr != NULL )
       { 
            free(pstBMMaster->pcIPAddr); 
	    pstBMMaster->pcIPAddr = NULL;
       }
       if(pstBMMaster->pcDestPath != NULL )
       { 
            free(pstBMMaster->pcDestPath); 
	    pstBMMaster->pcDestPath = NULL;
       }
       free(pstBMMaster);
       pstBMMaster =NULL;
         
}

/*****************************************************************************
 *
 * name             :   BM_FillStruct 
 * description      :   Allocate memory and memcopy the variable values 
 * input parameters :   
 *                      pcDst -> pointer to the character
 *                      pstCurrVars ->pointer to the netsnmp_variable_list 
 *                      pstBMStruct -> pointer to the stBackupMaster table
 * return type      :   -
 * global variables :   - 
 * calls	    :   -
*****************************************************************************/
I16 BM_FillStruct(UI8  **pcDst,  netsnmp_variable_list *pstCurrVars)
{
        I16 iLen=0;	
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
    
    I16 iResult=0;

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
    if(((pstBMStruct->pcMasterID== NULL) || (pstBMStruct->pcStartTime == NULL)
        || (pstBMStruct->pcEndTime==NULL) || ( pstBMStruct->iInterval== -1)) && (iDecisionFlag != BM_DELETE))
    {
          return  NASC_MANDATORY_MISSING;  	    
    }


    switch(iDecisionFlag)
    {
        case BM_ADD:
             iResult = BM_AddBackupToSched(pstBMStruct); 		
	     if(iResult== NASC_FAILURE)
             {
#ifdef BM_DEBUG
                  BM_Log("Error : Error occured while adding task to scheduler \n");	
#endif
               	  return NASC_FAILURE;   
             }
             break;		
        case BM_MODIFY:
             iResult = BM_ModifyBackupToSched(pstBMStruct); 		
	     if(iResult == NASC_FAILURE)
             {
#ifdef BM_DEBUG
                  BM_Log("Error : Error occured while modifying task to scheduler \n");		     
#endif
               	  return NASC_FAILURE;   
             }
	     
             break;		
        case BM_DELETE:
             iResult = BM_DeteteFromSched(pstBMStruct); 		
	     if(iResult == NASC_FAILURE)
             {
#ifdef BM_DEBUG
                  BM_Log(": No Task Id found \n");		     
#endif
               	  return NASC_FAILURE;   
             }
             break;		
    }	    
    return 1;
}

/*****************************************************************************
 *
 * name             :   BM_ScheduleLogInfo 
 * description      :   Call Scheduler to add the task for log file transfer 
 * input parameters :   
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :   - 
 * calls	    :   BM_Log	
 * 		        SCH_AddTask
 * 		        SCH_DeleteTask 
*****************************************************************************/
I16 BM_ScheduleLogInfo()	   
{

    stTask task; 
    static long lTaskId= -1;
    struct tm stTm={0};
    struct timeval stTv;
    static I16 iInterVal=0;
    I16 iMinute=0;
    I16 iHour =0;

    pthread_mutex_lock(&log_mutex);
    if(backupInterval == iInterVal)
    {
#ifdef BM_DEBUG
    BM_Log(" Interval  matches .. No need to Add the task again \n");
#endif	    
	pthread_mutex_unlock(&log_mutex);
    return NASC_SUCCESS;
    }
    iInterVal= backupInterval;
	pthread_mutex_unlock(&log_mutex);


#ifdef BM_DEBUG
    BM_Log("BM_ScheduleLogInfo... Entering \n");
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
    
    task.fp = BM_CallBackLogFile;

    if(lTaskId == -1)
    {
#ifdef BM_DEBUG
         BM_Log("No need to delete the task from scheduler\n ");	    
#endif
    }else
    {
	  if(SCH_DeleteTask(lTaskId) == NASC_SUCCESS)   
          {
#ifdef BM_DEBUG
         BM_Log("Task has been deleted \n ");	    
#endif
	  }
    }
    /*Add to the scheduler */
    lTaskId = SCH_AddTask(task);           	
#ifdef BM_DEBUG    
    printf("Task ID is %ld \n",lTaskId );
#endif    

    if(lTaskId == -1)
    {
#ifdef BM_DEBUG
         BM_Log("Error occured while adding the task to scheduler \n ");	    
#endif
    }
#ifdef BM_DEBUG
    BM_Log("BM_ScheduleLogInfo... Leaving\n");
#endif
    /*Delete the task ID*/


    /*Add the node in the */
    return NASC_SUCCESS;

}

/*****************************************************************************
 *
 * name             :   BM_CallBackLogFile 
 * description      :   Thread for Sending log file to server 
 * input parameters :   
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :   - 
 * calls	    :   
*****************************************************************************/
void BM_CallBackLogFile(void *args)
{
     pthread_t logThread;	
          
     if((pthread_create(&logThread,NULL,(void*)BM_SendLogFile,NULL)) != 0)
     {
#ifdef BM_DEBUG
         BM_Log("Thread can not be created \n");	   
#endif
	 return;
     }
}

/*****************************************************************************
 *
 * name             :   BM_SendLogFile 
 * description      :   Log file is transfered to log file server 
 * input parameters :   
 * return type      :   - 
 * global variables :   - 
 * calls	    :   
*****************************************************************************/
void BM_SendLogFile()
{
      struct sockaddr_in sock_addr;
      I16 iError;
      I16 iResult;
      char cBuff[16];
       
#ifdef BM_DEBUG
      BM_Log("Calling BM_SendLogFile.. \n"); 
#endif

      //memset(&sock_addr,0,sizeof(struct sockaddr_in));
      sock_addr.sin_family = AF_INET;
      pthread_mutex_lock(&log_mutex);	     
	  if(fpForBM != NULL)
	  {
               fprintf(fpForBM, "%s",charBuff);       	  
               fclose(fpForBM);
               fpForBM = NULL;
	  }
      /*Send the log file to the manager */
      sock_addr.sin_port = htons(backupServerPort);
      memset(cBuff,'\0',16);
      sprintf(cBuff,"%d.%d.%d.%d",(0xff& backupServerIP[0]),(0xff & 
      backupServerIP[1]), (0xff & backupServerIP[2]),(0xff& backupServerIP[3]));
      memset(charBuff, '\0',C_MAX_SIZE); 
	  pthread_mutex_unlock(&log_mutex);	 
	  
      sock_addr.sin_addr.s_addr = inet_addr(cBuff);

      iError = FileTrans_SendFile (backupLogFileName, &sock_addr ,NULL, 512);             
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
              pthread_detach(pthread_self());
              pthread_exit(NULL);
              return;
      }
      /*Add the task to scheduler*/ 
      iResult = BM_ScheduleLogInfo();  
      if(iResult == NASC_FAILURE)
      {
#ifdef BM_DEBUG
         BM_Log("Error in adding the task to scheduler \n");	   
#endif
      }
      /*Open the file decriptor for log file*/
      pthread_mutex_lock(&log_mutex);	        
      fpForBM = fopen(backupLogFileName , "a+");
      if(fpForBM == NULL)
      {
             pthread_mutex_unlock(&log_mutex);	        
             pthread_detach(pthread_self());
             pthread_exit(NULL);
             return;	   
      }
      pthread_mutex_unlock(&log_mutex);	        
#ifdef BM_DEBUG
      BM_Log("Ending BM_SendLogFile.. \n"); 
#endif
      pthread_detach(pthread_self());
      pthread_exit(NULL);
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
    long lTaskId;
    UI32 ulTime;
    struct tm pstTmStart= {0};
    struct tm pstTmEnd ={0};
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
#ifdef BM_DEBUG    
     printf("Day count %d \n",iRetCount);
#endif     

    
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
               task.eWeekDay           =   iRetCount; 
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
    lTaskId = SCH_AddTask(task);           	

#ifdef BM_DEBUG    
    printf("Task id in Add Task %ld \n",lTaskId);
#endif
    if(lTaskId ==0)
    {
#ifdef BM_DEBUG
         BM_Log("Error occured while adding the task to scheduler \n ");	    
#endif
    }
    BM_AddNodeInBackupList(lTaskId ,pstBMStruct->iIndex, stListPtr);
    return NASC_SUCCESS;
}

/*****************************************************************************
 *
 * name             :   BM_FillTimeStruct 
 * description      :   Function fill the tm struct which contains
 *                      hour , minute, sec etc 
 * input parameters :   
 * return type      :   long value equivalent to time 
 * global variables :   - 
 * calls	    :   
*****************************************************************************/
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
#ifndef WIN32
    //pstTm->tm_year += 1900;
#endif
    iTime = mktime(pstTm);
	pstTm->tm_year = iYear;

    return iTime;
}	

/*****************************************************************************
 *
 * name             :   BM_ModifyBackupToSched 
 * description      :   Used to handle the modification of existing records 
 * input parameters :   
 *                      pstBMStruct -> pointer to the stBackupMaster structure
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :   - 
 * calls	    :   BM_AddNodeInBackupList
 *                      BM_GetTaskID
 *                      SCH_GetTask 
 *                      BM_FillTimeStruct
 *                      SCH_ModifyTask
 *                      BM_DeleteNodeFromList
*****************************************************************************/
I16 BM_ModifyBackupToSched(stBackupMaster  *pstBMStruct)
{
    long lTaskId;
    long oldTaskID;
    unsigned long ulTime;
    I16 iDayCount;
    struct tm pstTmStart = {0};
    struct tm pstTmEnd = {0};
    stTask *pstModTask;
    stBackupMaster  *pstBMMod;
    I16 iResult;

    /*Get the task ID from the list*/
    /*To be checked */
    lTaskId = BM_GetTaskID(pstBMStruct->iIndex, stListPtr);
    if(lTaskId == -1)
    {
#ifdef BM_DEBUG
          BM_Log("No task ID found in the List ");		
#endif  
          BM_FreeBackupMaster(pstBMStruct);
          return NASC_FAILURE;
    }
    /*Get the task structure */

    /*Allocate memory for pstModTask struct*/
    pstModTask= malloc(sizeof(stTask));
    
    iResult = SCH_GetTask(lTaskId, &pstModTask);
    if(iResult ==  NASC_FAILURE)
    {
#ifdef BM_DEBUG
        BM_Log("Error in retrieving Task Id from scheduler \n ");	    
#endif
        BM_FreeBackupMaster(pstBMStruct);
	return NASC_FAILURE;
    }
    
    if(pstBMStruct->pcStartTime != NULL) 
    {
	 ulTime =0;   
         /*fill the task structure */
         ulTime = BM_FillTimeStruct(&pstTmStart,pstBMStruct->pcStartTime);  	    
         if(ulTime == 0)
         {
#ifdef BM_DEBUG
               BM_Log("No Start time has been set \n"); 		 
#endif
               BM_FreeBackupMaster(pstBMStruct);
               return NASC_FAILURE; 		  
         }
	 iDayCount= BM_GetDay(ulTime);

#ifdef BM_DEBUG    
	 printf("Day count %d \n",iDayCount);
#endif	 
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
	 ulTime =0;   
         ulTime = BM_FillTimeStruct(&pstTmEnd ,pstBMStruct->pcEndTime);  	    
         if(ulTime == 0)
         {
#ifdef BM_DEBUG
               BM_Log("No End time has been set \n"); 		 
#endif
               BM_FreeBackupMaster(pstBMStruct);
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
                    pstModTask->eWeekDay           =   iDayCount; 
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
    
    pstBMMod->uiEndTimeinNum = pstBMStruct->uiEndTimeinNum;

    if(pstBMStruct->pcMasterID != NULL)
    {
#ifdef BM_DEBUG
	 printf("Old Master ID %s\n",pstBMMod->pcMasterID );
#endif	 
         free(pstBMMod->pcMasterID);
	 
 	 pstBMMod->pcMasterID = malloc(strlen(pstBMStruct->pcMasterID)+1); 
	 memset(pstBMMod->pcMasterID,0,strlen(pstBMStruct->pcMasterID)+1);
	 strcpy(pstBMMod->pcMasterID,pstBMStruct->pcMasterID);

#ifdef BM_DEBUG
	 printf("New Master ID %s\n",pstBMMod->pcMasterID );
#endif	 
    }

    /*Modify the Master name*/
    if(pstBMStruct->pcMasterName != NULL)
    {
	 free(pstBMMod->pcMasterName);
 	 pstBMMod->pcMasterName = malloc(strlen(pstBMStruct->pcMasterName)+1);
	 memset(pstBMMod->pcMasterName,0,strlen(pstBMStruct->pcMasterName)+1);
	 strcpy(pstBMMod->pcMasterName,pstBMStruct->pcMasterName);
    }
     
    /*Modify the IP Address*/
    if(pstBMStruct->pcIPAddr != NULL)
    {
	 free(pstBMMod->pcIPAddr);
 	 pstBMMod->pcIPAddr = malloc(strlen(pstBMStruct->pcIPAddr)+1); 
         memset(pstBMMod->pcIPAddr,0,strlen(pstBMStruct->pcIPAddr)+1); 
	 strcpy(pstBMMod->pcIPAddr,pstBMStruct->pcIPAddr);
    }
    /*Modify the destination path*/ 
    if(pstBMStruct->pcDestPath != NULL)
    {
	 free(pstBMMod->pcDestPath);
 	 pstBMMod->pcDestPath = malloc(strlen(pstBMStruct->pcDestPath)+1);
	 memset(pstBMMod->pcDestPath,0,strlen(pstBMStruct->pcDestPath)+1);
	 strcpy(pstBMMod->pcDestPath,pstBMStruct->pcDestPath);
    }

    
    /*Copy the Start time*/ 
    if(pstBMStruct->pcStartTime != NULL)
    {
	 free(pstBMMod->pcStartTime);
 	 pstBMMod->pcStartTime = malloc(strlen(pstBMStruct->pcStartTime)+1);
	 memset(pstBMMod->pcStartTime,0,strlen(pstBMStruct->pcStartTime)+1);
	 memcpy(pstBMMod->pcStartTime,pstBMStruct->pcStartTime,strlen(pstBMStruct->pcStartTime));
    }

    /*Copy the Finish time*/ 
    if(pstBMStruct->pcEndTime != NULL)
    {
	 free(pstBMMod->pcEndTime);
 	 pstBMMod->pcEndTime = malloc(strlen(pstBMStruct->pcEndTime)+1);
	 memset(pstBMMod->pcEndTime,0,strlen(pstBMStruct->pcEndTime)+1);
	 memcpy(pstBMMod->pcEndTime,pstBMStruct->pcEndTime,strlen(pstBMStruct->pcEndTime));
    }
	    
      pstModTask->vpArg = (void*)pstBMMod;
	 
    pstModTask->fp = BM_PerformBackupAction;
    /*Add to the scheduler */
    oldTaskID = lTaskId;
#ifdef BM_DEBUG    
    printf("Task ID in modified Task %ld \n",oldTaskID);
#endif    

    lTaskId = SCH_ModifyTask(lTaskId, *pstModTask); 
    
    if(lTaskId == -1)
    {
#ifdef BM_DEBUG
         BM_Log("Error occured while Modifying the task to scheduler \n ");	    
#endif
         BM_FreeBackupMaster(pstBMStruct);
	 return NASC_FAILURE;
    }
    

    /*delete the old task from the list*/  	    
    iResult = BM_DeleteNodeFromList(stListPtr,pstBMStruct->iIndex,oldTaskID);
    if(iResult ==0)
    {
#ifdef BM_DEBUG	   
          BM_Log("Error in deleting the task ID from the list \n");   		 
#endif	  
          BM_FreeBackupMaster(pstBMStruct);
          return NASC_FAILURE;

    }else
    {
#ifdef BM_DEBUG	   
          //BM_Log("Node deleted from the list \n");   		 
#endif	  
    }
     
    BM_AddNodeInBackupList(lTaskId ,pstBMStruct->iIndex ,stListPtr);
    BM_FreeBackupMaster(pstBMStruct);
    return NASC_SUCCESS;
}

/*****************************************************************************
 *
 * name             :   BM_GetDay 
 * description      :   Finds a Day which is existing in time 
 * input parameters :   
 * return type      :   Returns the Day ( Sun...Sat) 
 * global variables :   - 
 * calls	    :   BM_GetDayNum 
*****************************************************************************/
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

/*****************************************************************************
 *
 * name             :   BM_GetDayNum 
 * description      :   Get the Number equivalent to Day 
 * input parameters :   
 * return type      :   Returns the Day ( Sun...Sat) 
 * global variables :   - 
 * calls	    :   
*****************************************************************************/
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
    long lDelTaskId;
    UI16 iResult;
    stBackupMaster  *pstBMMod;
    stTask *pstModTask;

    if(pstBMStruct == NULL)
    {
#ifdef BM_DEBUG
         BM_Log("No record found \n");	    
#endif
         return NASC_FAILURE;   				
    }
  
    /*Get the task ID*/ 	    
    lDelTaskId = BM_GetTaskID(pstBMStruct->iIndex,stListPtr );    
    if(lDelTaskId == -1)
    {
         BM_FreeBackupMaster(pstBMStruct);
	 return NASC_FAILURE;
    }


    /*Allocate memory for pstModTask struct*/
    pstModTask= malloc(sizeof(stTask));
    
    iResult = SCH_GetTask(lDelTaskId, &pstModTask);
    if(iResult ==  NASC_FAILURE)
    {
#ifdef BM_DEBUG
        BM_Log("Error in retrieving Task Id from scheduler \n ");	    
#endif
        if(pstModTask != NULL)   
        {
             free(pstModTask);
             pstModTask =NULL;
        }
        BM_FreeBackupMaster(pstBMStruct);
	return NASC_FAILURE;
    }

    pstBMMod = pstModTask->vpArg;   

    iResult = SCH_DeleteTask(lDelTaskId);
    if(iResult ==0)
    {
#ifdef BM_DEBUG
          BM_Log("Error in deleting the task \n");   		 
#endif
          BM_FreeBackupMaster(pstBMStruct);
          return NASC_SUCCESS;
    }else
    {
#ifdef BM_DEBUG
          BM_Log("Node found and Deleted \n");   		 
#endif

    }
    iResult = BM_DeleteNodeFromList(stListPtr,pstBMStruct->iIndex ,lDelTaskId);
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
          BM_FreeBackupMaster(pstBMMod);
          BM_FreeBackupMaster(pstBMStruct);
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
#ifdef WIN32
						break;
#endif				 
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
          case 1: 		  
                 iDecisionFlag = BM_ADD;
                 break;
          case 6:
          case 2:		 
                 iDecisionFlag = BM_DELETE;
                 break;
          default:
                 iDecisionFlag = BM_MODIFY;
                 break;
    }
    
    BM_UpdateBackupToScheduler(pstBMMaster, iDecisionFlag);
    /*Free the respPDU */
    if(respPDU != NULL)
    {
         snmp_free_pdu(respPDU);	       
         respPDU = NULL;
    }
    return ;
}

/*****************************************************************************
 *
 * name             :   BM_GetIndex 
 * description      :   Get the index value (0..65535) from the oid
 * input parameters :   pstCurrVars-> pointer to the snmp_variable_list 
 * return type      :   index 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
I16 BM_GetIndex(netsnmp_variable_list *pstCurrVars)
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
 * name             :   BM_CheckTable  
 * description      :   Function checks the incoming value is subtree or not  
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
 * name             :   BM_CheckField
 * description      :   Function finds the field type 
 * input parameters :   pstSNMPPDU -> Pointer to the netsnmp_variable_list
 * return type      :   iRetValue 
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

/*****************************************************************************
 *
 * name             :   BM_PerformBackupAction
 * description      :   Creates a thread to perform the Backup action 
 * input parameters :   - 
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
void BM_PerformBackupAction(void *args)
{
     pthread_t backupThread;

#ifdef BM_DEBUG
     BM_Log("BM_PerformBackupAction called \n");
#endif

     /*Create a thread to perform the backup action*/
     if((pthread_create(&backupThread,NULL,(void*)BM_SpawnBackup,args)) != 0)
     {
#ifdef BM_DEBUG
         BM_Log("Thread can not be created \n");	   
#endif
	 return;
     }
}

/*****************************************************************************
 * name             :   BM_SpawnBackup 
 * description      :   perform the Backup action 
 * input parameters :   - 
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
I16 BM_SpawnBackup(void *pvBkpPtr)
{
    pthread_t  Thread[5];
    struct snmp_pdu *reqPDU;
    struct snmp_pdu *respPDU=NULL;
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
    oid *pstRowStat ;
    I16 iRowStatus;
    I16 iCnt=0;
    I16 iStatLen = 0 ;
 
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
    UI8 *pcMasterName= NULL;
    UI32 uiCurrentTime;

    /*Fetch the Detail table records  */  
    pstBackup = (stBackupMaster *)pvBkpPtr;
    
     uiCurrentTime = BM_GetCurrentTime(); 
     if(pstBackup->uiEndTimeinNum < uiCurrentTime)
     {
#ifdef BM_DEBUG	     
          BM_Log("Current Time is exceeded the end time \n");	     
#endif	  
          pthread_detach(pthread_self());
          pthread_exit(NULL);
	  return NASC_FAILURE;
     }

     
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

    pstRowStat =  backupDetailStatus;
    iStatLen = sizeof(backupDetailStatus)/ sizeof(oid);
    


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
       snmp_add_null_var(reqPDU,pstRowStat,iStatLen);

       /*Free the respPDU */
       if(respPDU != NULL)
       {
            snmp_free_pdu(respPDU);	       
            respPDU = NULL;
       }

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
           if(respPDU == NULL)	        
	   {
#ifdef BM_DEBUG
                BM_Log("Error in sync response \n");		   
#endif
                pthread_detach(pthread_self());
                pthread_exit(NULL);
		return NASC_FAILURE;
	   }
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
                  pthread_detach(pthread_self());
                  pthread_exit(NULL);
                  return NASC_FAILURE;		   
	   }
       } else 
       {
#ifdef BM_DEBUG
           BM_Log("Error in snmp_synch_response \n");
#endif
           snmp_perror("Error ");   	   
           pthread_detach(pthread_self());
           pthread_exit(NULL);
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
	       if(pstCurrVars == NULL)
	       {
                    pthread_detach(pthread_self());
                    pthread_exit(NULL);
                    return NASC_FAILURE;		       
	       }	       
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

	       if(pstCurrVars == NULL )
	       {	       
                       pthread_detach(pthread_self());
                       pthread_exit(NULL);
		       return NASC_FAILURE;
	       }
	       /*Get the Master ID Name */
	      iResult = BM_FillStruct(&pcMasterName, pstCurrVars);
	      if(iResult == NASC_INSUFFICIENT_MEMORY)
              {  		      
                      pthread_detach(pthread_self());
                      pthread_exit(NULL);
		      return NASC_FAILURE;
	      }

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
	      if(pstCurrVars == NULL )
	      {
                      pthread_detach(pthread_self());
                      pthread_exit(NULL);
		      return NASC_FAILURE;
	      }

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
              if(pcMasterName != NULL)   
              {
                     free(pcMasterName);
                     pcMasterName =NULL;
              }  
	      if((cStrCmpflag == 1) || (cMasterNameFlag ==0))
	      {
#ifdef BM_DEBUG
                   BM_Log(" Master record Name does not match \n"); 		      
#endif
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
                     pstRowStat = pstCurrVars->name;
                     iStatLen= pstCurrVars->name_length;
		      
                     /*Move the Pointer to the next localtion*/  
                     pstCurrVars = pstCurrVars->next_variable;
		     continue;
              }

	      /*Get the File location with path 1*/
	      iResult = BM_FillStruct(&(inputArray[0]), pstCurrVars);
	      if(iResult == NASC_INSUFFICIENT_MEMORY)
	      {
                      pthread_detach(pthread_self());
                      pthread_exit(NULL);
		      return NASC_FAILURE;
	      }
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
	      {
                      pthread_detach(pthread_self());
                      pthread_exit(NULL);
		      return NASC_FAILURE;
	      }
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
	      {
                      pthread_detach(pthread_self());
                      pthread_exit(NULL);
		      return NASC_FAILURE;
	      }
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
	      {
                      pthread_detach(pthread_self());
                      pthread_exit(NULL);
		      return NASC_FAILURE;
	      }
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
	      {
                      pthread_detach(pthread_self());
                      pthread_exit(NULL);
		      return NASC_FAILURE;
	      }
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


	      iRowStatus = 0;
	      /*Check row status is inactive*/ 
              if(pstCurrVars->val.integer != NULL)
                      iRowStatus = (I16)(*pstCurrVars->val.integer);  
              
	      pstRowStat =  pstCurrVars->name;
              iStatLen   =  pstCurrVars->name_length ;  

	      if(iRowStatus  == 2) 
	      {
#ifdef BM_DEBUG
		  printf("\n\n");
                  BM_Log("Received in active record continue \n");
		  printf("\n\n");
#endif		      
		  /*Free the files */
		  for (iCnt=0;iCnt<5;iCnt++)
	          {		  
		          if(inputArray[iCnt] != NULL )
                          { 				  
                             free(inputArray[iCnt]);
			     inputArray[iCnt] = NULL;
			  }
                  }			  
	          /*Move the Pointer to the next localtion*/  
                  pstCurrVars = pstCurrVars->next_variable;
                  continue;		   
	      }	      
	       
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
                         {				 
			      pthread_detach(pthread_self());
			      pthread_exit(NULL);
			      return NASC_FAILURE; 
			 }
			
			 pstBkp->pcDestIP = NULL;
			 pstBkp->pcStartDate = NULL;
			 pstBkp->pcEndDate = NULL;
			 pstBkp->MasterID = NULL;
			 pstBkp->pcMasterName = NULL;
			 pstBkp->pcDestPath = NULL;
				 
			 
			if(BM_FillBackupStruct(&pstBkp ,pstBackup )== NASC_FAILURE)
			{
			      pthread_detach(pthread_self());
			      pthread_exit(NULL);
                              return NASC_FAILURE;				
			}
#ifdef BM_DEBUG
                         BM_Log("pstBackup structure is copied \n"); 		      
#endif
			pstBkp->iMasterIndex = pstBackup->iIndex;

                        pstBkp->pcSourcePath= malloc(strlen(inputArray[iCount])+1);
			if(pstBkp->pcSourcePath == NULL)
			{
			         pthread_detach(pthread_self());
                                 pthread_exit(NULL);
				 return NASC_FAILURE; 
			}
                        memset(pstBkp->pcSourcePath,0,(strlen(inputArray[iCount])+1));
		        strcpy(pstBkp->pcSourcePath,inputArray[iCount]);  
                        
                        /*Free the input Array */    
                        if(inputArray[iCount] != NULL)
                        {
                              free(inputArray[iCount]);
                              inputArray[iCount] = NULL;
                        }
                        if ( (pthread_create(&Thread[iCount],NULL,(void*)BM_DoBackup,
                        (void*)pstBkp)) != 0 )
                        {
                              perror("Thread can not be created \n ");
			      return NASC_FAILURE;
                        }

		   }
              }
       }
       if(iCheckTable == FALSE)
       {
           break; 		   
       }		   
    }
    /*Free the respPDU */
    if(respPDU != NULL)
    {
         snmp_free_pdu(respPDU);	       
         respPDU = NULL;
    }
    pthread_detach(pthread_self());
    pthread_exit(NULL);
    return NASC_SUCCESS;
}

/*****************************************************************************
 * name             :   BM_FreeBackup 
 * description      :   Free the stBackupStruct structure 
 * input parameters :   pstBkp -> poiner to the stBackupStruct structure 
 * return type      :   - 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
void BM_FreeBackup(stBackupStruct *pstBkp)
{

      /*Free the structure */

       if(pstBkp->pcDestIP != NULL )
       { 
            free(pstBkp->pcDestIP); 
	    pstBkp->pcDestIP = NULL;
       }
       if(pstBkp->MasterID != NULL )
       { 
            free(pstBkp->MasterID); 
	    pstBkp->MasterID = NULL;
       }
       if(pstBkp->pcMasterName != NULL )
       { 
            free(pstBkp->pcMasterName); 
	    pstBkp->pcMasterName = NULL;
       }
       if(pstBkp->pcDestPath != NULL )
       { 
            free(pstBkp->pcDestPath); 
	    pstBkp->pcDestPath = NULL;
       }
       if(pstBkp->pcStartDate != NULL )
       { 
            free(pstBkp->pcStartDate); 
	    pstBkp->pcStartDate= NULL;
       }

       if(pstBkp->pcEndDate != NULL )
       { 
            free(pstBkp->pcEndDate); 
	    pstBkp->pcEndDate= NULL;
       }

       if(pstBkp->pcSourcePath != NULL)
       {
            free(pstBkp->pcSourcePath); 
	    pstBkp->pcSourcePath= NULL;

       }	       

       free(pstBkp);
				 
}
/*****************************************************************************
 * name             :   BM_FillBackupStruct 
 * description      :   Fills the modified destination IP, destination path etc
 * input parameters :   - 
 * return type      :   NASC_SUCCESS / NASC_FAILURE 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
I16 BM_FillBackupStruct( stBackupStruct **pstBkp , stBackupMaster *pstBackup )
{
         /*Fill the Destination IP*/
         (*pstBkp)->pcDestIP = malloc((strlen(pstBackup->pcIPAddr))+1);
         if((*pstBkp)->pcDestIP == NULL)
	       return NASC_FAILURE; 
         memset((*pstBkp)->pcDestIP,0,((strlen(pstBackup->pcIPAddr))+1));
         memcpy((*pstBkp)->pcDestIP ,pstBackup->pcIPAddr,strlen(pstBackup->pcIPAddr));

         /*Check the destination path is not null because as it is not mandatory*/    
	 if(pstBackup->pcDestPath != NULL)
	 {
               /*Fill the Destination Path*/
               (*pstBkp)->pcDestPath= malloc(strlen(pstBackup->pcDestPath)+1);
               if((*pstBkp)->pcDestPath == NULL)
	               return NASC_FAILURE; 
               memset((*pstBkp)->pcDestPath,0,(strlen(pstBackup->pcDestPath))+1);
               strcpy((*pstBkp)->pcDestPath,pstBackup->pcDestPath); 
	 }
	 
         /*Fill the Start Date */
         (*pstBkp)->pcStartDate= malloc(strlen(pstBackup->pcStartTime)+1);
         if((*pstBkp)->pcStartDate == NULL)
                return NASC_FAILURE; 
         memset((*pstBkp)->pcStartDate,0,(strlen(pstBackup->pcStartTime))+1);
         memcpy((*pstBkp)->pcStartDate,pstBackup->pcStartTime,strlen(pstBackup->pcStartTime));


	 /*Fill the End Date*/
         (*pstBkp)->pcEndDate = malloc(strlen(pstBackup->pcEndTime)+1);
         if((*pstBkp)->pcEndDate  == NULL)
                return NASC_FAILURE; 
         memset((*pstBkp)->pcEndDate ,0,(strlen(pstBackup->pcEndTime))+1);
         memcpy((*pstBkp)->pcEndDate ,pstBackup->pcEndTime,strlen(pstBackup->pcEndTime));
         
	 /*Fill the Master ID*/
         (*pstBkp)->MasterID = malloc(strlen(pstBackup->pcMasterID)+1);
         if((*pstBkp)->MasterID == NULL)
                return NASC_FAILURE; 
         memset((*pstBkp)->MasterID,0,(strlen(pstBackup->pcMasterID))+1);
         strcpy((*pstBkp)->MasterID,pstBackup->pcMasterID);

             	 
	 /*Fill the Master Name*/
         if(pstBackup->pcMasterName != NULL)  	 
	 {
              (*pstBkp)->pcMasterName= malloc(strlen(pstBackup->pcMasterName)+1);
              if((*pstBkp)->pcMasterName == NULL)
                     return NASC_FAILURE; 
              memset((*pstBkp)->pcMasterName,0,(strlen(pstBackup->pcMasterName))+1);
              strcpy((*pstBkp)->pcMasterName,pstBackup->pcMasterName);
	 }
	
	return NASC_SUCCESS; 
}

/*****************************************************************************
 * name             :   BM_DoBackup 
 * description      :   Fills the modified destination IP, destination path etc
 * input parameters :   - 
 * return type      :   - 
 * global variables :   - 
 * calls	    :   FileTrans_SendFile 
 * 		        
*****************************************************************************/
void BM_DoBackup(void *pvBKP)
{
     I16 iError;
     UI8 localBuff[1000];
     UI8 tempStr[100];
     stBackupStruct *pstBacup;
     struct sockaddr_in sock_addr;
     struct sockaddr_in sock_addr_log;
     UI8 buff[16];
     struct tm pstTmStart = {0};
     UI8 cStartDate[60];
     UI8 cEndDate[60];
     UI8 *pcTempDestPath=NULL;
     UI8 cTempDestPath[1000] ;
     UI8 cTempMasName[60];
     UI32  ulTime;
     UI8 cBuff[16];
     long iTotLen=0;

	 
#ifdef BM_DEBUG    
     static int count =0;
#endif     
     
     memset(&sock_addr,0,sizeof(struct sockaddr_in));
     memset(&sock_addr_log,0,sizeof(struct sockaddr_in));

#ifdef BM_DEBUG    
     printf("BM_ DoBackup  called  %d \n",count++);
#endif     

     /*Check end time is exceeded the current time */ 
      
     pstBacup = (stBackupStruct*)pvBKP ;

#ifdef BM_DEBUG
            BM_Log("BM_DoBackup Called \n");  	      
#endif

     /*Check if Destination IP and source path */
      if((pstBacup->pcDestIP == NULL) || (pstBacup->pcSourcePath == NULL)) 
      {
#ifdef BM_DEBUG
            BM_Log("IP or Path could be null \n");  	      
#endif
	    BM_FreeBackup(pstBacup);
            pthread_detach(pthread_self());
            pthread_exit(NULL);
	    return;
      }

     sock_addr.sin_family = AF_INET;
     sock_addr.sin_port = htons(BACKUP_PORT);
    
	 memset(buff,0,16);
     sprintf(buff,"%d.%d.%d.%d",(0xff& pstBacup->pcDestIP[0]),(0xff & pstBacup->pcDestIP[1]),
         (0xff & pstBacup->pcDestIP[2]),(0xff& pstBacup->pcDestIP[3]));
     
     printf("IP Address in Do_Backup  %s \n ", buff);
     sock_addr.sin_addr.s_addr = inet_addr(buff);


#ifdef BM_DEBUG
     BM_Log("Before Calling FileTrans_SendFile\n"); 		      
#endif	
     if(pstBacup->pcSourcePath != NULL)
     {
#ifdef BM_DEBUG    
        printf("Source Path %s \n",pstBacup->pcSourcePath);
#endif	
     }
     if(pstBacup->pcDestPath != NULL)
     {
#ifdef BM_DEBUG    
        printf("Destination Path %s \n",pstBacup->pcDestPath);
#endif	
     }
     
     /*Copy the Destination path */
     if(pstBacup->pcDestPath != NULL)
     {
         pcTempDestPath =  malloc(strlen(pstBacup->pcDestPath)+1);
	 if(pcTempDestPath == NULL)
         {		 
                 BM_FreeBackup(pstBacup);
		 return;
	 }
        memset(pcTempDestPath,0,(strlen(pstBacup->pcDestPath)+1));
        memcpy(pcTempDestPath,pstBacup->pcDestPath,strlen(pstBacup->pcDestPath));
     }

//      pthread_mutex_lock(&log_mutex);	     
     iError = FileTrans_SendFile (pstBacup->pcSourcePath, &sock_addr ,pcTempDestPath, 512);             
  //    pthread_mutex_unlock(&log_mutex);	     
#ifdef BM_DEBUG
     BM_Log("After  Calling FileTrans_SendFile\n"); 		      
#endif
 
     /*Free the destination path pointer*/ 
     if(pcTempDestPath != NULL)
     {
          free(pcTempDestPath);
          pcTempDestPath = NULL;
     }

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
     
    
if(pstBacup->pcStartDate != NULL)      
{
     ulTime = BM_FillTimeStruct(&pstTmStart,pstBacup->pcStartDate);
     if(ulTime == 0)
     {
#ifdef BM_DEBUG
            BM_Log("No Start time has been set \n");
#endif
     }
}

	     

     memset(cStartDate,0,60);
     /*Add the Date format */
     sprintf(cStartDate, "Year %d Month %d Day %d Hour%d Min%d Sec%d",
     (pstTmStart.tm_year), (pstTmStart.tm_mon +1), pstTmStart.tm_mday,
     pstTmStart.tm_hour ,pstTmStart.tm_min, pstTmStart.tm_sec);

#ifdef BM_DEBUG    
     printf("Start Date %s\n",cStartDate);
#endif     


     if(pstBacup->pcEndDate != NULL)	 
     {	     
            ulTime = BM_FillTimeStruct(&pstTmStart,pstBacup->pcEndDate);
            if(ulTime == 0)
            {
#ifdef BM_DEBUG
                  BM_Log("No End time has been set \n");
#endif
            }
     }

      memset(cEndDate,0,60);
    /*Add the Date format */
     sprintf(cEndDate, "Year %d Month %d Day %d Hour%d Min%d Sec%d",
     (pstTmStart.tm_year), (pstTmStart.tm_mon+1), pstTmStart.tm_mday,
      pstTmStart.tm_hour ,pstTmStart.tm_min, pstTmStart.tm_sec);

     memset(cTempMasName,'\0',60);
      
     /*Check any of the field in null*/
     if(pstBacup->pcMasterName != NULL)
	     memcpy(cTempMasName,pstBacup->pcMasterName,strlen(pstBacup->pcMasterName));
     else
	     strcpy(cTempMasName,"None");
     

     memset(cTempDestPath,'\0',1000);
     /*Check any of the field in null*/ 
     if(pstBacup->pcDestPath!= NULL)
	     memcpy(cTempDestPath,pstBacup->pcDestPath,strlen(pstBacup->pcDestPath));
     else
	     strcpy(cTempDestPath,"None");
     
	     
      sprintf(localBuff,"%s %s %d %s %s %s %s %s \n ",cStartDate,
      cEndDate,pstBacup->iMasterIndex,pstBacup->MasterID,
      cTempMasName,buff,cTempDestPath,tempStr);

      pthread_mutex_lock(&log_mutex);	     
      strcat(charBuff,localBuff); 
      iTotLen= strlen(charBuff);
      pthread_mutex_unlock(&log_mutex);	     

	  
      
#ifdef BM_DEBUG
      printf("Backup : Size of buffer %d \n",strlen(charBuff));
#endif

      if( iTotLen > (C_MAX_SIZE - 200))
      {		
#ifdef BM_DEBUG
            BM_Log(" Size exceeds .. so transfer the file to server \n");
#endif
	      
		   pthread_mutex_lock(&log_mutex);	 
		   if(fpForBM != NULL)
		   {
                       fprintf(fpForBM, "%s",charBuff);       	  
	               memset(charBuff, 0,C_MAX_SIZE); 
	               fclose(fpForBM);
		       fpForBM= NULL;
		   }
		   sock_addr_log.sin_port = htons(backupServerPort);
		   /*Send the file*/
           sock_addr_log.sin_family = AF_INET;
           memset(cBuff,'\0',16);

           sprintf(cBuff,"%d.%d.%d.%d",(0xff& backupServerIP[0]),(0xff & 
           backupServerIP[1]), (0xff & backupServerIP[2]),(0xff& backupServerIP[3]));

           memset(charBuff, '\0',C_MAX_SIZE); 
           sock_addr_log.sin_addr.s_addr = inet_addr(cBuff); 
           pthread_mutex_unlock(&log_mutex);	     

           iError = FileTrans_SendFile (backupLogFileName, &sock_addr_log
          ,NULL, 512);             

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
           
           /*Open the file decriptor for log file*/
           fpForBM = fopen(backupLogFileName , "a+");
           if(fpForBM == NULL)
	   {
               pthread_detach(pthread_self());
               pthread_exit(NULL);
               BM_FreeBackup(pstBacup);
               return;	   
	   }
	   /*Send the log file*/ 
      }
      pthread_mutex_unlock(&log_mutex);	     

#ifdef BM_DEBUG
            BM_Log("BM_DoBackup Returning\n");  	      
#endif
      BM_FreeBackup(pstBacup);
      pthread_detach(pthread_self());
      pthread_exit(NULL);
}

/*****************************************************************************
 * name             :   BM_GetCurrentTime 
 * description      :   Return seconds from 00:00 hours for current day
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :   
 *****************************************************************************/

UI32 BM_GetCurrentTime ()
{
    struct tm stTm = {0};
    struct timeval stTv;
    UI32 iTime;

    gettimeofday(&stTv, (struct timezone *) 0);
    memcpy(&stTm, localtime((time_t *) & stTv.tv_sec), sizeof(stTm));
    iTime = mktime(&stTm);

    return iTime;
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
#if defined(BM_DEBUG) | defined (BM_PRINT)
void BM_Log(UI8 *logMsg)
{

   FILE *ifdForBM;
	
   /*Open the file descriptor for Log file */   
   ifdForBM= fopen("BM_Log_file.log","a+");
   if((ifdForBM == NULL))
   {
       BM_Log("Error in opening the file \n");		   
       return;
   }
   fprintf(ifdForBM,"%s",logMsg );	
   fclose(ifdForBM);

   printf("%s",logMsg);   

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
    I16 iResult;

    stListPtr = BM_MakeEmpty(NULL);

    //snmp_set_do_debugging(1);
    memset(backupLogFileName,'\0',255);

    memset(backupServerIP,'\0',16);

    /*memset the char buff*/  
    memset(charBuff, '\0',C_MAX_SIZE); 
     
    /*Open the session with the NASCenter sub agent*/	
    BM_InitSession();
    if((pthread_create(&thID,NULL,(void*)BM_Read_Packets,NULL)) != 0 )
    {
#ifdef BM_DEBUG
          BM_Log("Thread can not be created \n");	    
#endif	  
	  exit(2);

    }
    iResult = BM_StartBackupProcess();
    if(iResult == NASC_FAILURE)
    {
         SCH_StopScheduler(); 	    
	 exit(0);
    }
  
    //WaitForScheduler();
    pthread_join(thID, NULL);
    return 0;
}

/*****************************************************************************
 *
 * name             :   BM_Read_Packets
 * description      :   Reads the trap message 
 * input parameters :  
 * return type      :   int 
 * global variables :   - 
 * calls	    :   - 
 *                      
*****************************************************************************/

void BM_Read_Packets(void * arg)
{
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
#ifndef WIN32
             if(errno == EBADF)
	     {
#ifdef BM_DEBUG
                   BM_Log("An invalid file descriptor \n ");
#endif
	     }else if(errno == EINTR)
	     {
#ifdef BM_DEBUG
                 BM_Log("A non blocked signal was caught \n");		     
#endif
	     }else if(errno == ENOMEM)		     
	     {
#ifdef BM_DEBUG
                 BM_Log("Select was unable to allocate memory \n");		     
#endif
	     }
#endif
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


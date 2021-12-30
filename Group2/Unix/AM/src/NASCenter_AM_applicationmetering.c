/*****************************************************************************
 *
 * NASCenter_AM_applicationmetering.c
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved777
 *
 * author:
 *      Karthikeyan Subramani.
 *
 * description :
 *      
 *      
 *
 * TODO:
 *      1. 
 *      
 *      2. 
 *
 * CM Details:
 *
 * $Author: karthis $
 * $Log: NASCenter_AM_applicationmetering.c,v $
 * Revision 1.46  2004/01/29 09:38:16  karthis
 * snmp_free_PDU for snmp sunc response included
 *
 * Revision 1.45  2004/01/14 08:36:06  karthis
 * memory leak fixed
 *
 * Revision 1.44  2004/01/11 07:28:00  karthis
 * Reaction ==1 is included
 *
 * Revision 1.43  2004/01/11 07:12:39  karthis
 * memset updated for username memory allocation in checkproc funtion for all respective OS
 *
 * Revision 1.42  2004/01/06 11:31:14  baravind
 * 1sec to 60 sec TRU64
 *
 * Revision 1.41  2004/01/06 11:29:35  baravind
 * Added flag to List(stapp,int) TRU64
 *
 * Revision 1.40  2004/01/05 11:47:48  baravind
 * printf APPMET comes under AM_DEBUG FLAG
 *
 * Revision 1.39  2004/01/05 10:43:51  karthis
 * linux procmon changed to Procmon
 *
 * Revision 1.38  2004/01/02 08:32:42  baravind
 * strstr changed to strcmp TRU64
 *
 * Revision 1.37  2004/01/02 07:59:06  baravind
 * TRU64 Sleep 1sec changed to sleep 60sec
 *
 * Revision 1.36  2004/01/02 07:55:31  baravind
 * TRU64 return -1 changed to return Reaction
 *
 * Revision 1.35  2004/01/02 07:47:12  baravind
 * oid/sizeof(int) changed to oid/sizeof(oid) TRU64
 *
 * Revision 1.34  2004/01/02 04:51:16  karthis
 * #define TRAPPORT included
 *
 * Revision 1.33  2004/01/02 04:20:18  karthis
 * index== included solaris
 *
 * Revision 1.32  2004/01/01 11:41:02  karthis
 * BUFSIze for solaris has been changed to BUFSIZE line no 856
 *
 * Revision 1.31  2004/01/01 11:06:43  karthis
 * code review comments updated
 *
 * Revision 1.30  2003/12/31 12:43:40  rajesh
 * Fix done for TRU64
 *
 * Revision 1.29  2003/12/31 04:54:57  karthis
 * compilation error fixed for linux
 *
 * Revision 1.28  2003/12/29 09:53:31  karthis
 * removed most of the printf for code review
 *
 * Revision 1.27  2003/12/29 05:08:14  karthis
 * Debug flag AM_DEBUG has been included karthi
 *
 * Revision 1.25  2003/12/24 16:55:20  karthis
 * updating sleep to 60 seconds
 *
 * Revision 1.24  2003/12/24 15:34:21  karthis
 * Index initialized to zero
 *
 * Revision 1.23  2003/12/24 10:56:38  karthis
 * checkproc error fixed for HPUX
 *
 * Revision 1.22  2003/12/24 04:54:54  karthis
 * Row created segmentation fault fixed
 *
 * Revision 1.21  2003/12/23 12:10:42  karthis
 * Modified Solaris
 *
 * Revision 1.19  2003/12/23 11:22:12  karthis
 * Included AM in unixware
 *
 * Revision 1.18  2003/12/23 07:16:17  karthis
 * Systemtesting bug fixed for solaris
 *
 * Revision 1.17  2003/12/23 06:28:10  karthis
 * os ifdef and system testing bug fixed
 *
 * Revision 1.15  2003/12/19 03:49:11  testteam
 * modified link list
 *
 * Revision 1.14  2003/12/18 09:49:51  testteam
 *
 * minor modification kill command added comments removed
 *
 * Revision 1.13  2003/12/18 09:02:00  testteam
 * Link List Modified
 *
 * rEvision 1.10  2003/12/16 16:26:07  testteam
 * final module with out logfile module
 *
 * Revision 1.9  2003/12/16 16:01:17  testteam
 * Alert section Updated
 *
 * Revision 1.8  2003/12/16 14:31:29  testteam
 * updated structure successfully
 *
 * Revision 1.7  2003/12/16 13:01:17  testteam
 * structure updated for logfile
 *
 * Revision 1.6  2003/12/16 09:07:10  testteam
 * calling Modify link list function successfully for Linux
 *
 * Revision 1.5  2003/12/16 08:40:27  testteam
 * Added Agentinfo and Threads
 *
 * Revision 1.4  2003/11/20 16:03:06  karthis
 * major change has been upgraded
 *
 * Revision 1.9  2003/11/18 11:34:50  
 * 18th November
 *
 * Revision 1.2  2003/11/01 12:19:34  
 * Application metering source files
 *
 * 
 * 
 * $Id: NASCenter_AM_applicationmetering.c,v 1.46 2004/01/29 09:38:16 karthis Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/
/********************************* includes *********************************/
#include <stdio.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/session_api.h>
#include "appMeteringTable.h"
#include "AgentInfo.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include "ctypes.h"
#include "NASCenter_AM_applicationmetering.h"
#include "NASCenter_AlertMessage.h"
#include "NASCenter_Common.h"
#include "NASCenter_Errors.h"
#include <signal.h>
#include <fcntl.h>

#if defined(osf5) 
#include <sys/table.h>
#include <sys/user.h>
#include <unistd.h>
#endif
 

/************************ globals within this module*************************/

UI8 gcNASSACommunity[]="public";
UI8 gcAgentSessPeerName[] = "localhost";
I16 giNASSAPort=400;
#define IPADDRLEN 16
#define MAXPATHLEN 1024
#define TRAPPORT "2503"
Stapp *Appmov,*node,*Head=NULL,*prev,*Save;
Stappsub *newsub,*prevsubmove,*Submove,*gpsub;


	
/********************* prototypes for local functions ***********************/
void AM_ApplicationMetering();
I16 AM_GetAppMetTable(void);
void AM_StartApplicationMetering(void);
I16 AM_DoReaction(void);
I16 AM_SetAppMetTable(struct stappMeteringStruct );
void AM_InitSession();
void AM_Read_Packets(void *);
I16 AM_GetAgentInfoTable();
int ProcMonCheckProc(StappMeteringStruct *process);  
void *tSess;
void List(StappMeteringStruct *pappMet,int);
int ProcMon_SendMesg(char *pcMesg);
void SearchList(StappMeteringStruct *pAMeter,Stapp* Save);


/************************************************************
* MAIN FUCNTION
* description      : Main Function 
* input parameters :   None
*                      
* return type      :   int
* global variables :   -
* calls            :   
*************************************************************/

int  main()
{ 

	AM_StartApplicationMetering();
	return 1;
		
}

/*****************************************************************************
 *
 * name             :   AM_StartApplication Metering
 * description      :   Here a SNMP session is opened and communication 
                        with the NASCenter Subagent initialized. 
						The configuration MIB information for the application 
						metering table is retrieved and stored in memory for the 
						application monitoring. A separate thread is created to
						do the actual application monitoring. 
 * input parameters :   None
 *                      
 * return type      :   None 
 * global variables :   -
 * calls            :   AM_InitSession,AM_GetAppMetTable,AM_GetAgentInfoTable
						AM_init_CBuffer
 *****************************************************************************/


void AM_StartApplicationMetering()
{
	pthread_t thID;
	void * pstatus;
	pthread_t stlogthread;
	pthread_t stappmonthread;
	pthread_t sttrapthread;


	I16 iretval=0;
	
   /*Open the file descriptor for Log file */   
   ifdForAM= fopen("AM_LOG_FILE","a+"); /* Remove this */
   if((ifdForAM == NULL))
   {
    #ifdef AM_DEBUG
       printf("Error in opening the file \n");		   	   
    #endif
       return;
   }	
	
   /*Session Initiation with the nascenter agent*/
	AM_InitSession();     

	/*Register callback for TRAP from the agent */
	if((pthread_create(&thID,NULL,(void*)AM_Read_Packets,NULL)) != 0 )
    {
#ifdef AM_DEBUG
		printf("Thread can not be created \n");	    
#endif
		return;
    }
	
	
	/*Retrieve the MIB information */
	iretval=AM_GetAppMetTable();
    if(iretval == NASC_FAILURE)
	{
    #ifdef AM_DEBUG
		printf("Error while executing\n");
    #endif
           	return;
	}
#ifdef AM_DEBUG
         Display(Head);
#endif
	/*Retrieve the agent information MIB information */
	iretval=AM_GetAgentInfoTable();
    if(iretval==1)
	{
		AM_InitLogView(&stLogView);
	pthread_create(&stlogthread , '\0' , (void *) AM_SendLogFile , NULL);		
	}   
 
	/*Create a circular buffer */
	AM_init_CBuffer(&stCB);	
	
	pthread_create(&sttrapthread , '\0' , (void *) AM_ProcessTrapMsg , (void *) &stCB);
        while(1)
       {
    	AM_DoReaction();
        sleep(60);
       }
	FreeHead(Head);
}



/*****************************************************************************
 *
 * name             :   AM_GetAppMetTable
 * description      :   This function will get the appMetering table info and 
                        store it in the local structure appMeteringStruct.
 * input parameters :   
 *                      
 * return type      :   0 ' if success
						-1 ' if failed

 * global variables :   agentSess
 * calls            :   
 *****************************************************************************/
I16 AM_GetAppMetTable()
{
    I16 i=0;
    struct snmp_pdu *pstreqPDU;
    struct snmp_pdu *pstrespPDU;
    struct variable_list *pstvars;
	
    I16 iCheckTable;
    I16 iProcessCount;
    StappMeteringStruct *pstappMet;
    I16 istatus;
    I8 *pcbuf;
    oid *nextID;
    I32 nextIDLen;
    oid *nextStartTime;
    I32 nextStartTimeLen;
    oid *nextEndTime;
    I32 nextEndTimeLen;
    oid *nextReaction;
    I32 nextReactionLen;
    oid *nextBinaryName;
    I32 nextBinaryNameLen;
    oid *nextalert;
    I32 nextalertLen;
    oid *nextstatus;
    I32 nextstatusLen;
    I32 iLen;
    int ii=0;
	
    
		pstreqPDU = snmp_pdu_create(SNMP_MSG_GETBULK);
		pstreqPDU->non_repeaters = 0;
		pstreqPDU->max_repetitions = MAX_REPETATION;
		
       /*For the PDU with relavant fields*/		
		

		iLen=sizeof(appMeteringID_variables_oid)/sizeof(oid);
		snmp_add_null_var(pstreqPDU,appMeteringID_variables_oid,iLen);		 
		
		
		iLen=sizeof(appMeteringStartTime_variables_oid)/sizeof(oid);
		snmp_add_null_var(pstreqPDU,appMeteringStartTime_variables_oid,iLen);
		                          
		
		iLen=sizeof(appMeteringEndTime_variables_oid)/sizeof(oid);
		snmp_add_null_var(pstreqPDU,appMeteringEndTime_variables_oid,iLen);
		
		iLen=sizeof(appMeteringReaction_variables_oid)/sizeof(oid);
		snmp_add_null_var(pstreqPDU,appMeteringReaction_variables_oid,iLen);
		
		
		iLen=sizeof(appMeteringBinaryName_variables_oid)/sizeof(oid);
		snmp_add_null_var(pstreqPDU,appMeteringBinaryName_variables_oid,iLen);
		
		
		iLen=sizeof(appMeteringAlertMessage_variables_oid)/sizeof(oid);
		snmp_add_null_var(pstreqPDU,appMeteringAlertMessage_variables_oid,iLen);

		iLen=sizeof(appMeteringStatus_variables_oid)/sizeof(oid);
		snmp_add_null_var(pstreqPDU,appMeteringStatus_variables_oid,iLen);

	
	while(1)
	{

		istatus = snmp_synch_response(agentSess, pstreqPDU, &pstrespPDU);

		if(istatus!=STAT_SUCCESS)
		{
			printf("Error in snmp_synch_response \n ");	       
			return NASC_FAILURE;
		}
                if(istatus==STAT_SUCCESS)
		{
                #ifdef AM_DEBUG
			printf("Sync Response for APPMET Success\n");
                #endif
                }
	/*Flag for checkiing whether the fetched table details belongs to 
	* required table*/ 
		pstvars = pstrespPDU->variables;
		iCheckTable = TRUE;
		
		for(iProcessCount =0; iProcessCount < MAX_REPETATION;
             iProcessCount++ )
		{
                    if ( pstvars->type == SNMP_NOSUCHOBJECT ||
                     pstvars->type == ASN_NULL )
                {
                    return NASC_FAILURE;
                }

                if ( pstvars->type == SNMP_ENDOFMIBVIEW)
                {
                    return NASC_FAILURE;
                }


			iLen=sizeof(appMeteringID_variables_oid)/sizeof(oid);
			if(netsnmp_oid_is_subtree(appMeteringID_variables_oid,iLen,
							pstvars->name, pstvars->name_length))
			{
				iCheckTable = FALSE;
                break;
			}
            #ifdef AM_DEBUG
			printf("Index : %d\n" , pstvars->name[13]);
            #endif
	                pstappMet=(StappMeteringStruct *)malloc(sizeof(StappMeteringStruct));
			if(pstappMet == NULL)
			{
				AM_AddMsgToLogfile("Memory allocation failed \n "); 			
				return NASC_INSUFFICIENT_MEMORY;
			}

			pstappMet->uiappMeteringIndex=0;	
			pstappMet->pcappMeteringID=NULL;
			pstappMet->ulappMeteringStartTime=0;
			pstappMet->ulappMeteringEndTime=0;
			pstappMet->iappMeteringReaction=0;
			pstappMet->pcappMeteringUsername=NULL;
			pstappMet->pcappMeteringBinaryName=NULL;
			pstappMet->pcalertmessage=NULL;
			pstappMet->istatus=0;	

			 
			if(pstvars->val_len!=0)
		        {
                    #ifdef AM_DEBUG
                         printf("val_len = %d\n",pstvars->val_len);
                    #endif
				pstappMet->pcappMeteringID=(char *)malloc(pstvars->val_len+1);
                                memset(pstappMet->pcappMeteringID,'\0',pstvars->val_len+1);
				strncpy(pstappMet->pcappMeteringID,pstvars->val.string,pstvars->val_len);
                        #ifdef AM_DEBUG
                        printf("val string %s\n",pstvars->val.string);
                        #endif
			}
				nextID        = pstvars->name;
				nextIDLen     = pstvars->name_length;			
				pstvars = pstvars->next_variable;
				pstappMet->uiappMeteringIndex=pstvars->name[--pstvars->name_length];
			
			if(pstvars->val_len!=0)
			{
				pcbuf = (I8 *) malloc(pstvars->val_len+1);					
				if(pcbuf==NULL)
					return NASC_INSUFFICIENT_MEMORY;			
                                memset(pcbuf,'\0',pstvars->val_len+1);
			        memcpy(pcbuf,pstvars->val.string,pstvars->val_len);				
			        Util_Convert_Date_To_Long(pcbuf,&(pstappMet->ulappMeteringStartTime));
			        free(pcbuf);
			}
			nextStartTime        = pstvars->name;
                        nextStartTimeLen     = pstvars->name_length;
			pstvars = pstvars->next_variable;

			if(pstvars->val_len!=0)
			{
				pcbuf = (I8 *) malloc(pstvars->val_len+1);					
				if(pcbuf==NULL)
					return NASC_INSUFFICIENT_MEMORY;

                                memset(pcbuf,'\0',pstvars->val_len+1);
				memcpy(pcbuf,pstvars->val.string,pstvars->val_len);				
				Util_Convert_Date_To_Long(pcbuf,&(pstappMet->ulappMeteringEndTime));
				free(pcbuf);
			}
			nextEndTime        = pstvars->name;
                        nextEndTimeLen     = pstvars->name_length;			
			pstvars = pstvars->next_variable;

			
			if(pstvars->val_len!=0)
		        pstappMet->iappMeteringReaction=(I16)(*pstvars->val.integer);
			nextReaction        = pstvars->name;
                        nextReactionLen     = pstvars->name_length;
			pstvars = pstvars->next_variable;

			if(pstvars->val_len!=0)
			{
				pstappMet->pcappMeteringBinaryName=(I8 *)malloc(pstvars->val_len+1);
                                memset(pstappMet->pcappMeteringBinaryName,'\0',pstvars->val_len+1);
				strncpy(pstappMet->pcappMeteringBinaryName,pstvars->val.string,pstvars->val_len);
			}
					
			
			nextBinaryName        = pstvars->name;
                        nextBinaryNameLen     = pstvars->name_length;
			pstvars = pstvars->next_variable;

			
			if(pstvars->val_len!=0)
			{
				pstappMet->pcalertmessage=(I8 *)malloc(pstvars->val_len+1);
                                memset(pstappMet->pcalertmessage,'\0',pstvars->val_len+1);
				strncpy(pstappMet->pcalertmessage,pstvars->val.string,pstvars->val_len+1);
			}
			nextalert        = pstvars->name;
                        nextalertLen     = pstvars->name_length;
			pstvars = pstvars->next_variable;

			pstappMet->istatus=(I16)(*pstvars->val.integer);
			nextstatus		= pstvars->name;
			nextstatusLen	= pstvars->name_length;
			pstvars = pstvars->next_variable;


                        if(pstappMet->istatus!=2)
                       {
                        List(pstappMet,0);
                       }
                       



                        if (pstappMet->pcappMeteringID)
                        {
                             free(pstappMet->pcappMeteringID);
                             pstappMet->pcappMeteringID=NULL;
                        }

                        if (pstappMet->pcappMeteringUsername)
                        {
                             free(pstappMet->pcappMeteringUsername);
                             pstappMet->pcappMeteringUsername=NULL;
                        }
		
                        if (pstappMet->pcappMeteringBinaryName)
                        {
                             free(pstappMet->pcappMeteringBinaryName);
                             pstappMet->pcappMeteringBinaryName=NULL;
                        }

                        if (pstappMet->pcalertmessage)
                        {
                             free(pstappMet->pcalertmessage);
                             pstappMet->pcalertmessage=NULL;
                        }

                        if (pstappMet)
                        {
                             free(pstappMet);
                             pstappMet=NULL;
                        }

		}
		if(iCheckTable == FALSE)
        {
             if(pstrespPDU != NULL)
             {
                 snmp_free_pdu(pstrespPDU);  
                 pstrespPDU =NULL;
             }
    		break;
        }

		pstreqPDU = snmp_pdu_create(SNMP_MSG_GETBULK);
		pstreqPDU->non_repeaters = 0;
		pstreqPDU->max_repetitions = MAX_REPETATION;

      
		
		snmp_add_null_var(pstreqPDU,nextID,nextIDLen);		 		
		snmp_add_null_var(pstreqPDU,nextStartTime,nextStartTimeLen);                  
		snmp_add_null_var(pstreqPDU,nextEndTime,nextEndTimeLen);	
		snmp_add_null_var(pstreqPDU,nextReaction,nextReactionLen);	
		snmp_add_null_var(pstreqPDU,nextBinaryName,nextBinaryNameLen);		
		snmp_add_null_var(pstreqPDU,nextalert,nextalertLen);
		snmp_add_null_var(pstreqPDU,nextstatus,nextstatusLen); 

        if(pstrespPDU != NULL)
        {
             snmp_free_pdu(pstrespPDU);  
             pstrespPDU =NULL;
        }

	}/*End of While */ 
	return NASC_SUCCESS;  

}


/*****************************************************************************
 *
 * name             :   AM_GetAgentInfoTable
 * description      :   This function will get the Agent info table and store 
                        the ManagerIp address, ManagerPort, Transferinterval 
						and the logfilename in global variables
 * input parameters :   Session sessionid;
 *                      
 * return type      :   0 ' if success
						-1 ' if failed
 * global variables :   agentSess
 * calls            :   
 *****************************************************************************/
I16 AM_GetAgentInfoTable()
{

	struct snmp_pdu *pstreqPDU;
    struct snmp_pdu *pstrespPDU;
    netsnmp_variable_list *pstCurrVars;
    char FileTransferIP[IPADDRLEN];

    I16 status=0;
    I16 iLen=0;   

    
               
	pstreqPDU = snmp_pdu_create(SNMP_MSG_GET);


       
    /*Get the Relevant Fields*/
	/*Alert IP*/
	iLen=sizeof( agentInfoAlertManagerIP_oid)/sizeof(oid); 
    snmp_add_null_var(pstreqPDU,agentInfoAlertManagerIP_oid,iLen);

	/*Alert port no */
	iLen=sizeof(agentInfoAlertManagerPortNo_oid)/sizeof(oid); 
    snmp_add_null_var(pstreqPDU,agentInfoAlertManagerPortNo_oid,iLen);
    
       /*Get the IP Address */
	iLen=sizeof( agentInfoFileTransferIP_oid)/sizeof(oid); 
    snmp_add_null_var(pstreqPDU,agentInfoFileTransferIP_oid,iLen);
      
      /*Get the File Tranfer Interval */ 
    iLen=sizeof(agentInfoFileTransferInterval_oid)/sizeof(oid); 
    snmp_add_null_var(pstreqPDU,agentInfoFileTransferInterval_oid,iLen);

       /*Get the port number*/
    iLen=sizeof(agentInfoFileTransferPortNo_oid)/sizeof(oid); 
    snmp_add_null_var(pstreqPDU,agentInfoFileTransferPortNo_oid,iLen);

       /*Get the Log File Name*/
    iLen=sizeof(agentInfoLogFileAppMetering_oid)/sizeof(oid);
    snmp_add_null_var(pstreqPDU,agentInfoLogFileAppMetering_oid,iLen);

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
            #ifdef AM_DEBUG
	          printf("sync response success \n");	   
            #endif
		}else
		{
            #ifdef AM_DEBUG
	          printf("sync response failure \n");	   
            #endif
                  return NASC_FAILURE;		   
		}
        if(pstrespPDU == NULL)	        
		{
            #ifdef AM_DEBUG
			printf("Error in sync response \n");		   
            #endif
			return NASC_FAILURE;
		}
	} else 
    {
        #ifdef AM_DEBUG
		printf("Error in snmp_synch_response \n");
        #endif
        snmp_perror("Error ");   	   
        return NASC_FAILURE;		   
    }

    pstCurrVars = pstrespPDU->variables;

	/*Get the Alert IP Address*/
    iLen =0;
	if(pstCurrVars->val.string!=NULL)
	{
		iLen =  pstCurrVars->val_len;
		memcpy(FileTransferIP,pstCurrVars->val.string,iLen);
		Util_GetIPFrom_ASNFormat(FileTransferIP,gstagentInfo.pcAlertIpAddr);
		pstCurrVars = pstCurrVars->next_variable;
	}
	else
		return NASC_FAILURE;

	/*Get the port number */
	if(pstCurrVars->val.integer!=NULL)
	{
		gstagentInfo.iAlertPort=(I16)(*pstCurrVars->val.integer);
		pstCurrVars = pstCurrVars->next_variable;
	}
	else
		return NASC_FAILURE;

	/*Get the IP Address*/
    iLen =0;
	if(pstCurrVars->val.string!=NULL)
	{
		iLen =  pstCurrVars->val_len;
		memcpy(FileTransferIP,pstCurrVars->val.string,iLen);
		Util_GetIPFrom_ASNFormat(FileTransferIP,gstagentInfo.pcMgrIpAddr);
		pstCurrVars = pstCurrVars->next_variable;
	}
	else
		return NASC_FAILURE;

    /*Get the File Tranfer Interval */ 
	if(pstCurrVars->val.integer!=NULL)
	{
		gstagentInfo.iTransferInterval =(I16)(*pstCurrVars->val.integer);
		pstCurrVars = pstCurrVars->next_variable;
	}
	else
		return NASC_FAILURE;
       
    /*Get the port number */
	if(pstCurrVars->val.integer!=NULL)
	{
		gstagentInfo.iMgrPort=(I16)(*pstCurrVars->val.integer);
		pstCurrVars = pstCurrVars->next_variable;
	}
	else
		return NASC_FAILURE;

    /*Get the backup Log file */
    iLen =0;
	if(pstCurrVars->val.string!=NULL)
	{
		iLen =  pstCurrVars->val_len;
		memcpy(gstagentInfo.pclogfilename,pstCurrVars->val.string,iLen);       
	}
	else
		return NASC_FAILURE;
#ifdef AM_DEBUG
         printf("IPAddress :%s\n",gstagentInfo.pcMgrIpAddr);
         printf("MAnger Port : %d\n",gstagentInfo.iMgrPort);
         printf("interval : %d\n",gstagentInfo.iTransferInterval);
         printf("Filename : %s\n",gstagentInfo.pclogfilename);
         printf("Alert Ip Address: %s\n",gstagentInfo.pcAlertIpAddr);
         printf("Port No:%d\n",gstagentInfo.iAlertPort);
#endif

      if (pstrespPDU != NULL)
      {
           snmp_free_pdu(pstrespPDU);
           pstrespPDU = NULL;
      }
return NASC_SUCCESS;
}

/*****************************************************************************
 *
 * name             :   ProcMon_CheckProc
 * description      :   This function is used to send the alert mesg to the
 *                      manager
 * input parameters :   char *name- Process Name
 *                      char *mesg - Alert message
 * output parameters:   None
 * return type      :   int - returns success (0) or failure (1)
 * global variables :   None
 * calls            :   None
 */
#if defined(sysv4)
#include <sys/procfs.h>
int
ProcMonCheckProc (StappMeteringStruct *process)
{
    int Reaction=0;
    char acdata[BUFSIZE];
    char ip[100];
    char port[100];
    char binname[100];
    char alert[100];
    I8 actempbuf[BUFSIZE];
   
    DIR                 *dirstream = {0};
    struct dirent       *direntry = NULL;
    struct psinfo       pInfo = {0};
    char                procBuff[MAXPATHLEN+1]="";
    int                 procFD = 0 ;
    int                 nread = 0;
    int                 psInfoSize =sizeof(struct psinfo);
    struct passwd *pwd = NULL;


    // Open proc system
    dirstream = opendir("/proc");

    if ( dirstream )
    
    // Loop until we find the process
    while((direntry = readdir(dirstream))!= '\0')
    {
        if(!strcmp(direntry->d_name,".") || !strcmp(direntry->d_name,"..") ||
                        !strcmp(direntry->d_name,"0"))
        {
            continue;
        }

        sprintf(procBuff,"/proc/%d/psinfo",atoi(direntry->d_name));
        procFD = open(procBuff,O_RDONLY);

        if(procFD<0)
        {
            continue;
        }

      
          nread = read(procFD,&pInfo,psInfoSize);

        if(nread!=psInfoSize)
        {
            close(procFD);
            continue;
        }
        close(procFD);
	if (strcmp(pInfo.pr_fname, process->pcappMeteringBinaryName) == 0)
    {
                sprintf(actempbuf,"%d",APPMET);
                strcpy(binname,process->pcappMeteringBinaryName);

                sprintf(ip,"*****************************IP<%s>************",
                gstagentInfo.pcAlertIpAddr);
                sprintf(port,"***********************port<%d>****************"
                ,gstagentInfo.iAlertPort);
                if(process->pcalertmessage)
                strcpy(alert,process->pcalertmessage);
                sprintf(acdata,"%s %s %s %s",binname,ip,port,alert);
                if(process->iappMeteringReaction==0)
                {
                        Reaction=1;
                        ProcMon_SendMesg(acdata);
                    #ifdef AM_DEBUG
                        printf("******%s******\n",process->pcalertmessage);
                    #endif
                }
                if(process->iappMeteringReaction==1)
                {
                     Reaction=1;
                     kill(pInfo.pr_pid,SIGTERM);
         		}
                if(process->iappMeteringReaction==2)
                {
                     Reaction=1;
                     kill(pInfo.pr_pid,SIGTERM);
                     ProcMon_SendMesg(acdata);
   	             }

     }

}
            closedir(dirstream);
            close(procFD);
      if(Reaction==1) 
      pwd=getpwuid(pInfo.pr_uid);
      if(pwd)
     {
#ifdef AM_DEBUG
        printf("user who started %s\n" , pwd->pw_name);
#endif
        process->pcappMeteringUsername= ( I8 *)malloc(strlen(pwd->pw_name)+1);

        if(process->pcappMeteringUsername)
        {
            memset(process->pcappMeteringUsername,'\0',strlen(pwd->pw_name)+1);
            strcpy(process->pcappMeteringUsername,pwd->pw_name);
        }
    }

    return Reaction;
}
#endif /* sysv4 */





/*****************************************************************************
 *
 * name             :   ProcMonCheckProc
 * description      :   This function is used to send the alert mesg to the
 *                      manager
 * input parameters :   StappMeteringStruct *process
 *                      
 * output parameters:   None
 * return type      :   int - returns success (0) or failure (1)
 * global variables :   None
 * calls            :   None
 */

#if defined(solaris2)  // Check proc for solaris
#include <procfs.h>
#include <fcntl.h>
int
ProcMonCheckProc(StappMeteringStruct *process)
{
    long iPid=-1;
    int Reaction=0;
    char acdata[BUFSIZE];
    char ip[100];
    char port[100];
    char binname[100];
    char alert[100];
    char actempbuf[BUFSIZE];

#if _SLASH_PROC_METHOD_
    psinfo_t stPsInfo;
    DIR                 *pstProcDir;
    struct dirent       *pstDirEnt;
    int                 iProcFd;
    char                acProcFn[sizeof "/proc/00000/psinfo"];
    struct passwd       *pstUserInfo=NULL;

    #ifdef AM_DEBUG
        printf("check_proc_solaris called\n");
    #endif
   process->pcappMeteringUsername=NULL;
    if ((pstProcDir = opendir ((const char*)"/proc")) == NULL)
    {
        perror ("Unable to open /proc dir\n");
        return (0);
    }
 
    while (((pstDirEnt = readdir (pstProcDir)) != NULL) )
    {
        if ((!strcmp (pstDirEnt->d_name, ".")) || (!strcmp (pstDirEnt->d_name, "..")))
        {
            continue;
        }

        iPid = atoi(pstDirEnt->d_name);

        sprintf(acProcFn, "/proc/%.5d/psinfo", iPid);

        if ((iProcFd = open(acProcFn, O_RDONLY)) != -1)
        {

            if (read(iProcFd, &stPsInfo, sizeof(stPsInfo)) !=
                                        sizeof(stPsInfo))
            {
                perror("Error while read /proc");
                close(iProcFd);
                closedir(pstProcDir);
                return(0);
            }
            close(iProcFd);
        }
        if ( iPid == -1 )
             return(0);

          if ( strcmp(stPsInfo.pr_fname,process->pcappMeteringBinaryName)==0)
          {
                    // Do the action here which is specified for
                    // this process

		    sprintf(actempbuf,"%d",APPMET);
		    strcpy(binname,process->pcappMeteringBinaryName);
            sprintf(ip,"*********IP<%s>**********",gstagentInfo.pcAlertIpAddr);
            sprintf(port,"*******port<%d>*****",gstagentInfo.iAlertPort);
            if(process->pcalertmessage)
            strcpy(alert,process->pcalertmessage);
            sprintf(acdata,"%s %s %s %s",binname,ip,port,alert);  
    	    if(process->iappMeteringReaction==0)
		    {			
		            Reaction=1;
                            ProcMon_SendMesg(acdata);
                    #ifdef AM_DEBUG
                        printf("****%s*****\n",process->pcalertmessage);
                    #endif
		    }
		    if(process->iappMeteringReaction==1)
    		{
                     Reaction=1;
                     kill(iPid,SIGTERM);

	    	}
		    if(process->iappMeteringReaction==2)
		    {
                     Reaction=1;
                     kill(iPid,SIGTERM);
                     ProcMon_SendMesg(acdata);
               
		    }
            if(Reaction==1)
            pstUserInfo = getpwuid(stPsInfo.pr_uid);

            if(pstUserInfo)
            {
                #ifdef AM_DEBUG
                        printf("user who started %s\n" , pstUserInfo->pw_name);
                #endif
                if( process->pcappMeteringUsername)
                free( process->pcappMeteringUsername);
                process->pcappMeteringUsername= ( I8 *)malloc(strlen(pstUserInfo->pw_name)+1);
                if(process->pcappMeteringUsername)
               {
                 memset(process->pcappMeteringUsername,'\0',strlen(pstUserInfo
->pw_name)+1);
                 strcpy(process->pcappMeteringUsername,pstUserInfo->pw_name);
               }
            }        
	
            
     }

  } // End of while

#endif // End of check proc for solaris
   if(pstProcDir)
   closedir(pstProcDir);
   return Reaction;
}
#endif

/*****************************************************************************
 *
 * name             :   ProcMon_CheckProc
 * description      :   This function is used to get the process status in
 *                      AIX machine
 * input parameters :   char *process - The proccess name
 * output parameters:   None
 * return type      :   int - PID of the process, if running, else -1
 * global variables :   None
 * calls            :   None
 */

#if defined(AIX_4_3)
#include <procinfo.h> 
int
ProcMonCheckProc(StappMeteringStruct *process)
{
 	int Reaction=0;
        char acdata[BUFSIZE];
        char ip[100];
        char port[100];
        char binname[100];
        char alert[100];
        I8 actempbuf[BUFSIZE];
        struct passwd *pwd;  
	int iIdx=0;
    	int iMaxNumProc = 1024;
    	int  iNumProc=0;
    	int iPid=-1;
    	int iIndex=0;
    	struct procsinfo *pstProcTable=NULL;
     
    do
    {
        //check whether we are here in the loop, then pstProcTable will
        //have some data. It will be NULL for the first time.
        if(pstProcTable != NULL)
            free(pstProcTable);

        pstProcTable = (struct procsinfo *)calloc(iMaxNumProc,sizeof(struct procsinfo));

        if(pstProcTable == NULL)
        {
        #ifdef AM_DEBUG
            printf("Process Not Found\n"); 
        #endif
            return -1;
        }

        iIdx = 0;

        // Now Get process table populated for the active processes.
        // Get the list of process currently active
        iNumProc = getprocs(pstProcTable, sizeof(struct procsinfo),
                                                NULL, 0,&iIdx,iMaxNumProc);
        if (iNumProc == -1)
        {
          #ifdef AM_DEBUG
            printf("iNumProc==-1\n");
          #endif
                
               
            iNumProc = 0 ;
            if(pstProcTable != NULL)
               free(pstProcTable);

            return -1;
        }
        else
        if (iNumProc == iMaxNumProc)
        {
            //Since the number of processes is above iMaxNumProc.
            //We have to loop again after multiplying iMaxNumProc by 2.
            iMaxNumProc *= 2;
            iNumProc = -1;
        }

    } while (iNumProc == -1);

    // Iterate thru the process list
    // and get our process out of it, if it exists
    for(iIndex=0;iIndex<iNumProc;iIndex++)
    {
        /* Check for swapper process */
        if (pstProcTable[iIndex].pi_pid == 0)
        {
            strcpy(pstProcTable[iIndex].pi_comm,"swapper");
        }

        iPid = pstProcTable[iIndex].pi_pid;

        /* Check for zombie process */
        if(pstProcTable[iIndex].pi_state == SZOMB)
        {
            strcpy(pstProcTable[iIndex].pi_comm, "<defunct>"); /* Zombie process */
        }

        if(strcmp(pstProcTable[iIndex].pi_comm,"")==0)
        {
            strcpy(pstProcTable[iIndex].pi_comm,"unknown");
        }


        if (!strcmp(pstProcTable[iIndex].pi_comm,process->pcappMeteringBinaryName))
       { 

                sprintf(actempbuf,"%d",APPMET);
                strcpy(binname,process->pcappMeteringBinaryName);

                sprintf(ip,"*****************************IP<%s>************",
                gstagentInfo.pcAlertIpAddr);
                sprintf(port,"***********************port<%d>****************"
                ,gstagentInfo.iAlertPort);
                if(process->pcalertmessage)
                strcpy(alert,process->pcalertmessage);
                sprintf(acdata,"%s %s %s %s",binname,ip,port,alert);
                if(process->iappMeteringReaction==0)
                {
                        Reaction=1;
                        ProcMon_SendMesg(acdata);
                    #ifdef AM_DEBUG
                      printf("*****%s*******\n",process->pcalertmessage);
                    #endif
                }
                if(process->iappMeteringReaction==1)
                {
                     Reaction=1;
                     kill(iPid,SIGTERM);

                }
                if(process->iappMeteringReaction==2)
                {
                 Reaction=1;
                 kill(iPid,SIGTERM);
                 ProcMon_SendMesg(acdata);

                }

            // Do the action here which is specified for
            // this process
                #ifdef AM_DEBUG
                    printf("AIX Process Found\n");
                #endif
          //  break;
        } // End of if


    } // End of for
     if(Reaction==1)
     pwd = getpwuid( pstProcTable[iIndex].pi_uid); 
     if(pwd)
     {
        #ifdef AM_DEBUG
            printf("user who started %s\n" , pwd->pw_name);
        #endif
         if( process->pcappMeteringUsername)
         free( process->pcappMeteringUsername);
        process->pcappMeteringUsername= ( I8 *)malloc(strlen(pwd->pw_name)+1);
        if(process->pcappMeteringUsername)
        {
         	memset(process->pcappMeteringUsername,'\0',strlen(pwd->pw_name)+1);
		  strcpy(process->pcappMeteringUsername,pwd->pw_name);
        }
     }

    if (pstProcTable != NULL)
        free(pstProcTable);

    return Reaction;
}
#endif // End of hash def for AIX


/*****************************************************************************
 *
 * name             :   ProcMon_CheckProc
 * description      :   This function is used to get the process status in
 *                      HP-UX machine
 * input parameters :   char *process - The proccess name
 * output parameters:   None
 * return type      :   int - PID of the process, if running, else -1
 * global variables :   None
 * calls            :   None
 */

#if defined(hpux10) || defined(hpux11)
#include <sys/pstat.h>
int
ProcMonCheckProc(StappMeteringStruct *process)
{
    
    char acdata[BUFSIZE];
    char ip[100];
    char port[100];
    char binname[100];
    char alert[100];
    I8 actempbuf[BUFSIZE];
    int iIdx=0;
    int iCount=0;
    int iUtil=0;
    int iPid=-1;
    int iNumProc = 0, bytes;
    int Reaction;
    int iMsgSize = 0;
    //struct pst_status stPstat;
    struct pst_dynamic stPstDyn;
    struct pst_status *pstProcTable;
    struct passwd *pstUserInfo = NULL;

    pstat_getdynamic(&stPstDyn, sizeof(struct pst_dynamic), 1, 0);
    iNumProc = stPstDyn.psd_activeprocs;

    bytes = iNumProc * sizeof(struct pst_status);

    if ((pstProcTable =
        (struct pst_status *) calloc(iNumProc, sizeof(*pstProcTable))) == NULL)
    {
        return;
    }

    if (pstat_getproc (pstProcTable, sizeof (struct pst_status), iNumProc, 0) != -1)
    {

        for (iIdx=0;iIdx < iNumProc;iIdx++)
        {
            iCount += (pstProcTable[iIdx].pst_stat & PS_RUN);

            iUtil += pstProcTable[iIdx].pst_cpu;

            iPid = pstProcTable[iIdx].pst_pid;
            if ( ! strcmp(pstProcTable[iIdx].pst_ucomm,process->pcappMeteringBinaryName) )
            {
                
                    sprintf(actempbuf,"%d",APPMET);
                    strcpy(binname,process->pcappMeteringBinaryName);
                    sprintf(ip,"*********IP<%s>*********",gstagentInfo.pcAlertIpAddr);
                    sprintf(port,"********port<%d>*****",gstagentInfo.iAlertPort); 
                    if(process->pcalertmessage)
                    strcpy(alert,process->pcalertmessage);
                    sprintf(acdata,"%s %s %s %s",binname,ip,port,alert);  
                    if(process->iappMeteringReaction==0)
                    {			
                        Reaction=1;
                        ProcMon_SendMesg(acdata);
                        #ifdef AM_DEBUG
                        printf("*****%s**********\n",process->pcalertmessage);
                        #endif
                    }
		            if(process->iappMeteringReaction==1)
		            {
                                 Reaction=1;
                                 kill(iPid,SIGTERM);

		            }
		            if(process->iappMeteringReaction==2)
		            {
                             Reaction=1;
                             kill(iPid,SIGTERM);
                             ProcMon_SendMesg(acdata);
               
		            }
                            if(Reaction==1)  
                            pstUserInfo = getpwuid(pstProcTable[iIdx].pst_uid); 
      		        if(pstUserInfo)
     		        {
                        #ifdef AM_DEBUG
	                        printf("user who started %s\n" ,pstUserInfo->pw_name);
                        #endif
				  
         		if( process->pcappMeteringUsername)
         		free( process->pcappMeteringUsername);
      		             process->pcappMeteringUsername= ( I8 *)malloc(strlen(pstUserInfo->pw_name)+1);
     		            if(process->pcappMeteringUsername)
                            {
                             memset(process->pcappMeteringUsername,'\0',strlen(pstUserInfo->pw_name)+1);
       		                    strcpy(process->pcappMeteringUsername,pstUserInfo->pw_name);
                            }
    		         }        
                            #ifdef AM_DEBUG

                            snmp_log(LOG_ERR,"(HP) Process is found <%s>\n",
                                            pstProcTable[iIdx].pst_ucomm);
                            #endif
                } // End of If

       //         iPid=-1;

        } // End of for
    } // End of If

    return Reaction;
}

#endif // End of HP-UX hash defined

/*****************************************************************************
 *
 * name             :   ProcMon_CheckProc
 * description      :   This function is used to get the process status in
 *                      LINUX machine
 * input parameters :   char *process - The proccess name
 * output parameters:   None
 * return type      :   int - PID of the process, if running, else -1
 * global variables :   None
 * calls            :   None
 */
 
#ifdef linux
int ProcMonCheckProc(StappMeteringStruct *process)
    {
        int Reaction=0;
        char acdata[BUFSIZE];
        char ip[100];
        char port[100];
        char binname[100];
        char alert[100];
        char actempbuf[BUFSIZE];
        struct sockaddr_in stsocAddr;
        struct passwd *pwd;
        char *cp;
        char    buf[256];
        char    string[256];
        char    string1[30];
        int uid ;
        char *str;
        FILE  *fpProc = NULL;
        static char      szBuff[256];
        static char  *pcPos;
        int    iPid;
        DIR             *pdProcDir = NULL;
        struct dirent   *pstProcEntry = NULL;
        static char      szString[256];

        #ifdef AM_DEBUG   
        printf("Reaction =%d\n",process->iappMeteringReaction);
        snmp_log(LOG_INFO,"ProcMon_CheckProc: linux  called\n");
        #endif


        if (pdProcDir != NULL)
        closedir(pdProcDir);
        pdProcDir = opendir("/proc");
         if(pdProcDir == NULL){
          snmp_log(LOG_INFO, "/proc open error. Exiting..\n");
          return -1;
         }
  while ((pstProcEntry = readdir(pdProcDir)) != NULL)
  {

        iPid = atoi(pstProcEntry->d_name);
    if(iPid)
    {
        sprintf(szString, "/proc/%d/status", iPid);
            if(fpProc)
            fclose(fpProc);
            if ((fpProc = fopen(szString, "r")) == NULL)
            {
                #ifdef AM_DEBUG
                printf("%s open error\n", szString); 
                #endif
                continue;
            }
       fgets(szBuff, sizeof(szBuff), fpProc);   /* Name: Process name */
       pcPos = (char *)strstr(szBuff, "Name:");
       if(pcPos)
       {
        sscanf(pcPos,"Name : %s ", szString);
       }
       else
       {
            if(fpProc)
            fclose(fpProc);
            continue;
        }
       if (strcmp(szString, process->pcappMeteringBinaryName)==0)
       {
            sprintf(actempbuf,"%d",APPMET);
            strcpy(binname,process->pcappMeteringBinaryName);
            sprintf(ip,"**********IP<%s>******",gstagentInfo.pcAlertIpAddr);
            sprintf(port,"********port<%d>*********",gstagentInfo.iAlertPort); 
            if(process->pcalertmessage)
            strcpy(alert,process->pcalertmessage);
            sprintf(acdata,"%s %s %s %s",binname,ip,port,alert);  
    	    if(process->iappMeteringReaction==0)
		    {			
                Reaction=1;
                ProcMon_SendMesg(acdata);
                #ifdef AM_DEBUG
                    printf("**********%s***********\n",process->pcalertmessage);
                #endif
		    }
		    if(process->iappMeteringReaction==1)
		    {
                         Reaction=1;
                         kill(iPid,SIGTERM);

		    }
		    if(process->iappMeteringReaction==2)
		    {
                     Reaction=1;
                     kill(iPid,SIGTERM);
                     ProcMon_SendMesg(acdata);
               
		    }
	        while( 1 )
	        {
                fgets(buf, sizeof(buf), fpProc);
                str = (char*)strstr(buf, "Uid");
                if ( str != NULL) break;
            }
            cp = (char*)strchr(buf, ':');
            ++cp;
            while (isspace(*cp))
            ++cp;
            sscanf(cp , "%s" , string1);
            if (cp)
            *cp = 0;
            uid = atoi(string1);
            if(Reaction==1)
            pwd = getpwuid(uid);
            if(pwd)
            {
                #ifdef AM_DEBUG
                printf("user who started %s\n" , pwd->pw_name);
                #endif
               if( process->pcappMeteringUsername)
                free( process->pcappMeteringUsername);
                process->pcappMeteringUsername= ( I8 *)malloc(strlen(pwd->pw_name)+1);
                if(process->pcappMeteringUsername)
                {
                    memset(process->pcappMeteringUsername,'\0',strlen(pwd->pw_name)+1);
                    strcpy(process->pcappMeteringUsername,pwd->pw_name);
                }
            }   
            #ifdef AM_DEBUG   
                snmp_log(LOG_INFO,"ProcMon_CheckProc: linux  process %s %d found. exiting..\n", szString, iPid);
            #endif
          }
       }
     }
 
     if(fpProc)
     fclose(fpProc);
     if (pdProcDir != NULL)
     closedir(pdProcDir);
     return Reaction;
    }
#endif // End of LINUX hash defined
 
/*****************************************************************************
 *
 * name             :   AM_DoReaction
 * description      :   Check if it is a disallowed process and do the necessary
                        action as per the MIB configuration                        
 * input parameters :   HANDLE, char *
 *                      
 * return type      :   int
 * global variables :   -
 * calls            :   
 */
I16 AM_DoReaction()
{
	UI32 ulcurrtime;
	I16 iretval=0;
	static int check=0,checksub=0;
	//struct timeval time_val;
	StappMeteringStruct tempapp;
        int Reaction=0; 
        I16 istatus;

	Stapp * applist;
	Stappsub *appsub;

	pthread_mutex_lock(&appmetering_lock);
	for(applist=Head;applist!=NULL;applist=applist->next)
	{  
		if(check==0)
		{
			applist=applist->next;
			check=1;
		}
		checksub=0;
		if(applist==NULL)
		break;
		for(appsub=applist->head;appsub!=NULL;appsub=appsub->next)
		{	
			if(checksub==0)
			{
				appsub=appsub->next;
				checksub=1;
			}

			ulcurrtime = Util_Get_Current_Time();
                        if(appsub==NULL)
                        {
                          printf("appsub == NULL\n");
                          break;
                        }
                        
			if((ulcurrtime >= appsub->ulappMeteringStartTime) 
						&&(ulcurrtime 
                       <= appsub->ulappMeteringEndTime))
                        

			{			
			
                         
	       tempapp.pcappMeteringBinaryName=
                 (I8 *)malloc(strlen(applist->pcappMeteringBinaryName)+1);
           
           if(tempapp.pcappMeteringBinaryName)
           {
                  memset(tempapp.pcappMeteringBinaryName,'\0',strlen(applist->pcappMeteringBinaryName)+1);
                  strcpy(tempapp.pcappMeteringBinaryName,
                      applist->pcappMeteringBinaryName);
           }
           tempapp.uiappMeteringIndex=appsub->uiappMeteringIndex;
		   tempapp.pcappMeteringID=
                    (I8 *)malloc(strlen(appsub->pcappMeteringID)+1);
            if(tempapp.pcappMeteringID)
            {
             memset(tempapp.pcappMeteringID,'\0',strlen(appsub->pcappMeteringID)+1);
              strcpy(tempapp.pcappMeteringID,appsub->pcappMeteringID);
            }
		    tempapp.ulappMeteringStartTime=appsub->ulappMeteringStartTime;
		    tempapp.ulappMeteringEndTime=appsub->ulappMeteringEndTime;
	        tempapp.iappMeteringReaction=appsub->iappMeteringReaction;
		    tempapp.pcalertmessage=
                  ( I8 *)malloc(strlen(appsub->pcalertmessage)+1);
            if(tempapp.pcalertmessage)
            {
                memset(tempapp.pcalertmessage,'\0',strlen(appsub->pcalertmessage)+1);
                strcpy(tempapp.pcalertmessage,appsub->pcalertmessage);
            }
            tempapp.istatus=appsub->istatus;	
            Reaction=ProcMonCheckProc(&tempapp);
         if(Reaction==1)
         {
             AM_LogData(&tempapp);
             AM_SetAppMetTable(tempapp); 
             if(tempapp.pcappMeteringUsername)
             free(tempapp.pcappMeteringUsername);
          }
          if(tempapp.pcappMeteringBinaryName)
          {
            free(tempapp.pcappMeteringBinaryName);
          }
          if(tempapp.pcappMeteringID)
          {
            free(tempapp.pcappMeteringID);
          }
          if(tempapp.pcalertmessage)
          {
            free(tempapp.pcalertmessage);
          }
	      iretval=1;
    	  break;
		
        }
	}
  }
	pthread_mutex_unlock(&appmetering_lock);	
	check=0;
	checksub=0;
	return iretval;
}

/*****************************************************************************
 *
 * name             :   AM_SetAppMetTable
 * description      :   For Setting the application metering table with the user who
                        started the application
                        
 * input parameters :   struct stappMeteringStruct *
 *                      
 * return type      :   int
 * global variables :   -
 * calls            :   
 *****************************************************************************/


I16 AM_SetAppMetTable(struct stappMeteringStruct tempappMet)
{
	struct snmp_pdu *pstreqPDU;
	struct snmp_pdu *pstrespPDU;

	I16 istatus;
	I32 iLen;

	oid UserName[MAX_OID_LEN];
    //    I32 UserNameLen=MAX_OID_LEN;
		
	iLen=sizeof(appMeteringUserName_variables_oid)/sizeof(oid);
	memcpy(UserName,appMeteringUserName_variables_oid,
					sizeof(appMeteringUserName_variables_oid));
	
	UserName[iLen++]=tempappMet.uiappMeteringIndex;
	pstreqPDU = snmp_pdu_create(SNMP_MSG_SET);
	
	snmp_pdu_add_variable(pstreqPDU,UserName,iLen,4,
		tempappMet.pcappMeteringUsername,strlen(tempappMet.pcappMeteringUsername));
	
	istatus = snmp_synch_response(agentSess, pstreqPDU, &pstrespPDU);
	if(istatus!=STAT_SUCCESS)
	{
		AM_AddMsgToLogfile("Error in snmp_synch_response \n ");	       
		return NASC_FAILURE;
	}
	return NASC_SUCCESS;
}

/*****************************************************************************
 *
 * name             :   AM_InitSession
 * description      :   For Initiating the session with the manager. Two session
						is created one for trap and one for get and set. The trap 
						session listens on port 20                        
                        
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/
void AM_InitSession()
{
   struct snmp_session tempSess;
   struct snmp_session trapSess;
#ifdef AM_DEBUG
   UI8 localAddr[30]; 
   UI8 gIPAddrDebug[]="192.168.162.5";
   I16 gAgentPortDebug=2140;
#endif

   /* For Session Initiation to read data from the table*/
   snmp_sess_init(&tempSess);
   /*snmp version*/
   tempSess.version = SNMP_VERSION_2c;
   tempSess.peername = gcAgentSessPeerName;
   //tempSess.local_port = 161;
#ifdef AM_DEBUG
   memset(localAddr,'\0',30);
   sprintf(localAddr,"%s:%d",gIPAddrDebug,gAgentPortDebug);
   printf(" Local Address %s \n", localAddr);
   tempSess.peername = localAddr;
#endif
   //tempSess.peername = gcAgentSessPeerName;

   tempSess.community = gcNASSACommunity;
   tempSess.community_len = strlen(gcNASSACommunity);
   tempSess.callback_magic = NULL;

 /* Seperate Session Initiation for TRAP*/
   snmp_sess_init(&trapSess);
   /*snmp version*/
   trapSess.version = SNMP_VERSION_2c;
   trapSess.peername = "localhost:"TRAPPORT;
   trapSess.local_port = atoi(TRAPPORT);
   /*trapSess.remote_port = giNASSAPort;*/
   trapSess.community = gcNASSACommunity;
   trapSess.community_len = strlen(gcNASSACommunity);
   /*Call Back function*/
   trapSess.callback = AM_ListenTrap; // Trap registering
   trapSess.callback_magic = NULL;
 
   SOCK_STARTUP;
   agentSess = snmp_open(&tempSess);/*This session is for GET/SET */
   SOCK_STARTUP;
   tSess = snmp_sess_open(&trapSess); /*This session is for trap */
  
   if(agentSess == NULL) 
   {
#ifdef AM_DEBUG
       printf("Error opening Session\n");
#endif
       snmp_perror("NASCenterAM Snmp Open: ");
       exit(1);
   }
}






/*****************************************************************************
 *
 * name             :   AM_Read_Packet
 * description      :   Read the snmp packet and put it in the circular buffer
 * input parameters : 
 * return type      :  
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
void AM_Read_Packets(void * arg){
     fd_set snmpSet;
   int snmpFds, blocking;
   struct timeval tv;
#ifdef AM_DEBUG
   printf("Inside REad_Pkt......\n");
#endif
   while(1)
   {
        snmpFds=0;
        blocking=1;
        FD_ZERO(&snmpSet); 
	
	snmp_sess_select_info(tSess, &snmpFds,&snmpSet,&tv,&blocking);
#ifdef AM_DEBUG
	printf("Waitinf for select %d\n",snmpFds);
#endif
        snmpFds = select(snmpFds,&snmpSet,0,0,NULL);
	if(snmpFds == -1)
	{
             if(snmpFds == EBADF)
	     {
    #ifdef AM_DEBUG
			printf("An invalid file descriptor \n ");
    #endif
	     }else if(snmpFds == EINTR)
	     {
    #ifdef AM_DEBUG
                 printf("A non blocked signal was caught \n");		     
    #endif
	     }else if(snmpFds == ENOMEM)		     
	     {
         #ifdef AM_DEBUG
                 printf("Select was unable to allocate memory non \n");		     
         #endif
	     }
        #ifdef AM_DEBUG
	     printf("Select returned error \n");	
        #endif
	     continue;
		}
        if(snmpFds){
        #ifdef AM_DEBUG
            printf("Inside  READ TRAP WHILE......\n");
        #endif
	    snmp_sess_read(tSess, &snmpSet);
        } else
		{
        #ifdef AM_DEBUG
            printf("No FileDS to READ......\n");
        #endif
	
		} 
 }// end of while(1)
}


/*****************************************************************************
 *
 * name             :   AM_CheckField
 * description      :   
 * input parameters : 
 * return type      :  
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
I16 AM_CheckField( netsnmp_variable_list *pstSNMPPDU )
{
	I16 iLen;
    I16 iRetValue=-1;


	iLen=sizeof(appMeteringID_variables_oid)/sizeof(oid);
    if(!netsnmp_oid_is_subtree(appMeteringID_variables_oid,iLen,
                                     pstSNMPPDU->name,pstSNMPPDU->name_length))		
    {
		iRetValue=AM_ID;	     
	}
		
	iLen=sizeof(appMeteringStartTime_variables_oid)/sizeof(oid);
	if(!netsnmp_oid_is_subtree(appMeteringStartTime_variables_oid,iLen,
                                    pstSNMPPDU->name,pstSNMPPDU->name_length))		
    {
		iRetValue=AM_APPSTARTTIME;	     
	
	}

	iLen=sizeof(appMeteringEndTime_variables_oid)/sizeof(oid);
	if(!netsnmp_oid_is_subtree(appMeteringEndTime_variables_oid,iLen,
                                   pstSNMPPDU->name,pstSNMPPDU->name_length))		
    {
		iRetValue=AM_APPENDTIME;     
	}

	iLen=sizeof(appMeteringReaction_variables_oid)/sizeof(oid);
	if(!netsnmp_oid_is_subtree(appMeteringReaction_variables_oid,iLen,
                              pstSNMPPDU->name,pstSNMPPDU->name_length))		
    {
		iRetValue=AM_APPREACTION;	     
	}

		
	iLen=sizeof(appMeteringBinaryName_variables_oid)/sizeof(oid);
	if(!netsnmp_oid_is_subtree(appMeteringBinaryName_variables_oid,iLen,
                                     pstSNMPPDU->name,pstSNMPPDU->name_length))		
    {
		iRetValue=AM_APPBINNAME;	     
	}

        iLen=sizeof(appMeteringAlertMessage_variables_oid)/sizeof(oid);
	if(!netsnmp_oid_is_subtree(appMeteringAlertMessage_variables_oid,iLen,
                                    pstSNMPPDU->name,pstSNMPPDU->name_length))		
    {
		iRetValue=AM_ALERTMESSG;	     
	}

    iLen=sizeof(appMeteringStatus_variables_oid)/sizeof(oid);
	if(!netsnmp_oid_is_subtree(appMeteringStatus_variables_oid,iLen,
                                       pstSNMPPDU->name,pstSNMPPDU->name_length))		
	{     
		iRetValue=AM_APPSTATUS;
	}
     return iRetValue;
}



/*****************************************************************************
 *
 * name             :   AM_CheckTable  
 * description      :     
 * input parameters :   - 
 * return type      :   
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
I16 AM_CheckTable(netsnmp_variable_list *pstCurrVars)
{
     I16 iOperFlag; 
     I16 iLen;

     iOperFlag =-1;
    /*Check the fetched record is pertaining to Backup Master Table */
     iLen =0;
     iLen = sizeof(appMeteringTable_variables_oid) / sizeof(oid);
    if(!netsnmp_oid_is_subtree(appMeteringTable_variables_oid,
         iLen, pstCurrVars->name, pstCurrVars->name_length))
     { 
         iOperFlag = 1;
     }

    return iOperFlag;
}


/********************************************************************************
 * LinkList Module
 *
 ********************************************************************************/

/***************************************************************************
 *
 * name             :   List( )
 * description      :   This function is used to add
 *                      the new node 
 * input parameters :   StappMeteringStruct *pappMet
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * 
 */

/* This function maintains one dummy node for Head inorder not to change the Head pointer*/
/* This function also maintains one more dummy node for every subtree head. This is required
   because not to change the subtree head pointer even if the first node is deleted*/
void List(StappMeteringStruct *pappMet,int sfind)
 {       
        int find=0;
        /* check whether the Head node is created already*/
        if(Head!=NULL)
            Appmov=Head;
        else
        { 
         /*create a new node and assign as an Head Pointer*/
            Head=(Stapp *)malloc(sizeof(Stapp));
            if(Head==NULL)
            {
          #ifdef AM_DEBUG
            printf("Memory Error\n");
          #endif
            return;
            }
            memset(Head,'\0',sizeof(Stapp));
            Save=Head;
            Head->next=NULL;
            Head->head=NULL;
            /*Head pointer cannot be moved so assign it to a temp pointer*/
            Appmov=Head;
        }

        while(Appmov)
        { 
            if(!Appmov->next)
            prev=Appmov; 
            /*Binary name acts as an index check for the binary name whether the binary name already exists.*/
            /* Any match has been detected find flag will be intialized to 1*/
            if(!strcmp(Appmov->pcappMeteringBinaryName,pappMet->pcappMeteringBinaryName))
            {
                find=1;
                break;
            }
            Appmov=Appmov->next;
        }
        /*This condition is required because the above while loop makes Appmov 
        pointer to NULL.*/
        if(find!=1)
        Appmov=prev;
        /*Binary doesn't match with the link create a new binary node*/
        if(!find)
        { 
            node=(Stapp *)malloc(sizeof(Stapp));
            if(node==NULL)
            {
            #ifdef AM_DEBUG
                printf("Memory Error\n");
            #endif
                return;
            }
            memset(node,'\0',sizeof(Stapp));
            node->next=NULL;
            /*copy the binary name for the newly created node*/
            strcpy((char*)node->pcappMeteringBinaryName,pappMet->pcappMeteringBinaryName);
            Appmov->next=node;
            Appmov=Appmov->next;
            Appmov->head=NULL;
        #ifdef AM_DEBUG
	        printf("Before Appmov->head==NULL\n");
        #endif
            /*Inorder to ensure whether the Appmov->head==NULL. This check
            is required */
            if(Appmov->head==NULL)
            {
            #ifdef AM_DEBUG
                printf("Inside Appmov->head\n");	    
            #endif
                /*Dummy node has been created for subtree head*/
                newsub=(Stappsub *)malloc(sizeof(Stappsub));  
		if(newsub==NULL)
                {
                #ifdef AM_DEBUG
                    printf("Memory Error\n");
                #endif 
                    free(node);
                    return;
                }
                memset(newsub,'\0',sizeof(Stappsub));
                newsub->uiappMeteringIndex=0;
                newsub->next=NULL;
                Appmov->head=newsub;
                //newsub->a=0;
                Appmov->head->next=NULL;
            }
            find=0;
        }
        Submove=Appmov->head;
        /*This while loop is required inorder insert the node based on the index.*/
        while(Submove)
       {   
        #ifdef AM_DEBUG
           printf("Inside while\n");
        #endif
           prevsubmove=Submove;
            if(gpsub!=NULL) 
            if(Submove->uiappMeteringIndex>pappMet->uiappMeteringIndex)
	    {
        #ifdef AM_DEBUG
                        printf("Inside if\n");
        #endif
            newsub=(Stappsub *)malloc(sizeof(Stappsub));
            if(newsub==NULL)
            {
                #ifdef AM_DEBUG
                    printf("MEMORY error\n");
                #endif
                return;
            }
            memset(newsub,'\0',sizeof(Stappsub));
            newsub->uiappMeteringIndex=0;	
            newsub->ulappMeteringStartTime=0;
            newsub->ulappMeteringEndTime=0;
            newsub->iappMeteringReaction=0;
            newsub->istatus=0;
            newsub->next=NULL;
            newsub->next=Submove;
     		gpsub->next=newsub;
			newsub->uiappMeteringIndex=pappMet->uiappMeteringIndex;
            if(pappMet->pcappMeteringID)
            {
              	strcpy(newsub->pcappMeteringID,pappMet->pcappMeteringID);
            }
            newsub->ulappMeteringStartTime=pappMet->ulappMeteringStartTime;
            newsub->ulappMeteringEndTime=pappMet->ulappMeteringEndTime;
            newsub->iappMeteringReaction=pappMet->iappMeteringReaction;
            if(pappMet->pcalertmessage)
            strcpy(newsub->pcalertmessage,pappMet->pcalertmessage); 
            return;
 		}
            else
            if(Submove->uiappMeteringIndex==pappMet->uiappMeteringIndex && sfind==0)
           {
            if(pappMet->pcappMeteringID)
            {
              	strcpy(Submove->pcappMeteringID,pappMet->pcappMeteringID);
            }
            Submove->ulappMeteringStartTime=pappMet->ulappMeteringStartTime;
            Submove->ulappMeteringEndTime=pappMet->ulappMeteringEndTime;
            Submove->iappMeteringReaction=pappMet->iappMeteringReaction;
            if(pappMet->pcalertmessage)
            strcpy(Submove->pcalertmessage,pappMet->pcalertmessage); 
            return;
          }
            gpsub=Submove;
            if(Submove) 
            {
                Submove=Submove->next;
            }
	}
       
        /*if the new node index is greater than the one that is present in the 
        list create a new node and add it to the end*/
        if(!Submove) 
        {           
            newsub=NULL;  
            Submove=prevsubmove;
            newsub=(Stappsub *)malloc(sizeof(Stappsub));
            if(newsub==NULL)
            {
                #ifdef AM_DEBUG
                    printf("MEMORY error\n");
                #endif
                return;
            }
            memset(newsub,'\0',sizeof(Stappsub));
            newsub->next=NULL;
            Submove->next=newsub;
            newsub->uiappMeteringIndex=pappMet->uiappMeteringIndex;
	    if(pappMet->pcappMeteringID)
            strcpy(newsub->pcappMeteringID,pappMet->pcappMeteringID);
	    newsub->ulappMeteringStartTime=pappMet->ulappMeteringStartTime;
	    newsub->ulappMeteringEndTime=pappMet->ulappMeteringEndTime;
	    newsub->iappMeteringReaction=pappMet->iappMeteringReaction;
            if(pappMet->pcalertmessage)
	    strcpy(newsub->pcalertmessage,pappMet->pcalertmessage);
            newsub->istatus=pappMet->istatus;
          
      }
    return;
 }


/***************************************************************************
 *
 * name             :   Modify( )
 * description      :   This function is used to Modify
 *                      the particular node 
 * input parameters :   StappMeteringStruct *pappMet
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * 
 */


void Modify(StappMeteringStruct *pappMet)
    {
        Stapp *mov;
        Stappsub *smov;
        int flag=0;
        mov=Head;
        if(mov)
        mov=mov->next;
        /*This functio is use to modify the the List */
        while(mov)
        {  
            smov=mov->head;
        #ifdef AM_DEBUG
            printf("Strcmp : %s : %s\n",pappMet->pcappMeteringBinaryName,mov->pcappMeteringBinaryName);
        #endif
	    if(pappMet->pcappMeteringBinaryName)
            if(!strcmp(pappMet->pcappMeteringBinaryName,mov->pcappMeteringBinaryName))
            {
                #ifdef AM_DEBUG
                printf("Modified Application Name :: %s\n",mov->pcappMeteringBinaryName);
                #endif
                while(smov)
                {
                    printf("Inside modify while\n"); 
                    smov=smov->next;
                    if(smov)
                    if(smov->uiappMeteringIndex==pappMet->uiappMeteringIndex)
    	            {
                        flag=1;
                        smov->uiappMeteringIndex=pappMet->uiappMeteringIndex;	
                        if(pappMet->pcappMeteringID)
                        strcpy(smov->pcappMeteringID,pappMet->pcappMeteringID);
                        smov->ulappMeteringStartTime=pappMet->ulappMeteringStartTime;
                        smov->ulappMeteringEndTime=pappMet->ulappMeteringEndTime;
                        smov->iappMeteringReaction=pappMet->iappMeteringReaction;
                        if(pappMet->pcalertmessage)
                        strcpy(smov->pcalertmessage,pappMet->pcalertmessage);
                        smov->istatus=pappMet->istatus;
        		    }
                      
                 }
            }
            mov=mov->next;
        }
        /*If the user is trying to add a new entry in the table
          the trap will be received as Modify if the node was not found it has to
          add a new node*/
          if(flag==0)
          SearchList(pappMet,Head);          
    return;
    }
/***************************************************************************
 *
 * name             :   Delete( )
 * description      :   This function is used to Delete
 *                      the particular node 
 * input parameters :   StappMeteringStruct *pappMet
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * 
 */

/*This function is required inorder to delete a node*/
void Delete(StappMeteringStruct *pappMet)
    {
        Stapp *mov=NULL,*pre=NULL;
        Stappsub *smov=NULL,*prev=NULL;
        mov=Head;
        pre=Head;
        #ifdef AM_DEBUG
         printf("\nDelete Called\n");
        #endif
        if(mov)
        {  
            mov=mov->next;
        }
      while(mov)
      {   
            smov=mov->head;
	    if(pappMet->pcappMeteringBinaryName)
	    if(!strcmp(pappMet->pcappMeteringBinaryName,mov->pcappMeteringBinaryName))
            { 
            #ifdef AM_DEBUG
               printf("\n\n%d %d",pappMet->istatus,pappMet->uiappMeteringIndex);
            #endif
                while(smov)
                {
                  prev=smov;
                  smov=smov->next;
                  if(smov)
                  { 
                    if(smov->uiappMeteringIndex==pappMet->uiappMeteringIndex)
                    {
                        prev->next=smov->next;     
                        free(smov);
                        smov=NULL;
                        #ifdef AM_DEBUG
                        Display(Head);
                        printf("Deleted\n");
                        #endif  
                        break;
                        //smov=prev;
                    }
                 }
                }
                if(mov->head->next==NULL)
                {
                    pre->next=mov->next;  	 
                    free(mov->head);
                    free(mov);
                    mov=NULL;   
                }
            }
            if(mov)
            {
                mov=mov->next;
                pre=pre->next;
            }
        } 
        return;
     }
/***************************************************************************
 *
 * name             :   Free( )
 * description      :   This function is used to clean
 *                      up the entire list
 * input parameters :   Stapp* Head
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * 
 */
 
void FreeHead(Stapp * Head)
    {
        Stapp *mv,*fv;
        Stappsub *m,*f;
        mv=Head;
        if(mv)
        mv=mv->next;
        while(mv)
        {
            m=mv->head;
            while(m)
            {
                f=m;
                m=m->next;
                if(m);
                free(f);
            }
            mv=mv->next;
        }  
        mv=Head;		   
        while(mv)
        {
            fv=mv;
            mv=mv->next;
            if(fv);
            free(fv);
        }
    return;
    }
/***************************************************************************
 *
 * name             :   Display( )
 * description      :   This function is used to display
 *                      the nodes loaded
 * input parameters :   Stapp* Head
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * 
 */
#ifdef AM_DEBUG

void Display( Stapp* Head)
    {
        Stapp *mv=NULL;
        Stappsub *m=NULL;


        if(Head)
        {
            mv=Head;
            mv=mv->next;
        }
        while(mv)
        {
            if(mv->head)
            {
                m=mv->head;
                printf("Appliation Name %s\n",mv->pcappMeteringBinaryName);
            
            }
            while(m)
            {
                m=m->next;
                if(m)
                {
                    printf("IND %d\n",m->uiappMeteringIndex);
                    printf("ID %s\n",m->pcappMeteringID);
                    printf("ST %d\n",m->ulappMeteringStartTime);
                    printf("ET %d\n",m->ulappMeteringEndTime);
                    printf("Reaction %d\n",m->iappMeteringReaction);
                   // printf("%s\n",m->pcappMeteringUsername);
                    printf("Pcalertmessage %s\n",m->pcalertmessage);
                    printf("status %d\n",m->istatus);
                }

            }
            if(mv->next)
            mv=mv->next;
            else
            break;
        }
    return;
    }
#endif

/***************************************************************************
 *
 * name             :   ProcMon_SendMesg
 * description      :   This function is used to send UDP message to the
 *                      manager
 * input parameters :   char *pcMesg - UDP message to be sent
 * output parameters:   None
 * return type      :   int - returns sucess (0) or failure (1)
 * global variables :   None
 * calls            :   getIPFromASNFormat
 */

int
ProcMon_SendMesg(char *pcMesg)
{
    int                 iSock=0;
    int                 iBufLen=0;
    char                aBuffer[16];
    struct sockaddr_in  stSrvName;
    struct hostent      *pstHostEnt;
    #ifdef AM_DEBUG
        printf("Common_SendAlert Message <%s>\n",pcMesg);
    #endif
    // create socket for connecting to server
    iSock = socket(AF_INET, SOCK_DGRAM,0);

    if(iSock < 0)
    {
        perror("opening datagram socket");
        return(1);
    }

    // construct stSrvName for connecting to server
    stSrvName.sin_family = AF_INET;
    stSrvName.sin_port = htons(gstagentInfo.iAlertPort);

    strcpy(aBuffer,gstagentInfo.pcAlertIpAddr);

    // convert hostname to IP address and enter into stSrvName
    stSrvName.sin_addr.s_addr=inet_addr(aBuffer);

    // Check the buffer length
    iBufLen = strlen(pcMesg)+1;

    // send message to server
    if(sendto(iSock, (const void*)pcMesg, iBufLen, 0,
                (struct sockaddr *)&stSrvName, sizeof(stSrvName)) <0)
    {
        perror("Error sending datagram message");
        close (iSock);
        return(1);
    }
    else
    #ifdef AM_DEBUG
            printf("Alert message <%s> \nSent to Manager IP <%s> \nManager Port <%d>\n",
                pcMesg,
                aBuffer,
                gstagentInfo.iAlertPort);
    #endif

    // close connection
    close(iSock);

    return(0);
}

 /********************************************************************************
 * name             :   SearchList
 * description      :   This function is used find the node from the 
 *                      list
 * input parameters :   StappMeteringStruct *pAMeter,Stapp* Save
 * output parameters:   None
 * return type      :   void 
 * global variables :   None
 * calls            :   List()
 */
/*This function is used to find the node and exactly it calls the List function*/
void SearchList(StappMeteringStruct *pAMeter,Stapp* Save)
{ 
    Stapp *mv=NULL;
    Stappsub *m=NULL;
    char Bname[255];
    StappMeteringStruct *temp;

    temp=(StappMeteringStruct*)malloc(sizeof(StappMeteringStruct));

    if(temp==NULL)
    {
      #if AM_DEBUG
       printf("Memory error in search list\n");
      #endif
      return;
     }
    memset(temp,'\0',sizeof(StappMeteringStruct));
    if(Save)
    {
        mv=Save;
        mv=mv->next;
    }
    while(mv)
    {
        if(mv->head)
        {
            m=mv->head;
            if(mv)
            strcpy(Bname,mv->pcappMeteringBinaryName);
        }
        while(m)
        {
            m=m->next;
            if(m)
            {
                if(pAMeter->uiappMeteringIndex==m->uiappMeteringIndex)
                { 
                    #ifdef AM_DEBUG
                    printf("Inside inner if\n");
                    #endif
                    pAMeter->istatus=m->istatus;
                    pAMeter->iappMeteringReaction=m->iappMeteringReaction;
                    if(m->pcalertmessage && pAMeter->pcalertmessage)
                    strcpy(pAMeter->pcalertmessage,m->pcalertmessage);
                    if(m->pcappMeteringID &&pAMeter->pcappMeteringID)
                    { 
                        pAMeter->pcappMeteringID=(char *)malloc(strlen(m->pcappMeteringID)+1);
                        strcpy(pAMeter->pcappMeteringID,m->pcappMeteringID);
                    } 
                    pAMeter->ulappMeteringEndTime=m->ulappMeteringEndTime;
                    pAMeter->ulappMeteringStartTime=m->ulappMeteringStartTime;

                    if(m->pcappMeteringUsername && pAMeter->pcappMeteringUsername)
                    {
                        pAMeter->pcappMeteringUsername=(char*)malloc(strlen(m->pcappMeteringUsername)+1);
                        strcpy(pAMeter->pcappMeteringUsername,m->pcappMeteringUsername);
                    }
                    List(pAMeter,1);
                    if(pAMeter->pcappMeteringID)
                    free(pAMeter->pcappMeteringID);
                    if(pAMeter->pcappMeteringUsername)
                    free(pAMeter->pcappMeteringUsername); 
                    temp->pcappMeteringBinaryName=(char *) malloc(strlen(Bname)+1); 
                    strcpy(temp->pcappMeteringBinaryName,Bname);
                    temp->uiappMeteringIndex=pAMeter->uiappMeteringIndex;
                    temp->istatus=6;
                    Delete(temp);
                    if(temp->pcappMeteringBinaryName)
                    free(temp->pcappMeteringBinaryName);
                    break;
                }
            }	
        }
        if(mv->next)
        mv=mv->next;
        else
        break;
    }

                    if(temp)
                    free(temp);
    return;
}


/*****************************************************************************
 *
 * name             :   ProcMon_CheckProc
 * description      :   This function is used to get the process status in
 *                      UnixWare machine
 * input parameters :   char *process - The proccess name
 * output parameters:   None
 * return type      :   int - PID of the process, if running, else -1
 * global variables :   None
 * calls            :   None
 */
#if defined(osf5)
int
ProcMonCheckProc (StappMeteringStruct *process)
{
    struct tbl_procinfo pinfo;
    DIR                 *dirstream = {0};
    struct dirent       *direntry = NULL;
    int                 nread = 0;
    int                 pInfoSize = sizeof(struct tbl_procinfo);
    long iPid=-1;
    int Reaction=0;
    char acdata[BUFSIZE];
    struct passwd *pstUserInfo = NULL;
    char ip[100];
    char port[100];
    char binname[100];
    char alert[100];
    char actempbuf[BUFSIZE];


    // Open proc system
    dirstream = opendir("/proc");

    if ( dirstream ) {
	} else {
		 return -1;
	}

    // Loop until we find the process
    while((direntry = readdir(dirstream))!= '\0')
    {
        if(!strcmp(direntry->d_name,".") || !strcmp(direntry->d_name,"..") ||
                        !strcmp(direntry->d_name,"0"))
        {
            continue;
        }
   		if(table(TBL_PROCINFO,atoi(direntry->d_name),&pinfo,1,pInfoSize)>0){
   		} else{
				continue;
        }
        if( strcmp(pinfo.pi_comm,process->pcappMeteringBinaryName) == 0 )
        {
                    // Do the action here which is specified for
                    // this process

		    sprintf(actempbuf,"%d",APPMET);
		    strcpy(binname,process->pcappMeteringBinaryName);
            sprintf(ip,"*********IP<%s>**********",gstagentInfo.pcAlertIpAddr);
            sprintf(port,"*******port<%d>*****",gstagentInfo.iAlertPort);
            if(process->pcalertmessage)
            strcpy(alert,process->pcalertmessage);
            sprintf(acdata,"%s %s %s %s",binname,ip,port,alert);  
    	    if(process->iappMeteringReaction==0)
		    {			
		            Reaction=1;
                            ProcMon_SendMesg(acdata);
                    #ifdef AM_DEBUG
                        printf("****%s*****\n",process->pcalertmessage);
                    #endif
		    }
		    if(process->iappMeteringReaction==1)
    		{
                     Reaction=1;
                     kill(iPid,SIGTERM);

	    	}
		    if(process->iappMeteringReaction==2)
		    {
                     Reaction=1;
                     kill( atoi(direntry->d_name),SIGTERM);
                     ProcMon_SendMesg(acdata);
               
		    }

            pstUserInfo = getpwuid(pinfo.pi_uid);

            if(pstUserInfo)
            {
                #ifdef AM_DEBUG
                        printf("user who started %s\n" , pstUserInfo->pw_name);
                #endif
                process->pcappMeteringUsername= ( I8 *)malloc(strlen(pstUserInfo->pw_name)+1);
                if(process->pcappMeteringUsername)
                strcpy(process->pcappMeteringUsername,pstUserInfo->pw_name);
            }        
     }
  }
	closedir(dirstream);
    return Reaction;
}
#endif /* osf5 */


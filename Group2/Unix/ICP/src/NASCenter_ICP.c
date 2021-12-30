/*****************************************************************************
 *
 * filename.c: NASCenter_ICP.c
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Aravind Balan
 *
 * description :
 *      This file contains functions for processing ICP commands
 *
 * TODO:
 *      [ Things to be improved ]
 *
 * CM Details:
 *
 * $Author: sspriya $
 * $Log: NASCenter_ICP.c,v $
 * Revision 1.24  2004/06/16 13:41:33  sspriya
 * Storing timeofday after read from client to be used in fix to client system blackout problem in ICP.
 *
 * Revision 1.23  2004/03/30 05:05:48  sspriya
 * Added code to check for RowStatus before adding ICP command to the list of disallowed commands. Fixed memory leak by freeing SNMP response PDU. Reviewed by VijaiGeetha.
 *
 * Revision 1.22  2004/01/13 08:10:13  baravind
 * Updates after Purify test II
 *
 * Revision 1.21  2004/01/13 05:07:36  baravind
 * Updates after a round of purify test
 *
 * Revision 1.20  2004/01/09 11:01:41  sig
 * Added default shell for user management
 *
 * Revision 1.19  2004/01/09 06:30:00  baravind
 * Fixed warnings
 *
 * Revision 1.18  2004/01/07 08:11:49  baravind
 * *** empty log message ***
 *
 * Revision 1.17  2004/01/07 05:45:10  baravind
 * Big Change !!! Catching signals in pthread behaves erratically . Changed to  message queue instead of signals for sending trap to child process.
 *
 * Revision 1.16  2004/01/07 05:30:02  baravind
 * *** empty log message ***
 *
 * Revision 1.15  2004/01/06 10:53:53  baravind
 * Added htonl / ntohl to enable ICP get commands from different OS
 *
 * Revision 1.14  2004/01/06 09:59:29  baravind
 * Added pclose after popen
 *
 * Revision 1.13  2004/01/05 05:30:21  baravind
 * Fixed Code review comments
 *
 * Revision 1.12  2003/12/29 12:15:01  sig
 * *** empty log message ***
 *
 * Revision 1.11  2003/12/25 11:57:43  baravind
 * Added by Aravind.B
 *
 * Revision 1.10  2003/12/24 13:25:02  sig
 * Added by Aravind.B . Fix for Linux . Changed from popen -> system
 *
 * Revision 1.9  2003/12/22 16:49:26  baravind
 * Added by Aravind.B . Ignored SIGCHLD in child process . Otherwise , crashing in unixware
 *
 * Revision 1.8  2003/12/20 04:12:05  baravind
 * Added by Aravind.B on Dec-20-2003 .Fixed bugs
 *
 * Revision 1.7  2003/12/20 02:47:03  baravind
 * Added by Aravind.B on Dec-20-2003 . Fixed ST bugs.
 *
 * Revision 1.6  2003/12/17 07:48:51  baravind
 * Added by Aravind on Dec-17-2003  - Released for system testing
 *
 * Revision 1.5  2003/12/16 08:56:55  baravind
 * Added by Aravind.B on 16-Dec-2003 . Code for Linux
 *
 * Revision 1.4  2003/12/11 05:16:32  baravind
 * Afrer Unit testing (1) - Added by Aravind.B on Dec-11-2003
 *
 * Revision 1.3  2003/12/08 11:26:48  baravind
 * Added by Aravind.B on 8-Dev-2003
 *
 * Revision 1.1.1.2  2003/10/17 06:41:51  hari
 * no message
 *
 * Revision 1.1.1.1  2003/10/01 05:23:22  thiagav
 * no message
 *
 * $State: Exp $
 * $Id: NASCenter_ICP.c,v 1.24 2004/06/16 13:41:33 sspriya Exp $
 * $Name:  $
 *
 ****************************************************************************/


/********************************* includes *********************************/
#define REENTRANT
#ifndef NASCENTER_ICP_H
#include <NASCenter_ICP.h>
#endif


#include <pthread.h>
#include <sys/wait.h>
#include <NASCenter_ICP_oid.h>

/****************************** local macros ********************************
 *
 * (Macros shareable with rest of the modules are present in .h file)
 */

 /* ICP table attributes */

 #define ICPTABLE_OID icpTable_variables_oid
 #define ICPINDEX_OID icpIndex_variables_oid
 #define ICPUSERID_OID icpUserId_variables_oid
 #define ICPCOMMAND_OID icpCommand_variables_oid
 #define ICPROWSTATUS_OID icpStatus_variables_oid

 #define ICPTABLE_OID_LEN icpTable_oid_len
 #define ICPINDEX_OID_LEN icpIndex_oid_len
 #define ICPUSERID_OID_LEN icpUserId_oid_len
 #define ICPCOMMAND_OID_LEN icpCommand_oid_len
 #define ICPROWSTATUS_OID_LEN icpStatus_oid_len 

/********************* local structure/union definitions ********************/
struct session_list1 {
  struct session_list1 *next;
  netsnmp_session *session;
  netsnmp_transport *transport;
  struct snmp_internal_session *internal;
};
sigset_t       signalSet;



/********************* prototypes for local functions ***********************/

void ICP_InitSNMPSession(void);

ENASCenterError ICP_GetICPTable(stIcpList * stIcpList); 

void ICP_updateList ( void );



/************************* Globals within this module ***********************/

stIcpList    gstIcpList;
stCmdPacket  gstSendICPCmdPkt;
I32 giMsgQueueId = -1;
I32 giChildId;


void  *gpstSnmpSessionICP = NULL ;
 UI8 gcNASSACommunityICP[] = "public";

I32 	giProcessTrap = 0;



/************************* externs  ***********************/

extern UI8  gpcAgentSessPeerName[];
extern UI8 gcNASSACommunity[];

extern UI8 gcPrompt [] ;
extern UI8 gcPktDelimiter ;
extern UI8 gEqCmdDelimiter ;
extern UI8 *gpcErrorMsg[] ;
extern UI8 * gpcEquivalentCmd [] ; 
extern struct snmp_session  *gpstAgentSession;
extern I32 gTrapSockId;
extern struct timeval stLastReadTimeValue;

/*****************************************************************************
 *
 * name             :   ICP_ProcessICPCommand
 * description      :   This function prcesses ICP commands from console 
 * input parameters :   I32 iSocketFd  TCP socket id 
 *                      stCmdPacket * pstCmdPkt  Command pkt got 
 *
 * output parameters:   none
 *                      
 * return type      :   ENASCenterError
 * global variables :   gstIcpList , gpstSnmpSessionICP
 * calls            :   ICP_InitICPList , ICP_InitSNMPSession , 
 *                      ICP_GetICPTable , ICP_SendPkt , ICP_CheckCmdInList ,
 *                      ICP_ExecCommand                    
 ****************************************************************************/

 
ENASCenterError ICP_ProcessICPCommand (I32 iSocketFd , stCmdPacket * pstCmdPkt)
{
    ENASCenterError eRetValue = NASC_SUCCESS;
    BOOL  iFirstTime = TRUE;
    I32 iTotalCmdSize = 0;
    stCmdPacket  stSendCmdPkt = { 0 } ;
    I32 iMesgSize = 0;
    UI8 * pucScratchForCmd = NULL;
    int  iPipeId[2];
    I32 iPileFlag;
    pthread_t iThreadId;
    I32 retval;

    giMsgQueueId = msgget(MESSAGE_Q , 0666 | IPC_CREAT);

    printf("MEssage q in cjhild %d\n" , giMsgQueueId);

  
    if ( giMsgQueueId < 0 )
    {
        printf("Message Queue cannot be got .. \n");
    }

    giChildId = getpid();


     sigfillset ( &signalSet );

    ICP_Signal(SIGCHLD , SIG_IGN); 

     sigdelset ( &signalSet , SIGTRAP);
     sigdelset ( &signalSet , SIGINT);
     sigdelset ( &signalSet , SIGCHLD);

     /* block all signals except SIGTRAP , SIGINT and SIGCHLD */

     pthread_sigmask ( SIG_BLOCK, &signalSet, NULL);

     


     /* printf("Group id of child : %ld\n" , getpgrp()); */

     /* create pipe . This will be used for getting errors if any on
       executing  command */

     pipe(iPipeId);

     iPileFlag = fcntl(iPipeId[0] , F_GETFL , 0);

    retval = fcntl(iPipeId[0] , F_SETFL , iPileFlag | O_NONBLOCK);

    if ( retval < 0 ) perror("fcntl failed..");       

    /* Dupe it to stderr */

    dup2(iPipeId[1] , 2);

    close(iPipeId[1]);


    ICP_InitICPList( &gstIcpList);

    printf("Initialised ICP list \n");


    ICP_InitSNMPSession();
    
    eRetValue = ICP_GetICPTable(&gstIcpList);

    printf("calling thread...\n");

    /* Spawn thread to update list of disallowed commands on getting 
       Trap from parent  */

    if ( giMsgQueueId >= 0 )
    {
        if((pthread_create(&iThreadId , NULL , (void *) ICP_updateList , 
                           NULL)) != 0)
        {
             printf("Thread can not be created \n");	   
	            return NASC_FAILURE;
        }  
        printf("Tread Created in ICP child process");
    }



    while ( 1 ) 
    {
        if ( iFirstTime != TRUE)
        {
            memset(pstCmdPkt , '\0' , sizeof(stCmdPacket));
            memset(&stSendCmdPkt , '\0' , sizeof(stCmdPacket));
            iMesgSize  =  read(iSocketFd , pstCmdPkt , sizeof(stCmdPacket));
            gettimeofday(&stLastReadTimeValue , NULL);
            if ( iMesgSize == 0 )
            {
                puts("Connection closed..");
                /* sock connection closed at other end */
                break;
            }

            if ( iMesgSize < 0 )
            {
                if ( errno == EINTR)
                {
                    continue;
                }
                perror("ICP FAILED..  ");
               ICP_SendPkt(iSocketFd , ICPROC ,gpcErrorMsg[0] , 
                           &gstSendICPCmdPkt);
               ICP_SendPkt(iSocketFd , ICPROC ,gcPrompt , &gstSendICPCmdPkt);
               eRetValue = NASC_FAILURE;
               break;
            }
        }
        iFirstTime = FALSE;

        ICP_ConvertCmdToHostForm( pstCmdPkt , pstCmdPkt);

        if ( pstCmdPkt->iDataLen == 0 )
        {
            ICP_SendPkt(iSocketFd , ICPROC ,gcPrompt , &gstSendICPCmdPkt);
            continue;
        }

        iTotalCmdSize += pstCmdPkt->iDataLen;
   
        if ( pucScratchForCmd == NULL)
        {
            iTotalCmdSize += 1;
            pucScratchForCmd = (char *) malloc (sizeof(char) *
                                                iTotalCmdSize );
            if ( pucScratchForCmd == NULL)
            {
                break;  
            }
            memset(pucScratchForCmd , '\0' , sizeof(char) * iTotalCmdSize);
        }
        else
        {
            pucScratchForCmd = (char *) realloc ( pucScratchForCmd , 
                                                  sizeof(UI8) * iTotalCmdSize);
        }

        strncat(pucScratchForCmd , pstCmdPkt->ucData , pstCmdPkt->iDataLen);
        pucScratchForCmd[iTotalCmdSize - 1] = '\0';
        if ( pstCmdPkt->iLastPkt != TRUE)
        {
            memset(pstCmdPkt , '\0' , sizeof(stCmdPacket));
            continue;
        }
        /* pucScratchForCmd = (char *) realloc ( pucScratchForCmd , 
                                            sizeof(UI8) * (iTotalCmdSize + 1)); */

       iTotalCmdSize = 0;
        
       /* check if the given command is disallowed */ 

        if ( ICP_CheckCmdInList( &gstIcpList , pucScratchForCmd , 
                             strlen(pucScratchForCmd)) != NULL)
        {
            ICP_SendPkt(iSocketFd , ICPROC ,gpcErrorMsg[2] ,&stSendCmdPkt);
            ICP_SendPkt(iSocketFd , ICPROC  ,gcPrompt ,&stSendCmdPkt);
            FREE_PTR(pucScratchForCmd);
            continue;
        }    


        eRetValue = ICP_ExecCommand ( iSocketFd , pucScratchForCmd , 
                                      &stSendCmdPkt , iPipeId);

        if ( eRetValue != NASC_SUCCESS)
        {
            ICP_SendPkt(iSocketFd , ICPROC ,gpcErrorMsg[0] ,&stSendCmdPkt);
        }
        	printf("Command successfully executed..\n");

        ICP_SendPkt(iSocketFd , ICPROC  ,gcPrompt ,&stSendCmdPkt);

        FREE_PTR (pucScratchForCmd);
        memset(pstCmdPkt , '\0' , sizeof(stCmdPacket));
    }
    FREE_PTR(pucScratchForCmd);
    return eRetValue;
}

/*****************************************************************************
 *
 * name             :   ICP_InitSNMPSession
 * description      :   This function initialises SNMP session for accessing
 *                      mib variables
 * input parameters :   None
 *
 * output parameters:   struct snmp_session * pstSnmpSession  snmp session
 *                      
 * return type      :   [int]
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

void ICP_InitSNMPSession(void)
{
    struct snmp_session stTempICPAgentSess;
    UI8 * pcSnmpAgent = NULL;
    UI8 cNASSACommunityICP [7];
    struct session_list1 *stSession_List = NULL;


    pcSnmpAgent = (UI8 *) malloc ( strlen(gpcAgentSessPeerName) + 
                                   + 5 + 3);

    if ( pcSnmpAgent == NULL)
    {
        return;
    }

    memset(pcSnmpAgent , '\0' , strlen(gpcAgentSessPeerName) +  
                                     + 5 + 3);

    memset(cNASSACommunityICP , '\0' ,   7);
   
    memcpy(cNASSACommunityICP , "public" , 6);

    sprintf(pcSnmpAgent , "%s:%d" , gpcAgentSessPeerName , 
                                    ICP_SNMP_LISTEN_PORT);

    /* For Session Initiation */

    snmp_sess_init(&stTempICPAgentSess);


    stTempICPAgentSess.version = SNMP_VERSION_2c;
    stTempICPAgentSess.peername = pcSnmpAgent;
    stTempICPAgentSess.community = cNASSACommunityICP;
    stTempICPAgentSess.community_len = strlen(cNASSACommunityICP);
    stTempICPAgentSess.callback_magic = NULL;
    stTempICPAgentSess.callback = NULL;

    SOCK_STARTUP;
 
    gpstSnmpSessionICP = snmp_sess_open(&stTempICPAgentSess);
    stSession_List = (struct session_list1 *) gpstSnmpSessionICP;
     if ( stSession_List->transport != NULL)
     {
         printf("Trap sock :%d\n" , stSession_List->transport->sock); 
     }
 
    if(gpstSnmpSessionICP == NULL) 
    {
        printf("Error opening Session\n");
        snmp_perror("NASCenterICP Snmp Open: ");
        exit(1);
    }
    FREE_PTR(pcSnmpAgent);
    printf("Got SNMP session..\n");
}

/*****************************************************************************
 *
 * name             :   ICP_GetICPTable
 * description      :   This function gets ICP table variables
 * input parameters :   stIcpList * pstIcpList  ICP list
 *
 * output parameters:   None  
 *                      
 * return type      :   ENASCenterError
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

ENASCenterError ICP_GetICPTable(stIcpList * pstIcpList) 
{
    struct snmp_pdu *pstSnmpPdu = NULL;
    struct snmp_pdu *pstSnmpResponse = NULL;
    stIcpTableCtx   *pstICPTableCtx = NULL;
    netsnmp_variable_list *pstVariableList = NULL;
    BOOL iEndOfTable = FALSE;
    I32 iStatus = 0;
    ENASCenterError eRetVal = NASC_SUCCESS;
    I32 i = 0;
    oid *icpCommandId = ICPCOMMAND_OID;
    size_t icpCommandId_Len = ICPCOMMAND_OID_LEN;
	oid *icpRowStatus = ICPROWSTATUS_OID;
	size_t icpRowStatus_Len = ICPROWSTATUS_OID_LEN;
	netsnmp_variable_list *pstCmd = NULL;
	I16 iRowStatus;
    

    if (gpstSnmpSessionICP == NULL)
    {
        return NASC_FAILURE;
    }


    while ( iEndOfTable != TRUE)
    {
        pstSnmpPdu = snmp_pdu_create(SNMP_MSG_GETBULK);

        snmp_add_null_var(pstSnmpPdu , icpCommandId , icpCommandId_Len);
		snmp_add_null_var(pstSnmpPdu , icpRowStatus , icpRowStatus_Len);

        pstSnmpPdu->non_repeaters = 0;
        pstSnmpPdu->max_repetitions = MAX_REPETITIONS;

        netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, 
                           NETSNMP_DS_LIB_OID_OUTPUT_FORMAT,
                           NETSNMP_OID_OUTPUT_NUMERIC); 

		if(pstSnmpResponse != NULL)
		{
			snmp_free_pdu(pstSnmpResponse);
			pstSnmpResponse = NULL;
		}
        iStatus = snmp_sess_synch_response(gpstSnmpSessionICP, pstSnmpPdu, 
                                            &pstSnmpResponse);
    
        printf("Got response .. \n");

        if ( iStatus == STAT_SUCCESS && 
            pstSnmpResponse->errstat == SNMP_ERR_NOERROR) 
        {
            pstVariableList = pstSnmpResponse->variables;

            if ( pstVariableList == NULL)
            {
                eRetVal = NASC_FAILURE;
                break;
            }
            
    
            while (pstVariableList != NULL)                  
            {
                if ( pstVariableList->type == SNMP_NOSUCHOBJECT || 
                     pstVariableList->type == ASN_NULL )
                {
                    continue;
                }

                if ( pstVariableList->type == SNMP_ENDOFMIBVIEW)
                {
                    printf("End..of..mib\n");
                    iEndOfTable = TRUE;
                }

                if ( netsnmp_oid_is_subtree(ICPTABLE_OID ,
                                            ICPTABLE_OID_LEN ,
                                            pstVariableList->name , 
                                            pstVariableList->name_length))
                {
                    printf("Other oid\n");
                    iEndOfTable = TRUE;
                    break;
                }

                if ( netsnmp_oid_equals ( pstVariableList->name , 
                                          pstVariableList->name_length - 1,
                                          ICPCOMMAND_OID , ICPCOMMAND_OID_LEN)
                                          == 0)
                {					
					pstCmd = pstVariableList;
					pstVariableList = pstVariableList->next_variable;
					
					if ( netsnmp_oid_equals ( pstVariableList->name,
						pstVariableList->name_length - 1,
						ICPROWSTATUS_OID , ICPROWSTATUS_OID_LEN) == 0 )
					{								
						iRowStatus = (I16)(*pstVariableList->val.integer);
						icpRowStatus = pstVariableList->name;
						icpRowStatus_Len = pstVariableList->name_length;
						if (iRowStatus == 1)
						{
							printf("adding Command to list\n");
							pstICPTableCtx = ICP_CreateICPNode(
								pstCmd->val.string,
								pstCmd->val_len);
							ICP_AddToCntx ( pstIcpList , pstICPTableCtx);
						}
					}	
					icpCommandId = pstCmd->name;
					icpCommandId_Len = pstCmd->name_length;
                }
				pstVariableList = pstVariableList->next_variable;
            }
        }
        else 
        {
            perror("Response failed.. : ");
            printf ( "Errno : %d\n" , errno);
            eRetVal = NASC_FAILURE;
            break;
        }    
        i++;
	}
	if(pstSnmpResponse != NULL)
	{
		snmp_free_pdu(pstSnmpResponse);
		pstSnmpResponse = NULL;
	}
    return eRetVal;
}

/*****************************************************************************
 *
 * name             :   ICP_updateList
 * description      :   This function reloads ICP context. 
                        This is threaded function .
 * input parameters :   None
 *
 * output parameters:   None  
 *                      
 * return type      :   void
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

void ICP_updateList ( void )
{
    stIcpList stICPList;    
    ENASCenterError eRetVal = NASC_SUCCESS;
    stMessage iMessage;

    /* printf("Group id of child thread : %ld\n" , getpgrp()); */

    while ( 1 )
    {

        printf("Inside Thread Waiting for message....\n");
   
        iMessage.lMesgType = giChildId;

        msgrcv(giMsgQueueId , (void *) &iMessage , sizeof(stMessage) , 
               giChildId , 0);

        /* wait till we get SIGUSR1 */
        /* This part is commented out  'coz of erratic behavior . 
           Instead message queue is used as IPC . Code is retained for 
           future analysis*/

       /*  sigemptyset(&signal_to_catch);
        sigaddset(&signal_to_catch , SIGUSR1);
        sigwait(&signal_to_catch , (int *) &caught);


        printf("Got signal %d\n" , (int) caught); */

        printf("Got message .........\n");

        ICP_InitICPList(&stICPList);

        eRetVal = ICP_GetICPTable ( &stICPList);

        if (eRetVal == NASC_SUCCESS)
        {
            ICP_DelAllFromCtx ( &gstIcpList);

            gstIcpList.stHeader = stICPList.stHeader;
       
            stICPList.stHeader = NULL;

        }
        else 
        {
           ICP_DelAllFromCtx(&stICPList);
        } 

        printf("end update list \n");
    }

    pthread_exit((void *) 0);


}


/* End of filename.c [ Change filename.c ] */

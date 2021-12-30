/*****************************************************************************
 *
 * NASCenter_ICP_main.c: [EDIT FILE NAME]
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Aravind Balan 
 *
 * description :
 *      Main file for ICP / File Browsing modules
 *
 * TODO:
 *      [ Things to be improved ]
 *
 * CM Details:
 *
 * $Author: sspriya $
 * $Log: NASCenter_ICP_main.c,v $
 * Revision 1.33  2004/06/16 13:43:25  sspriya
 * Fixed client system blackout problem in ICP. ICP will now detect any connection problem with the client system and exits from the forked child.
 *
 * Revision 1.32  2004/03/30 04:42:48  sspriya
 * Fixed a memory leak by freeing the response PDU. Reviewed by VijaiGeetha.
 *
 * Revision 1.31  2004/02/18 11:01:00  sspriya
 * Resolved a memory allocation problem caused as the size in malloc was
 * calculated based on uninitialised string's length.
 * Reviewed by Vijaigeetha
 *
 * Revision 1.30  2004/02/10 08:42:46  sspriya
 * Using localhost for trap port instead of IP address.
 * Reviewed by Karthik
 *
 * Revision 1.29  2004/02/05 10:44:59  vijaige
 * Added GetLocalHostIp() procedure and using the same to get the hostname
 * for AgentPeerSession name.
 *
 * Revision 1.28  2004/01/14 06:37:49  baravind
 * Brought alarm call inside while loop
 *
 * Revision 1.27  2004/01/13 05:07:36  baravind
 * Updates after a round of purify test
 *
 * Revision 1.26  2004/01/12 04:58:46  sig
 * After testing LINUX
 *
 * Revision 1.25  2004/01/09 06:30:00  baravind
 * Fixed warnings
 *
 * Revision 1.24  2004/01/07 06:59:58  baravind
 * Removed some unnecessary printfs
 *
 * Revision 1.23  2004/01/07 05:45:10  baravind
 * Big Change !!! Catching signals in pthread behaves erratically . Changed to  message queue instead of signals for sending trap to child process.
 *
 * Revision 1.22  2004/01/07 05:30:02  baravind
 * *** empty log message ***
 *
 * Revision 1.21  2004/01/06 10:53:53  baravind
 * Added htonl / ntohl to enable ICP get commands from different OS
 *
 * Revision 1.20  2004/01/05 09:44:29  baravind
 * changed from size_t -> socklen_t
 *
 * Revision 1.19  2004/01/05 07:06:14  baravind
 * fixed Agent Info get
 *
 * Revision 1.18  2004/01/05 05:30:21  baravind
 * Fixed Code review comments
 *
 * Revision 1.17  2003/12/29 12:15:01  sig
 * *** empty log message ***
 *
 * Revision 1.16  2003/12/25 13:08:24  baravind
 * Added by Aravind.B . Fixed RUN command problem
 *
 * Revision 1.15  2003/12/25 12:54:52  baravind
 * Added by Aravind.B. Fixed Log file creation problem
 *
 * Revision 1.14  2003/12/25 11:54:29  baravind
 * Added by Aravind.B . Fixed agentinfo
 *
 * Revision 1.13  2003/12/25 07:58:39  baravind
 * *** empty log message ***
 *
 * Revision 1.12  2003/12/25 05:39:04  sig
 * Added by Aravind.B . Removed printfs
 *
 * Revision 1.11  2003/12/24 13:25:02  sig
 * Added by Aravind.B . Fix for Linux . Changed from popen -> system
 *
 * Revision 1.10  2003/12/20 04:12:05  baravind
 * Added by Aravind.B on Dec-20-2003 .Fixed bugs
 *
 * Revision 1.9  2003/12/20 02:47:03  baravind
 * Added by Aravind.B on Dec-20-2003 . Fixed ST bugs.
 *
 * Revision 1.8  2003/12/18 07:44:04  baravind
 * Added by Aravind.B on Dec-18-2003 . Changed inet_pton -> inet_addr
 *
 * Revision 1.7  2003/12/17 08:16:32  baravind
 * Added by Aravind.B on Dec-17-2003 . Changed peer name to localhost
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
 * $Id: NASCenter_ICP_main.c,v 1.33 2004/06/16 13:43:25 sspriya Exp $
 * $Name:  $
 *
 ****************************************************************************/


/********************************* includes *********************************/

#ifndef NASCENTER_ICP_H
#include <NASCenter_ICP.h>
#endif


#include <sys/wait.h>
#include <NASCenter_ICP_AgentOid.h>
#include <sys/acct.h>



/****************************** local macros ********************************
 *
 * (Macros shareable with rest of the modules are present in .h file)
 */

 /* AgentInfo Table */

  #define AGENT_IP            agentInfoIP_variables_oid
 #define AGENT_NAME          agentInfoName_variables_oid
 #define AGENT_FILE_TRAS_IP agentInfoFileTransferIP_variables_oid
 #define AGENT_FILE_TRANS_PORT agentInfoFileTransferPortNo_variables_oid
 #define AGENT_LOGFILE_NAME   agentInfoLogFileCommand_variables_oid 
 #define AGENT_FILE_TRANS_INT agentInfoFileTransferInterval_variables_oid

 #define AGENT_FILE_TRAS_IP_LEN agentInfoFileTransferIP_oid_len
 #define AGENT_FILE_TRANS_PORT_LEN agentInfoFileTransferPortNo_oid_len
 #define AGENT_LOGFILE_NAME_LEN agentInfoLogFileCommand_oid_len
 #define AGENT_FILE_TRANS_INT_LEN agentInfoFileTransferInterval_oid_len
 #define AGENT_IP_LEN agentInfoIP_oid_len
 #define AGENT_NAME_LEN agentInfoName_oid_len


 #ifdef LINUX
   #define ACCNT_FILE "/var/log/wtmp" 
 #else
   #define ACCNT_FILE "/var/adm/pacct" 
 #endif


/********************* local structure/union definitions ********************/



/********************* prototypes for local functions ***********************/

void ICP_SigChld_Hndler ( int  iSignalNo);
void ICP_HandleSIGUSER ( int isignalNo );
void ICP_SendLogFile ( void );
void ICP_ProcessRequest ( I32, struct sockaddr_in );
void ICP_InitTrapSession(void);
struct snmp_session *  ICP_InitAgentSession(void);
ENASCenterError ICP_GetAgentInfo ( stAgentInfo *pstAgentDetails ,
                                   struct snmp_session * pstAgentSession);
void ICP_Enable_Accounting ( UI8 * pcLogFileName);
int ICP_ListenTrap(int id, struct snmp_session *sess, int req, 
                   netsnmp_pdu *pdu, void *arg);
void ICP_ReadTrap(void * arg);
void ICP_FillInPidArray(I32 iChildPid);
I32 ICP_GetPidLocInArray(I32 iChildPid);
void GetLocalHostIp(char *pcSysIp);
int ICP_checkalive(struct sockaddr_in *stClientAddress);
int ICP_GetRTT(struct sockaddr_in *  pstDestAddr);
ENASCenterError ICP_process_pkt (UI8 * pcRecvPkt , I32 iRecvMsgLen ,
                                        I32 iIdentifier ,
                                        struct timeval * pstTimeValue);
I32 ICP_Checksum ( UI16 *pusPacket , I32 iPktLen);
void ICP_GetTimeDiff (struct timeval *pstOutTime , 
                              struct timeval *pstInTime);



/************************* Globals within this module ***********************/

  void  *gpstTrapSession = NULL;
  struct snmp_session  *gpstAgentSession = NULL;
  pid_t giProcessGroupId ;
  stAgentInfo gstAgentDetails;
  I32 giNoOfConnections = 0;

  UI8  gpcAgentSessPeerName[20];
  UI8  gcNASSACommunity []= "public";

  UI8 gcPrompt [] = "$";
  UI8 gcPktDelimiter [] = "%";
  UI8 gEqCmdDelimiter [] = "$";
  UI8 *gpcErrorMsg[] = 
                {
                    "unable to process the command" ,
                    "Session interupted .. closing session" ,
                    "Command not allowed at this time"
                } ;

  /* Equivalent command array for command types in socketlisting sheet */

  stEquivalentCommand  gpcEquivalentCmd [] = 
                          {
                              "ls" , 0 ,
                              "ls -l $" , 1 ,
                              "cp $ $" , 2 ,
                              "./$" , 1 ,
                              "mv $ $" , 2 ,
                              "mkdir $" , 1 ,
                              "tar -cvf $/$ $" , 3 ,
                              "chmod g$,u$ $" , 3 ,
                              "" , 1 ,
                              "useradd -u $ -p $ -G $ -c $ -d $ $" , 6 ,
                              "usermod -u $ -p $ -G $ -c $ $" , 5 ,
                              #ifdef AIX
                               "rmuser -p $" , 1 ,
                              #else
                                "userdel -r $" , 1 ,
                              #endif
                              "" , 1,
                              #ifdef AIX
                                "mkgroup id=$ $" , 2 ,
                                "chgroup id=$ $" , 2 ,
                                "rmgroup $" , 1 , 
                              #else  
                                "groupadd -g $ $" , 2 ,
                                "groupmod -g $ $" , 2 ,
                                "groupdel  $" ,1 ,
                              #endif

                          };

  UI8 gcErrorFile[] = "./ErrFile";

  I32 gTrapSockId;

  I32 giMsgQueue;

  BOOL eFinished = FALSE;
  struct timeval stLastReadTimeValue;


struct session_list {
  struct session_list *next;
  netsnmp_session *session;
  netsnmp_transport *transport;
  struct snmp_internal_session *internal;
};

I32 giChildProcIdArray[MAX_ICP_SESSIONS];




/************************* externs  ***********************/


/*****************************************************************************
 *
 * name             :   ICP_SigChld_Hndler
 * description      :   This is Handler for SIGCHLD 
 * input parameters :   int signo  signal no
 *
 * output parameters:   None  
 *                      
 * return type      :   [int]
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

 void ICP_SigChld_Hndler ( int isignalNo )
 {
     I32 iStatus ;
     I32 iPid;
     I32 iLocation;
   
     /* wait for child termination */

     while ((iPid = waitpid(-1 , (int *) &iStatus , WNOHANG))  > 0)
     {
        iLocation = ICP_GetPidLocInArray(iPid);

        /* if the terminated child is in parent's process id array 
           decrement no of sessions (connections)  */

        if ( iLocation != -1 )
        {
            giNoOfConnections -- ;
        }
     }        
     ICP_Signal ( SIGCHLD , ICP_SigChld_Hndler);
     return;
 }
/*****************************************************************************
 *
 * name             :   ICP_HandleSIGUSER
 * description      :   This is Handler for SIGUSR1
 * input parameters :   int isignalNo  Signal no
 *
 * output parameters:   None  
 *                      
 * return type      :   None
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

 void ICP_HandleSIGUSER ( int isignalNo )
 {

     ICP_Signal ( SIGUSR1 , ICP_HandleSIGUSER);
     return;
 }

/*****************************************************************************
 *
 * name             :   ICP_Signal
 * description      :   This function installs signal desposition for a signal 
 * input parameters :   I32 iSigNo  Signal no
 *                      sigHndlr * pSignalHndlr Handler function for the signal
 *
 * output parameters:   None  
 *                      
 * return type      :   sigHndlr *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

 sigHndlr * ICP_Signal ( I32 iSigNo , sigHndlr * pSignalHndlr)
 {
     struct sigaction stSignalAction;
     struct sigaction stPrevSignalAction;

     stSignalAction.sa_handler = pSignalHndlr;

     sigemptyset(&stSignalAction.sa_mask);

     stSignalAction.sa_flags = 0;

     if ( sigaction(iSigNo , &stSignalAction , &stPrevSignalAction) < 0 )
     {
         return SIG_ERR;
     }

     return stPrevSignalAction.sa_handler;
}

/*****************************************************************************
 *
 * name             :   ICP_FillInPidArray
 * description      :   This function stores child id  in processid array
 * input parameters :   I32 iChildPid  Child process id
 *
 * output parameters:   None  
 *                      
 * return type      :   void
 * global variables :   giChildProcIdArray  process Id array
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

void ICP_FillInPidArray(I32 iChildPid)
{
    I16 iLoopVar1;

     for ( iLoopVar1 = 0 ; iLoopVar1 < MAX_ICP_SESSIONS ; iLoopVar1++)
     {
         if ( giChildProcIdArray[iLoopVar1] == 0 )
         {
              giChildProcIdArray[iLoopVar1] = iChildPid;
              break;
         }
    }
}

/*****************************************************************************
 *
 * name             :   ICP_GetPidLocInArray
 * description      :   This function checks if the child process is in the 
 *                      process id array
 * input parameters :   I32 iChildPid child id
 *
 * output parameters:   None  
 *                      
 * return type      :  I32  childId   
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

I32 ICP_GetPidLocInArray(I32 iChildPid)
{
    I16 iLoopVar1;
    I32 iLoc = -1;

     for ( iLoopVar1 = 0 ; iLoopVar1 < MAX_ICP_SESSIONS ; iLoopVar1++)
     {
         if ( giChildProcIdArray[iLoopVar1] == iChildPid )
         {
              iLoc = iChildPid;
              break;
         }
    }
    return iLoc;
}
    


/*****************************************************************************
 *
 * name             :   main
 * description      :   This is entry point for ICP process
 * input parameters :   none
 *
 * output parameters:   None  
 *                      
 * return type      :   [int]
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

 int  main (void )
 {
     struct sockaddr_in stICPAdress;
     struct sockaddr_in stClientAdress;
     I32 iListenSockFd = 0;
     I32 iConnectSockFd = 0;
     I32 iClientAddrLen = sizeof(struct sockaddr_in);
     I32 iAddrReuseFlag = 1;
     I32 iChildPid;
     I32 fds=-1;
     pthread_t  iTrapThreadId;
     ENASCenterError eRetVal = NASC_SUCCESS;
     I32 iProcessId;
     int i;
     sigset_t  signalSet;

     giMsgQueue = msgget(MESSAGE_Q , 0666 | IPC_CREAT);
     printf("Message ! id %ld\n" , giMsgQueue);

     if ( giMsgQueue < 0 )
     {
         printf("Message Queue cannot be created..\n");
     }

     sigfillset ( &signalSet );


     sigdelset (&signalSet , SIGCHLD );
     sigdelset (&signalSet , SIGINT );

     pthread_sigmask ( SIG_BLOCK, &signalSet, NULL); 

     /* Get Process id */

     iProcessId = getpid();

     /* set Process Group Id */

     giProcessGroupId = setpgid(iProcessId , iProcessId);

     if (giProcessGroupId == -1 )
     {
         perror("Unable to get process group");
         exit(0);
     }

     giProcessGroupId =  getpgrp();


     /* Init Trap session */
 
     ICP_InitTrapSession();

     /* Init Snmp session */
 
     gpstAgentSession = ICP_InitAgentSession();


     /* Get AgentInfo Table */

     eRetVal = ICP_GetAgentInfo(&gstAgentDetails , gpstAgentSession);

     if ( eRetVal == NASC_SUCCESS)
     {
       /* enable accounting */
       printf("Log file interval %ld\n" , gstAgentDetails.iTransInterval);
       ICP_Enable_Accounting(ACCNT_FILE);

     }

      /* Spawn thread for Processing Trap */

      pthread_create(&iTrapThreadId , NULL , (void *) ICP_ReadTrap , 
                    (void *) NULL);  

      /* Spawn thread for SEnding Logn file  */

      pthread_create(&iTrapThreadId , NULL , (void *) ICP_SendLogFile , 
                     (void *) NULL);  

     /* fill in socket address structure */

     bzero(&stICPAdress , sizeof(stICPAdress));

     stICPAdress.sin_family = AF_INET;
     stICPAdress.sin_port = htons(NASC_ICP_PORTNO);
     stICPAdress.sin_addr.s_addr = ntohs(INADDR_ANY);

     iListenSockFd = socket(AF_INET , SOCK_STREAM , 0);

     if (setsockopt(iListenSockFd, SOL_SOCKET, SO_REUSEADDR,
                       &iAddrReuseFlag, sizeof(iAddrReuseFlag)) < 0)
     {
          perror("Set Sock Option Failed :  ");
          msgctl(giMsgQueue , IPC_RMID , (struct msqid_ds *) 0);
          exit (0);
     }

     /* bind the socket */

     if (bind(iListenSockFd ,(struct sockaddr *) &stICPAdress , 
              sizeof(stICPAdress)) != 0)
     {
         perror("Socket bind Failed : ");
         msgctl(giMsgQueue , IPC_RMID , (struct msqid_ds *) 0);
         exit(0);
     }


     /* Listen for connections */

     listen(iListenSockFd , NASC_MAX_BACKLOG);

     /* set Signal handlers */

     ICP_Signal ( SIGCHLD , ICP_SigChld_Hndler); 
     ICP_Signal ( SIGUSR1 , ICP_HandleSIGUSER); 

     /* accept connections */

     while ( 1 ) 
     {
         #ifndef UNIXWARE
         iConnectSockFd = accept (iListenSockFd , 
                                 (struct sockaddr *) &stClientAdress , 
                                 (socklen_t *) &iClientAddrLen);
         #else
         iConnectSockFd = accept (iListenSockFd , 
                                 (struct sockaddr *) &stClientAdress , 
                                 (size_t *) &iClientAddrLen);
         #endif
         if ( iConnectSockFd < 0 )
         {
             if ( errno == EINTR )
             {
                 puts("accetp Interupted..");
                 continue ;
              }
         }

         if (giNoOfConnections > MAX_ICP_SESSIONS)
         {
             close(iConnectSockFd);
             continue;
         }
         if ((iChildPid = fork()) == 0)
         {
             fds = 3;
             for ( i = fds ; i < 64 ; i++ )
             {
                 if ( i != iConnectSockFd )
                     close( i );
             }
             close(gTrapSockId);
             close ( iListenSockFd );
             ICP_ProcessRequest ( iConnectSockFd, stClientAdress );
             close ( iConnectSockFd);
             exit(0);
         }
         else if (iChildPid < 0 )
         {
             /* sysLog */
         }
         else 
         {
             /* set process groupid of child to parent's process group id */
             setpgid(iChildPid , giProcessGroupId);
             ICP_FillInPidArray(iChildPid);
             giNoOfConnections ++ ;
         }
    } 
    msgctl(giMsgQueue , IPC_RMID , (struct msqid_ds *) 0);
    return 0;
}
     
/*****************************************************************************
 *
 * name             :   ICP_ProcessRequest
 * description      :   This function process ICP/FileBrowse/UserCommand 
 *                      request from other end
 * input parameters :   [datatype arg1]  - description for arg1
 *                      [datatype arg2]  - description for arg2
 * output parameters:   [datatype var1]  - description for var1
 *                      [datatype var2]  - description for var2, etc..
 * return type      :   [int]
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 *****************************************************************************/

 void ICP_ProcessRequest ( I32 iSocketFd, struct sockaddr_in stClientAddress )
 {
     I32 iMesgSize = 0;
     I32 ModuleId = 0;
     // BOOL eFinished = FALSE; Moved to global
     BOOL eInvalidMdoule = FALSE; 
     stCmdPacket stCommandPkt ;
     stCmdPacket stCommandPktInHstForm ;
     pthread_t th_chkalive;

     memset(&stCommandPkt , '\0' , sizeof(stCmdPacket));

     gettimeofday(&stLastReadTimeValue , NULL);
     pthread_create(&th_chkalive,NULL,(void*)ICP_checkalive,(void*)&stClientAddress);
     /* Receive command */
    while ( eFinished == FALSE  && eInvalidMdoule == FALSE) 
    {
        iMesgSize = read(iSocketFd , &stCommandPkt , 
                         sizeof(stCmdPacket));
         if ( iMesgSize == 0)
         {
             break;
         }

        if ( iMesgSize < 0 )
        {
            if ( errno == EINTR )
            {
                continue;
            }
        }

        else if ( iMesgSize > 0 )
        {
            ICP_ConvertCmdToHostForm (&stCommandPkt , &stCommandPktInHstForm);
            ModuleId = stCommandPktInHstForm.iModuleId;
            switch ( ModuleId ) 
            {
                case ICPROC : 
                    ICP_ProcessICPCommand ( iSocketFd , 
                                            &stCommandPkt);
                    eFinished = TRUE;
                     break;      

                case FILBRWS : 
                     ICP_ProcessFileBrwsCmd ( iSocketFd , 
                                             &stCommandPkt); 
                    eFinished = TRUE;
                    break;
                
                case USRMGMT :
                     ICP_ProcessUserMGMTCmd ( iSocketFd ,
                                             &stCommandPkt); 
                    eFinished = TRUE;
                    break;

                default     :
                    eInvalidMdoule = TRUE;
                    break;
            }
        }
    }
    memset(&stCommandPkt , '\0' , sizeof(stCommandPkt));
}

/*****************************************************************************
 *
 * name             :   ICP_InitTrapSession
 * description      :   This function initialises Trap session
 * input parameters :   None
 *
 * output parameters:   struct snmp_session * pstTrapSession Trap session
 *                      
 * return type      :   void
 * global variables :   gpcAgentSessPeerName
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

void  ICP_InitTrapSession()
{
     struct snmp_session stTempTrapSess;
     void *pstTrapSession;
     UI8 *pcTrapAgent = NULL;
     struct session_list *stSession_List = NULL;

     pcTrapAgent = (UI8 *) malloc ( strlen("localhost") + 
                                   sizeof(I32) + 2);

    if ( pcTrapAgent == NULL)
    {
        return;
    }

    memset(pcTrapAgent , '\0' , strlen("localhost") +  
                                    sizeof(I32) + 2);

    sprintf(pcTrapAgent , "%s:%d" , "localhost" , 
                                    ICP_TRAP_LISTEN_PORT);


    snmp_sess_init(&stTempTrapSess);


    stTempTrapSess.version = SNMP_VERSION_2c;
    stTempTrapSess.peername = pcTrapAgent;
    stTempTrapSess.local_port = ICP_TRAP_LISTEN_PORT;
    stTempTrapSess.community = gcNASSACommunity;
    stTempTrapSess.community_len = strlen(gcNASSACommunity);
    stTempTrapSess.callback_magic = NULL;
    stTempTrapSess.callback = ICP_ListenTrap;

    SOCK_STARTUP;

    pstTrapSession = snmp_sess_open(&stTempTrapSess);
 
    if(pstTrapSession == NULL) 
    {
        snmp_perror("NASCenterICP Snmp Trap Open: ");
        exit(1);
    } 
    gpstTrapSession = pstTrapSession;
    stSession_List = (struct session_list *) pstTrapSession;
    if ( stSession_List->transport != NULL)
    {
        gTrapSockId = stSession_List->transport->sock;
    }
    FREE_PTR(pcTrapAgent);
   
}

/*****************************************************************************
 *
 * name             :   ICP_InitAgentSession
 * description      :   This function initialises SNMP session
 * input parameters :   None
 *
 * output parameters:   struct snmp_session * pstAgentSession SNMP session
 *                      
 * return type      :   void
 * global variables :   gpcAgentSessPeerName  , gcNASSACommunity
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/
struct snmp_session * ICP_InitAgentSession(void )
{
    struct snmp_session stTempAgentSess;
    struct snmp_session * pstAgentSession;
    UI8 *pcSnmpAgent = NULL;


    memset(gpcAgentSessPeerName,0,20);
    GetLocalHostIp(gpcAgentSessPeerName);
	pcSnmpAgent = (UI8 *) malloc ( strlen(gpcAgentSessPeerName) + 
                                   sizeof(I32) + 3);

    if ( pcSnmpAgent == NULL)
    {
        return NULL;
    }

    memset(pcSnmpAgent , '\0' , strlen(gpcAgentSessPeerName) +  
                                    sizeof(I32) + 3);

    sprintf(pcSnmpAgent , "%s:%d" , gpcAgentSessPeerName , 
                                    ICP_SNMP_LISTEN_PORT);


    /* For Session Initiation */

    snmp_sess_init(&stTempAgentSess);


    stTempAgentSess.version = SNMP_VERSION_2c;
    stTempAgentSess.peername = pcSnmpAgent;
    stTempAgentSess.community = gcNASSACommunity;
    stTempAgentSess.community_len = strlen(gcNASSACommunity);
    stTempAgentSess.callback_magic = NULL;

    SOCK_STARTUP;
 
    pstAgentSession = snmp_open(&stTempAgentSess);
 
    if(pstAgentSession == NULL) 
    {
        snmp_perror("NASCenterICP Snmp Open: ");
        exit(1);
    }

    FREE_PTR(pcSnmpAgent);


    return pstAgentSession;
}
/*****************************************************************************
 *
 * name             :   ICP_GetAgentInfo
 * description      :   This function gets AgentInfo table variables
 * input parameters :   stAgentInfo *pstAgentDetails  AgentInfo Context
 *
 * output parameters:   struct snmp_session * pstTrapSession Trap session
 *                      
 * return type      :   void
 * global variables :   gpcAgentSessPeerName
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

ENASCenterError ICP_GetAgentInfo ( stAgentInfo *pstAgentDetails , 
                   struct snmp_session * pstAgentSession)
{
    
    struct snmp_pdu *pstSnmpPdu = NULL;
    struct snmp_pdu *pstSnmpResponse = NULL;
    netsnmp_variable_list *pstVariableList = NULL;
    ENASCenterError eRetVal = NASC_SUCCESS;
    I32 iStatus;
    UI8 cFileTransferIP[IP_ADDR_LEN + 1] ;

    pstSnmpPdu = snmp_pdu_create(SNMP_MSG_GET);


    snmp_add_null_var(pstSnmpPdu , AGENT_IP , 
                      AGENT_IP_LEN);

    snmp_add_null_var(pstSnmpPdu , AGENT_NAME , 
                      AGENT_NAME_LEN);

    snmp_add_null_var(pstSnmpPdu , AGENT_FILE_TRAS_IP , 
                      AGENT_FILE_TRAS_IP_LEN);

    snmp_add_null_var(pstSnmpPdu , AGENT_FILE_TRANS_PORT , 
                      AGENT_FILE_TRANS_PORT_LEN);

    snmp_add_null_var(pstSnmpPdu , AGENT_LOGFILE_NAME , 
                      AGENT_LOGFILE_NAME_LEN);

    snmp_add_null_var(pstSnmpPdu , AGENT_FILE_TRANS_INT , 
                      AGENT_FILE_TRANS_INT_LEN);

    /* pstSnmpPdu->non_repeaters = 0;
    pstSnmpPdu->max_repetitions = 1; */


    netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, 
                       NETSNMP_DS_LIB_OID_OUTPUT_FORMAT,
                       NETSNMP_OID_OUTPUT_NUMERIC);

    iStatus = snmp_synch_response(pstAgentSession, pstSnmpPdu, 
                                  &pstSnmpResponse);
    
    if ( iStatus == STAT_SUCCESS && 
         pstSnmpResponse->errstat == SNMP_ERR_NOERROR) 
    {
        pstVariableList = pstSnmpResponse->variables;

        if ( pstVariableList == NULL)
        {
            return NASC_FAILURE;
        }
        for (; pstVariableList ; 
               pstVariableList = pstVariableList->next_variable)
        {
            if ( pstVariableList->type == SNMP_NOSUCHOBJECT || 
                 pstVariableList->type == ASN_NULL )
            {
                continue;
            }

            if ( pstVariableList->type == SNMP_ENDOFMIBVIEW)
            {
                printf("End..of..mib\n");
				eRetVal = NASC_FAILURE;
				break; 
            }
            if ( netsnmp_oid_equals(pstVariableList->name , 
                                    pstVariableList->name_length - 1 , 
                                    AGENT_FILE_TRAS_IP ,
                                    AGENT_FILE_TRAS_IP_LEN - 1 ) == 0)
           {
               memset(cFileTransferIP , '\0' , IP_ADDR_LEN + 1);
               memcpy(cFileTransferIP , pstVariableList->val.string , 
                      pstVariableList->val_len);
               Util_GetIPFrom_ASNFormat(cFileTransferIP ,
                                        pstAgentDetails->cAgentFileTransIp);
           }

           if ( netsnmp_oid_equals(pstVariableList->name , 
                                   pstVariableList->name_length - 1 ,
                                   AGENT_FILE_TRANS_PORT , 
                                   AGENT_FILE_TRANS_PORT_LEN - 1) == 0)
           {
               pstAgentDetails->iAgentFileTransPort = 
                                             *(pstVariableList->val.integer);
           }

           if ( netsnmp_oid_equals(pstVariableList->name ,
                                   pstVariableList->name_length - 1 ,
                                   AGENT_NAME , AGENT_NAME_LEN - 1) == 0)
           {
               memcpy(pstAgentDetails->cAgentName , 
                      pstVariableList->val.string ,  pstVariableList->val_len);
           }

           if ( netsnmp_oid_equals(pstVariableList->name ,
                                   pstVariableList->name_length - 1 ,
                                   AGENT_FILE_TRANS_INT , 
                                   AGENT_FILE_TRANS_INT_LEN - 1) == 0)
           {
               pstAgentDetails->iTransInterval =  
                                           *(pstVariableList->val.integer);
           }
           if ( netsnmp_oid_equals(pstVariableList->name ,
                                   pstVariableList->name_length - 1 ,
                                   AGENT_LOGFILE_NAME , 
                                   AGENT_LOGFILE_NAME_LEN - 1) == 0)
           {
               memset(pstAgentDetails->cCmdLogFile , '\0' ,255);
               memcpy(pstAgentDetails->cCmdLogFile , 
                      pstVariableList->val.string ,  pstVariableList->val_len);
           }

                                    
        }
    }
    else 
    {
        eRetVal = NASC_FAILURE;
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
 * name             :   ICP_Enable_Accounting
 * description      :   This function Enables user accounting using Operating
 *                      system's accounting functionality
 * input parameters :   UI8 * pcLogFileName  Log file name
 *
 * output parameters:   None
 *                      
 * return type      :   void
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

void ICP_Enable_Accounting ( UI8 * pcLogFileName)
{

    I32 iStatus;
    I32 iFileDesc;
    struct stat  stFilestat;


    iStatus = stat(pcLogFileName , &stFilestat);

    iFileDesc = open(pcLogFileName , O_CREAT | O_RDWR);

    if ( iFileDesc < 0 )
    {
        return;
    }

    /* Enable command loggin */

    truncate(ACCNT_FILE , 0);
        
    acct(pcLogFileName);
}
/*****************************************************************************
 *
 * name             :   ICP_SendLogFile
 * description      :   This function sends Log file to manager at specified
 *                      Intervals 
 * input parameters :   None
 *
 * output parameters:   
 *                      
 * return type      :   void
 * global variables :   gpcAgentSessPeerName
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

void ICP_SendLogFile ( void )
{
 
    UI8 * pcFileName = NULL;
    struct sockaddr_in stSockAddr;
    sigset_t signal_to_catch;
    int  caught;
    ENASCenterError eRetValue ;
    I32 iFileId1 = 0;
    FILE *pFileptr = 0;
    struct acct stAccntInfo;



    while ( 1 )
    {

      printf("Wait for ALarm sig %ld.,.\n" , gstAgentDetails.iTransInterval);
      alarm ( gstAgentDetails.iTransInterval);


        /* wait till we get SIGALRM */
        sigemptyset(&signal_to_catch);
        sigaddset(&signal_to_catch , SIGALRM);
        sigaddset(&signal_to_catch , SIGINT);
        sigwait(&signal_to_catch , &caught);

        printf("Got sig %d\n" , caught);


        if ( caught == SIGINT)
        {
            exit(0);
        }

        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_port  = htons(gstAgentDetails.iAgentFileTransPort);
        stSockAddr.sin_addr.s_addr = inet_addr(
                                            gstAgentDetails.cAgentFileTransIp);

        pcFileName = gstAgentDetails.cCmdLogFile;

        acct(NULL);

        iFileId1 = open(ACCNT_FILE , O_RDONLY);

        if ( iFileId1 < 0 )
        {
            continue;
        }

        pFileptr = fopen(gstAgentDetails.cCmdLogFile , "w");


        if ( pFileptr == NULL)
        {
            continue;
        }

         while(read(iFileId1 , &stAccntInfo , sizeof(struct acct)) > 0)
         {
             fprintf( pFileptr, "%s : %s\n" , "User Name" , 
                                    (getpwuid(stAccntInfo.ac_uid))->pw_name);
             
             fprintf( pFileptr, "%s : %s\n" , "Command" , stAccntInfo.ac_comm);
         }
         if (  pFileptr != NULL)
         {
             fclose (pFileptr);
         }

        close(iFileId1);

        eRetValue = FileTrans_SendFile(gstAgentDetails.cCmdLogFile ,
                                             &stSockAddr , NULL , 512 );



       truncate(gstAgentDetails.cCmdLogFile , 0);
       truncate(ACCNT_FILE , 0);

       eRetValue = ICP_GetAgentInfo(&gstAgentDetails , gpstAgentSession);
       if ( eRetValue == NASC_SUCCESS)
       {
           ICP_Enable_Accounting(ACCNT_FILE);

           alarm ( gstAgentDetails.iTransInterval);
        }
    }

}
/*****************************************************************************
 *
 * name             :   ICP_ListenTrap
 * description      :   This is callback function for Trap . On getting trap
 *                      sends SIGUSR1 to child processes to update mib details
 * input parameters :   int id 
 *                      struct snmp_session *sess 
 *                      int req
 *                      netsnmp_pdu *pdu 
 *                      void *arg
 *
 * output parameters:   None
 *                      
 * return type      :   int 
 * global variables :  
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/
int ICP_ListenTrap(int id, struct snmp_session *sess, int req, netsnmp_pdu *pdu, void *arg)
{

   stMessage iMessage = { 0 };
   I16 iLoopVar1;

   
   /* Send SIGUSR1 to child process within parent group */
   printf("Inside trap \n");

   if ( giMsgQueue >= 0 )
   {
       for ( iLoopVar1 = 0 ; iLoopVar1 < MAX_ICP_SESSIONS ; iLoopVar1++)
       {
           if ( giChildProcIdArray[iLoopVar1] != 0 || 
              giChildProcIdArray[iLoopVar1] != -1)
           {
               iMessage.lMesgType = giChildProcIdArray[iLoopVar1];

               msgsnd(giMsgQueue , (void *) &iMessage , sizeof(stMessage) , 
                      IPC_NOWAIT);
           }
       }
       printf("Message Sent..\n");
    }


 //   kill(0 , SIGUSR1);
    return 1;
}

/*****************************************************************************
 *
 * name             :   ICP_ReadTrap
 * description      :   This function waits for Trap
 * input parameters :   void * arg
 *
 * output parameters:   None
 *                      
 * return type      :   void
 * global variables :   None 
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/
void ICP_ReadTrap(void * arg)
{
    fd_set snmpSet;
    int snmpFds ,blocking;
    struct timeval  tv;


    while( 1 )
   {
       snmpFds = 0;
       blocking = 1;
       FD_ZERO(&snmpSet);
       snmp_sess_select_info(gpstTrapSession , &snmpFds , &snmpSet , &tv , 
            &blocking);
       snmpFds = select(snmpFds , &snmpSet , 0 , 0 , 0);
       if (snmpFds == -1)
       {
           if ( errno == EINTR)
           {
               continue;
           }
           else
           {
               continue;
           }
        }
        if ( snmpFds)
        {
           snmp_sess_read(gpstTrapSession, &snmpSet);
        }
    }
}

void ICP_ConvertCmdToHostForm(stCmdPacket *pstCommandPkt ,
                              stCmdPacket *pstCmdPktInHstFrm)
{
   pstCmdPktInHstFrm->iModuleId = ntohl(pstCommandPkt->iModuleId);
   pstCmdPktInHstFrm->iCommandId = ntohl(pstCommandPkt->iCommandId);
   pstCmdPktInHstFrm->iLastPkt = ntohl(pstCommandPkt->iLastPkt);
   pstCmdPktInHstFrm->iDataLen = ntohl(pstCommandPkt->iDataLen);
}


/*****************************************************************************
 *
 * name             :   GetLocalHostIp
 * description      :   This function is used to get the IP address of the local
                        host machine
 * input parameters :   None
 * output parameters:   char * - returns the IP Address string
 * return type      :   void
 * global variables :   None
 * calls            :   None
 *****************************************************************************/
 
void GetLocalHostIp(char *pcSysIp)
{
    struct sockaddr_in stInAddr; // Common variable for Windows & Unix
    struct hostent *pstHostEnt;
 
#ifndef WIN32 // For Unix
 
    struct utsname stHostName;
 
    // Get the localhost name
    uname (&stHostName);
#ifdef ICP_DEBUG
    printf("Inside GetLocalhost Name: %s\n",stHostName.nodename);
#endif
    if ((pstHostEnt = gethostbyname(stHostName.nodename)) == NULL)
    {
        snmp_log_perror("error resolving hostname..");
        return;
    }
 
#else  // For windows
 
    WSADATA wsadata;
    char* stHostName;
    int namelen = 50;
 
    stHostName = (char*) malloc(namelen);
 
    WSAStartup(MAKEWORD( 2, 2 ),&wsadata);
 
    if (gethostname(stHostName,namelen)!=0)
    {
        snmp_log_perror("error resolving hostname..");
        return;
    }
	if ((pstHostEnt = gethostbyname(stHostName)) == NULL)
		return; 
#endif
 
    // Common code for Windows & Unix
    memcpy(&stInAddr.sin_addr, pstHostEnt->h_addr_list[0], pstHostEnt->h_length);
#ifdef ICP_DEBUG
    //printf("Inside GetHost sinaddr :%s\n",stInAddr.sin_addr);
#endif
 
    // Convert it to IP string
    strcpy(pcSysIp,(char*)inet_ntoa(stInAddr.sin_addr));
#ifdef ICP_DEBUG
    printf("ICP_GetLocalHostIp %s\n",pcSysIp); 
#endif

    return;
}

int ICP_checkalive(struct sockaddr_in *stClientAddress)
{
    struct timeval stCurTimeValue;
    
    while (1) {
        gettimeofday(&stCurTimeValue , NULL);
        ICP_GetTimeDiff(&stCurTimeValue, &stLastReadTimeValue);
        if (( stCurTimeValue.tv_sec * 1000.0 + stCurTimeValue.tv_usec / 1000.0 ) > ICP_IDLE_TIME ) {
            ICP_GetRTT(stClientAddress);
        }        
        sleep(ICP_PING_INTERVAL);
    }
}

int ICP_GetRTT(struct sockaddr_in *  pstDestAddr)
{
    int iSocketfd = 0;
    int iIdentifier = 0;
    UI8 cSendPkt[MAX_PKT_SIZE];
    UI8 cRecvPkt[MAX_PKT_SIZE];
    struct icmp *pstIcmpHdr = NULL;
    struct timeval stTimeOutValue;
    struct timeval stTimeValue;
    int iICMPpktSize =  DEF_ICMP_DATA_LEN + ICMP_HDR_LEN;
    I32 iSentMsgLen = 0;
    fd_set iFileDescSet;
    BOOL eReplyGot       = FALSE;
    I32  iSelectRetValue;
    struct sockaddr_in stFromaddr;
    size_t iSockAddrLen = 0;
    struct timeval stCurTimeValue;
    UI8 *pcIpaddress = NULL;
    UI8 *pcDestAddress = NULL;
    I32 iRecvMsgLen = 0;
    ENASCenterError eResult = NASC_SUCCESS;
    int iTimeOutCount = 0;

    /* Get process id to be used as identifier in the ICMP header */
    iIdentifier = getpid() & 0xffff;

    /* Create Raw socket */
    iSocketfd = socket(PF_INET , SOCK_RAW , IPPROTO_ICMP);

    if ( iSocketfd == -1 ) {
        return NASC_FAILURE;    
    }

    while(iTimeOutCount < 3 && eReplyGot == FALSE )
    {
    /* memset character array */
    memset(cSendPkt , '\0' , MAX_PKT_SIZE);
    memset(cRecvPkt , '\0' , MAX_PKT_SIZE);

    /* Fill in ICMP details */
    pstIcmpHdr = (struct icmp *) cSendPkt;
    pstIcmpHdr->icmp_type = ICMP_ECHO;
    pstIcmpHdr->icmp_id = iIdentifier;
    pstIcmpHdr->icmp_seq = 1;
    pstIcmpHdr->icmp_cksum = 0;
    pstIcmpHdr->icmp_code = 0;

    /* store the timestamp in data portion of ICMP pkt. This is needed
       for rtt calculation on recipt of ECHO_RESPONSE */
    gettimeofday(&stTimeValue , NULL);

    memcpy(pstIcmpHdr->icmp_data , &stTimeValue , sizeof(stTimeValue));

    /* Calculate ICMP checksum */
    pstIcmpHdr->icmp_cksum = ICP_Checksum((UI16 *) pstIcmpHdr ,
                                                iICMPpktSize);
    /* send ICMP tacket */
    iSentMsgLen =  sendto(iSocketfd, cSendPkt, iICMPpktSize, 0, 
                          (struct sockaddr *) pstDestAddr , sizeof(struct sockaddr));

    /* Set time out value for select */
    stTimeOutValue.tv_sec = ICMP_WAIT_TIME;
    stTimeOutValue.tv_usec = 0;

    /* set file discriptors to be checked for data */
    FD_ZERO(&iFileDescSet);
    FD_SET(iSocketfd , &iFileDescSet);

    /* set time before which response should be got */

        iSelectRetValue = select(iSocketfd + 1 ,&iFileDescSet , NULL ,
                                 NULL , &stTimeOutValue);

        if ( iSelectRetValue == 0)
        {
            iTimeOutCount++;
            eResult = NASC_ICP_TIME_OUT;
        }
        else if ( iSelectRetValue < 0 )
        {
            eResult = NASC_FAILURE;
            break;
        }
        else
        {
            if ( (iRecvMsgLen =
                 recvfrom(iSocketfd , cRecvPkt , MAX_PKT_SIZE ,  0,
                 (struct sockaddr *) &stFromaddr , &iSockAddrLen )) < 0 )
            {
            }
            else
            {
                gettimeofday(&stCurTimeValue , NULL);

                pcIpaddress =  inet_ntoa(stFromaddr.sin_addr);
                pcDestAddress =  inet_ntoa(pstDestAddr->sin_addr);
                if ( pcIpaddress != NULL && pcDestAddress != NULL )
                {
                    if ( strcmp(pcIpaddress ,  pcDestAddress) != 0)
                    {
                        continue;
                    }
                }
                eResult = ICP_process_pkt(cRecvPkt , iRecvMsgLen ,
                                                iIdentifier ,
                                                &stCurTimeValue);
                if ( eResult != NASC_SUCCESS &&
                     eResult != NASC_ICP_IGNORE_ERROR)
                {
                    break;
                }
                if ( eResult == NASC_SUCCESS )
                {
                    eReplyGot = TRUE;
                }
            }
        }
    }

    if ( iTimeOutCount >= 3 ) {
      printf("Client IP: %s not reachable\n", inet_ntoa(pstDestAddr->sin_addr));
      exit(0);
    }

    close(iSocketfd);
    return eResult;
}

I32 ICP_Checksum ( UI16 *pusPacket , I32 iPktLen)
{
    I32 inleft = iPktLen;
    UI16 *pusPkt = pusPacket;
    I32 iSum = 0;
    I32 usAnswer = 0;

    while (inleft > 1) {
        iSum += *pusPkt++;
        inleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (inleft == 1) {
        *(u_char *)(&usAnswer) = *(u_char *)pusPacket ;
        iSum += usAnswer;
    }

    /* add back carry outs from top 16 bits to low 16 bits */
    iSum = (iSum >> 16) + (iSum & 0xffff);/* add hi 16 to low 16 */
    iSum += (iSum >> 16);/* add carry */
    usAnswer = ~iSum;/* truncate to 16 bits */
    return(usAnswer);
}

ENASCenterError ICP_process_pkt (UI8 * pcRecvPkt , I32 iRecvMsgLen ,
                                        I32 iIdentifier ,
                                        struct timeval * pstTimeValue)
{
    struct ip    * pstIpHdr;
    struct icmp  * pstIcmpHdr;
    I32          iOptionLength;
    ENASCenterError  eStatus = NASC_SUCCESS;

    pstIpHdr = (struct ip *) pcRecvPkt;

    iOptionLength  = pstIpHdr->ip_hl  << 2;
    pstIcmpHdr = (struct icmp *) (pcRecvPkt +  iOptionLength);

    if (pstIcmpHdr->icmp_type != ICMP_ECHOREPLY) {
        eStatus = NASC_FAILURE;
    } else {
        if (pstIcmpHdr->icmp_id != iIdentifier) {
            printf("ignore error\n");
            eStatus  = NASC_ICP_IGNORE_ERROR;
        }
    }
    return eStatus ;
}

void ICP_GetTimeDiff (struct timeval *pstOutTime , 
                              struct timeval *pstInTime)
{
    if((pstOutTime->tv_usec -= pstInTime->tv_usec) < 0)
    {
        -- pstOutTime->tv_sec;
        pstOutTime->tv_usec += 1000000;
    }

   pstOutTime->tv_sec -= pstInTime->tv_sec;
}

/* End of NASCenter_ICP_main.c */

#ifndef NASCENTER_ICP_H
#define NASCENTER_ICP_H

/*****************************************************************************
 *
 * NASCenter_ICP.h [Edit filename.h]
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *      Aravind Balan
 *
 * description:
 *      NASCenter_ICP.h provides common includes,macros,and external
 *              declarations/definitions for ICP / File browsing modules
 *
 * CM Details:
 *
 * $Author: sspriya $
 * $Log: NASCenter_ICP.h,v $
 * Revision 1.22  2004/06/16 13:40:37  sspriya
 * Added #define used in fix to client system blackout problem in ICP.
 *
 * Revision 1.21  2004/02/09 12:10:37  vijaige
 * Added include files required for GetLocalHostIP
 * Changed by Priya, Reviewed by Buvaneswari
 *
 * Revision 1.20  2004/01/09 12:26:05  baravind
 * *** empty log message ***
 *
 * Revision 1.19  2004/01/09 11:02:14  sig
 * Added default shell for user management
 *
 * Revision 1.18  2004/01/09 06:31:01  baravind
 * *** empty log message ***
 *
 * Revision 1.17  2004/01/07 09:16:57  baravind
 * Removed unused variables
 *
 * Revision 1.16  2004/01/07 08:11:29  baravind
 * Fixed warning . Defined _REENTRANT
 *
 * Revision 1.15  2004/01/07 07:00:25  baravind
 * *** empty log message ***
 *
 * Revision 1.14  2004/01/07 05:51:24  baravind
 * Big Change !!! Catching signals in pthread behaves erratically . Changed to message queue instead of signals for sending trap to child process.
 *
 * Revision 1.13  2004/01/06 03:55:14  baravind
 * Fixed compilation error in UNIXWARE
 *
 * Revision 1.12  2004/01/05 09:43:02  baravind
 * Included typedef for UNIXWARE . typedef socklen_t to size_t
 *
 * Revision 1.11  2003/12/22 05:26:05  baravind
 * Added by Aravind.B .
 *
 * Revision 1.10  2003/12/22 05:19:23  baravind
 * *** empty log message ***
 *
 * Revision 1.9  2003/12/22 05:18:28  baravind
 * *** empty log message ***
 *
 * Revision 1.8  2003/12/22 05:01:57  sig
 * Added by Aravind on Dec-22-2003 . Fix for LINUX
 *
 * Revision 1.7  2003/12/22 04:10:31  baravind
 * Added by Aravind.B on Dec-22-2003 . Fixed compilation erros for UNIXWARE
 *
 * Revision 1.6  2003/12/17 07:51:13  baravind
 * Added by Aravind.B on Dec-17-2003 . Released for system testing
 *
 * Revision 1.5  2003/12/16 09:03:28  baravind
 * Added by Aravind.B on Dec-16-2003 .Lunix,  Solaris , HP and Unixware completed
 *
 * Revision 1.4  2003/12/11 05:07:56  baravind
 * Added by Aravind.B on Dec-11-2003
 *
 * Revision 1.3  2003/12/08 11:27:32  baravind
 * Added by Aravind.B on 8-Dec-2003
 *
 * Revision 1.1.1.2  2003/10/17 06:41:51  hari
 * no message
 *
 * Revision 1.1.1.1  2003/10/01 05:23:22  thiagav
 * no message
 *
 * $State: Exp $
 * $Id: NASCenter_ICP.h,v 1.22 2004/06/16 13:40:37 sspriya Exp $
 * $Name:  $
 *
 ****************************************************************************/


/********************************** includes *******************************/
#define _REENTRANT
#include <stdio.h>
#include <string.h> 
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/utsname.h>
#ifdef LINUX
#define _XOPEN_SOURCE
#endif
#include <unistd.h>
#include <sys/time.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#ifdef UNIXWARE
#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 199506
#define _XOPEN_SOURCE_EXTENDED 1
#endif
#include <signal.h>
#include <stdlib.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/snmp_api.h>
#include <net-snmp/library/snmp_secmod.h>
#include <net-snmp/library/callback.h>
#include <net-snmp/library/keytools.h>
#include <net-snmp/library/default_store.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#if defined(HPUNIX) || defined(SOLARIS)
#include <strings.h>
#endif

#ifdef AIX
#include <userpw.h>
#include<usersec.h>
#endif

#ifndef  __NASCENTER_ERRORS_H__
#include <NASCenter_Errors.h>
#endif

#ifndef __ctypes_H__
#include <ctypes.h>
#endif

#ifndef __NASCENTER_COMMON_H__
#include <NASCenter_Common.h>
#endif

#ifndef __NASCENTER_FILE_TRANS_H__
#include <NASCenter_FileTrans_my.h>
#endif



 /********************************** macros *********************************/

 #ifdef UNIXWARE
   typedef socklen_t size_t;
 #endif
 
 #define MAX_ICP_SESSIONS 30  /* Max no of sessions for ICP , FileBrowsing and
                                 User command all inclusive */
 #define MAX_CMD_SIZE     500  /* Max command size */

 #define MAX_OUTP_SIZE    80
 
 #define  MAX_CMD_LEN_PER_PKT 100

 #define NASC_ICP_PORTNO 3003 

 #define ICP_TRAP_LISTEN_PORT 2504

 #define ICP_SNMP_LISTEN_PORT 161
 
 #define NASC_MAX_BACKLOG 5

 #define MAX_REPETITIONS 10
 
 #define MAX_OPT 15

 #define IP_ADDR_LEN 15

 #define MAX_CMD_LEN 15

 #define MESSAGE_Q  81757

 #define  MAX_PARAMS 20

 #define  CREATE_AND_GO 4
 #define  DESTROY 6
 #define  ACTIVE 1
 #define  NOT_IN_SERVICE 2
 #define MAX_PKT_SIZE 1500
 #define DEF_ICMP_DATA_LEN  56
 #define ICMP_HDR_LEN  8
 #define ICMP_WAIT_TIME 10      //in sec
 #define ICP_PING_INTERVAL 300  //in sec
 #define ICP_IDLE_TIME 300000.0 // in millisec

 #define USER_SHELL "/bin/sh"



#define FREE_PTR(pointer) if ( pointer != NULL) \
                         { \
                             free(pointer); \
                             pointer = NULL; \
                         }


 typedef enum eCommandType
 {
     LISTDIR = 0 ,
     LISTFILE , 
     PASTEFILE ,
     RUN ,
     RENAME ,
     CRTDIR ,
     COMPRS ,
     CHGPERM ,
     USERLIST ,
     USERCREATE ,
     USERMODIFY ,
     USERREMOVE ,
     GRPLIST ,
     GRPCREATE ,
     GRPMODIFY ,
     GRPREMOVE
  }eCommandType ;


 /******************************* exported types ****************************/


typedef struct stCmdPacket
{
    I32 iModuleId;
    I32 iCommandId;
    I32 iLastPkt;
    UI8  ucData[MAX_CMD_LEN_PER_PKT];
    I32  iDataLen;
}stCmdPacket;


typedef struct stIcpTableCtx
{
    unsigned long lIcpIndex;
    UI8 cIcpCommand[255];
    long lIcpCommandLen;
    struct stIcpTableCtx *stNext;
}stIcpTableCtx; 

typedef struct stIcpList
{
    struct stIcpTableCtx *stHeader;
    pthread_mutex_t icpListmutex;
} stIcpList;

typedef struct stAgentInfo
{
   UI8 cAgentName[255];
   UI8 cCmdLogFile[255];
   I32 iTransInterval;
   I32 iAgentFileTransPort;
   UI8 cAgentFileTransIp[IP_ADDR_LEN + 1]; 
} stAgentInfo;

typedef struct stEquivalentCommand
{
    char *pcEquiCommand;
    int  iNoOfParmNeeded;
}stEquivalentCommand;

typedef struct stMessage
{
    long lMesgType;
}stMessage;    

typedef void sigHndlr (int);


    

    


 /************************* exported function prototypes ********************/

 ENASCenterError ICP_ProcessFileBrwsCmd (I32 iSocketFd , 
                                         stCmdPacket * pstCmdPkt);

 extern ENASCenterError ICP_ProcessICPCommand (I32 iSocketFd , 
                                         stCmdPacket * pstCmdPkt);

 ENASCenterError ICP_ProcessUserMGMTCmd (I32 iSocketFd , 
                                         stCmdPacket * pstCmdPkt);

 void ICP_SendPkt(I32 iSocketFd  ,I32 iModuleId , void * pvData ,
                 stCmdPacket *pstSendCmdPkt);

 void Util_GetIPFrom_ASNFormat(char *asnFormat, char *buff);

 ENASCenterError ICP_InitICPList (stIcpList *gstIcpList);

 stIcpTableCtx * ICP_CreateICPNode ( UI8 * pcCmdName , I16 iCmdLen);

 void ICP_AddToCntx ( stIcpList *stIcpList , stIcpTableCtx *stIcpCntx);

 void ICP_DelFromCtx ( stIcpList *gstIcpList , UI8 * pcCmdName , I16 iCmdLen);

 void ICP_DelAllFromCtx(stIcpList *gstIcpList);

 stIcpTableCtx * ICP_CheckCmdInList ( stIcpList *gstIcpList ,
                                     UI8 * pcCmdName , I16 iCmdLen);

 ENASCenterError  ICP_ExecCommand ( I32 iSocketFd , UI8 * pucCommand ,
                                   stCmdPacket * pstSendCmdPkt , int *iPipeId);

 ENASCenterError ICP_ExtractParams(UI8 *** pppcTempString , UI8 *pcParamList ,
                  I32 *iNoOfParam);

 void ICP_FormCmd(UI8 **ppcParamList , I32 iNoOfParam ,
            UI8 *pcCommandToExec , I32 iCommandType);

 void ICP_Free_Ptr_Array ( UI8 ** pvPtr , I32 iNoOfElem);

 sigHndlr * ICP_Signal ( I32 iSigNo , sigHndlr * pSignalHndlr);

 void ICP_ConvertCmdToHostForm(stCmdPacket *pstCommandPkt ,
                              stCmdPacket *pstCmdPktInHstFrm);


 /***************************** exported globals ****************************/


#endif
/* end of filename.h */

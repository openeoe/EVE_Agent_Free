#ifndef __NASCENTER_BM_LINUX_H__
#define __NASCENTER_BM_LINUX_H__

/*****************************************************************************
 *
 * NASCenter_BM_Firewall.h
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *      Thiagarajan V.
 *
 *
 * description:
 *      NASCenter_BM_Firewall.h provides common includes,macros,and external
 *              declarations/definitions for users of NASCenter_BM_Firewall.c
 *
 * CM Details:
 *
 * $Log: NASCenter_BM.h,v $
 * Revision 1.10  2004/01/05 04:20:21  thiagav
 * Changed the C_MAX_SIZE
 *
 * Revision 1.9  2003/12/20 04:33:12  thiagav
 * Code modified to long with respect to long value
 *
 * Revision 1.8  2003/12/19 15:16:20  thiagav
 * Added after check finish time check
 *
 * Revision 1.7  2003/12/18 10:16:00  testteam
 * Added after detail table fix
 *
 * Revision 1.6  2003/12/18 07:50:21  thiagav
 * Added
 *
 * Revision 1.5  2003/12/16 07:15:52  thiagav
 * added
 *
 * Revision 1.4  2003/12/15 03:38:08  thiagav
 * Added after final testing
 *
 * Revision 1.3  2003/12/04 04:23:47  thiagav
 * Added
 *
 * Revision 1.2  2003/12/02 11:35:47  thiagav
 * Added
 *
 * Revision 1.1  2003/12/02 11:26:23  thiagav
 * Added
 *
 * Revision 1.7  2003/11/29 14:35:02  guga
 * Added after unit testing
 *
 * Revision 1.4  2003/11/21 13:00:22  thiagav
 * Added after basic testing
 *
 * Revision 1.3  2003/11/17 07:15:28  thiagav
 * Added in to cvs after basic testing
 *
 * Revision 1.2  2003/11/17 06:09:53  thiagav
 * Added after basic testing
 *
 * Revision 1.1  2003/11/12 08:21:57  thiagav
 * Added after first time compilation
 *
 *
 * $Id: NASCenter_BM.h,v 1.10 2004/01/05 04:20:21 thiagav Exp $
 *
 ****************************************************************************/

 /********************************** includes *******************************/
#include <stdlib.h>
#include "ctypes.h"
#include "NASCenter_BM_List.h"

 /********************************** macros *********************************/
#define BM_ADD 1
#define BM_MODIFY 2
#define BM_DELETE 3 
#define BM_DAY_CNT 7 

#define  BM_MASTER_ID 1 
#define  BM_MASTER_NAME 2
#define  BM_PERIODIC_INTERVAL 3
#define  BM_START_TIME 4
#define  BM_FINISH_TIME 5
#define  BM_DEST_IPADDR 6
#define  BM_DEST_PATH 7
#define  BM_ROW_STATUS 8
#define  BM_MAX_ELEMENT 10
#define  C_MAX_SIZE 1000000
//#define  C_MAX_SIZE 1000
#define NASC_MANDATORY_MISSING 10


 /******************************* exported types ****************************/

typedef struct 
{
    I16 iIndex;       
    UI8 *pcMasterID; /*Master ID*/
    UI8 *pcMasterName;   /*Master Name */
    I16 iInterval;     /* Interval */
    UI8 *pcStartTime;   /*Start Time */
    UI8 *pcEndTime;   /*End Time */
    UI32 uiEndTimeinNum;
    UI8 *pcIPAddr;   /* Ip Address */
    UI8 *pcDestPath;   /*Destination path*/
    I16 iStatus;       /*Row status of the record */
}stBackupMaster;

typedef netsnmp_pdu *QueueEntry ;

typedef struct CBuffer  
{
  pthread_mutex_t queue_lock;
  pthread_cond_t  queue_not_full;
  pthread_cond_t  queue_not_empty;
  I16 iQRear;
  I16 iQFront;
  QueueEntry  queueEntry[BM_MAX_ELEMENT];
}SCBuffer;

typedef struct backupStruct
{
     char *pcSourcePath;
     char *pcDestIP;
     char *pcDestPath;
     char *pcStartDate;
     char *pcEndDate;
     char *MasterID;
     char *pcMasterName;   
     int   iMasterIndex;   
}stBackupStruct;

UI8 Days[7][5]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};



/************************* exported function prototypes ********************/
extern stList        BM_MakeEmpty(stList L);
extern void BM_FatalError (char* );
extern void BM_AddNodeInBackupList( long  , I16 , stList );
extern I16  BM_DeleteNodeFromList(stList , I16 ,long );
extern long  BM_GetTaskID(I16 , stList );
extern void BM_DeleteList(stList stListPtr);

/***************************** exported globals ****************************/

UI8 gcNASSACommunity[]="public";
UI8 BM_DEST_PATH_DIR[]= "";
UI8 gcAgentSessPeerName[] = "localhost";
I16 giNASSAPort=161;
UI8 BM_CURRENT_PATH_LOGFILE[] ="/home/thiagav";


/*User can Modify this paramaters Start*/
/*Trap port number */
I16 cTrapPort=2502;
/* Only in Debug Case Set this  */
UI8 gIPAddr[]="192.168.165.69"; /*Ip address where the BM module is running*/
I16 gAgentPort =2130; /*Port number where the BM agent is running*/
/* Only in Debug Case Set this  */
/*User can Modify this paramaters End*/

I16 BACKUP_PORT=14941;
#endif /* __NASCENTER_BM_LINUX_H__ */
/* end of  NASCenter_BM_Firewall.h */

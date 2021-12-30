#ifndef __NASCENTER_UM_FIREWALL_H__
#define __NASCENTER_UM_FIREWALL_H__

/*****************************************************************************
 *
 * NASCenter_UM_Firewall.h
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *      Thiagarajan V.
 *
 *
 * description:
 *      NASCenter_UM_Firewall.h provides common includes,macros,and external
 *              declarations/definitions for users of NASCenter_UM_Firewall.c
 *
 * CM Details:
 *
 * $Log: NASCenter_UM_Linux_Firewall.h,v $
 * Revision 1.8  2003/12/29 11:24:23  thiagav
 * Tested and not in service field test is added
 *
 * Revision 1.7  2003/12/09 05:07:05  thiagav
 * *** empty log message ***
 *
 * Revision 1.6  2003/12/08 11:14:15  thiagav
 * Added
 *
 * Revision 1.4  2003/11/07 17:35:35  thiagav
 * Tested for add/ modify for firewall rules
 *
 * Revision 1.3  2003/11/05 12:52:42  thiagav
 * Modified and updated
 *
 * Revision 1.2  2003/11/05 09:21:55  thiagav
 * Compiled and added to CVS
 *
 * Revision 1.1  2003/11/05 08:58:59  thiagav
 * Compiled and added to CVS
 *
 * Revision 1.4  2003/11/05 05:44:35  thiagav
 * Header and comments are added
 *
 * Revision 1.3  2003/11/03 13:34:59  thiagav
 * Compiled and checked in CVS
 *
 * Revision 1.2  2003/10/31 13:11:59  thiagav
 * Compiled and added in to CVS
 *
 * Revision 1.1  2003/10/30 12:47:04  thiagav
 * Added Firewall.h file in to CVS on 30th Oct
 *
 *
 * $Id: NASCenter_UM_Linux_Firewall.h,v 1.8 2003/12/29 11:24:23 thiagav Exp $
 *
 ****************************************************************************/

 /********************************** includes *******************************/
#include <stdlib.h>
#include "ctypes.h"
#include "NASCenter_UM_Linux_List.h"

 /********************************** macros *********************************/
#define UM_ADD 1
#define UM_MODIFY 2
#define UM_DELETE 3 
#define UM_GENERAL 100 

#define UM_RULES_EXCEEDED -1

#define ALLOW_TELNET 0
#define ALLOW_FTP 1
#define ALLOW_SMTP 2
#define ALLOW_POP3 3

#define DENY_TELNET 0
#define DENY_FTP 1
#define DENY_SMTP 2
#define DENY_POP3 3

#define MAX_OID_LEN 12

#define UM_START_RULE_NUM 1
#define UM_MAX_ELEMENT 10
#define UM_FIELD_LEN 14
#define UM_NO_RECORD_FOUND 99
#define NASC_MANDATORY_MISSING 10

#define IP_ADDR 1
#define IP_PORT_NUM 2
#define IP_STATUS 3

 
 /******************************* exported types ****************************/

/* Generic User Control Table stores required information of 
 * Telnet table , FTP table, SMTP table, POP3 table */

typedef struct 
{
    I16 iIndex;       /* Index ID of the table*/
    UI8 *pcUserName; /*User Name */
    UI8 *pcIpAddr;   /*character pointer to the IP address */
    I16 iPortNum;     /*Port number*/
    I16 iFlag;     /* Flag says which table the record belong to */
    I16 iStatus;       /*Row status of the record */
}stAllowUM;

typedef struct
{
   struct snmp_session  stSession;
}SSNMPSession;
typedef netsnmp_pdu *QueueEntry ;

typedef struct SUserControl
{
    stAllowUM  *pstAllowUM;
    struct SUserControl  *pstNext;
}SUserControl;

typedef struct CBuffer  
{
  pthread_mutex_t queue_lock;
  pthread_cond_t  queue_not_full;
  pthread_cond_t  queue_not_empty;
  I16 iQRear;
  I16 iQFront;
  QueueEntry  queueEntry[UM_MAX_ELEMENT];
}SCBuffer;



/************************* exported function prototypes ********************/
extern stList UM_MakeEmpty(stList);
extern void UM_Insert( ElementType ,ElementType, ElementType ,  stList , struct ip_fwnew *);
extern stRetStruct *  UM_GetstructForMod(I16 , stList,I16);
extern I16 UM_DelRuleNumFromList(I16 , stList );
extern I16  UM_ModifyList(stList ,  struct ip_fwnew *,I16 );
extern I16  UM_DeleteFromList(stList , I16,I16 );

/***************************** exported globals ****************************/
extern I8 iVal;
extern I32 iLen;

UI8 gcNASSACommunity[]="public";
UI8 gcAgentSessPeerName[] = "localhost";
I16 giNASSAPort=161;

UI8 cSrcMask[]="255.255.255.255";
UI8 cDstMask[]="255.255.255.255";
UI8 cDstAddr[21];
UI8 gIPAddr[15]= "192.168.162.205";
I16 gAgentPort=2222;





#endif /* __NASCENTER_UM_FIREWALL_H__ */
/* end of  NASCenter_UM_Firewall.h */

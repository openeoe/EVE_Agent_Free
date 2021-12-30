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
 * $Author: thiagav $
 * $Log: NASCenter_UM_IBM_Firewall.h,v $
 * Revision 1.10  2004/01/02 11:23:04  thiagav
 * *** empty log message ***
 *
 * Revision 1.9  2003/12/30 04:49:46  thiagav
 * Updated IBM
 *
 * Revision 1.8  2003/12/10 05:09:37  thiagav
 * Added mkfilt genfilt , all filter rule fiunctionalities
 *
 * Revision 1.7  2003/12/09 06:50:12  thiagav
 * Modified with respect to Linux firewall
 *
 * Revision 1.6  2003/11/08 13:28:51  thiagav
 * *** empty log message ***
 *
 * Revision 1.4  2003/11/05 12:53:00  thiagav
 * Updated and modified
 *
 * Revision 1.3  2003/11/05 09:04:04  thiagav
 * Compiled code is added in to CVS
 *
 * Revision 1.1  2003/11/05 06:39:54  thiagav
 * Compiled, Header and comments are added
 *
 * $Id: NASCenter_UM_IBM_Firewall.h,v 1.10 2004/01/02 11:23:04 thiagav Exp $
 *
 ****************************************************************************/



 /********************************** includes *******************************/
#include <stdlib.h>
#include "ctypes.h"
#include "NASCenter_UM_IBM_List.h"


 /********************************** macros *********************************/
#define UM_ADD 1
#define UM_MODIFY 2
#define UM_DELETE 3 

#define UM_RULES_EXCEEDED -1

#define ALLOW_TELNET 0
#define ALLOW_FTP 1
#define ALLOW_SMTP 2
#define ALLOW_POP3 3

#define UM_START_RULE_NUM 101
 
#define DENY_TELNET 0
#define DENY_FTP 1
#define DENY_SMTP 2
#define DENY_POP3 3    
#define NASC_MANDATORY_MISSING 10

#define IP_ADDR 1
#define IP_PORT_NUM 2
#define IP_STATUS 3 
#define UM_MAX_ELEMENT 10
#define UM_FIELD_LEN 14
#define UM_NO_RECORD_FOUND 99

UI8 cgenFiltString[]="genfilt -v 4 -n 2 -a P ";
UI8 cChFiltString[]="chfilt -v 4 -a P ";
UI8 cRMFiltString[]="rmfilt -v 4 ";
UI8  cMakeFilt[] = "/usr/sbin/mkdev -c ipsec -t 4";
UI8 cRemoveAll[]="rmfilt -f  -v 4 -n all";
UI8 cActivateRule[] = "mkfilt -v 4 -u -z P start";
UI8 cStopRule[] = "mkfilt -v 4 -u -z P stop";
UI8 cBlockTELNET[] = "genfilt -v 4 -n 2  -a D -s 0.0.0.0 -c tcp -P 23 -O eq -P 23  -m 0.0.0.0 -d 192.168.165.59 -M 255.255.255.255 -i all";
UI8 cBlockFTP[]    = "genfilt -v 4  -n 2 -a D -s 0.0.0.0  -c tcp -P 21 -O eq -P 21 -m  0.0.0.0 -d 192.168.165.59  -M 255.255.255.255 -i all";
UI8 cBlockSMTP[]   = "genfilt -v 4  -n 2 -a D -s 0.0.0.0  -c tcp -P 25 -O eq -P 25 -m  255.255.255.255 -d 192.168.165.59  -M 255.255.255.255 -i all";
UI8 cBlockPOP3[]   = "genfilt -v 4  -n 2 -a D -s 0.0.0.0  -c tcp -P 110 -O eq -P 110 -m  255.255.255.255 -d 192.168.165.59 -M 255.255.255.255 -i all";



 /******************************* exported types ****************************/

/* Generic User Control Table stores required information of 
 * Telnet table , FTP table, SMTP table, POP3 table */

typedef struct
{
   struct snmp_session  stSession;
}SSNMPSession;

typedef struct stNode  stNode1;

typedef netsnmp_pdu *QueueEntry ;

typedef struct 
{
    I16 iIndex;       /* Index ID of the table*/
    UI8 *pcUserName; /*User Name */
    UI8 *pcIpAddr;   /*character pointer to the IP address */
    I16 iPortNum;     /*Port number*/
    I16 iFlag;     /* Flag says which table the record belong to */
    I16 iStatus;       /*Row status of the record */
}stAllowUM;


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


/*Local host address where user meetering is running */
UI8 cLocalHostIPAddr[18];
UI8 cLocalAddr[10]="localhost";
UI8 cInterfaceName[]="all";
UI8 cSMask[]="255.255.255.255";
UI8 cDMask[]="255.255.255.255";
UI8 gcAgentSessPeerName[] = "0.0.0.0:0";
UI8 gcNASSACommunity[255]="public";
I16 giNASSAPort = 161;
UI8 gIPAddr[]="192.168.165.59";
I16 gAgentPort =2222;

extern stList UM_MakeEmpty(stList);
extern void UM_InsertToList( ElementType , stList , I16  , char *, I16 );
//extern void UM_Insert( ElementType ,ElementType, ElementType ,  stList , struct ip_fwnew *);
extern stRetStruct *  UM_GetstructForMod(I16 , stList,I16);
extern I16 UM_DelRuleNumFromList(I16 , stList );
extern I16  UM_ModifyList(stList ,  struct ip_fwnew *,I16 );
extern I16  UM_DeleteFromList(stList , I16,I16 );




/************************* exported function prototypes ********************/
extern stList UM_MakeEmpty(stList);
extern  stPosition UM_Header(stList);
extern stRetStruct* UM_GetPosFromList(I16 , stList , I16 );


/***************************** exported globals ****************************/
extern I8 iVal;
extern I32 iLen;



#endif /* __NASCENTER_UM_FIREWALL_H__ */

/* end of  NASCenter_UM_Firewall.h */

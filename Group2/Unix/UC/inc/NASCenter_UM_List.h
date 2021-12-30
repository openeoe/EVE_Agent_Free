#ifndef _List_H
#define _List_H
/*****************************************************************************
 *
 * NASCenter_UM_IBM_List.h
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
 * $Author: baravind $
 * $Log: NASCenter_UM_List.h,v $
 * Revision 1.6  2003/12/22 10:33:41  baravind
 * Added by Aravind.B . UM for sol , unixware and hpunix
 *
 * Revision 1.6  2003/12/10 05:09:43  thiagav
 * Added mkfilt genfilt , all filter rule fiunctionalities
 *
 * Revision 1.5  2003/12/09 11:48:22  thiagav
 * *** empty log message ***
 *
 * Revision 1.4  2003/11/08 13:28:30  thiagav
 * *** empty log message ***
 *
 * Revision 1.3  2003/11/05 09:04:17  thiagav
 * Compiled code is added in to CVS
 *
 * Revision 1.1  2003/11/05 06:40:22  thiagav
 * Compiled , Header and comments are added
 *
 * $Id: NASCenter_UM_List.h,v 1.6 2003/12/22 10:33:41 baravind Exp $
 *
 ****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
//#include <netinet/in.h>
#include <sys/types.h>
#include <signal.h>

//#include <netinet/ip.h>
#include <pthread.h>
//#include <netinet/tcp.h>
//#include <netinet/udp.h>
#include <sys/param.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include "ctypes.h"


typedef I16 ElementType;
/* Define Linked List stNode */
typedef struct stNode {
  ElementType iIndex;
  ElementType iRuleNum;
  ElementType iPortNum;
  ElementType iFlag;
  ElementType iStatus;
  UI8 *pcIPAddr;
  struct stNode *Next;
}stNode;

typedef struct modStruct{
  stNode *modStruct;
  I16 iCount;
}stRetStruct;


/* typedef struct modStruct{
  struct ip_fwnew *ipfwPtr;
  I16 iCount;
}stRetStruct; */

typedef struct stNode *psToNode;
typedef psToNode stList;
typedef psToNode stPosition;
typedef struct ip_fwnew stIPfwNew;

stList        UM_MakeEmpty(stList L);
void UM_Insert( ElementType , ElementType, ElementType ,  stList , stIPfwNew *);
stRetStruct*  UM_GetstructForMod(I16 , stList ,I16);
void          UM_DeleteList(stList L);
I16  UM_ModifyList(stList ,struct ip_fwnew *,I16 );
I16  UM_GetPos(I16 iIndex , stList , I16 );
I16  UM_DelRuleNumFromList(I16 ,stList );
void UM_FatalError (char* );
extern stRetStruct* UM_GetPosFromList(I16 iIndex , stList stHeadPtr ,
				      I16);


#endif    /* _List_H */

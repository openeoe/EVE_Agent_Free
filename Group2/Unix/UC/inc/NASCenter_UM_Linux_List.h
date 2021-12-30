
#ifndef _List_H
#define _List_H
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <signal.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/if.h>
#include <sys/param.h>
#include <linux/types.h>
#include <linux/icmp.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <linux/netfilter_ipv4/ipchains_core.h>
#include "ctypes.h"

typedef I16 ElementType;
/* Define Linked List stNode */
typedef struct stNode {
  ElementType iIndex;
  ElementType iRuleNum;
  ElementType iFlag;
  struct ip_fwnew *ipfwPtr;
  struct stNode *Next;
}stNode;

typedef struct modStruct{
  struct ip_fwnew *ipfwPtr;
  I16 iCount;
}stRetStruct;

struct stNode;
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
#endif    /* _List_H */

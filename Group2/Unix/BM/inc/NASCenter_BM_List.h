
#ifndef _List_H
#define _List_H
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include "ctypes.h"

typedef I16 ElementType;
/* Define Linked List stNode */
typedef struct stNode {
  ElementType iIndex;
  long lTaskID;
  struct stNode *Next;
}stNode;

struct stNode;
typedef struct stNode *psToNode;
typedef psToNode stList;
typedef psToNode stPosition;

stList        BM_MakeEmpty(stList L);
void BM_FatalError (char* );
void BM_AddNodeInBackupList( long  , I16 , stList );
I16  BM_DeleteNodeFromList(stList , I16 ,long );
long  BM_GetTaskID(I16 , stList );
void BM_DeleteList(stList stListPtr);
#endif    /* _List_H */

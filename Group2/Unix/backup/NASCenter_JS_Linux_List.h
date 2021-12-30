
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
  UI32 uiTaskID;
  struct stNode *Next;
}stNode;

struct stNode;
typedef struct stNode *psToNode;
typedef psToNode stList;
typedef psToNode stPosition;

stList        JS_MakeEmpty(stList L);
void JS_FatalError (char* );
void JS_AddNodeInJobList( UI32  , I16 , stList );
I16  JS_DeleteNodeFromList(stList , I16 ,UI32 );
UI32  JS_GetTaskID(I16 , stList );
void JS_DeleteList(stList stListPtr);
#endif    /* _List_H */

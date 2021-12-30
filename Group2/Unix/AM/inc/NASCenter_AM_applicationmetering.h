#ifndef __NASCENTER_AM_APPLICATIONMETERING_H__
#define __NASCENTER_AM_APPLICATIONMETERING_H__

/*****************************************************************************
 *
 * NASCenter_AM_applicationmetering.h
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Vijaigeetha C.
 *
 * description :
 *     NASCenter_AM_applicationmetering.h provides common includes,macros,and 
       external declarations/definitions of NASCenter_AM_applicationmetering.c
 *      
 *  
 *
 * CM Details:
 *
 * $Author: baravind $
 * $Log: NASCenter_AM_applicationmetering.h,v $
 * Revision 1.7  2004/01/06 11:30:17  baravind
 * Added flag to List(stapp,int) TRU64
 *
 * Revision 1.6  2003/12/31 12:58:52  rajesh
 * Uncommented 175
 *
 * Revision 1.5  2003/12/31 12:45:37  rajesh
 * Line number 175 is commented
 *
 * Revision 1.4  2003/12/23 11:54:12  karthis
 * Modified
 *
 * Revision 1.3  2003/12/22 03:10:42  karthis
 *
 * AM Solaris Updated
 *
 * Revision 1.2  2003/11/20 05:36:04  karthis
 * minor
 *
 * Revision 1.6  2003/11/05 08:57:02  vijaige
 * after set and get working
 *
 * 
 * 
 * $Id: NASCenter_AM_applicationmetering.h,v 1.7 2004/01/06 11:30:17 baravind Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/

 /********************************** macros *********************************/


#define BUFSIZE		256
#define LOGSIZE 500
#define MAX_LOG 20
#define MAX_ELEMENT 5
#define MAX_REPETATION 3*2




#define  AM_ID 1 
#define  AM_APPSTARTTIME 2
#define  AM_APPENDTIME 3
#define  AM_APPREACTION 4
#define  AM_APPBINNAME 5
#define  AM_ALERTMESSG 6
#define  AM_APPSTATUS 7




 /******************************* exported types ****************************/
 
/*
typedef struct stappMeteringStruct
{
        int uiappMeteringIndex;
        char *pcappMeteringID;
        int ulappMeteringStartTime;
        int ulappMeteringEndTime;
        int iappMeteringReaction;
        char *pcappMeteringUsername;
        char *pcappMeteringBinaryName;
        char *pcalertmessage;
        int istatus;
}StappMeteringStruct;


typedef struct stappM
{
        int  uiappMeteringIndex;
        char pcappMeteringID[255];
        int ulappMeteringStartTime;
        int ulappMeteringEndTime;
        int iappMeteringReaction;
        char pcappMeteringUsername[255];
        char pcalertmessage[255];
        int istatus;
        struct stappM *next;
}Stappsub;

typedef struct stappMetering
{
        char pcappMeteringBinaryName[255];
        Stappsub *head;
        struct stappMetering *next;
}Stapp;*/

typedef struct stappMeteringStruct
{
	UI32 uiappMeteringIndex;	
	char *pcappMeteringID;
	UI32 ulappMeteringStartTime;
	UI32 ulappMeteringEndTime;
	I16 iappMeteringReaction;
	char *pcappMeteringUsername;
	char  *pcappMeteringBinaryName;
	char  *pcalertmessage;
	I16 istatus;	
}StappMeteringStruct;


typedef struct stappM
{
	UI32 uiappMeteringIndex;	
	char pcappMeteringID[255];
	UI32 ulappMeteringStartTime;
	UI32 ulappMeteringEndTime;
	I16 iappMeteringReaction;
	char pcappMeteringUsername[255];
	char pcalertmessage[255];
       	I16 istatus;
	struct stappM *next;
}Stappsub;

typedef struct stappMetering
{
        char  pcappMeteringBinaryName[255];
        Stappsub *head; 
	struct stappMetering *next;
}Stapp;

	

typedef struct stagentInfoStruct
{
	I8 pcMgrIpAddr[20];
	I16 iMgrPort;
	I16 iTransferInterval;
	I8 pclogfilename[BUFSIZE];
	I8 pcAlertIpAddr[20];
	I16 iAlertPort;
}StAgent_InfoStruct;

typedef struct stLogFormat
{
	I16 inext;
	I8 *pcLogInfo[20];
}StLogFormat;


typedef netsnmp_pdu *QueueEntry ;

typedef struct CBuffer  
{
  pthread_mutex_t queue_lock;
  pthread_cond_t  queue_not_full;
  pthread_cond_t  queue_not_empty;
  I16 iQRear;
  I16 iQFront;
  QueueEntry  queueEntry[MAX_ELEMENT];
}SCBuffer;


/************************* exported function prototypes ********************/

void AM_SendLogFile();
I16 AM_LogData(struct stappMeteringStruct *);
I16 Util_Convert_Date_To_Long(I8 *,UI32 *);
UI32 Util_Get_Current_Time(void);
void Util_GetIPFrom_ASNFormat(char *, char *);
void AM_InitLogView();
int AM_ListenTrap(int,struct snmp_session *, int , netsnmp_pdu *, void *);
void AM_AddMsgToLogfile(UI8 *);
void AM_init_CBuffer(SCBuffer *);
I16 AM_GetAgentInfoTable(void);
void AM_Modify_Appmeteringlist(netsnmp_pdu *);
void AM_ParseTrap( struct snmp_pdu *);
void AM_ProcessTrapMsg(void *);
I16 AM_CheckTable(netsnmp_variable_list *);




void FreeHead(Stapp *);
void List(StappMeteringStruct *,int);
void Modify(StappMeteringStruct *);
void Delete(StappMeteringStruct *);





 /***************************** exported globals ****************************/


StLogFormat stLogView;
pthread_mutex_t appmetering_lock;
pthread_mutex_t log_lock;
FILE *ifdForAM;



StAgent_InfoStruct gstagentInfo;
StappMeteringStruct *gpstHeadappMet;
struct snmp_session *agentSess;
void *tSess;

SCBuffer stCB;


#endif /* __NASCENTER_AM_APPLICATIONMETERING_H__ */

/* end of NAScenter_AM_applicationMetering.h*/




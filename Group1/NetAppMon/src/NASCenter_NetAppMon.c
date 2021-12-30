 /*****************************************************************************
 *
 * NASCenter_NetAppMon.c
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      D. Sai Siva Kuamr
 *
 * description :
 *      This code does scheduleing of tasks.
 *
 * TODO:
 *      [ Things to be improved ]
 *
 * CM Details:
 *
 * $Author: buvanes $
 * $Log: NASCenter_NetAppMon.c,v $
 * Revision 1.8  2003/11/27 06:31:14  buvanes
 * FileTrans_SendFile commented as resulting in Too many open files error
 *
 * Revision 1.4  2003/11/25 15:34:01  saisk
 * 1. Sending File on Interval based or file size based is completed
 * 2. CopyFile function for Unix implemented
 *
 * Revision 1.3  2003/11/24 14:21:40  saisk
 * Updated
 *
 * Revision 1.9  2003/11/24 13:59:40  saisk
 * Changed COUNTER64 to Integer for InOctets,OutOctets, InPkts,OutPkts
 *
 * Revision 1.8  2003/11/21 13:02:46  saisk
 * Updated
 *
 * Revision 1.6  2003/11/19 04:30:39  saisk
 * Code Clean up
 *
 * Revision 1.5  2003/11/18 13:27:03  saisk
 * Added Function to Write into Log File
 *
 * Revision 1.4  2003/11/17 14:46:11  saisk
 * 1. Removed un wanted code
 * 2. Changed IP as string from unsigned long as received from MIB
 *
 * Revision 1.3  2003/11/14 15:09:22  saisk
 * Integrated with NetSnmp and it is working
 *
 * Revision 1.2  2003/11/14 11:05:58  saisk
 * Updated
 *
 * Revision 1.1  2003/11/11 14:32:13  saisk
 * Getting Process name and CPU and Memory percent is done
 *
 * $State: Exp $
 * $Id: NASCenter_NetAppMon.c,v 1.8 2003/11/27 06:31:14 buvanes Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/


/********************************* includes *********************************/
#ifdef WIN32
#include <Winsock2.h>
#else
#include <netinet/in.h>
#include <sys/stat.h>
#define _stat stat
#endif
#include "ctypes.h"
#include <pthread.h>
#include "NASCenter_Errors.h"
#include "NASCenter_NetAppMon.h"
#include "NASCenter_FileTrans.h"


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

/****************************** local macros ********************************
 *
 * (Macros shareable with rest of the modules are present in .h file)
 */
#ifdef NAM_DEBUG
#define DEBUG_FILE              "NAMDebug.log"
#define DEBUG_PRINT_PROC_LIST() DebugPrintProcList();
#define DEBUG_PRINT(x)          DebugPrint(x);
#else
#define DEBUG_PRINT_PROC_LIST() 
#define DEBUG_PRINT(x)
#endif

/********************* local structure/union definitions ********************/
typedef struct
{
   struct snmp_session  stSession;
}SSNMPSession;

typedef struct 
{
    UI8 caProcessName[MAX_PROC_NAME]; /* Process Name */
    UI32 uiProcCPUPercent; 
    UI32 uiProcMemPercent; 
    
}stProcessDetails;

typedef struct stProcNode stProcNode ;
struct stProcNode
{
    stProcessDetails stProcDetails;
    stProcNode *pNext;
};
typedef struct 
{
    stProcNode *pHeadProcData;
    stProcNode *pTailProcData;
    int nSize;
}stProcList;



/********************* prototypes for local functions ***********************/
void NAM_InitSession(void);
ENASCenterError NAM_GetInterfaceDetails(unsigned long ulServerIp, UI16 uiPort,\
                                        UI32 uiIndex, stSetDetails *stSetDet);
ENASCenterError NAM_AddToProcList(char* cpProcName,I32 iCPUPcnt, I32 iMemPcnt);
ENASCenterError NAM_GetProcessCPUMemPercent(char* cpProcName, stSetDetails *pstSetDet);


#ifdef NAM_DEBUG
void DebugPrintProcList() ;
void DebugPrint(char* str) ;
#endif

/************************* Globals within this module ***********************/
char gcCommunity[]              ="public";
char gcAgentSessPeerName[]      ="localhost";
UI16 giDummyPort                = 5000;
struct snmp_session *gAgentSession  = NULL;
static stProcList gProcList;
static pthread_mutex_t gProcList_mutex = PTHREAD_MUTEX_INITIALIZER ;
static pthread_mutex_t gLogFile_mutex  = PTHREAD_MUTEX_INITIALIZER ;
FILE *gFpl = NULL;
char gcTempLogFile[] = "NAMTempLogFile.dat" ;
/*Required oid's */

oid psRunningName_oid[]        = {1,3,6,1,4,1,3204,1,3,33,1,1,4};  
oid psRunningPercentCPU_oid[]  = {1,3,6,1,4,1,3204,1,3,33,1,1,15};  
oid psRunningPercentSize_oid[] = {1,3,6,1,4,1,3204,1,3,33,1,1,26};
oid ifInOctets_oid[]           = {1,3,6,1,2,1,2,2,1,10};
oid ifInUcastPkts_oid[]        = {1,3,6,1,2,1,2,2,1,11};
oid ifOutOctets_oid[]          = {1,3,6,1,2,1,2,2,1,16};
oid ifOutUcastPkts_oid[]       = {1,3,6,1,2,1,2,2,1,17};

#ifdef NAM_DEBUG
FILE *fpDebugLog = NULL;
#endif
/************************* externs  ***********************/
/* defined in agentInfo.c */
extern char gcInfoLogFileNetApp[];
extern long glAgentInfoFileTrInterval ;
extern long glAgentInfoFileTrPort;
extern char gcAgentInfoFileTransferIP[];
long glLogFileLimitSize = 100;/* In Bytes */
/* defined in netApplicationMonitoringTable.c */
extern unsigned int guiLogFileSendAlarmReg;
/*****************************************************************************
 *
 * name             :   InitNAM
 * description      :   Initialize NAM
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   
 * calls            :  InitProcList(), NAM_InitSession()
 */

void InitNAM(void)
{
    InitProcList();
    NAM_InitSession();
}

/*****************************************************************************
 *
 * name             :   InitProcList
 * description      :   Initialize the Process List
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   gProcList
 * calls            :   
 */

void InitProcList(void)
{
    gProcList.pHeadProcData  = NULL;
    gProcList.pTailProcData  = NULL;
    gProcList.nSize          = 0;
}

/*****************************************************************************
 *
 * name             :   NAM_InitSession 
 * description      :   Initialize the SNMP Session  
 * input parameters :   void
 * return type      :   void 
 * global variables :   -gcAgentSessPeerName  - peer name 
 *                      -giDummyPort - port number used for opening the snmp
 *                        session
 *                      -gcCommunity - Community name for snmp session
 *                        
 * calls	        :     
 *                      
 *
 */

void NAM_InitSession(void)
{
   struct snmp_session tempSess;
   //void * sessp;
   /* For Session Initiation */
   snmp_sess_init(&tempSess);
   tempSess.version = SNMP_VERSION_2c;
   tempSess.peername = gcAgentSessPeerName;
   //tempSess.local_port = 200;
   tempSess.remote_port    = giDummyPort;
   tempSess.community      = gcCommunity;
   tempSess.community_len  = strlen(gcCommunity);
   tempSess.callback_magic = NULL;


   SOCK_STARTUP;

   gAgentSession = snmp_open(&tempSess);
    //sessp= snmp_sess_open(&tempSess);
    SOCK_STARTUP;
    //gAgentSession = snmp_sess_session(sessp);

   if(gAgentSession == NULL) 
   {
       snmp_perror("NASCenterUM Snmp Open: ");
       exit(1);
   }
}

/*****************************************************************************
 *
 * name             :   NAM_GetProcessDetails 
 * description      :   Gets Process Name, CPU Percent and MEM Percent from 
 *                      Nuri Enterprise Process Table
 * input parameters :   unsigned long, UI16
 * return type      :   ENASCenterError 
 * global variables :   
 * calls	        :     
 *                      
 *
 */

ENASCenterError NAM_GetProcessDetails(unsigned long ulServerIp, UI16 uiPort)
{
    struct snmp_pdu *reqPDU = NULL ;
    struct snmp_pdu *respPDU = NULL ;
    netsnmp_variable_list *pstCurrVars;
    char caProcessName[MAX_PROC_NAME];
    I32  iCPUPercent = 0 ;
    I32  iMemPercent = 0;
    oid *pProcRunName       = psRunningName_oid ;
    I16 iProcRunNameLen     = (sizeof(psRunningName_oid)/ sizeof(oid));
    oid *pProcPercentCPU    = psRunningPercentCPU_oid;
    I16 iProcPercentCPULen  = (sizeof(psRunningPercentCPU_oid)/ sizeof(oid));
    oid *pProcPercentMem    = psRunningPercentSize_oid;
    I16 iProcPercentMemLen  = (sizeof(psRunningPercentSize_oid)/ sizeof(oid));

    oid *pFieldOid = pProcRunName;
    I16 FieldOidLen= iProcRunNameLen;

    oid *ptableID  = pProcRunName;
    I16 iTableLen  = iProcRunNameLen;

    I16 status=0;
    I16 iCheckTable;
    I16 iCount;
    struct sockaddr_in ServerAddress ;

    DEBUGMSGTL(("nuri/nam", "Inside NAM_GetProcessDetails....\n"));
    /* Lock */
    //pthread_mutex_lock(&gProcList_mutex);

    DEBUGMSGTL(("nuri/nam", "Inside NAM_GetProcessDetails after lock....\n"));

    ServerAddress.sin_family= AF_INET;
    ServerAddress.sin_port = htons(uiPort);
    ServerAddress.sin_addr.s_addr  = ulServerIp;

    while(1) 
    {
        reqPDU = snmp_pdu_create(SNMP_MSG_GETBULK);
        reqPDU->non_repeaters = 0;

        /*Set the number of records to be fetched */
        reqPDU->max_repetitions = MAX_REPETITIONS;

        reqPDU->transport_data = &ServerAddress;
        reqPDU->transport_data_length = sizeof(ServerAddress);


       /* Add variable to be queried */
       snmp_add_null_var(reqPDU,pProcRunName,iProcRunNameLen);
       snmp_add_null_var(reqPDU,pProcPercentCPU,iProcPercentCPULen);
       snmp_add_null_var(reqPDU,pProcPercentMem,iProcPercentMemLen);

       /*Get the response in the respPDU*/
       DEBUGMSGTL(("nuri/nam", "Inside NAM_GetProcessDetails B4 response....\n"));
       status = snmp_synch_response(gAgentSession,reqPDU,&respPDU);	
       DEBUGMSGTL(("nuri/nam", "Inside NAM_GetProcessDetails After response....\n"));
       if(status==STAT_SUCCESS)
       {
           DEBUG_PRINT("Request Succeeded\n");
           if( respPDU->errstat==SNMP_ERR_NOERROR && 
               respPDU->errindex==0 && 
               respPDU->variables->type!= ASN_NULL &&
               respPDU->variables->type!= SNMP_NOSUCHINSTANCE &&
               respPDU->variables->type!= SNMP_NOSUCHOBJECT &&
               respPDU->variables->type!= SNMP_ENDOFMIBVIEW)

           {
                pstCurrVars = respPDU->variables;
           }
           else
           {
               break;
           }
       } 
       else 
       {
           snmp_perror("Error ");   	   
           /* Unlock */
           //pthread_mutex_unlock(&gProcList_mutex);
           
           DEBUGMSGTL(("nuri/nam", "Leaving NAM_GetProcessDetails status not SUCCESS....\n"));
	       return NASC_FAILURE;
       }
       
       
       
       /*Check the fetched record is belong to respective table */

       if(netsnmp_oid_is_subtree(ptableID,
           iTableLen, pstCurrVars->name, pstCurrVars->name_length))
       { 
           /* Unlock */
           //pthread_mutex_unlock(&gProcList_mutex);
           DEBUGMSGTL(("nuri/nam", "Leaving NAM_GetProcessDetails is subtree failure....\n"));
           return NASC_FAILURE;	       
       }
       
       /*Flag for checkiing whether the fetched table details belongs to 
	* required table*/ 
       iCheckTable = TRUE;
       for(iCount =0; iCount < MAX_REPETITIONS;iCount++ )
       {
            /*Check the fetched record is belong to respective table */
            if(netsnmp_oid_is_subtree(pFieldOid,
              FieldOidLen, pstCurrVars->name, pstCurrVars->name_length))
            { 
	            iCheckTable = FALSE;
                break;
            }
            /* Get Process Name */
            memset(caProcessName,0,MAX_PROC_NAME);
            strncpy(caProcessName,pstCurrVars->val.string,
                    pstCurrVars->val_len);

            pProcRunName       = pstCurrVars->name;
            iProcRunNameLen    = pstCurrVars->name_length; 

            /* Move to next variablke to get CPU Percent */
            pstCurrVars = pstCurrVars->next_variable;
            if(pstCurrVars->val.integer != NULL)
            {
                iCPUPercent = *pstCurrVars->val.integer;
            }
            pProcPercentCPU       = pstCurrVars->name;
            iProcPercentCPULen    = pstCurrVars->name_length; 

            /* Move to next variablke to get MEM Percent */
            pstCurrVars = pstCurrVars->next_variable;
            if(pstCurrVars->val.integer != NULL)
            {
                iMemPercent = *pstCurrVars->val.integer;
            }

            pProcPercentMem       = pstCurrVars->name;
            iProcPercentMemLen    = pstCurrVars->name_length; 

            /* Add to ProcList Process Name, CPU % and Mem % */
            NAM_AddToProcList(caProcessName,iCPUPercent,iMemPercent);

            pstCurrVars = pstCurrVars->next_variable;

       }/* End of for loop */
       if(iCheckTable == FALSE)
       {
            break; 		   
       }		   
     } /* end of while(1) */

    //DEBUG_PRINT_PROC_LIST();      

       /* Unlock */
       //pthread_mutex_unlock(&gProcList_mutex);
       DEBUGMSGTL(("nuri/nam", "Leaving NAM_GetProcessDetails....\n"));
        return NASC_SUCCESS;
}

/*****************************************************************************
 *
 * name             :   NAM_GetInterfaceDetails 
 * description      :   Gets InOctets,OutOctets,InUcastPkts,OutUcastPkts
 *                      from Interface Table
 * input parameters :   unsigned long, UI16, UI32,
 * output parameters:   stSetDetails*
 * return type      :   ENASCenterError 
 * global variables :   
 * calls	        :     
 *                      
 *
 */

ENASCenterError NAM_GetInterfaceDetails(unsigned long ulServerIp, UI16 uiPort,\
                                        UI32 uiIndex, stSetDetails *stSetDet)
{
    struct snmp_pdu *reqPDU;
    struct snmp_pdu *respPDU;
    netsnmp_variable_list *pstCurrVars;
    I16 status=0;
    oid *pifInOctets_oid = NULL ;
    oid *pifInUcastPkts_oid = NULL ;
    oid *pifOutOctets_oid = NULL ;
    oid *pifOutUcastPkts_oid = NULL ;
    I16 iifInOctetsLen = 0;
    I16 iifInUcastPktsLen = 0;
    I16 iifOutOctetsLen = 0;
    I16 iifOutUcastPktsLen = 0;
    ENASCenterError eErrorStatus = NASC_FAILURE;

    struct sockaddr_in ServerAddress ;
    /* Allocate memory to hold base oid + index */
    pifInOctets_oid     = malloc(sizeof(ifInOctets_oid) + sizeof(oid));
    iifInOctetsLen   = (sizeof(ifInOctets_oid)+sizeof(oid))/ sizeof(oid);
    memset(pifInOctets_oid,0,(sizeof(ifInOctets_oid) + sizeof(oid)));
    memcpy(pifInOctets_oid,ifInOctets_oid,sizeof(ifInOctets_oid));
    pifInOctets_oid[iifInOctetsLen-1] = uiIndex ;

    /* Allocate memory to hold base oid + index */
    pifInUcastPkts_oid     = malloc(sizeof(ifInUcastPkts_oid) + sizeof(oid));
    iifInUcastPktsLen   = (sizeof(ifInUcastPkts_oid)+sizeof(oid))/ sizeof(oid);
    memset(pifInUcastPkts_oid,0,(sizeof(ifInUcastPkts_oid) + sizeof(oid)));
    memcpy(pifInUcastPkts_oid,ifInUcastPkts_oid,sizeof(ifInUcastPkts_oid));
    pifInUcastPkts_oid[iifInUcastPktsLen-1] = uiIndex ;

    /* Allocate memory to hold base oid + index */
    pifOutOctets_oid     = malloc(sizeof(ifOutOctets_oid) + sizeof(oid));
    iifOutOctetsLen   = (sizeof(ifOutOctets_oid)+sizeof(oid))/ sizeof(oid);
    memset(pifOutOctets_oid,0,(sizeof(ifOutOctets_oid) + sizeof(oid)));
    memcpy(pifOutOctets_oid,ifOutOctets_oid,sizeof(ifOutOctets_oid));
    pifOutOctets_oid[iifOutOctetsLen-1] = uiIndex ;

    /* Allocate memory to hold base oid + index */
    pifOutUcastPkts_oid     = malloc(sizeof(ifOutUcastPkts_oid) + sizeof(oid));
    iifOutUcastPktsLen   = (sizeof(ifOutUcastPkts_oid)+sizeof(oid))/ sizeof(oid);
    memset(pifOutUcastPkts_oid,0,(sizeof(ifOutUcastPkts_oid) + sizeof(oid)));
    memcpy(pifOutUcastPkts_oid,ifOutUcastPkts_oid,sizeof(ifOutUcastPkts_oid));
    pifOutUcastPkts_oid[iifOutUcastPktsLen-1] = uiIndex ;


    /* Set IP and port number were query has to be performed*/
    ServerAddress.sin_family        = AF_INET;
    ServerAddress.sin_port          = htons(uiPort);
    ServerAddress.sin_addr.s_addr   = ulServerIp;

    reqPDU = snmp_pdu_create(SNMP_MSG_GET);
    reqPDU->non_repeaters = 0;

    /*Set the number of records to be fetched */
    reqPDU->max_repetitions = MAX_REPETITIONS;

    reqPDU->transport_data = &ServerAddress;
    reqPDU->transport_data_length = sizeof(ServerAddress);

    snmp_add_null_var(reqPDU,pifInOctets_oid,iifInOctetsLen);
    snmp_add_null_var(reqPDU,pifInUcastPkts_oid,iifInUcastPktsLen);
    snmp_add_null_var(reqPDU,pifOutOctets_oid,iifOutOctetsLen);
    snmp_add_null_var(reqPDU,pifOutUcastPkts_oid,iifOutUcastPktsLen);


   /*Get the response in the respPDU*/
    status = snmp_synch_response(gAgentSession,reqPDU,&respPDU);	
    if(status==STAT_SUCCESS)
    {
       if( respPDU->errstat==SNMP_ERR_NOERROR && 
           respPDU->errindex==0 && 
           respPDU->variables->type!= ASN_NULL &&
           respPDU->variables->type!= SNMP_NOSUCHINSTANCE &&
           respPDU->variables->type!= SNMP_NOSUCHOBJECT)
       {
            pstCurrVars = respPDU->variables;
            if(pstCurrVars->val.integer != NULL)
            {
                 stSetDet->uiInOctets = *pstCurrVars->val.integer;
            }
            /* Move to next variable */             
            pstCurrVars = pstCurrVars->next_variable;
            if(pstCurrVars->val.integer != NULL)  
            {
                stSetDet->uiInUcastPkts = *pstCurrVars->val.integer;
            }
            /* Move to next variable */             
            pstCurrVars = pstCurrVars->next_variable;
            if(pstCurrVars->val.integer != NULL)
            {
                 stSetDet->uiOutOctets = *pstCurrVars->val.integer;
            }
            /* Move to next variable */             
            pstCurrVars = pstCurrVars->next_variable;
            if(pstCurrVars->val.integer != NULL)  
            {
                stSetDet->uiOutUcastPkts = *pstCurrVars->val.integer;
            }
            eErrorStatus = NASC_SUCCESS;
            DEBUGMSGTL(("nuri/nam", "Test InOctets =%d\n", stSetDet->uiInOctets));
            DEBUGMSGTL(("nuri/nam", "Test OutOctets =%d\n", stSetDet->uiOutOctets));
            DEBUGMSGTL(("nuri/nam", "Test InPkts =%d\n", stSetDet->uiInUcastPkts));
            DEBUGMSGTL(("nuri/nam", "Test OutPkts =%d\n", stSetDet->uiOutUcastPkts));

       }
       else
       {
           DEBUGMSGTL(("nuri/nam", "Error in fetchinf Interface Details\n"));
           eErrorStatus = NASC_FAILURE;
       }
    } 
    else 
    {
       snmp_perror("Error ");   	   
       eErrorStatus = NASC_FAILURE;
       
    }

    if (pifInOctets_oid != NULL)
    {
        free(pifInOctets_oid);
    }
    if (pifInUcastPkts_oid != NULL)
    {
        free(pifInUcastPkts_oid);
    }
    if (pifOutOctets_oid != NULL)
    {
        free(pifOutOctets_oid);
    }
    if (pifOutUcastPkts_oid != NULL)
    {
        free(pifOutUcastPkts_oid);
    }
    return 1;

}


/*****************************************************************************
 *
 * name             :   NAM_AddToProcList
 * description      :   Add Process Details to the Proc List
 * input parameters :   char*,I32,I32
 * output parameters:   void
 * return type      :   ENASCenterError
 * global variables :   gProcList
 * calls            :   
 */

ENASCenterError NAM_AddToProcList(char* cpProcName,I32 iCPUPcnt, I32 iMemPcnt)
{
	stProcNode *endProcNode   = NULL;
	stProcNode *newProcNode   = NULL;
    ENASCenterError           eErrorStatus;
    /* Memory will be freed in NAM_ClearProcList() */
    newProcNode = malloc (sizeof(stProcNode));

    /*Add to Procution List*/
    if(newProcNode!=NULL){
        memset(newProcNode->stProcDetails.caProcessName,0,MAX_PROC_NAME);
        strcpy(newProcNode->stProcDetails.caProcessName,cpProcName);
	    newProcNode->stProcDetails.uiProcCPUPercent = iCPUPcnt;
        newProcNode->stProcDetails.uiProcMemPercent = iMemPcnt;
	    newProcNode->pNext = NULL;
        
        if (gProcList.pHeadProcData == NULL)
        {
            gProcList.pHeadProcData = newProcNode;

        }
        else
        {
            endProcNode = gProcList.pTailProcData;
            endProcNode->pNext = newProcNode;
        }
        gProcList.pTailProcData = newProcNode;
        gProcList.nSize++;
        eErrorStatus = NASC_SUCCESS;
    }
    else
    {
         eErrorStatus = NASC_FAILURE;
    }

    return eErrorStatus ;
}

/*****************************************************************************
 *
 * name             :   NAM_GetProcessCPUMemPercent
 * description      :   Gets CPU and MEM percent for the given Process from Proc List
 * input parameters :   char*,
 * output parameters:   stSetDetails*
 * return type      :   ENASCenterError
 * global variables :   gProcList
 * calls            :   
 */

ENASCenterError NAM_GetProcessCPUMemPercent(char* cpProcName, stSetDetails *pstSetDet)
{
    ENASCenterError  eErrorStatus = NASC_FAILURE;
    stProcNode *pProcTemp = NULL;
    pProcTemp = gProcList.pHeadProcData;

    /* Lock */
    //pthread_mutex_lock(&gProcList_mutex);

    while(pProcTemp != NULL)
    {
        if (strcmp(cpProcName,pProcTemp->stProcDetails.caProcessName) == 0)
        {

            //iRetVal = pProcTemp->stProcDetails.iIndex;
            pstSetDet->uiCPU = pProcTemp->stProcDetails.uiProcCPUPercent;
            pstSetDet->uiMem = pProcTemp->stProcDetails.uiProcMemPercent;
            eErrorStatus = NASC_SUCCESS; 
            break;
        }
    
        pProcTemp = pProcTemp->pNext;
    }
    /* Unlock */
    //pthread_mutex_unlock(&gProcList_mutex);

    return eErrorStatus;
}

/*****************************************************************************
 *
 * name             :   NAM_GetValue
 * description      :   Gets Process and Interface Details
 * input parameters :   char*,unsigned long,UI16,UI32
 * output parameters:   stSetDetails*
 * return type      :   ENASCenterError
 * global variables :   
 * calls            :   NAM_GetProcessCPUMemPercent(),NAM_GetInterfaceDetails
 */

ENASCenterError NAM_GetValue(char* cpProcName,unsigned long ulServerIp, UI16 uiPort, 
                             UI32 uiIFIndex, stSetDetails *pstSetDet )
{
    ENASCenterError  eErrorStatus = NASC_FAILURE;

    pstSetDet->uiInOctets     = 0;
    pstSetDet->uiInUcastPkts  = 0;
    pstSetDet->uiOutOctets    = 0;
    pstSetDet->uiOutUcastPkts = 0;

    
    if ( NAM_GetProcessCPUMemPercent(cpProcName,pstSetDet) == NASC_SUCCESS)
    {
        NAM_GetInterfaceDetails(ulServerIp, uiPort, uiIFIndex,pstSetDet);        
        eErrorStatus = NASC_SUCCESS;
    }


    return eErrorStatus;

}

/*****************************************************************************
 *
 * name             :   NAM_ClearProcList
 * description      :   Clears memory of Process List
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   gProcList
 * calls            :   
 */

void NAM_ClearProcList(void)
{
    stProcNode *pProcNewHead = NULL;
    /* Lock */
    //pthread_mutex_lock(&gProcList_mutex);

    while(gProcList.pHeadProcData != NULL)
    {
        pProcNewHead = gProcList.pHeadProcData->pNext ;
        free(gProcList.pHeadProcData);
        gProcList.nSize--;
        gProcList.pHeadProcData = pProcNewHead;
    }
    gProcList.pHeadProcData = NULL;
    gProcList.pTailProcData = NULL;
    /* Unlock */
    //pthread_mutex_unlock(&gProcList_mutex);
    
}
/*****************************************************************************
 *
 * name             :   NAM_WriteToLogFile
 * description      :   Writes to Temp Log file
 * input parameters :   char*, stSetDetails, BOOL
 * output parameters:   void
 * return type      :   void
 * global variables :   gFpl,gcTempLogFile
 * calls            :   
 */

void NAM_WriteToLogFile(char* cpProcName, stSetDetails stDetails, BOOL bProcStatus)
{
    static UI32 ui32FileSize = 0;
    static UI32 ui32MaxWrittenSize = 0;
    I32 i32WrittenSize = 0;
    time_t CurrTime;
    pthread_mutex_lock(&gLogFile_mutex);
    if (gFpl == NULL)
    {
        if ((gFpl = fopen (gcTempLogFile, "a")) == NULL) 
        {
            /* File Open Failure */
            DEBUGMSGTL(("nuri/nam", " Temporary Log file open failed \n"));
            gFpl = NULL;
            pthread_mutex_unlock(&gLogFile_mutex);
            return;
        }
        else
        {
            ui32FileSize = 0;
            i32WrittenSize = 0;
            ui32MaxWrittenSize = 0;

        }

    }
    time(&CurrTime);
    if (bProcStatus)
    {
        i32WrittenSize = fprintf (gFpl, "%s%s %s%s %d%s %d%s %d%s %d%s %d%s %d%s\n",
                        ctime(&CurrTime),NAM_DELIMITER,
                        cpProcName,NAM_DELIMITER,
                        stDetails.uiCPU,NAM_DELIMITER,
                        stDetails.uiMem,NAM_DELIMITER,
                        stDetails.uiInOctets,NAM_DELIMITER,
                        stDetails.uiOutOctets,NAM_DELIMITER,
                        stDetails.uiInUcastPkts,NAM_DELIMITER,
                        stDetails.uiOutUcastPkts);
        fflush(gFpl);
    }
    else
    {
        i32WrittenSize = fprintf (gFpl, "%s%s%s Not Running\n",ctime(&CurrTime),
                         NAM_DELIMITER, cpProcName);
        fflush(gFpl);
    }
    
    if (i32WrittenSize > 0)
    {
        ui32FileSize += i32WrittenSize;
        
        (ui32MaxWrittenSize < i32WrittenSize) ? (ui32MaxWrittenSize = i32WrittenSize):(ui32MaxWrittenSize);

    }
    /*Checking whether space is available for next time*/
    if ((ui32FileSize + ui32MaxWrittenSize) > glLogFileLimitSize)
    {
        ui32FileSize = 0;
        i32WrittenSize = 0;
        ui32MaxWrittenSize = 0;
        if (guiLogFileSendAlarmReg != 0)
        {
            DEBUGMSGTL(("nuri/nam", "Unregistering guiLogFileSendAlarmReg.....\n"));
            snmp_alarm_unregister(guiLogFileSendAlarmReg);
            guiLogFileSendAlarmReg = 0;
        }
        fclose(gFpl); /* To be cross-checked */
        gFpl = NULL; /* To be cross- checked */
        pthread_mutex_unlock(&gLogFile_mutex);

        SendLogFileToManager (NULL, NULL);
        return ;
    }
    pthread_mutex_unlock(&gLogFile_mutex);
}

/*****************************************************************************
 *
 * name             :   SendLogFileToManager
 * description      :   Sends Log file to Manager
 * input parameters :   unsigned int
 * output parameters:   void *
 * return type      :   void
 * global variables :   (extern)glAgentInfoFileTrPort,gcAgentInfoFileTransferIP,
 *                      gcTempLogFile,gcInfoLogFileNetApp
 * calls            :   
 */
void SendLogFileToManager (unsigned int guiReg, void *pclientarg)
{
    struct sockaddr_in sock_addr;
    I16 error;
   
#ifdef WIN32
    WSADATA wsadata;
    WSAStartup(MAKEWORD( 2, 2 ),&wsadata);
#endif

    if((glAgentInfoFileTrPort < 0 )||(strlen(gcAgentInfoFileTransferIP) <= 0))
    {
        /* Error Case */
        return;
    }
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons((unsigned short)glAgentInfoFileTrPort);
    sock_addr.sin_addr.s_addr = inet_addr(gcAgentInfoFileTransferIP);    
    
    pthread_mutex_lock(&gLogFile_mutex);
    if(gFpl != NULL) /* Condition to be cross-checked */
    {
        fclose(gFpl);
        gFpl = NULL;
    }
    /*Copy from temp file to actual file */
    if(strlen(gcInfoLogFileNetApp) > 0)
    {
        CopyFile(gcTempLogFile,gcInfoLogFileNetApp,FALSE);
      //  CopyFile(FALSE);
    }
    /* Unlock here so that NAM_WriteToLogFile() can take next logging*/
    /* Assumption here is file transfer will complete before next interval
       or file size limit reached */
    pthread_mutex_unlock(&gLogFile_mutex);
    if(strlen(gcInfoLogFileNetApp) > 0)
    {
/* File Transfer - commented for this release */
#ifdef DUMMY
       #ifdef WIN32
        error = FileTrans_SendFile ( gcInfoLogFileNetApp , &sock_addr , "C:\\" , 512);  
       #else
        error = FileTrans_SendFile ( gcInfoLogFileNetApp , &sock_addr , "/tmp/" , 512);  
       #endif
#endif /* DUMMY */
    }
    if ( error != NASC_SUCCESS)
    {
        DEBUGMSGTL(("nuri/nam", "SendLogFileToManager() Failed ....\n"));
    }
    else 
    {
        DEBUGMSGTL(("nuri/nam", "SendLogFileToManager() Success ....\n"));
    }
    #ifdef WIN32
    WSACleanup();
    #endif
    guiLogFileSendAlarmReg = snmp_alarm_register (glAgentInfoFileTrInterval,
                             (int) NULL,SendLogFileToManager, NULL);


}
#ifndef WIN32
/*Iplmentation for UNIX */
BOOL CopyFile(char * SrcFile,char * DesFile,BOOL bCreateFlag)
{
    FILE *fpSrcFile = NULL;
    FILE *fpDestFile = NULL;
    struct _stat stStat;
    I32 i32Result;
    I32 i32FileSize = 0;
    char *cpBuffer = NULL;


    fpSrcFile  = fopen(gcTempLogFile, "r");
    fpDestFile = fopen(gcInfoLogFileNetApp, "w");

    if ((fpSrcFile != NULL) && (fpDestFile != NULL))
    {

       i32Result = _stat( gcTempLogFile, &stStat );

       /* Check if statistics are valid: */
       if( i32Result == 0 )
       {
           DEBUGMSGTL(("nuri/nam", "Src: %s Dest: %s \n",gcTempLogFile,gcInfoLogFileNetApp));
           i32FileSize = stStat.st_size ;
           cpBuffer = (char*) malloc(sizeof(char) * (i32FileSize + 1 ));
           /* Fix given */
           if(cpBuffer)
           {
               memset(cpBuffer,0,(sizeof(char) * (i32FileSize + 1 )));
               fread(cpBuffer,i32FileSize,1,fpSrcFile);
               fwrite(cpBuffer,i32FileSize,1,fpDestFile);
               free(cpBuffer);
           }
       }
    }
    else
    {
        /* Error */
        /* if conditions added - to avoid "Too many Files Open" Error */
        /* To be cross-checked */
        if(fpSrcFile != NULL)
        {
            DEBUGMSGTL(("nuri/nam", "###Source file fpSrcFile closed..."));
            fclose (fpSrcFile);
        }
        if(fpDestFile != NULL)
        {
            DEBUGMSGTL(("nuri/nam", "###Destination file fpDestFile closed..."));
            fclose (fpDestFile);
        }
        
        return FALSE;
    }

    fclose(fpSrcFile);
    fclose(fpDestFile);
    return TRUE;
}
#endif
/* Following are Debug Print function for Debugging purpose only */

void DebugPrintProcList(void)
{
#ifdef NAM_DEBUG
    if ((fpDebugLog = fopen (DEBUG_FILE, "a+")) != NULL) 
    {
        stProcNode *pProcTemp = NULL;
        pProcTemp = gProcList.pHeadProcData;

        while(pProcTemp != NULL)
        {
            
            fprintf (fpDebugLog, "Process: %s CPU%: %d Mem%: %d\n",
                        pProcTemp->stProcDetails.caProcessName,
                        pProcTemp->stProcDetails.uiProcCPUPercent,
                        pProcTemp->stProcDetails.uiProcMemPercent);
            pProcTemp = pProcTemp->pNext;
        }
        fprintf (fpDebugLog, "List Size: %d \n", gProcList.nSize);
            
        fclose( fpDebugLog );

    }
    else
    {
        printf("File Open Failure\n");
    }
#endif
}
void DebugPrint(char* str) 
{
#ifdef NAM_DEBUG
    if ((fpDebugLog = fopen (DEBUG_FILE, "a+")) != NULL) 
    {
        fprintf (fpDebugLog, "%s\n",str);
        fclose( fpDebugLog );
    }
#endif
}

/* End of NASCenter_NetAppMon.c */

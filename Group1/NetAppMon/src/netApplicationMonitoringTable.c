/*****************************************************************************
 *
 * netApplicationMonitoringTable.c
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      D. Sai Siva Kumar
 *
 * description :
 *      This file is a C source code for netApplicationMonitoring
 *
 * TODO:
 *      None
 *
 * CM Details:
 *
 * $Author: vijaysp $
 * $Log: netApplicationMonitoringTable.c,v $
 * Revision 1.44  2004/01/13 13:07:14  vijaysp
 * A small memory leak in head node of linked list where is no row present in the table is fixed
 *
 * Revision 1.43  2004/01/08 04:06:50  vijaysp
 * pthread_detach and pthread_exit() called in FileTransfer fn.
 *
 * Revision 1.42  2004/01/05 04:26:56  saisk
 * Made all includes related to windows as relative path
 *
 * Revision 1.41  2004/01/01 10:49:09  saisk
 * Converted RTT from seconds to time ticks
 *
 * Revision 1.40  2004/01/01 09:05:38  saisk
 * File size checking for file transfer: changed from bytes to Kilo Bytes
 *
 * Revision 1.39  2003/12/31 13:08:34  saisk
 * Updated : moved StartTime before if condition
 *
 * Revision 1.38  2003/12/31 12:41:35  saisk
 * Modified to invoke GetProcessDetails when there is change in Server IP in the list
 *
 * Revision 1.37  2003/12/18 14:35:46  saisk
 * Update
 *
 * Revision 1.36  2003/12/18 04:46:49  saisk
 * Updated "NAM_Convert_Date_To_Long"
 *
 * Revision 1.35  2003/12/17 04:41:27  saisk
 * Changed "giContextChanged" to "giNAMContextChanged"
 *
 * Revision 1.34  2003/12/16 14:48:00  saisk
 * Moved Previous interval to global scope
 *
 * Revision 1.33  2003/12/16 13:11:21  saisk
 * Added giNAMContextChanged to check if there is any change in the container
 *
 * Revision 1.32  2003/12/13 11:50:42  saisk
 * Send file Alarm  function  invokes thread to send file
 *
 * Revision 1.31  2003/12/13 07:48:37  saisk
 * Added Wipro File Header
 *
 * $State: Exp $
 * $Id   :  Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/
/********************************* includes *********************************/

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <net-snmp/library/snmp_assert.h>

#ifdef WIN32
#include "../../Windows/Common/inc/pthread.h"
#include "../../../Common/NASCenter_Errors.h"
#include "../../../Common/NASCenter_FileTrans.h"
#include "../inc/netApplicationMonitoringTable.h"
#else
#include <pthread.h>
#include "NASCenter_Errors.h"
#include "NASCenter_FileTrans.h"
#include "netApplicationMonitoringTable.h"
#endif

#include <errno.h>

/* START: WIP_NURI_ALL */

#ifdef WIN32
//#include <Winsock2.h>
#else
#include <netinet/in.h>
#include <sys/stat.h>
#define _stat stat
#endif

#include <pthread.h>
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

#ifndef MAX_PATH
#define MAX_PATH 255
#endif
#define DATE_TIME_LEN 26
/********************* local structure/union definitions ********************/
typedef struct
{
   struct snmp_session  stSession;
}SSNMPSession;

typedef struct 
{
    char caProcessName[MAX_PROC_NAME]; /* Process Name */
    unsigned int uiProcCPUPercent; 
    unsigned int uiProcMemPercent; 
    
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
ENASCenterError NAM_GetInterfaceDetails(unsigned long ulServerIp, unsigned short uiPort,\
                                        unsigned int uiIndex, stSetDetails *stSetDet);
ENASCenterError NAM_AddToProcList(char* cpProcName,int iCPUPcnt, int iMemPcnt);
ENASCenterError NAM_GetProcessCPUMemPercent(char* cpProcName, stSetDetails *pstSetDet);
void NAM_ResetCounters(netApplicationMonitoringTable_context *ctx) ;
void NAM_SendLogFileToManagerThreadFn (unsigned int guiReg, void *pclientarg);
#ifdef NAM_DEBUG
void DebugPrintProcList() ;
void DebugPrint(char* str) ;
#endif

/************************* Globals within this module ***********************/
char gcCommunity[]              ="public";
char gcAgentSessPeerName[]      ="localhost";
unsigned short giDummyPort       = 5000;
struct snmp_session *gAgentSession  = NULL;
static stProcList gProcList;
static pthread_mutex_t gLogFile_mutex  = PTHREAD_MUTEX_INITIALIZER ;
FILE *gFpl = NULL;
char gcTempLogFile[] = "NAMTempLogFile.txt" ;
static pthread_t ghPThread; 
static int gPrevInterval = 0;
static unsigned long gServerIp = 0;
static int giNAMContextChanged = 0;
BOOL gbFileTransferInProgress = FALSE;
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
/* Begin definition in agentInfo.c */
extern char gcInfoLogFileNetApp[];
extern long glAgentInfoFileTrInterval ;
extern long glAgentInfoFileTrPort;
extern char gcAgentInfoFileTransferIP[];
extern long glAgentInfoLogFileLimitSize ;
extern void getIPFromASNFormat(char *asnFormat, char *buff);
/* End definition in agentInfo.c */


static pthread_mutex_t gContainer_mutex = PTHREAD_MUTEX_INITIALIZER ;
static pthread_mutex_t gList_mutex = PTHREAD_MUTEX_INITIALIZER ;
static BOOL gbIfInProcess = FALSE;
/* END: WIP_NURI_ALL */

static     netsnmp_handler_registration *my_handler = NULL;
static     netsnmp_table_array_callbacks cb;

oid netApplicationMonitoringTable_oid[] = { netApplicationMonitoringTable_TABLE_OID };
size_t netApplicationMonitoringTable_oid_len = OID_LENGTH(netApplicationMonitoringTable_oid);

/* START: WIP_NURI_ALL */


NAM_List *gpstHeadList = NULL;

unsigned int guiReg1=0;
unsigned int guiReg2=0;
unsigned int guiReg3=0;
unsigned int guiLogFileSendAlarmReg=0;

    
//int iLogFlag = 0;


/* Function Declarations */
void NAM_callback (unsigned int reg, void *pclientarg);
void NAM_Collect_Store_Variables (unsigned int guiReg, void *pclientarg);
unsigned long NAM_Get_Current_Time ();
void NAM_Load_Container_From_File ();
int NAM_Form_Linked_List (NAM_List *pstList, int *iLength);
int NAM_Sort_Linked_List (NAM_List *pstStart);
int NAM_Calc_Num_Nodes_Interval (int *pInterval);
int NAM_Within_Period (netApplicationMonitoringTable_context *pstCtx);
int NAM_Add_Intervals_To_List (netApplicationMonitoringTable_context *pstCtx,
                               NAM_List *pstList,int *piInterval, int *piLength);
/* END: WIP_NURI_ALL */


#ifdef netApplicationMonitoringTable_IDX2
/************************************************************
 * keep binary tree to find context by name
 */
static int netApplicationMonitoringTable_cmp( const void *lhs, const void *rhs );

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int
netApplicationMonitoringTable_cmp( const void *lhs, const void *rhs )
{
    netApplicationMonitoringTable_context *context_l =
        (netApplicationMonitoringTable_context *)lhs;
    netApplicationMonitoringTable_context *context_r =
        (netApplicationMonitoringTable_context *)rhs;

    /*
     * check primary key, then secondary. Add your own code if
     * there are more than 2 indexes
     */
    //int rc;

    /*
     * TODO: implement compare. Remove this ifdef code and
     * add your own code here.
     */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR,
             "netApplicationMonitoringTable_compare not implemented! Container order undefined\n" );
    return 0;
#endif
    
    /*
     * EXAMPLE:
     *   
     * rc = strcmp( context_l->xxName, context_r->xxName);
     *
     * if(rc)
     *   return rc;
     *
     * TODO: fix secondary keys (or delete if there are none)
     *
     * if(context_l->yy < context_r->yy) 
     *   return -1;
     *
     * return (context_l->yy == context_r->yy) ? 0 : 1;
     */

    /* START: WIP_NURI_ALL */
    return 0;
    /* END: WIP_NURI_ALL */


}

/************************************************************
 * search tree
 */
/** TODO: set additional indexes as parameters */
netApplicationMonitoringTable_context *
netApplicationMonitoringTable_get( const char *name, int len )
{
    //netApplicationMonitoringTable_context tmp;

    /** we should have a secondary index */
    netsnmp_assert(cb.container->next != NULL);
    
    /*
     * TODO: implement compare. Remove this ifdef code and
     * add your own code here.
     */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "netApplicationMonitoringTable_get not implemented!\n" );
    return NULL;
#endif

    /*
     * EXAMPLE:
     *
     * if(len > sizeof(tmp.xxName))
     *   return NULL;
     *
     * strncpy( tmp.xxName, name, sizeof(tmp.xxName) );
     * tmp.xxName_len = len;
     *
     * return CONTAINER_FIND(cb.container->next, &tmp);
     */

    /* START: WIP_NURI_ALL */

    return NULL ;

    /* END: WIP_NURI_ALL */

}
#endif


/************************************************************
 * Initializes the netApplicationMonitoringTable module
 */
void
init_netApplicationMonitoringTable(void)
{
    initialize_table_netApplicationMonitoringTable();

    /*
     * TODO: perform any startup stuff here
     */
/* <Start: WIP_NURI_ALL > */

    NAM_Load_Container_From_File ();
    InitNAM();
    NAM_callback (1, (void *) NULL);
    guiLogFileSendAlarmReg = snmp_alarm_register (glAgentInfoFileTrInterval,
                             (int) NULL,NAM_SendLogFileToManagerThreadFn, NULL);
    /* <End: WIP_NURI_ALL > */
}

/************************************************************
 * the *_row_copy routine
 */
static int netApplicationMonitoringTable_row_copy(netApplicationMonitoringTable_context * dst,
                         netApplicationMonitoringTable_context * src)
{
    if(!dst||!src)
        return 1;
        
    /*
     * copy index, if provided
     */
    if(dst->index.oids)
        free(dst->index.oids);
    if(snmp_clone_mem( (void*)&dst->index.oids, src->index.oids,
                           src->index.len * sizeof(oid) )) {
        dst->index.oids = NULL;
        return 1;
    }
    dst->index.len = src->index.len;
    

    /*
     * copy components into the context structure
     */
    dst->netApplicationMonitoringIndex = src->netApplicationMonitoringIndex;

    memcpy( dst->netApplicationMonitoringCreator, src->netApplicationMonitoringCreator, src->netApplicationMonitoringCreator_len );
    dst->netApplicationMonitoringCreator_len = src->netApplicationMonitoringCreator_len;

    memcpy( dst->netApplicationMonitoringDescription, src->netApplicationMonitoringDescription, src->netApplicationMonitoringDescription_len );
    dst->netApplicationMonitoringDescription_len = src->netApplicationMonitoringDescription_len;

    dst->netApplicationMonitoringServerAddress = src->netApplicationMonitoringServerAddress;

    memcpy( dst->netApplicationMonitoringProcessName, src->netApplicationMonitoringProcessName, src->netApplicationMonitoringProcessName_len );
    dst->netApplicationMonitoringProcessName_len = src->netApplicationMonitoringProcessName_len;

    dst->netApplicationMonitoringPort = src->netApplicationMonitoringPort;

    dst->netApplicationMonitoringSampleInterval = src->netApplicationMonitoringSampleInterval;

    dst->netApplicationMonitoringInterfaceNumber = src->netApplicationMonitoringInterfaceNumber;

    memcpy( dst->netApplicationMonitoringStartDate, src->netApplicationMonitoringStartDate, src->netApplicationMonitoringStartDate_len );
    dst->netApplicationMonitoringStartDate_len = src->netApplicationMonitoringStartDate_len;

    memcpy( dst->netApplicationMonitoringEndDate, src->netApplicationMonitoringEndDate, src->netApplicationMonitoringEndDate_len );
    dst->netApplicationMonitoringEndDate_len = src->netApplicationMonitoringEndDate_len;

    dst->netApplicationMonitoringMonStartTime = src->netApplicationMonitoringMonStartTime;

    dst->netApplicationMonitoringMonEndTime = src->netApplicationMonitoringMonEndTime;

    dst->netApplicationMonitoringTueStartTime = src->netApplicationMonitoringTueStartTime;

    dst->netApplicationMonitoringTueEndTime = src->netApplicationMonitoringTueEndTime;

    dst->netApplicationMonitoringWebStartTime = src->netApplicationMonitoringWebStartTime;

    dst->netApplicationMonitoringWebEndTime = src->netApplicationMonitoringWebEndTime;

    dst->netApplicationMonitoringThuStartTime = src->netApplicationMonitoringThuStartTime;

    dst->netApplicationMonitoringThuEndTime = src->netApplicationMonitoringThuEndTime;

    dst->netApplicationMonitoringFriStartTime = src->netApplicationMonitoringFriStartTime;

    dst->netApplicationMonitoringFriEndTime = src->netApplicationMonitoringFriEndTime;

    dst->netApplicationMonitoringSatStartTime = src->netApplicationMonitoringSatStartTime;

    dst->netApplicationMonitoringSatEndTime = src->netApplicationMonitoringSatEndTime;

    dst->netApplicationMonitoringSunStartTime = src->netApplicationMonitoringSunStartTime;

    dst->netApplicationMonitoringSunEndTime = src->netApplicationMonitoringSunEndTime;

    memcpy( dst->netApplicationMonitoringCreateDateTime, src->netApplicationMonitoringCreateDateTime, src->netApplicationMonitoringCreateDateTime_len );
    dst->netApplicationMonitoringCreateDateTime_len = src->netApplicationMonitoringCreateDateTime_len;

    dst->netApplicationMonitoringStatus = src->netApplicationMonitoringStatus;

    dst->netApplicationMonitoringRTT = src->netApplicationMonitoringRTT;

    dst->netApplicationMonitoringAvgCPUPercentage = src->netApplicationMonitoringAvgCPUPercentage;

    dst->netApplicationMonitoringAvgMemoryPercentage = src->netApplicationMonitoringAvgMemoryPercentage;

    dst->netApplicationMonitoringInOctets = src->netApplicationMonitoringInOctets;

    dst->netApplicationMonitoringInPkts = src->netApplicationMonitoringInPkts;

    dst->netApplicationMonitoringOutOctets = src->netApplicationMonitoringOutOctets;

    dst->netApplicationMonitoringOutPkts = src->netApplicationMonitoringOutPkts;

    return 0;
}

#ifdef netApplicationMonitoringTable_SET_HANDLING

/*
 * the *_extract_index routine
 */
int
netApplicationMonitoringTable_extract_index( netApplicationMonitoringTable_context * ctx, netsnmp_index * hdr )
{
    /*
     * temporary local storage for extracting oid index
     */
    netsnmp_variable_list var_netApplicationMonitoringIndex;
    int err;

    /*
     * copy index, if provided
     */
    if(hdr) {
        netsnmp_assert(ctx->index.oids == NULL);
        if(snmp_clone_mem( (void*)&ctx->index.oids, hdr->oids,
                           hdr->len * sizeof(oid) )) {
            return -1;
        }
        ctx->index.len = hdr->len;
    }

    /**
     * Create variable to hold each component of the index
     */
       memset( &var_netApplicationMonitoringIndex, 0x00, sizeof(var_netApplicationMonitoringIndex) );
       var_netApplicationMonitoringIndex.type = ASN_UNSIGNED;
       /** TODO: link this index to the next, or NULL for the last one */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "netApplicationMonitoringTable_extract_index index list not implemented!\n" );
    return 0;
#else
      /*  var_netApplicationMonitoringIndex.next_variable = &var_XX; */
#endif


    /*
     * parse the oid into the individual components
     */
    err = parse_oid_indexes( hdr->oids, hdr->len, &var_netApplicationMonitoringIndex );
    if (err == SNMP_ERR_NOERROR) {
       /*
        * copy components into the context structure
        */
                ctx->netApplicationMonitoringIndex = *var_netApplicationMonitoringIndex.val.integer;
   
   
           /*
            * TODO: check index for valid values. For EXAMPLE:
            *
              * if ( *var_netApplicationMonitoringIndex.val.integer != XXX ) {
          *    err = -1;
          * }
          */
    }

    /*
     * parsing may have allocated memory. free it.
     */
    snmp_reset_var_buffers( &var_netApplicationMonitoringIndex );

    return err;
}

/************************************************************
 * the *_can_activate routine is called
 * when a row is changed to determine if all the values
 * set are consistent with the row's rules for a row status
 * of ACTIVE.
 *
 * return 1 if the row could be ACTIVE
 * return 0 if the row is not ready for the ACTIVE state
 */
int netApplicationMonitoringTable_can_activate(netApplicationMonitoringTable_context *undo_ctx,
                      netApplicationMonitoringTable_context *row_ctx,
                      netsnmp_request_group * rg)
{
    /*
     * TODO: check for activation requirements here
     */
    return 1;
}

/************************************************************
 * the *_can_deactivate routine is called when a row that is
 * currently ACTIVE is set to a state other than ACTIVE. If
 * there are conditions in which a row should not be allowed
 * to transition out of the ACTIVE state (such as the row being
 * referred to by another row or table), check for them here.
 *
 * return 1 if the row can be set to a non-ACTIVE state
 * return 0 if the row must remain in the ACTIVE state
 */
int netApplicationMonitoringTable_can_deactivate(netApplicationMonitoringTable_context *undo_ctx,
                        netApplicationMonitoringTable_context *row_ctx,
                        netsnmp_request_group * rg)
{
    /*
     * TODO: check for deactivation requirements here
     */
    return 1;
}

/************************************************************
 * the *_can_delete routine is called to determine if a row
 * can be deleted.
 *
 * return 1 if the row can be deleted
 * return 0 if the row cannot be deleted
 */
int netApplicationMonitoringTable_can_delete(netApplicationMonitoringTable_context *undo_ctx,
                    netApplicationMonitoringTable_context *row_ctx,
                    netsnmp_request_group * rg)
{
    /*
     * probably shouldn't delete a row that we can't
     * deactivate.
     */
    if(netApplicationMonitoringTable_can_deactivate(undo_ctx,row_ctx,rg) != 1)
        return 0;
    
    /*
     * TODO: check for other deletion requirements here
     */
    return 1;
}

#ifdef netApplicationMonitoringTable_ROW_CREATION
/************************************************************
 * the *_create_row routine is called by the table handler
 * to create a new row for a given index. If you need more
 * information (such as column values) to make a decision
 * on creating rows, you must create an initial row here
 * (to hold the column values), and you can examine the
 * situation in more detail in the *_set_reserve1 or later
 * states of set processing. Simple check for a NULL undo_ctx
 * in those states and do detailed creation checking there.
 *
 * returns a newly allocated netApplicationMonitoringTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
netApplicationMonitoringTable_context *
netApplicationMonitoringTable_create_row( netsnmp_index* hdr)
{
    netApplicationMonitoringTable_context * ctx =
        SNMP_MALLOC_TYPEDEF(netApplicationMonitoringTable_context);
    if(!ctx)
        return NULL;
        
    /*
     * TODO: check indexes, if necessary.
     */
    if(netApplicationMonitoringTable_extract_index( ctx, hdr )) {
        free(ctx->index.oids);
        free(ctx);
        return NULL;
    }

    /* netsnmp_mutex_init(ctx->lock);
       netsnmp_mutex_lock(ctx->lock); */

    /*
     * TODO: initialize any default values here. This is also
     * first place you really should allocate any memory for
     * yourself to use.  If you allocated memory earlier,
     * make sure you free it for earlier error cases!
     */
    /**
     ctx->netApplicationMonitoringCreator = 0;
     ctx->netApplicationMonitoringDescription = 0;
     ctx->netApplicationMonitoringServerAddress = 0;
     ctx->netApplicationMonitoringProcessName = 0;
     ctx->netApplicationMonitoringPort = 0;
     ctx->netApplicationMonitoringSampleInterval = 0;
     ctx->netApplicationMonitoringInterfaceNumber = 0;
     ctx->netApplicationMonitoringStartDate = 0;
     ctx->netApplicationMonitoringEndDate = 0;
     ctx->netApplicationMonitoringMonStartTime = 0;
     ctx->netApplicationMonitoringMonEndTime = 0;
     ctx->netApplicationMonitoringTueStartTime = 0;
     ctx->netApplicationMonitoringTueEndTime = 0;
     ctx->netApplicationMonitoringWebStartTime = 0;
     ctx->netApplicationMonitoringWebEndTime = 0;
     ctx->netApplicationMonitoringThuStartTime = 0;
     ctx->netApplicationMonitoringThuEndTime = 0;
     ctx->netApplicationMonitoringFriStartTime = 0;
     ctx->netApplicationMonitoringFriEndTime = 0;
     ctx->netApplicationMonitoringSatStartTime = 0;
     ctx->netApplicationMonitoringSatEndTime = 0;
     ctx->netApplicationMonitoringSunStartTime = 0;
     ctx->netApplicationMonitoringSunEndTime = 0;
     ctx->netApplicationMonitoringCreateDateTime = 0;
     ctx->netApplicationMonitoringStatus = 0;
     ctx->netApplicationMonitoringRTT = 0;
     ctx->netApplicationMonitoringAvgCPUPercentage = 0;
     ctx->netApplicationMonitoringAvgMemoryPercentage = 0;
     ctx->netApplicationMonitoringInOctets = 0;
     ctx->netApplicationMonitoringInPkts = 0;
     ctx->netApplicationMonitoringOutOctets = 0;
     ctx->netApplicationMonitoringOutPkts = 0;
    */

    return ctx;
}
#endif

/************************************************************
 * the *_duplicate row routine
 */
netApplicationMonitoringTable_context *
netApplicationMonitoringTable_duplicate_row( netApplicationMonitoringTable_context * row_ctx)
{
    netApplicationMonitoringTable_context * dup;

    if(!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(netApplicationMonitoringTable_context);
    if(!dup)
        return NULL;
        
    if(netApplicationMonitoringTable_row_copy(dup,row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index * netApplicationMonitoringTable_delete_row( netApplicationMonitoringTable_context * ctx )
{
  /* netsnmp_mutex_destroy(ctx->lock); */
    DEBUGMSGTL(("nuri/nam","Inside netApplicationMonitoringTable_delete_row() \n"));
    pthread_mutex_lock(&gContainer_mutex);
    if(ctx->index.oids)
        free(ctx->index.oids);

    /*
     * TODO: release any memory you allocated here...
     */

    /*
     * release header
     */
    free( ctx );
    pthread_mutex_unlock(&gContainer_mutex);
    return NULL;
}


/************************************************************
 * RESERVE is used to check the syntax of all the variables
 * provided, that the values being set are sensible and consistent,
 * and to allocate any resources required for performing the SET.
 * After this stage, the expectation is that the set ought to
 * succeed, though this is not guaranteed. (In fact, with the UCD
 * agent, this is done in two passes - RESERVE1, and
 * RESERVE2, to allow for dependancies between variables).
 *
 * BEFORE calling this routine, the agent will call duplicate_row
 * to create a copy of the row (unless this is a new row; i.e.
 * row_created == 1).
 *
 * next state -> SET_RESERVE2 || SET_FREE
 */
void netApplicationMonitoringTable_set_reserve1( netsnmp_request_group *rg )
{
    netApplicationMonitoringTable_context *row_ctx =
            (netApplicationMonitoringTable_context *)rg->existing_row;
    netApplicationMonitoringTable_context *undo_ctx =
            (netApplicationMonitoringTable_context *)rg->undo_info;
    netsnmp_variable_list *var;
    netsnmp_request_group_item *current;
    int rc;
    
    DEBUGMSGTL(("nuri/nam","Inside netApplicationMonitoringTable_set_reserve1() \n"));
    //pthread_mutex_lock(&gContainer_mutex);

    /*
     * TODO: loop through columns, check syntax and lengths. For
     * columns which have no dependencies, you could also move
     * the value/range checking here to attempt to catch error
     * cases as early as possible.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;
        rc = SNMP_ERR_NOERROR;

        switch(current->tri->colnum) {

        case COLUMN_NETAPPLICATIONMONITORINGCREATOR:
            /** OCTETSTR = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                                                sizeof(row_ctx->netApplicationMonitoringCreator));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, var->val_len);
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSERVERADDRESS:
            /** IPADDR = ASN_IPADDRESS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_IPADDRESS,
                                                sizeof(row_ctx->netApplicationMonitoringServerAddress));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGPROCESSNAME:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,var->val_len);
        break;

        case COLUMN_NETAPPLICATIONMONITORINGPORT:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->netApplicationMonitoringPort));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSAMPLEINTERVAL:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->netApplicationMonitoringSampleInterval));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINTERFACENUMBER:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->netApplicationMonitoringInterfaceNumber));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSTARTDATE:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,var->val_len);
        break;

        case COLUMN_NETAPPLICATIONMONITORINGENDDATE:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,var->val_len);
        break;

        case COLUMN_NETAPPLICATIONMONITORINGMONSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringMonStartTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGMONENDTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringMonEndTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTUESTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringTueStartTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTUEENDTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringTueEndTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGWEBSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringWebStartTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGWEBENDTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringWebEndTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTHUSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringThuStartTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTHUENDTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringThuEndTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGFRISTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringFriStartTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGFRIENDTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringFriEndTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSATSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringSatStartTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSATENDTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringSatEndTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSUNSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringSunStartTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSUNENDTIME:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringSunEndTime));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGCREATEDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,var->val_len);
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSTATUS:
            /** RowStatus = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->netApplicationMonitoringStatus));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGRTT:
            /** TICKS = ASN_TIMETICKS */
            rc = netsnmp_check_vb_type_and_size(var, ASN_TIMETICKS,
                                                sizeof(row_ctx->netApplicationMonitoringRTT));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGAVGCPUPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->netApplicationMonitoringAvgCPUPercentage));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGAVGMEMORYPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->netApplicationMonitoringAvgMemoryPercentage));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
            rc = netsnmp_check_vb_type_and_size(var, ASN_COUNTER,
                                                sizeof(row_ctx->netApplicationMonitoringInOctets));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
            rc = netsnmp_check_vb_type_and_size(var, ASN_COUNTER,
                                                sizeof(row_ctx->netApplicationMonitoringInPkts));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGOUTOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
            rc = netsnmp_check_vb_type_and_size(var, ASN_COUNTER,
                                                sizeof(row_ctx->netApplicationMonitoringOutOctets));
        break;

        case COLUMN_NETAPPLICATIONMONITORINGOUTPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
            rc = netsnmp_check_vb_type_and_size(var, ASN_COUNTER,
                                                sizeof(row_ctx->netApplicationMonitoringOutPkts));
        break;

        default: /** We shouldn't get here */
            rc = SNMP_ERR_GENERR;
            snmp_log(LOG_ERR, "unknown column in "
                     "netApplicationMonitoringTable_set_reserve1\n");
        }

        if (rc)
           netsnmp_set_mode_request_error(MODE_SET_BEGIN, current->ri, rc );
        rg->status = SNMP_MAX( rg->status, current->ri->status );
    }

    //pthread_mutex_unlock(&gContainer_mutex);
    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

void netApplicationMonitoringTable_set_reserve2( netsnmp_request_group *rg )
{
    netApplicationMonitoringTable_context *row_ctx = (netApplicationMonitoringTable_context *)rg->existing_row;
    netApplicationMonitoringTable_context *undo_ctx = (netApplicationMonitoringTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;
    netsnmp_variable_list *var;
    int rc;

    rg->rg_void = rg->list->ri;

    /*
     * TODO: loop through columns, check for valid
     * values and any range constraints.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;
        rc = SNMP_ERR_NOERROR;

        switch(current->tri->colnum) {

        case COLUMN_NETAPPLICATIONMONITORINGCREATOR:
            /** OCTETSTR = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSERVERADDRESS:
            /** IPADDR = ASN_IPADDRESS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_ip( *var->val.integer ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGPROCESSNAME:
            /** DisplayString = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGPORT:
            /** INTEGER = ASN_INTEGER */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSAMPLEINTERVAL:
            /** INTEGER = ASN_INTEGER */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINTERFACENUMBER:
            /** INTEGER = ASN_INTEGER */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSTARTDATE:
            /** DateAndTime = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGENDDATE:
            /** DateAndTime = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGMONSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGMONENDTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTUESTARTTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTUEENDTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGWEBSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGWEBENDTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTHUSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTHUENDTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGFRISTARTTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGFRIENDTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSATSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSATENDTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSUNSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSUNENDTIME:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGCREATEDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSTATUS:
            /** RowStatus = ASN_INTEGER */
                rc = netsnmp_check_vb_rowstatus(current->ri->requestvb,
                                                undo_ctx ?
                                                undo_ctx->netApplicationMonitoringStatus:0);
                rg->rg_void = current->ri;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGRTT:
            /** TICKS = ASN_TIMETICKS */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGAVGCPUPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGAVGMEMORYPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGOUTOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGOUTPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }

        if (rc)
           netsnmp_set_mode_request_error(MODE_SET_BEGIN, current->ri, rc);
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

/************************************************************
 * Assuming that the RESERVE phases were successful, the next
 * stage is indicated by the action value ACTION. This is used
 * to actually implement the set operation. However, this must
 * either be done into temporary (persistent) storage, or the
 * previous value stored similarly, in case any of the subsequent
 * ACTION calls fail.
 *
 * In your case, changes should be made to row_ctx. A copy of
 * the original row is in undo_ctx.
 */
void netApplicationMonitoringTable_set_action( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    netApplicationMonitoringTable_context *row_ctx = (netApplicationMonitoringTable_context *)rg->existing_row;
    netApplicationMonitoringTable_context *undo_ctx = (netApplicationMonitoringTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    int            row_err = 0;
   
    DEBUGMSGTL(("nuri/nam","Inside netApplicationMonitoringTable_set_action() \n"));
    //pthread_mutex_lock(&gContainer_mutex);

    /*
     * TODO: loop through columns, copy varbind values
     * to context structure for the row.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_NETAPPLICATIONMONITORINGCREATOR:
            /** OCTETSTR = ASN_OCTET_STR */
            memcpy(row_ctx->netApplicationMonitoringCreator,var->val.string,var->val_len);
            row_ctx->netApplicationMonitoringCreator_len = var->val_len;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->netApplicationMonitoringDescription,var->val.string,var->val_len);
            row_ctx->netApplicationMonitoringDescription_len = var->val_len;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSERVERADDRESS:
            /** IPADDR = ASN_IPADDRESS */
            row_ctx->netApplicationMonitoringServerAddress = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGPROCESSNAME:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->netApplicationMonitoringProcessName,var->val.string,var->val_len);
            row_ctx->netApplicationMonitoringProcessName_len = var->val_len;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGPORT:
            /** INTEGER = ASN_INTEGER */
            row_ctx->netApplicationMonitoringPort = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSAMPLEINTERVAL:
            /** INTEGER = ASN_INTEGER */
            row_ctx->netApplicationMonitoringSampleInterval = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINTERFACENUMBER:
            /** INTEGER = ASN_INTEGER */
            row_ctx->netApplicationMonitoringInterfaceNumber = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSTARTDATE:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->netApplicationMonitoringStartDate,var->val.string,var->val_len);
            row_ctx->netApplicationMonitoringStartDate_len = var->val_len;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGENDDATE:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->netApplicationMonitoringEndDate,var->val.string,var->val_len);
            row_ctx->netApplicationMonitoringEndDate_len = var->val_len;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGMONSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringMonStartTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGMONENDTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringMonEndTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTUESTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringTueStartTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTUEENDTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringTueEndTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGWEBSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringWebStartTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGWEBENDTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringWebEndTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTHUSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringThuStartTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTHUENDTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringThuEndTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGFRISTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringFriStartTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGFRIENDTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringFriEndTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSATSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringSatStartTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSATENDTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringSatEndTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSUNSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringSunStartTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSUNENDTIME:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringSunEndTime = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGCREATEDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->netApplicationMonitoringCreateDateTime,var->val.string,var->val_len);
            row_ctx->netApplicationMonitoringCreateDateTime_len = var->val_len;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSTATUS:
            /** RowStatus = ASN_INTEGER */
            row_ctx->netApplicationMonitoringStatus = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGRTT:
            /** TICKS = ASN_TIMETICKS */
            row_ctx->netApplicationMonitoringRTT = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGAVGCPUPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->netApplicationMonitoringAvgCPUPercentage = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGAVGMEMORYPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->netApplicationMonitoringAvgMemoryPercentage = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
            row_ctx->netApplicationMonitoringInOctets = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
            row_ctx->netApplicationMonitoringInPkts = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGOUTOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
            row_ctx->netApplicationMonitoringOutOctets = *var->val.integer;
        break;

        case COLUMN_NETAPPLICATIONMONITORINGOUTPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
            row_ctx->netApplicationMonitoringOutPkts = *var->val.integer;
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
#ifndef netApplicationMonitoringTable_CAN_MODIFY_ACTIVE_ROW
    if( undo_ctx && RS_IS_ACTIVE(undo_ctx->netApplicationMonitoringStatus) &&
        row_ctx && RS_IS_ACTIVE(row_ctx->netApplicationMonitoringStatus) ) {
            row_err = 1;
    }
#endif

    /*
     * check activation/deactivation
     */
    row_err = netsnmp_table_array_check_row_status(&cb, rg,
                                  row_ctx ? &row_ctx->netApplicationMonitoringStatus : NULL,
                                  undo_ctx ? &undo_ctx->netApplicationMonitoringStatus : NULL);

   	/* Begin: WIP_NURI_ALL */
	if(((row_ctx->netApplicationMonitoringServerAddress <= 0 )  ||
		(row_ctx->netApplicationMonitoringPort <=0 ) || 
		(row_ctx->netApplicationMonitoringProcessName_len==0)) && 
		(row_ctx->netApplicationMonitoringStatus==4 ||
		row_ctx->netApplicationMonitoringStatus==1)) {
		row_err=1;
	} 
	/* End: WIP_NURI_ALL */

    if(row_err) {
        netsnmp_set_mode_request_error(MODE_SET_BEGIN,
                                       (netsnmp_request_info*)rg->rg_void,
                                       row_err);
      //  pthread_mutex_unlock(&gContainer_mutex);
        return;
    }

    //pthread_mutex_unlock(&gContainer_mutex);
    /*
     * TODO: if you have dependencies on other tables, this would be
     * a good place to check those, too.
     */
}

/************************************************************
 * Only once the ACTION phase has completed successfully, can
 * the final COMMIT phase be run. This is used to complete any
 * writes that were done into temporary storage, and then release
 * any allocated resources. Note that all the code in this phase
 * should be "safe" code that cannot possibly fail (cue
 * hysterical laughter). The whole intent of the ACTION/COMMIT
 * division is that all of the fallible code should be done in
 * the ACTION phase, so that it can be backed out if necessary.
 *
 * BEFORE calling this routine, the agent will update the
 * container (inserting a row if row_created == 1, or removing
 * the row if row_deleted == 1).
 *
 * AFTER calling this routine, the agent will delete the
 * undo_info.
 */
void netApplicationMonitoringTable_set_commit( netsnmp_request_group *rg )
{
    FILE *fp = NULL;
    netApplicationMonitoringTable_context *mycontext = NULL;
    netsnmp_variable_list *var;
    netApplicationMonitoringTable_context *row_ctx = (netApplicationMonitoringTable_context *)rg->existing_row;
    netApplicationMonitoringTable_context *undo_ctx = (netApplicationMonitoringTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    DEBUGMSGTL(("nuri/nam","Inside netApplicationMonitoringTable_set_commit() \n"));
    pthread_mutex_lock(&gContainer_mutex);
    DEBUGMSGTL(("nuri/nam","After lock set_commit() \n"));

    /* Begin: WIP_NURI_ALL */
    /*Added on 8.12.2003 */
	if(row_ctx->netApplicationMonitoringStatus==4 || 
	   row_ctx->netApplicationMonitoringStatus==1) 
    {
		NAM_ResetCounters(row_ctx);
        giNAMContextChanged = 1;
	}
	/* End: WIP_NURI_ALL */

    fp = fopen (NAM_CONTAINER_FILE,"wb");

    if (!fp)
    {
        DEBUGMSGTL(("nuri/nam",
                    "Couldn't open NAM_CONTAINER_FILE error no: %d\n", errno));
        pthread_mutex_unlock(&gContainer_mutex);
        return;
    }

    mycontext = CONTAINER_FIRST(cb.container);
    if (mycontext)
    {
        /* Assign mycontext index->oids to idx */
        mycontext->idx = (unsigned long) *mycontext->index.oids;

        if (fwrite (mycontext,sizeof (netApplicationMonitoringTable_context), 1, fp) != 1)
        {
            DEBUGMSGTL (("nuri/nam", "1 fwrite failed errno=%d\n", errno));
            pthread_mutex_unlock(&gContainer_mutex);
            /* Added by Sai on 08.12.2003 */
            if (fp != NULL)
            {
                fclose(fp);
            }
            return;
        }
    }

    while( mycontext ) {
        mycontext = CONTAINER_NEXT(cb.container,mycontext);
        if(mycontext)
        {
            /* Assign mycontext index->oids to idx */
            mycontext->idx = (unsigned long) *mycontext->index.oids;

            if (fwrite (mycontext, sizeof (netApplicationMonitoringTable_context),1,fp) != 1)
            {
                DEBUGMSGTL (("nuri/nam",
                             "2 fwrite mycontext failed errno=%d\n", errno));
                pthread_mutex_unlock(&gContainer_mutex);
                /* Added on 08.12.2003 by Sai */
                if (fp != NULL)
                {
                    fclose(fp);
                }
                return;
             }
         }
    }

    if (fp != NULL)
    {
        fclose(fp);
    }


    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_NETAPPLICATIONMONITORINGCREATOR:
            /** OCTETSTR = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSERVERADDRESS:
            /** IPADDR = ASN_IPADDRESS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGPROCESSNAME:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGPORT:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSAMPLEINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINTERFACENUMBER:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSTARTDATE:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGENDDATE:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGMONSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGMONENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTUESTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTUEENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGWEBSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGWEBENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTHUSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTHUENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGFRISTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGFRIENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSATSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSATENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSUNSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSUNENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGCREATEDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSTATUS:
            /** RowStatus = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGRTT:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGAVGCPUPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGAVGMEMORYPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGOUTOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGOUTPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */

    /* Form linked list once again */
    pthread_mutex_unlock(&gContainer_mutex);
    DEBUGMSGTL(("nuri/nam","Inside set_commit() b4 calling NAM_callback() \n"));
    NAM_callback(1, (void *) NULL);
    /* Trigger RTC to collect variables if it is idle */
    if (guiReg1 != 0)
    {
        giNAMContextChanged = 0;
        NAM_callback (1, (void *) NULL);
    }
    else
    {
        giNAMContextChanged = 1;
    }

}

/************************************************************
 * If either of the RESERVE calls fail, the write routines
 * are called again with the FREE action, to release any resources
 * that have been allocated. The agent will then return a failure
 * response to the requesting application.
 *
 * AFTER calling this routine, the agent will delete undo_info.
 */
void netApplicationMonitoringTable_set_free( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    netApplicationMonitoringTable_context *row_ctx = (netApplicationMonitoringTable_context *)rg->existing_row;
    netApplicationMonitoringTable_context *undo_ctx = (netApplicationMonitoringTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_NETAPPLICATIONMONITORINGCREATOR:
            /** OCTETSTR = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSERVERADDRESS:
            /** IPADDR = ASN_IPADDRESS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGPROCESSNAME:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGPORT:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSAMPLEINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINTERFACENUMBER:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSTARTDATE:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGENDDATE:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGMONSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGMONENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTUESTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTUEENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGWEBSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGWEBENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTHUSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTHUENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGFRISTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGFRIENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSATSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSATENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSUNSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSUNENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGCREATEDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSTATUS:
            /** RowStatus = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGRTT:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGAVGCPUPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGAVGMEMORYPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGOUTOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGOUTPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
        break;

        default: /** We shouldn't get here */
        break;
            /** should have been logged in reserve1 */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

/************************************************************
 * If the ACTION phase does fail (for example due to an apparently
 * valid, but unacceptable value, or an unforeseen problem), then
 * the list of write routines are called again, with the UNDO
 * action. This requires the routine to reset the value that was
 * changed to its previous value (assuming it was actually changed),
 * and then to release any resources that had been allocated. As
 * with the FREE phase, the agent will then return an indication
 * of the error to the requesting application.
 *
 * BEFORE calling this routine, the agent will update the container
 * (remove any newly inserted row, re-insert any removed row).
 *
 * AFTER calling this routing, the agent will call row_copy
 * to restore the data in existing_row from the date in undo_info.
 * Then undo_info will be deleted (or existing row, if row_created
 * == 1).
 */
void netApplicationMonitoringTable_set_undo( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    netApplicationMonitoringTable_context *row_ctx = (netApplicationMonitoringTable_context *)rg->existing_row;
    netApplicationMonitoringTable_context *undo_ctx = (netApplicationMonitoringTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_NETAPPLICATIONMONITORINGCREATOR:
            /** OCTETSTR = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSERVERADDRESS:
            /** IPADDR = ASN_IPADDRESS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGPROCESSNAME:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGPORT:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSAMPLEINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINTERFACENUMBER:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSTARTDATE:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGENDDATE:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGMONSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGMONENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTUESTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTUEENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGWEBSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGWEBENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTHUSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGTHUENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGFRISTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGFRIENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSATSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSATENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSUNSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSUNENDTIME:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGCREATEDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGSTATUS:
            /** RowStatus = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGRTT:
            /** TICKS = ASN_TIMETICKS */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGAVGCPUPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGAVGMEMORYPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGINPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGOUTOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
        break;

        case COLUMN_NETAPPLICATIONMONITORINGOUTPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

#endif /** netApplicationMonitoringTable_SET_HANDLING */


/************************************************************
 *
 * Initialize the netApplicationMonitoringTable table by defining its contents and how it's structured
 */
void
initialize_table_netApplicationMonitoringTable(void)
{
    netsnmp_table_registration_info *table_info;

    if(my_handler) {
        snmp_log(LOG_ERR, "initialize_table_netApplicationMonitoringTable_handler called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /* if your table is read only, it's easiest to change the
       HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY */
    my_handler = netsnmp_create_handler_registration("netApplicationMonitoringTable",
                                             netsnmp_table_array_helper_handler,
                                             netApplicationMonitoringTable_oid,
                                             netApplicationMonitoringTable_oid_len,
                                             HANDLER_CAN_RWRITE);
            
    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_netApplicationMonitoringTable_handler\n");
        return; /** mallocs failed */
    }

    /***************************************************
     * Setting up the table's definition
     */
    /*
     * TODO: add any external indexes here.
     */

    /*
     * internal indexes
     */
        /** index: netApplicationMonitoringIndex */
        netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

    table_info->min_column = netApplicationMonitoringTable_COL_MIN;
    table_info->max_column = netApplicationMonitoringTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = netApplicationMonitoringTable_get_value;
    cb.container = netsnmp_container_find("netApplicationMonitoringTable_primary:"
                                          "netApplicationMonitoringTable:"
                                          "table_container");
#ifdef netApplicationMonitoringTable_IDX2
    netsnmp_container_add_index(cb.container,
                                netsnmp_container_find("netApplicationMonitoringTable_secondary:"
                                                       "netApplicationMonitoringTable:"
                                                       "table_container"));
    cb.container->next->compare = netApplicationMonitoringTable_cmp;
#endif
#ifdef netApplicationMonitoringTable_SET_HANDLING
    cb.can_set = 1;
#ifdef netApplicationMonitoringTable_ROW_CREATION
    cb.create_row = (UserRowMethod*)netApplicationMonitoringTable_create_row;
#endif
    cb.duplicate_row = (UserRowMethod*)netApplicationMonitoringTable_duplicate_row;
    cb.delete_row = (UserRowMethod*)netApplicationMonitoringTable_delete_row;
    cb.row_copy = (Netsnmp_User_Row_Operation *)netApplicationMonitoringTable_row_copy;

    cb.can_activate = (Netsnmp_User_Row_Action *)netApplicationMonitoringTable_can_activate;
    cb.can_deactivate = (Netsnmp_User_Row_Action *)netApplicationMonitoringTable_can_deactivate;
    cb.can_delete = (Netsnmp_User_Row_Action *)netApplicationMonitoringTable_can_delete;

    cb.set_reserve1 = netApplicationMonitoringTable_set_reserve1;
    cb.set_reserve2 = netApplicationMonitoringTable_set_reserve2;
    cb.set_action = netApplicationMonitoringTable_set_action;
    cb.set_commit = netApplicationMonitoringTable_set_commit;
    cb.set_free = netApplicationMonitoringTable_set_free;
    cb.set_undo = netApplicationMonitoringTable_set_undo;
#endif
    DEBUGMSGTL(("initialize_table_netApplicationMonitoringTable",
                "Registering table netApplicationMonitoringTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);
}

/************************************************************
 * netApplicationMonitoringTable_get_value
 */
int netApplicationMonitoringTable_get_value(
            netsnmp_request_info *request,
            netsnmp_index *item,
            netsnmp_table_request_info *table_info )
{
    netsnmp_variable_list *var = request->requestvb;
    netApplicationMonitoringTable_context *context = (netApplicationMonitoringTable_context *)item;

    switch(table_info->colnum) {

        case COLUMN_NETAPPLICATIONMONITORINGINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->netApplicationMonitoringIndex,
                         sizeof(context->netApplicationMonitoringIndex) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGCREATOR:
            /** OCTETSTR = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->netApplicationMonitoringCreator,
                         context->netApplicationMonitoringCreator_len );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->netApplicationMonitoringDescription,
                         context->netApplicationMonitoringDescription_len );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGSERVERADDRESS:
            /** IPADDR = ASN_IPADDRESS */
            snmp_set_var_typed_value(var, ASN_IPADDRESS,
                         (char*)&context->netApplicationMonitoringServerAddress,
                         sizeof(context->netApplicationMonitoringServerAddress) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGPROCESSNAME:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->netApplicationMonitoringProcessName,
                         context->netApplicationMonitoringProcessName_len );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGPORT:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->netApplicationMonitoringPort,
                         sizeof(context->netApplicationMonitoringPort) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGSAMPLEINTERVAL:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->netApplicationMonitoringSampleInterval,
                         sizeof(context->netApplicationMonitoringSampleInterval) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGINTERFACENUMBER:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->netApplicationMonitoringInterfaceNumber,
                         sizeof(context->netApplicationMonitoringInterfaceNumber) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGSTARTDATE:
            /** DateAndTime = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->netApplicationMonitoringStartDate,
                         context->netApplicationMonitoringStartDate_len );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGENDDATE:
            /** DateAndTime = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->netApplicationMonitoringEndDate,
                         context->netApplicationMonitoringEndDate_len );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGMONSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringMonStartTime,
                         sizeof(context->netApplicationMonitoringMonStartTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGMONENDTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringMonEndTime,
                         sizeof(context->netApplicationMonitoringMonEndTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGTUESTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringTueStartTime,
                         sizeof(context->netApplicationMonitoringTueStartTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGTUEENDTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringTueEndTime,
                         sizeof(context->netApplicationMonitoringTueEndTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGWEBSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringWebStartTime,
                         sizeof(context->netApplicationMonitoringWebStartTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGWEBENDTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringWebEndTime,
                         sizeof(context->netApplicationMonitoringWebEndTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGTHUSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringThuStartTime,
                         sizeof(context->netApplicationMonitoringThuStartTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGTHUENDTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringThuEndTime,
                         sizeof(context->netApplicationMonitoringThuEndTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGFRISTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringFriStartTime,
                         sizeof(context->netApplicationMonitoringFriStartTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGFRIENDTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringFriEndTime,
                         sizeof(context->netApplicationMonitoringFriEndTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGSATSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringSatStartTime,
                         sizeof(context->netApplicationMonitoringSatStartTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGSATENDTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringSatEndTime,
                         sizeof(context->netApplicationMonitoringSatEndTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGSUNSTARTTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringSunStartTime,
                         sizeof(context->netApplicationMonitoringSunStartTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGSUNENDTIME:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringSunEndTime,
                         sizeof(context->netApplicationMonitoringSunEndTime) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGCREATEDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->netApplicationMonitoringCreateDateTime,
                         context->netApplicationMonitoringCreateDateTime_len );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGSTATUS:
            /** RowStatus = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->netApplicationMonitoringStatus,
                         sizeof(context->netApplicationMonitoringStatus) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGRTT:
            /** TICKS = ASN_TIMETICKS */
            snmp_set_var_typed_value(var, ASN_TIMETICKS,
                         (char*)&context->netApplicationMonitoringRTT,
                         sizeof(context->netApplicationMonitoringRTT) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGAVGCPUPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->netApplicationMonitoringAvgCPUPercentage,
                         sizeof(context->netApplicationMonitoringAvgCPUPercentage) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGAVGMEMORYPERCENTAGE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->netApplicationMonitoringAvgMemoryPercentage,
                         sizeof(context->netApplicationMonitoringAvgMemoryPercentage) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGINOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
            snmp_set_var_typed_value(var, ASN_COUNTER,
                         (char*)&context->netApplicationMonitoringInOctets,
                         sizeof(context->netApplicationMonitoringInOctets) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGINPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
            snmp_set_var_typed_value(var, ASN_COUNTER,
                         (char*)&context->netApplicationMonitoringInPkts,
                         sizeof(context->netApplicationMonitoringInPkts) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGOUTOCTETS:
            /** COUNTER64 = ASN_COUNTER64 */
            snmp_set_var_typed_value(var, ASN_COUNTER,
                         (char*)&context->netApplicationMonitoringOutOctets,
                         sizeof(context->netApplicationMonitoringOutOctets) );
        break;
    
        case COLUMN_NETAPPLICATIONMONITORINGOUTPKTS:
            /** COUNTER64 = ASN_COUNTER64 */
            snmp_set_var_typed_value(var, ASN_COUNTER,
                         (char*)&context->netApplicationMonitoringOutPkts,
                         sizeof(context->netApplicationMonitoringOutPkts) );
        break;
    
    default: /** We shouldn't get here */
        snmp_log(LOG_ERR, "unknown column in "
                 "netApplicationMonitoringTable_get_value\n");
        return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}

/************************************************************
 * netApplicationMonitoringTable_get_by_idx
 */
const netApplicationMonitoringTable_context *
netApplicationMonitoringTable_get_by_idx(netsnmp_index * hdr)
{
    return (const netApplicationMonitoringTable_context *)
        CONTAINER_FIND(cb.container, hdr );
}

/* <Start: WIP_NURI_ALL > */

/*****************************************************************************
 * name             :   NAM_callback
 * description      :   This is a main function for Network Application Monitoring
 *                      module
 * input parameters :
 * output parameters:
 * return type      :   void
 * global variables :
 * calls            :  NAM_Form_Linked_List()
 *****************************************************************************/

void
NAM_callback (unsigned int reg, void *pclientarg)
{
    int iCnt=0;
    int iLength = 0;
    NAM_List *stList = NULL;
    NAM_List *stHead = NULL;
    NAM_List *stNewHead = NULL;

    DEBUGMSGTL(("nuri/nam", "STARTING NAM_callback.....\n"));

    /* Unregister pending alarms registerd by RTC, if any */

    if (guiReg1 != 0)
    {
        DEBUGMSGTL(("nuri/nam", "Unregistering guiReg1.....\n"));
        snmp_alarm_unregister(guiReg1);
        guiReg1 = 0;
    }
    if (guiReg2 != 0)
    {
        DEBUGMSGTL(("nuri/nam", "Unregistering guiReg2.....\n"));
        snmp_alarm_unregister(guiReg2);
        guiReg2 = 0;
    }
    if (guiReg3 != 0)
    {
        DEBUGMSGTL(("nuri/nam", "Unregistering guiReg3.....\n"));
        snmp_alarm_unregister(guiReg3);
        guiReg3 = 0;
    }

    if(gbIfInProcess)
    {
        return;
    }

    /*Reset the context changed*/
    giNAMContextChanged = 0;
    gPrevInterval    = 0;
    gServerIp        = 0;
    NAM_ClearProcList();
    pthread_mutex_lock(&gList_mutex);
    /* Deallocate linked list of intervals before creating a new one */
    if (gpstHeadList)
    {
        iCnt=0;
        while (gpstHeadList->next != NULL)
        {
            stNewHead = gpstHeadList->next;
            free (gpstHeadList);
            gpstHeadList = stNewHead;
            iCnt++;
        }
        free (gpstHeadList);
        iCnt++;
        DEBUGMSGTL(("nuri/nam",
                    "After freeing <%d> nodes from linked list\n", iCnt));
    }

    /* Allocate a new linked list */
    stList = (NAM_List *) calloc (1, sizeof (NAM_List));
    stHead = stList;

    /* Form Linked List of intervals and return the length of linked list */
    NAM_Form_Linked_List (stList, &iLength);

    DEBUGMSGTL(("nuri/nam", "Length of new linked list: <%d>\n", iLength));

    /* If Linked List length is zero, wait for NAM_IDLE_TIME (1 minute) 
       and call NAM_callback() to try to form the linked list once again */
    if (iLength == 0)
    {
      DEBUGMSGTL(("nuri/nam",
                  "No matching row present in NAM Table!!\n"));
      pthread_mutex_unlock(&gList_mutex);
      guiReg1 = snmp_alarm_register (NAM_IDLE_TIME, (int) NULL, NAM_callback,
                                                                        NULL);
      free (stHead);
      return;
    }

    DEBUGMSGTL(("nuri/nam", "initdelay=%d index=%lu\n", stList->interval,
                stList->my_index));
 
    stList = stHead;

    stNewHead = (NAM_List *) calloc (1, sizeof (NAM_List));
    stNewHead->interval = 0;
    stNewHead->my_index    = 0;
    stNewHead->next     = stList;
    stList              = stNewHead;

    /* Sort the Linked List using bubble sort algorithm */
    NAM_Sort_Linked_List (stList);

    stNewHead = stList->next;
    free (stList);
    stList = stNewHead;

    gpstHeadList = stList;


   DEBUGMSGTL(("nuri/nam", "Waiting for <%d> seconds\n", stList->interval));

    /* Wait for linked list first node 'interval' seconds to process  */
    
    pthread_mutex_unlock(&gList_mutex);
    guiReg2 = snmp_alarm_register (stList->interval, (int) NULL,
                                NAM_Collect_Store_Variables, (void *) stList);
    
    return;
}
/*****************************************************************************
 * name             :   NAM_Retrieve_And_Set_Data
 * description      :   Retrieves the variable value and updates MIB and the file
 * input parameters :   unsigned long , long
 * output parameters:   
 * return type      :   int
 * global variables :
 * calls            : 
 *****************************************************************************/
int NAM_Retrieve_And_Set_Data (unsigned long ulIndex, long interval)
{
    time_t StartTime;
    time_t EndTime;
    int iErr=0;
    int iCnt=0;
    int iCollVarLen = 0;
    unsigned long ulVal = 0;
    netsnmp_index stIdx = { 0 };
    oid array[] = { ulIndex };

    netApplicationMonitoringTable_context stCtx;
    netApplicationMonitoringTable_context *pstFound = NULL;

    char szValStr[80] = { 0 };
    char szTimeStr[80] = { 0 };
    char szVarStr[MAX_OID_LEN] = { 0 };

    unsigned int uiIFIndex    = 0;
    unsigned short uiPort     = 0;
    unsigned long  ulServerIp = 0;
    char *cpProcName = NULL;
    stSetDetails stSetDet;

    stSetDet.ulCPU = 0;
    stSetDet.ulMem = 0;
    stSetDet.ulInOctets     = 0;
    stSetDet.ulOutOctets    = 0;
    stSetDet.ulInUcastPkts  = 0;
    stSetDet.ulOutUcastPkts = 0;
    
    gbIfInProcess = TRUE;

    stIdx.len = 1;
    stIdx.oids = array;
    stCtx.index = stIdx;

  DEBUGMSGTL(("nuri/nam", "Inside Set Data.... ulIndex: %lu.\n",ulIndex));

  pthread_mutex_lock(&gContainer_mutex);  
  /* Find out the netApplicationMonitoringTable row using index */

    pstFound = CONTAINER_FIND (cb.container, &stCtx);



    if (pstFound)
    {
        /* Getting Ip Address */
        ulServerIp = pstFound->netApplicationMonitoringServerAddress;
        DEBUGMSGTL(("nuri/nam", "ServerIp: %lu.\n",ulServerIp));
        
        /* Getting Process Name */
        cpProcName = (void*) malloc(pstFound->netApplicationMonitoringProcessName_len+1);
        memset(cpProcName,0,pstFound->netApplicationMonitoringProcessName_len+1);
        strncpy(cpProcName,pstFound->netApplicationMonitoringProcessName,
                pstFound->netApplicationMonitoringProcessName_len);
        DEBUGMSGTL(("nuri/nam", "ProcName: %s.\n",cpProcName));
        
        /* Get Sub Agent Port Number from MIB */
        uiPort  = (short) pstFound->netApplicationMonitoringPort;
        DEBUGMSGTL(("nuri/nam", "Port: %d.\n",uiPort));
        
        /* Get IF Number*/
        uiIFIndex = pstFound->netApplicationMonitoringInterfaceNumber;
        DEBUGMSGTL(("nuri/nam", "IFIndex: %d.\n",uiIFIndex));
        
        pthread_mutex_unlock(&gContainer_mutex);

    }
    else
    {
        pthread_mutex_unlock(&gContainer_mutex);
        gbIfInProcess = FALSE;
        return 0;
    }
    /* Added by Sai on 12.12.2003 */
    if((ulServerIp <= 0) || (uiPort <= 0) || ((strlen(cpProcName) ==0)))
    {
        if (cpProcName != NULL)
        {
            free(cpProcName);
        }
        gbIfInProcess = FALSE;
        return 0;
    }
    DEBUGMSGTL(("nuri/nam", "Collecting Variables....\n"));
    /* Get Start Time */
    time(&StartTime);
    /*Server Ip checking added by Sai on 31.12.2003 */
    if((gPrevInterval != interval)||(gServerIp != ulServerIp))
    {
        DEBUGMSGTL(("nuri/nam", "B4 calling NAM_ClearProcList()....\n"));
        NAM_ClearProcList();
        DEBUGMSGTL(("nuri/nam", "B4 calling NAM_GetProcessDetails()....\n"));
        NAM_GetProcessDetails(ulServerIp,uiPort);

        gPrevInterval = interval;
        gServerIp     = ulServerIp;
    }

    DEBUGMSGTL(("nuri/nam", "Before NAM_GetValue...\n"));
    if (NAM_GetValue(cpProcName,ulServerIp,uiPort,uiIFIndex,&stSetDet) 
        == NASC_SUCCESS )
    { 
        long RTT = 0;
        /* Get End Time */
        time(&EndTime);
        RTT = (long) difftime(EndTime, StartTime);
        RTT = RTT * 100; /* to convert seconds to time ticks*/
            /*Set has happened*/
        if (giNAMContextChanged)
        {
            gbIfInProcess = FALSE;
            return 0;
        }

        pthread_mutex_lock(&gContainer_mutex);
        pstFound = CONTAINER_FIND (cb.container, &stCtx);
        if(pstFound)
        {
            pstFound->netApplicationMonitoringAvgCPUPercentage    = stSetDet.ulCPU;
            pstFound->netApplicationMonitoringAvgMemoryPercentage = stSetDet.ulMem;
            pstFound->netApplicationMonitoringInOctets  = stSetDet.ulInOctets;
            pstFound->netApplicationMonitoringOutOctets = stSetDet.ulOutOctets;
            pstFound->netApplicationMonitoringInPkts    = stSetDet.ulInUcastPkts;
            pstFound->netApplicationMonitoringOutPkts   = stSetDet.ulOutUcastPkts;
            pstFound->netApplicationMonitoringRTT       = RTT;
            pthread_mutex_unlock(&gContainer_mutex);
        }
        else
        {
            DEBUGMSGTL(("nuri/nam", "CONTAINER_FIND failed in NAM_Retrieve_And_Set_Data after NAM_GetValue "));
            pthread_mutex_unlock(&gContainer_mutex);
        }

        DEBUGMSGTL(("nuri/nam", "ProcessName: %s, CPU: %lu, Mem: %lu\n",
        cpProcName,stSetDet.ulCPU,stSetDet.ulMem));

        DEBUGMSGTL(("nuri/nam", "InOctets: %lu, InPkts: %lu, OutOctets: %lu, OutPkts: %lu\n",
        stSetDet.ulInOctets,
        stSetDet.ulInUcastPkts,
        stSetDet.ulOutOctets,
        stSetDet.ulOutUcastPkts));

        NAM_WriteToLogFile(cpProcName,stSetDet,TRUE);
    }
    else
    {
        DEBUGMSGTL(("nuri/nam", "Process: \"%s\" is not running\n",cpProcName));
        NAM_WriteToLogFile(cpProcName,stSetDet,FALSE);
    }

    if (cpProcName != NULL)
    {
        free(cpProcName);
    }

    gbIfInProcess = FALSE;
    return 1; 
} 

/*****************************************************************************
 * name             :   NAM_Load_Container_From_File
 * description      :   Loads the netApplicationMonitoringTable
 *                      from file to memory context
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :   
 *****************************************************************************/

void
NAM_Load_Container_From_File ()
{
    FILE *fp=NULL;
    int iCnt=0;
    netApplicationMonitoringTable_context stReadCtx;
    netApplicationMonitoringTable_context *pstContext = NULL;

    /* Open the container file and load all the netApplicationMonitoringTable
       rows from this file during start up. */

    fp = fopen (NAM_CONTAINER_FILE, "rb");

    if (fp==NULL)
    {
      DEBUGMSGTL(("nuri/nam",
                  "NAM_CONTAINER_FILE file open failed errno=%d\n", errno));
      return;
    }
    else
    {
	/* Loading the netApplicationMonitoringTable rows from file to sub-agent memory */
        while (fread (&stReadCtx, sizeof (stReadCtx),1,fp) == 1) 
        {
            pstContext = SNMP_MALLOC_TYPEDEF (netApplicationMonitoringTable_context);
            memcpy (pstContext, &stReadCtx, sizeof (stReadCtx));

            netsnmp_assert(pstContext->index.oids == NULL);
            if(snmp_clone_mem( (void*)&pstContext->index.oids, &stReadCtx.idx,
                             stReadCtx.index.len * sizeof(oid) )) 
            {
                    if(fp != NULL)
                    {
                        fclose(fp);
                    }
                    return;
            }

            pstContext->index.len = stReadCtx.index.len;

            iCnt = CONTAINER_INSERT (cb.container, pstContext);
            if (iCnt)
            {
                DEBUGMSGTL(("nuri/nam", "ERROR IN CONTAINER_INSERT FUNCION\n"));
            }
       }

        DEBUGMSGTL(("nuri/nam",
                    "CONTAINER SIZE=%u\n", CONTAINER_SIZE (cb.container)));
    }

    /* Close the file */
    if(fp != NULL)
    {
        fclose(fp);
    }

    return;
}

/*****************************************************************************
 * name             :   NAM_Find_Current_Day
 * description      :   To find present day in numeric format
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :   NAM_Add_Intervals_To_List
 *****************************************************************************/

int
NAM_Find_Current_Day()
{
    struct tm stTm = { 0 };
    struct timeval  stTv = { 0 };

    /* get the present time in seconds */
    gettimeofday(&stTv, (struct timezone *) 0);

    /* convert seconds into tm structure and return the week day */
    memcpy(&stTm, localtime((time_t *) & stTv.tv_sec), sizeof(stTm));

    return stTm.tm_wday;
}

/*****************************************************************************
 * name             :   NAM_Get_Time_Frequency
 * description      :   To store <day>StartTime, <day>EndTime, <day>Frequent
 *                      based on the day.
 * input parameters :
 * output parameters:
 * return type      :   void
 * global variables :
 * calls            :   NAM_Add_Intervals_To_List
 *****************************************************************************/

void
NAM_Get_Time_Frequency (int iToday, netApplicationMonitoringTable_context *pstCtx,
                    unsigned long *pulSTime, unsigned long *pulETime, int *piFreq)
{
    if (pstCtx == NULL || pulSTime == NULL || pulETime == NULL || piFreq == NULL)
    {
        DEBUGMSGTL(("nuri/nam", "Invalid Argument!!"));
        return;
    }

    /* Fill up start time, end time and frequency based on iToday */
    switch (iToday)
    {
        case 0:
	    /* NAM SunStartTime and NAM SunEndTime are
	       in centi seconds. Convert it to seconds */

            *pulSTime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringMonStartTime);
            *pulETime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringMonEndTime);
            break;
        case 1:
            *pulSTime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringTueStartTime);
            *pulETime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringTueEndTime);
            break;
        case 2:
            *pulSTime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringWebStartTime);
            *pulETime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringWebEndTime);
            break;
        case 3:
            *pulSTime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringThuStartTime);
            *pulETime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringThuEndTime);
            break;
        case 4:
            *pulSTime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringFriStartTime);
            *pulETime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringFriEndTime);
            break;
        case 5:
            *pulSTime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringSatStartTime);
            *pulETime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringSatEndTime);
            break;
        case 6:
            *pulSTime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringSunStartTime);
            *pulETime = CONVERT_TO_SEC (pstCtx->netApplicationMonitoringSunEndTime);
            break;
        default:
            DEBUGMSGTL(("nuri/nam", "Invalid Day=%d\n", iToday));
            break;
    }
    *piFreq  = pstCtx->netApplicationMonitoringSampleInterval;
    return;
}

/*****************************************************************************
 * name             :   NAM_Form_Linked_List
 * description      :   To form linked list containing interval and tableIndex.
 * input parameters :
 * output parameters:   NAM_List *, int
 * return type      :   int
 * global variables :
 * calls            :   NAM_callback
 *****************************************************************************/

int
NAM_Form_Linked_List (NAM_List *pstList, int *piLength)
{
    int iToday = 0;
    int iFreq  = 0;
    int iListLen = 0;
    int iInterval = 0;

    unsigned long ulCTime = 0;
    unsigned long ulSTime = 0;
    unsigned long ulETime = 0;

    netApplicationMonitoringTable_context *pstCtx = NULL;

    /* Get present day in numeric format Eg: sun=0, mon=1 etc. */
    iToday = NAM_Find_Current_Day();

    /* Calculate the interval based on the NAM_MAX_LIST_NODES */
    NAM_Calc_Num_Nodes_Interval (&iInterval);

    DEBUGMSGTL(("nuri/nam", "#####iInterval=%d#####\n", iInterval));

    /* Read the first row of netApplicationMonitoringTable and assign it to a pointer */
    pstCtx = CONTAINER_NEXT (cb.container, NULL);

    while (pstCtx)
    {
	/* Is present time within netApplicationMonitoringstartDate and EndDate? */
        if (NAM_Within_Period (pstCtx) == 0)
        {
	    /* Get the start time, end time and frequency based on iToday */
            NAM_Get_Time_Frequency (iToday, pstCtx, &ulSTime, &ulETime, &iFreq);

	    /* Get current time in seconds from midnight 00:00 */
            ulCTime = NAM_Get_Current_Time ();

            /* Is current time within start time and end time? */
            if (ulCTime > ulSTime && ulCTime < ulETime)
            {
		/* Add the intervals for the row in the linked list */
                NAM_Add_Intervals_To_List (pstCtx, pstList, &iInterval,
                                           &iListLen);

                /* Move to the end of the linked list */
                while (pstList->next != NULL)
                {
                    pstList = pstList->next;
                }
           }
        }
        pstCtx = CONTAINER_NEXT (cb.container, pstCtx);
    }
    *piLength = iListLen;
    return 0;
}

/*****************************************************************************
 * name             :   NAM_Convert_Date_To_Long
 * description      :   Converts DateAndTime format to unsigned long format
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :   
 *****************************************************************************/
int
NAM_Convert_Date_To_Long (char *pcDate, unsigned long *pulDate)
{
    int iYear=0;
    struct tm stTm = { 0 };

    /* Convert DateAndTime format into unsigned long format */

    iYear = pcDate[0];
    iYear = (iYear << 8) | (pcDate[1] & 0xff);
    stTm.tm_year = iYear - 1900;

    stTm.tm_mon  = (int) (pcDate[2] & 0xff) - 1;
    stTm.tm_mday = (int) (pcDate[3] & 0xff);
    stTm.tm_hour = (int) (pcDate[4] & 0xff);
    stTm.tm_min =  (int) (pcDate[5] & 0xff);
    stTm.tm_sec =  (int) (pcDate[6] & 0xff);

    DEBUGMSGTL(("nuri/rtc", "***** iYear=%d *****\n", iYear));
    DEBUGMSGTL(("nuri/rtc", "**********month=%d day=%d "
                "hour=%d min=%d sec=%d*****\n", stTm.tm_mon+1, stTm.tm_mday,
                stTm.tm_hour, stTm.tm_min, stTm.tm_sec));
    
#ifdef DUMMY
    if (daylight)
        stTm.tm_isdst = 1;

    stTm.tm_sec -= timezone;
#endif /* DUMMY */

    /* Assuming that mktime will take care of timezone issue */

    *pulDate = mktime(&stTm);

    DEBUGMSGTL(("nuri/rtc", "long format date=%lu\n", pulDate));
    return 0;
}

/*****************************************************************************
 * name             :   NAM_Calc_Num_Nodes_Interval
 * description      :   Calculates duration for which linked list has to be 
 *                      formed based on NAM_MAX_LIST_NODES.
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :   
 *****************************************************************************/

int
NAM_Calc_Num_Nodes_Interval (int *pInterval)
{
    int iCnt = 0;
    int iToday = 0;
    int iFreq  = 0;
    int iInterval = 0;
    int iInitDelay  = 0;

    unsigned long ulCTime = 0;
    unsigned long ulSTime = 0;
    unsigned long ulETime = 0;
    unsigned long ulASTime  = 0;

    netApplicationMonitoringTable_context *pstCtx = NULL;

    /* Get today in numeric format */
    iToday = NAM_Find_Current_Day();

    /* Read the first row of netApplicationMonitoringTable */
    pstCtx = CONTAINER_NEXT (cb.container, NULL);

    /* Start calculating the interval from INIT_INTERVAL */
    iInterval = INIT_INTERVAL;

    while (pstCtx)
    {
	/* Is present time within netApplicationMonitoringDatastartDate and EndDate? */
        if (NAM_Within_Period (pstCtx) == 0)
        {
	    /* Get the start time, end time and frequency based on iToday */
            NAM_Get_Time_Frequency (iToday, pstCtx, &ulSTime, &ulETime, &iFreq);

            if (iFreq == 0)
            {
                pstCtx = CONTAINER_NEXT (cb.container, pstCtx);
                continue;
            }

	    /* Get current time in seconds from midnight 00:00 */
            ulCTime = NAM_Get_Current_Time ();

            /* Calculate initial delay which will be the difference between
	       scheduled execution time and present time. */
            iInitDelay = iFreq - ((ulCTime - ulSTime)%iFreq);

            if (iInitDelay == iFreq)
            {
                /* Minimum initDelay can be 1 second */
                iInitDelay = 1;
            }

            
            /* Is current time within start time and end time? */
            if (ulCTime > ulSTime && ulCTime < ulETime)
            {
                ulASTime = iInitDelay + ulCTime;

                if (ulASTime > ulETime)
                {
                    /* Actual time should not exceed End Time */
                    pstCtx = CONTAINER_NEXT (cb.container, pstCtx);

                    continue;
                }
                iCnt += 1;

		/* Calculate the possible number of linked list nodes for
		   current row for the interval specified */
                if (ulETime > iInterval+ulASTime)
                {
                    iCnt = iCnt + iInterval/iFreq;
                }
                else
                {
                    iCnt = iCnt + (ulETime - ulASTime)/iFreq;
                }

                /* If total number of linked list nodes exceed the limit */
                if (iCnt > NAM_MAX_LIST_NODES)
                {
                    if (iInterval != 1)
                    {
                        iCnt = 0;
                        iInterval = iInterval/2;
                        DEBUGMSGTL(("nuri/nam",
                        "Count Exceeds Limit.  New iInterval=%d\n", iInterval));
                        pstCtx = CONTAINER_NEXT (cb.container, NULL);
                        continue;
                    }
                    else
                    {
			/* interval can't be less than 1 second. Now the
			   limitation on NAM_MAX_LIST_NODES is ignored */
                        DEBUGMSGTL(("nuri/nam",
                                    "NAM_MAX_LIST_NODES exceeded\n"));
                    }
                }
            }
        }
        pstCtx = CONTAINER_NEXT (cb.container, pstCtx);
    }

    /* Assign the calculated interval */
    *pInterval = iInterval;
    return 0;
}

/*****************************************************************************
 * name             :   NAM_Add_Intervals_To_List
 * description      :   Form Complete Linked by adding intervals for all rows
 *                      for the calculated interval based on NAM_MAX_LIST_NODES
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :   
 *****************************************************************************/

int
NAM_Add_Intervals_To_List (netApplicationMonitoringTable_context *pstCtx, NAM_List *pstList,
                           int *piInterval, int *piLength)
{
    int ii = 0;
    int iCnt = 0;
    int iToday = 0;
    int iFreq  = 0;
    int iInitDelay  = 0;
    int iDelay = 0;

    unsigned long ulCTime = 0;
    unsigned long ulSTime = 0;
    unsigned long ulETime = 0;
    unsigned long ulASTime  = 0;

    iToday = NAM_Find_Current_Day();

    NAM_Get_Time_Frequency (iToday, pstCtx, &ulSTime, &ulETime, &iFreq);

    if (iFreq == 0)
    {
        DEBUGMSGTL(("nuri/nam", "NAM_Add_Intervals_To_List: "
                    "Skipping Row: Reason: iFreq is ZERO for idx=%lu."
                    "iToday=%d\n", pstCtx->netApplicationMonitoringIndex,
                    iToday));
        return 0;
    }

    ulCTime = NAM_Get_Current_Time ();

    /* If frequency is set to 0, change it to 1 second
       frequency can not be less than 1 second */
    if (iFreq == 0) iFreq = 1;

    iInitDelay = iFreq - ((ulCTime - ulSTime)%iFreq);

    if (iInitDelay == iFreq) 
    {
        iInitDelay = 1;
    }

    ulASTime = iInitDelay + ulCTime;
    
    
    if (ulETime > *piInterval+ulASTime)
    {
        iCnt = *piInterval/iFreq;
    }
    else
    {
        iCnt = (ulETime - ulCTime)/iFreq;
    }

    pstList->interval = iInitDelay;
    pstList->my_index    = pstCtx->netApplicationMonitoringIndex;

    *piLength += 1;

    /* First list is already added, so, decrementing iCnt by one */
    if (iCnt > 0) iCnt -= 1;

    iDelay = iFreq + iInitDelay;

    /* Add intervals to linked list for the current row */
    for (ii=0; ii < iCnt; ii++)
    {
        pstList->next = (NAM_List *) calloc (1, sizeof (NAM_List));
        pstList = pstList->next;

        *piLength += 1;
        pstList->interval = iDelay;
        pstList->my_index    = pstCtx->netApplicationMonitoringIndex;
        iDelay += iFreq;
    }

    /* Last node next should be NULL */
    pstList->next = (NAM_List *) calloc (1, sizeof (NAM_List));
    pstList = pstList->next;

    /* Assign unique values to last node */
    pstList->interval = 2147483647L;
    pstList->my_index = 65537;
    pstList->next = NULL;
     
    return 0;
}

/*****************************************************************************
 * name             :   NAM_Within_Period
 * description      :   Check if present time is within the SDate and EndDate
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :   , NAM_Calc_Num_Nodes_Interval
 *****************************************************************************/

int
NAM_Within_Period (netApplicationMonitoringTable_context *pstCtx)
{
    int iErr   = 0;
    time_t now = 0;
    long ulSDate = 0;
    long ulEDate = 0;
    struct timeval  stTv;

    /* Convert DateAndTime format to unsigned long format */
    iErr = NAM_Convert_Date_To_Long (pstCtx->netApplicationMonitoringStartDate,
                                     &ulSDate);
    if (iErr)
    {
        DEBUGMSGTL(("nuri/nam", "Invalid Start Date Format %s\n",
                     pstCtx->netApplicationMonitoringStartDate));
        return 1;
    } 

    iErr = NAM_Convert_Date_To_Long (pstCtx->netApplicationMonitoringEndDate,
                                     &ulEDate);
    if (iErr)
    {
        DEBUGMSGTL(("nuri/nam", "Invalid End Date Format %s\n",
                     pstCtx->netApplicationMonitoringEndDate));
        return 1;
    }

    if (ulEDate < ulSDate)
    {
        DEBUGMSGTL(("nuri/nam", "netApplicationMonitoringDataEndDate cannot be less"
                    " than collectDataStartDate\n"));
        return 1;
    }

    gettimeofday(&stTv, (struct timezone *) 0);
    now = stTv.tv_sec;

    if (now >= ulSDate && now <= ulEDate)
    {
        return 0;
    }
    return 1;
}

/*****************************************************************************
 * name             :   NAM_Get_Current_Time
 * description      :   Return seconds from 00:00 hours for current day
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :   NAM_Calc_Num_Nodes_Interval, NAM_Add_Intervals_To_List
 *****************************************************************************/

unsigned long
NAM_Get_Current_Time ()
{
    unsigned long ulValue=0;
    struct tm stTm;
    struct timeval stTv;

    gettimeofday(&stTv, (struct timezone *) 0);
    memcpy(&stTm, localtime((time_t *) & stTv.tv_sec), sizeof(stTm));
  
    ulValue = stTm.tm_hour * 60 * 60 + stTm.tm_min * 60 + stTm.tm_sec;
    return ulValue;
}

/*****************************************************************************
 * name             :   NAM_Sort_Linked_List
 * description      :   Sorts the linked list based on interval as key
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :   NAM_callback
 *****************************************************************************/
int
NAM_Sort_Linked_List (NAM_List *pstStart)
{
    NAM_List *pstP = NULL;
    NAM_List *pstQ = NULL;
    NAM_List *pstR = NULL;
    NAM_List *pstS = NULL;
    NAM_List *pstTemp = NULL;

    /* pstR precedes pstP and pstS points to the node up to which comparisons
       are to be made */ 

    /* temporary add a node and add interval zero */

    while ( pstS != pstStart -> next ) 
    { 
        pstR = pstP = pstStart; 
        pstQ = pstP -> next ;

        while ( pstP != pstS ) 
        {  
            if ( pstP -> interval > pstQ -> interval ) 
            { 
                if ( pstP == pstStart ) 
                { 
                    pstTemp = pstQ -> next ; 
                    pstQ -> next = pstP ; 
                    pstP -> next = pstTemp ;
                    pstStart = pstQ ; 
                    pstR = pstQ ; 
                } 
                else 
                { 
                    pstTemp = pstQ -> next ; 
                    pstQ -> next = pstP ; 
                    pstP -> next = pstTemp ;
                    pstR -> next = pstQ ; 
                    pstR = pstQ ; 
                } 
            } 
            else 
            { 
                pstR = pstP ; 
                pstP = pstP -> next ; 
            } 
            pstQ = pstP -> next ; 
            if ( pstQ == pstS ) 
                pstS = pstP ; 
        } 
    }

    return 0;
}

/*****************************************************************************
 * name             :   NAM_Collect_Store_Variables
 * description      :   Wait till next interval and call function to collect
 *                      and store the variable
 * input parameters :   unsigned int, void*
 * output parameters:
 * return type      :   void
 * global variables :
 * calls            :   NAM_callback, NAM_Collect_Store_Variables
 *****************************************************************************/

void
NAM_Collect_Store_Variables (unsigned int guiReg, void *pclientarg)
{
    NAM_List *pstList = NULL;
    NAM_List *newhead = NULL;
    int seconds = 0;
    int removed = 0;
    
    DEBUGMSGTL(("nuri/nam","Inside NAM_Collect_Store_Variables() \n"));
    pthread_mutex_lock(&gList_mutex);

    if(pclientarg == NULL)
    {
        pthread_mutex_unlock(&gList_mutex);
        return; 
    }
     

    pstList = (NAM_List *) pclientarg;
     
    DEBUGMSGTL(("nuri/nam", "Collecting Variable Value for"
                " interval=%d index=%lu\n", pstList->interval, pstList->my_index));

    /* Initialize the alarm regid values */
    if (guiReg == guiReg2)
        guiReg2 = 0;
    else if (guiReg == guiReg3)
        guiReg3 = 0;

    while (pstList->next != NULL)
    {
        if (pstList->interval == pstList->next->interval)
        {
	    /* Retrieve the variable and store it in a file */

            NAM_Retrieve_And_Set_Data (pstList->my_index,pstList->interval);

            /* Remove the current linked list node */
            newhead = pstList->next;
            free (pstList);
            pstList = newhead;
            gpstHeadList = pstList;
            removed = 1;
        }
        else
        {
            NAM_Retrieve_And_Set_Data (pstList->my_index,pstList->interval);
            break;
        }
        if (giNAMContextChanged)
        {
            break;
        }
    }
    
    if (giNAMContextChanged)
    {
        newhead = pstList->next;
        free (pstList);
        gpstHeadList = newhead;
        pthread_mutex_unlock(&gList_mutex);
        NAM_callback(1, (void *) NULL);
        return;
    }
    if (pstList->next)
    {
        seconds = pstList->next->interval - pstList->interval;

        /* Remove the current linked list node */
        newhead = pstList->next;
        free (pstList);
        pstList = newhead;

        gpstHeadList = pstList;

        DEBUGMSGTL(("nuri/nam", "Waiting for %d seconds\n", seconds));

        pthread_mutex_unlock(&gList_mutex);
        /* Wait for 'seconds' seconds to retrieve & store the next variable */
        if(giNAMContextChanged == 0)
        {
            guiReg3 = snmp_alarm_register (seconds, (int) NULL,
                              NAM_Collect_Store_Variables, (void *) pstList);
        }
        else
        {
            giNAMContextChanged = 0;
            NAM_callback(1, (void *) NULL);
        }
        
    }
    else
    {
        free (pstList);
        gpstHeadList = NULL;
        pthread_mutex_unlock(&gList_mutex);
        NAM_callback(1, (void *) NULL);
    }
}



/*****************************************************************************
 * name             :   NAM_Get_ASCII_Time
 * description      :   Returns current time stamp in ascii format
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :
 *****************************************************************************/

int
NAM_Get_ASCII_Time (char *pcTimeStr)
{
    char *pcPtr;
    struct tm stTm;
    struct timeval stTv;

    gettimeofday(&stTv, (struct timezone *) 0);
    memcpy(&stTm, localtime((time_t *) & stTv.tv_sec), sizeof(stTm));

    errno = 0;
    if ((pcPtr = asctime (&stTm)) ==  (char *) NULL)
    {
           DEBUGMSGTL(("nuri/nam", "asctime returns NULL errno=%d\n", errno));
           return 1;
    }

    if (pcPtr[strlen(pcPtr) - 1] == '\n')
        pcPtr[strlen(pcPtr) - 1] = '\0';

    strcpy (pcTimeStr, pcPtr);

    DEBUGMSGTL(("nuri/nam", "tv_sec=%ld pcTimeStr=%s$ errno=%d\n",
                stTv.tv_sec, pcTimeStr, errno));
    return 0;
}

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
    #ifdef WIN32
    WSADATA wsadata;
    WSAStartup(MAKEWORD( 2, 2 ),&wsadata);
    #endif
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
   /* For Session Initiation */
   snmp_sess_init(&tempSess);
   tempSess.version = SNMP_VERSION_2c;
   tempSess.peername = gcAgentSessPeerName;
   tempSess.remote_port    = giDummyPort;
   tempSess.community      = gcCommunity;
   tempSess.community_len  = strlen(gcCommunity);
   tempSess.callback_magic = NULL;


   SOCK_STARTUP;

   gAgentSession = snmp_open(&tempSess);

   if(gAgentSession == NULL) 
   {
       snmp_perror("NASCenterUM Snmp Open: Failed ");
       exit(1);
   }
}

/*****************************************************************************
 *
 * name             :   NAM_GetProcessDetails 
 * description      :   Gets Process Name, CPU Percent and MEM Percent from 
 *                      Nuri Enterprise Process Table
 * input parameters :   unsigned long, unsigned short
 * return type      :   ENASCenterError 
 * global variables :   
 * calls	        :     
 *                      
 *
 */

ENASCenterError NAM_GetProcessDetails(unsigned long ulServerIp, unsigned short uiPort)
{
    struct snmp_pdu *reqPDU  = NULL ;
    struct snmp_pdu *respPDU = NULL ;
    netsnmp_variable_list *pstCurrVars;
    char caProcessName[MAX_PROC_NAME];
    int  iCPUPercent = 0 ;
    int  iMemPercent = 0;
    oid *pProcRunName       = psRunningName_oid ;
    short iProcRunNameLen     = (sizeof(psRunningName_oid)/ sizeof(oid));
    oid *pProcPercentCPU    = psRunningPercentCPU_oid;
    short iProcPercentCPULen  = (sizeof(psRunningPercentCPU_oid)/ sizeof(oid));
    oid *pProcPercentMem    = psRunningPercentSize_oid;
    short iProcPercentMemLen  = (sizeof(psRunningPercentSize_oid)/ sizeof(oid));

    oid *pFieldOid = psRunningName_oid;
    short FieldOidLen= (sizeof(psRunningName_oid)/ sizeof(oid));

    oid *ptableID  = psRunningName_oid;
    short iTableLen  = (sizeof(psRunningName_oid)/ sizeof(oid));

    short status=0;
    short iCheckTable;
    short iCount;
    struct sockaddr_in *pServerAddress = NULL;

    DEBUGMSGTL(("nuri/nam", "Inside NAM_GetProcessDetails....\n"));
    /* Lock */
    //pthread_mutex_lock(&gProcList_mutex);

    DEBUGMSGTL(("nuri/nam", "Inside NAM_GetProcessDetails after lock....\n"));
    

    while(1) 
    {
        reqPDU  = NULL ;
        reqPDU  = snmp_pdu_create(SNMP_MSG_GETBULK);
        reqPDU->non_repeaters = 0;
        /* Added on 06.12.2003 */
        /* Note : reqPDU->community will be freed by Agent */
        reqPDU->community = calloc(1, strlen(gcCommunity)+1);
        strcpy(reqPDU->community,gcCommunity);

        /*Set the number of records to be fetched */
        reqPDU->max_repetitions = MAX_REPETITIONS;

        /* Added on 06.12.2003 */
        /*Note : pServerAddress will be freed by Agent */
        pServerAddress = calloc(1, sizeof(struct sockaddr_in));

        pServerAddress->sin_family = AF_INET;
        pServerAddress->sin_port   = htons(uiPort);
        //pServerAddress->sin_addr.s_addr  = htonl(ulServerIp);
        pServerAddress->sin_addr.s_addr  = ulServerIp;

        reqPDU->transport_data = pServerAddress;
        reqPDU->transport_data_length = sizeof(struct sockaddr_in);

        //DEBUGMSGTL(("nuri/nam", "Server IP Addr in dot format: %s \n",inet_ntoa(htonl(ulServerIp))));

       /* Add variable to be queried */
       snmp_add_null_var(reqPDU,pProcRunName,iProcRunNameLen);
       snmp_add_null_var(reqPDU,pProcPercentCPU,iProcPercentCPULen);
       snmp_add_null_var(reqPDU,pProcPercentMem,iProcPercentMemLen);

       /* Added on 06.12.2003  Moved from end of while to here
          since it was not fetching next cycle on 12.12.2003  */
       if(respPDU != NULL)
       {
            snmp_free_pdu(respPDU);
            respPDU = NULL;
       }
       /*Get the response in the respPDU*/
       DEBUGMSGTL(("nuri/nam", "Inside NAM_GetProcessDetails B4 response....\n"));
       status = snmp_synch_response(gAgentSession,reqPDU,&respPDU);	
       DEBUGMSGTL(("nuri/nam", "Inside NAM_GetProcessDetails After response....\n"));
       if(status==STAT_SUCCESS)
       {
           DEBUGMSGTL(("nuri/nam", "Request Succeeded...\n"));
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

    if(respPDU != NULL)
    {
        snmp_free_pdu(respPDU);
        respPDU = NULL;
    }


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
 * input parameters :   unsigned long, unsigned short, unsigned int,
 * output parameters:   stSetDetails*
 * return type      :   ENASCenterError 
 * global variables :   
 * calls	        :     
 *                      
 *
 */

ENASCenterError NAM_GetInterfaceDetails(unsigned long ulServerIp, unsigned short uiPort,\
                                        unsigned int uiIndex, stSetDetails *stSetDet)
{
    struct snmp_pdu *reqPDU  = NULL ;
    struct snmp_pdu *respPDU = NULL;
    netsnmp_variable_list *pstCurrVars = NULL ;
    short status=0;
    oid *pifInOctets_oid = NULL ;
    oid *pifInUcastPkts_oid = NULL ;
    oid *pifOutOctets_oid = NULL ;
    oid *pifOutUcastPkts_oid = NULL ;
    short iifInOctetsLen = 0;
    short iifInUcastPktsLen = 0;
    short iifOutOctetsLen = 0;
    short iifOutUcastPktsLen = 0;
    ENASCenterError eErrorStatus = NASC_FAILURE;
    struct sockaddr_in *pServerAddress = NULL;
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



    reqPDU = snmp_pdu_create(SNMP_MSG_GET);
    reqPDU->non_repeaters = 0;

    /* Added on 06.12.2003 */
    /* Note : reqPDU->community will be freed by Agent */
    reqPDU->community = calloc(1, strlen(gcCommunity)+1);
    strcpy(reqPDU->community,gcCommunity);

    /*Set the number of records to be fetched */
    reqPDU->max_repetitions = MAX_REPETITIONS;

    /* Added on 06.12.2003 */
    /*Note : pServerAddress will be freed by Agent */
    pServerAddress = calloc(1, sizeof(struct sockaddr_in));

    pServerAddress->sin_family = AF_INET;
    pServerAddress->sin_port   = htons(uiPort);
    //pServerAddress->sin_addr.s_addr  = htonl(ulServerIp);
    pServerAddress->sin_addr.s_addr  = ulServerIp;


    DEBUGMSGTL(("nuri/nam", "******* Ip in String Format **** %s \n",inet_ntoa(pServerAddress->sin_addr)));


    /* Set IP and port number were query has to be performed*/
    reqPDU->transport_data = pServerAddress;
    reqPDU->transport_data_length = sizeof(struct sockaddr_in);


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
                 stSetDet->ulInOctets = *pstCurrVars->val.integer;
            }
            /* Move to next variable */             
            pstCurrVars = pstCurrVars->next_variable;
            if(pstCurrVars->val.integer != NULL)  
            {
                stSetDet->ulInUcastPkts = *pstCurrVars->val.integer;
            }
            /* Move to next variable */             
            pstCurrVars = pstCurrVars->next_variable;
            if(pstCurrVars->val.integer != NULL)
            {
                 stSetDet->ulOutOctets = *pstCurrVars->val.integer;
            }
            /* Move to next variable */             
            pstCurrVars = pstCurrVars->next_variable;
            if(pstCurrVars->val.integer != NULL)  
            {
                stSetDet->ulOutUcastPkts = *pstCurrVars->val.integer;
            }
            eErrorStatus = NASC_SUCCESS;
            DEBUGMSGTL(("nuri/nam", "Test InOctets  =%lu\n", stSetDet->ulInOctets));
            DEBUGMSGTL(("nuri/nam", "Test OutOctets =%lu\n", stSetDet->ulOutOctets));
            DEBUGMSGTL(("nuri/nam", "Test InPkts    =%lu\n", stSetDet->ulInUcastPkts));
            DEBUGMSGTL(("nuri/nam", "Test OutPkts   =%lu\n", stSetDet->ulOutUcastPkts));
            /* Added on 06.12.2003*/
            if(respPDU != NULL)
            {
                snmp_free_pdu(respPDU);
            }
       }
       else
       {
           DEBUGMSGTL(("nuri/nam", "Error in fetching Interface Details\n"));
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
 * input parameters :   char*,int,int
 * output parameters:   void
 * return type      :   ENASCenterError
 * global variables :   gProcList
 * calls            :   
 */

ENASCenterError NAM_AddToProcList(char* cpProcName,int iCPUPcnt, int iMemPcnt)
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
            pstSetDet->ulCPU = pProcTemp->stProcDetails.uiProcCPUPercent;
            pstSetDet->ulMem = pProcTemp->stProcDetails.uiProcMemPercent;
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
 * input parameters :   char*,unsigned long,unsigned short,unsigned int
 * output parameters:   stSetDetails*
 * return type      :   ENASCenterError
 * global variables :   
 * calls            :   NAM_GetProcessCPUMemPercent(),NAM_GetInterfaceDetails
 */

ENASCenterError NAM_GetValue(char* cpProcName,unsigned long ulServerIp, unsigned short uiPort, 
                             unsigned int uiIFIndex, stSetDetails *pstSetDet )
{
    ENASCenterError  eErrorStatus = NASC_FAILURE;

    pstSetDet->ulCPU          = 0;
    pstSetDet->ulMem          = 0;
    pstSetDet->ulInOctets     = 0;
    pstSetDet->ulInUcastPkts  = 0;
    pstSetDet->ulOutOctets    = 0;
    pstSetDet->ulOutUcastPkts = 0;

    
    if ( NAM_GetProcessCPUMemPercent(cpProcName,pstSetDet) == NASC_SUCCESS)
    {
        DEBUGMSGTL(("nuri/nam","NAM_GetProcessCPUMemPercent().. Success \n"));
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
 * name             :   NAM_ResetCounters
 * description      :   Resets Counter whenever there is some set on the row
 * input parameters :   netApplicationMonitoringTable_context*
 * output parameters:   void
 * return type      :   void
 * global variables :   
 * calls            :   
 */

void NAM_ResetCounters(netApplicationMonitoringTable_context *ctx) 
{
	ctx->netApplicationMonitoringInOctets=0;
	ctx->netApplicationMonitoringOutOctets=0;
	ctx->netApplicationMonitoringInPkts=0;
	ctx->netApplicationMonitoringOutPkts=0;
	ctx->netApplicationMonitoringRTT=0;
    ctx->netApplicationMonitoringAvgCPUPercentage =0;
    ctx->netApplicationMonitoringAvgMemoryPercentage =0;
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
    static unsigned int ui32FileSize = 0;
    static unsigned int ui32MaxWrittenSize = 0;
    int i32WrittenSize = 0;
    char caCurrTime[DATE_TIME_LEN];
    pthread_mutex_lock(&gLogFile_mutex);
    if (gFpl == NULL)
    {
        if ((gFpl = fopen (gcTempLogFile, "w")) == NULL) 
        {
            /* File Open Failure */
            DEBUGMSGTL(("nuri/nam", " Temporary Log file open failed \n"));
            gFpl = NULL;
            pthread_mutex_unlock(&gLogFile_mutex);
            return;
        }
        else
        {
            DEBUGMSGTL(("nuri/nam", " Temporary Log file open Success \n"));
            ui32FileSize = 0;
            i32WrittenSize = 0;
            ui32MaxWrittenSize = 0;

        }

    }
    memset(caCurrTime,0,DATE_TIME_LEN);
    NAM_Get_ASCII_Time (caCurrTime);

    if (bProcStatus)
    {
        
        i32WrittenSize = fprintf (gFpl, "%s%s %s%s %lu%s %lu%s %lu%s %lu%s %lu%s %lu\n",
                        caCurrTime,NAM_DELIMITER,
                        cpProcName,NAM_DELIMITER,
                        stDetails.ulCPU,NAM_DELIMITER,
                        stDetails.ulMem,NAM_DELIMITER,
                        stDetails.ulInOctets,NAM_DELIMITER,
                        stDetails.ulOutOctets,NAM_DELIMITER,
                        stDetails.ulInUcastPkts,NAM_DELIMITER,
                        stDetails.ulOutUcastPkts);
        fflush(gFpl);
    }
    else
    {
        i32WrittenSize = fprintf (gFpl, "%s%s%s Not Running\n",caCurrTime,
                         NAM_DELIMITER, cpProcName);
        fflush(gFpl);
    }
    
    if (i32WrittenSize > 0)
    {
        ui32FileSize += i32WrittenSize;
        
        (ui32MaxWrittenSize < (unsigned int)i32WrittenSize) ? (ui32MaxWrittenSize = (unsigned int)i32WrittenSize):(ui32MaxWrittenSize);

    }
    /*Checking whether space is available for next time*/
    if ((ui32FileSize + ui32MaxWrittenSize) > ((unsigned int)( glAgentInfoLogFileLimitSize * 1024)))
    {
        if (gbFileTransferInProgress == FALSE)
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
            pthread_mutex_unlock(&gLogFile_mutex);

            NAM_SendLogFileToManager (NULL);// Pass Dummy Parameters
            return;
        }
    }
    pthread_mutex_unlock(&gLogFile_mutex);
}
/*****************************************************************************
 *
 * name             :   NAM_SendLogFileToManagerThreadFn
 * description      :   Creates thread to send log file to manager
 * input parameters :   unsigned int
 * output parameters:   void *
 * return type      :   void
 * global variables :   gbFileTransferInProgress
 * calls            :   
 */
void NAM_SendLogFileToManagerThreadFn (unsigned int guiReg, void *pclientarg)
{
    if (gbFileTransferInProgress == FALSE)
    {
        if ( pthread_create(&ghPThread,NULL,(void*) NAM_SendLogFileToManager,NULL) != 0 )
        {
    #ifdef _DEBUG
           printf("NAM_SendLogFileToManager()........Failed\n");
    #endif
        }
        else
        {
    #ifdef _DEBUG
           printf("NAM_SendLogFileToManager()........Success\n ");
    #endif
        }
    }
    return ;

}

/*****************************************************************************
 *
 * name             :   NAM_SendLogFileToManager
 * description      :   Sends Log file to Manager
 * input parameters :   void*
 * output parameters:   
 * return type      :   void
 * global variables :   (extern)glAgentInfoFileTrPort,gcAgentInfoFileTransferIP,
 *                      gcTempLogFile,gcInfoLogFileNetApp,gbFileTransferInProgress
 * calls            :   
 */
void NAM_SendLogFileToManager (void* pArg)
{
    struct sockaddr_in sock_addr;
    short error = 0 ;
    char caIP_STR[IP_STR_SIZE];

    pthread_detach(pthread_self());

    gbFileTransferInProgress = TRUE;

    DEBUGMSGTL(("nuri/nam", "NAM_SendLogFileToManager() Called ....\n"));
    DEBUGMSGTL(("nuri/nam", "glAgentInfoFileTrInterval : %u\n",glAgentInfoFileTrInterval));
    
   
    pthread_mutex_lock(&gLogFile_mutex);
    memset(caIP_STR,0,IP_STR_SIZE);
    getIPFromASNFormat(gcAgentInfoFileTransferIP, caIP_STR);
    if(gFpl != NULL) /* Condition to be cross-checked */
    {
        fclose(gFpl);
        gFpl = NULL;
    }
    else
    {
        pthread_mutex_unlock(&gLogFile_mutex);
        gbFileTransferInProgress = FALSE;
        pthread_exit(NULL);
    }

    if((glAgentInfoFileTrPort < 0 )||(strlen(caIP_STR) <= 1))
    {
        /* Error Case */
        pthread_mutex_unlock(&gLogFile_mutex);
        gbFileTransferInProgress = FALSE;
        DEBUGMSGTL(("nuri/nam", " Failure :glAgentInfoFileTrPort : %d, gcAgentInfoFileTransferIP: %s\n",glAgentInfoFileTrPort,caIP_STR));
        pthread_exit(NULL);
    }
    else
    {
        DEBUGMSGTL(("nuri/nam", " Success :glAgentInfoFileTrPort : %d, gcAgentInfoFileTransferIP: %s\n",glAgentInfoFileTrPort,caIP_STR));
    }
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons((unsigned short)glAgentInfoFileTrPort);
    sock_addr.sin_addr.s_addr = inet_addr(caIP_STR);    
    
    /*Copy from temp file to actual file */
    if(strlen(gcInfoLogFileNetApp) > 1)
    {
        DEBUGMSGTL(("nuri/nam", " Success :gcInfoLogFileNetApp: %sSAI***********",gcInfoLogFileNetApp));
        if (CopyFile(gcTempLogFile,gcInfoLogFileNetApp,FALSE) > 0)
        {
            DEBUGMSGTL(("nuri/nam", " Copy File Success\n"));
        }
        else
        {
            DEBUGMSGTL(("nuri/nam", " Copy File Failure\n"));
#ifdef WIN32
            DEBUGMSGTL(("nuri/nam", " GetLastError() %d\n",GetLastError()));
            DEBUGMSGTL(("nuri/nam", " len of gcInfoLogFileNetApp %d\n",strlen(gcInfoLogFileNetApp)));
#endif
        }
    }
    else
    {
        DEBUGMSGTL(("nuri/nam", " Failure :gcInfoLogFileNetApp: %s\n",gcInfoLogFileNetApp));
    }
    /* Unlock here so that NAM_WriteToLogFile() can take next logging*/
    /* Assumption here is file transfer will complete before next interval
       or file size limit reached */
    pthread_mutex_unlock(&gLogFile_mutex);
    if(strlen(gcInfoLogFileNetApp) > 1)
    {
        error = FileTrans_SendFile ( gcInfoLogFileNetApp , &sock_addr , NULL , 512);  
    }
    if ( error != NASC_SUCCESS)
    {
        DEBUGMSGTL(("nuri/nam", "FileTrans_SendFile() Failed ....\n"));
    } 
    else 
    {
        DEBUGMSGTL(("nuri/nam", "FileTrans_SendFile() Success ....\n"));
    }
    guiLogFileSendAlarmReg = snmp_alarm_register (glAgentInfoFileTrInterval,
                             (int) NULL,NAM_SendLogFileToManagerThreadFn, NULL);
    
    gbFileTransferInProgress = FALSE;
    pthread_exit(NULL);
}

#ifndef WIN32
/*Implementation for UNIX */
BOOL CopyFile(char * cpSrcFile,char * cpDesFile,BOOL bCreateFlag)
{
    FILE *fpSrcFile  = NULL;
    FILE *fpDestFile = NULL;
    struct _stat stStat;
    int i32Result;
    unsigned int ui32FileSize = 0;
    char *cpBuffer  = NULL;
    BOOL bResult    = FALSE;
    
    fpSrcFile  = fopen(cpSrcFile, "r");
    fpDestFile = fopen(cpDesFile, "w");

    if ((fpSrcFile != NULL) && (fpDestFile != NULL))
    {

       i32Result = _stat( cpSrcFile, &stStat );

       /* Check if statistics are valid: '0' - indicates success */
       if( i32Result == 0 )
       {
           DEBUGMSGTL(("nuri/nam", "Src: %s Dest: %s \n",cpSrcFile,cpDesFile));
           ui32FileSize = stStat.st_size ;
           cpBuffer = (char*) malloc(sizeof(char) * (ui32FileSize + 1 ));
           if(cpBuffer != NULL)
           {
               memset(cpBuffer,0,(sizeof(char) * (ui32FileSize + 1 )));
               fread(cpBuffer,ui32FileSize,1,fpSrcFile);
               fwrite(cpBuffer,ui32FileSize,1,fpDestFile);
               free(cpBuffer);
               bResult = TRUE;
           }
       }
    }
    else
    {
        /* Error case */
        bResult = FALSE;
    }

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

    return bResult;
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
/* <End: WIP_NURI_ALL > */

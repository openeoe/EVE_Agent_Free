/*****************************************************************************
 *
 * psMonitorTable.h
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Virendra Singh
 *
 * description :
 *      This file is a C header code for Process Monitoring
 *
 * TODO:
 *      None
 *
 * CM Details:
 *
 * $Author: ashok $
 * $Log: psMonitorTable.h,v $
 * Revision 1.30  2003/11/21 05:27:50  ashok
 * Added Windows Functionality.
 *
 * Revision 1.28  2003/11/20 10:30:23  viren
 * Updated with new MIB
 *
 * Revision 1.27  2003/11/20 10:24:12  viren
 * Updated with new MIB
 *
 * Revision 1.24  2003/11/06 07:10:22  sig
 * psMonitorTable.h has been modified for linux
 *
 * Revision 1.23  2003/11/06 05:43:07  viren
 * Fixed all known bugs
 *
 * Revision 1.22  2003/11/05 18:27:16  viren
 * Fixed all known bugs
 *
 * $State: Exp $
 * $Id: psMonitorTable.h,v 1.30 2003/11/21 05:27:50 ashok Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/


#ifndef PSMONITORTABLE_H
#define PSMONITORTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>
// <Start: WIP_NURI_ALL>
/********************************* includes *********************************/
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

// Common Includes
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32 // Windows Includes

#include <windows.h>
#include <io.h>
#include <pdh.h>
#include <winsock.h>
#include <time.h>

#else // Start of Unix Includes

#include <netdb.h>
#include <dirent.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>

#ifndef linux
#include <sys/proc.h>
#endif

#if defined(solaris2) // Start of Solaris includes

#include <signal.h>

#endif // End of Solaris includes

#if defined(AIX_4_3) // Start of AIX includes

#include <unistd.h>
#include <procinfo.h>
#include <sys/types.h>

#endif // End of AIX includes


#if defined(hpux10) || defined(hpux11) // Start of HP-UX includes

#include <stdio.h>
#include <sys/pstat.h>

#endif // End of HP-UX includes

#endif // End of Unix includes

#if HAVE_KVM_H
#include <kvm.h>
#endif

#if HAVE_SYS_PROC_H
#include <sys/proc.h>
#endif

#if _SLASH_PROC_METHOD_
#include <procfs.h>
#endif


// <End: WIP_NURI_ALL>

        /** Index psMonitorIndex is internal */

typedef struct psMonitorTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long psMonitorIndex;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char psMonitorName[255];
            long psMonitorName_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char psMonitorParameter[255];
            long psMonitorParameter_len;

        /** INTEGER = ASN_INTEGER */
            long psMonitorNumber;

        /** INTEGER = ASN_INTEGER */
            long psMonitorCheckInterval;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char psMonitorFilter[80];
            long psMonitorFilter_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char psMonitorDescription[255];
            long psMonitorDescription_len;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char psMonitorCreatedDateTime[20];
            long psMonitorCreatedDateTime_len;

        /** INTEGER = ASN_INTEGER */
            long psMonitorReaction;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char psMonitorReactionPath[255];
            long psMonitorReactionPath_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char psMonitorAlertMessage[255];
            long psMonitorAlertMessage_len;

        /** RowStatus = ASN_INTEGER */
            long psMonitorStatus;


    /*
     * OR
     *
     * Keep a pointer to your data
     */
    void * data;

    /*
     *add anything else you want here
     */
     // <Start: WIP_NURI_ALL: Added this extra member to keep OID value>

     unsigned long idx;

     // <End: WIP_NURI_ALL>

} psMonitorTable_context;

/*************************************************************
 * function declarations
 */
void init_psMonitorTable(void);
void initialize_table_psMonitorTable(void);
const psMonitorTable_context * psMonitorTable_get_by_idx(netsnmp_index *);
const psMonitorTable_context * psMonitorTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int psMonitorTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);
// <Start: WIP_NURI_ALL: Added definitions>

/********************* local definitions ********************/

#ifndef _WIN32
#define PROC_MON_DATA_FILE "/var/net-snmp/psMonitorTable.data"
#define PROC_MON_DATA_FILE_TMP "/var/net-snmp/psMonitorTable.tmp"
#else
#define PROC_MON_DATA_FILE "psMonitorTable.data"
#define PROC_MON_DATA_FILE_TMP "psMonitorTable.tmp"
#define MAXHOSTNAMELEN 64

// Change all calls to _call for Windows
#define write   _write
#define open    _open
#define lseek   _lseek
#define close   _close
#define read    _read
#endif
#define MAX_PROC_PTHREAD_NUM 2000

#ifndef MAX_MOD_CODE_LEN
#define MAX_MOD_CODE_LEN 5
#endif

#ifndef MAX_DATE_TIME_LEN
#define MAX_DATE_TIME_LEN 20
#endif

#ifndef MAX_HOST_IP_LEN
#define MAX_HOST_IP_LEN 16
#endif

#ifndef MAX_ALERT_MESG_LEN
#define MAX_ALERT_MESG_LEN 256
#endif

#ifdef WIN32
#define MAXHOSTNAMELEN 64
#endif

/********************* local structure/union definitions ********************/

typedef struct nuriAlert_s
{
    char    modCode[MAX_MOD_CODE_LEN];
    char    masterIp[MAX_HOST_IP_LEN];
    char    dateTime[MAX_DATE_TIME_LEN];
    char    sysName[MAXHOSTNAMELEN+1];
    char    alertMesg[MAX_ALERT_MESG_LEN];
} nuriAlert;


/********************* prototypes for local functions ***********************/

void ProcMon_LoadContainerFromFile();
void ProcMon_MasterThread();
void *ProcMon_ProcMonThreadMain(void*);
void ProcMon_GetLocalHostIp(char *);
void ProcMon_GetCurrDateTime(char *);
int ProcMon_SendAlert(char *,char *);
int ProcMon_SendMesg(nuriAlert );
int ProcMon_RestartProcess(char *);
int ProcMon_CheckProc(char *);

/************************* externs  ***********************/

extern char gcAgentInfoAlertMgrIp[];
extern char gcInfoLogFileProc[];
extern long glAgentInfoAlertMgrPort;

extern void getIPFromASNFormat(char *, char *);

// <End: WIP_NURI_ALL>


/*************************************************************
 * oid declarations
 */
extern oid psMonitorTable_oid[];
extern size_t psMonitorTable_oid_len;

#define psMonitorTable_TABLE_OID 1,3,6,1,4,1,3204,1,3,35,3

/*************************************************************
 * column number definitions for table psMonitorTable
 */
#define COLUMN_PSMONITORINDEX 1
#define COLUMN_PSMONITORNAME 2
#define COLUMN_PSMONITORPARAMETER 3
#define COLUMN_PSMONITORNUMBER 4
#define COLUMN_PSMONITORCHECKINTERVAL 5
#define COLUMN_PSMONITORFILTER 6
#define COLUMN_PSMONITORDESCRIPTION 7
#define COLUMN_PSMONITORCREATEDDATETIME 8
#define COLUMN_PSMONITORREACTION 9
#define COLUMN_PSMONITORREACTIONPATH 10
#define COLUMN_PSMONITORALERTMESSAGE 11
#define COLUMN_PSMONITORSTATUS 12
#define psMonitorTable_COL_MIN 2
#define psMonitorTable_COL_MAX 12

/* comment out the following line if you don't handle SET-REQUEST for psMonitorTable */
#define psMonitorTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define psMonitorTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define psMonitorTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define psMonitorTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef psMonitorTable_SET_HANDLING

int psMonitorTable_extract_index( psMonitorTable_context * ctx, netsnmp_index * hdr );

void psMonitorTable_set_reserve1( netsnmp_request_group * );
void psMonitorTable_set_reserve2( netsnmp_request_group * );
void psMonitorTable_set_action( netsnmp_request_group * );
void psMonitorTable_set_commit( netsnmp_request_group * );
void psMonitorTable_set_free( netsnmp_request_group * );
void psMonitorTable_set_undo( netsnmp_request_group * );

psMonitorTable_context * psMonitorTable_duplicate_row( psMonitorTable_context* );
netsnmp_index * psMonitorTable_delete_row( psMonitorTable_context* );

int psMonitorTable_can_activate(psMonitorTable_context *undo_ctx,
                      psMonitorTable_context *row_ctx,
                      netsnmp_request_group * rg);
int psMonitorTable_can_deactivate(psMonitorTable_context *undo_ctx,
                        psMonitorTable_context *row_ctx,
                        netsnmp_request_group * rg);
int psMonitorTable_can_delete(psMonitorTable_context *undo_ctx,
                    psMonitorTable_context *row_ctx,
                    netsnmp_request_group * rg);


#ifdef psMonitorTable_ROW_CREATION
psMonitorTable_context * psMonitorTable_create_row( netsnmp_index* );
#endif
#endif

#ifdef psMonitorTable_IDX2
psMonitorTable_context * psMonitorTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** PSMONITORTABLE_H */

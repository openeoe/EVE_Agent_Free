/*****************************************************************************
 *
 * fileMonitorTable.h
 *
 *      Copyright (c) 2009 Nuri Telecom Co., LTD 
 *      All rights Reserved
 *
 * author:
 *      SungHae Kim     	 
 *
 * description :
 *      This file is a C header code for File Monitoring
 *
 * TODO:
 *      None
 *
 * CM Details:
 *
 ****************************************************************************/

#ifndef FILEMONITORTABLE_H
#define FILEMONITORTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>
#include "psMonitorTable.h"

#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#ifdef WIN32 // Windows Includes

#include <windows.h>
#include <io.h>
#include <pdh.h>
#include <winsock.h>
#include <time.h>
#include <psapi.h>

#else // Start of Unix Includes

#include <pwd.h>
#include <netdb.h>
#include <dirent.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <regex.h>

#ifndef linux
#ifndef osf5
#include <sys/proc.h>
#endif
#endif

#if defined(solaris2) 
#include<kstat.h>
#include<sys/kstat.h>
#include<sys/sysinfo.h>
#include<sys/cpuvar.h>
#endif

#if defined(AIX_4_3) // Start of AIX includes

#include <unistd.h>
#include <sys/sysinfo.h>
#include <procinfo.h>

#endif // End of AIX includes

#if defined(hpux10) || defined(hpux11) // Start of HP-UX includes
#include <stdio.h>
#include <sys/sysinfo.h>
#ifndef _PSTAT64
#define _PSTAT64
#endif

#include <sys/pstat.h>

#endif // End of HP-UX includes

#endif // End of Unix includes

#if HAVE_KVM_H
#include <kvm.h>
#endif

#if HAVE_SYS_PROC_H
#ifndef osf5
#include <sys/proc.h>
#endif
#endif

#if _SLASH_PROC_METHOD_
#include <procfs.h>
#endif

#ifdef sysv4
#include <sys/procfs.h>
#include <sys/ksym.h>
#endif /* sysv4*/


#define FILE_ARG_LEN 255

#define MAX_OWNER_LEN 255

typedef struct _file_cache_info
{
    long time;
	long size;
    char name[FILE_ARG_LEN];

    struct _file_cache_info* next;
} file_cache_info;


        /** Index fileMonitorIndex is internal */

typedef struct _fileMonitorTable_context
{
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long fileMonitorIndex;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char fileMonitorName[255];
            long fileMonitorName_len;

        /** INTEGER = ASN_INTEGER */
            long fileMonitorCheckInterval;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char fileMonitorDescription[255];
            long fileMonitorDescription_len;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char fileMonitorCreatedDateTime[20];
            long fileMonitorCreatedDateTime_len;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char fileMonitorModifiedDateTime[20];
            long fileMonitorModifiedDateTime_len;

        /** INTEGER = ASN_INTEGER */
            long fileMonitorFileSize;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char fileMonitorOwner[255];
            long fileMonitorOwner_len;

        /** INTEGER = ASN_INTEGER */
            long fileMonitorReaction;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char fileMonitorReactionPath[255];
            long fileMonitorReactionPath_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char fileMonitorAlertMessage[255];
            long fileMonitorAlertMessage_len;

        /** RowStatus = ASN_INTEGER */
            long fileMonitorStatus;

    /*
     * OR
     *
     * Keep a pointer to your data
     */

    void* data;

    /*
     * add anything else you want here
     */

     unsigned long idx;

     int threadActive; // 0 - False, 1 - True

} fileMonitorTable_context;

/*************************************************************
 * function declarations
 */
void init_fileMonitorTable(void);
void initialize_table_fileMonitorTable(void);
const fileMonitorTable_context* fileMonitorTable_get_by_idx(netsnmp_index *);
const fileMonitorTable_context* fileMonitorTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int fileMonitorTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);


/********************* local definitions ********************/

#define FILE_MON_CONTEXT_SIZE		sizeof(fileMonitorTable_context)

#ifdef WIN32

#define FILE_MON_DATA_FILE "fileMonitorTable.data"
#define FILE_MON_DATA_FILE_TMP "fileMonitorTable.tmp"

#else

#define FILE_MON_DATA_FILE "/var/net-snmp/fileMonitorTable.data"
#define FILE_MON_DATA_FILE_TMP "/var/net-snmp/fileMonitorTable.tmp"

#endif

/********************* local structure/union definitions ********************/


/********************* prototypes for local functions ***********************/

void FileMon_LoadContainerFromFile();
void FileMon_MasterThread();
void* FileStatusCheckThread(void*);
int FileMon_UpdateDataFile(fileMonitorTable_context *);

stVarBindArgProc* makeFileMonitorVarBind_Modified(fileMonitorTable_context *);

/************************* externs  ***********************/

extern char gcAgentInfoAlertMgrIp[];
extern char gcInfoLogFileProc[];
extern char gcAgentInfoFileTransferIP[];
extern long glAgentInfoAlertMgrPort;
extern long glAgentInfoFileTrInterval ;
extern long glAgentInfoFileTrPort;
extern long glAgentInfoLogFileLimitSize;
extern long glAgentInfoSerialNo;

/*************************************************************
 * oid declarations
 */
extern oid fileMonitorTable_oid[];
extern size_t fileMonitorTable_oid_len;

#define fileMonitorTable_TABLE_OID 1,3,6,1,4,1,3204,1,3,35,7

/*************************************************************
 * column number definitions for table fileMonitorTable
 */
#define COLUMN_FILEMONITORINDEX				1
#define COLUMN_FILEMONITORNAME				2
#define COLUMN_FILEMONITORCHECKINTERVAL		3
#define COLUMN_FILEMONITORDESCRIPTION		4
#define COLUMN_FILEMONITORCREATEDDATETIME	5
#define COLUMN_FILEMONITORMODIFIEDDATETIME	6
#define COLUMN_FILEMONITORFILESIZE	        7
#define COLUMN_FILEMONITOROWNER	            8
#define COLUMN_FILEMONITORREACTION			9
#define COLUMN_FILEMONITORREACTIONPATH		10	
#define COLUMN_FILEMONITORALERTMESSAGE		11	
#define COLUMN_FILEMONITORSTATUS			12	

#define fileMonitorTable_COL_MIN			2
#define fileMonitorTable_COL_MAX			12	

/* comment out the following line if you don't handle SET-REQUEST for fileMonitorTable */
#define fileMonitorTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define fileMonitorTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define fileMonitorTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define fileMonitorTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef fileMonitorTable_SET_HANDLING

int fileMonitorTable_extract_index( 
		fileMonitorTable_context* ctx, 
		netsnmp_index* hdr);

void fileMonitorTable_set_reserve1( netsnmp_request_group * );
void fileMonitorTable_set_reserve2( netsnmp_request_group * );
void fileMonitorTable_set_action( netsnmp_request_group * );
void fileMonitorTable_set_commit( netsnmp_request_group * );
void fileMonitorTable_set_free( netsnmp_request_group * );
void fileMonitorTable_set_undo( netsnmp_request_group * );

fileMonitorTable_context* fileMonitorTable_duplicate_row(fileMonitorTable_context* );
netsnmp_index* fileMonitorTable_delete_row(fileMonitorTable_context* );

int fileMonitorTable_can_activate(
						fileMonitorTable_context* undo_ctx,
						fileMonitorTable_context* row_ctx,
						netsnmp_request_group* rg);
int fileMonitorTable_can_deactivate(
						fileMonitorTable_context* undo_ctx,
						fileMonitorTable_context* row_ctx,
						netsnmp_request_group* rg);
int fileMonitorTable_can_delete(
						fileMonitorTable_context* undo_ctx,
						fileMonitorTable_context* row_ctx,
						netsnmp_request_group* rg);

#ifdef fileMonitorTable_ROW_CREATION
fileMonitorTable_context* fileMonitorTable_create_row(netsnmp_index* );
#endif

#endif		/* fileMonitorTable_SET_HANDLING */

#ifdef fileMonitorTable_IDX2
fileMonitorTable_context* fileMonitorTable_get(const char *name, int len);
#endif


#ifdef __cplusplus
};
#endif

#endif /** FILEMONITORTABLE_H */


/*****************************************************************************
 *
 * logCollectorTable.h
 *
 *      Copyright (c) 2009 Nuri Telecom Co., LTD 
 *      All rights Reserved
 *
 * author:
 *      SungHae Kim     	 
 *
 * description :
 *      This file is a C header code for Log Collector
 *
 * TODO:
 *      None
 *
 * CM Details:
 *
 ****************************************************************************/

#ifndef LOGCOLLECTORTABLE_H
#define LOGCOLLECTORTABLE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

/********************************* includes *********************************/

// Common includes
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "psMonitorTable.h"

// Windows includes
#ifdef WIN32

#include <windows.h>
#include <io.h>
#include <pdh.h>
#include <winsock.h>
#include <time.h>
#define MAXPATHLEN 1024

#else

// Start of Unix includes
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <regex.h>

#include <utmp.h>
#include <utmpx.h>

#ifdef hpux11
#include <utmps.h>
#endif

#endif

#ifdef linux
#include <asm/param.h>
#endif

#ifdef AIX_4_3
#include <sys/param.h>
#endif

#ifndef WIN32
#if !defined(MAXPATHLEN)
#include <sys/param.h>
#endif
#endif
        /** Index logCollectorIndex is internal */

typedef struct logCollectorTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long logCollectorIndex;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logCollectorFile[255];
            long logCollectorFile_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logCollectorFileType[20];
            long logCollectorFileType_len;

        /** INTEGER = ASN_INTEGER */
            long logCollectorReadInterval;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logCollectorFilter[255];
            long logCollectorFilter_len;

        /** INTEGER = ASN_INTEGER */
            long logCollectorFilterCount;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logCollectorDescription[255];
            long logCollectorDescription_len;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char logCollectorCreatedTime[20];
            long logCollectorCreatedTime_len;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char logCollectorUpdatedTime[20];
            long logCollectorUpdatedTime_len;

        /** INTEGER = ASN_INTEGER */
            long logCollectorLine;

        /** INTEGER = ASN_INTEGER */
            long logCollectorGetLine;

        /** INTEGER = ASN_INTEGER */
            long logCollectorFileSize;

        /** INTEGER = ASN_INTEGER */
            long logCollectorReadLine;

        /** RowStatus = ASN_INTEGER */
            long logCollectorStatus;

    /*
     * OR
     *
     * Keep a pointer to your data
     */
    void * data;

    /*
     *add anything else you want here
     */

     unsigned long idx;

     int threadActiveFlag; // 0 : False, 1 : True

     char oldFileName[MAXPATHLEN+1];
     char changedFileName[MAXPATHLEN+1];
     long logCollectorReadLineSeek;
	 int isStart;

     long filterCount;

} logCollectorTable_context;

/*************************************************************
 * function declarations
 */

void init_logCollectorTable(void);
void initialize_table_logCollectorTable(void);
const logCollectorTable_context * logCollectorTable_get_by_idx(netsnmp_index *);
const logCollectorTable_context * logCollectorTable_get_by_idx_rs(netsnmp_index *, int row_status);
int logCollectorTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);


#define LOG_COL_CONTEXT_SIZE 	sizeof(logCollectorTable_context)

#ifndef WIN32  // For Unix

#define LOG_COL_DATA_FILE "/var/net-snmp/logCollectorTable.data"
#define LOG_COL_DATA_FILE_TMP "/var/net-snmp/logCollectorTable.tmp"

#else // For Windows

#define LOG_COL_DATA_FILE "logCollectorTable.data"
#define LOG_COL_DATA_FILE_TMP "logCollectorTable.tmp"
#define MAXHOSTNAMELEN 64  // Not following coding std since it is system
                          // defined macro for unix
#define MAX_NAM_LEN 50

#endif


#define LOG_MAX_LEN 2048

/************************* externs  ***********************/

extern char gcInfoLogFileProc[];
extern char gcAgentInfoAlertMgrIp[];
extern long glAgentInfoAlertMgrPort;
extern long glAgentInfoFileTrInterval ;
extern long glAgentInfoFileTrPort;
extern char gcAgentInfoFileTransferIP[];

/********************* prototypes for local functions ***********************/

void LogCol_LoadContainerFromFile();
void *LogCol_MasterThread();
void *LogCollectorThread(void *);
int LogCol_GetLineCount(char *);
int LogCol_UpdateDataFile(logCollectorTable_context *);

stVarBindArgProc* makeLogColVarBind(logCollectorTable_context*, char*, int);

/*************************************************************
 * oid declarations
 */
extern oid logCollectorTable_oid[];
extern size_t logCollectorTable_oid_len;

#define logCollectorTable_TABLE_OID 1,3,6,1,4,1,3204,1,3,35,9

/*************************************************************
 * column number definitions for table logCollectorTable
 */
#define COLUMN_LOGCOLLECTORINDEX			1
#define COLUMN_LOGCOLLECTORFILE				2
#define COLUMN_LOGCOLLECTORFILETYPE			3
#define COLUMN_LOGCOLLECTORREADINTERVAL		4
#define COLUMN_LOGCOLLECTORFILTER			5
#define COLUMN_LOGCOLLECTORFILTERCOUNT		6	
#define COLUMN_LOGCOLLECTORDESCRIPTION		7
#define COLUMN_LOGCOLLECTORCREATEDTIME		8
#define COLUMN_LOGCOLLECTORUPDATEDTIME		9
#define COLUMN_LOGCOLLECTORLINE				10	
#define COLUMN_LOGCOLLECTORGETLINE			11	
#define COLUMN_LOGCOLLECTORFILESIZE			12	
#define COLUMN_LOGCOLLECTORREADLINE			13
#define COLUMN_LOGCOLLECTORSTATUS			14	

#define logCollectorTable_COL_MIN 2
#define logCollectorTable_COL_MAX 14

/* comment out the following line if you don't handle SET-REQUEST for logCollectorTable */
#define logCollectorTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define logCollectorTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define logCollectorTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define logCollectorTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef logCollectorTable_SET_HANDLING

int logCollectorTable_extract_index( logCollectorTable_context * ctx, netsnmp_index * hdr );

void logCollectorTable_set_reserve1( netsnmp_request_group * );
void logCollectorTable_set_reserve2( netsnmp_request_group * );
void logCollectorTable_set_action( netsnmp_request_group * );
void logCollectorTable_set_commit( netsnmp_request_group * );
void logCollectorTable_set_free( netsnmp_request_group * );
void logCollectorTable_set_undo( netsnmp_request_group * );

logCollectorTable_context * logCollectorTable_duplicate_row( logCollectorTable_context* );
netsnmp_index * logCollectorTable_delete_row( logCollectorTable_context* );

int logCollectorTable_can_activate(logCollectorTable_context *undo_ctx,
                      logCollectorTable_context *row_ctx,
                      netsnmp_request_group * rg);
int logCollectorTable_can_deactivate(logCollectorTable_context *undo_ctx,
                        logCollectorTable_context *row_ctx,
                        netsnmp_request_group * rg);
int logCollectorTable_can_delete(logCollectorTable_context *undo_ctx,
                    logCollectorTable_context *row_ctx,
                    netsnmp_request_group * rg);

#ifdef logCollectorTable_ROW_CREATION
logCollectorTable_context * logCollectorTable_create_row( netsnmp_index* );
#endif

#endif

#ifdef logCollectorTable_IDX2
logCollectorTable_context * logCollectorTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** LOGCOLLECTORTABLE_H */

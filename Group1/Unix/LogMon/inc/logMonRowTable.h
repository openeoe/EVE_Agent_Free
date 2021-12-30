/*****************************************************************************
 *
 * logMonRowtable.h
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Virendra Singh
 *
 * description :
 *      This file is a C Header file for Log Monitoring - Row
 *
 * TODO:
 *      None
 *
 * CM Details:
 *
 * $Author: buvanes $
 * $Log: logMonRowTable.h,v $
 * Revision 1.17  2003/11/21 09:32:03  buvanes
 * included param.h file for AIX
 *
 * Revision 1.16  2003/11/21 05:10:59  viren
 * Updated with Windows changes
 *
 * Revision 1.14  2003/11/20 16:38:28  viren
 * Hopefully stable
 *
 * Revision 1.11  2003/11/07 09:54:08  viren
 * Updated with the latest MIB
 *
 * Revision 1.10  2003/11/06 13:15:26  viren
 * Fixed all known bugs
 *
 * Revision 1.9  2003/11/06 07:00:08  sig
 * logMonRowTable.h has been modified for linux
 *
 * Revision 1.8  2003/11/04 17:09:35  viren
 * Log Mon Row Define was modified
 *
 * Revision 1.7  2003/11/04 13:11:44  viren
 * After beautification
 *
 * $State: Exp $
 * $Id: logMonRowTable.h,v 1.17 2003/11/21 09:32:03 buvanes Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/

#ifndef LOGMONROWTABLE_H
#define LOGMONROWTABLE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

// <Start: WIP_NURI_SOL: Header files to be included>

/********************************* includes *********************************/

// Common includes
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

// Windows includes
#ifdef WIN32
#include <windows.h>
#include <io.h>
#include <pdh.h>
#include <winsock.h>
#include <time.h>
#else 

// Start of Unix includes
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#endif

// <End: WIP_NURI_SOL>

// <Start: WIP_NURI_LIN>
#ifdef linux
#include <asm/param.h>
#endif

#ifdef AIX_4_3
#include <sys/param.h>
#endif

// <End: WIP_NURI_LIN>


        /** Index logMonRowIndex is internal */

typedef struct logMonRowTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long logMonRowIndex;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logMonRowLogFile[255];
            long logMonRowLogFile_len;

        /** INTEGER = ASN_INTEGER */
            long logMonRowReadInterval;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logMonRowFilter[255];
            long logMonRowFilter_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logMonRowDescription[255];
            long logMonRowDescription_len;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char logMonRowCreatedTime[20];
            long logMonRowCreatedTime_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logMonRowAlertMessage[255];
            long logMonRowAlertMessage_len;

        /** RowStatus = ASN_INTEGER */
            long logMonRowStatus;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char logMonRowUpdatedTime[20];
            long logMonRowUpdatedTime_len;

        /** INTEGER = ASN_INTEGER */
            long logMonRowLine;

        /** INTEGER = ASN_INTEGER */
            long logMonRowGetLine;

        /** INTEGER = ASN_INTEGER */
            long logMonRowFileSize;

        /** INTEGER = ASN_INTEGER */
            long logMonRowReadLine;


    /*
     * OR
     *
     * Keep a pointer to your data
     */
    void * data;

    /*
     *add anything else you want here
     */

     // <Start: WIP_NURI_SOL: Added extra member to keep the OID value>

     unsigned long idx;

     // <End: WIP_NURI_SOL>

} logMonRowTable_context;

/*************************************************************
 * function declarations
 */

void init_logMonRowTable(void);
void initialize_table_logMonRowTable(void);
const logMonRowTable_context * logMonRowTable_get_by_idx(netsnmp_index *);
const logMonRowTable_context * logMonRowTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int logMonRowTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);

// <Start: WIP_NURI_ALL: Definitions, structs>

//#define LOG_MON_ROW_DATA_FILE "/var/net-snmp/logMonRowDataFile.data"
//#define LOG_MON_ROW_DATA_FILE_TMP "/var/net-snmp/logMonRowDataFile.tmp"
#ifndef _WIN32
#define LOG_MON_ROW_DATA_FILE "/var/net-snmp/logMonRowDataFile.data"
#define LOG_MON_ROW_DATA_FILE_TMP "/var/net-snmp/logMonRowDataFile.tmp"
#else
#define LOG_MON_ROW_DATA_FILE "logMonRowDataFile.data"
#define LOG_MON_ROW_DATA_FILE_TMP "logMonRowDataFile.tmp"
#define MAXHOSTNAMELEN 64

// Change all calls to _call for Windows
#define write   _write
#define open    _open
#define lseek   _lseek
#define close   _close
#define read    _read
#endif

#define MAX_LOG_MON_ROWS 5000
#define LINE_MAX_LEN 500
#define MAX_LOG_PTHREAD_NUM 2000
#define MAX_MOD_CODE_LEN 5
#define MAX_IP_ADDR_LEN 16
#define MAX_DATE_TIME_LEN 16
#define MAX_ALERT_MESG_LEN 255

/********************* local structure/union definitions ********************/

typedef struct nuriAlertLGRow_s
{
    char    modCode[MAX_MOD_CODE_LEN];
    char    masterIp[MAX_IP_ADDR_LEN];
    char    dateTime[MAX_DATE_TIME_LEN];
    char    sysName[MAXHOSTNAMELEN+1];
    char    alertMesg[MAX_ALERT_MESG_LEN];
} nuriAlertLGRow;

/************************* externs  ***********************/

extern char gcInfoLogFileMonRow[];
extern char gcAgentInfoAlertMgrIp[];
extern long glAgentInfoAlertMgrPort;

extern void getIPFromASNFormat(char *, char *);

/********************* prototypes for local functions ***********************/

void *LogMonRow_MasterThread();
void *LogMonRow_LogMonThreadMain(void *);
void LogMonRow_LoadContainerFromFile();
void LogMonRow_GetLocalHostIp(char *);
void LogMonRow_GetCurrLogTime(char *);
int LogMonRow_SendAlert(char *,char *);
int LogMonRow_GetLineCount(char *);
int LogMonRow_SendLogMesg(nuriAlertLGRow);
int LogMonCol_UpdateDataFile(logMonRowTable_context *);

// <End: WIP_NURI_ALL>

/*************************************************************
 * oid declarations
 */
extern oid logMonRowTable_oid[];
extern size_t logMonRowTable_oid_len;

#define logMonRowTable_TABLE_OID 1,3,6,1,4,1,3204,1,3,35,2,2

/*************************************************************
 * column number definitions for table logMonRowTable
 */
#define COLUMN_LOGMONROWINDEX 1
#define COLUMN_LOGMONROWLOGFILE 2
#define COLUMN_LOGMONROWREADINTERVAL 3
#define COLUMN_LOGMONROWFILTER 4
#define COLUMN_LOGMONROWDESCRIPTION 5
#define COLUMN_LOGMONROWCREATEDTIME 6
#define COLUMN_LOGMONROWALERTMESSAGE 7
#define COLUMN_LOGMONROWSTATUS 8
#define COLUMN_LOGMONROWUPDATEDTIME 9
#define COLUMN_LOGMONROWLINE 10
#define COLUMN_LOGMONROWGETLINE 11
#define COLUMN_LOGMONROWFILESIZE 12
#define COLUMN_LOGMONROWREADLINE 13
#define logMonRowTable_COL_MIN 2
#define logMonRowTable_COL_MAX 13

/* comment out the following line if you don't handle SET-REQUEST for logMonRowTable */
#define logMonRowTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define logMonRowTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define logMonRowTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define logMonRowTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef logMonRowTable_SET_HANDLING

int logMonRowTable_extract_index( logMonRowTable_context * ctx, netsnmp_index * hdr );

void logMonRowTable_set_reserve1( netsnmp_request_group * );
void logMonRowTable_set_reserve2( netsnmp_request_group * );
void logMonRowTable_set_action( netsnmp_request_group * );
void logMonRowTable_set_commit( netsnmp_request_group * );
void logMonRowTable_set_free( netsnmp_request_group * );
void logMonRowTable_set_undo( netsnmp_request_group * );

logMonRowTable_context * logMonRowTable_duplicate_row( logMonRowTable_context* );
netsnmp_index * logMonRowTable_delete_row( logMonRowTable_context* );

int logMonRowTable_can_activate(logMonRowTable_context *undo_ctx,
                      logMonRowTable_context *row_ctx,
                      netsnmp_request_group * rg);
int logMonRowTable_can_deactivate(logMonRowTable_context *undo_ctx,
                        logMonRowTable_context *row_ctx,
                        netsnmp_request_group * rg);
int logMonRowTable_can_delete(logMonRowTable_context *undo_ctx,
                    logMonRowTable_context *row_ctx,
                    netsnmp_request_group * rg);


#ifdef logMonRowTable_ROW_CREATION
logMonRowTable_context * logMonRowTable_create_row( netsnmp_index* );
#endif
#endif

#ifdef logMonRowTable_IDX2
logMonRowTable_context * logMonRowTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** LOGMONROWTABLE_H */

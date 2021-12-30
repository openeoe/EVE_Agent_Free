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
 * Revision 1.19  2004/01/22 09:00:54  buvanes
 * MAXPATHLEN defined
 *
 * Revision 1.18  2004/01/20 08:10:47  buvanes
 * Old file name field added to MIB structure
 *
 * Revision 1.16  2003/12/23 04:03:52  sig
 * *** empty log message ***
 *
 * Revision 1.15  2003/12/13 19:05:13  viren
 * Wrong OID in trap mesg - fixed
 *
 * Revision 1.11  2003/12/11 07:39:26  viren
 * Added few more debug information
 *
 * Revision 1.9  2003/12/10 08:27:16  viren
 * static added for global variable
 *
 * Revision 1.8  2003/12/09 19:48:36  viren
 * file close problems fixed
 *
 * Revision 1.7  2003/12/09 18:09:39  viren
 * File transfer implemented - Working fine
 *
 * Revision 1.6  2003/12/08 18:21:32  viren
 * Updated review comments
 *
 * Revision 1.5  2003/12/02 10:56:40  vijaysp
 * Trap msg. format changed. TrapOID value changed from Nuri OID to table oid and arg. changed to alert message string alone
 *
 * Revision 1.4  2003/12/02 04:14:36  vijaysp
 * std. macro MAX_DATE_TIME_LEN changed to MAX_DATETIME_LEN
 *
 * Revision 1.3  2003/12/01 10:33:17  vijaysp
 * Trap is sent to AlertMsg instead of UDP alert message
 *
 * Revision 1.2  2003/11/28 05:44:44  ashok
 * Minor Changes
 *
 * Revision 1.1  2003/11/24 06:42:11  viren
 * moved from old directories
 *
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
 * $Id: logMonRowTable.h,v 1.19 2004/01/22 09:00:54 buvanes Exp $
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

#ifndef WIN32
#if !defined(MAXPATHLEN)
#include <sys/param.h>
#endif
#endif
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

		/** added by D.J Park in 2005.02.22 [start]*/

        /** INTEGER = ASN_INTEGER */
            long logMonRowFileSizeThreshold;

        /** INTEGER = ASN_INTEGER */
            long logMonRowFilterCounter;

        /** INTEGER = ASN_INTEGER */
            long logMonRowTrapType;

		/** added by D.J Park in 2005.02.22 [end]*/


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

     int iThreadActiveFlag; // 0 - False, 1 - True

     char acOldFileName[MAXPATHLEN+1];

	 /** added by D.J Park in 2004.03.30 [start]*/
     char acChangedFileName[MAXPATHLEN+1];
     long logMonRowReadLineSeek;
	 int isStart;
	 /** added by D.J Park in 2004.03.30 [end]*/

	 /** added by D.J Park in 2004.02.22 [start]*/
     long filterCounter;
	 /** added by D.J Park in 2004.02.22 [end]*/

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

#ifndef WIN32  // For Unix
#define LOG_MON_ROW_DATA_FILE "/var/net-snmp/logMonRowDataFile.data"
#define LOG_MON_ROW_DATA_FILE_TMP "/var/net-snmp/logMonRowDataFile.tmp"

#else // For Windows

#define LOG_MON_ROW_DATA_FILE "logMonRowDataFile.data"
#define LOG_MON_ROW_DATA_FILE_TMP "logMonRowDataFile.tmp"
#define MAXHOSTNAMELEN 64  // Not following coding std since it is system
                          // defined macro for unix
#define MAX_NAM_LEN 50

#endif

#ifndef MY_THR_ID
#define MY_THR_ID   pthread_self()
#endif

#ifdef WIN32 // For windows
#ifndef SLEEP
#define SLEEP(x) Sleep(x * 1000)  //x in seconds
#endif
#else // For Unix
#ifndef SLEEP
#define SLEEP(x) sleep(x)         //x in seconds
#endif
#endif

#ifndef TIME_TO_SLEEP
#define TIME_TO_SLEEP 3
#endif

#define LINE_MAX_LEN 500
#define MAX_LOG_PTHREAD_NUM 2000
#define MAX_MODCODE_LEN 5
#define MAX_IP_ADDR_LEN 16

#ifndef MAX_DATE_TIME_LEN
#define MAX_DATE_TIME_LEN 20
#endif

#define MAX_ALERTMESG_LEN 255

/************************* externs  ***********************/

extern char gcInfoLogFileMonRow[];
extern char gcAgentInfoAlertMgrIp[];
extern long glAgentInfoAlertMgrPort;
extern long glAgentInfoFileTrInterval ;
extern long glAgentInfoFileTrPort;
extern char gcAgentInfoFileTransferIP[];

extern void getIPFromASNFormat(char *, char *);
extern int Common_SendAlert(char *, char *, char *, unsigned long);
extern void Common_SendFileThread(unsigned int , void *);
extern int Common_CheckFileSize(char *);

extern long glTransInfoMaxLogFileSize;

// Global variables
static unsigned int    guiLogFileLogRowAlarmReg=0;

/********************* prototypes for local functions ***********************/

void *LogMonRow_MasterThread();
void *LogMonRow_LogMonThreadMain(void *);
void LogMonRow_LoadContainerFromFile();
int LogMonRow_GetLineCount(char *);
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
/** added by D.J Park in 2005.02.22 [start] */
#define COLUMN_LOGMONROWFILESIZETHRESHOLD 14
#define COLUMN_LOGMONROWFILTERCOUNTER 15
#define COLUMN_LOGMONROWTRAPTYPE 16
/** added by D.J Park in 2005.02.22 [end] */
#define logMonRowTable_COL_MIN 2
/** modified by D.J Park in 2005.02.22 [start] */
/*
#define logMonRowTable_COL_MAX 13
*/
#define logMonRowTable_COL_MAX 16
/** modified by D.J Park in 2005.02.22 [end] */

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

/** rewrite by D.J Park in 2004.01.30
 */
extern int Common_SendAlert_And_Vars(char *, char *, char *, unsigned long,stVarBindArgProc *,int);
stVarBindArgProc *makeLogMonRowVarBind(logMonRowTable_context *, char *,int );
size_t strtimeformat(char  * s, size_t maxsize, const char  * format);
int LogMonRow_GetLineCountAndSeek(char *pcFileName,long *readLineSeek);
/** added by D.J Park in 2005.02.22 [start]*/
stVarBindArgProc *makeThresholdLogMonRowVarBind(logMonRowTable_context *, int );
/** added by D.J Park in 2005.02.22 [end]*/

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

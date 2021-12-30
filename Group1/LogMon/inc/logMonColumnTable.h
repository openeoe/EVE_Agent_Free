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
 *      This file is a C Header file for Log Monitoring - Column
 *
 * TODO:
 *      None
 *
 * CM Details:
 *
 * $Author: buvanes $
 * $Log: logMonColumnTable.h,v $
 * Revision 1.20  2004/01/22 09:00:59  buvanes
 * MAXPATHLEN defined
 *
 * Revision 1.19  2004/01/20 08:10:28  buvanes
 * Old file name field added to MIB structure
 *
 * Revision 1.18  2003/12/23 04:03:14  sig
 * *** empty log message ***
 *
 * Revision 1.17  2003/12/13 19:05:13  viren
 * Wrong OID in trap mesg - fixed
 *
 * Revision 1.13  2003/12/11 07:39:20  viren
 * Added few more debug information
 *
 * Revision 1.12  2003/12/10 08:27:36  viren
 * static added for global variable
 *
 * Revision 1.11  2003/12/09 19:48:16  viren
 * file close problems fixed
 *
 * Revision 1.10  2003/12/08 18:21:25  viren
 * Updated review comments
 *
 * Revision 1.9  2003/12/02 10:56:40  vijaysp
 * Trap msg. format changed. TrapOID value changed from Nuri OID to table oid and arg. changed to alert message string alone
 *
 * Revision 1.8  2003/12/02 04:19:08  vijaysp
 * Trap is send iso Alert Msg
 *
 * Revision 1.7  2003/11/28 12:01:32  viren
 * wrong data file path was given
 *
 * Revision 1.6  2003/11/28 06:45:17  ashok
 * Minor Changes
 *
 * Revision 1.5  2003/11/28 05:57:01  ashok
 * Minor Changes
 *
 * Revision 1.4  2003/11/27 12:41:59  buvanes
 * included param.h for AIX
 *
 * Revision 1.3  2003/11/26 10:38:52  viren
 * Wrong data file path was given - fixed
 *
 * Revision 1.1  2003/11/24 06:42:11  viren
 * moved from old directories
 *
 * Revision 1.3  2003/11/20 16:38:19  viren
 * Hopefully stable
 *
 * $State: Exp $
 * $Id: logMonColumnTable.h,v 1.20 2004/01/22 09:00:59 buvanes Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/
#ifndef LOGMONCOLUMNTABLE_H
#define LOGMONCOLUMNTABLE_H

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
#define MAXHOSTNAMELEN 64
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

// <End: WIP_NURI_LIN>
/* Start: WIP_NURI_AIX: */
#ifdef AIX_4_3
#include <sys/param.h>
#endif
/* End: WIP_NURI_AIX */

#ifndef WIN32
#if !defined(MAXPATHLEN)
#include <sys/param.h>
#endif
#endif

        /** Index logMonColumnIndex is internal */

typedef struct logMonColumnTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long logMonColumnIndex;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logMonColumnLogFile[255];
            long logMonColumnLogFile_len;

        /** INTEGER = ASN_INTEGER */
            long logMonColumnReadInterval;

        /** INTEGER = ASN_INTEGER */
            long logMonColumnCol1;

        /** INTEGER = ASN_INTEGER */
            long logMonColumnCol2;

        /** INTEGER = ASN_INTEGER */
            long logMonColumnCol3;

        /** INTEGER = ASN_INTEGER */
            long logMonColumnCol4;

        /** INTEGER = ASN_INTEGER */
            long logMonColumnCol5;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logMonColumnColFilter1[80];
            long logMonColumnColFilter1_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logMonColumnColFilter2[80];
            long logMonColumnColFilter2_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logMonColumnColFilter3[80];
            long logMonColumnColFilter3_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logMonColumnColFilter4[80];
            long logMonColumnColFilter4_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logMonColumnColFilter5[80];
            long logMonColumnColFilter5_len;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char logMonColumnCreatedTime[20];
            long logMonColumnCreatedTime_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logMonColumnDescription[255];
            long logMonColumnDescription_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char logMonColumnAlertMessage[255];
            long logMonColumnAlertMessage_len;

        /** RowStatus = ASN_INTEGER */
            long logMonColumnStatus;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char logMonColumnUpdatedTime[20];
            long logMonColumnUpdatedTime_len;

        /** INTEGER = ASN_INTEGER */
            long logMonColumnFileSize;

        /** INTEGER = ASN_INTEGER */
            long logMonColumnReadLine;


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
     // <End: WIP_NURI_SOL>

} logMonColumnTable_context;

/*************************************************************
 * function declarations
 */
void init_logMonColumnTable(void);
void initialize_table_logMonColumnTable(void);
const logMonColumnTable_context * logMonColumnTable_get_by_idx(netsnmp_index *);
const logMonColumnTable_context * logMonColumnTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int logMonColumnTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);

// <Start: WIP_NURI_ALL: Definitions, structs>

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


#ifndef WIN32
#define LOG_MON_COL_DATA_FILE "/var/net-snmp/logMonColDataFile.data"
#define LOG_MON_COL_DATA_FILE_TMP "/var/net-snmp/logMonColDataFile.tmp"
#else
#define LOG_MON_COL_DATA_FILE "logMonColDataFile.data"
#define LOG_MON_COL_DATA_FILE_TMP "logMonColDataFile.tmp"
#define MAXHOSTNAMELEN 64

#endif

#define LINE_MAX_LEN 500
#define MAX_FORMAT_LEN 100

#ifndef MAX_LOG_PTHREAD_NUM
#define MAX_LOG_PTHREAD_NUM 2000
#endif

#ifndef MAX_MODCODE_LEN
#define MAX_MODCODE_LEN 5
#endif

#ifndef MAX_IP_ADDR_LEN
#define MAX_IP_ADDR_LEN 16
#endif

#ifndef MAX_DATE_TIME_LEN
#define MAX_DATE_TIME_LEN 20
#endif

#ifndef MAX_ALERTMESG_LEN
#define MAX_ALERTMESG_LEN 255
#endif

/************************* externs  ***********************/


extern char gcInfoLogFileMonCol[];
extern char gcAgentInfoAlertMgrIp[];
extern long glAgentInfoAlertMgrPort;
extern long glAgentInfoFileTrInterval ;
extern long glAgentInfoFileTrPort;
extern char gcAgentInfoFileTransferIP[];

extern void getIPFromASNFormat(char *, char *);
extern int Common_SendAlert(char *, char *, char *, unsigned long);
extern void Common_SendFileThread(unsigned int , void *);
extern int Common_CheckFileSize(char *);

// Global variables
static unsigned int    guiLogFileColRowAlarmReg=0;

/********************* prototypes for local functions ***********************/

void *LogMonCol_MasterThread();
void *LogMonCol_LogMonThreadMain(void *);
void LogMonCol_LoadContainerFromFile();
int LogMonCol_SearchPatterns(logMonColumnTable_context *);

// <End: WIP_NURI_ALL>


/*************************************************************
 * oid declarations
 */
extern oid logMonColumnTable_oid[];
extern size_t logMonColumnTable_oid_len;

#define logMonColumnTable_TABLE_OID 1,3,6,1,4,1,3204,1,3,35,2,1

/*************************************************************
 * column number definitions for table logMonColumnTable
 */
#define COLUMN_LOGMONCOLUMNINDEX 1
#define COLUMN_LOGMONCOLUMNLOGFILE 2
#define COLUMN_LOGMONCOLUMNREADINTERVAL 3
#define COLUMN_LOGMONCOLUMNCOL1 4
#define COLUMN_LOGMONCOLUMNCOL2 5
#define COLUMN_LOGMONCOLUMNCOL3 6
#define COLUMN_LOGMONCOLUMNCOL4 7
#define COLUMN_LOGMONCOLUMNCOL5 8
#define COLUMN_LOGMONCOLUMNCOLFILTER1 9
#define COLUMN_LOGMONCOLUMNCOLFILTER2 10
#define COLUMN_LOGMONCOLUMNCOLFILTER3 11
#define COLUMN_LOGMONCOLUMNCOLFILTER4 12
#define COLUMN_LOGMONCOLUMNCOLFILTER5 13
#define COLUMN_LOGMONCOLUMNCREATEDTIME 14
#define COLUMN_LOGMONCOLUMNDESCRIPTION 15
#define COLUMN_LOGMONCOLUMNALERTMESSAGE 16
#define COLUMN_LOGMONCOLUMNSTATUS 17
#define COLUMN_LOGMONCOLUMNUPDATEDTIME 18
#define COLUMN_LOGMONCOLUMNFILESIZE 19
#define COLUMN_LOGMONCOLUMNREADLINE 20
#define logMonColumnTable_COL_MIN 2
#define logMonColumnTable_COL_MAX 20

/* comment out the following line if you don't handle SET-REQUEST for logMonColumnTable */
#define logMonColumnTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define logMonColumnTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define logMonColumnTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define logMonColumnTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef logMonColumnTable_SET_HANDLING

int logMonColumnTable_extract_index( logMonColumnTable_context * ctx, netsnmp_index * hdr );

void logMonColumnTable_set_reserve1( netsnmp_request_group * );
void logMonColumnTable_set_reserve2( netsnmp_request_group * );
void logMonColumnTable_set_action( netsnmp_request_group * );
void logMonColumnTable_set_commit( netsnmp_request_group * );
void logMonColumnTable_set_free( netsnmp_request_group * );
void logMonColumnTable_set_undo( netsnmp_request_group * );

logMonColumnTable_context * logMonColumnTable_duplicate_row( logMonColumnTable_context* );
netsnmp_index * logMonColumnTable_delete_row( logMonColumnTable_context* );

int logMonColumnTable_can_activate(logMonColumnTable_context *undo_ctx,
                      logMonColumnTable_context *row_ctx,
                      netsnmp_request_group * rg);
int logMonColumnTable_can_deactivate(logMonColumnTable_context *undo_ctx,
                        logMonColumnTable_context *row_ctx,
                        netsnmp_request_group * rg);
int logMonColumnTable_can_delete(logMonColumnTable_context *undo_ctx,
                    logMonColumnTable_context *row_ctx,
                    netsnmp_request_group * rg);


#ifdef logMonColumnTable_ROW_CREATION
logMonColumnTable_context * logMonColumnTable_create_row( netsnmp_index* );
#endif
#endif

#ifdef logMonColumnTable_IDX2
logMonColumnTable_context * logMonColumnTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** LOGMONCOLUMNTABLE_H */

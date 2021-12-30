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
 * $Author: rajesh $
 * $Log: psMonitorTable.h,v $
 * Revision 1.26  2003/12/31 11:40:53  rajesh
 * * Handled includes for Tru64 (osf5)
 *
 * Revision 1.25  2003/12/15 14:13:42  viren
 * Introduced Signal handler to fix unixware start process problem - fixed
 *
 * Revision 1.24  2003/12/13 19:01:52  viren
 * Wrong OID in trap mesg - fixed
 *
 * Revision 1.19  2003/12/12 14:31:04  viren
 * added signal.h to common include
 *
 * Revision 1.18  2003/12/11 04:26:46  viren
 * Fixed data file updation issue
 *
 * Revision 1.17  2003/12/10 08:43:40  ashok
 * added #define MAXPATHLEN for Windows
 *
 * Revision 1.16  2003/12/10 07:52:46  sspriya
 * Removed 'st' stray character from psMonitorTable.h
 *
 * Revision 1.15  2003/12/10 05:59:59  viren
 * MIN_PARENT_FD and MAX_PARENT_FD introduced
 *
 * Revision 1.14  2003/12/10 05:48:07  viren
 * guiTransLogFileAlarmReg, glTransInfoMaxLogFileSize and glTransCurrFileInterval were made static
 *
 * Revision 1.13  2003/12/09 19:43:58  viren
 * file close problems fixed
 *
 * Revision 1.12  2003/12/09 18:07:31  viren
 * File transfer implemented - Working fine
 *
 * Revision 1.10  2003/12/09 13:51:23  viren
 * File transfer implemented
 *
 * Revision 1.9  2003/12/08 18:21:54  viren
 * Updated review comments
 *
 * Revision 1.8  2003/12/08 13:05:59  ashok
 * Minor Changes
 *
 * Revision 1.7  2003/12/07 15:08:35  viren
 * Review comments incorporated
 *
 * Revision 1.6  2003/12/02 11:01:49  vijaysp
 * Trap msg. format changed. TrapOID value changed from Nuri OID to table oid and arg. changed to alert message string alone
 *
 * Revision 1.5  2003/12/02 03:27:35  vijaysp
 * Send Trap To Manager instead of Alert Message
 *
 * Revision 1.4  2003/11/26 12:10:16  viren
 * DIR_SEPARATOR was given wronly for unix - fixed
 *
 * Revision 1.3  2003/11/26 10:39:40  viren
 * Fixed reaction path problem
 *
 * Revision 1.2  2003/11/25 11:31:39  ashok
 * Minor Changes
 *
 * Revision 1.1  2003/11/24 06:44:11  viren
 * moved from old dir
 *
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
 * $Id: psMonitorTable.h,v 1.26 2003/12/31 11:40:53 rajesh Exp $
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
#include <string.h>
#include <signal.h>
#include<math.h>
#include "psMonitorTable.h"

#ifdef WIN32 // Windows Includes

#include <windows.h>
#include <io.h>
#include <pdh.h>
#include <winsock.h>
#include <time.h>
#include <psapi.h>

#else // Start of Unix Includes

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

// <End: WIP_NURI_ALL>

/** added by D.J Park in 2005.02.23 [start]
 * cpu ������ �ֱ������� ���Ѵ�.
 */
#define NAS_CPU_USER 0
#define NAS_CPU_KERNEL 1
#define NAS_CPU_WAIT 2
#define NAS_CPU_IDLE 3
#define NAS_CPU_UTIL 4

typedef struct psmon_cpuinfo
{
    int  ncpus;
#ifdef WIN32
    LONGLONG sampletime;
	LONGLONG total;
	LONGLONG state[5];
#else
    long sampletime;
    long long int total;
    long long int state[5];
#endif
    long stateUsage[5];
    long stateUsageFloat[5];
} psmon_cpuinfo_t;

/** added by D.J Park in 2005.02.23 [end]*/

/** added by Corbin in 2004.02.02
 * add variable by D.J Park in 2004.10.15
 * time :  current time(centi-second)
 * cputime : CPU time (centi-second)
 * cpu : percent CPU
 * hcpu : percent CPU * 100
 * mem : memory size(RSS)
 **/
 #define PROC_ARG_LEN 255

typedef struct ps_cache_info_s {
	
	int pid;
    long time;
    long cputime;
    long cpu;
    long hcpu;
    long mem;
	char name[PROC_ARG_LEN];
	char args[PROC_ARG_LEN];
	
	struct ps_cache_info_s *next;
} ps_cache_info;

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

            unsigned char psMonitorDownMessage[255];
            long psMonitorDownMessage_len;

        /** RowStatus = ASN_INTEGER */
            long psMonitorStatus;

        /** added by D.J Park in 2004.10.15 */
        /** INTEGER = ASN_INTEGER */
            long psMonitorThreshold;

        /** INTEGER = ASN_INTEGER */
            long psMonitorCPU;

        /** INTEGER = ASN_INTEGER */
            long psMonitorMEM;

        /** added by D.J Park in 2005.03.02 */
        /** INTEGER = ASN_INTEGER */
            long psMonitorStartHour;

        /** INTEGER = ASN_INTEGER */
            long psMonitorEndHour;


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

     int iThreadActiveFlag; // 0 - False, 1 - True

     /** added by D.J Park in 2004.10.15 */
     int thrcpu;
     int thrmem;

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

#ifndef MY_THR_ID
#define MY_THR_ID   pthread_self()
#endif

#ifndef TIME_TO_SLEEP
#define TIME_TO_SLEEP 3
#endif

#ifndef ALERT_SEPARATOR
#define ALERT_SEPARATOR "|"
#endif

#ifdef WIN32 // For windows
/** added by HJ.Kwon. in 2005.04.11. */
#define NSPERSEC 10000000
#define NSPERMSEC 10000

#ifndef SLEEP
#define SLEEP(x) Sleep(x * 1000)  //x in seconds
#endif

#else // For Unix

#ifndef SLEEP
#define SLEEP(x) sleep(x)         //x in seconds
#endif

#endif

#ifndef WIN32

#define PROC_MON_DATA_FILE "/var/net-snmp/psMonitorTable.data"
#define PROC_MON_DATA_FILE_TMP "/var/net-snmp/psMonitorTable.tmp"
#define DIR_SEPARATOR 47

#else

#define PROC_MON_DATA_FILE "psMonitorTable.data"
#define PROC_MON_DATA_FILE_TMP "psMonitorTable.tmp"
#define MAXHOSTNAMELEN 64
#define MAXPATHLEN     1024
#define DIR_SEPARATOR 92

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

#ifndef SUCCESS
#define SUCCESS 1
#endif

#ifndef MAX_ALERT_MESG_LEN
#define MAX_ALERT_MESG_LEN 256
#endif

#define MAX_PARENT_FD   25
#define MIN_PARENT_FD   3
#define IGNORE_ALL_SIG  0

/********************* local structure/union definitions ********************/

typedef struct nuriAlert_s
{
    char    modCode[MAX_MOD_CODE_LEN];
    char    masterIp[MAX_HOST_IP_LEN];
    char    dateTime[MAX_DATE_TIME_LEN];
    char    sysName[MAXHOSTNAMELEN+1];
    char    alertMesg[MAX_ALERT_MESG_LEN];
    unsigned long index;
} nuriAlert;


typedef struct
{
    char    variable[MAX_OID_LEN];
    char    type;
    char    value[600];
} stVarBindArgProc;


/********************* prototypes for local functions ***********************/

void ProcMon_LoadContainerFromFile();
void ProcMon_MasterThread();
void *ProcMon_ProcMonThreadMain(void*);
void Common_GetLocalHostIp(char *);
void Common_GetCurrDateTime(char *);
int Common_SendAlert(char *, char *, char *, unsigned long);
int Common_SendMesg(nuriAlert );
int ProcMon_RestartProcess(char *);
/** Modified by Corbin in 2004.02.02
	Modified by HJ.Kwon. in 2004.07.15
	Modified by D.J. Park in 2004.10.15
int ProcMon_CheckProc(char *);
int ProcMon_CheckProc(char *, char *);
int ProcMon_CheckProc(char *, char *);
**/
int ProcMon_CheckProc(psMonitorTable_context *);
void *Common_SendLogFileToManager (void *);
void Common_SendFileThread(unsigned int guiReg, void *pcInfoLogFile);
int Common_CheckFileSize(char *);
void Common_SignalIgnore(int);

/** added by Corbin in 2004.02.02
 **/

void* ProcMon_ProcCheckThreadMain(void*);
ps_cache_info* ProcMon_GetProcessList(int* ret);

/** rewrite by D.J Park in 2004.1.13
 */
int Common_SendAlert_And_Vars(char *, char *, char *, unsigned long,stVarBindArgProc *,int);
int Common_SendMesg_And_Vars(nuriAlert ,stVarBindArgProc *,int);
/** modified by HJ.Kwon in 2004.07.15 
stVarBindArgProc *makePsMonitorVarBind_Up(psMonitorTable_context *);
stVarBindArgProc *makePsMonitorVarBind_Down(psMonitorTable_context *);
*/
stVarBindArgProc *makePsMonitorVarBind_Up(psMonitorTable_context *, int);
stVarBindArgProc *makePsMonitorVarBind_Down(psMonitorTable_context *, int);
void getCurrentTime(char *);
void getProcessName(char *, char *);
void getSeperatedCmd(char *,char **);
/** added by D.J Park in 2004.10.15 */
long getcstime();
long getPercentCPU(int ,long ,long,int );
ps_cache_info *searchPrePsInfo(int );
stVarBindArgProc *makePsMonitorVarBind_RThreshold(psMonitorTable_context *, ps_cache_info *, int , int );
void sendThresholdTrap(psMonitorTable_context *,ps_cache_info *,int ,int );
void checkThreshold(ps_cache_info *, psMonitorTable_context *);
long getPsPercentCPU(int );

/** added by HJ.Kwon in 2004.07.14 */
stVarBindArgProc *makePsMonitorVarBind_Threshold(psMonitorTable_context *, int);
stVarBindArgProc *makePsMonitorVarBind_Recovery(psMonitorTable_context *, int);

/** added by D.J Park in 2005.02.23 [start]
 * cpu ������ �ֱ������� ���Ѵ�.
 */
void setCpuAverage();
long getPeriodCPUTime(int );
/** added by D.J Park in 2005.02.23 [end]*/

/** added by D.J Park in 2005.03.02 [start]*/
int getMonitoringTime(int startHour, int endHour);
/** added by D.J Park in 2005.03.02 [end]*/

/** added by HJ.Kwon. in 2005.04.11. */
#ifdef WIN32
static unsigned long to_msec(PFILETIME ptr);
int DetectNumProcessors();
#endif
#ifdef sysv4 
int getProcessorNum();
#endif

/************************* externs  ***********************/

extern char gcAgentInfoAlertMgrIp[];
extern char gcInfoLogFileProc[];
extern char gcAgentInfoFileTransferIP[];
extern long glAgentInfoAlertMgrPort;
extern long glAgentInfoFileTrInterval ;
extern long glAgentInfoFileTrPort;
extern long glAgentInfoLogFileLimitSize;
/** added by D.J Park in 2005.02.24 [start] */
extern long glAgentInfoSerialNo;
/** added by D.J Park in 2005.02.24 [end] */

extern void getIPFromASNFormat(char *, char *);

/** added by Corbin in 2004.02.02
**/
extern void Process_GetPath(int , char *, int );

// Global variables
static unsigned int    guiTransLogFileAlarmReg=0;
static int             giFileTransInProgress=0; // By default false

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
/** added by D.J Park in 2004.10.15 */
#define COLUMN_PSMONITORTHRESHOLD 13
#define COLUMN_PSMONITORCPU 14
#define COLUMN_PSMONITORMEM 15
/** added by D.J Park in 2005.03.02 */
#define COLUMN_PSMONITORSTARTHOUR 16
#define COLUMN_PSMONITORENDHOUR 17

#define psMonitorTable_COL_MIN 2
/** modified by D.J Park in 2004.10.15 
#define psMonitorTable_COL_MAX 12
*/
#define psMonitorTable_COL_MAX 17

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

/** added by Corbin in 2004.02.03
 */
pthread_mutex_t procMonProcessCheckLock;

/** added by D.J Park in 2004.1.13
*/
pthread_mutex_t sendTrapLock;

#ifdef __cplusplus
};
#endif

#endif /** PSMONITORTABLE_H */


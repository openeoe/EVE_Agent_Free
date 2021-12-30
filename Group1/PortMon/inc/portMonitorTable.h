/*****************************************************************************
 *
 * portMonitorTable.h
 *
 *      Copyright (c) 2009 Nuri Telecom Co., LTD 
 *      All rights Reserved
 *
 * author:
 *      SungHae Kim     	 
 *
 * description :
 *      This file is a C header code for Port Monitoring
 *
 * TODO:
 *      None
 *
 * CM Details:
 *
 ****************************************************************************/

#ifndef PORTMONITORTABLE_H
#define PORTMONITORTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************/

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_SYS_PROTOSW_H
#include <sys/protosw.h>
#endif

#if HAVE_SYS_SYSMP_H
#include <sys/sysmp.h>
#endif
#if defined(IFNET_NEEDS_KERNEL) && !defined(_KERNEL)
#define _KERNEL 1
#define _I_DEFINED_KERNEL
#endif
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if HAVE_WINSOCK_H
#include <winsock.h>
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if HAVE_NET_IF_H
#include <net/if.h>
#endif
#if HAVE_NET_IF_VAR_H
#include <net/if_var.h>
#endif
#ifdef _I_DEFINED_KERNEL
#undef _KERNEL
#endif
#if HAVE_SYS_STREAM_H
#include <sys/stream.h>
#endif
#if HAVE_NET_ROUTE_H
#include <net/route.h>
#endif
#if HAVE_NETINET_IN_SYSTM_H
#include <netinet/in_systm.h>
#endif
#if HAVE_NETINET_IP_H
#include <netinet/ip.h>
#endif
#if HAVE_SYS_QUEUE_H
#include <sys/queue.h>
#endif
#if HAVE_NETINET_IP_VAR_H
#include <netinet/ip_var.h>
#endif
#ifdef INET6
#if HAVE_NETINET6_IP6_VAR_H
#include <netinet6/ip6_var.h>
#endif
#endif
#if HAVE_SYS_SOCKETVAR_H
#include <sys/socketvar.h>
#endif
#if HAVE_NETINET_IN_PCB_H
#include <netinet/in_pcb.h>
#endif
#if HAVE_INET_MIB2_H
#include <inet/mib2.h>
#endif
#ifdef solaris2
#include "kernel_sunos5.h"
#else

#ifdef linux
#include <linux/kernel.h>
#else
#include "kernel.h"
#endif

#endif

#if HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#if defined(osf4) || defined(aix4) || defined(hpux10)
/*
 * these are undefed to remove a stupid warning on osf compilers
 * because they get redefined with a slightly different notation of the
 * same value.  -- Wes 
 */
#undef TCP_NODELAY
#undef TCP_MAXSEG
#endif
#if HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#endif
#if HAVE_NETINET_TCPIP_H
#include <netinet/tcpip.h>
#endif
#if HAVE_NETINET_TCP_TIMER_H
#include <netinet/tcp_timer.h>
#endif
#if HAVE_NETINET_TCP_VAR_H
#include <netinet/tcp_var.h>
#endif
#if HAVE_NETINET_TCP_FSM_H
#include <netinet/tcp_fsm.h>
#endif
#if HAVE_SYS_TCPIPSTATS_H
#include <sys/tcpipstats.h>
#endif

#ifdef hpux
#include <sys/mib.h>
#include <netinet/mib_kern.h>
#endif                          /* hpux */

#ifdef cygwin
#define WIN32
#include <windows.h>
#endif

#if HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

/**************************************************************/

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>
#include "psMonitorTable.h"

// #include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/auto_nlist.h>

#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
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


#define PORT_ARG_LEN 255

typedef struct _port_cache_info
{
	int listen_port;

    struct _port_cache_info* next;
} port_cache_info;


        /** Index portMonitorIndex is internal */

typedef struct _portMonitorTable_context
{
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long portMonitorIndex;

        /** INTEGER = ASN_INTEGER */
            long portMonitorNumber;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char portMonitorType[255];
            long portMonitorType_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char portMonitorState[20];
            long portMonitorState_len;

        /** INTEGER = ASN_INTEGER */
            long portMonitorCheckInterval;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char portMonitorDescription[255];
            long portMonitorDescription_len;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char portMonitorCreatedDateTime[20];
            long portMonitorCreatedDateTime_len;

        /** INTEGER = ASN_INTEGER */
            long portMonitorReaction;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char portMonitorReactionPath[255];
            long portMonitorReactionPath_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char portMonitorAlertMessage[255];
            long portMonitorAlertMessage_len;

        /** RowStatus = ASN_INTEGER */
            long portMonitorStatus;

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

} portMonitorTable_context;

/*************************************************************
 * function declarations
 */
void init_portMonitorTable(void);
void initialize_table_portMonitorTable(void);
const portMonitorTable_context* portMonitorTable_get_by_idx(netsnmp_index *);
const portMonitorTable_context* portMonitorTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int portMonitorTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);


/********************* local definitions ********************/

#define PORT_MON_CONTEXT_SIZE	sizeof(portMonitorTable_context)

#ifdef WIN32

#define PORT_MON_DATA_FILE "portMonitorTable.data"
#define PORT_MON_DATA_FILE_TMP "portMonitorTable.tmp"

#else

#define PORT_MON_DATA_FILE "/var/net-snmp/portMonitorTable.data"
#define PORT_MON_DATA_FILE_TMP "/var/net-snmp/portMonitorTable.tmp"

#endif

/********************* local structure/union definitions ********************/


/********************* prototypes for local functions ***********************/

void PortMon_LoadContainerFromFile();
void PortMon_MasterThread();
void* PortCheckThread(void*);
void* PortListThread(void*);
int PortMon_UpdateDataFile(portMonitorTable_context*);

int checkListenPort(portMonitorTable_context*);
port_cache_info* getTcpTable(int*);

stVarBindArgProc* makePortMonVarBind(portMonitorTable_context*);

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
extern oid portMonitorTable_oid[];
extern size_t portMonitorTable_oid_len;

#define portMonitorTable_TABLE_OID 1,3,6,1,4,1,3204,1,3,35,8

/*************************************************************
 * column number definitions for table portMonitorTable
 */
#define COLUMN_PORTMONITORINDEX				1
#define COLUMN_PORTMONITORNUMBER			2
#define COLUMN_PORTMONITORTYPE				3
#define COLUMN_PORTMONITORSTATE				4
#define COLUMN_PORTMONITORCHECKINTERVAL		5
#define COLUMN_PORTMONITORDESCRIPTION		6
#define COLUMN_PORTMONITORCREATEDDATETIME	7
#define COLUMN_PORTMONITORREACTION			8
#define COLUMN_PORTMONITORREACTIONPATH		9	
#define COLUMN_PORTMONITORALERTMESSAGE		10 
#define COLUMN_PORTMONITORSTATUS			11 

#define portMonitorTable_COL_MIN			2
#define portMonitorTable_COL_MAX			11

/* comment out the following line if you don't handle SET-REQUEST for portMonitorTable */
#define portMonitorTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define portMonitorTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define portMonitorTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define portMonitorTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef portMonitorTable_SET_HANDLING

int portMonitorTable_extract_index( 
		portMonitorTable_context* ctx, 
		netsnmp_index* hdr);

void portMonitorTable_set_reserve1( netsnmp_request_group * );
void portMonitorTable_set_reserve2( netsnmp_request_group * );
void portMonitorTable_set_action( netsnmp_request_group * );
void portMonitorTable_set_commit( netsnmp_request_group * );
void portMonitorTable_set_free( netsnmp_request_group * );
void portMonitorTable_set_undo( netsnmp_request_group * );

portMonitorTable_context* portMonitorTable_duplicate_row(portMonitorTable_context* );
netsnmp_index* portMonitorTable_delete_row(portMonitorTable_context* );

int portMonitorTable_can_activate(
						portMonitorTable_context* undo_ctx,
						portMonitorTable_context* row_ctx,
						netsnmp_request_group* rg);
int portMonitorTable_can_deactivate(
						portMonitorTable_context* undo_ctx,
						portMonitorTable_context* row_ctx,
						netsnmp_request_group* rg);
int portMonitorTable_can_delete(
						portMonitorTable_context* undo_ctx,
						portMonitorTable_context* row_ctx,
						netsnmp_request_group* rg);

#ifdef portMonitorTable_ROW_CREATION
portMonitorTable_context* portMonitorTable_create_row(netsnmp_index* );
#endif

#endif		/* portMonitorTable_SET_HANDLING */

#ifdef portMonitorTable_IDX2
portMonitorTable_context* portMonitorTable_get(const char *name, int len);
#endif


pthread_mutex_t portMonCheckLock;


#ifdef __cplusplus
};
#endif

#endif /** PORTMONITORTABLE_H */


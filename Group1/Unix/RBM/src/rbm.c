/*
 * snmptrapd.c - receive and log snmp traps
 */

/********************************* includes *********************************/

#include <net-snmp/net-snmp-config.h>

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <sys/types.h>
#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#if HAVE_WINSOCK_H
#include <winsock.h>
#else
#include <sys/socket.h>
#endif
#if HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#include <stdio.h>
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#if TIME_WITH_SYS_TIME
#include <time.h>
#endif
#else
#include <time.h>
#endif
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_SYSLOG_H
#include <syslog.h>
#endif
#if HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#if HAVE_NET_IF_H
#include <net/if.h>
#endif
#if HAVE_NETDB_H
#include <netdb.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif
#if HAVE_PROCESS_H              /* Win32-getpid */
#include <process.h>
#endif
#include <signal.h>
#include <errno.h>

#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/stat.h>
#include <sys/utsname.h>
#endif /* WIN32 */

#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "ctypes.h"

#if USE_LIBWRAP
#include <tcpd.h>
#endif

/****************************** local macros ********************************
 *
 * (Macros shareable with rest of the modules are present in .h file)
 */

#ifndef BSD4_3
#define BSD4_2
#endif

#ifndef FD_SET

typedef long    fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)        /* bits per mask */

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)      memset((p), 0, sizeof(*(p)))
#endif

#define monitorPolicyTable_ENTRY_OID ".1.3.6.1.4.1.3204.1.3.35.1.1"

#define RBM_START_COLUMN 7
#define RBM_END_COLUMN   26

#define RBM_VAR_COLUMNS      RBM_END_COLUMN-RBM_START_COLUMN+1
#define RBM_OID_COLUMNS      5

#define RBM_INVALID_OID  ".0"
#define RBM_SNMP_REMOTE_PORT 161

#define MAX_MOD_CODE_LEN 5
#define RBM_MODULE_CODE  "RBMM"

#define RBM_STR_LEN 255

#ifdef WIN32
#define RBM_DEFAULT_LOGFILE_PATH "RBM_Logfile"
#else
#define RBM_DEFAULT_LOGFILE_PATH "/usr/local/conf/RBM_Logfile"
#endif /* WIN32 */

#define RBM_DEFAULT_MAX_LOGFILE_SIZE 1048576 /* 1 MB */

#define ALERTMGR_IP_OID   ".1.3.6.1.4.1.3204.1.3.36.1.4.0"
#define ALERTMGR_PORT_OID ".1.3.6.1.4.1.3204.1.3.36.1.5.0"

#ifdef WIN32
#define MAXHOSTNAMELEN 64
#endif

/********************* local structure/union definitions ********************/

typedef struct RBM_AlertMsg
{
    char    modCode[MAX_MOD_CODE_LEN];
    char    masterIp[16];
    char    dateTime[35];
    char    sysName[MAXHOSTNAMELEN+1];
    char    alertMesg[256];
} RBM_AlertMsg;

typedef struct RBM_List {
    I8 szVariable [MAX_OID_LEN];
    I8 szValue    [MAX_OID_LEN];
} RBM_List;

/********************* prototypes for local functions ***********************/

#ifndef WIN32
int
RBM_Daemon_Start ();
#endif

int
SNMP_Trapd_Init();

void
RBM_Copy_OID_List (RBM_List *pstList, RBM_List *pstOidList, I32 iLength);

int
RBM_Validate_Single_Variable (I16 sCnt, I16 sSavedRes, RBM_List *pstList);

int
RBM_get_local_host_ip_log (char *pcStr);

int
RBM_Fill_Variables_List (RBM_List *pstList, I32 *iLength);

int
RBM_Form_Send_AlertMsg ();

int
RBM_Validate_Variables (RBM_List *pstList, I32 iLength);

int
RBM_SNMP_Get_Row (RBM_List *pstList, I32 iLength);

int
RBM_Fill_OID_List (RBM_List *pstList, RBM_List *pstOidList, I32 *iLength);

int
RBM_SendAlertMsg (RBM_AlertMsg stMesg);

int
RBM_Get_ASCII_Time (char *pcTimeStr);

/************************* Globals within this module ***********************/

FILE *fp = NULL;
char           *logfile = 0;
int             running = 1;
int             reconfig = 0;
u_long          num_received = 0;
char            default_port[] = "udp:162";

int  tcnt = 0;

struct timeval  Now;

I8   szRemAddress [25] = { 0 };
I8   szTrapMessage [RBM_STR_LEN] = { 0 };
UI16  usRemPort = 0;
I32  iSAPort = 0;
UI32 uiPolicyId = 0;

char gszFPath[150] = { 0 };
char gcAgentInfoAlertMgrIp[RBM_STR_LEN];
long glRBMAlertMgrPort;

const oid aulNuriPolicy_variables_oid[] = { 1, 3, 6, 1, 4, 1, 3204, 1, 3, 35 };

/************************* externs  ***********************/

int
main()
{

#ifndef WIN32
    RBM_Daemon_Start ();
#endif /* WIN32 */

    fp = fopen (RBM_DEFAULT_LOGFILE_PATH, "w");

    if (fp == (FILE *) NULL)
    {
        DEBUGMSGTL (("nuri/rbm", "fopen of RBM_DEFAULT_LOGFILE_PATH failed. "
                     "errno=%d", errno));
        return 1;
    }
    strcpy (gszFPath, RBM_DEFAULT_LOGFILE_PATH);

    SNMP_Trapd_Init();

    return 0;
}

#ifdef SIGHUP
RETSIGTYPE
hup_handler(int sig)
{
    reconfig = 1;
    DEBUGMSGTL (("nuri/rbm", "Signal %d received...dummy reconfiguring", sig));
    signal(SIGHUP, hup_handler);
}
#endif

RETSIGTYPE
term_handler (int sig)
{
    DEBUGMSGTL (("nuri/rbm", "Signal %d received...shutting down", sig));
    running = 0;
}

#ifndef WIN32
int
RBM_Daemon_Start ()
{
    register int childpid;
    int i=0;

    if (getppid() == 1)
        goto out;

    /* If we were not started in the background, fork and let the
       parent exit.  This also guarantees the first child is not
       a process group leader. */
  
    if ((childpid = fork()) < 0)
        perror ("error");
    else if (childpid > 0)
        exit (0); /* parent */

    if (setpgrp() == -1)
        perror ("can't change process group");

    if ((childpid = fork()) < 0)
        perror ("can't fork second child");
    else if (childpid > 0)
        exit (0);  /* first child */

    /* second child */

out:

    /* Close all unnecessary file descriptors */
    for (i=0; i < NOFILE; i++)
        close (i);

    errno = 0;

    /* Move the current directory to root, to avoid
       running from a mounted filesystem */
    chdir ("/");

    /* Clean any inherited file mode creation mask */
    umask (0);

    /* Ignore the sig child signal */
    signal (SIGCLD, SIG_IGN);

    signal (SIGHUP, hup_handler); /* immune from pgrp leader death */
    signal (SIGTERM, term_handler);
    signal (SIGINT, term_handler);
    return 0;
}
#endif /* WIN32 */

int
snmp_input (int op,
            netsnmp_session * session,
            int reqid, netsnmp_pdu *pdu, void *magic)
{
    #define NETSNMP_DS_APP_NUMERIC_IP 8
    netsnmp_transport *transport = (netsnmp_transport *) magic;
    struct hostent *host = NULL;
    netsnmp_variable_list *vptr;

    I16 sErr    = 0;
    I16 sLen    = 0;
    I16 sArgCnt = 0;
    I32 iLength = 0;
    I32 iListLen = 0;
    I32 iSendAlert = 0;
    RBM_List *pstList;
    RBM_List *pstOidList;
    RBM_List *pstHeadList;

    sLen = OID_LENGTH (aulNuriPolicy_variables_oid);

    for (vptr = pdu->variables; vptr; vptr = vptr->next_variable) {
 
    sArgCnt++;

    /* Only Nuri Policy OIDs are allowed to be present in the trap */
    if (netsnmp_oid_is_subtree (aulNuriPolicy_variables_oid, sLen,
                                              vptr->name, vptr->name_length))
    {
        /* Skipping the uptime and trap oid arguments */
        if (sArgCnt <= 2)
        {
            continue;
        }
        else
        {
            DEBUGMSGOID (("nuri/rbm", vptr->name, vptr->name_length));
            DEBUGMSGTL  (("nuri/rbm", "Unexpected Table Trap Received!!\n"));
            return 1;
        }
    }

    /* Return immediately if trap is from not trusted application */
    if (sArgCnt <= 2)
    {
        DEBUGMSGOID (("nuri/rbm", vptr->name, vptr->name_length));
        DEBUGMSGTL  (("nuri/rbm", "Unexpected Table Trap Received!!\n"));
        return 1;
    }
    
    switch ((short) vptr->type)
    {
        case ASN_INTEGER:
            iSAPort = *(vptr->val.integer);
            break;
        case ASN_UNSIGNED:
            uiPolicyId = (unsigned long) *(vptr->val.integer);
            break;
        case ASN_OCTET_STR:
            memset (szTrapMessage, '\0', sizeof (szTrapMessage));
            strncpy (szTrapMessage, vptr->val.string, vptr->val_len);
            break;
        default:
            break;
     }

   }

   fprintf (fp, "MESSAGE RECEIVED for POLICY ID: %lu\nRTM IP=%s RTM Port=%u "
                "SubAgent Port=%ld Trap Message=%s\n", uiPolicyId,
                szRemAddress, usRemPort, iSAPort, szTrapMessage);
   fflush (fp);


    if (op == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE) {

        if (pdu->command == SNMP_MSG_TRAP2) {

            num_received++;
            if (!netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, 
					NETSNMP_DS_APP_NUMERIC_IP)) {
                /*
                 * Right, apparently a name lookup is wanted.  This is only
                 * reasonable for the UDP and TCP transport domains (we
                 * don't want to try to be too clever here).  
                 */

                struct sockaddr_in *addr =
                        (struct sockaddr_in *) pdu->transport_data;

                usRemPort = addr->sin_port;
                strcpy (szRemAddress, inet_ntoa (addr->sin_addr));

                if (transport != NULL
                    && (transport->domain == netsnmpUDPDomain
                        || transport->domain == netsnmp_snmpTCPDomain)) {
                    /*
                     * This is kind of bletcherous -- it breaks the opacity of
                     * transport_data but never mind -- the alternative is a
                     * lot of munging strings from f_fmtaddr.
                     */
                    struct sockaddr_in *addr =
                        (struct sockaddr_in *) pdu->transport_data;
                    if (addr != NULL && 
                    pdu->transport_data_length == sizeof(struct sockaddr_in)) {
                        host = gethostbyaddr((char *) &(addr->sin_addr),
                        sizeof(struct in_addr), AF_INET);
                    }
                }
            }
        }
        else
        {
            DEBUGMSGTL(("nuri/rbm",
                             "Unsupported version of trap msg. received!\n"));
        }

    }
    if (op == NETSNMP_CALLBACK_OP_TIMED_OUT) {
        DEBUGMSGTL(("nuri/rbm", "Timeout: This shouldn't happen!"));
    }

    /* Allocate memory to store the monitorPolicy variable columns */
    pstList = (RBM_List *) calloc (RBM_VAR_COLUMNS, sizeof (RBM_List));

    pstHeadList = pstList;

    RBM_Fill_Variables_List (pstList, &iLength);

    iListLen = iLength;

    sErr = RBM_SNMP_Get_Row (pstList, iLength);

    if (sErr)
    {
        DEBUGMSGTL(("nuri/rbm", "1 RBM_SNMP_Get_Row failed!!!"));
        /* Free the allocated memory */
        free (pstList);
        return 1;
    }

    /* Allocate memory to store the monitorPolicy variable columns */
    pstOidList = (RBM_List *) calloc (RBM_OID_COLUMNS, sizeof (RBM_List));

    RBM_Fill_OID_List (pstList, pstOidList, &iLength);

    if (iLength == 0)
    {
        DEBUGMSGTL(("nuri/rbm", "Warning: None of the PolicyVariables Initialized !!"));
        return 0;
    }

    sErr = RBM_SNMP_Get_Row (pstOidList, iLength);

    if (sErr)
    {
        DEBUGMSGTL(("nuri/rbm", "1 RBM_SNMP_Get_Row failed!!!"));
        /* Free the allocated memory */
        free (pstList);
        free (pstOidList);
        return 1;
    }

    RBM_Copy_OID_List (pstList, pstOidList, iLength);

    iSendAlert = RBM_Validate_Variables (pstList, iLength);

    DEBUGMSGTL (("nuri/rbm", "SENDALERT is=%d", iSendAlert));

    if (iSendAlert)
    {
        RBM_Form_Send_AlertMsg ();
    }

    /* Free the allocated memory */
    free (pstList);
    free (pstOidList);

    return 1;
}

static int
pre_parse(netsnmp_session * session, netsnmp_transport *transport,
          void *transport_data, int transport_data_length)
{
    return 1;
}

static netsnmp_session *
snmptrapd_add_session(netsnmp_transport *t)
{
    netsnmp_session sess, *session = &sess, *rc = NULL;

    snmp_sess_init(session);
    session->peername = SNMP_DEFAULT_PEERNAME;  /* Original code had NULL here */
    session->version = SNMP_DEFAULT_VERSION;
    session->community_len = SNMP_DEFAULT_COMMUNITY_LEN;
    session->retries = SNMP_DEFAULT_RETRIES;
    session->timeout = SNMP_DEFAULT_TIMEOUT;
    session->callback = snmp_input;
    session->callback_magic = (void *) t;
    session->authenticator = NULL;
    sess.isAuthoritative = SNMP_SESS_UNKNOWNAUTH;

    rc = snmp_add(session, t, pre_parse, NULL);
    if (rc == NULL) {
        snmp_sess_perror("snmptrapd", session);
    }
    return rc;
}

static void
snmptrapd_close_sessions(netsnmp_session * sess_list)
{
    netsnmp_session *s = NULL, *next = NULL;

    for (s = sess_list; s != NULL; s = next) {
        next = s->next;
        snmp_close(s);
    }
}

int
SNMP_Trapd_Init ()
{
    netsnmp_session *sess_list = NULL, *ss = NULL;
    netsnmp_transport *transport = NULL;
    int             count, numfds, block;
    fd_set          fdset;
    struct timeval  timeout, *tvp;
    char           *cp, *listen_ports = NULL;
    int             agentx_subagent = 1;
#if HAVE_GETPID
    FILE           *PID;
    char           *pid_file = NULL;
#endif

    listen_ports = default_port;
#ifdef USING_AGENTX_SUBAGENT_MODULE
    /*
     * we're an agentx subagent? 
     */
    if (agentx_subagent) {
        /*
         * make us a agentx client. 
         */
        netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
			       NETSNMP_DS_AGENT_ROLE, 1);
    }
#endif

    /*
     * don't fail if we can't do agentx (ie, socket not there, or not root) 
     */
    netsnmp_ds_toggle_boolean(NETSNMP_DS_APPLICATION_ID, 
			      NETSNMP_DS_AGENT_NO_ROOT_ACCESS);
    /*
     * ignore any warning messages.
     */
    netsnmp_ds_toggle_boolean(NETSNMP_DS_APPLICATION_ID, 
			      NETSNMP_DS_AGENT_NO_CONNECTION_WARNINGS);

    /*
     * initialize the agent library 
     */
    init_agent("snmptrapd");

    /*
     * initialize local modules 
     */
    if (agentx_subagent) {
#ifdef USING_AGENTX_SUBAGENT_MODULE
        init_subagent();
#endif
    }

    /*
     * Initialize the world. Create initial user 
     */
    init_snmp("snmptrapd");

    if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, 
			       NETSNMP_DS_AGENT_QUIT_IMMEDIATELY)) {
        /*
         * just starting up to process specific configuration and then
         * shutting down immediately. 
         */
        DEBUGMSGTL (("nuri/rbm", "Shutting down immediately "
                     "NETSNMP_DS_AGENT_QUIT_IMMEDIATELY....\n"));
        running = 0;
    }
#if HAVE_GETPID
    if (pid_file != NULL) {
        if ((PID = fopen(pid_file, "w")) == NULL) {
            DEBUGMSGTL (("nuri/rbm", "fopen <%s> file failed! errno=%d",
                         pid_file, errno));
            exit(1);
        }
        fprintf(PID, "%d\n", (int) getpid());
        fclose(PID);
    }
#endif


    SOCK_STARTUP;

    cp = listen_ports;

    while (cp != NULL) {
        char *sep = strchr(cp, ',');
        if (sep != NULL) {
            *sep = 0;
        }

        transport = netsnmp_tdomain_transport(cp, 1, "udp");
        if (transport == NULL) {
            DEBUGMSGTL (("nuri/rbm","couldn't open %s -- errno %d "
                         "(\"%s\")\n", cp, errno, strerror(errno)));
            snmptrapd_close_sessions(sess_list);
            SOCK_CLEANUP;
            exit(1);
        } else {
            ss = snmptrapd_add_session(transport);
            if (ss == NULL) {
                /*
                 * Shouldn't happen?  We have already opened the transport
                 * successfully so what could have gone wrong?  
                 */
                snmptrapd_close_sessions(sess_list);
                netsnmp_transport_free(transport);
                SOCK_CLEANUP;
                exit(1);
            } else {
                ss->next = sess_list;
                sess_list = ss;
            }
        }

        /*
         * Process next listen address, if there is one.  
         */

        if (sep != NULL) {
            *sep = ',';
            cp = sep + 1;
        } else {
            cp = NULL;
        }
    }


    while (running) {
        numfds = 0;
        FD_ZERO(&fdset);
        block = 0;
        tvp = &timeout;
        timerclear(tvp);
        tvp->tv_sec = 5;
        snmp_select_info(&numfds, &fdset, tvp, &block);
        if (block == 1)
            tvp = NULL;         /* block without timeout */
        count = select(numfds, &fdset, 0, 0, tvp);
        gettimeofday(&Now, 0);
        if (count > 0) {
            snmp_read(&fdset);
        } else
            switch (count) {
            case 0:
                snmp_timeout();
                break;
            case -1:
#ifndef WIN32
                if (errno == EINTR)
                    continue;
#endif /* WIN32 */
                DEBUGMSGTL(("nuri/rbm", "select error errno=%d", errno));
                running = 0;
                break;
            default:
                DEBUGMSGTL (("nuri/rbm", "select returned %d\n", count));
                running = 0;
            }
    }

    snmptrapd_close_sessions(sess_list);
    snmp_shutdown("snmptrapd");
    snmp_disable_log();
    SOCK_CLEANUP;
    return 0;
}

int
RBM_SNMP_Get_Row (RBM_List *pstList, I32 iLength)
{
  int cnt=0;

  struct snmp_session session, *ss;
  struct snmp_pdu *pdu;
  struct snmp_pdu *response;

  oid anOID[MAX_OID_LEN];
  size_t anOID_len = MAX_OID_LEN;

  struct variable_list *vars;
  int status;
  int count=1;

  I8 str[80] = { 0 };
  I8 tmp_str[80] = { 0 };

  /*
   * Initialize the SNMP library
   */

  init_snmp ("rulemon");

  /*
   * Initialize a "session" that defines who we're going to talk to
   */
  snmp_sess_init( &session );                   /* set up defaults */
  /* session.peername = "localhost"; */
  session.peername = szRemAddress;

  /* Temporarily send request to port 161 */
  session.remote_port = RBM_SNMP_REMOTE_PORT;

  /* set the SNMP version number */
  session.version = SNMP_VERSION_1;

  /* set the SNMPv1 community name used for authentication */
  session.community = "public";
  session.community_len = strlen(session.community);

  /*
   * Open the session
   */
  SOCK_STARTUP;
  ss = snmp_open(&session);                     /* establish the session */

  if (!ss) {
    snmp_perror("ack");
    DEBUGMSGTL (("nuri/rbm", "something horrible happened!!!\n"));
    exit(2);
  }

  /*
   * Create the PDU for the data for our request.
   *   1) We're going to GET the system.sysDescr.0 node.
   */
   pdu = snmp_pdu_create (SNMP_MSG_GET);

    for (cnt=0; cnt < iLength; cnt++) 
    {
        read_objid (pstList[cnt].szVariable, anOID, &anOID_len);
        snmp_add_null_var(pdu, anOID, anOID_len);
    }

  /*
   * Send the Request out.
   */
  status = snmp_synch_response(ss, pdu, &response);

  /*
   * Process the response.
   */
   if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
      /*
       * SUCCESS: Print the result variables
       */

      I16 sTmpCnt = 0;

      for(vars = response->variables; vars; vars = vars->next_variable)
      {
        /* print_variable(vars->name, vars->name_length, vars); */
 
        memset (str, '\0', sizeof (str));
        memset (tmp_str, '\0', sizeof (str));
        for (cnt=0; cnt < (int) vars->name_length; cnt++)
        {
            sprintf (tmp_str, ".%ld", vars->name[cnt]);
            strcat (str, tmp_str);
        }

        switch ((short) vars->type)
        {
            case ASN_INTEGER:
                sprintf (str, "%ld", *(vars->val.integer));
                strcpy (pstList[sTmpCnt++].szValue, str);
                break;
            case ASN_UNSIGNED:
            case ASN_IPADDRESS:
                sprintf (str, "%lu", (UI32) *(vars->val.integer));
                strcpy (pstList[sTmpCnt++].szValue, str);
                break;
            case ASN_OBJECT_ID:
                 memset (str, '\0', sizeof (str));
                 memset (tmp_str, '\0', sizeof (str));
 
                 for (cnt=0; cnt < (int) (vars->val_len/sizeof (oid)); cnt++)
                 {
                     sprintf (tmp_str, ".%ld", vars->val.objid[cnt]);
                     strcat (str, tmp_str);
                 }
                strcpy (pstList[sTmpCnt++].szValue, str);
                break;
            default:
                fprintf (fp, "New vars->type encountered. vars->type=%d\n", vars->type);
                fflush (fp);
                break;
        }
      }

      /* manipuate the information ourselves */
      for(vars = response->variables; vars; vars = vars->next_variable) {
        if (vars->type == ASN_OCTET_STR) {
          char *sp = (char *)malloc(1 + vars->val_len);
          memcpy(sp, vars->val.string, vars->val_len);
          sp[vars->val_len] = '\0';
          printf("value #%d is a string: %s\n", count++, sp);
          free(sp);
        }
        else
          printf("value #%d is NOT a string! Ack!\n", count++);
      }
    } else {
      /*
       * FAILURE: print what went wrong!
       */

      if (status == STAT_SUCCESS)
      {
        DEBUGMSGTL (("nuri/rbm", "Error in packet\nReason: %s\n",
                     snmp_errstring(response->errstat)));
      }
      else
      {
        /* snmp_sess_perror("snmpget", ss); */
        DEBUGMSGTL (("nuri/rbm", "basic snmpget problem\n"));
      }

      status = -1;

    }

    /*
     * Clean up:
     *  1) free the response.
     *  2) close the session.
     */
    if (response)
      snmp_free_pdu(response);
    snmp_close(ss);

    SOCK_CLEANUP;

    if (status == -1)
    {
        return 1;
    }
    
    return 0;
}

/*****************************************************************************
 * name             :   RBM_Fill_Variables_List
 * description      :   Fill up the variables to be validated in an array
 * input parameters :
 * output parameters:   I8 **acVarList - Two dimensional char. array filled up
 *                      by the list of variables to be retrieved through SNMP.
 * return type      :   int
 * global variables :
 * calls            :   RTM_callback
 *****************************************************************************/

int
RBM_Fill_Variables_List (RBM_List *pstList, I32 *iLength)
{
    I8 szStrOid[80] = { 0 };
    I16 sCnt = 0;

    for (sCnt = 0; sCnt < RBM_END_COLUMN - RBM_START_COLUMN; sCnt++)
    {
        memset (szStrOid, '\0', sizeof (szStrOid));
        sprintf (szStrOid, "%s.%d.%ld", monitorPolicyTable_ENTRY_OID,
                                           sCnt+RBM_START_COLUMN, uiPolicyId);
        strcpy (pstList[sCnt].szVariable, szStrOid);
    }
    *iLength = sCnt;

    return 0;
}

int
RBM_Fill_OID_List (RBM_List *pstList, RBM_List *pstOidList, I32 *iLength)
{
    I16 sCnt = 0;
    I16 sAdd = 0;

    for (sCnt = 0; sCnt < RBM_OID_COLUMNS; sCnt++)
    {
        if (!strncmp (pstList[sAdd].szValue, RBM_INVALID_OID,
                                                   strlen (RBM_INVALID_OID)))
        {
            break;
        }
        strcpy (pstOidList[sCnt].szVariable, pstList[sAdd].szValue);
        sAdd += 4;
    }
    *iLength = sCnt;
    return 0;
}

void
RBM_Copy_OID_List (RBM_List *pstList, RBM_List *pstOidList, I32 iLength)
{
    I16 sCnt = 0;
    I16 sAdd = 0;

    for (sCnt = 0; sCnt < iLength; sCnt++)
    {
        strcpy (pstList[sAdd].szValue, pstOidList[sCnt].szValue);
        sAdd += 4;
    }
    return;
}

int
RBM_Validate_Variables (RBM_List *pstList, I32 iLength)
{
    I16 sCnt = 0;
    I16 sCondn = 1;
    I16 sSavedRes = 0;
    I16 sOutput = 0;

    fprintf (fp, "Condition %d: ", sCondn++);
    fflush (fp);

    /* First validate the first variable with threshold value */
    sSavedRes = RBM_Validate_Single_Variable (0, 0, pstList);

    for (sCnt = 3; sCnt < iLength*4 - 1; sCnt+=4)
    {
        fprintf (fp, "Condition %d: ", sCondn++);
        fflush (fp);

        sOutput = RBM_Validate_Single_Variable (sCnt, sSavedRes, pstList);

        sSavedRes = sOutput;
    }

    fprintf (fp, "Send Alert?: ");

    if  (sSavedRes)
        fprintf (fp, "YES\n");
    else
        fprintf (fp, "NO\n");

    fflush (fp);

    return sSavedRes;
}

int
RBM_Validate_Single_Variable (I16 sCnt, I16 sSavedRes, RBM_List *pstList)
{
    I32 iExpr      = 0; 
    I32 iValue     = 0;
    I32 iThreshold = 0;
    I32 iOperation = 0;
    I32 iResult    = 0;
    I8  *aExp[7] = { "<", ">", "<=", ">=", "!=", "==" ""};
    I8  *aOpn[3] = { "AND", "OR", ""};
    
    if (sCnt == 0)
    {
        iValue      = atoi (pstList[sCnt].szValue);
        iExpr       = atoi (pstList[sCnt+1].szValue);
        iThreshold  = atol (pstList[sCnt+2].szValue);

        fprintf (fp, "Value=%ld Expr=%s Threshold=%ld ",
                                                   iValue, aExp[iExpr], iThreshold);
        fflush (fp);
    }
    else
    {
        iOperation  = atol (pstList[sCnt].szValue);
        iValue      = atoi (pstList[sCnt+1].szValue);
        iExpr       = atoi (pstList[sCnt+2].szValue);
        iThreshold  = atol (pstList[sCnt+3].szValue);
        fprintf (fp, "Operation=%s Value=%ld Expr=%s Threshold=%ld ",
                          aOpn[iOperation], iValue, aExp[iExpr], iThreshold);
        fflush (fp);
    }


    switch (iExpr)
    {
        case 0:
            if (iValue < iThreshold)
                iResult = 1;
            break;
        case 1:
            if (iValue > iThreshold)
                iResult = 1;
            break;
        case 2:
            if (iValue <= iThreshold)
                iResult = 1;
            break;
        case 3:
            if (iValue >= iThreshold)
                iResult = 1;
            break;
        case 4:
            if (iValue != iThreshold)
                iResult = 1;
            break;
        case 5:
            if (iValue == iThreshold)
                iResult = 1;
            break;
        default:
            DEBUGMSGTL(("nuri/rbm",
                        "Invalid monitorPolicyExpression. Value=%d", iExpr));
            break;
    }

    if (iResult)
    {
        fprintf (fp, "Satisfied!!\n");
        fflush (fp);
    }
    else
    {
        fprintf (fp, "Not Satisfied!!\n");
        fflush (fp);
    }

    if (sCnt == 0)
        return iResult;

    if (iOperation == 0) /* AND operation */
    {
        iResult = sSavedRes & iResult;
    }
    else /* OR operation */
    {
        iResult = sSavedRes | iResult;
    }
   
    return iResult;
}

int
RBM_Form_Send_AlertMsg ()
{
    int fd=0;

    RBM_AlertMsg stMsg;
    char acTmpStr [150] = { 0 };
    char acMessage[512] = { 0 };

#ifndef WIN32
    struct stat status;
    struct utsname sys;
    uname(&sys);
#else
    int iNameLen=50;
    char* sys;
    WSADATA stWsData;
    sys = (char*) malloc(iNameLen);
    WSAStartup(MAKEWORD( 2, 2 ),&stWsData);

    //This Statement can be unsucessful at times.
    gethostname(sys,iNameLen);
#endif

    memset (stMsg.modCode, '\0', sizeof (stMsg.modCode));
    strcpy (stMsg.modCode, RBM_MODULE_CODE);

    RBM_get_local_host_ip_log (stMsg.masterIp);

    RBM_Get_ASCII_Time (stMsg.dateTime);
#ifndef WIN32
    strcpy (stMsg.sysName, sys.nodename);
#else
    strcpy(stMsg.sysName, sys);
#endif
    strcpy (stMsg.alertMesg, szTrapMessage);

    memset  (acMessage, '\0', sizeof (acMessage));
    sprintf (acMessage, "Module Code=%s Master Agent IP=%s Time=%s\nSysName=%s "
                        "Message=%s", stMsg.modCode, stMsg.masterIp,
                        stMsg.dateTime, stMsg.sysName, stMsg.alertMesg);

#ifndef WIN32
    /* Check if the file size exceeds the limit */
    fd = fileno (fp);
    if (fstat (fd, &status) == 0)
    {
        if (status.st_size + strlen (acMessage) >= RBM_DEFAULT_MAX_LOGFILE_SIZE)
        {
            fclose (fp);
            sprintf (acTmpStr, "%s.1", gszFPath);
            rename (gszFPath, acTmpStr);

            fp = fopen (gszFPath, "w");
            if (fp == (FILE *) NULL)
            {
                DEBUGMSGTL(("nuri/rbm",
                   "Couldn't open %s file errno=%d\n", gszFPath, errno));
                return 1;
            }
        }
    }
#endif /* WIN32 */

    fprintf (fp, "MESSAGE SENT:\n%s\n", acMessage);
    fflush (fp);

    // Now, send this alert msg over UDP socket
    if ( RBM_SendAlertMsg (stMsg) )
    {
        DEBUGMSGTL (("nuri/rbm", "UDP mesg sending to manager failed\n"));
        return(1);
    }
    return 0;
}

int
RBM_SendAlertMsg (RBM_AlertMsg stMesg)
{
    int iSock;
    int iBufLen;
    char aBuffer[16] = { 0 };
    char *pcAlertMesg;
    struct sockaddr_in stSrvName;
    struct hostent *pstHost;
    struct in_addr stIn;
    int sErr = 0;
    RBM_List pstList[2];

    /* Get the values of AgentInfoAlertMgrIP and Port */
    strcpy (pstList[0].szVariable, ALERTMGR_IP_OID);
    strcpy (pstList[1].szVariable, ALERTMGR_PORT_OID);

    sErr = RBM_SNMP_Get_Row (&pstList[0], 2);

    if (sErr)
    {
        DEBUGMSGTL(("nuri/rbm", "3 RBM_SNMP_Get_Row failed!!!"));
        return 1;
    }

    strcpy (gcAgentInfoAlertMgrIp, pstList[0].szValue);
    glRBMAlertMgrPort = atol (pstList[1].szValue);

    stIn.s_addr = atol (gcAgentInfoAlertMgrIp);
    memset (gcAgentInfoAlertMgrIp, '\0', sizeof (gcAgentInfoAlertMgrIp));
    strcpy (gcAgentInfoAlertMgrIp, (char*) inet_ntoa (stIn));

    fprintf (fp, "TO: Alert Manager IP=%s Port=%ld\n\n",
                 gcAgentInfoAlertMgrIp, glRBMAlertMgrPort);
    fflush (fp);

    pcAlertMesg = (char *) malloc (sizeof(char)*(sizeof(RBM_AlertMsg) + 1));

    // memset the alert message struct pointer
    memset(pcAlertMesg,'\0', sizeof(RBM_AlertMsg)+1);

    // Form the alert message
    sprintf (pcAlertMesg,"%s|%s|%s|%s|%s",
	                              stMesg.modCode,
	                              stMesg.masterIp,
	                              stMesg.dateTime,
	                              stMesg.sysName,
	                              stMesg.alertMesg);

    // create socket for connecting to server
    iSock = socket (AF_INET, SOCK_DGRAM,0);

    if(iSock < 0)
    {
        DEBUGMSGTL (("nuri/rbm", "Error open socket errno=%d", errno));
        return(1);
    }

    // construct name for connecting to server

    stSrvName.sin_family = AF_INET;
    stSrvName.sin_port = htons((short) glRBMAlertMgrPort);

    pstHost = gethostbyname (gcAgentInfoAlertMgrIp);

    if(pstHost == NULL)
    {
        DEBUGMSGTL (("nuri/rbm", "%s:unknown host\n", gcAgentInfoAlertMgrIp));
        return(1);
    }

    bcopy((char *)pstHost->h_addr, (char *)&stSrvName.sin_addr, pstHost->h_length);

    // send message to server

    iBufLen = sizeof (RBM_AlertMsg) +1;

    if(sendto(iSock, (const void*)pcAlertMesg, iBufLen, 0,
                          (struct sockaddr *)&stSrvName, sizeof(stSrvName)) <0)
    {
        DEBUGMSGTL (("nuri/rbm", "Error in sendto errno=%d", errno));
        return(1);
    }
    else
        DEBUGMSGTL (("nuri/rbm",
                "message <%s> sent to Manager IP <%s> Port <%d>\n",
                pcAlertMesg, aBuffer, glRBMAlertMgrPort));

    // close connection
    free(pcAlertMesg);
#ifdef WIN32
    closesocket(iSock);
    WSACleanup();
#else
    close(iSock);
#endif
    return(0);
}

int
RBM_get_local_host_ip_log (char *pcStr)
{
    struct hostent *he;
    struct sockaddr_in in;
#ifndef WIN32
    struct utsname host_name;
#else  // For windows
    char host_name[50] = { 0 };
    int  iNameLen = 0;
    WSADATA stWsData;
#endif
     char sys_ip [25] = { 0 };

#ifndef WIN32
    uname (&host_name);
    if ((he = gethostbyname(host_name.nodename)) == NULL) {
#else
    // Modified for Windows
    WSAStartup(MAKEWORD( 2, 2 ),&stWsData);
    iNameLen = sizeof (host_name);
    gethostname(host_name, iNameLen);
    if ((he = gethostbyname(host_name)) == NULL)
    {
#endif
    snmp_log_perror("error resolving hostname..");
    return 1;
    }

    memcpy(&in.sin_addr, he->h_addr_list[0], he->h_length);
    strcpy (sys_ip, (char*) inet_ntoa(in.sin_addr));
    DEBUGMSGTL (("nuri/rbm", "Host IP <%s>\n", sys_ip));
    strcpy (pcStr, sys_ip);
    return 0;
}

/*****************************************************************************
 * name             :   RBM_Get_ASCII_Time
 * description      :   Returns current time stamp in ascii format
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :   RBM_Form_Send_AlertMsg
 *****************************************************************************/

int
RBM_Get_ASCII_Time (char *pcTimeStr)
{
    char *pcPtr;
    struct tm stTm;
    struct timeval stTv;

    gettimeofday(&stTv, (struct timezone *) 0);
    memcpy(&stTm, localtime((time_t *) & stTv.tv_sec), sizeof(stTm));

    errno = 0;
    if ((pcPtr = asctime (&stTm)) ==  (char *) NULL)
    {
           DEBUGMSGTL(("nuri/rbm", "asctime returns NULL errno=%d\n",
                       errno));
           return 1;
    }

    if (pcPtr[strlen(pcPtr) - 1] == '\n')
        pcPtr[strlen(pcPtr) - 1] = '\0';

    strcpy (pcTimeStr, pcPtr);

    DEBUGMSGTL(("nuri/rbm", "tv_sec=%ld pcTimeStr=%s$ errno=%d\n",
                stTv.tv_sec, pcTimeStr, errno));
    return 0;
}

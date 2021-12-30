/*$Id: NASCenter_MA.c,v 1.22 2004/01/20 09:43:52 rajesh Exp $*/
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#define NASCENTER_MA_PUBLIC
#include "NASCenter_Util_List.h"
#include "NASCenter_MA.h"


 #define SA_IP_OID {1, 3, 6, 1, 4, 1, 3204, 1, 3, 34, 5, 5, 0 }
 #define SA_IP_OID_LEN  13
 #define SUBAGENT_TABLE_OIDLEN 10
 
 #ifndef NOFILE
 #define NOFILE 20
 #endif

 extern oid subAgentTable[];

 // Thread Function
 extern void *sendPDU(void *arg);

 // The SNMP version for the session we r going to use
 int giSnmpVersion;

 char *gcPeerName;
 // The SubAgent Session
 struct snmp_session *agentSess;
 void *managerSess;

 int giNASMAPort = DEFAULT_MA_PORT;
 char gcNASMAPeerName[40];


 int giNASSAPort = DEFAULT_SA_PORT;
#ifdef XMLSET
 int giNASSASetPort =  DEFAULT_MA_SET_PORT;
#endif
 oid gOidNASSAEnterprise[MAX_OID_LEN];
 int gOidNASSAEnterpriseLen = MAX_OID_LEN;
 
 oid gOidSAPortOID[] = SA_IP_OID;
 int giSAPortOIDLen = SA_IP_OID_LEN;
 
 char gcNASSACommunity[255];
 int giNASSAConfigRead = 0;

 int reloadConfig;

 char gcAgentSessPeerName[] = "0.0.0.0:0";

 char localIP[INET_ADDRSTRLEN];

/* 
 * Starts the Agent as Demon
 */

int
MA_Daemon_Start ()
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

    return 0;
}

	/* 
	* Reads the Configureation from the NASMA_CONF file
	*/
 void MA_ReadConfig() {
	FILE *fp;
	char parameterName[255];
	char cValue[255];
	int iValue;
	// Setting Default MA Port
	sprintf(gcNASMAPeerName,"%s:%d","0.0.0.0",giNASMAPort);
	fp = fopen(NASMA_CONFFILE,"r");
	if(fp!=NULL){
		while(fscanf(fp,"%s",parameterName)>0){
			if(!strcmp(parameterName,"NASMA_PORT")){
				fscanf(fp,"%d",&iValue);
				giNASMAPort = iValue;
				sprintf(gcNASMAPeerName,"%s:%d","0.0.0.0",giNASMAPort);
			} else
			if(!strcmp(parameterName,"NASSA_PORT")){
				fscanf(fp,"%d",&iValue);
				giNASSAPort = iValue;
			} else
			if(!strcmp(parameterName,"NASSA_COMMUNITY")){
				fscanf(fp,"%s",cValue);
				strcpy(gcNASSACommunity,cValue);
			}
#ifdef XMLSET
			else
			if(!strcmp(parameterName,"NASSA_SET_PORT")){
				fscanf(fp,"%d",&iValue);
				giNASSASetPort = iValue;
			}
#endif
		}// end of while(fscanf)
	} 
 }

 /*
  * This will read the configuration from the NAS SubAgent from the 
  * SubAgent Table.
  * 
 */
 void MA_NASSAReadConfig(){
 	struct snmp_pdu *reqPDU;
 	struct snmp_pdu *respPDU;
	struct sockaddr_in *remAdd;
	netsnmp_variable_list *vars;
	int status=0;
	int i=0;
	reloadConfig=0;
	reqPDU = snmp_pdu_create(SNMP_MSG_GETBULK);
	reqPDU->community=malloc(strlen(gcNASSACommunity));
	memcpy(reqPDU->community,gcNASSACommunity,strlen(gcNASSACommunity));
	reqPDU->community_len = strlen(gcNASSACommunity);
	// Setting non-repeators to 0
	reqPDU->errindex = MAX_REPEATORS_FOR_AGENT_CONFIG;
	// Setting max-repeatitions
	reqPDU->errstat = 0;
	reqPDU->version = SNMP_VERSION_2c;
	snmp_add_null_var(reqPDU,gOidSAPortOID,giSAPortOIDLen);
	
    remAdd = malloc(sizeof(struct sockaddr_in));

	remAdd->sin_family = AF_INET;
	remAdd->sin_port = htons(giNASSAPort);
	remAdd->sin_addr.s_addr = inet_addr(localIP);
 	reqPDU->transport_data = remAdd;
	reqPDU->transport_data_length = sizeof(struct sockaddr_in);

	status = snmp_synch_response(agentSess,reqPDU,&respPDU);
	if(status==STAT_SUCCESS){
		if(respPDU!=NULL && respPDU->errstat==0){
			vars = respPDU->variables;
			if(netsnmp_oid_is_subtree(subAgentTable,
							SUBAGENT_TABLE_OIDLEN,
							vars->name,vars->name_length)==0) {
				for(i=0;i<MAX_SUB_AGENTS;i++){
					// The SubAgent IP 
					sprintf(giSAIP[i],"%d.%d.%d.%d",(vars->val.string)[0],
								(vars->val.string)[1],
								(vars->val.string)[2],
								(vars->val.string[3]));
					// The SubAgent Port
					vars = vars->next_variable;
					giSAPort[i] = *(vars->val.integer);
					// The SubAgent Enterprise OID
					vars=vars->next_variable;
					vars=vars->next_variable;
					giSAEnterprise[i] =*(vars->val.integer);
					vars=vars->next_variable;
				}
			}
		}else {
			//printf("Error Retrieving Configuration\n");
		}
	} else {
		//printf("Error Retrievin Configuration STAT Failure\n");
	}
	
	// Now the Receiver can process the requests. so we set this to 1.
	giNASSAConfigRead=1;
 }

 int MA_Receiver(int operation, struct snmp_session *sess, int requestID, 
                struct snmp_pdu *pdu, void *magic){
	struct snmp_pdu *request;
    if(pdu==NULL){ 
		return 1;
	}
	// Dropping PDUs other than Requests
	if(!(pdu->command == SNMP_MSG_GET ||  pdu->command == SNMP_MSG_SET 
			|| pdu->command == SNMP_MSG_GETNEXT
					|| pdu->command == SNMP_MSG_GETBULK)){
		return 1;
	}
	// Drop direct SET requests
#ifdef XMLSET
	if(pdu->command == SNMP_MSG_SET){
    	DEBUGMSGTL (("nuri/MA", "Dropping SET request"));
		return 1;
	}
#endif
	request = snmp_clone_pdu(pdu);
	pthread_mutex_lock(&gRequestListLock);
	addToList(gstRequestList,request);
	pthread_mutex_unlock(&gRequestListLock);
	pthread_cond_signal(&gRequestListCond);
	return 1;
 }

 void MA_InitSession(){
   struct snmp_session initSess;
   struct snmp_session tempSess;
   
   // For Manager Session
   snmp_sess_init(&initSess);
   MA_ReadConfig();
   initSess.version = SNMP_DEFAULT_VERSION;
   initSess.peername = gcNASMAPeerName;
   initSess.local_port = giNASMAPort;
   initSess.remote_port = 0;
   initSess.community = "public";
   initSess.community_len = strlen("public");
   initSess.callback = MA_Receiver;
   initSess.callback_magic = NULL;
   
   // For Agent Session
   snmp_sess_init(&tempSess);
   tempSess.version = SNMP_DEFAULT_VERSION;
   tempSess.peername = gcAgentSessPeerName;
   tempSess.local_port = 0;
   tempSess.remote_port = giNASSAPort;
   tempSess.community = gcNASSACommunity;
   tempSess.community_len = strlen(gcNASSACommunity);
   tempSess.callback_magic= NULL;
   SOCK_STARTUP;
   agentSess = snmp_open(&tempSess);
   managerSess = snmp_sess_open(&initSess);
   if(managerSess == NULL || agentSess == NULL) {
    	DEBUGMSGTL (("nuri/MA", "Error Opening Session"));
		snmp_perror("NASCenterMA Snmp Open: ");
		exit(1);
   }
   MA_NASSAReadConfig();
 }
 

 void MA_Read_Packets(){
   fd_set snmpSet;
   int snmpFds, blocking;
   struct timeval tv;
   while(1){
        snmpFds=0;
        blocking=1;
        FD_ZERO(&snmpSet);
        snmp_sess_select_info(managerSess,&snmpFds,&snmpSet,&tv,&blocking);
        snmpFds = select(snmpFds,&snmpSet,0,0,0);
        if(snmpFds){
            snmp_sess_read((struct snmp_session *)managerSess,&snmpSet);
            //snmp_read(&snmpSet);
        }
    } // end of while(1)
 }
 
 initPthread() {
 	if(pthread_mutex_init(&gRequestListLock,NULL)==0){
		//printf("Mutex inited\n");
	}
	if( pthread_cond_init(&gRequestListCond,NULL)==0){
		//printf("Cond inited\n");
	}
	if( pthread_mutex_init(&gRequestListCondLock,NULL)==0){
		//printf("Request List Mutext Cond inited\n");
	}
 	pthread_create(&gFwThreadID,NULL,sendPDU,NULL);
#ifdef XMLSET
 	pthread_create(&gXMLThreadID,NULL,readXMLSetPacket,&giNASSASetPort);
#endif
 }

 void getLocalIP() {
    struct utsname un;
    struct hostent *he;
    char ipStr[INET_ADDRSTRLEN];
    uname(&un);
    he = gethostbyname(un.nodename);
    inet_ntop(he->h_addrtype,he->h_addr,localIP,INET_ADDRSTRLEN);
 }

 void init_MA(int argc, char *argv[]) {
	int i;
	MA_Daemon_Start();
    init_mib();
    if(argc>1){
        strcpy(localIP,argv[1]);;
    } else {
        getLocalIP();
    }
    gstRequestList = initList();
#ifdef XMLSET
	initXML();
#endif

	// Initialize the SubAgent Configuration
	for(i=0;i<MAX_SUB_AGENTS;i++){
		giSAPort[i]=0;
		giSAEnterprise[i]=0;
		giSAIP[i] = (char *)malloc(16);
	}
    MA_InitSession();
    initPthread();
 }
 
#ifdef ENCRYPTION
void initBlowFish() {
	bf_setkey(&bctx,KEY,strlen(KEY));
}
#endif

 main(int argc, char *argv[]) {
    //struct snmp_session *managerSess;
    //netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, 
	//	   NETSNMP_DS_LIB_DUMP_PACKET, 1);
	#ifdef ENCRYPTION
	initBlowFish();
	#endif
	init_MA(argc,argv);
    MA_Read_Packets();
 }



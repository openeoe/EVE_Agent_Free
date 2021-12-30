/* $Id: NASCenter_MA.h,v 1.9 2004/01/14 04:11:33 rajesh Exp $*/
#ifndef __NASCenter_MA_H__
#define __NASCenter_MA_H__

#include <pthread.h>

#ifndef NASMA_CONFFILE
#define NASMA_CONFFILE "/usr/local/conf/NAS_MA.conf"
#endif

// Uncomment the following line if blowfish encryption is required
//#define ENCRYPTION

// Uncomment if XML SET support is required
//#define XMLSET

#ifndef MAX_SUB_AGENTS
// The Subagents configuration -- There will be only a maximum of 5 subagents as in the MIB
#define MAX_SUB_AGENTS 5
#endif

#ifndef ENTERPRISE_OID_POS
// The Enterprise OID position in the incoming OIDs
#define ENTERPRISE_OID_POS 7
#endif

#ifndef MAX_REPEATORS_FOR_AGENT_CONFIG
// The Enterprise OID position in the incoming OIDs
#define MAX_REPEATORS_FOR_AGENT_CONFIG 20
#endif

#ifndef NAS_SUBAGENT_ENTERPRISE_OID
// The Enterprise OID for NAS System Agent
#define NAS_SUBAGENT_ENTERPRISE_OID 3204
#endif

#ifndef INET_ADDRSTRLEN
// Internet Address String Length
#define INET_ADDRSTRLEN 16
#endif



// The default Subagent Port
#define DEFAULT_SA_PORT 163

// The default Master Agent SET Port
#ifdef XMLSET
#define DEFAULT_MA_SET_PORT 161
#endif

// The Default Master Agent GET/GETNEXT/GETBULK port
#define DEFAULT_MA_PORT 161

#ifdef ENCRYPTION
// The blowfish context used by the Master Agent
#define BLOWFISH_ROUNDS 16
typedef struct {
    unsigned long s0[256];
    unsigned long s1[256];
    unsigned long s2[256];
    unsigned long s3[256];
    unsigned long p[BLOWFISH_ROUNDS+2];
} BLOWFISH_context;

BLOWFISH_context bctx;
// Change the following to the required key
#define KEY "NASSYSTEMAGENT"
#endif

#ifdef hpux11
#define inet_ntop(AF,SRC,DST,CNT) \
     ((AF) == AF_INET \
      ? ((CNT) < 16 ? (-1, (char *) NULL) \
          : (sprintf((DST), "%d.%d.%d.%d", \
          ((const unsigned char *)(const void *)(SRC))[0] & 0xff,  \
          ((const unsigned char *)(const void *)(SRC))[1] & 0xff,  \
          ((const unsigned char *)(const void *)(SRC))[2] & 0xff,  \
          ((const unsigned char *)(const void *)(SRC))[3] & 0xff), \
	  (DST))) \
          : (-1, (char *) NULL))
#endif
#ifdef XMLSET
extern void *readXMLSetPacket(void *port);
extern initXML();
extern void sendErrorMsg(int reqid,int code);
extern void sendSuccessMsg(int reqid);
#endif

#ifdef NASCENTER_MA_PUBLIC
NASCENTER_MA_PUBLIC pthread_t gFwThreadID;

#ifdef XMLSET
NASCENTER_MA_PUBLIC pthread_t gXMLThreadID;
#endif

NASCENTER_MA_PUBLIC List *gstRequestList;
NASCENTER_MA_PUBLIC pthread_mutex_t gRequestListLock;
NASCENTER_MA_PUBLIC pthread_mutex_t gRequestListCondLock;
NASCENTER_MA_PUBLIC pthread_cond_t gRequestListCond;

NASCENTER_MA_PUBLIC int giSAPort[MAX_SUB_AGENTS];
NASCENTER_MA_PUBLIC char *giSAIP[MAX_SUB_AGENTS];
NASCENTER_MA_PUBLIC int giSAEnterprise[MAX_SUB_AGENTS];
#else
#define NASCENTER_MA_PUBLIC extern
NASCENTER_MA_PUBLIC pthread_t gFwThreadID;

#ifdef XMLSET
NASCENTER_MA_PUBLIC pthread_t gXMLThreadID;
#endif

NASCENTER_MA_PUBLIC List *gstRequestList;
NASCENTER_MA_PUBLIC pthread_mutex_t gRequestListLock;
NASCENTER_MA_PUBLIC pthread_mutex_t gRequestListCondLock;
NASCENTER_MA_PUBLIC pthread_cond_t gRequestListCond;

NASCENTER_MA_PUBLIC int giSAPort[MAX_SUB_AGENTS];
NASCENTER_MA_PUBLIC char *giSAIP[MAX_SUB_AGENTS];
NASCENTER_MA_PUBLIC int giSAEnterprise[MAX_SUB_AGENTS];

#endif
#endif

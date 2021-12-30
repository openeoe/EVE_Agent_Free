/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf,v 1.3 2002/10/17 09:40:46 dts12 Exp $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "psRunningTable.h"
#include <fcntl.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/user.h>   /* PAGE_SIZE */
#include <dirent.h>

#define CACHE_TIMEOUT 5
#define MICROSEC 1000000
#define KBYTE 1024
#define PWD "PWD="

/* 
 * psRunningTable_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */

oid psRunningTable_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,33 };

/* 
 * variable4 psRunningTable_variables:
 *   this variable defines function callbacks and type return information 
 *   for the  mib section 
 */

struct variable4 psRunningTable_variables[] = {
/*  magic number        , variable type , ro/rw , callback fn  , L, oidsuffix */

};
/*    (L = length of the oidsuffix) */

typedef struct{
	
	int  iPsPid;			                       /* 1 */
	char szPsName[128];                            /* 2 */
	char cPsState;                                 /* 3 */

	int  iPsUid;	
	int  iPsPPid;           						/* 4 */
	int  iPsGid;            						/* 5 */
	int  iPsSid;            						/* 6 */
	int  iPsSession;        						/* 7 */
	int  iPsTTY;								    /* 8 */
	int  iPsTpGID;								    /* 9 */
	
	unsigned long ulPsFlags;	       				/* 10 */
	unsigned long ulPsMin_Flt;						/* 11 */
	unsigned long ulPsCMin_Flt;						/* 12 */
	unsigned long ulPsMaj_Flt;						/* 13 */
	unsigned long ulPsCMaj_Flt;						/* 14 */
	
	long lPsUTime;								    /* 15 */
	long lPsSTime;								    /* 16 */
	
	long lPsCUTime;								    /* 17 */
	long lPsCSTime;								    /* 18 */
	long lPsPriority;							    /* 19 */
	long lPsNice;								    /* 20 */
	long lPsTimeout;							    /* 21 */
	long lPsIt_Real_Value;					        /* 22 */

	unsigned long ulPsStartTime;			    	/* 23 */
	unsigned long ulPsVmSize;						/* 24 */
	unsigned long ulPsVmRSS;						/* 25 */
	unsigned long ulPsVmRSS_rlimit;					/* 26 */

	unsigned long ulPsStart_Code;					/* 27 */
	unsigned long ulPsEnd_Code;						/* 28 */
	unsigned long ulPsStart_Stack;			   		/* 29 */
	unsigned long ulPsStack_sp;						/* 30 */
	unsigned long ulPsStack_ip;						/* 31 */

	unsigned long ulPsSignal;						/* 32 */
	unsigned long ulPsBlocked;						/* 33 */
	unsigned long ulPsSigIgnore;		    		/* 34 */
	unsigned long ulPsSigCatch;						/* 35 */
	unsigned long ulPsKernelFn;						/* 36 */
	unsigned long ulPSNumSwap;						/* 37 */
	unsigned long ulPSCNumSwap;						/* 38 */

	
}gstPsStat;

/* Prototype for local function */
static void psRunningTable_Init();
static int  psRunningTable_GetNextProc();
static void psRunningTable_End();
static int psRunningTable_GetProcStat(int, gstPsStat *);
static int psRunningTable_GetHandles(int);

/* Global Declaration */
static int gILowProcIndex;
static unsigned char gString[SPRINT_MAX_LEN];
static struct timeval gstDCTimeVal={0};
DIR *fdDir = NULL;
struct dirent *pstFdDir=NULL;

/* Extern Declaration */
extern int nproc;
extern struct procsinfo *proc_table;
extern int current_proc_entry;

/* static declaration */
static FILE *fpProc = NULL;

/** Initializes the psRunningTable module */
void
init_psRunningTable(void)
{

    DEBUGMSGTL(("psRunningTable", "Initializing\n"));

    /* register ourselves with the agent to handle our mib tree */
    REGISTER_MIB("psRunningTable", psRunningTable_variables, variable4,
               psRunningTable_variables_oid);

    /* place any other initialization junk you need here */
}

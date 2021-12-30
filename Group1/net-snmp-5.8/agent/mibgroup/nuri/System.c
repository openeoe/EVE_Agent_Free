/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf,v 1.3 2002/10/17 09:40:46 dts12 Exp $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "System.h"
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <utmp.h>
#include <dirent.h>
#include <math.h>

#define STAT_FILE "/proc/stat"
#define MAXDISK 64 
#define CACHE_TIMEOUT 3 
#define MICROSEC 1000000
#define KBYTE 1024

/* 
 * System_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */

oid System_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,20 };

/* 
 * variable4 System_variables:
 *   this variable defines function callbacks and type return information 
 *   for the  mib section 
 */

extern unsigned long zero_round(double d);

struct variable4 System_variables[] = {
/*  magic number        , variable type , ro/rw , callback fn  , L, oidsuffix */
#define SYSFREEMEMORYSIZE		1
{SYSFREEMEMORYSIZE,  ASN_INTEGER,  RONLY ,  var_System, 1,  {6}},
#define SYSBUFFERBLOCKWRITES		2
{SYSBUFFERBLOCKWRITES,  ASN_INTEGER,  RONLY ,  var_System, 1,  {20}},
#define SYSPAGESSCANNED		3
{SYSPAGESSCANNED,  ASN_COUNTER,  RONLY ,  var_System, 1,  {18}},
#define SYSBUFFERBLOCKREADS		4
{SYSBUFFERBLOCKREADS,  ASN_INTEGER,  RONLY ,  var_System, 1,  {19}},
#define SYSPROCESSMEMORYUSED		5
{SYSPROCESSMEMORYUSED,  ASN_INTEGER,  RONLY ,  var_System, 1,  {27}},
#define SYSPGOUTSPAGING		6
{SYSPGOUTSPAGING,  ASN_COUNTER,  RONLY ,  var_System, 1,  {13}},
#define SYSTOTALVIRTUALMEMORY		7
{SYSTOTALVIRTUALMEMORY,  ASN_INTEGER,  RONLY ,  var_System, 1,  {16}},
#define SYSTOTALMEMORY		8
{SYSTOTALMEMORY,  ASN_INTEGER,  RONLY ,  var_System, 1,  {14}},
#define SYSNUMBERIODONE		9
{SYSNUMBERIODONE,  ASN_INTEGER,  RONLY ,  var_System, 1,  {7}},
#define SYSCPUIDLETIME		10
{SYSCPUIDLETIME,  ASN_INTEGER,  RONLY ,  var_System, 1,  {1}},
#define SYSPHYSICALREADS		11
{SYSPHYSICALREADS,  ASN_COUNTER,  RONLY ,  var_System, 1,  {23}},
#define SYSCPUUSERTIME		12
{SYSCPUUSERTIME,  ASN_INTEGER,  RONLY ,  var_System, 1,  {2}},
#define SYSUSERSNUM		13
{SYSUSERSNUM,  ASN_INTEGER,  RONLY ,  var_System, 1,  {11}},
#define SYSPAGEINS		14
{SYSPAGEINS,  ASN_COUNTER,  RONLY ,  var_System, 1,  {8}},
#define SYSFILESMEMORYUSED		15
{SYSFILESMEMORYUSED,  ASN_INTEGER,  RONLY ,  var_System, 1,  {26}},
#define SYSBUFFERCACHESIZE		16
{SYSBUFFERCACHESIZE,  ASN_INTEGER,  RONLY ,  var_System, 1,  {25}},
#define SYSMESSAGE		17
{SYSMESSAGE,  ASN_INTEGER,  RONLY ,  var_System, 1,  {28}},
#define SYSSEMAPHORE		18
{SYSSEMAPHORE,  ASN_INTEGER,  RONLY ,  var_System, 1,  {29}},
#define SYSPROCESSNUM		19
{SYSPROCESSNUM,  ASN_INTEGER,  RONLY ,  var_System, 1,  {10}},
#define SYSLOCALBUFFERWRITES		20
{SYSLOCALBUFFERWRITES,  ASN_INTEGER,  RONLY ,  var_System, 1,  {22}},
#define SYSPAGEOUTS		21
{SYSPAGEOUTS,  ASN_COUNTER,  RONLY ,  var_System, 1,  {9}},
#define SYSPGINSPAGING		22
{SYSPGINSPAGING,  ASN_COUNTER,  RONLY ,  var_System, 1,  {12}},
#define SYSTOTALPAGEFAULTS		23
{SYSTOTALPAGEFAULTS,  ASN_COUNTER,  RONLY ,  var_System, 1,  {5}},
#define SYSLOCALBUFFERREADS		24
{SYSLOCALBUFFERREADS,  ASN_INTEGER,  RONLY ,  var_System, 1,  {21}},
#define SYSFREEVIRTUALMEMORY		25
{SYSFREEVIRTUALMEMORY,  ASN_INTEGER,  RONLY ,  var_System, 1,  {17}},
#define SYSCPUSYSTEMTIME		26
{SYSCPUSYSTEMTIME,  ASN_INTEGER,  RONLY ,  var_System, 1,  {3}},
#define SYSCPUWAITTIME		27
{SYSCPUWAITTIME,  ASN_INTEGER,  RONLY ,  var_System, 1,  {4}},
#define SYSPHYSICALWRITES		28
{SYSPHYSICALWRITES,  ASN_COUNTER,  RONLY ,  var_System, 1,  {24}},
#define SYSMEMORYUTILIZATION		29
{SYSMEMORYUTILIZATION,  ASN_INTEGER,  RONLY ,  var_System, 1,  {15}},
#define SYSCPUAVERAGE  30
{SYSCPUAVERAGE,  ASN_INTEGER,  RONLY ,  var_System, 1,  {30}},

/** added by HJ.Kwon. 03.31.2004 */
#define SYSLWPSNUM                  31
{SYSLWPSNUM,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 31 }},
#define SYSRUNQSZ                   32
{SYSRUNQSZ,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 32 }},
#define SYSRUNOCC                   33
{SYSRUNOCC,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 33 }},
#define SYSSWAPSZ                   34
{SYSSWAPSZ,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 34 }},
#define SYSSWPOCC                   35
{SYSSWPOCC,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 35 }},
#define SYSSCALL                    36
{SYSSCALL,  ASN_COUNTER,  RONLY ,  var_System, 1,  { 36 }},
#define SYSSREAD                    37
{SYSSREAD,  ASN_COUNTER,  RONLY ,  var_System, 1,  { 37 }},
#define SYSSWRIT                    38
{SYSSWRIT,  ASN_COUNTER,  RONLY ,  var_System, 1,  { 38 }},
#define SYSFORK                     39
{SYSFORK,  ASN_COUNTER,  RONLY ,  var_System, 1,  { 39 }},
#define SYSEXEC                     40
{SYSEXEC,  ASN_COUNTER,  RONLY ,  var_System, 1,  { 40 }},
/** END : HJ.Kwon. */

/** added by C.E.Lee. 04.03.2004 */
#define SYSPAGEATT                  41
{SYSPAGEATT,             ASN_COUNTER,  RONLY ,  var_System, 1,  { 41 }},
#define SYSPAGEFREELIST             42
{SYSPAGEFREELIST,        ASN_COUNTER,  RONLY ,  var_System, 1,  { 42 }},
#define SYSPAGENOTFULFILL           43
{SYSPAGENOTFULFILL,      ASN_COUNTER,  RONLY ,  var_System, 1,  { 43 }},
#define SYSPAGEPROTECT              44
{SYSPAGEPROTECT,         ASN_INTEGER,  RONLY ,  var_System, 1,  { 44 }},
#define SYSPAGEADDRTRANS            45
{SYSPAGEADDRTRANS,       ASN_INTEGER,  RONLY ,  var_System, 1,  { 45 }},
#define SYSPAGESWLOCK               46
{SYSPAGESWLOCK,          ASN_INTEGER,  RONLY ,  var_System, 1,  { 46 }},
#define SYSVIRTUALPAGEDEAMON        47
{SYSVIRTUALPAGEDEAMON,   ASN_COUNTER,  RONLY ,  var_System, 1,  { 47 }},
#define SYSPHYSICALPAGEDEAMON       48
{SYSPHYSICALPAGEDEAMON,  ASN_COUNTER,  RONLY ,  var_System, 1,  { 48 }},
/** END : C.E.LEE 04/03/2004 */

/** created by SM.Jeong. in 04.07.2004 */
#define SYSCLOSEWAITNUM                  49
{SYSCLOSEWAITNUM,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 49 }},

/** added by D.J Park in 2005.02.24 */
#define SYSPERIODCPUUSERTIME                  51
{SYSPERIODCPUUSERTIME,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 51 }},
#define SYSPERIODCPUSYSTEMTIME                  52
{SYSPERIODCPUSYSTEMTIME,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 52 }},
#define SYSPERIODCPUWAITTIME                  53
{SYSPERIODCPUWAITTIME,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 53 }},
#define SYSPERIODCPUIDLETIME                  54
{ SYSPERIODCPUIDLETIME,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 54 }},
#define SYSPERIODCPUAVERAGE                  55
{SYSPERIODCPUAVERAGE,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 55 }},
#define SYSPERIODFLOATCPUUSERTIME                  56
{SYSPERIODFLOATCPUUSERTIME,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 56 }},
#define SYSPERIODFLOATCPUSYSTEMTIME                  57
{SYSPERIODFLOATCPUSYSTEMTIME,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 57 }},
#define SYSPERIODFLOATCPUWAITTIME                  58
{SYSPERIODFLOATCPUWAITTIME,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 58 }},
#define SYSPERIODFLOATCPUIDLETIME                  59
{ SYSPERIODFLOATCPUIDLETIME,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 59 }},
#define SYSPERIODFLOATCPUAVERAGE                  60
{SYSPERIODFLOATCPUAVERAGE,  ASN_INTEGER,  RONLY ,  var_System, 1,  { 60 }},

};
/*    (L = length of the oidsuffix) */

/* Local function Declaration */
void System_GetProcStat(u_long*, u_long*, u_long*, u_long*, 
			u_long*, u_long*, u_long*, u_long*, u_long*);

/* Global variable Declaration */
static struct timeval gstDCTimeVal={0};
unsigned char gString[SPRINT_MAX_LEN];

u_long   ulCpuUser, ulCpuNice, ulCpuSys, ulCpuIdle = 0;
long oldTime = 0;

/** Initializes the System module */
void
init_System(void)
{

    DEBUGMSGTL(("System", "Initializing\n"));

    /* register ourselves with the agent to handle our mib tree */
    REGISTER_MIB("System", System_variables, variable4,
               System_variables_oid);

    /* place any other initialization junk you need here */
}

/*
 * var_System():
 *   This function is called every time the agent gets a request for
 *   a scalar variable that might be found within your mib section
 *   registered above.  It is up to you to do the right thing and
 *   return the correct value.
 *     You should also correct the value of "var_len" if necessary.
 *
 *   Please see the documentation for more information about writing
 *   module extensions, and check out the examples in the examples
 *   and mibII directories.
 */


unsigned char *
var_System(struct variable *vp, 
                oid     *name, 
                size_t  *length, 
                int     exact, 
                size_t  *var_len, 
                WriteMethod **write_method)
{
    /* variables we may use later */
    static u_long ulong_ret = 0;
    static int int_ret = 0;
    static oid objid[MAX_OID_LEN];
    static struct counter64 c64;

    static u_long   ulPgpgin, ulPgpgout, pswpin, pswpout;
    static u_long   ulIOTot, ulIORead, ulBlkRead, ulIOWrite, ulBlkWrite;
    double  ulTemp;
    unsigned long ulCpuTotal;
    static struct sysinfo stSysInfo;
    struct timeval stDCTimeStamp = {0};

    ulPgpgin = ulPgpgout = pswpin = pswpout = 0;
    ulIOTot = ulIORead = ulBlkRead = ulIOWrite = ulBlkWrite = 0;
    
    ulTemp = ulCpuTotal = 0;

    if (header_generic(vp,name,length,exact,var_len,write_method)
               == MATCH_FAILED )
    {
    	return NULL;
    }
    /*
     *  Data Caching - invoke function call if "CACHE_TIMEOUT" seconds elapsed 
     *  between two successive requests 
     */
    gettimeofday(&stDCTimeStamp, NULL);

    stDCTimeStamp.tv_sec = stDCTimeStamp.tv_sec - gstDCTimeVal.tv_sec;
    stDCTimeStamp.tv_usec = stDCTimeStamp.tv_usec - gstDCTimeVal.tv_usec;
    ulong_ret = stDCTimeStamp.tv_sec + (stDCTimeStamp.tv_usec / MICROSEC); 

    if(ulong_ret > CACHE_TIMEOUT){                                                /* data caching */
        System_GetProcStat ( &ulPgpgin, &ulPgpgout, &pswpin, &pswpout,
			     &ulIOTot, &ulIORead, &ulBlkRead,
		             &ulIOWrite,  &ulBlkWrite);

        sysinfo(&stSysInfo);    
    }
    
    ulCpuTotal = ulCpuUser + ulCpuSys + ulCpuIdle;
    *var_len = sizeof(ulong_ret);

  /* 
   * this is where we do the value assignments for the mib results.
   */
    switch(vp->magic) {
    case SYSFREEMEMORYSIZE:
        ulong_ret = stSysInfo.freeram/KBYTE;
        return (u_char*) &ulong_ret;
    case SYSBUFFERBLOCKWRITES:
      	return (u_char*) &ulBlkWrite;

    case SYSPAGESSCANNED:
        ulong_ret = 0;	
        return (u_char*) &ulong_ret;

    case SYSBUFFERBLOCKREADS:
        return (u_char*) &ulBlkRead;
    case SYSPROCESSMEMORYUSED:
        ulong_ret = 0; 
        return (u_char*) &ulong_ret;
    case SYSPGOUTSPAGING:
        return (u_char*) &pswpout;
    case SYSTOTALVIRTUALMEMORY:
	ulong_ret = stSysInfo.totalswap/KBYTE;
	return (u_char*) &ulong_ret;
    case SYSTOTALMEMORY:
        ulong_ret = stSysInfo.totalram/KBYTE;
        return (u_char*) &ulong_ret;
    case SYSNUMBERIODONE:
        return (u_char*) &ulIOTot;
    case SYSCPUIDLETIME:
	ulTemp = 100 * ulCpuIdle;
	if(ulCpuTotal != 0 && ulTemp != 0)
	    ulong_ret = zero_round(ulTemp / ulCpuTotal);
	else
	    ulong_ret = 0;
        return (u_char*) &ulong_ret;
    case SYSPHYSICALREADS:
        return (u_char*) &ulIORead;
    case SYSCPUUSERTIME:
	/* CpuUser time since system boot time */
	ulTemp = 100 * ulCpuUser;
	if(ulCpuTotal != 0 && ulTemp != 0)
	    ulong_ret = zero_round(ulTemp / ulCpuTotal);
	else
	    ulong_ret = 0;
        return (u_char*) &ulong_ret;
    case SYSUSERSNUM:
        int_ret = System_CountUsers();
	return (u_char*) &int_ret;
    case SYSPAGEINS:
        return (u_char*) &ulPgpgin;
    case SYSFILESMEMORYUSED:
        ulong_ret = 0;	
	return (u_char*) &ulong_ret;
    case SYSBUFFERCACHESIZE:
        ulong_ret = 0;
    	return (u_char*) &ulong_ret;
    case SYSMESSAGE:
        ulong_ret = 0;	
        return (u_char*) &ulong_ret;
    case SYSSEMAPHORE:
        ulong_ret = 0;	
        return (u_char*) &ulong_ret;

    case SYSPROCESSNUM:
	return (u_char*) &(stSysInfo.procs);
    case SYSLOCALBUFFERWRITES:
       	ulong_ret = 0;
	*var_len = sizeof(ulong_ret);
        return (u_char*) &ulong_ret;
    case SYSPAGEOUTS:
        return (u_char*) &ulPgpgout;
    case SYSPGINSPAGING:
        return (u_char*) &pswpin;
    case SYSTOTALPAGEFAULTS:
        ulong_ret = 0;	
        return (u_char*) &ulong_ret;
    case SYSLOCALBUFFERREADS:
        ulong_ret = 0;	
        return (u_char*) &ulong_ret;
    case SYSFREEVIRTUALMEMORY:
	if(stSysInfo.freeswap != 0)
	    ulong_ret = stSysInfo.freeswap/KBYTE;
	else
	    ulong_ret = 0;
     	return (u_char*) &ulong_ret;
    case SYSCPUSYSTEMTIME:
	/* CpuSys time since system boot time */
	ulTemp = 100 * ulCpuSys;
	if(ulCpuTotal != 0 && ulTemp != 0)
	    ulong_ret = zero_round(ulTemp / ulCpuTotal);
	else
	    ulong_ret = 0;
        return (u_char*) &ulong_ret;
  case SYSCPUWAITTIME:
        ulong_ret = 0;	
        return (u_char*) &ulong_ret;
    
    case SYSPHYSICALWRITES:
        return (u_char*) &ulIOWrite;
    case SYSMEMORYUTILIZATION:
	if(stSysInfo.totalram != 0)
	    ulong_ret = 0;
	else
	    ulong_ret = 0;
        return (u_char*) &ulong_ret;
   case SYSCPUAVERAGE:
	if(ulCpuTotal != 0 && ((ulCpuUser + ulCpuSys) != 0))
            ulong_ret = (u_long) (((ulCpuUser + ulCpuSys ) * 100.0) / ulCpuTotal);
	else
 	    ulong_ret = 0;
        return (u_char*) &ulong_ret;

#ifdef NOT_IMPLEMENTED
	/* created by HJ.Kwon. in 03.24.2004. start */ 
	case SYSLWPSNUM:
     		long_ret = 0;
		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;
	case SYSRUNQSZ:
		long_return = 0;
		*var_len = sizeof(long_ret);
		return (u_char*) &long_return;
	case SYSRUNOCC:
		long_return = 0;
		*var_len = sizeof(long_ret);
		return (u_char*) &long_return;
	case SYSSWAPSZ:
		long_return = 0;
		*var_len = sizeof(long_ret);
		return (u_char*) &long_return;
	case SYSSWPOCC:
		long_ret = 0;
		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;
	case SYSSCALL:
		long_ret = 0;
		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;
	case SYSSREAD:
		long_ret = 0;
		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;
	case SYSSWRIT:
		long_ret = 0;
		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;
	case SYSFORK:
		long_ret = 0;
		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;
	case SYSEXEC:
		long_ret = 0;
		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;

	/** END : HJ. Kwon */

/** created by C.E.LEE. in 04.03.2004. start */
	case SYSPAGEATT:
		long_ret = 0;
       		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;

	case SYSPAGEFREELIST:
       		long_ret = 0;
       		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;

	case SYSPAGENOTFULFILL:
		long_ret = 0;
       		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;

	case SYSPAGEPROTECT:
		long_ret = 0;
       		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;

	case SYSPAGEADDRTRANS:
		long_ret = 0;
       		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;

	case SYSPAGESWLOCK:
		long_ret = 0;
       		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;
	
	case SYSVIRTUALPAGEDEAMON:
		long_ret = 0;
       		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;

	case SYSPHYSICALPAGEDEAMON:
		long_ret = 0;
       		*var_len = sizeof(long_ret);
		return (u_char*) &long_ret;
    /** END : C.E.LEE 04.03.2004 **/
	/** created by SM.Jeong. 04.07.2004 */
	case SYSCLOSEWAITNUM:
		long_return = 0;
		return (u_char*) &long_return;
#endif
    /** added by D.J Park in 2005.02.24 [start] 
     *      *  added by HJ.Kwon. in 2005.04.13.        */
	case SYSPERIODCPUUSERTIME:
		long_return = getPeriodCPUTime(0);
		return (u_char*) &long_return;
	case SYSPERIODCPUSYSTEMTIME:
		long_return = getPeriodCPUTime(1);
		return (u_char*) &long_return;
	case SYSPERIODCPUWAITTIME:
		long_return = getPeriodCPUTime(2);
		return (u_char*) &long_return;
	case SYSPERIODCPUIDLETIME:
		long_return = getPeriodCPUTime(3);
		return (u_char*) &long_return;
	case SYSPERIODCPUAVERAGE:
		long_return = getPeriodCPUTime(4);
		return (u_char*) &long_return;
	case SYSPERIODFLOATCPUUSERTIME:
		long_return = getPeriodFloatCPUTime(0);
		return (u_char*) &long_return;
	case SYSPERIODFLOATCPUSYSTEMTIME:
		long_return = getPeriodFloatCPUTime(1);
		return (u_char*) &long_return;
	case SYSPERIODFLOATCPUWAITTIME:
		long_return = getPeriodFloatCPUTime(2);
		return (u_char*) &long_return;
	case SYSPERIODFLOATCPUIDLETIME:
		long_return = getPeriodFloatCPUTime(3);
		return (u_char*) &long_return;
	case SYSPERIODFLOATCPUAVERAGE:
		long_return = getPeriodFloatCPUTime(4);
		return (u_char*) &long_return;
    /** added by D.J Park in 2005.02.24 [end] 
     *  added by HJ.Kwon. in 2005.04.13.      */
	default:
	ERROR_MSG("");
	return NULL;
    }
    return NULL;
}

/*****************************************************************************
 * name             :   System_GetProcStat
 * description      :   
 * input parameters :   None
 * output parameters:   *ulPIn, *ulPOut,
 *		        *ulSwPin, *ulSwOut, *ulDIOTot, *ulDIORead, 
 *                      *ulDBlkRead, *ulDIOWrite, u_long *ulDBlkWrite 
 * return type      :   void
 * global variables :   None
 * calls            :   void
 *****************************************************************************/

void
System_GetProcStat(u_long *ulPIn,   u_long *ulPOut,
		   u_long *ulSwPin, u_long *ulSwOut, u_long *ulDIOTot, 
		   u_long *ulDIORead,  u_long *ulDBlkRead, 
		   u_long *ulDIOWrite, u_long *ulDBlkWrite )
{
    FILE            *fpStat = NULL;
    static char     szBuff[8192];
    static char     *pcPos;
    unsigned int    uiMajor, uiMinor;
    u_long          ulTot, ulRio, ulRBlk, ulWio, ulWBlk;
    int		    iDkIdx;
    
    ulTot = ulRio = ulRBlk = ulWio = ulWBlk = 0;
    uiMajor = uiMinor = 0; 
    *ulPIn = *ulPOut = *ulSwPin = *ulSwOut = 0;
    *ulDIOTot = *ulDIORead = *ulDBlkRead = *ulDIOWrite = *ulDBlkWrite = 0;
    iDkIdx = 0;  

    gettimeofday(&gstDCTimeVal, NULL);

    if(fpStat != NULL)
        fclose(fpStat);

    fpStat = fopen(STAT_FILE, "r");

    if(fpStat != NULL){
       while(fgets(szBuff, 8192, fpStat) != NULL){
	   	if(!strncmp(szBuff, "page ", 5)){
		    sscanf(szBuff, "page %lu %lu", ulPIn, ulPOut);
		}
		else if(!strncmp(szBuff, "swap ", 5)){
                    sscanf(szBuff, "swap %lu %lu", ulSwPin, ulSwOut);
		}
		else if(!strncmp(szBuff, "disk_io:", 8)){
		    	pcPos = strchr(szBuff, '(');
			if (pcPos != NULL)
			{ 
				while ( sscanf(pcPos,"(%lu,%lu):(%lu,%lu,%lu,%lu,%lu)",
					&uiMajor, &uiMinor, &ulTot, &ulRio, &ulRBlk, &ulWio, &ulWBlk)== 7) 
				{
					*ulDIOTot += ulTot;
					*ulDIORead += ulRio;
					*ulDBlkRead += ulRBlk;
					*ulDIOWrite += ulWio;
					*ulDBlkWrite += ulWBlk;
					iDkIdx++;

					if (iDkIdx >= MAXDISK)
					{
                            			break;
					}
					if ( (pcPos = strchr(pcPos, ' ')) == NULL)
					{
			    			break;
					}
				
					while (*++pcPos == ' ');
		    		}
			}
		}
	   }  
           if(fpStat)  
	       fclose(fpStat);
      }    
      else{
		snmp_log(LOG_ERR, "/proc/stat open error");
      }
}



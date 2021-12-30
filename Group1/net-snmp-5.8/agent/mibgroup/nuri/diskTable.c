/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf,v 1.3 2002/10/17 09:40:46 dts12 Exp $
 */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "diskTable.h"
#include <net-snmp/agent/auto_nlist.h>
#include <fcntl.h>

/* #defines section */
#define DISK_MONOTONICALLY_INCREASING
#define PAGING_NOT_SUPPORTED 0
#define KBYTE 1024
#define STAT "/proc/stat"
#define MAXDISK 64

/* 
 * diskTable_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */
  
oid diskTable_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,22 };

/* 
 * variable4 diskTable_variables:
 *   this variable defines function callbacks and type return information 
 *   for the  mib section 
 */

struct variable4 diskTable_variables[] = {
/*  magic number        , variable type , ro/rw , callback fn  , L, oidsuffix */

#define DISKINDEX		1
{DISKINDEX,  ASN_UNSIGNED,  RONLY,   var_diskTable, 3,  { 1, 1, 1 }},
#define DISKNAME		2
{DISKNAME,  ASN_OCTET_STR,  RONLY,   var_diskTable, 3,  { 1, 1, 2 }},
#define DISKBLOCKSIZE		3
{DISKBLOCKSIZE,  ASN_INTEGER,  RONLY,   var_diskTable, 3,  { 1, 1, 3 }},
#define DISKTRANSFERS		4
{DISKTRANSFERS,  ASN_INTEGER,  RONLY,   var_diskTable, 3,  { 1, 1, 4 }},
#define DISKTOTALIO		5
{DISKTOTALIO,  ASN_INTEGER,  RONLY,   var_diskTable, 3,  { 1, 1, 5 }},
#define DISKBLOCKSREAD		6
{DISKBLOCKSREAD,  ASN_INTEGER,  RONLY,   var_diskTable, 3,  { 1, 1, 6 }},
#define DISKBLOCKSWRITE		7
{DISKBLOCKSWRITE,  ASN_INTEGER,  RONLY,   var_diskTable, 3,  { 1, 1, 7 }},
#define DISKACTIVETIME		8
{DISKACTIVETIME,  ASN_INTEGER,  RONLY,   var_diskTable, 3,  { 1, 1, 8 }},
#define DISKPAGING		9
{DISKPAGING,  ASN_INTEGER,  RONLY,   var_diskTable, 3,  { 1, 1, 9 }},
#define DISKPAGESIZE		10
{DISKPAGESIZE,  ASN_INTEGER,  RONLY,   var_diskTable, 3,  { 1, 1, 10 }},
#define DISKFACTOR		11
{DISKFACTOR,  ASN_INTEGER,  RONLY,   var_diskTable, 3,  { 1, 1, 11 }},
#define DISKRESPONSETIME		12
{DISKRESPONSETIME,  ASN_TIMETICKS,  RONLY,   var_diskTable, 3,  { 1, 1, 12 }},
};
/*    (L = length of the oidsuffix) */

/* typedefs */
struct gstDiskStat{
    unsigned int  uiMajor;
    unsigned int  uiIndex;
    unsigned long ulIOTot;
    unsigned long ulIORead;
    unsigned long ulIOWrite;
    unsigned long ulBRead;
    unsigned long ulBWrite;
};


/* Function Prototypes */
void diskTable_Init();
int diskTable_GetNextDisk();
void diskTable_GetDiskStat();

/* Global Declaration */
static int giDiskIdx=0;
static int giDiskCnt=0;
static struct gstDiskStat *gpstDiskFirst = NULL;
static struct gstDiskStat *gpstDiskStat = NULL;


/** Initializes the diskTable module */
void
init_diskTable(void)
{

    DEBUGMSGTL(("diskTable", "Initializing\n"));

    /* register ourselves with the agent to handle our mib tree */
    REGISTER_MIB("diskTable", diskTable_variables, variable4,
               diskTable_variables_oid);

    /* place any other initialization junk you need here */
}

/*
 * var_diskTable():
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
int
header_diskTable(struct variable *vp, 
                oid     *name, 
                size_t  *length, 
                int     exact, 
                size_t  *var_len, 
                WriteMethod **write_method)
{
    /* variables we may use later */
    static long long_ret;
    static u_long ulong_ret;
    static unsigned char string[SPRINT_MAX_LEN];
    static oid objid[MAX_OID_LEN];
    static struct counter64 c64;

  /*  if (header_generic(vp,name,length,exact,var_len,write_method)
                                  == MATCH_FAILED )
    return NULL; */
  #define NAME_LENGTH    13
    oid             newname[MAX_OID_LEN];
    int             iDiskIdx, iLowIndex = -1;
    int             iResult;

    DEBUGMSGTL(("nuri/diskTable", "header_diskTable: "));
    DEBUGMSGOID(("nuri/diskTable", name, *length));
    DEBUGMSG(("nuri/diskTable", " %d\n", exact));

    memcpy((char *) newname, (char *) vp->name, vp->namelen * sizeof(oid));
    /*
     * Find "next" Disk Entry
     */

    diskTable_Init();
    for (;;) {
        iDiskIdx = diskTable_GetNextDisk();
        if (iDiskIdx == -1)
            break;
        newname[NAME_LENGTH] = iDiskIdx;
        iResult = snmp_oid_compare(name, *length, newname, vp->namelen + 1);
        if (exact && (iResult == 0)) {
            iLowIndex = iDiskIdx;
            break;
        }
        if ((!exact && (iResult < 0)) &&
            (iLowIndex == -1 || iDiskIdx < iLowIndex)) {
            iLowIndex = iDiskIdx;
#ifdef DISK_MONOTONICALLY_INCREASING
            break;
#endif
        }
    }

    if (iLowIndex == -1) {
        DEBUGMSGTL(("host/hr_filesys", "... uiIndex out of range\n"));
        return (MATCH_FAILED);
    }

    memcpy((char *) name, (char *) newname, (vp->namelen + 1) * sizeof(oid));
    *length = vp->namelen + 1;
    *write_method = 0;
    *var_len = sizeof(u_long);    /* default to 'long' results */

    return iLowIndex;

}

/*
 * var_diskTable():
 *   Handle this table separately from the scalar value case.
 *   The workings of this are basically the same as for var_ above.
 */
unsigned char *
var_diskTable(struct variable *vp,
    	    oid     *name,
    	    size_t  *length,
    	    int     exact,
    	    size_t  *var_len,
    	    WriteMethod **write_method)
{
    /* variables we may use later */
    static long long_ret;
    static u_long ulong_ret;
    static unsigned char string[SPRINT_MAX_LEN];
    static oid objid[MAX_OID_LEN];
    static struct counter64 c64;

    static int iDiskIdx;
    static int iTemp;
    static struct gstDiskStat stDiskStat;

  /* 
   * This assumes that the table is a 'simple' table.
   *	See the implementation documentation for the meaning of this.
   *	You will need to provide the correct value for the TABLE_SIZE parameter
   *
   * If this table does not meet the requirements for a simple table,
   *	you will need to provide the replacement code yourself.
   *	Mib2c is not smart enough to write this for you.
   *    Again, see the implementation documentation for what is required.
   */

    DEBUGMSGTL(("nuri/diskTable", "var_diskTable entered\n"));
    if ((iDiskIdx = header_diskTable(vp,name,length,exact,var_len,write_method)) == MATCH_FAILED){
        if(gpstDiskFirst != NULL){
       	    free(gpstDiskFirst);
	    gpstDiskFirst = NULL;
	}
	return NULL;
    }
    memcpy(&stDiskStat, gpstDiskStat, sizeof(struct gstDiskStat));
    if(gpstDiskFirst != NULL){
    	free(gpstDiskFirst);
    	gpstDiskFirst = NULL;
    }
   *var_len = sizeof(u_long);

   switch(vp->magic)
   {
    case DISKINDEX:
       	DEBUGMSGTL(("nuri/diskTable", "Var_diskTable: DISKINDEX: %d", iDiskIdx));
        return (u_char*) &iDiskIdx; /* Disk Index */
    case DISKNAME:
        sprintf(string,"dev%d-%d", stDiskStat.uiMajor, stDiskStat.uiIndex ); 
        *var_len = strlen(string);
        return (u_char*) string;
  /*  case DISKBLOCKSIZE:
        ulong_ret = 0;
        return (u_char*) &ulong_ret; 
    case DISKTRANSFERS:
         ulong_ret = 0;
        return (u_char*) &ulong_ret; 
  */ 
    case DISKTOTALIO:
    	ulong_ret = stDiskStat.ulIOTot;    
   	return (u_char*) &ulong_ret; 
    case DISKBLOCKSREAD:
    	ulong_ret = stDiskStat.ulBRead;    
   	return (u_char*) &ulong_ret; 
    case DISKBLOCKSWRITE:
    	ulong_ret = stDiskStat.ulBWrite;    
   	return (u_char*) &ulong_ret; 
/*
    case DISKACTIVETIME:
        ulong_ret = 0;
        return (u_char*) &ulong_ret;  
    case DISKPAGING:
        ulong_ret = PAGING_NOT_SUPPORTED;	
        *var_len = sizeof(ulong_ret);
        return (u_char*) &ulong_ret;
    case DISKPAGESIZE:
        ulong_ret = PAGING_NOT_SUPPORTED;	
        *var_len = sizeof(ulong_ret);
        return (u_char*) &ulong_ret;
    case DISKFACTOR:
        ulong_ret = PAGING_NOT_SUPPORTED;	
        *var_len = sizeof(ulong_ret);
        return (u_char*) &ulong_ret;
    case DISKRESPONSETIME:
        ulong_ret = 0;
        *var_len = sizeof(ulong_ret);
        return (u_char*) &ulong_ret;
*/
    default:
      ERROR_MSG("");
}
return NULL;
}


/*****************************************************************************
 * name             :   diskTable_Init
 * description      :   Initializes the disk table index
 * input parameters :   None
 * output parameters:   None 
 * return type      :   void
 * global variables :   giDiskIdx
 * calls            :   diskTable_GetDiskStat
 *****************************************************************************/
void diskTable_Init(){
    DEBUGMSGTL(("diskTable", "diskTable_Init() start\n"));

    giDiskIdx = 1;
    diskTable_GetDiskStat();   
    DEBUGMSGTL(("diskTable", "diskTable_Init() stop\n"));
}

/*****************************************************************************
 * name             :   diskTable_GetNextDisk 
 * description      :   Gets the next disk status information
 * input parameters :   None
 * output parameters:   None 
 * return type      :   int  
 * global variables :   gpstDiskStat
 * calls            :   void 
 *****************************************************************************/
int diskTable_GetNextDisk(){
    DEBUGMSGTL(("diskTable", "diskTable_GetNextDisk() start\n"));
    if(giDiskIdx <= giDiskCnt){
        gpstDiskStat = &gpstDiskFirst[giDiskIdx - 1];
    	DEBUGMSGTL(("diskTable", "diskTable_GetNextDisk() stop\n"));
	return giDiskIdx++;
    }else{
	gpstDiskStat = NULL;
    	DEBUGMSGTL(("diskTable", "diskTable_GetNextDisk() stop giDiskIdx[null]\n"));
    	return -1;
    }
}

/*****************************************************************************
 * name             :   diskTable_GetDiskStat
 * description      :   
 * input parameters :   None
 * output parameters:   None
 * return type      :   void
 * global variables :   giDiskCnt, gpstDiskFirst
 * calls            :   void
 *****************************************************************************/
void diskTable_GetDiskStat(){
    FILE               *fpStat = NULL;
    char 	        szBuff[8192];
    char               *pcPos;
    int                 iSize = 0;
	int 				scanResult=0;
    struct gstDiskStat *pstDiskCurrent = NULL; 

    giDiskCnt = 0; 
    DEBUGMSGTL(("nuri/diskTable", "diskTable_GetDiskStat entered\n"));
/*
   if(fpStat != NULL)
	fclose(fpStat);
*/
    fpStat = fopen(STAT, "r");
    if(fpStat != NULL){
        memset(szBuff, '\0', 8192);
	while(fgets(szBuff, 8192, fpStat) != NULL){
            if(!strncmp(szBuff, "disk_io:", 8)){
		pcPos = strchr(szBuff, '(');
		if (pcPos != NULL)
		{ 
			while (1)
			{
				iSize = (giDiskCnt + 1) * sizeof(struct gstDiskStat);			
				pstDiskCurrent = (struct gstDiskStat *)realloc(pstDiskCurrent, 
							iSize);	
				scanResult = sscanf(pcPos,"(%d,%d):(%lu,%lu,%lu,%lu,%lu)",
		    			&(pstDiskCurrent[giDiskCnt].uiMajor), 
		    			&(pstDiskCurrent[giDiskCnt].uiIndex), 
		    			&(pstDiskCurrent[giDiskCnt].ulIOTot), 
		    			&(pstDiskCurrent[giDiskCnt].ulIORead), 
		    			&(pstDiskCurrent[giDiskCnt].ulBRead), 
		    			&(pstDiskCurrent[giDiskCnt].ulIOWrite),
		    			&(pstDiskCurrent[giDiskCnt].ulBWrite));
				if(!scanResult) {
					break;
				}
		    		giDiskCnt++;
				
		    		if (giDiskCnt >= MAXDISK)
					break;
                    		if ( (pcPos = strchr(pcPos, ' ')) == NULL)
					break;
				
		    		while (*++pcPos == ' ');
			}
		}
		gpstDiskFirst = pstDiskCurrent;
                if(fpStat != NULL)
		    fclose(fpStat);
		return;
	   }   
	   else{
	       DEBUGMSGTL(("nuri/diskTable", "disk_io not found in /proc/stat\n"));
	   }
	}
	if(fpStat != NULL)
	    fclose(fpStat);
    }
    else{
	snmp_log(LOG_ERR, "/proc/stat open error");
    }
}


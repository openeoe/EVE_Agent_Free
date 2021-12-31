/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf,v 1.3 2002/10/17 09:40:46 dts12 Exp $
 */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "fsTable.h"
#include <sys/statfs.h>
#include <sys/stat.h>
#include <mntent.h> 

/* #defines section */
#define HRFS_MONOTONICALLY_INCREASING
#define KBYTE 1024

/* Global Declaration */
/* 
 * fsTable_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */

oid fsTable_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,24 };

/* 
 * variable4 fsTable_variables:
 *   this variable defines function callbacks and type return information 
 *   for the  mib section 
 */

struct variable4 fsTable_variables[] = {
/*  magic number        , variable type , ro/rw , callback fn  , L, oidsuffix */

#define FSINDEX		1
{FSINDEX,  ASN_UNSIGNED,  RONLY,   var_fsTable, 3,  { 1, 1, 1 }},
#define FSID		2
{FSID,  ASN_OCTET_STR,  RONLY,   var_fsTable, 3,  {  1, 1, 2 }},
#define FSNAME		3
{FSNAME,  ASN_OCTET_STR,  RONLY,   var_fsTable, 3,  {  1, 1, 3 }},
#define FSCAPACITY		4
{FSCAPACITY,  ASN_INTEGER,  RONLY,   var_fsTable, 3,  { 1, 1, 4 }},
#define FSTYPE		5
{FSTYPE,  ASN_OCTET_STR,  RONLY,   var_fsTable, 3,  { 1, 1, 5 }},
#define FSFREE		6
{FSFREE,  ASN_INTEGER,  RONLY,   var_fsTable, 3,  { 1, 1, 6 }},
#define FSRATIO		7
{FSRATIO,  ASN_INTEGER,  RONLY,   var_fsTable, 3,  { 1, 1, 7 }},
#define FSUTILIZATION		8
{FSUTILIZATION,  ASN_INTEGER,  RONLY,   var_fsTable, 3,  { 1, 1, 8 }},
#define FSINODERATIO		9
{FSINODERATIO,  ASN_INTEGER,  RONLY,   var_fsTable, 3,  { 1, 1, 9 }},
#define FSLOGICALVOLNAME		10
{FSLOGICALVOLNAME,  ASN_OCTET_STR,  RONLY,   var_fsTable, 3,  { 1, 1, 10 }},
#define FSTOTALINODESINFS		11
{FSTOTALINODESINFS,  ASN_INTEGER,  RONLY,   var_fsTable, 3,  { 1, 1, 11 }},
#define FSFREEINODESINFS		12
{FSFREEINODESINFS,  ASN_INTEGER,  RONLY,   var_fsTable, 3,  { 1, 1, 12 }},
#define FSUSEDINODES		13
{FSUSEDINODES,  ASN_INTEGER,  RONLY,   var_fsTable, 3,  { 1, 1, 13 }},
#define FSUSERFREESPACE		14
{FSUSERFREESPACE,  ASN_INTEGER,  RONLY,   var_fsTable, 3,  { 1, 1, 14 }},
#define FSBLOCKSIZE		15
{FSBLOCKSIZE,  ASN_INTEGER,  RONLY,   var_fsTable, 3,  { 1, 1, 15 }},
#define FSFREEBLOCKS		16
{FSFREEBLOCKS,  ASN_INTEGER,  RONLY,   var_fsTable, 3,  { 1, 1, 16 }},
#define FSTOTALBLOCKS		17
{FSTOTALBLOCKS,  ASN_INTEGER,  RONLY,   var_fsTable, 3,  { 1, 1, 17 }},
};

/* Extern Declaration */
extern struct mntent *HRFS_entry;

/* Static Declaration */
static struct statfs gstStatFs;

/*    (L = length of the oidsuffix) */
unsigned long VAR;
#define VALUE 0
#define TABLE_SIZE 6

/** Initializes the fsTable module */
void
init_fsTable(void)
{

    DEBUGMSGTL(("fsTable", "Initializing\n"));

    /* register ourselves with the agent to handle our mib tree */
    REGISTER_MIB("fsTable", fsTable_variables, variable4,
               fsTable_variables_oid);

    /* place any other initialization junk you need here */
}

/*
 * var_fsTable():
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
header_fsTable(struct variable *vp, 
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
    return NULL;*/


    #define HRFSYS_ENTRY_NAME_LENGTH	13
    oid             newname[MAX_OID_LEN];
    int             iFsysIdx, iLowIndex = -1;
    int             iResult;

    DEBUGMSGTL(("nuri/fsTable", "header_fsTable "));
    DEBUGMSGOID(("nuri/fsTable", name, *length));
    DEBUGMSG(("nuri/fsTable", " %d\n", exact));

    memcpy((char *) newname, (char *) vp->name, vp->namelen * sizeof(oid));
    /*
     * Find "next" file system entry 
     */

    Init_HR_FileSys();
    for (;;) {
        iFsysIdx = Get_Next_HR_FileSys();
        if (iFsysIdx == -1)
            break;
        newname[HRFSYS_ENTRY_NAME_LENGTH] = iFsysIdx;
        iResult = snmp_oid_compare(name, *length, newname, vp->namelen + 1);
        if (exact && (iResult == 0)) {
            iLowIndex = iFsysIdx;
            break;
        }
        if ((!exact && (iResult < 0)) &&
            (iLowIndex == -1 || iFsysIdx < iLowIndex)) {
            iLowIndex = iFsysIdx;
#ifdef HRFS_MONOTONICALLY_INCREASING
            break;
#endif
        }
    }

    if (iLowIndex == -1) {
        DEBUGMSGTL(("host/hr_filesys", "... index out of range\n"));
        return (MATCH_FAILED);
    }

    memcpy((char *) name, (char *) newname,
           (vp->namelen + 1) * sizeof(oid));
    *length = vp->namelen + 1;
    *write_method = 0;
    *var_len = sizeof(long);    /* default to 'long' results */

    DEBUGMSGTL(("nuri/fsTable", "... get filesys stats "));
    DEBUGMSGOID(("nuri/fsTable", name, *length));
    DEBUGMSG(("nuri/fsTable", "\n"));

    return iLowIndex;

}


/*
 * var_fsTable():
 *   Handle this table separately from the scalar value case.
 *   The workings of this are basically the same as for var_ above.
 */
unsigned char *
var_fsTable(struct variable *vp,
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
   
    static int iFsysIdx;
    static unsigned long ulTemp;
    static struct statfs stStatFs;
    static struct stat stStat;
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
 /*   if (header_simple_table(vp,name,length,exact,var_len,write_method, TABLE_SIZE)
                                                == MATCH_FAILED )
    return NULL; */

    iFsysIdx = header_fsTable(vp, name, length, exact, var_len, write_method);
    if(iFsysIdx == MATCH_FAILED){
        return NULL;
    }
    if(statfs(HRFS_entry->mnt_dir,(struct statfs *)&stStatFs)<0){
        return NULL;
    }
    if(stat(HRFS_entry->mnt_dir,(struct stat *)&stStat)<0){
        return NULL;
    }
    *var_len = sizeof(unsigned long);
  /* 
   * this is where we do the value assignments for the mib results.
   */
    switch(vp->magic) {
    case FSINDEX:
        long_ret = iFsysIdx;
        return (u_char*) &long_ret;
    case FSID:
        sprintf(string,"%d",stStat.st_dev);  /* File system identifier */
        *var_len = strlen(string);
        return (u_char*) string;
    case FSNAME:
        sprintf(string,"%s",HRFS_entry->mnt_dir);	/* File system name */
        *var_len = strlen(string);
        return (u_char*) string;
    case FSCAPACITY:
        long_ret = ((long long)stStatFs.f_bsize * stStatFs.f_blocks)/KBYTE;	/* File system capacity in Kilobytes */
        return (u_char*) &long_ret;
    case FSTYPE:
        sprintf(string,"%s",HRFS_entry->mnt_type);	/* File system type */
        *var_len = strlen(string);	
        return (u_char*) string;
    case FSFREE:
        long_ret = ((long long) stStatFs.f_bavail * stStatFs.f_bsize)/KBYTE;	/* File system Free space in Kilo bytes*/
        return (u_char*) &long_ret;
    case FSRATIO:
        ulTemp = (u_long)((stStatFs.f_blocks - stStatFs.f_bavail) * 100);
        if(stStatFs.f_blocks == 0){
	       	long_ret = 0;
        }else{
         	long_ret = ulTemp / (unsigned long)stStatFs.f_blocks;
       	}
       	return (u_char*) &long_ret;
    case FSUTILIZATION:
       ulTemp = ((unsigned long)stStatFs.f_blocks - (unsigned long)stStatFs.f_bfree) *100;
       if(stStatFs.f_blocks == 0){
           long_ret = 0;
       }
       else{
           long_ret = ulTemp / (unsigned long)stStatFs.f_blocks;
	}
        return (u_char*) &long_ret;
    case FSINODERATIO:
        ulTemp = (unsigned long)(stStatFs.f_files - stStatFs.f_ffree) * 100;
	if(stStatFs.f_files == 0){
	    long_ret = 0;
	}
	else{
            long_ret = ulTemp / (unsigned long)stStatFs.f_files;	/* Inodes used to Total Inodes ratio */
	}
        return (u_char*) &long_ret;
    case FSLOGICALVOLNAME:
        sprintf(string,"%s",HRFS_entry->mnt_fsname);   /* Logical volume name */
        *var_len = strlen(string);
        return (u_char*) string;
    case FSTOTALINODESINFS:
        long_ret = stStatFs.f_files;	/* Total Inodes of filesystem */
        return (u_char*) &long_ret;
    case FSFREEINODESINFS:
        long_ret = stStatFs.f_ffree;	/* Free Inodes of filesystem */
        return (u_char*) &long_ret;
    case FSUSEDINODES:
        long_ret = stStatFs.f_files - stStatFs.f_ffree;    /* Used Inodes */
        return (u_char*) &long_ret;
    case FSUSERFREESPACE:
        long_ret = ((unsigned long)stStatFs.f_bsize/KBYTE) * (unsigned long)stStatFs.f_bavail;	/* Free space available to non-super user */
        return (u_char*) &long_ret;
    case FSBLOCKSIZE:
        long_ret = stStatFs.f_bsize;	/* File system block size */
        return (u_char*) &long_ret;
    case FSFREEBLOCKS:
        long_ret = stStatFs.f_bfree;	/* Free blocks on file system */
        return (u_char*) &long_ret;
    case FSTOTALBLOCKS:
        long_ret = stStatFs.f_blocks;	/* Total data blocks in file system */
        return (u_char*) &long_ret;
    default:
      ERROR_MSG("");
    }
    return NULL;
}



/*****************************************************************************
 *
 * [hrStorageEntry_Win.c]
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Name : Ashok Shankar
 *
 * description :
 *      [ Collecting information about the Network devices present in the 
 *		  System ]
 *
 * TODO:
 *      [ Facilitate dynamic data collection. use Timestamps.]
 *
 * CM Details:
 *
 * $Author: howonsuk $
 * $Log: hrStorageEntry_NTWin.c,v $
 * Revision 1.2  2004/03/03 09:09:04  howonsuk
 * timecache계산에 절대값 사용
 *
 * Revision 1.1  2004/03/03 06:14:20  howonsuk
 * 로그모니터링: 정규식 추가
 * 프로세스모니터링 : 파라미터보여주기 및 파라미터에서 정규식으로 찾기 추가
 *
 * Revision 1.1  2004/01/22 09:55:58  karthis
 * update volume label for NT
 *
 * Revision 1.16  2004/01/13 14:46:26  karthis
 * updated volume name and serial number
 *
 * Revision 1.15  2004/01/12 11:51:45  karthis
 * beautified the description
 *
 * Revision 1.14  2004/01/12 05:33:25  karthis
 * none map drive problem solved
 *
 * Revision 1.13  2004/01/12 04:06:24  karthis
 * network drive updated for hrstorageentry
 *
 * Revision 1.12  2004/01/10 11:28:02  saisk
 * corrected remote drives it inside gerr if (this is used to find out  whether disk is present inside drive or not )
 *
 * Revision 1.11  2004/01/09 12:54:32  saisk
 * Updated : modified Disk Description to display the disk label in paranthesis
 *
 * Revision 1.10  2004/01/09 11:26:46  saisk
 * Updated : memset for "gNrem" and "gcdisklabel"
 *
 * Revision 1.9  2004/01/08 06:59:09  karthis
 * allocation units included for hrstorage and some description added
 *
 * Revision 1.8  2004/01/08 03:13:34  karthis
 * label included
 *
 * Revision 1.7  2003/12/24 12:58:01  ashok
 * Changed values for used memory
 *
 * Revision 1.6  2003/11/27 15:47:37  ashok
 * Minor Changes
 *
 * Revision 1.5  2003/11/27 13:45:44  ashok
 * Minor Changes
 *
 * Revision 1.4  2003/11/20 06:22:16  ashok
 * Minor Corrections
 *
 * Revision 1.3  2003/11/18 05:02:04  ashok
 * Added collection for hrMemorySize
 *
 * Revision 1.2  2003/11/08 15:23:18  ashok
 * no message
 *
 * Revision 1.1  2003/10/31 12:08:44  ashok
 * Added into CVS by Ashok
 *
 * $State: Exp $
 * $Id: hrStorageEntry_NTWin.c,v 1.2 2004/03/03 09:09:04 howonsuk Exp $
 * $Name:  $ 
 *
 ****************************************************************************/
/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf,v 1.3 2002/10/17 09:40:46 dts12 Exp $
 */

/********************************* includes *********************************/
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "hrStorageEntry_Win.h"
#include <Winnetwk.h>


/****************************** local macros ********************************/
// For Index Genaration
#define HRSTORAGE_MONOTONICALLY_INCREASING
#define VIRTUAL         100
#define CACHETIME       5000
#define NOOFDRIVE 50
#define DRIVENAME MAX_PATH
/********************* local structure/union definitions ********************/

/********************* prototypes for local functions ***********************/
int     header_hrstorage(struct variable *vp,
              oid * name,
              size_t * length,
              int exact, size_t * var_len, WriteMethod ** write_method);
void    Init_HR_Storage(void);
void volumelabel(char *drive);
int     Get_Next_HR_Storage(void);


/************************ globals within this module*************************/
// Global Variables siSet in STOGet_Data Function
DWORD           gaSectorPerCluster[50],
                gaBytesPerSector[50],
                gaNoOfFreeClust[50],
		        gaTotalClust[50],
                gaUsedClus[50],
                gdwPhyMem,
                gaunits[50];
                
UINT            giType[50];
MEMORYSTATUS    gstMemStatus;
int             giErr = 0;
int             gir=0;
char            gaStorageDescription[100];
long            glDiskSize;
long            glDiskUsed;
char            gcdisklabel[NOOFDRIVE][DRIVENAME];
char            gNrem[NOOFDRIVE][DRIVENAME];
int             girrc=0; 

//extern int Remotemount(void);  
//extern NETRESOURCE gNR[NOOFDRIVE]; 

 
// TimeStamp
int             giSTOTimeStamp;

//For Storage Type 
oid storage_type_id[] = { 1, 3, 6, 1, 2, 1, 25, 2, 1, 1 };/* hrStorageOther */
int storage_type_len  =  sizeof(storage_type_id) / sizeof(storage_type_id[0]);


/************************ statics *************************/
// Variable for Index No.
static int      HRS_index;

/* 
 * hrStorage_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */

oid hrStorage_variables_oid[] = { 1,3,6,1,2,1,25,2 };

/* 
 * variable4 hrStorage_variables:
 *   this variable defines function callbacks and type return information 
 *   for the  mib section 
 */

struct variable4 hrStorage_variables[] = {
/*  magic number        , variable type , ro/rw , callback fn  , L, oidsuffix */
#define HRMEMORYSIZE		1
{HRMEMORYSIZE,  ASN_INTEGER,  RONLY ,  var_hrStorage, 1,  { 2 }},

#define HRSTORAGEINDEX		2
{HRSTORAGEINDEX,  ASN_INTEGER,  RONLY,   var_hrStorageTable, 3,  { 3, 1, 1 }},
#define HRSTORAGETYPE		3
{HRSTORAGETYPE,  ASN_OBJECT_ID,  RONLY,   var_hrStorageTable, 3,  { 3, 1, 2 }},
#define HRSTORAGEDESCR		4
{HRSTORAGEDESCR,  ASN_OCTET_STR,  RONLY,   var_hrStorageTable, 3,  { 3, 1, 3 }},
#define HRSTORAGEALLOCATIONUNITS		5
{HRSTORAGEALLOCATIONUNITS,  ASN_INTEGER,  RONLY,   var_hrStorageTable, 3,  { 3, 1, 4 }},
#define HRSTORAGESIZE		6
{HRSTORAGESIZE,  ASN_INTEGER,  RWRITE,  var_hrStorageTable, 3,  { 3, 1, 5 }},
#define HRSTORAGEUSED		7
{HRSTORAGEUSED,  ASN_INTEGER,  RONLY,   var_hrStorageTable, 3,  { 3, 1, 6 }},
#define HRSTORAGEALLOCATIONFAILURES		8
{HRSTORAGEALLOCATIONFAILURES,  ASN_COUNTER,  RONLY,   var_hrStorageTable, 3,  { 3, 1, 7 }},
};
/*    (L = length of the oidsuffix) */


/*****************************************************************************
 *
 * name             :   STOGet_Data
 * description      :   Data Collection & Index Limitation Module
 * input parameters :   _
 * return type      :   _
 * global variables :   giType,gaSectorPerCluster,gaBytesPerSector,gaNoOfFreeClust
 *						gaTotalClust,glDiskSize,giErr
 * calls            :   _
 */
/*  */
void STOGet_Data(void)
{	
	// For Disk Related Information
	static char         saStr[50];
	static unsigned int i,j,k,l,m,n;
    static int          siClust;
    static int          siSet = 0,siSetu = 0;
    char                aBuf[1024];
	char                s[50];
	char                d[50];
    int                 Block=0;
    DWORD               dwBuffer,dwCount;
	BOOL                bResult;
    int                 p=0;
    int                 ind=0;
    int rc=0;
    dwBuffer            = MAX_PATH;
 
    
    l = j = k = l = m = 0;

    memset(gcdisklabel,0, sizeof(char) * NOOFDRIVE * DRIVENAME);
    memset(gNrem,0, sizeof(char) * NOOFDRIVE * DRIVENAME);


    dwCount = GetLogicalDriveStrings(dwBuffer,aBuf);
    giSTOTimeStamp=GetTickCount();
    //girrc=Remotemount();

    for(i=0;i<dwCount;i++)
	{
		if(aBuf[i] != '\0' && aBuf[i] != '\\')
		{
			s[j] = aBuf[i];
			j++;
		}

		if(aBuf[i] == '\0')
		{ 
            s[j] = '\0';
            strcpy(saStr,"\\\\.\\");
            strcat(saStr,s); 
            strcpy(d,s);
            sprintf(gcdisklabel[p++],"%s",d);
            strcat(d,"\\");
            giType[indexlimit] = GetDriveType(d);
            SetErrorMode(SEM_FAILCRITICALERRORS);

            SetLastError(ERROR_SUCCESS);
            bResult = GetDiskFreeSpace(d,&gaSectorPerCluster[indexlimit],
                        &gaBytesPerSector[indexlimit],&gaNoOfFreeClust[indexlimit],
                        &gaTotalClust[indexlimit]);

            if(GetLastError() != ERROR_SUCCESS)
            {
                giErr=1;
                DEBUGMSGTL(("DISKSIZE","Inside Error==1\n"));
                SetLastError(ERROR_SUCCESS);
            }
            else
                giErr=0;
            switch(giType[indexlimit])
                {
            /*     case DRIVE_REMOTE:
                      for(rc=0;rc<girrc;rc++)
                      {
                         if(gNR[rc].lpLocalName)
                         if(!stricmp(gNR[rc].lpLocalName,s))
                          {
                                break;
                          }

                        }
                       sprintf(gNrem[p-1],"%s",gNR[rc].lpRemoteName);*/

                }
                   
                
           if(giErr==0)
          {
                SetErrorMode(0);
                glDiskSize = (gaSectorPerCluster[indexlimit]*gaTotalClust[indexlimit]);
                glDiskUsed = (gaSectorPerCluster[indexlimit]*gaNoOfFreeClust[indexlimit]);
               
                siClust = gaTotalClust[indexlimit] - gaNoOfFreeClust[indexlimit];
                DEBUGMSGTL(("DISKSIZE","siClust : %d\n",siClust));
                glDiskUsed = (gaSectorPerCluster[indexlimit]*siClust);
                if(glDiskSize >65530)
                    glDiskSize /= 1024;
            else
                siSet = 1;
             
            SetErrorMode(SEM_FAILCRITICALERRORS);


            if(glDiskUsed >65530) 
                glDiskUsed /= 1024;
            else
                siSetu = 2;

            glDiskSize *= gaBytesPerSector[indexlimit];
            glDiskUsed *= gaBytesPerSector[indexlimit];
            
            if(siSet == 1)  
                glDiskSize /= 1024;
            if(siSetu == 2)
            glDiskUsed /= 1024;
            siSet = 0;
            siSetu = 0;
            if(giErr == 1)
            {
                glDiskUsed = 0;
                glDiskSize = 0;     
            }
             if(gaBytesPerSector[indexlimit]!=0)
                    Block=(gaSectorPerCluster[indexlimit]*gaBytesPerSector[indexlimit]);
               else
                    Block=gaSectorPerCluster[indexlimit];
            DEBUGMSGTL(("DISKSIZE","disksize : %d\n",glDiskSize));
           if (Block>0)
          {
            gaTotalClust[indexlimit] = (glDiskSize/Block);
            DEBUGMSGTL(("DISKSIZE","disksize : %d\n",glDiskSize/Block));   
            gaUsedClus[indexlimit] = (glDiskUsed/Block);
            gaunits[indexlimit]=Block;
           }
           else
               {
                    gaTotalClust[indexlimit] = 0;
                     gaUsedClus[indexlimit] = 0;
                     gaunits[indexlimit]=0;
               }
		}
        else
             {
                    gaTotalClust[indexlimit] = 0;
                     gaUsedClus[indexlimit] = 0;
                     gaunits[indexlimit]=0;
               }
            strcpy(s,"\0");
            j=0;
            indexlimit++;
      }
    }
	//Getting Values for RAM - Physical Memory

	GlobalMemoryStatus(&gstMemStatus);
	gaTotalClust[indexlimit] = gstMemStatus.dwTotalPhys/1024;
    gdwPhyMem                = gstMemStatus.dwTotalPhys/1024;
	gaUsedClus[indexlimit] = gstMemStatus.dwTotalPhys/1024 - 
                               gstMemStatus.dwAvailPhys/1024;
	giType[indexlimit] = DRIVE_RAMDISK;

	//Getting Values for Virtual Memory
	indexlimit++;
	gaTotalClust[indexlimit] = gstMemStatus.dwTotalVirtual/1024;
	gaUsedClus[indexlimit] = gstMemStatus.dwTotalVirtual/1024 - 
                                gstMemStatus.dwAvailVirtual/1024;
	giType[indexlimit] = VIRTUAL;
	indexlimit++;
}



/*
 * header_hrstorage(...
 * Arguments:
 * vp     IN      - pointer to variable entry that points here
 * name    IN/OUT  - IN/name requested, OUT/name found
 * length  IN/OUT  - length of IN/OUT oid's 
 * exact   IN      - TRUE if an exact match was requested
 * var_len OUT     - length of variable or 0 if function returned
 * write_method
 * 
 */
int header_hrstorage(struct variable *vp,
              oid * name,
              size_t * length,
              int exact, size_t * var_len, WriteMethod ** write_method)
{
#define HRSTORAGE_ENTRY_NAME_LENGTH	11
    oid             newname[MAX_OID_LEN];
    int             storage_idx, LowIndex = -1;
    int             result;

    DEBUGMSGTL(("host/hr_storage", "var_hrstorage: "));
    DEBUGMSGOID(("host/hr_storage", name, *length));
    DEBUGMSG(("host/hr_storage", " %d\n", exact));

    memcpy((char *) newname, (char *) vp->name, vp->namelen * sizeof(oid));
    /*
     * Find "next" storage entry 
     */

    Init_HR_Storage();
    for (;;) {
        storage_idx = Get_Next_HR_Storage();
        if (storage_idx == -1)
            break;
        newname[HRSTORAGE_ENTRY_NAME_LENGTH] = storage_idx;
        result = snmp_oid_compare(name, *length, newname, vp->namelen + 1);
        if (exact && (result == 0)) {
            LowIndex = storage_idx;
            /*
             * Save storageessor status information 
             */
            break;
        }
        if ((!exact && (result < 0)) &&
            (LowIndex == -1 || storage_idx < LowIndex)) {
            LowIndex = storage_idx;
            /*
             * Save storageessor status information 
             */
#ifdef HRSTORAGE_MONOTONICALLY_INCREASING
            break;
#endif
        }
    }

    if (LowIndex == -1) {
        DEBUGMSGTL(("host/hr_storage", "... index out of range\n"));
        return (MATCH_FAILED);
    }

    memcpy((char *) name, (char *) newname,
           (vp->namelen + 1) * sizeof(oid));
    *length = vp->namelen + 1;
    *write_method = 0;
    *var_len = sizeof(long);    /* default to 'long' results */

    DEBUGMSGTL(("host/hr_storage", "... get storage stats "));
    DEBUGMSGOID(("host/hr_storage", name, *length));
    DEBUGMSG(("host/hr_storage", "\n"));

	indexgenerated=LowIndex;

    return LowIndex;
}



/** Initializes the hrStorageEntry module */
/** Initializes the hrStorage module */
void
init_hrStorage(void)
{

    DEBUGMSGTL(("hrStorage", "Initializing\n"));

    /* register ourselves with the agent to handle our mib tree */
    REGISTER_MIB("hrStorage", hrStorage_variables, variable4,
               hrStorage_variables_oid);

    /* place any other initialization junk you need here */
	// Used For the time Being call dynamically use timestamps
    
	STOGet_Data();
}

/*
 * var_hrStorage():
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
var_hrStorage(struct variable *vp, 
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

    if (header_hrstorage(vp,name,length,exact,var_len,write_method)
                                  == MATCH_FAILED )
    return NULL;

    /* 
   * this is where we do the value assignments for the mib results.
   */
    switch(vp->magic) {
    case HRMEMORYSIZE:
        *var_len = sizeof(gdwPhyMem);
        return (u_char*) &gdwPhyMem;
    default:
      ERROR_MSG("");
    }
    return NULL;
}


/*
 * var_hrStorageTable():
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
unsigned char * var_hrStorageTable(struct variable *vp, 
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
    int var_hrstorage;
    int storage_idx;
    int Block;
    // Get TimeStamp
	var_hrstorage = GetTickCount();

    if(abs(var_hrstorage-giSTOTimeStamp)>CACHETIME)  // Inorder to refresh.
        { 
            indexlimit=0;                
		    STOGet_Data(); 
		}

    storage_idx = header_hrstorage(vp, name, length, exact, var_len, write_method);
    if (storage_idx == MATCH_FAILED)
        return NULL;
          
	
 
    /* 
   * this is where we do the value assignments for the mib results.
   */
    switch(vp->magic) 
	{
		//HRSTORAGEALLOCATIONFAILURES:
			//VAR = VALUE;	/* XXX */
			//return (u_char*) &VAR;
		case HRSTORAGEINDEX:
			*var_len = sizeof(indexgenerated );	/* XXX */
			return (u_char*) &indexgenerated;
		case HRSTORAGETYPE:
			
			//To find the OID value of the corresponding giType.
			switch(giType[indexgenerated-1])
			{
			    case DRIVE_UNKNOWN:
				    storage_type_id[storage_type_len - 1] = 1;  
				    break;
			    case DRIVE_FIXED:
				    storage_type_id[storage_type_len - 1] = 4;  
				    break;
			    case DRIVE_CDROM:
				    storage_type_id[storage_type_len - 1] = 7;  
				    break;
			    case DRIVE_RAMDISK:
				    storage_type_id[storage_type_len - 1] = 8;  
				    break;
                /*case DRIVE_REMOTE:
				    storage_type_id[storage_type_len - 1] = 10;  
				    break;*/
			    case VIRTUAL:
				    storage_type_id[storage_type_len - 1] = 3;  
				    break;
			    case DRIVE_REMOVABLE:
				    storage_type_id[storage_type_len - 1] = 6;
				    break;
			    default:
                    storage_type_id[storage_type_len - 1] = 1;      /* Other */
                    break;
			}

			*var_len = sizeof(storage_type_id);
			return (u_char *) storage_type_id;
			
		case HRSTORAGESIZE:
			//*write_method = write_hrStorageSize;
            *var_len = sizeof(gaTotalClust[indexgenerated-1]);	/* XXX */
			return (u_char*) &gaTotalClust[indexgenerated-1];
		case HRSTORAGEDESCR:
			
			//To store the description in the string
			switch(giType[indexgenerated-1])
			{
			    case DRIVE_UNKNOWN:
                    strcpy(gaStorageDescription,gcdisklabel[indexgenerated-1]);
				    strcat(gaStorageDescription,"The drive cannot be determined.");
                    break;
			    case DRIVE_FIXED:
                    volumelabel(gcdisklabel[indexgenerated-1]);
                    strcpy(gaStorageDescription,gcdisklabel[indexgenerated-1]);
				    strcat(gaStorageDescription," - Fixed Disk");
				    break;
			    case DRIVE_CDROM:
                     volumelabel(gcdisklabel[indexgenerated-1]);
                    strcpy(gaStorageDescription,gcdisklabel[indexgenerated-1]);
				    strcat(gaStorageDescription," - CD-ROM drive.");
				    break;
			    case DRIVE_RAMDISK:
                    strcpy(gaStorageDescription,gcdisklabel[indexgenerated-1]);
				    strcat(gaStorageDescription,"RAM disk.");
				    break;
			    case VIRTUAL:
				    strcpy(gaStorageDescription,"Virtual Memory");
				    break;
			    case DRIVE_REMOVABLE:
                    volumelabel(gcdisklabel[indexgenerated-1]);
                    strcpy(gaStorageDescription,gcdisklabel[indexgenerated-1]);
				    strcat(gaStorageDescription," - Floppy Disk");
				    break;
                /*case DRIVE_REMOTE:
                    strcpy(gaStorageDescription,gcdisklabel[indexgenerated-1]);
                    strcat(gaStorageDescription,gNrem[indexgenerated-1]);
                    if (strlen(gNrem[indexgenerated-1]) > 0)
                    {
                        strcat(gaStorageDescription," ");
                    }
				    strcat(gaStorageDescription," - Network Disk");
                    gir++;
				    break;*/
                
			    default:
                    strcpy(gaStorageDescription,"The drive giType cannot be determined.");   
				    /* Other */
                    break;
			}
			*var_len = strlen(gaStorageDescription);
            return (u_char *) gaStorageDescription;
			
		case HRSTORAGEALLOCATIONUNITS:
	        *var_len= sizeof(gaunits[indexgenerated-1]);
             return (u_char*) &gaunits[indexgenerated-1];
		case HRSTORAGEUSED:
            *var_len = sizeof(gaUsedClus[indexgenerated-1]);	/* XXX */
			return (u_char*) &gaUsedClus[indexgenerated-1]; 
            /*UsedClus[indexgenerated-1] = gaTotalClust[indexgenerated-1] - 
				gaNoOfFreeClust[indexgenerated-1];*/
		default:
		  ERROR_MSG("");
    }
    return NULL;
}



int write_hrStorageSize(int      action,
            u_char   *var_val,
            u_char   var_val_type,
            size_t   var_val_len,
            u_char   *statP,
            oid      *name,
            size_t   name_len)
{
    long value;
    int size;

    switch ( action )
	{
        case RESERVE1:
          if (var_val_type != ASN_INTEGER) {
              fprintf(stderr, "write to hrStorageEntry not ASN_INTEGER\n");
              return SNMP_ERR_WRONGTYPE;
          }
          if (var_val_len > sizeof(long)) {
              fprintf(stderr,"write to hrStorageEntry: bad length\n");
              return SNMP_ERR_WRONGLENGTH;
          }
          break;

        case RESERVE2:
          size  = var_val_len;
          value = * (long *) var_val;

          break;

        case FREE:
             /* Release any resources that have been allocated */
          break;

        case ACTION:
             /*
              * The variable has been stored in 'value' for you to use,
              * and you have just been asked to do something with it.
              * Note that anything done here must be reversable in the UNDO case
              */
          break;

        case UNDO:
             /* Back out any changes made in the ACTION case */
          break;

        case COMMIT:
             /*
              * Things are working well, so it's now safe to make the change
              * permanently.  Make sure that anything done here can't fail!
              */
          break;
    }
    return SNMP_ERR_NOERROR;
}


/*********************
 *
 *  Internal implementation functions for Index Generation
 *
 *********************/
/*****************************************************************************
 *
 * name             :   Init_HR_Storage
 * description      :   Initializing the static Index variable HRS_index
 * input parameters :   _
 * return type      :   _
 * global variables :   _
 * calls            :   _
 */
void Init_HR_Storage(void)
{
    HRS_index = 1;
}

/*****************************************************************************
 *
 * name             :   Get_Next_HR_Storage
 * description      :   Increments the static Index variable HRS_index and 
 *						generates the index
 * input parameters :   _
 * return type      :   int
 * global variables :   _
 * calls            :   _
 */
int Get_Next_HR_Storage(void)
{
    
    if (HRS_index <= indexlimit)
        return (HRS_index++);
    else
        return -1;
}


/*****************************************************************************
 *
 * name             :   volumelabel
 * description      :   volume related information
 * input parameters :   _ char * string
 * global variables :   _
 * calls            :   _
 */
void volumelabel(char *drive)
{
  char name[MAX_PATH];
  DWORD nsize=sizeof(name);
  DWORD serial;
  DWORD component;
  DWORD sysflag;
  char filesystem[MAX_PATH];
  DWORD systemsize=sizeof(filesystem);
  BOOL success;
  char s[MAX_PATH];
  
  strcpy(s,drive);
  strcat(s,"\\");
  strcpy(drive,s);
  success=GetVolumeInformation(s,name,nsize,&serial,&component,
                             &sysflag,filesystem,systemsize);
  if(success)
  {
      
      strcat(drive," Label:");
      strcat(drive,name);
      strcat(drive,"  SerialNo: ");
      sprintf(s,"%x",serial);
      strcat(drive,s);
      
  }
}


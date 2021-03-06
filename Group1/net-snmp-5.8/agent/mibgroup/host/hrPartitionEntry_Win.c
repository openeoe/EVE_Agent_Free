/*****************************************************************************
 *
 * [hrPartitionEntry_Win.c]
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Name Karthikeyan Subramani
 *
 * description :
 *      [ This will give the entire partition Details of the system]
 *
 * TODO:
 *      [ Things to be improved ]
 *
 * CM Details:
 *
 * $Author: howonsuk $
 * $Log: hrPartitionEntry_Win.c,v $
 * Revision 1.2  2004/03/03 09:09:03  howonsuk
 * timecache계산에 절대값 사용
 *
 * Revision 1.1  2004/03/03 06:14:18  howonsuk
 * 로그모니터링: 정규식 추가
 * 프로세스모니터링 : 파라미터보여주기 및 파라미터에서 정규식으로 찾기 추가
 *s
 * $State: Exp $
 * $Id: hrPartitionEntry_Win.c,v 1.2 2004/03/03 09:09:03 howonsuk Exp $
 * $Name:  $
 *
 ****************************************************************************//*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf,v 1.3 2002/10/17 09:40:46 dts12 Exp $
 */


/********************************* includes *********************************/

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "hrPartitionEntry_Win.h"
#include "host_res_Win.h"

#include <stdlib.h>



/****************************** local macros ********************************
 
 * (Macros shareable with rest of the modules are present in .h file)
 */


#define HRP_MONOTONICALLY_INCREASING
#define BUFSIZE MAX_PATH        
#define FILESYSNAMEBUFSIZE MAX_PATH
#define NOOFDRIVE 50


/********************* local structure/union definitions ********************/
struct hard
{ int pcount;
  struct 
      {
         char partname[50];
         unsigned long size;
         int fsid;
      }partition[30];
}harddisk[100];

/********************* prototypes for local functions ***********************/
BOOL Get_DataPart(void);
//void Init_HR_DiskPart(void);
//int Get_Next_HR_DiskPart(void);
int
header_hrdiskpart(struct variable *vp,
              oid * name,
              size_t * length,
              int exact, size_t * var_len, WriteMethod ** write_method);
static void Init_HR_Partition(void);
static int Get_Next_HR_Partition(void);
extern void Init_HR_Disk(void);
extern int Get_Next_HR_Disk(void);
 

/************************* Globals within this module ***********************/


/* variable declaration that maintain the values*/

int giindexpart=0;
long glcoun;
int gipp=0;
int gif_c=0;   
int m;
static int      HRP_DiskIndex;	
extern long glrem[NOOFDRIVE];
extern int giHRP_index;
extern int Dcoun;
static int      HRP_index;
static int HR_f;
static giH_index=0;
static int gifcount;
int gifsindex=0;
int giremove=0;

/* 
 * hrPartitionEntry_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */

oid hrPartitionEntry_variables_oid[] = { 1,3,6,1,2,1,25,3,7,1 };

/* 
 * variable4 hrPartitionEntry_variables:
 *   this variable defines function callbacks and type return information 
 *   for the  mib section 
 */

struct variable4 hrPartitionEntry_variables[] = {
/*  magic number        , variable type , ro/rw , callback fn  , L, oidsuffix */
#define HRPARTITIONINDEX		1
{HRPARTITIONINDEX,  ASN_INTEGER,  RONLY ,  var_hrPartitionEntry, 1,  { 1 }},
#define HRPARTITIONLABEL		2
{HRPARTITIONLABEL,  ASN_OCTET_STR,  RONLY ,  var_hrPartitionEntry, 1,  { 2 }},
#define HRPARTITIONFSINDEX		3
{HRPARTITIONFSINDEX,  ASN_INTEGER,  RONLY ,  var_hrPartitionEntry, 1,  { 5 }},
#define HRPARTITIONID		4
{HRPARTITIONID,  ASN_OCTET_STR,  RONLY ,  var_hrPartitionEntry, 1,  { 3 }},
#define HRPARTITIONSIZE		5
{HRPARTITIONSIZE,  ASN_INTEGER,  RONLY ,  var_hrPartitionEntry, 1,  { 4 }},

};

/************************* externs  ***********************/

 

/*****************************************************************************
 *
 * name             :   [Get_DataPart.c]
 * description      :   [Collects infromation about partition]
 * input parameters :   [datatype arg1]  - No input parameters
 * output parameters:   [datatype var1]  - No output parameters
 * return type      :   void
 * global variables :   [gipp=1,glcoun,gif_c,glspace,gis]
 * calls            :   [var_hrPartitionEntry]
 */



/*********************************************************************************************
* Get logical drive strings is used to inorder to get all drives and this will be seperated and 
* passed as a argument to get drive type if it is FIXED then it will set the pa variable and
* this will tell that this is a drive partition ans get free space is used for partition size 
* calculation
***********************************************************************************************/


BOOL Get_DataPart(void)
{
    HANDLE fileHandle;
    STORAGE_DEVICE_NUMBER deviceInfo;
    DWORD bytesReturned;
  	char Buf[1024];
    unsigned int i;
    int j;
    int k;
    int l ;
    int id;
    int y;
    DWORD sectorpercluster;
    DWORD size=BUFSIZE;
    DWORD bytespersector;
    DWORD nooffreeclust;
    DWORD totalclust;
    DWORD buffer;
    DWORD count;
    ULONGLONG DiskSize;  
    UINT type;
    BOOL fr;
    char s[BUFSIZE];
    char d[BUFSIZE];
    char str[BUFSIZE];
    DWORD dwSysFlags;// flags that describe the file system
#ifdef WINNT
    char strResult[BUFSIZE];
#endif
    char FileSysNameBuf[FILESYSNAMEBUFSIZE];
    j=k=l=0;
    giremove=0;

    for(i=0;i<100;i++)
    {
     harddisk[i].pcount=0;
     for(y=0;y<30;y++)
     harddisk[i].partition[y].fsid=0;
    }
    gifcount=0;  
    gifsindex=0;
    buffer=BUFSIZE;
    gif_c=GetTickCount();
    count=GetLogicalDriveStrings(buffer,Buf);
    SetErrorMode(SEM_FAILCRITICALERRORS);
    for(i=0;i<count;i++)
    {
        if(Buf[i]!='\0' && Buf[i]!='\\')
        {
            s[j]=Buf[i];
            j++;
        }

        if(Buf[i]=='\0')
        {
            gifsindex++;
            s[j]='\0';
            strcpy(d,s);
            strcpy(str,"\\\\.\\");
            strcat(str,d);
            type=GetDriveType(d);
            switch(type)
            {
                case DRIVE_FIXED :
                     gipp=1;
                     glcoun++;
                     break;
            }

            strcat(d,"\\");
            // Supported by all Windows Version
            if(gipp==1)
            {	
#ifndef WINNT

                fileHandle = CreateFile(str, GENERIC_READ,
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                          NULL, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL, NULL);
                if (fileHandle == INVALID_HANDLE_VALUE) return FALSE;
                if (!DeviceIoControl(fileHandle, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL,
                       0, &deviceInfo, sizeof(deviceInfo), &bytesReturned,
                       NULL))
                {
                     DEBUGMSGTL(("Next","Returning false from Get_DataPart\n"));
                     for(y=0;y<100;y++)
                     harddisk[y].pcount=0;
                     return FALSE;

                }
                if (bytesReturned != sizeof(deviceInfo))
                return FALSE;
                CloseHandle(fileHandle);
                id=deviceInfo.DeviceNumber;
#endif

#ifdef WINNT
                QueryDosDevice(s,strResult,size)    
               for(l=0;strResult[l]!='P';l++);
               id=atoi(strResult+l-2);
#endif

                GetVolumeInformation( d, NULL, 0, NULL, NULL,
                 &dwSysFlags, FileSysNameBuf, 
                 FILESYSNAMEBUFSIZE);
 
               /* if(!strcmp(FileSysNameBuf,"NTFS"))
                harddisk[id].
                   partition[harddisk[id]
                       .pcount].fsid=2;
                   
                 
                if(!strcmp(FileSysNameBuf,"FAT"))
                harddisk[id].partition
                  [harddisk[id]
                           .pcount].fsid=1;
                if(!strcmp(FileSysNameBuf,"FAT32"))*/
                  harddisk[id].
                    partition[harddisk[id]
                       .pcount].fsid=gifsindex;                    //3
                 strcpy(harddisk[id]
                      .partition[harddisk[id].pcount].partname,d);


               
                
                fr=GetDiskFreeSpace(d,&sectorpercluster,
                     &bytespersector,&nooffreeclust,&totalclust);
                DiskSize=(sectorpercluster*totalclust);
                DiskSize/=1024;
                DiskSize*=bytespersector;
                harddisk[id].
                     partition[harddisk[id].pcount].size=DiskSize;
                harddisk[id].pcount+=1;
                gipp=0;
                //DEBUGMSGTL(("Harddisk","id : %d harddisk partition no %d\n",id,harddisk[id].pcount));

            }
            strcpy(s,"\0");
                        j=0;
        }

    } 	
    SetErrorMode(0);
    DEBUGMSGTL(("Harddisk","id : %d harddisk partition no %d\n",id,harddisk[id].pcount));
    return TRUE;
}

/* End of Get_DataPart.c  */



/*    (L = length of the oidsuffix) */


/** Initializes the hrPartitionEntry module */
void
init_hrPartitionEntry(void)
{   
    
    DEBUGMSGTL(("hrPartitionEntry", "Initializing\n"));

    /* register ourselves with the agent to handle our mib tree */
    REGISTER_MIB("hrPartitionEntry", hrPartitionEntry_variables, variable4,
               hrPartitionEntry_variables_oid);

    /* place any other initialization junk you need here */
	
	glcoun=0;
	Get_DataPart();
	
}


/*
 * header_partition(...
 * Arguments:
 * vp     IN      - pointer to variable entry that points here
 * name    IN/OUT  - IN/name requested, OUT/name found
 * length  IN/OUT  - length of IN/OUT oid's 
 * exact   IN      - TRUE if an exact match was requested
 * var_len OUT     - length of variable or 0 if function returned
 * write_method
 * 
 */

int
header_partition(struct variable *vp,
                   oid * name,
                   size_t * length,
                   int exact,
                   size_t * var_len, WriteMethod ** write_method)
{
#define HRPART_DISK_NAME_LENGTH		11
#define HRPART_ENTRY_NAME_LENGTH	12
    oid             newname[MAX_OID_LEN];
    int             part_idx, LowDiskIndex = -1, LowPartIndex = -1;
    int             result;

    DEBUGMSGTL(("host/hr_partition", "var_hrpartition: "));
    DEBUGMSGOID(("host/hr_partition", name, *length));
    DEBUGMSG(("host/hr_partition", " %d\n", exact));

    memcpy((char *) newname, (char *) vp->name,
           (int) vp->namelen * sizeof(oid));
    /*
     * Find "next" partition entry 
     */
    giH_index=0;
    part_idx=0;
    HRP_DiskIndex=0;
    giremove=0;
    Init_HR_Disk();
    result=0;
    DEBUGMSGTL(("Get_Next_HR_Partition"," glrem[0] =%d \n",glrem[0]));
    Init_HR_Partition();
    
     
    /*
     *  Find the "next" disk and partition entries.
     *  If we're in the middle of the table, then there's
     *     no point in examining earlier disks, so set the
     *     starting disk to that of the variable being queried.
     *
     *  If we've moved from one column of the table to another,
     *     then we need to start at the beginning again.
     *     (i.e. the 'compare' fails to match)
     *  Similarly if we're at the start of the table
     *     (i.e. *length is too short to be a full instance)
     */
    
    if ((snmp_oid_compare(vp->name, vp->namelen, name, vp->namelen) == 0)
        && (*length > HRPART_DISK_NAME_LENGTH)) {
        LowDiskIndex =
            (name[HRPART_DISK_NAME_LENGTH] &
             ((1 << HRDEV_TYPE_SHIFT)-1 ));
        while (HRP_DiskIndex < LowDiskIndex) {
                  DEBUGMSGTL(("Get_Next_HR_Partition"," Moves to Next disk \n"));
            Init_HR_Partition();        /* moves to next disk */
            if (HRP_DiskIndex == -1)
                {
                  DEBUGMSGTL(("Get_Next_HR_Partition"," Returning -1 \n"));
                 return (MATCH_FAILED);
                }
        }
    }

    for (;;) {
        part_idx = Get_Next_HR_Partition();
          DEBUGMSGTL(("Get_Next_HR_Partition"," Inside For\n"));
        if (part_idx == 0)
            break;
        newname[HRPART_DISK_NAME_LENGTH] =
            (HRDEV_DISK << HRDEV_TYPE_SHIFT) + HRP_DiskIndex;
        newname[HRPART_ENTRY_NAME_LENGTH] = part_idx;
        result = snmp_oid_compare(name, *length, newname, vp->namelen + 2);
        if (exact && (result == 0)) {
            //Save_HR_Partition(HRP_DiskIndex, part_idx);
            LowDiskIndex = HRP_DiskIndex;
            LowPartIndex = part_idx;
            break;
        }
        if (!exact && (result < 0)) {
            if (LowPartIndex == -1) {
                //Save_HR_Partition(HRP_DiskIndex, part_idx);
                LowDiskIndex = HRP_DiskIndex;
                LowPartIndex = part_idx;
            } else if (LowDiskIndex < HRP_DiskIndex)
                break;
            else if (part_idx < LowPartIndex) {
               // Save_HR_Partition(HRP_DiskIndex, part_idx);
                LowDiskIndex = HRP_DiskIndex;
                LowPartIndex = part_idx;
            }
#ifdef HRP_MONOTONICALLY_INCREASING
            break;
#endif
        }
    }

    if (LowPartIndex == -1) {
        DEBUGMSGTL(("host/hr_partition", "... index out of range\n"));
        return (MATCH_FAILED);
    }

    newname[HRPART_DISK_NAME_LENGTH] =
        (HRDEV_DISK << HRDEV_TYPE_SHIFT) + LowDiskIndex;
    newname[HRPART_ENTRY_NAME_LENGTH] = LowPartIndex;
    memcpy((char *) name, (char *) newname,
           ((int) vp->namelen + 2) * sizeof(oid));
    *length = vp->namelen + 2;
    *write_method = 0;
    *var_len = sizeof(long);    /* default to 'long' results */

    DEBUGMSGTL(("host/hr_partition", "... get partition stats "));
    DEBUGMSGOID(("host/hr_partition", name, *length));
    DEBUGMSG(("host/hr_partition", "\n"));
    giindexpart=LowPartIndex;
    DEBUGMSGTL(("Get_Next_HR_Partition"," LowPartIndex  %d\n",LowPartIndex));
    return LowPartIndex;
}

   /*********************
	 *
	 *  Internal implementation functions
	 *
	 *********************/



static void
Init_HR_Partition(void)
{   
    HRP_DiskIndex = Get_Next_HR_Disk();
    DEBUGMSGTL(("Get_Next_HR_Partition"," Init Partition DiskIndex1  called .....%d\n",HRP_DiskIndex));
    //HR_f=((HRDEV_DISK >>HRDEV_TYPE_SHIFT));+(giHRP_index-(gifcount+1)));
    HR_f=HRP_DiskIndex-1537;
     
    DEBUGMSGTL(("Get_Next_HR_Partition"," Init Partition  glrem[HR_f: %d] : %d\n",HR_f,glrem[HR_f]));

    if(HRP_DiskIndex < 0 )
    {
        DEBUGMSGTL(("Get_Next_HR_Partition","Inside HRP_DiskIndex < 0 "));
        HRP_index = -1;     
        return;
    }
    else if(glrem[HR_f]!= 3 ) //if(glrem[HR_f]==4 || glrem[HR_f]==5)
        {
        DEBUGMSGTL(("Get_Next_HR_Partition","glrem[HR_f]!= 3 \n"));
        giremove++;
        Init_HR_Partition();
        }

    /*while((glrem[HR_f]==4 && HRP_DiskIndex!=-1) || (glrem[HR_f]==5 && HRP_DiskIndex!=-1) )
   {
    HRP_DiskIndex=Get_Next_HR_Disk();   
               DEBUGMSGTL(("Get_Next_HR_Partition"," Init Partition HR_f  called .....%d\n",HRP_DiskIndex));
    //HR_f=((HRDEV_DISK >>HRDEV_TYPE_SHIFT));//+(giHRP_index-(gifcount+1)));
                   HR_f=HRP_DiskIndex-1537;
    DEBUGMSGTL(("Get_Next_HR_Partition"," Inside While\n"));
    DEBUGMSGTL(("Get_Next_HR_Partition"," Init Partition HR_f  called .....%d Dcoun  %d\n",HR_f,Dcoun));
    gifinish=0;
    if(HR_f > Dcoun)
    break;
   }*/

    DEBUGMSGTL(("Get_Next_HR_Partition"," Init Partition HRP_DiskIndex  called .....%d\n",HRP_DiskIndex));
    if (HRP_DiskIndex != -1538)
        HRP_DiskIndex &= ((1 << HRDEV_TYPE_SHIFT) - 1);
 

    HRP_index = -1;
}

 



 

/*
 * var_hrPartitionEntry():
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
var_hrPartitionEntry(struct variable *vp, 
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
	static int inded;
    int var_count;
    int valp=0;
    BOOL rr;
	
	var_count = GetTickCount();
    	if(abs(var_count-gif_c)>5000)  // Inorder to refresh.
            { 
                         glcoun=0;
		                 rr=Get_DataPart(); 
   
		}


    if (header_partition(vp,name,length,exact,var_len,write_method)
                                  == MATCH_FAILED )
    return NULL;
		
	  
           

    /* 
   * this is where we do the value assignments for the mib results.
   */
   
    switch(vp->magic) {
    case HRPARTITIONINDEX: // returning the value from here
	    *var_len=sizeof(giindexpart);
		return (u_char*) &giindexpart;
    case HRPARTITIONLABEL:
        *var_len=strlen(harddisk[HR_f-giremove].partition[giindexpart-1].partname);
		return (u_char*) harddisk[HR_f-giremove].partition[giindexpart-1].partname;
    case HRPARTITIONFSINDEX:
	    *var_len= sizeof(harddisk[HR_f-giremove].partition[giindexpart-1].fsid); 
		return (u_char*)&harddisk[HR_f-giremove].partition[giindexpart-1].fsid;
    case HRPARTITIONID:
         *var_len=sizeof(valp);	
        return (u_char*) &valp;
    case HRPARTITIONSIZE:
        *var_len=sizeof(harddisk[HR_f-giremove].partition[giindexpart-1].size);
		return (u_char*) &harddisk[HR_f-giremove].partition[giindexpart-1].size;
    default:
      ERROR_MSG("");
    }
    return NULL;
}






static int
Get_Next_HR_Partition(void)
{ 
    if (HRP_DiskIndex < 0 || HR_f < 0) {
        return 0;
    }
     
  //HRP_index++;

  if((harddisk[HR_f-giremove].pcount-1)<0)
  {
     glcoun=0;
     DEBUGMSGTL(("Next","Calling Get_DataPart"));
     harddisk[HR_f-1].pcount=0;
     Get_DataPart(); 
     
  }
 //exit(1);
 

 // while ( giH_index <=((harddisk[0].pcount-1))){                //HR_f-1
    if ( giH_index <((harddisk[HR_f-giremove].pcount))){               
 
           giH_index++;
           DEBUGMSGTL(("pcount","pcount %d\n, HR_f : %d\n",HR_f,harddisk[0].pcount,HR_f));
            
           return giH_index;
           // return ++HRP_index;
            //HRP_index++;
    }
    giH_index=0;
 //   HRP_index++;
    /*
     * Finished with this disk, try the next
     */

   
    DEBUGMSGTL(("Get_Next_HR_Partition"," Finished with this disk, try the next.. \n"));
    Init_HR_Partition();
    return (Get_Next_HR_Partition());
}

/*****************************************************************************
 *
 * [hrSWRunPerfEntry_Win.c]
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Name : Ashok Shankar
 *
 * description :
 *      [ Collecting information about the softwares running in the System ]
 *
 * TODO:
 *      [ Problem with glPerfCPU returning 0's ]
 *
 * CM Details:
 *
 * $Author: howonsuk $
 * $Log: hrSWRunPerfEntry_Win.c,v $
 * Revision 1.2  2004/03/03 09:09:04  howonsuk
 * timecache계산에 절대값 사용
 *
 * Revision 1.1  2004/03/03 06:14:20  howonsuk
 * 로그모니터링: 정규식 추가
 * 프로세스모니터링 : 파라미터보여주기 및 파라미터에서 정규식으로 찾기 추가
 *
 * Revision 1.13  2004/01/21 05:00:07  karthis
 * updated cpupercent and memory
 *
 * Revision 1.12  2004/01/14 13:52:40  saisk
 * gave relative path
 *
 * Revision 1.11  2004/01/09 16:56:07  ashok
 * Added collection for hrSWRunName with exe & hrSWRunPath
 *
 * Revision 1.10  2003/12/24 12:57:32  ashok
 * Added Correction for Avoiding Negative Values
 *
 * Revision 1.9  2003/12/08 13:20:41  ashok
 * Minor Changes
 *
 * Revision 1.8  2003/11/27 15:47:59  ashok
 * Minor Changes
 *
 * Revision 1.7  2003/11/25 07:11:55  ashok
 * Added new members for processdetails structure
 *
 * Revision 1.6  2003/11/13 09:59:38  ashok
 * Modified Data Collection Function for faster collection
 *
 * Revision 1.5  2003/11/11 13:43:04  ashok
 * Changes in ProcessDetails array
 *
 * Revision 1.4  2003/11/10 11:04:02  ashok
 * new version
 *
 * Revision 1.3  2003/11/10 06:51:41  ashok
 * Minor Changes
 *
 * Revision 1.2  2003/11/08 15:34:00  ashok
 * no message
 *
 * Revision 1.1  2003/10/31 12:08:44  ashok
 * Added into CVS by Ashok
 *
 * $State: Exp $
 * $Id: hrSWRunPerfEntry_Win.c,v 1.2 2004/03/03 09:09:04 howonsuk Exp $
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
#include "hrSWRunPerfEntry_Win.h"
#include "../NuriEnterprise/Win32/inc/NASCenter_EM_PDHCommon_Win.h"
//Headers for Windows API support
#include <windows.h>
#include <shlwapi.h>
#include <stdlib.h>
#include <process.h>
/****************************** local macros ********************************/
// For Index Genaration
#define HRSWRUNPERFTAB_MONOTONICALLY_INCREASING

// For Collecting UserName
#define NAMELEN         1024

#define MAXPROCESS      1000

#define CACHETIME       10000

#ifndef stProcessDetail
typedef struct _stProcessDetail
{
    TCHAR   gcInstance[MAX_PATH];
    char    gcUserName[NAMELEN];
    char    gcBinaryName[MAX_PATH];
    char    gcPathName[MAX_PATH];
    char    gcRunCLine[MAX_PATH];

    long    glProcessID;
    long	glPerfCPU;
    long    glPerfPCPU;
    long	glPerfMem;
    long    glParentID;
    long    glRunState;
    long    glProcessAge;
    long    glRunPriority;
    long    glRunSize;
    long    glRunRSS;
    long    glRunPageFaults;
    long    glRunHardPageFaults;
    long    glContextSwitches;
    long    glRunThreads;
    long    glRunHandles;
    long    glRunKernelTime;
    long    glRunUserTime;
    long    glRunStartTime;
    long    glRunUID;
    long    glRunUName;
    long    glRunDevice;
    long    glRunCLine;
    long    glRunFilePath;
    long    glRunCWD;
    long    glRunPercentRSS;
    long    glRunPercentSize;
} stProcessDetail;
#endif

/********************* local structure/union definitions ********************/

/********************* prototypes for local functions ***********************/
int     header_swrunperftab(struct variable*, oid*, size_t*, int,
    size_t*, WriteMethod**);
void    Init_HR_SWRunPerfTab(void);
int     Get_Next_HR_SWRunPerfTab(void);
void    HRSWRUNPERFGet_Data();

//added By Heyman 2005.01.10
//remade by HJ.Kwon. 2005.04.14.
//For Percent CPU Time
void    HRSWRUNPERFGet_CPUTime(int);
unsigned __stdcall update_hrSWRunPerf(void* pArguments);


/************************ globals within this module*************************/
//Time Stamps
int                     giHRSWRUNPERFTimeStamp = 0;

//Process Count
int                     giHRSWRUNPERFNoInstances;

//PDH related Inits
PDH_FMT_COUNTERVALUE    gstHRSWRUNPERFPDHFmtValue;
PDH_RAW_COUNTER         gstHRSWRUNPERFPDHRawValue;
long PCPU = 0;

// Thread Test
int loop = 1;

/************************ externs *************************/
extern int SWRUNGet_Data(void);
extern stProcessDetail stProcessDetails[MAXPROCESS];

/** added by HJ.Kwon. in 2005.04.13 */
extern      long getPsPercentCPU(int);

/************************ statics *************************/
//Index for var_entry Function
static int      HRSWRP_index;

/*
 * hrSWRunPerfEntry_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */

oid hrSWRunPerfEntry_variables_oid[] = { 1,3,6,1,2,1,25,5,1,1 };

/*
 * variable4 hrSWRunPerfEntry_variables:
 *   this variable defines function callbacks and type return information
 *   for the  mib section
 */

struct variable4 hrSWRunPerfEntry_variables[] = {
    /*  magic number        , variable type , ro/rw , callback fn  , L, oidsuffix */
    #define HRSWRUNPERFCPU		1
    {HRSWRUNPERFCPU,  ASN_INTEGER,  RONLY ,  var_hrSWRunPerfEntry, 1,  { 1 }},
    #define HRSWRUNPERFMEM		2
    {HRSWRUNPERFMEM,  ASN_INTEGER,  RONLY ,  var_hrSWRunPerfEntry, 1,  { 2 }},
    #define HRSWRUNPERFPCPU		3
    {HRSWRUNPERFPCPU,  ASN_INTEGER,  RONLY ,  var_hrSWRunPerfEntry, 1,  { 3 }},

};
/*    (L = length of the oidsuffix) */
/*****************************************************************************
 *
 * name             :   HRSWRUNPERFGet_Data
 * description      :   Function for Getting Software running in the System
 * input parameters :   int vp->magic
 * return type      :   void
 * global variables :   stProcessDetails, giHRSWRUNPERFNoInstances.
 * calls            :   SWRUNGet_Data
 */
 //*********

 //ThreadTest
 //int magic
void HRSWRUNPERFGet_Data()
{
    // TimeStamp for Calling SWRUN
    static int         SWRUNimeStamp = 0;
    int         i;

    //Get the TimeStamp during Entry
    giHRSWRUNPERFTimeStamp = GetTickCount();

    DEBUGMSGTL(("NURI", " Inside HRSWRUNPERFGet_Data\n"));

    // Decide Whether to call SWRUNGet_Data or not.
    if (abs(giHRSWRUNPERFTimeStamp - SWRUNimeStamp) > CACHETIME)
    {
        // Call HRSWRUN for Basic variables
        giHRSWRUNPERFNoInstances = SWRUNGet_Data();
        SWRUNimeStamp = GetTickCount();
    }

    //modified By Heyman 2005.01.10
    for (i = 0; i < giHRSWRUNPERFNoInstances; i++)
    {
        HRSWRUNPERFGet_CPUTime(i);
    }
}

/*****************************************************************************
 *
 * name             :   HRSWRUNPERFGet_CPUTime
 * description      :   Function for Getting Software running in the System
 * input parameters :   int giHRSWRUNPERFNoInstances
 * return type      :   void
 * global variables :   stProcessDetails, giHRSWRUNPERFNoInstances.
 *****************************************************************************/
void HRSWRUNPERFGet_CPUTime(int num)
{
    //Init Vars for process count
    char        pathName[MAX_PATH];

    /** modified by HJ.Kwon. 2005.04.14.
    long		firstTotalCPUTime = 0;
    long		secondTotalCPUTime = 0;
    long		secondCPUTime[MAXPROCESS];
    long		diffTotalCPUTime = 0;
    **/

    // For UserName
    DWORD       dwSize = NAMELEN;

    /** modified by HJ.Kwon. 2005.04.14
    //added By Heyman 2005.01.10
    // Fisrt Total CPU Time Get
    strcpy(pathName,"\\Process(");
    strcat(pathName,"_Total");
    strcat(pathName,")\\");

    strcat(pathName,"% Processor Time");
    COMPDH_Collection_RAW(pathName, &gstHRSWRUNPERFPDHRawValue);
    firstTotalCPUTime = (gstHRSWRUNPERFPDHRawValue.FirstValue/100000);
    **/

    /**********Performance Counter Monitoring**********/
    // Common Call for all counters
    strcpy(pathName, "\\Process(");
    strcat(pathName, stProcessDetails[num].gcInstance);
    strcat(pathName, ")\\");
    strcat(pathName, "Working Set");

    // Collect Data using common PDH Function
    COMPDH_Collection(pathName, &gstHRSWRUNPERFPDHFmtValue);
    stProcessDetails[num].glPerfMem = gstHRSWRUNPERFPDHFmtValue.
        longValue / 1024;


    strcpy(pathName, "\\Process(");
    strcat(pathName, stProcessDetails[num].gcInstance);
    strcat(pathName, ")\\");

    strcat(pathName, "% Processor Time");
    COMPDH_Collection_RAW(pathName, &gstHRSWRUNPERFPDHRawValue);
    stProcessDetails[num].glPerfCPU = (long)(gstHRSWRUNPERFPDHRawValue.FirstValue / 100000);

    //added by HJ.Kwon. in 2005.04.14.
    stProcessDetails[num].glPerfPCPU = getPsPercentCPU(stProcessDetails[num].glProcessID);
    if (stProcessDetails[num].glPerfPCPU < 0)
        stProcessDetails[num].glPerfPCPU = 0;
    else if (stProcessDetails[num].glPerfPCPU > 10000)
        stProcessDetails[num].glPerfPCPU = 10000;


#ifdef DUMMY_Jan20
    switch (magic)
    {
    case HRSWRUNPERFCPU:
        /****** For hrSWRunPerfCPU collect % Processor Time*/
        /****** To collect Runing Kernel Time *****/
        strcat(pathName, "% Privileged Time");

        // Collect Data using common PDH Function
        COMPDH_Collection_RAW(pathName, &gstHRSWRUNPERFPDHRawValue);
        stProcessDetails[num].glRunKernelTime = gstHRSWRUNPERFPDHRawValue.
            FirstValue / (20 * 60 * 60);
        StrTrim(pathName, "% Privileged Time");

        /****** To collect Runing User Time *****/
        strcat(pathName, "% User Time");

        COMPDH_Collection_RAW(pathName, &gstHRSWRUNPERFPDHRawValue);
        stProcessDetails[num].glRunUserTime = gstHRSWRUNPERFPDHRawValue.
            FirstValue / (20 * 60 * 60);
        StrTrim(pathName, "% User Time");

        /****** To collect Process Age*/
        strcat(pathName, "Elapsed Time");
        // Collect Data using common PDH Function
        COMPDH_Collection(pathName, &gstHRSWRUNPERFPDHFmtValue);
        stProcessDetails[num].glProcessAge = gstHRSWRUNPERFPDHFmtValue.
            longValue * 100;
        /*(stProcessDetails[num].glProcessAge==0)
            stProcessDetails[num].glPerfCPU = 0;

        else
            stProcessDetails[num].glPerfCPU = (stProcessDetails[num].
            glRunKernelTime + stProcessDetails[num].glRunUserTime)*100/
            stProcessDetails[num].glProcessAge;

        if(stProcessDetails[num].glPerfCPU > 99)
            stProcessDetails[num].glPerfCPU = 99;
        else if(stProcessDetails[num].glPerfCPU < 0)
            stProcessDetails[num].glPerfCPU = 0;
        break;*/

        //case HRSWRUNPERFMEM:
            /****** hrSWRunPerfMem collect Working Set*/


        break;

    default:
        break;
    }
#endif

}

/*
 * header_swrunperftab(...
 * Arguments:
 * vp     IN      - pointer to variable entry that points here
 * name    IN/OUT  - IN/name requested, OUT/name found
 * length  IN/OUT  - length of IN/OUT oid's
 * exact   IN      - TRUE if an exact match was requested
 * var_len OUT     - length of variable or 0 if function returned
 * write_method
 *
 */
int header_swrunperftab(struct variable* vp,
    oid* name,
    size_t* length,
    int exact, size_t* var_len, WriteMethod** write_method)
{

#define HRSWRUNENTRY_NAME_LENGTH	11
    oid             newname[MAX_OID_LEN];
    int             swrunperftab_idx, LowIndex = -1;
    int             result;



    DEBUGMSGTL(("host/hr_swrunperftab", "var_swrunperftab: "));
    DEBUGMSGOID(("host/hr_swrunperftab", name, *length));
    DEBUGMSG(("host/hr_swrunperftab", " %d\n", exact));

    memcpy((char*)newname, (char*)vp->name, vp->namelen * sizeof(oid));
    /*
     * Find "next" swrunperftab entry
     */

    Init_HR_SWRunPerfTab();

    for (;;)
    {
        swrunperftab_idx = Get_Next_HR_SWRunPerfTab();
        if (swrunperftab_idx == -1)
            break;
        newname[HRSWRUNENTRY_NAME_LENGTH] = swrunperftab_idx;
        result = snmp_oid_compare(name, *length, newname, vp->namelen + 1);
        if (exact && (result == 0))
        {
            LowIndex = swrunperftab_idx;

            /*
             * Save swrunperftabessor status information
             */
            break;
        }
        if ((!exact && (result < 0)) &&
            (LowIndex == -1 || swrunperftab_idx < LowIndex))
        {
            LowIndex = swrunperftab_idx;
            /*
             * Save swrunperftabessor status information
             */

#ifdef HRSWRUNPERFTAB_MONOTONICALLY_INCREASING
            break;
#endif
        }
    }

    if (LowIndex == -1)
    {
        DEBUGMSGTL(("host/hr_swrunperftab", "... index out of range\n"));
        return (MATCH_FAILED);
    }

    memcpy((char*)name, (char*)newname,
        (vp->namelen + 1) * sizeof(oid));
    *length = vp->namelen + 1;
    *write_method = 0;
    *var_len = sizeof(long);    /* default to 'long' results */

    DEBUGMSGTL(("host/hr_swrunperftab", "... get swrunperftab stats "));
    DEBUGMSGOID(("host/hr_swrunperftab", name, *length));
    DEBUGMSG(("host/hr_swrunperftab", "\n"));


    return LowIndex;
}

/** Initializes the hrSWRunPerfEntry module */
void init_hrSWRunPerfEntry(void)
{
    HANDLE hThread;
    unsigned threadID;

    DEBUGMSGTL(("hrSWRunPerfEntry", "Initializing\n"));

    /* register ourselves with the agent to handle our mib tree */
    REGISTER_MIB("hrSWRunPerfEntry", hrSWRunPerfEntry_variables, variable4,
        hrSWRunPerfEntry_variables_oid);

    /* place any other initialization junk you need here */
    hThread = (HANDLE)_beginthreadex(NULL, 0, &update_hrSWRunPerf, NULL, 0, &threadID);
    CloseHandle(hThread);
}


void uninit_hrSWRunPerfEntry(void) {
    loop = 0;
}

unsigned __stdcall update_hrSWRunPerf(void* pArguments) {
    while (loop) {
        DEBUGMSGTL(("host/hr_swrunperftab", "update_hrSWRunPerf Thread Test\n"));
        HRSWRUNPERFGet_Data();
        Sleep(CACHETIME);
    }

    _endthreadex(0);
    return 0;
}

/*
 * var_hrSWRunPerfEntry():
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
unsigned char* var_hrSWRunPerfEntry(struct variable* vp,
    oid* name,
    size_t* length,
    int     exact,
    size_t* var_len,
    WriteMethod** write_method)
{
    /* variables we may use later */
    static long long_ret;
    static u_long ulong_ret;
    static unsigned char string[SPRINT_MAX_LEN];
    static oid objid[MAX_OID_LEN];
    static struct counter64 c64;

    // For Index generation.
    int swrunperftab_idx;

    //Get the TimeStamp during Entry
    int var_swrunperftab = 0;

    //Decide whether to call HRSWRUNPERFGet_Data( )
    var_swrunperftab = GetTickCount();
    swrunperftab_idx = header_swrunperftab(vp, name, length, exact, var_len,
        write_method);

    if (swrunperftab_idx == MATCH_FAILED)
        return NULL;

    /*
   * this is where we do the value assignments for the mib results.
   */
    switch (vp->magic) {
    case HRSWRUNPERFCPU:
        *var_len = sizeof(stProcessDetails[HRSWRP_index].glPerfCPU);
        return (u_char*)&stProcessDetails[HRSWRP_index].glPerfCPU;

    case HRSWRUNPERFMEM:
        *var_len = sizeof(stProcessDetails[HRSWRP_index].glPerfMem);
        return (u_char*)&stProcessDetails[HRSWRP_index].glPerfMem;

    case HRSWRUNPERFPCPU:
        *var_len = sizeof(stProcessDetails[HRSWRP_index].glPerfPCPU);
        return (u_char*)&stProcessDetails[HRSWRP_index].glPerfPCPU;

    default:
        ERROR_MSG("");
    }
    return NULL;
}



/*********************
 *
 *  Internal implementation functions for Index Generation
 *
 *********************/
 /*****************************************************************************
  *
  * name             :   Init_HR_SWRunPerfTab
  * description      :   Initializing the static Index variable HRSWRP_index
  * input parameters :   _
  * return type      :   _
  * global variables :   _
  * calls            :   _
  */
void Init_HR_SWRunPerfTab(void)
{
    HRSWRP_index = -1;

}

/*****************************************************************************
 *
 * name             :   Get_Next_HR_SWRunPerfTab
 * description      :   Increments the static Index variable HRSWRP_index and
 *						generates the index
 * input parameters :   _
 * return type      :   int
 * global variables :   _
 * calls            :   _
 */
int Get_Next_HR_SWRunPerfTab(void)
{
    if (HRSWRP_index < giHRSWRUNPERFNoInstances)
    {
        HRSWRP_index++;
        return (stProcessDetails[HRSWRP_index].glProcessID);
    }
    else
        return -1;
}

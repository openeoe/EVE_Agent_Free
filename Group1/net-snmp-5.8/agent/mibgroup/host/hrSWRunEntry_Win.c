/*****************************************************************************
 *
 * [hrSWRunEntry_Win.c]
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
 *      [ Should handle multiple instances ]
 *
 * CM Details:
 *
 * $Author: howonsuk $
 * $Log: hrSWRunEntry_Win.c,v $
 * Revision 1.2  2004/03/03 09:09:04  howonsuk
 * timecache계산에 절대값 사용
 *
 * Revision 1.1  2004/03/03 06:14:19  howonsuk
 * 로그모니터링: 정규식 추가
 * 프로세스모니터링 : 파라미터보여주기 및 파라미터에서 정규식으로 찾기 추가
 *
 * Revision 1.25  2004/02/06 05:07:32  karthis
 * hr os index ? hrmemorysize issues fixed
 *
 * Revision 1.24  2004/01/28 05:44:13  karthis
 * fixed XP Problem enum process
 *
 * Revision 1.23  2004/01/21 05:21:25  saisk
 * "strcmp" for "exe" and "EXE" cahnged to "stricmp"
 *
 * Revision 1.22  2004/01/20 10:44:16  saisk
 * Moved CloseHandle from out side for loop to end of for loop in "SWRUNGet_Data()"
 *
 * Revision 1.21  2004/01/14 13:52:40  saisk
 * gave relative path
 *
 * Revision 1.20  2004/01/10 09:48:57  saisk
 * Modified : removed process name from file path
 *
 * Revision 1.19  2004/01/09 16:55:39  ashok
 * Added collection for hrSWRunName with exe & hrSWRunPath
 *
 * Revision 1.18  2004/01/08 10:52:49  saisk
 * Modified to support SWOS Index
 *
 * Revision 1.17  2003/12/24 05:50:17  saisk
 * Removed "free " done for array
 *
 * Revision 1.16  2003/12/02 16:31:06  ashok
 * Minor Changes
 *
 * Revision 1.15  2003/11/27 15:47:52  ashok
 * Minor Changes
 *
 * Revision 1.14  2003/11/27 13:43:35  ashok
 * Minor Changes
 *
 * Revision 1.13  2003/11/27 13:19:19  ashok
 * Minor Changes
 *
 * Revision 1.12  2003/11/25 07:11:43  ashok
 * Added new members for processdetails structure
 *
 * Revision 1.11  2003/11/20 06:22:24  ashok
 * Minor Corrections
 *
 * Revision 1.10  2003/11/18 05:54:51  ashok
 * Minor Corrections
 *
 * Revision 1.9  2003/11/13 11:14:00  ashok
 * Modified Data Collection Function for faster collection
 *
 * Revision 1.8  2003/11/13 09:59:25  ashok
 * Modified Data Collection Function for faster collection
 *
 * Revision 1.7  2003/11/11 13:42:56  ashok
 * Changes in ProcessDetails array
 *
 * Revision 1.6  2003/11/10 11:04:15  ashok
 * new version
 *
 * Revision 1.5  2003/11/10 06:51:53  ashok
 * Minor Changes
 *
 * Revision 1.4  2003/11/08 15:34:10  ashok
 * no message
 *
 * Revision 1.3  2003/11/05 09:36:57  ashok
 * Small Modifications made Ashok
 *
 * Revision 1.2  2003/11/01 12:15:26  ashok
 * Minor Changes done
 *
 * Revision 1.1  2003/10/31 12:08:44  ashok
 * Added into CVS by Ashok
 *
 * $State: Exp $
 * $Id: hrSWRunEntry_Win.c,v 1.2 2004/03/03 09:09:04 howonsuk Exp $
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
#include <pdh.h>
#include <psapi.h>
#include "hrSWRunEntry_Win.h"
#include "../NuriEnterprise/Win32/inc/NASCenter_EM_PDHCommon_Win.h"
#include <stdlib.h> 
#include <process.h>

/****************************** local macros ********************************/
// For Index Genaration
#define HRSWRUNTAB_MONOTONICALLY_INCREASING

// For Collecting UserName
#define NAMELEN         1024

#define MAXPROCESS      1000

#define CACHETIME       5000 

#define PDHBUFFLENGTH   4096

/********************* local structure/union definitions ********************/
typedef struct _stProcessDetail
{
    TCHAR   gcInstance [MAX_PATH];
    char    gcUserName [NAMELEN];
    char    gcBinaryName[MAX_PATH];
    char    gcPathName  [MAX_PATH];
    char	gcRunCLine[MAX_PATH];

    long    glProcessID;
    long	glPerfCPU;
	long    glPerfPCPU;
    long	glPerfMem;
    long    glParentID;
    long    glRunState;
    long    glProcessAge;
    long    glRunPriority;
    long    glRunRSS;
    long    glRunSize;
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

stProcessDetail stProcessDetails[MAXPROCESS];
stProcessDetail temp;

/********************* prototypes for local functions ***********************/
int header_swruntab(struct variable*, oid*, size_t*, int,
    size_t*, WriteMethod**);

//Function Prototypes for Index Generation
void Init_HR_SWRunTab(void);
int Get_Next_HR_SWRunTab(void);

//Funtion for populating the stProcessDetails array
int SWRUNGet_Data(void);
extern char gcaFilePath[MAX_PATH];
extern void RemoveProcessName(char* inStr);
unsigned char*
var_hrSWRun(struct variable* vp,
    oid* name,
    size_t* length,
    int     exact,
    size_t* var_len,
    WriteMethod** write_method);


unsigned __stdcall update_hrSWRun(void* pArguments);
/************************ globals within this module*************************/
//PDH related Inits
PDH_FMT_COUNTERVALUE gstHRSWRUNPDHFmtValue;

//Time Stamps
int     giSWRUNTimeStamp;

//Constants for return 
char    gaEmpty[] = " ";
int     giStatus = 1;
int     giId = 0;
int     giSWType = 4;

//Global for returning Table Index 
int     swruntab_idx;

//Count for number of processes
int     giHRSWRUNNoInstances;


/************************ externs *************************/


/************************ statics *************************/



// For Index Generation
int HRSWRUN_OS_index = 0;
static int HRSWR_index;

/*
 * hrSWRunEntry_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */

oid hrSWRunEntry_variables_oid[] = { 1,3,6,1,2,1,25,4 };

/*
 * variable4 hrSWRunEntry_variables:
 *   this variable defines function callbacks and type return information
 *   for the  mib section
 */

struct variable4 hrSWRunEntry_variables[] = {
    /*  magic number        , variable type , ro/rw , callback fn  , L, oidsuffix */
    #define HRSWRUN_OSINDEX		1
    {HRSWRUN_OSINDEX,  ASN_INTEGER,  RONLY ,  var_hrSWRun,1,  { 1 }},
    #define HRSWRUNINDEX		2
    {HRSWRUNINDEX,  ASN_INTEGER,  RONLY ,  var_hrSWRunEntry, 3,  { 2,1,1 }},
    #define HRSWRUNPARAMETERS		3
    {HRSWRUNPARAMETERS,  ASN_OCTET_STR,  RONLY ,  var_hrSWRunEntry, 3,  { 2,1,5 }},
    #define HRSWRUNTYPE		4
    {HRSWRUNTYPE,  ASN_INTEGER,  RONLY ,  var_hrSWRunEntry, 3,  { 2,1,6 }},
    #define HRSWRUNNAME		5
    {HRSWRUNNAME,  ASN_OCTET_STR,  RONLY ,  var_hrSWRunEntry, 3,  { 2,1,2 }},
    #define HRSWRUNPATH		6
    {HRSWRUNPATH,  ASN_OCTET_STR,  RONLY ,  var_hrSWRunEntry, 3,  { 2,1,4 }},
    #define HRSWRUNID		7
    {HRSWRUNID,  ASN_OBJECT_ID,  RONLY ,  var_hrSWRunEntry, 3,  { 2,1,3 }},
    #define HRSWRUNSTATUS		8
    {HRSWRUNSTATUS,  ASN_INTEGER,  RWRITE,  var_hrSWRunEntry, 3,  { 2,1,7 }},

};
/*    (L = length of the oidsuffix) */

/*****************************************************************************
 * 정렬
 */
void Swap(stProcessDetail arr[], int a, int b) 
{
    stProcessDetail temp = arr[a];
    arr[a] = arr[b];
    arr[b] = temp;
}
int Partition(stProcessDetail arr[], int left, int right)
{
    stProcessDetail pivot = arr[left];
    int low = left + 1;
    int high = right;

    while (low <= high) 
    {
        while (low <= right && pivot.glProcessID >= arr[low].glProcessID) 
        {
            low++;
        }
        while (high >= (left + 1) && pivot.glProcessID <= arr[high].glProcessID)
        {
            high--;
        }
        if (low <= high)
        {
            Swap(arr, low, high);
        }
    }
    Swap(arr, left, high);
    return high;

}


void QuickSort(stProcessDetail arr[], int left, int right)
{
    if (left <= right)
    {
        int pivot = Partition(arr, left, right);
        QuickSort(arr, left, pivot - 1);
        QuickSort(arr, pivot + 1, right);
    }
}

/*****************************************************************************
 *
 * name             :   SWRUNGet_Data
 * description      :   Function for Getting Software running in the System
 * input parameters :   _
 * return type      :   int
 * global variables :   stProcessDetails,giSWRUNTimeStamp
 * calls            :   _
 */
 //*********

int SWRUNGet_Data(void)
{
    int         row = 0;
    int         col = 0;
    int         i, j;
    int         g = 0;
    int         count[MAXPROCESS];
    int         flag[MAXPROCESS];

    char        pathName[MAX_PATH];
    char        szCommandLine[MAX_PATH] = { 0, };
    char* ttemp;

    PDH_STATUS  iStatus = 0;

    TCHAR       mszCounterList[PDHBUFFLENGTH];
    TCHAR       mszInstanceList[PDHBUFFLENGTH];
    TCHAR       mszObjectList[PDHBUFFLENGTH];
    TCHAR       buff[MAX_PATH];

    DWORD       cchCounterList = PDHBUFFLENGTH;
    DWORD       cchInstanceList = PDHBUFFLENGTH;
    DWORD       cchBufferLength = PDHBUFFLENGTH;

    HMODULE     hMods[1024];
    HANDLE      hProcess;
    DWORD64       cbNeeded;
    char        binaryname[MAX_PATH];
    char        szModName[MAX_PATH];

    //Get the TimeStamp during Entry
    giSWRUNTimeStamp = GetTickCount();

    memset(mszInstanceList, 0, (sizeof(TCHAR) * PDHBUFFLENGTH));
    memset(mszCounterList, 0, (sizeof(TCHAR) * PDHBUFFLENGTH));
    memset(mszObjectList, 0, (sizeof(TCHAR) * PDHBUFFLENGTH));

    /*For commented and changed to memset by Sai on 20.1.2004*/
    memset(count, 0, sizeof(int) * MAXPROCESS);
    memset(flag, 0, sizeof(int) * MAXPROCESS);
    /*
    for(i=0;i<MAXPROCESS;i++)
        count[i]=0;
    for(i=0;i<MAXPROCESS;i++)
        flag[i]=0;
     */

    giHRSWRUNNoInstances = 0;

    /* To refresh the list of performance objects. So that new instances are
       captured */
    iStatus = PdhEnumObjects(NULL, NULL, mszObjectList, &cchBufferLength,
        PERF_DETAIL_WIZARD, TRUE);

    //For Collecting Process Instances From System
    iStatus = PdhEnumObjectItems(NULL, NULL, "Process", mszCounterList,
        &cchCounterList, mszInstanceList, &cchInstanceList,
        PERF_DETAIL_WIZARD, 0);


    //Formatting the Arrays
    for (i = 0, j = 1; (mszInstanceList[i] != '\0' || mszInstanceList[j] != '\0'); i++, j++)
    {
        //To Avoid adding Null at the begining of new string
        if (mszInstanceList[i] != '\0')
        {
            stProcessDetails[row].gcInstance[col] = mszInstanceList[i];
            col++;
        }

        //To Complete a Row
        if (mszInstanceList[j] == '\0')
        {
            stProcessDetails[row].gcInstance[col] = '\0';
            col = 0;
            row++;
        }

    }
    giHRSWRUNNoInstances = row - 1;

    //For adding # entry for multiple entries
    for (i = 0; i < giHRSWRUNNoInstances; i++)
    {
        if (stricmp(stProcessDetails[i].gcInstance, "System") == 0)
        {
            HRSWRUN_OS_index = stProcessDetails[i].glProcessID;
        }

        for (j = 0; j < i; j++)
        {
            if (((strcmp(stProcessDetails[i].gcInstance, stProcessDetails[j].
                gcInstance) == 0) && flag[j] == 0))
            {
                count[i] = count[j] + 1;
                flag[j] = 1;
            }
        }
    }

    for (i = 0; i < giHRSWRUNNoInstances; i++)
    {
        if (flag[i] == 1)
        {
            sprintf(buff, "%s%c%d", stProcessDetails[i].gcInstance, '#',
                count[i] + 1);
            strcpy(stProcessDetails[i].gcInstance, buff);
            strcpy(buff, "");
        }

    }
    //End of adding #


    //Collecting Actual Values
    for (i = 0; i < giHRSWRUNNoInstances; i++)
    {
        /**********Performance Counter Monitoring**********/
        /****** For HRSWRunIdex collect Proccess ID*/

        strcpy(pathName, "\\");
        strcat(pathName, "Process");
        strcat(pathName, "(");
        strcat(pathName, stProcessDetails[i].gcInstance);
        strcat(pathName, ")");
        strcat(pathName, "\\");
        strcat(pathName, "ID Process");

        // Collect Data using common PDH Function
        COMPDH_Collection(pathName, &gstHRSWRUNPDHFmtValue);
        stProcessDetails[i].glProcessID = gstHRSWRUNPDHFmtValue.longValue;

    }

    // //Sorting the stProcessDetails structure array.
    // for(i=0;i<giHRSWRUNNoInstances;i++)
    // {
    // 	for(j=0;j<i;j++)
    // 	{
    // 		if(stProcessDetails[i].glProcessID < stProcessDetails[j].
    //             glProcessID)
    // 		{
    // 			temp = stProcessDetails[i];
    // 			stProcessDetails[i] = stProcessDetails[j];
    // 			stProcessDetails[j] = temp;
    // 		}
    // 	}
    // }

    // For Collecting Binary name & Run Path.
    QuickSort(stProcessDetails, 0, giHRSWRUNNoInstances-1);
    for (i = 0; i < giHRSWRUNNoInstances; i++)
    {
        memset(stProcessDetails[i].gcBinaryName, '\0', MAX_PATH);
        // Get a list of all the modules in this process.
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | SYNCHRONIZE,
                                        FALSE, stProcessDetails[i].glProcessID);
        //hProcess = GetProcessHandleWithEnoughRights(stProcessDetails[i].glProcessID, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ);
        DEBUGMSGTL(("host/hr_swruntab", "GetProcessHandleWithEnoughRights(pid : %ld,..., %d)\n", stProcessDetails[i].glProcessID, GetLastError()));
        if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
        {
            for (j = 0; j < (cbNeeded / sizeof(HMODULE)); j++)
            {
                memset(szModName, '\0', MAX_PATH);

                // Get the full path to the module's file.
                if (GetModuleFileNameEx(hProcess, hMods[j], szModName,
                    sizeof(szModName)))
                {
                    // Store thea path for the Process with extension .exe only
                    /*  if ((strcmp(&(szModName[strlen(szModName)-3]),"exe") == 0)
                        ||(strcmp(&(szModName[strlen(szModName)-3]),"EXE") == 0))
                        */

                    if (stricmp(&(szModName[strlen(szModName) - 3]), "exe") == 0)
                    {
                        GetModuleBaseName(hProcess, hMods[j], binaryname, sizeof(binaryname));
                        memset(stProcessDetails[i].gcPathName, '\0', MAX_PATH);
                        for (g = 0; g < strlen(szModName); g++)
                        {
                            if (szModName[g] == 63)
                            {
                                g = 1;
                                strtok(szModName, "?");
                                ttemp = strtok(NULL, "?");
                                ttemp++;
                                strcpy(stProcessDetails[i].gcPathName, ttemp);
                                break;
                            }
                        }
                        if (g != 1)
                            strcpy(stProcessDetails[i].gcPathName, szModName);
                        strcpy(stProcessDetails[i].gcBinaryName, binaryname);

                        strcpy(stProcessDetails[i].gcRunCLine, "unknown");

                        //Added by Howonsuck
                        // to desplay the command line
                        if (hProcess != NULL)
                        {
                            if (GetPsCommandLineEx(hProcess, sizeof(szCommandLine), szCommandLine))
                            {

                                strncpy(stProcessDetails[i].gcRunCLine, szCommandLine, sizeof(szCommandLine));
                                DEBUGMSGTL(("host/hr_swruntab", "i: %d, pid : %ld : %s\n", i, stProcessDetails[i].glProcessID, stProcessDetails[i].gcRunCLine));
                            }
                        }
                        else
                        {
                            DEBUGMSGTL(("host/hr_swruntab", "%d : %s\n", stProcessDetails[i].glProcessID, "Not found "));
                        }



                    }
                }
            }
        }
        CloseHandle(hProcess);
    }

    // Return Process Count
    return giHRSWRUNNoInstances;
}

int
header_swrun(struct variable* vp,
    oid* name,
    size_t* length,
    int exact, size_t* var_len, WriteMethod** write_method)
{
#define HRSWRUNPERF_NAME_LENGTH	9
    oid             newname[MAX_OID_LEN];
    int             result;

    DEBUGMSGTL(("host/hr_storage", "var_hrstore: "));
    DEBUGMSGOID(("host/hr_storage", name, *length));
    DEBUGMSG(("host/hr_storage", " %d\n", exact));

    memcpy((char*)newname, (char*)vp->name, vp->namelen * sizeof(oid));
    newname[HRSWRUNPERF_NAME_LENGTH] = 0;
    result = snmp_oid_compare(name, *length, newname, vp->namelen + 1);
    if ((exact && (result != 0)) || (!exact && (result >= 0)))
        return (MATCH_FAILED);
    memcpy((char*)name, (char*)newname,
        (vp->namelen + 1) * sizeof(oid));
    *length = vp->namelen + 1;

    *write_method = 0;
    *var_len = sizeof(long);    /* default to 'long' results */
    return (MATCH_SUCCEEDED);

}


/*
 * header_swruntab(...
 * Arguments:
 * vp     IN      - pointer to variable entry that points here
 * name    IN/OUT  - IN/name requested, OUT/name found
 * length  IN/OUT  - length of IN/OUT oid's
 * exact   IN      - TRUE if an exact match was requested
 * var_len OUT     - length of variable or 0 if function returned
 * write_method
 *
 */
int header_swruntab(struct variable* vp,
    oid* name,
    size_t* length,
    int exact, size_t* var_len, WriteMethod** write_method)
{

#define HRSWRUNENTRY_NAME_LENGTH	11
    oid             newname[MAX_OID_LEN];
    int             swruntab_idx, LowIndex = -1;
    int             result;
    //int				i;



    DEBUGMSGTL(("host/hr_swruntab", "var_swruntab: "));
    DEBUGMSGOID(("host/hr_swruntab", name, *length));
    DEBUGMSG(("host/hr_swruntab", " %d\n", exact));

    memcpy((char*)newname, (char*)vp->name, vp->namelen * sizeof(oid));
    /*
     * Find "next" swruntab entry
     */

    Init_HR_SWRunTab();

    for (;;)
    {
        swruntab_idx = Get_Next_HR_SWRunTab();
        if (swruntab_idx == -1)
            break;
        newname[HRSWRUNENTRY_NAME_LENGTH] = swruntab_idx;
        result = snmp_oid_compare(name, *length, newname, vp->namelen + 1);

        //Index for SNMPGet
        if (exact && (result == 0))
        {
            LowIndex = swruntab_idx;

            /*
             * Save swrun Status information
             */
            break;
        }

        if ((!exact && (result < 0)) &&
            (LowIndex == -1 || swruntab_idx < LowIndex))
        {
            LowIndex = swruntab_idx;
            /*
             * Save swrun Status information
             */
#ifdef HRSWRUNTAB_MONOTONICALLY_INCREASING
            break;
#endif
        }
    }

    if (LowIndex == -1)
    {
        DEBUGMSGTL(("host/hr_swruntab", "... index out of range\n"));
        return (MATCH_FAILED);
    }

    memcpy((char*)name, (char*)newname,
        (vp->namelen + 1) * sizeof(oid));
    *length = vp->namelen + 1;
    *write_method = 0;
    *var_len = sizeof(long);    /* default to 'long' results */

    DEBUGMSGTL(("host/hr_swruntab", "... get swruntab stats "));
    DEBUGMSGOID(("host/hr_swruntab", name, *length));
    DEBUGMSG(("host/hr_swruntab", "\n"));


    return LowIndex;
}


/** Initializes the hrSWRunEntry module */
void init_hrSWRunEntry(void)
{
    HANDLE hThread;
    unsigned threadID;

    DEBUGMSGTL(("hrSWRunEntry", "Initializing\n"));

    /* register ourselves with the agent to handle our mib tree */
    REGISTER_MIB("hrSWRunEntry", hrSWRunEntry_variables, variable4,
        hrSWRunEntry_variables_oid);

    SWRUNGet_Data();
    /* place any other initialization junk you need here */
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
unsigned char*
var_hrSWRun(struct variable* vp,
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
    //TimeStamp.
    int var_hrswrun = 0;

    var_hrswrun = GetTickCount();
    if (vp->magic == HRSWRUN_OSINDEX) {

        if (header_swrun(vp, name, length, exact, var_len, write_method)
            == MATCH_FAILED)
            return NULL;

    }

    /*
   * this is where we do the value assignments for the mib results.
   */

   /*
    *Start:WIP_NURI_UXW:implementation inside the case added,
    *the function Check_HR_FileSys_NFS is implemented
    *and the array hrs_descr is added
    */

    switch (vp->magic) {
    case HRSWRUN_OSINDEX:
        *var_len = sizeof(HRSWRUN_OS_index);
        return (u_char*)&HRSWRUN_OS_index;
        break;
    default:
        ERROR_MSG("");
    }
    return NULL;
}

/*
 * var_hrSWRunEntry():
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
unsigned char* var_hrSWRunEntry(struct variable* vp,
    oid* name,
    size_t* length,
    int     exact,
    size_t* var_len,
    WriteMethod** write_method)
{
    /* variables we may use later */
    static long long_ret;
    static u_long ulong_ret;
    static unsigned char string[HRSWRUNENTRY_NAME_LENGTH];
    static oid objid[MAX_OID_LEN];
    static struct counter64 c64;
    char caFilePath[MAX_PATH];
    //TimeStamp.
    int var_hrswrun = 0;

    //Get the TimeStamp during Entry
    var_hrswrun = GetTickCount();
    swruntab_idx = header_swruntab(vp, name, length, exact, var_len,
        write_method);

    if (swruntab_idx == MATCH_FAILED)
        return NULL;

    /*
    * this is where we do the value assignments for the mib results.
    */
    switch (vp->magic) {

    case HRSWRUNINDEX:
        *var_len = sizeof(swruntab_idx);
        return (u_char*)&swruntab_idx;

        //case HRSWRUNPARAMETERS:
        //	*var_len = strlen(gaEmpty);
        //	return (u_char*) gaEmpty;
    case HRSWRUNPARAMETERS:
        *var_len = strlen(stProcessDetails[HRSWR_index - 1].gcRunCLine);
        return (u_char*)stProcessDetails[HRSWR_index - 1].gcRunCLine;

    case HRSWRUNTYPE:
        *var_len = sizeof(giSWType);
        return (u_char*)&giSWType;

    case HRSWRUNNAME:
        if (strlen(stProcessDetails[HRSWR_index - 1].gcBinaryName) != 0)
        {
            *var_len = strlen(stProcessDetails[HRSWR_index - 1].gcBinaryName);
            return (u_char*)stProcessDetails[HRSWR_index - 1].gcBinaryName;
        }
        else
        {
            *var_len = strlen(stProcessDetails[HRSWR_index - 1].gcInstance);
            return (u_char*)stProcessDetails[HRSWR_index - 1].gcInstance;
        }

    case HRSWRUNPATH:
        memset(gcaFilePath, '\0', MAX_PATH);
        RemoveProcessName(stProcessDetails[HRSWR_index - 1].gcPathName);
        *var_len = strlen(gcaFilePath);
        return (u_char*)gcaFilePath;

        /*
        *var_len = strlen(stProcessDetails[HRSWR_index-1].gcPathName);
        return (u_char*) stProcessDetails[HRSWR_index-1].gcPathName;
        */

    case HRSWRUNID:
        *var_len = sizeof(giId);
        return (u_char*)&giId;

    case HRSWRUNSTATUS:
        *var_len = sizeof(giStatus);
        return (u_char*)&giStatus;
        //    *write_method = write_hrSWRunStatus;
    default:
        ERROR_MSG("ERROR");
    }
    return NULL;
}


int write_hrSWRunStatus(int      action,
    u_char* var_val,
    u_char   var_val_type,
    size_t   var_val_len,
    u_char* statP,
    oid* name,
    size_t   name_len)
{
    long value;
    int size;

    switch (action)
    {
    case RESERVE1:
        if (var_val_type != ASN_INTEGER) {
            fprintf(stderr, "write to hrSWRunEntry not ASN_INTEGER\n");
            return SNMP_ERR_WRONGTYPE;
        }
        if (var_val_len > sizeof(long)) {
            fprintf(stderr, "write to hrSWRunEntry: bad length\n");
            return SNMP_ERR_WRONGLENGTH;
        }
        break;

    case RESERVE2:
        size = var_val_len;
        value = *(long*)var_val;

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
  * name             :   Init_HR_SWRunTab
  * description      :   Initializing the static Index variable HRSWR_index
  * input parameters :   _
  * return type      :   _
  * global variables :   _
  * calls            :   _
  */
void Init_HR_SWRunTab(void)
{

    HRSWR_index = 0;

}

/*****************************************************************************
 *
 * name             :   Get_Next_HR_SWRunTab
 * description      :   Increments the static Index variable HRSWR_index and
 *						generates the index
 * input parameters :   _
 * return type      :   int
 * global variables :   _
 * calls            :   _
 */

int Get_Next_HR_SWRunTab(void)
{

    if (HRSWR_index < giHRSWRUNNoInstances)
    {   
        return (stProcessDetails[HRSWR_index++].glProcessID);
    }
    else
        return -1;
}



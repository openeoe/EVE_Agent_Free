/*****************************************************************************
 *
 * [hr_device_Win.c]
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Name : Ashok Shankar
 *
 * description :
 *      [ Collecting information about all devices present in the System.]
 *
 * TODO:
 *      [ ]
 *
 * CM Details:
 *
 * $Author:$
 * $Log:$
 * $State:$
 * $Id:$
 * $Name:$ 
 *   
 ****************************************************************************/
/*
 *  Host Resources MIB - Device group implementation - hr_device.c
 *
 */
/********************************* includes *********************************/
#include <net-snmp/net-snmp-config.h>
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
//For Registering with Device Table
#include "host_res_Win.h"
#include "hr_device_Win.h"
#include <iphlpapi.h>

/****************************** local macros ********************************/
#define	HRDEV_INDEX		1
#define	HRDEV_TYPE		2
#define	HRDEV_DESCR		3
#define	HRDEV_ID		4
#define	HRDEV_STATUS	5
#define	HRDEV_ERRORS	6
#define NOOFDRIVE       25
#define MAX_NO_PRINTER  10
#define MAX_NO_INTERFACE 25

/********************* local structure/union definitions ********************/
/* Added by Sai */
typedef struct 
{
    char caName[100];
    int  nType;
}stPrinterDetails;
typedef struct 
{
    char caName[25];
}stCommPortDetails;

/* End Added */
/********************* prototypes for local functions ***********************/
int ifcall(void);
/* Added by Sai */
int GetPrinterDetails(void);
//int GetCommPortDetails(void);
/* End Added */
    /*********************
 *
 *  Kernel & interface information,
 *   and internal forward declarations
 *
 *********************/

int             Get_Next_Device(void);

PFV             init_device[HRDEV_TYPE_MAX];
PFIV            next_device[HRDEV_TYPE_MAX];
PFV             save_device[HRDEV_TYPE_MAX];
int             dev_idx_inc[HRDEV_TYPE_MAX];

PFS             device_descr[HRDEV_TYPE_MAX];
PFO             device_prodid[HRDEV_TYPE_MAX];
PFI             device_status[HRDEV_TYPE_MAX];
PFI             device_errors[HRDEV_TYPE_MAX];

int             current_type;
char                gNdesc[MAX_NO_INTERFACE][100];
stPrinterDetails    gstPrinterInfo[MAX_NO_PRINTER];
//stCommPortDetails   gstCommPortInfo[15]; 

char            tempp[100];
int             gifcount;
int             gPrinterCount;
int             gCommPortCount;
void            Init_Device(void);
int             header_hrdevice(struct variable *, oid *, size_t *, int,
                                size_t *, WriteMethod **);

/************************ globals within this module*************************/

/************************ externs *************************/
extern long     glrem[NOOFDRIVE];

    /*********************
 *
 *  Initialisation & common implementation functions
 *
 *********************/

SYSTEM_INFO sys;
struct variable4 hrdevice_variables[] = {
    {HRDEV_INDEX, ASN_INTEGER, RONLY, var_hrdevice, 2, {1, 1}},
    {HRDEV_TYPE, ASN_OBJECT_ID, RONLY, var_hrdevice, 2, {1, 2}},
    {HRDEV_DESCR, ASN_OCTET_STR, RONLY, var_hrdevice, 2, {1, 3}},
    {HRDEV_ID, ASN_OBJECT_ID, RONLY, var_hrdevice, 2, {1, 4}},
    {HRDEV_STATUS, ASN_INTEGER, RONLY, var_hrdevice, 2, {1, 5}},
    {HRDEV_ERRORS, ASN_COUNTER, RONLY, var_hrdevice, 2, {1, 6}}
};
oid             hrdevice_variables_oid[] = { 1, 3, 6, 1, 2, 1, 25, 3, 2 };


void init_hr_device(void)
{
    int             i;

    /*
     * Initially assume no devices
     *    Insert pointers to initialisation/get_next routines
     *    for particular device types as they are implemented
     *      (set up in the appropriate 'init_*()' routine )
     */

    for (i = 0; i < HRDEV_TYPE_MAX; ++i) {
        init_device[i] = NULL;
        next_device[i] = NULL;
        save_device[i] = NULL;
        dev_idx_inc[i] = 0;     /* Assume random indices */

        device_descr[i] = NULL;
        device_prodid[i] = NULL;
        device_status[i] = NULL;
        device_errors[i] = NULL;
    }

    REGISTER_MIB("host/hr_device", hrdevice_variables, variable4,
                 hrdevice_variables_oid);
                 
    gifcount=ifcall();
    gPrinterCount =  GetPrinterDetails();
    //gCommPortCount = GetCommPortDetails();
}


/*
 * header_hrdevice(...
 * Arguments:
 * vp     IN      - pointer to variable entry that points here
 * name    IN/OUT  - IN/name requested, OUT/name found
 * length  IN/OUT  - length of IN/OUT oid's 
 * exact   IN      - TRUE if an exact match was requested
 * var_len OUT     - length of variable or 0 if function returned
 * write_method
 * 
 */
int header_hrdevice(struct variable *vp,
                oid * name,
                size_t * length,
                int exact, size_t * var_len, WriteMethod ** write_method)
{
#define HRDEV_ENTRY_NAME_LENGTH	11
    oid             newname[MAX_OID_LEN];
    int             dev_idx, LowIndex = -1, LowType = -1;
    int             result;

    DEBUGMSGTL(("host/hr_device", "var_hrdevice: "));
    DEBUGMSGOID(("host/hr_device", name, *length));
    DEBUGMSG(("host/hr_device", " %d\n", exact));

    memcpy((char *) newname, (char *) vp->name,
           (int) vp->namelen * sizeof(oid));


    /*
     *  Find the "next" device entry.
     *  If we're in the middle of the table, then there's
     *     no point in examining earlier types of devices,
     *     so set the starting type to that of the variable
     *     being queried.
     *  If we've moved from one column of the table to another,
     *     then we need to start at the beginning again.
     *     (i.e. the 'compare' fails to match)
     *  Similarly if we're at the start of the table
     *     (i.e. *length is too short to be a full instance)
     */

    if ((snmp_oid_compare(vp->name, vp->namelen, name, vp->namelen) == 0)
        && (*length > HRDEV_ENTRY_NAME_LENGTH))
        current_type = (name[HRDEV_ENTRY_NAME_LENGTH] >> HRDEV_TYPE_SHIFT);
    else
        current_type = 0;

    Init_Device();
    for (;;) {
        dev_idx = Get_Next_Device();
        DEBUGMSG(("host/hr_device", "(index %d ....", dev_idx));
        if (dev_idx == -1)
            break;
        if (LowType != -1 && LowType < (dev_idx >> HRDEV_TYPE_SHIFT))
            break;
        newname[HRDEV_ENTRY_NAME_LENGTH] = dev_idx;
        DEBUGMSGOID(("host/hr_device", newname, *length));
        DEBUGMSG(("host/hr_device", "\n"));
        result = snmp_oid_compare(name, *length, newname, vp->namelen + 1);
        if (exact && (result == 0)) {
            if (save_device[current_type] != NULL)
                (*save_device[current_type]) ();
            LowIndex = dev_idx;
            break;
        }
        if ((!exact && (result < 0)) &&
            (LowIndex == -1 || dev_idx < LowIndex)) {
            if (save_device[current_type] != NULL)
                (*save_device[current_type]) ();
            LowIndex = dev_idx;
            LowType = (dev_idx >> HRDEV_TYPE_SHIFT);
            if (dev_idx_inc[LowType])   /* Increasing indices => now done */
                break;
        }

    }

    if (LowIndex == -1) {
        DEBUGMSGTL(("host/hr_device", "... index out of range\n"));
        return (MATCH_FAILED);
    }

    newname[HRDEV_ENTRY_NAME_LENGTH] = LowIndex;
    memcpy((char *) name, (char *) newname,
           ((int) vp->namelen + 1) * sizeof(oid));
    *length = vp->namelen + 1;
    *write_method = 0;
    *var_len = sizeof(long);    /* default to 'long' results */

    DEBUGMSGTL(("host/hr_device", "... get device stats "));
    DEBUGMSGOID(("host/hr_device", name, *length));
    DEBUGMSG(("host/hr_device", "\n"));

    return LowIndex;
}


oid device_type_id[] = { 1, 3, 6, 1, 2, 1, 25, 3, 1, 99 };  /* hrDeviceType99 */
int device_type_len = sizeof(device_type_id) / sizeof(device_type_id[0]);


    /*********************
 *
 *  System specific implementation functions
 *
 *********************/


u_char         *
var_hrdevice(struct variable *vp,
             oid * name,
             size_t * length,
             int exact, size_t * var_len, WriteMethod ** write_method)
{
    int             dev_idx, type, Dtype;
    static int             ifi=0;
    oid            *oid_p;
    static char     string[100];

    dev_idx =
        header_hrdevice(vp, name, length, exact, var_len, write_method);
    if (dev_idx == MATCH_FAILED)
        return NULL;

    type = (dev_idx >> HRDEV_TYPE_SHIFT);
    if (type == 6)
    {
        Dtype = dev_idx - 1537;
    }


    switch (vp->magic) {
    case HRDEV_INDEX:
        long_return = dev_idx;
        return (u_char *) & long_return;
    case HRDEV_TYPE:
        device_type_id[device_type_len - 1] = type;
        *var_len = sizeof(device_type_id);
        return (u_char *) device_type_id;
    case HRDEV_DESCR:
        if (device_descr[type] != NULL) {
            strncpy(string, ((*device_descr[type]) (dev_idx)),
                    sizeof(string)-1);
            string[ sizeof(string)-1] = 0;
        } else
#if NO_DUMMY_VALUES
            return NULL;
#else
            //sprintf(string, "a black box of some sort");
        switch(type)
		{
		case 2:
			sprintf(string,"Unknown Device");
			break;
		case 3:
            GetSystemInfo(&sys);
            sprintf(string,"System Processor");
            switch(sys.wProcessorArchitecture)
            {
               case PROCESSOR_ARCHITECTURE_INTEL:
                    strcat(string,"PROCESSOR_ARCHITECTURE_INTEL");
                    break;
               case PROCESSOR_ARCHITECTURE_IA64:
                    strcat(string,"PROCESSOR_ARCHITECTURE_IA64");
                    break;
               case PROCESSOR_ARCHITECTURE_MIPS:
                    strcat(string,"PROCESSOR_ARCHITECTURE_MIPS");
                    break;
               case PROCESSOR_ARCHITECTURE_ALPHA:
                    strcat(string,"PROCESSOR_ARCHITECTURE_ALPHA");
                    break;
               case PROCESSOR_ARCHITECTURE_PPC:
                    strcat(string,"PROCESSOR_ARCHITECTURE_PPC");
                    break;
             }

            sprintf(tempp,"%d",sys.dwProcessorType);
            strcat(string,tempp);   
			break;
		case 4:
            strcpy(string,"Network Interface  :");
            strcat(string,gNdesc[dev_idx-1025]);
    		break;
		case 5:
              strcpy(string,gstPrinterInfo[dev_idx-1280].caName);
              strcat(string," - ");
              if (gstPrinterInfo[dev_idx-1280].nType  == PRINTER_ATTRIBUTE_LOCAL)
              {
                strcat(string,"Local Printer");
              }
              else
              {
                  strcat(string,"Remote Printer");
              }
            break;
		case 6:
        {
            if(glrem[Dtype] == 3)
                sprintf(string,"Hard Disk Present in the system");
            else if(glrem[Dtype] == 4)
                sprintf(string,"Floppy Disk Present in the system");
            else if(glrem[Dtype] == 5)
                sprintf(string,"CD-ROM Present in the system");
			break;
        }
        /*
      case 17:
          //strcpy(string,gstCommPortInfo[dev_idx-4352].caName);
          strcpy(string,"testport");
          break;
          */
		default:
			sprintf(string,"Unknown Device");
			break;
		}
#endif
        *var_len = strlen(string);
        return (u_char *) string;
    case HRDEV_ID:
        if (device_prodid[type] != NULL)
            oid_p = ((*device_prodid[type]) (dev_idx, var_len));
        else {
            oid_p = nullOid;
            *var_len = nullOidLen;
        }
        return (u_char *) oid_p;
    case HRDEV_STATUS:
        if (device_status[type] != NULL)
            long_return = ((*device_status[type]) (dev_idx));
        else
#if NO_DUMMY_VALUES
            return NULL;
#else
            long_return = 1;    /* Assume Unknown */
#endif
        return (u_char *) & long_return;
    case HRDEV_ERRORS:
        if (device_errors[type] != NULL)
            long_return = (*device_errors[type]) (dev_idx);
        else
#if NO_DUMMY_VALUES
            return NULL;
#else
            long_return = 0;    /* Assume OK */
#endif
        return (u_char *) & long_return;
    default:
        DEBUGMSGTL(("snmpd", "unknown sub-id %d in var_hrdevice\n",
                    vp->magic));
    }
    return NULL;
}


    /*********************
 *
 *  Internal implementation functions
 *
 *********************/
/*****************************************************************************
 *
 * name             :   Init_Device
 * description      :   Initialization function.
 * input parameters :   _
 * return type      :   _
 * global variables :   _
 * calls            :   _
 */

void Init_Device(void)
{
    /*
     *  Find the first non-NULL initialisation function
     *    and call it
     */
    while (current_type < HRDEV_TYPE_MAX &&
           init_device[current_type] == NULL)
        if (++current_type >= HRDEV_TYPE_MAX)
            return;
    (*init_device[current_type]) ();
}

/*****************************************************************************
 *
 * name             :   Get_Next_Device
 * description      :   Generates the index
 * input parameters :   _
 * return type      :   int
 * global variables :   _
 * calls            :   _
 */
int Get_Next_Device(void)
{
    int             result = -1;

    /*
     *  Call the 'next device' function for the current
     *    type of device
     *
     *  TODO:  save the necessary information about that device
     */
    if (current_type < HRDEV_TYPE_MAX && next_device[current_type] != NULL)
        result = (*next_device[current_type]) ();

    /*
     *  No more devices of the current type.
     *  Try the next type (if any)
     */
    if (result == -1) {
        if (++current_type >= HRDEV_TYPE_MAX) {
            current_type = 0;
            return -1;
        }
        Init_Device();
        return Get_Next_Device();
    }
    return result;
}


int ifcall()
{
   DWORD ifIndex=0;
   PMIB_IFTABLE    pIfTable = NULL;
   DWORD dwActualSize = 0;
   DWORD status = NO_ERROR;
   DWORD count;
  

	status = GetIfTable(pIfTable, &dwActualSize, TRUE);

    if (status == ERROR_INSUFFICIENT_BUFFER) 
	{ pIfTable = (PMIB_IFTABLE) malloc(dwActualSize);
 
      if (pIfTable != NULL) 
       GetIfTable(pIfTable, &dwActualSize, TRUE);
    }

    count = pIfTable->dwNumEntries; 
    if (count > MAX_NO_INTERFACE)
    {
      count = MAX_NO_INTERFACE ;
    }
   
	for (ifIndex = 0; ifIndex < count; ifIndex++) 
	{
        sprintf(gNdesc[ifIndex],"%s",pIfTable->table[ifIndex].bDescr);
	}


    if(pIfTable)
    free(pIfTable);

    return count;
    
}

int GetPrinterDetails(void)
{
    PRINTER_INFO_4 *pPrintInfo4 = NULL;
    DWORD dwSizeNeeded = 0;
    DWORD dwNumOfPrinters = 0;
    int nCount = 0;

    if( EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS ,
                      NULL,4,pPrintInfo4,0,&dwSizeNeeded,
                      &dwNumOfPrinters) == 0)
    {
        pPrintInfo4 = malloc (dwSizeNeeded) ;
        EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS ,
                      NULL,4,pPrintInfo4,dwSizeNeeded,&dwSizeNeeded,
                      &dwNumOfPrinters);

        if (dwNumOfPrinters > MAX_NO_PRINTER)
        {
            dwNumOfPrinters = MAX_NO_PRINTER ;
        }
        for(nCount =0;nCount<dwNumOfPrinters;nCount++)
        {
            strcpy(gstPrinterInfo[nCount].caName,pPrintInfo4[nCount].pPrinterName);
            gstPrinterInfo[nCount].nType = pPrintInfo4[nCount].Attributes;
        }

    }

    free(pPrintInfo4);
    return dwNumOfPrinters;
}

/*
int GetCommPortDetails(void)
{
   PORT_INFO_2 *pPortInfo2 = NULL;
   DWORD dwSizeNeeded = 0;
   DWORD dwNumOfPorts = 0;
   int nCount = 0;
    DEBUGMSGTL(("nuri/nam", "Entered GetCommPortDetails()\n"));
    if( EnumPorts(NULL,2,pPortInfo2,0,&dwSizeNeeded,
                      &dwNumOfPorts) == 0)
    {
        if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
        {
            DEBUGMSGTL(("nuri/nam", "ERROR_INSUFFICIENT_BUFFER :Needed: %d ",dwSizeNeeded));
            pPortInfo2 = malloc (dwSizeNeeded) ;
            if ( EnumPorts( NULL,2,pPortInfo2,dwSizeNeeded,&dwSizeNeeded,
                          &dwNumOfPorts) != 0)
            {
                DEBUGMSGTL(("nuri/nam", "Num of Ports :%d ",dwNumOfPorts));
                for(nCount =0;nCount<dwNumOfPorts;nCount++)
                {
                    //strcpy(gstCommPortInfo[nCount].caName,pPortInfo2[nCount].pPortName);
                }
            }
            else
            {
                DEBUGMSGTL(("nuri/nam", "Ports Error1 :%d ",GetLastError()));

            }
        }
        else
        {
            DEBUGMSGTL(("nuri/nam", "Ports Error2 :%d ",GetLastError()));
        }

    }
    else
    {
        DEBUGMSGTL(("nuri/nam", "Else BUFFER :Needed: %d ",dwSizeNeeded));
    }

    free(pPortInfo2);
    return dwNumOfPorts;
}
*/

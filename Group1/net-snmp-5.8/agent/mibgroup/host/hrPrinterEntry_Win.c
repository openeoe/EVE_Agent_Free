/*****************************************************************************
 *
 * [hrPrinterEntry_Win.c]
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Name : Ashok Shankar
 *
 * description :
 *      [ Collecting information about Printer present in the System ]
 *
 * TODO:
 *      [ ]
 *
 * CM Details:
 *
 * $Author: howonsuk $
 * $Log: hrPrinterEntry_Win.c,v $
 * Revision 1.2  2004/03/03 09:09:03  howonsuk
 * timecache계산에 절대값 사용
 *
 * Revision 1.1  2004/03/03 06:14:18  howonsuk
 * 로그모니터링: 정규식 추가
 * 프로세스모니터링 : 파라미터보여주기 및 파라미터에서 정규식으로 찾기 추가
 *
 * Revision 1.4  2003/11/27 15:47:14  ashok
 * Minor Changes
 *
 * Revision 1.3  2003/11/20 06:21:58  ashok
 * Minor Corrections
 *
 * Revision 1.2  2003/11/08 15:23:39  ashok
 * no message
 *
 * Revision 1.1  2003/10/31 12:08:44  ashok
 * Added into CVS by Ashok
 *
 * $State: Exp $
 * $Id: hrPrinterEntry_Win.c,v 1.2 2004/03/03 09:09:03 howonsuk Exp $
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
#include "hrPrinterEntry_Win.h"

//For Registering with Device Table
#include "host_res_Win.h"
#include <stdlib.h>
/****************************** local macros ********************************/
// For Index Genaration
#define HRPROC_MONOTONICALLY_INCREASING

#define CACHETIME       5000

/********************* local structure/union definitions ********************/

/********************* prototypes for local functions ***********************/
int     header_hrprint(struct variable *, oid *, size_t *, int,size_t *, 
				   WriteMethod **);
void    Init_HR_Print(void);
int     Get_Next_HR_Print(void);
void    HRPRINTGet_Data(void);

/************************ globals within this module*************************/
//Time Stamps
int             giHRPRINTTimeStamp;

// Define API Specific VAriables 
HANDLE          ghPrinter;
BOOL            gResult;
PRINTER_INFO_2  gstPrinter;
DWORD           gPcbNeeded;


/************************ externs *************************/

/************************ Statics *************************/
static int      HRP_index;

/* 
 * hrPrinterEntry_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */

oid hrPrinterEntry_variables_oid[] = { 1,3,6,1,2,1,25,3,5,1 };

/* 
 * variable4 hrPrinterEntry_variables:
 *   this variable defines function callbacks and type return information 
 *   for the  mib section 
 */

struct variable4 hrPrinterEntry_variables[] = {
/*  magic number        , variable type , ro/rw , callback fn  , L, oidsuffix */
/*    (L = length of the oidsuffix) */
#define HRPRINTERSTATUS		1
{HRPRINTERSTATUS,  ASN_INTEGER,  RONLY ,  var_hrPrinterEntry, 1,  { 1 }},
#define HRPRINTERDETECTEDERRORSTATE		2
{HRPRINTERDETECTEDERRORSTATE,  ASN_OCTET_STR,  RONLY ,  var_hrPrinterEntry, 1,  { 2 }},

};

/*****************************************************************************
 *
 * name             :   HRPRINTGet_Data
 * description      :   Data Collection 
 * input parameters :   _
 * return type      :   _
 * global variables :   ghPrinter, gstPrinter, gPcbNeeded, gResult, 
 *                      giHRPRINTTimeStamp
 * calls            :   _
 */
void HRPRINTGet_Data(void)
{
    //Get the TimeStamp during Entry
	giHRPRINTTimeStamp = GetTickCount();

    gResult = OpenPrinter(NULL,&ghPrinter,NULL);
	if (gResult)
	{
		gResult = GetPrinter(ghPrinter,2,(unsigned char*) &gstPrinter,100,&gPcbNeeded);
		if(!gResult)
		{
			gResult = GetPrinter(ghPrinter,2,(unsigned char*) &gstPrinter,gPcbNeeded,
								 &gPcbNeeded);
		}
	}
}

/** Initializes the hrPrinterEntry module */
void init_hrPrinterEntry(void)
{
	//For Registering in hrDevice Table.
	init_device[HRDEV_PRINTER] = Init_HR_Print;
    next_device[HRDEV_PRINTER] = Get_Next_HR_Print;
#ifdef HRPROC_MONOTONICALLY_INCREASING
    dev_idx_inc[HRDEV_PRINTER] = 1;
#endif

    DEBUGMSGTL(("hrPrinterEntry", "Initializing\n"));
    /* register ourselves with the agent to handle our mib tree */
    REGISTER_MIB("hrPrinterEntry", hrPrinterEntry_variables, variable4,
               hrPrinterEntry_variables_oid);

    /* place any other initialization junk you need here */
	
}

/*
 * header_hrprint(...
 * Arguments:
 * vp     IN      - pointer to variable entry that points here
 * name    IN/OUT  - IN/name requested, OUT/name found
 * length  IN/OUT  - length of IN/OUT oid's 
 * exact   IN      - TRUE if an exact match was requested
 * var_len OUT     - length of variable or 0 if function returned
 * write_method
 * 
 */
int header_hrprint(struct variable *vp,
              oid * name,
              size_t * length,
              int exact, size_t * var_len, WriteMethod ** write_method)
{	
#define HRPROC_ENTRY_NAME_LENGTH	11
    oid             newname[MAX_OID_LEN];
    int             print_idx, LowIndex = -1;
    int             gResult;
	
    DEBUGMSGTL(("host/hr_print", "var_hrprint: "));
    DEBUGMSGOID(("host/hr_print", name, *length));
    DEBUGMSG(("host/hr_print", " %d\n", exact));

    memcpy((char *) newname, (char *) vp->name, vp->namelen * sizeof(oid));
    /*
     * Find "next" print entry 
     */

    Init_HR_Print();

    for (;;)
	{
        print_idx = Get_Next_HR_Print();
        if (print_idx == -1)
            break;
        newname[HRPROC_ENTRY_NAME_LENGTH] = print_idx;
        gResult = snmp_oid_compare(name, *length, newname, vp->namelen + 1);
        if (exact && (gResult == 0))
		{
            LowIndex = print_idx;

            /*
             * Save printer status information 
             */
            break;
        }
        if ((!exact && (gResult < 0)) &&
            (LowIndex == -1 || print_idx < LowIndex))
		{
            LowIndex = print_idx;
            /*
             * Save printer status information 
             */
#ifdef HRPROC_MONOTONICALLY_INCREASING
            break;
#endif
        }
    }

    if (LowIndex == -1)
	{
        DEBUGMSGTL(("host/hr_print", "... index out of range\n"));
        return (MATCH_FAILED);
    }

    memcpy((char *) name, (char *) newname,
           (vp->namelen + 1) * sizeof(oid));
    *length = vp->namelen + 1;
    *write_method = 0;
    *var_len = sizeof(long);    /* default to 'long' gResults */

    DEBUGMSGTL(("host/hr_print", "... get print stats "));
    DEBUGMSGOID(("host/hr_print", name, *length));
    DEBUGMSG(("host/hr_print", "\n"));
	

    return LowIndex;
}


/*
 * var_hrPrinterEntry():
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
var_hrPrinterEntry(struct variable *vp, 
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
	
	// Index Variable
	int print_idx;

        //TimeStamp.
	int var_hrprin = 0;
    
	//Get the TimeStamp during Entry
	var_hrprin = GetTickCount();
	
    //Decide whether to call HRPRINTGet_Data( )
    if(abs(var_hrprin - giHRPRINTTimeStamp) > CACHETIME)
		HRPRINTGet_Data();

    print_idx = header_hrprint(vp, name, length, exact, var_len, write_method);

    if (print_idx == MATCH_FAILED)

        return NULL;

    /* 
   * this is where we do the value assignments for the mib gResults.
   */
    switch(vp->magic) {
    case HRPRINTERSTATUS:
        *var_len = sizeof(gstPrinter.Status);
        return (u_char*) &gstPrinter.Status;
    //HRPRINTERDETECTEDERRORSTATE:
    //    VAR = VALUE;	/* XXX */
    //    return (u_char*) &VAR;
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
 * name             :   Init_HR_Print
 * description      :   Initializing the static Index variable HRP_index
 * input parameters :   _
 * return type      :   _
 * global variables :   _
 * calls            :   _
 */
void Init_HR_Print(void)
{
    HRP_index = 0;
}

/*****************************************************************************
 *
 * name             :   Get_Next_HR_Print
 * description      :   Increments the static Index variable HRP_index and 
 *						generates the index
 * input parameters :   _
 * return type      :   int
 * global variables :   _
 * calls            :   _
 */
int Get_Next_HR_Print(void)
{
    if (HRP_index < 1)
        return ((HRDEV_PRINTER << HRDEV_TYPE_SHIFT) + ++HRP_index);
    else
        return -1;
}

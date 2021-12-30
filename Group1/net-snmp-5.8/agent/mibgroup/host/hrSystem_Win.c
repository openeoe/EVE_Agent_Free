/*****************************************************************************
 *
 * [hrSystem_Win.c]
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Name Karthikeyan Subramani
 *
 * description :
 *      [ Collecting System information]
 *
 * TODO:
 *      [ Things to be improved ]
 *
 * CM Details:
 *
 * $Author: howonsuk $
 * $Log: hrSystem_Win.c,v $
 * Revision 1.1  2004/03/03 06:14:21  howonsuk
 * 로그모니터링: 정규식 추가
 * 프로세스모니터링 : 파라미터보여주기 및 파라미터에서 정규식으로 찾기 추가
 *
 * $State: Exp $
 * $Id: hrSystem_Win.c,v 1.1 2004/03/03 06:14:21 howonsuk Exp $   
 * $Name:  $
 *
 ****************************************************************************/


/********************************* includes *********************************/
/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf,v 1.3 2002/10/17 09:40:46 dts12 Exp $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "../NuriEnterprise/Win32/inc/NASCenter_EM_PDHCommon_Win.h"
#include "hrSystem_Win.h"
#include "util_funcs.h"
#include <windows.h>
#include <string.h>
#include <psapi.h>
 


/****************************** local macros ********************************
 *
 * (Macros shareable with rest of the modules are present in .h file)
 */


/********************* local structure/union definitions ********************/

/********************* prototypes for local functions ***********************/
WriteMethod  write_hrSystemDate;
 
/************************* Globals within this module ***********************/
//To collect the formatted value from Query
PDH_FMT_COUNTERVALUE gstSYSPDHValue;

/************************ externs *************************/
//extern          tree_head;

 /* 
 * hrSystem_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */

oid hrSystem_variables_oid[] = { 1,3,6,1,2,1,25,1 };
/* 
 * variable4 hrSystem_variables:
 *   this variable defines function callbacks and type return information 
 *   for the  mib section 
 */

struct variable4 hrSystem_variables[] = {
/*  magic number        , variable type , ro/rw , callback fn  , L, oidsuffix */
#define HRSYSTEMINITIALLOADDEVICE		1
{HRSYSTEMINITIALLOADDEVICE,  ASN_INTEGER,  RWRITE,  var_hrSystem, 1,  { 3 }},
#define HRSYSTEMINITIALLOADPARAMETERS		2
{HRSYSTEMINITIALLOADPARAMETERS,  ASN_OCTET_STR,  RWRITE,  var_hrSystem, 1,  { 4 }},
#define HRSYSTEMPROCESSES		3
{HRSYSTEMPROCESSES,  ASN_GAUGE,  RONLY ,  var_hrSystem, 1,  { 6 }},
#define HRSYSTEMUPTIME		4
{HRSYSTEMUPTIME,  ASN_TIMETICKS,  RONLY ,  var_hrSystem, 1,  { 1 }},   
#define HRSYSTEMNUMUSERS		5
{HRSYSTEMNUMUSERS,  ASN_GAUGE,  RONLY ,  var_hrSystem, 1,  { 5 }},
#define HRSYSTEMDATE		6
{HRSYSTEMDATE,  ASN_OCTET_STR,  RWRITE,  var_hrSystem, 1,  { 2 }},       
#define HRSYSTEMMAXPROCESSES		7
{HRSYSTEMMAXPROCESSES,  ASN_INTEGER,  RONLY ,  var_hrSystem, 1,  { 7 }},

};
unsigned char * gcoldstat;
	static SYSTEMTIME st;
    static char buffer[15];
	static char buffer1[20];
    static char dest[50];  
	static long uptim;
    static char *date;
/*    (L = length of the oidsuffix) */


/** Initializes the hrSystem module */
void
init_hrSystem(void)
{

    DEBUGMSGTL(("hrSystem", "Initializing\n"));

    //DEBUGMSGTL(("main", "####Inside init_hrSystem --> tree_head=%lu#################\n", tree_head));

    /* register ourselves with the agent to handle our mib tree */
    REGISTER_MIB("hrSystem", hrSystem_variables, variable4,
               hrSystem_variables_oid);
    

    /* place any other initialization junk you need here */
}

/*
 * var_hrSystem():
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
var_hrSystem(struct variable *vp, 
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
 
    static long cProcesses; 
    DWORD aProcesses[1024];
    DWORD cbNeeded; 

  
     
	

    if (header_generic(vp,name,length,exact,var_len,write_method)
                                  == MATCH_FAILED )
    return NULL;

    /* 
   * this is where we do the value assignments for the mib results.
   */
    switch(vp->magic) {
     /*HRSYSTEMINITIALLOADDEVICE:
        *write_method = write_hrSystemInitialLoadDevice;
        VAR = VALUE;	 
        return (u_char*) &VAR;
        HRSYSTEMINITIALLOADPARAMETERS:
        *write_method = write_hrSystemInitialLoadParameters;
        VAR = VALUE;	 
        return (u_char*) &VAR;
        */
      // supported OS version Windows XP/ NT 4.0 2000        
    case HRSYSTEMPROCESSES: // counting no of process running on the system
            if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
            {	 cProcesses =-1;
             *var_len=sizeof(cProcesses);
             return (u_char*) &cProcesses;
            }
            cProcesses =(long) cbNeeded / sizeof(DWORD);
            *var_len=sizeof(cProcesses);
            return (u_char*) &cProcesses;

    case HRSYSTEMUPTIME: //supported in all Windows version
		  uptim = GetTickCount();
		  uptim = (long)(uptim/10);
          *var_len=sizeof(uptim);
		  return (unsigned char *)&uptim;
		  break;

    case HRSYSTEMNUMUSERS: // Windows XP / 2000 / NT 
            // Use Common PDH function to collect the data
            COMPDH_Collection("\\Server\\Server Sessions", &gstSYSPDHValue);

            uptim = gstSYSPDHValue.longValue;
            *var_len=sizeof(uptim);
            return (unsigned char *)&uptim;
            break;

     case HRSYSTEMDATE: // Supported in all Windows version
		  GetLocalTime(&st);
          dest[1]=(char)(st.wYear&0xff);
          dest[0]=(char)((st.wYear>>8)&0xff);
          dest[2]=(char)st.wMonth;
          dest[3]=(char)st.wDay;
          dest[4]=(char)st.wHour;
          dest[5]=(char)st.wMinute;
          dest[6]=(char)st.wSecond;

          //GetDateFormat(LOCALE_SYSTEM_DEFAULT,0,&st,"ddd','MMM dd yy",buffer,sizeof(buffer));
		  //GetTimeFormat(LOCALE_SYSTEM_DEFAULT,LOCALE_NOUSEROVERRIDE,&st,NULL,buffer1,sizeof(buffer1));
		  //sprintf (dest,"%s %s",buffer,buffer1);
          //sprintf(dest,"%s","Tuesday May 26, 1992 at 1:30:15 PM");
		  date=dest;
          

          //*write_method =write_hrSystemDate;
          *var_len = strlen(dest);
          return (u_char *) date;
		  break;

     case HRSYSTEMMAXPROCESSES:
         uptim=0;
         *var_len =sizeof(uptim);	
        return (u_char*) &uptim;
    default:
     ERROR_MSG("");
    }
    return NULL;
}





int
write_hrSystemInitialLoadDevice(int      action,
            u_char   *var_val,
            u_char   var_val_type,
            size_t   var_val_len,
            u_char   *statP,
            oid      *name,
            size_t   name_len)
{
    long value;
    int size;

    switch ( action ) {
        case RESERVE1:
          if (var_val_type != ASN_INTEGER) {
              fprintf(stderr, "write to hrSystem not ASN_INTEGER\n");
              return SNMP_ERR_WRONGTYPE;
          }
          if (var_val_len > sizeof(long)) {
              fprintf(stderr,"write to hrSystem: bad length\n");
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


int
write_hrSystemInitialLoadParameters(int      action,
            u_char   *var_val,
            u_char   var_val_type,
            size_t   var_val_len,
            u_char   *statP,
            oid      *name,
            size_t   name_len)
{
    char value;
    int size;

    switch ( action ) {
        case RESERVE1:
          if (var_val_type != ASN_OCTET_STR) {
              fprintf(stderr, "write to hrSystem not ASN_OCTET_STR\n");
              return SNMP_ERR_WRONGTYPE;
          }
          if (var_val_len > sizeof(char)) {
              fprintf(stderr,"write to hrSystem: bad length\n");
              return SNMP_ERR_WRONGLENGTH;
          }
          break;

        case RESERVE2:
          size  = var_val_len;
          value = * (char *) var_val;

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


int
write_hrSystemDate(int      action,
            u_char   *var_val,
            u_char   var_val_type,
            size_t   var_val_len,
            u_char   *statP,
            oid      *name,
            size_t   name_len)
{
     
    
	
    
    switch ( action ) {
        case RESERVE1:
          if (var_val_type != ASN_OCTET_STR) {
              fprintf(stderr, "write to hrSystem not ASN_OCTET_STR\n");
              return SNMP_ERR_WRONGTYPE;
          }
          /*if (var_val_len > sizeof(char)) {
              fprintf(stderr,"write to hrSystem: bad length\n");
              return SNMP_ERR_WRONGLENGTH;
           }*/
         
            
           break;

        case RESERVE2:
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




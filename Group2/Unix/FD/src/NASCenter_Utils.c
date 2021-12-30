/*****************************************************************************
 *
 * Utils.c
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Vijaigeetha C.
 *
 * description : Common utilities used in all modules *      
 *      
 *
 * TODO:
 *      1. 
 *      
 *      2. 
 *
 * CM Details:
 *
 * $Author: vijaige $
 * $Log: NASCenter_Utils.c,v $
 * Revision 1.4  2003/12/22 06:32:15  vijaige
 * removed warnings
 *
 * Revision 1.3  2003/12/10 07:38:44  vijaige
 * 10/12/2003 with unix porting
 *
 * Revision 1.2  2003/12/02 12:07:41  vijaige
 * after removing printf
 *
 * 
 * 
 * $Id: NASCenter_Utils.c,v 1.4 2003/12/22 06:32:15 vijaige Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/

#ifdef WIN32
#include <windows.h>
#include <Lmcons.h>
#endif
#include <stdio.h>
#include <pthread.h>
/*NetSnmp includes */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "ctypes.h"


/*****************************************************************************
 *
 * name             :   Util_Convert_Date_To_Long
 * description      :   This function will convert the date to Long integer
                        
 * input parameters :   char *, unsigned long
 *                      
 * return type      :   int -1 failure 0 for success

 * global variables :   -
 * calls            :   
 *****************************************************************************/

I16 Util_Convert_Date_To_Long(I8 *pctempstr,UI32 *pulStartTime)
{
	struct tm tm = { 0 };
    char str[80] = { 0 };
    

    int year=0;
    

    

    strcpy (str, pctempstr);    
	
    year = str[0];
    year = (year << 8) | (str[1] & 0xff);
    tm.tm_year = year - 1900;

    tm.tm_mon  = (int) (str[2] & 0xff) - 1;
    tm.tm_mday = (int) (str[3] & 0xff);
    tm.tm_hour = (int) (str[4] & 0xff);
    tm.tm_min =  (int) (str[5] & 0xff);
    tm.tm_sec =  (int) (str[6] & 0xff);

	*pulStartTime = mktime(&tm);


    return 0;
	
}

/*****************************************************************************
 *
 * name             :   Util_Get_Current_Time
 * description      :   This function will get the current system time in 
                        unsigned long form
                        
 * input parameters :   
 *                      
 * return type      :   unsigned long 

 * global variables :   -
 * calls            :   
 *****************************************************************************/

unsigned long Util_Get_Current_Time()
{
    UI32 value=0;
	time_t sttm;

    value=time(&sttm);
    
    return value;
}

/***********************IP Address Conversion*************************************/

/* This function will convert the IP address stored in asnFormat to
 * dotted format IP. Buff should contain sufficient memory for storing the
 * dotted decimal format IP */

void Util_GetIPFrom_ASNFormat(char *asnFormat, char *buff){
	sprintf(buff,"%d.%d.%d.%d",(0xff& asnFormat[0]),(0xff & asnFormat[1]),
								(0xff&asnFormat[2]),
								(0xff&asnFormat[3]));
}


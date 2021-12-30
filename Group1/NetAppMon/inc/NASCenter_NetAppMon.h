#ifndef __NASCENTER_NET_APP_MON_H__
#define __NASCENTER_NET_APP_MON_H__

/*****************************************************************************
 *
 * NASCenter_NetAppMon.h:
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      D. Sai Siva Kuamr
 *
 * description :
 *      This code does scheduleing of tasks.
 *
 * TODO:
 *      [ Things to be improved ]
 *
 * CM Details:
 *
 * $Author: saisk $
 * $Log: NASCenter_NetAppMon.h,v $
 * Revision 1.4  2003/12/04 06:28:17  saisk
 * Merged NASCenter_NetAppMon.c and .h with netApplicationMonitor.c and .h
 *
 * Revision 1.3  2003/11/25 15:33:53  saisk
 * 1. Sending File on Interval based or file size based is completed
 * 2. CopyFile function for Unix implemented
 *
 * Revision 1.2  2003/11/24 14:21:33  saisk
 * Updated
 *
 * Revision 1.1  2003/11/24 13:18:28  saisk
 * Renamed from  NASCenterNetAppMon.h to NASCenter_NetAppMon.h
 *
 * Revision 1.6  2003/11/21 05:42:01  saisk
 * New Place
 *
 * Revision 1.1  2003/11/21 04:53:01  saisk
 * Readme
 *
 * Revision 1.5  2003/11/18 13:26:28  saisk
 * Added Function to Write into Log File
 *
 * Revision 1.4  2003/11/17 14:46:11  saisk
 * 1. Removed un wanted code
 * 2. Changed IP as string from unsigned long as received from MIB
 *
 * Revision 1.3  2003/11/14 15:09:31  saisk
 * Integrated with NetSnmp and it is working
 *
 * Revision 1.2  2003/11/14 11:05:49  saisk
 * Updated
 *
 * Revision 1.1  2003/11/11 14:32:02  saisk
 * Getting Process name and CPU and Memory percent is done
 *
 * $State: Exp $
 * $Id: NASCenter_NetAppMon.h,v 1.4 2003/12/04 06:28:17 saisk Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/


/********************************* includes *********************************/
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
//#include "ctypes.h"
#include "NASCenter_Errors.h"


/********************************** macros *********************************/
#define MAX_REPETITIONS  6
#define IP_STR_SIZE      16 /*xxx.xxx.xxx.xxx*/
#define MAX_PROC_NAME    255 
#define NAM_DELIMITER    "  "
#define NAM_DEFAULT_LOG_FILE "NAMDefaultLog.log"
/******************************* exported types ****************************/
typedef struct
{
    unsigned int uiCPU ;
    unsigned int uiMem ;
    unsigned int uiInOctets;   
    unsigned int uiInUcastPkts;
    unsigned int uiOutOctets;   
    unsigned int uiOutUcastPkts;
}stSetDetails;

/************************* exported function prototypes ********************/

/*****************************************************************************
 *
 * name             :   InitNAM
 * description      :   Initialize NAM
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   
 * calls            :  
 */

void InitNAM(void);

/*****************************************************************************
 *
 * name             :   InitProcList
 * description      :   Initialize the Process List
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   
 * calls            :   
 */

void InitProcList(void);

/*****************************************************************************
 *
 * name             :   NAM_Retrieve_And_Set_Data
 * description      :   Retrieves and set data in to MIB
 * input parameters :   unsigned long, long
 * output parameters:   void
 * return type      :   void
 * global variables :   
 * calls            :   
 */

int NAM_Retrieve_And_Set_Data (unsigned long ulIndex, long interval);

/*****************************************************************************
 *
 * name             :   NAM_GetProcessDetails 
 * description      :   Gets Process Name, CPU Percent and MEM Percent from 
 *                      Nuri Enterprise Process Table
 * input parameters :   unsigned long, unsigned short
 * outputparameters :   
 * return type      :   ENASCenterError 
 * global variables :   
 * calls	        :     
 *                      
 *
 */

ENASCenterError NAM_GetProcessDetails(unsigned long ulServerIp, unsigned short uiPort);

/*****************************************************************************
 *
 * name             :   NAM_GetValue
 * description      :   Gets Process and Interface Details
 * input parameters :   char*,unsigned long,unsigned short,unsigned int
 * output parameters:   stSetDetails*
 * return type      :   ENASCenterError
 * global variables :   
 * calls            :   
 */

ENASCenterError NAM_GetValue(char* cpProcName,unsigned long ulServerIp, unsigned short uiPort, 
                             unsigned int uiIFIndex, stSetDetails *pstSetDet );

/*****************************************************************************
 *
 * name             :   NAM_ClearProcList
 * description      :   Clears memory of Process List
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   
 * calls            :   
 */

void NAM_ClearProcList(void);

/*****************************************************************************
 *
 * name             :   NAM_WriteToLogFile
 * description      :   Writes to Log file
 * input parameters :   char*, stSetDetails, BOOL
 * output parameters:   void
 * return type      :   void
 * global variables :   
 * calls            :   
 */

void NAM_WriteToLogFile(char* cpProcName, stSetDetails stDetails, BOOL bProcStatus);

/*****************************************************************************
 *
 * name             :   SendLogFileToManager
 * description      :   Sends Log file to Manager
 * input parameters :   unsigned int
 * output parameters:   void *
 * return type      :   void
 * global variables :   
 * calls            :   
 */

void SendLogFileToManager (unsigned int guiReg, void *pclientarg);

#endif /* __NASCENTER_NET_APP_MON_H__ */


/* end of NASCenter_NetAppMon.h */
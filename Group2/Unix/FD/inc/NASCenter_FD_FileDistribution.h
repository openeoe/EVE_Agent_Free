#ifndef __NASCENTER_FD_FILEDISTRIBUTION_H__
#define __NASCENTER_FD_FILEDISTRIBUTION_H__

/*****************************************************************************
 *
 * NASCenter_FD_FileDistribution.h
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Vijaigeetha C.
 *
 * description :
 *     NASCenter_FD_FileDistribution.h provides common includes,macros,and 
       external declarations/definitions of NASCenter_FD_FileDistribution.c
 *      
 *  
 *
 * CM Details:
 *
 * $Author: vijaige $
 * $Log: NASCenter_FD_FileDistribution.h,v $
 * Revision 1.10  2003/12/25 12:38:54  vijaige
 * Included MAXPATH and MINPATH
 *
 * Revision 1.9  2003/12/25 08:53:41  vijaige
 * Included MAXPATH and MINPATH
 *
 * Revision 1.8  2003/12/20 05:19:30  vijaige
 * change of task id to long
 *
 * Revision 1.7  2003/12/11 10:28:31  vijaige
 * after compiling in cvs
 *
 * Revision 1.6  2003/11/27 05:13:17  vijaige
 * after testing
 *
 * 
 * 
 * $Id: NASCenter_FD_FileDistribution.h,v 1.10 2003/12/25 12:38:54 vijaige Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/
/********************************** macros *********************************/
#define UDP_PORT 3000
#define TCP_PORT 3001
#define BUFSIZE	 1500
#define LOGSIZE 500
#define MAX_LOG 20
#define MAX_PARENT_FD   255
#define MIN_PARENT_FD   3
#define IGNORE_ALL_SIG  0
 /******************************* exported types ****************************/
typedef struct stFDDate
{
	I8 day[10];
	I16 date;
	I16 month;
	I16 year;
	I16 hr;
	I16 min;
	I16 sec;
	I16 flag; /*1 for immediate and 0 for date time */
}stFDDate;
typedef struct IPList
{
	char IP[16];
	struct IPList *next;
}IPList;
typedef struct stConfigInfo
{
	I8 *sourceIp;
	IPList *destIp;	
	I16 iactive;
	I16 inoofdestIP;
}stConfigInfo;

typedef struct FileDistInfoStruct 
{
	I8 *PackageIdentification;
	I8 *PackageName;	
	stFDDate Install_Schedule_DateTime; 
	stFDDate Distribution_schedule;
	I8 *InstallationCommand;
	I8 *InstallationPath;
	I16 InstallationRestart;
	I16 BandwidthConsumption;
	stConfigInfo config;	
	I8 *pcPackageBuffer; /*Pointer to the XML tree pointer which was received*/	
	I32 isize;
	I16 delFlag; /*This is for checking if installation completed or distribution 
				 completed and releasing the memory */
}FileDistInfoStruct;

typedef struct commonFDList
{
	I16 iflag; /*1 for Distribution 2 for installation */
	FileDistInfoStruct *fileInfo;
    long lTaskId; /*Task ID which will be go after scheduling it*/
	struct commonFDList *next;
}commonFDList;

typedef struct stagentInfoStruct
{
	I8 pcMgrIpAddr[20];
	I16 iMgrPort;
	I16 iTransferInterval;
	I8 pclogfilename[BUFSIZE];
}StAgent_InfoStruct;


typedef struct stLogFormat
{
	I16 inext;
	I8 *pcLogInfo[20];
}StLogFormat;
 /************************* exported function prototypes ********************/
void FD_SendLogFile();
void FD_InitLogView();
I16 FD_LogData(FileDistInfoStruct *pstFDInfo,I8 aaction[],I8 aresult[]);
I16 FD_GetAgentInfoTable();
void FD_GetLocalHostIp(char *);
void Util_GetIPFrom_ASNFormat(char *, char *);
#ifndef WIN32
void Common_SignalIgnore(int);
#endif


 /***************************** exported globals ****************************/



StAgent_InfoStruct gstagentInfo;
struct snmp_session *agentSess;
void *tSess;
FileDistInfoStruct stFileDistList;
commonFDList *pstHeadptr;


StLogFormat stLogView;
pthread_mutex_t log_lock;
FILE *ifdForAM;

#endif /* __NASCENTER_FD_FileDistribution_H__ */

/* end of NAScenter_FD_FileDistribution.h*/

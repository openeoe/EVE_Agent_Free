/*****************************************************************************
 *
 * NASCenter_FD_logstatus.c
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Vijaigeetha C.
 *
 * description :
 *      
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
 * $Log: NASCenter_FD_logstatus.c,v $
 * Revision 1.16  2004/01/06 08:29:24  vijaige
 * log file should be put in the current path
 *
 * Revision 1.15  2003/12/29 06:23:39  vijaige
 * Fixed some code review comments
 *
 * Revision 1.14  2003/12/23 11:53:27  vijaige
 * Fixed a memory leak
 *
 * Revision 1.13  2003/12/22 06:32:22  vijaige
 * removed warnings
 *
 * Revision 1.12  2003/12/20 05:19:44  vijaige
 * change of task id to long
 *
 * Revision 1.11  2003/12/16 05:50:55  vijaige
 * check for fp==NULL
 *
 * Revision 1.10  2003/12/12 07:48:18  vijaige
 * After replacing fputs with fprintf in FD_WriteLog
 *
 * Revision 1.9  2003/12/11 12:16:37  vijaige
 * fixed sleep
 *
 * Revision 1.8  2003/12/10 09:55:26  karthis
 * After porting to Unix
 *
 * Revision 1.7  2003/12/10 07:38:44  vijaige
 * 10/12/2003 with unix porting
 *
 * Revision 1.6  2003/12/02 12:07:41  vijaige
 * after removing printf
 *
 * 
 * 
 * $Id: NASCenter_FD_logstatus.c,v 1.16 2004/01/06 08:29:24 vijaige Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/
#ifdef WIN32
#include <Winsock2.h>
#include <windows.h>
#include <Lmcons.h>
#else
#include <arpa/inet.h>
#include <time.h>
#endif


#include <stdio.h>
#include <pthread.h>
/*NetSnmp includes */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "ctypes.h"


#include "NASCenter_FileTrans.h"
#include "NASCenter_FD_FileDistribution.h"

/********************* prototypes for local functions ***********************/
void FD_WriteLog();

/*****************************************************************************
 *
 * name             :   FD_InitLogView
 * description      :   This function will initialize the logView structure
 
 * input parameters :   
 *                      
 * return type      :   None
 * global variables :   -
 * calls            :   
 */
void FD_InitLogView()
{
	stLogView.inext=0;
	pthread_mutex_init(&log_lock, NULL);
}
 
 /*****************************************************************************
 *
 * name             :   FD_SendLogFile
 * description      :   This function will transfer the logfile to the manager
                        at specified intervals 
 * input parameters :   
 *                      
 * return type      :   None
 * global variables :   -
 * calls            :   
 */

void FD_SendLogFile()
{
	struct sockaddr_in sock_addr;
   #ifdef WIN32
	WSADATA wsadata;
   #endif
	I16 error;
	while(1)
	{
#ifdef WIN32
		Sleep(gstagentInfo.iTransferInterval * 1000);
#else

		sleep(gstagentInfo.iTransferInterval);
#endif
		pthread_mutex_lock(&log_lock);
		FD_WriteLog();
		pthread_mutex_unlock(&log_lock);
#ifdef WIN32
		WSAStartup(MAKEWORD( 2, 2 ),&wsadata);
#endif
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_port = htons(gstagentInfo.iMgrPort);
		sock_addr.sin_addr.s_addr = inet_addr(gstagentInfo.pcMgrIpAddr);    
#ifdef WIN32
		error = FileTrans_SendFile ( gstagentInfo.pclogfilename , &sock_addr , NULL , 512);  
#else
        error = FileTrans_SendFile ( gstagentInfo.pclogfilename , &sock_addr , NULL , 512);  
#endif
		if ( error != NASC_SUCCESS)
		{
			puts("Failed");
		}
		else 
		{
			puts ("Succeeded");
		}
#ifdef WIN32
		WSACleanup();
#endif
		FD_GetAgentInfoTable();
	}
}

 /*****************************************************************************
 *
 * name             :   FD_LogData
 * description      :   This function will log the status in the memory mapped 
                        file
 * input parameters :   tempappMet
 *                      
 * return type      :   int
 * global variables :   -
 * calls            :   
 */
I16 FD_LogData(FileDistInfoStruct *pstFDInfo,I8 aaction[],I8 aresult[])
{
	time_t ultime;
	I8 acbuf[LOGSIZE];
	I16 iretval=0;
	
	I8 acSysIp[16];
        I8 *pctemp;

    printf("Inside FD_LogData\n");
	time( &ultime );
	FD_GetLocalHostIp(acSysIp);
	pctemp=ctime(&ultime);
	pctemp[24]='\0';
	if(pstFDInfo->PackageIdentification==NULL)
		sprintf(acbuf,"%s;;%s;%s;%s\n",pctemp,
								acSysIp,
								aaction,
								aresult);    
	else
		sprintf(acbuf,"%s;%s;%s;%s;%s\n",pctemp,
						pstFDInfo->PackageIdentification,
								acSysIp,
								aaction,
								aresult);    
    pthread_mutex_lock(&log_lock);	
	stLogView.pcLogInfo[stLogView.inext]=(char *) malloc(strlen(acbuf)+1);
	if(stLogView.pcLogInfo[stLogView.inext]==NULL)
		return NASC_FAILURE;
	memset(stLogView.pcLogInfo[stLogView.inext],0,(strlen(acbuf)+1));	
	strcpy(stLogView.pcLogInfo[stLogView.inext],acbuf);	
	stLogView.inext++;
	
	if(stLogView.inext==MAX_LOG)
	{
		FD_WriteLog();				
		
	}
    pthread_mutex_unlock(&log_lock);
	printf("Outside FD_LogData\n");

	if(iretval==0)
		return NASC_FAILURE;
	else
		return NASC_SUCCESS;

}

/*****************************************************************************
 *
 * name             :   FD_WriteLog
 * description      :   This function will write the log in to the physical 
                        file
 * input parameters :   
 *                      
 * return type      :   None
 * global variables :   -
 * calls            :   
 */
void FD_WriteLog()
{
	I16 i;
	FILE *fp;		
	fp=fopen(gstagentInfo.pclogfilename,"a+");
	if(fp==NULL)
	{
		for(i=0;i<stLogView.inext;i++)
			free(stLogView.pcLogInfo[i]);
		stLogView.inext=0;	
		return ;
	}
	for(i=0;i<stLogView.inext;i++)
	{
				
		fprintf(fp,"%s",stLogView.pcLogInfo[i]);
		//fputs(stLogView.pcLogInfo[i],fp);				
		free(stLogView.pcLogInfo[i]);
	}
	fclose(fp);		
	stLogView.inext=0;	
}




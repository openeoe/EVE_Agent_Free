/****************************************************************************
 *
 * NASCenter_AM_logstatus.c
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
 * $Author: vijaysp $
 * $Log: NASCenter_AM_logstatus.c,v $
 * Revision 1.10  2003/12/30 10:43:31  vijaysp
 * HPUX core problem fixed
 *
 * Revision 1.9  2003/12/29 09:55:11  karthis
 * removed special characters in printf for code review
 *
 * Revision 1.8  2003/12/29 05:19:14  karthis
 * Debug flag AM_DEBUG has been included karthi
 *
 * Revision 1.6  2003/12/22 03:09:17  karthis
 * AM Solaris updated memory leak fixed
 *
 * Revision 1.5  2003/12/18 09:02:50  testteam
 * link list modified
 *
 * Revision 1.2  2003/12/17 05:47:44  testteam
 * final version
 *
 * Revision 1.1  2003/11/20 05:37:42  karthis
 * minor
 *
 * Revision 1.9  2003/11/19 06:52:45  vijaige
 * 19th Nov trap and all working
 *
 * 
 * 
 * $Id: NASCenter_AM_logstatus.c,v 1.10 2003/12/30 10:43:31 vijaysp Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/
#include <arpa/inet.h>
#include <stdio.h>
#include <pthread.h>



/*NetSnmp includes */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "ctypes.h"

#include "NASCenter_AM_applicationmetering.h"
#include "NASCenter_AlertMessage.h"
#include "NASCenter_Errors.h"
#include "NASCenter_FileTrans.h"

/********************* prototypes for local functions ***********************/
void AM_WriteLog();

/*****************************************************************************
 *
 * name             :   AM_InitLogView
 * description      :   This function will initialize the logView structure
 
 * input parameters :   
 *                      
 * return type      :   None
 * global variables :   -
 * calls            :   
 */
void AM_InitLogView()
{
	stLogView.inext=0;
#ifdef AM_DEBUG
        printf("\nMutex Intialization\n");
#endif
	pthread_mutex_init(&log_lock, NULL);
}
 
 /*****************************************************************************
 *
 * name             :   AM_SendLogFile
 * description      :   This function will transfer the logfile to the manager
                        at specified intervals 
 * input parameters :   
 *                      
 * return type      :   None
 * global variables :   -
 * calls            :   
 */

void AM_SendLogFile()
{
	struct sockaddr_in sock_addr;
    #ifdef AM_DEBUG
        printf("Calling AM_SendLogFile\n");
    #endif
	sleep(gstagentInfo.iTransferInterval);
    #ifdef AM_DEBUG
        printf("\nAM_SendLogFile mutex lock\n");
    #endif
	pthread_mutex_lock(&log_lock);
	AM_WriteLog();
    #ifdef AM_DEBUG
        printf("\nAM_SendLogFile Mutex Unlock\n");
    #endif
	pthread_mutex_unlock(&log_lock);

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(gstagentInfo.iMgrPort);
	sock_addr.sin_addr.s_addr = inet_addr(gstagentInfo.pcMgrIpAddr);    

	FileTrans_SendFile ( gstagentInfo.pclogfilename , &sock_addr ,NULL, 512);  
	AM_GetAgentInfoTable();

}

 /*****************************************************************************
 *
 * name             :   AM_LogData
 * description      :   This function will log the status in the memory mapped 
                        file
 * input parameters :   tempappMet
 *                      
 * return type      :   int
 * global variables :   -
 * calls            :   
 */
I16 AM_LogData(StappMeteringStruct *psttempappMet)
{
	time_t ultime;
	I8 acbuf[LOGSIZE];
	I16 iretval=0;
	static I16 count=0;	
    #ifdef AM_DEBUG
    printf("Inside AM_LogData\n");
    #endif
	time( &ultime );    	
	
	sprintf(acbuf,"%s;%d;%s;%s;%d\n",ctime(&ultime),psttempappMet->uiappMeteringIndex,
								psttempappMet->pcappMeteringUsername,
								psttempappMet->pcappMeteringBinaryName,
								psttempappMet->iappMeteringReaction);    
#ifdef AM_DEBUG
  printf("\nAM_LogData Mutex Lock\n");
#endif
    pthread_mutex_lock(&log_lock);
	stLogView.pcLogInfo[stLogView.inext]=(char *) malloc(strlen(acbuf)+1);
        if(stLogView.pcLogInfo[stLogView.inext]==NULL)
	{
		printf("Memory Error\n");
		pthread_mutex_unlock(&log_lock);
		return;
	}	 
	memset(stLogView.pcLogInfo[stLogView.inext],'\0',(strlen(acbuf)+1));
	strcpy(stLogView.pcLogInfo[stLogView.inext],acbuf);	
	stLogView.inext++;
	
	if(stLogView.inext==MAX_LOG)
	{
		AM_WriteLog();
		
	}
#ifdef AM_DEBUG
   printf("\nAM_LogData Mutex UnLock\n");
#endif
    pthread_mutex_unlock(&log_lock);
#ifdef AM_DEBUG
	printf("Outside AM_LogData\n");
#endif

	if(iretval==0)
		return NASC_FAILURE;
	else
		return NASC_SUCCESS;

}

/*****************************************************************************
 *
 * name             :   AM_WriteLog
 * description      :   This function will write the log in to the physical 
                        file
 * input parameters :   
 *                      
 * return type      :   None
 * global variables :   -
 * calls            :   
 */
void AM_WriteLog()
{
	I16 i,j;
	FILE *fp;
    #ifdef AM_DEBUG
        printf("\n\n\nInside write Log....\n\n");
    #endif
          
		fp=fopen(gstagentInfo.pclogfilename,"a+");
	for(i=0;i<stLogView.inext;i++)
	{
		if(fp==NULL)
		{
        #ifdef AM_DEBUG
                 printf("\n\nFile Error returning\n");    
        #endif
		  for(j=0;j<stLogView.inext;j++)
		  {

                if(stLogView.pcLogInfo[j])
		free(stLogView.pcLogInfo[j]);
		}

			stLogView.inext = 0;
			return;
		}
        #ifdef AM_DEBUG
                printf("\n\n\nvalue to be written in to the file:%s\n\n",stLogView.pcLogInfo[i]); 
        #endif
		fprintf(fp,"%s\n",stLogView.pcLogInfo[i]);
                fflush(fp);
		free(stLogView.pcLogInfo[i]);
	}
	stLogView.inext=0;	
        fclose(fp);
}


void AM_AddMsgToLogfile(UI8 *logMsg)
{
   fprintf(ifdForAM,"%s",logMsg );	
}

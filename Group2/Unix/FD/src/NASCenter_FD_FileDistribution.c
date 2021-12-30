/*****************************************************************************
 *
 * NASCenter_FD_FileDistribution.c
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
 * $Log: NASCenter_FD_FileDistribution.c,v $
 * Revision 1.40  2004/01/13 10:46:49  vijaige
 * After running purify
 *
 * Revision 1.39  2004/01/12 10:49:32  vijaige
 * fixed error log when receving failed
 *
 * Revision 1.38  2004/01/08 08:28:47  vijaige
 * after fixing handle leaks
 *
 * Revision 1.37  2004/01/08 05:49:58  vijaige
 * after releasing xmlDoc free
 *
 * Revision 1.36  2004/01/08 05:11:17  vijaige
 * Close Handle after create process success
 *
 * Revision 1.35  2004/01/02 12:04:51  vijaige
 * fixed bug for transferring the udp file
 *
 * Revision 1.34  2004/01/01 08:47:37  vijaige
 * intallation not happening in the installation path
 *
 * Revision 1.33  2004/01/01 06:10:57  vijaige
 * Fix for build error
 *
 * Revision 1.32  2004/01/01 05:07:53  vijaige
 * Fixed bug for starting the next day
 *
 * Revision 1.31  2003/12/30 11:11:38  vijaige
 * memset for logfile name
 *
 * Revision 1.30  2003/12/29 06:23:29  vijaige
 * Fixed some code review comments
 *
 * Revision 1.29  2003/12/25 13:25:43  vijaige
 * errno commented for windows
 *
 * Revision 1.28  2003/12/25 12:39:07  vijaige
 * Included MAXPATH and MINPATH
 *
 * Revision 1.27  2003/12/25 08:52:51  vijaige
 * Included Signal Handling
 *
 * Revision 1.25  2003/12/23 11:20:56  karthis
 * Fix for putting in download path
 *
 * Revision 1.24  2003/12/23 10:34:01  karthis
 * Fix for xml parsing in XP
 *
 * Revision 1.23  2003/12/23 06:44:12  vijaige
 * Fix for first node empty
 *
 * Revision 1.22  2003/12/23 04:58:04  karthis
 * Change made in Move next node
 *
 * Revision 1.21  2003/12/20 05:19:44  vijaige
 * change of task id to long
 *
 * Revision 1.20  2003/12/17 11:42:28  vijaige
 * Changed the Filetrans send package
 *
 * Revision 1.19  2003/12/16 09:10:28  vijaige
 * ; removed for #define
 *
 * Revision 1.18  2003/12/16 09:07:37  vijaige
 * ; removed for #define
 *
 * Revision 1.17  2003/12/16 05:06:06  vijaige
 * Fixed the hardcoded path problem
 *
 * Revision 1.16  2003/12/16 04:07:00  karthis
 * After changing package path in Unix
 *
 * Revision 1.15  2003/12/16 04:03:31  karthis
 * After changing package path in Unix
 *
 * Revision 1.14  2003/12/15 13:23:24  vijaige
 * changed the installation path
 *
 * Revision 1.13  2003/12/15 09:27:49  vijaige
 * After changing delflag =1
 *
 * Revision 1.12  2003/12/11 08:24:42  vijaige
 * after compiling in cvs
 *
 * Revision 1.9  2003/12/10 07:38:44  vijaige
 * 10/12/2003 with unix porting
 *
 * Revision 1.8  2003/12/02 12:07:41  vijaige
 * after removing printf
 *
 * Revision 1.2  2003/11/01 12:19:34  vijaige
 * FileDistribution source files
 *
 * 
 * 
 * $Id: NASCenter_FD_FileDistribution.c,v 1.40 2004/01/13 10:46:49 vijaige Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/

 /********************************* includes *********************************/
#ifdef WIN32
 #include <windows.h>
 #include <winioctl.h>
 #include <process.h>
 #include <conio.h>
 #include <Lmcons.h>
 #include <psapi.h>
#include <direct.h>
/*LibXml includes */
#include <libxml/tree.h>
#include <libxml/parser.h>
#else
/*LibXml includes */
#include <gnome-xml/tree.h>
#include <gnome-xml/parser.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <arpa/inet.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#endif
#include <string.h> 
#include <stdio.h>
#include <pthread.h>

/*NetSnmp includes */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "AgentInfo.h"
#include "ctypes.h"
#include "NASCenter_Errors.h"
#include "NASCenter_FileTrans.h"
#include "NASCenter_FD_FileDistribution.h"
#include "NASCenter_Scheduler.h"
#include "NASCenter_FD_TagStrings.h"



/************************ globals within this module*************************/

UI8 gcNASSACommunity[]="public";
UI8 gcAgentSessPeerName[] = "localhost";
I16 giNASSAPort=161;
#ifdef WIN32
I8 cPackagePath[MAX_PATH];
#else
#define MAX_PATH  1024
I8 cPackagePath[MAX_PATH];
#endif
//I8 cPackagePath[]="";


#define IPADDRLEN 16

#ifndef WIN32
#define children childs
#define stricmp strcasecmp
#define closesocket close
#endif

I32    iUDPSocketFd;
I32    iTCPSocketFd;

	
/********************* prototypes for local functions ***********************/
void FD_StartFileDistribution();
void FD_RecvConfig();
I16 FD_AddJobToSched(FileDistInfoStruct *);
void FD_PerformAction(void *);
void FD_DoScheduledFunction(void *);
void FD_InstallPackage(FileDistInfoStruct *);
commonFDList * FD_AddNodeInCommonFDList(FileDistInfoStruct *,I16 );
I16 FD_SendConfig(FileDistInfoStruct *);
I16 FD_SendPackage(FileDistInfoStruct *,IPList * );
void FD_RecvPkg();
void HandleRequest (void * stThreadParm);
I16 FD_OpenUDPSocket();
void FD_InitSession();
void FD_DeleteNodeInCommonFDList(	commonFDList *pstFDList);
I16 FD_ParseXMLData(char *,FileDistInfoStruct *);
xmlNode * MoveNextNode(xmlNode *);
void FD_DeleteDisInfo(FileDistInfoStruct *);
void FD_CheckCurrenttime(FileDistInfoStruct *pstFDInfo);
void main()
{ 

	FD_StartFileDistribution();
}

/*****************************************************************************
 *
 * name             :   FD_StartFileDistribution
 * description      :   Here a SNMP session is opened to get the agent info with 
						the NASCenter subagent
 * input parameters :   None
 *                      
 * return type      :   None 
 * global variables :   -
 * calls            :   FD_InitSession,FD_GetAgentInfoTable
						
 *****************************************************************************/
void FD_StartFileDistribution()
{
	
	pthread_t stlogthread;
	pthread_t strecvpkgthread;	
	pthread_t strecvconfigthread;	
	pthread_t schedThread;
	I16 iretval;
	void * pstatus;	
	

#ifdef FD_DEBUG
    printf("Before Starting the scheduler \n");
#endif
   /*Start the scheduler */
	SCH_StartScheduler();
#ifdef FD_DEBUG
	printf("Scheduler started \n");
#endif 

	if( getcwd( cPackagePath, MAX_PATH ) == NULL )
	{
      perror( "_getcwd error" );
	  return;
	}
#ifdef WIN32
    strcat(cPackagePath,"\\download\\");
#else
	strcat(cPackagePath,"/download/");
#endif

   /*Session Initiation with the nascenter agent*/
	FD_InitSession();     	
	
	pthread_create(&schedThread , NULL , (void *)SCH_StartScheduler , NULL);
	
	/*Retrieve the agent information MIB information */
	iretval=FD_GetAgentInfoTable();
	if(iretval==NASC_SUCCESS)
	{
		FD_InitLogView(&stLogView);
		pthread_create(&stlogthread , '\0' , (void *) FD_SendLogFile , NULL);
	}    
    pstHeadptr=(commonFDList *) malloc(sizeof(commonFDList));
	pstHeadptr->next=NULL;
    

    // Kick off our threads and wait forever...
    pthread_create(&strecvconfigthread ,'\0' , (void *) FD_RecvConfig ,NULL);
	pthread_create(&strecvpkgthread ,'\0' , (void *) FD_RecvPkg ,NULL);
	/*pthread_join(stlogthread, &pstatus);*/
	pthread_join(strecvpkgthread, &pstatus);	
	pthread_join(strecvconfigthread, &pstatus);	

	SOCK_CLEANUP;
}

/*****************************************************************************
 *
 * name             :   FD_RecvPkg
 * description      :   This is the thread function that receives the actual 
						package info. 
                        
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/
void FD_RecvPkg()
{
	struct sockaddr_in sock_addr;

#ifdef WIN32
	WSADATA wsadata;
	WSAStartup(MAKEWORD( 2, 2 ),&wsadata);
#endif
	sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = htons(UDP_PORT);
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      /*inet_pton(AF_INET , argv[1] , &sock_addr.sin_addr);*/
       FileTrans_RecvFile ( &sock_addr);
#ifdef WIN32
      WSACleanup();
#endif   
}


/*****************************************************************************
 *
 * name             :   FD_AddJobToSched
 * description      :   This function schedules receives the package 						
                        
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/
I16 FD_AddJobToSched(FileDistInfoStruct *pstFDInfo)
{
	I8 *pcbuf=NULL;
	stTask task; 	
	long lTaskId;
	I16 iFlag;
	commonFDList *pstFDList=NULL;
	FILE *fp;
	I16 iLen; 
	


#ifdef FD_DEBUG
     printf("Inside Adding Job to Scheduler\n");
     printf("pstFDInfo->Distribution_schedule.flag: %d\n",pstFDInfo->Distribution_schedule.flag);
#endif	
	if(pstFDInfo->Distribution_schedule.flag==1)
	{
#ifdef FD_DEBUG
    printf("Inside Adding job to scheduler for distribution :%s\n",cPackagePath);
#endif

		iLen = strlen(pstFDInfo->PackageName)+strlen(cPackagePath)+2;
		pcbuf=(char *)malloc(iLen+1);
		memset(pcbuf,0,iLen+1);
		strcpy(pcbuf,cPackagePath);
		strcat(pcbuf,pstFDInfo->PackageName);
#ifdef FD_DEBUG
    printf("Inside Add_Job for distribution-pcbuf :%s\n",pcbuf);
#endif

		/*Check if the package is present in the current directory */
		if((fp=fopen(pcbuf,"r"))==NULL)
		{
#ifdef FD_DEBUG
    printf("Inside Opening File failed\n");
#endif

			FD_LogData(pstFDInfo,"sending","failed");
			if(pcbuf)
				free(pcbuf);
			//if(pstFDInfo->delFlag==1)
				FD_DeleteDisInfo(pstFDInfo);
			/*else
				pstFDInfo->delFlag=1;*/
			return NASC_FAILURE;
		}
		else
		{
#ifdef FD_DEBUG
    printf("File found\n");
#endif

			if(pcbuf)
				free(pcbuf);
			fclose(fp);					
			FD_SendConfig(pstFDInfo);		
		}
		

	}
	else
	{
		

	/*Schedule for Distribution*/
		FD_CheckCurrenttime(pstFDInfo);

		task.StartTime.ui16Hour   =   pstFDInfo->Distribution_schedule.hr;
		task.StartTime.ui16Minute =   pstFDInfo->Distribution_schedule.min ;
		task.StartTime.ui16Second =   pstFDInfo->Distribution_schedule.sec;
		task.StartDate.ui16Day    =   pstFDInfo->Distribution_schedule.date;
		task.StartDate.ui16Month  =   pstFDInfo->Distribution_schedule.month;
		task.StartDate.ui16Year   =   pstFDInfo->Distribution_schedule.year;

		task.EndTime.ui16Hour   =   pstFDInfo->Distribution_schedule.hr;
		task.EndTime.ui16Minute =   pstFDInfo->Distribution_schedule.min ;
		task.EndTime.ui16Second =   pstFDInfo->Distribution_schedule.sec;
		task.EndDate.ui16Day    =   pstFDInfo->Distribution_schedule.date;
		task.EndDate.ui16Month  =   pstFDInfo->Distribution_schedule.month;
		/*The year is incremented to 1 because we need a end time for the scheduler*/
		task.EndDate.ui16Year   =   (pstFDInfo->Distribution_schedule.year +1); 
		task.eType              =   SCH_ONE_SHOT; 
		task.ePCity             =   SCH_DAY; 
        task.eWeekDay           =   SCH_ALL_DAYS; 

		iFlag=1;		
		pstFDList=FD_AddNodeInCommonFDList(pstFDInfo,iFlag);
		if(pstFDList==NULL)
		{
#ifdef FD_DEBUG
			printf("error while adding node\n");
#endif
		}
		task.vpArg = (void*)pstFDList;
		task.fp = FD_PerformAction;
		lTaskId = SCH_AddTask(task);           	
		if(lTaskId ==0)
		{
#ifdef FD_DEBUG
			 printf("Error occured while adding the task to scheduler \n ");	   
#endif 
		}
		pstFDList->lTaskId = lTaskId;
		
	}
	if(pstFDInfo->Install_Schedule_DateTime.flag==1)
	{
#ifdef FD_DEBUG
printf("Inside Install Immediately\n");
#endif
		FD_InstallPackage(pstFDInfo);	

	}
	else
	{
		

	/*Schedule for Distribution*/
		FD_CheckCurrenttime(pstFDInfo);
		
		task.StartTime.ui16Hour   =   pstFDInfo->Install_Schedule_DateTime.hr;
		task.StartTime.ui16Minute =   pstFDInfo->Install_Schedule_DateTime.min ;
		task.StartTime.ui16Second =   pstFDInfo->Install_Schedule_DateTime.sec;
		task.StartDate.ui16Day    =   pstFDInfo->Install_Schedule_DateTime.date;
		task.StartDate.ui16Month  =   pstFDInfo->Install_Schedule_DateTime.month;
		task.StartDate.ui16Year   =   pstFDInfo->Install_Schedule_DateTime.year; 

		task.EndTime.ui16Hour   =   pstFDInfo->Install_Schedule_DateTime.hr;
		task.EndTime.ui16Minute =   pstFDInfo->Install_Schedule_DateTime.min ;
		task.EndTime.ui16Second =   pstFDInfo->Install_Schedule_DateTime.sec;
		task.EndDate.ui16Day    =   pstFDInfo->Install_Schedule_DateTime.date;
		task.EndDate.ui16Month  =   pstFDInfo->Install_Schedule_DateTime.month;
		task.EndDate.ui16Year   =   pstFDInfo->Install_Schedule_DateTime.year+1; 
		task.eType              =   SCH_ONE_SHOT; 
		task.ePCity             =   SCH_DAY; 
        task.eWeekDay           =   SCH_ALL_DAYS; 
		
		iFlag=2;
		pstFDList=FD_AddNodeInCommonFDList(pstFDInfo,iFlag);
		if(pstFDList==NULL)
		{
#ifdef FD_DEBUG
			printf("error while adding node\n");
#endif
		}
		task.vpArg = (void*)pstFDList;
		task.fp = FD_PerformAction;
		lTaskId = SCH_AddTask(task);           	
		if(lTaskId ==0)
		{
#ifdef FD_DEBUG
			 printf("Error occured while adding the task to scheduler \n ");	  
#endif  
		}
		
		pstFDList->lTaskId = lTaskId;
	}
	return NASC_SUCCESS;

}

/*****************************************************************************
 *
 * name             :   FD_PerformAction
 * description      :   This function will be called by the scheduler when the 
						scheduled time is reached
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/
void FD_PerformAction(void *args)
{
     pthread_t jobThread;

     /*Create a thread to perform the Job action*/
     pthread_create(&jobThread,NULL,(void*)FD_DoScheduledFunction,args);
    
}
/*****************************************************************************
 *
 * name             :   FD_DoSheduledFunction
 * description      :   This function will be do the distritribution or  
						installation based on the flag value
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/
void FD_DoScheduledFunction(void *pstList)
{
	FileDistInfoStruct *pstFDInfo;
	commonFDList *pstFDList;
	I32 iLen;
	I8 *pcbuf;
	FILE *fp;

	pstFDList =(commonFDList *) pstList;
	pstFDInfo = pstFDList->fileInfo;
#ifdef FD_DEBUG
printf("Inside do Job Schedule\n");
#endif
	if(pstFDList->iflag==1)
	{
		/*Check if the package is present if not log error and do not send the config */
		iLen = strlen(pstFDInfo->PackageName)+strlen(cPackagePath)+2;
		pcbuf=(char *)malloc(iLen+1);
		memset(pcbuf,0,iLen+1);
		strcpy(pcbuf,cPackagePath);
		strcat(pcbuf,pstFDInfo->PackageName);
		/*Check if the package is present in the current directory */
		if((fp=fopen(pcbuf,"r"))==NULL)
		{
			FD_LogData(pstFDInfo,"sending","failed");
			if(pcbuf)
				free(pcbuf);
			if(pstFDInfo->delFlag==1)
				FD_DeleteDisInfo(pstFDInfo);
			else
				pstFDInfo->delFlag=1;			
		}
		else
		{
			if(pcbuf)
				free(pcbuf);
			fclose(fp);							
			FD_SendConfig(pstFDInfo);
		}
	}
	else
	{
#ifdef FD_DEBUG
printf("Inside Perform Action-Before calling InstallPackage\n");
#endif
		FD_InstallPackage(pstFDInfo);
	}
	FD_DeleteNodeInCommonFDList(pstFDList);
	pthread_detach(pthread_self());
    pthread_exit(NULL);

}

/*****************************************************************************
 *
 * name             :   FD_DeleteNodeInCommonFDList
 * description      :   This function will delete the Node in the common FD List  						
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/
void FD_DeleteNodeInCommonFDList(commonFDList *pstFDList)
{
	commonFDList *psttempList;
	commonFDList *pstprevList;	
	pstprevList = pstHeadptr;
	
	
	for(psttempList = pstHeadptr->next;psttempList!=NULL;
					psttempList=psttempList->next,pstprevList=pstprevList->next)
	{
		if(psttempList->lTaskId==pstFDList->lTaskId)
		{			
			pstprevList->next=psttempList->next;						
			free(psttempList);
			break;
		}
	}
}

/*****************************************************************************
 *
 * name             :   FD_DeleteDisInfo
 * description      :   This function will delete the FileDistInfoStruct  						
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/
void FD_DeleteDisInfo(FileDistInfoStruct *pstDisInfo)
{
	IPList *psttemp;
	IPList *pstnext;
	if(pstDisInfo->PackageIdentification)
		free(pstDisInfo->PackageIdentification);
	if(pstDisInfo->PackageName)
		free(pstDisInfo->PackageName);
	if(pstDisInfo->InstallationCommand)
		free(pstDisInfo->InstallationCommand);
	if(pstDisInfo->InstallationPath)
		free(pstDisInfo->InstallationPath);
	if(pstDisInfo->config.sourceIp)
		free(pstDisInfo->config.sourceIp);
	for(pstnext=psttemp = pstDisInfo->config.destIp;pstnext!=NULL;psttemp=pstnext)
	{
		pstnext=psttemp->next;
		free(psttemp);
	}
	if(pstDisInfo->pcPackageBuffer)
		free(pstDisInfo->pcPackageBuffer);
	if(pstDisInfo)
		free(pstDisInfo);
}
/*****************************************************************************
 *
 * name             :   FD_InstallPackage
 * description      :   This function Installs the package
                        
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/
void FD_InstallPackage(FileDistInfoStruct *pstFDInfo)
{
  #ifdef WIN32
	I16 iRetval;
 #else
     pid_t iRetval;
     int *status;
     I8 dir[1024];
     int fds;
 #endif	
	I8 *pctar=NULL;
#ifdef WIN32
	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo; 
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );  
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO);
#else
     
     char *environ[1];
#endif
     if((pctar=(char *) malloc(strlen(pstFDInfo->PackageName)+strlen(cPackagePath) +15))==NULL)
     {
       FD_LogData(pstFDInfo,"installation","failed");
                if(pstFDInfo->delFlag==1)
                        FD_DeleteDisInfo(pstFDInfo);
                else
                        pstFDInfo->delFlag = 1;
                return;

        }
#ifdef WIN32
	strcpy(pctar,"nttar -xvf ");
#else
        strcpy(pctar,"tar -xvf ");
#endif
	strcat(pctar,cPackagePath);
	strcat(pctar,pstFDInfo->PackageName);	
	/*strcat(pctar," -C ");	
	strcat(pctar, pstFDInfo->InstallationPath);*/	


#ifdef FD_DEBUG
    printf("Inside InstallPackage pcTar :%s\n",pctar);
#endif

	if(system(pctar)<0)
	{
#ifdef FD_DEBUG
    printf("Inside Install Package - system(pcTar)<0\n");
#endif

		FD_LogData(pstFDInfo,"installation","failed");
		if(pstFDInfo->delFlag==1)
			FD_DeleteDisInfo(pstFDInfo);
		else
			pstFDInfo->delFlag = 1;
		return;

	}
#ifndef WIN32
#ifdef FD_DEBUG
    printf("Inside InstallPackage before creating child Process\n");
#endif
	
        //argv[0] = "sh";
        //argv[1] = "-c";
        Common_SignalIgnore(SIGCHLD); 
	iRetval=fork();
#ifdef FD_DEBUG
    printf("Inside InstallPackage RetVal = %d\n",iRetval);
#endif

        
         
        if(iRetval == -1){
#ifdef FD_DEBUG
    printf("Inside InstallPackage - create child process retval is -ve\n");
#endif

	    FD_LogData(pstFDInfo,"installation","failed");
            FD_LogData(pstFDInfo,"installation","failed");
                if(pstFDInfo->delFlag==1)
                        FD_DeleteDisInfo(pstFDInfo);
                else
                        pstFDInfo->delFlag = 1;
                return;

        }
        if(iRetval == 0)
        {
          fds = MIN_PARENT_FD;
          for(fds=3;fds<MAX_PARENT_FD;fds++)
                     close (fds);
         

           chdir(pstFDInfo->InstallationPath);
          #ifdef FD_DEBUG 
            printf("Inside Install package: before execve:%s\n",pstFDInfo->InstallationPath);
          #endif 

           //argv[2] = pstFDInfo->InstallationCommand;
         
          // argv[3] = 0;
		  Common_SignalIgnore(IGNORE_ALL_SIG);
         // if(execv("/bin/sh",argv) != 0){
		  if(execl("/bin/sh","sh","-c",pstFDInfo->InstallationCommand,0)!=0){

          #ifdef FD_DEBUG 
            printf("Insidie Install package: After execve\n");
          #endif 
          _exit(1);
          }
          #ifdef FD_DEBUG 
            printf("Insidie Install package: After execve\n");
          #endif
        }//end of if
       
#ifdef FD_DEBUG
    printf("Inside InstallPackage - After coming out of execve\n");
#endif
#else
	iRetval=CreateProcess(pstFDInfo->InstallationCommand, // name of executable module
						NULL,                      // command line string
						NULL, // SD
						NULL,  // SD
						FALSE,                      // handle inheritance option
						0,						
						NULL,                      // new environment block
						pstFDInfo->InstallationPath,                // current directory name
						&siStartInfo,&piProcInfo
						);
	if(iRetval==0)
	{
		FD_LogData(pstFDInfo,"installation","failed");
	}
	else
	{
		FD_LogData(pstFDInfo,"installation","sucess");		
		CloseHandle( piProcInfo.hProcess );
		CloseHandle( piProcInfo.hThread );
	}
	//free(pcbuf);
#endif
 if (pctar!=NULL){
#ifdef FD_DEBUG
    printf("Inside Free pctar\n");
#endif
         free(pctar);
}
#ifdef FD_DEBUG
    printf("After InstallPackage return Success\n");
#endif


	if(pstFDInfo->delFlag==1){
#ifdef FD_DEBUG
    printf("After InstallPackage Inside pst-Delflg\n");
#endif

		FD_DeleteDisInfo(pstFDInfo);
}
	else
		pstFDInfo->delFlag = 1;


}
/*****************************************************************************
 *
 * name             :   FD_AddNodeInCommonFDList
 * description      :   This function adds the pstFDInfo in the commonFDList
                        
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/

commonFDList * FD_AddNodeInCommonFDList(FileDistInfoStruct *pstFDInfo,I16 iFlag)
{
	commonFDList *psttempList;	
	for(psttempList=pstHeadptr;psttempList->next!=NULL;psttempList=psttempList->next);	
	psttempList->next=(commonFDList *) malloc(sizeof(commonFDList));
	if(psttempList->next==NULL)
		return NULL;
	psttempList = psttempList->next;
	psttempList->iflag = iFlag;	
	psttempList->fileInfo = pstFDInfo;
	psttempList->next = NULL;
	return psttempList;

}

/*****************************************************************************
 *
 * name             :   FD_SendConfig
 * description      :   This function send the config info over TCP socket
                        
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/

I16 FD_SendConfig(FileDistInfoStruct *pstFDInfo)
{
	struct sockaddr_in addr;
	I32 iclientFD;
	I16 iRetval;
	IPList *pstIp;
	I16 inoofpacket;
	I16 iextrapacket=0;
	I8 *pctemp;
	I16 i;
	I16 inwrite;	
#ifdef FD_DEBUG
printf("Inside SendConfig ipAddress=%s\n",pstFDInfo->config.destIp->next->IP);
#endif	
	for(pstIp=pstFDInfo->config.destIp->next;pstIp!=NULL;pstIp=pstIp->next)
	{
#ifdef FD_DEBUG
printf("Before calling SendPackage in SendConfig\n");
#endif         
		FD_SendPackage(pstFDInfo,pstIp);
#ifdef FD_DEBUG
printf("Inside for of SendConfig\n");
#endif
		iclientFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(iclientFD == -1) {
			perror("Could not create socket");
			return NASC_FAILURE;
		}
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(pstIp->IP);
		addr.sin_port=htons(TCP_PORT);
		iRetval = connect(iclientFD, (struct sockaddr *)&addr, sizeof(struct sockaddr));
		if(iRetval == -1) {
#ifdef FD_DEBUG
printf("Inside SendConfig-iRetVal ==-1\n");
#endif
			perror("Cannot connect to server");
			closesocket(iclientFD);
			FD_LogData(pstFDInfo,"sending","failed");
			continue;
		}
		inoofpacket = pstFDInfo->isize/BUFSIZE;
		pctemp = pstFDInfo->pcPackageBuffer;
        iextrapacket = pstFDInfo->isize%BUFSIZE;
		for(i=1;i<inoofpacket;i++)
		{
			if((inwrite=send(iclientFD, pctemp,BUFSIZE,0))<=0)
			{
				FD_LogData(pstFDInfo,"sending","failed");
				//closesocket(iclientFD);				
				break;
			}
			pctemp=pctemp + BUFSIZE;

		}
		if(iextrapacket)/*Fix for if strlen turns to be zero*/
		{
			if((inwrite=send(iclientFD, pctemp,strlen(pctemp),0))<=0)
			{
				FD_LogData(pstFDInfo,"sending","failed");
				closesocket(iclientFD);
				continue;
			}
		}
		closesocket(iclientFD);
      
		
	}

	if(pstFDInfo->delFlag==1)
		FD_DeleteDisInfo(pstFDInfo);
	else
		pstFDInfo->delFlag = 1;
	return NASC_SUCCESS;

}

/*****************************************************************************
 *
 * name             :   FD_SendPackage
 * description      :   This function send the package over UDP socket
                        
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/
I16 FD_SendPackage(FileDistInfoStruct *pstFDInfo,IPList *pstIp)
{
	struct sockaddr_in sock_addr;

    I16 error;
	I32 iLen;
	I8 *pcbuf=NULL;
#ifdef WIN32	
	WSADATA wsadata;
	WSAStartup(MAKEWORD( 2, 2 ),&wsadata);
#endif	
#ifdef FD_DEBUG
printf("Inside SendPackage\n");
#endif     
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(UDP_PORT);
	sock_addr.sin_addr.s_addr = inet_addr(pstIp->IP);    
	iLen = strlen(pstFDInfo->PackageName)+strlen(cPackagePath)+2;
	pcbuf=(char *)malloc(iLen+1);
	memset(pcbuf,0,iLen+1);
	strcpy(pcbuf,cPackagePath);
	strcat(pcbuf,pstFDInfo->PackageName);
#ifdef WIN32
    error = FileTrans_SendFile ( pcbuf , &sock_addr ,
				".\\download", pstFDInfo->BandwidthConsumption);
#else
	error = FileTrans_SendFile ( pcbuf , &sock_addr ,
				"./download", pstFDInfo->BandwidthConsumption);
#endif

    if ( error != NASC_SUCCESS)
    {
        FD_LogData(pstFDInfo,"sending","failed");
    }
	else		
		FD_LogData(pstFDInfo,"sending","success");
	if(pcbuf)
		free(pcbuf);
#ifdef WIN32
	WSACleanup();
#endif	
	return error;
}
/*****************************************************************************
 *
 * name             :   FD_RecvConfig
 * description      :   This is the thread function that receives the configuration
						info and schedules in the scheduler 						
                        
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/
void FD_RecvConfig()
{
	I16 iRetval;
#ifdef WIN32 
        WSADATA wsadata;
#endif	
	struct sockaddr_in sock_addr;	
	struct sockaddr_in strecv_addr;
	I32 iTCPclientFd;
	I32 iTCPSocketFd;
	pthread_t thread;
	socklen_t len = sizeof(struct sockaddr_in);
#ifdef WIN32
	WSAStartup(MAKEWORD( 2, 2 ),&wsadata);	
#endif	
	sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = htons(TCP_PORT);
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    /*inet_pton(AF_INET , argv[1] , &sock_addr.sin_addr);*/
    iTCPSocketFd = socket( AF_INET , SOCK_STREAM , 0 );
    if(iTCPSocketFd == -1) 
	{
        perror("Could not create socket");
        return ;
    }

    iRetval = bind ( iTCPSocketFd , (struct sockaddr *)&sock_addr , sizeof(struct sockaddr));
	if(iRetval == -1) 
	{
        perror("Could not bind to local socket");
        closesocket(iTCPSocketFd);
        return; 
    }
    
	while(1)
	{
		iRetval = listen(iTCPSocketFd, SOMAXCONN);
		if(iRetval == -1) 
		{
			perror("Could not listen to local socket");
			closesocket(iTCPSocketFd);
			return ;
		}
	
	
		iTCPclientFd = accept(iTCPSocketFd, (struct sockaddr*)&strecv_addr, &len);
	
		if(iTCPclientFd == -1) 
		{
#ifndef WIN32
                     if ( errno == EINTR)
                     {
                         continue;
                     }
#endif
			perror("Could not accept new connection");
			//closesocket(iTCPSocketFd);
			//break;
                        continue;
		}
		pthread_create (&thread , NULL , 
                           (void *) HandleRequest , 
                           (void *) &iTCPclientFd);	
#ifdef FD_DEBUG
    printf("Last line of Recv Config\n");
#endif

	}

#ifdef WIN32
	WSACleanup();
#endif
    
}


	

/*****************************************************************************
 *
 * name             :   HandleRequest
 * description      :   This function will handle a TCP request connection 
						
                        
 * input parameters :   none
 *                      
 * return type      :   I16
 * global variables :   -
 * calls            :   
 ******************************************************************************/
void HandleRequest (void * stThreadParm)
{
	FileDistInfoStruct *stFDInfo=NULL;
	I32 iTCPclientFd;	
	I8 acBuffer[BUFSIZE];	
	I16 readLen;
	I16 iretval;
	I16 iLen=0;
	I8 *pctemp=NULL;	
	I8 *pcFDval=NULL;	
	I32 iFDTotalSize=0;
	I16 iFlag=0;
	I16 iRetVal=0;
	fd_set iSocketFDList;


	memset(acBuffer,0,BUFSIZE);	
	iTCPclientFd = *((int *)stThreadParm);	
	
	FD_ZERO(&iSocketFDList);

    FD_SET(iTCPclientFd , &iSocketFDList);

	if(stFDInfo == NULL)
	{
		stFDInfo = (FileDistInfoStruct *) malloc(sizeof(FileDistInfoStruct)); 
		if(stFDInfo==NULL)
		{
			closesocket(iTCPclientFd);
#ifdef FD_DEBUG
			printf("Error in malloc\n");
#endif
			pthread_detach(pthread_self());
			pthread_exit(NULL);
			return;

		}
		memset(stFDInfo,'\0',sizeof(FileDistInfoStruct));
	}
	stFDInfo->delFlag = 0;
	stFDInfo->PackageIdentification=NULL;
	stFDInfo->PackageName=NULL;
	stFDInfo->InstallationPath=NULL;
	stFDInfo->pcPackageBuffer=NULL;
	stFDInfo->InstallationCommand=NULL;

	stFDInfo->config.destIp=(IPList *)malloc(sizeof(IPList));	
	stFDInfo->config.destIp->next=NULL;
	stFDInfo->config.sourceIp=NULL;
	stFDInfo->Distribution_schedule.flag=1;
	
	while(1)
	{
		iRetVal = select( iTCPclientFd + 1 , &iSocketFDList , NULL, NULL,
										NULL ) ;
		if(iRetVal <0)
		{
#ifdef FD_DEBUG
			printf("Error : \n" );
#endif
			FD_LogData(stFDInfo,"received","failure");
			break;
		}
		memset(acBuffer,'\0',sizeof(acBuffer));
		readLen=recv(iTCPclientFd, acBuffer, BUFSIZE-1,0);
		 if ( readLen < 0)
		 {
	#ifdef FD_DEBUG		
           printf("Error : \n" );
#endif
		   FD_LogData(stFDInfo,"received","failure");
			break;

		 }
		if(readLen > 0)
		{
			acBuffer[readLen]='\0';
#ifdef FD_DEBUG
			printf("%s\n",acBuffer);
#endif
			iLen=strlen(acBuffer);			
			iFDTotalSize = iFDTotalSize + iLen;
				
			if(pcFDval==NULL)					
			{
				pcFDval = (char *)malloc(iFDTotalSize+1);
				memset(pcFDval,'\0',iFDTotalSize+1);
				strcpy(pcFDval,acBuffer);					
			}
			else
			{
				pcFDval = (char *)realloc(pcFDval, (iFDTotalSize+1));					
				strcat(pcFDval,acBuffer);					
			}				
			pctemp = pcFDval;
			pctemp = pctemp + (iFDTotalSize-11);
			if(stricmp(pctemp,"</FileDist>")==0)			
				iretval=FD_ParseXMLData(pcFDval,stFDInfo);				
			else
			{
				readLen=0;
				iRetVal=0;
				continue;
			}

			if(iretval==NASC_FAILURE)
			{	
#ifdef FD_DEBUG				
				printf("Inside Calling Parse Data\n");
#endif
                FD_LogData(stFDInfo,"received","failure");
				break;
			}
			else
			{
				iFlag=1;
				FD_LogData(stFDInfo,"received","sucess");				
				break;
			}
		
		memset(acBuffer,'\0',BUFSIZE);
		
		}	
		readLen	= 0;
		iRetVal = 0;
	}//end of while(1)
	
	if(iFlag==1)
	{			
		FD_AddJobToSched(stFDInfo);
#ifdef FD_DEBUG
printf("After Adding Job to Scheduler\n");
#endif
	}
	else
	{
		if(pcFDval!=NULL)
			free(pcFDval);
		if(stFDInfo)
			FD_DeleteDisInfo(stFDInfo);
	}
	closesocket(iTCPclientFd);
#ifdef FD_DEBUG
    printf("Last Line of Handle Request\n");
#endif
	pthread_detach(pthread_self());
    pthread_exit(NULL);

}

/*****************************************************************************
 *
 * name             :   FD_ParseXMLData
 * description      :   This function Parses the XML data and fills the 
						FileDistInfoStruct
                        
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/
I16 FD_ParseXMLData(char *pcBuffer,FileDistInfoStruct *pstFDInfo)
{
	xmlDocPtr doc = NULL;
	xmlNode *rootNode = NULL;
        xmlNode *currentNode = NULL;
	xmlNode *prevNode = NULL;
	IPList *tempList;
	I32 i;
	I16 iFlag=0;
	I8 acSysIp[16];
	LIBXML_TEST_VERSION;
	i=strlen(pcBuffer);
	FD_GetLocalHostIp(acSysIp);
	doc = xmlParseMemory(pcBuffer,i);
	if(doc==NULL)
	{
#ifdef FD_DEBUG
               printf("Inside Doc Failed\n");	
#endif	
		return NASC_FAILURE;
	}
#ifdef FD_DEBUG
       printf("Inside Parse Data: After Doc and before rootNode\n");
#endif
	rootNode = xmlDocGetRootElement(doc);
	currentNode = rootNode->children;
#ifdef FD_DEBUG
       printf("Current Node Name %s\n",currentNode->name);
#endif
#ifdef WIN32	
	while(currentNode->type != XML_ELEMENT_NODE && currentNode->next!=NULL)
		currentNode=currentNode->next;
#endif
#ifdef FD_DEBUG
printf("After moving from Current Node Name %s\n",currentNode->name); 
#endif
	if((stricmp(currentNode->name,gpcPackageID)==0) && 
						(currentNode->children!=NULL))
	{
		pstFDInfo->PackageIdentification=(char *)malloc(strlen(currentNode->children->content)+1);
                memset(pstFDInfo->PackageIdentification,0,
                       strlen(currentNode->children->content)+1); 
		strcpy(pstFDInfo->PackageIdentification,currentNode->children->content);
#ifdef FD_DEBUG
    printf("Inside XMLParser Package Identification Value :%s\n",pstFDInfo->PackageIdentification);
#endif

	}
	else
	{
#ifdef FD_DEBUG
              printf("Inside Else of xmlParseData of currentNode->name\n");
#endif
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return NASC_FAILURE;
	}
	currentNode = MoveNextNode(currentNode);
	if((stricmp(currentNode->name,gpcPackName)==0) && 
						(currentNode->children!=NULL))
	{
		pstFDInfo->PackageName=(char *)malloc(strlen(currentNode->children->content)+1);
                memset(pstFDInfo->PackageName,0,
                       strlen(currentNode->children->content)+1); 
		strcpy(pstFDInfo->PackageName,currentNode->children->content);
#ifdef FD_DEBUG
    printf("Inside XMLParser Package Name Value :%s\n",pstFDInfo->PackageName);
#endif

	}
	else
	{
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return NASC_FAILURE;
	}
	
	currentNode = MoveNextNode(currentNode);
#ifdef FD_DEBUG
    printf("Inside XMLParser Package Description :%s\n",currentNode->name);
#endif
	if((stricmp(currentNode->name,gpcPackDesc)==0))
	{
		currentNode = MoveNextNode(currentNode);
	}
	else
	{
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return NASC_FAILURE;
	}
#ifdef FD_DEBUG
    printf("Inside XMLParser PackageCreateDate :%s\n",currentNode->name);
#endif
	if((stricmp(currentNode->name,gpcPacCreateDt)==0)) 
	{
		currentNode = MoveNextNode(currentNode);
	}
	else
	{
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return NASC_FAILURE;
	}	
	//prevNode = currentNode;

	/*For Retreiving the Packaging schedule */

	prevNode = currentNode;
#ifdef FD_DEBUG
    printf("Inside XMLParser Schedule name :%s\n",currentNode->name);
    printf("Inside XMLParser gpPkgSched :%s\n",gpPkgShed);
#endif
	if((stricmp(currentNode->name,gpPkgShed))==0)	
	{
#ifdef FD_DEBUG
    printf("Inside XMLParser Schedule Value :%s\n",currentNode->properties->name);
#endif
		if((stricmp(currentNode->properties->name,gpcShed))==0)
		{
#ifdef WIN32
			if((stricmp(currentNode->properties->children->content,"Immediate"))==0)
			pstFDInfo->Distribution_schedule.flag=1;
			
                       if((stricmp(currentNode->properties->children->content,"Datetime"))==0)
#else
#ifdef FD_DEBUG
    printf("Inside XMLParser properties->val->content Value :%s\n",currentNode->properties->val->content);
#endif
			if((stricmp(currentNode->properties->val->content,"Immediate"))==0)
			pstFDInfo->Distribution_schedule.flag=1;

#ifdef FD_DEBUG
    printf("Inside XMLParser Distribution Schedule Flag :%d\n",pstFDInfo->Distribution_schedule.flag);
#endif  


                       if((stricmp(currentNode->properties->val->content,"Datetime"))==0)
#endif
			

			{
#ifdef FD_DEBUG
    printf("Inside XMLParser Date and time Present\n");
#endif
				pstFDInfo->Distribution_schedule.flag=0;
				currentNode = currentNode->children;
#ifdef WIN32
				while(currentNode->type != XML_ELEMENT_NODE && currentNode->next!=NULL)
					currentNode=currentNode->next;
#endif
				if((stricmp(currentNode->name,gpcdate))==0)
					pstFDInfo->Distribution_schedule.date=atoi(currentNode->children->content);
#ifdef FD_DEBUG
    printf("Inside XMLParser Distribution Schedule Date :%d\n",pstFDInfo->Distribution_schedule.date);
#endif

				currentNode = MoveNextNode(currentNode);
				if((stricmp(currentNode->name,gpcmonth))==0)
					pstFDInfo->Distribution_schedule.month=atoi(currentNode->children->content);
				currentNode = MoveNextNode(currentNode);
				if((stricmp(currentNode->name,gpcyear))==0)
					pstFDInfo->Distribution_schedule.year=atoi(currentNode->children->content);
				currentNode = MoveNextNode(currentNode);
				if((stricmp(currentNode->name,gpchr))==0)
					pstFDInfo->Distribution_schedule.hr=atoi(currentNode->children->content);
				currentNode = MoveNextNode(currentNode);
				if((stricmp(currentNode->name,gpcmin))==0)
					pstFDInfo->Distribution_schedule.min=atoi(currentNode->children->content);
				currentNode = MoveNextNode(currentNode);
				if((stricmp(currentNode->name,gpcsec))==0)
					pstFDInfo->Distribution_schedule.sec=atoi(currentNode->children->content);			
			}

		}
	}
	else
	{
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return NASC_FAILURE;
	}
	currentNode=prevNode;
	currentNode = MoveNextNode(currentNode);

	/*For Retreiving the Installation schedule */
       prevNode = currentNode;
#ifdef FD_DEBUG
    printf("Inside XMLParser Schedule Name after Date and time:%s\n",currentNode->name);
#endif
#ifdef FD_DEBUG
    printf("Inside XMLParser gpcInstaShed  :%s\n",gpcInstaShed);
#endif


	if((stricmp(currentNode->name,gpcInstaShed))==0)
	{
		if((stricmp(currentNode->properties->name,gpcShed))==0)
		{
#ifdef WIN32
			if((stricmp(currentNode->properties->children->content,"Immediate"))==0)
				pstFDInfo->Install_Schedule_DateTime.flag=1;
			if((stricmp(currentNode->properties->children->content,"Datetime"))==0)
#else

			if((stricmp(currentNode->properties->val->content,"Immediate"))==0)
				pstFDInfo->Install_Schedule_DateTime.flag=1;
			if((stricmp(currentNode->properties->val->content,"Datetime"))==0)
#endif
			{
				pstFDInfo->Install_Schedule_DateTime.flag=0;
				currentNode = currentNode->children;
#ifdef WIN32
				while(currentNode->type != XML_ELEMENT_NODE && currentNode->next!=NULL)
					currentNode=currentNode->next;
#endif
				if((stricmp(currentNode->name,gpcdate))==0)
					pstFDInfo->Install_Schedule_DateTime.date=atoi(currentNode->children->content);
				currentNode = MoveNextNode(currentNode);
				if((stricmp(currentNode->name,gpcmonth))==0)
					pstFDInfo->Install_Schedule_DateTime.month=atoi(currentNode->children->content);
				currentNode = MoveNextNode(currentNode);
				if((stricmp(currentNode->name,gpcyear))==0)
					pstFDInfo->Install_Schedule_DateTime.year=atoi(currentNode->children->content);
				currentNode = MoveNextNode(currentNode);
				if((stricmp(currentNode->name,gpchr))==0)
					pstFDInfo->Install_Schedule_DateTime.hr=atoi(currentNode->children->content);
				currentNode = MoveNextNode(currentNode);
				if((stricmp(currentNode->name,gpcmin))==0)
					pstFDInfo->Install_Schedule_DateTime.min=atoi(currentNode->children->content);
				currentNode = MoveNextNode(currentNode);
				if((stricmp(currentNode->name,gpcsec))==0)
					pstFDInfo->Install_Schedule_DateTime.sec=atoi(currentNode->children->content);
				
			}

		}
	}
	else
	{
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return NASC_FAILURE;
	}
	currentNode=prevNode;
	currentNode = MoveNextNode(currentNode);
	
	if((stricmp(currentNode->name,gpccomm)==0) && 
						(currentNode->children!=NULL))
	{
		pstFDInfo->InstallationCommand=(char *)malloc(strlen(currentNode->children->content)+1);
                memset(pstFDInfo->InstallationCommand,0,
                       strlen(currentNode->children->content)+1); 
		strcpy(pstFDInfo->InstallationCommand,currentNode->children->content);
#ifdef FD_DEBUG
    printf("Inside XMLParser Installation Command :%s\n",pstFDInfo->InstallationCommand);
#endif

	}
	else
	{
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return NASC_FAILURE;
	}

	currentNode = MoveNextNode(currentNode);
	
	if((stricmp(currentNode->name,gppath)==0) && 
						(currentNode->children!=NULL))	
	{
		pstFDInfo->InstallationPath=(char *)malloc(strlen(currentNode->children->content)+1);
                memset(pstFDInfo->InstallationPath,0,
                       strlen(currentNode->children->content)+1); 
		strcpy(pstFDInfo->InstallationPath,currentNode->children->content);
#ifdef FD_DEBUG
    printf("Inside XMLParser Installation Path :%s\n",pstFDInfo->InstallationPath);
#endif

	}
	else
	{
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return NASC_FAILURE;
	}
	currentNode = MoveNextNode(currentNode);
	if((stricmp(currentNode->name,gpband))==0)
	{
		pstFDInfo->BandwidthConsumption=atoi(currentNode->children->content);
#ifdef FD_DEBUG
    printf("Inside XMLParser BAndwidth consumption :%d\n",pstFDInfo->BandwidthConsumption);
#endif
		
	}
	else
	{
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return NASC_FAILURE;
	}
	currentNode = MoveNextNode(currentNode);
#ifdef FD_DEBUG
    printf("Inside XMLParser currentNode of distpts :%s\n",currentNode->name);
#endif
#ifdef FD_DEBUG
    printf("Inside XMLParser gpdistPts :%s\n",gpdistPts);
#endif


	if((stricmp(currentNode->name,gpdistPts))==0)
	{
		prevNode = currentNode;		
		currentNode = currentNode->children;
#ifdef WIN32
		while(currentNode->type != XML_ELEMENT_NODE && currentNode->next!=NULL)
			currentNode=currentNode->next;
#endif
		while(!(stricmp(currentNode->name,gpdiststartPt))&&iFlag==0)
		{
			prevNode = currentNode;		
#ifdef WIN32		
			while(!(stricmp(currentNode->properties->children->content,acSysIp))&& iFlag==0)

#else
/*#ifdef FD_DEBUG
    printf("Inside XMLParser SysIP :%s\n",currentNode->properties->val->content);
#endif
#ifdef FD_DEBUG
    printf("Inside XMLParser acsysIP :%s\n",acSysIp);
#endif*/

				
			
                        while(!(stricmp(currentNode->properties->val->content,acSysIp))&& iFlag==0)
#endif
			{			
				pstFDInfo->config.sourceIp=(char *)malloc(strlen(acSysIp)+1);
#ifdef WIN32				
                      strcpy(pstFDInfo->config.sourceIp,currentNode->properties->children->content);
#else

                      strcpy(pstFDInfo->config.sourceIp,currentNode->properties->val->content);
#ifdef FD_DEBUG
    printf("Inside XMLParser Config Source IP :%s\n",pstFDInfo->config.sourceIp);
#endif

#endif
				
				if((stricmp(currentNode->properties->next->name,gpactive))==0)
				{
#ifdef FD_DEBUG
    printf("Inside XMLParser properties.name :%s\n",currentNode->properties->next->name);
#endif
#ifdef WIN32

					currentNode = currentNode->properties->next;

					if((stricmp(currentNode->children->content,"Continuous"))==0)
	
					pstFDInfo->config.iactive=1;
					
					if((stricmp(currentNode->children->content,"Stop"))==0)					

						pstFDInfo->config.iactive=0;
				}
#else //For Unix
                         

#ifdef FD_DEBUG
    printf("Inside XMLParser After moving currentnode->properties->val->content %s\n",currentNode->properties->next->val->content);
#endif

					if((stricmp(currentNode->properties->next->val->content,"Continuous"))==0)
#ifdef FD_DEBUG
    printf("Inside XMLParser:currentNode->val->content :%s\n",currentNode->properties->next->val->content);
#endif
	
					pstFDInfo->config.iactive=1;
#ifdef FD_DEBUG
    printf("Inside XMLParser config.iactive :%d\n",pstFDInfo->config.iactive);
#endif

					
					if((stricmp(currentNode->properties->next->val->content,"Stop"))==0)					
						pstFDInfo->config.iactive=0;
				}
#endif
				currentNode = prevNode;
				currentNode = currentNode->children;
#ifdef WIN32
				while(currentNode->type != XML_ELEMENT_NODE && currentNode->next!=NULL)
					currentNode=currentNode->next;
#endif
#ifdef FD_DEBUG
    printf("Inside XMLParser After moving current node -1 \n");   
#endif

				for(tempList = pstFDInfo->config.destIp;tempList->next!=NULL;tempList=tempList->next);
				if((stricmp(currentNode->name,gpiplist))==0)
				{
					currentNode = currentNode->children;				
#ifdef WIN32
					while(currentNode->type != XML_ELEMENT_NODE && currentNode->next!=NULL)
						currentNode=currentNode->next;
#endif
#ifdef FD_DEBUG
    printf("Inside XMLParser After Current Node\n");
#endif

					while(!(stricmp(currentNode->name,gpip)))
					{
#ifdef FD_DEBUG
    printf("Inside XMLParser-currentNode->name inside while :%s\n",currentNode->name);
#endif
						if(!stricmp(currentNode->children->content,acSysIp)) 
						{
							/*This check is for cyclic file distribution */
							if((currentNode = MoveNextNode(currentNode))==NULL)
								break;
						}

						tempList->next=(IPList *)malloc(sizeof(IPList)); 
						tempList=tempList->next;
						tempList->next=NULL;					
						strcpy(tempList->IP,currentNode->children->content);	
#ifdef FD_DEBUG
    printf("Inside XMLParser-currentNode->children->content :%s\n",currentNode->children->content);
#endif
				
						if((currentNode = MoveNextNode(currentNode))==NULL)
							break;
#ifdef FD_DEBUG
    printf("After moving to the next node\n");
#endif

					}
				}				
				else
				{
					xmlFreeDoc(doc);
					xmlCleanupParser();
					return NASC_FAILURE;
				}
				iFlag=1;
				currentNode = prevNode;
				break;
			}
			if(iFlag==1)
				break;
			currentNode = prevNode;
		        if((currentNode = MoveNextNode(currentNode))==NULL)
                               break;
			continue;
#ifdef FD_DEBUG
    printf("After Continue \n");
#endif
		

		}
			
	}
	else
	{
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return NASC_FAILURE;
	}

#ifdef FD_DEBUG
    printf("Before iFlag\n");
#endif

	if(iFlag==0)
		return NASC_FAILURE;
	pstFDInfo->pcPackageBuffer = pcBuffer;
	pstFDInfo->isize = strlen(pcBuffer);
#ifdef FD_DEBUG
    printf("Inside XMLParser Success\n");
#endif
    xmlFreeDoc(doc);
	xmlCleanupParser();
	return NASC_SUCCESS;

}
		
xmlNode * MoveNextNode(xmlNode *tempNode)
{
	if(tempNode->next==NULL)		
            return NULL;
	else
		tempNode=tempNode->next;

	while(tempNode->type != XML_ELEMENT_NODE && tempNode->next!=NULL)
		tempNode=tempNode->next;
	
	return tempNode;
}

/*****************************************************************************
 *
 * name             :   FD_OpenUDPSocket
 * description      :   This function will open a UDP socket for receiving the 
						package.
                        
 * input parameters :   none
 *                      
 * return type      :   I16
 * global variables :   -
 * calls            :   
 ******************************************************************************/

I16 FD_OpenUDPSocket()
{
	struct sockaddr_in sock_addr;    

	sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(UDP_PORT);
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    FileTrans_RecvFile ( &sock_addr);
	return NASC_SUCCESS;


}

/*****************************************************************************
 *
 * name             :   FD_InitSession
 * description      :   For Initiating the session with the manager. Two session
						is created one for trap and one for get and set. The trap 
						session listens on port 20                        
                        
 * input parameters :   none
 *                      
 * return type      :   none
 * global variables :   -
 * calls            :   
 ******************************************************************************/
void FD_InitSession()
{
   struct snmp_session tempSess;
   

   /* For Session Initiation */
   snmp_sess_init(&tempSess);
   tempSess.version = SNMP_VERSION_2c;
   tempSess.peername = gcAgentSessPeerName;
   //tempSess.local_port = 200;
   /*tempSess.remote_port = giNASSAPort;*/
   tempSess.community = gcNASSACommunity;
   tempSess.community_len = strlen(gcNASSACommunity);
   tempSess.callback_magic = NULL;

   
 
   SOCK_STARTUP;

   agentSess = snmp_open(&tempSess);/*This session is for GET/SET */

   if(agentSess == NULL) 
   {
#ifdef FD_DEBUG
       printf("Error opening Session\n");
       printf("Error opening Session\n");
#endif
       snmp_perror("NASCenterAM Snmp Open: ");
       exit(1);
   }
}


/*****************************************************************************
 *
 * name             :   FD_GetAgentInfoTable
 * description      :   This function will get the Agent info table and store 
                        the ManagerIp address, ManagerPort, Transferinterval 
						and the logfilename in global variables
 * input parameters :   Session sessionid;
 *                      
 * return type      :   0 ' if success
						-1 ' if failed
 * global variables :   agentSess
 * calls            :   
 *****************************************************************************/
I16 FD_GetAgentInfoTable()
{

	struct snmp_pdu *pstreqPDU;
    struct snmp_pdu *pstrespPDU;
    netsnmp_variable_list *pstCurrVars;
    char FileTransferIP[IPADDRLEN];

    I16 status=0;
    I16 iLen=0;  

    
               
	pstreqPDU = snmp_pdu_create(SNMP_MSG_GET);

       
    /*Get the Relevant Fields*/
    
       /*Get the IP Address */
    iLen=sizeof( agentInfoFileTransferIP_oid)/sizeof(oid); 
    snmp_add_null_var(pstreqPDU,agentInfoFileTransferIP_oid,iLen);
      
      /*Get the File Tranfer Interval */ 
    iLen=sizeof(agentInfoFileTransferInterval_oid)/sizeof(oid); 
    snmp_add_null_var(pstreqPDU,agentInfoFileTransferInterval_oid,iLen);

       /*Get the port number*/
    iLen=sizeof(agentInfoFileTransferPortNo_oid)/sizeof(oid); 
    snmp_add_null_var(pstreqPDU,agentInfoFileTransferPortNo_oid,iLen);

       /*Get the Log File Name*/
    iLen=sizeof(agentInfoLogFileDistribution_oid)/sizeof(oid);
    snmp_add_null_var(pstreqPDU,agentInfoLogFileDistribution_oid,iLen);

       /*Get the response in the respPDU*/
    status = snmp_synch_response(agentSess,pstreqPDU,&pstrespPDU);	
    if(status==STAT_SUCCESS)
    {
	   if(pstrespPDU->errstat==SNMP_ERR_NOERROR && 
           pstrespPDU->errindex==0 && 
           pstrespPDU->variables->type != ASN_NULL &&
           pstrespPDU->variables->type != SNMP_NOSUCHINSTANCE &&
           pstrespPDU->variables->type != SNMP_NOSUCHOBJECT )
		{
#ifdef FD_DEBUG
	          printf("sync response success \n");	  
#endif 
		}else
		{
#ifdef FD_DEBUG
	          printf("sync response failure \n");	  
#endif 
                  return NASC_FAILURE;		   
		}
        if(pstrespPDU == NULL)	        
		{
#ifdef FD_DEBUG
			printf("Error in sync response \n");		   
#endif
			return NASC_FAILURE;
		}
	} else 
    {
#ifdef FD_DEBUG
		printf("Error in snmp_synch_response \n");
#endif
        snmp_perror("Error ");   	   
        return NASC_FAILURE;		   
    }

    pstCurrVars = pstrespPDU->variables;

       /*Get the IP Address*/
	iLen =0;
	
	if(pstCurrVars->val.string !=NULL)
	{
		iLen =  pstCurrVars->val_len;
		memcpy(FileTransferIP,pstCurrVars->val.string,iLen);
		Util_GetIPFrom_ASNFormat(FileTransferIP,gstagentInfo.pcMgrIpAddr);
		pstCurrVars = pstCurrVars->next_variable;
	}	
	else
		return NASC_FAILURE;

    /*Get the File Tranfer Interval */ 
	if(pstCurrVars->val.integer !=NULL)
	{
		gstagentInfo.iTransferInterval =(I16)(*pstCurrVars->val.integer);
		pstCurrVars = pstCurrVars->next_variable;
	}
	else
		return NASC_FAILURE;
       
       /*Get the port number */
	if(pstCurrVars->val.integer != NULL)
	{
		gstagentInfo.iMgrPort=(I16)(*pstCurrVars->val.integer);
		pstCurrVars = pstCurrVars->next_variable;
	}
	else
		return NASC_FAILURE;

    /*Get the backup Log file */
    iLen =0;
	if(pstCurrVars->val.string!=NULL)
	{
		iLen =  pstCurrVars->val_len;
		memset(gstagentInfo.pclogfilename,'\0',BUFSIZE);
		memcpy(gstagentInfo.pclogfilename,pstCurrVars->val.string,iLen);       
	}

    return NASC_SUCCESS;
}

/*****************************************************************************
 *
 * name             :   FD_GetLocalHostIp
 * description      :   This function is used to get the IP address of the local
                        host machine
 * input parameters :   None
 * output parameters:   None
 * return type      :   char * - returns the IP Address string
 * global variables :   None
 * calls            :   None
 */
 
void FD_GetLocalHostIp(char *pcSysIp)
{
    struct sockaddr_in stInAddr; // Common variable for Windows & Unix
    struct hostent *pstHostEnt;
 
#ifndef WIN32 // For Unix
 
    struct utsname stHostName;
 
    // Get the localhost name
    uname (&stHostName);
#ifdef FD_DEBUG
    printf("Inside GetLocalhost Name: %s\n",stHostName.nodename);
#endif
    if ((pstHostEnt = gethostbyname(stHostName.nodename)) == NULL)
    {
        snmp_log_perror("error resolving hostname..");
        return;
    }
 
#else  // For windows
 
   WSADATA wsadata;
    char* stHostName;
    int namelen = 50;
 
    stHostName = (char*) malloc(namelen);
 
    WSAStartup(MAKEWORD( 2, 2 ),&wsadata);
 
    if (gethostname(stHostName,namelen)!=0)
    {
        snmp_log_perror("error resolving hostname..");
        return;
    }
	if ((pstHostEnt = gethostbyname(stHostName)) == NULL)
		return; 
#endif
 
    // Common code for Windows & Unix
    memcpy(&stInAddr.sin_addr, pstHostEnt->h_addr_list[0], pstHostEnt->h_length);
#ifdef FD_DEBUG
    //printf("Inside GetHost sinaddr :%s\n",stInAddr.sin_addr);
#endif
 
    // Convert it to IP string
    strcpy(pcSysIp,(char*)inet_ntoa(stInAddr.sin_addr));
 #ifdef FD_DEBUG
 printf("LogMonRow_GetLocalHostIp %s\n",pcSysIp); 
#endif
 
 

    return;
 
}

/*****************************************************************************
 *
 * name             :   FD_CheckCurrentTime
 * description      :   This function is used check the current system time and if 
						the time has exceeded then increment the time by a day
 * input parameters :   FileDistInfoStruct *pstFDInfo
 * output parameters:   FileDistInfoStruct *pstFDInfo
 * return type      :   None
 * global variables :   None
 * calls            :   None
 */
void FD_CheckCurrenttime(FileDistInfoStruct *pstFDInfo) 
{
	I32 year=0;
	time_t ulStartTime;
	time_t ulCurrTime;
	struct tm tm = { 0 };
	struct tm *newtime;
	
    tm.tm_year = pstFDInfo->Distribution_schedule.year - 1900;

    tm.tm_mon  = pstFDInfo->Distribution_schedule.month-1;
    tm.tm_mday = pstFDInfo->Distribution_schedule.date;
    tm.tm_hour = pstFDInfo->Distribution_schedule.hr;
    tm.tm_min =  pstFDInfo->Distribution_schedule.min;
    tm.tm_sec =  pstFDInfo->Distribution_schedule.sec;

	ulStartTime = mktime(&tm);
	time(&ulCurrTime);
	if(ulCurrTime > ulStartTime)
	{
		ulStartTime = ulStartTime + 86400;	
		newtime = localtime(&ulStartTime);	
		pstFDInfo->Distribution_schedule.year = newtime->tm_year + 1900;
		pstFDInfo->Distribution_schedule.month = newtime->tm_mon + 1 ;
		pstFDInfo->Distribution_schedule.date = newtime->tm_mday;
		pstFDInfo->Distribution_schedule.hr = newtime->tm_hour;
		pstFDInfo->Distribution_schedule.min = newtime->tm_min;
		pstFDInfo->Distribution_schedule.sec = newtime->tm_sec;
	}


}


/*****************************************************************************
 *
 * name             :   Common_SignalIgnore
 * description      :   Ignores the specified signal
 * input parameters :   int iSigNum - The signal to be ignored
 * output parameters:   void
 * return type      :   void
 * global variables :   None
 * calls            :   None
 */
#ifndef WIN32
void
Common_SignalIgnore(int iSigNum)
{

    if(iSigNum == IGNORE_ALL_SIG) // Ignore all signals
    {
        if (signal(SIGINT, SIG_IGN) == SIG_ERR)
        {
            snmp_log_perror("Common_SignalIgnore: \
                error \"uncatching\" SIGINT\n");
            return;
        }


        if (signal(SIGALRM, SIG_IGN) == SIG_ERR)
        {
            snmp_log_perror("Common_SignalIgnore: \
                error \"uncatching\" SIGALRM\n");
            return;
        }

    }
    else
    {
        if(signal(iSigNum,SIG_IGN) == SIG_ERR)
        {
            snmp_log_perror("Common_SignalIgnore: \
                Error in signal handling\n");
            return;
        }
    }

    return;
}
#endif
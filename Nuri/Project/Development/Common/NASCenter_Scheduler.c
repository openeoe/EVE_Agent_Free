/*****************************************************************************
 *
 * NASCenter_Scheduler.c:
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
 * $Log: NASCenter_Scheduler.c,v $
 * Revision 1.18  2004/01/08 12:14:22  saisk
 * Moved MutexInitilize of Execution list from InitExecutionList to StartSchdeuler
 *
 * Revision 1.17  2004/01/08 08:37:51  saisk
 * Changed _DEBUG to SCH_DEBUG
 *
 * Revision 1.16  2003/12/20 04:08:28  saisk
 * Changed TaskId from I32 to long
 *
 * Revision 1.15  2003/12/18 14:57:32  sig
 * Added after modification bug fix
 *
 * Revision 1.14  2003/12/15 03:23:22  thiagav
 * Final Scheduler code
 *
 * Revision 1.13  2003/12/11 02:21:50  thiagav
 * Added after bug fixes
 *
 * Revision 1.12  2003/12/05 05:00:10  thiagav
 * Added
 *
 * Revision 1.11  2003/12/03 15:59:12  saisk
 * Fixed Unit Testing Bugs and modified Validate and Scheduler to Handle Hourly Schedule
 *
 * Revision 1.8  2003/11/24 11:08:03  saisk
 * Final Version
 *
 * Revision 1.10  2003/11/24 11:05:01  saisk
 * Updated and did code clean up
 *
 * Revision 1.9  2003/11/20 13:17:07  saisk
 * Updated
 *
 * Revision 1.8  2003/11/13 11:25:07  saisk
 * !. Modified SortExecutionList
 *
 * Revision 1.7  2003/11/13 09:44:11  saisk
 * 1. Modified SortExecutionList: if start is NULL I am returning.
 * 2. Added ValidateTask function: Implementation should be done
 *
 * Revision 1.6  2003/11/07 07:44:07  saisk
 * Added function ClearMasterTable()
 * Modified SCH_StopScheduler and accordingly handled in Schdeuler()
 *
 * Revision 1.5  2003/11/06 13:29:05  saisk
 * Implemented Modify and Delete Task
 *
 * Revision 1.4  2003/11/06 07:25:27  saisk
 * Changed gpSchMasterTable to gSchMasterTable, added function header
 *
 * Revision 1.3  2003/11/05 13:04:43  saisk
 * Changed ExecList from pointer to static structure
 *
 * Revision 1.2  2003/11/05 12:26:25  saisk
 * Implemented synchronization , completed Scheduler function , SCH_StartScheduler modified using pThreads
 *
 * Revision 1.1  2003/11/04 13:47:16  saisk
 * First Check In, renamed Scheduler.c and scheduler.h as per coding standard also exported function changed as per coding standard.
 *
 * $State: Exp $
 * $Id: NASCenter_Scheduler.c,v 1.18 2004/01/08 12:14:22 saisk Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/


/********************************* includes *********************************/
#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif 
#include <string.h>
#include "NASCenter_Scheduler.h"
#include "ctypes.h"

#include "NASCenter_Errors.h"


/****************************** local macros ********************************
 *
 * (Macros shareable with rest of the modules are present in .h file)
 */

#define MASTER_ARR_SIZE     25      /* EVERY_HOUR_TASK + 1 */
#define EVERY_HOUR_TASK     24      
#define NO_SECS_IN_HOUR     3600
#define NO_SECS_IN_MIN      60
#define NO_SECS_IN_DAY      86400
#define NO_HOUR_IN_DAY      24
#define NO_MINS_IN_HOUR     60
#define CHECK_STATUS_TIME   6   /* In seconds */

#ifdef WIN32
#define SLEEP(X)            Sleep(X*1000)
#else
#define SLEEP(X)            sleep(X)
#endif

#ifdef SCH_DEBUG
#define DEBUG_FILE              "SchDebug.log"
#define DEBUG_PRINT_EXEC_LIST() DebugPrintExecList();
#define DEBUG_PRINT(x)          DebugPrint(x);
#else
#define DEBUG_PRINT_EXEC_LIST() 
#define DEBUG_PRINT(x)
#endif


/********************* local structure/union definitions ********************/

typedef struct stMasterNode stMasterNode;

struct stMasterNode
{
	stTask MTask;
	stMasterNode *pNext;
};

typedef struct 
{
    stMasterNode *pHeadMasterData;
    stMasterNode *pTailMasterData;
    int nSize;
}stMasterTable;

typedef struct
{
   	FP fp;
	void *vpArg;
    UI32 ui32Interval;
}stExecuteTask;

typedef struct stExecNode stExecNode ;
struct stExecNode
{
    stExecuteTask ExecTask;
    stExecNode *pNext;
};
typedef struct 
{
    stExecNode *pHeadExecData;
    stExecNode *pTailExecData;
    pthread_mutex_t ExecList_mutex ;
    int nSize;
}stExecutionList;

typedef enum {
    SCH_DATE_ONLY, 
    SCH_TIME_ONLY,
    SCH_BOTH
}eValidateDateTime;
/********************* prototypes for local functions ***********************/

void Scheduler(void* lpParam );

/* Master Table Related functions */
void InitMasterTable(void);
void ClearMasterTable(void);

/* Execution List Related functions */
void InitExecutionList(void);
ENASCenterError AddToExecutionList(stTask MTask, UI32 ui32Interval);
void BuildExecutionList(void);
void SortExecutionList (stExecNode **ppStart);
void ClearExecutionList(void);
void SCH_AutoCleanTask(void);

/* Validation Functions*/
ENASCenterError ValidateTask(stTask task);
ENASCenterError ValidateDateTime(stDate inDate, stTime inTime, 
                                 eValidateDateTime eVDTFlag);
void WaitForScheduler(void);

#ifdef SCH_DEBUG
void DebugPrintExecList() ;
void DebugPrint(char* str) ;
#endif

/************************* Globals within this module ***********************/

static stMasterTable   gSchMasterTable[MASTER_ARR_SIZE];
static pthread_mutex_t gMasterTable_mutex = PTHREAD_MUTEX_INITIALIZER ;
static pthread_mutex_t gExecList_mutex    = PTHREAD_MUTEX_INITIALIZER ;
static stExecutionList gExecList;
static BOOL gbTaskChanged   = FALSE ;
static BOOL gbStopScheduler = FALSE ;
static pthread_t hPThread;    

#ifdef SCH_DEBUG
FILE *fpDebugLog = NULL;
#endif

/************************* externs  ***********************/

/*****************************************************************************
 *
 * name             :   SCH_StartScheduler
 * description      :   Initialize and Starts the Scheduler
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   hPThread
 * calls            :   InitMasterTable(),InitExecutionList(),Scheduler()
 */
void SCH_StartScheduler(void)
{
        
    InitMasterTable();
    InitExecutionList();
    gbStopScheduler = FALSE;
    gbTaskChanged   = FALSE ;
    gExecList.ExecList_mutex = gExecList_mutex;
    if ( pthread_create(&hPThread,NULL,(void*) Scheduler,NULL) != 0 )
    {
#ifdef SCH_DEBUG
       printf("SCH_StartScheduler()........Failed\n");
#endif
    }
    else
    {
#ifdef SCH_DEBUG
       printf("SCH_StartScheduler()........Success\n ");
#endif
    }
 
}

/*****************************************************************************
 *
 * name             :   SCH_StopScheduler
 * description      :   Clears memory and Stops the Scheduler
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   
 * calls            :   
 */

void SCH_StopScheduler(void)
{
	gbStopScheduler = TRUE;
    WaitForScheduler();
}

/*****************************************************************************
 *
 * name             :   WaitForScheduler
 * description      :   Waits till scheduler thread completes
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   hPThread
 * calls            :   
 */

void WaitForScheduler(void)
{
    void * status;
    pthread_join(hPThread , &status);
}


/*****************************************************************************
 *
 * name             :   InitMasterTable
 * description      :   Initialize the Master Table
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   gSchMasterTable
 * calls            :   
 */
   
void InitMasterTable(void)
{
    I16 nCount  = 0;
    for (nCount = 0; nCount < MASTER_ARR_SIZE ; nCount++)
        {
          gSchMasterTable[nCount].pHeadMasterData = NULL;
          gSchMasterTable[nCount].pTailMasterData = NULL;
          gSchMasterTable[nCount].nSize = 0;
        }
}

/*****************************************************************************
 *
 * name             :   InitExecutionList
 * description      :   Initialize the Execution List
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   gExecList
 * calls            :   
 */

void InitExecutionList(void)
{
    gExecList.pHeadExecData  = NULL;
    gExecList.pTailExecData  = NULL;
    gExecList.nSize          = 0;
}

/*****************************************************************************
 *
 * name             :   SCH_AddTask
 * description      :   Add Task to the Scheduler
 * input parameters :   stTask
 * output parameters:   void
 * return type      :   long ( TaskId )
 * global variables :   gSchMasterTable 
 * calls            :   ClearExecutionList(),InitExecutionList(),BuildExecutionList()
 */
long SCH_AddTask(stTask task)
{
    stMasterNode **pHead = NULL;
    stMasterNode **pTail = NULL;
    int *nSize = NULL;
    stMasterNode *endNode = NULL;
    stMasterNode *newNode = NULL;

#ifdef SCH_DEBUG
    static int iCount=0;
    printf("Add task called %d\n", ++iCount);	
#endif
 
   if (ValidateTask(task) != NASC_SUCCESS)
   {
       return -1;
   }
   else
   {
       
   }
    /* Lock */
    pthread_mutex_lock(&gMasterTable_mutex);
    switch (task.ePCity)
    {
    case SCH_SEC_MIN_HOUR:
        pHead = &(gSchMasterTable[EVERY_HOUR_TASK].pHeadMasterData);
        pTail = &(gSchMasterTable[EVERY_HOUR_TASK].pTailMasterData);
        nSize = &(gSchMasterTable[EVERY_HOUR_TASK].nSize);
        break;
	case SCH_DAY:
        /* break has been avoided deliberately*/
    case SCH_WEEK: 
        /* break has been avoided deliberately*/
    case SCH_MONTH:
        {
            pHead =  &(gSchMasterTable[task.StartTime.ui16Hour].pHeadMasterData);
            pTail =  &(gSchMasterTable[task.StartTime.ui16Hour].pTailMasterData);
            nSize =  &(gSchMasterTable[task.StartTime.ui16Hour].nSize);
        }
        break;
    default:
        pthread_mutex_unlock(&gMasterTable_mutex);
        return -1;
        
    }
    
	newNode = malloc(sizeof(stMasterNode));
    if(newNode!=NULL)
    {

        memcpy(&(newNode->MTask),&task, sizeof(stTask));
        newNode->pNext = NULL;

        if (*pHead == NULL)
        {
            *pHead = newNode;
        }
        else
        {
            endNode = *pTail;
            endNode->pNext = newNode;
        }
        *pTail = newNode;
        (*nSize)++;
        gbTaskChanged = TRUE;

        #ifdef SCH_DEBUG
        printf("Hour %d \n", task.StartTime.ui16Hour );
        printf("Minute %d \n", task.StartTime.ui16Minute);
        printf("Second %d \n", task.StartTime.ui16Second);
        printf("Task pType %d \n", task.eType);
        printf("Task pCity %d \n", task.ePCity);
        printf("Task eWeekDay%d \n", task.eWeekDay);
        #endif
        }
    else
    {
        /* Unlock */
        pthread_mutex_unlock(&gMasterTable_mutex);
        return -1;
    }
    /* Unlock */
    pthread_mutex_unlock(&gMasterTable_mutex);
    DEBUG_PRINT("Returning Task ID \n");

    return (long)newNode; 
}

/*****************************************************************************
 *
 * name             :   ValidateTask
 * description      :   Validates Task
 * input parameters :   stTask
 * output parameters:   void
 * return type      :   ENASCenterError (NASC_SUCCESS or NASC_FAILURE)
 * global variables :   
 * calls            :   
 */


ENASCenterError ValidateTask(stTask task)
{
  	time_t t1;
	struct tm *pTime;

    time(&t1);
	pTime = localtime(&t1);

    /* Validation for Start Date  */
    /* Modified by thiagu*/ 	
    if  (((task.StartDate.ui16Year < (pTime->tm_year + 1900))   ||
        ((task.StartDate.ui16Year == pTime->tm_year + 1900)  && 
          (task.StartDate.ui16Month < pTime->tm_mon)) ||
         ((task.StartDate.ui16Year == pTime->tm_year + 1900) &&
          (task.StartDate.ui16Month == pTime->tm_mon) &&
          (task.StartDate.ui16Day < pTime->tm_mday))) && 
	      (task.eType != SCH_PERIODIC))
    {
        return NASC_FAILURE;
    }

    /* Validation for Start Time  */
    if  (((task.StartTime.ui16Hour   > (NO_HOUR_IN_DAY  - 1)) ||
         (task.StartTime.ui16Minute > (NO_MINS_IN_HOUR - 1)) ||
         (task.StartTime.ui16Second > (NO_SECS_IN_MIN  - 1))) && 
         (task.eType != SCH_PERIODIC))
    {
        return NASC_FAILURE;
    }

    /* Validation for eType */
    if  ((task.eType != SCH_ONE_SHOT) && (task.eType != SCH_PERIODIC))
    {
        return NASC_FAILURE;
    }

    /* Validation for SCH_SEC_MIN_HOUR */  
    if(task.ePCity == SCH_SEC_MIN_HOUR) 
    {
       if ((task.ui32Interval == 0) || (task.ui32Interval > NO_SECS_IN_DAY))
       {
           return NASC_FAILURE;
       }
       if ((task.eWeekDay < SCH_SUN) || (task.eWeekDay > SCH_ALL_DAYS))
       {
           return NASC_FAILURE;
       }
       if (task.eType != SCH_PERIODIC)
       {
           return NASC_FAILURE;
       }
    }
    /* Validation for SCH_DAY  */ 
    if((task.ePCity == SCH_DAY) &&
       ((task.eWeekDay < SCH_SUN) || (task.eWeekDay > SCH_ALL_DAYS)))
        
    {
        return NASC_FAILURE;
    }

    /* Validation for SCH_WEEK, SCH_MONTH */ 
    if(((task.ePCity == SCH_WEEK)||(task.ePCity == SCH_MONTH)) &&
       ((task.eWeekDay < SCH_SUN) || (task.eWeekDay > SCH_SAT)))
        
    {
        return NASC_FAILURE;
    }    
    return NASC_SUCCESS;
}
/*****************************************************************************
 *
 * name             :   ValidateDateTime
 * description      :   Validate Date and Time
 * input parameters :   stDate , stTime, eValidateDateTime
 * output parameters:   void
 * return type      :   ENASCenterError (NASC_SUCCESS or NASC_FAILURE)
 * global variables :   
 * calls            :   
 */

ENASCenterError ValidateDateTime(stDate inDate, stTime inTime, eValidateDateTime eVDTFlag)
{
  	time_t t1;
	struct tm *pTime;
    BOOL bValidDate = TRUE;
    BOOL bValidTime = TRUE;
    I32 i32CurrTimeInSec = 0;
    I32 i32TimeInSec     = 0;

    time(&t1);
	pTime = localtime(&t1);

    /* Validation for Date  */
    if ((eVDTFlag == SCH_DATE_ONLY) ||(eVDTFlag == SCH_BOTH))
    {
        if  ((inDate.ui16Year < (pTime->tm_year + 1900))   ||
            ((inDate.ui16Year == pTime->tm_year + 1900)  && 
              (inDate.ui16Month < pTime->tm_mon)) ||
             ((inDate.ui16Year == pTime->tm_year + 1900) &&
              (inDate.ui16Month == pTime->tm_mon) &&
              (inDate.ui16Day < pTime->tm_mday)))
        {
            bValidDate = FALSE;
        }
    }


    /* Validation for  Time  */
    if ((eVDTFlag == SCH_TIME_ONLY) ||(eVDTFlag == SCH_BOTH))
    {
        i32TimeInSec = (inTime.ui16Hour * NO_SECS_IN_HOUR ) +
                       (inTime.ui16Minute * NO_SECS_IN_MIN) + 
                       inTime.ui16Second ;
                    
        i32CurrTimeInSec =  (pTime->tm_hour * NO_SECS_IN_HOUR ) +
                            (pTime->tm_min* NO_SECS_IN_MIN) + 
                             pTime->tm_sec ;

        if (i32TimeInSec < i32CurrTimeInSec)
        {
            bValidTime = FALSE;
        }
    }

    if (eVDTFlag == SCH_DATE_ONLY)
    {
        if (bValidDate == FALSE)
        {
            return NASC_FAILURE;
        }
        else
        {
            return NASC_SUCCESS;
        }
    }
    else if (eVDTFlag == SCH_TIME_ONLY)
    {
        if (bValidTime == FALSE)
        {
            return NASC_FAILURE;
        }
        else
        {
            return NASC_SUCCESS;
        }
    }
    else 
    {
        if((bValidDate == FALSE) && (bValidTime == FALSE)  )
        {
            return NASC_FAILURE;
        }
        else
        {
            return NASC_SUCCESS;
        }
    }
}

/*****************************************************************************
 *
 * name             :   BuildExecutionList
 * description      :   Build's Execution List from Master Table
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   gSchMasterTable, gExecList
 * calls            :   AddToExecutionList()
 */

void BuildExecutionList(void)
{
    stMasterNode *pHead      = NULL;
    stMasterNode *pTemp      = NULL;
	time_t t1;
	struct tm *pTime;
    UI32 ui32NoOfExec       = 0;
    UI32 ui32Count          = 0;
    UI32 ui32Interval       = 0;
    UI16 ui16StartHr        = 0;
    UI16 ui16EndHr          = 0;
    UI16 ui16StartHrInSec   = 0;
    UI16 ui16EndHrInSec     = 0;
    
    /* Lock */
    pthread_mutex_lock(&(gExecList.ExecList_mutex));
    time(&t1);
	pTime = localtime(&t1);
    ui16StartHr = pTime->tm_hour ;
    ui16EndHr = ui16StartHr + 1 ;

    ui16StartHrInSec   = pTime->tm_hour * NO_SECS_IN_HOUR;
    ui16EndHrInSec     = ui16StartHrInSec + NO_SECS_IN_HOUR;

    if (((&(gSchMasterTable[ui16StartHr].pHeadMasterData)) == NULL) &&

        ((&(gSchMasterTable[EVERY_HOUR_TASK].pHeadMasterData)) == NULL))
    {
        /* No Task to be scheduled */
        /* Unlock */
        pthread_mutex_unlock(&(gExecList.ExecList_mutex));
        DEBUG_PRINT(" No Node exists \n");
        return ;
    }
    /*Traverse through EVERY_HOUR_TASK and Add Task to Execution List */
    pHead = (gSchMasterTable[EVERY_HOUR_TASK].pHeadMasterData);
    pTemp = pHead;
    if(pTemp != NULL)
    {
    
        while((pTemp) != NULL)
        {
           /* ui32Interval - in seconds */
           ui32Interval = (pTemp)->MTask.ui32Interval;
           if (((pTemp)->MTask.ePCity == SCH_SEC_MIN_HOUR) && 
               (((pTemp)->MTask.eWeekDay == pTime->tm_wday) || 
                ((pTemp)->MTask.eWeekDay  == SCH_ALL_DAYS)))
           {
               /* Task with interval less than hour*/
               if ((ui32Interval > 0) && (ui32Interval <= NO_SECS_IN_HOUR))
               {
                    ui32NoOfExec = NO_SECS_IN_HOUR / ui32Interval;
               }
               /* Task with interval greater than hour*/
               /* ui32Interval - Number of seconds since 00:00 AM */
               /* Calculate interval w.r.t currnet hour and store 
                  it in ui32Interval */
               else if ((ui32Interval > NO_SECS_IN_HOUR) && 
                        (ui32Interval <= NO_SECS_IN_DAY))
               {
                   /*Check whether this Task should be scheduled for current hour */
                   UI32 ui32Mod = 0 ;
                   ui32Mod = ui16EndHrInSec % ui32Interval ;
                   if ( ui32Mod < NO_SECS_IN_HOUR)
                   {
                       /*Set ui32Interval with actual time in second with 
                         '0' as start of the hour */
                       if (ui32Mod == 0)
                       {
                           ui32Interval = NO_SECS_IN_HOUR;
                       }
                       else
                       {
                           ui32Interval = ui32Mod;
                       }
                       ui32NoOfExec = 1;/* Always one */
                   }
               }
               else if ( ui32Interval > NO_SECS_IN_DAY)
               {
                   /* Wrong category */
                   /* Unlock */
                   pthread_mutex_unlock(&(gExecList.ExecList_mutex));
                   return ;
               }
                
           }
           /* Add the Task into  Execution List*/
           for (ui32Count = 0; ui32Count < ui32NoOfExec ; ui32Count++)
           {
                AddToExecutionList((pTemp)->MTask,(ui32Interval *(ui32Count+1)));
           }
           ui32Interval = 0;/* Reset the interval */
           (pTemp) = (pTemp)->pNext ;
        }/* End of while*/
    } /* End of if */

    /*Traverse through current hour list and Add Task to Execution List */
    pHead = (gSchMasterTable[ui16StartHr].pHeadMasterData);
    pTemp = pHead;
    if(pTemp != NULL)
    {
#ifdef SCH_DEBUG
        //printf("Size in MT %d \n",gSchMasterTable[ui16StartHr].nSize );
#endif
        while((pTemp) != NULL)
        {
            /* Check if the Periodicity is 'SCH_DAY' 
               and it is for current day or all days.*/
            if (((pTemp)->MTask.ePCity == SCH_DAY) && 
               (((pTemp)->MTask.eWeekDay == pTime->tm_wday) || 
                ((pTemp)->MTask.eWeekDay  == SCH_ALL_DAYS)))
            {
                /*Calculate at what interval the Task has to run*/
                ui32Interval = ((pTemp)->MTask.StartTime.ui16Minute) *\
                                NO_SECS_IN_MIN +\
                              ((pTemp)->MTask.StartTime.ui16Second);
#ifdef SCH_DEBUG    
                printf(" Node is added it to EL\n");
                printf(" EL Time Min %d \n",(pTemp)->MTask.StartTime.ui16Minute);
                printf(" EL Time Sec %d \n",(pTemp)->MTask.StartTime.ui16Second);
                printf("Interval in Day activity  %ld \n", ui32Interval);
#endif 

                AddToExecutionList((pTemp)->MTask,ui32Interval );
 
            }
            /* Check if the Periodicity is 'SCH_WEEK' and and it is for current day 
               to schedule */
            if (((pTemp)->MTask.ePCity == SCH_WEEK) &&
                ((pTemp)->MTask.eWeekDay == pTime->tm_wday))
            {
                  ui32Interval = ((pTemp)->MTask.StartTime.ui16Minute) *\
                                NO_SECS_IN_MIN +\
                              ((pTemp)->MTask.StartTime.ui16Second);
#ifdef SCH_DEBUG    		        
                printf("Interval in Weekly activity  %ld \n", ui32Interval);
#endif 
                AddToExecutionList((pTemp)->MTask,ui32Interval );
            }
            /* Check if the Periodicity is 'SCH_MONTH' and and it is for current day 
               to schedule by getting the start date's day */
            if (((pTemp)->MTask.ePCity == SCH_MONTH) &&
                ((pTemp)->MTask.StartDate.ui16Day == pTime->tm_mday))
            {
                 ui32Interval =  ((pTemp)->MTask.StartTime.ui16Minute) *\
                                NO_SECS_IN_MIN +\
                              ((pTemp)->MTask.StartTime.ui16Second);
#ifdef SCH_DEBUG    		        
                printf("Interval in Monthly activity  %ld \n", ui32Interval);
#endif 
                AddToExecutionList((pTemp)->MTask,ui32Interval );

            }

            (pTemp) = (pTemp)->pNext ;
        }/* End of while */
    } /* End of if */
    /* Unlock */
    DEBUG_PRINT("Before Leaving EL \n");
    pthread_mutex_unlock(&(gExecList.ExecList_mutex));
}



/*****************************************************************************
 *
 * name             :   AddToExecutionList
 * description      :   Add's Task to Execution List
 * input parameters :   stTask, Interval in seconds
 * output parameters:   void
 * return type      :   ENASCenterError
 * global variables :   gExecList
 * calls            :   
 */

 /* Note: This function does not need locking since it is been called by
 BuildExecutionList, where lock has been provided */

ENASCenterError AddToExecutionList(stTask MTask, UI32 ui32Interval)
{
	stExecNode *endExecNode   = NULL;
	stExecNode *newExecNode   = NULL;
    ENASCenterError           eErrorStatus;
    newExecNode = malloc (sizeof(stExecNode));

    /*Add to Execution List*/
    if(newExecNode!=NULL){
	    newExecNode->ExecTask.fp = MTask.fp;
        newExecNode->ExecTask.vpArg = MTask.vpArg ;
        newExecNode->ExecTask.ui32Interval = ui32Interval;
	    newExecNode->pNext = NULL;
        if (gExecList.pHeadExecData == NULL)
        {
            gExecList.pHeadExecData = newExecNode;

        }
        else
        {
            endExecNode = gExecList.pTailExecData;
            endExecNode->pNext = newExecNode;
        }
        gExecList.pTailExecData = newExecNode;
        gExecList.nSize++;
        eErrorStatus = NASC_SUCCESS;
    }
    else
    {
         eErrorStatus = NASC_FAILURE;
#ifdef SCH_DEBUG
	     printf("AddToExecutionList : Failed\n ");
#endif 
    }

    return eErrorStatus ;
}

/*****************************************************************************
 *
 * name             :   SortExecutionList
 * description      :   Sort's Task in the Execution List
 * input parameters :   stExecNode**
 * output parameters:   void
 * return type      :   void
 * global variables :   gExecList
 * calls            :   
 */

void SortExecutionList (stExecNode ** ppStart)
{

    stExecNode *start;
    stExecNode *p, *q, *r, *s, *temp; 
    stExecNode *pstNewHead;
    s = NULL;

    /* r precedes p and s points to the node up to which comparisons are to
       be made */ 
    /* temporary add a node and add interval zero */

    /* Lock */
    pthread_mutex_lock(&(gExecList.ExecList_mutex));

    if(*ppStart == NULL)
    {
        pthread_mutex_unlock(&(gExecList.ExecList_mutex));
        return;
    }
    else
    {
        start = *ppStart;
    }
    
    /*Add One node and remove it latter */

    pstNewHead = (stExecNode *) calloc (1, sizeof (stExecNode));
    pstNewHead->ExecTask.ui32Interval = 0;
    pstNewHead->pNext = start;
    start = pstNewHead;

    while ( s != start->pNext ) 
    { 
        r = p = start ; 
        q = p->pNext ;

        while ( p != s ) 
        {  
            if ( p->ExecTask.ui32Interval > q->ExecTask.ui32Interval) 
            { 
                if ( p == start ) 
                { 
                    temp = q->pNext ; 
                    q->pNext = p ; 
                    p->pNext = temp ;
                    start = q ; 
                    r = q ; 
                } 
                else 
                { 
                    temp = q->pNext ; 
                    q->pNext = p ; 
                    p->pNext = temp ;
                    r->pNext = q ; 
                    r = q ; 
                } 
            } 
            else 
            { 
                r = p ; 
                p = p->pNext ; 
            } 
            q = p->pNext ; 
            if ( q == s ) 
                s = p ; 
        } 
    }
 /* remove the added code */
    pstNewHead = start->pNext;
    free (start);
    start = pstNewHead;
    *ppStart = start;


    /* Lock */
    pthread_mutex_unlock(&(gExecList.ExecList_mutex));

    return ;
}

/*****************************************************************************
 *
 * name             :   ClearExecutionList
 * description      :   Free's memory of Nodes in the Execution List
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   gExecList
 * calls            :   
 */

void ClearExecutionList(void)
{

    stExecNode *pExecNewHead = NULL;
    
    /* Lock */
    pthread_mutex_lock(&(gExecList.ExecList_mutex));

    while(gExecList.pHeadExecData != NULL)
    {
        pExecNewHead = gExecList.pHeadExecData->pNext ;
        free(gExecList.pHeadExecData);
        gExecList.nSize--;
        gExecList.pHeadExecData = pExecNewHead;
    }
    gExecList.pHeadExecData = NULL;
    gExecList.pTailExecData = NULL;
    
    /* Unlock */
    pthread_mutex_unlock(&(gExecList.ExecList_mutex));
    
}

/*****************************************************************************
 *
 * name             :   SCH_ModifyTask
 * description      :   Modifies Task in the Scheduler
 * input parameters :   long( Old TaskId), stTask
 * output parameters:   void
 * return type      :   long( New TaskId) - '-1' indicates failure
 * global variables :   
 * calls            :   SCH_DeleteTask, SCH_AddTask
 */

/*
Note :Locking is handled in SCH_DeleteTask and SCH_AddTask
*/
long SCH_ModifyTask(long lTaskId, stTask task)
{
    BOOL bFound = FALSE;
    long lNewTaskId = -1;
    
    bFound = SCH_DeleteTask(lTaskId);

    if (bFound)
    {
        lNewTaskId = SCH_AddTask(task);
    }
    return lNewTaskId;
}
/*****************************************************************************
 *
 * name             :   SCH_GetTask
 * description      :   Returns Task in the Scheduler
 * input parameters :   long( TaskID)
 * output parameters:   stTask**
 * return type      :   ENASCenterError
 * global variables :   
 * calls            :   
 */

ENASCenterError SCH_GetTask(long lTaskId, stTask **ppTask)
{
    stMasterNode *pPrevious = NULL;
    stMasterNode *pCurrent = NULL;
    stMasterNode *pHead = NULL;


    BOOL bFound = FALSE;
    I16 nCount  = 0;

    /* Lock */
    pthread_mutex_lock(&gMasterTable_mutex);

    for (nCount = 0; nCount < MASTER_ARR_SIZE ; nCount++)
        {
          pCurrent = gSchMasterTable[nCount].pHeadMasterData;
          pHead    = gSchMasterTable[nCount].pHeadMasterData;
          if(pCurrent == NULL)
          {
              continue;
          }
          /*First time previous is NULL */
          pPrevious = NULL;
          while( pCurrent != NULL)
          {
              if (lTaskId == (long) pCurrent)
              {
                  memcpy((*ppTask),&(pCurrent->MTask),sizeof(stTask));
                  bFound = TRUE;
                  break;
              }
              else
              {
                  pPrevious = pCurrent;
                  pCurrent = pCurrent->pNext ;
              }
              
          }/* End of while */
          /* Restore the Head position */
          gSchMasterTable[nCount].pHeadMasterData = pHead ;

          if (bFound)
          {
              break;
          }
        }/* End of for*/
    /* Unlock */
    pthread_mutex_unlock(&gMasterTable_mutex);
    if (bFound)
    {
        return NASC_SUCCESS;
    }
    else
    {
        return NASC_FAILURE ;
    }

}

/*****************************************************************************
 *
 * name             :   SCH_DeleteTask
 * description      :   Removes Task from the Scheduler
 * input parameters :   long( TaskId)
 * output parameters:   void
 * return type      :   ENASCenterError
 * global variables :   gSchMasterTable, gMasterTable_mutex
 * calls            :   
 */

ENASCenterError SCH_DeleteTask(long lTaskId)
{
    stMasterNode *pPrevious = NULL;
    stMasterNode *pCurrent = NULL;

    BOOL bFound = FALSE;
    I16 nCount  = 0;

    /* Lock */
    pthread_mutex_lock(&gMasterTable_mutex);

    for (nCount = 0; nCount < MASTER_ARR_SIZE ; nCount++)
    {

      pCurrent = gSchMasterTable[nCount].pHeadMasterData;

      if(pCurrent == NULL)
      {
          continue;
      }
      /*First time previous is NULL */
      pPrevious = NULL;
      while( pCurrent != NULL)
      {
        
          if (lTaskId == (long) pCurrent)
          {
                  		  
              if(pPrevious)
              {
                  pPrevious->pNext = pCurrent->pNext;
                  /* Conditionally Reset Tail Position */
		          if(pPrevious->pNext == NULL )
                  {
                      gSchMasterTable[nCount].pTailMasterData = pPrevious;

                  }
              }
              else if((pCurrent->pNext == NULL) && (pPrevious == NULL))
              {
                  /* Only One Node is Available and that to be removed  */
                  /* Reset Head and Tail Position to NULL*/
                  gSchMasterTable[nCount].pHeadMasterData = NULL;
                  gSchMasterTable[nCount].pTailMasterData = NULL;
              }
		      else if((pPrevious == NULL) && (pCurrent->pNext != NULL ))
		      {
                  /* Head Node to be removed */
                  /* Reset Head Position */
                  gSchMasterTable[nCount].pHeadMasterData = pCurrent->pNext;                          			  
		      }
              /* Deallocate current */		      
              free(pCurrent);
		      pCurrent = NULL;
              gSchMasterTable[nCount].nSize--; 
              bFound = TRUE;
              break;
          }
          else
          {
              pPrevious = pCurrent;
              pCurrent = pCurrent->pNext ;
          }
          
      }/* End of while */
      if (bFound)
      {
          break;
      }
    }/* End of for*/
    /* Unlock */
    pthread_mutex_unlock(&gMasterTable_mutex);
    if (bFound)
    {
        return NASC_SUCCESS;
    }
    else
    {
        return NASC_FAILURE ;
    }

}
/*****************************************************************************
 *
 * name             :   SCH_AutoCleanTask
 * description      :   Removes Expired ONE_SHOT Task from the Scheduler
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   gSchMasterTable, gMasterTable_mutex
 * calls            :   
 */

void SCH_AutoCleanTask(void)
{
    stMasterNode *pPrevious = NULL;
    stMasterNode *pCurrent = NULL;
    stMasterNode *pTemp = NULL;

    I16 nCount  = 0;

    /* Lock */
    pthread_mutex_lock(&gMasterTable_mutex);

    for (nCount = 0; nCount < MASTER_ARR_SIZE ; nCount++)
    {

      pCurrent = gSchMasterTable[nCount].pHeadMasterData;

      if(pCurrent == NULL)
      {
          continue;
      }
      /*First time previous is NULL */
      pPrevious = NULL;
      while( pCurrent != NULL)
      {
        
          if (pCurrent->MTask.eType == SCH_ONE_SHOT)
          {
              /* Check if Date expired for ONE_SHOT job*/
              if (ValidateDateTime(pCurrent->MTask.StartDate,
                                   pCurrent->MTask.StartTime,
                                   SCH_DATE_ONLY) 
                                   == NASC_FAILURE)
              {
                  		  
                  if(pPrevious)
                  {
                      pPrevious->pNext = pCurrent->pNext;
                      /* Conditionally Reset Tail Position */
		              if(pPrevious->pNext == NULL )
                      {
                          gSchMasterTable[nCount].pTailMasterData = pPrevious;
                      }
                  }
                  else if((pPrevious == NULL) && (pCurrent->pNext == NULL))
                  {
                      /* Only One Node is Available and that to be removed  */
                      /* Reset Head and Tail Position to NULL*/
                      gSchMasterTable[nCount].pHeadMasterData = NULL;
                      gSchMasterTable[nCount].pTailMasterData = NULL;
                  }
		          else if((pPrevious == NULL) && (pCurrent->pNext != NULL ))
		          {
                     /* Head Node to be removed */
                     /* Reset Head Position */
                     gSchMasterTable[nCount].pHeadMasterData = pCurrent->pNext;                          			  
		          }
		          pTemp = pCurrent->pNext;
                  /*Deallocate */
                  free(pCurrent);
                  gSchMasterTable[nCount].nSize--; 
                  pCurrent = pTemp ;   
              }
          }
          else
          {
              pPrevious = pCurrent;
              pCurrent = pCurrent->pNext ;
          }
          
      }/* End of while */
    }/* End of for*/
    /* Unlock */
    pthread_mutex_unlock(&gMasterTable_mutex);

}
/*****************************************************************************
 *
 * name             :   ClearMasterTable
 * description      :   Clears the memory of Master Table
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   gSchMasterTable, gMasterTable_mutex
 * calls            :   
 */

void ClearMasterTable(void)
{
    stMasterNode *pNewHead = NULL;
    I16 nCount  = 0;
    
    DEBUG_PRINT("Entered ClearMasterTable()\n");
    /* Lock */
    pthread_mutex_lock(&gMasterTable_mutex);

    for (nCount = 0; nCount < MASTER_ARR_SIZE ; nCount++)
        {
            if(gSchMasterTable[nCount].pHeadMasterData == NULL)
            {
              continue;
            }
            while(gSchMasterTable[nCount].pHeadMasterData != NULL)
            {
                pNewHead = gSchMasterTable[nCount].pHeadMasterData->pNext ;
                free(gSchMasterTable[nCount].pHeadMasterData);
                gSchMasterTable[nCount].nSize--;
                gSchMasterTable[nCount].pHeadMasterData = pNewHead;
            }
                gSchMasterTable[nCount].pHeadMasterData = NULL;
                gSchMasterTable[nCount].pTailMasterData = NULL;

        }/* End of for*/


    /* Unlock */
    pthread_mutex_unlock(&gMasterTable_mutex);
    DEBUG_PRINT("Leaving ClearMasterTable()\n");
}

/*****************************************************************************
 *
 * name             :   Scheduler
 * description      :   Schedules the Task in Execution List
 * input parameters :   void*
 * output parameters:   void
 * return type      :   void
 * global variables :   gExecList
 * calls            :   
 */

void Scheduler(void* lpParam )
{
	time_t t1;
    struct tm *pCurrTime    = NULL;
    stExecNode **ppExecHead = NULL;
    stExecNode *pExecTemp   = NULL;
    UI32 ui32SleepTime      = 0;
    BOOL bHourlySchedule    = FALSE;
    
    /* Number of seconds since start of hour*/
    UI32 ui32Seconds = 0; 
	
    while(1)
    {
        
        DEBUG_PRINT("Scheduler is running \n");    

        if (gbStopScheduler)
        {
            gbStopScheduler = FALSE;
            ClearExecutionList();
            DEBUG_PRINT("Before Calling ClearMasterTable()\n");
            ClearMasterTable();
            DEBUG_PRINT("After Calling ClearMasterTable()\n");
            /* break from infinite while loop to return from this function*/
            break;
        }
        time(&t1);
        pCurrTime = localtime(&t1);
        ui32Seconds = pCurrTime->tm_min * NO_SECS_IN_MIN + pCurrTime->tm_sec ;

        /* Every start of hour the BuildExecutionList() has to be called */
        /* Check whether it is time for Hourly Schedule */
        if((bHourlySchedule != TRUE) && (gbTaskChanged != TRUE))
        {
            if ((NO_SECS_IN_HOUR - ui32Seconds) <=  CHECK_STATUS_TIME )
            {
                ui32SleepTime = (NO_SECS_IN_HOUR ) - (ui32Seconds) ;
                SLEEP(ui32SleepTime);    
                bHourlySchedule = TRUE;
            }
            else
            {
                ui32SleepTime = CHECK_STATUS_TIME;
            }
        }


       	/* Check again if any new Task is added to scheduler or 
           it is time for Hourly Schedule */
        if((gbTaskChanged == FALSE) && (bHourlySchedule == FALSE))
        {
          SLEEP(ui32SleepTime);             		
	      continue;
        }		
        /* Do Auto Clean at 00 Hour, to remove Expired ONE_SHOT TASK */
        if ((pCurrTime->tm_hour == 0) && (ui32Seconds <= CHECK_STATUS_TIME))
        {
            SCH_AutoCleanTask();
        }
        ClearExecutionList();
        InitExecutionList();
        BuildExecutionList();
        gbTaskChanged = FALSE;
        bHourlySchedule = FALSE ;

        DEBUG_PRINT("Before Sorting");
        DEBUG_PRINT_EXEC_LIST();

        SortExecutionList(&gExecList.pHeadExecData);

        DEBUG_PRINT("After Sorting");
        DEBUG_PRINT_EXEC_LIST();

        
        pthread_mutex_lock(&(gExecList.ExecList_mutex));
       
        ppExecHead = &(gExecList.pHeadExecData);
        
        if(*ppExecHead == NULL)
        {
            /* No Task has been scheduled for this hour */
            /* Sleep for minimum time and check again for any new Task added */
            
            pthread_mutex_unlock(&(gExecList.ExecList_mutex));
            DEBUG_PRINT("Scheduler : Before sleeping Head is NULL\n");		    
            SLEEP(CHECK_STATUS_TIME);
        }
        else 
        {
            DEBUG_PRINT("Scheduler : Head is not NULL\n");		    
            /* Task Execution loop for current hour*/
            while(*ppExecHead != NULL)
            {
                time(&t1);
                pCurrTime = localtime(&t1);
                ui32Seconds = pCurrTime->tm_min * NO_SECS_IN_MIN + pCurrTime->tm_sec ;

                #ifdef SCH_DEBUG
		                printf("ui32Seconds %ld \n",ui32Seconds); 
                #endif		

                if (gbStopScheduler)
                {
                    pthread_mutex_unlock(&(gExecList.ExecList_mutex));
                    break;
                }
                /* If Scheduler starts in middle of a hour, skip all Task prior 
                   to the current time */
                if (ui32Seconds > (*ppExecHead)->ExecTask.ui32Interval)
                {
                    /* Remove the Invalid Task */
                    pExecTemp = (*ppExecHead)->pNext ;
                    free(*ppExecHead);
                    gExecList.nSize--;
                    *ppExecHead = pExecTemp ;
                }
                /* Execute all the task matches current time */
                else if (ui32Seconds == (*ppExecHead)->ExecTask.ui32Interval)
                {
                    /* Execute Task */
                    (*ppExecHead)->ExecTask.fp((*ppExecHead)->ExecTask.vpArg);
                    DEBUG_PRINT("Task Executed");
                    /* Remove Executed Task */
                    pExecTemp = (*ppExecHead)->pNext ;
                    free(*ppExecHead);
                    gExecList.nSize--;
                    *ppExecHead = pExecTemp ;
                 
                }
	       	    else if ((ui32Seconds == (NO_SECS_IN_HOUR-1)) && 
                        ((*ppExecHead)->ExecTask.ui32Interval == NO_SECS_IN_HOUR)) 
                {
                    /* For end of Hour ui32Seconds will be 3599, hence to execute 
                       the task with 3600 interval in the Execution List we do this */
                    bHourlySchedule = TRUE ;
                    
                    /* Execute Task */
                    (*ppExecHead)->ExecTask.fp((*ppExecHead)->ExecTask.vpArg);
                    DEBUG_PRINT("Task Executed");
                    
                    /* Remove Executed Task */
                    pExecTemp = (*ppExecHead)->pNext ;
                    free(*ppExecHead);
                    gExecList.nSize--;
                    *ppExecHead = pExecTemp ;
		            
                    if(gExecList.nSize ==0)
		            {
                        /*Scheduler is forced to sleep to adjust time */  			    
                        SLEEP(1);			    
                        break;     			    
		            }
                }
                /* Sleep till next task */
                else
                {
                   /* If Sleep time is greates than CHECK_STATUS_TIME, sleep 
                      for CHECK_STATUS_TIME  and check for any new Task added */

                    time(&t1);
                    pCurrTime = localtime(&t1);
                    ui32Seconds = pCurrTime->tm_min * NO_SECS_IN_MIN + pCurrTime->tm_sec ;
                    ui32SleepTime = (*ppExecHead)->ExecTask.ui32Interval - ui32Seconds ;
                    
                    #ifdef SCH_DEBUG
                      printf("Interval %ld\n",(*ppExecHead)->ExecTask.ui32Interval);
                    #endif
                    
                      /* Unlock before Sleeping */
                    pthread_mutex_unlock(&(gExecList.ExecList_mutex));
                    
                    /* Caluculate Sleep Time for the tasks to be executed 
                       before end of hour*/
		            if ((ui32SleepTime == CHECK_STATUS_TIME) && 
                        (ui32Seconds == (NO_SECS_IN_HOUR - CHECK_STATUS_TIME)))
		            {
                         SLEEP((ui32SleepTime-1));
                                   	          		            
		            }
                    else if(( ui32SleepTime < CHECK_STATUS_TIME) && (ui32Seconds > (NO_SECS_IN_HOUR - CHECK_STATUS_TIME))) 
		            {
		            
		                SLEEP((ui32SleepTime -1));  	    
                        			            
		            }
                    else if ((ui32SleepTime == 1) && (ui32Seconds > (NO_SECS_IN_HOUR - CHECK_STATUS_TIME) ))
		            {
                         bHourlySchedule = TRUE ;
                         /* Execute Task */
                         (*ppExecHead)->ExecTask.fp((*ppExecHead)->ExecTask.vpArg);
                         DEBUG_PRINT("Task Executed");
                         /* Remove Executed Task */
                         pExecTemp = (*ppExecHead)->pNext ;
                         free(*ppExecHead);
                         gExecList.nSize--;
                         *ppExecHead = pExecTemp ;
	                     if(gExecList.nSize ==0)
		                 {
                             /*Scheduler is forced to sleep*/  			    
                             SLEEP(1);			    
                             break;     			    
                         }
		            }
		            else
		            {
                            (ui32SleepTime < CHECK_STATUS_TIME) ? (SLEEP(ui32SleepTime)) : (SLEEP(CHECK_STATUS_TIME));
		            }
                    /* Check whether Task has Changed */
                    if(gbTaskChanged)
                    {
                        #ifdef SCH_DEBUG
                        printf("Inside gbTaskChanged if\n"); 			
                        #endif
                        /* Break to get new list */
                        break;
                    }
                    else
                    {
                        #ifdef SCH_DEBUG
                        printf("Inside gbTaskChanged else \n"); 			
                        #endif
                        /* Lock and continue scheduling */
                        pthread_mutex_lock(&(gExecList.ExecList_mutex));
                    }
                }
            }/* Task Execution loop for current hour*/
       	    /*For safety do the unlock */
            pthread_mutex_unlock(&(gExecList.ExecList_mutex));
        }
    }/* Infinite While loop */
    DEBUG_PRINT("Scheduler  is dead \n");        
    return ;
}

/* Following are Debug Print function for Debugging purpose only */

void DebugPrintExecList()
{

#ifdef SCH_DEBUG    
    
    I32 nCount = 0 ;
    if ((fpDebugLog = fopen (DEBUG_FILE, "a+")) != NULL) 
    {
        stExecNode *pExecTemp = NULL;
        pExecTemp = gExecList.pHeadExecData;

        while(pExecTemp != NULL)
        {
            
            fprintf (fpDebugLog, "fp: %p   Interval: %ld\n",
                        pExecTemp->ExecTask.fp,
                        pExecTemp->ExecTask.ui32Interval);
            pExecTemp = pExecTemp->pNext;
            nCount++;
        }
        fprintf (fpDebugLog, "List Size: %ld \n", nCount);
            
        fclose( fpDebugLog );

    }
    else
    {
        printf("File Open Failure\n");
    }
#endif    
}
void DebugPrint(char* str) 
{
#ifdef SCH_DEBUG	
    if ((fpDebugLog = fopen (DEBUG_FILE, "a+")) != NULL) 
    {
        fprintf (fpDebugLog, "%s\n",str);
        fclose( fpDebugLog );
    }
#endif    
}

/* End of NASCenter_Scheduler.c */


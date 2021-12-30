#ifndef __NASCENTER_SCHEDULER_H__
#define __NASCENTER_SCHEDULER_H__

/*****************************************************************************
 *
 * NASCenter_Scheduler.h 
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *      D. Sai Siva Kumar
 *
 * description:
 *      NASCenter_Scheduler.h provides common includes,macros,and external
 *              declarations/definitions for users of NASCenter_Scheduler.c
 *
 * CM Details:
 *
 * $Author: howonsuk $
 * $Log: NASCenter_Scheduler.h,v $
 * Revision 1.1  2004/03/03 06:13:42  howonsuk
 * 로그모니터링: 정규식 추가
 * 프로세스모니터링 : 파라미터보여주기 및 파라미터에서 정규식으로 찾기 추가
 *
 * Revision 1.11  2003/12/20 04:08:28  saisk
 * Changed TaskId from I32 to long
 *
 * Revision 1.10  2003/12/15 03:23:31  thiagav
 * Final scheduler code
 *
 * Revision 1.9  2003/12/11 02:22:04  thiagav
 * Added after bug fixes
 *
 * Revision 1.8  2003/12/03 15:59:12  saisk
 * Fixed Unit Testing Bugs and modified Validate and Scheduler to Handle Hourly Schedule
 *
 * Revision 1.7  2003/11/24 11:05:01  saisk
 * Updated and did code clean up
 *
 * Revision 1.6  2003/11/13 09:44:11  saisk
 * 1. Modified SortExecutionList: if start is NULL I am returning.
 * 2. Added ValidateTask function: Implementation should be done
 *
 * Revision 1.5  2003/11/07 07:44:07  saisk
 * Added function ClearMasterTable()
 * Modified SCHStopScheduler and accordingly handled in Schdeuler()
 *
 * Revision 1.4  2003/11/06 13:29:05  saisk
 * Implemented Modify and Delete Task
 *
 * Revision 1.3  2003/11/06 07:25:16  saisk
 * Added Function Header
 *
 * Revision 1.2  2003/11/05 12:26:25  saisk
 * Implemented synchronization , completed Scheduler function , SCH_StartScheduler modified using pThreads
 *
 * Revision 1.1  2003/11/04 13:47:16  saisk
 * First Check In, renamed Scheduler.c and scheduler.h as per coding standard also exported function changed as per coding standard.
 *
 * $State: Exp $
 * $Id: NASCenter_Scheduler.h,v 1.1 2004/03/03 06:13:42 howonsuk Exp $
 * $Name:  $
 *
 ****************************************************************************/



 /********************************** includes *******************************/
#include <time.h>
#include "ctypes.h"
#include "NASCenter_Errors.h"
 /********************************** macros *********************************/

 /******************************* exported types ****************************/
typedef void (*FP)(void *);

typedef enum TYPE{
    SCH_ONE_SHOT, 
    SCH_PERIODIC
}eTYPE;

typedef enum PERIODICITY{
    SCH_SEC_MIN_HOUR=0,
	SCH_DAY,
	SCH_WEEK,
	SCH_MONTH
}ePERIODICITY;

/* Note: Do not change the value of this enum,it has to 
         match to "tm_wday" of "struct tm" defined in time.h */
typedef enum WEEK_DAY{
    SCH_SUN=0,
    SCH_MON,
    SCH_TUE,
    SCH_WED,
    SCH_THU,
    SCH_FRI,
    SCH_SAT,
    SCH_ALL_DAYS
}eWEEK_DAY ;

typedef struct 
{
	UI16 ui16Day;    /* day of the month - [1,31] */
	UI16 ui16Month;  /* months since January - [0,11] */
	UI16 ui16Year;   /* Current Year*/
}stDate;

typedef struct 
{
	UI16 ui16Hour;   /* hours since midnight - [0,23] */
	UI16 ui16Minute; /* minutes after the hour - [0,59] */
	UI16 ui16Second; /* seconds after the minute - [0,59] */
}stTime;

typedef struct
{
	UI16 ui16Day;    /* day of the month - [1,31] */
	UI16 ui16Month;  /* months since January - [0,11] */
	UI16 ui16Year;   /* Current Year */
	UI16 ui16Hour;   /* hours since midnight - [0,23] */
	UI16 ui16Minute; /* minutes after the hour - [0,59] */
	UI16 ui16Second; /* seconds after the minute - [0,59] */
}stDateTime;

typedef struct 
{
	stDate StartDate;
	stDate EndDate;
	stTime StartTime;
	stTime EndTime;
	eTYPE  eType;
	ePERIODICITY ePCity;
	UI32   ui32Interval; /* in Seconds  */
	eWEEK_DAY    eWeekDay;
	FP     fp;
	void   *vpArg;
}stTask;
/*******************************************************************************
Rules to fill "stTask" before calling SCH_AddTask().
Y   - Mandatory
O   - Optional
Os  - One Shot
Pe  - Periodic
"-" - Not Applicable 
---------------------------------------------------------------------------------
      ePCity    |StartDate|EndDate|StartTime|EndTime|eType|ui32Interval|eWeekDay|
---------------------------------------------------------------------------------
SCH_SEC_MIN_HOUR|    Y    |   O   |    Y    |   O   |  Pe |     Y      |   Y    |
---------------------------------------------------------------------------------
SCH_DAY         |    Y    |   O   |    Y    |   O   |  Pe |     -      |   Y    |
---------------------------------------------------------------------------------
SCH_DAY         |    Y    |   O   |    Y    |   O   |  Os |     -      |   Y    |
---------------------------------------------------------------------------------
SCH_WEEK        |    Y    |   O   |    Y    |   O   |  Pe |     -      |   Y    |
---------------------------------------------------------------------------------
SCH_MONTH       |    Y    |   O   |    Y    |   O   |  Pe |     -      |   Y    |
**************************************************************************************/

/************************* exported function prototypes ********************/

/*****************************************************************************
 *
 * name             :   SCH_StartScheduler
 * description      :   Initialize and Starts the Scheduler
 * input parameters :   void
 * output parameters:   void
 * return type      :   void
 * global variables :   
 * calls            :   
 */
void SCH_StartScheduler(void);

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
void SCH_StopScheduler(void);

/*****************************************************************************
 *
 * name             :   SCH_AddTask
 * description      :   Add Task to the Scheduler
 * input parameters :   stTask
 * output parameters:   void
 * return type      :   long ( TaskID )
 * global variables :   
 * calls            :   
 */
long SCH_AddTask(stTask task);

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

long SCH_ModifyTask(long lTaskId, stTask task);


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

ENASCenterError SCH_GetTask(long lTaskId, stTask **ppTask);

/*****************************************************************************
 *
 * name             :   SCH_DeleteTask
 * description      :   Removes Task from the Scheduler
 * input parameters :   long( TaskID)
 * output parameters:   void
 * return type      :   ENASCenterError
 * global variables :   
 * calls            :   
 */

ENASCenterError SCH_DeleteTask(long lTaskId);

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

void WaitForScheduler(void);
/* For testing purpose */
void SCH_AutoCleanTask(void);
 /***************************** exported globals ****************************/


#endif /* __NASCENTER_SCHEDULER_H__ */
/* end of NASCenter_Scheduler.h */

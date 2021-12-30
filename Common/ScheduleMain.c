#ifdef WIN32
#include <windows.h>

#define SLEEP(x)    Sleep(x * 1000)
#elif
#define SLEEP(x)    sleep()
#endif

#include <stdio.h>
#include <time.h>
#include "NASCenter_Scheduler.h"

void One(void *);
void Two(void *);
void Five(void *);
void Ten(void *);
void Fifteen(void *);
void TestScheduleTime(int Interval);

void main(void)
{
	stTask task;
    I32 i32TaskId = -1;
    I32 Newi32TaskId = -1;
  //  stTask *pTask;
  //  pTask = (stTask*) calloc (1,sizeof(stTask));
    SCH_StartScheduler();
  
    
	task.fp = One;
    task.ui32Interval = 1;
    task.StartDate.ui16Day = 2;
    task.StartDate.ui16Month=11;
    task.StartDate.ui16Year = 2003;
    
    task.StartTime.ui16Hour = 0 ;
    task.StartTime.ui16Minute = 10;
    task.StartTime.ui16Second = 0;
    
    task.eWeekDay = SCH_ALL_DAYS;
	task.eType = SCH_PERIODIC ;
    task.ePCity = SCH_SEC_MIN_HOUR;
	i32TaskId = SCH_AddTask(task);
    if (i32TaskId == -1)
    {
        printf("AddTask Failed\n");
    }
    else
    {
        printf("AddTask Success\n");
    }
    //SCH_GetTask(i32TaskId, &pTask);
    //SCH_DeleteTask(i32TaskId);

   
/*
    task.fp = One;
    task.ui32Interval = 1;
    task.StartDate.ui16Day = 3;
    task.StartDate.ui16Month=11;
    task.StartDate.ui16Year = 2003;
    
    task.StartTime.ui16Hour = 0 ;
    task.StartTime.ui16Minute = 1;
    task.StartTime.ui16Second = 0;
    
    task.eWeekDay = SCH_ALL_DAYS;
	task.eType = SCH_PERIODIC ;
    task.ePCity = SCH_SEC_MIN_HOUR;
	i32TaskId = SCH_AddTask(task);
    if (i32TaskId == -1)
    {
        printf("AddTask Failed\n");
    }
    else
    {
        printf("AddTask Success\n");
    }

  */  
//    SCH_GetTask(i32TaskId, &pTask);
    
  //  i32TaskId = SCH_ModifyTask(i32TaskId,task);
	//i32TaskId = SCH_AddTask(task);
    //SCH_DeleteTask(i32TaskId);

    /*
	task.fp = Two;
	task.ui32Interval = 2;
    task.StartTime.ui16Hour = 12 ;
    task.StartTime.ui16Hour = 0;
    task.StartTime.ui16Second = 0;
	task.eType = SCH_ONE_SHOT ;
    task.ePCity = SCH_SEC_MIN_HOUR;
	i32TaskId = SCH_AddTask(task);
    SCH_DeleteTask(i32TaskId);

  
	SLEEP(15);
	SCH_StopScheduler();
    */
    WaitForScheduler();
    
}

void One(void * vp)
{
    /*
	time_t t1,t2;
    struct tm time1;
    struct tm time2;
    static int i = 0;

	time(&t1);
    printf("Time1: %d\n",t1);
    Sleep(1000);
    time(&t2);
    printf("Time2: %d\n",t2);

	//printf(ctime(&t));
    printf("Diff Time: %f f\n",difftime(t2, t1));

    time1.tm_hour = i++;
    time1.tm_min  = 0;
    time1.tm_sec  = 0;
    t1 = mktime(&time1);
    printf(ctime(&t1));
    
    Sleep(1000);
*/
    time_t t1;
    struct tm time1;
    time(&t1);
    printf("One seconds Task: ");
    printf(ctime(&t1));
}
void Two(void * vp)
{
	printf("Two seconds Task\n");
}
void Five(void * vp)
{
	printf("Five seconds Task\n");
}
void Ten(void * vp)
{
	printf("Ten seconds Task\n");
}
void Fifteen(void * vp)
{
	printf("Fifteen seconds Task\n");
}


void TestScheduleTime(int Interval)
{
    int ArrTime[] = {0,3600,7200,10800,14400,18000,21600,25200,28800,32400,36000};
    int i=0;
    int mod=0;
    
    for (i=0;i<11;i++)
    {
        printf("Start Tiime: %d\t, End Time:  %d\t",ArrTime[i] -3600, ArrTime[i]);
        mod = ArrTime[i] % Interval ;
        if ( mod < 3600)
        {
            printf("Schedule\n");
            if (mod == 0)
            {
                   printf(" Schedule Time : %d\n\n ",ArrTime[i]);
            }
            else
            {
                printf(" Schedule Time : %d\n\n ",(ArrTime[i] - 3600 + mod));
            }
        }
        else
        {
            printf("Don't Schedule\n\n");
        }
    }


}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#define NAS_CPU_USER 	0
#define NAS_CPU_KERNEL	1
#define NAS_CPU_WAIT 	2
#define NAS_CPU_IDLE 	3
#define NAS_CPU_UTIL 	4

typedef struct psmon_cpuinfo {
    int  ncpus;
    long sampletime;
    long total;
    long state[5];
    long stateUsage[5];
    long stateUsageFloat[5];
} psmon_cpuinfo_t;


psmon_cpuinfo_t psMonCpuInfo = {0};

void init();
void setCpuAverage();
void getCurrentTime(char *tbuf);


int main()
{
	init();

	while (1) {
		setCpuAverage();
		sleep(30);
	}

	exit(0);
}

void init()
{
    psMonCpuInfo.ncpus = 1;
    psMonCpuInfo.sampletime = 0;
    psMonCpuInfo.total = 0;
    psMonCpuInfo.state[0] = 0;
    psMonCpuInfo.state[1] = 0;
    psMonCpuInfo.state[2] = 0;
    psMonCpuInfo.state[3] = 0;
    psMonCpuInfo.state[4] = 0;
    psMonCpuInfo.stateUsage[0] = 0;
    psMonCpuInfo.stateUsage[1] = 0;
    psMonCpuInfo.stateUsage[2] = 0;
    psMonCpuInfo.stateUsage[3] = 0;
    psMonCpuInfo.stateUsage[4] = 0;
    psMonCpuInfo.stateUsageFloat[0] = 0;
    psMonCpuInfo.stateUsageFloat[1] = 0;
    psMonCpuInfo.stateUsageFloat[2] = 0;
    psMonCpuInfo.stateUsageFloat[3] = 0;
    psMonCpuInfo.stateUsageFloat[4] = 0;
}


#define STAT "/proc/stat"
void setCpuAverage()
{
    int    tmpIdx;
    long   ncpu = 0;
	long   curtotal;
	long   pretotal;
	long   total;
	long   userUsage;
	long   kernelUsage;
	long   waitUsage;
	long   idleUsage;
	long   utilUsage;

	long   userUsageFloat;
	long   kernelUsageFloat;
	long   waitUsageFloat;
	long   idleUsageFloat;
	long   utilUsageFloat;

	long   curTime;
    long   state[4] = { 0 };
    long   tmp_state[4] = { 0 };
    FILE   *fpStat = NULL;
	char   szBuff[80];
    char   tbuf[30] ={0};

    time(&curTime);
	fpStat = fopen(STAT, "r");
	if(fpStat != NULL)
    {
        memset(szBuff, '\0', 80);

        while(fgets(szBuff, 80, fpStat) != NULL)
        {
		    if(!strncmp(szBuff, "cpu ", 4))
			    continue;
		    if(!strncmp(szBuff, "cpu", 3))
            {
			    sscanf(szBuff, "cpu%d %lu %lu %lu %lu ",
			        &tmpIdx, 
			        &tmp_state[NAS_CPU_USER], 
			        &tmp_state[NAS_CPU_WAIT], 
			        &tmp_state[NAS_CPU_KERNEL], 
			        &tmp_state[NAS_CPU_IDLE]);
			    ncpu++;
                state[NAS_CPU_USER]+=tmp_state[NAS_CPU_USER];
                state[NAS_CPU_KERNEL]+=tmp_state[NAS_CPU_KERNEL];
                state[NAS_CPU_WAIT]+=tmp_state[NAS_CPU_WAIT];
                state[NAS_CPU_IDLE]+=tmp_state[NAS_CPU_IDLE];
                memset(szBuff, '\0', 80);
		    }
	    }
        if(fpStat)
	        fclose(fpStat); 
    }
    psMonCpuInfo.ncpus = ncpu;
    
    curtotal = state[NAS_CPU_USER] + state[NAS_CPU_KERNEL] + state[NAS_CPU_WAIT]
        + state[NAS_CPU_IDLE];
    if(psMonCpuInfo.sampletime < 1)
    {
		psMonCpuInfo.stateUsage[NAS_CPU_USER] = 0;
 		psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] = 0;
 		psMonCpuInfo.stateUsage[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_UTIL] = 0;

		psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] = 0;
 		psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] = 0;
 		psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] = 0;
    }
    else
    {
        pretotal = psMonCpuInfo.total;
        total = curtotal - pretotal;
        if(total > 0)
        {
            userUsageFloat =(long) ((state[NAS_CPU_USER]-psMonCpuInfo.state[NAS_CPU_USER]) * 10000.0 /(float)total);
            kernelUsageFloat = (long)((state[NAS_CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_KERNEL]) * 10000.0 /(float)total);
            waitUsageFloat = (long)((state[NAS_CPU_WAIT]-psMonCpuInfo.state[NAS_CPU_WAIT]) * 10000.0 /(float)total);
            idleUsageFloat = (long)((state[NAS_CPU_IDLE]-psMonCpuInfo.state[NAS_CPU_IDLE]) * 10000.0 /(float)total);
            utilUsageFloat = (long)((state[NAS_CPU_USER]+state[NAS_CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_UTIL])* 10000.0 /(float)total);


            userUsage = (long)(userUsageFloat/100.0);
            kernelUsage = (long)(kernelUsageFloat/100.0);
            waitUsage = (long)(waitUsageFloat/100.0);
            idleUsage = (long)(idleUsageFloat/100.0);
            utilUsage = (long)(utilUsageFloat/100.0);

            psMonCpuInfo.stateUsage[NAS_CPU_USER] =
                userUsage > 100 ? 100 : userUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] =
                kernelUsage > 100 ? 100 : kernelUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT] =
                waitUsage > 100 ? 100 : waitUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE] =
                idleUsage > 100 ? 100 : idleUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL] =
                utilUsage > 100 ? 100 : utilUsage;

            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] =
                userUsageFloat > 10000 ? 10000 : userUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] =
                kernelUsageFloat > 10000 ? 10000 : kernelUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] =
                waitUsageFloat > 10000 ? 10000 : waitUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] =
                idleUsageFloat > 10000 ? 10000 : idleUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] =
                utilUsageFloat > 10000 ? 10000 : utilUsageFloat;
        }
    }

    psMonCpuInfo.sampletime = curTime;
    psMonCpuInfo.total = curtotal;
    psMonCpuInfo.state[NAS_CPU_USER] = state[NAS_CPU_USER];
    psMonCpuInfo.state[NAS_CPU_KERNEL] = state[NAS_CPU_KERNEL];
    psMonCpuInfo.state[NAS_CPU_WAIT] = state[NAS_CPU_WAIT];
    psMonCpuInfo.state[NAS_CPU_IDLE] = state[NAS_CPU_IDLE];
    psMonCpuInfo.state[NAS_CPU_UTIL] = state[NAS_CPU_USER]+state[NAS_CPU_KERNEL];

	memset(tbuf, 0, sizeof(tbuf));
    getCurrentTime(tbuf);

    printf("[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
            psMonCpuInfo.stateUsage[NAS_CPU_USER],
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL]);
    printf("[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL]);
}


void getCurrentTime(char *tbuf)
{
    time_t t;
    struct tm *ptm;

    time(&t);
    ptm = localtime(&t);

    sprintf(tbuf,"%02d-%02d [%02d:%02d:%02d]",ptm->tm_mon+1,ptm->tm_mday,
            ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
}

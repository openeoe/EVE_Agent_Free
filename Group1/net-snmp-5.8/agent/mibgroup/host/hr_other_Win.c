/*
 *  Host Resources MIB - other device implementation - hr_other.c
 *
 */

#include <net-snmp/net-snmp-config.h>

#include "host_res_Win.h"
#include "hr_other_Win.h"
#include <windows.h>


#if HAVE_UNISTD_H
#include <unistd.h>
#endif

extern void Init_HR_Disk(void);
extern int Get_Next_HR_Disk(void);
extern void HRPROCGet_Data(void);

SYSTEM_INFO syst;
extern int giHRP_index;
extern int Dcoun;
extern int gPrinterCount;
extern int gCommPortCount;
extern int giNoProcessors;
char string[100];
char tempc[100];


void            Init_HR_CPU(void);
int             Get_Next_HR_CPU(void);
const char     *describe_cpu(int);

void            Init_HR_CoProc(void);
int             Get_Next_HR_CoProc(void);
const char     *describe_coproc(int);

void            Init_HR_Key(void);
int             Get_Next_HR_Key(void);
const char     *describe_Key(int);
/*
void            Init_HR_Serial(void);
int             Get_Next_HR_Serial(void);
const char     *describe_Serial(int);
*/
void            Init_HR_Modem(void);
int             Get_Next_HR_Modem(void);
const char     *describe_Modem(int);

void            Init_HR_Pointer(void);
int             Get_Next_HR_Pointer(void);
const char     *describe_Pointer(int);

void            Init_HR_Vol(void);
int             Get_Next_HR_Vol(void);
const char     *describe_Vol(int);

void            Init_HR_NVol(void);
int             Get_Next_HR_NVol(void);
const char     *describe_NVol(int);

void            Init_HR_Tape(void);
int             Get_Next_HR_Tape(void);
const char     *describe_Tape(int);

void            Init_HR_Para(void);
int             Get_Next_HR_Para(void);
const char     *describe_Para(int);

void            Init_HR_Video(void);
int             Get_Next_HR_Video(void);
const char     *describe_Video(int);

void            Init_HR_Audio(void);
int             Get_Next_HR_Audio(void);
const char     *describe_Audio(int);

void            Init_HR_Clk(void);
int             Get_Next_HR_Clk(void);
const char     *describe_Clk(int);

const char     *describe_Disk(int);

/*Added by Sai on 13.1.2004 */
void            Init_HR_Printer(void);
int             Get_Next_HR_Printer(void);

/*
void Init_HR_SerialPort(void);
int  Get_Next_HR_SerialPort(void);
*/
/*End Added by Sai*/


void
init_hr_other(void)
{
    init_device[HRDEV_PROC] = Init_HR_CPU;
    next_device[HRDEV_PROC] = Get_Next_HR_CPU;
    device_descr[HRDEV_PROC] = describe_cpu;

    init_device[HRDEV_COPROC] = Init_HR_CoProc;
    next_device[HRDEV_COPROC] = Get_Next_HR_CoProc;
    device_descr[HRDEV_COPROC] = describe_coproc;

    init_device[HRDEV_KEYBOARD] = Init_HR_Key;
    next_device[HRDEV_KEYBOARD] = Get_Next_HR_Key;
    device_descr[HRDEV_KEYBOARD] = describe_Key;

    init_device[HRDEV_POINTER] = Init_HR_Pointer;
    next_device[HRDEV_POINTER] = Get_Next_HR_Pointer;
    device_descr[HRDEV_POINTER] = describe_Pointer;

    init_device[HRDEV_DISK] = Init_HR_Disk;
    next_device[HRDEV_DISK] = Get_Next_HR_Disk;
//    device_descr[HRDEV_DISK] = describe_Pointer;


    init_device[HRDEV_PRINTER] = Init_HR_Printer;
    next_device[HRDEV_PRINTER] = Get_Next_HR_Printer;
    //device_descr[HRDEV_PRINTER] = describe_Pointer;

/*
  
    init_device[HRDEV_SERIAL] = Init_HR_SerialPort;
    next_device[HRDEV_SERIAL] = Get_Next_HR_SerialPort;
    */
    //device_descr[HRDEV_SERIAL] = describe_Serial;

/*    init_device[HRDEV_MODEM] = Init_HR_Modem;
    next_device[HRDEV_MODEM] = Get_Next_HR_Modem;
    device_descr[HRDEV_MODEM] = describe_Modem;
*/

/*    
    init_device[HRDEV_VMEM] = Init_HR_Vol;
    next_device[HRDEV_VMEM] = Get_Next_HR_Vol;
    device_descr[HRDEV_VMEM] = describe_Vol;

    init_device[HRDEV_NVMEM] = Init_HR_NVol;
    next_device[HRDEV_NVMEM] = Get_Next_HR_NVol;
    device_descr[HRDEV_NVMEM] = describe_NVol;

    init_device[HRDEV_TAPE] = Init_HR_Tape;
    next_device[HRDEV_TAPE] = Get_Next_HR_Tape;
    device_descr[HRDEV_TAPE] = describe_Tape;

    init_device[HRDEV_PARALLEL] = Init_HR_Para;
    next_device[HRDEV_PARALLEL] = Get_Next_HR_Para;
    device_descr[HRDEV_PARALLEL] = describe_Para;

    init_device[HRDEV_VIDEO] = Init_HR_Video;
    next_device[HRDEV_VIDEO] = Get_Next_HR_Video;
    device_descr[HRDEV_VIDEO] = describe_Video;

    init_device[HRDEV_AUDIO] = Init_HR_Audio;
    next_device[HRDEV_AUDIO] = Get_Next_HR_Audio;
    device_descr[HRDEV_AUDIO] = describe_Audio;

    init_device[HRDEV_CLOCK] = Init_HR_Clk;
    next_device[HRDEV_CLOCK] = Get_Next_HR_Clk;
    device_descr[HRDEV_CLOCK] = describe_Clk;
*/
}


static int      done_CPU;

void
Init_HR_CPU(void)
{
    done_CPU = 0;
}

int
Get_Next_HR_CPU(void)
{
    /*
     * Assumes a single CPU system
     * I think it's safe to assume at least one! 
     */
    //snmp_log(LOG_INFO,"No of Processor %d\n",giNoProcessors);
    //snmp_log(LOG_INFO,"The value of done_CPU %d\n",done_CPU);
    if (done_CPU <giNoProcessors) {
        return (HRDEV_PROC << HRDEV_TYPE_SHIFT)+ done_CPU++;
    } else
        return -1;
}

const char     *
describe_cpu(int idx)
{
#ifdef _SC_CPU_VERSION
    int             result;

    result = sysconf(_SC_CPU_VERSION);
    switch (result) {
    case CPU_HP_MC68020:
        return (" Motorola MC68020 ");
    case CPU_HP_MC68030:
        return (" Motorola MC68030 ");
    case CPU_HP_MC68040:
        return (" Motorola MC68040 ");
    case CPU_PA_RISC1_0:
        return (" HP PA-RISC 1.0 ");
    case CPU_PA_RISC1_1:
        return (" HP PA-RISC 1.1 ");
    case CPU_PA_RISC1_2:
        return (" HP PA-RISC 1.2 ");
    case CPU_PA_RISC2_0:
        return (" HP PA-RISC 2.0 ");
    default:
        return ("An electronic chip with an HP label");

    }
#else
            GetSystemInfo(&syst);
            strcpy(string,"System Processor:");
            switch(syst.wProcessorArchitecture)
            {
               case PROCESSOR_ARCHITECTURE_INTEL:
                    strcat(string," PROCESSOR_ARCHITECTURE_INTEL");
                    break;
               case PROCESSOR_ARCHITECTURE_IA64:
                    strcat(string,"PROCESSOR_ARCHITECTURE_IA64");
                    break;
               case PROCESSOR_ARCHITECTURE_MIPS:
                    strcat(string,"PROCESSOR_ARCHITECTURE_MIPS");
                    break;
               case PROCESSOR_ARCHITECTURE_ALPHA:
                    strcat(string,"PROCESSOR_ARCHITECTURE_ALPHA");
                    break;
               case PROCESSOR_ARCHITECTURE_PPC:
                    strcat(string,"PROCESSOR_ARCHITECTURE_PPC");
                    break;
             }

            sprintf(tempc," %d",syst.dwProcessorType);
            strcat(string,tempc);
		    return string;
#endif
}



static int      done_coProc;

void
Init_HR_CoProc(void)
{
    done_coProc = 0;
}

int
Get_Next_HR_CoProc(void)
{
    /*
     * How to identify the presence of a co-processor ? 
     */

    if (done_coProc != 1) {
        done_coProc = 1;
        return (HRDEV_COPROC << HRDEV_TYPE_SHIFT);
    } else
        return -1;
}


const char     *
describe_coproc(int idx)
{
    return ("Guessing that there's a floating point co-processor");
}


static int      done_key;

void
Init_HR_Key(void)
{
    done_key = 0;
    HRPROCGet_Data();
}

int
Get_Next_HR_Key(void)
{
    /*
     * How to identify the presence of a co-processor ? 
     */

    if (done_key != 1) {
        done_key = 1;
        return (HRDEV_KEYBOARD << HRDEV_TYPE_SHIFT);
    } else
        return -1;
}


const char     *
describe_Key(int idx)
{
    int gitype;
    gitype=GetKeyboardType(0);	
	switch(gitype)
	{
        case 1:
		    return("IBM PC/XT or compatible (83-key) keyboard");
		    
        case 2:
            return("Olivetti ICO (102-key) keyboard");
            
        case 3:
            return("IBM PC/AT (84-key) or similar keyboard ");
            
        case 4:
            return("IBM enhanced (101- or 102-key) keyboard");
            
        case 5:
            return("Nokia 1050 and similar keyboards");
            
        case 6:
            return("Olivetti Nokia 1050 and similar keyboards");
            
        case 7:
            return("Japanese keyboard");
            
        default:
            return("Unknown");
            
    }
}

/*
static int      done_Serial;

void
Init_HR_Serial(void)
{
    done_Serial = 0;
}

int
Get_Next_HR_Serial(void)
{
  
    if (done_Serial != 1) {
        done_Serial = 1;
        return (HRDEV_SERIAL << HRDEV_TYPE_SHIFT);
    } else
        return -1;
}


const char     *
describe_Serial(int idx)
{
    return ("Serial Device present in the System");
}

*/
static int      done_Modem;

void
Init_HR_Modem(void)
{
    done_Modem = 0;
}

int
Get_Next_HR_Modem(void)
{
    /*
     * How to identify the presence of a co-processor ? 
     */

    if (done_Modem != 1) {
        done_Modem = 1;
        return (HRDEV_MODEM << HRDEV_TYPE_SHIFT);
    } else
        return -1;
}


const char     *
describe_Modem(int idx)
{
    return ("Modem present in the System");
}


static int      done_Pointer;

void
Init_HR_Pointer(void)
{
    done_Pointer = 0;
}

int
Get_Next_HR_Pointer(void)
{
    /*
     * How to identify the presence of a co-processor ? 
     */

    if (done_Pointer != 1) {
        done_Pointer = 1;
        return (HRDEV_POINTER << HRDEV_TYPE_SHIFT);
    } else
        return -1;
}


const char     *
describe_Pointer(int idx)
{
    int gitype;
    char str[100];
    memset(str,'\0',100);
    gitype=GetSystemMetrics(SM_CMOUSEBUTTONS);
         
    if(gitype)
    {
        sprintf(str,"A %d Button Mouse is present in the System",gitype);
        return(str);
    }
    else
        return ("Unknown Pointing device present in the System");
}

static int      done_Vol;

void
Init_HR_Vol(void)
{
    done_Vol = 0;
}

int
Get_Next_HR_Vol(void)
{
    /*
     * How to identify the presence of a co-processor ? 
     */

    if (done_Vol != 1) {
        done_Vol = 1;
        return (HRDEV_VMEM << HRDEV_TYPE_SHIFT);
    } else
        return -1;
}


const char     *
describe_Vol(int idx)
{
    return ("Volatile Memory Present in the system");
}

static int      done_NVol;

void
Init_HR_NVol(void)
{
    done_NVol = 0;
}

int
Get_Next_HR_NVol(void)
{
    /*
     * How to identify the presence of a co-processor ? 
     */

    if (done_NVol != 1) {
        done_NVol = 1;
        return (HRDEV_NVMEM << HRDEV_TYPE_SHIFT);
    } else
        return -1;
}


const char     *
describe_NVol(int idx)
{
    return ("Non-Volatile Memory Present in the system");
}


static int      done_Tape;

void
Init_HR_Tape(void)
{
    done_Tape = 0;
}

int
Get_Next_HR_Tape(void)
{
    /*
     * How to identify the presence of a co-processor ? 
     */

    if (done_Tape != 1) {
        done_Tape = 1;
        return (HRDEV_TAPE << HRDEV_TYPE_SHIFT);
    } else
        return -1;
}


const char     *
describe_Tape(int idx)
{
    return ("Tape-Drive Present in the system");
}

static int      done_Para;

void
Init_HR_Para(void)
{
    done_Para = 0;
}

int
Get_Next_HR_Para(void)
{
    /*
     * How to identify the presence of a co-processor ? 
     */

    if (done_Para != 1) {
        done_Para = 1;
        return (HRDEV_PARALLEL << HRDEV_TYPE_SHIFT);
    } else
        return -1;
}


const char     *
describe_Para(int idx)
{
    return ("Parallel Device Present in the system");
}


static int      done_Video;

void
Init_HR_Video(void)
{
    done_Video = 0;
}

int
Get_Next_HR_Video(void)
{
    /*
     * How to identify the presence of a co-processor ? 
     */

    if (done_Video != 1) {
        done_Video = 1;
        return (HRDEV_VIDEO << HRDEV_TYPE_SHIFT);
    } else
        return -1;
}


const char     *
describe_Video(int idx)
{
    return ("Video Device Present in the system");
}

static int      done_Audio;

void
Init_HR_Audio(void)
{
    done_Audio = 0;
}

int
Get_Next_HR_Audio(void)
{
    /*
     * How to identify the presence of a co-processor ? 
     */

    if (done_Audio != 1) {
        done_Audio = 1;
        return (HRDEV_AUDIO << HRDEV_TYPE_SHIFT);
    } else
        return -1;
}


const char     *
describe_Audio(int idx)
{
    return ("Audio Device Present in the system");
}

static int      done_Clk;

void
Init_HR_Clk(void)
{
    done_Clk = 0;
}

int
Get_Next_HR_Clk(void)
{
    /*
     * How to identify the presence of a co-processor ? 
     */

    if (done_Clk != 1) {
        done_Clk = 1;
        return (HRDEV_CLOCK << HRDEV_TYPE_SHIFT);
    } else
        return -1;
}


const char     *
describe_Clk(int idx)
{
    return ("Clock Present in the system");
}

/*Added by Sai on 13.1.2004*/

static int      done_printer;

void
Init_HR_Printer(void)
{
    done_printer = 0;
    gPrinterCount = GetPrinterDetails();
}

int
Get_Next_HR_Printer(void)
{
   
    if (done_printer < gPrinterCount) {
         return ((HRDEV_PRINTER << HRDEV_TYPE_SHIFT)+done_printer++);
    } else
        return -1;
}

/*
static int      done_serialports;

void
Init_HR_SerialPort(void)
{
    done_serialports = 0;
    gCommPortCount = GetCommPortDetails();
}

int
Get_Next_HR_SerialPort(void)
{
    
    if (done_serialports < gCommPortCount) {
         return ((HRDEV_SERIAL << HRDEV_TYPE_SHIFT)+done_serialports++);
    } else
        return -1;
}
*/

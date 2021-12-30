#ifndef __NASCENTER_PING_H__
#define __NASCENTER_PING_H__
/*****************************************************************************
 *
 * NASCenter_Ping.h 
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *      Aravind Balan
 *
 * description:
 *      NASCenter_FileTrans.h provides common includes,macros,and external
 *      declarations/definitions for users of 
 *      NASCenter_FileTrans_SendFun.c & NASCenter_FileTrans_RecvFun.c
 *
 * CM Details:
 *
 * $Author: vijaige $
 * $Log: NASCenter_Ping.h,v $
 * Revision 1.1  2003/12/04 03:54:06  vijaige
 * after porting for windows
 *
 * Revision 1.4  2003/11/11 06:17:05  baravind
 * Added by Aravind.B on 11-NOV-2003
 *
 * Revision 1.2  2003/11/06 09:17:34  baravind
 * Checked in by Aravind Balan on 6-Nov-2003
 *
 * Revision 1.1  2003/11/03 08:39:08  baravind
 * Added by Aravind.B on Nov-3-2003 
 *
 * Revision 1 2003/10/27 04:00:00  hari
 * no message
 *
 * Revision 1.1.1.1  2003/10/01 05:23:22  thiagav
 * no message
 *
 * $State: Exp $
 * $Id: NASCenter_Ping.h,v 1.1 2003/12/04 03:54:06 vijaige Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/

/*typedef struct {
    unsigned char Ttl;                         // Time To Live
    unsigned char Tos;                         // Type Of Service
    unsigned char Flags;                       // IP header flags
    unsigned char OptionsSize;                 // Size in bytes of options data
    unsigned char *OptionsData;                // Pointer to options data
} IP_OPTION_INFORMATION, * PIP_OPTION_INFORMATION;*/
typedef HANDLE (WINAPI* pfnHV)(VOID);
typedef BOOL (WINAPI* pfnBH)(HANDLE);
typedef DWORD (WINAPI* pfnDHDPWPipPDD)(HANDLE, DWORD, LPVOID, WORD,
            PIP_OPTION_INFORMATION, LPVOID, DWORD, DWORD); // evil, no?
typedef struct {
    DWORD Address;                             // Replying address
    unsigned long  Status;                     // Reply status
    unsigned long RoundTripTime;              // RTT in milliseconds
    unsigned short DataSize;                   // Echo data size
    unsigned short Reserved;                   // Reserved for system use
    void *Data;                                // Pointer to the echo data
    IP_OPTION_INFORMATION Options;             // Reply options
} IP_ECHO_REPLY, * PIP_ECHO_REPLY;
#endif /* __NASCENTER_PING_H__ */
#ifndef __NASCENTER_FILETRANS_COMMON_H__
#define __NASCENTER_FILETRANS_COMMON_H__

/*****************************************************************************
 *
 * NASCenter_FileTrans_Common.h 
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *      Aravind Balan
 *
 * description:
 *      NASCenter_FileTrans_Common.h provides common includes,macros,and 
 *      external  declarations/definitions for users of FileTransfer 
 *
 * CM Details:
 *
 * $Author: vijaige $
 * $Log: NASCenter_FileTrans_Common.h,v $
 * Revision 1.2  2003/12/11 11:29:52  vijaige
 * fixed handle leak
 *
 * Revision 1.1  2003/12/11 07:53:53  baravind
 * Added by Aravind.B on Dec-11-2003
 *
 * Revision 1.1.1.2  2003/10/17 06:41:51  hari
 * no message
 *
 * Revision 1.1.1.1  2003/10/01 05:23:22  thiagav
 * no message
 *
 * $State: Exp $
 * $Id: NASCenter_FileTrans_Common.h,v 1.2 2003/12/11 11:29:52 vijaige Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/



 /********************************** includes *******************************/
#ifndef WIN32
 #include <stdio.h>
 #include <string.h>
 #include <sys/types.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <netdb.h>
 #include <unistd.h>
 #include <sys/time.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <pthread.h>
 #include <errno.h>
 #include <sys/param.h>
 #include <sys/socket.h>
 #include <sys/file.h>
 #include <netinet/in_systm.h>
 #include <netinet/ip.h>
 #include <netinet/ip_icmp.h>
 #include <netdb.h>
 #include <signal.h>
 #include <stdlib.h>
#endif

 #ifndef  __NASCENTER_ERRORS_H__
 #include <NASCenter_Errors.h>
 #endif


 #ifndef __ctypes_H__
 #include <ctypes.h>
 #endif


 #ifndef __NASCENTER_COMMON_H__
 #include <NASCenter_Common.h>
 #endif




 /********************************** macros *********************************/


 /******************************* exported types ****************************/


 /************************* exported function prototypes ********************/


 /***************************** exported globals ****************************/




#endif /* __NASCENTER_FILETRANS_COMMON_H__ */

/* end of NASCenter_FileTrans_Common.h */

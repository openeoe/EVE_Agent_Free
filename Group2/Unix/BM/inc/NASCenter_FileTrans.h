#ifndef __NASCENTER_FILETRANS_H__
#define __NASCENTER_FILETRANS_H__

/*****************************************************************************
 *
 * NASCenter_FileTrans.h 
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
 * $Author: thiagav $
 * $Log: NASCenter_FileTrans.h,v $
 * Revision 1.3  2003/12/15 15:56:36  thiagav
 * Added
 *
 * Revision 1.2  2003/12/04 08:23:31  vijaige
 * windows porting
 *
 * Revision 1.1  2003/12/02 11:30:49  thiagav
 * Added
 *
 * Revision 1.3  2003/11/29 14:35:02  guga
 * Added after unit testing
 *
 * Revision 1.1  2003/11/12 08:23:23  thiagav
 * Added after compilation
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
 * $Id: NASCenter_FileTrans.h,v 1.3 2003/12/15 15:56:36 thiagav Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/



 /********************************** includes *******************************/


#ifndef  __NASCENTER_ERRORS_H__
#include <NASCenter_Errors.h>
#endif

#ifndef __ctypes_H__
#include <ctypes.h>
#endif

#ifndef __NASCENTER_COMMON_H__
#include <NASCenter_Common.h>
#endif

#ifndef WIN32
#include <sys/mman.h>
#endif
#include <limits.h>


 /********************************** macros *********************************/

 extern ENASCenterError FileTrans_SendFile(UI8 * pcFileName , 
                                   struct sockaddr_in *pstDestAddr , 
                                   UI8 *pcDestPath , I32 iMsgSize);


extern  void  FileTrans_RecvFile(struct sockaddr_in * pstSocketAddress);

extern  ENASCenterError FileTrans_SendData(struct sockaddr_in * pstSocketAddress , 
                                   void * pdata);

extern void * FileTrans_RecvData(struct sockaddr_in *pstSocketAddress);
 

 


 /***************************** exported globals ****************************/


#endif /* __NASCENTER_FILETRANS_H__ */

/* end of filename.h */

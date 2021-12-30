#ifndef __NASCENTER_FILETRANS_UTIL_H__
#define __NASCENTER_FILETRANS_UTIL_H__

/*****************************************************************************
 *
 * NASCenter_FileTrans_Util.h
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *      Aravind Balan
 *
 * description:
 *      NASCenter_FileTrans_Util.h provides prototype for utility functions
 * 
 * CM Details:
 *
 * $Author: baravind $
 * $Log: NASCenter_FileTrans_Util.h,v $
 * Revision 1.11  2003/12/11 07:54:29  baravind
 * Added by Aravind on Dec-11-2003
 *
 * Revision 1.10  2003/12/09 15:16:59  baravind
 * Incorporated code reivew comments - Aravind.B 9-Dec-2003
 *
 * Revision 1.9  2003/12/04 03:53:38  vijaige
 * after porting for windows
 *
 * Revision 1.8  2003/12/01 05:09:49  baravind
 * Added by Aravind.B on Dec-1-2003
 *
 * Revision 1.4  2003/11/15 11:29:42  vijaige
 * 15th nov checkin
 *
 * Revision 1.4  2003/11/11 06:17:26  baravind
 * Added by Aravind.B on 11-NOV-2003
 *
 * Revision 1.2  2003/11/06 09:18:31  baravind
 * Checked in by Aravind Balan on 6-Nov-2003
 *
 * Revision 1.1  2003/11/03 08:40:05  baravind
 * Added by Aravind.B on Nov-3-2003
 *
 * Revision 1.1.1.2  2003/10/17 06:41:51  hari
 * no message
 *
 * Revision 1.1.1.1  2003/10/01 05:23:22  thiagav
 * no message
 *
 * $State: Exp $
 * $Id: NASCenter_FileTrans_Util.h,v 1.11 2003/12/11 07:54:29 baravind Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/



 /********************************** includes *******************************/
 #ifndef __NASCENTER_FILETRANS_COMMON_H__
 #include <NASCenter_FileTrans_Common.h>
 #endif


 /********************************** macros *********************************/


 /******************************* exported types ****************************/

 typedef struct stSequence
 {
     I32 iMmapNo;
     I32 iCorrelater;
 }stSequence;

 /************************* exported function prototypes ********************/
#ifdef WIN32
 void FileTrans_GetTimeDiff ( struct _timeb  * pstOutTime , 
                              struct _timeb  * pstInTime);
#else
 void FileTrans_GetTimeDiff ( struct timeval * pstOutTime ,
                              struct timeval * pstInTime);
#endif

 ENASCenterError FileTrans_GetRTT(struct sockaddr_in *  pstDestAddr ,
                                 double * pdRtt);

 ENASCenterError FileTransfer_InitHandshake( I32 iSocketFd ,
                                        struct sockaddr_in * pstDestAddr ,
                                        struct sockaddr_in * pstDestPortDetail ,
                                        double dRtt  , UI8 * pcFileName ,
                                        UI8 * pcDestPath , I32 iTotNoOfPkts ,
                                        I32 iNoPktsPerMMap ,
                                        I32 iExtraPktsPerMap , 
                                        I32 iLastPktSize , I32 iMsgSize ,
                                        I32 iNoOfMappingNeeded);

#ifdef WIN32
 ENASCenterError  FileTransfer_SendData (I32 iSocketFd , UI8 * pcFileName ,
                                        HANDLE FileHandle , I32 iFileSize ,
                                        I32 iTotNoOfPkts ,
                                        I32 iPageSize , I32 iNoOfMappingNeeded ,
                                        I32 iNoPktsPerMMap , I32 iMsgSizei ,
                                        double dRtt , I32 iLastPktSize, 
										I32 iExtraMapForLastMap , struct sockaddr_in *pstSockAddr);
#else
 ENASCenterError  FileTransfer_SendData (I32 iSocketFd , UI8 * pcFileName ,
                                        I32 iFileDescriptor , I32 iFileSize ,
                                        I32 iTotNoOfPkts ,
                                        I32 iPageSize , I32 iNoOfMappingNeeded ,
                                        I32 iNoPktsPerMMap , I32 iMsgSizei ,
                                        double dRtt , I32 iLastPktSize  ,
                                        I32 iExtraMapForLastMap ,
                                        struct sockaddr_in *pstSockAddr);
#endif

 void FileTrans_ProcessRecvMsg ( I32 iSocketFd);



 /***************************** exported globals ****************************/

#endif /* __FILENAME_H__ */

/* end of filename.h */

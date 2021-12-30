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
 * $Author: howonsuk $
 * $Log: NASCenter_FileTrans.h,v $
 * Revision 1.1  2004/03/03 06:13:42  howonsuk
 * 로그모니터링: 정규식 추가
 * 프로세스모니터링 : 파라미터보여주기 및 파라미터에서 정규식으로 찾기 추가
 *
 * Revision 1.2  2003/12/11 12:53:11  saisk
 * 1. Changed include from double quote to angular bracket
 *
 * Revision 1.1  2003/12/09 09:50:27  thiagav
 * *** empty log message ***
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
 * $Id: NASCenter_FileTrans.h,v 1.1 2004/03/03 06:13:42 howonsuk Exp $
 * $Name:  $
 *
 ****************************************************************************/



 /********************************** includes *******************************/


#ifndef  __NASCENTER_ERRORS_H__
#include "NASCenter_Errors.h"
#endif


#ifndef __ctypes_H__
#include "ctypes.h"
#endif


#ifndef __NASCENTER_COMMON_H__
#include "NASCenter_Common.h"
#endif

#ifndef WIN32
#include <sys/mman.h>
#endif
#include <limits.h>


 /********************************** macros *********************************/

#define MAX_NO_OF_RETRIES 5
#define DEF_MSG_SIZE 512
#define PATH_TO_STORE "."
#define VALUE_SEPERATOR ',' 
#define MAX_FILE_NAME_LEN 500 
#define MAX_PATH_LEN 500 
#define DELTA_TIME_FOR_RECV_DATA 5
#define DELTA_TIME_FOR_RECV_ACK  7

#define FREE_PTR(pointer) if ( pointer != NULL) \
                         { \
                             free(pointer); \
                             pointer = NULL; \
                         }


 /******************************* exported types ****************************/


typedef enum ePacketType 
{
  HND_SHAKE = 0 , 
  HND_SHAKE_ACK,
  CNTRL ,
  CNTRL_ACK,
  DATA ,
  DATA_ACK,
  DATA_NEG_ACK
}ePacketType;

 typedef struct stPacketHeader
 {
   ePacketType ePktType;
   I32  eModuleId;
   I32 iPktNo;
   I32 iMmapNo;
   I32 iCorrelater;
  }stPacketHeader;    

  typedef struct stCntrlData
  {
    I32 dRtt;
    I32  iTotNoPkts;
    I32  iTotNoMaps;
    I32  iNoPktsPerMMap;
    I32  iNoExtraMapPackets;
    I32  iMesgSizePerPkt;
    I32  iMesgSizeForLastPkt;
    I8  pcDestPath[MAX_PATH_LEN] ;
    I8  pcFileName [MAX_FILE_NAME_LEN];
  }stCntrlData;

typedef struct stmsg 
{
	stPacketHeader pvHeader;
	I32 iDataLen;
	void *pvData;
}StMsg;

 /************************* exported function prototypes ********************/


ENASCenterError FileTrans_SendFile(UI8 * pcFileName , 
                                   struct sockaddr_in *pstDestAddr , 
                                   UI8 *pcDestPath , I32 iMsgSize);


void  FileTrans_RecvFile(struct sockaddr_in * pstSocketAddress);

void FileTrans_ConvertPktHdrToHostForm(stPacketHeader *pstPktHdrInNetForm ,
                                  stPacketHeader *pvHeader);

 /***************************** exported globals ****************************/


#endif /* __NASCENTER_FILETRANS_H__ */

/* end of filename.h */

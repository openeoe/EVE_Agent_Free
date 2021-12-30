#ifndef __NASCENTER_COMMON_H__
#define __NASCENTER_COMMON_H__

/*****************************************************************************
 *
 * NASCenter_Common.h
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *       C.VijaiGeetha
 *
 * description:
 *      NASCenter_Common.h defines all common errors.
 *
 * CM Details:
 *
 * $Author: howonsuk $
 * $Log: NASCenter_Common.h,v $
 * Revision 1.1  2004/03/03 06:13:42  howonsuk
 * 로그모니터링: 정규식 추가
 * 프로세스모니터링 : 파라미터보여주기 및 파라미터에서 정규식으로 찾기 추가
 *
 * Revision 1.6  2003/11/05 08:57:02  vijaige
 * after set and get working
 *
 * $State: Exp $
 * $Id: NASCenter_Common.h,v 1.1 2004/03/03 06:13:42 howonsuk Exp $
 * $Name:  $
 *
 ****************************************************************************/

/********************************** includes *******************************/

/********************************** macros *********************************/

/******************************* exported types ****************************/

/*Module ID*/
typedef enum   {
    
    APPMET=1,
    FILBRWS,
    USRMGMT,
    ICPROC,
    FILDIST,
    JOBPROC,
	BACKMGMT,
	USERMET

    /*Module IDs*/


}ENASCenterModuleId;

/************************* exported function prototypes ********************/

/***************************** exported globals ****************************/




#endif /* __NASCENTER_COMMON_H__ */

/* end of NASCenter_Errors.h */



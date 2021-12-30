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
 * $Author: vijaige $
 * $Log: NASCenter_Common.h,v $
 * Revision 1.6  2003/11/05 08:57:02  vijaige
 * after set and get working
 *
 * $State: Exp $
 * $Id: NASCenter_Common.h,v 1.6 2003/11/05 08:57:02 vijaige Exp $
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



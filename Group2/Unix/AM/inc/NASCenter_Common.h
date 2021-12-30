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
 * $Author: karthis $
 * $Log: NASCenter_Common.h,v $
 * Revision 1.1  2003/11/20 05:37:00  karthis
 * minor
 *
 * Revision 1.6  2003/11/05 08:57:02  vijaige
 * after set and get working
 *
 * $State: Exp $
 * $Id: NASCenter_Common.h,v 1.1 2003/11/20 05:37:00 karthis Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
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



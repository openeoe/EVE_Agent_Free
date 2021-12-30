#ifndef __NASCENTER_ERRORS_H__
#define __NASCENTER_ERRORS_H__

/*****************************************************************************
 *
 * NASCenter_errors.h
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *       D. Sai Siva Kumar
 *
 * description:
 *      NASCenter_Errors.h defines all common errors.
 *
 * CM Details:
 *
 * $Author: karthis $
 * $Log: NASCenter_Errors.h,v $
 * Revision 1.1  2003/11/20 05:36:54  karthis
 * minor
 *
 * Revision 1.6  2003/11/05 08:57:02  vijaige
 * after set and get working
 *
 * $State: Exp $
 * $Id: NASCenter_Errors.h,v 1.1 2003/11/20 05:36:54 karthis Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/

/********************************** includes *******************************/

/********************************** macros *********************************/

/******************************* exported types ****************************/
typedef enum   {
    /* Common Errors */
    NASC_FAILURE=0,
    NASC_SUCCESS,
    NASC_INVALID_PARAM,
    NASC_INSUFFICIENT_MEMORY,
    NASC_TIMEOUT,
    NASC_ILLEGAL_OPERATION,

    /* Module1 Specific Errors */    

    /* Module2 Specific Errors */    


}ENASCenterError;


/************************* exported function prototypes ********************/

/***************************** exported globals ****************************/




#endif /* __NASCENTER_ERRORS_H__ */

/* end of NASCenter_Errors.h */




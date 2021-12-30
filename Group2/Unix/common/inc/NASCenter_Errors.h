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

 * $Author: thiagav $

 * $Log: NASCenter_Errors.h,v $
 * Revision 1.1  2003/10/31 13:11:31  thiagav
 * Added in to CVS first time
 *

 * $State: Exp $

 * $Id: NASCenter_Errors.h,v 1.1 2003/10/31 13:11:31 thiagav Exp $

 * $Name:  $

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
    NASC_SOCK_CREATION_FAILED ,

    /* Module1 Specific Errors */

    /* File Transfer ErrorCodes */

    NASC_FILE_TRANS_IGNORE_ERROR ,
    NASC_FILE_TRANS_CONN_CLOSED ,
    NASC_FILE_TRANS_TIME_OUT ,
    NASC_ICP_IGNORE_ERROR ,
    NASC_ICP_TIME_OUT


}ENASCenterError;





/************************* exported function prototypes ********************/



/***************************** exported globals ****************************/









#endif /* __NASCENTER_ERRORS_H__ */



/* end of NASCenter_Errors.h */








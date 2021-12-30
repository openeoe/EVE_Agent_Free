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

 * $Author: howonsuk $

 * $Log: NASCenter_Errors.h,v $
 * Revision 1.1  2004/03/03 06:13:42  howonsuk
 * 로그모니터링: 정규식 추가
 * 프로세스모니터링 : 파라미터보여주기 및 파라미터에서 정규식으로 찾기 추가
 *
 * Revision 1.1  2003/10/31 13:11:31  thiagav
 * Added in to CVS first time
 *

 * $State: Exp $

 * $Id: NASCenter_Errors.h,v 1.1 2004/03/03 06:13:42 howonsuk Exp $

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
    NASC_FILE_TRANS_TIME_OUT


}ENASCenterError;





/************************* exported function prototypes ********************/



/***************************** exported globals ****************************/









#endif /* __NASCENTER_ERRORS_H__ */



/* end of NASCenter_Errors.h */








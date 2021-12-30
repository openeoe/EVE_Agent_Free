/*****************************************************************************
 *
 * filename.c: NASCenter_ICP_Utils_FB
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Aravind Balan
 *
 * description :
 *      This file contains all utility functions for ICP module
 *
 * TODO:
 *      [ Things to be improved ]
 *
 * CM Details:
 *
 * $Author: sspriya $
 * $Log: NASCenter_ICP_FileBRWS.c,v $
 * Revision 1.14  2004/06/16 13:41:40  sspriya
 * Storing timeofday after read from client to be used in fix to client system blackout problem in ICP.
 *
 * Revision 1.13  2004/01/13 05:07:36  baravind
 * Updates after a round of purify test
 *
 * Revision 1.12  2004/01/06 10:53:53  baravind
 * Added htonl / ntohl to enable ICP get commands from different OS
 *
 * Revision 1.11  2004/01/06 09:59:29  baravind
 * Added pclose after popen
 *
 * Revision 1.10  2004/01/05 05:30:21  baravind
 * Fixed Code review comments
 *
 * Revision 1.9  2003/12/29 12:15:01  sig
 * *** empty log message ***
 *
 * Revision 1.8  2003/12/24 13:25:02  sig
 * Added by Aravind.B . Fix for Linux . Changed from popen -> system
 *
 * Revision 1.7  2003/12/20 02:47:03  baravind
 * Added by Aravind.B on Dec-20-2003 . Fixed ST bugs.
 *
 * Revision 1.6  2003/12/17 07:48:51  baravind
 * Added by Aravind on Dec-17-2003  - Released for system testing
 *
 * Revision 1.5  2003/12/16 08:56:55  baravind
 * Added by Aravind.B on 16-Dec-2003 . Code for Linux
 *
 * Revision 1.4  2003/12/11 05:16:32  baravind
 * Afrer Unit testing (1) - Added by Aravind.B on Dec-11-2003
 *
 * Revision 1.3  2003/12/08 11:26:48  baravind
 * Added by Aravind.B on 8-Dev-2003
 *
 * Revision 1.1.1.2  2003/10/17 06:41:51  hari
 * no message
 *
 * Revision 1.1.1.1  2003/10/01 05:23:22  thiagav
 * no message
 *
 * $State: Exp $
 * $Id: NASCenter_ICP_FileBRWS.c,v 1.14 2004/06/16 13:41:40 sspriya Exp $
 * $Name:  $
 *
 ****************************************************************************/


/********************************* includes *********************************/
#ifndef NASCENTER_ICP_H
#include <NASCenter_ICP.h>
#endif



/****************************** local macros ********************************
 *
 * (Macros shareable with rest of the modules are present in .h file)
 */


/********************* local structure/union definitions ********************/



/********************* prototypes for local functions ***********************/



/************************* Globals within this module ***********************/

stCmdPacket  gstSendCmdPkt;



/************************* externs  ***********************/
extern UI8 gcPrompt [] ;
extern UI8 gcPktDelimiter ;
extern UI8 gEqCmdDelimiter ;
extern UI8 *gpcErrorMsg[] ;
extern UI8 * gpcEquivalentCmd [] ; 
extern struct timeval stLastReadTimeValue;


/*****************************************************************************
 *
 * name             :   ICP_ProcessFileBrwsCmd
 * description      :   This Function processes FileBrowser commands
 * input parameters :   I32 iSocketFd  TCP socket id
 *                      stCmdPacket * pstCmdPkt  command packet
 *
 * output parameters:   None  
 *                      
 * return type      :   ENASCenterError
 * global variables :   gcPrompt , gcPktDelimiter , gEqCmdDelimiter , 
 *                      gpcErrorMsg , gpcEquivalentCmd
 * calls            :   ICP_ExtractParams , ICP_SendPkt , ICP_FormCmd , 
 *                      ICP_ExecCommand
 ****************************************************************************/
 
ENASCenterError ICP_ProcessFileBrwsCmd (I32 iSocketFd , stCmdPacket * pstCmdPkt)
{
    I32 iCommandType ;
    UI8 *pcCommandToExec = NULL;
    ENASCenterError eRetValue = NASC_SUCCESS;
    UI8  *pcCurDir = NULL;
    UI8  **pcParamList = NULL;
    I32  iNoOfParam = 0;
    BOOL  iFirstTime = TRUE;
    UI8 *pucScratchForCmd = NULL;
    I32 iTotalCmdSize = 0;
    I32 iMesgSize = 0;
    int iPipeId[2];
    I32 iPileFlag;
    int retval;

    ICP_Signal(SIGUSR1 , SIG_IGN);

     /* create pipe . This will be used for getting errors if any on
       executing  command */

    pipe(iPipeId);

    iPileFlag = fcntl(iPipeId[0] , F_GETFL , 0);

    retval = fcntl(iPipeId[0] , F_SETFL , iPileFlag | O_NONBLOCK);

    if ( retval < 0 ) perror("fcntl failed..");

    /* dup it to stderr */

    dup2(iPipeId[1] , 2);

    close(iPipeId[1]);

    while ( 1 ) 
    {
        if ( iFirstTime != TRUE)
        {
            memset(pstCmdPkt , '\0' , sizeof(stCmdPacket));
            iMesgSize  =  read(iSocketFd , pstCmdPkt , sizeof(stCmdPacket));
            gettimeofday(&stLastReadTimeValue , NULL);
            if ( iMesgSize == 0 )
            {
                puts("Connection closed..");
                /* sock connection closed at other end */
                break;
            }

            if ( iMesgSize < 0 )
            {
                if ( errno == EINTR)
                {
                    continue;
                }
               ICP_SendPkt(iSocketFd , FILBRWS ,gpcErrorMsg[0] , 
                           &gstSendCmdPkt);
               ICP_SendPkt(iSocketFd , FILBRWS ,gcPrompt , &gstSendCmdPkt);
               eRetValue = NASC_FAILURE;
               break;
            }
        }

        iFirstTime = FALSE;

        ICP_ConvertCmdToHostForm( pstCmdPkt , pstCmdPkt);

        if ( pstCmdPkt->iDataLen == 0 )
        {
            ICP_SendPkt(iSocketFd , FILBRWS ,gcPrompt , &gstSendCmdPkt);
            continue;
        }

        iTotalCmdSize += pstCmdPkt->iDataLen;
   
        if ( pucScratchForCmd == NULL)
        {
            iTotalCmdSize += 1;
            pucScratchForCmd = (char *) malloc (sizeof(char) *
                                                iTotalCmdSize );
            if ( pucScratchForCmd == NULL)
            {
               ICP_SendPkt(iSocketFd , FILBRWS ,gpcErrorMsg[1] , 
                           &gstSendCmdPkt);
               eRetValue = NASC_FAILURE ;
                break;
            }
            memset(pucScratchForCmd , '\0' , iTotalCmdSize);
        }
        else
        {
            pucScratchForCmd = (char *) realloc ( pucScratchForCmd , 
                                                  sizeof(UI8) * iTotalCmdSize);
        }

        strncat(pucScratchForCmd , pstCmdPkt->ucData , pstCmdPkt->iDataLen);
        pucScratchForCmd[iTotalCmdSize - 1] = '\0';
        if ( pstCmdPkt->iLastPkt != TRUE)
        {
            memset(pstCmdPkt , '\0' , sizeof(stCmdPacket));
            continue;
        }

        /* pucScratchForCmd = (char *) realloc ( pucScratchForCmd , 
                                            sizeof(UI8) * (iTotalCmdSize + 1)); */

        iCommandType = pstCmdPkt->iCommandId;



        ICP_ExtractParams(&pcParamList , pucScratchForCmd, &iNoOfParam);

        if ( iNoOfParam <= 0 )
        {
           FREE_PTR(pucScratchForCmd);
           FREE_PTR(pcParamList);
           ICP_SendPkt(iSocketFd , FILBRWS ,gpcErrorMsg[0] , &gstSendCmdPkt);
           ICP_SendPkt(iSocketFd , FILBRWS ,gcPrompt , &gstSendCmdPkt);
           iTotalCmdSize = 0;
           continue;
        } 

        pcCommandToExec = (UI8 *) malloc ( sizeof(UI8) * 
                                         (iTotalCmdSize + MAX_OPT));
        if ( pcCommandToExec == NULL)
        {
            FREE_PTR(pucScratchForCmd);
            FREE_PTR(pcParamList);
            ICP_SendPkt(iSocketFd , FILBRWS ,gpcErrorMsg[0] , &gstSendCmdPkt);
            ICP_SendPkt(iSocketFd , FILBRWS ,gcPrompt , &gstSendCmdPkt);
            iTotalCmdSize = 0;
            continue;
           
        } 

        memset(pcCommandToExec , '\0', (iTotalCmdSize + MAX_OPT) *
                sizeof(UI8));


        pcCurDir = getcwd(NULL , 255);

        retval = chdir(pcParamList[0]);


        if ( retval != 0 )
        {
           FREE_PTR(pucScratchForCmd);
           FREE_PTR(pcParamList);
           FREE_PTR(pcCommandToExec);
           FREE_PTR(pcCurDir);
           ICP_SendPkt(iSocketFd , FILBRWS ,gpcErrorMsg[0] , &gstSendCmdPkt); 
           ICP_SendPkt(iSocketFd , FILBRWS ,gcPrompt , &gstSendCmdPkt);
           iTotalCmdSize = 0;
           continue;
        }

         if (iCommandType != RUN &&  iCommandType !=CRTDIR )
         { 
												if ( iNoOfParam >= 2 ) 
												{
                retval = chdir(pcParamList[1]);
                if ( retval != 0 )
                {
                    FREE_PTR(pucScratchForCmd);
                    FREE_PTR(pcParamList);
                    FREE_PTR(pcCommandToExec);
                    FREE_PTR(pcCurDir);
                    ICP_SendPkt(iSocketFd , FILBRWS ,gpcErrorMsg[0] , 
                                &gstSendCmdPkt); 
                    ICP_SendPkt(iSocketFd , FILBRWS ,gcPrompt , &gstSendCmdPkt);
                    iTotalCmdSize = 0;
                    continue;
                 }

                ICP_FormCmd(&pcParamList[2] , iNoOfParam - 2 , 
																												pcCommandToExec , iCommandType);
            }																													
												else
            {
                    FREE_PTR(pucScratchForCmd);
                    FREE_PTR(pcParamList);
                    FREE_PTR(pcCommandToExec);
                    FREE_PTR(pcCurDir);
                    ICP_SendPkt(iSocketFd , FILBRWS ,gpcErrorMsg[0] , 
                                &gstSendCmdPkt); 
                    ICP_SendPkt(iSocketFd , FILBRWS ,gcPrompt , &gstSendCmdPkt);
                    iTotalCmdSize = 0;
                    continue;
            }
            
         } 
        else
        {

            ICP_FormCmd(&pcParamList[1] , iNoOfParam - 1, pcCommandToExec , 
                        iCommandType);
        } 


                        

        eRetValue = ICP_ExecCommand ( iSocketFd , pcCommandToExec , 
                                      &gstSendCmdPkt , iPipeId);

        if ( eRetValue != NASC_SUCCESS)
        {
           ICP_SendPkt(iSocketFd , FILBRWS ,gpcErrorMsg[0] , &gstSendCmdPkt);
        }
        ICP_SendPkt(iSocketFd , FILBRWS ,gcPrompt , &gstSendCmdPkt);

        chdir(pcCurDir);
        FREE_PTR (pucScratchForCmd);
        FREE_PTR (pcParamList);
        FREE_PTR(pcCommandToExec);
        FREE_PTR(pcCurDir);
        iTotalCmdSize = 0;
    }

    return eRetValue;
}

/* End of filename.c [ Change filename.c ] */

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
 * $Log: NASCenter_ICP_UMGMT_Linux.c,v $
 * Revision 1.2  2004/06/16 13:42:00  sspriya
 * Storing timeofday after read from client to be used in fix to client system blackout problem in ICP.
 *
 * Revision 1.1  2003/12/16 08:56:55  baravind
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
 * $Id: NASCenter_ICP_UMGMT_Linux.c,v 1.2 2004/06/16 13:42:00 sspriya Exp $
 * $Name:  $
 *
 ****************************************************************************/


/********************************* includes *********************************/

#ifndef NASCENTER_ICP_H
#include <NASCenter_ICP.h>
#endif

#include <pwd.h>

#include <grp.h>



/****************************** local macros ********************************
 *
 * (Macros shareable with rest of the modules are present in .h file)
 */

 #define NULL '\0'



/********************* local structure/union definitions ********************/



/********************* prototypes for local functions ***********************/

void ICP_ListUsers( I32 iSocketFd , UI8 * pucScratchForCmd);
void ICP_ListGrp(I32 iSocketFd);



/************************* Globals within this module ***********************/

stCmdPacket  gstSendUMGMTCmdPkt;



/************************* externs  ***********************/
extern UI8 gcPrompt [] ;
extern UI8 gcPktDelimiter ;
extern UI8 gEqCmdDelimiter ;
extern UI8 *gpcErrorMsg[] ;
extern stEquivalentCommand  gpcEquivalentCmd [];
extern struct timeval stLastReadTimeValue;
 
ENASCenterError ICP_ProcessUserMGMTCmd (I32 iSocketFd , stCmdPacket * pstCmdPkt)
{
    I32 iCommandType ;
    UI8 *pcCommandToExec = NULL;
    ENASCenterError eRetValue = NASC_SUCCESS;
    UI8  **pcParamList = NULL;
    I32  iNoOfParam = 0;
    BOOL  iFirstTime = TRUE;
    UI8 *pucScratchForCmd = NULL;
    I32 iTotalCmdSize = 0;
    I32 iMesgSize = 0;
    int iPipeId[2];
    I32 iPileFlag;
    I32 retval;

    ICP_Signal(SIGUSR1 , SIG_IGN);
    ICP_Signal(SIGTRAP , SIG_IGN);

    pipe(iPipeId);

    iPileFlag = fcntl(iPipeId[0] , F_GETFL , 0);

    retval = fcntl(iPipeId[0] , F_SETFL , iPileFlag | O_NONBLOCK);

    if ( retval < 0 ) perror("fcntl failed..");

    dup2(iPipeId[1] , 2);

    close(iPipeId[1]); 
    

    while ( 1 ) 
    {
        if ( iFirstTime != TRUE)
        {
            memset(pstCmdPkt , '\0' , sizeof(stCmdPacket));
            printf("Waiting for sock read in child..");
            iMesgSize  =  read(iSocketFd , pstCmdPkt , sizeof(stCmdPacket));
            gettimeofday(&stLastReadTimeValue , NULL);
            printf("MEssage len %ld\n" , iMesgSize);
            if ( iMesgSize == 0 )
            {
                /* sock connection closed at other end */
                break;
            }

            if ( iMesgSize < 0 )
            {
                if ( errno == EINTR)
                {
                    continue;
                }
               ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] , 
                           &gstSendUMGMTCmdPkt);
               ICP_SendPkt(iSocketFd , USRMGMT ,gcPrompt , &gstSendUMGMTCmdPkt);
               eRetValue = NASC_FAILURE;
               break;
            }
        }

        iFirstTime = FALSE;

        if ( pstCmdPkt->iDataLen == 0 )
        {
            ICP_SendPkt(iSocketFd , USRMGMT ,gcPrompt , &gstSendUMGMTCmdPkt);
            continue;
        }

        iTotalCmdSize += pstCmdPkt->iDataLen;
   
        if ( pucScratchForCmd == NULL)
        {
            iTotalCmdSize += 1;
            pucScratchForCmd = (UI8 *) malloc (sizeof(UI8) *
                                                (iTotalCmdSize) );
            if ( pucScratchForCmd == NULL)
            {
               ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[1] , 
                           &gstSendUMGMTCmdPkt);
                continue;
            }
            memset(pucScratchForCmd , '\0' , sizeof(UI8) * iTotalCmdSize);
        }
        else
        {
            pucScratchForCmd = (UI8 *) realloc ( pucScratchForCmd , 
                                                  sizeof(UI8) * 
                                                  iTotalCmdSize);
        }

        strncat(pucScratchForCmd , pstCmdPkt->ucData , pstCmdPkt->iDataLen);
        if ( pstCmdPkt->iLastPkt != TRUE)
        {
            memset(pstCmdPkt , '\0' , sizeof(stCmdPacket));
            continue;
        }

        /* pucScratchForCmd = (UI8 *) realloc ( pucScratchForCmd ,
                                                  sizeof(UI8) *
                                                  (iTotalCmdSize + 1)); */

        pucScratchForCmd[iTotalCmdSize] = '\0';

        
        iCommandType = pstCmdPkt->iCommandId;

        switch( iCommandType )
        {
            case USERLIST :
                ICP_ListUsers( iSocketFd , pucScratchForCmd);
                break;
            case GRPLIST :
                ICP_ListGrp(iSocketFd);
                break;
            case USERCREATE :
            case USERMODIFY :
            case USERREMOVE :
            case GRPCREATE :
            case GRPMODIFY :
            case GRPREMOVE :
               pcCommandToExec = (UI8 *) malloc ( sizeof(UI8) * 
                                 (strlen(pucScratchForCmd) + 
                                  strlen(gpcEquivalentCmd[iCommandType].
                                  pcEquiCommand) + 1));
               if ( pcCommandToExec == NULL)
               {
                   FREE_PTR(pucScratchForCmd);
                   ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] , 
                               &gstSendUMGMTCmdPkt);
                   ICP_SendPkt(iSocketFd , USRMGMT ,gcPrompt , 
                               &gstSendUMGMTCmdPkt);
                   continue;
           
               } 

                memset(pcCommandToExec , '\0', sizeof(UI8) *
                       (strlen(pucScratchForCmd) + 
                        strlen(gpcEquivalentCmd[iCommandType].pcEquiCommand) 
                       + 1));  

               ICP_ExtractParams(&pcParamList , pucScratchForCmd, &iNoOfParam);

               if ( iNoOfParam <= 0 )
               {
                  FREE_PTR(pucScratchForCmd);
                  FREE_PTR(*pcParamList);
                   pcParamList= NULL;
                  FREE_PTR(pcCommandToExec);
                  ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] , 
                              &gstSendUMGMTCmdPkt);
                  ICP_SendPkt(iSocketFd , USRMGMT ,gcPrompt , 
                              &gstSendUMGMTCmdPkt);
                  continue;
               } 


                ICP_FormCmd(pcParamList , iNoOfParam , pcCommandToExec , 
                            iCommandType);

                eRetValue = ICP_ExecCommand ( iSocketFd , pcCommandToExec , 
                                          &gstSendUMGMTCmdPkt ,iPipeId);

                if ( eRetValue != NASC_SUCCESS)
                {
                    ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] , 
                                &gstSendUMGMTCmdPkt);
                }
        }
        ICP_SendPkt(iSocketFd , USRMGMT ,gcPrompt , &gstSendUMGMTCmdPkt);
        FREE_PTR (pucScratchForCmd);
        FREE_PTR(pcParamList);
        FREE_PTR(pcCommandToExec);
        memset(pstCmdPkt , '\0' , sizeof(stCmdPacket));
        iTotalCmdSize = 0;
    }
    FREE_PTR (pucScratchForCmd);
    FREE_PTR(pcParamList);
    FREE_PTR(pcCommandToExec);
    return eRetValue;
}

void ICP_ListUsers( I32 iSocketFd , UI8 * pucScratchForCmd)
{
     struct passwd *stUserList;
    stCmdPacket  stCommandPakcet;
    I32 iCurrentPktSize = 0;
    I32 iUserNameLen = 0;
    I32 iGroupId = -1;

     memset(&stCommandPakcet , '\0' , sizeof(stCmdPacket));

     if (strcmp(pucScratchForCmd , "*") != 0 )
    {
        iGroupId = atoi(pucScratchForCmd);
    } 
     
    setpwent();
    while ( ( stUserList = getpwent()) != NULL)
    {
        if ( iGroupId != -1 && stUserList->pw_gid != iGroupId)
        {
            continue;
        }

        iUserNameLen = strlen(stUserList->pw_name);
        if ( (iCurrentPktSize + iUserNameLen ) >= MAX_CMD_LEN_PER_PKT )
        {
            stCommandPakcet.iDataLen = iCurrentPktSize;
            write ( iSocketFd , &stCommandPakcet , sizeof(stCmdPacket));
            iCurrentPktSize = 0;
            memset(stCommandPakcet.ucData , '\0' , 
            sizeof(UI8) * MAX_CMD_LEN_PER_PKT);
        }
        strncat(stCommandPakcet.ucData , stUserList->pw_name , 
               iUserNameLen);
        iCurrentPktSize += iUserNameLen;
        stCommandPakcet.ucData[iCurrentPktSize] = '\n';
        iCurrentPktSize += 1;
    }

   stCommandPakcet.iDataLen = iCurrentPktSize;
   write ( iSocketFd , &stCommandPakcet , sizeof(stCmdPacket));
   iCurrentPktSize = 0;
   memset(stCommandPakcet.ucData , '\0' , 
           sizeof(UI8) * MAX_CMD_LEN_PER_PKT);

   endpwent();

}
 

void ICP_ListGrp(I32 iSocketFd)
{
    struct group *stGrpList;
    stCmdPacket  stCommandPakcet;
    I32 iCurrentPktSize = 0;
    I32 iGrpNameLen = 0;

    memset(&stCommandPakcet , '\0' , sizeof(stCmdPacket));

    setgrent();
    while ( ( stGrpList = getgrent()) != NULL)
    {
        iGrpNameLen = strlen(stGrpList->gr_name);

        if ( (iCurrentPktSize + iGrpNameLen ) >= MAX_CMD_LEN_PER_PKT )
        {
            stCommandPakcet.iDataLen = iCurrentPktSize;
            write ( iSocketFd , &stCommandPakcet , sizeof(stCmdPacket));
            iCurrentPktSize = 0;
            memset(stCommandPakcet.ucData , '\0' , 
                   sizeof(UI8) * MAX_CMD_LEN_PER_PKT);
        }
        strncat(stCommandPakcet.ucData , stGrpList->gr_name ,
               iGrpNameLen);
        iCurrentPktSize += iGrpNameLen;
        stCommandPakcet.ucData[iCurrentPktSize] = '\n';
        iCurrentPktSize += 1;
   }
   stCommandPakcet.iDataLen = iCurrentPktSize;
   write ( iSocketFd , &stCommandPakcet , sizeof(stCmdPacket));
   iCurrentPktSize = 0;
   memset(stCommandPakcet.ucData , '\0' , 
           sizeof(UI8) * MAX_CMD_LEN_PER_PKT);
   endgrent();

}

void ICP_CreateUser ( UI8 * pcUserDetails) 
{
    struct passwd stUserDetails;
    UI8*   pcFileName = "/etc/passwd";
    FILE * fp;

    fp = fopen(pcFileName , a);

    memcpy(stUserDetails.pw_name , pcUserDetails[5]);
    memcpy(stUserDetails.pw_passwd , pcUserDetails[1]);
    stUserDetails.pw_uid = pcUserDetails[0];
    stUserDetails.pw_gid = pcUserDetails[2];
    memcpy(stUserDetails.pw_comment , pcUserDetails[3]);
    memcpy(stUserDetails.pw_dir , pcUserDetails[4]);

    stUserDetails.pw_gecos = NULL;
    stUserDetails.pw_shell = NULL;

    putpwent(&stUserDetails , fp);
};





/* End of filename.c [ Change filename.c ] */

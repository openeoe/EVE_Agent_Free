/*****************************************************************************
 *
 * filename.c: NASCenter_ICP_UMGMT
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
 * $Log: NASCenter_ICP_UMGMT.c,v $
 * Revision 1.22  2004/06/16 13:41:52  sspriya
 * Storing timeofday after read from client to be used in fix to client system blackout problem in ICP.
 *
 * Revision 1.21  2004/04/30 12:07:11  sspriya
 * Added code to:
 * 1. Check username length in HPUX.
 * 2. Add description for Usermodify command in User Management module.
 *
 * Revision 1.20  2004/03/30 04:44:05  sspriya
 * Added code to add description in the passwd entry while the useradd command is issued through ICP User Management. Reviewed by VijaiGeetha.
 *
 * Revision 1.19  2004/01/13 05:07:36  baravind
 * Updates after a round of purify test
 *
 * Revision 1.18  2004/01/09 11:01:41  sig
 * Added default shell for user management
 *
 * Revision 1.17  2004/01/09 06:30:00  baravind
 * Fixed warnings
 *
 * Revision 1.16  2004/01/06 10:53:53  baravind
 * Added htonl / ntohl to enable ICP get commands from different OS
 *
 * Revision 1.15  2004/01/06 09:59:29  baravind
 * Added pclose after popen
 *
 * Revision 1.14  2004/01/05 09:49:07  baravind
 * Fixed warnings
 *
 * Revision 1.13  2004/01/05 05:30:21  baravind
 * Fixed Code review comments
 *
 * Revision 1.12  2003/12/31 11:57:04  rajesh
 * Fixed for Tru64 by adding flags
 *
 * Revision 1.11  2003/12/29 12:15:01  sig
 * *** empty log message ***
 *
 * Revision 1.10  2003/12/24 13:25:02  sig
 * Added by Aravind.B . Fix for Linux . Changed from popen -> system
 *
 * Revision 1.9  2003/12/24 08:50:19  baravind
 * Added by Aravind.B . fix for UNIXWARE
 *
 * Revision 1.8  2003/12/22 08:28:25  sig
 * Added by Aravind.B .Compilation error Fix  For LINUX
 *
 * Revision 1.7  2003/12/22 05:18:02  baravind
 * *** empty log message ***
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
 * $Id: NASCenter_ICP_UMGMT.c,v 1.22 2004/06/16 13:41:52 sspriya Exp $
 * $Name:  $
 *
 ****************************************************************************/


/********************************* includes *********************************/

#ifndef NASCENTER_ICP_H
#include <NASCenter_ICP.h>
#endif

#include <pwd.h>

#include <grp.h>

#include <unistd.h>



/****************************** local macros ********************************
 *
 * (Macros shareable with rest of the modules are present in .h file)
 */




/********************* local structure/union definitions ********************/



/********************* prototypes for local functions ***********************/

void ICP_ListUsers( I32 iSocketFd , UI8 * pucScratchForCmd);
void ICP_ListGrp(I32 iSocketFd);

#ifdef AIX 
ENASCenterError ICP_UpdateUser_AIX ( UI8 ** pcParamList , I32 iCommandType  , 
                            				 int *iPipe);
ENASCenterError ICP_CreateUser_AIX ( UI8 ** pcUserDetails);
#else

ENASCenterError ICP_UpdateUser ( UI8 ** pcParamList , I32 iCommandType  , 
                            				 int * iPipe);
ENASCenterError ICP_CreateUser ( UI8 ** pcUserDetails);
#endif



/************************* Globals within this module ***********************/

stCmdPacket  gstSendUMGMTCmdPkt;
UI8 * gpcPasswdCmd = "/usr/lib/scoadmin/account/password.tcl";



/************************* externs  ***********************/
extern UI8 gcPrompt [] ;
extern UI8 gcPktDelimiter ;
extern UI8 gEqCmdDelimiter ;
extern UI8 *gpcErrorMsg[] ;
extern stEquivalentCommand  gpcEquivalentCmd [] ; 
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
    UI8 * pcPasswd = NULL; 
	UI8 *pcUsrDescrAddCommand = NULL;
	UI8 pcUsrModCmd[] = "usermod -c";    

    ICP_Signal(SIGUSR1 , SIG_IGN);
    ICP_Signal(SIGTRAP , SIG_IGN);

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
        ICP_ConvertCmdToHostForm( pstCmdPkt , pstCmdPkt);

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
               eRetValue = NASC_FAILURE;
               break;
            }
            memset(pucScratchForCmd , '\0' , iTotalCmdSize);
        }
        else
        {
            pucScratchForCmd = (UI8 *) realloc ( pucScratchForCmd , 
                                                  sizeof(UI8) * 
                                                  iTotalCmdSize);
        }

        strncat(pucScratchForCmd , pstCmdPkt->ucData , pstCmdPkt->iDataLen);
        pucScratchForCmd[iTotalCmdSize - 1] = '\0';
        if ( pstCmdPkt->iLastPkt != TRUE)
        {
            memset(pstCmdPkt , '\0' , sizeof(stCmdPacket));
            continue;
        }

        /* pucScratchForCmd = (UI8 *) realloc ( pucScratchForCmd ,
                                                  sizeof(UI8) *
                                                  (iTotalCmdSize + 1)); */


        
        iCommandType = pstCmdPkt->iCommandId;

        ICP_ExtractParams(&pcParamList , pucScratchForCmd, &iNoOfParam);


        if ( iNoOfParam <= 0 )
        {
            FREE_PTR(pucScratchForCmd);
            FREE_PTR(pcParamList);
            pcParamList= NULL;
            ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] , 
                        &gstSendUMGMTCmdPkt);
            ICP_SendPkt(iSocketFd , USRMGMT ,gcPrompt , 
                        &gstSendUMGMTCmdPkt);
            iTotalCmdSize = 0;
            continue;
        } 

	if ( iNoOfParam != gpcEquivalentCmd[iCommandType].iNoOfParmNeeded )
	{
            FREE_PTR(pucScratchForCmd);
            FREE_PTR(pcParamList);
	    pcParamList = NULL;
            ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] , 
                        &gstSendUMGMTCmdPkt);
            ICP_SendPkt(iSocketFd , USRMGMT ,gcPrompt , 
                        &gstSendUMGMTCmdPkt);
            iTotalCmdSize = 0;
            continue;
        }

        switch( iCommandType )
        {
            case USERLIST :
                ICP_ListUsers( iSocketFd , pucScratchForCmd);
                break;
            case GRPLIST :
                ICP_ListGrp(iSocketFd);
                break;
            #ifndef UNIXWARE
            case USERCREATE :
              #ifdef AIX
               if ( strlen(pcParamList[5])  > 8 )
               {
                   ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] ,
                               &gstSendUMGMTCmdPkt);
                   break;
               }
               eRetValue = ICP_CreateUser_AIX(pcParamList);
               if ( eRetValue == NASC_FAILURE )
               {
                    ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] , 
                                &gstSendUMGMTCmdPkt);
                    break;
               }

				/* To add the description */
				pcUsrDescrAddCommand = (UI8 *) malloc( sizeof(UI8) * (strlen(pcUsrModCmd) + strlen(pcParamList[3])
					+ strlen(pcParamList[5])));
				if (pcUsrDescrAddCommand == NULL)
				{
					ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] ,
						&gstSendUMGMTCmdPkt);
					break;
				}
				memset(pcUsrDescrAddCommand, '\0', sizeof(UI8) * (strlen(pcUsrModCmd) + strlen(pcParamList[3])
					+ strlen(pcParamList[5])));
				memcpy(pcUsrDescrAddCommand, pcUsrModCmd, strlen(pcUsrModCmd));
				strcat(pcUsrDescrAddCommand, " \"");
				strcat(pcUsrDescrAddCommand, pcParamList[3]);
				strcat(pcUsrDescrAddCommand, "\" ");
				strcat(pcUsrDescrAddCommand, pcParamList[5]);
				ICP_ExecCommand ( iSocketFd, pcUsrDescrAddCommand,
					&gstSendUMGMTCmdPkt ,iPipeId);
				free(pcUsrDescrAddCommand);
             #else
                #ifdef HPUNIX
                if (strlen(pcParamList[5]) > 8 )
                {
                    ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] , 
                                &gstSendUMGMTCmdPkt);
                    break;
                }
                #endif /* Username Length check for HPUX only */

                eRetValue = ICP_CreateUser ( pcParamList );
                if ( eRetValue == NASC_FAILURE )
                {
                    ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] , 
                                &gstSendUMGMTCmdPkt);
                    break;
                }
          
                ICP_ExecCommand ( iSocketFd , "pwconv", 
                                  &gstSendUMGMTCmdPkt ,iPipeId);

				/* To add the description */
				pcUsrDescrAddCommand = (UI8 *) malloc( sizeof(UI8) * (strlen(pcUsrModCmd) + strlen(pcParamList[3])
					+ strlen(pcParamList[5])));
				if (pcUsrDescrAddCommand == NULL)
				{
					ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] ,
						&gstSendUMGMTCmdPkt);
					break;
				}
				memset(pcUsrDescrAddCommand, '\0', sizeof(UI8) * (strlen(pcUsrModCmd) + strlen(pcParamList[3])
					+ strlen(pcParamList[5])));
				memcpy(pcUsrDescrAddCommand, pcUsrModCmd, strlen(pcUsrModCmd));
				strcat(pcUsrDescrAddCommand, " \"");
				strcat(pcUsrDescrAddCommand, pcParamList[3]);
				strcat(pcUsrDescrAddCommand, "\" ");
				strcat(pcUsrDescrAddCommand, pcParamList[5]);
				ICP_ExecCommand ( iSocketFd, pcUsrDescrAddCommand,
					&gstSendUMGMTCmdPkt ,iPipeId);
				free(pcUsrDescrAddCommand);
              #endif
                break;

            case USERMODIFY :
              #ifdef AIX
                eRetValue = ICP_UpdateUser_AIX ( pcParamList , 
                                                 iCommandType , iPipeId);
                if ( eRetValue == NASC_FAILURE)
                {
                    ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] ,
                                &gstSendUMGMTCmdPkt);
                      break;
                }

                /* To add the description */
				pcUsrDescrAddCommand = (UI8 *) malloc( sizeof(UI8) * (strlen(pcUsrModCmd) + strlen(pcParamList[3])
					+ strlen(pcParamList[4])));
				if (pcUsrDescrAddCommand == NULL)
				{
					ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] ,
						&gstSendUMGMTCmdPkt);
					break;
				}
				memset(pcUsrDescrAddCommand, '\0', sizeof(UI8) * (strlen(pcUsrModCmd) + strlen(pcParamList[3])
					+ strlen(pcParamList[4])));
				memcpy(pcUsrDescrAddCommand, pcUsrModCmd, strlen(pcUsrModCmd));
				strcat(pcUsrDescrAddCommand, " \"");
				strcat(pcUsrDescrAddCommand, pcParamList[3]);
				strcat(pcUsrDescrAddCommand, "\" ");
				strcat(pcUsrDescrAddCommand, pcParamList[4]);
				ICP_ExecCommand ( iSocketFd, pcUsrDescrAddCommand,
					&gstSendUMGMTCmdPkt ,iPipeId);
				free(pcUsrDescrAddCommand);
              #else 	
                 eRetValue = ICP_UpdateUser ( pcParamList , 
                                              iCommandType  ,iPipeId);
                 if ( eRetValue == NASC_FAILURE)
                 {
                   ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] ,
                               &gstSendUMGMTCmdPkt);
                      break;
                 }

                ICP_ExecCommand ( iSocketFd , "pwconv", 
                                     &gstSendUMGMTCmdPkt ,iPipeId);

                /* To add the description */
                pcUsrDescrAddCommand = (UI8 *) malloc( sizeof(UI8) * (strlen(pcUsrModCmd) + strlen(pcParamList[3])
					+ strlen(pcParamList[4])));                
				if (pcUsrDescrAddCommand == NULL)
				{
					ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] ,
						&gstSendUMGMTCmdPkt);
					break;
				}
                memset(pcUsrDescrAddCommand, '\0', sizeof(UI8) * (strlen(pcUsrModCmd) + strlen(pcParamList[3])
					+ strlen(pcParamList[4])));
				memcpy(pcUsrDescrAddCommand, pcUsrModCmd, strlen(pcUsrModCmd));
				strcat(pcUsrDescrAddCommand, " \"");
				strcat(pcUsrDescrAddCommand, pcParamList[3]);
				strcat(pcUsrDescrAddCommand, "\" ");
				strcat(pcUsrDescrAddCommand, pcParamList[4]);                                
				ICP_ExecCommand ( iSocketFd, pcUsrDescrAddCommand,
					&gstSendUMGMTCmdPkt ,iPipeId);
				free(pcUsrDescrAddCommand);
               #endif
                 break;
            #else
            case USERCREATE :
            case USERMODIFY : 
                pcPasswd = pcParamList[1];
                pcParamList[1] = NULL;
            #endif
            case USERREMOVE :
            case GRPCREATE :
            case GRPMODIFY :
            case GRPREMOVE :
               pcCommandToExec = (UI8 *) malloc ( sizeof(UI8) * 
                                 (iTotalCmdSize + 
                                  strlen(gpcEquivalentCmd[iCommandType].
                                  pcEquiCommand) + 1));
               if ( pcCommandToExec == NULL)
               {
                   FREE_PTR(pucScratchForCmd);
                   FREE_PTR(pcParamList);
                   ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] , 
                               &gstSendUMGMTCmdPkt);
                   ICP_SendPkt(iSocketFd , USRMGMT ,gcPrompt , 
                               &gstSendUMGMTCmdPkt);
                   iTotalCmdSize = 0;
                   continue;
           
               } 

                memset(pcCommandToExec , '\0', sizeof(UI8) *
                       (iTotalCmdSize + 
                        strlen(gpcEquivalentCmd[iCommandType].pcEquiCommand) 
                       + 1));  


                ICP_FormCmd(pcParamList , iNoOfParam , pcCommandToExec , 
                            iCommandType);

                 printf("Command : %s\n" , pcCommandToExec);

                eRetValue = ICP_ExecCommand ( iSocketFd , pcCommandToExec , 
                                          &gstSendUMGMTCmdPkt ,iPipeId);
                  
                 printf("Command Executed\n");

                if ( eRetValue != NASC_SUCCESS)
                {
                    ICP_SendPkt(iSocketFd , USRMGMT ,gpcErrorMsg[0] , 
                                &gstSendUMGMTCmdPkt);
                }
                else
                {
                    if ( iCommandType == USERCREATE || 
                         iCommandType == USERMODIFY )
                    {
                        FREE_PTR(pcCommandToExec);
                        pcCommandToExec = (UI8 *) malloc ( 
                                                       strlen(gpcPasswdCmd) +
                                                       strlen(pcPasswd) +
                                                       strlen(pcParamList[5]) +
                                                        50);
                        memset ( pcCommandToExec , '\0' , strlen(gpcPasswdCmd) +
                             strlen(pcPasswd) + strlen(pcParamList[5])
                             + 50);

                        sprintf(pcCommandToExec , 
                                "echo %s | %s %s > /dev/null 2>&1" ,
                                pcPasswd , gpcPasswdCmd , pcParamList[5]);
                        printf("Passwd cmd %s\n" , pcCommandToExec);
                        
                        eRetValue = ICP_ExecCommand ( iSocketFd , 
                                                      pcCommandToExec ,
                                                      &gstSendUMGMTCmdPkt ,
                                                      iPipeId);
                     }
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
    printf("End of func UTIL\n");
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
            stCommandPakcet.iDataLen = htonl(iCurrentPktSize);
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

   stCommandPakcet.iDataLen = htonl(iCurrentPktSize);
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
            stCommandPakcet.iDataLen = htonl(iCurrentPktSize);
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
   stCommandPakcet.iDataLen = htonl(iCurrentPktSize);
   write ( iSocketFd , &stCommandPakcet , sizeof(stCmdPacket));
   iCurrentPktSize = 0;
   memset(stCommandPakcet.ucData , '\0' , 
           sizeof(UI8) * MAX_CMD_LEN_PER_PKT);
   endgrent();

}

#ifdef AIX

ENASCenterError ICP_CreateUser_AIX ( UI8 ** pcUserDetails) 
{
    struct passwd stUserDetails;
    struct passwd *pstPwDetails = NULL;
				struct userpw stUsershadowDetails;
    I8 *   pcFileName = "/etc/passwd";
    FILE * fp;
    char * pcMessage = NULL;

    pstPwDetails =  getpwnam (  pcUserDetails[5]);
    if ( pstPwDetails != NULL)
    {
         return NASC_FAILURE;
    }
    pstPwDetails = NULL;

    pstPwDetails =  getpwuid ( atoi(pcUserDetails[0]) );
    if ( pstPwDetails != NULL)
    {
         return NASC_FAILURE;
    }

    fp = fopen(pcFileName , "a");
    if ( fp == NULL)
    {
        return NASC_FAILURE;
    }

    stUserDetails.pw_name = pcUserDetails[5];
    stUserDetails.pw_passwd = crypt(pcUserDetails[1] , "a0");
    stUserDetails.pw_uid = atoi(pcUserDetails[0]);
    stUserDetails.pw_gid = atoi(pcUserDetails[2]);
    stUserDetails.pw_dir = pcUserDetails[4];
    stUserDetails.pw_gecos = "";
    stUserDetails.pw_shell = USER_SHELL;
    
    /* create home dir for the user */

    mkdir(pcUserDetails[4] , 
          S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    putpwent(&stUserDetails , fp);

    fclose(fp); 

    strcpy(stUsershadowDetails.upw_name , pcUserDetails[5]);
    stUsershadowDetails.upw_passwd = crypt(pcUserDetails[1] , "a0");
    stUsershadowDetails.upw_flags = PW_NOCHECK;

    setpwdb(S_READ | S_WRITE);

    putuserpwhist (&stUsershadowDetails , &pcMessage);
    return NASC_SUCCESS;
}

ENASCenterError ICP_UpdateUser_AIX ( UI8 ** pcParamList , I32 iCommandType  ,
                                     int *iPipeId)
{
    struct passwd *pstUserDetails = NULL;
    struct userpw stUsershadowDetails;
    char *   pcFileName = "/etc/passwd";
    UI8 * pcCommandToExec = NULL;
    FILE * fp;
    char * pcMessage = NULL; 

    pstUserDetails =  getpwnam (  pcParamList[4]);
    if ( pstUserDetails == NULL)
    {
        return NASC_FAILURE;
    }



    if ( getpwuid (atoi(pcParamList[0])) != NULL )
    {
        return NASC_FAILURE;
    }
    pcCommandToExec = (UI8 *) malloc ( sizeof(UI8) * 
                                 (strlen(pcParamList[4]) + 
                                  strlen(gpcEquivalentCmd[iCommandType].
                                  pcEquiCommand) + 1));

    memset(pcCommandToExec , '\0' ,  sizeof(UI8) * (strlen(pcParamList[4]) + 
                                     strlen(gpcEquivalentCmd[iCommandType].
                                     pcEquiCommand) + 1));                                     

    ICP_FormCmd(&pcParamList[4] , 1 , pcCommandToExec , USERREMOVE);

    ICP_ExecCommand ( -1 , pcCommandToExec, &gstSendUMGMTCmdPkt ,iPipeId);

    fp = fopen(pcFileName , "a");
    if ( fp == NULL)
    {
        return NASC_FAILURE;
    }
    pstUserDetails->pw_name = pcParamList[4];
    pstUserDetails->pw_passwd =  crypt(pcParamList[1] , "a0");
    pstUserDetails->pw_uid = atoi(pcParamList[0]);
    pstUserDetails->pw_gid = atoi(pcParamList[2]);
    pstUserDetails->pw_gecos = "";
    pstUserDetails->pw_shell = USER_SHELL;

    mkdir(pstUserDetails->pw_dir , 
          S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    
    chown ( pstUserDetails->pw_dir , pstUserDetails->pw_uid , 
            pstUserDetails->pw_gid);


    putpwent(pstUserDetails , fp);
    fclose(fp);

    strcpy(stUsershadowDetails.upw_name , pcParamList[4]);
    stUsershadowDetails.upw_passwd = crypt(pcParamList[1] , "a0");
    stUsershadowDetails.upw_flags = PW_NOCHECK;

    setpwdb(S_READ | S_WRITE);

    putuserpwhist (&stUsershadowDetails , &pcMessage);

    FREE_PTR(pcCommandToExec);

    return NASC_SUCCESS;
}
#elif !defined (UNIXWARE)

ENASCenterError ICP_CreateUser ( UI8 ** pcUserDetails) 
{
    struct passwd stUserDetails;
    struct passwd *pstPwDetails = NULL;
    char *   pcFileName = "/etc/passwd";
    FILE * fp = NULL;

    pstPwDetails =  getpwnam (  pcUserDetails[5]);
    if ( pstPwDetails != NULL)
    {
         return NASC_FAILURE;
    }
    pstPwDetails = NULL;

    pstPwDetails =  getpwuid ( atoi(pcUserDetails[0]));
    if ( pstPwDetails != NULL)
    {
         return NASC_FAILURE;
    }

    fp = fopen(pcFileName , "a");

    if ( fp == NULL)
    {
        return NASC_FAILURE;
    }

    stUserDetails.pw_name = pcUserDetails[5];
    stUserDetails.pw_passwd = crypt(pcUserDetails[1] , "a0");
    stUserDetails.pw_uid = atoi(pcUserDetails[0]);
    stUserDetails.pw_gid = atoi(pcUserDetails[2]);
    #if !(defined LINUX) && !(defined TRUE64)
    stUserDetails.pw_comment = pcUserDetails[3];
    stUserDetails.pw_age = "";
    #endif
    stUserDetails.pw_dir = pcUserDetails[4];
    stUserDetails.pw_gecos = "";
    stUserDetails.pw_shell = USER_SHELL;
    
    /* create home dir for the user */

    mkdir(pcUserDetails[4] , 
          S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

    chown ( stUserDetails.pw_dir , stUserDetails.pw_uid  ,
            stUserDetails.pw_gid);

    putpwent(&stUserDetails , fp);

    fclose(fp); 
    return NASC_SUCCESS;
}


ENASCenterError ICP_UpdateUser ( UI8 ** pcParamList , I32 iCommandType  ,
                                 int *iPipeId)
                                 
{
    struct passwd *pstUserDetails = NULL;
    char *   pcFileName = "/etc/passwd";
    UI8 * pcCommandToExec = NULL;

    FILE * fp = NULL;

    pstUserDetails =  getpwnam (  pcParamList[4]);
    if ( pstUserDetails == NULL)
    {
        return NASC_FAILURE;
    }



    if ( getpwuid (atoi(pcParamList[0])) != NULL )
    {
        return NASC_FAILURE;
    }
    pcCommandToExec = (UI8 *) malloc ( sizeof(UI8) * 
                                 (strlen(pcParamList[4]) + 
                                  strlen(gpcEquivalentCmd[iCommandType].
                                  pcEquiCommand) + 1));

    memset(pcCommandToExec , '\0' ,  sizeof(UI8) * (strlen(pcParamList[4]) + 
                                     strlen(gpcEquivalentCmd[iCommandType].
                                     pcEquiCommand) + 1));                                     
    ICP_FormCmd(&pcParamList[4] , 1 , pcCommandToExec , USERREMOVE);

    ICP_ExecCommand ( -1 , pcCommandToExec, &gstSendUMGMTCmdPkt ,iPipeId);

    fp = fopen(pcFileName , "a");
    if ( fp == NULL)
    {
        return NASC_FAILURE;
    }
    pstUserDetails->pw_name = pcParamList[4];
    pstUserDetails->pw_passwd =  crypt(pcParamList[1] , "a0");
    pstUserDetails->pw_uid = atoi(pcParamList[0]);
    pstUserDetails->pw_gid = atoi(pcParamList[2]);
    #if !(defined LINUX) && !(defined TRUE64)
    pstUserDetails->pw_comment = pcParamList[3];
    pstUserDetails->pw_age = "";
    #endif
    pstUserDetails->pw_gecos = "";
    pstUserDetails->pw_shell = USER_SHELL;

    mkdir(pstUserDetails->pw_dir , 
          S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);


    chown ( pstUserDetails->pw_dir , pstUserDetails->pw_uid , 
            pstUserDetails->pw_gid);

    putpwent(pstUserDetails , fp);
    fclose(fp);
    FREE_PTR(pcCommandToExec);
    return NASC_SUCCESS;
}
#endif

/* End of filename.c [ Change filename.c ] */

/*****************************************************************************
 *
 * filename.c: NASCenter_ICP_Utils
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
 * $Author: baravind $
 * $Log: NASCenter_ICP_Utils.c,v $
 * Revision 1.13  2004/01/06 10:53:53  baravind
 * Added htonl / ntohl to enable ICP get commands from different OS
 *
 * Revision 1.12  2004/01/06 09:59:29  baravind
 * Added pclose after popen
 *
 * Revision 1.11  2004/01/05 05:30:21  baravind
 * Fixed Code review comments
 *
 * Revision 1.10  2003/12/29 12:15:01  sig
 * *** empty log message ***
 *
 * Revision 1.9  2003/12/24 13:25:02  sig
 * Added by Aravind.B . Fix for Linux . Changed from popen -> system
 *
 * Revision 1.8  2003/12/24 08:50:19  baravind
 * Added by Aravind.B . fix for UNIXWARE
 *
 * Revision 1.7  2003/12/22 05:09:49  baravind
 * Added by Aravind.B . Fixed realloc problm in UNIXWARE .
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
 * $Id: NASCenter_ICP_Utils.c,v 1.13 2004/01/06 10:53:53 baravind Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
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

FILE *gpFilePointer = NULL;
stIcpTableCtx *gstIcpTableCtx = NULL;
stIcpList     gstIcpList;


/************************* externs  ***********************/

extern UI8 gEqCmdDelimiter[];
extern stEquivalentCommand gpcEquivalentCmd[];
extern UI8 gcPktDelimiter[];
extern UI8 gcPrompt[];
extern UI8 gcErrorFile[];


/*****************************************************************************
 *
 * name             :   ICP_ExecCommand
 * description      :   This function executes the given command 
 * input parameters :   I32 iSocketFd  TCP socket id 
 *                      UI8 * pucCommand  Command to be executed
 *                      stCmdPacket * pstSendCmdPkt  command packet to be
 *                                                   sent to manager
 *                      int *piPipeId    pipe id for getting errors on 
 *                                       command execution
 * output parameters:   none
 *                      
 * return type      :   ENASCenterError
 * global variables :   None 
 * calls            :   None 
 *                      
 ****************************************************************************/
ENASCenterError  ICP_ExecCommand ( I32 iSocketFd , UI8 * pucCommand , 
                                   stCmdPacket * pstSendCmdPkt , int *piPipeId)
{
    UI8 cCommandOutput [ MAX_OUTP_SIZE + 1 ]  ;
    I32 iNoOfCharInOutput = 0;
    I32 iMsgSize;


    memset(cCommandOutput, '\0' , MAX_OUTP_SIZE + 1);

    if ( pucCommand == NULL )
    {
       return NASC_FAILURE;
    }

     /* execute the command using popen call */

     gpFilePointer = popen(pucCommand , "r") ;

    if ( gpFilePointer == NULL ) 
    {
        return NASC_FAILURE;
    } 

    sleep(3);

    pstSendCmdPkt->iLastPkt = htonl(0);

    if ( iSocketFd != -1 )
    {
        /* check for errors if any */ 
        while ((iMsgSize = read(piPipeId[0] , cCommandOutput , 
                                MAX_OUTP_SIZE)) != 0) 
        {
            if ( iMsgSize > 0)
            {
                cCommandOutput[iMsgSize] = 0;
                strcpy(pstSendCmdPkt->ucData , (UI8 *) cCommandOutput);
                pstSendCmdPkt->iDataLen = htonl(iMsgSize);
                write(iSocketFd , pstSendCmdPkt, sizeof(struct stCmdPacket));
                memset(cCommandOutput , '\0' , MAX_OUTP_SIZE);
            }
            if ( iMsgSize == -1)
            {
                break;
            }
        }

         /* send command output to manager */

         while(fgets(cCommandOutput , MAX_OUTP_SIZE , gpFilePointer) != '\0')
         {
            iNoOfCharInOutput = strlen(cCommandOutput);
            strcpy(pstSendCmdPkt->ucData , (UI8 *) cCommandOutput);
            pstSendCmdPkt->iDataLen = htonl(iNoOfCharInOutput);
            write(iSocketFd , pstSendCmdPkt, sizeof(struct stCmdPacket));
            memset(cCommandOutput , '\0' , MAX_OUTP_SIZE);
        } 
    }

    pclose(gpFilePointer);
    gpFilePointer = NULL;

    return NASC_SUCCESS;
}
/*****************************************************************************
 *
 * name             :   ICP_ExtractParams
 * description      :   This function parses command packet sent by the
 *                      manager and extracts  command parameters in the packet 
 *
 * input parameters :   UI8 *pcParamList Parameter list
 *                      I32 *iNoOfParam  No Of parameters 
 *
 * output parameters:   UI8 *** pppcTempString  Parameter List 
 *                      
 * return type      :   ENASCenterError
 * global variables :   None 
 * calls            :   None
 ****************************************************************************/

ENASCenterError ICP_ExtractParams(UI8 *** pppcTempString , UI8 *pcParamList , 
                  I32 *iNoOfParam)
{
     UI8 * pcParam = NULL;    
     UI8 * pcTempString = NULL;    
     I32   iNoOfParms = 0;
     ENASCenterError eStatus = NASC_SUCCESS;
     UI8 * pcPtrToComStr = NULL;

     *pppcTempString = (UI8 **) malloc ( sizeof(UI8 *) * MAX_PARAMS);

     if ( *pppcTempString == NULL )
     {
         return NASC_INSUFFICIENT_MEMORY;
     }

    pcPtrToComStr = pcParamList;

    pcParam = strtok_r(pcPtrToComStr , gcPktDelimiter ,
                       (char **)&pcTempString);

    (*pppcTempString)[0] = pcParam;

    pcParam = NULL; 

    iNoOfParms++;

    while ( (pcParam = strtok_r(NULL , gcPktDelimiter , 
                                      (char **)&pcTempString)) != NULL)
    {
        (*pppcTempString)[iNoOfParms] = pcParam;
        pcParam = NULL;
        iNoOfParms++;
    }

    
    *iNoOfParam = iNoOfParms;
    return eStatus;
}
/*****************************************************************************
 *
 * name             :   ICP_FormCmd
 * description      :   This function forms command to be executed
 * input parameters :   UI8 **ppcParamList Parameter list
 *                      I32 iNoOfParam  No Of Parameters
 *                      I32 iCommandType  Command type
 *
 * output parameters:   UI8 *pcCommandToExec  Command to be executed
 *                      
 * return type      :   void
 * global variables :   None
 * calls            :   None
 ****************************************************************************/

void ICP_FormCmd(UI8 **ppcParamList , I32 iNoOfParam , 
            UI8 *pcCommandToExec , I32 iCommandType)
{
    I32 iLoopVar = 0;
    UI8 *pcString = NULL;
    char *pcRemainingPart = NULL;
    UI8 * pcComStr = NULL;
    UI8 *pcPtrToComStr = NULL;

    printf("Inside form command \n");

    if ( gpcEquivalentCmd[iCommandType].iNoOfParmNeeded != iNoOfParam)
    {
        return;
    }

    /* copy equivalent command */

    if ( iNoOfParam == 0 )
    {
        memcpy(pcCommandToExec ,gpcEquivalentCmd[iCommandType].pcEquiCommand , 
	       sizeof(UI8) * 
	       strlen(gpcEquivalentCmd[iCommandType].pcEquiCommand));
        return;
    }

    pcComStr = (UI8 *) malloc ( sizeof(UI8) * 
                                (strlen(gpcEquivalentCmd[iCommandType].
                                        pcEquiCommand) + 1));

    if ( pcComStr == NULL)
    {
        return ;
    }

    memset(pcComStr , '\0' , sizeof(UI8) * 
           (strlen(gpcEquivalentCmd[iCommandType].pcEquiCommand) + 1));

    memcpy(pcComStr ,gpcEquivalentCmd[iCommandType].pcEquiCommand , 
           sizeof(UI8) * strlen(gpcEquivalentCmd[iCommandType].pcEquiCommand));

    
    
    pcPtrToComStr = pcComStr;

    pcString = strtok_r(pcPtrToComStr,
                            gEqCmdDelimiter , &pcRemainingPart);

    strcat(pcCommandToExec , pcString);


    strcat(pcCommandToExec , ppcParamList[0]);
           
    iLoopVar ++;

    while ( (pcString = strtok_r(NULL , gEqCmdDelimiter , 
                                     &pcRemainingPart)) != NULL)
    {
        if (ppcParamList[iLoopVar] !=  NULL)
        {
            strcat(pcCommandToExec , pcString);
            strcat(pcCommandToExec , ppcParamList[iLoopVar]);
        }
        pcString = NULL;  
        iLoopVar++;
    } 
    FREE_PTR(pcComStr);
} 
/*****************************************************************************
 *
 * name             :   ICP_SendPkt
 * description      :   This function Sends command packet to manager
 * input parameters :   I32 iSocketFd  TCP socket id 
 *                      I32 iModuleId   Module Id
 *                      void * pvData   Data to be sent 
 *                      stCmdPacket * pstCmdPkt  Command pkt  
 *
 * output parameters:   none
 *                      
 * return type      :   void
 * global variables :   none
 * calls            :   none
 ****************************************************************************/

void ICP_SendPkt(I32 iSocketFd  ,I32 iModuleId , void * pvData , 
                 stCmdPacket *pstSendCmdPkt)
{

    memset(pstSendCmdPkt , '\0' , sizeof(stCmdPacket));
    
     pstSendCmdPkt->iModuleId =  htonl(iModuleId);
     strcpy(pstSendCmdPkt->ucData , (UI8 *) pvData);
     pstSendCmdPkt->iDataLen = htonl(strlen( (UI8 *) pvData ));
     pstSendCmdPkt->iLastPkt = 0;
     if ( strcmp(gcPrompt , (UI8 *) pvData) == 0 )
     {
         pstSendCmdPkt->iLastPkt = htonl(1);
     }
     write(iSocketFd , pstSendCmdPkt , sizeof(stCmdPacket));

     memset(pstSendCmdPkt , '\0' , sizeof(stCmdPacket));
}

/*****************************************************************************
 *
 * name             :   ICP_InitICPList
 * description      :   This function Initialised ICP list of 
 *                      disallowed command 
 * input parameters :   stIcpList *gstIcpList  List
 *
 * output parameters:   none
 *                      
 * return type      :   ENASCenterError
 * global variables :   stIcpList  List 
 * calls            :   none
 ****************************************************************************/

ENASCenterError ICP_InitICPList (stIcpList *gstIcpList)
{
    gstIcpList->stHeader = (stIcpTableCtx *) malloc ( sizeof(stIcpTableCtx));

    if ( gstIcpList->stHeader == NULL)
    {
        return NASC_INSUFFICIENT_MEMORY;
    }

    memset(gstIcpList->stHeader , '\0' , sizeof(stIcpTableCtx));

    pthread_mutex_init(&gstIcpList->icpListmutex , NULL);
    return NASC_SUCCESS;
}

/*****************************************************************************
 *
 * name             :   ICP_CreateICPNode
 * description      :   This function Creates a linklist node for ICP 
 *                      disallowed command
 * input parameters :   UI8 * pcCmdName  command name
 *                      I16 iCmdLen  command name length
 *
 * output parameters:   none
 *                      
 * return type      :   stIcpTableCtx node
 * global variables :   none
 * calls            :   none
 ****************************************************************************/
stIcpTableCtx * ICP_CreateICPNode ( UI8 * pcCmdName , I16 iCmdLen)
{
    stIcpTableCtx *stIcpCntx = NULL;


    stIcpCntx = (stIcpTableCtx *) malloc ( sizeof(stIcpTableCtx));


    if ( stIcpCntx != NULL)
    {
        memset(stIcpCntx , '\0' , sizeof(stIcpTableCtx));
        stIcpCntx->stNext =   NULL;

        if ( pcCmdName != NULL)
        {
            memcpy(stIcpCntx->cIcpCommand , pcCmdName , iCmdLen);
            stIcpCntx->lIcpCommandLen = iCmdLen;
        }
    }
  
    return stIcpCntx;
}

/*****************************************************************************
 *
 * name             :   ICP_AddToCntx
 * description      :   This function Adds none to ICP list context
 * input parameters :   stIcpList *stIcpList ICP list
 *                      stIcpTableCtx *stIcpCntx context
 *
 * output parameters:   none
 *                      
 * return type      :   void
 * global variables :   stIcpList  List
 * calls            :   none
 ****************************************************************************/

void ICP_AddToCntx ( stIcpList *stIcpList , stIcpTableCtx *stIcpCntx)
{

    pthread_mutex_lock(&stIcpList->icpListmutex);

    if ( stIcpList->stHeader->stNext == NULL)
    {
        stIcpList->stHeader->stNext = stIcpCntx;
    }
    else
    {
        stIcpCntx->stNext = stIcpList->stHeader->stNext;
        stIcpList->stHeader->stNext = stIcpCntx;
    }
    pthread_mutex_unlock(&stIcpList->icpListmutex);
}

/*****************************************************************************
 *
 * name             :   ICP_DelFromCtx
 * description      :   This function deletes node from ICP list
 * input parameters :   stIcpList *gstIcpList List
 *                      UI8 * pcCmdName  Command to be deleted from list  
 *                      I16 iCmdLen Command name length
 *
 * output parameters:   none
 *                      
 * return type      :   void
 * global variables :   stIcpList  List 
 * calls            :   none
 ****************************************************************************/

void ICP_DelFromCtx ( stIcpList *gstIcpList , UI8 * pcCmdName , I16 iCmdLen)
{
    stIcpTableCtx *stTmpIcpCntx = NULL;
    stIcpTableCtx *stPrevIcpCntx = NULL;

    pthread_mutex_lock(&gstIcpList->icpListmutex);

    if ( gstIcpList->stHeader->stNext != NULL)
    {
        stTmpIcpCntx = gstIcpList->stHeader->stNext;
        stPrevIcpCntx = gstIcpList->stHeader;

        while ( stTmpIcpCntx != NULL)
        {
            if ( strncmp(pcCmdName , stTmpIcpCntx->cIcpCommand , iCmdLen) == 0)
            {
                     
                    stPrevIcpCntx->stNext = stTmpIcpCntx->stNext;
                    stTmpIcpCntx->stNext = NULL;
                    FREE_PTR(stTmpIcpCntx);
                    break;
            }
            stTmpIcpCntx = stTmpIcpCntx->stNext; 
       }
    }
    pthread_mutex_unlock(&gstIcpList->icpListmutex);
}
/*****************************************************************************
 *
 * name             :   ICP_DelAllFromCtx
 * description      :   This function deletes ICP list context
 * input parameters :   stIcpList *gstIcpList List
 *
 * output parameters:   none
 *                      
 * return type      :   void
 * global variables :   stIcpList ICP list
 * calls            :   none
 ****************************************************************************/

void ICP_DelAllFromCtx(stIcpList *gstIcpList)
{
    stIcpTableCtx *stTmpIcpCntx = NULL;

    while ( gstIcpList->stHeader->stNext != NULL)
    {
        stTmpIcpCntx = gstIcpList->stHeader->stNext;

        gstIcpList->stHeader->stNext = stTmpIcpCntx->stNext;
   
        FREE_PTR(stTmpIcpCntx);
    }

   FREE_PTR(gstIcpList->stHeader);
 
}
/*****************************************************************************
 *
 * name             :   ICP_CheckCmdInList
 * description      :   This function checks if the command is in disallowed
 *                      command list
 *
 * input parameters :   stIcpList *stIcpList ICP list
 *                      UI8 * pcCmdName Command name
 *                      I16 iCmdLen Command name length
 *
 * output parameters:   none
 *                      
 * return type      :   stIcpTableCtx * ICP node if present or NULL otherwise
 *
 * global variables :   stIcpList ICP List
 * calls            :   none
 ****************************************************************************/

stIcpTableCtx * ICP_CheckCmdInList ( stIcpList *stIcpList , 
                                     UI8 * pcCmdName , I16 iCmdLen)
{
    stIcpTableCtx *pstTmpIcpCntx = NULL;
    stIcpTableCtx *pstPrevIcpCntx = NULL;
    UI8 cCommand[MAX_CMD_LEN] ;

    pthread_mutex_lock(&stIcpList->icpListmutex);

    memset(cCommand , '\0' , MAX_CMD_LEN);

    sscanf(pcCmdName , "%s" , cCommand);

    if ( stIcpList->stHeader->stNext != NULL)
    {
        pstTmpIcpCntx = stIcpList->stHeader->stNext;
        pstPrevIcpCntx = stIcpList->stHeader;

        while ( pstTmpIcpCntx != NULL)
        {
            if ( strncmp(cCommand , pstTmpIcpCntx->cIcpCommand , iCmdLen) == 0)
            {
                    break;
            }
            pstTmpIcpCntx = pstTmpIcpCntx->stNext; 
       }
    }
    pthread_mutex_unlock(&stIcpList->icpListmutex);
    return pstTmpIcpCntx;
}

/*****************************************************************************
 *
 * name             :   ICP_Free_Ptr_Array
 * description      :   This function frees pointer array
 *
 * input parameters :   UI8 ** pvPtr  pointer array 
 *                      I32 iNoOfElem  no of elements in array  
 *
 * output parameters:   none
 *                      
 * return type      :   void
 * global variables :   none
 * calls            :   none
 ****************************************************************************/

void ICP_Free_Ptr_Array ( UI8 ** pvPtr , I32 iNoOfElem)
{
     I32 iLocalVar = 0;

    if ( pvPtr != NULL)
    {
        while ( iLocalVar < iNoOfElem)
        {
            if ( pvPtr[iLocalVar] != NULL)
            {
                free( pvPtr[iLocalVar] );
                pvPtr[iLocalVar] = NULL;
            }
            iLocalVar++;
        }
    }
    pvPtr = NULL;
}
        
/*****************************************************************************
 *
 * name             :   Util_GetIPFrom_ASNFormat
 * description      :   This function converts IP address in ASNFormat to
 *                      dotted decimal notation
 * input parameters :   char *asnFormat  IP address in ASN format
 *
 * output parameters:   char *buff  Ipaddress in dotted notation
 *                      
 * return type      :   none
 * global variables :   none
 * calls            :   none
 ****************************************************************************/
    

void Util_GetIPFrom_ASNFormat(char *asnFormat, char *buff)
{
    sprintf(buff,"%d.%d.%d.%d",(0xff& asnFormat[0]),(0xff & asnFormat[1]),
                               (0xff&asnFormat[2]), (0xff&asnFormat[3]));
}

/* End of filename.c [ Change filename.c ] */

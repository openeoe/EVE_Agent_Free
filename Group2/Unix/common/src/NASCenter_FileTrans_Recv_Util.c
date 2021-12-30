/*****************************************************************************
 *
 * NASCenter_FileTrans_Recv_Util.c
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Aravind Balan
 *
 * description :
 *      Contains utility functions for file transfer
 *
 * TODO:
 *      [ Things to be improved ]
 *
 * CM Details:
 *
 * $Author: baravind $
 * $Log: NASCenter_FileTrans_Recv_Util.c,v $
 * Revision 1.21  2004/01/13 15:44:46  baravind
 * Updates after Purify test . Since pure pointer manupulations behaving erratically , changed all pointer manupulations while sending/receiving packets  to static structure StMsg .
 *
 * Revision 1.20  2004/01/13 10:19:41  baravind
 * Updates after purify test
 *
 * Revision 1.19  2004/01/12 11:41:37  baravind
 * Check for sucess of thread creation done
 *
 * Revision 1.18  2003/12/11 15:42:46  baravind
 * Fixed warnings - Added by Aravind.B on Dec-11-2003
 *
 * Revision 1.17  2003/12/11 11:57:58  baravind
 * *** empty log message ***
 *
 * Revision 1.16  2003/12/11 11:29:34  vijaige
 * fixed handle leak
 *
 * Revision 1.15  2003/12/11 05:38:00  vijaige
 * After fixing handles
 *
 * Revision 1.14  2003/12/10 13:00:19  baravind
 * Code Fix for "Too many Open descriptors"  - Added by Aravind.B on 10-Dec-2003
 *
 * Revision 1.13  2003/12/10 11:46:11  vijaige
 * after the fix for five thread problem
 *
 * Revision 1.12  2003/12/10 07:03:42  vijaige
 * 10/12/2003
 *
 * Revision 1.11  2003/12/09 15:15:14  baravind
 * Incorporated codereview comments - Aravind.B on 9-Dev-2003
 *
 * Revision 1.10  2003/12/04 03:52:56  vijaige
 * after porting for windows
 *
 * Revision 1.9  2003/12/01 11:42:00  baravind
 * Added by Aravind.B on 1-Dec-2003
 *
 * Revision 1.5  2003/11/17 11:17:36  vijaige
 * 17th November
 *
 * Revision 1.4  2003/11/11 06:18:23  baravind
 * Added by Aravind.B on 11-NOV-2003
 *
 * Revision 1.2  2003/11/06 09:06:59  baravind
 * Checked in by Aravind Balan on 6-Nov-2003
 *
 * Revision 1.1  2003/11/05 16:29:08  baravind
 *  Added by Aravind Balan on 5-Nov-2003
 *
 * Revision 1.1.1.2  2003/10/17 06:41:51  hari
 * no message
 *
 * Revision 1.1.1.1  2003/10/01 05:23:22  thiagav
 * no message
 *
 * $State: Exp $
 * $Id: NASCenter_FileTrans_Recv_Util.c,v 1.21 2004/01/13 15:44:46 baravind Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/


/********************************* includes *********************************/
#ifdef WIN32
#include <Winsock2.h>
#include <stdio.h> 
#include <iostream.h> 
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>
#include <direct.h>
#include <time.h>
#endif

#include <NASCenter_FileTrans_Util.h>
#include <NASCenter_FileTrans.h>


/****************************** local macros ********************************
 *
 * (Macros shareable with rest of the modules are present in .h file)
 */

#define MAX_PKT_SIZE 1500

/********************* local structure/union definitions ********************/

typedef struct  stRecvData 
{
    void * pvData ;
    I32 iDataLen;
} stRecvData;

typedef struct stThreadArg
{
    struct sockaddr_in stClientAddr;
    stPacketHeader stPktHdr;
    stCntrlData stCntrlData;
}stThreadArg;

typedef int eNASCenterModuleId;




/********************* prototypes for local functions ***********************/

void FileTrans_HandleRequest (void * stThreadArg);

ENASCenterError FileTrans_SendAck ( I32 iSocketFd , void * pvHeader ,
                                    void * pvData , I32 iDataLen );

ENASCenterError FileTrans_RecvPkt ( I32 iSocketFd ,  stPacketHeader * pvHeader ,
                                    void * pvData , I32 iDataLen ,
                                    struct timeval *pstTimeOutValue ,
                                    struct sockaddr_in * pstFromAddr ,
                                    I32 iSocketAddrLen );

ENASCenterError FileTrans_HandleDataPkt ( I32 iSockFd ,
                                             stCntrlData * pstData  ,
                                             eNASCenterModuleId eModuleId);

ENASCenterError FileTrans_RecvDataPkt ( I32 iSocketFd ,
                                    struct timeval *pstTimeOutValue ,
                                    I32 iNoOfPktsExpected  ,
                                    UI8 * piPktStatus ,
                                    stRecvData * pstICoreAreaForData ,
                                    I32 *iToNoOfPktsRecvd ,
                                    stSequence * pstSequencing ,
                                    stCntrlData *pstData );

ENASCenterError FileTrans_SendDataAck ( I32 iSockFd ,  UI8 * piPktStatus ,
                                          I32 iNoPktsPerMMap ,
                                          I32 * piNoOfPktsExpected ,
                                          stSequence *pstSequencing ,
                                          ePacketType ePktType ,
                                          eNASCenterModuleId eModuleId);


void FileTrans_FreeScratch ( stRecvData * piPointer , I32 iNoOfElements);






/************************* Globals within this module ***********************/



/************************* externs  ***********************/

/*****************************************************************************
 *
 * name             :   FileTrans_SendAck
 * description      :   This function sends Ack packet to the other end 
 *
 * input parameters :   I32 iSocketFd  Socket descriptor 
 *                      void * pvHeader packet header 
 *                      void * pvData   data
 *                      I32 iDataLen    length of data 
 *
 * output parameters:   none
 *
 * return type      :   NASC_SUCCESS - if sucess
 *                      NASC_FAILURE - if failed 
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

ENASCenterError FileTrans_SendAck ( I32 iSocketFd , void * pvHeader , 
                                    void * pvData , I32 iDataLen )
{
    I32 iRetVal;
    StMsg pPktData ;
    

    ENASCenterError eReturnVal = NASC_SUCCESS;

    /* #ifdef WIN32
        itoa(iDataLen,len,10);
    #else
        sprintf(len , "%d" , (int) iDataLen);
    #endif

    iPkLen=sizeof(stPacketHeader)+strlen(len)+iDataLen;

    pPktData=(UI8  *) malloc( (iPkLen + 1) * sizeof(UI8) );

    if(pPktData==NULL)
    {
        return NASC_INSUFFICIENT_MEMORY;
    }

    memset(pPktData , '\0' , (iPkLen + 1) * sizeof(UI8));

    pPktStart = pPktData;

    memcpy(pPktStart,pvHeader,sizeof(stPacketHeader));

    pPktStart = pPktStart + sizeof(stPacketHeader);

 
    memcpy(pPktStart,len,strlen(len));

    pPktStart = pPktStart + strlen(len);

    memcpy(pPktStart,pvData,iDataLen);	 */
    memset(&pPktData , '\0' , sizeof(StMsg));
    memcpy(&pPktData.stHeader , pvHeader , sizeof(stPacketHeader));
    if ( pvData != NULL)
    {
        pPktData.iDataLen = htonl(iDataLen);
        memcpy(pPktData.cData , pvData , iDataLen);
    }

    iRetVal= sendto(iSocketFd,&pPktData,sizeof(StMsg),0,NULL,0);

    if(iRetVal < 0)
    {
        eReturnVal = NASC_FAILURE;
    }

    // FREE_PTR(pPktData);

    return eReturnVal;

}

/*****************************************************************************
 *
 * name             :   FileTrans_RecvPkt
 * description      :   This function receives ACK packet
 *                      If iSocketFd is unconnect UDP , pstDestAddr , 
 *                      iSocketAddrLen  should be specified . If iSocketFd is 
 *                      connected UDP socket , pstDestAddr should be NULL.
 *
 * input parameters :   I32 iSocketFd  Socket descriptor 
 *                      void * pvHeader packet header 
 *                      void * pvData   data
 *                      I32 iDataLen    length of data 
 *                      struct timeval *pstTimeOutValue timeout interval
 *                      struct sockaddr_in * pstFromAddr  destination address
 *                      I32 iSocketAddrLen  dest socket address length
 *
 * output parameters:   none
 *
 * return type      :   NASC_SUCCESS - if sucess
 *                      NASC_FAILURE - if failed 
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/
ENASCenterError FileTrans_RecvPkt ( I32 iSocketFd ,  
                                    stPacketHeader * pvHeader , 
                                    void * pvData , I32 iDataLen , 
                                    struct timeval *pstTimeOutValue , 
                                    struct sockaddr_in * pstFromAddr ,
                                    I32 iSocketAddrLen ) 
                                    
{
    I32 iRetVal;
    fd_set iSocketFDList;
    ENASCenterError eResult = NASC_SUCCESS;
    stPacketHeader stPktHdrInNetForm = { 0 } ;
    StMsg pcPktData;
   

    
    /* set file discriptors to be checked for data */

    FD_ZERO(&iSocketFDList);

    FD_SET(iSocketFd , &iSocketFDList);
  
    while ( 1 )
    { 
        memset(&pcPktData , '\0' , sizeof(StMsg));
        iRetVal = select( iSocketFd + 1 , &iSocketFDList , NULL , NULL ,
                          pstTimeOutValue ) ;

        if ( iRetVal == 0 )
        {
            /* syslog() */
            eResult =  NASC_FILE_TRANS_TIME_OUT;
            break;
        }
        else if (iRetVal < 0 )
        {
         #ifndef WIN32         
            if ( errno == EINTR)
            {
                continue;
            }
         #endif
            /* syslog() */
            eResult = NASC_FAILURE;
            break;
        }
        else 
        {
            iRetVal= recvfrom(iSocketFd,&pcPktData,sizeof(StMsg),0,
                              (struct sockaddr *)pstFromAddr,
                              (size_t *)&iSocketAddrLen);
            if ( iRetVal == 0 )
            {
                eResult = NASC_FILE_TRANS_CONN_CLOSED;
                break;
            }
            if(iRetVal < 0)
            {
                eResult = NASC_FAILURE;
                break;
            }
            else 
            {
                /* pcPktStart = pcPktData;		

                memcpy(&stPktHdrInNetForm,pcPktStart,sizeof(stPacketHeader));

                FileTrans_ConvertPktHdrToHostForm(&stPktHdrInNetForm , 
                                                  pvHeader); 

                pcPktStart = pcPktStart + sizeof(stPacketHeader);

                memcpy(clen,pcPktStart,sizeof(I32));

                iDataLen=atoi(clen);

                pcPktStart = pcPktStart + sizeof(I32);

                memcpy(pvData,pcPktStart,iDataLen); */

                memcpy(&stPktHdrInNetForm,&pcPktData.stHeader , sizeof(stPacketHeader));
                pcPktData.iDataLen = ntohl(pcPktData.iDataLen);

                memcpy(pvData , pcPktData.cData , iDataLen);

                eResult = NASC_SUCCESS;
                
                break;
            }
        }
    }
    return eResult;
}

/*****************************************************************************
 *
 * name             :   FileTrans_ProcessRecvMsg
 * description      :   This function Process messages received from other
 *                      end 
 *
 * input parameters :   I32 iSocketFd  
 *
 * output parameters:   none
 *
 * return type      :   none
 *
 * global variables :   [which are used in the function]
 * calls            :   FileTrans_HandleRequest
 ****************************************************************************/

void FileTrans_ProcessRecvMsg ( I32 iSocketFd)              
{
    pthread_t  thread;
    stPacketHeader stPktHdr = { 0 } ;
    stCntrlData    stData  = { 0 } ;
    struct sockaddr_in stFromAddr = { 0 }  ;
    stThreadArg  *stThreadParm = NULL;

    while ( 1 ) 
    {
        FileTrans_RecvPkt ( iSocketFd ,
                            &stPktHdr , 
                            &stData , sizeof(stCntrlData) , 
                            NULL , &stFromAddr , 
                            sizeof(struct sockaddr_in));

        if ( stPktHdr.ePktType != HND_SHAKE )
        {
            continue;
        }

        else 
        {
            stThreadParm = (stThreadArg *) malloc(sizeof(stThreadArg));
            memcpy(&(stThreadParm->stClientAddr) , &stFromAddr , 
                   sizeof ( struct sockaddr_in));

            memcpy(&(stThreadParm->stPktHdr) , &stPktHdr ,
                   sizeof (stPacketHeader));

            memcpy(&(stThreadParm->stCntrlData) , &stData ,
                   sizeof (stCntrlData));			

            if ( (pthread_create (&thread , NULL , 
                           (void *) FileTrans_HandleRequest , 
                           (void *) stThreadParm)) != 0 )
            {
																printf("Unable to create thread \n");
																exit ( 0 );
            }
       
            pthread_detach(thread);
        }

    }
}

/*****************************************************************************
 *
 * name             :   FileTrans_HandleRequest
 * description      :   This function will be executed in a thread for handling
 *                      pkts received from other end of communication
 * input parameters :   void * stThreadParm  Thead parameters
 *
 * output parameters:   none
 *
 * return type      :   none
 *
 * global variables :   [which are used in the function]
 * calls            :   FileTrans_SendAck , FileTrans_HandleDataPkt
 ****************************************************************************/
 void FileTrans_HandleRequest (void * stThreadParm)
 {
     struct sockaddr_in stClientAddr = { 0 };
     stPacketHeader     stPktHdr = { 0 } ;
     struct sockaddr_in stMyAddress = { 0 } ;
     I32 iSockFd;
     stThreadArg  *stThreadParms;
     ENASCenterError eRetVal = NASC_SUCCESS;
     stCntrlData    stData = { 0 } ;
     stSequence  stSequencing; 
 


    stSequencing.iMmapNo = 0;
    stSequencing.iCorrelater = 1;

    
     stThreadParms = (stThreadArg *) stThreadParm;

     
     memcpy(&stClientAddr ,  
            (struct sockaddr_in *) &(stThreadParms->stClientAddr) , 
            sizeof ( struct sockaddr_in));

     memcpy(&stPktHdr ,  
            (struct stPacketHeader *) &(stThreadParms->stPktHdr) , 
            sizeof ( struct stPacketHeader));
     memcpy(&stData ,  
            (stCntrlData *) &(stThreadParms->stCntrlData) , 
            sizeof (stCntrlData));
     if(stThreadParms)
         FREE_PTR(stThreadParms);



     /* create socket for communication */

     iSockFd = socket(AF_INET , SOCK_DGRAM , 0);

     /* Fill in Socket details */
 

     stMyAddress.sin_port = htons(0);
     stMyAddress.sin_family = AF_INET;
     stMyAddress.sin_addr.s_addr = (INADDR_ANY);

     bind ( iSockFd , (struct sockaddr *) &stMyAddress , sizeof ( stMyAddress));

     /* connect to client socket */

     connect ( iSockFd , (struct sockaddr *) &stClientAddr , 
              sizeof(struct sockaddr_in));

     stPktHdr.ePktType =  htonl(HND_SHAKE_ACK) ; 

     /* Add sequencing */

    stPktHdr.iMmapNo = htonl(stSequencing.iMmapNo);
    stPktHdr.iCorrelater = htonl(stSequencing.iCorrelater);


     /* send Ack for Handshake */

     eRetVal = FileTrans_SendAck ( iSockFd , &stPktHdr , NULL , 0);

     if ( eRetVal == NASC_SUCCESS ) 
     {
         /* Handle Data Pkts */

         eRetVal = FileTrans_HandleDataPkt ( iSockFd , &stData , 
                                             stPktHdr.eModuleId);
     }
#ifdef WIN32
     closesocket( iSockFd );
#else 
     close ( iSockFd ); 
#endif
 
     
     pthread_exit((void *) eRetVal);

}

/*****************************************************************************
 *
 * name             :   FileTrans_HandleDataPkt
 * description      :   This function Handles data packets received from 
 *                      other end of communication
 *
 * input parameters :   I32 iSockFd  Socket descriptor 
 *                      stCntrlData * pstData control data 
 *                      eNASCenterModuleId eModuleId  module id of application
 *                      which called this library
 *
 * output parameters:   none
 *
 * return type      :   NASC_SUCCESS - if sucess
 *                      NASC_FAILURE - if failed 
 *
 * global variables :   [which are used in the function]
 * calls            :   FileTrans_RecvDataPkt , FileTrans_SendDataAck
 ****************************************************************************/

ENASCenterError FileTrans_HandleDataPkt ( I32 iSockFd , 
                                           stCntrlData * pstData  ,
                                          eNASCenterModuleId eModuleId)
{
    I32 iNoOfTimeOuts = 0 ; 
    ENASCenterError eReturnValue = NASC_SUCCESS;
    struct timeval stTimeoutInterval ; 
    double dRtt ;
    I32 iNoOfPktsExpected;
    I32 iNoOfPktsPerMap = 0;
    UI8 * piPktStatus = NULL;
    stRecvData * pstICoreAreaForData = NULL;
    I32 iToNoOfPktsRecvd = 0;
    stSequence  stSequencing; 
    struct stat stDirStatus ;
#ifdef WIN32
HANDLE iFileDesc;
DWORD dwbytes;
#else
    I32 iFileDesc;
#endif
    I32 iLoopCntr = 0;
    UI8 *pcFileName = NULL;
    stCntrlData stControlData; 


    stSequencing.iMmapNo = 1;
    stSequencing.iCorrelater = 1;

    stControlData.dRtt = ntohl(pstData->dRtt);
    stControlData.iTotNoPkts = ntohl(pstData->iTotNoPkts);
    stControlData.iTotNoMaps = ntohl(pstData->iTotNoMaps);
    stControlData.iNoPktsPerMMap = ntohl(pstData->iNoPktsPerMMap);
    stControlData.iNoExtraMapPackets = ntohl(pstData->iNoExtraMapPackets);
    stControlData.iMesgSizeForLastPkt = ntohl(pstData->iMesgSizeForLastPkt);

    memset(stControlData.pcDestPath , '\0' , MAX_PATH_LEN);
    memset(stControlData.pcFileName , '\0' , MAX_FILE_NAME_LEN);

    memcpy(stControlData.pcDestPath , pstData->pcDestPath , 
           strlen(pstData->pcDestPath));

    memcpy(stControlData.pcFileName , pstData->pcFileName , 
           strlen(pstData->pcFileName));

     /* Check if Destination path is been specified */
 
     if ( strlen(stControlData.pcDestPath) == 0 )
     {
         /* if not specified , assume current dir */

         memcpy(stControlData.pcDestPath , "." , sizeof(I8)); 
         stControlData.pcDestPath[1] = '\0';
     }
     else 
     {
         /* check if specified dir exists */
         if ( stat(stControlData.pcDestPath , &stDirStatus) < 0 )
         {
             /* if dir does not exist attempt to create */
#ifdef WIN32
    if( mkdir(stControlData.pcDestPath) != 0 ) 
#else             
             if ( mkdir(stControlData.pcDestPath  , 0766) < 0 ) 
#endif
             {
                 /* Assume current dir */
                 memcpy(stControlData.pcDestPath , "." , sizeof(I8)); 
                 stControlData.pcDestPath[1] = '\0';
             }
          }
      }

     /* allocate space for File name . Extra 3 space is for / charater */

     pcFileName = (UI8 *) malloc (strlen(stControlData.pcDestPath) + 
                                strlen(stControlData.pcFileName) + 3);      

     if ( pcFileName == NULL) 
     {
         return NASC_FAILURE;
     }

     memset ( pcFileName , '\0' , strlen(stControlData.pcDestPath) + 
              strlen(stControlData.pcFileName) + 3 );
#ifdef WIN32
    sprintf(pcFileName , "%s\\%s" , stControlData.pcDestPath , stControlData.pcFileName);
#else
     sprintf(pcFileName , "%s/%s" , stControlData.pcDestPath , stControlData.pcFileName);
#endif

     printf("File : %s\n" , pcFileName);

    /* open the file */
#ifdef WIN32
    iFileDesc = CreateFile(pcFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    if(!iFileDesc) 
#else
    iFileDesc = open ( pcFileName , O_RDWR | O_CREAT , 
                       S_IREAD | S_IWUSR | S_IRGRP | S_IROTH);
    if ( iFileDesc == -1 )
#endif

    
    {
        /* syslog */
        if ( pcFileName != NULL)
        {
            FREE_PTR(pcFileName);
        }
        return NASC_FAILURE;
    }

    dRtt = stControlData.dRtt;

    iNoOfPktsExpected = stControlData.iNoPktsPerMMap;
    iNoOfPktsPerMap = stControlData.iNoPktsPerMMap;

    /* Allocate memory for packet status and data */

    piPktStatus = (UI8 *) malloc ( (stControlData.iNoPktsPerMMap + 1) 
                                   * sizeof(UI8));

    if ( piPktStatus == NULL )
    {
        /* syslog */
        FREE_PTR(pcFileName);
#ifdef WIN32
    CloseHandle(iFileDesc);
#else
        close(iFileDesc);
#endif
        return NASC_INSUFFICIENT_MEMORY;
    }
    memset ( piPktStatus , '0' , (stControlData.iNoPktsPerMMap + 1)
             * sizeof(UI8));

    pstICoreAreaForData = (stRecvData *) malloc ( stControlData.iNoPktsPerMMap * 
                                           sizeof(stRecvData)); 
    if ( pstICoreAreaForData == NULL )
    {
        /* syslog */
        FREE_PTR(piPktStatus); 
        FREE_PTR(pcFileName);
#ifdef WIN32
    CloseHandle(iFileDesc);
#else
        close(iFileDesc);
#endif
        return NASC_INSUFFICIENT_MEMORY;
    }
    memset ( pstICoreAreaForData , '\0' , stControlData.iNoPktsPerMMap * 
            sizeof(stRecvData));

    while ( 1 ) 
    {
        /* fill in timeout details */

        stTimeoutInterval.tv_sec = ( long ) (dRtt / 1000.0) ;
        stTimeoutInterval.tv_usec = (( dRtt * 1000 ) - 
                                     stTimeoutInterval.tv_sec) ; 
        stTimeoutInterval.tv_sec += DELTA_TIME_FOR_RECV_DATA;

        eReturnValue = FileTrans_RecvDataPkt ( iSockFd ,
                                              &stTimeoutInterval ,
                                              iNoOfPktsExpected ,
                                              piPktStatus , 
                                              pstICoreAreaForData  , 
                                              &iToNoOfPktsRecvd ,
                                              &stSequencing , &stControlData);

        if (eReturnValue == NASC_FILE_TRANS_TIME_OUT )
        {
            iNoOfTimeOuts ++;
           
            if ( iNoOfTimeOuts >  MAX_NO_OF_RETRIES ) 
            {
                /* syslog */
                break;
            }

            FileTrans_SendDataAck ( iSockFd ,  piPktStatus , 
                                    iNoOfPktsPerMap , 
                                    &iNoOfPktsExpected  , &stSequencing ,
                                    DATA_NEG_ACK , eModuleId );

           stSequencing.iCorrelater++;
           continue;
        }            
        if ( eReturnValue != NASC_SUCCESS)
        {
            /* syslog */
            break;
        }
        else 
        {
            eReturnValue = FileTrans_SendDataAck ( iSockFd ,  NULL , 
                                                      0 , 
                                                     NULL ,
                                                     &stSequencing ,
                                                     DATA_ACK , eModuleId );
            if ( eReturnValue == NASC_SUCCESS)
            {
                /* write data into file */
                while ( iLoopCntr < iNoOfPktsPerMap )
                {
#ifdef WIN32
    WriteFile(iFileDesc,pstICoreAreaForData[iLoopCntr].pvData , 
                          pstICoreAreaForData[iLoopCntr].iDataLen,&dwbytes,NULL);
      
#else
                    write(iFileDesc , pstICoreAreaForData[iLoopCntr].pvData , 
                          pstICoreAreaForData[iLoopCntr].iDataLen);
#endif
                    iLoopCntr++;                    
                }
                iLoopCntr = 0;
                FileTrans_FreeScratch(pstICoreAreaForData , iNoOfPktsPerMap); 
                memset ( pstICoreAreaForData , '\0' , 
                         stControlData.iNoPktsPerMMap * sizeof(stRecvData));
                memset(piPktStatus , '0' ,(stControlData.iNoPktsPerMMap + 1) *
                       sizeof(UI8)); 
               /*  printf("Total packets received %d\n" , iToNoOfPktsRecvd); */               
                if ( iToNoOfPktsRecvd == stControlData.iTotNoPkts )
                {
                    // printf("All packets received..\n");
                    break;
                }

                stSequencing.iMmapNo++;
                stSequencing.iCorrelater = 1;
                iNoOfPktsExpected = stControlData.iNoPktsPerMMap;
               if ( stSequencing.iMmapNo == stControlData.iTotNoMaps)
               {
                   if ( stControlData.iNoExtraMapPackets != 0 )
                   {
                       iNoOfPktsExpected = stControlData.iNoExtraMapPackets;
                       iNoOfPktsPerMap =   stControlData. iNoExtraMapPackets;
                   }
                   
               }     
            }
            else
            {
                /* syslog */
                break;
            }
        }
    } 

    /* close file */
#ifdef WIN32
    CloseHandle(iFileDesc);
#else
    close ( iFileDesc );
#endif
    FileTrans_FreeScratch(pstICoreAreaForData , iNoOfPktsPerMap);    
    FREE_PTR ( pstICoreAreaForData);
    FREE_PTR(piPktStatus);
    FREE_PTR(pcFileName);
    return eReturnValue;
}
/*****************************************************************************
 *
 * name             :   FileTrans_SendDataAck
 * description      :   This function Sends Ack / Negative Ack 
 *                      for data packet . 
 *                      
 *
 * input parameters :   I32 iSockFd  Socket descriptor 
 *                      I32 * piPktStatus Array which contains status of packet
 *                      I32 iNoPktsPerMMap No Of packets per sliding window
 *                      stSequence *stSequencing  Sequencing parameters
 *                      eNASCenterModuleId eModuleId  Module id of application
 *                      that called this library
 *
 *
 * output parameters:   none
 *
 * return type      :   NASC_SUCCESS - if sucess
 *                      NASC_FAILURE - if failed 
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

ENASCenterError FileTrans_SendDataAck ( I32 iSockFd ,  UI8 * piPktStatus , 
                                          I32 iNoPktsPerMMap , 
                                          I32 * piNoOfPktsExpected , 
                                          stSequence *pstSequencing , 
                                          ePacketType ePktType , 
                                          eNASCenterModuleId eModuleId)
{
    StMsg pcPktData;
    I32 iDataLen;
    stPacketHeader pkHeader = { 0 } ;     
    I32 iRetVal;
    ENASCenterError eResult = NASC_SUCCESS;
    I32 iLoopCntr = 0;



    /* fill up Header */  

    pkHeader.ePktType = htonl(ePktType);
    pkHeader.eModuleId = htonl(eModuleId);
    pkHeader.iMmapNo = htonl(pstSequencing->iMmapNo);
    pkHeader.iCorrelater = htonl(pstSequencing->iCorrelater);	

    
    /* fill up data */

    if ( piPktStatus != NULL)
    { 
        *piNoOfPktsExpected = 0;

         while ( iLoopCntr < iNoPktsPerMMap )
         {
             if ( piPktStatus[iLoopCntr] == '0' )
             {
                 *piNoOfPktsExpected = *piNoOfPktsExpected + 1;
             }
             iLoopCntr++;
         }
    }
    iDataLen = sizeof(UI8) * iNoPktsPerMMap;
    /*  #ifdef WIN32
        itoa(iDataLen,clen,10);
    #else 
        sprintf(clen , "%d" , (int)iDataLen);
    #endif */

    /* pcPktData = (UI8  *)malloc( (iPkLen + 1) * sizeof(UI8));
    if(pcPktData==NULL)
    {
        return NASC_INSUFFICIENT_MEMORY;
    } */
   
    memset(&pcPktData , '\0' ,sizeof(StMsg) ); 
    
    /* pcPktStart = pcPktData;

    memcpy(pcPktStart,&pkHeader,sizeof(stPacketHeader));

    pcPktStart = pcPktStart + sizeof(stPacketHeader);
   

    memcpy(pcPktStart,clen,strlen(clen));

    pcPktStart = pcPktStart + strlen(clen);

    memcpy(pcPktStart,piPktStatus,iDataLen);	 */

    memcpy(&pcPktData.stHeader , &pkHeader , sizeof(stPacketHeader));

     pcPktData.iDataLen = htonl(iDataLen);

     memcpy(pcPktData.cData , piPktStatus , iDataLen);


    iRetVal = sendto(iSockFd,&pcPktData,sizeof(StMsg),0,NULL,0);

    if(iRetVal <0)
    {
        eResult  = NASC_FAILURE;
    }

    // FREE_PTR(pcPktData);

    return eResult;
}    
/*****************************************************************************
 *
 * name             :   FileTrans_RecvDataPkt
 * description      :   This function receives Data packet 
 *
 * input parameters :   I32 iSocketFd  Socket descriptor 
 *                      struct timeval *pstTimeOutValue time out value
 *                      I32 iNoOfPktsExpected No of pakcets expected from 
 *                                            other end of communication
 *                      I32 * piPktStatus Array which contains status of packet
 *                      stRecvData * pstICoreAreaForData  Memory area which
 *                      contain address of data received from other end
 *                      I32 *iToNoOfPktsRecvd  Total noof pkts received 
 *                      stSequence * stSequencing  sequencing parameters
 *
 * output parameters:   none
 *
 * return type      :   NASC_SUCCESS - if sucess
 *                      NASC_FAILURE - if failed 
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/
    
ENASCenterError FileTrans_RecvDataPkt ( I32 iSocketFd , 
                                    struct timeval *pstTimeOutValue , 
                                    I32 iNoOfPktsExpected  , 
                                    UI8 * piPktStatus , 
                                    stRecvData * pstICoreAreaForData , 
                                    I32 *iToNoOfPktsRecvd , 
                                    stSequence * pstSequencing  , 
                                    stCntrlData *pstData) 
                                    
{
    // UI8 *pcPktData=NULL;
    StMsg pcPktData  ;
    I32 iDataLen;
    UI8 *uData=NULL;
    stPacketHeader stpkHeaderInNetForm; 
    stPacketHeader stpkHeaderInHstForm; 
    I32 iRetVal;
    fd_set iSocketFDList;
    ENASCenterError eResult = NASC_SUCCESS;
    I32 iLoopCntr = 0; 
    I32 iArraySubScript = 0; 


    /* set file discriptors to be checked for data */

    FD_ZERO(&iSocketFDList);

    FD_SET(iSocketFd , &iSocketFDList);

    /* pcPktData=(UI8 *)malloc( (MAX_PKT_SIZE + 1) * sizeof(UI8));
    if(pcPktData == NULL)
    {
        return NASC_INSUFFICIENT_MEMORY;
    } */

   
    while ( 1 )
    {
        memset(&pcPktData , '\0' , sizeof(StMsg));

        if ( iLoopCntr >=  iNoOfPktsExpected )
        {
            eResult = NASC_SUCCESS;
            break;
        } 

        iRetVal = select( iSocketFd + 1 , &iSocketFDList , NULL , NULL ,
                          pstTimeOutValue ) ;

        if ( iRetVal == 0 )
        {
            /* syslog() */
            eResult =  NASC_FILE_TRANS_TIME_OUT;
            break ; 
        }
        else if (iRetVal < 0 )
        {
#ifndef WIN32
            if ( errno == EINTR)
            {
                continue;
            }
#endif
            /* syslog() */
            eResult = NASC_FAILURE;
            break;
        }
        else 
        {
            iRetVal= recvfrom(iSocketFd,&pcPktData,sizeof(StMsg),0, NULL,0);			

            if ( iRetVal == 0 )
            {
                eResult = NASC_FILE_TRANS_CONN_CLOSED;
                break;
            }
            
            if ( iRetVal < 0 )
            {
                /* syslog */
                eResult =  NASC_FAILURE;
                break;
            }
            else 
            {
                // pcPktStart=pcPktData;		
                memcpy(&stpkHeaderInNetForm,&pcPktData.stHeader,sizeof(stPacketHeader));
                FileTrans_ConvertPktHdrToHostForm(&stpkHeaderInNetForm , 
                                                  &stpkHeaderInHstForm);          
                // pcPktStart = pcPktStart + sizeof(stPacketHeader);
                /* memcpy(clen,pcPktStart,sizeof(I32));
                iDataLen=atoi(clen);
                pcPktStart = pcPktStart + sizeof(I32); */

                 iDataLen = ntohl(pcPktData.iDataLen);
                uData=(UI8 *) malloc ( (iDataLen +  1) * sizeof(UI8));			
                if(uData ==NULL)
                {
                    eResult  = NASC_INSUFFICIENT_MEMORY;
                    break;
                }

                memset(uData , '\0' , iDataLen);

                // memcpy(uData,pcPktStart,iDataLen);
                memcpy(uData,pcPktData.cData,iDataLen);


                if ( stpkHeaderInHstForm.ePktType != DATA ) 
                {
                    /* syslog */
                    FREE_PTR(uData);
                    continue;
                    
                }
                if ( stpkHeaderInHstForm.iMmapNo == pstSequencing->iMmapNo &&
                     stpkHeaderInHstForm.iCorrelater == 
                                                    pstSequencing->iCorrelater)
                {
                    iArraySubScript = (stpkHeaderInHstForm.iPktNo  - 
                                       ((pstSequencing->iMmapNo - 1) * 
                                       pstData->iNoPktsPerMMap)) - 1 ;

                    piPktStatus [iArraySubScript] = '1';
                    pstICoreAreaForData[iArraySubScript].pvData 
                                         = uData;
                    pstICoreAreaForData[iArraySubScript].
                    iDataLen = iDataLen; 

                    uData = NULL;
                    iLoopCntr++;
                    *iToNoOfPktsRecvd = *iToNoOfPktsRecvd + 1;
                    continue ;
                }
                else 
                {
                    FREE_PTR(uData);
                    continue ;
                         
                }
                   
            }
        }
    }

    // FREE_PTR(pcPktData);

    return eResult;
}

void FileTrans_FreeScratch ( stRecvData * piPointer , I32 iNoOfElements) 
{
    I32 iLocalVar = 0; 

    if ( piPointer != NULL) 
    { 
        while (iLocalVar < iNoOfElements) 
        { 
            if ( piPointer[iLocalVar].pvData != NULL) 
            { 
                free(piPointer[iLocalVar].pvData); 
               piPointer[iLocalVar].pvData = NULL; 
            }
            iLocalVar++;
       } 
    } 
}  



/* End of NASCenter_FileTrans_util.c */

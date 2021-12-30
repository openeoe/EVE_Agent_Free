/*****************************************************************************
 *
 * NASCenter_FileTrans.c
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Aravind Balan
 *
 * description :
 *      Contains functions for transfering file
 *
 * TODO:
 *      [ Things to be improved ]
 *
 * CM Details:
 *
 * $Author: baravind $
 * $Log: NASCenter_FileTrans.c,v $
 * Revision 1.20  2004/01/13 15:44:46  baravind
 * Updates after Purify test . Since pure pointer manupulations behaving erratically , changed all pointer manupulations while sending/receiving packets  to static structure StMsg .
 *
 * Revision 1.19  2003/12/11 15:42:46  baravind
 * Fixed warnings - Added by Aravind.B on Dec-11-2003
 *
 * Revision 1.18  2003/12/11 14:59:38  baravind
 * Fixed sendto problem . sento returned -1 in AIX . Added by Aravind.B on Dec-11-2003
 *
 * Revision 1.17  2003/12/11 11:57:58  baravind
 * *** empty log message ***
 *
 * Revision 1.16  2003/12/11 11:29:34  vijaige
 * fixed handle leak
 *
 * Revision 1.14  2003/12/11 05:38:00  vijaige
 * After fixing handles
 *
 * Revision 1.13  2003/12/10 13:30:22  baravind
 * Fix for source  File path  -  Added by Aravind on 10-Dec-2003
 *
 * Revision 1.12  2003/12/10 13:00:19  baravind
 * Code Fix for "Too many Open descriptors"  - Added by Aravind.B on 10-Dec-2003
 *
 * Revision 1.11  2003/12/10 06:52:55  vijaige
 * 10/12/2003
 *
 * Revision 1.10  2003/12/09 15:15:14  baravind
 * Incorporated codereview comments - Aravind.B on 9-Dev-2003
 *
 * Revision 1.9  2003/12/04 03:53:19  vijaige
 * after porting for windows
 *
 * Revision 1.8  2003/12/01 05:08:55  baravind
 * Added by Aravind.B on Dec-1-2003
 *
 * Revision 1.5  2003/11/17 11:17:36  vijaige
 * 17th November
 *
 * Revision 1.4  2003/11/11 06:18:01  baravind
 * Added by Aravind.B on 11-NOV-2003
 *
 * Revision 1.2  2003/11/06 09:07:46  baravind
 * Checked in by Aravind Balan on 6-Nov-2003
 *
 * Revision 1.1  2003/11/03 08:34:43  baravind
 *  Checked in by Aravind.b on Nov-3-2003 
 *
 * Revision 1.1.1.2  2003/10/17 06:41:51  hari
 * no message
 *
 * Revision 1.1.1.1  2003/10/01 05:23:22  thiagav
 * no message
 *
 * $State: Exp $
 * $Id: NASCenter_FileTrans.c,v 1.20 2004/01/13 15:44:46 baravind Exp $
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
#endif

#include <NASCenter_FileTrans_Util.h>
#include <NASCenter_FileTrans.h>



/****************************** local macros ********************************
 *
 * (Macros shareable with rest of the modules are present in .h file)
 */


/********************* local structure/union definitions ********************/



/********************* prototypes for local functions ***********************/



/************************* Globals within this module ***********************/
#ifdef WIN32
UI8 gcFileDelimiter = '\\';
UI8 gcStringFileDelimiter [] = "\\";
#else
UI8 gcFileDelimiter = '/';
UI8 gcStringFileDelimiter [] = "/";
#endif

/************************* externs  ***********************/

/*****************************************************************************
 *
 * name             :   FileTrans_SendFile
 * description      :   This function Sends specified file to given destination
 * input parameters :   I8 * pcFileName  Name of file  to be sent 
 *                      struct sockaddr_in * pstDestAddr Destination address
 *                      I8 * pcDestPath Path in which file has to be stored at
 *                                      the destination
 *                      I32 iMsgSize    Message size per packet
 *
 * output parameters:   
 *
 * return type      :   ENASCenterError
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

 ENASCenterError FileTrans_SendFile ( UI8 * pcFileName , 
                                      struct sockaddr_in * pstDestAddr , 
                                      UI8 * pcDestPath , I32 iMsgSize)
 {
     double dRtt;
     struct sockaddr_in stMyPortDetail;
     struct sockaddr_in  stDestPortDetail;
     struct stat  stFileInfo;
     UI8 * pcDestAddress = NULL;
     ENASCenterError eStatus = NASC_SUCCESS;
     I32 iSocketFd;
     I32 retValue;
     I32 iTotNoOfPkts;
     I32 iPageSize = 0;
     I32 iNoPktsPerMMap = 0;
     I32 iNoOfMappingNeeded = 0;
     I32 iExtraFileSize = 0;
     UI8 * pcTempStr = NULL;
     UI8 * pcTempString = NULL;
     #ifdef WIN32
         HANDLE FileHandle;
     #else
         I32 iFileDescriptor;
     #endif
     I32 iExtraPktsPerMap = 0;
     I32 iLastPktSize = 0;
     #ifdef WIN32
         SYSTEM_INFO stsysInfo;
     #endif
   
     /* Check if iMsgSize is specified */
     
     if ( iMsgSize == 0 )
     {
         iMsgSize =  DEF_MSG_SIZE;
      }

      if ( iMsgSize > MAX_DATA_SIZE )
      {
          iMsgSize = MAX_DATA_SIZE;
      }

     /* Creat UDP socket for file tranfer */

     iSocketFd = socket( AF_INET , SOCK_DGRAM , 0 );

     /* Fill in port  details */

     stMyPortDetail.sin_family = AF_INET;
     stMyPortDetail.sin_addr.s_addr = htonl(INADDR_ANY);
     stMyPortDetail.sin_port = htons(0);    

     /* bind the socket to the port */

     retValue = bind( iSocketFd , (struct sockaddr *) &stMyPortDetail , 
           sizeof( stMyPortDetail ) );

     if ( retValue < 0 )
     {
        /* sysLog */
        #ifdef WIN32
            closesocket(iSocketFd);
        #else
            close(iSocketFd);
        #endif
        return NASC_FAILURE;
     }
     if ( pstDestAddr == NULL) 
     {
         return NASC_FAILURE;
     } 
     pcDestAddress = inet_ntoa(pstDestAddr->sin_addr); 

     /*eStatus = FileTrans_GetRTT(pstDestAddr ,  &dRtt); 

     if ( eStatus != NASC_SUCCESS )
     {
       
         #ifdef WIN32
             closesocket(iSocketFd);
         #else
             close(iSocketFd);
         #endif
         return NASC_FAILURE;
     }*/

    dRtt = 0;


     /* Open the given file */
    #ifdef WIN32
        FileHandle=CreateFile(pcFileName,
                              GENERIC_READ,
                              0,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);

        if(!FileHandle)
        {

            closesocket(iSocketFd);
            return NASC_FAILURE;

        }
#else    
     iFileDescriptor = open ( pcFileName , O_RDWR);


     if ( iFileDescriptor < 0 )
     {
         /* syslog */
         close(iSocketFd);
         return NASC_FAILURE;
     } 
#endif

     /* get file size */

     retValue = stat ( pcFileName , &stFileInfo);
     if(retValue !=0)
     {
         #ifdef WIN32
             CloseHandle(FileHandle);
             closesocket(iSocketFd);
         #else
             close(iFileDescriptor);
             close(iSocketFd);
         #endif
         return NASC_FAILURE;
     }

     /* Calculate total no pkts required for sending the file */

     iTotNoOfPkts = stFileInfo.st_size / iMsgSize;

     if ( stFileInfo.st_size % iMsgSize ) 
     {
         iTotNoOfPkts++;
     }

     /* For getting data from file , either mmap will be used  Each time 
        one pagesize data will be sent in pakcets at one shot 
        and wait for ACK . This continues until entire file is transfered */

    /* get Pagesize of the Operation system */

    #ifdef WIN32
        GetSystemInfo(&stsysInfo);
        iPageSize=stsysInfo.dwPageSize;
    #else
        iPageSize = sysconf(_SC_PAGE_SIZE);
    #endif

    iNoOfMappingNeeded =  stFileInfo.st_size / iPageSize ;

    if ( stFileInfo.st_size % iPageSize != 0 )
    {
        iNoOfMappingNeeded++;
    }
    iExtraFileSize = stFileInfo.st_size % iPageSize ;

    if ( stFileInfo.st_size <= iPageSize)
    {
        iNoPktsPerMMap = iTotNoOfPkts ;
    }   
    else 
    {
        iNoPktsPerMMap = iPageSize / iMsgSize;

         if (iPageSize % iMsgSize != 0 )
         {
             iNoPktsPerMMap ++;
         }
        iExtraPktsPerMap = iExtraFileSize / iMsgSize;
        if ( (iExtraFileSize % iMsgSize)   != 0)
        {
            iExtraPktsPerMap++;
        }
    }
     

     iLastPktSize = iExtraFileSize % iMsgSize; 

     pcTempStr = strrchr(pcFileName , gcFileDelimiter);

    if ( pcTempStr != NULL)
    {
        #ifdef WIN32
            pcFileName = strtok(pcTempStr , gcStringFileDelimiter);
        #else
            pcFileName = strtok_r(pcTempStr , gcStringFileDelimiter , 
                                  (char **) &pcTempString);
        #endif
    }


     eStatus = FileTransfer_InitHandshake( iSocketFd , pstDestAddr , 
                                           &stDestPortDetail , dRtt ,
                                           pcFileName , 
                                           pcDestPath , 
                                           iTotNoOfPkts , iNoPktsPerMMap ,
                                           iExtraPktsPerMap , iLastPktSize ,
                                           iMsgSize , iNoOfMappingNeeded);
     /* connect to the destination port / ipaddress in stDestPortDetail */

     retValue = connect (iSocketFd ,  (struct sockaddr *)&stDestPortDetail , 
                         sizeof(struct sockaddr_in));
     if ( retValue < 0 )
     {
         /*syslog */
         #ifdef WIN32
             CloseHandle(FileHandle);
             closesocket(iSocketFd);
         #else
             close ( iFileDescriptor );
             close(iSocketFd);
         #endif
         return NASC_FAILURE;
     }

     if ( eStatus != NASC_SUCCESS )
     {
         /* syslog */
         #ifdef WIN32
             CloseHandle(FileHandle);
             closesocket(iSocketFd);
         #else
             close ( iFileDescriptor );
             close(iSocketFd);
         #endif
         
         return NASC_FAILURE;
      }
     #ifdef WIN32 
      eStatus =  FileTransfer_SendData (iSocketFd , pcFileName , 
                                        FileHandle , stFileInfo.st_size ,
                                        iTotNoOfPkts ,
                                        iPageSize , iNoOfMappingNeeded , 
                                        iNoPktsPerMMap , iMsgSize ,
                                         dRtt ,iLastPktSize , iExtraPktsPerMap ,
                                         &stDestPortDetail);

     CloseHandle ( FileHandle );

     #else
      eStatus =  FileTransfer_SendData (iSocketFd , pcFileName , 
                                        iFileDescriptor , stFileInfo.st_size ,
                                        iTotNoOfPkts ,
                                        iPageSize , iNoOfMappingNeeded , 
                                        iNoPktsPerMMap , iMsgSize ,
                                         dRtt ,iLastPktSize , iExtraPktsPerMap ,
                                         NULL);

     close ( iFileDescriptor );
     #endif
     #ifdef WIN32
         closesocket(iSocketFd);
     #else
        close(iSocketFd);
     #endif

     return eStatus;
}
      
/*****************************************************************************
 *
 * name             :   FileTrans_RecvFile
 * description      :   This function receives file from other end of 
 *                      communication
 * input parameters :   struct sockaddr_in * pstSockAddr - well known port 
 *
 * output parameters:   
 *
 * return type      :   ENASCenterError
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

void FileTrans_RecvFile ( struct sockaddr_in * pstSockAddr )
{
    I32    iSocketFd;
    I32    iRetValue;

    /* Creat Socket and bind it to well-knowm port given in pstSockAddr */

    iSocketFd = socket( AF_INET , SOCK_DGRAM , 0 );

    iRetValue = bind ( iSocketFd , (struct sockaddr *) pstSockAddr , 
                       sizeof(struct sockaddr_in));
    if ( iRetValue < 0 )
    {
        #ifdef WIN32 
    	    closesocket(iSocketFd);
        #else
            close(iSocketFd);
        #endif
        return;
    }

    /* Recv pkts from well-known port and fall in infinite loop */

    FileTrans_ProcessRecvMsg ( iSocketFd );

}
      
     
        
         

  

     

            


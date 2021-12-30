/*****************************************************************************
 *
 * NASCenter_FileTrans_util.c
 *
 *      Copyright (c) 2002 Wipro Technologies
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
 * $Log: NASCenter_FileTrans_Send_Util.c,v $
 * Revision 1.23  2004/01/14 06:05:56  baravind
 * Added memset after Purify test
 *
 * Revision 1.22  2004/01/13 15:44:46  baravind
 * Updates after Purify test . Since pure pointer manupulations behaving erratically , changed all pointer manupulations while sending/receiving packets  to static structure StMsg .
 *
 * Revision 1.21  2004/01/13 10:19:41  baravind
 * Updates after purify test
 *
 * Revision 1.20  2004/01/12 13:01:06  baravind
 * updates after purify test
 *
 * Revision 1.19  2003/12/13 15:46:37  baravind
 * changed socklen_t -> size_t (socklen_t not there in unixware) . Added by Aravind.b on 13-Dec-2003
 *
 * Revision 1.18  2003/12/11 15:42:46  baravind
 * Fixed warnings - Added by Aravind.B on Dec-11-2003
 *
 * Revision 1.17  2003/12/11 15:12:56  baravind
 * *** empty log message ***
 *
 * Revision 1.16  2003/12/11 14:59:38  baravind
 * Fixed sendto problem . sento returned -1 in AIX . Added by Aravind.B on Dec-11-2003
 *
 * Revision 1.15  2003/12/11 11:29:34  vijaige
 * fixed handle leak
 *
 * Revision 1.13  2003/12/11 05:38:01  vijaige
 * After fixing handles
 *
 * Revision 1.12  2003/12/11 02:54:16  vijaige
 * Fixed MemHandle
 *
 * Revision 1.11  2003/12/10 13:00:19  baravind
 * Code Fix for "Too many Open descriptors"  - Added by Aravind.B on 10-Dec-2003
 *
 * Revision 1.10  2003/12/10 06:43:29  vijaige
 * windows porting
 *
 * Revision 1.9  2003/12/09 15:15:14  baravind
 * Incorporated codereview comments - Aravind.B on 9-Dev-2003
 *
 * Revision 1.8  2003/12/04 03:52:56  vijaige
 * after porting for windows
 *
 * Revision 1.7  2003/12/01 05:08:55  baravind
 * Added by Aravind.B on Dec-1-2003
 *
 * Revision 1.5  2003/11/17 11:17:36  vijaige
 * 17th November
 *
 * Revision 1.3  2003/11/11 06:18:43  baravind
 * Added by Aravind.B on 11-NOV-2003
 *
 * Revision 1.1  2003/11/06 09:08:39  baravind
 * Added by Aravind Balan on 6-Nov-2003
 *
 * Revision 1.1  2003/11/03 08:35:57  baravind
 *  checked in by Aravind.B on Nov-3-2003 
 *
 * Revision 1.1.1.2  2003/10/17 06:41:51  hari
 * no message
 *
 * Revision 1.1.1.1  2003/10/01 05:23:22  thiagav
 * no message
 *
 * $State: Exp $
 * $Id: NASCenter_FileTrans_Send_Util.c,v 1.23 2004/01/14 06:05:56 baravind Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/


/********************************* includes *********************************/

#ifdef WIN32
#include <Winsock2.h>
#include <Iphlpapi.h>
#include <windowsx.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <NASCenter_Ping.h>
#endif

#include <NASCenter_FileTrans_Util.h>
#include <NASCenter_FileTrans.h>





/****************************** local macros ********************************
 *
 * (Macros shareable with rest of the modules are present in .h file)
 */

#define ICMP_WAIT_TIME 10  /* 10 sec */ 
#define DEF_ICMP_DATA_LEN  56
#define ICMP_HDR_LEN  8

/********************* local structure/union definitions ********************/



/********************* prototypes for local functions ***********************/

 I32 FileTrans_Checksum ( UI16 * pusPacket , I32 iPktLen);

 ENASCenterError FileTrans_process_pkt (UI8 * pcRecvPkt , I32 iRecvMsgLen ,
                                        I32 iIdentifier , double * pdRtt,
                                        struct timeval * pstTimeValue);


ENASCenterError FileTrans_RecvAck ( I32 iSocketFd ,
                                    stPacketHeader * pvHeader , I32 iMapNo ,
                                    I32 iCorrelarter , void * pvData ,
                                    I32 iDataLen ,
                                    struct timeval *pstTimeOutValue ,
                                    struct sockaddr_in * pstFromAddr ,
                                    I32 iSocketAddrLen );



ENASCenterError FileTrans_SendPkt ( I32 iSocketFd , void * pvHeader ,
                                    void * pvData , I32 iDataLen ,
                                    struct sockaddr_in * pstDestAddr ,
                                    I32 iSocketAddrLen);

ENASCenterError FileTrans_SendActualData ( I32 iSocketFd ,
                                           void * pvScratchForFile ,
                                           UI8 *piPktStatus , I32 iMmapCntr ,
                                           stSequence * pstSendSequence ,
                                           I32 iMsgSize , I32 iNoPktsPerMMap ,
                                           I32 iLastPacketSize ,
                                           I32 iTotNoOfPkts  , 
                                           I32 iLastPktNo ,
                                           struct sockaddr_in *pstSockAddr);



/************************* Globals within this module ***********************/



/************************* externs  ***********************/




/*****************************************************************************
 *
 * name             :   FileTrans_Checksum
 * description      :   This function calculates Checksum
 * input parameters :   UI16 * pusPacket    Pakcet 
 *                      I32 iPktLen        Length of the packet
 *
 * output parameters:   none
 *
 * return type      : I32    
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

 I32 FileTrans_Checksum ( UI16 * pusPacket , I32 iPktLen)
 {
     I32 inleft = iPktLen;
     UI16 *pusPkt = pusPacket;
     I32 iSum = 0;
     I32 usAnswer = 0;

     while (inleft > 1)  {
     iSum += *pusPkt++;
     inleft -= 2;
     }
     
     /* mop up an odd byte, if necessary */
     if (inleft == 1) {
     *(u_char *)(&usAnswer) = *(u_char *)pusPacket ;
     iSum += usAnswer;
     }
     
     /* add back carry outs from top 16 bits to low 16 bits */
     iSum = (iSum >> 16) + (iSum & 0xffff);/* add hi 16 to low 16 */
     iSum += (iSum >> 16);/* add carry */
     usAnswer = ~iSum;/* truncate to 16 bits */
     return(usAnswer);
}


/*****************************************************************************
 *
 * name             :   FileTrans_process_pkt
 * description      :   This function calculates Time difference 
 * input parameters :   struct timeval * pstOutTime  out time 
 *                      struct timeval * pstInTime    int time
 *
 * output parameters:   struct timeval * pstOutTime time diff
 *
 * return type      : -    
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/
#ifndef WIN32
 ENASCenterError FileTrans_process_pkt (UI8 * pcRecvPkt , I32 iRecvMsgLen ,
                                        I32 iIdentifier , double * pdRtt, 
                                        struct timeval * pstTimeValue)

 {

     struct ip    * pstIpHdr;
     struct icmp  * pstIcmpHdr;
     I32          iOptionLength;
     struct timeval * stTimesent;
     ENASCenterError  eStatus = NASC_SUCCESS;
  
     pstIpHdr = (struct ip *) pcRecvPkt;

     iOptionLength  = pstIpHdr->ip_hl  << 2;

     pstIcmpHdr = (struct icmp *) (pcRecvPkt +  iOptionLength);

     if ( pstIcmpHdr->icmp_type != ICMP_ECHOREPLY)
     {
         printf("%d\n" , pstIcmpHdr->icmp_type);
         eStatus = NASC_FAILURE;
     }
     else
     {
         if (pstIcmpHdr->icmp_id != iIdentifier)
         {
             eStatus  = NASC_FILE_TRANS_IGNORE_ERROR;
         } 
         else
         {
             stTimesent = (struct timeval *) pstIcmpHdr->icmp_data;
             FileTrans_GetTimeDiff(pstTimeValue , stTimesent);
             *pdRtt = pstTimeValue->tv_sec * 1000.0 + 
                      pstTimeValue->tv_usec / 1000.0;
         }
         
     }
 
     return eStatus ;

}
#endif

/*****************************************************************************
 *
 * name             :   getRTT
 * description      :   This function calculates rount trip time between
 *                      source and specified destination 
 * input parameters :   UI8 pcDestAddr  - destination address
 *
 * output parameters:   double * pdRtt  - Round trip time
 *
 * return type      :   NASC_SUCCESS - if sucess
 *                      FAILURE - if failed 
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

ENASCenterError FileTrans_GetRTT(struct sockaddr_in *  pstDestAddr , 
                                 double * pdRtt)
{
#ifdef WIN32

    UI32 value=0;
    struct _timeb ststarttime;
    struct _timeb stendtime;    
    WSADATA wsaData;
    HINSTANCE hIcmp;
    pfnHV pIcmpCreateFile;
    pfnBH pIcmpCloseHandle;
    pfnDHDPWPipPDD pIcmpSendEcho;
    HANDLE hIP;
    char acPingBuffer[64];
    PIP_ECHO_REPLY pIpe;
    DWORD dwStatus;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        return NASC_SOCK_CREATION_FAILED;
    }
    hIcmp = LoadLibrary("ICMP.DLL");
    if (hIcmp == 0) 
    {
        WSACleanup();
        return NASC_FAILURE;
    }
// Get handles to the functions inside ICMP.DLL that we'll need
    pIcmpCreateFile = (pfnHV)GetProcAddress(hIcmp,
            "IcmpCreateFile");
    pIcmpCloseHandle = (pfnBH)GetProcAddress(hIcmp,
            "IcmpCloseHandle");
    pIcmpSendEcho = (pfnDHDPWPipPDD)GetProcAddress(hIcmp,
            "IcmpSendEcho");
    if ((pIcmpCreateFile == 0) || (pIcmpCloseHandle == 0) || 
            (pIcmpSendEcho == 0)) 
{        
WSACleanup();
        return NASC_FAILURE;
    }

// Open the ping service
    hIP = pIcmpCreateFile();
    if (hIP == INVALID_HANDLE_VALUE) 
{
FreeLibrary(hIcmp);
        WSACleanup();
        return NASC_FAILURE;
    }

// Build ping packet

    memset(acPingBuffer, '\xAA', sizeof(acPingBuffer));
    pIpe = (PIP_ECHO_REPLY)GlobalAlloc(
            GMEM_FIXED | GMEM_ZEROINIT,
            sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer));
    if (pIpe == 0) 
{   
CloseHandle(hIP);
GlobalFree(pIpe);
FreeLibrary(hIcmp);
WSACleanup();
        return NASC_FAILURE;
    }
pIpe->Data = acPingBuffer;
    pIpe->DataSize = sizeof(acPingBuffer);      	
_ftime( &ststarttime);

dwStatus = pIcmpSendEcho(hIP, inet_addr(inet_ntoa(pstDestAddr->sin_addr)), 
            acPingBuffer, sizeof(acPingBuffer), NULL, pIpe, 
            sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer), 5000);


    if (dwStatus != 0) 
{
_ftime(&stendtime);		
FileTrans_GetTimeDiff(&stendtime,&ststarttime);
*pdRtt = stendtime.time * 1000.0 + 
                      stendtime.millitm / 1000.0;
}
else 
{
CloseHandle(hIP);
GlobalFree(pIpe);
FreeLibrary(hIcmp);
WSACleanup();
return NASC_FAILURE;
    }
CloseHandle(hIP);
GlobalFree(pIpe);
FreeLibrary(hIcmp);	
WSACleanup();	
return NASC_SUCCESS;

#else
    I32 iSocketfd = 0;
    I32 iIdentifier = 0;
    I32 iSentMsgLen = 0;
    I32 iRecvMsgLen = 0;
    size_t iSockAddrLen = 0;
    struct icmp * pstIcmpHdr = NULL;
    struct timeval stTimeValue;
    struct timeval stCurTimeValue;
    struct timeval stTimeOutValue;
    struct sockaddr_in stFromaddr;    
    BOOL eTimeOutReached = FALSE;
    BOOL eReplyGot       = FALSE;
    ENASCenterError eResult = NASC_SUCCESS;
    UI8 cSendPkt[MAX_PKT_SIZE];
    UI8 cRecvPkt[MAX_PKT_SIZE];
    UI8 * pcIpaddress = NULL;
    I32 iICMPpktSize =  DEF_ICMP_DATA_LEN + ICMP_HDR_LEN;
    fd_set iFileDescSet;
    I32  iSelectRetValue;
    UI8 *pcDestAddress = NULL;
   
    /* Get process id to be used as identifier in the ICMP header */

    iIdentifier = getpid() & 0xffff;

    /* Create Raw socket */

    iSocketfd = socket(AF_INET , SOCK_RAW , IPPROTO_ICMP);

    if ( iSocketfd == -1 )
    {
       /* add syslog */
      return NASC_SOCK_CREATION_FAILED;
    }

    /* memset character array */
    memset(cSendPkt , '\0' , MAX_PKT_SIZE);
    memset(cRecvPkt , '\0' , MAX_PKT_SIZE);

    /* Fill in ICMP details */

    pstIcmpHdr = (struct icmp *) cSendPkt;
    pstIcmpHdr->icmp_type = ICMP_ECHO;  
    pstIcmpHdr->icmp_id = iIdentifier;
    pstIcmpHdr->icmp_seq = 1;
    pstIcmpHdr->icmp_cksum = 0;
    pstIcmpHdr->icmp_code = 0;

    /* store the timestamp in data portion of ICMP pkt. This is needed
       for rtt calculation on recipt of ECHO_RESPONSE */

    gettimeofday(&stTimeValue , NULL);

    memcpy(pstIcmpHdr->icmp_data , &stTimeValue , sizeof(stTimeValue));

    /* Calculate ICMP checksum */

    pstIcmpHdr->icmp_cksum = FileTrans_Checksum((UI16 *) pstIcmpHdr , 
                                                iICMPpktSize);


    /* send ICMP tacket */

    iSentMsgLen =  sendto(iSocketfd , cSendPkt , iICMPpktSize ,0 ,
               (struct sockaddr *) pstDestAddr , sizeof(struct sockaddr)); 
   

    /* Set time out value for select */

    stTimeOutValue.tv_sec = ICMP_WAIT_TIME;
    stTimeOutValue.tv_usec = 0;

    /* set file discriptors to be checked for data */

    FD_ZERO(&iFileDescSet);

    FD_SET(iSocketfd , &iFileDescSet);

    /* set time before which response should be got */


    while(eTimeOutReached == FALSE || eReplyGot == FALSE )
    {
        iSelectRetValue = select(iSocketfd + 1 ,&iFileDescSet , NULL ,
                                 NULL , &stTimeOutValue);

        if ( iSelectRetValue == 0)
        {
            eTimeOutReached = TRUE;
            eResult = NASC_FILE_TRANS_TIME_OUT;
        }
        else if ( iSelectRetValue < 0 ) 
        {
            eResult = NASC_FAILURE;
            break;
        }  
        else
        {
            if ( (iRecvMsgLen = 
                 recvfrom(iSocketfd , cRecvPkt , MAX_PKT_SIZE ,  0,
                 (struct sockaddr *) &stFromaddr , &iSockAddrLen )) < 0 )
            {
#ifndef WIN32
                if ( errno == EINTR )
                {
                  continue;
                }
#endif
            }
            else
            {
                gettimeofday(&stCurTimeValue , NULL);

                pcIpaddress =  inet_ntoa(stFromaddr.sin_addr);
                pcDestAddress =  inet_ntoa(pstDestAddr->sin_addr);
                if ( pcIpaddress != NULL && pcDestAddress != NULL ) 
                {
                    if ( strcmp(pcIpaddress ,  pcDestAddress) != 0)
                    {
                        continue;
                    }
                }
                eResult = FileTrans_process_pkt(cRecvPkt , iRecvMsgLen , 
                                                iIdentifier , 
                                                pdRtt, &stCurTimeValue);
                if ( eResult != NASC_SUCCESS || 
                     eResult != NASC_FILE_TRANS_IGNORE_ERROR)
                {
                  break; 
                }
                if ( eResult == NASC_SUCCESS )
                {
                    eReplyGot = TRUE;
                }
            } 
        }
    }        
    /* Close the RAW socket */
    #ifdef WIN32
        closesocket(iSocketfd);
    #else
       close(iSocketfd);
    #endif
    return eResult;
#endif
}

/*****************************************************************************
 *
 * name             :   FileTransfer_InitHandshake
 * description      :   This function gets server's free port for actual 
 *                      file transfer
 *
 * input parameters :   I32 iSocketFd  socket discriptor
 *                      struct sockaddr_in * pstDestAddr Server address and 
 *                                                       well-known prot
 * output parameters:   struct sockaddr_in * pstDestPortDetail Server's free
 *                                                             port
 *
 * return type      :   NASC_SUCCESS - if sucess
 *                      FAILURE - if failed 
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

ENASCenterError FileTransfer_InitHandshake( I32 iSocketFd , 
                                        struct sockaddr_in * pstDestAddr ,
                                        struct sockaddr_in * pstDestPortDetail ,
                                        double dRtt  , UI8 * pcFileName , 
                                        UI8 * pcDestPath , I32 iTotNoOfPkts , 
                                        I32 iNoPktsPerMMap ,
                                        I32 iExtraPktsPerMap ,  
                                        I32 iLastPktSize , I32 iMsgSize  , 
                                        I32 iNoOfMappingNeeded)
{
    stPacketHeader stPktHdr = { 0 } ;
    stPacketHeader stRecvPktHdr =  { 0 } ;
    ENASCenterError eReturnValue = NASC_SUCCESS;
    fd_set iSocketFDList;
    I32 iNoOfRetries = 0;
    stCntrlData    stData  = { 0 } ;
    stSequence stSendSequence = { 0 } ;
    struct timeval stTimeOutValue = { 0 };


    memset(&stData , '\0' , sizeof(stCntrlData));

    /* init sequencing */

    stSendSequence.iMmapNo = 0;
    stSendSequence.iCorrelater = 1;

    /* Fill up packet header */
 
    stPktHdr.ePktType = htonl(HND_SHAKE);
    stPktHdr.iPktNo = htonl(1);
    stPktHdr.iMmapNo = htonl(stSendSequence.iMmapNo);
    stPktHdr.iCorrelater = htonl(stSendSequence.iCorrelater);

    /* Fill up data */

    if ( strlen(pcFileName) > MAX_FILE_NAME_LEN)
    {
        /*syslog */
        return NASC_FAILURE;
    } 

    memcpy(stData.pcFileName , pcFileName , strlen(pcFileName) * sizeof(char));

    stData.dRtt = htonl(dRtt);
    stData.iTotNoPkts = htonl(iTotNoOfPkts);

    if (pcDestPath != '\0')
    {
        if ( strlen ( pcDestPath ) > MAX_PATH_LEN)
        {   
            /*syslog */
            return NASC_FAILURE;
        }

        memcpy ( stData.pcDestPath , pcDestPath ,  
                 strlen ( pcDestPath ) * sizeof (char));
    }

    stData.iNoPktsPerMMap = htonl(iNoPktsPerMMap);
    stData.iNoExtraMapPackets = htonl(iExtraPktsPerMap);
    stData.iMesgSizePerPkt = htonl(iMsgSize);
    stData.iMesgSizeForLastPkt = htonl(iLastPktSize);
    stData.iTotNoMaps = htonl(iNoOfMappingNeeded);
 
    /* set file discriptors to be checked for data */

    FD_SET(iSocketFd , &iSocketFDList);

    while ( 1 )
    {
        if ( iNoOfRetries > MAX_NO_OF_RETRIES ) 
        { 
            /* sysLog */
            eReturnValue = NASC_FAILURE;
            break;
         }

        stTimeOutValue.tv_sec = ( long ) (dRtt / 1000.0);
        stTimeOutValue.tv_usec = (( dRtt * 1000 ) - stTimeOutValue.tv_sec); 
        stTimeOutValue.tv_sec += DELTA_TIME_FOR_RECV_ACK;

        /* send Handshake Packet */
        

        eReturnValue = FileTrans_SendPkt ( iSocketFd , (void *) &stPktHdr , 
                                           &stData , sizeof(stData) , 
                                           pstDestAddr , 
                                           sizeof(struct sockaddr_in ));

        if ( eReturnValue != NASC_SUCCESS )
        {
            /* syslog */
            return eReturnValue;
        } 

        iNoOfRetries++;        

        eReturnValue =  FileTrans_RecvAck ( iSocketFd ,
                                            &stRecvPktHdr , 
                                            stSendSequence.iMmapNo , 
                                            stSendSequence.iCorrelater , 
                                            NULL , 0 ,
                                            &stTimeOutValue , 
                                            pstDestPortDetail ,
                                            sizeof(struct sockaddr_in ));

        if ( eReturnValue != NASC_SUCCESS)
        {
            if ( eReturnValue == NASC_FILE_TRANS_TIME_OUT)
            {
                iNoOfRetries ++ ;
                if ( iNoOfRetries > MAX_NO_OF_RETRIES )
                {
                    /* syslog */
                    eReturnValue = NASC_FAILURE;
                }
                continue ;
            }
            /* syslog */
            eReturnValue = NASC_FAILURE ;
        }
        else 
        {
            if ( stRecvPktHdr.ePktType == HND_SHAKE_ACK )
            {
                 eReturnValue = NASC_SUCCESS;
                 break;
            }
        }
    }
    return eReturnValue;
}

/*****************************************************************************
 *
 * name             :   FileTrans_SendPkt
 * description      :   This function sends the packet to destination address.
 *                      If iSocketFd is unconnect UDP , pstDestAddr , 
 *                      iSocketAddrLen  should be specified . If iSocketFd is 
 *                      connected UDP socket , pstDestAddr should be NULL.
 *
 * input parameters :   I32 iSocketFd  Socket descriptor 
 *                      void * pvHeader packet header 
 *                      void * pvData   data
 *                      I32 iDataLen    length of data 
 *                      struct sockaddr_in * pstDestAddr  destination address
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

ENASCenterError FileTrans_SendPkt ( I32 iSocketFd , void * pvHeader , 
                                    void * pvData , I32 iDataLen , 
                                    struct sockaddr_in * pstDestAddr , 
                                    I32 iSocketAddrLen)
{
    I32 iRetVal;
    ENASCenterError eRetVal = NASC_SUCCESS;
    StMsg stmessage;

    /* sprintf(clen , "%d" , iDataLen);

    iPkLen = sizeof(stPacketHeader)+sizeof(I32)+iDataLen;

    pcPktData = (UI8 * )malloc((iPkLen + 1) * sizeof(UI8));

    if(pcPktData == NULL)
    {
        return NASC_INSUFFICIENT_MEMORY;   
    } 

    memset(pcPktData , '\0' , (iPkLen + 1) * sizeof(UI8));

    pcPktStart = pcPktData;

    memcpy(pcPktStart,pvHeader,sizeof(stPacketHeader));
    pcPktStart = pcPktStart + sizeof(stPacketHeader);


    memcpy(pcPktStart,clen,sizeof(I32));

    pcPktStart = pcPktStart + sizeof(I32);

    memcpy(pcPktStart,pvData,iDataLen);	
 */
    memset(&stmessage , '\0' , sizeof(StMsg));
    memcpy(&stmessage.stHeader , pvHeader , sizeof(stPacketHeader)); 
    stmessage.iDataLen = htonl(iDataLen);
    memcpy(stmessage.cData , pvData , iDataLen); 

    if ( pstDestAddr != NULL)
    {

        iRetVal = sendto(iSocketFd,&stmessage,sizeof(stmessage),0,
                         (struct sockaddr *)pstDestAddr,iSocketAddrLen);
    }
    else
    {
        iRetVal = sendto(iSocketFd,&stmessage,sizeof(stmessage),0,NULL,0);
    }

    if ( iRetVal == -1 )
    {
        /* syslog */
        eRetVal = NASC_FAILURE;
    } 

    // FREE_PTR(pcPktData);

    return eRetVal;


}

/*****************************************************************************
 *
 * name             :   FileTrans_RecvAck
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
ENASCenterError FileTrans_RecvAck ( I32 iSocketFd ,  
                                    stPacketHeader * pvHeader , I32 iMapNo , 
                                    I32 iCorrelarter , void * pvData ,
                                    I32 iDataLen , 
                                    struct timeval *pstTimeOutValue ,
                                    struct sockaddr_in * pstFromAddr ,
                                    I32 iSocketAddrLen ) 
                                    
{
    I32 iRetVal;
    fd_set iSocketFDList;
    ENASCenterError eResult = NASC_SUCCESS;
    stPacketHeader stPktHdrInNetForm;
    StMsg pcPktData;

    /* pcPktData=(UI8 *) malloc((MAX_PKT_SIZE  + 1)* sizeof(UI8));

    if(pcPktData==NULL)
        return NASC_INSUFFICIENT_MEMORY;

     memset(pcPktData , '\0' , (MAX_PKT_SIZE  + 1)* sizeof(UI8)); */

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
            /* syslog() */
            eResult = NASC_FAILURE;
            break;
        }
        if (pstFromAddr != NULL)
        {
            iRetVal= recvfrom(iSocketFd,&pcPktData,sizeof(StMsg),0,
                              (struct sockaddr *)pstFromAddr,
                              (size_t *) &iSocketAddrLen);
        }
        else
        {
            iRetVal = recvfrom(iSocketFd , &pcPktData , sizeof(StMsg) , 0 ,
                               NULL , 0);
        }
        if(iRetVal < 0)
        {
#ifndef WIN32
            if ( errno == EINTR)
            {
                continue;
            }
#endif
            perror("FAILED : .. ");
            eResult = NASC_FAILURE;
            break;
        }
        if ( iRetVal == 0 ) 
        {
            eResult = NASC_FILE_TRANS_CONN_CLOSED;
            break;
        }
        /* pcPktStart=pcPktData;

        memcpy(&stPktHdrInNetForm,pcPktStart,sizeof(stPacketHeader));

        FileTrans_ConvertPktHdrToHostForm(&stPktHdrInNetForm , pvHeader);

        pcPktStart = pcPktStart + sizeof(stPacketHeader);

        memcpy(clen,pcPktStart,sizeof(I32));

        iDataLen=atoi(clen);

        pcPktStart = pcPktStart + sizeof(I32);

        memcpy(pvData,pcPktStart,iDataLen); */
        memcpy(&stPktHdrInNetForm,&pcPktData.stHeader,sizeof(stPacketHeader));
        FileTrans_ConvertPktHdrToHostForm(&stPktHdrInNetForm , pvHeader);
        if ( pvData != NULL )
        {
            iDataLen = ntohl(pcPktData.iDataLen);
            memcpy(pvData,pcPktData.cData,iDataLen); 
        }


        if ( pvHeader->iMmapNo == iMapNo && 
             pvHeader->iCorrelater ==  iCorrelarter )
        {
           eResult = NASC_SUCCESS;
           break;
        }
        else 
        {
            continue;
        }
    }

   //  FREE_PTR(pcPktData);

    return eResult;
}

/*****************************************************************************
 *
 * name             :   FileTransfer_SendData
 * description      :   This function sends the file packets
 *
 * input parameters :   I32 iSocketFd  Socket descriptor 
 *                      UI8 * pcFileName  name of file to be transfered
 *                      I32 iTotNoOfPkts No of pkts for file transfer
 *
 * output parameters:   none
 *
 * return type      :   NASC_SUCCESS - if sucess
 *                      NASC_FAILURE - if failed 
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/
#ifdef WIN32
 ENASCenterError  FileTransfer_SendData (I32 iSocketFd , UI8 * pcFileName ,
                                        HANDLE FileHandle , I32 iFileSize ,
                                        I32 iTotNoOfPkts ,
                                        I32 iPageSize , I32 iNoOfMappingNeeded ,
                                        I32 iNoPktsPerMMap , I32 iMsgSize ,
                                        double dRtt  , I32 iLastPktSize, 
                                        I32 iExtraPktForLastMap , 
                                        struct sockaddr_in *pstSockAddr)
#else
 ENASCenterError  FileTransfer_SendData (I32 iSocketFd , UI8 * pcFileName ,
                                        I32 iFileDescriptor , I32 iFileSize ,
                                        I32 iTotNoOfPkts ,
                                        I32 iPageSize , I32 iNoOfMappingNeeded ,
                                        I32 iNoPktsPerMMap , I32 iMsgSize ,
                                        double dRtt  , I32 iLastPktSize , 
                                        I32 iExtraPktForLastMap , 
                                        struct sockaddr_in *pstSockAddr)
#endif

{
    stPacketHeader stRecvPktHdr;
    ENASCenterError eReturnValue = NASC_SUCCESS;
    I32 iNoOfRetries = 0;

    #ifdef WIN32
        HANDLE MemHandle;
        DWORD offset=0;
        SYSTEM_INFO stsysInfo;
        I32 iWinMapCntr =0;
        I32 iallocgran;
        void * pvWinScratchForFile =NULL;
    #endif

    void * pvScratchForFile = NULL;

    I32  iLastPacketSize = 0;
    I32 iMmapCntr = 0;
    UI8 *piPktStatus = NULL;
    stSequence stSendSequence ;
    struct timeval stTimeOutValue;
    I32 iLastPktNo = 0;


    iLastPacketSize = iLastPktSize;
    stSendSequence.iMmapNo = 1;
    stSendSequence.iCorrelater = 1;

    #ifdef WIN32
        /*Create a memory mapped file for writing the log information */			
        GetSystemInfo(&stsysInfo);
        iallocgran = stsysInfo.dwAllocationGranularity;
        
        if(!(MemHandle=CreateFileMapping(FileHandle,NULL,
                                         PAGE_READONLY,
                                         0,
                                         0,
                                         NULL)))
        {
            return NASC_FAILURE;
        }
        if(iallocgran > iFileSize)
        {
            iallocgran=iFileSize;
        }
        pvScratchForFile=MapViewOfFile(MemHandle,FILE_MAP_READ,0,0,iallocgran);
                                       iWinMapCntr = 1;
        if( pvScratchForFile == NULL)
        {
            CloseHandle(MemHandle);
            return NASC_FAILURE;
        }
        pvWinScratchForFile = pvScratchForFile;
            
    #else
         pvScratchForFile = mmap ( (void *) 0 , iPageSize , 
                                    PROT_WRITE|PROT_READ , MAP_SHARED , 
                                    iFileDescriptor , 0);


         if ( pvScratchForFile == MAP_FAILED )
         {
             /* sysLog */    
             return NASC_FAILURE;
         } 
    #endif


    /* Allocate memory for PktStatus */

    piPktStatus = (UI8 *) malloc ( (iNoPktsPerMMap + 1) * sizeof ( UI8));

    if ( piPktStatus == NULL)
    {
        /*syslog */
    #ifdef WIN32
        UnmapViewOfFile(pvWinScratchForFile);
        CloseHandle(MemHandle);
    #else
        munmap ( pvScratchForFile , iPageSize);
								pvScratchForFile = NULL;
    #endif

        return NASC_INSUFFICIENT_MEMORY;
    } 

    memset ( piPktStatus , '0' , (iNoPktsPerMMap + 1) * sizeof ( UI8));

 

    while ( iMmapCntr < iNoOfMappingNeeded )  
    {
            if ( iNoOfRetries > MAX_NO_OF_RETRIES)
            {
                /* syslog */
                eReturnValue = NASC_FAILURE;
                break;
            }

            eReturnValue  = FileTrans_SendActualData( iSocketFd , 
                                               pvScratchForFile , 
                                               piPktStatus , 
                                               iMmapCntr , 
                                               (stSequence *) &stSendSequence , 
                                               iMsgSize , 
                                               iNoPktsPerMMap , 
                                               iLastPacketSize ,
                                               iTotNoOfPkts  , 
                                               iLastPktNo ,
                                               pstSockAddr);

            if ( eReturnValue != NASC_SUCCESS )
            {
                /* sysLog */
                break;
            }       
            /* calculate wait time for packet */

            stTimeOutValue.tv_sec = ( long ) (dRtt / 1000.0); 

            stTimeOutValue.tv_usec = (( dRtt * 1000 ) - stTimeOutValue.tv_sec); 

            stTimeOutValue.tv_sec += DELTA_TIME_FOR_RECV_ACK;
                                     

        eReturnValue =  FileTrans_RecvAck ( iSocketFd ,
                                            &stRecvPktHdr , 
                                            stSendSequence.iMmapNo ,
                                            stSendSequence.iCorrelater , 
                                            piPktStatus , iNoPktsPerMMap ,
                                            &stTimeOutValue , pstSockAddr ,
                                            sizeof(struct sockaddr_in));

        if ( eReturnValue != NASC_SUCCESS )
        {
            if ( eReturnValue == NASC_FILE_TRANS_TIME_OUT)
            {
                iNoOfRetries ++ ;
                continue;
            }
            /* syslog */
            break;
        }
        else
        {
            if ( stRecvPktHdr.ePktType == DATA_ACK )
            {
                iLastPktNo += iNoPktsPerMMap;
                #ifndef WIN32
																				if ( pvScratchForFile != NULL )
																				{
                        munmap ( pvScratchForFile , iPageSize);
																								pvScratchForFile = NULL;
                    }
                #endif
                iMmapCntr ++;    
                // printf("MAP %d Finished \n" , iMmapCntr);
                if ( iMmapCntr >= iNoOfMappingNeeded ) 
                {
                     // printf("Finished packts \n");
                    break;
                }
                memset ( piPktStatus , '0' , 
                         (iNoPktsPerMMap + 1) * sizeof ( UI8));
                if ( iMmapCntr + 1  ==  iNoOfMappingNeeded && 
                     iExtraPktForLastMap != 0  )
                {
                    // printf("LAst map.. \n");
                    iNoPktsPerMMap = iExtraPktForLastMap ;                 
                }
                #ifdef WIN32
                if((iMmapCntr * iPageSize)>= (iWinMapCntr * iallocgran))
                {
                    UnmapViewOfFile(pvWinScratchForFile);
                    offset = iWinMapCntr * iallocgran;
                    pvScratchForFile = MapViewOfFile(MemHandle,FILE_MAP_READ,
                                                     0,
                                                     offset,
                                                     iallocgran);
                    if((iFileSize - offset)< iallocgran)
                      pvScratchForFile = MapViewOfFile(MemHandle,FILE_MAP_READ,
                                                       0,
                                                       offset,
                                                       (iFileSize - offset));
                      iWinMapCntr ++;
                      if( pvScratchForFile == NULL)
                      {
                          eReturnValue =  NASC_FAILURE;
                          break;
                      }
                      pvWinScratchForFile = pvScratchForFile;
        }
        else
        pvScratchForFile = (char *)pvScratchForFile + iPageSize;


#else
                pvScratchForFile = mmap ( (void *) 0 , iPageSize , 
                                          PROT_WRITE|PROT_READ , MAP_SHARED , 
                                          iFileDescriptor , 
                                          (iMmapCntr * iPageSize));
                if ( pvScratchForFile == MAP_FAILED )
                {
                    /* sysLog */    
                   eReturnValue =  NASC_FAILURE;
                   break;
                } 
#endif
                stSendSequence.iMmapNo++ ;
                stSendSequence.iCorrelater = 1;
                iNoOfRetries = 0;
            }

            else if (stRecvPktHdr.ePktType == DATA_NEG_ACK )
            {
                stSendSequence.iCorrelater++;
                iNoOfRetries ++;
                continue ;
            }
        }
    }

#ifdef WIN32
    UnmapViewOfFile(pvWinScratchForFile);
    CloseHandle(MemHandle);
#else 
								if ( pvScratchForFile != NULL)
								{
            munmap ( pvScratchForFile , iPageSize);
												pvScratchForFile = NULL;
        }
#endif
    FREE_PTR(piPktStatus);
    return eReturnValue;
}

/*****************************************************************************
 *
 * name             :   FileTrans_SendActualData
 * description      :   This function sends packets to other end of 
 *                      communication
 *
 * input parameters :   I32 iSocketFd  Socket descriptor 
 *                      void * pvScratchForFile  - starting addr of Memory  i
 *                      where data  from file is stored
 *                      I32 iPktStatus Status of pkts previously sent .
 *                      I32 iMmapCntr  Memory map no being processed
 *                      stPacketHeader  * stPktHdr  Pkt header
 *                      I32 iMsgSize    Message size
 *                      I32 iNoPktsPerMMap  No Of pkts need for the map 
 *                      I32 iLastPacketSize  size of Last pkt
 *                      I32 iTotNoOfPkts   Total no of pkts for the file
 *
 * output parameters:   none
 *
 * return type      :   NASC_SUCCESS - if sucess
 *                      NASC_FAILURE - if failed 
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/

ENASCenterError FileTrans_SendActualData ( I32 iSocketFd , 
                                           void * pvScratchForFile , 
                                           UI8 *piPktStatus , I32 iMmapCntr , 
                                           stSequence * pstSendSequence ,
                                           I32 iMsgSize , I32 iNoPktsPerMMap , 
                                           I32 iLastPacketSize ,
                                           I32 iTotNoOfPkts ,  
                                           I32 iLastPktNo ,
                                           struct sockaddr_in *pstSockAddr)
{ 
    void * pvTempScratchFile = NULL;
    I32 iPktNo = 1;
    UI8 * pcData = NULL;
    ENASCenterError eReturnValue = NASC_SUCCESS;
    stPacketHeader stPktHdr;
    I32 iLenOfDataCopied = 0 ;

    memset ( &stPktHdr , '\0' , sizeof(stPacketHeader));
    

    pvTempScratchFile = pvScratchForFile;

    pcData = (UI8 *) malloc ( ( iMsgSize + 1 ) * sizeof ( UI8 ));

    if ( pcData == NULL )
    {
        /* syslog */
        return NASC_INSUFFICIENT_MEMORY;
    }

    memset( pcData , '\0' , (iMsgSize + 1) * sizeof ( UI8 ));

  

    while ( iPktNo <= iNoPktsPerMMap )
    {
        /* Fill up packet header */

        stPktHdr.ePktType = htonl(DATA);

        stPktHdr.iMmapNo = htonl(pstSendSequence->iMmapNo);

        stPktHdr.iCorrelater = htonl(pstSendSequence->iCorrelater);

        pvTempScratchFile = (char *)pvTempScratchFile + iLenOfDataCopied ;

        iLastPktNo ++;

        memset( pcData , '\0' , (iMsgSize + 1) * sizeof ( UI8 ));

        if ( iLastPktNo == iTotNoOfPkts  && iLastPacketSize != 0 )
        {
            /* If pkt we are about to send is last packet no , and 
            if left over data size is not zero , then the packet 
            is not multiple of page data  size . So copy the extra data 
            size */ 
            iLenOfDataCopied = iLastPacketSize;
        }
        else 
        {
            iLenOfDataCopied = iMsgSize;
        }
        if ( piPktStatus[iPktNo - 1]  == '0')
        {
            stPktHdr.iPktNo = htonl(iLastPktNo);
            memcpy ( pcData , pvTempScratchFile , iLenOfDataCopied );
            eReturnValue = FileTrans_SendPkt ( iSocketFd , (void *) &stPktHdr ,
                                       pcData , iLenOfDataCopied , 
                                       pstSockAddr  ,
                                       sizeof(struct sockaddr_in));
             if ( eReturnValue != NASC_SUCCESS )
             {
                 /* sysLog */
                 break;
              }
          }
              iPktNo++;
    }

    if ( pcData != NULL )
    {
        free ( pcData );
    }
    return eReturnValue;
}

/*****************************************************************************
 *
 * name             :   FileTrans_GetTimeDiff
 * description      :   This function calculates Time difference 
 * input parameters :   struct timeval * pstOutTime  out time 
 *                      struct timeval * pstInTime    int time
 *
 * output parameters:   struct timeval * pstOutTime time diff
 *
 * return type      : -    
 *
 * global variables :   [which are used in the function]
 * calls            :   [ Names of function which it calls ]
 ****************************************************************************/
#ifdef WIN32
void FileTrans_GetTimeDiff ( struct _timeb  * pstOutTime , 
                              struct _timeb  * pstInTime)
{
    if((pstOutTime->millitm -= pstInTime->millitm) < 0)
    {
        -- pstOutTime->time;
        pstOutTime->time += 1000000;
    }

   pstOutTime->time -= pstInTime->time;
}

#else
 void FileTrans_GetTimeDiff ( struct timeval * pstOutTime , 
                              struct timeval * pstInTime)
{
    if((pstOutTime->tv_usec -= pstInTime->tv_usec) < 0)
    {
        -- pstOutTime->tv_sec;
        pstOutTime->tv_usec += 1000000;
    }

   pstOutTime->tv_sec -= pstInTime->tv_sec;
}
#endif


void FileTrans_ConvertPktHdrToHostForm(stPacketHeader *pstPktHdrInNetForm , 
                                  stPacketHeader *pvHeader)
{

    pvHeader->ePktType = ntohl(pstPktHdrInNetForm->ePktType);
    pvHeader->eModuleId = ntohl(pstPktHdrInNetForm->eModuleId);
    pvHeader->iPktNo = ntohl(pstPktHdrInNetForm->iPktNo);
    pvHeader->iMmapNo = ntohl(pstPktHdrInNetForm->iMmapNo);
    pvHeader->iCorrelater = ntohl(pstPktHdrInNetForm->iCorrelater);
}
  
    

/* End of NASCenter_FileTrans_util.c */


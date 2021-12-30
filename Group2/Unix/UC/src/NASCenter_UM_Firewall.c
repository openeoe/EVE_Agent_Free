/*****************************************************************************
 *
 * NASCenter_UM_firewall.c
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Thiagarajan V.
 *
 * description :
 *
 * TODO:
 *
 *
 *
 *  CM Details:
 *
 * $Author: baravind $
 * $Log: NASCenter_UM_Firewall.c,v $
 * Revision 1.21  2004/01/16 06:25:35  baravind
 * Updates for Hp-UNIX.
 *
 * Revision 1.20  2004/01/14 15:19:49  baravind
 * After purify test
 *
 * Revision 1.19  2004/01/05 10:54:34  baravind
 * Added kill -HUP in code
 *
 * Revision 1.17  2004/01/03 08:53:57  baravind
 * Removed printf
 *
 * Revision 1.16  2004/01/03 08:39:26  baravind
 * After  Hp unixi testing .
 *
 * Revision 1.15  2004/01/02 14:17:27  baravind
 * Code for True64
 *
 * Revision 1.14  2004/01/02 12:12:56  baravind
 * *** empty log message ***
 *
 * Revision 1.13  2004/01/01 12:26:23  baravind
 * After Uniware / solaris System testing
 *
 * Revision 1.12  2003/12/31 12:01:55  baravind
 * Fix for Sol/unixware/Hp-unix/true64
 *
 * Revision 1.11  2003/12/24 09:26:51  baravind
 * *** empty log message ***
 *
 * Revision 1.10  2003/12/24 09:23:29  baravind
 * Added by Aravind.B . Fix for solaris
 *
 * Revision 1.9  2003/12/22 10:33:01  baravind
 * Added by Aravind.B . UM for sol , unixware and hpunix
 *
 * Revision 1.11  2003/12/10 10:44:31  thiagav

 *
 * Revision 1.10  2003/12/10 05:09:19  thiagav
 * Added mkfilt genfilt , all filter rule fiunctionalities
 *
 * Revision 1.9  2003/12/09 11:45:04  thiagav
 * *** empty log message ***
 *
 * Revision 1.8  2003/12/09 06:23:56  thiagav
 * Modified with respect to Linux firewall
 *
 * Revision 1.7  2003/12/09 05:34:20  thiagav
 * *** empty log message ***
 *
 * Revision 1.6  2003/11/08 13:27:36  thiagav
 * *** empty log message ***
 *
 * Revision 1.4  2003/11/05 12:52:12  thiagav
 * Modified and updated
 *
 * Revision 1.3  2003/11/05 08:57:17  thiagav
 * Added in to CVS
 *
 * Revision 1.1  2003/11/05 06:37:45  thiagav
 * Compiled Header, comments are added
 *
 *
 * $Id: NASCenter_UM_Firewall.c,v 1.21 2004/01/16 06:25:35 baravind Exp $
 *
 ****************************************************************************/


/********************************* includes *********************************/
#include "NASCenter_UM_Firewall.h"
#include "NASCenter_UM_List.h"
#include "NASCenter_Errors.h"
#include "NASCenter_Errors.h"
#include "allowTelnetTable.h"
#include "allowFTPTable.h" 
#include "allowPOP3Table.h"
#include "allowSMTPTable.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



/****************************** local macros *********************************
*
* (Macros shareable with rest of the modules are present in http_sample.h file)
*/

/********************* local structure/union definitions ********************/

/********************* prototypes for local functions ***********************/
void UM_StartUserMetering( ) ;
void UM_InitCBuffer(SCBuffer *);
void UM_ProcessTrapMsg(void *);
void UM_WriteQueue(SCBuffer *);
void UM_UserMeetering( struct snmp_pdu *);
void UM_Log(UI8 *logMsg);

void UM_InitSession();
I16 UM_DeleteAllRule();
I16 UM_GetTableDetails(I16 );
I16 UM_CreateFilter(stAllowUM *, I16 );
int UM_ListenTrap(int id, struct snmp_session *, int , netsnmp_pdu *,
void *arg);
I16 UM_CheckTable();

BOOL  UM_IsQueueEmpty(SCBuffer *);
BOOL  UM_IsQueue_Full(SCBuffer *);
I16 UM_AddRule(stAllowUM  *,  stList );
I16 UM_ModifyRule(stAllowUM  *,  stList );
I16 UM_DelRule(stAllowUM  *,  stList );
I16 UM_InsertRule(UI8 *cpRuleStr);
I16 UM_CheckFieldTelnet( netsnmp_variable_list *);
I16 UM_CheckFieldFTP( netsnmp_variable_list * );
I16 UM_CheckFieldSMTP( netsnmp_variable_list *);
I16 UM_CheckFieldPOP3( netsnmp_variable_list * );
I16 UM_GetIndex(netsnmp_variable_list *pstCurrVars);
ENASCenterError UM_HandleInetd (void );
I32 UM_GetLine(I16 giFileId , UI8 * pcData , I16 iMaxChar);


#define MAX_REPETITIONS 6 
#define  ALLOW_ALL "255.255.255.255" 


/************************ globals within this module*************************/

pthread_t readThread;
pthread_t writeThread;
SCBuffer stCB;
stNode stListPtr;
struct snmp_session *agentSess;
void *tSess;
FILE *ifdForUM;


I16 UM_DEFAULT_PORT=0;
UI8 UM_DEFAULT_IPADDR[]="0.0.0.0";


FILE *fpForCMD;

UI8  gpcServiceDeamons [4] [80]= {
			       "in.telnetd" ,
			       "in.ftpd"
			       "" ,
			       ""
			       };

#ifdef UNIXWARE
		UI8 * gpcHostAllow = "/etc/inet/hosts.allow";
		UI8 * gpcHostDeny = "/etc/inet/hosts.deny";
#else
		UI8 * gpcHostAllow = "/etc/hosts.allow";
		UI8 * gpcHostDeny = "/etc/hosts.deny";
#endif
		UI8 * gpcInetd    = "/etc/inetd.conf";
		UI8 gpcTCPWrapperPath [200] = { 0 } ;
		UI8 * gpcTcpWrapperd    = "tcpd";


UI8 gcEmptyStr[MAX_CHAR] = {0};

I16 giFileId = 0;


/************************ externs *************************/


I32 UM_GetLine(I16 giFileId , UI8 * pcData , I16 iMaxChar)
{
    char cData;
    I16 iNoOfChar = 0;
    I16 iRetVal = 0;
    UI8 * pcBuff = pcData;

    while ( 1 )
    {
       if ( (iRetVal = read(giFileId , &cData , 1)) == 1 )
       {
           iNoOfChar++;
           if ( cData == '\n')
               break;
           *pcBuff++ = cData; 
           if ( iNoOfChar >= iMaxChar) 
            break;
           
       }
       else if ( iRetVal == 0 )
       {
           if (iNoOfChar == 1)
           {
               return 0;
           }
           else 
           {
               break;
           }
       }
       else
       {
           if ( errno == EINTR)
               continue;
           return -1;
       }

    }

    *pcBuff = '\0';
    return  iNoOfChar;

}
 
       

   
   

/*****************************************************************************
 *
 * name             :   UM_StartUserMetering 
 * description      :   Starting function of User Meetering application 
 *                      This function retrieves the table values  
 *                      Creates a thread for processing the trap message 
 * input parameters :   - 
 * return type      :   - 
 * global variables :   ifdForUM -> File Descriptor for logging the debug log
 *                      information 
 * calls            :   UM_GetTableDetails()
 *                  :   UM_ProcessTrapMsg()
*****************************************************************************/
void UM_StartUserMetering() 
{
   I16 iResult; 	
   iResult=0;

			printf("UM_StartUserMetering..\n");

			/* Open hosts.allow file */

giFileId = open(gpcHostAllow ,  O_CREAT | O_RDWR , S_IREAD | S_IWUSR);

			if ( giFileId == -1 )
			{
							printf("UNABLE TO OPEN FILE..\n");

							return;
			}

   /*Get Telnet table details */
   iResult = UM_GetTableDetails(ALLOW_TELNET);
   if(!iResult)
   {
#ifdef UM_DEBUG	   
       UM_Log("Error in Retrieving Telnet table records \n");	
#endif       
   }
   iResult=0;
   /*Get FTP table details */
   iResult = UM_GetTableDetails(ALLOW_FTP);
   if(!iResult)
   {
#ifdef UM_DEBUG	   
       UM_Log("Error in Retrieving FTP table records \n");	
#endif       
   }

   iResult=0;
   /*Get POP3 table details */
   iResult = UM_GetTableDetails(ALLOW_POP3);
   if(!iResult)
   {
#ifdef UM_DEBUG	   
       UM_Log("Error in Retrieving POP3 table records \n");	
#endif       
   }
   iResult=0;
   /*Get SMTP table details */
   iResult = UM_GetTableDetails(ALLOW_SMTP);
   if(!iResult)
   {
#ifdef UM_DEBUG	   
       UM_Log("Error in Retrieving SMTP table records \n");	
#endif       
   }
   /*Initializt the circular queue*/
   UM_InitCBuffer(&stCB) ;
   /*Create a thread for processing the trap message  */
   pthread_create(&readThread , NULL , (void *) UM_ProcessTrapMsg ,
                 NULL);
}

I16 UM_DeleteAllRule()
{
    return 1;
}
/*****************************************************************************
 *
 * name             :   UM_InitSession 
 * description      :   Initialize the SNMP Session with NASCenter sub agent 
 * input parameters :   - 
 * return type      :   - struct snmp_session * - Poniter to the snmp session 
 * global variables :   -gcAgentSessPeerName  - peer name 
 *                      - giNASSAPort - port number used for opening the snmp
 *                        session
 *                      - gcNASSACommunity - Community name for snmp session
 *                        
 * calls	    :   -UM_ListenTrap  
 *                      -UM_AddMsgToLogFile
 *
*****************************************************************************/

void UM_InitSession()
{
   struct snmp_session tempSess;
   struct snmp_session trapSess;

   /* For Session Initiation */
   snmp_sess_init(&tempSess);
   tempSess.version = SNMP_VERSION_2c;
   tempSess.peername = "localhost";
   //tempSess.local_port = 200;
   /*tempSess.remote_port = giNASSAPort;*/
   tempSess.community = gcNASSACommunity;
   tempSess.community_len = strlen(gcNASSACommunity);
   tempSess.callback_magic = NULL;


   snmp_sess_init(&trapSess);
   trapSess.version = SNMP_VERSION_2c;
   trapSess.peername = "localhost:2505";
   trapSess.local_port = 2505;
   trapSess.community = gcNASSACommunity;
   trapSess.community_len = strlen(gcNASSACommunity);
   trapSess.callback = UM_ListenTrap;
   trapSess.callback_magic = NULL;
 

   SOCK_STARTUP;
   agentSess = snmp_open(&tempSess);
   SOCK_STARTUP;
   tSess = snmp_sess_open(&trapSess);

   if(agentSess == NULL) 
   {
#ifdef UM_DEBUG
       UM_Log("Error opening Session\n");
#endif
       snmp_perror("NASCenterJS Snmp Open: ");
       exit(1);
   }
}

/*****************************************************************************
 *
 * name             :   UM_GetTableDetails
 * description      :   Fetch the table rows with respect to flag (TELNET / FTP /
 *                      POP3 / SMTP
 * input parameters :   - 
 * return type      :   UM_SUCCESS / UM_FAILURE
 * global variables :   
 * calls	    :   UM_CreateFilter
 *                      UM_Log   	
 *
*****************************************************************************/

I16 UM_GetTableDetails(I16 iFlag)
{
    struct snmp_pdu *reqPDU;
    struct snmp_pdu *respPDU;
    netsnmp_variable_list *vars;
    netsnmp_variable_list *pstCurrVars;
    stAllowUM *pstGenTable;
    I16 iCheckTable;
    oid *nextIPAddr;
    oid *tableID;
    I16 nextIPAddrLen;
    oid *nextPortNum;
    I16 nextPortNumLen;
    oid *nextRowStatus;
    I16 nextRowStatusLen;
    I16 iProcessCount;
    I16 iDecisionFlag;
    I16 status=0;
    I16 iIPAddrLen;
    I16 iLen;
    I16 tableLen;
	
    /*UI8 cCommunity[6]="public";*/
    /*If row fetching belongs to telnet table*/
    if(iFlag == ALLOW_TELNET)
    {
       nextIPAddr       = allowTelenetIPAddress_variables_oid;
       nextIPAddrLen    = sizeof(allowTelenetIPAddress_variables_oid)/ sizeof(oid);
       nextPortNum      = allowTelenetPortNo_variables_oid;
       nextPortNumLen   = sizeof(allowTelenetPortNo_variables_oid)/ sizeof(oid); 
       nextRowStatus    = allowTelnetStatus_variables_oid;
       nextRowStatusLen= sizeof(allowTelnetStatus_variables_oid)/ sizeof(oid); 
    /*If row fetching belongs to FTP table*/
    }else if(iFlag == ALLOW_FTP)
    {
       nextIPAddr       = allowFTPIPAddress_variables_oid;
       nextIPAddrLen    = sizeof(allowFTPIPAddress_variables_oid)/ sizeof(oid);
       nextPortNum      = allowFTPPortNo_variables_oid;
       nextPortNumLen   = sizeof(allowFTPPortNo_variables_oid)/ sizeof(oid); 
       nextRowStatus    = allowFTPStatus_variables_oid;
       nextRowStatusLen= sizeof(allowFTPStatus_variables_oid)/ sizeof(oid); 
    /*If row fetching belongs to POP3 table*/
    }else if(iFlag == ALLOW_POP3)
    {
       nextIPAddr       = allowPOP3IPAddress_variables_oid;
       nextIPAddrLen    = sizeof(allowPOP3IPAddress_variables_oid)/ sizeof(oid);
       nextPortNum      = allowPOP3PortNo_variables_oid;
       nextPortNumLen   = sizeof(allowPOP3PortNo_variables_oid)/ sizeof(oid); 
       nextRowStatus    = allowPOP3Status_variables_oid;
       nextRowStatusLen= sizeof(allowPOP3Status_variables_oid)/ sizeof(oid); 
    /*If row fetching belongs to SMTP table*/
    }else if(iFlag == ALLOW_SMTP)
    {
       nextIPAddr       = allowSMTPIPAddress_variables_oid;
       nextIPAddrLen    = sizeof(allowSMTPIPAddress_variables_oid)/ sizeof(oid);
       nextPortNum      = allowSMTPPortNo_variables_oid;
       nextPortNumLen   = sizeof(allowSMTPPortNo_variables_oid)/ sizeof(oid); 
       nextRowStatus    = allowSMTPStatus_variables_oid;
       nextRowStatusLen= sizeof(allowSMTPStatus_variables_oid)/ sizeof(oid); 
    }

       
    tableID =nextIPAddr;
    tableLen =nextIPAddrLen; 


    while(1) 
    {
       reqPDU = snmp_pdu_create(SNMP_MSG_GETBULK);
       reqPDU->non_repeaters = 0;
       /*Set the number of records to be fetched */
       reqPDU->max_repetitions = MAX_REPETITIONS;

       snmp_add_null_var(reqPDU,nextIPAddr,nextIPAddrLen);
       snmp_add_null_var(reqPDU,nextPortNum,nextPortNumLen);
       snmp_add_null_var(reqPDU,nextRowStatus,nextRowStatusLen);

       /*Get the response in the respPDU*/
       status = snmp_synch_response(agentSess,reqPDU,&respPDU);	
       if(status==STAT_SUCCESS)
       {
	   if(respPDU->errstat==SNMP_ERR_NOERROR && 
           respPDU->errindex==0 && 
           respPDU->variables->type != ASN_NULL &&
           respPDU->variables->type != SNMP_NOSUCHINSTANCE &&
           respPDU->variables->type != SNMP_NOSUCHOBJECT  &&
	   respPDU->variables->type !=  SNMP_ENDOFMIBVIEW )
	   {
#ifdef UM_DEBUG
	          UM_Log("sync response success \n");	   
#endif
	   }else
	   {
#ifdef UM_DEBUG
	          UM_Log("sync response failure \n");	   
#endif
		  close(giFileId);
		  giFileId = 0;
                  return NASC_FAILURE;		   
	   }
           if(respPDU == NULL)	        
	   {
#ifdef UM_DEBUG
                UM_Log("Error in sync response \n");		   
#endif
		  close(giFileId);
		  giFileId = 0;
		return NASC_FAILURE;
	   }
#ifdef UM_DEBUG
           UM_Log("Request Succeeded\n");
#endif
       } else 
       {
#ifdef UM_DEBUG
           UM_Log("Error in snmp_synch_response \n");
#endif
           snmp_perror("Error ");   	   
		  close(giFileId);
		  giFileId = 0;
	   return NASC_FAILURE;
       }
       vars = respPDU->variables;
       pstCurrVars = vars;


       /*Flag for checkiing whether the fetched table details belongs to 
	* required table*/ 
       iCheckTable = TRUE;
       for(iProcessCount =0; iProcessCount < MAX_REPETITIONS;
             iProcessCount++ )
       {
              /*Check the fetched record is belong to respective table */
              if(netsnmp_oid_is_subtree(tableID,
                  tableLen, pstCurrVars->name, pstCurrVars->name_length))
	      { 
		       iCheckTable = FALSE;
                       break;
              }
   	      pstGenTable = malloc(sizeof(stAllowUM)); 
              if(pstGenTable == NULL)
              {
#ifdef UM_DEBUG	   
                   UM_Log("Memory allocation failed \n "); 			
#endif       
		  close(giFileId);
		  giFileId = 0;
                   return NASC_INSUFFICIENT_MEMORY;
              }
	      
              pstGenTable->pcIpAddr = NULL;  
	      pstGenTable->iPortNum = -1;
	      pstGenTable->iStatus  = -1;


	      
              /*Get the Length of IP Address */
              iIPAddrLen =0;
              iIPAddrLen =  pstCurrVars->val_len;
	      if(iIPAddrLen != 0)
	      {
                   pstGenTable->pcIpAddr= malloc(iIPAddrLen+1);
                   if(pstGenTable->pcIpAddr == NULL)
                   {
#ifdef UM_DEBUG	   
                   	UM_Log("Memory allocation failed \n ");	
#endif       
		  close(giFileId);
		  giFileId = 0;
                   	return NASC_INSUFFICIENT_MEMORY;
                    }
                    memset(pstGenTable->pcIpAddr,0,(iIPAddrLen+1));
                    memcpy(pstGenTable->pcIpAddr,pstCurrVars->val.string,iIPAddrLen);
	      }

              nextIPAddr       = pstCurrVars->name;
              nextIPAddrLen    = pstCurrVars->name_length;
	      iLen=0;
	      iLen = pstCurrVars->name_length-1;
	      /*UM_Log("Value %ld \n", pstCurrVars->name[iLen] );*/
              
	      /*Get the Index ID of the table*/
              pstGenTable->iIndex = pstCurrVars->name[iLen];
		
              /*Get the Port number */
              pstCurrVars = pstCurrVars->next_variable;
	      if(pstCurrVars->val.integer != NULL)
	      {
                    pstGenTable->iPortNum = (I16)(*pstCurrVars->val.integer);
	      }

              nextPortNum      = pstCurrVars->name;
              nextPortNumLen   = pstCurrVars->name_length;

              /*Get the row status */
              pstCurrVars = pstCurrVars->next_variable;
	     
	      if(pstCurrVars->val.integer != NULL) 
              {
                     pstGenTable->iStatus = (I16)(*pstCurrVars->val.integer); 
              }
              nextRowStatus    =  pstCurrVars->name;
              nextRowStatusLen = pstCurrVars->name_length;
              
              /*Set the Decision Flag */  	      
              iDecisionFlag = UM_ADD;     
	      /*Add filter rule */
	      pstGenTable->iFlag = iFlag;
          if((UM_CreateFilter(pstGenTable,iDecisionFlag)) == NASC_MANDATORY_MISSING)
	      {
              free(pstGenTable->pcIpAddr);
              pstGenTable->pcIpAddr = NULL;
		      free(pstGenTable);
		      pstGenTable = NULL;
	      }
	      

	      /*Move the Pointer to the next localtion*/  
              pstCurrVars = pstCurrVars->next_variable;

       }
       if(iCheckTable == FALSE)
       {
            break; 		   
       }		   
     } // end of while(1)
    return NASC_SUCCESS;
} 

/*****************************************************************************
 *
 * name             :   UM_CreateFilter  
 * description      :   Adds the filter rule 
 * input parameters :   pstGenFilt -> pointer to the stAllowUM table
 *                      iDecisionFlag -> UM_ADD / UM_MODIFY / UM_DEL values
 * return type      :   UM_SUCCESS / UM_FAILURE / UM_NO_RECORD_FOUND
 * global variables :   - 
 * calls	    :   UM_Log	
 * 		        UM_AddRule
 * 		        IM_ModifyRule
 * 		        UM_DelRule
*****************************************************************************/
I16 UM_CreateFilter( stAllowUM  *pstGenFilt, I16 iDecisionFlag)
{
    
    I16 iResult;

    /*Check any * present in the record
     * It means just allow all the hosts 
     * No need to add any firewall rules */   

    /* Switch with respect to iDecisionFlag */


    if ( giFileId == -1) 
    {
        giFileId = open(gpcHostAllow ,  O_CREAT | O_RDWR , S_IREAD | S_IWUSR);

			     if ( giFileId == -1 )
			     {
						     	printf("UNABLE TO OPEN FILE..\n");

							     return NASC_FAILURE;
			     }
    }



    if(pstGenFilt->pcIpAddr != NULL)
    {
        if(strcmp(pstGenFilt->pcIpAddr, "*")==0)  
        {
#ifdef UM_DEBUG	   
             UM_Log(" No Entry in the table \n");        
#endif       
             return UM_NO_RECORD_FOUND;   				
        }
    }

    
    switch(iDecisionFlag)
    {
        case UM_ADD:
             iResult =0; 		
             iResult = UM_AddRule(pstGenFilt,&stListPtr);
             close ( giFileId );
             giFileId = -1;
	     if(iResult == NASC_MANDATORY_MISSING )
             {
                   return NASC_MANDATORY_MISSING;		     
             }
	     if(iResult == NASC_FAILURE )
             {
#ifdef UM_DEBUG	   
             UM_Log("Error : Error occured while adding the rule \n");		     
#endif       
             }
             break;		
        case UM_MODIFY:
             iResult =0; 		
             iResult = UM_ModifyRule(pstGenFilt,&stListPtr); 		
             close ( giFileId );
             giFileId = -1;
	     if(iResult == NASC_FAILURE)
             {
#ifdef UM_DEBUG	   
                  UM_Log("Error : Error occured while modifying the rule \n");		     
#endif       
             }
	     
             break;		
        case UM_DELETE:
             iResult =0; 		
             iResult = UM_DelRule(pstGenFilt,&stListPtr); 		
             close ( giFileId );
             giFileId = -1;
	     if(iResult == NASC_FAILURE)
             {
#ifdef UM_DEBUG	   
                  UM_Log("Error : Error occured while deleting the rule \n");		     
#endif       
             }
             break;		
    }	    
    return 1;
}    

/*****************************************************************************
 * name             :   UM_AddRule
 * description      :   Adds the filter rule 
 * input parameters :   pstGenFilt -> pointer to the stAllowUM table
 *                      stListPtr-> Pointer to the stList structure 
 * return type      :   UM_SUCCESS / UM_FAILURE  
 * global variables :   - 
 * calls	    :   UM_Log	
 * 		        IM_Insert 	
**************************************************************/
I16 UM_AddRule(stAllowUM  *pstGenFilt,  stList stListPtr)
{
    I16 iProtocol;
    char cAddRuleBuff[2000];
    
    char cBuff[17];
    I16 iPortNum;
    I16 iRetValue = 0;
    UI8 cData[MAX_CHAR];
				I32  iFound= 0;
    I32 iAllowAll = 0;

    memset(cAddRuleBuff , '\0' , 2000);
    memset(cBuff , '\0' , 17);
    memset(cData , '\0' , MAX_CHAR);

    /*Check any mandatory is missing*/    
    if(pstGenFilt->pcIpAddr == NULL  || pstGenFilt->iPortNum == -1 ||
      pstGenFilt->iStatus == -1 )
    {
#ifdef UM_DEBUG	    
          printf("Mandatory is missing \n");	    
	  return NASC_MANDATORY_MISSING;
#endif	  
    }

	    
	/*Get the IP Address */
    /* if(strcmp(pstGenFilt->pcUserName,"*")==0)
    {
        lseek(giFileId , 0 , SEEK_END);
        write(giFileId , "ALL" , 3);

    } */
	/*Convert IP Address*/
    sprintf(cBuff,"%d.%d.%d.%d",(0xff& pstGenFilt->pcIpAddr[0]),(0xff & 
    pstGenFilt->pcIpAddr[1]), (0xff & pstGenFilt->pcIpAddr[2]),
		    (0xff& pstGenFilt->pcIpAddr[3]));
	
    /*Check if the record is Not in service*/
     if(pstGenFilt->iStatus == 2)
     {
        UM_InsertToList(pstGenFilt->iIndex,stListPtr,pstGenFilt->iPortNum,
            cBuff, pstGenFilt->iFlag , pstGenFilt->iStatus);
#ifdef UM_DEBUG	    
          printf("Row Status is Not in service\n");	    
#endif	  
	  return NASC_MANDATORY_MISSING;
     }                        

    
   /* if(pstGenFilt->iFlag == ALLOW_TELNET)	
    {
        iProtocol=23; 
	
    }else if( pstGenFilt->iFlag == ALLOW_FTP)
    {
         iProtocol=21; 
	
    }else if (pstGenFilt->iFlag == ALLOW_SMTP)
    {
        iProtocol=25; 
	
    }else if(pstGenFilt->iFlag == ALLOW_POP3 )
    {
        iProtocol=110; 
    }*/

    iProtocol = 6;

    /*Check the values are source ip address and port numbers are not null*/
    if(pstGenFilt->iPortNum != UM_DEFAULT_PORT)
    {
        iPortNum = pstGenFilt->iPortNum; 	    
    }else 
    {
        iPortNum = 0;  	    
    }

	if(pstGenFilt->pcIpAddr  == NULL)
    {
        UM_Log("UM_AddRule: Rule Can not be set!!! IP Address is missing !!!");	    
        return NASC_FAILURE;
    } 

    /* sprintf(cAddRuleBuff,"%s ,
        cgenFiltString ,cBuff,6, cSMask , iPortNum ,cLocalHostIPAddr,
            cDMask,cInterfaceName); */


    if ( strcmp(cBuff , ALLOW_ALL) == 0 )
    {
        iAllowAll = 1;
    }


    sprintf(cAddRuleBuff , "%s : %s\n" , gpcServiceDeamons[pstGenFilt->iFlag] , 
                                       ((iAllowAll) ? "ALL" : cBuff) );
    lseek ( giFileId , 0 , SEEK_SET);

    while (UM_GetLine(giFileId , cData , MAX_CHAR) > 0)  
    {
        
        if ( ( (( iAllowAll) ? strstr(cData , "ALL") :  strstr(cData , cBuff)) != NULL ) && 
             (strstr(cData , gpcServiceDeamons[pstGenFilt->iFlag]) != NULL)
           )
        {
             iFound  = 1;
             break;
         }
         else
         {
            memset(cData , '\0' , MAX_CHAR);
            continue;
        }
    }

				iRetValue = 1;
if ( iFound == 0 )
{
        lseek ( giFileId , 0 , SEEK_END);
       if (strlen(gpcServiceDeamons[pstGenFilt->iFlag]) > 0 ) 
   {

        lseek(giFileId , 0 , SEEK_END);

        iRetValue = write(giFileId , cAddRuleBuff , 
strlen(cAddRuleBuff));
  }
}

      if ( iRetValue > 0 )
      {
        UM_Log("UM_AddRule :Rule has been added successfully \n");	    
	 
        UM_InsertToList(pstGenFilt->iIndex,stListPtr,pstGenFilt->iPortNum,
            cBuff, pstGenFilt->iFlag , pstGenFilt->iStatus);

      }else
      {
         
         UM_Log("UM_AddRule :Error in adding the rule \n");	    
		       return NASC_FAILURE;
      }
    return NASC_SUCCESS;
}

/*****************************************************************************
 * name             :   UM_InsertRule
 * description      :   Adds the filter rule 
 * input parameters :   cpRuleStr-> pointer to the character buffer 
 * return type      :   UM_SUCCESS / UM_FAILURE  
 * global variables :   - 
 * calls	    :   -
*****************************************************************************/
I16 UM_InsertRule(UI8 *cpRuleStr)
{
    I16 iSuccessFlag;     


    return iSuccessFlag;  
}
/*****************************************************************************
 * name             :   UM_ModifyRule 
 * description      :   Modifies the filter rule 
 * input parameters :   pstGenFilt -> pointer to the stAllowUM table
 *                      iFlag ->flag decides what protocol it is
 *                      stListPtr-> Pointer to the stList structure 
 * return type      :   UM_SUCCESS / UM_FAILURE  
 * global variables :   - 
 * calls	    :   UM_Log	
 *                      UM_GetPosFromList
*****************************************************************************/
I16 UM_ModifyRule(stAllowUM  *pstGenTable,  stList stListPtr)
{
    I16 iProtocol;
    char cModRuleBuff[2000];
    I16 iPortNum;
    char cIPAddr[17];
    I16 iResult = 0;
    stRetStruct* pstListModPtr = NULL;
    UI8 *pcTempPtr;
    I16 iPortFlag =0;
    I16 iIPAddrFlag=1;
    I16 iRulNum;
    UI8 cData[MAX_CHAR]; 
    I32 iDataLen = 0;
    I32 iAllowAll = 0;
    I32 iPrevIpAddrAllowAll = 0;


    memset(cModRuleBuff , '\0' , 2000); 
    memset(cData , '\0' , MAX_CHAR); 
    memset(cIPAddr , '\0' , 17); 
    
     
    /*Get the position of the rule in the List*/
    pstListModPtr = UM_GetPosFromList(pstGenTable->iIndex, stListPtr, pstGenTable->iFlag);

    if(pstListModPtr == NULL)
    {
        UM_Log("UM_ModifyRule: No Record found \n");
	       return NASC_FAILURE;
    }
				if ( pstListModPtr->modStruct == NULL)
				{
								return NASC_FAILURE;
				}
    /*bzero(cModRuleBuff,0,2000);*/

    /*if(pstGenTable->iFlag == ALLOW_TELNET)	
    {
        iProtocol=23; 
	
    }else if(pstGenTable->iFlag == ALLOW_FTP)
    {
         iProtocol=21; 
	
    }else if (pstGenTable->iFlag == ALLOW_SMTP)
    {
        iProtocol=25; 
	
    }else if(pstGenTable->iFlag == ALLOW_POP3 )
    {
        iProtocol=110; 
    }*/

    iProtocol=6; 

    /*Check the values are source ip address and port numbers are not null*/
    if(pstGenTable->iPortNum != UM_DEFAULT_PORT)
    {
        iPortNum = pstGenTable->iPortNum; 	    
	    iPortFlag =1; 
    }else 
    {
	    iPortNum = pstListModPtr->modStruct->iPortNum;  	    
	}
    if(pstGenTable->pcIpAddr != UM_DEFAULT_IPADDR)
    {
        /*Convert IP Address*/
        sprintf(cIPAddr,"%d.%d.%d.%d",(0xff& pstGenTable->pcIpAddr[0]),(0xff & 
        pstGenTable->pcIpAddr[1]), (0xff & pstGenTable->pcIpAddr[2]),
		      (0xff& pstGenTable->pcIpAddr[3]));
        iIPAddrFlag =1;	
    }else
    {
        strcpy(cIPAddr ,pstListModPtr->modStruct->pcIPAddr);	    
    }


    if ( strcmp(cIPAddr , ALLOW_ALL) == 0 )
    {
        iAllowAll = 1; 
    }

    if ( strcmp(pstListModPtr->modStruct->pcIPAddr , ALLOW_ALL) == 0 )
    {
      iPrevIpAddrAllowAll = 1;
    }

if ( pstGenTable->iStatus == 2)
				{
         iResult = UM_DelRule(pstGenTable,stListPtr);
    }
else if ( pstGenTable->iStatus == 1)
				{
         iResult = UM_AddRule(pstGenTable,stListPtr);
    }

    else
   {
    
   	iRulNum = (pstListModPtr->iCount+1);

    lseek(giFileId , 0 , SEEK_SET);

    while (UM_GetLine(giFileId , cData , MAX_CHAR) > 0)  
    {
        
        if ( ( ((iPrevIpAddrAllowAll) ? strstr(cData , "ALL") :  (strstr(cData , pstListModPtr->modStruct->pcIPAddr))) != NULL ) && 
             (strstr(cData , gpcServiceDeamons[pstGenTable->iFlag]) != NULL)
           )
        {
            iDataLen = strlen(cData) + 1;
            iDataLen = 0 - iDataLen;
            lseek(giFileId , iDataLen , SEEK_CUR);
            write ( giFileId , gcEmptyStr  , strlen(cData) + 1);
            iDataLen = 0 - (strlen(cData) + 1);
            lseek(giFileId , iDataLen , SEEK_CUR);
            sprintf(cModRuleBuff,"%s : %s\n" , 
                    gpcServiceDeamons[pstGenTable->iFlag] , ((iAllowAll) ? "ALL" : cIPAddr) );
            iResult = 1;
            write ( giFileId , cModRuleBuff  , strlen(cModRuleBuff));
            break;
         }
         else
         {
             memset(cData , '\0' , MAX_CHAR); 
         }    

    }
   }

    if(iResult == 1)
    {
         UM_Log("UM_ModifyRule :Rule has been modified successfully \n");	    
	 /*Modify the port number */
         if(iPortFlag ==1)	 
         {		 
              pstListModPtr->modStruct->iPortNum = iPortNum;
         }
	 if(iIPAddrFlag ==1)
         { 		 
             pcTempPtr = pstListModPtr->modStruct->pcIPAddr;
             pstListModPtr->modStruct->pcIPAddr = malloc(strlen(cIPAddr)+1);
             if(pstListModPtr->modStruct->pcIPAddr ==  NULL)
             {
                 UM_Log("UM_ModifyRule :Memory allocation failed \n ");   		
                 return NASC_FAILURE;		
             }
             memset(pstListModPtr->modStruct->pcIPAddr,'\0',strlen(cIPAddr)+1);
             memcpy(pstListModPtr->modStruct->pcIPAddr,cIPAddr,
                  strlen(cIPAddr));	    
															pstListModPtr->modStruct->iStatus = pstGenTable->iStatus;
             free(pcTempPtr);
         }

    }else
    {
          UM_Log("UM_ModifyRule :Error in modifying the rule \n");	    
    }
    return NASC_SUCCESS;
}


/*****************************************************************************
 * name             :   UM_DelRule
 * description      :   Delete the filter rule 
 * input parameters :   pstGenFilt -> pointer to the stAllowUM table
 *                      iFlag ->flag decides what protocol it is
 *                      stListPtr-> Pointer to the stList structure 
 * return type      :   UM_SUCCESS / UM_FAILURE  
 * global variables :   - 
 * calls	    :   UM_DelRuleNumFromList
 *                      UM_DeleteFromList
 *                      UM_DelRuleNum
*****************************************************************************/
I16 UM_DelRule(stAllowUM  *pstGenFilt, stList stListPtr)
{
    I32 iResult = 0;
    UI8 cData[MAX_CHAR]; 
    UI8 cIPAddr[17];
    I32 iDataLen = 0;
    stRetStruct* pstListModPtr = NULL;
    UI8 * pcIpAddr;
    I32 iAllowAll = 0;


    /*Get the list position*/
    pstListModPtr = UM_GetPosFromList(pstGenFilt->iIndex, stListPtr, pstGenFilt->iFlag);

    if ( pstListModPtr == NULL )
    {
          printf("UM_DelRule :Error in Deleting the rule \n");	    
          return NASC_FAILURE;
    }

				if ( pstListModPtr->modStruct == NULL)
				{
								return NASC_FAILURE;
				}

    pcIpAddr = pstListModPtr->modStruct->pcIPAddr;



        /*Convert IP Address*/
        memset(cIPAddr,'\0',17);
        sprintf(cIPAddr,"%d.%d.%d.%d",(0xff& pcIpAddr[0]),(0xff & 
        pcIpAddr[1]), (0xff & pcIpAddr[2]),
		      (0xff& pcIpAddr[3]));

    

    if ( strcmp(pcIpAddr , ALLOW_ALL) == 0 )
    {
        printf("ALLOW ALL IPADDR\n");
        iAllowAll = 1; 
    }
    lseek(giFileId , SEEK_SET , 0);

    while (UM_GetLine(giFileId , cData , MAX_CHAR) > 0)  
    {
        
        if ( ((iAllowAll) ? strstr(cData , "ALL") : strstr(cData , pcIpAddr)) && 
             strstr(cData , gpcServiceDeamons[pstGenFilt->iFlag]))
           
        {
            iDataLen = strlen(cData) + 1;
            iDataLen = 0 - iDataLen;
            lseek(giFileId , iDataLen , SEEK_CUR);
            write ( giFileId, gcEmptyStr  , strlen(cData) + 1);
            iResult = 1;
            break;
         }
         else
         {
             memset(cData , '\0' , MAX_CHAR); 
         }    
 
    }
    

    if(iResult == 1)
    {
         UM_Log("UM_DelRule: Rule has been Deleted successfully \n");
									iResult = 1;
									if ( pstGenFilt->iStatus == 6) 
           iResult = UM_DeleteFromList(stListPtr,pstGenFilt->iIndex, 
																																							pstGenFilt->iFlag);
         if(iResult)
         {
              UM_Log("UM_DelRule :Structure has been deleted from the list \n ");		  
         }else
         {
              UM_Log("UM_DelRule :Structure can not be deleted from the list \n ");		  
         }		 
    }else
    {
          UM_Log("UM_DelRule :Error in Deleting the rule \n");	    
          return NASC_FAILURE;
    }
    return NASC_SUCCESS;
}

/*****************************************************************************
 *
 * name             :   UM_InitCBuffer  
 * description      :   Initialize the circular queue 
 * input parameters :   pstBuffer pointer to the circular buffer queue 
 * return type      :   void 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
void UM_InitCBuffer(SCBuffer *pstCBuffer)
{
  /*Initialize the circular queue*/	
  pstCBuffer->iQRear = -1;
  pstCBuffer->iQFront = 0;
  pthread_mutex_init(&(pstCBuffer->queue_lock) , NULL);
  pthread_cond_init(&(pstCBuffer->queue_not_full) , NULL);
  pthread_cond_init(&(pstCBuffer->queue_not_empty) , NULL);
}

/*****************************************************************************
 *
 * name             :   UM_IsQueueEmpty  
 * description      :   Check if the circular queue is empty 
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   BOOL 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
BOOL  UM_IsQueueEmpty(SCBuffer *pstCBuffer)
{
  /*Check Queue is empty*/	
  BOOL status = FALSE;
  if(((pstCBuffer->iQRear + 1) % (UM_MAX_ELEMENT)) == pstCBuffer->iQFront)
  {
    status = TRUE;
  }
  return status;
}

/*****************************************************************************
 *
 * name             :   UM_IsQueue_Full  
 * description      :   Check if the circular queue is full 
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   BOOL 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
BOOL  UM_IsQueue_Full(SCBuffer *pstCBuffer)
{
  /*Check queue is full*/	
  BOOL status = FALSE;
  if(((pstCBuffer->iQRear + 2) % (UM_MAX_ELEMENT )) == pstCBuffer->iQFront)
  {
    status = TRUE;
  }
  return status;

}

/*****************************************************************************
 *
 * name             :   UM_ProcessTrapMsg 
 * description      :   Copy the trap PDU and process the same  
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   void 
 * global variables :   - 
 * calls	    :   UM_IsQueueEmpty
 *                      UM_UserMeetering   
 * 		        
*****************************************************************************/
void UM_ProcessTrapMsg(void *args)
{
  netsnmp_pdu *trapPDU;
  netsnmp_pdu *pstClonePDU;
  
  
  /*Thread function to process the trap message*/
  while(1)
  {
       pthread_mutex_lock(&(stCB.queue_lock));
#ifdef UM_DEBUG	   
       UM_Log("Enter into queue \n");
#endif       
       if(UM_IsQueueEmpty(&stCB))
       {
           puts("UM_ProcessTrapMsg Queue Empty.. waiting for signal");
           pthread_cond_wait(&(stCB.queue_not_empty) ,
               &(stCB.queue_lock));
           pthread_mutex_unlock(&(stCB.queue_lock));
       }
       trapPDU =  stCB.queueEntry[stCB.iQFront]; 
       pstClonePDU= snmp_clone_pdu(trapPDU);
       snmp_free_pdu(trapPDU);  
       stCB.queueEntry[stCB.iQFront] = NULL;  

       stCB.iQFront = ++(stCB.iQFront) % (UM_MAX_ELEMENT); 
    //   pthread_cond_signal(&(pstCBuffer->queue_not_full)); 
       pthread_mutex_unlock(&(stCB.queue_lock));
       /*Call function to parse the retrieved trap message */     
       UM_UserMeetering(pstClonePDU);
  }
} 

/*****************************************************************************
 *
 * name             :   UM_ListenTrap 
 * description      :   Listens the trap message and copy the same in
 *                      to the circular buffer 
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   NASC_SUCCESS 
 * global variables :   - 
 * calls	    :   UM_IsQueue_Full 
 * 		        
*****************************************************************************/
int UM_ListenTrap(int id, struct snmp_session *sess, int req, netsnmp_pdu *pdu, void *arg)
{
    netsnmp_pdu *pstPDUClone;

    /* Call back function to write the trap message in to circular queue */ 	

#ifdef UM_DEBUG	   
    UM_Log("Received Trap message \n ");
#endif       

    pthread_mutex_lock(&(stCB.queue_lock));
    if(UM_IsQueue_Full(&stCB))
    {
      puts("UM_WriteQueue Queue Full");
#ifdef UM_DEBUG	   
      UM_Log("Dropping the Trap Msg \n");
#endif       
      pthread_mutex_unlock(&(stCB.queue_lock));
      /*Error should be logged in the syslog */
    }
    stCB.iQRear = ++(stCB.iQRear) % (UM_MAX_ELEMENT);
    pstPDUClone = snmp_clone_pdu(pdu);
    stCB.queueEntry[stCB.iQRear] = pstPDUClone;
    pthread_cond_signal(&(stCB.queue_not_empty)); 
    pthread_mutex_unlock(&(stCB.queue_lock));
    return NASC_SUCCESS;
                                                                    
}   
/*****************************************************************************
 *
 * name             :   UM_UserMeetering 
 * description      :   Parse the trap message and calls the function for 
 *                      creating the rule  
 * input parameters :   respPDU -> pointer to the snmp_pus structure 
 * return type      :   void 
 * global variables :   - 
 * calls	    :   UM_Log
 *                      UM_CreateFilter
 *                      UM_CheckTable
*****************************************************************************/
void UM_UserMeetering( struct snmp_pdu *respPDU)
{
    I16 iOperFlag;
    I16 iIPAddrLen;
    I16 iDecisionFlag;
    netsnmp_variable_list *vars;
    netsnmp_variable_list *pstCurrVars;
    I16 iPortFlag=0;
    I16 iIPAddrFlag=0;
    I16 iFieldFlag=0;
    stAllowUM *pstGenTable;
    
    vars = respPDU->variables;
    pstCurrVars = vars;
    iOperFlag = UM_CheckTable(pstCurrVars);
    if(iOperFlag == -1)
    {
#ifdef UM_DEBUG	   
         UM_Log("Not belong to this table \n");	    
#endif       
         printf("Not belong to this table \n");	    
         return;
    }
    /*Allocate Memory for Generic table structure*/

    pstGenTable =malloc(sizeof(stAllowUM));
    if(pstGenTable == NULL)
    {
#ifdef UM_DEBUG	   
         UM_Log("Memory allocation failed \n "); 			
#endif       
         printf("Memory allocation failed \n "); 			
         return ;
    }
    pstGenTable->pcIpAddr = NULL;
    pstGenTable->iPortNum = -1;
    pstGenTable->iStatus  = -1;

    if(pstCurrVars != NULL)
    {
         pstGenTable->iIndex = UM_GetIndex(pstCurrVars);
    }else
    {
#ifdef UM_DEBUG	   
         UM_Log("No record exists \n");	    
#endif       
         printf("No record exists \n");	    
	 return ;
    }
    /*Assign the operation flag */ 
    pstGenTable->iFlag =iOperFlag; 
    while(pstCurrVars != NULL) 
    {
	    switch(iOperFlag)
            {
                case ALLOW_TELNET:
                    iFieldFlag = UM_CheckFieldTelnet(pstCurrVars);
                    break;
                case ALLOW_FTP:
                    iFieldFlag = UM_CheckFieldFTP(pstCurrVars);
                    break;
                case ALLOW_SMTP:
                    iFieldFlag = UM_CheckFieldSMTP(pstCurrVars);
                    break;
                case ALLOW_POP3:
                    iFieldFlag = UM_CheckFieldPOP3(pstCurrVars);
                    break;
                default:
	            /*Move to the next record */ 
	            pstCurrVars = pstCurrVars->next_variable;
                    continue;		    
            }
	    switch(iFieldFlag)
            {
             case IP_ADDR:
                   /*Get the Length of IP Address */
                   iIPAddrLen =0;
                   iIPAddrLen =  pstCurrVars->val_len;
		   if(iIPAddrLen == 0)
		   {
#ifdef UM_DEBUG
                   UM_Log("Invalid IP address \n");  			   
#endif		   
			   break;
		   }
                   pstGenTable->pcIpAddr= malloc(iIPAddrLen+1);
                   if(pstGenTable->pcIpAddr == NULL)
                   {
#ifdef UM_DEBUG	   
                        UM_Log("Memory allocation failed \n ");	
#endif       
                        return;
                   }
		   memset(pstGenTable->pcIpAddr,0,(iIPAddrLen+1));
                   memcpy(pstGenTable->pcIpAddr,pstCurrVars->val.string,iIPAddrLen);
		   iIPAddrFlag=1;
		   break;
             case IP_PORT_NUM:
                   /*Get the Port number */
		   if(pstCurrVars->val.integer != NULL)
		   {
                        pstGenTable->iPortNum = (I16)(*pstCurrVars->val.integer); 
                        iPortFlag =1;
		   }
                   break;		     
             case IP_STATUS:
                   /*Get the row status */
		   if(pstCurrVars->val.integer != NULL)
		   {
                          pstGenTable->iStatus = (I16)(*pstCurrVars->val.integer);  
                          /*Get the index ID*/
		   }
                   break;
             case -1:
#ifdef UM_DEBUG	   
                   UM_Log("UM_UserMeetering : Stop the parsing and continue for update \n");
		   break;
#endif       
                       		   
             default:
#ifdef UM_DEBUG	   
                   UM_Log("UM_UserMeetering : Not an valid record \n");
#endif       
                   break;

		    /*if(pstGenTable->pcIpAddr != NULL)
		   {
			   free(pstGenTable->pcIpAddr);
			   pstGenTable->pcIpAddr = NULL;
		   } */
           }

	    /*Move to the next record */ 
	    pstCurrVars = pstCurrVars->next_variable;
    }

    if(iIPAddrFlag ==0)
    {
      pstGenTable->pcIpAddr = NULL;    	    
    }
    if(iPortFlag ==0)
    {
      pstGenTable->iPortNum= -1;    	    
    }
 
    /*Fine the decision flag*/
    /*Check record is for Add / Modify / Del */
    switch(pstGenTable->iStatus)
    {
          case 4:
                 iDecisionFlag = UM_ADD;
                 break;
          /*case 5:
                 iDecisionFlag = UM_MODIFY;
                 break;*/
          case 6:
                 iDecisionFlag = UM_DELETE;
                 break;
          default:
                 iDecisionFlag = UM_MODIFY;
                 break;
    }

  if ( giFileId == -1 )    
  {
      giFileId = open(gpcHostAllow , O_RDWR | O_CREAT);
  
	  		 if ( giFileId == -1 )
		  	 {
			   				printf("UNABLE TO OPEN FILE..\n");
   
   							return;
 	  		}
  }
       UM_CreateFilter(pstGenTable, iDecisionFlag);
}

I16 UM_GetIndex(netsnmp_variable_list *pstCurrVars)
{
       I16 iLen=0;
       I16 iIndex;

       iLen = pstCurrVars->name_length-1;
       iIndex = pstCurrVars->name[iLen];
       return  iIndex;
}  
/*****************************************************************************
 *
 * name             :   UM_CheckTable  
 * description      :   Function finds the fetched table is Telnet or 
 *                      FTP or SMTP or POP3 table   
 * input parameters :   - 
 * return type      :   Protocol to be allowed ( Telnet / FTP/ SMTP /POP3 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
I16 UM_CheckTable(netsnmp_variable_list *pstCurrVars)
{
    I16 iOperFlag;
    I16 indexLenTel;
    I16 indexLenFTP;
    I16 indexLenSMTP;
    I16 indexLenPOP3;
    
    oid *iIndexTel;
    oid *iIndexFTP;
    oid *iIndexSMTP;
    oid *iIndexPOP3; 
    
    

    iIndexTel  = allowTelnetTable_variables_oid;
    iIndexFTP  = allowFTPTable_variables_oid;
    iIndexSMTP = allowSMTPTable_variables_oid;
    iIndexPOP3 = allowPOP3Table_variables_oid; 

    indexLenTel  =  sizeof(allowTelnetTable_variables_oid) / sizeof (oid);
    indexLenFTP  =  sizeof(allowFTPTable_variables_oid) / sizeof (oid);
    indexLenSMTP =  sizeof(allowSMTPTable_variables_oid) / sizeof (oid);
    indexLenPOP3 =  sizeof(allowPOP3Table_variables_oid) / sizeof (oid);
  
     iOperFlag =-1;
    /*Check the fetched record is belong to TELNET / FTP / SMTP / POP3 table */
    if(!netsnmp_oid_is_subtree(iIndexTel,
        indexLenTel, pstCurrVars->name, pstCurrVars->name_length))
    { 
        iOperFlag = ALLOW_TELNET;
    }
    if (!netsnmp_oid_is_subtree(iIndexFTP,
        indexLenFTP, pstCurrVars->name, pstCurrVars->name_length))
    {
        iOperFlag = ALLOW_FTP;
    }
    if (!netsnmp_oid_is_subtree(iIndexSMTP,
        indexLenSMTP, pstCurrVars->name, pstCurrVars->name_length))
    {
        iOperFlag = ALLOW_SMTP;
    }
    if (!netsnmp_oid_is_subtree(iIndexPOP3,
        indexLenPOP3, pstCurrVars->name, pstCurrVars->name_length))
    {
        iOperFlag = ALLOW_POP3;
    }
    return iOperFlag;
}

/*****************************************************************************
 *
 * name             :   UM_CheckFieldTelnet 
 * description      :   Function finds the field type in telnet 
 * input parameters :   pstSNMPPDU -> Pointer to the netsnmp_variable_list
 * return type      :   I16 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
I16 UM_CheckFieldTelnet( netsnmp_variable_list *pstSNMPPDU )
{
     I16 iLen;
     I16 iRetValue=-1;

     iLen=sizeof(allowTelenetIPAddress_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(allowTelenetIPAddress_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue= IP_ADDR;	     
     }
     iLen=sizeof(allowTelenetPortNo_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(allowTelenetPortNo_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =IP_PORT_NUM;	     
     }
     iLen=sizeof(allowTelnetStatus_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(allowTelnetStatus_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          iRetValue =IP_STATUS;	     
     }
     return iRetValue;
}

/*****************************************************************************
 *
 * name             :   UM_CheckFieldFTP
 * description      :   Function finds the field type in FTP 
 * input parameters :   pstSNMPPDU -> Pointer to the netsnmp_variable_list
 * return type      :   I16 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
I16 UM_CheckFieldFTP( netsnmp_variable_list *pstSNMPPDU )
{
     I16 iLen;

     iLen=sizeof(allowFTPIPAddress_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(allowFTPIPAddress_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          return IP_ADDR;	     
     }
     iLen=sizeof(allowFTPPortNo_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(allowFTPPortNo_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          return IP_PORT_NUM;	     
     }
     iLen=sizeof(allowFTPStatus_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(allowFTPStatus_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          return IP_STATUS;	     
     }
     return NASC_FAILURE;
}
/*****************************************************************************
 *
 * name             :   UM_CheckFieldSMTP
 * description      :   Function finds the field type in FTP 
 * input parameters :   pstSNMPPDU -> Pointer to the netsnmp_variable_list
 * return type      :   I16 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
I16 UM_CheckFieldSMTP( netsnmp_variable_list *pstSNMPPDU )
{
     I16 iLen;

     iLen=sizeof(allowSMTPIPAddress_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(allowSMTPIPAddress_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          return IP_ADDR;	     
     }
     iLen=sizeof(allowSMTPPortNo_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(allowSMTPPortNo_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          return IP_PORT_NUM;	     
     }
     iLen=sizeof(allowSMTPStatus_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(allowSMTPStatus_variables_oid,iLen,
          pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          return IP_STATUS;	     
     }
     return NASC_FAILURE;
}
/*****************************************************************************
 *
 * name             :   UM_CheckFieldPOP3
 * description      :   Function finds the field type in FTP 
 * input parameters :   pstSNMPPDU -> Pointer to the netsnmp_variable_list
 * return type      :   I16 
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/
I16 UM_CheckFieldPOP3( netsnmp_variable_list *pstSNMPPDU )
{
     I16 iLen;

     iLen=sizeof(allowPOP3IPAddress_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(allowPOP3IPAddress_variables_oid,iLen,
           pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          return  IP_ADDR;	     
     }
     iLen=sizeof(allowPOP3PortNo_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(allowPOP3PortNo_variables_oid,iLen,
           pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          return IP_PORT_NUM;	     
     }
     iLen=sizeof(allowPOP3Status_variables_oid)/sizeof(oid);
     if(!netsnmp_oid_is_subtree(allowPOP3Status_variables_oid,iLen,
           pstSNMPPDU->name,pstSNMPPDU->name_length))		
     {
          return IP_STATUS;	     
     }
     return NASC_FAILURE;
}      

/*****************************************************************************
 *
 * name             :   UM_Log
 * description      :   Writes the log message in to log file 
 * input parameters :   logMsg ->pointer to the log message
 * return type      :   void 
 * global variables :   ifdForUM-> File descriptor for storing the 
 *                      log message in to log file  
 * calls	    :   - 
 * 		        
*****************************************************************************/
void UM_Log(UI8 *logMsg)
{
#ifdef UM_DEBUG
      fprintf(ifdForUM,"%s \n",logMsg );	
#endif      
#ifdef  UM_PRINT
      printf("%s \n",logMsg);
#endif       
}

	 

void UM_Read_Packets(void * arg){
   fd_set snmpSet;
   int snmpFds, blocking;
   struct timeval tv;
#ifdef UM_DEBUG
   UM_Log("Inside REad_Pkt......\n");
#endif
  printf("INSIDE READ PKT..\n");
   while(1){
        snmpFds=0;
        blocking=1;
        //FD_ZERO(&snmpSet);
        snmp_sess_select_info(tSess, &snmpFds,&snmpSet,&tv,&blocking);
        snmpFds = select(snmpFds,&snmpSet,0,0,0);
	printf("Select intrap :%d\n" , errno);
	if(snmpFds == -1)
	{
	    printf("SELECT FAILED\n");
	    perror("Select failed : ");
             if(snmpFds == EBADF)
	     {
#ifdef UM_DEBUG
                   UM_Log("An invalid file descriptor \n ");
#endif
	     }else if(errno == EINTR)
	     {
#ifdef UM_DEBUG
                 UM_Log("A non blocked signal was caught \n");		     
#endif
	     }else if(errno == ENOMEM)		     
	     {
#ifdef UM_DEBUG
                 UM_Log("Select was unable to allocate memory non \n");		     
#endif
	     }
#ifdef UM_DEBUG
	     UM_Log("Select returned error \n");	
#endif
	     continue;
	}
        if(snmpFds){
	printf("Inside  READ TRAP WHILE getting trap data......\n");
#ifdef UM_DEBUG
            UM_Log("Inside  READ TRAP WHILE......\n");
#endif
            snmp_sess_read(tSess, &snmpSet);
        } else {
#ifdef UM_DEBUG
            UM_Log("No FileDS to READ......\n");
#endif
	
    } // end of while(1)
 }
}
/*****************************************************************************
 * name             :   main 
 * description      :   Initialize the ruleArray , initializes the pointer
 *                      for list  
 * input parameters :  
 * return type      :   int 
 * global variables :   
 *                      stListPtrForFTP - List pointer for FTP list 
 *                      stListPtrForSMTP - List pointer for SMTP list 
 *                      stListPtrForPOP3 - List pointer for POP3 list 
 *                      stListPtrForTelnet - List pointer for Telnet list 
 * calls	    :       UM_StartUserMetering
 *                      UM_MakeEmpty
*****************************************************************************/
int main(int argc , char ** argv)
{
    pthread_t thID;
    I32 iFileId = 0;
				I32 iRetVal = 0;
				UI8 cData[MAX_CHAR];
				I32 iFound = 0;


    if ( argc < 2 )
    {
        printf("Usage : <execuatable> <tcpwraper path> \n");
        exit( 0 );
    } 

    strcpy(gpcTCPWrapperPath , argv[1]);

				/* Check if inetd.conf is compatible .. otherwise make it compatible with
							TCP wrapper  */

							UM_HandleInetd ();

    /*  open hosts.deny and add ALL */
    iFileId = open(gpcHostDeny , O_RDWR | O_CREAT);

			if ( iFileId == -1 )
			{
							printf("UNABLE TO OPEN FILE..\n");

							return 0;
			}
   truncate(gpcHostDeny , 0);
   while (UM_GetLine(iFileId , cData , MAX_CHAR) > 0)  
    {

          if ( (strstr(cData , "ALL") != NULL ) && (strstr(cData , ":") != NULL ) )
																{
																				iFound = 1;
                    printf(" match for ALL \n");
												        break;
								        }
																else
																{
													       memset(cData , '\0' , MAX_CHAR);
                    printf(" no match for ALL \n");
																				continue;
																}				
					}

					if ( iFound == 0)
					{
			      lseek(iFileId , 0 ,  SEEK_END);

         iRetVal = write(iFileId , "ALL : ALL\n" , 10);


			      if ( iRetVal < 0 ) 
         {
							      perror("Unable to write file ..\n");
							      exit(0);
         }
    }


			close ( iFileId );

	   /*Open the session with the NASCenter sub agent*/	
    UM_InitSession();
				printf("Initialised snmp session..\n");
    pthread_create(&thID,NULL,(void*)UM_Read_Packets,NULL);
    UM_StartUserMetering();
    pthread_join(thID, NULL);
    return 1;
}

ENASCenterError UM_HandleInetd (void ) 
{
				I32 iFileId = 0;
    UI8 cData[MAX_CHAR]; 
    UI8 cField1[MAX_FILED_CHAR];
    UI8 cField2[MAX_FILED_CHAR];
    UI8 cField3[MAX_FILED_CHAR];
    UI8 cField4[MAX_FILED_CHAR];
    UI8 cField5[MAX_FILED_CHAR];
    UI8 cField6[MAX_FILED_CHAR];
    UI8 cField7[MAX_FILED_CHAR];
    UI8 cField8[MAX_FILED_CHAR];
    I32 iDataLen;
				#if defined(TRUE64) || defined(HPUNIX)
    UI8 cTempString[MAX_FILED_CHAR];
    UI8 *pcTempStr1 = NULL ;
    UI8 *pcTempStr2 = NULL ;
    UI8 **ppcTempStr3 = NULL ;
				#endif
			  
    iFileId = open(gpcInetd , O_RDWR | O_CREAT);							

				if ( iFileId < 0 )
				{
								perror("Error in opening inetd.conf.. : ");
								return NASC_FAILURE;
    }
    while (UM_GetLine(iFileId , cData , MAX_CHAR) > 0)  
    {
							if ( strlen(cData) <= 0) 
							{
           memset(cData , '\0' , MAX_CHAR); 
									  continue;
								}		
           memset(cField1 , '\0' , MAX_FILED_CHAR); 
           memset(cField2, '\0' , MAX_FILED_CHAR); 
           memset(cField3 , '\0' , MAX_FILED_CHAR); 
           memset(cField4 , '\0' , MAX_FILED_CHAR); 
           memset(cField5 , '\0' , MAX_FILED_CHAR); 
           memset(cField6 , '\0' , MAX_FILED_CHAR); 
           memset(cField7 , '\0' , MAX_FILED_CHAR); 
           memset(cField8 , '\0' , MAX_FILED_CHAR);

       sscanf(cData , "%s%s%s%s%s%s%s%s" , cField1 , cField2 , cField3 ,
                                       cField4 , cField5 , cField6 ,cField7 ,
                                       cField8);


       if ( cField1[0] == '#'  || (strcmp(cField1 , "ftp") != 0  && 
												strcmp(cField1 , "telnet") !=0 && strcmp(cField1 , "smtp") !=0 && 
												strcmp(cField1 , "pop3")!= 0 && strcmp(cField1 , "pop-3")!= 0))
       {
													memset(cData , '\0' , MAX_CHAR);
             continue;
       }
							cField7[MAX_FILED_CHAR] = '\0';
       if ( strstr(cField6  , gpcTcpWrapperd ) != NULL )
       {
				#if defined(TRUE64) || defined(HPUNIX)
           pcTempStr1 = strrchr(cField7 , '/'); 
           if( pcTempStr1 != NULL)
           {
              pcTempStr2 = (char *)strtok_r(pcTempStr1 , "/" , (char **) &ppcTempStr3);
              memset(cTempString , '\0' , MAX_FILED_CHAR);
              memcpy(cTempString , pcTempStr2 , strlen(pcTempStr2));
              memset(cField7 , '\0' , MAX_FILED_CHAR);
              memcpy(cField7 , cTempString , strlen(cTempString));
           }
											#endif
           if ( strcmp(cField1 , "smtp") == 0)
             strcpy(gpcServiceDeamons[ALLOW_SMTP] , cField7); 
           else if ( (strcmp(cField1 , "pop3") == 0) || (strcmp(cField1 , "pop-3") == 0))
           {
             strcpy( gpcServiceDeamons[ALLOW_POP3] , cField7);
           }
           else if ( strcmp(cField1 , "telnet") == 0 )
           {
               strcpy( gpcServiceDeamons[ALLOW_TELNET] , cField7);
           }
           else if ( strcmp(cField1 , "ftp") == 0 )
           {
               strcpy( gpcServiceDeamons[ALLOW_FTP] , cField7);
           }
           memset(cData , '\0' , MAX_CHAR); 
           
           continue;
       }
							cField6[MAX_FILED_CHAR] = '\0';
							printf("After Strlen ..\n");

         if ( strcmp(cField1 , "smtp") == 0 )
       {
          strcpy(gpcServiceDeamons[ALLOW_SMTP] , cField7); 
       }
       else if ( (strcmp(cField1 , "pop3") == 0) || (strcmp(cField1 , "pop-3") == 0) )
       {
           strcpy( gpcServiceDeamons[ALLOW_POP3] , cField7);
       }  
       else if ( (strcmp(cField1 , "telnet") == 0))
       {
           strcpy( gpcServiceDeamons[ALLOW_TELNET] , cField7);
       }  
       else if ( (strcmp(cField1 , "ftp") == 0))
       {
           strcpy( gpcServiceDeamons[ALLOW_FTP] , cField7);
       }  
				#if defined(TRUE64) || defined(HPUNIX)
       memset(cField7 , '\0' , MAX_FILED_CHAR);
       strcpy(cField7 , cField6);
       if ( strlen(cField8) > 0 ) 
       {
           strcat(cField7 , " ");
           strcat(cField7 , cField8);
           memset(cField8 , '\0' , MAX_FILED_CHAR);
       }
							#endif
       memset(cField6 , '\0' , MAX_FILED_CHAR);
       strcpy(cField6 , gpcTCPWrapperPath);

       
        iDataLen = strlen(cData) + 1  ;
        iDataLen = 0 - iDataLen;
        lseek(iFileId , iDataLen , SEEK_CUR);
        write ( iFileId , gcEmptyStr  , strlen(cData));
        iDataLen = 0 - (strlen(cData));
        lseek(iFileId , iDataLen , SEEK_CUR);
         memset( cData , '\0' , MAX_CHAR);
        sprintf(cData,"%s %s %s %s %s %s %s" ,  cField1 , cField2 , 
                 cField3 , cField4 , cField5 , cField6 , cField7);

             write ( iFileId , cData  , strlen(cData));
	   memset(cData , '\0' , MAX_CHAR);
           memset(cField1 , '\0' , MAX_FILED_CHAR); 
           memset(cField2, '\0' , MAX_FILED_CHAR); 
           memset(cField3 , '\0' , MAX_FILED_CHAR); 
           memset(cField4 , '\0' , MAX_FILED_CHAR); 
           memset(cField5 , '\0' , MAX_FILED_CHAR); 
           memset(cField6 , '\0' , MAX_FILED_CHAR); 
           memset(cField7 , '\0' , MAX_FILED_CHAR); 
           memset(cField8 , '\0' , MAX_FILED_CHAR);
         }

									close(iFileId);

         popen("kill -HUP `ps -e | grep inetd | grep -v grep | awk '{print $1}'`" , "r");


								return NASC_SUCCESS;
    }


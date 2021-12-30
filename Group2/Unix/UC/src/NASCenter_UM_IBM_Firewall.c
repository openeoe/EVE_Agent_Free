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
 * $Author: thiagav $
 * $Log: NASCenter_UM_IBM_Firewall.c,v $
 * Revision 1.15  2004/01/29 07:01:21  thiagav
 * Memory leak checked and reviewed by shanmuga priya
 *
 * Revision 1.14  2004/01/02 11:29:30  thiagav
 * *** empty log message ***
 *
 * Revision 1.13  2004/01/02 11:22:42  thiagav
 * *** empty log message ***
 *
 * Revision 1.12  2003/12/30 04:49:32  thiagav
 * Updated IBM
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
 * $Id: NASCenter_UM_IBM_Firewall.c,v 1.15 2004/01/29 07:01:21 thiagav Exp $
 *
 ****************************************************************************/


/********************************* includes *********************************/
#include "NASCenter_UM_IBM_Firewall.h"
#include "NASCenter_Errors.h"
#include "NASCenter_Errors.h"
#include "allowTelnetTable.h"
#include "allowFTPTable.h"
#include "allowPOP3Table.h"
#include "allowSMTPTable.h"


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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <sys/utsname.h>

void UM_InitSession();
I16 UM_DeleteAllRule();
I16 UM_GetTableDetails(I16 );
I16 UM_CreateFilter(stAllowUM *, I16 );
int UM_ListenTrap(int id, struct snmp_session *, int , netsnmp_pdu *,
void *arg);
I16 UM_CheckTable();
void FreeStAllowUM( stAllowUM  *);

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
void UM_GetLocalHostIp(char *);

#define MAX_REPETITIONS 6 


/************************ globals within this module*************************/

pthread_t readThread;
pthread_t writeThread;
SCBuffer stCB;
stList stListPtr;
struct snmp_session *agentSess;
void *tSess;
FILE *ifdForUM;


I16 UM_DEFAULT_PORT=0;
UI8 UM_DEFAULT_IPADDR[]="0.0.0.0";


FILE *fpForCMD;


/************************ externs *************************/

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

#ifdef UM_UNIQUE_PORT
   UI8 localAddr[30]; 
#endif

   /* For Session Initiation */
   snmp_sess_init(&tempSess);
   tempSess.version = SNMP_VERSION_2c;

#ifdef UM_UNIQUE_PORT
   memset(localAddr,'\0',30); 
   sprintf(localAddr,"%s:%d",gIPAddr,gAgentPort);
   tempSess.peername = localAddr;
#endif
   tempSess.peername = cLocalAddr ;
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
    struct snmp_pdu *respPDU=NULL;
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

      if(respPDU != NULL)
      {
           free(respPDU);
           respPDU= NULL;
      }

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
                  return NASC_FAILURE;		   
	   }
           if(respPDU == NULL)	        
	   {
#ifdef UM_DEBUG
                UM_Log("Error in sync response \n");		   
#endif
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
             

              /*Check row status is in active*/ 
              if(pstGenTable->iStatus == 2)
               {
#ifdef UM_DEBUG
                 UM_Log("Received inactive records so continue \n");
#endif
	         /*Move the Pointer to the next localtion*/  
                 pstCurrVars = pstCurrVars->next_variable;
                 free(pstGenTable->pcIpAddr);
                 pstGenTable->pcIpAddr = NULL;
		 free(pstGenTable);
		 pstGenTable = NULL;
                 continue;
               } 
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
     if(respPDU != NULL)
     {
          free(respPDU);
          respPDU= NULL;
     }
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
             iResult = UM_AddRule(pstGenFilt,stListPtr);
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
             iResult = UM_ModifyRule(pstGenFilt,stListPtr); 		
	     if(iResult == NASC_FAILURE)
             {
#ifdef UM_DEBUG	   
                  UM_Log("Error : Error occured while modifying the rule \n");		     
#endif       
             }
	     
             break;		
        case UM_DELETE:
             iResult =0; 		
             iResult = UM_DelRule(pstGenFilt,stListPtr); 		
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
    
    char cBuff[15];
    I16 iResult;
    I16 iPortNum;
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
    if(strcmp(pstGenFilt->pcIpAddr,"*")==0)
    {
		return NASC_MANDATORY_MISSING;
         
    }
	
    /*Check if the record is Not in service*/
     if(pstGenFilt->iStatus == 2)
     {
#ifdef UM_DEBUG	    
          printf("Row Status is Not in service\n");	    
	  return NASC_MANDATORY_MISSING;
#endif	  
     }                        

    

    /*bzero(cAddRuleBuff,0,2000);*/


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


	/*Convert IP Address*/
    memset(cBuff,0,15);
    sprintf(cBuff,"%d.%d.%d.%d",(0xff& pstGenFilt->pcIpAddr[0]),(0xff & 
    pstGenFilt->pcIpAddr[1]), (0xff & pstGenFilt->pcIpAddr[2]),
		    (0xff& pstGenFilt->pcIpAddr[3]));

    /*Check if the IP Address is 255.255.255.255*/
   
   if((strcmp(cBuff,"255.255.255.255")) ==0)
   {
       sprintf(cAddRuleBuff,"%s -s %s -c %d -m %s -O eq -P %d  -d %s -M %s  -i %s",cgenFiltString ,"0.0.0.0",6, "0.0.0.0", iPortNum ,cLocalHostIPAddr, cDMask,cInterfaceName); 
   }else
   {
    sprintf(cAddRuleBuff,"%s -s %s -c %d -m %s -O eq -P %d  -d %s -M %s  -i %s",
        cgenFiltString ,cBuff,6, cSMask , iPortNum ,cLocalHostIPAddr,
            cDMask,cInterfaceName); 
   }

    UM_InsertRule(cStopRule);
    iResult = UM_InsertRule(cAddRuleBuff);

    if(iResult == NASC_SUCCESS)
    {
         UM_Log("UM_AddRule :Rule has been added successfully \n");	    
	 
	     UM_InsertToList(pstGenFilt->iIndex,stListPtr,pstGenFilt->iPortNum,
            cBuff, pstGenFilt->iFlag);

    }else
    {
         
         UM_Log("UM_AddRule :Error in adding the rule \n");	    
		 return NASC_FAILURE;
    }
    UM_InsertRule(cActivateRule);
    FreeStAllowUM(pstGenFilt);
    return NASC_SUCCESS;
}

void FreeStAllowUM( stAllowUM  *pstGenFilt)
{ 
       if(pstGenFilt == NULL)	
       {
            return;	       
       }
       if(pstGenFilt->pcIpAddr != NULL)
       {
            free(pstGenFilt->pcIpAddr);
            pstGenFilt->pcIpAddr = NULL;
       }
       free(pstGenFilt);
       pstGenFilt= NULL;
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
    system(cpRuleStr);
    return NASC_SUCCESS;

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
    char cIPAddr[15];
    I16 iResult;
    stRetStruct* pstListModPtr;
    UI8 *pcTempPtr;
    I16 iPortFlag =0;
    I16 iIPAddrFlag=1;
    I16 iRulNum;
    
     
    /*Get the position of the rule in the List*/
    pstListModPtr = UM_GetPosFromList(pstGenTable->iIndex, stListPtr, pstGenTable->iFlag);

    if(pstListModPtr == NULL)
    {
        UM_Log("UM_ModifyRule: No Record found \n");
        FreeStAllowUM(pstGenTable); 
	return NASC_FAILURE;
    }
    /*bzero(cModRuleBuff,0,2000);*/
    /*Check no node exists */
     if(pstListModPtr->iCount ==0)
      {
#ifdef UM_DEBUG
          UM_Log("No Node found ..  continue ..");
#endif 
      }

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
        memset(cIPAddr,'\0',15);
        sprintf(cIPAddr,"%d.%d.%d.%d",(0xff& pstGenTable->pcIpAddr[0]),(0xff & 
        pstGenTable->pcIpAddr[1]), (0xff & pstGenTable->pcIpAddr[2]),
		      (0xff& pstGenTable->pcIpAddr[3]));
        iIPAddrFlag =1;	
    }else
    {
        strcpy(cIPAddr ,pstListModPtr->modStruct->pcIPAddr);	    
    }
    
	//iRulNum = (pstListModPtr->iCount+1);
	iRulNum = (pstListModPtr->iCount +1);

    sprintf(cModRuleBuff,"%s -n %d -s %s -c %d -m %s -d %s -M %s  -i %s -O eq -P %d ",
       cChFiltString,iRulNum,cIPAddr,6, cSMask ,cLocalHostIPAddr,
          cDMask,cInterfaceName, iPortNum); 

    UM_InsertRule(cStopRule);
    iResult = UM_InsertRule(cModRuleBuff);
    if(iResult == NASC_SUCCESS)
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
             pstListModPtr->modStruct->pcIPAddr = malloc(strlen(pstGenTable->pcIpAddr)+1);
             if(pstListModPtr->modStruct->pcIPAddr ==  NULL)
             {
                 UM_Log("UM_ModifyRule :Memory allocation failed \n ");   		
                 UM_InsertRule(cActivateRule);
                 return NASC_FAILURE;		
             }
             memset(pstListModPtr->modStruct->pcIPAddr,'\0',strlen(pstGenTable->pcIpAddr)+1);
             memcpy(pstListModPtr->modStruct->pcIPAddr,pstGenTable->pcIpAddr,
                  strlen(pstGenTable->pcIpAddr));	    
             free(pcTempPtr);
         }

    }else
    {
          UM_Log("UM_ModifyRule :Error in modifying the rule \n");	    
    }
    UM_InsertRule(cActivateRule);
    FreeStAllowUM(pstGenTable); 
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
    I16 iDelRulNum;	
    I16 iRuleNumPos;	
    UI8 cDelRuleBuff[20];
    I16 iResult;

    /*Get the list position*/
    iDelRulNum = UM_GetPos(pstGenFilt->iIndex ,stListPtr,pstGenFilt->iFlag); 

    if(iDelRulNum ==0)
    {
#ifdef BM_DEBUG
        UM_Log(" No Rule found \n ");
#endif 
       FreeStAllowUM(pstGenFilt); 
       return NASC_FAILURE;
    }
    
	/*Get the rule number*/
	/*Adding this to 1 because initial Rule number will start from 2 */
    iRuleNumPos = (iDelRulNum+1);

    memset(cDelRuleBuff,0,20);

    sprintf(cDelRuleBuff,"%s -n  %d",cRMFiltString,iRuleNumPos);

    UM_InsertRule(cStopRule);
    iResult = UM_InsertRule(cDelRuleBuff);

    if(iResult == NASC_SUCCESS)
    {
         UM_Log("UM_DelRule: Rule has been Deleted successfully \n");
	     iResult =0;

         iResult = UM_DeleteFromList(stListPtr,pstGenFilt->iIndex, pstGenFilt->iFlag);
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
          FreeStAllowUM(pstGenFilt); 
          return NASC_FAILURE;
    }
    UM_InsertRule(cActivateRule);
    FreeStAllowUM(pstGenFilt); 
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
         return;
    }
    /*Allocate Memory for Generic table structure*/

    pstGenTable =malloc(sizeof(stAllowUM));
    if(pstGenTable == NULL)
    {
#ifdef UM_DEBUG	   
         UM_Log("Memory allocation failed \n "); 			
#endif       
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
                   UM_Log("UM_UserMeetering : Other record received record \n");
#endif       
                   break;
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
          case 1:
                 iDecisionFlag = UM_ADD;
                 break;
          /*case 5:
                 iDecisionFlag = UM_MODIFY;
                 break;*/
          case 6:
          case 2:
                 iDecisionFlag = UM_DELETE;
                 break;
          default:
                 iDecisionFlag = UM_MODIFY;
                 break;
    }
    
    UM_CreateFilter(pstGenTable, iDecisionFlag);
     /*Free the response PDU*/
    if(respPDU != NULL)
    {
         snmp_free_pdu(respPDU);	       
         respPDU = NULL;
    } 
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
    indexLenFTP  =  sizeof(allowFTPTable_variables_oid) / sizeof (int);
    indexLenSMTP =  sizeof(allowSMTPTable_variables_oid) / sizeof (int);
    indexLenPOP3 =  sizeof(allowPOP3Table_variables_oid) / sizeof (int);

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
   while(1){
        snmpFds=0;
        blocking=1;
        //FD_ZERO(&snmpSet);
        snmp_sess_select_info(tSess, &snmpFds,&snmpSet,&tv,&blocking);
        snmpFds = select(snmpFds,&snmpSet,0,0,0);
	if(snmpFds == -1)
	{
             if(snmpFds == EBADF)
	     {
#ifdef UM_DEBUG
                   UM_Log("An invalid file descriptor \n ");
#endif
	     }else if(snmpFds == EINTR)
	     {
#ifdef UM_DEBUG
                 UM_Log("A non blocked signal was caught \n");		     
#endif
	     }else if(snmpFds == ENOMEM)		     
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
#ifdef UM_DEBUG
           // UM_Log("Inside  READ TRAP WHILE......\n");
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
int main()
{
    pthread_t thID;
    stListPtr = UM_MakeEmpty(NULL);

	/*Perform the mkdev operations */
    UM_InsertRule(cMakeFilt);

    memset(cLocalHostIPAddr,0,18);

    /*Get the IP Address */
   UM_GetLocalHostIp(cLocalHostIPAddr);
    
#ifdef UM_DEBUG
   printf("Local Host Address %s \n", cLocalHostIPAddr);
#endif 
  
   /*Remove all default filter rules*/
    UM_InsertRule(cStopRule);
    UM_InsertRule(cRemoveAll);
    UM_InsertRule(cRemoveAll);
    UM_InsertRule(cActivateRule);

    UM_InsertRule(cStopRule);
	/*Add the basic rule for blocking FTP / SMTP / POP3 / TELNET */
    UM_InsertRule(cBlockTELNET);
    /*Add the basic rule for blocking FTP / SMTP / POP3 / TELNET */
    UM_InsertRule(cBlockFTP);
	/*Add the basic rule for blocking FTP / SMTP / POP3 / TELNET */
    UM_InsertRule(cBlockPOP3);
	/*Add the basic rule for blocking FTP / SMTP / POP3 / TELNET */
    UM_InsertRule(cBlockSMTP);
	/*Activate above rules */
    UM_InsertRule(cActivateRule);

	/*Open the session with the NASCenter sub agent*/	
    UM_InitSession();
    pthread_create(&thID,NULL,(void*)UM_Read_Packets,NULL);
    UM_StartUserMetering();
    pthread_join(thID, NULL);
    return 1;
}

void UM_GetLocalHostIp(char *pcSysIp)
{
    struct hostent *pstHostEnt=NULL;
    struct sockaddr_in stInAddr={0}; 
 
    struct utsname stHostName;
    memset(&stHostName,0,sizeof(struct utsname));
 
    // Get the localhost name
    uname (&stHostName);
 

#ifdef UM_DEBUG
    printf("Inside GetLocalhost Name: %s\n",stHostName.nodename);
#endif
    if ((pstHostEnt = gethostbyname(stHostName.nodename)) == NULL)
    {
        snmp_log_perror("error resolving hostname..");
        return;
   }
    memcpy(&stInAddr.sin_addr, pstHostEnt->h_addr_list[0], pstHostEnt->h_length);
 
    // Convert it to IP string
    strcpy(pcSysIp,(char*)inet_ntoa(stInAddr.sin_addr));


#ifdef UM_DEBUG
 printf("UM %s\n",pcSysIp); 
#endif

 
    return;
 
}

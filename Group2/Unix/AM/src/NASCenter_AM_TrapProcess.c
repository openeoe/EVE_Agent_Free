/*NASCenter_AM_TrapProcess.c
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Vijaigeetha C.
 *
 * description :
 *      
 *      
 *
 * TODO:
 *      1. 
 *      
 *      2. 
 *
 * CM Details:
 *
 * $Author: karthis $
 * $Log: NASCenter_AM_TrapProcess.c,v $
 * Revision 1.13  2004/01/14 08:37:06  karthis
 * memory leak fixed
 *
 * Revision 1.12  2004/01/06 11:29:53  baravind
 * Added flag to List(stapp,int) TRU64
 *
 * Revision 1.11  2004/01/05 11:48:18  baravind
 * puts AM_DEBUG FLAG
 *
 * Revision 1.10  2004/01/01 11:26:57  karthis
 * code review comments updated
 *
 * Revision 1.9  2003/12/29 09:56:29  karthis
 * removed reference purpose comments for code review
 *
 * Revision 1.8  2003/12/29 05:14:10  karthis
 * Debug flag AM_DEBUG has been included karthi
 *
 * Revision 1.6  2003/12/23 11:57:11  karthis
 * Modified
 *
 * Revision 1.5  2003/12/19 03:49:58  testteam
 * Link List Modifed
 *
 * Revision 1.4  2003/12/18 09:02:29  testteam
 * link list modified
 *
 * Revision 1.2  2003/12/16 08:41:05  testteam
 * Updated
 *
 * Revision 1.1  2003/11/20 05:37:37  karthis
 * minor
 *
 * Revision 1.8  2003/11/18 11:34:50  vijaige
 * 18th November
 *
 * Revision 1.6  2003/11/05 08:57:17  vijaige
 * after set and get working
 *
 * 
 * 
 * $Id: NASCenter_AM_TrapProcess.c,v 1.13 2004/01/14 08:37:06 karthis Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/
#include <stdio.h>
#include <pthread.h>


/*NetSnmp includes */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>


#include "ctypes.h"


#include "NASCenter_AM_applicationmetering.h"
#include "NASCenter_AlertMessage.h"
#include "NASCenter_Errors.h"







/********************* prototypes for local functions ***********************/

BOOL  AM_IsQueue_Empty(SCBuffer *);
BOOL  AM_IsQueue_Full(SCBuffer *);
I16 AM_CheckField( netsnmp_variable_list * );
I16 AM_FillStruct(I8  **,  netsnmp_variable_list *);
UI32 AM_GetIndex(netsnmp_variable_list *);


/*****************************************************************************
 *
 * name             :   AM_ListenTrap 
 * description      :   Listens the trap message and copy the same in
 *                      to the circular buffer 
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   NASC_SUCCESS 
 * global variables :   - 
 * calls	    :   AM_IsQueue_Full 
 * 		        
*****************************************************************************/
int AM_ListenTrap(int id, struct snmp_session *sess, int req, netsnmp_pdu *pdu, void *arg)
{
    netsnmp_pdu *pstPDUClone;

    /* Call back function to write the trap message in to circular queue */ 	
    if(pdu == NULL)
    {
         return 0;	    
    }
#ifdef AM_DEBUG
    printf("Received Trap message \n ");
#endif

    pthread_mutex_lock(&(stCB.queue_lock));
    if(AM_IsQueue_Full(&stCB))
    {
    #ifdef AM_DEBUG
      printf("Dropping the Trap Msg \n");
    #endif
      pthread_mutex_unlock(&(stCB.queue_lock));
      /*Error should be logged in the syslog */
      return -1;
    }
    stCB.iQRear = ++(stCB.iQRear) % (MAX_ELEMENT);
    pstPDUClone = snmp_clone_pdu(pdu);
    stCB.queueEntry[stCB.iQRear] = pstPDUClone;
    pthread_cond_signal(&(stCB.queue_not_empty)); 
    pthread_mutex_unlock(&(stCB.queue_lock));
    return 1;
                                                                    
}

/*****************************************************************************
 *
 * name             :   AM_init_CBuffer
 * description      :   This function is for Initializing the cricular buffer
                        
 * input parameters :   None
 *                      
 * return type      :   None
 * global variables :   -
 * calls            :   
 */
 void AM_init_CBuffer(SCBuffer *psttrapbuffer)
{
  I16 iCount;	
  psttrapbuffer->iQRear = -1;
  psttrapbuffer->iQFront = 0;
  pthread_mutex_init(&(psttrapbuffer->queue_lock) , NULL);
  pthread_cond_init(&(psttrapbuffer->queue_not_full) , NULL);
  pthread_cond_init(&(psttrapbuffer->queue_not_empty) , NULL);
  for(iCount =0;iCount <MAX_ELEMENT;iCount++)
  {
     psttrapbuffer->queueEntry[iCount] = NULL;
  }
}

/*****************************************************************************
 *
 * name             :   AM_ProcessTrapMsg 
 * description      :   Copy the trap PDU and process the same  
 * input parameters :   pstBuffer -> pointer to the circular buffer 
 * return type      :   void 
 * global variables :   - 
 * calls	    :   AM_IsQueueEmpty
 *                      AM_ParseTrap   
 * 		        
*****************************************************************************/
void AM_ProcessTrapMsg(void *args)
{
  I16 i = 0;
  netsnmp_pdu *trapPDU;
  netsnmp_pdu *pstClonePDU;
  
  
  /*Thread function to process the trap message*/
  while(1)
  {
       pthread_mutex_lock(&(stCB.queue_lock));
       i = 1;
    #ifdef AM_DEBUG
       printf("Enter into queue \n");
    #endif
       if(AM_IsQueue_Empty(&stCB) == TRUE)
       {
    #ifdef AM_DEBUG
          puts("AM_ProcessTrapMsg Queue Empty.. waiting for signal");
    #endif
           pthread_cond_wait(&(stCB.queue_not_empty) ,
               &(stCB.queue_lock));
    #ifdef AM_DEBUG
		   puts("After AM_ProcessTrapMsg Queue Empty.. waiting for signal");
    #endif
           pthread_mutex_unlock(&(stCB.queue_lock));
           i = 0;
       }
       if ( i == 0)
       {
           pthread_mutex_lock(&(stCB.queue_lock));
       }
       trapPDU =  stCB.queueEntry[stCB.iQFront]; 
       pstClonePDU= snmp_clone_pdu(trapPDU);
       snmp_free_pdu(trapPDU);  
       stCB.queueEntry[stCB.iQFront] = NULL;  

       stCB.iQFront = ++(stCB.iQFront) % (MAX_ELEMENT); 
       pthread_mutex_unlock(&(stCB.queue_lock));
       /*Call function to parse the retrieved trap message */     
       AM_ParseTrap(pstClonePDU);
       snmp_free_pdu(pstClonePDU);
  }
}
/*****************************************************************************
 *
 * name             :   AM_isQueue_Empty
 * description      :   This function is for checking if the queue is empty
                        
 * input parameters :   None
 *                      
 * return type      :   None
 * global variables :   -
 * calls            :   
 */
BOOL  AM_IsQueue_Empty(SCBuffer *pstcbuffer)
{
  BOOL status = FALSE;
  if(((pstcbuffer->iQRear + 1) % (MAX_ELEMENT)) == pstcbuffer->iQFront)
  {
    status = TRUE;
  }
  return status;
}

/*****************************************************************************
 *
 * name             :   AM_isQueue_Full
 * description      :   This function is for checking if the queue is full                        
 * input parameters :   None
 *                      
 * return type      :   None
 * global variables :   -
 * calls            :   
 */
BOOL  AM_IsQueue_Full(SCBuffer *pstcbuffer)
{
  BOOL status = FALSE;
  
  if(((pstcbuffer->iQRear + 2) % (MAX_ELEMENT )) == pstcbuffer->iQFront)
  {
    status = TRUE;
  }
  return status;

}


/*****************************************************************************
 *
 * name             :   AM_ParseTrap 
 * description      :   Parse the trap message and calls the function for 
 *                      creating the rule  
 * input parameters :   respPDU -> pointer to the snmp_pus structure 
 * return type      :   void 
 * global variables :   - 
 * calls	    :  
 *                     
 *                   
*****************************************************************************/
void AM_ParseTrap( struct snmp_pdu *respPDU)
{
    I16 iDecisionFlag;
    netsnmp_variable_list *vars;
    netsnmp_variable_list *pstCurrVars;
    I16 iFieldFlag= -1 ;
    StappMeteringStruct *pappMet;    
	I8 *pcbuf;
	I16 iOperFlag;

    
    vars = respPDU->variables;
    pstCurrVars = vars;
#ifdef AM_DEBUG
    printf("AM_ParseTrap called \n"); 
#endif
    iOperFlag = AM_CheckTable(pstCurrVars);
    if(iOperFlag == -1)
    {
    #ifdef AM_DEBUG
         printf("Not belong to this table \n");	    
    #endif
         return;
    }

    /*Allocate Memory for Generic table structure*/
    pappMet=malloc(sizeof(StappMeteringStruct));
    if(pappMet == NULL)
    {
    #ifdef AM_DEBUG
        printf("Memory allocation failed \n "); 			
    #endif

         NASC_INSUFFICIENT_MEMORY ;
    }
     if(pstCurrVars != NULL)
    {
         /*Get the Index ID of the table*/
         pappMet->uiappMeteringIndex = AM_GetIndex(pstCurrVars);
    }else
    {
         free(pappMet);
         #ifdef AM_DEBUG
             printf("No record exists \n");	    
        #endif
	 return ;
    }
    
    
    pappMet->pcappMeteringID=NULL;
    pappMet->ulappMeteringEndTime=0;
    pappMet->ulappMeteringStartTime=0;
    pappMet->iappMeteringReaction=0;
    pappMet->pcappMeteringBinaryName=NULL;
    pappMet->pcalertmessage=NULL;
    pappMet->istatus=0;



    while(pstCurrVars != NULL) 
    {
		iFieldFlag = AM_CheckField(pstCurrVars);  	    
		switch(iFieldFlag)
        {

			case AM_ID:
                   /*Get the AM ID */
					if((AM_FillStruct(&(pappMet->pcappMeteringID),pstCurrVars))==0)
					{	
                    #ifdef AM_DEBUG
						printf("AM_ID is Null \n"); 			   
                    #endif  
						free(pappMet);
						return ;		      
					}
					break;
             case AM_APPSTARTTIME:
                   /*Get the start time */
					pcbuf = (I8 *) malloc(pstCurrVars->val_len);					
					if(pcbuf==NULL)
						return ;
					memcpy(pcbuf,pstCurrVars->val.string,pstCurrVars->val_len);				
					Util_Convert_Date_To_Long(pcbuf,&(pappMet->ulappMeteringStartTime));
					free(pcbuf);
					break;		     
             case AM_APPENDTIME:
                   /*Get the End Time */
					pcbuf = (I8 *) malloc(pstCurrVars->val_len);					
					if(pcbuf==NULL)
						return; 
					memcpy(pcbuf,pstCurrVars->val.string,pstCurrVars->val_len);				
					Util_Convert_Date_To_Long(pcbuf,&(pappMet->ulappMeteringEndTime));
					free(pcbuf);
					break;
             case AM_APPREACTION:
                   /*Get the AppReaction*/
					pappMet->iappMeteringReaction=(I16)(*pstCurrVars->val.integer);
					break;
             case AM_APPBINNAME:
                   /*Get the End Time*/
          			if((AM_FillStruct(&(pappMet->pcappMeteringBinaryName),pstCurrVars))==0)
					{	
                    #ifdef AM_DEBUG
						printf("APPBINNAME is Null \n"); 			   
                    #endif
                        free(pappMet->pcappMeteringID);
						free(pappMet);
						return ;		      
					}
					break;

			 case  AM_ALERTMESSG:
          	   /*Get the Alert Message*/
					if((AM_FillStruct(&(pappMet->pcalertmessage),pstCurrVars))==0)
					{	
                    #ifdef AM_DEBUG
						printf("Alert Message is Null \n"); 			   
                    #endif
                        free(pappMet->pcappMeteringID);
                        free(pappMet->pcappMeteringBinaryName);
						free(pappMet);
						return ;		      
					}
					break;

             case AM_APPSTATUS:
                   /*Get the row status */
                    pappMet->istatus= (I16)(*pstCurrVars->val.integer);  
					break;
             default :
                   printf("Received other records so continue..\n");
           }

	    /*Move to the next record */ 
	    pstCurrVars = pstCurrVars->next_variable;
    }
  
    /*Check any field is set */
   
   
    if((pappMet->pcappMeteringID==NULL) && (pappMet->ulappMeteringEndTime==0)
        && (pappMet->ulappMeteringStartTime==0) && (pappMet->iappMeteringReaction==0)
        && (pappMet->pcappMeteringBinaryName==NULL) && (pappMet->pcalertmessage==NULL))
	{
    #ifdef AM_DEBUG
        printf("It is an Dummy record and return \n");	    
    #endif  
		free(pappMet);
		return ;
    }

 
    /*Fine the decision flag*/
    /*Check record is for Add / Modify / Del */
    switch(pappMet->istatus)
    {
        case 4:
        case 1:
              //iDecisionFlag = AM_ADD;
            #ifdef AM_DEBUG   
                printf("Trap processed calling add list function\n");
            #endif
               List(pappMet,0);
               break;
        case 2:
        case 6:
             //iDecisionFlag = AM_DELETE;
            #ifdef AM_DEBUG
                printf("Trap Processed calling Delete List function\n");
            #endif
               Delete(pappMet);
               break;
        default:
              // iDecisionFlag = AM_MODIFY;
            #ifdef AM_DEBUG
                printf("Trap Processed calling Modify List function\n");
            #endif
                 Modify(pappMet);
                 break;
    }
if(pappMet->pcappMeteringBinaryName)
free(pappMet->pcappMeteringBinaryName);
if(pappMet->pcappMeteringID)
free(pappMet->pcappMeteringID);
if(pappMet->pcalertmessage)
free(pappMet->pcalertmessage);
if(pappMet)
free(pappMet);
    return ;
}


/*****************************************************************************
 *
 * name             :   AM_FillStruct
 * description      :   
 * input parameters :   I8  **pcDst,  netsnmp_variable_list *pstCurrVars
 * return type      :   I16
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/

I16 AM_FillStruct(I8  **pcDst,  netsnmp_variable_list *pstCurrVars)
{
        I16 iLen;	
        iLen =0;
        iLen =  pstCurrVars->val_len;
	if(iLen ==0)
	{
             *pcDst=NULL;
             return NASC_FAILURE;		
	     
	}
        *pcDst  = malloc(iLen+1);
        if(*pcDst == NULL)
        {
        #ifdef AM_DEBUG
           printf("Memory allocation failed \n ");	
        #endif
           return NASC_INSUFFICIENT_MEMORY;
        }
        memset(*pcDst,0,(iLen+1));
        memcpy(*pcDst,pstCurrVars->val.string,iLen);
	return NASC_SUCCESS;
}
/*****************************************************************************
 *
 * name             :   AM_GetIndex
 * description      :   
 * input parameters :   netsnmp_variable_list *pstCurrVars 
 * return type      :  
 * global variables :   - 
 * calls	    :   - 
 * 		        
*****************************************************************************/


UI32 AM_GetIndex(netsnmp_variable_list *pstCurrVars)
{
       I16 iLen=0;
       UI32 iIndex=-1;
       iLen = pstCurrVars->name_length-1;
       /*Get the Index ID of the table*/
       iIndex = pstCurrVars->name[iLen];
       return  iIndex;
}



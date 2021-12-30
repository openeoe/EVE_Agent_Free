/*****************************************************************************
 *
 * NASCenter_UM_List.c
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
 * $Log: NASCenter_UM_Linux_List.c,v $
 * Revision 1.6  2004/01/29 06:32:18  thiagav
 * Memory leak checked and reviewed by venkat
 *
 * Revision 1.5  2003/12/09 05:06:52  thiagav
 * *** empty log message ***
 *
 * Revision 1.4  2003/11/08 10:35:48  thiagav
 * Working for add /modify / delete operation for telnet table
 *
 * Revision 1.3  2003/11/07 17:34:48  thiagav
 * Tested for addition , modification of firewall rules
 *
 * Revision 1.2  2003/11/05 09:21:04  thiagav
 * Compiled and added to CVS
 *
 * Revision 1.1  2003/11/05 08:54:54  thiagav
 * Compiled code is added in to CVS
 *
 * Revision 1.5  2003/11/05 05:42:57  thiagav
 * Added comments and followed coding standards
 *
 * Revision 1.4  2003/11/04 03:30:15  thiagav
 * Added after header addition
 *
 *
 * $Id: NASCenter_UM_Linux_List.c,v 1.6 2004/01/29 06:32:18 thiagav Exp $
 *
 ****************************************************************************/

#include "NASCenter_UM_Linux_List.h"


/*****************************************************************************
 *
 * name             :   UM_MakeEmpty
 * description      :   Initializes the header node and returns the same
 * input parameters :   stListPtr-> pointer to the stList 
 * return type      :   stList Pointer to the List 
 * global variables :   - 
 * calls            :   - 
*****************************************************************************/

stList UM_MakeEmpty(stList stListPtr) {

  if (stListPtr != NULL) {
    UM_DeleteList(stListPtr);
  }

  stListPtr = malloc( sizeof(struct stNode) );

  if (stListPtr == NULL) {
    UM_FatalError ("UM_MakeEmpty Error: Unable to allocate more memory.");
  }

  stListPtr->Next = NULL;
  stListPtr->iIndex = 0;
  stListPtr->iRuleNum = 0;
  return stListPtr;
}

/*****************************************************************************
 *
 * name             :   UM_FatalError
 * description      :   Print the error messages
 * input parameters :   ErrorMessage -> pointer to the error message string 
 * return type      :   - 
 * global variables :   -
 * calls            :   
*****************************************************************************/
void UM_FatalError (char* ErrorMessage) {
  printf ("%s\n", ErrorMessage);
  exit;
}

/*****************************************************************************
 *
 * name             :   UM_Insert
 * description      :   Inserts the Node at the end of the List
 * input parameters :   Index -> Index value of the table 
 *                      RuleNum-> Rule number associated with record 
 *                      Flag-> Rule number associated with record 
 * return type      :   - 
 * global variables :   -
 * calls            :   
*****************************************************************************/
void UM_Insert( ElementType Index, ElementType iFlag, ElementType RuleNum, stList stListPtr, 
     struct ip_fwnew *insertRecordPtr ) {

  stPosition TmpCell;
  stList tempPtr;

  TmpCell = malloc( sizeof( struct stNode ) );
  if (TmpCell == NULL) {
    UM_FatalError("Out of space!!!");
  }
  if((TmpCell->ipfwPtr = malloc (sizeof(struct ip_fwnew))) == NULL)
  {
    UM_FatalError("Out of space!!!");
    return;
  }
  
  TmpCell->iIndex = Index;
  TmpCell->iRuleNum= RuleNum;
  TmpCell->iFlag = iFlag;
  TmpCell->ipfwPtr= insertRecordPtr ;

  /*Always insert after head pointer*/
  tempPtr = stListPtr;

  TmpCell->Next = tempPtr->Next;
  tempPtr->Next =TmpCell; 

}

/*****************************************************************************
 *
 * name             :   UM_GetstructForMod
 * description      :   Returns the record for modification with respect to 
 *                      Index number 
 * input parameters :   Index -> Index value of the table 
 *                      stHeadPtr-> Pointer to the stList Head
 * return type      :   returns ip_fwnew structure 
 * global variables :   -
 * calls            :   - 
*****************************************************************************/
stRetStruct * UM_GetstructForMod(I16 iIndex , stList stHeadPtr ,I16 iFlag)
{
       stList stPrevPtr;
       I16 iCount=0;	       
       I16 iFoundFlag;
       stRetStruct *pstRetStruct;

       stPrevPtr = stHeadPtr->Next;
       iFoundFlag =0;

       /*allocate memory for pstRetStruct structure */
       pstRetStruct = malloc(sizeof(stRetStruct));
       if(pstRetStruct == NULL)
       {
            exit(0);  		       
       }
       while(stPrevPtr != NULL)
       {
         iCount++;
         if((stPrevPtr->iIndex == iIndex) && (stPrevPtr->iFlag == iFlag )) 	 
         {
               iFoundFlag =1;		 
               pstRetStruct->ipfwPtr = stPrevPtr->ipfwPtr;
               pstRetStruct->iCount  = iCount;
               break;  		 
         }
         stPrevPtr =stPrevPtr->Next;
       }
  if(iFoundFlag == 0)       
  {
       pstRetStruct->ipfwPtr = NULL;
       pstRetStruct->iCount = 0;
  }
  return pstRetStruct;
}

/*****************************************************************************
 *
 * name             :   UM_DelRuleNumFromList 
 * description      :   Returns the rule number to be deleted  
 * input parameters :   Index -> Index value of the table 
 *                      stHeadPtr-> Pointer to the stList Head 
 * return type      :   returns rul number 
 * global variables :   -
 * calls            :   - 
*****************************************************************************/
I16 UM_DelRuleNumFromList(I16 iIndex, stList stHeadPtr) 
{
  stList stModstruct;	 
  stModstruct = stHeadPtr;
  
  while(stModstruct != NULL && stModstruct->iIndex != iIndex ) {
    stModstruct = stModstruct->Next;
  }
  if(stModstruct->iIndex == iIndex )
  {	  
       return stModstruct->iRuleNum;
  }else 
  {
       printf("Record not found \n ");	  
       return 0;
  }
}

/*****************************************************************************
 *
 * name             :   UM_ModifyList 
 * description      :   Modifies the List record 
 * input parameters :   insertRecordMod-> Record to be modified 
 *                      stListPtr-> Pointer to the stList Head 
 * return type      :   SUCCESS / FAILURE 
 * global variables :   -
 * calls            :   - 
*****************************************************************************/
I16  UM_ModifyList(stList stListPtr,  struct ip_fwnew *insertRecordMod,
       I16 iIndex)
{
       stList stModstruct;
       stList tempPtr;
       stModstruct= stListPtr;

       while(stModstruct != NULL && stModstruct->iIndex != iIndex ) {
         stModstruct = stModstruct->Next;
       }
       if(stModstruct->iIndex == iIndex )
       {	  
            tempPtr = stModstruct;
            stModstruct->ipfwPtr = insertRecordMod;
	    free(tempPtr);
            return 1;
       }else 
       {
            printf("Record not found \n ");	  
            return 0;
       }
}
	
/*****************************************************************************
 *
 * name             :   UM_DeleteFromList 
 * description      :   Delete the record from List 
 * input parameters :   iIndex-> Record index number 
 *                      stListPtr-> Pointer to the stList Head 
 * return type      :   SUCCESS / FAILURE 
 * global variables :   -
 * calls            :   - 
*****************************************************************************/
I16  UM_DeleteFromList(stList stListPtr, I16 iIndex , I16 iFlag)
{
       stList stDelStruct;
       stList stPrevPtr;
       struct ip_fwnew *delPtr; 
       stList tempPtr;
       I16 iFoundFlag =0;

       stPrevPtr = stListPtr;
       stDelStruct= stListPtr->Next;

       while(stDelStruct != NULL )
       {
           if((stDelStruct->iIndex == iIndex) && (stDelStruct->iFlag == iFlag))	       
           {
		iFoundFlag =1;   
                break; 		   
           }
           stPrevPtr=stDelStruct;           	       
           stDelStruct = stDelStruct->Next;
       }
       if(iFoundFlag ==1)
       {	  
            tempPtr = stDelStruct;
	    stPrevPtr->Next = stDelStruct->Next; 
	    delPtr = tempPtr->ipfwPtr;  

            if(delPtr != NULL)
	    {
	        free(delPtr); 
	        delPtr = NULL;
	    }
            if(tempPtr != NULL)
	    {
	        free(tempPtr);
	        tempPtr = NULL;
            }
            return 1;
       }else 
       {
            printf("Record not found \n ");	  
            return 0;
       }
}

I16  UM_GetPos(I16 iIndex , stList stListPtr , I16 iFlag)
{
       I16 iCount= 0;	       
       I16 iFoundFlag = 0;	       
       
       stList stPrevPtr;

       stPrevPtr = stListPtr->Next;

       while(stPrevPtr != NULL)
       {
         iCount++;
         if((stPrevPtr->iIndex == iIndex) && (stPrevPtr->iFlag == iFlag)) 	 
         {
               iFoundFlag=1;                		 
               break;  		 
         }
         stPrevPtr =stPrevPtr->Next;
       }
       if(iFoundFlag == 0)
       {
            iCount =0;	       
       }
       return iCount;
}
/*****************************************************************************
 *
 * name             :   UM_DeleteList 
 * description      :   Delete the entire list 
 * input parameters :   stListPtr-> Pointer to the stList Head 
 * return type      :   - 
 * global variables :   -
 * calls            :   - 
*****************************************************************************/
void UM_DeleteList(stList stListPtr) {

  stPosition pstNewNode, Tmp;

  /* Header assumed */
  pstNewNode = stListPtr->Next;
  stListPtr->Next = NULL;
  while(pstNewNode != NULL) {
    Tmp = pstNewNode->Next;
    free(pstNewNode);
    pstNewNode = Tmp;
  }
}

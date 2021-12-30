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
 * $Log: NASCenter_UM_IBM_List.c,v $
 * Revision 1.6  2003/12/30 04:49:16  thiagav
 * Added
 *
 * Revision 1.5  2003/12/10 05:09:29  thiagav
 * Added mkfilt genfilt , all filter rule fiunctionalities
 *
 * Revision 1.4  2003/11/08 13:27:57  thiagav
 * *** empty log message ***
 *
 * Revision 1.3  2003/11/05 08:57:29  thiagav
 * Added in to CVS
 *
 * Revision 1.1  2003/11/05 06:38:22  thiagav
 * Compiled , comments , header added
 *
 *
 *
 * $Id: NASCenter_UM_IBM_List.c,v 1.6 2003/12/30 04:49:16 thiagav Exp $
 *
 ****************************************************************************/

#include "NASCenter_UM_IBM_List.h"

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
}

/*****************************************************************************
 *
 * name             :   UM_InsertToList
 * description      :   Inserts the Node at the end of the List
 * input parameters :   Index -> Index value of the table 
 *                      RuleNum-> Rule number associated with record 
 *                      Flag-> Rule number associated with record 
 * return type      :   - 
 * global variables :   -
 * calls            :   
*****************************************************************************/
void UM_InsertToList( ElementType Index, stList stListPtr, I16  portNum, char *pcIPAddr , I16 iFlag) 
{

  stPosition TmpCell;
  stList tempPtr;

  TmpCell = malloc( sizeof( struct stNode ) );
  if (TmpCell == NULL) {
    UM_FatalError("Out of space!!!");
  }
  
  TmpCell->pcIPAddr = malloc(strlen(pcIPAddr) +1);
  memset(TmpCell->pcIPAddr,'\0',strlen(pcIPAddr) +1);

  if(TmpCell->pcIPAddr == NULL)
  {
    UM_FatalError("Out of space!!!");
  }
  
  TmpCell->iIndex = Index;
  TmpCell->iFlag = iFlag;
  memcpy(TmpCell->pcIPAddr,pcIPAddr,strlen(pcIPAddr));
  //strcpy(TmpCell->pcIPAddr,pcIPAddr);

  /*Always insert after head pointer*/
  tempPtr = stListPtr;

  TmpCell->Next = tempPtr->Next;
  tempPtr->Next =TmpCell; 

}


/*****************************************************************************
 *
 * name             :   UM_GetPosFromList
 * description      :   With respect to index Id this function
 *                      returns the Node  
 * input parameters :   Index -> Index value of the table 
 *                      stHeadPtr-> Pointer to the head Node 
 * return type      :   - 
 * global variables :   -
 * calls            :   
*****************************************************************************/
stRetStruct* UM_GetPosFromList(I16 iIndex , stList stHeadPtr, I16 iFlag)
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
               pstRetStruct->modStruct = stPrevPtr;
               pstRetStruct->iCount  = iCount;
               break;  		 
         }
         stPrevPtr =stPrevPtr->Next;
       }
  if(iFoundFlag == 0)       
  {
       pstRetStruct->modStruct = NULL;
       pstRetStruct->iCount = 0;
  }
  return pstRetStruct;

}



/*****************************************************************************
 *
 * name             :   UM_DelRuleNumFromList
 * description      :   Finds the rule number with respect to index Id 
 *                      and return the same 
 * input parameters :   iIndex -> Index value of the table 
 *                      stHeadPtr-> Pointer to the head Node 
 * return type      :   - 
 * global variables :   -
 * calls            :   
*****************************************************************************/
I16  UM_DeleteFromList(stList stListPtr, I16 iIndex , I16 iFlag)
{
       stList stDelStruct;
       stList stPrevPtr;
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
	    free(tempPtr);
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

  stPosition P, Tmp;

  /* Header assumed */
  P = stListPtr->Next;
  stListPtr->Next = NULL;
  while(P != NULL) {
    Tmp = P->Next;
    free(P);
    P = Tmp;
  }

}

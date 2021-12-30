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
 * $Author: baravind $
 * $Log: NASCenter_UM_List.c,v $
 * Revision 1.8  2003/12/31 12:01:55  baravind
 * Fix for Sol/unixware/Hp-unix/true64
 *
 * Revision 1.7  2003/12/22 10:33:01  baravind
 * Added by Aravind.B . UM for sol , unixware and hpunix
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
 * $Id: NASCenter_UM_List.c,v 1.8 2003/12/31 12:01:55 baravind Exp $
 *
 ****************************************************************************/

#include "NASCenter_UM_List.h"

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
 A
 * name             :   UM_InsertToList
 * description      :   Inserts the Node at the end of the List
 * input parameters :   Index -> Index value of the table 
 *                      RuleNum-> Rule number associated with record 
 *                      Flag-> Rule number associated with record 
 * return type      :   - 
 * global variables :   -
 * calls            :   
*****************************************************************************/
void UM_InsertToList( ElementType Index, stList stListPtr, I16  portNum, char *pcIPAddr , I16 iFlag ,I16 iStatus) 
{

  stPosition TmpCell;
  stList tempPtr;
  stRetStruct* pstListModPtr = NULL;

  pstListModPtr = UM_GetPosFromList(Index, stListPtr, iFlag);
  if(pstListModPtr != NULL)
    {
				    if ( pstListModPtr->modStruct != NULL)
				    {
							    	printf("Error : data  in LIST ");
								    return ;
				    }
				}

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
  TmpCell->iStatus = iStatus;
  memcpy(TmpCell->pcIPAddr,pcIPAddr,strlen(pcIPAddr));
  //strcpy(TmpCell->pcIPAddr,pcIPAddr);

  /*Always insert after head pointer*/
  tempPtr = stListPtr;

		if ( tempPtr != NULL )
		{
    TmpCell->Next = tempPtr->Next;
  }
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

       if ( stHeadPtr == NULL)
       {
	   return NULL;
       }

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

I16 UM_GetPos(I16 iIndex , stList stListPtr , I16 iFlag)
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

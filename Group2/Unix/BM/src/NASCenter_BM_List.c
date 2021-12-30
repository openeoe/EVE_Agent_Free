/*****************************************************************************
 *
 * NASCenter_BM_List.c
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
 * $Log: NASCenter_BM_List.c,v $
 * Revision 1.5  2003/12/21 06:39:19  thiagav
 * Bug fixed for row atatus active and inactive conditions
 *
 * Revision 1.4  2003/12/20 04:32:57  thiagav
 * Code modified to long with respect to long value
 *
 * Revision 1.3  2003/12/15 03:37:42  thiagav
 * Added after final testing
 *
 * Revision 1.2  2003/12/02 11:33:59  thiagav
 * Added after name modification
 *
 * Revision 1.1  2003/12/02 11:21:32  thiagav
 * Added
 *
 * Revision 1.3  2003/11/29 14:29:17  guga
 * Added after unit testing
 *
 * Revision 1.1  2003/11/12 08:18:14  thiagav
 * Added first time after compilation
 *
 * $Id: NASCenter_BM_List.c,v 1.5 2003/12/21 06:39:19 thiagav Exp $
 *
 ****************************************************************************/

#include "NASCenter_BM_List.h"

#ifdef BM_DEBUG
extern void BM_Log(char *);
#endif



/*****************************************************************************
 *
 * name             :   BM_MakeEmpty
 * description      :   Initializes the header node and returns the same
 * input parameters :   stListPtr-> pointer to the stList 
 * return type      :   stList Pointer to the List 
 * global variables :   - 
 * calls            :   - 
*****************************************************************************/

stList BM_MakeEmpty(stList stListPtr) {

  if (stListPtr != NULL) {
    BM_DeleteList(stListPtr);
  }

  stListPtr = malloc( sizeof(struct stNode) );

  if (stListPtr == NULL) {
    BM_FatalError ("BM_MakeEmpty Error: Unable to allocate more memory.");
  }

  stListPtr->Next = NULL;
  stListPtr->iIndex = 0;
  stListPtr->lTaskID= 0;
  return stListPtr;
}

/*****************************************************************************
 *
 * name             :   BM_FatalError
 * description      :   Print the error messages
 * input parameters :   ErrorMessage -> pointer to the error message string 
 * return type      :   - 
 * global variables :   -
 * calls            :   
*****************************************************************************/
void BM_FatalError (char* ErrorMessage) {
#ifdef BM_DEBUG	
#ifdef BM_PRINT
  BM_Log(" ErrorMessage");
#endif  
#endif  
  exit;
}

/*****************************************************************************
 *
 * name             :   BM_AddNodeInBackupList 
 * description      :   Inserts the Node at the start of the List
 * input parameters :   taskID -> Identifier to the task 
 *                      iIndex -> Index number associated with task ID 
 *                      stListPtr -> Pointer to the stList structure 
 * return type      :   - 
 * global variables :   -
 * calls            :   
*****************************************************************************/
void BM_AddNodeInBackupList( long  taskID, I16 iIndex, stList stListPtr) {

  stPosition TmpCell;
  stList tempPtr;

  TmpCell = malloc( sizeof( struct stNode ) );
  if (TmpCell == NULL) {
    BM_FatalError("Out of space!!!");
  }
  
  TmpCell->iIndex   = iIndex;
  TmpCell->lTaskID = taskID;

  /*Always insert after head pointer*/
  tempPtr = stListPtr;

  TmpCell->Next = tempPtr->Next;
  tempPtr->Next =TmpCell; 

}

/*****************************************************************************
 *
 * name             :   BM_DeleteNodeFromList 
 * description      :   Delete the record from List 
 * input parameters :   iIndex-> Record index number 
 *                      stListPtr-> Pointer to the stList Head 
 *                      taskId -->task Id to be deleted 
 * return type      :   SUCCESS / FAILURE 
 * global variables :   -
 * calls            :   - 
*****************************************************************************/
I16  BM_DeleteNodeFromList(stList stListPtr, I16 iIndex ,long taskID )
{
       stList stDelStruct;
       stList stPrevPtr;
       stList tempPtr;
       I16 iFoundFlag =0;

       stPrevPtr = stListPtr;
       stDelStruct= stListPtr->Next;

       while(stDelStruct != NULL )
       {
           if((stDelStruct->iIndex == iIndex) && (stDelStruct->lTaskID ==taskID ))	       
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
#ifdef BM_DEBUG	
#ifdef BM_PRINT
            BM_Log("Record not found \n ");	  
#endif	    
#endif	    
            return 0;
       }
}

/*****************************************************************************
 *
 * name             :   BM_GetTaskID
 * description      :   Rteun the task ID
 * input parameters :   iIndex-> Record index number 
 *                      stListPtr-> Pointer to the stList Head 
 * return type      :    
 * global variables :   -
 * calls            :   - 
*****************************************************************************/
long  BM_GetTaskID(I16 iIndex , stList stListPtr)
{
       long lTaskID = -1;
       
       stList stPrevPtr;

       stPrevPtr = stListPtr->Next;

       while(stPrevPtr != NULL)
       {
         if((stPrevPtr->iIndex == iIndex) ) 	 
         {
	       lTaskID = stPrevPtr->lTaskID;
               break;  		 
         }
         stPrevPtr =stPrevPtr->Next;
       }
       return lTaskID;
}
/*****************************************************************************
 *
 * name             :   BM_DeleteList 
 * description      :   Delete the entire list 
 * input parameters :   stListPtr-> Pointer to the stList Head 
 * return type      :   - 
 * global variables :   -
 * calls            :   - 
*****************************************************************************/
void BM_DeleteList(stList stListPtr) {

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

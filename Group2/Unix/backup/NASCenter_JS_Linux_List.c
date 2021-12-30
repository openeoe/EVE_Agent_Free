/*****************************************************************************
 *
 * NASCenter_JS_List.c
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
 * $Log: NASCenter_JS_Linux_List.c,v $
 * Revision 1.1  2003/12/02 06:25:05  thiagav
 * added
 *
 * Revision 1.4  2003/12/02 05:52:47  thiagav
 * Added after ifdef with all OS
 *
 * Revision 1.3  2003/11/25 11:18:52  thiagav
 * Added after basic testing
 *
 * Revision 1.2  2003/11/21 06:47:47  thiagav
 * Added after testing
 *
 * Revision 1.1  2003/11/18 05:35:06  thiagav
 * Added after compilation
 *
 * $Id: NASCenter_JS_Linux_List.c,v 1.1 2003/12/02 06:25:05 thiagav Exp $
 *
 ****************************************************************************/

#include "NASCenter_JS_Linux_List.h"


/*****************************************************************************
 *
 * name             :   JS_MakeEmpty
 * description      :   Initializes the header node and returns the same
 * input parameters :   stListPtr-> pointer to the stList 
 * return type      :   stList Pointer to the List 
 * global variables :   - 
 * calls            :   - 
*****************************************************************************/

stList JS_MakeEmpty(stList stListPtr) {

  if (stListPtr != NULL) {
    JS_DeleteList(stListPtr);
  }

  stListPtr = malloc( sizeof(struct stNode) );

  if (stListPtr == NULL) {
    JS_FatalError ("JS_MakeEmpty Error: Unable to allocate more memory.");
  }

  stListPtr->Next = NULL;
  stListPtr->iIndex = 0;
  stListPtr->uiTaskID= 0;
  return stListPtr;
}

/*****************************************************************************
 *
 * name             :   JS_FatalError
 * description      :   Print the error messages
 * input parameters :   ErrorMessage -> pointer to the error message string 
 * return type      :   - 
 * global variables :   -
 * calls            :   
*****************************************************************************/
void JS_FatalError (char* ErrorMessage) {
#ifdef JS_DEBUG
  printf ("%s\n", ErrorMessage);
#endif   
  exit(0);     
}

/*****************************************************************************
 *
 * name             :   JS_AddNodeInJobList 
 * description      :   Inserts the Node at the start of the List
 * input parameters :   taskID -> Identifier to the task 
 *                      iIndex -> Index number associated with task ID 
 *                      stListPtr -> Pointer to the stList structure 
 * return type      :   - 
 * global variables :   -
 * calls            :   
*****************************************************************************/
void JS_AddNodeInJobList( UI32  taskID, I16 iIndex, stList stListPtr) {

  stPosition TmpCell;
  stList tempPtr;

  TmpCell = malloc( sizeof( struct stNode ) );
  if (TmpCell == NULL) {
    JS_FatalError("Out of space!!!");
  }
  
  TmpCell->iIndex   = iIndex;
  TmpCell->uiTaskID = taskID;

  /*Always insert after head pointer*/
  tempPtr = stListPtr;

  TmpCell->Next = tempPtr->Next;
  tempPtr->Next =TmpCell; 

  return;

}

/*****************************************************************************
 *
 * name             :   JS_DeleteNodeFromList 
 * description      :   Delete the record from List 
 * input parameters :   iIndex-> Record index number 
 *                      stListPtr-> Pointer to the stList Head 
 *                      taskId -->task Id to be deleted 
 * return type      :   SUCCESS / FAILURE 
 * global variables :   -
 * calls            :   - 
*****************************************************************************/
I16  JS_DeleteNodeFromList(stList stListPtr, I16 iIndex ,UI32 taskID )
{
       stList stDelStruct;
       stList stPrevPtr;
       stList tempPtr;
       I16 iFoundFlag =0;

       stPrevPtr = stListPtr;
       stDelStruct= stListPtr->Next;

       while(stDelStruct != NULL )
       {
           if((stDelStruct->iIndex == iIndex) && (stDelStruct->uiTaskID ==taskID ))	       
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
#ifdef JS_DEBUG
            printf("Record not found \n ");	  
#endif
            return 0;
       }
}

/*****************************************************************************
 *
 * name             :   JS_GetTaskID
 * description      :   Rteun the task ID
 * input parameters :   iIndex-> Record index number 
 *                      stListPtr-> Pointer to the stList Head 
 * return type      :    
 * global variables :   -
 * calls            :   - 
*****************************************************************************/
UI32  JS_GetTaskID(I16 iIndex , stList stListPtr)
{
       UI32 uiTaskID =0;
       
       stList stPrevPtr;

       stPrevPtr = stListPtr->Next;

       while(stPrevPtr != NULL)
       {
         if((stPrevPtr->iIndex == iIndex) ) 	 
         {
	       uiTaskID = stPrevPtr->uiTaskID;
               break;  		 
         }
         stPrevPtr =stPrevPtr->Next;
       }
       return uiTaskID;
}
/*****************************************************************************
 *
 * name             :   JS_DeleteList 
 * description      :   Delete the entire list 
 * input parameters :   stListPtr-> Pointer to the stList Head 
 * return type      :   - 
 * global variables :   -
 * calls            :   - 
*****************************************************************************/
void JS_DeleteList(stList stListPtr) {

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

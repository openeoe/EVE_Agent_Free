#ifndef __NASCENTER_ALERTMESSAGE_H__
#define __NASCENTER_ALERTMESSAGE_H__

/*****************************************************************************
 *
 * NASCenter_AlertMessage.h
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Vijaigeetha C.
 *
 * description :
 *     List of all alert messages
 *      
 *  
 *
 * CM Details:
 *
 * $Author: karthis $
 * $Log: NASCenter_AlertMessage.h,v $
 * Revision 1.1  2003/11/20 05:37:05  karthis
 * minor
 *
 * Revision 1.6  2003/11/05 08:57:02  vijaige
 * after set and get working
 *
 * 
 * 
 * $Id: NASCenter_AlertMessage.h,v 1.1 2003/11/20 05:37:05 karthis Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/
/********************************** macros *********************************/
/*Trap Header */
#define ADDROW 1
#define MODIFYROW 2
#define DELETEROW 3

/*Alert Messages in ICP/File Browsing/User Management module*/
#define ERROR_COPY_FILE 1000 /*Error copying file*/
#define FILE_EXECUTE_SUCCESS 1001 /*File has been successfully executed*/
#define SUCCESS_COPY_FILE    1002 /*Files successfully copied*/
#define SUCCESS_PASTE_FILE   1003 /*Files have been successfully pasted*/
#define RENAME_SUCCESS       1004 /*File successfully rename*/
#define DIRECTORY_CREATE_SUCCESS 1005 /*Directory successfully created*/
#define PERMISSION_CHANGED_SUCCESS 1006 /*Permission successfully changed*/
#define FILE_COMPRESS_SUCCESS 1007 /*Files successfully compressed*/
#define COMMAND_NOT_ALLOWED   1008 /*Not a allowed command*/

/*Alert Messages in User Metering*/
#define APPLICATION_ILLEGAL 2001 /*Illegal Application started*/

#endif /* __NASCENTER_ALERTMESSAGE_H__ */
/* end of NAScenter_AlertMessage.h*/

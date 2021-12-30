/*****************************************************************************
 *
 * hrSWRunEntry_Win.h
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *      Ashok Shankar
 *
 *
 * description:
 *      hrSWRunEntry_Win.h provides common includes,macros,and external
 *              declarations/definitions for users of hrSWRunEntry_Win.c
 *
 * CM Details:
 *
 * $Author: ashok $
 * $Log: hrSWRunEntry_Win.h,v $
 * Revision 1.3  2003/11/11 13:42:27  ashok
 * Minor Corrections
 *
 * Revision 1.2  2003/11/08 15:33:48  ashok
 * no message
 *
 * Revision 1.1  2003/10/31 12:08:44  ashok
 * Added into CVS by Ashok
 *
 * $State: Exp $
 * $Id: hrSWRunEntry_Win.h,v 1.3 2003/11/11 13:42:27 ashok Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/
/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf,v 1.3 2002/10/17 09:40:46 dts12 Exp $
 */
#ifndef HRSWRUNENTRY_H
#define HRSWRUNENTRY_H

/************************* exported function prototypes ********************/
/* function declarations */
void init_hrSWRunEntry(void);
FindVarMethod var_hrSWRunEntry;
    WriteMethod write_hrSWRunStatus;

#endif /* HRSWRUNENTRY_H */
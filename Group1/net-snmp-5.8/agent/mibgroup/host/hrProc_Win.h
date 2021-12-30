/*****************************************************************************
 *
 * hrProc_Win.h
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *      Ashok Shankar
 *
 *
 * description:
 *      hrProc_Win.h provides common includes,macros,and external
 *              declarations/definitions for users of hrProc_Win.c
 *
 * CM Details:
 *
 * $Author: ashok $
 * $Log: hrProc_Win.h,v $
 * Revision 1.3  2003/11/11 13:41:23  ashok
 * Minor Corrections
 *
 * Revision 1.2  2003/11/08 15:24:00  ashok
 * no message
 *
 * Revision 1.1  2003/10/31 12:08:44  ashok
 * Added into CVS by Ashok
 *
 * $State: Exp $
 * $Id: hrProc_Win.h,v 1.3 2003/11/11 13:41:23 ashok Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/
/*
 *  Host Resources MIB - processor device group interface - hr_proc.h
 *
 */
#ifndef HRPROC_H
#define HRPROC_H



config_require(ucd-snmp/loadave)

/************************* exported function prototypes ********************/
extern void     init_hr_proc(void);
extern FindVarMethod var_hrproc;

#endif                          /* _MIBGROUP_HRPROC_H */

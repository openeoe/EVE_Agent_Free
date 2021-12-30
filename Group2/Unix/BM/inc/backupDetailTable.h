/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf,v 1.1 2003/10/11 09:58:35 rajesh Exp $
 */
#ifndef BACKUPDETAILTABLE_H
#define BACKUPDETAILTABLE_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

/* 
 * backupDetailTable_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */

oid backupDetailTable_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2};

oid backupDetailIndex_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,1};
oid backupDetailMasterID_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,2};
oid backupDetailFileLocation1_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,3};
oid backupDetailFileDescription1_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,4};
oid backupDetailFileLocation2_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,5};
oid backupDetailFileDescription2_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,6};
oid backupDetailFileLocation3_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,7};
oid backupDetailFileDescription3_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,8};
oid backupDetailFileLocation4_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,9};
oid backupDetailFileDescription4_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,10};
oid backupDetailFileLocation5_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,11};
oid backupDetailFileDescription5_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,12}; 
oid backupDetailStatus[] = { 1,3,6,1,4,1,3204,1,3,34,2,2,1,13}; 

#endif /* BACKUPDETAILTABLE_H */

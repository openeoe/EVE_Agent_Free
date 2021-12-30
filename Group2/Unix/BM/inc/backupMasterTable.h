/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf,v 1.1 2003/10/11 09:58:35 rajesh Exp $
 */
#ifndef BACKUPMASTERTABLE_H
#define BACKUPMASTERTABLE_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

/* 
 * backupMasterTable_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */

oid backupMasterTable_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1 };

oid backupMasterIndex_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,1};
oid backupMasterID_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,2};
oid backupMasterName_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,3};
oid backupMasterDescription_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,4};
oid backupMasterCreatedDate_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,5};
oid backupMasterCreatedUserID_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,6};
oid backupMasterCreatedUserName_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,7};
oid backupMasterPeriodicInterval_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,8};
oid backupMasterStartTime_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,9};
oid backupMasterFinishTime_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,10};
oid backupMasterDestinationIPAddress_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,11};
oid backupMasterDestinationPath_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,12};
oid backupMasterCompressOption_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,13};
oid backupMasterStatus_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,14};
oid backupMasterLastUpdatedDateAndTime_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,2,1,1,15};

#endif /* BACKUPMASTERTABLE_H */
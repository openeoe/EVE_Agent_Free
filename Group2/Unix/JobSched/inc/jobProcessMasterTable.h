/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf,v 1.1 2003/10/11 09:58:35 rajesh Exp $
 */
#ifndef JOBPROCESSMASTERTABLE_H
#define JOBPROCESSMASTERTABLE_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

/* 
 * jobProcessMasterTable_variables_oid:
 *   this is the top level oid that we want to register under.  This
 *   is essentially a prefix, with the suffix appearing in the
 *   variable below.
 */

oid jobProcessMasterTable_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1 };

oid jobProcessMasterIndex_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,1};
oid jobProcessMasterID_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,2};
oid jobProcessMasterName_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,3};
oid jobProcessMasterDescription_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,4};
oid jobProcessMasterCreatedDate_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,5};
oid jobProcessMasterCreatedUserID_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,6};
oid jobProcessMasterCreatedUserName_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,7};
oid jobProcessMasterPeriodicInterval_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,8};
oid jobProcessMasterStartTime_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,9};
oid jobProcessMasterFinishTime_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,10};
oid jobProcessMasterCPUPercentUsage_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,11};
oid jobProcessMasterMemoryPercentUsage_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,12};
oid jobProcessMasterStatus_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,13};
oid jobProcessMasterLastUpdatedDateAndTime_variables_oid[] = { 1,3,6,1,4,1,3204,1,3,34,1,1,1,14};

#endif /* JOBPROCESSMASTERTABLE_H */

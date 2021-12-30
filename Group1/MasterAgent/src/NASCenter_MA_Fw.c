#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <pthread.h>
#include <time.h>
#include "NASCenter_Util_List.h"
#include "NASCenter_MA.h"

#define MIB2_OIDLEN  6
#define SUBAGENT_TABLE_OIDLEN 11

extern int giNASSAPort;
extern struct snmp_session *agentSess;
extern int reloadConfig;
extern void MA_NASSAReadConfig();

oid mib2OID[] = { 1, 3, 6, 1, 2, 1 };
oid subAgentTable[] = {1, 3, 6, 1, 4, 1, 3204, 1, 3, 34, 5};

extern char localIP[];

void *sendPDU(void *arg){
	struct snmp_pdu *response;
	struct snmp_pdu *request;
	struct in_addr tempAdd;
	struct variable_list *list;
	int status;
	struct sockaddr_in *remAdd;
	Element *pduElement;
	int i;
	struct timespec delay;
	int setRequest = 0;
	int nassaflag = 0;
	int tempPort = 0;
	delay.tv_sec = 2;
	delay.tv_nsec = 0;
	
	while(1){
		while(gstRequestList->size==0){
			pthread_mutex_lock(&gRequestListCondLock);
			pthread_cond_wait(&gRequestListCond,&gRequestListCondLock);
			pthread_mutex_unlock(&gRequestListCondLock);
		}
		setRequest=0;
		pthread_mutex_lock(&gRequestListLock);
		pduElement=gstRequestList->start;
		request = (struct snmp_pdu *) pduElement->value;
		deleteFromList(gstRequestList,pduElement->value);
		pthread_mutex_unlock(&gRequestListLock);
		remAdd = (struct sockaddr_in *)request->transport_data;
		tempPort = remAdd->sin_port;
		tempAdd.s_addr=remAdd->sin_addr.s_addr;
		list =  request->variables;
		if(list == NULL){
			snmp_free_pdu(request);
			continue;
		}
		if(netsnmp_oid_is_subtree(mib2OID,MIB2_OIDLEN,list->name,
								list->name_length)){
			// This will reload the subagent configuration in case any of
			// the tables are modified
			if(reloadConfig){
				MA_NASSAReadConfig();
			}
			if(list->name_length>=ENTERPRISE_OID_POS){
				if(list->name[ENTERPRISE_OID_POS-1]==
					NAS_SUBAGENT_ENTERPRISE_OID){
						nassaflag=1;
				} else {
					for(i=0;i<MAX_SUB_AGENTS;i++){
						if(giSAEnterprise[i]==list->name[ENTERPRISE_OID_POS-1]
									   	&& giSAPort!=0){
							nassaflag=0;
							break;
						}
					} // end of for
				} // end of else(enterprise)
			}// end of if(list)
			else{
				nassaflag=1;
			}
		} // end of if(netsnmp_oid_is_subtree) 
		else{
			nassaflag=1;
		}
		if(nassaflag){
			remAdd->sin_port = htons(giNASSAPort);
			remAdd->sin_addr.s_addr = (in_addr_t) inet_addr(localIP);
		} else {
			// In case there is no subagent configuration for the received
			// PDU
			if(i==MAX_SUB_AGENTS){
				snmp_free_pdu(request);
				continue;
			}
			remAdd->sin_port = htons(giSAPort[i]);
			remAdd->sin_addr.s_addr = inet_addr(giSAIP[i]);
		}
		if(request->command==SNMP_MSG_SET){
			// This is to make sure that during the next request the 
			// Subagent Configuration is reloaded
			if(netsnmp_oid_is_subtree(subAgentTable,
							SUBAGENT_TABLE_OIDLEN,
							list->name,list->name_length)==0) {
				reloadConfig=1;
			}
			// The Request is a SET. We get the reqID
			setRequest=request->reqid;
		}
 		status = snmp_synch_response(agentSess,request,&response);
#ifdef XMLSET
		if(setRequest){
			if(status == STAT_SUCCESS){
				if(response!=NULL){
					if(response->errstat!=0){
						sendErrorMsg(setRequest,response->errstat);
					}else{
						sendSuccessMsg(setRequest);
					}
				}
			} else {
				sendErrorMsg(setRequest,0);
			}
		} else {
#endif
			if(status == STAT_SUCCESS){
				remAdd = (struct sockaddr_in *)response->transport_data;
				remAdd->sin_port = tempPort;
				remAdd->sin_addr.s_addr = tempAdd.s_addr;
				snmp_send(agentSess,response);
			} else{
				printf("Error Sending\n");
			}
#ifdef XMLSET
		}
#endif
	}
}

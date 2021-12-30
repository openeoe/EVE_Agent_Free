/* $Id: NASCenter_MA_Set.c,v 1.14 2004/01/14 04:09:42 rajesh Exp $*/

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include "NASCenter_Util_List.h"
#include "NASCenter_MA.h"

#ifdef XMLSET
#include <gnome-xml/parser.h>
#include <gnome-xml/tree.h>
#if defined(AIX_4_3)
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif



#define MAX_SET_MSG_SIZE 2048

#define IPADDR_LEN 4

#define MAX_OID_SIZE 5

#define END_TAG "</nas:env>"

//The Header tag constant
char *gcHeaderString = "header";

// The ID tag node
char *gcIdString = "id";

//The Body tag constant
char *gcBodyString = "body";

// The param tag contant
char *gcParamString = "param";

// The number type
char *gcNumberType = "number";

// The string type
char *gcStringType = "string";

// The OID Type
char *gcOIDType = "ObjectID";

// The IP Address type
char *gcIPType = "ipAddress";

// The client Socket Fd -- Since we serve only one connection
int clSockFd=0;

// Response SuccessMessage

char *XMLSuccessMsg =  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<nas:env>\
	<nas:header>\
		<nas:id>%d</nas:id>\
		<nas:status code=\"0\">OK</nas:status>\
	</nas:header>\
	<nas:body >\
		<nas:param type=\"number\">0</nas:param>\
		<nas:param type=\"number\">0</nas:param>\
	</nas:body>\
</nas:env>";


char *XMLErrMsg =  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<nas:env>\
	<nas:header>\
		<nas:id>%d</nas:id>\
		<nas:status code=\"-2\">Fail to load DOM Object</nas:status>\
	</nas:header>\
</nas:env>";

char *XMLSetFailErrMsg ="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<nas:env>\
	<nas:header>\
		<nas:id>%d</nas:id>\
		<nas:status code=\"0\">OK</nas:status>\
	</nas:header>\
	<nas:body >\
		<nas:param type=\"number\">%d</nas:param>\
	</nas:body>\
</nas:env>";

char *msgBuffer;
#ifdef ENCRYPTION
char *msgEncBuffer;
#endif

// XML VarBind Structure

typedef struct xmlVarBind {
	char *oid;
	char *value;
	char *type;
} XMLVarBind;



int getID(xmlNode *envNode){
	xmlNode *idNode;
	idNode = envNode->childs;
	if(strcmp(idNode->name,gcIdString)==0){
		return atoi(idNode->childs->content);
	} else{
		return -1;
	}
}

char *getIPAddress(xmlNode *ipNode){
	if(strcmp(ipNode->name,gcParamString)==0){
		return ipNode->childs->content;
	} else{
		return NULL;
	}

}

char *getCommunity(xmlNode *cmNode){
	if(strcmp(cmNode->name,gcParamString)==0){
		return cmNode->childs->content;
	} else{
		return NULL;
	}
}

int getVarBind(xmlNode *oidNode, XMLVarBind *xmlVb){
	xmlNode *valueNode;
	if(strcmp(oidNode->name,gcParamString)==0){
		xmlVb->oid = oidNode->childs->content;
	}else{
		return -1;
	}
	valueNode = oidNode->next;
	if(strcmp(oidNode->name,gcParamString)==0){
		xmlVb->value = valueNode->childs->content;
		if(valueNode->properties!=NULL){
			xmlVb->type = valueNode->properties->val->content;
		}else{
			return -1;
		}
	} else{
		return -1;
	}
	return 0;
}

int readoid(char *oidString, oid *name, size_t *nameLen) {
	char *oidPtr;
	char temp[MAX_OID_SIZE+1];
	size_t oidSize=0;
	int i=0;
	int j=0;
	int k=0;
	oidPtr = oidString;
	while(oidPtr[j]){
		if(oidPtr[j]=='.'){
			if(i!=0){
				temp[i]='\0';
				name[k++]=atoi(temp);
				oidSize++;
				i=0;
			}
		}else{
		 	temp[i++]=oidPtr[j];
			if(i>MAX_OID_SIZE){
				return 0;
			}
		}
		j++;
	}
	temp[i]='\0';
	name[k]=atoi(temp);
	oidSize++;
	*nameLen = oidSize;
	return oidSize;
}


struct snmp_pdu *parsePDUInXML(char *xmlString){
	XMLVarBind xmlVb;
	size_t nameLen;
	int i;
	long tempIntValue;
	oid tempOIDValue[MAX_OID_LEN];
	oid name[MAX_OID_LEN];
	struct sockaddr_in *tempAddr;
	long ipAddr = 0;
	xmlDocPtr doc = NULL;
	char *ipAddress= NULL;
	char *community= NULL;
	int nasId=0;
    xmlNode *rootNode = NULL;
    xmlNode *currentNode = NULL;
    struct snmp_pdu *setPdu = NULL;
	doc = xmlParseMemory(xmlString,strlen(xmlString));
	if(doc==NULL){
		return NULL;
	}
	rootNode = xmlDocGetRootElement(doc);
	// Header Node
    currentNode = rootNode->childs;

    setPdu = snmp_pdu_create(SNMP_MSG_SET);
	
	setPdu->version = SNMP_VERSION_2c;
	
	nasId=getID(currentNode);
	if(nasId<0){
		snmp_free_pdu(setPdu);
    	xmlFreeDoc(doc);
    	xmlCleanupParser();
		return NULL;
	}
	setPdu->reqid = nasId;

	// Body Node
	currentNode = currentNode->next;

	// Param Node -- first node is IP
	currentNode = currentNode->childs;

	ipAddress =  getIPAddress(currentNode);
	
	if(ipAddress==NULL) {
		snmp_free_pdu(setPdu);
    	xmlFreeDoc(doc);
    	xmlCleanupParser();
		return NULL;
	}
	/* Update Transport Data */
	tempAddr = malloc(sizeof(struct sockaddr_in));
	tempAddr->sin_family = AF_INET;
	tempAddr->sin_addr.s_addr = inet_addr("0.0.0.0");
	// This is a arbitrary port which will be changed to the actual 
	// port during Forwarding
	tempAddr->sin_port = htons(161);
	setPdu->transport_data = tempAddr;
	setPdu->transport_data_length = sizeof(struct sockaddr_in);
	// Community 
	currentNode = currentNode->next;
	community = getCommunity(currentNode);
	if(community==NULL){
		snmp_free_pdu(setPdu);
    	xmlFreeDoc(doc);
    	xmlCleanupParser();
		return NULL;
	}
	setPdu->community = malloc(strlen(community));
	memcpy(setPdu->community,community,strlen(community));
	setPdu->community_len = strlen(community);
	// Variables 
	currentNode = currentNode->next;
	while(currentNode){
		if(getVarBind(currentNode,&xmlVb) <0){
			snmp_free_pdu(setPdu);
    		xmlFreeDoc(doc);
    		xmlCleanupParser();
			return NULL;
		}
		if(readoid(xmlVb.oid,name,&nameLen)==0){
			snmp_free_pdu(setPdu);
    		xmlFreeDoc(doc);
    		xmlCleanupParser();
			return NULL;
		}
		currentNode = currentNode->next;
		if(strstr(xmlVb.type,gcNumberType)){
			tempIntValue = atol(xmlVb.value);
			snmp_pdu_add_variable(setPdu,name,nameLen,ASN_INTEGER,
							 (u_char *) &tempIntValue, sizeof(long));
		} else if(strstr(xmlVb.type,gcStringType)){
			snmp_pdu_add_variable(setPdu,name,nameLen,ASN_OCTET_STR,
									xmlVb.value,strlen(xmlVb.value));
		} else if(strstr(xmlVb.type,gcOIDType)){
			readoid(xmlVb.value,tempOIDValue,&nameLen);
			snmp_pdu_add_variable(setPdu,name,nameLen,ASN_OBJECT_ID,
									(u_char *)tempOIDValue,nameLen);
		} else if(strstr(xmlVb.type,gcIPType)){
			readoid(xmlVb.oid,tempOIDValue,&nameLen);
			ipAddr = inet_addr(xmlVb.value);
			snmp_pdu_add_variable(setPdu,name,nameLen,ASN_IPADDRESS,
									(u_char *)&ipAddr,IPADDR_LEN);
		}else{
			snmp_free_pdu(setPdu);
    		xmlFreeDoc(doc);
    		xmlCleanupParser();
			return NULL;
		}
		if(currentNode){
			currentNode = currentNode->next;
		}else{
			break;
		}
	}
    xmlFreeDoc(doc);
    xmlCleanupParser();
	return setPdu;
}

void sendErrorMsg(int reqID, int errCode){
	if(errCode<0){
		sprintf(msgBuffer,XMLErrMsg,reqID);
		printf("Error Message %s\n", msgBuffer);
	}if(errCode>0) {
		sprintf(msgBuffer,XMLSetFailErrMsg,reqID,errCode);
		printf("Error Message %s\n", msgBuffer);
	} else {
		printf("Invalid Error Code \n");
		return;
	}
	#ifdef ENCRYPTION
	NAS_Encrypt(&bctx,msgEncBuffer,msgBuffer);
	write(clSockFd,msgEncBuffer,strlen(msgEncBuffer));
	#else
	write(clSockFd,msgBuffer,strlen(msgBuffer));
	#endif
}

void sendSuccessMsg(int reqID){
	sprintf(msgBuffer,XMLSuccessMsg,reqID);
	printf("Success Message %s\n", msgBuffer);
	#ifdef ENCRYPTION
	NAS_Encrypt(&bctx,msgEncBuffer,msgBuffer);
	write(clSockFd,msgEncBuffer,strlen(msgEncBuffer));
	#else
	write(clSockFd,msgBuffer,strlen(msgBuffer));
	#endif
}

char * readXML(int sockFd){
	int readLen=0;
	int totalLen=0;
	fd_set fds;
	char *finalBuff;
	struct timeval delay;
	int fdNum=0;
	char readBuffer[MAX_SET_MSG_SIZE];
	finalBuff=NULL;
	FD_ZERO(&fds);
	FD_SET(sockFd,&fds);
	delay.tv_sec = 1;
	delay.tv_usec = 0;
	while(select(sockFd+1,&fds,NULL,NULL,&delay)) {
	//while((readLen=read(sockFd,readBuffer,MAX_SET_MSG_SIZE))>0) 
		readLen=read(sockFd,readBuffer,MAX_SET_MSG_SIZE);
		if(readLen==0){
			break;
		}
		totalLen+=readLen;
		finalBuff = realloc(finalBuff,totalLen);
		memcpy(finalBuff+(totalLen-readLen),readBuffer,readLen);
		if(strstr(END_TAG,finalBuff)){
			break;
		}
	}
	if(finalBuff){
		finalBuff = realloc(finalBuff,totalLen+1);
		finalBuff[totalLen]='\0';
	}
	return finalBuff;
}

void * readXMLSetPacket(void *port ) {
	struct sockaddr_in laddr;
	int sockReuseAddrFlag=1;
	struct snmp_pdu *pdu = NULL;
	struct sockaddr_in *raddr=NULL;
	int raddrLen=0;
	int sockFd=0;
	int readLen=0;
	int sockFlag = 0;
	//char readBuffer[MAX_SET_MSG_SIZE];
	char *readBuffer;
	#ifdef ENCRYPTION
	char readEncBuffer[MAX_SET_MSG_SIZE];
	#endif
	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(*(int*)port);
	laddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	sockFd = socket(PF_INET,SOCK_STREAM,0);
	if(sockFd<0){
		return;
	}
	setsockopt(sockFd,SOL_SOCKET,SO_REUSEADDR, 
					&sockReuseAddrFlag, sizeof(sockReuseAddrFlag));
	if(bind(sockFd,(struct sockaddr *) &laddr,sizeof(laddr))<0){
		close(sockFd);
		return;
	}
	//Start Reading the XML packet
	while(1) {
		if(listen(sockFd,1)<0){
			close(sockFd);
			return;
		}
		if((clSockFd=accept(sockFd,(struct sockaddr *)raddr,&raddrLen))>0){
			while(1){
				#ifdef ENCRYPTION
				if((readLen=read(clSockFd,readEncBuffer,MAX_SET_MSG_SIZE))>0){
				#else
				sockFlag = fcntl(clSockFd,F_GETFL,0);
				fcntl(clSockFd,F_SETFL, (sockFlag | O_NONBLOCK));
				if((readBuffer=readXML(clSockFd))!=NULL){
				#endif
						#ifdef ENCRYPTION
					if(readLen>0){
						NAS_Decrypt(&bctx,readBuffer,readEncBuffer);
						readBuffer[readLen]='\0';
						#endif 
						pdu = parsePDUInXML(readBuffer);
						free(readBuffer);
						if(pdu==NULL){
							sendErrorMsg(1,-1);
							close(clSockFd);
							break;
						}else {
							pthread_mutex_lock(&gRequestListLock);
							addToList(gstRequestList,pdu);
							pthread_mutex_unlock(&gRequestListLock);
							pthread_cond_signal(&gRequestListCond);
						}
					#ifdef ENCRYPTION
					}
					#endif
				}else{
					close(clSockFd);
					break;
				}
			} //end of while(1)
		}
	} // end of while(1)

}


initXML() {

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION
	// The maximum error message size + length of integers
	msgBuffer = malloc(strlen(XMLSuccessMsg)+ 20);
	#ifdef ENCRYPTION
	msgEncBuffer = malloc(strlen(XMLSuccessMsg)+ 20);
	#endif
    //xmlCleanupParser();
}
#endif

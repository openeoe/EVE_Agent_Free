/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.15.2.1 2003/02/27 05:59:41 rstory Exp $
 *
 * $Id:$
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef BACKUPMASTERTABLE_H
#define BACKUPMASTERTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

/** #define values**/
#define BM_MAX_OID_LEN 128
#define BM_MAX_VAR_LEN 255
#define BM_NO_OF_FIELDS 15
#define BM_CONTAINER_FILE "backupMasterTable.conf"
#define BM_MAX_DT_LEN 20
#define BM_SUCCESS 1
#define BM_FAILURE 2
#define BM_TRAP 10
#define BM_ROW_CREATE 4
#define BM_ROW_DELETE 6	


        /** Index backupMasterIndex is internal */

typedef struct backupMasterTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long backupMasterIndex;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupMasterID[255];
            long backupMasterID_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupMasterName[255];
            long backupMasterName_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupMasterDescription[255];
            long backupMasterDescription_len;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char backupMasterCreatedDate[20];
            long backupMasterCreatedDate_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupMasterCreatedUserID[255];
            long backupMasterCreatedUserID_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupMasterCreatedUserName[255];
            long backupMasterCreatedUserName_len;

        /** INTEGER = ASN_INTEGER */
            long backupMasterPeriodicInterval;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char backupMasterStartTime[20];
            long backupMasterStartTime_len;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char backupMasterFinishTime[20];
            long backupMasterFinishTime_len;

        /** IPADDR = ASN_IPADDRESS */
            unsigned long backupMasterDestinationIPAddress;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupMasterDestinationPath[255];
            long backupMasterDestinationPath_len;

        /** INTEGER = ASN_INTEGER */
            long backupMasterCompressOption;

        /** RowStatus = ASN_INTEGER */
            long backupMasterStatus;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char backupMasterLastUpdatedDateAndTime[20];
            long backupMasterLastUpdatedDateAndTime_len;


    /*
     * OR
     *
     * Keep a pointer to your data
     */
    void * data;

    /*
     *add anything else you want here
     */
   unsigned long idx;

} backupMasterTable_context;

#ifndef VARBIND
#define VARBIND

typedef struct VarBind{
    char  acVariable[MAX_OID_LEN];
    char cType;
    char pcValue [255];
}stVarBindArgs;

#endif /*VARBIND*/ 


/*************************************************************
 * function declarations
 */
void init_backupMasterTable(void);
void initialize_table_backupMasterTable(void);
const backupMasterTable_context * backupMasterTable_get_by_idx(netsnmp_index *);
const backupMasterTable_context * backupMasterTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int backupMasterTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);

int backupMasterTable_send_snmptrap(int length,stVarBindArgs *trapPDU,int port);
static int
backupMasterTable_snmp_input(int operation,
       netsnmp_session * session,
       int reqid, netsnmp_pdu *pdu, void *magic);

void backupMasterTable_Load_Container_From_File();


/*************************************************************
 * oid declarations
 */
extern oid backupMasterTable_oid[];
extern size_t backupMasterTable_oid_len;

#define backupMasterTable_TABLE_OID 1,3,6,1,4,1,3204,1,3,34,2,1
    
/*************************************************************
 * column number definitions for table backupMasterTable
 */
#define COLUMN_BACKUPMASTERINDEX 1
#define COLUMN_BACKUPMASTERID 2
#define COLUMN_BACKUPMASTERNAME 3
#define COLUMN_BACKUPMASTERDESCRIPTION 4
#define COLUMN_BACKUPMASTERCREATEDDATE 5
#define COLUMN_BACKUPMASTERCREATEDUSERID 6
#define COLUMN_BACKUPMASTERCREATEDUSERNAME 7
#define COLUMN_BACKUPMASTERPERIODICINTERVAL 8
#define COLUMN_BACKUPMASTERSTARTTIME 9
#define COLUMN_BACKUPMASTERFINISHTIME 10
#define COLUMN_BACKUPMASTERDESTINATIONIPADDRESS 11
#define COLUMN_BACKUPMASTERDESTINATIONPATH 12
#define COLUMN_BACKUPMASTERCOMPRESSOPTION 13
#define COLUMN_BACKUPMASTERSTATUS 14
#define COLUMN_BACKUPMASTERLASTUPDATEDDATEANDTIME 15
#define backupMasterTable_COL_MIN 2
#define backupMasterTable_COL_MAX 15

/* comment out the following line if you don't handle SET-REQUEST for backupMasterTable */
#define backupMasterTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define backupMasterTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define backupMasterTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define backupMasterTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef backupMasterTable_SET_HANDLING

int backupMasterTable_extract_index( backupMasterTable_context * ctx, netsnmp_index * hdr );

void backupMasterTable_set_reserve1( netsnmp_request_group * );
void backupMasterTable_set_reserve2( netsnmp_request_group * );
void backupMasterTable_set_action( netsnmp_request_group * );
void backupMasterTable_set_commit( netsnmp_request_group * );
void backupMasterTable_set_free( netsnmp_request_group * );
void backupMasterTable_set_undo( netsnmp_request_group * );

backupMasterTable_context * backupMasterTable_duplicate_row( backupMasterTable_context* );
netsnmp_index * backupMasterTable_delete_row( backupMasterTable_context* );

int backupMasterTable_can_activate(backupMasterTable_context *undo_ctx,
                      backupMasterTable_context *row_ctx,
                      netsnmp_request_group * rg);
int backupMasterTable_can_deactivate(backupMasterTable_context *undo_ctx,
                        backupMasterTable_context *row_ctx,
                        netsnmp_request_group * rg);
int backupMasterTable_can_delete(backupMasterTable_context *undo_ctx,
                    backupMasterTable_context *row_ctx,
                    netsnmp_request_group * rg);
    
    
#ifdef backupMasterTable_ROW_CREATION
backupMasterTable_context * backupMasterTable_create_row( netsnmp_index* );
#endif
#endif

#ifdef backupMasterTable_IDX2
backupMasterTable_context * backupMasterTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** BACKUPMASTERTABLE_H */

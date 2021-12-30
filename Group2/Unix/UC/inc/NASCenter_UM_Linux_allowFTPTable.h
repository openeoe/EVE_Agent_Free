/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.15.2.1 2003/02/27 05:59:41 rstory Exp $
 *
 * $Id: NASCenter_UM_Linux_allowFTPTable.h,v 1.1 2003/11/13 06:19:13 guga Exp $
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef ALLOWFTPTABLE_H
#define ALLOWFTPTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

/** #define values**/
#define MAX_OID_LEN 128
#define MAX_VAR_LEN 255
#define NO_OF_FIELDS 5

        /** Index allowFTPIndex is internal */

typedef struct allowFTPTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long allowFTPIndex;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char allowFTPID[255];
            long allowFTPID_len;

        /** OCTETSTR = ASN_OCTET_STR */
            unsigned char allowFTPIPAddress[65535];
            long allowFTPIPAddress_len;

        /** INTEGER = ASN_INTEGER */
            long allowFTPPortNo;

        /** RowStatus = ASN_INTEGER */
            long allowFTPStatus;


    /*
     * OR
     *
     * Keep a pointer to your data
     */
    void * data;

    /*
     *add anything else you want here
     */

} allowFTPTable_context;
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
void init_allowFTPTable(void);
void initialize_table_allowFTPTable(void);
const allowFTPTable_context * allowFTPTable_get_by_idx(netsnmp_index *);
const allowFTPTable_context * allowFTPTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int allowFTPTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);
int allowFTPTable_send_snmptrap(int length,stVarBindArgs *trapPDU,int port);
static int
allowFTPTable_snmp_input(int operation,
       netsnmp_session * session,
       int reqid, netsnmp_pdu *pdu, void *magic);


/*************************************************************
 * oid declarations
 */
extern oid allowFTPTable_oid[];
extern size_t allowFTPTable_oid_len;

#define allowFTPTable_TABLE_OID 1,3,6,1,4,1,3204,1,3,34,4,4
    
/*************************************************************
 * column number definitions for table allowFTPTable
 */
#define COLUMN_ALLOWFTPINDEX 1
#define COLUMN_ALLOWFTPID 2
#define COLUMN_ALLOWFTPIPADDRESS 3
#define COLUMN_ALLOWFTPPORTNO 4
#define COLUMN_ALLOWFTPSTATUS 5
#define allowFTPTable_COL_MIN 2
#define allowFTPTable_COL_MAX 5

/* comment out the following line if you don't handle SET-REQUEST for allowFTPTable */
#define allowFTPTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define allowFTPTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define allowFTPTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define allowFTPTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef allowFTPTable_SET_HANDLING

int allowFTPTable_extract_index( allowFTPTable_context * ctx, netsnmp_index * hdr );

void allowFTPTable_set_reserve1( netsnmp_request_group * );
void allowFTPTable_set_reserve2( netsnmp_request_group * );
void allowFTPTable_set_action( netsnmp_request_group * );
void allowFTPTable_set_commit( netsnmp_request_group * );
void allowFTPTable_set_free( netsnmp_request_group * );
void allowFTPTable_set_undo( netsnmp_request_group * );

allowFTPTable_context * allowFTPTable_duplicate_row( allowFTPTable_context* );
netsnmp_index * allowFTPTable_delete_row( allowFTPTable_context* );

int allowFTPTable_can_activate(allowFTPTable_context *undo_ctx,
                      allowFTPTable_context *row_ctx,
                      netsnmp_request_group * rg);
int allowFTPTable_can_deactivate(allowFTPTable_context *undo_ctx,
                        allowFTPTable_context *row_ctx,
                        netsnmp_request_group * rg);
int allowFTPTable_can_delete(allowFTPTable_context *undo_ctx,
                    allowFTPTable_context *row_ctx,
                    netsnmp_request_group * rg);
    
    
#ifdef allowFTPTable_ROW_CREATION
allowFTPTable_context * allowFTPTable_create_row( netsnmp_index* );
#endif
#endif

#ifdef allowFTPTable_IDX2
allowFTPTable_context * allowFTPTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** ALLOWFTPTABLE_H */

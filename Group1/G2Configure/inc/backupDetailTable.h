/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.15.2.1 2003/02/27 05:59:41 rstory Exp $
 *
 * $Id:$
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef BACKUPDETAILTABLE_H
#define BACKUPDETAILTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>


#define BD_CONTAINER_FILE "backupDetailTable.conf"




        /** Index backupDetailIndex is internal */

typedef struct backupDetailTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long backupDetailIndex;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupDetailMasterID[255];
            long backupDetailMasterID_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupDetailFileLocation1[255];
            long backupDetailFileLocation1_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupDetailFileDescription1[255];
            long backupDetailFileDescription1_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupDetailFileLocation2[255];
            long backupDetailFileLocation2_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupDetailFileDescription2[255];
            long backupDetailFileDescription2_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupDetailFileLocation3[255];
            long backupDetailFileLocation3_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupDetailFileDescription3[255];
            long backupDetailFileDescription3_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupDetailFileLocation4[255];
            long backupDetailFileLocation4_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupDetailFileDescription4[255];
            long backupDetailFileDescription4_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupDetailFileLocation5[255];
            long backupDetailFileLocation5_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char backupDetailFileDescription5[255];
            long backupDetailFileDescription5_len;

        /** RowStatus = ASN_INTEGER */
            long backupDetailStatus;


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

} backupDetailTable_context;




/*************************************************************
 * function declarations
 */
void init_backupDetailTable(void);
void initialize_table_backupDetailTable(void);
const backupDetailTable_context * backupDetailTable_get_by_idx(netsnmp_index *);
const backupDetailTable_context * backupDetailTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int backupDetailTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);

void backupDetailTable_Load_Container_From_File();


/*************************************************************
 * oid declarations
 */
extern oid backupDetailTable_oid[];
extern size_t backupDetailTable_oid_len;

#define backupDetailTable_TABLE_OID 1,3,6,1,4,1,3204,1,3,34,2,2
    
/*************************************************************
 * column number definitions for table backupDetailTable
 */
#define COLUMN_BACKUPDETAILINDEX 1
#define COLUMN_BACKUPDETAILMASTERID 2
#define COLUMN_BACKUPDETAILFILELOCATION1 3
#define COLUMN_BACKUPDETAILFILEDESCRIPTION1 4
#define COLUMN_BACKUPDETAILFILELOCATION2 5
#define COLUMN_BACKUPDETAILFILEDESCRIPTION2 6
#define COLUMN_BACKUPDETAILFILELOCATION3 7
#define COLUMN_BACKUPDETAILFILEDESCRIPTION3 8
#define COLUMN_BACKUPDETAILFILELOCATION4 9
#define COLUMN_BACKUPDETAILFILEDESCRIPTION4 10
#define COLUMN_BACKUPDETAILFILELOCATION5 11
#define COLUMN_BACKUPDETAILFILEDESCRIPTION5 12
#define COLUMN_BACKUPDETAILSTATUS 13
#define backupDetailTable_COL_MIN 2
#define backupDetailTable_COL_MAX 13

/* comment out the following line if you don't handle SET-REQUEST for backupDetailTable */
#define backupDetailTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define backupDetailTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define backupDetailTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define backupDetailTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef backupDetailTable_SET_HANDLING

int backupDetailTable_extract_index( backupDetailTable_context * ctx, netsnmp_index * hdr );

void backupDetailTable_set_reserve1( netsnmp_request_group * );
void backupDetailTable_set_reserve2( netsnmp_request_group * );
void backupDetailTable_set_action( netsnmp_request_group * );
void backupDetailTable_set_commit( netsnmp_request_group * );
void backupDetailTable_set_free( netsnmp_request_group * );
void backupDetailTable_set_undo( netsnmp_request_group * );

backupDetailTable_context * backupDetailTable_duplicate_row( backupDetailTable_context* );
netsnmp_index * backupDetailTable_delete_row( backupDetailTable_context* );

int backupDetailTable_can_activate(backupDetailTable_context *undo_ctx,
                      backupDetailTable_context *row_ctx,
                      netsnmp_request_group * rg);
int backupDetailTable_can_deactivate(backupDetailTable_context *undo_ctx,
                        backupDetailTable_context *row_ctx,
                        netsnmp_request_group * rg);
int backupDetailTable_can_delete(backupDetailTable_context *undo_ctx,
                    backupDetailTable_context *row_ctx,
                    netsnmp_request_group * rg);
    
    
#ifdef backupDetailTable_ROW_CREATION
backupDetailTable_context * backupDetailTable_create_row( netsnmp_index* );
#endif
#endif

#ifdef backupDetailTable_IDX2
backupDetailTable_context * backupDetailTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** BACKUPDETAILTABLE_H */

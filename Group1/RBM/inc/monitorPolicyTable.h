/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.15.2.1 2003/02/27 05:59:41 rstory Exp $
 *
 * $Id: monitorPolicyTable.h,v 1.4 2003/11/28 04:14:54 vijaysp Exp $
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef MONITORPOLICYTABLE_H
#define MONITORPOLICYTABLE_H

#ifdef WIN32
#define RBM_CONTAINER_FILE "monitorPolicyTable.conf"
#else
#define RBM_CONTAINER_FILE "/usr/local/conf/monitorPolicyTable.conf"
#endif

#define RBM_EXPR_MIN  0L
#define RBM_EXPR_MAX  5L

#ifdef __cplusplus
extern "C" {
#endif

    
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

        /** Index monitorPolicyIndex is internal */

typedef struct monitorPolicyTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long monitorPolicyIndex;

        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long monitorPolicyID;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char monitorPolicyName[255];
            long monitorPolicyName_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char monitorPolicyDescription[255];
            long monitorPolicyDescription_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char monitorPolicyContructor[255];
            long monitorPolicyContructor_len;

        /** DateAndTime = ASN_OCTET_STR */
            unsigned char monitorPolicyCreateDate[MAX_OID_LEN];
            long monitorPolicyCreateDate_len;

        /** OBJECTID = ASN_OBJECT_ID */
            oid monitorPolicyVariable1[MAX_OID_LEN];
            long monitorPolicyVariable1_len;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyExpression1;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyValue1;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyOperation1;

        /** OBJECTID = ASN_OBJECT_ID */
            oid monitorPolicyVariable2[MAX_OID_LEN];
            long monitorPolicyVariable2_len;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyExpression2;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyValue2;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyOperation2;

        /** OBJECTID = ASN_OBJECT_ID */
            oid monitorPolicyVariable3[MAX_OID_LEN];
            long monitorPolicyVariable3_len;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyExpression3;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyValue3;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyOperation3;

        /** OBJECTID = ASN_OBJECT_ID */
            oid monitorPolicyVariable4[MAX_OID_LEN];
            long monitorPolicyVariable4_len;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyExpression4;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyValue4;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyOperation4;

        /** OBJECTID = ASN_OBJECT_ID */
            oid monitorPolicyVariable5[MAX_OID_LEN];
            long monitorPolicyVariable5_len;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyExpression5;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyValue5;

        /** INTEGER = ASN_INTEGER */
            long monitorPolicyLevel;

        /**  =  */
    /** TODO: Is this type correct? */
            long monitorPolicySMS;

        /**  =  */
    /** TODO: Is this type correct? */
            long monitorPolicyPopup;

        /**  =  */
    /** TODO: Is this type correct? */
            long monitorPolicyEmail;

        /**  =  */
    /** TODO: Is this type correct? */
            long monitorPolicyPSTN;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char monitorPolicyCommand[255];
            long monitorPolicyCommand_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char monitorPolicyScript[255];
            long monitorPolicyScript_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char monitorPolicyAlertMessage[255];
            long monitorPolicyAlertMessage_len;

        /** RowStatus = ASN_INTEGER */
            long monitorPolicyStatus;


    /*
     * OR
     *
     * Keep a pointer to your data
     */
    void * data;

    /*
     *add anything else you want here
     */
    /* <Start: WIP_NURI_ALL: Introduce new flag for secondary storage */

    unsigned long idx;

    /* <End: WIP_NURI_ALL */

} monitorPolicyTable_context;

/*************************************************************
 * function declarations
 */
void init_monitorPolicyTable(void);
void initialize_table_monitorPolicyTable(void);
const monitorPolicyTable_context * monitorPolicyTable_get_by_idx(netsnmp_index *);
const monitorPolicyTable_context * monitorPolicyTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int monitorPolicyTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);


/*************************************************************
 * oid declarations
 */
extern oid monitorPolicyTable_oid[];
extern size_t monitorPolicyTable_oid_len;

#define monitorPolicyTable_TABLE_OID 1,3,6,1,4,1,3204,1,3,35,1
    
/*************************************************************
 * column number definitions for table monitorPolicyTable
 */
#define COLUMN_MONITORPOLICYINDEX 1
#define COLUMN_MONITORPOLICYID 2
#define COLUMN_MONITORPOLICYNAME 3
#define COLUMN_MONITORPOLICYDESCRIPTION 4
#define COLUMN_MONITORPOLICYCONTRUCTOR 5
#define COLUMN_MONITORPOLICYCREATEDATE 6
#define COLUMN_MONITORPOLICYVARIABLE1 7
#define COLUMN_MONITORPOLICYEXPRESSION1 8
#define COLUMN_MONITORPOLICYVALUE1 9
#define COLUMN_MONITORPOLICYOPERATION1 10
#define COLUMN_MONITORPOLICYVARIABLE2 11
#define COLUMN_MONITORPOLICYEXPRESSION2 12
#define COLUMN_MONITORPOLICYVALUE2 13
#define COLUMN_MONITORPOLICYOPERATION2 14
#define COLUMN_MONITORPOLICYVARIABLE3 15
#define COLUMN_MONITORPOLICYEXPRESSION3 16
#define COLUMN_MONITORPOLICYVALUE3 17
#define COLUMN_MONITORPOLICYOPERATION3 18
#define COLUMN_MONITORPOLICYVARIABLE4 19
#define COLUMN_MONITORPOLICYEXPRESSION4 20
#define COLUMN_MONITORPOLICYVALUE4 21
#define COLUMN_MONITORPOLICYOPERATION4 22
#define COLUMN_MONITORPOLICYVARIABLE5 23
#define COLUMN_MONITORPOLICYEXPRESSION5 24
#define COLUMN_MONITORPOLICYVALUE5 25
#define COLUMN_MONITORPOLICYLEVEL 26
#define COLUMN_MONITORPOLICYSMS 27
#define COLUMN_MONITORPOLICYPOPUP 28
#define COLUMN_MONITORPOLICYEMAIL 29
#define COLUMN_MONITORPOLICYPSTN 30
#define COLUMN_MONITORPOLICYCOMMAND 31
#define COLUMN_MONITORPOLICYSCRIPT 32
#define COLUMN_MONITORPOLICYALERTMESSAGE 33
#define COLUMN_MONITORPOLICYSTATUS 34
#define monitorPolicyTable_COL_MIN 2
#define monitorPolicyTable_COL_MAX 34

/* comment out the following line if you don't handle SET-REQUEST for monitorPolicyTable */
#define monitorPolicyTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define monitorPolicyTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
/* #define monitorPolicyTable_IDX2 */

/* uncommend the following line if you allow modifications to an
 * active row */
#define monitorPolicyTable_CAN_MODIFY_ACTIVE_ROW

#ifdef monitorPolicyTable_SET_HANDLING

int monitorPolicyTable_extract_index( monitorPolicyTable_context * ctx, netsnmp_index * hdr );

void monitorPolicyTable_set_reserve1( netsnmp_request_group * );
void monitorPolicyTable_set_reserve2( netsnmp_request_group * );
void monitorPolicyTable_set_action( netsnmp_request_group * );
void monitorPolicyTable_set_commit( netsnmp_request_group * );
void monitorPolicyTable_set_free( netsnmp_request_group * );
void monitorPolicyTable_set_undo( netsnmp_request_group * );

monitorPolicyTable_context * monitorPolicyTable_duplicate_row( monitorPolicyTable_context* );
netsnmp_index * monitorPolicyTable_delete_row( monitorPolicyTable_context* );

int monitorPolicyTable_can_activate(monitorPolicyTable_context *undo_ctx,
                      monitorPolicyTable_context *row_ctx,
                      netsnmp_request_group * rg);
int monitorPolicyTable_can_deactivate(monitorPolicyTable_context *undo_ctx,
                        monitorPolicyTable_context *row_ctx,
                        netsnmp_request_group * rg);
int monitorPolicyTable_can_delete(monitorPolicyTable_context *undo_ctx,
                    monitorPolicyTable_context *row_ctx,
                    netsnmp_request_group * rg);
    
    
#ifdef monitorPolicyTable_ROW_CREATION
monitorPolicyTable_context * monitorPolicyTable_create_row( netsnmp_index* );
#endif
#endif

#ifdef monitorPolicyTable_IDX2
monitorPolicyTable_context * monitorPolicyTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** MONITORPOLICYTABLE_H */

/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.15.2.1 2003/02/27 05:59:41 rstory Exp $
 *
 * $Id: JobProcessDetailTable.h,v 1.1 2003/12/01 14:16:22 guga Exp $
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef JOBPROCESSDETAILTABLE_H
#define JOBPROCESSDETAILTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

#define JD_CONTAINER_FILE "JobProcessDetailTable.conf"


        /** Index jobProcessDetailIndex is internal */

typedef struct JobProcessDetailTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long jobProcessDetailIndex;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char jobProcessDetailMasterID[255];
            long jobProcessDetailMasterID_len;

        /** DisplayString = ASN_OCTET_STR */
            unsigned char jobProcessDetailScriptFile[255];
            long jobProcessDetailScriptFile_len;

        /** RowStatus = ASN_INTEGER */
            long jobProcessDetailStatus;


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
} JobProcessDetailTable_context;



/*************************************************************
 * function declarations
 */
void init_JobProcessDetailTable(void);
void initialize_table_JobProcessDetailTable(void);
const JobProcessDetailTable_context * JobProcessDetailTable_get_by_idx(netsnmp_index *);
const JobProcessDetailTable_context * JobProcessDetailTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int JobProcessDetailTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);

void JobProcessDetailTable_Load_Container_From_File();


/*************************************************************
 * oid declarations
 */
extern oid JobProcessDetailTable_oid[];
extern size_t JobProcessDetailTable_oid_len;

#define JobProcessDetailTable_TABLE_OID 1,3,6,1,4,1,3204,1,3,34,1,2
    
/*************************************************************
 * column number definitions for table JobProcessDetailTable
 */
#define COLUMN_JOBPROCESSDETAILINDEX 1
#define COLUMN_JOBPROCESSDETAILMASTERID 2
#define COLUMN_JOBPROCESSDETAILSCRIPTFILE 3
#define COLUMN_JOBPROCESSDETAILSTATUS 4
#define JobProcessDetailTable_COL_MIN 2
#define JobProcessDetailTable_COL_MAX 4

/* comment out the following line if you don't handle SET-REQUEST for JobProcessDetailTable */
#define JobProcessDetailTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define JobProcessDetailTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define JobProcessDetailTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define JobProcessDetailTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef JobProcessDetailTable_SET_HANDLING

int JobProcessDetailTable_extract_index( JobProcessDetailTable_context * ctx, netsnmp_index * hdr );

void JobProcessDetailTable_set_reserve1( netsnmp_request_group * );
void JobProcessDetailTable_set_reserve2( netsnmp_request_group * );
void JobProcessDetailTable_set_action( netsnmp_request_group * );
void JobProcessDetailTable_set_commit( netsnmp_request_group * );
void JobProcessDetailTable_set_free( netsnmp_request_group * );
void JobProcessDetailTable_set_undo( netsnmp_request_group * );

JobProcessDetailTable_context * JobProcessDetailTable_duplicate_row( JobProcessDetailTable_context* );
netsnmp_index * JobProcessDetailTable_delete_row( JobProcessDetailTable_context* );

int JobProcessDetailTable_can_activate(JobProcessDetailTable_context *undo_ctx,
                      JobProcessDetailTable_context *row_ctx,
                      netsnmp_request_group * rg);
int JobProcessDetailTable_can_deactivate(JobProcessDetailTable_context *undo_ctx,
                        JobProcessDetailTable_context *row_ctx,
                        netsnmp_request_group * rg);
int JobProcessDetailTable_can_delete(JobProcessDetailTable_context *undo_ctx,
                    JobProcessDetailTable_context *row_ctx,
                    netsnmp_request_group * rg);
    
    
#ifdef JobProcessDetailTable_ROW_CREATION
JobProcessDetailTable_context * JobProcessDetailTable_create_row( netsnmp_index* );
#endif
#endif

#ifdef JobProcessDetailTable_IDX2
JobProcessDetailTable_context * JobProcessDetailTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** JOBPROCESSDETAILTABLE_H */

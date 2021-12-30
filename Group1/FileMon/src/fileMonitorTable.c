/*****************************************************************************
 *
 * fileMonitorTable.c
 *
 *      Copyright (c) 2009 NURI Telecom Co., LTD. 
 *      All rights Reserved
 *
 * author:
 *      SungHae Kim 
 *
 * description :
 *      This file is a C source code for File status Monitoring
 *
 * TODO:
 *      None
 *
 ****************************************************************************/

/********************************* includes *********************************/
#ifdef osf5
#include <sys/table.h>
#include <sys/sysinfo.h>
#include <sys/user.h>
#include <unistd.h>
#endif

#ifdef sysv4
#define _KMEMUSER
#include <sys/metrics.h>
#define METPTR_SYMBOL "met_localdata_ptrs_p"
#endif

#define _REENTRANT
#define MALLOC_CHECK_ 1

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/library/snmp.h>
#include <net-snmp/library/snmp_assert.h>

#ifdef WIN32
#include "../inc/fileMonitorTable.h"
#include <tchar.h>
#else
#include "psMonitorTable.h"
#include "fileMonitorTable.h"
#endif

#define FCLOSE(x) fclose(x);\
                  x=NULL

#define CHECK_INTERVAL	30 // seconds
#define MAX(x,y) ((x)>(y)?(x):(y))

static netsnmp_handler_registration *my_handler = NULL;
static netsnmp_table_array_callbacks cb;

static oid objid_sysuptime[] = { 1, 3, 6, 1, 2, 1, 1, 3, 0 };
static oid objid_snmptrap[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };

oid fileMonitorTable_oid[] = { fileMonitorTable_TABLE_OID };
size_t fileMonitorTable_oid_len = OID_LENGTH(fileMonitorTable_oid);

u_char* date_n_time_buf(time_t * when, u_char* buf, size_t * length);

#ifdef fileMonitorTable_IDX2
/************************************************************
 * keep binary tree to find context by name
 */
static int fileMonitorTable_cmp(const void *lhs, const void *rhs);

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int
fileMonitorTable_cmp( const void *lhs, const void *rhs )
{
    fileMonitorTable_context *context_l = (fileMonitorTable_context*)lhs;
    fileMonitorTable_context *context_r = (fileMonitorTable_context*)rhs;

    /*
     * check primary key, then secondary. Add your own code if
     * there are more than 2 indexes
     */
    int rc;

    /*
     * TODO: implement compare. Remove this ifdef code and
     * add your own code here.
     */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR,
        "fileMonitorTable_cmp not implemented! Container order undefined\n" );
    return 0;
#endif

    /*
     * EXAMPLE:
     *
     * rc = strcmp( context_l->xxName, context_r->xxName);
     *
     * if(rc)
     *   return rc;
     *
     * TODO: fix secondary keys (or delete if there are none)
     *
     * if(context_l->yy < context_r->yy)
     *   return -1;
     *
     * return (context_l->yy == context_r->yy) ? 0 : 1;
     */
}

/************************************************************
 * search tree
 */
/** TODO: set additional indexes as parameters */
fileMonitorTable_context*
fileMonitorTable_get(const char *name, int len)
{
    fileMonitorTable_context tmp;

    /** we should have a secondary index */
    netsnmp_assert(cb.container->next != NULL);

    /*
     * TODO: implement compare. Remove this ifdef code and
     * add your own code here.
     */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "fileMonitorTable_get not implemented!\n" );
    return NULL;
#endif

    /*
     * EXAMPLE:
     *
     * if(len > sizeof(tmp.xxName))
     *   return NULL;
     *
     * strncpy( tmp.xxName, name, sizeof(tmp.xxName) );
     * tmp.xxName_len = len;
     *
     * return CONTAINER_FIND(cb.container->next, &tmp);
     */
}
#endif



/************************************************************
 * the *_row_copy routine
 */
static int fileMonitorTable_row_copy(fileMonitorTable_context* dst,
                         fileMonitorTable_context* src)
{
    if(!dst||!src)
        return 1;

    /*
     * copy index, if provided
     */
    if(dst->index.oids)
        free(dst->index.oids);
    if(snmp_clone_mem( (void*)&dst->index.oids, src->index.oids,
                           src->index.len * sizeof(oid) )) {
        dst->index.oids = NULL;
        return 1;
    }
    dst->index.len = src->index.len;


    /*
     * copy components into the context structure
     */
    dst->fileMonitorIndex = src->fileMonitorIndex;

    memcpy(dst->fileMonitorName, 
			src->fileMonitorName, 
			src->fileMonitorName_len);
    dst->fileMonitorName_len = src->fileMonitorName_len;

    dst->fileMonitorCheckInterval = src->fileMonitorCheckInterval;

    memcpy(dst->fileMonitorDescription, 
			src->fileMonitorDescription, 
			src->fileMonitorDescription_len );
    dst->fileMonitorDescription_len = src->fileMonitorDescription_len;

    memcpy(dst->fileMonitorCreatedDateTime, 
			src->fileMonitorCreatedDateTime, 
			src->fileMonitorCreatedDateTime_len );
    dst->fileMonitorCreatedDateTime_len = src->fileMonitorCreatedDateTime_len;

    memcpy(dst->fileMonitorModifiedDateTime, 
			src->fileMonitorModifiedDateTime, 
			src->fileMonitorModifiedDateTime_len );
    dst->fileMonitorModifiedDateTime_len = src->fileMonitorModifiedDateTime_len;

    dst->fileMonitorFileSize = src->fileMonitorFileSize;

    memcpy(dst->fileMonitorOwner, 
			src->fileMonitorOwner, 
			src->fileMonitorOwner_len );
    dst->fileMonitorOwner_len = src->fileMonitorOwner_len;

    dst->fileMonitorReaction = src->fileMonitorReaction;

    memcpy(dst->fileMonitorReactionPath, 
			src->fileMonitorReactionPath, 
			src->fileMonitorReactionPath_len );
    dst->fileMonitorReactionPath_len = src->fileMonitorReactionPath_len;

    memcpy(dst->fileMonitorAlertMessage, 
			src->fileMonitorAlertMessage, 
			src->fileMonitorAlertMessage_len );
    dst->fileMonitorAlertMessage_len = src->fileMonitorAlertMessage_len;

    dst->fileMonitorStatus = src->fileMonitorStatus;

    return 0;
}

#ifdef fileMonitorTable_SET_HANDLING

/*
 * the *_extract_index routine
 */
int fileMonitorTable_extract_index(fileMonitorTable_context* ctx, 
			netsnmp_index* hdr)
{
    /*
     * temporary local storage for extracting oid index
     */
    netsnmp_variable_list var_fileMonitorIndex;
    int err;

    /*
     * copy index, if provided
     */
    if(hdr) {
        netsnmp_assert(ctx->index.oids == NULL);
        if(snmp_clone_mem( (void*)&ctx->index.oids, hdr->oids,
                           hdr->len * sizeof(oid) )) {
            return -1;
        }
        ctx->index.len = hdr->len;
    }

    /**
     * Create variable to hold each component of the index
     */
       memset( &var_fileMonitorIndex, 0x00, sizeof(var_fileMonitorIndex) );
       var_fileMonitorIndex.type = ASN_UNSIGNED;
       /** TODO: link this index to the next, or NULL for the last one */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "fileMonitorTable_extract_index index list not implemented!\n" );
    return 0;
#endif

    /*
     * parse the oid into the individual components
     */
    err = parse_oid_indexes( hdr->oids, hdr->len, &var_fileMonitorIndex );
    if (err == SNMP_ERR_NOERROR) {
       /*
        * copy components into the context structure
        */
		ctx->fileMonitorIndex = *var_fileMonitorIndex.val.integer;

           /*
            * TODO: check index for valid values. For EXAMPLE:
            *
              * if ( *var_fileMonitorIndex.val.integer != XXX ) {
          *    err = -1;
          * }
          */
    }

    /*
     * parsing may have allocated memory. free it.
     */
    snmp_reset_var_buffers(&var_fileMonitorIndex);

    return err;
}

/************************************************************
 * the *_can_activate routine is called
 * when a row is changed to determine if all the values
 * set are consistent with the row's rules for a row status
 * of ACTIVE.
 *
 * return 1 if the row could be ACTIVE
 * return 0 if the row is not ready for the ACTIVE state
 */
int fileMonitorTable_can_activate(fileMonitorTable_context* undo_ctx,
                      fileMonitorTable_context* row_ctx,
                      netsnmp_request_group* rg)
{
    /*
     * TODO: check for activation requirements here
     */
    return 1;
}

/************************************************************
 * the *_can_deactivate routine is called when a row that is
 * currently ACTIVE is set to a state other than ACTIVE. If
 * there are conditions in which a row should not be allowed
 * to transition out of the ACTIVE state (such as the row being
 * referred to by another row or table), check for them here.
 *
 * return 1 if the row can be set to a non-ACTIVE state
 * return 0 if the row must remain in the ACTIVE state
 */
int fileMonitorTable_can_deactivate(fileMonitorTable_context* undo_ctx,
                        fileMonitorTable_context* row_ctx,
                        netsnmp_request_group* rg)
{
    /*
     * TODO: check for deactivation requirements here
     */
    return 1;
}

/************************************************************
 * the *_can_delete routine is called to determine if a row
 * can be deleted.
 *
 * return 1 if the row can be deleted
 * return 0 if the row cannot be deleted
 */
int fileMonitorTable_can_delete(fileMonitorTable_context* undo_ctx,
                    fileMonitorTable_context* row_ctx,
                    netsnmp_request_group* rg)
{
    /*
     * probably shouldn't delete a row that we can't
     * deactivate.
     */
    if(fileMonitorTable_can_deactivate(undo_ctx, row_ctx, rg) != 1)
        return 0;

    /*
     * TODO: check for other deletion requirements here
     */
    return 1;
}

#ifdef fileMonitorTable_ROW_CREATION
/************************************************************
 * the *_create_row routine is called by the table handler
 * to create a new row for a given index. If you need more
 * information (such as column values) to make a decision
 * on creating rows, you must create an initial row here
 * (to hold the column values), and you can examine the
 * situation in more detail in the *_set_reserve1 or later
 * states of set processing. Simple check for a NULL undo_ctx
 * in those states and do detailed creation checking there.
 *
 * returns a newly allocated fileMonitorTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
fileMonitorTable_context* fileMonitorTable_create_row(netsnmp_index* hdr)
{
    fileMonitorTable_context* ctx =
        SNMP_MALLOC_TYPEDEF(fileMonitorTable_context);

    if(ctx == NULL)
        return NULL;

    /*
     * TODO: check indexes, if necessary.
     */
    if(fileMonitorTable_extract_index(ctx, hdr)) {
        free(ctx->index.oids);
        free(ctx);
        return NULL;
    }

    /*
     * TODO: initialize any default values here. This is also
     * first place you really should allocate any memory for
     * yourself to use.  If you allocated memory earlier,
     * make sure you free it for earlier error cases!
     */
     ctx->fileMonitorCheckInterval = 30;

    return ctx;
}
#endif

/************************************************************
 * the *_duplicate row routine
 */
fileMonitorTable_context*
fileMonitorTable_duplicate_row(fileMonitorTable_context* row_ctx)
{
    fileMonitorTable_context * dup;

    if(!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(fileMonitorTable_context);
    if(!dup)
        return NULL;

    if(fileMonitorTable_row_copy(dup, row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index* fileMonitorTable_delete_row(fileMonitorTable_context* ctx)
{
    if(ctx->index.oids)
        free(ctx->index.oids);

    /*
     * TODO: release any memory you allocated here...
     */

    /*
     * release header
     */
    free( ctx );

    return NULL;
}


/************************************************************
 * RESERVE is used to check the syntax of all the variables
 * provided, that the values being set are sensible and consistent,
 * and to allocate any resources required for performing the SET.
 * After this stage, the expectation is that the set ought to
 * succeed, though this is not guaranteed. (In fact, with the UCD
 * agent, this is done in two passes - RESERVE1, and
 * RESERVE2, to allow for dependancies between variables).
 *
 * BEFORE calling this routine, the agent will call duplicate_row
 * to create a copy of the row (unless this is a new row; i.e.
 * row_created == 1).
 *
 * next state -> SET_RESERVE2 || SET_FREE
 */
void fileMonitorTable_set_reserve1( netsnmp_request_group *rg )
{
    fileMonitorTable_context* row_ctx =
            (fileMonitorTable_context *)rg->existing_row;
    fileMonitorTable_context* undo_ctx =
            (fileMonitorTable_context *)rg->undo_info;
    netsnmp_variable_list* var;
    netsnmp_request_group_item* current;
    int rc;

    /*
     * TODO: loop through columns, check syntax and lengths. For
     * columns which have no dependencies, you could also move
     * the value/range checking here to attempt to catch error
     * cases as early as possible.
     */
    for (current = rg->list; current; current = current->next) {

        var = current->ri->requestvb;
        rc = SNMP_ERR_NOERROR;

        switch (current->tri->colnum) {

        case COLUMN_FILEMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
            rc = netsnmp_check_vb_type_and_size(var, ASN_UNSIGNED, 
                    sizeof(row_ctx->fileMonitorIndex));
        break;

        case COLUMN_FILEMONITORNAME:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    var->val_len);
        break;

        case COLUMN_FILEMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->fileMonitorCheckInterval));
        break;

        case COLUMN_FILEMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    var->val_len);
        break;

        case COLUMN_FILEMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    var->val_len);
        break;

        case COLUMN_FILEMONITORMODIFIEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    var->val_len);
        break;

        case COLUMN_FILEMONITORFILESIZE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->fileMonitorFileSize));
        break;

        case COLUMN_FILEMONITOROWNER:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    var->val_len);
        break;

        case COLUMN_FILEMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->fileMonitorReaction));
        break;

        case COLUMN_FILEMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    var->val_len);
        break;

        case COLUMN_FILEMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
         		   var->val_len);
        break;

        case COLUMN_FILEMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->fileMonitorStatus));
        break;

        default: /** We shouldn't get here */
            rc = SNMP_ERR_GENERR;
            snmp_log(LOG_ERR, "unknown column in "
                     "fileMonitorTable_set_reserve1\n");
        }

        if (rc)
           netsnmp_set_mode_request_error(MODE_SET_BEGIN, current->ri, rc);

        rg->status = SNMP_MAX(rg->status, current->ri->status);
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

void fileMonitorTable_set_reserve2(netsnmp_request_group *rg)
{
    fileMonitorTable_context* row_ctx = 
		(fileMonitorTable_context *)rg->existing_row;
    fileMonitorTable_context* undo_ctx = 
		(fileMonitorTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;
    netsnmp_variable_list *var;
    int rc;

    rg->rg_void = rg->list->ri;

    /*
     * TODO: loop through columns, check for valid
     * values and any range constraints.
     */
    for (current = rg->list; current; current = current->next) {

        var = current->ri->requestvb;
        rc = SNMP_ERR_NOERROR;

        switch (current->tri->colnum) {

        case COLUMN_FILEMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_FILEMONITORNAME:
            /** DisplayString = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_FILEMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_FILEMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_FILEMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_FILEMONITORMODIFIEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_FILEMONITORFILESIZE:
            /** INTEGER = ASN_INTEGER */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_FILEMONITOROWNER:
            /** DateAndTime = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_FILEMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_FILEMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_FILEMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_FILEMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
            rc = netsnmp_check_vb_rowstatus(current->ri->requestvb,
                           undo_ctx ? undo_ctx->fileMonitorStatus : 0);
                rg->rg_void = current->ri;
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }

        if (rc)
           netsnmp_set_mode_request_error(MODE_SET_BEGIN, current->ri, rc);
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

/************************************************************
 * Assuming that the RESERVE phases were successful, the next
 * stage is indicated by the action value ACTION. This is used
 * to actually implement the set operation. However, this must
 * either be done into temporary (persistent) storage, or the
 * previous value stored similarly, in case any of the subsequent
 * ACTION calls fail.
 *
 * In your case, changes should be made to row_ctx. A copy of
 * the original row is in undo_ctx.
 */
void fileMonitorTable_set_action(netsnmp_request_group* rg)
{
    fileMonitorTable_context* row_ctx = 
		(fileMonitorTable_context*)rg->existing_row;
    fileMonitorTable_context* undo_ctx = 
		(fileMonitorTable_context*)rg->undo_info;
    netsnmp_variable_list *var;
    netsnmp_request_group_item *current;
    int row_err = 0;

    /*
     * TODO: loop through columns, copy varbind values
     * to context structure for the row.
     */
    for (current = rg->list; current; current = current->next) {

        var = current->ri->requestvb;

        switch (current->tri->colnum) {

        case COLUMN_FILEMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
            row_ctx->fileMonitorIndex = *var->val.integer;
        break;

        case COLUMN_FILEMONITORNAME:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->fileMonitorName, var->val.string, var->val_len);
            row_ctx->fileMonitorName_len = var->val_len;
            row_ctx->fileMonitorName[var->val_len] = 0;
        break;

        case COLUMN_FILEMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
            row_ctx->fileMonitorCheckInterval = *var->val.integer;
        break;

        case COLUMN_FILEMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->fileMonitorDescription, var->val.string, var->val_len);
            row_ctx->fileMonitorDescription_len = var->val_len;
            row_ctx->fileMonitorDescription[var->val_len]=0;
        break;

        case COLUMN_FILEMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->fileMonitorCreatedDateTime, var->val.string, var->val_len);
            row_ctx->fileMonitorCreatedDateTime_len = var->val_len;
            row_ctx->fileMonitorCreatedDateTime[var->val_len]=0;
        break;

        case COLUMN_FILEMONITORMODIFIEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->fileMonitorModifiedDateTime, var->val.string, var->val_len);
            row_ctx->fileMonitorModifiedDateTime_len = var->val_len;
            row_ctx->fileMonitorModifiedDateTime[var->val_len]=0;
        break;

        case COLUMN_FILEMONITORFILESIZE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->fileMonitorFileSize = *var->val.integer;
        break;

        case COLUMN_FILEMONITOROWNER:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->fileMonitorOwner, var->val.string, var->val_len);
            row_ctx->fileMonitorOwner_len = var->val_len;
            row_ctx->fileMonitorOwner[var->val_len]=0;
        break;

        case COLUMN_FILEMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
            row_ctx->fileMonitorReaction = *var->val.integer;
        break;

        case COLUMN_FILEMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->fileMonitorReactionPath, var->val.string, var->val_len);
            row_ctx->fileMonitorReactionPath_len = var->val_len;
            row_ctx->fileMonitorReactionPath[var->val_len]=0;
        break;

        case COLUMN_FILEMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->fileMonitorAlertMessage, var->val.string, var->val_len);
            row_ctx->fileMonitorAlertMessage_len = var->val_len;
            row_ctx->fileMonitorAlertMessage[var->val_len]=0;
        break;

        case COLUMN_FILEMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
            row_ctx->fileMonitorStatus = *var->val.integer;
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
#ifndef fileMonitorTable_CAN_MODIFY_ACTIVE_ROW
    if (undo_ctx && RS_IS_ACTIVE(undo_ctx->fileMonitorStatus) &&
        row_ctx && RS_IS_ACTIVE(row_ctx->fileMonitorStatus)) {
        row_err = 1;
    }
#endif

    /*
     * check activation/deactivation
     */
    row_err = netsnmp_table_array_check_row_status(&cb, rg,
                      row_ctx ? &row_ctx->fileMonitorStatus : NULL,
                      undo_ctx ? &undo_ctx->fileMonitorStatus : NULL);
    if (row_err) {
        netsnmp_set_mode_request_error(MODE_SET_BEGIN,
                                       (netsnmp_request_info*)rg->rg_void,
                                       row_err);
        return;
    }

    /*
     * TODO: if you have dependencies on other tables, this would be
     * a good place to check those, too.
     */
}

/************************************************************
 * Only once the ACTION phase has completed successfully, can
 * the final COMMIT phase be run. This is used to complete any
 * writes that were done into temporary storage, and then release
 * any allocated resources. Note that all the code in this phase
 * should be "safe" code that cannot possibly fail (cue
 * hysterical laughter). The whole intent of the ACTION/COMMIT
 * division is that all of the fallible code should be done in
 * the ACTION phase, so that it can be backed out if necessary.
 *
 * BEFORE calling this routine, the agent will update the
 * container (inserting a row if row_created == 1, or removing
 * the row if row_deleted == 1).
 *
 * AFTER calling this routine, the agent will delete the
 * undo_info.
 */
void fileMonitorTable_set_commit(netsnmp_request_group* rg)
{
    fileMonitorTable_context* row_ctx = 
		(fileMonitorTable_context *)rg->existing_row;
    fileMonitorTable_context *undo_ctx = 
		(fileMonitorTable_context *)rg->undo_info;
    netsnmp_variable_list* var;
    netsnmp_request_group_item* current;
    pthread_t psThr;
    time_t stNow;

	struct stat stbuf;
#ifndef WIN32
	struct passwd* pwd = NULL;
#endif
	u_char date_buf[20];


    DEBUGMSGTL(("FileMon/fileMonitorTable_set_commit",
                    "Commit is being called\n"));

    if (FileMon_UpdateDataFile(row_ctx) == 0) {
        DEBUGMSGTL(("FileMon/fileMonitorTable_set_commit",
            "Row updated in the FILE_MON_DATA_FILE\n"));
    }

    // Do the value check, before proceeding
    if ((strlen((char*)row_ctx->fileMonitorName) != 0) &&
        (row_ctx->fileMonitorCheckInterval != 0) &&
        (row_ctx->fileMonitorStatus == SNMP_ROW_ACTIVE )) {

        // Row Created/ Changed Time
        row_ctx->fileMonitorCreatedDateTime_len = MAX_DATE_TIME_LEN;

        // Get the Date-Time in time_t format
        // and update the created time for the newly created row
        (void *)time(&stNow);

		memset(date_buf, 0, sizeof(date_buf));

        strcpy(row_ctx->fileMonitorCreatedDateTime,
               (u_char *)date_n_time_buf(&stNow, date_buf,
               (size_t*)&(row_ctx->fileMonitorCreatedDateTime_len)));

		// shkim 2009.06.26
		// Check valid file name
		// get file size
		// get file owner

		memset(&stbuf, 0, sizeof(stbuf));
		memset(date_buf, 0, sizeof(date_buf));

		if (stat(row_ctx->fileMonitorName, &stbuf) == -1) {		/* failed */
			DEBUGMSGTL(("FileMon/fileMonitorTable_set_commit",
				"stat(%s) failed\n", row_ctx->fileMonitorName));

			row_ctx->fileMonitorFileSize = 0;
			row_ctx->fileMonitorModifiedDateTime_len = MAX_DATE_TIME_LEN;

			strcpy(row_ctx->fileMonitorModifiedDateTime,
				(u_char *)date_n_time_buf(&stNow, date_buf,
				(size_t*)&(row_ctx->fileMonitorModifiedDateTime_len)));

			row_ctx->fileMonitorOwner_len = 0;

		} else {							/* success */
			DEBUGMSGTL(("FileMon/fileMonitorTable_set_commit",
				"stat(%s) success\n", row_ctx->fileMonitorName));

			row_ctx->fileMonitorFileSize = stbuf.st_size;
			row_ctx->fileMonitorModifiedDateTime_len = MAX_DATE_TIME_LEN;

			strcpy(row_ctx->fileMonitorModifiedDateTime,
				(u_char *)date_n_time_buf(&stbuf.st_mtime, date_buf,
				(size_t*)&(row_ctx->fileMonitorModifiedDateTime_len)));

#ifndef WIN32
			if ((pwd = getpwuid(stbuf.st_uid)) != NULL) {
				row_ctx->fileMonitorOwner_len = MAX_OWNER_LEN;
				strncpy(row_ctx->fileMonitorOwner,
					pwd->pw_name, row_ctx->fileMonitorOwner_len);
			} else {
				row_ctx->fileMonitorOwner_len = 0;
			}
#else
			row_ctx->fileMonitorOwner_len = 0;
#endif
		}

        // i.e. if the thr has not been created yet
        if (row_ctx->threadActive == 0) {
            if (pthread_create(&psThr, NULL, 
							FileStatusCheckThread,
                            (void*)row_ctx) != 0) {
                snmp_log_perror("Thread creation failed");
            } else {
                // Change it to active
                row_ctx->threadActive = 1;

                DEBUGMSGTL(("FileMon/fileMonitorTable_set_commit",
                    "Thread started for < %u %d %s >\n",
                    row_ctx->idx,
                    row_ctx->index.len,
                    row_ctx->fileMonitorName));
            }
        }
    } else {
		DEBUGMSGTL(("FileMon/fileMonitorTable_set_commit",
			"if failed (%s, %d, %d, %d)\n",
			row_ctx->fileMonitorName,
			row_ctx->fileMonitorCheckInterval,
			row_ctx->fileMonitorStatus, SNMP_ROW_ACTIVE));
	}
}

/************************************************************
 * If either of the RESERVE calls fail, the write routines
 * are called again with the FREE action, to release any resources
 * that have been allocated. The agent will then return a failure
 * response to the requesting application.
 *
 * AFTER calling this routine, the agent will delete undo_info.
 */
void fileMonitorTable_set_free(netsnmp_request_group* rg)
{
    fileMonitorTable_context* row_ctx = 
		(fileMonitorTable_context *)rg->existing_row;
    fileMonitorTable_context* undo_ctx = 
		(fileMonitorTable_context *)rg->undo_info;
    netsnmp_variable_list* var;
    netsnmp_request_group_item* current;

    /*
     * loop through columns
     */
    for (current = rg->list; current; current = current->next) {

        var = current->ri->requestvb;

        switch (current->tri->colnum) {

        case COLUMN_FILEMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
        break;

        case COLUMN_FILEMONITORNAME:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_FILEMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORMODIFIEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORFILESIZE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_FILEMONITOROWNER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_FILEMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
        break;

        default: /** We shouldn't get here */
            /** should have been logged in reserve1 */
        break;
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

/************************************************************
 * If the ACTION phase does fail (for example due to an apparently
 * valid, but unacceptable value, or an unforeseen problem), then
 * the list of write routines are called again, with the UNDO
 * action. This requires the routine to reset the value that was
 * changed to its previous value (assuming it was actually changed),
 * and then to release any resources that had been allocated. As
 * with the FREE phase, the agent will then return an indication
 * of the error to the requesting application.
 *
 * BEFORE calling this routine, the agent will update the container
 * (remove any newly inserted row, re-insert any removed row).
 *
 * AFTER calling this routing, the agent will call row_copy
 * to restore the data in existing_row from the date in undo_info.
 * Then undo_info will be deleted (or existing row, if row_created
 * == 1).
 */
void fileMonitorTable_set_undo(netsnmp_request_group *rg)
{
    fileMonitorTable_context* row_ctx = 
		(fileMonitorTable_context *)rg->existing_row;
    fileMonitorTable_context* undo_ctx = 
		(fileMonitorTable_context *)rg->undo_info;
    netsnmp_variable_list* var;
    netsnmp_request_group_item* current;

    /*
     * loop through columns
     */
    for (current = rg->list; current; current = current->next) {

        var = current->ri->requestvb;

        switch (current->tri->colnum) {

        case COLUMN_FILEMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
        break;

        case COLUMN_FILEMONITORNAME:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_FILEMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORMODIFIEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORFILESIZE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_FILEMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_FILEMONITOROWNER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_FILEMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

#endif /** fileMonitorTable_SET_HANDLING */


/************************************************************
 *
 * Initialize the fileMonitorTable table by defining 
 *
 * its contents and how it's structured
 */
void initialize_table_fileMonitorTable(void)
{
    netsnmp_table_registration_info* table_info;

    if (my_handler) {
        snmp_log(LOG_ERR, "initialize_table_fileMonitorTable_handler called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /* if your table is read only, it's easiest to change the
       HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY */
    my_handler = netsnmp_create_handler_registration("fileMonitorTable",
                                  netsnmp_table_array_helper_handler,
                                  fileMonitorTable_oid,
                                  fileMonitorTable_oid_len,
                                  HANDLER_CAN_RWRITE);

    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_fileMonitorTable_handler\n");
        return; /** mallocs failed */
    }

    /***************************************************
     * Setting up the table's definition
     */

    /*
     * TODO: add any external indexes here.
     */

    /*
     * internal indexes
     */

	/** index: fileMonitorIndex */
	netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

    table_info->min_column = fileMonitorTable_COL_MIN;
    table_info->max_column = fileMonitorTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = fileMonitorTable_get_value;
    cb.container = netsnmp_container_find("fileMonitorTable_primary:"
                                          "fileMonitorTable:"
                                          "table_container");
#ifdef fileMonitorTable_IDX2
    netsnmp_container_add_index(cb.container,
            netsnmp_container_find("fileMonitorTable_secondary:"
                                   "fileMonitorTable:"
                                   "table_container"));
    cb.container->next->compare = fileMonitorTable_cmp;
#endif

#ifdef fileMonitorTable_SET_HANDLING
    cb.can_set = 1;
#ifdef fileMonitorTable_ROW_CREATION
    cb.create_row = (UserRowMethod*)fileMonitorTable_create_row;
#endif
    cb.duplicate_row = (UserRowMethod*)fileMonitorTable_duplicate_row;
    cb.delete_row = (UserRowMethod*)fileMonitorTable_delete_row;
    cb.row_copy = (Netsnmp_User_Row_Operation *)fileMonitorTable_row_copy;

    cb.can_activate = (Netsnmp_User_Row_Action *)fileMonitorTable_can_activate;
    cb.can_deactivate = (Netsnmp_User_Row_Action *)fileMonitorTable_can_deactivate;
    cb.can_delete = (Netsnmp_User_Row_Action *)fileMonitorTable_can_delete;

    cb.set_reserve1 = fileMonitorTable_set_reserve1;
    cb.set_reserve2 = fileMonitorTable_set_reserve2;
    cb.set_action = fileMonitorTable_set_action;
    cb.set_commit = fileMonitorTable_set_commit;
    cb.set_free = fileMonitorTable_set_free;
    cb.set_undo = fileMonitorTable_set_undo;
#endif

    DEBUGMSGTL(("FileMon/initialize_table_fileMonitorTable",
                "Registering table fileMonitorTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);
}

/************************************************************
 * fileMonitorTable_get_value
 */
int fileMonitorTable_get_value(
            netsnmp_request_info* request,
            netsnmp_index* item,
            netsnmp_table_request_info* table_info)
{
    netsnmp_variable_list* var = request->requestvb;
    fileMonitorTable_context* context = (fileMonitorTable_context *)item;

    switch (table_info->colnum) {

    case COLUMN_FILEMONITORINDEX:
        /** UNSIGNED32 = ASN_UNSIGNED */
        snmp_set_var_typed_value(var, ASN_UNSIGNED,
                     (char*)&context->fileMonitorIndex,
                     sizeof(context->fileMonitorIndex));
    break;

    case COLUMN_FILEMONITORNAME:
        /** DisplayString = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                     (char*)&context->fileMonitorName,
                     context->fileMonitorName_len);
    break;

    case COLUMN_FILEMONITORCHECKINTERVAL:
        /** INTEGER = ASN_INTEGER */
        snmp_set_var_typed_value(var, ASN_INTEGER,
                     (char*)&context->fileMonitorCheckInterval,
                     sizeof(context->fileMonitorCheckInterval));
    break;

    case COLUMN_FILEMONITORDESCRIPTION:
        /** DisplayString = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                 (char*)&context->fileMonitorDescription,
                     context->fileMonitorDescription_len);
    break;

    case COLUMN_FILEMONITORCREATEDDATETIME:
        /** DateAndTime = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                     (char*)&context->fileMonitorCreatedDateTime,
                     context->fileMonitorCreatedDateTime_len );
    break;

    case COLUMN_FILEMONITORMODIFIEDDATETIME:
        /** DateAndTime = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                     (char*)&context->fileMonitorModifiedDateTime,
                     context->fileMonitorModifiedDateTime_len );
    break;

    case COLUMN_FILEMONITORFILESIZE:
        /** INTEGER = ASN_INTEGER */
        snmp_set_var_typed_value(var, ASN_INTEGER,
                     (char*)&context->fileMonitorFileSize,
                     sizeof(context->fileMonitorFileSize));
    break;

    case COLUMN_FILEMONITOROWNER:
        /** DisplayString = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                     (char*)&context->fileMonitorOwner,
                     context->fileMonitorOwner_len);
    break;

    case COLUMN_FILEMONITORREACTION:
        /** INTEGER = ASN_INTEGER */
        snmp_set_var_typed_value(var, ASN_INTEGER,
                     (char*)&context->fileMonitorReaction,
                     sizeof(context->fileMonitorReaction));
    break;

    case COLUMN_FILEMONITORREACTIONPATH:
        /** DisplayString = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                     (char*)&context->fileMonitorReactionPath,
                     context->fileMonitorReactionPath_len);
    break;

    case COLUMN_FILEMONITORALERTMESSAGE:
        /** DisplayString = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                     (char*)&context->fileMonitorAlertMessage,
                     context->fileMonitorAlertMessage_len);
    break;

    case COLUMN_FILEMONITORSTATUS:
        /** RowStatus = ASN_INTEGER */
        snmp_set_var_typed_value(var, ASN_INTEGER,
                     (char*)&context->fileMonitorStatus,
                     sizeof(context->fileMonitorStatus));
    break;

    default: /** We shouldn't get here */
        snmp_log(LOG_ERR, "unknown column in "
                 "fileMonitorTable_get_value\n");
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

/************************************************************
 * fileMonitorTable_get_by_idx
 */
const fileMonitorTable_context*
fileMonitorTable_get_by_idx(netsnmp_index* hdr)
{
    return (const fileMonitorTable_context *)
        CONTAINER_FIND(cb.container, hdr);
}

/*****************************************************************************
 *
 * name             :   FileMon_MasterThread
 * description      :   This is the master thread function which gets called
 *                      at the initialization of table
 * input parameters :   None
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   None
 */

void FileMon_MasterThread()
{
    pthread_t fileMonThread;
    fileMonitorTable_context* fileRow = NULL;
	struct stat stbuf;

    // Get the first row in the container
    fileRow = CONTAINER_FIRST(cb.container);

    // As long as we get valid rows

    while (fileRow) {

        // Do the value check, before proceeding

        if ((strlen((char*)fileRow->fileMonitorName) != 0 ) &&
            (fileRow->fileMonitorCheckInterval != 0 )) {

            if (pthread_create(&fileMonThread, NULL,
                    FileStatusCheckThread, (void*)fileRow) != 0 ) {
                snmp_log_perror("Thread creation failed");
            } else {
                // Change it to active
                fileRow->threadActive = 1;

                DEBUGMSGTL(("FileMon/FileMon_MasterThread",
                    "#%d# Master Thread started for < %u %d %s >\n",
                    MY_THR_ID,
                    fileRow->idx,
                    fileRow->index.len,
                    fileRow->fileMonitorName));
            }
        } else {
            // Change it to not-active
            fileRow->threadActive = 0;

            snmp_log(LOG_ERR, "FileMon/FileMon_MasterThread \
                #%d# Thread could not be started for OID < %u >\n",
                MY_THR_ID, fileRow->idx);
        }

        // Get the next row
        fileRow = CONTAINER_NEXT(cb.container, fileRow);
    }
}


/*****************************************************************************
 *
 * name             :   FileStatusCheckThread
 * description      :   This is the main function for thread
 * input parameters :   void *psData - the File Monitoring row context
 * output parameters:   None
 * return type      :   void *
 * global variables :   None
 * calls            :   None
 */

void* FileStatusCheckThread(void *fileData)
{
    fileMonitorTable_context* myContext = NULL;
	stVarBindArgProc* varBinds = NULL;
	char tmpdate[MAX_DATE_TIME_LEN+1];
    struct stat stbuf;
	u_char date_buf[20];

    DEBUGMSGTL(("FileMon/FileStatusCheckThread",
            "#%d# Thread spawned\n", MY_THR_ID));

    myContext = (fileMonitorTable_context*)fileData;

    while (1) {
		memset(&stbuf, 0, sizeof(stbuf));
		memset(tmpdate, 0, sizeof(tmpdate));
		memset(date_buf, 0, sizeof(date_buf));

        // Before i start checking for file, get the latest values of the data
        myContext = CONTAINER_FIND(cb.container, &(myContext->index));

        // If the row does not exist anymore
        if (myContext == NULL) {
            DEBUGMSGTL(("FileMon/FileStatusCheckThread",
                "#%d# Thread exit; myContext == NULL\n", MY_THR_ID));
            pthread_detach(pthread_self());
            pthread_exit(NULL);
        }

        // Check, if the row status is not active, exit the thread
        if (myContext->fileMonitorStatus != SNMP_ROW_ACTIVE) {
            DEBUGMSGTL(("FileMon/FileStatusCheckThread",
                "#%d# Thread exit; not active\n", MY_THR_ID));

            // Unset the thread active flag
            myContext->threadActive = 0;
            pthread_detach(pthread_self());
            pthread_exit(NULL);
        }

        // Check if the Row is active, else continue until it becomes active
        if (myContext->fileMonitorStatus == SNMP_ROW_ACTIVE) {

            // Do the value check, before proceeding
            if ((strlen((char*)myContext->fileMonitorName) == 0 ) ||
                (myContext->fileMonitorCheckInterval == 0 )) {
                SLEEP(TIME_TO_SLEEP);
                continue;   // No need to process the request,
                            // until valid values are provided
            }

            // For reaction 2, 3 check for reaction path also.
            if ( (myContext->fileMonitorReaction == 2 ||
                myContext->fileMonitorReaction == 3 ) &&
                strlen((char*)myContext->fileMonitorReactionPath) == 0) {
                SLEEP(TIME_TO_SLEEP);
                continue;   // No need to process the request,
                            // until valid values are provided
            }
        } else {
            // since the row is not active
            SLEEP(TIME_TO_SLEEP);
            continue;
        }

        DEBUGMSGTL(("FileMon/FileStatusCheckThread",
            "#%d# Thread OID <%u> OID_LEN <%d> FILE <%s>\n",
            MY_THR_ID,
            *(myContext->index.oids),
            myContext->index.len,
            myContext->fileMonitorName));

		// shkim 2009.06.26
        // Check for the file status

		if (stat(myContext->fileMonitorName, &stbuf) == -1) {	/* failed */
            SLEEP(TIME_TO_SLEEP);
            continue;
		}

		/* success */
		myContext->fileMonitorFileSize = stbuf.st_size;	 /* update */

		strcpy(tmpdate, 
			(u_char *)date_n_time_buf(&stbuf.st_mtime, date_buf,
			(size_t*)&(myContext->fileMonitorModifiedDateTime_len)));

		if (strncmp(myContext->fileMonitorModifiedDateTime, tmpdate,
			MAX_DATE_TIME_LEN) != 0) {	/* modified */

			// shkim 2009.06.26
			// send trap

			memset(date_buf, 0, sizeof(date_buf));

			strcpy(myContext->fileMonitorModifiedDateTime, 
				(u_char *)date_n_time_buf(&stbuf.st_mtime, date_buf,
				(size_t*)&(myContext->fileMonitorModifiedDateTime_len)));

			varBinds = makeFileMonitorVarBind_Modified(myContext);
			Common_SendAlert_And_Vars("FLMN", gcInfoLogFileProc,
				myContext->fileMonitorAlertMessage,
				myContext->fileMonitorIndex, varBinds, 6);
		}

        // Sleep until the next interval
        DEBUGMSGTL(("FileMon/FileStatusCheckThread",
            "#%d# Thread is going to sleep for <%u> seconds!\n",
            MY_THR_ID, myContext->fileMonitorCheckInterval));

#ifdef WIN32
        SLEEP(MAX(myContext->fileMonitorCheckInterval, CHECK_INTERVAL));
#else
        SLEEP(myContext->fileMonitorCheckInterval);
#endif

        DEBUGMSGTL(("FileMon/FileStatusCheckThread",
            "#%d# Thread woke up!\n", MY_THR_ID));

    } // End of infinite while loop
}


/*****************************************************************************
 *
 * name             :   FileMon_LoadContainerFromFile
 * description      :   This function is used load the default configuration
 *                      from the data file
 * input parameters :   None
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   
 */

#ifndef WIN32

void FileMon_LoadContainerFromFile()
{
    int fd = 0;
    int fdTmp = 0;
    int cnt = 0;
    int rewrite = 0;
    fileMonitorTable_context ctx = {0};
    fileMonitorTable_context* row = NULL;

    // Open the Data file
    fd = open(FILE_MON_DATA_FILE, O_RDONLY);

    if (fd == -1) {
        snmp_log_perror("FILE_MON_DATA_FILE open failed\n");
		return;
	}

	DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
		"#%d# Thread <%s> file opened\n",
		MY_THR_ID, FILE_MON_DATA_FILE));

	while (read(fd, &ctx, FILE_MON_CONTEXT_SIZE) > 0) {
		// If the row, is already destroyed, no need to create it again.
		if (ctx.fileMonitorStatus == SNMP_ROW_DESTROY) {
			rewrite = 1;   // to re-write the mib data file
			continue; // As we have to load the container
		}

        DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
			"Loading MIB Row for <%s %s %s>\n", 
				ctx.fileMonitorName,
				ctx.fileMonitorModifiedDateTime,
				ctx.fileMonitorOwner));

		row = (fileMonitorTable_context*)malloc(FILE_MON_CONTEXT_SIZE);

		if (row == NULL) {
			snmp_log_perror("FileMon: Malloc failed");
			return;
		}

		memcpy (row, &ctx, FILE_MON_CONTEXT_SIZE);

		netsnmp_assert(row->index.oids == NULL);

		if (snmp_clone_mem( (void*)&row->index.oids,
				&(ctx.idx), ctx.index.len * sizeof(oid) )) {

			DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
				"#%d# Thread <%s> file closed; return\n",
				MY_THR_ID, FILE_MON_DATA_FILE));

			free(row);
			close(fd);
			return;
		}

		row->index.len = ctx.index.len;

		cnt = CONTAINER_INSERT(cb.container, row);

		if (cnt) {
			DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
			"ERROR IN CONTAINER_INSERT FUNCION\n"));
		}

		DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
			"OID in FileMon_LoadContainerFromFile <%u>\n",
			*(row->index.oids)));
	}	/* while */

	DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
		"CONTAINER SIZE=%u\n", CONTAINER_SIZE(cb.container)));

	if (rewrite == 0) {
		close (fd);
		return;
	}

	DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
		"Rewriting the data file\n"));

	// open a new temporary file
	if ((fdTmp = open (FILE_MON_DATA_FILE_TMP, O_CREAT| O_RDWR)) == -1) {
		snmp_log_perror("FILE_MON_DATA_FILE_TMP open failed\n");
		close(fd);
		return;
	}

	DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
		"#%d# Thread <%s> file opened\n",
		MY_THR_ID, FILE_MON_DATA_FILE_TMP));

	// Reset the file pointers to begining of the files
	lseek(fd, 0, SEEK_SET);
	lseek(fdTmp, 0, SEEK_SET);

	while (read(fd, &ctx, FILE_MON_CONTEXT_SIZE) > 0) {
		if (ctx.fileMonitorStatus != SNMP_ROW_DESTROY) {
			if (write(fdTmp, &ctx, FILE_MON_CONTEXT_SIZE) < 0)
				snmp_log_perror(("Error while writing data to FILE_MON_DATA_FILE"));
		}
	} /* while */

	// close temporary file
	close(fdTmp);
	close(fd);

	DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
		"#%d# Thread <%s> file closed\n",
		MY_THR_ID, FILE_MON_DATA_FILE_TMP));

	// Delete the old file and rename
	unlink(FILE_MON_DATA_FILE);
	rename(FILE_MON_DATA_FILE_TMP, FILE_MON_DATA_FILE);

	DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
		"#%d# Thread <%s> file closed\n",
		MY_THR_ID, FILE_MON_DATA_FILE));

	DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
		"CONTAINER SIZE=%u\n", CONTAINER_SIZE(cb.container)));
}

#else

// For Windows

void FileMon_LoadContainerFromFile()
{
    HANDLE  hFd = NULL;
    HANDLE  hFdTmp = NULL;
    DWORD num = 0;
    int iCnt = 0;
    int iIdx = 0;
    int iRewriteFlag=0;
    fileMonitorTable_context ctx = {0};
    fileMonitorTable_context* row = NULL;

    // Open the Data file
    hFd = CreateFile   (FILE_MON_DATA_FILE,
                        GENERIC_READ,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    if (hFd == INVALID_HANDLE_VALUE) {
        snmp_log_perror("FILE_MON_DATA_FILE file open failed\n");
		return;
	}

	DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
		"#%d# Thread <%s> file opened\n",
		MY_THR_ID, FILE_MON_DATA_FILE));

	iIdx=0;

	while (ReadFile(hFd, &ctx, FILE_MON_CONTEXT_SIZE, &num, NULL)) {
		snmp_log(LOG_ERR, "Size <%d> <%d>\n", num, FILE_MON_CONTEXT_SIZE);
            
		if (num == 0) {
			snmp_log(LOG_ERR, " %d breaking out \n", GetLastError());
			break;
		}

		// If the row, is already destroyed, no need to create it again.
		if (ctx.fileMonitorStatus == SNMP_ROW_DESTROY ) {
			iRewriteFlag = 1;   // to re-write the mib data file
			continue; // As we have to load the container anyways
		}

		DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
			"Loading MIB Row for File Name <%s>\n", ctx.fileMonitorName));

		row = (fileMonitorTable_context*)malloc(FILE_MON_CONTEXT_SIZE);

		if (row == NULL) {
			snmp_log_perror("FileMon: Malloc failed");
			return;
		}

		memcpy(row, &ctx, FILE_MON_CONTEXT_SIZE);

		netsnmp_assert(row->index.oids == NULL);

		if (snmp_clone_mem((void*)&row->index.oids,
			&(ctx.idx), ctx.index.len * sizeof(oid) )) {
			free(row);
			CloseHandle(hFd);
			return;
		}

		row->index.len = ctx.index.len;

		iCnt = CONTAINER_INSERT(cb.container, row);

		if (iCnt) {
			DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
				"ERROR IN CONTAINER_INSERT FUNCION\n"));
		}

		DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
			"OID in FileMon_LoadContainerFromFile <%u>\n", 
			*(row->index.oids)));
	} /* while */

	DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
		"CONTAINER SIZE=%u\n", CONTAINER_SIZE(cb.container)));

	if (iRewriteFlag == 0) {
		CloseHandle(hFd);
		return;
	}

	DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
		"Rewriting the data file\n"));

	hFdTmp = CreateFile(FILE_MON_DATA_FILE_TMP,
					GENERIC_READ|GENERIC_WRITE,
					0,
					NULL,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL);

	if (hFdTmp == INVALID_HANDLE_VALUE) {
		snmp_log_perror("FILE_MON_DATA_FILE_TMP open failed\n");
		CloseHandle(hFd);
		return;
	}

	DEBUGMSGTL(("FileMon/FileMon_LoadContainerFromFile",
		"#%d# Thread <%s> file opened\n",
		MY_THR_ID, FILE_MON_DATA_FILE_TMP));

	// Reset the file pointers to begining of the files
	SetFilePointer(hFd, 0, NULL, FILE_BEGIN);
	SetFilePointer(hFdTmp, 0, NULL, FILE_BEGIN);

	iIdx=0;
                
	while (ReadFile(hFd, &ctx, FILE_MON_CONTEXT_SIZE, &num, NULL)) {
		if (num == 0) {
			snmp_log(LOG_ERR," %d breaking out \n",GetLastError());
			break;
		}
            
		if (ctx.fileMonitorStatus != SNMP_ROW_DESTROY)
			if (WriteFile(hFdTmp, &ctx, FILE_MON_CONTEXT_SIZE, &num, NULL) == 0)
				snmp_log_perror(("Error while writing data to FILE_MON_DATA_FILE"));
	} // End of while

	// Close the file
	CloseHandle(hFdTmp);
	CloseHandle(hFd);

	// Delete the old file and rename
	unlink(FILE_MON_DATA_FILE);
	rename(FILE_MON_DATA_FILE_TMP, FILE_MON_DATA_FILE);
}

#endif

/*****************************************************************************
 *
 * name             :   FileMon_UpdateDataFile
 * description      :   This function is used to update the MIB data file with
 *                      the latest record
 * input parameters :   fileMonitorTable_context* fileMonRow
 * output parameters:   None
 * return type      :   int - "0" for success in writing new record,
 *                          - "-1" for error,
 *                          - "1" for success in modifying existing record
 * global variables :   None
 * calls            :   None
 */

#ifndef WIN32

// Function for Unix flavors
int FileMon_UpdateDataFile(fileMonitorTable_context* fileMonRow)
{
    fileMonitorTable_context inFile;
    int iFd=-1;
    int iFoundFlag=0;
    int iRowNum=0;
    unsigned long ulPos=0;

    // Open the MIB data file
    // Added 'S_IREAD|S_IWRITE' for Windows
    if ((iFd = open(FILE_MON_DATA_FILE,O_CREAT|O_RDWR,S_IREAD|S_IWRITE)) < 0) {
        snmp_log_perror("Error opening FILE_MON_DATA_FILE");
        return(-1);
    } else {
        DEBUGMSGTL(("FileMon/FileMon_UpdateDataFile",
            "#%d# Thread <%s> file opened\n",
            MY_THR_ID, FILE_MON_DATA_FILE));

        lseek(iFd, 0,SEEK_SET);

        if (fileMonRow->fileMonitorStatus == SNMP_ROW_DESTROY)
            fileMonRow->threadActive = 0;

        while(read(iFd, &inFile, FILE_MON_CONTEXT_SIZE) > 0) {
            if (inFile.idx == *fileMonRow->index.oids) {
                // Check if the row entry in the file
                // is actually deleted fromthe container
                if (inFile.fileMonitorStatus == SNMP_ROW_DESTROY)
                    iFoundFlag =0;
                else
                    iFoundFlag =1;

                if (lseek(iFd, ulPos, SEEK_SET) == (ulPos -1)) {
                    snmp_log_perror(
                        "Error while re-setting file descriptor position");
                } else {
                    DEBUGMSGTL(("FileMon/FileMon_UpdateDataFile",
                        "#%d# Thread OID <%u> found FoundFlag <%d>\n",
                         MY_THR_ID, *fileMonRow->index.oids, iFoundFlag));
                }

                break;
            }

            ulPos = lseek(iFd, 0, SEEK_CUR);
        }

        // Copy the oid, instead copying the pointer
        fileMonRow->idx = *(fileMonRow->index.oids);

        if (write(iFd, fileMonRow, FILE_MON_CONTEXT_SIZE) < 0) {
            snmp_log_perror("Error while writing data to FILE_MON_DATA_FILE");

            DEBUGMSGTL(("FileMon/FileMon_UpdateDataFile",
                "#%d# Thread <%s> file closed\n",
                MY_THR_ID, FILE_MON_DATA_FILE));

            close(iFd);
            return(-1);
        } else {
            DEBUGMSGTL(("FileMon/FileMon_UpdateDataFile",
                "#%d# Thread Record written successfully to data file <%d>\n",
                MY_THR_ID, fileMonRow->threadActive));
            DEBUGMSGTL(("FileMon/FileMon_UpdateDataFile",
                "#%d# Thread debug <%s %s %s>\n",
                MY_THR_ID, 
				fileMonRow->fileMonitorName,
				fileMonRow->fileMonitorModifiedDateTime,
				fileMonRow->fileMonitorOwner));
		}
    }

    close(iFd);

    DEBUGMSGTL(("FileMon/FileMon_UpdateDataFile",
        "#%d# Thread <%s> file closed\n",
        MY_THR_ID, FILE_MON_DATA_FILE));

    return(iFoundFlag);
}

#else

// Function for windows
int FileMon_UpdateDataFile(fileMonitorTable_context *fileMonRow)
{
    fileMonitorTable_context inFile;
    HANDLE hFd = NULL;
    DWORD NumberOfBytesRead;
    int iFoundFlag=0;
    int iRowNum=0;
    unsigned long ulPos=0;
    static int iashtemp;

    // Open the MIB data file
    hFd = CreateFile   (FILE_MON_DATA_FILE,
                        GENERIC_READ|GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    if(hFd == INVALID_HANDLE_VALUE) {
        snmp_log_perror("Error opening FILE_MON_DATA_FILE");
        return(-1);
    } else {
        DEBUGMSGTL(("FileMon/FileMon_UpdateDataFile",
            "#%d# Thread <%s> file opened\n",
            MY_THR_ID, FILE_MON_DATA_FILE));

        SetFilePointer(hFd, 0, NULL, FILE_BEGIN);
    
        if (fileMonRow->fileMonitorStatus == SNMP_ROW_DESTROY)
            fileMonRow->threadActive = 0;

        while (ReadFile(hFd, &inFile, FILE_MON_CONTEXT_SIZE,
                &NumberOfBytesRead, NULL)) {
            if (NumberOfBytesRead== 0) {
               snmp_log(LOG_ERR," %d breaking out \n", GetLastError());
               break;
            }

            if (inFile.idx == *fileMonRow->index.oids) {
                // Check if the row entry in the file
                // is actually deleted fromthe container
                if (inFile.fileMonitorStatus == SNMP_ROW_DESTROY)
                    iFoundFlag =0;
                else
                    iFoundFlag =1;

                if (SetFilePointer(hFd,ulPos,NULL,FILE_BEGIN) == (ulPos -1)) {
                    snmp_log_perror(
                        "Error while re-setting file descriptor position");
                } else {
                    DEBUGMSGTL(("FileMon/FileMon_UpdateDataFile",
                        "#%d# Thread OID <%u> found FoundFlag <%d>\n",
                        MY_THR_ID, *fileMonRow->index.oids, iFoundFlag));

                }

                break;
            }

            ulPos = SetFilePointer(hFd, 0, NULL, FILE_CURRENT);
        }

        // Copy the oid, instead copying the pointer
        fileMonRow->idx = *(fileMonRow->index.oids);

        if (WriteFile (hFd,
                       fileMonRow,
                       FILE_MON_CONTEXT_SIZE,
                       &NumberOfBytesRead,
                       NULL) == 0 ) {
            snmp_log_perror("Error while writing data to FILE_MON_DATA_FILE");
            DEBUGMSGTL(("FileMon/FileMon_UpdateDataFile",
                "#%d# Thread data file closed\n", MY_THR_ID));
            CloseHandle(hFd);
            return(-1);
        } else {
            DEBUGMSGTL(("FileMon/FileMon_UpdateDataFile",
                "#%d# Thread Record written successfully to the data file\n",
                MY_THR_ID));
		}
    }

    CloseHandle(hFd);
    return(iFoundFlag);
}

#endif


/************************************************************
 * Initializes the fileMonitorTable module
 */
void init_fileMonitorTable(void)
{
    initialize_table_fileMonitorTable();

    /*
     * TODO: perform any startup stuff here
     */

    FileMon_LoadContainerFromFile();

    FileMon_MasterThread();
}


stVarBindArgProc *makeFileMonitorVarBind_Modified(fileMonitorTable_context *myContext)
{
	stVarBindArgProc* vb = (stVarBindArgProc *)NULL;
	int var_cnt = 6;

	vb = (stVarBindArgProc *)malloc(sizeof(stVarBindArgProc) * var_cnt);
	memset(vb, 0x00, sizeof(stVarBindArgProc) * var_cnt);

	strcpy(vb[0].variable, ".1.3.6.1.4.1.3204.1.3.35.7.1.2");
	vb[0].type = 's';
	strcpy(vb[0].value, myContext->fileMonitorName);

	strcpy(vb[1].variable, ".1.3.6.1.4.1.3204.1.3.35.7.1.4");
	vb[1].type = 's';
	strcpy(vb[1].value, myContext->fileMonitorDescription);

	strcpy(vb[2].variable, ".1.3.6.1.4.1.3204.1.3.35.7.1.6");
	vb[2].type = 's';
	strcpy(vb[2].value, myContext->fileMonitorModifiedDateTime);

	strcpy(vb[3].variable, ".1.3.6.1.4.1.3204.1.3.35.7.1.7");
	vb[3].type = 'i';
	sprintf(vb[3].value, "%ld", myContext->fileMonitorFileSize);

	strcpy(vb[4].variable, ".1.3.6.1.4.1.3204.1.3.35.7.1.8");
	vb[4].type = 's';
	strcpy(vb[4].value, myContext->fileMonitorOwner);

	strcpy(vb[5].variable, ".1.3.6.1.4.1.3204.1.3.35.7.1.11");
	vb[5].type = 's';
	strcpy(vb[5].value, myContext->fileMonitorAlertMessage);

	return vb;
}

u_char* date_n_time_buf(time_t * when, u_char* buf, size_t * length)
{
    struct tm      *tm_p;

    /*
     * Null time
     */
    if (when == NULL || *when == 0 || *when == (time_t) - 1) {
        buf[0] = 0;
        buf[1] = 0;
        buf[2] = 1;
        buf[3] = 1;
        buf[4] = 0;
        buf[5] = 0;
        buf[6] = 0;
        buf[7] = 0;
        *length = 8;
        return buf;
    }


    /*
     * Basic 'local' time handling
     */
    tm_p = localtime(when);
    *(short *) buf = htons(tm_p->tm_year + 1900);
    buf[2] = tm_p->tm_mon + 1;
    buf[3] = tm_p->tm_mday;
    buf[4] = tm_p->tm_hour;
    buf[5] = tm_p->tm_min;
    buf[6] = tm_p->tm_sec;
    buf[7] = 0;
    *length = 8;

#ifndef cygwin
    /*
     * Timezone offset
     */
#if !defined(SYSV) && !defined(aix4) && !defined(aix5) && !defined(WIN32) && !defined(irix6)
#define timezone tm_p->tm_gmtoff
#endif
    if (timezone > 0)
        buf[8] = '-';
    else
        buf[8] = '+';
    buf[9] = abs(timezone) / 3600;
    buf[10] = (abs(timezone) - buf[9] * 3600) / 60;
    *length = 11;
#endif

#ifdef SYSV
    /*
     * Daylight saving time
     */
    if (tm_p->tm_isdst > 0) {
        /*
         * Assume add one hour 
         */
        if (buf[8] == '-')
            --buf[9];
        else
            ++buf[9];

        if (buf[9] == 0)
            buf[8] = '+';
    }
#endif

    return buf;
}


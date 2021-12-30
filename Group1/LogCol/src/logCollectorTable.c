/*****************************************************************************
 *
 * logCollectorTable.c
 *
 *      Copyright (c) 2009 Nuri Telecom Co., LTD 
 *      All rights Reserved
 *
 * author:
 *      SungHae Kim     	 
 *
 * description :
 *      This file is a C header code for Log Collector
 *
 * TODO:
 *      None
 *
 * CM Details:
 *
 ****************************************************************************/

#define _REENTRANT

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/library/snmp.h>
#include <net-snmp/library/snmp_assert.h>

#ifdef WIN32
#include "../inc/logCollectorTable.h"
#include "vbregexp_header.h"
#else
#include "psMonitorTable.h"
#include "logCollectorTable.h"
#endif

static size_t strtimeformat(char*, size_t, const char*);

static     netsnmp_handler_registration *my_handler = NULL;
static     netsnmp_table_array_callbacks cb;

static oid  objid_sysuptime[] = { 1, 3, 6, 1, 2, 1, 1, 3, 0 };
static oid  objid_snmptrap[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };

oid logCollectorTable_oid[] = { logCollectorTable_TABLE_OID };
size_t logCollectorTable_oid_len = OID_LENGTH(logCollectorTable_oid);


#define FCLOSE(x) if(x != NULL) fclose(x);\
                  x=NULL

#ifdef logCollectorTable_IDX2
/************************************************************
 * keep binary tree to find context by name
 */
static int logCollectorTable_cmp( const void *lhs, const void *rhs );

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int logCollectorTable_cmp( const void *lhs, const void *rhs )
{
    logCollectorTable_context *context_l =
        (logCollectorTable_context *)lhs;
    logCollectorTable_context *context_r =
        (logCollectorTable_context *)rhs;

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
             "logCollectorTable_cmp not implemented! Container order undefined\n" );
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
logCollectorTable_context* logCollectorTable_get( const char *name, int len )
{
    logCollectorTable_context tmp;

    /** we should have a secondary index */
    netsnmp_assert(cb.container->next != NULL);

    /*
     * TODO: implement compare. Remove this ifdef code and
     * add your own code here.
     */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "logCollectorTable_get not implemented!\n" );
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
static int logCollectorTable_row_copy(logCollectorTable_context* dst,
				logCollectorTable_context* src)
{
    if(dst == NULL || src == NULL)
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
    dst->logCollectorIndex = src->logCollectorIndex;

    memcpy(dst->logCollectorFile, src->logCollectorFile, src->logCollectorFile_len );
    dst->logCollectorFile_len = src->logCollectorFile_len;

    memcpy(dst->logCollectorFileType, src->logCollectorFileType, src->logCollectorFileType_len );
    dst->logCollectorFileType_len = src->logCollectorFileType_len;

    dst->logCollectorReadInterval = src->logCollectorReadInterval;

    memcpy(dst->logCollectorFilter, src->logCollectorFilter, src->logCollectorFilter_len );
    dst->logCollectorFilter_len = src->logCollectorFilter_len;

    dst->logCollectorFilterCount = src->logCollectorFilterCount;

    memcpy( dst->logCollectorDescription, src->logCollectorDescription, src->logCollectorDescription_len );
    dst->logCollectorDescription_len = src->logCollectorDescription_len;

    memcpy( dst->logCollectorCreatedTime, src->logCollectorCreatedTime, src->logCollectorCreatedTime_len );
    dst->logCollectorCreatedTime_len = src->logCollectorCreatedTime_len;

    memcpy( dst->logCollectorUpdatedTime, src->logCollectorUpdatedTime, src->logCollectorUpdatedTime_len );
    dst->logCollectorUpdatedTime_len = src->logCollectorUpdatedTime_len;

    dst->logCollectorLine = src->logCollectorLine;
    dst->logCollectorGetLine = src->logCollectorGetLine;
    dst->logCollectorFileSize = src->logCollectorFileSize;
    dst->logCollectorReadLine = src->logCollectorReadLine;

    dst->logCollectorStatus = src->logCollectorStatus;

    return 0;
}

#ifdef logCollectorTable_SET_HANDLING

/*
 * the *_extract_index routine
 */
int logCollectorTable_extract_index( logCollectorTable_context * ctx, netsnmp_index * hdr )
{
    /*
     * temporary local storage for extracting oid index
     */
    netsnmp_variable_list var_logCollectorIndex;
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
       memset( &var_logCollectorIndex, 0x00, sizeof(var_logCollectorIndex) );
       var_logCollectorIndex.type = ASN_UNSIGNED;
       /** TODO: link this index to the next, or NULL for the last one */
#ifdef TABLE_CONTAINER_TODO
	snmp_log(LOG_ERR, "logCollectorTable_extract_index index list not implemented!\n" );
    return 0;
#else
       //var_logCollectorIndex.next_variable = &var_XX;

#endif

    /*
     * parse the oid into the individual components
     */
    err = parse_oid_indexes( hdr->oids, hdr->len, &var_logCollectorIndex );
    if (err == SNMP_ERR_NOERROR) {
       /*
        * copy components into the context structure
        */
                ctx->logCollectorIndex = *var_logCollectorIndex.val.integer;


           /*
            * TODO: check index for valid values. For EXAMPLE:
            *
              * if ( *var_logCollectorIndex.val.integer != XXX ) {
          *    err = -1;
          * }
          */
    }

    /*
     * parsing may have allocated memory. free it.
     */
    snmp_reset_var_buffers( &var_logCollectorIndex );

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
int logCollectorTable_can_activate(logCollectorTable_context *undo_ctx,
                      logCollectorTable_context *row_ctx,
                      netsnmp_request_group * rg)
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
int logCollectorTable_can_deactivate(logCollectorTable_context *undo_ctx,
                        logCollectorTable_context *row_ctx,
                        netsnmp_request_group * rg)
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
int logCollectorTable_can_delete(logCollectorTable_context *undo_ctx,
                    logCollectorTable_context *row_ctx,
                    netsnmp_request_group * rg)
{
    /*
     * probably shouldn't delete a row that we can't
     * deactivate.
     */
    if(logCollectorTable_can_deactivate(undo_ctx,row_ctx,rg) != 1)
        return 0;

    /*
     * TODO: check for other deletion requirements here
     */
    return 1;
}

#ifdef logCollectorTable_ROW_CREATION
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
 * returns a newly allocated logCollectorTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
logCollectorTable_context* logCollectorTable_create_row( netsnmp_index* hdr)
{
    logCollectorTable_context* ctx =
        SNMP_MALLOC_TYPEDEF(logCollectorTable_context);

    if(!ctx)
        return NULL;

    /*
     * TODO: check indexes, if necessary.
     */
    if(logCollectorTable_extract_index( ctx, hdr )) {
        free(ctx->index.oids);
        free(ctx);
        return NULL;
    }

    /* netsnmp_mutex_init(ctx->lock);
       netsnmp_mutex_lock(ctx->lock); */

    /*
     * TODO: initialize any default values here. This is also
     * first place you really should allocate any memory for
     * yourself to use.  If you allocated memory earlier,
     * make sure you free it for earlier error cases!
     */
     ctx->logCollectorReadInterval = 30;
     ctx->logCollectorFilterCount = 1;

    return ctx;
}
#endif

/************************************************************
 * the *_duplicate row routine
 */
logCollectorTable_context *
logCollectorTable_duplicate_row( logCollectorTable_context * row_ctx)
{
    logCollectorTable_context * dup;

    if(!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(logCollectorTable_context);
    if(!dup)
        return NULL;

    if(logCollectorTable_row_copy(dup,row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index * logCollectorTable_delete_row( logCollectorTable_context * ctx )
{
  /* netsnmp_mutex_destroy(ctx->lock); */

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
void logCollectorTable_set_reserve1( netsnmp_request_group *rg )
{
    logCollectorTable_context *row_ctx =
            (logCollectorTable_context *)rg->existing_row;
    logCollectorTable_context *undo_ctx =
            (logCollectorTable_context *)rg->undo_info;
    netsnmp_variable_list *var;
    netsnmp_request_group_item *current;
    int rc;


    /*
     * TODO: loop through columns, check syntax and lengths. For
     * columns which have no dependencies, you could also move
     * the value/range checking here to attempt to catch error
     * cases as early as possible.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;
        rc = SNMP_ERR_NOERROR;

        switch(current->tri->colnum) {

        case COLUMN_LOGCOLLECTORFILE:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                               var->val_len);
        break;

        case COLUMN_LOGCOLLECTORFILETYPE:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                               var->val_len);
        break;

        case COLUMN_LOGCOLLECTORREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                 	sizeof(row_ctx->logCollectorReadInterval));
        break;

        case COLUMN_LOGCOLLECTORFILTER:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                               var->val_len);
        break;

        case COLUMN_LOGCOLLECTORFILTERCOUNT:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->logCollectorFilterCount));
        break;

        case COLUMN_LOGCOLLECTORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                               var->val_len);
        break;

        case COLUMN_LOGCOLLECTORCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                               var->val_len);
        break;

        case COLUMN_LOGCOLLECTORUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                               var->val_len);
        break;

        case COLUMN_LOGCOLLECTORLINE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                     sizeof(row_ctx->logCollectorLine));
        break;

        case COLUMN_LOGCOLLECTORGETLINE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                     sizeof(row_ctx->logCollectorGetLine));
        break;

        case COLUMN_LOGCOLLECTORFILESIZE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->logCollectorFileSize));
        break;

        case COLUMN_LOGCOLLECTORREADLINE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->logCollectorReadLine));
        break;

        case COLUMN_LOGCOLLECTORSTATUS:
            /** RowStatus = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
        			sizeof(row_ctx->logCollectorStatus));
        break;

        default: /** We shouldn't get here */
            rc = SNMP_ERR_GENERR;
            snmp_log(LOG_ERR, "unknown column in "
                     "logCollectorTable_set_reserve1\n");
        }

        if (rc)
           netsnmp_set_mode_request_error(MODE_SET_BEGIN, current->ri, rc );
        rg->status = SNMP_MAX( rg->status, current->ri->status );
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

void logCollectorTable_set_reserve2( netsnmp_request_group *rg )
{
    logCollectorTable_context *row_ctx = 
		(logCollectorTable_context *)rg->existing_row;
    logCollectorTable_context *undo_ctx = 
		(logCollectorTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;
    netsnmp_variable_list *var;
    int rc;

    rg->rg_void = rg->list->ri;

    /*
     * TODO: loop through columns, check for valid
     * values and any range constraints.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;
        rc = SNMP_ERR_NOERROR;

        switch(current->tri->colnum) {

        case COLUMN_LOGCOLLECTORFILE:
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

        case COLUMN_LOGCOLLECTORFILETYPE:
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

        case COLUMN_LOGCOLLECTORREADINTERVAL:
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

        case COLUMN_LOGCOLLECTORFILTER:
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

        case COLUMN_LOGCOLLECTORFILTERCOUNT:
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

        case COLUMN_LOGCOLLECTORDESCRIPTION:
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

        case COLUMN_LOGCOLLECTORCREATEDTIME:
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

        case COLUMN_LOGCOLLECTORUPDATEDTIME:
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

        case COLUMN_LOGCOLLECTORLINE:
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

        case COLUMN_LOGCOLLECTORGETLINE:
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

        case COLUMN_LOGCOLLECTORFILESIZE:
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

        case COLUMN_LOGCOLLECTORREADLINE:
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

        case COLUMN_LOGCOLLECTORSTATUS:
            /** RowStatus = ASN_INTEGER */
                rc = netsnmp_check_vb_rowstatus(current->ri->requestvb,
                        undo_ctx ?
                        undo_ctx->logCollectorStatus:0);
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
void logCollectorTable_set_action( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    logCollectorTable_context *row_ctx = 
		(logCollectorTable_context *)rg->existing_row;
    logCollectorTable_context *undo_ctx = 
		(logCollectorTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    int row_err = 0;

    /*
     * TODO: loop through columns, copy varbind values
     * to context structure for the row.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_LOGCOLLECTORFILE:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logCollectorFile,var->val.string,var->val_len);
            row_ctx->logCollectorFile_len = var->val_len;
            row_ctx->logCollectorFile[var->val_len]=0;
        break;

        case COLUMN_LOGCOLLECTORFILETYPE:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logCollectorFileType,var->val.string,var->val_len);
            row_ctx->logCollectorFileType_len = var->val_len;
            row_ctx->logCollectorFileType[var->val_len]=0;
        break;

        case COLUMN_LOGCOLLECTORREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logCollectorReadInterval = *var->val.integer;
        break;

        case COLUMN_LOGCOLLECTORFILTER:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logCollectorFilter,var->val.string,var->val_len);
            row_ctx->logCollectorFilter_len = var->val_len;
            row_ctx->logCollectorFilter[var->val_len]=0;
        break;

        case COLUMN_LOGCOLLECTORFILTERCOUNT:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logCollectorFilterCount = *var->val.integer;
        break;

        case COLUMN_LOGCOLLECTORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logCollectorDescription,var->val.string,var->val_len);
            row_ctx->logCollectorDescription_len = var->val_len;
            row_ctx->logCollectorDescription[var->val_len]=0;
        break;

        case COLUMN_LOGCOLLECTORCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->logCollectorCreatedTime,var->val.string,var->val_len);
            row_ctx->logCollectorCreatedTime_len = var->val_len;
            row_ctx->logCollectorCreatedTime[var->val_len]=0;
        break;

        case COLUMN_LOGCOLLECTORUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->logCollectorUpdatedTime,var->val.string,var->val_len);
            row_ctx->logCollectorUpdatedTime_len = var->val_len;
            row_ctx->logCollectorUpdatedTime[var->val_len]=0;
        break;

        case COLUMN_LOGCOLLECTORLINE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logCollectorLine = *var->val.integer;
        break;

        case COLUMN_LOGCOLLECTORGETLINE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logCollectorGetLine = *var->val.integer;
        break;

        case COLUMN_LOGCOLLECTORFILESIZE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logCollectorFileSize = *var->val.integer;
        break;

        case COLUMN_LOGCOLLECTORREADLINE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logCollectorReadLine = *var->val.integer;
        break;

        case COLUMN_LOGCOLLECTORSTATUS:
            /** RowStatus = ASN_INTEGER */
            row_ctx->logCollectorStatus = *var->val.integer;
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
#ifndef logCollectorTable_CAN_MODIFY_ACTIVE_ROW
    if( undo_ctx && RS_IS_ACTIVE(undo_ctx->logCollectorStatus) &&
        row_ctx && RS_IS_ACTIVE(row_ctx->logCollectorStatus) ) {
            row_err = 1;
    }
#endif

    /*
     * check activation/deactivation
     */
    row_err = netsnmp_table_array_check_row_status(&cb, rg,
                  row_ctx ? &row_ctx->logCollectorStatus : NULL,
                  undo_ctx ? &undo_ctx->logCollectorStatus : NULL);
    if(row_err) {
        netsnmp_set_mode_request_error(MODE_SET_BEGIN,
            (netsnmp_request_info*)rg->rg_void, row_err);
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
void logCollectorTable_set_commit( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    logCollectorTable_context *row_ctx = 
		(logCollectorTable_context *)rg->existing_row;
    logCollectorTable_context *undo_ctx = 
		(logCollectorTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    pthread_t p;
    time_t t;

    DEBUGMSGTL(("LogCol/logCollectorTable_set_commit", "called\n"));

    DEBUGMSGTL(("LogCol/logCollectorTable_set_commit",
        "File Interval Filter <%s %d %s>\n",
        row_ctx->logCollectorFile,
        row_ctx->logCollectorReadInterval,
        row_ctx->logCollectorFilter));

    if ( LogCol_UpdateDataFile(row_ctx) == 0 ) {
        DEBUGMSGTL(("LogCol/logCollectorTable_set_commit",
            "Row updated in the LOG_COL_DATA_FILE \n"));
    }

    // Do the value check, before proceeding
    if (( strlen(row_ctx->logCollectorFile) == 0 ) ||
		( strlen(row_ctx->logCollectorFileType) == 0) ||
        ( row_ctx->logCollectorReadInterval <= 0 ) || 
        ( row_ctx->logCollectorStatus != SNMP_ROW_ACTIVE )) {
		DEBUGMSGTL(("LogCol/logCollectorTable_set_commit",
			"if failed (%s, %s, %d, %d, %d)\n",
			row_ctx->logCollectorFile,
			row_ctx->logCollectorFileType,
			row_ctx->logCollectorReadInterval,
			row_ctx->logCollectorStatus, 
			SNMP_ROW_ACTIVE));
		return;
	}

	time(&t);

	row_ctx->logCollectorCreatedTime_len = MAX_DATE_TIME_LEN;

	strcpy(row_ctx->logCollectorCreatedTime,
	(u_char *)date_n_time(&t,(size_t*)&(row_ctx->logCollectorCreatedTime_len)));

	if ( row_ctx->threadActiveFlag == 0 ) {
		// Create a thread for this newly created row
		if (pthread_create(&p, NULL, LogCollectorThread, (void*)row_ctx) != 0) {
			snmp_log_perror("logCollectorTable: creating thread failed\n");
		} else {
			row_ctx->threadActiveFlag = 1;

			DEBUGMSGTL(("LogCol/logCollectorTable_set_commit",
				"Thread started for < %u %d %s >\n",
					row_ctx->idx,
					row_ctx->index.len,
					row_ctx->logCollectorFile));
		}
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
void logCollectorTable_set_free( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    logCollectorTable_context *row_ctx = 
		(logCollectorTable_context *)rg->existing_row;
    logCollectorTable_context *undo_ctx = 
		(logCollectorTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_LOGCOLLECTORFILE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGCOLLECTORFILETYPE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGCOLLECTORREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGCOLLECTORFILTER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGCOLLECTORFILTERCOUNT:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGCOLLECTORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGCOLLECTORCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGCOLLECTORUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGCOLLECTORLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGCOLLECTORGETLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGCOLLECTORFILESIZE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGCOLLECTORREADLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGCOLLECTORSTATUS:
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
void logCollectorTable_set_undo( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    logCollectorTable_context *row_ctx = 
		(logCollectorTable_context *)rg->existing_row;
    logCollectorTable_context *undo_ctx = 
		(logCollectorTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_LOGCOLLECTORFILE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGCOLLECTORFILETYPE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGCOLLECTORREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGCOLLECTORFILTER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGCOLLECTORFILTERCOUNT:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGCOLLECTORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGCOLLECTORCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGCOLLECTORUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGCOLLECTORLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGCOLLECTORGETLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGCOLLECTORFILESIZE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGCOLLECTORREADLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGCOLLECTORSTATUS:
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

#endif /** logCollectorTable_SET_HANDLING */


/************************************************************
 *
 * Initialize the logCollectorTable table by defining its contents and how it's structured
 */
void initialize_table_logCollectorTable(void)
{
    netsnmp_table_registration_info *table_info;

    if (my_handler) {
        snmp_log(LOG_ERR, "initialize_table_logCollectorTable called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /* if your table is read only, it's easiest to change the
       HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY */
    my_handler = netsnmp_create_handler_registration("logCollectorTable",
                                             netsnmp_table_array_helper_handler,
                                             logCollectorTable_oid,
                                             logCollectorTable_oid_len,
                                             HANDLER_CAN_RWRITE);

    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_logCollectorTable \n");
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
    /** index: logCollectorIndex */
    netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

    table_info->min_column = logCollectorTable_COL_MIN;
    table_info->max_column = logCollectorTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = logCollectorTable_get_value;
    cb.container = netsnmp_container_find("logCollectorTable_primary:"
                                          "logCollectorTable:"
                                          "table_container");
#ifdef logCollectorTable_IDX2
    netsnmp_container_add_index(cb.container,
        netsnmp_container_find("logCollectorTable_secondary:"
                               "logCollectorTable:"
                               "table_container"));
    cb.container->next->compare = logCollectorTable_cmp;
#endif
#ifdef logCollectorTable_SET_HANDLING
    cb.can_set = 1;
#ifdef logCollectorTable_ROW_CREATION
    cb.create_row = (UserRowMethod*)logCollectorTable_create_row;
#endif
    cb.duplicate_row = (UserRowMethod*)logCollectorTable_duplicate_row;
    cb.delete_row = (UserRowMethod*)logCollectorTable_delete_row;
    cb.row_copy = (Netsnmp_User_Row_Operation *)logCollectorTable_row_copy;

    cb.can_activate = (Netsnmp_User_Row_Action *)logCollectorTable_can_activate;
    cb.can_deactivate = (Netsnmp_User_Row_Action *)logCollectorTable_can_deactivate;
    cb.can_delete = (Netsnmp_User_Row_Action *)logCollectorTable_can_delete;

    cb.set_reserve1 = logCollectorTable_set_reserve1;
    cb.set_reserve2 = logCollectorTable_set_reserve2;
    cb.set_action = logCollectorTable_set_action;
    cb.set_commit = logCollectorTable_set_commit;
    cb.set_free = logCollectorTable_set_free;
    cb.set_undo = logCollectorTable_set_undo;
#endif
    DEBUGMSGTL(("LogCol/initialize_table_logCollectorTable",
                "Registering table logCollectorTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);
}

/************************************************************
 * logCollectorTable_get_value
 */
int logCollectorTable_get_value(
            netsnmp_request_info *request,
            netsnmp_index *item,
            netsnmp_table_request_info *table_info )
{
    netsnmp_variable_list *var = request->requestvb;
    logCollectorTable_context *context = (logCollectorTable_context *)item;

    switch(table_info->colnum) {

        case COLUMN_LOGCOLLECTORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->logCollectorIndex,
                         sizeof(context->logCollectorIndex) );
        break;

        case COLUMN_LOGCOLLECTORFILE:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logCollectorFile,
                         context->logCollectorFile_len );
        break;

        case COLUMN_LOGCOLLECTORFILETYPE:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logCollectorFileType,
                         context->logCollectorFileType_len );
        break;

        case COLUMN_LOGCOLLECTORREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logCollectorReadInterval,
                         sizeof(context->logCollectorReadInterval) );
        break;

        case COLUMN_LOGCOLLECTORFILTER:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logCollectorFilter,
                         context->logCollectorFilter_len );
        break;

        case COLUMN_LOGCOLLECTORFILTERCOUNT:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logCollectorFilterCount,
                         sizeof(context->logCollectorFilterCount) );
        break;

        case COLUMN_LOGCOLLECTORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logCollectorDescription,
                         context->logCollectorDescription_len );
        break;

        case COLUMN_LOGCOLLECTORCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logCollectorCreatedTime,
                         context->logCollectorCreatedTime_len );
        break;

        case COLUMN_LOGCOLLECTORUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logCollectorUpdatedTime,
                         context->logCollectorUpdatedTime_len );
        break;

        case COLUMN_LOGCOLLECTORLINE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logCollectorLine,
                         sizeof(context->logCollectorLine) );
        break;

        case COLUMN_LOGCOLLECTORGETLINE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logCollectorGetLine,
                         sizeof(context->logCollectorGetLine) );
        break;

        case COLUMN_LOGCOLLECTORFILESIZE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logCollectorFileSize,
                         sizeof(context->logCollectorFileSize) );
        break;

        case COLUMN_LOGCOLLECTORREADLINE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logCollectorReadLine,
                         sizeof(context->logCollectorReadLine) );
        break;

        case COLUMN_LOGCOLLECTORSTATUS:
            /** RowStatus = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logCollectorStatus,
                         sizeof(context->logCollectorStatus) );
        break;

        default: /** We shouldn't get here */
           snmp_log(LOG_ERR, "unknown column in "
                 "logCollectorTable_get_value\n");
        return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}

/************************************************************
 * logCollectorTable_get_by_idx
 */
const logCollectorTable_context *
logCollectorTable_get_by_idx(netsnmp_index * hdr)
{
    return (const logCollectorTable_context *)
        CONTAINER_FIND(cb.container, hdr );
}


/*****************************************************************************
 *
 * name             :   LogCol_MasterThread
 * description      :   This is the master thread , which will be created just
 *                      after the initialization of the table. It will create
 *                      the default data, which will be read from the data
 *                      files.
 * input parameters :   None
 * output parameters:   None
 * return type      :   void * - returns Nothing
 * global variables :   None
 * calls            :   None
 */

void *
LogCol_MasterThread()
{
    pthread_t p;
    logCollectorTable_context* row = NULL;

    row = CONTAINER_FIRST(cb.container);

#ifdef WIN32
    InitializeRegExp();
#endif

    while (row) {
        // Do the value check, before proceeding
        if ((strlen(row->logCollectorFile) == 0 ) || 
			(strlen(row->logCollectorFileType) == 0) || 
            (row->logCollectorReadInterval <= 0 )) {

            snmp_log(LOG_ERR,"LogCol/LogCol_MasterThread \
                #%d# Thread could not be started for OID < %u >\n",
                MY_THR_ID, row->idx);

            // Change it to not-active
            row->threadActiveFlag = 0;

			row = CONTAINER_NEXT(cb.container, row);

			continue;
		}

		// Now, spawn thread for it
		if (pthread_create(&p, NULL, LogCollectorThread, (void*)row) == 0) {
			// Change it to active
			row->threadActiveFlag = 1;

			DEBUGMSGTL(("LogCol/LogCol_MasterThread",
				"#%d# Master Thread started for < %u %d %s >\n",
					MY_THR_ID,
					row->idx,
					row->index.len,
					row->logCollectorFile));
		} else {
			snmp_log_perror("Thread creation failed");
		}

        row = CONTAINER_NEXT(cb.container, row);
    }
}


/*****************************************************************************
 *
 * name             :   LogCollectorThread
 * description      :   This function is the main function of the thread, which
 *                      will keep on running
 * input parameters :   void *logData-the argument to the thread, which will be
 *                      logCollectorTable_context data
 * output parameters:   None
 * return type      :   void * - Nothing
 * global variables :   
 * calls            :   
 */

void* LogCollectorThread(void *data)
{
    logCollectorTable_context* row;
	stVarBindArgProc* varBinds = NULL;

    DEBUGMSGTL(("LogCol/LogCollectorThread",
          "#%d# Thread spawned for OID <%u>\n",
          MY_THR_ID, ((logCollectorTable_context*)data)->idx));

    // we need the index value, so do this
    row = (logCollectorTable_context*)data;
    	
	// Copy the file name for later reference
	strtimeformat(row->changedFileName,
			sizeof(row->changedFileName), row->logCollectorFile);
	strcpy(row->oldFileName, row->changedFileName);

	row->logCollectorReadLine = 0;
	row->filterCount = 0;
	row->isStart = 1;

    if (row->logCollectorFilterCount < 1)
        row->logCollectorFilterCount = 1;
	
    while (1) {
        // Get a context to my row in the MIB container
        // Get the latest values of the data
        row = CONTAINER_FIND(cb.container, &(row->index));

        // If the row does not exist anymore
        if (row == NULL) {
            DEBUGMSGTL(("LogCol/LogCollectorThread",
                "#%d# Exiting thread; row == NULL\n", MY_THR_ID));
            pthread_detach(MY_THR_ID);
            pthread_exit(NULL);  // exit the thread
        }

        // Check, If the row status is not active, exit the thread
        if (row->logCollectorStatus != SNMP_ROW_ACTIVE) {
            DEBUGMSGTL(("LogCol/LogCollectorThread",
                "#%d# Exiting thread; not active\n", MY_THR_ID));

            // Unset the thread active status
            row->threadActiveFlag = 0;
            pthread_detach(MY_THR_ID);
            pthread_exit(NULL);  // exit the thread
        }

        // Do the value check, before proceeding
        if ((strlen(row->logCollectorFile) == 0 ) ||
			(strlen(row->logCollectorFileType) == 0) ||
            (row->logCollectorReadInterval <= 0 )) {
            SLEEP(TIME_TO_SLEEP);
            continue; // No need to process the request,
                     // until valid values are provided
        }

        // Check if the filename has been changed or not
        
		strtimeformat(row->changedFileName,
			sizeof(row->changedFileName), row->logCollectorFile);

        if (strcmp(row->changedFileName, row->oldFileName) != 0) {
        	DEBUGMSGTL(("LogCol/LogCollectorThread",
				"File Name Changed <%s -> %s>\n", 
					row->oldFileName, row->changedFileName)); 

	        row->logCollectorFilterCount = 0;
            row->logCollectorLine = 0;
            row->logCollectorGetLine = 0;
            row->logCollectorFileSize = 0;
            row->logCollectorReadLine = 0;

            strcpy(row->logCollectorUpdatedTime, "");
            strcpy(row->oldFileName, row->changedFileName);
        }
        
        DEBUGMSGTL(("LogCol/LogCollectorThread",
            "#%d# Checking OID <%u> OID_LEN <%d> LOGFILE <%s>\n",
            MY_THR_ID,
            *(row->index.oids),
            row->index.len,
            row->logCollectorFile));

        // Check for the patterns
		
		if (strcmp(row->logCollectorFileType, "TEXT") == 0) {
        	LogCol_SearchPatterns(row);
		} else if (strcmp(row->logCollectorFileType, "WTMP") == 0) {
			LogCol_SearchWtmp(row);
		} else if (strcmp(row->logCollectorFileType, "SYSLOG") == 0) {
			LogCol_SearchSyslog(row);
		} else {
        	LogCol_SearchPatterns(row);
		}

        // Update the data file with the latest row data
        if (LogCol_UpdateDataFile(row) == -1)
            snmp_log_perror("Error while updating LogCol MIB file");

       DEBUGMSGTL(("LogCol/LogCollectorThread",
            "#%d# going to SLEEP for logfile <%s>\n",
            MY_THR_ID, row->logCollectorFile));

       // Sleep until the next interval
       SLEEP(row->logCollectorReadInterval);

       DEBUGMSGTL(("LogCol/LogCollectorThread",
            "#%d# woke up for logfile <%s>\n",
            MY_THR_ID, row->logCollectorFile));

    } // End of infinite while loop
}


int LogCol_GetLineCountAndSeek(char* fileName, long* readLineSeek)
{
    FILE *fp = NULL;
    int  lineCount = 0;
    static char buf[LOG_MAX_LEN+1];	

    if ((fp = fopen(fileName, "r")) == NULL)
        return (-1);

    DEBUGMSGTL(("LogCol/LogCol_GetLineCountAndSeek",
        "#%d# File <%s> opened\n", MY_THR_ID, fileName));

    // Read until fgets returns error
    while (fgets(buf, LOG_MAX_LEN, fp)) {
        lineCount++;
	}

	*readLineSeek = ftell(fp);

    FCLOSE(fp);

    DEBUGMSGTL(("LogCol/LogCol_GetLineCountAndSeek",
        "#%d# File <%s> closed\n", MY_THR_ID, fileName));

    return (lineCount);
}

#ifndef WIN32

int LogCol_GetLineCountAndSeekWtmp(char* fileName)
{
    int	lineCount = 0;
#ifdef aix4
	struct utmp* utent;
	struct utmp tmp;
#elif hpux11
	struct utmps* utent;
	struct utmps tmp;
#else
	struct utmpx* utent;
	struct utmpx tmp;
#endif

#ifdef aix4
	if (utmpname(fileName) == 0) {
#elif hpux11
	bwtmpname(fileName);

	if ( 0 ) {
#else
	if (utmpxname(fileName) == 0) {
#endif
		DEBUGMSGTL(("LogCol/LogCol_GetLineCountAndSeekWtmp",
			"Failed to open file [%s, %d]\n",
			fileName, errno));

		return (0);
	}

	while (1) {
#ifdef aix4
		utent = getutent();
#elif hpux11
		utent = getbwent(sizeof(struct utmps));
#else
		utent = getutxent();
#endif

		if (utent == NULL) {
			DEBUGMSGTL(("LogCol/LogCol_GetLineCountAndSeekWtmp",
				"#%d# utent == NULL\n", MY_THR_ID));
			break;
		}
		
		lineCount++;
	}

#ifdef aix4
	endutent();
#elif hpux11
	endbwent();
#else
	endutxent();
#endif
    return (lineCount);
}

#else

int LogCol_GetLineCountAndSeekWtmp(char* fileName)
{
    return 0;
}

#endif

/*****************************************************************************
 *
 * name             :   LogCol_LoadContainerFromFile
 * description      :   This function is used to load the container with the
 *                      default values which were present when the last time
 *                      agent was up and running.
 * input parameters :   None
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   1) getIPFromASNFormat
 */

#ifndef WIN32

void LogCol_LoadContainerFromFile()
{
    logCollectorTable_context ctx = {0};
    logCollectorTable_context* row = NULL; // Container row context
    int fd = 0;				// Data file descriptor
    int fdTmp = 0;			// Temp data file descriptor
    int rewriteFlag = 0;	// Whether to rewrite the data file

    if ((fd = open(LOG_COL_DATA_FILE, O_RDONLY, S_IREAD|S_IWRITE)) == -1) {
         snmp_log_perror("LogCol_LoadContainerFromFile: LOG_COL_DATA_FILE open failed\n");
		 return;
    }

	DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
		"#%d# File <%s> opened\n", MY_THR_ID, LOG_COL_DATA_FILE));

	while (read(fd, &ctx, LOG_COL_CONTEXT_SIZE) > 0) {
		// If the row, is already destroyed, no need to create it again.
		if (ctx.logCollectorStatus == SNMP_ROW_DESTROY ) {
			DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
				"#%d# Found row status as destroyed <%d>\n",
				MY_THR_ID, ctx.logCollectorStatus));

			rewriteFlag = 1; // to re-write the mib data file
			continue; // as we have to load the container
		}

		DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
			"Loading MIB Row for Log File <%s %s>\n", 
			ctx.logCollectorFile,
			ctx.logCollectorFileType));

		row = (logCollectorTable_context*)malloc(LOG_COL_CONTEXT_SIZE);

		if (row == NULL) {
			snmp_log_perror("Malloc failed");
			return;
		}

		memcpy(row, &ctx, LOG_COL_CONTEXT_SIZE);

		netsnmp_assert(row->index.oids == NULL);

		if (snmp_clone_mem((void*)&row->index.oids, &(ctx.idx),
                            ctx.index.len * sizeof(oid) )) {
			DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
				"#%d# File <%s> closed\n",
				MY_THR_ID, LOG_COL_DATA_FILE));

			free(row);
			close(fd);
			return;
		}

		row->index.len = ctx.index.len;

		if (CONTAINER_INSERT(cb.container, row)) {
			DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
				"ERROR IN CONTAINER_INSERT FUNCION\n"));
		}

		DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
			"OID <%u>\n",*(row->index.oids)));
	} /* while */

	DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
		"CONTAINER SIZE=%u\n", CONTAINER_SIZE (cb.container)));

	if (rewriteFlag == 0) {
		close(fd);
		return;
	}

	/* rewrite LOG_COL_DATA_FILE except destroyed row */

	fdTmp = open(LOG_COL_DATA_FILE_TMP, O_CREAT|O_RDWR, S_IREAD|S_IWRITE);

	if (fdTmp == -1 ) {
		snmp_log_perror("LOG_COL_DATA_FILE_TMP file open failed\n");

		DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
			"#%d# File <%s> closed; open failed\n",
			MY_THR_ID, LOG_COL_DATA_FILE_TMP));

		close(fd);
		return;
	}

	DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
		"#%d# File <%s> opened\n",
		MY_THR_ID, LOG_COL_DATA_FILE_TMP));

	// Reset the file pointer to begining of the file
	lseek(fd, 0, SEEK_SET);
	lseek(fdTmp, 0, SEEK_SET);

	while (read(fd, &ctx, LOG_COL_CONTEXT_SIZE) > 0) {
		if (ctx.logCollectorStatus == SNMP_ROW_ACTIVE)
			if (write(fdTmp, &ctx, LOG_COL_CONTEXT_SIZE) < 0)
				snmp_log_perror("Error writing data to LOG_COL_DATA_FILE");
	} 

	close(fdTmp);
	close(fd);

	DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
		"#%d# File <%s> closed\n",
		MY_THR_ID, LOG_COL_DATA_FILE_TMP));

	// Delete the old data file and rename
	unlink(LOG_COL_DATA_FILE);
	rename(LOG_COL_DATA_FILE_TMP, LOG_COL_DATA_FILE);

	DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
		"#%d# File <%s> closed\n",
		MY_THR_ID, LOG_COL_DATA_FILE));
}

#else

void LogCol_LoadContainerFromFile()
{
    logCollectorTable_context ctx = {0};
    logCollectorTable_context* row = NULL;
    HANDLE fd = NULL; // Data file descriptor
    HANDLE fdTmp = NULL; // Temp data file descriptor
    DWORD  num = 0;
    int    rewriteFlag = 0; // Whether to rewrite the data file

    fd = CreateFile(LOG_COL_DATA_FILE,
                    GENERIC_READ|GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);

    if (fd == INVALID_HANDLE_VALUE) {
         snmp_log_perror("LogCol_LoadContainerFromFile: LOG_COL_DATA_FILE open failed\n");
		 return;
    }

	DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
		"#%d# File <%s> Opened\n",
		MY_THR_ID, LOG_COL_DATA_FILE));

	while (ReadFile(fd, &ctx, LOG_COL_CONTEXT_SIZE, &num, NULL)) {
		if (num == 0) {
			snmp_log(LOG_ERR," %d breaking out \n", GetLastError());
			break;
		}

		DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile", 
			"Status <%d>\n", 
			ctx.logCollectorStatus));

		// If the row, is already destroyed, no need to create it again.
		if (ctx.logCollectorStatus == SNMP_ROW_DESTROY) {
			rewriteFlag = 1;	// to re-write the mib data file
			continue;			// As we have to load the container anyways
		}

		DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
			"Loading MIB Row for Log File <%s>\n", 
			ctx.logCollectorFile));

		row = (logCollectorTable_context*)malloc(LOG_COL_CONTEXT_SIZE);

		if (row == NULL) {
			snmp_log_perror("LogCol_LoadContainerFromFile: Malloc failed");
			return;
		}

		memcpy(row, &ctx, LOG_COL_CONTEXT_SIZE);

		netsnmp_assert(row->index.oids == NULL);

		if (snmp_clone_mem((void*)&row->index.oids, &(ctx.idx),
                            ctx.index.len * sizeof(oid) )) {
			DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
				"#%d# File <%s> closed\n",
				MY_THR_ID, LOG_COL_DATA_FILE));

			CloseHandle(fd);
			free(row);
			return;
		}

		row->index.len = ctx.index.len;

		if (CONTAINER_INSERT(cb.container, row)) {
			DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
				"ERROR IN CONTAINER_INSERT FUNCION\n"));
		}

		DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
			"OID <%u>\n", *(row->index.oids)));
	}	/* while */

	DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
		"CONTAINER SIZE=%u\n", CONTAINER_SIZE(cb.container)));

	if (rewriteFlag == 0) {
    	CloseHandle(fd);
		return;
	}

	/* rewrite LOG_COL_DATA_FILE except destroyed row */

	fdTmp = CreateFile(LOG_COL_DATA_FILE_TMP,
					GENERIC_READ|GENERIC_WRITE,
					0, 
					NULL,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL);

	if (fdTmp == INVALID_HANDLE_VALUE) {
		snmp_log_perror("LOG_COL_DATA_FILE_TMP open failed\n");

		DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
			"#%d# File <%s> closed; tmp open failed\n",
			MY_THR_ID, LOG_COL_DATA_FILE));

		CloseHandle(fd);
		return;
	}

	DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
		"#%d# File <%s> opened\n",
		MY_THR_ID, LOG_COL_DATA_FILE_TMP));

	// Reset the file pointers to begining of the files
	SetFilePointer(fd, 0, NULL, FILE_BEGIN);
	SetFilePointer(fdTmp, 0, NULL, FILE_BEGIN);

	while (ReadFile(fd, &ctx, LOG_COL_CONTEXT_SIZE, &num, NULL)) {
		if (num == 0) {
			snmp_log(LOG_ERR," %d breaking out \n", GetLastError());
			break;
		}

		if (ctx.logCollectorStatus != SNMP_ROW_ACTIVE)
			continue;
           
		if (WriteFile(fdTmp, &ctx, LOG_COL_CONTEXT_SIZE, &num, NULL) == 0)
			snmp_log_perror("Error writing data to LOG_COL_DATA_FILE");
	}	/* while */ 

	CloseHandle(fdTmp);
    CloseHandle(fd);

	DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
		"#%d# File <%s> closed\n",
		MY_THR_ID, LOG_COL_DATA_FILE_TMP));

	// Delete the old data file and rename
	unlink(LOG_COL_DATA_FILE);
	rename(LOG_COL_DATA_FILE_TMP, LOG_COL_DATA_FILE);

    DEBUGMSGTL(("LogCol/LogCol_LoadContainerFromFile",
        "#%d# File <%s> closed\n",
        MY_THR_ID, LOG_COL_DATA_FILE));
}

#endif


/*****************************************************************************
 *
 * name             :   LogCol_SearchPatterns
 * description      :   This function is used to search the patterns in the
 *                      input file
 * input parameters :   logCollectorTable_context *row
 * output parameters:   None
 * return type      :   int - 1 for pattern found, 0 for not found, -1 for error
 * global variables :   None
 * calls            :   LogCol_GetCurrDateTime
 */

int LogCol_SearchPatterns(logCollectorTable_context *row)
{
    int     strFound = 0;
    int     lineNum = 0; // Current line number
    time_t  now = 0;
    FILE*	fp = NULL; // Input file descriptor
    struct stat st = {0};
    static char buf[LOG_MAX_LEN+1]; // the line read
	stVarBindArgProc* varBinds = NULL;

    // Find the input log file size and check for error

    if (stat(row->changedFileName, &st) == -1) {
        DEBUGMSGTL(("LogCol/LogCol_SearchPatterns",
            "#%d# File <%s %s> closed; stat failed\n",
            MY_THR_ID, 
			row->logCollectorFile, 
			row->changedFileName));

        return (-1);
    }

	/* CR2 : Whenever file name is changed, monitor from beginning */
    /* CR1 : If file size is less than last time, monitor from begining */
    /* CR3 : When monitoring start, move to end-of-file for skip previous log */
	
    if (row->logCollectorFileSize > st.st_size 
	|| row->logCollectorReadLine == 0) {
        DEBUGMSGTL(("LogCol/LogCol_SearchPatterns",
			"FileSize=[%d], st.size=[%d], ReadLine=[%d], isStart=[%d]\n", 
			row->logCollectorFileSize, 
			st.st_size,
			row->logCollectorReadLine, 
			row->isStart));

		if (row->isStart) {
			SLEEP(rand() % 60);

        	row->logCollectorReadLine = LogCol_GetLineCountAndSeek(
					row->changedFileName, 
					&row->logCollectorReadLineSeek);

			row->isStart = 0;

			snmp_log(LOG_INFO,
				"LOGCOL TEXT:: THR<%ld> FILE[%s] LINE[%ld] SIZE[%ld]\n",
				MY_THR_ID,
				row->changedFileName,
				row->logCollectorReadLine, 
				row->logCollectorReadLineSeek);
		} else {
        	row->logCollectorReadLine = 0;
			row->logCollectorReadLineSeek = 0;
		}
        
        // Now, update the container with the latest results
        row->logCollectorFileSize = st.st_size;
	
        // Row Modified Time
        row->logCollectorUpdatedTime_len = MAX_DATE_TIME_LEN;

        time(&now);

		strcpy(row->logCollectorUpdatedTime,
			(u_char *)date_n_time(&now,
				(size_t*)&(row->logCollectorUpdatedTime_len)));

        // Update the line number
        row->logCollectorGetLine = row->logCollectorReadLine;
        row->logCollectorLine = 0;
         
		if (row->logCollectorReadLineSeek == row->logCollectorFileSize)
        	return(0);
	}
       
    if ((fp = fopen(row->changedFileName, "r")) == NULL) {
		DEBUGMSGTL(("LogCol/LogCol_SearchPatterns",
			"Failed to open file <%s %s> <%d>\n",
			row->logCollectorFile, 
			row->changedFileName,
			errno));

		return (-1);
	}

    DEBUGMSGTL(("LogCol/LogCol_SearchPatterns",
        "#%d# File <%s %s> opened\n",
        MY_THR_ID, 
		row->logCollectorFile, 
		row->changedFileName));

    // Set file pointer to matched line, last time - 
	// CR4: always search till the EOF

    lineNum = row->logCollectorReadLine;

	if (st.st_size < row->logCollectorReadLineSeek) {
		snmp_log(LOG_ERR, "logFile[%s] size[%ld] readLineSeek[%ld]\n",
			row->changedFileName, 
			st.st_size,
			row->logCollectorReadLineSeek);

		return 0;
	}

	fseek(fp, row->logCollectorReadLineSeek, SEEK_SET);

    strFound = 0;

    // Read the Log file and find the pattern

	while (1) {
		memset(buf, 0, sizeof(buf));

		if (fgets(buf, LOG_MAX_LEN, fp) == NULL) {

			// if EOF reached or read error then
			// free memory and close files

			if (fp) {
				row->logCollectorReadLineSeek = ftell(fp);
				FCLOSE(fp);
			}

			DEBUGMSGTL(("LogCol/LogCol_SearchPatterns",
				"#%d# File <%s %s> closed; fgets failed\n",
					MY_THR_ID,
					row->logCollectorFile,
					row->changedFileName));

			break;
		}

		buf[LOG_MAX_LEN-1] = '\0';

		lineNum++;

		// Check if match is present in the current line read
#ifdef WIN32
		if ((strlen(row->logCollectorFilter) > 0) && 
			(strstr(buf, row->logCollectorFilter) != NULL))
#else // Only for Unix
		if ((strlen(row->logCollectorFilter) > 0) && 
			(LogCol_CheckPattern(buf, row->logCollectorFilter) == 1))
#endif
		{
			row->filterCount++;
			row->filterCount = row->filterCount % row->logCollectorFilterCount;

			if (row->filterCount == 0) {
				varBinds = makeLogColVarBind(row, buf, lineNum);
				if (Common_SendAlert_And_Vars("LGCL", gcInfoLogFileProc, 
					row->logCollectorDescription, 
					row->logCollectorIndex, varBinds, 5)) { 
					snmp_log(LOG_ERR, "Sending log failed for <%s>\n", 
						row->changedFileName); 
				}

				if (varBinds != NULL) 
					free(varBinds);
			}

			// Set the string found flag-atleast once the pattern got matched
			strFound = lineNum;
		}
	} // Until we find a match or EOF is reached

    if (fp) {
		row->logCollectorReadLineSeek = ftell(fp);
        FCLOSE(fp);
	}

    DEBUGMSGTL(("LogCol/LogCol_SearchPatterns",
         "#%d# File <%s %s> closed\n",
		 MY_THR_ID,
		 row->logCollectorFile,
		 row->changedFileName));

    /* Last row that matched */ 
    if (strFound)
    	row->logCollectorLine = strFound;
    
    // Now, update the container with the latest results
    row->logCollectorFileSize = st.st_size;
	
    // Row Modified Time
    row->logCollectorUpdatedTime_len = MAX_DATE_TIME_LEN;

    time(&now);

    strcpy(row->logCollectorUpdatedTime,
        (u_char *) date_n_time(&now,
        (size_t*)&(row->logCollectorUpdatedTime_len)));

    // Update the line number
    row->logCollectorGetLine = lineNum;

    // Last line read
    row->logCollectorReadLine = lineNum;

    return (strFound);
}

#ifndef WIN32

int LogCol_SearchWtmp(logCollectorTable_context *row)
{
	int		i = 0, flag = 0;
    int     strFound = 0;
    int     lineNum = 0;	// Current line number
    time_t  now = 0;
    char	msg[256+1];
    struct stat st = {0};
#ifdef aix4
	struct utmp* utent;
	struct utmp tmp;
#elif hpux11
	struct utmps* utent;
	struct utmps tmp;
#else
	struct utmpx* utent;
	struct utmpx tmp;
#endif
	time_t	last_time = 0;
	struct tm* tmptr = NULL;

	char remote_host[256+1];
	char* ptr, *type;

	stVarBindArgProc* varBinds = NULL;

    // Find the input log file size and check for error

    if (stat(row->changedFileName, &st) == -1) {
        DEBUGMSGTL(("LogCol/LogCol_SearchWtmp",
            "#%d# File <%s %s> closed; stat failed\n",
            MY_THR_ID, 
			row->logCollectorFile, 
			row->changedFileName));

        return (-1);
    }

    if (row->logCollectorFileSize > st.st_size 
	|| row->logCollectorReadLine == 0) {
        DEBUGMSGTL(("LogCol/LogCol_SearchWtmp",
			"FileSize=[%d], st.size=[%d], ReadLine=[%d], isStart=[%d]\n", 
			row->logCollectorFileSize, 
			st.st_size,
			row->logCollectorReadLine, 
			row->isStart));

		if (row->isStart) {
			SLEEP(rand() % 60);

        	row->logCollectorReadLine = LogCol_GetLineCountAndSeekWtmp(row->changedFileName);

        	row->logCollectorReadLineSeek = row->logCollectorFileSize;
			row->isStart = 0;

			snmp_log(LOG_INFO,
				"LOGCOL WTMP:: THR<%ld> FILE[%s] LINE[%ld] SIZE[%ld]\n",
				MY_THR_ID,
				row->changedFileName,
				row->logCollectorReadLine, 
				row->logCollectorReadLineSeek);
		} else {
        	row->logCollectorReadLine = 0;
			row->logCollectorReadLineSeek = 0;
		}
        
        // Now, update the container with the latest results
        row->logCollectorFileSize = st.st_size;
	
        // Row Modified Time
        row->logCollectorUpdatedTime_len = MAX_DATE_TIME_LEN;

        time(&now);

		strcpy(row->logCollectorUpdatedTime,
			(u_char *)date_n_time(&now,
				(size_t*)&(row->logCollectorUpdatedTime_len)));

        // Update the line number
        row->logCollectorGetLine = row->logCollectorReadLine;
        row->logCollectorLine = 0;

		if (row->logCollectorReadLineSeek == row->logCollectorFileSize)
        	return(0);
	}

#ifdef aix4
	if (utmpname(row->changedFileName) == 0) {
#elif hpux11
	bwtmpname(row->changedFileName);

	if ( 0 ) {
#else
	if (utmpxname(row->changedFileName) == 0) {
#endif
		DEBUGMSGTL(("LogCol/LogCol_SearchWtmp",
			"Failed to open file <%s %s> <%d>\n",
			row->logCollectorFile, 
			row->changedFileName,
			errno));

		return (-1);
	}

    DEBUGMSGTL(("LogCol/LogCol_SearchWtmp",
        "#%d# File <%s %s> opened\n",
        MY_THR_ID, 
		row->logCollectorFile, 
		row->changedFileName));

	lineNum = row->logCollectorReadLine;

	for (i = 0; i < lineNum; i++) {
#ifdef aix4
		utent = getutent();
#elif hpux11
		utent = getbwent(sizeof(struct utmps));
#else
		utent = getutxent();
#endif
		if (utent == NULL) {
			DEBUGMSGTL(("LogCol/LogCol_SearchWtmp",
				"#%d# utent == NULL\n", MY_THR_ID));
			flag = 1;
			break;
		}
	}

	if (flag == 1) {		/* reset file pointer */
		DEBUGMSGTL(("LogCol/LogCol_SearchWtmp",
			"#%d# reset wtmp pointer\n", MY_THR_ID));
		row->logCollectorReadLine = 0;
#ifdef aix4
		endutent();
#elif hpux11
		endbwent();
#else
		endutxent();
#endif
		return (-1);
	}

    strFound = 0;

	while (1) {
		memset(msg, 0, sizeof(msg));
#ifdef aix4
		utent = getutent();
#elif hpux11
		utent = getbwent(sizeof(struct utmps));
#else
		utent = getutxent();
#endif
		// if EOF reached or read error then break

		if (utent == NULL) {
			DEBUGMSGTL(("LogCol/LogCol_SearchWtmp",
				"#%d# File <%s %s> getutxent failed\n",
					MY_THR_ID,
					row->logCollectorFile,
					row->changedFileName));
			break;
		}

		if (utent->ut_type == USER_PROCESS) {
			type = "USER";
		} else if (utent->ut_type == DEAD_PROCESS) {
			type = "DEAD";
		} else if (utent->ut_type == LOGIN_PROCESS) {
			type = "LOGIN";
		} else if (utent->ut_type == INIT_PROCESS) {
			type = "INIT";
		} else {
			type = " ? ";
		}

		lineNum++;

		if (strcmp(type, "USER") != 0 && strcmp(type, "DEAD") != 0) 
			continue;

#ifdef aix4
		last_time = utent->ut_time;
#elif hpux11
		last_time = utent->ut_tv.tv_sec;
#else
		last_time = utent->ut_tv.tv_sec;
#endif
		tmptr = localtime(&last_time);

		sprintf(msg, 
		"%s,%s,%s,%s,%04d/%02d/%02d %02d:%02d:%02d",
			type, 
			utent->ut_user, 
			utent->ut_line, 
			utent->ut_host,
			tmptr->tm_year+1900,
			tmptr->tm_mon+1,
			tmptr->tm_mday,
			tmptr->tm_hour,
			tmptr->tm_min,
			tmptr->tm_sec);
			
		// Check if match is present in the current line read

#ifdef WIN32
		if ((strlen(row->logCollectorFilter) > 0) && 
			(strstr(msg, row->logCollectorFilter) != NULL))
#else // Only for Unix
		if ((strlen(row->logCollectorFilter) > 0) && 
			(LogCol_CheckPattern(msg, row->logCollectorFilter) == 1))
#endif
		{
			row->filterCount++;
			row->filterCount = row->filterCount % row->logCollectorFilterCount;

			if (row->filterCount == 0) {
				varBinds = makeLogColVarBind(row, msg, lineNum);
				if (Common_SendAlert_And_Vars("LGCL", gcInfoLogFileProc, 
					row->logCollectorDescription, 
					row->logCollectorIndex, varBinds, 5)) { 
					snmp_log(LOG_ERR, "Sending log failed for <%s>\n", 
						row->changedFileName); 
				}

				if (varBinds != NULL) 
					free(varBinds);
			}

			// Set the string found flag-atleast once the pattern got matched
			strFound = lineNum;
		}
	} // Until we find a match or EOF is reached

#ifdef aix4
	endutent();
#elif hpux11
	endbwent();
#else
	endutxent();
#endif
    DEBUGMSGTL(("LogCol/LogCol_SearchWtmp",
         "#%d# File <%s %s> closed\n",
		 MY_THR_ID,
		 row->logCollectorFile,
		 row->changedFileName));

    /* Last row that matched */ 
    if (strFound)
    	row->logCollectorLine = strFound;
    
    // Now, update the container with the latest results
    row->logCollectorFileSize = st.st_size;
	
    // Row Modified Time
    row->logCollectorUpdatedTime_len = MAX_DATE_TIME_LEN;

    time(&now);

    strcpy(row->logCollectorUpdatedTime,
        (u_char *) date_n_time(&now,
        (size_t*)&(row->logCollectorUpdatedTime_len)));

    // Update the line number
    row->logCollectorGetLine = lineNum;

    // Last line read
    row->logCollectorReadLine = lineNum;

    return (strFound);
}

#else

int LogCol_SearchWtmp(logCollectorTable_context *row)
{
	return (0);
}

#endif


int LogCol_SearchSyslog(logCollectorTable_context *row)
{
    int     strFound = 0;
    int     lineNum = 0; // Current line number
    time_t  now = 0;
    FILE*	fp = NULL; // Input file descriptor
    struct stat st = {0};
    static char buf[LOG_MAX_LEN+1]; // the line read
	stVarBindArgProc* varBinds = NULL;

    // Find the input log file size and check for error

    if (stat(row->changedFileName, &st) == -1) {
        DEBUGMSGTL(("LogCol/LogCol_SearchPatterns",
            "#%d# File <%s %s> closed; stat failed\n",
            MY_THR_ID, 
			row->logCollectorFile, 
			row->changedFileName));

        return (-1);
    }

	/* CR2 : Whenever file name is changed, monitor from beginning */
    /* CR1 : If file size is less than last time, monitor from begining */
    /* CR3 : When monitoring start, move to end-of-file for skip previous log */
	
    if (row->logCollectorFileSize > st.st_size
	|| row->logCollectorReadLine == 0) { 
		if (row->isStart) {
			SLEEP(rand() % 60);

        	row->logCollectorReadLine = LogCol_GetLineCountAndSeek(
					row->changedFileName, 
					&row->logCollectorReadLineSeek);

			row->isStart = 0;

			snmp_log(LOG_INFO,
				"LOGCOL SYSLOG:: THR<%ld> FILE[%s] LINE[%ld] SIZE[%ld]\n",
				MY_THR_ID,
				row->changedFileName,
				row->logCollectorReadLine, 
				row->logCollectorReadLineSeek);
		} else {
        	row->logCollectorReadLine = 0;
			row->logCollectorReadLineSeek = 0;
		}
        
        // Now, update the container with the latest results
        row->logCollectorFileSize = st.st_size;
	
        // Row Modified Time
        row->logCollectorUpdatedTime_len = MAX_DATE_TIME_LEN;

        time(&now);

		strcpy(row->logCollectorUpdatedTime,
			(u_char *)date_n_time(&now,
				(size_t*)&(row->logCollectorUpdatedTime_len)));

        // Update the line number
        row->logCollectorGetLine = row->logCollectorReadLine;
        row->logCollectorLine = 0;
         
		if (row->logCollectorReadLineSeek == row->logCollectorFileSize)
        	return(0);
	}
       
    if ((fp = fopen(row->changedFileName, "r")) == NULL) {
		DEBUGMSGTL(("LogCol/LogCol_SearchPatterns",
			"Failed to open file <%s %s> <%d>\n",
			row->logCollectorFile, 
			row->changedFileName,
			errno));

		return (-1);
	}

    DEBUGMSGTL(("LogCol/LogCol_SearchPatterns",
        "#%d# File <%s %s> opened\n",
        MY_THR_ID, 
		row->logCollectorFile, 
		row->changedFileName));

    // Set file pointer to matched line, last time - 
	// CR4: always search till the EOF

    lineNum = row->logCollectorReadLine;

	if (st.st_size < row->logCollectorReadLineSeek) {
		snmp_log(LOG_ERR, "logFile[%s] size[%ld] readLineSeek[%ld]\n",
			row->changedFileName, 
			st.st_size,
			row->logCollectorReadLineSeek);

		return 0;
	}

	fseek(fp, row->logCollectorReadLineSeek, SEEK_SET);

    strFound = 0;

    // Read the Log file and find the pattern

	while (1) {
		memset(buf, 0, sizeof(buf));

		if (fgets(buf, LOG_MAX_LEN, fp) == NULL) {

			// if EOF reached or read error then
			// free memory and close files

			if (fp) {
				row->logCollectorReadLineSeek = ftell(fp);
				FCLOSE(fp);
			}

			DEBUGMSGTL(("LogCol/LogCol_SearchPatterns",
				"#%d# File <%s %s> closed; fgets failed\n",
					MY_THR_ID,
					row->logCollectorFile,
					row->changedFileName));

			break;
		}

		buf[LOG_MAX_LEN-1] = '\0';

		lineNum++;

		// Check if match is present in the current line read
#ifdef WIN32
		if ((strlen(row->logCollectorFilter) > 0) && 
			(strstr(buf, row->logCollectorFilter) != NULL))
#else // Only for Unix
		if ((strlen(row->logCollectorFilter) > 0) && 
			(LogCol_CheckPattern(buf, row->logCollectorFilter) == 1))
#endif
		{
			row->filterCount++;
			row->filterCount = row->filterCount % row->logCollectorFilterCount;

			if (row->filterCount == 0) {
				varBinds = makeLogColVarBind(row, buf, lineNum);
				if (Common_SendAlert_And_Vars("LGCL", gcInfoLogFileProc, 
					row->logCollectorDescription, 
					row->logCollectorIndex, varBinds, 5)) { 
					snmp_log(LOG_ERR, "Sending log failed for <%s>\n", 
						row->changedFileName); 
				}

				if (varBinds != NULL) 
					free(varBinds);
			}

			// Set the string found flag-atleast once the pattern got matched
			strFound = lineNum;
		}
	} // Until we find a match or EOF is reached

    if (fp) {
		row->logCollectorReadLineSeek = ftell(fp);
        FCLOSE(fp);
	}

    DEBUGMSGTL(("LogCol/LogCol_SearchPatterns",
         "#%d# File <%s %s> closed\n",
		 MY_THR_ID,
		 row->logCollectorFile,
		 row->changedFileName));

    /* Last row that matched */ 
    if (strFound)
    	row->logCollectorLine = strFound;
    
    // Now, update the container with the latest results
    row->logCollectorFileSize = st.st_size;
	
    // Row Modified Time
    row->logCollectorUpdatedTime_len = MAX_DATE_TIME_LEN;

    time(&now);

    strcpy(row->logCollectorUpdatedTime,
        (u_char *) date_n_time(&now,
        (size_t*)&(row->logCollectorUpdatedTime_len)));

    // Update the line number
    row->logCollectorGetLine = lineNum;

    // Last line read
    row->logCollectorReadLine = lineNum;

    return (strFound);
}


stVarBindArgProc *makeLogColVarBind(logCollectorTable_context *row, 
	char *read_line, int read_line_num)
{
	stVarBindArgProc* varBinds = (stVarBindArgProc *)NULL;
	int var_cnt = 5;

	varBinds = (stVarBindArgProc *)malloc(sizeof(stVarBindArgProc) * var_cnt);

	memset(varBinds, 0, sizeof(stVarBindArgProc) * var_cnt);

	strcpy(varBinds[0].variable, ".1.3.6.1.4.1.3204.1.3.35.9.1.2");
	varBinds[0].type='s';
	strcpy(varBinds[0].value, row->logCollectorFile);

	strcpy(varBinds[1].variable, ".1.3.6.1.4.1.3204.1.3.35.9.1.3");
	varBinds[1].type='s';
	strcpy(varBinds[1].value, row->logCollectorFileType);

	strcpy(varBinds[2].variable, ".1.3.6.1.4.1.3204.1.3.35.9.1.4");
	varBinds[2].type='s';
	strcpy(varBinds[2].value, row->logCollectorDescription);

	strcpy(varBinds[3].variable, ".1.3.6.1.4.1.3204.1.3.35.9.1.5");
	varBinds[3].type='s';
	strncpy(varBinds[3].value, read_line, 299);

	strcpy(varBinds[4].variable, ".1.3.6.1.4.1.3204.1.3.35.9.1.6");
	varBinds[4].type='i';
	sprintf(varBinds[4].value, "%d", read_line_num);

	return varBinds;
}

/*****************************************************************************
 *
 * name             :   LogCol_UpdateDataFile
 * description      :   This function is used to update the MIB data file with
 *                      the latest record
 * input parameters :   logCollectorTable_context *row
 * output parameters:   None
 * return type      :   int - "0" for success in writing new record,
 *                          - "-1" for error,
 *                          - "1" for success in modifying existing record
 * global variables :   None
 * calls            :   None
 */

#ifndef WIN32

int LogCol_UpdateDataFile(logCollectorTable_context* row)
{
	int fd = -1;
	int found = 0;
	unsigned long  pos = 0;
	logCollectorTable_context ctx;

    // Open the MIB data file
    if ((fd = open(LOG_COL_DATA_FILE, O_CREAT|O_RDWR, S_IREAD|S_IWRITE)) < 0) {
        snmp_log_perror("Error opening LOG_COL_DATA_FILE");
        return(-1);
    }

	DEBUGMSGTL(("LogCol/LogCol_UpdateDataFile",
		"#%d# File <%s> opened\n", 
		MY_THR_ID, 
		LOG_COL_DATA_FILE));

	// Set the file descriptor to begining of the file
	lseek(fd, 0, SEEK_SET);

	if (row->logCollectorStatus == SNMP_ROW_DESTROY) {
		DEBUGMSGTL(("LogCol/LogCol_UpdateDataFile",
			"#%d# Found row status as destroyed <%s %d>\n",
			MY_THR_ID, 
			row->logCollectorFile, 
			row->logCollectorStatus));

            row->threadActiveFlag = 0;
	}

	while (read(fd, &ctx, LOG_COL_CONTEXT_SIZE) > 0 ) {
		if (ctx.idx == *row->index.oids) {
			// Check if the row entry in the file 
			// is actually deleted from container
			if (ctx.logCollectorStatus == SNMP_ROW_DESTROY)
				found = 0;
			else
				found = 1;

			if (lseek(fd, pos, SEEK_SET) == (pos - 1)) {
				snmp_log_perror("Error while re-setting fd position");
			} else {
				DEBUGMSGTL(("LogCol/LogCol_UpdateDataFile",
					"OID <%u> found FoundFlag <%d>\n",
						*row->index.oids, 
						found));
			}

			break;
		}

		pos = lseek(fd, 0, SEEK_CUR);
	}

	// Copy the oid, instead copying the pointer
	row->idx = *(row->index.oids);

	if (write(fd, row, LOG_COL_CONTEXT_SIZE) < 0) {
		snmp_log_perror("Error while writing data to LOG_COL_DATA_FILE");

		DEBUGMSGTL(("LogCol/LogCol_UpdateDataFile",
			"#%d# File <%s> closed; write failed\n", 
			MY_THR_ID, 
			LOG_COL_DATA_FILE));

		close(fd);

		return(-1);
	}

	DEBUGMSGTL(("LogCol/LogCol_UpdateDataFile", "Record written\n"));

    DEBUGMSGTL(("LogCol/LogCol_UpdateDataFile",
        "#%d# File <%s> closed; successful\n", 
		MY_THR_ID, 
		LOG_COL_DATA_FILE));

    close(fd);

    return (found);
}


/*****************************************************************************
 *
 * name             :   LogCol_CheckPattern
 * description      :   This function returns 1 if the pattern is found in str
 *                      else returns 0
 * input parameters :   char *str, char *pattern
 * output parameters:   None
 * return type      :   int - "0" for pattern not found,
 *                          - "1" for pattern found
 * global variables :   None
 * calls            :   None
 */

int LogCol_CheckPattern(char *str, char *pattern) 
{
    int found = 0;
    int compileResult = 0;
    regex_t reg;
    regmatch_t matches[1];

    DEBUGMSGTL(("LogCol/LogCol_CheckPattern", 
		"LogCol_CheckPattern [%s][%s]\n", 
		str, pattern));

    compileResult = regcomp(&reg, pattern, REG_EXTENDED);

	if (compileResult != 0) {			/* failed */
		DEBUGMSGTL(("LogCol/LogCol_CheckPattern", 
			"LogCol_CheckPattern compile failed\n"));
		snmp_log(LOG_ERR, "LogCol_CheckPattern regex compile error <%d>\n",
			compileResult);

		return found;
	}

	if (regexec(&reg, str, 1, matches, 0) == 0) {
        DEBUGMSGTL(("LogCol/LogCol_CheckPattern", 
			"LogCol_CheckPattern found match pattern\n"));
        found = 1;
    }
	    
    regfree(&reg);
    
    return found;
}

#else			// Windows

int LogCol_UpdateDataFile(logCollectorTable_context *row)
{
    HANDLE fd = NULL;
    DWORD  num = 0;
    int    found = 0;
    unsigned long pos = 0;
    logCollectorTable_context inFile;

    // Open the MIB data file
    fd = CreateFile   (LOG_COL_DATA_FILE,
                        GENERIC_READ|GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    if (fd == INVALID_HANDLE_VALUE) {
        snmp_log_perror("Error opening LOG_COL_DATA_FILE");
        return(-1);
	}

	DEBUGMSGTL(("LogCol/LogCol_UpdateDataFile",
		"#%d# File <%s> opened\n",
		MY_THR_ID, LOG_COL_DATA_FILE));

	// Set the file descriptor to begining of the file
	SetFilePointer(fd, 0, NULL, FILE_BEGIN);
	ReadFile(fd, &inFile, LOG_COL_CONTEXT_SIZE, &num, NULL);

	if (row->logCollectorStatus == SNMP_ROW_DESTROY )
		row->threadActiveFlag = 0;

	while (num == LOG_COL_CONTEXT_SIZE) {
		if (inFile.idx == *row->index.oids ) {
			// Check if the row entry in the file
			// is actually deleted fromthe container
			if (inFile.logCollectorStatus == SNMP_ROW_DESTROY )
				found = 0;
			else
				found = 1;

			if (SetFilePointer(fd, pos, NULL, FILE_BEGIN) == (pos -1)) {
				snmp_log_perror("Error while re-setting fd position");
			} else {
				DEBUGMSGTL(("LogCol/LogCol_UpdateDataFile",
					"OID <%u> found FoundFlag <%d>\n",
					*row->index.oids, 
					found));
			}

			break;
		}

		pos = SetFilePointer(fd, 0, NULL, FILE_CURRENT);
		ReadFile(fd, &inFile, LOG_COL_CONTEXT_SIZE, &num, NULL);
	}

	// Copy the oid, instead copying the pointer
	row->idx = *(row->index.oids);

	if (WriteFile(fd, row, LOG_COL_CONTEXT_SIZE, &num, NULL) == 0) {
		snmp_log_perror("Error while writing data to LOG_COL_DATA_FILE");

		DEBUGMSGTL(("LogCol/LogCol_UpdateDataFile",
			"#%d# File <%s> closed; write failed\n",
			MY_THR_ID, LOG_COL_DATA_FILE));

		CloseHandle(fd);
		return (-1);
	}

	DEBUGMSGTL(("LogCol/LogCol_UpdateDataFile", "Record written\n"));

    DEBUGMSGTL(("LogCol/LogCol_UpdateDataFile",
        "#%d# File <%s> closed\n",
        MY_THR_ID, LOG_COL_DATA_FILE));

    CloseHandle(fd);

    return (found);
}


/*****************************************************************************
 * name             :   RTM_Get_Uptime_Win
 * description      :   To get system uptime for windows
 * input parameters :
 * output parameters:
 * return type      :   long
 * global variables :
 * calls            :   send_snmptrap
 *****************************************************************************/

static long RTM_Get_Uptime_Win ()
{
    long uptim = 0;

    uptim = GetTickCount();
    uptim = (long)(uptim/10);

    return uptim;
}

#endif /* WIN32 */


/************************************************************
 * Initializes the logCollectorTable module
 */
void init_logCollectorTable(void)
{
    initialize_table_logCollectorTable();

    /*
     * TODO: perform any startup stuff here
     */

    LogCol_LoadContainerFromFile();

    LogCol_MasterThread();
}

/** added by D.J Park in 2004.3.30
 * make the date formated log file name
 * '%Y' : year format(ex, 2004)
 * '%y' : year format(ex, 04)
 * '%z' : year format(ex, 4)
 * '%M' : month format(ex, 03)
 * '%m' : month format(ex, 3)
 * '%D' : day format(ex, 09)
 * '%d' : day format(ex, 9)
 * '%H' : hour format(ex, 09)
 * '%h' : hour format(ex, 9)
 * '%I' : minute format(ex, 09)
 * '%i' : minute format(ex, 9)
 */
static size_t strtimeformat(char* s, size_t maxsize, const char* format)
{
	char *tempbuf;
	char tbuf[10];
	size_t ret;
	int format_len;
	int i, j;
	struct tm *timeptr;
	time_t tm;

	tm = time(NULL);
	timeptr = localtime(&tm);

	tempbuf = (char *)malloc(maxsize);
	memset(tempbuf,0x00,maxsize);

	format_len = strlen(format);

	for (j = 0 ,i = 0 ; i < format_len ; i++) {
		if (format[i] != '%') {
			tempbuf[j++]=format[i];			
			continue;
		}

		switch(format[i+1]) {	// format[i] == '%'
		case 'Y' :
			sprintf(tbuf,"%d",timeptr->tm_year+1900);
			strcat(tempbuf,tbuf);
			j+=strlen(tbuf);
			i++;
			break;
		case 'y' :
			sprintf(tbuf,"%02d",(timeptr->tm_year+1900)%100);
			strcat(tempbuf,tbuf);
			j+=strlen(tbuf);
			i++;
			break;
		case 'z' :
			sprintf(tbuf,"%d",(timeptr->tm_year+1900)%100);
			strcat(tempbuf,tbuf);
			j+=strlen(tbuf);
			i++;
			break;
		case 'M' :
			sprintf(tbuf,"%02d",timeptr->tm_mon+1);
			strcat(tempbuf,tbuf);
			j+=strlen(tbuf);
			i++;
			break;
		case 'm' :
			sprintf(tbuf,"%d",timeptr->tm_mon+1);
			strcat(tempbuf,tbuf);
			j+=strlen(tbuf);
			i++;
			break;
		case 'D' :
			sprintf(tbuf,"%02d",timeptr->tm_mday);
			strcat(tempbuf,tbuf);
			j+=strlen(tbuf);
			i++;
			break;
		case 'd' :
			sprintf(tbuf,"%d",timeptr->tm_mday);
			strcat(tempbuf,tbuf);
			j+=strlen(tbuf);
			i++;
			break;
		case 'H' :
			sprintf(tbuf,"%02d",timeptr->tm_hour);
			strcat(tempbuf,tbuf);
			j+=strlen(tbuf);
			i++;
			break;
		case 'h' :
			sprintf(tbuf,"%d",timeptr->tm_hour);
			strcat(tempbuf,tbuf);
			j+=strlen(tbuf);
			i++;
			break;
		case 'I' :
			sprintf(tbuf,"%02d",timeptr->tm_min);
			strcat(tempbuf,tbuf);
			j+=strlen(tbuf);
			i++;
			break;
		case 'i' :
			sprintf(tbuf,"%d",timeptr->tm_min);
			strcat(tempbuf,tbuf);
			j+=strlen(tbuf);
			i++;
			break;
		default :
			tempbuf[j++]=format[i];			
		}
	}

	strcpy(s, tempbuf);

	if (tempbuf)
		free(tempbuf);
	
	return strlen(s);
}

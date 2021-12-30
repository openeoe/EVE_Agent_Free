/*****************************************************************************
 *
 * psMonitorTable.c
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Virendra Singh
 *
 * description :
 *      This file is a C source code for Process Monitoring
 *
 * TODO:
 *      None
 *
 * CM Details:
 *
 * $Author: ashok $
 * $Log: psMonitorTable.c,v $
 * Revision 1.39  2003/11/24 05:21:07  ashok
 * added funtion for case independent comparison in Windows
 *
 * Revision 1.38  2003/11/21 13:11:49  sig
 * ProcMon_CheckProc() has been modified for linux
 *
 * Revision 1.36  2003/11/21 05:26:16  ashok
 * Added Windows Functionality.
 *
 * Revision 1.34  2003/11/20 11:43:19  viren
 * Updated with new MIB
 *
 * Revision 1.33  2003/11/20 10:28:44  viren
 * Updated with new MIB
 *
 * Revision 1.30  2003/11/07 12:14:36  viren
 * slight change for pthread_create
 *
 * Revision 1.29  2003/11/07 05:58:46  viren
 * Added extra debug info
 *
 * Revision 1.28  2003/11/06 07:10:04  sig
 * psMonitorTable.c has been modified for linux
 *
 * Revision 1.27  2003/11/05 18:27:07  viren
 * Fixed all known bugs
 *
 * $State: Exp $
 * $Id: psMonitorTable.c,v 1.39 2003/11/24 05:21:07 ashok Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/

/********************************* includes *********************************/
// <Start: WIP_NURI_SOL: Added REENTRANT>
#define _REENTRANT
#define MALLOC_CHECK_ 1
// <End: WIP_NURI_SOL>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
// <Start: WIP_NURI_ALL: Added snmp.h>
#include <net-snmp/library/snmp.h>
// <End: WIP_NURI_ALL>

#include <net-snmp/library/snmp_assert.h>

#include "psMonitorTable.h"
// Added for Windows.
#include <string.h>

static     netsnmp_handler_registration *my_handler = NULL;
static     netsnmp_table_array_callbacks cb;

oid psMonitorTable_oid[] = { psMonitorTable_TABLE_OID };
size_t psMonitorTable_oid_len = OID_LENGTH(psMonitorTable_oid);


#ifdef psMonitorTable_IDX2
/************************************************************
 * keep binary tree to find context by name
 */
static int psMonitorTable_cmp( const void *lhs, const void *rhs );

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int
psMonitorTable_cmp( const void *lhs, const void *rhs )
{
    psMonitorTable_context *context_l =
        (psMonitorTable_context *)lhs;
    psMonitorTable_context *context_r =
        (psMonitorTable_context *)rhs;

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
             "psMonitorTable_compare not implemented! Container order undefined\n" );
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
psMonitorTable_context *
psMonitorTable_get( const char *name, int len )
{
    psMonitorTable_context tmp;

    /** we should have a secondary index */
    netsnmp_assert(cb.container->next != NULL);

    /*
     * TODO: implement compare. Remove this ifdef code and
     * add your own code here.
     */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "psMonitorTable_get not implemented!\n" );
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
 * Initializes the psMonitorTable module
 */
void
init_psMonitorTable(void)
{
    initialize_table_psMonitorTable();

    /*
     * TODO: perform any startup stuff here
     */

    // <Start: WIP_NURI_ALL: Added this code>
    ProcMon_LoadContainerFromFile();

    ProcMon_MasterThread();
    // <End: WIP_NURI_ALL>
}

/************************************************************
 * the *_row_copy routine
 */
static int psMonitorTable_row_copy(psMonitorTable_context * dst,
                         psMonitorTable_context * src)
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
    dst->psMonitorIndex = src->psMonitorIndex;

    memcpy( dst->psMonitorName, src->psMonitorName, src->psMonitorName_len );
    dst->psMonitorName_len = src->psMonitorName_len;

    memcpy( dst->psMonitorParameter, src->psMonitorParameter, src->psMonitorParameter_len );
    dst->psMonitorParameter_len = src->psMonitorParameter_len;

    dst->psMonitorNumber = src->psMonitorNumber;

    dst->psMonitorCheckInterval = src->psMonitorCheckInterval;

    memcpy( dst->psMonitorFilter, src->psMonitorFilter, src->psMonitorFilter_len );
    dst->psMonitorFilter_len = src->psMonitorFilter_len;

    memcpy( dst->psMonitorDescription, src->psMonitorDescription, src->psMonitorDescription_len );
    dst->psMonitorDescription_len = src->psMonitorDescription_len;

    memcpy( dst->psMonitorCreatedDateTime, src->psMonitorCreatedDateTime, src->psMonitorCreatedDateTime_len );
    dst->psMonitorCreatedDateTime_len = src->psMonitorCreatedDateTime_len;

    dst->psMonitorReaction = src->psMonitorReaction;

    memcpy( dst->psMonitorReactionPath, src->psMonitorReactionPath, src->psMonitorReactionPath_len );
    dst->psMonitorReactionPath_len = src->psMonitorReactionPath_len;

    memcpy( dst->psMonitorAlertMessage, src->psMonitorAlertMessage, src->psMonitorAlertMessage_len );
    dst->psMonitorAlertMessage_len = src->psMonitorAlertMessage_len;

    dst->psMonitorStatus = src->psMonitorStatus;

    return 0;
}

#ifdef psMonitorTable_SET_HANDLING

/*
 * the *_extract_index routine
 */
int
psMonitorTable_extract_index( psMonitorTable_context * ctx, netsnmp_index * hdr )
{
    /*
     * temporary local storage for extracting oid index
     */
    netsnmp_variable_list var_psMonitorIndex;
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
       memset( &var_psMonitorIndex, 0x00, sizeof(var_psMonitorIndex) );
       var_psMonitorIndex.type = ASN_UNSIGNED;
       /** TODO: link this index to the next, or NULL for the last one */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "psMonitorTable_extract_index index list not implemented!\n" );
    return 0;
#else
       // <Start: WIP_NUIR_ALL: Commented>
       //var_psMonitorIndex.next_variable = &var_XX;
           // <End: WIP_NURI_ALL>
#endif


    /*
     * parse the oid into the individual components
     */
    err = parse_oid_indexes( hdr->oids, hdr->len, &var_psMonitorIndex );
    if (err == SNMP_ERR_NOERROR) {
       /*
        * copy components into the context structure
        */
                ctx->psMonitorIndex = *var_psMonitorIndex.val.integer;


           /*
            * TODO: check index for valid values. For EXAMPLE:
            *
              * if ( *var_psMonitorIndex.val.integer != XXX ) {
          *    err = -1;
          * }
          */
    }

    /*
     * parsing may have allocated memory. free it.
     */
    snmp_reset_var_buffers( &var_psMonitorIndex );

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
int psMonitorTable_can_activate(psMonitorTable_context *undo_ctx,
                      psMonitorTable_context *row_ctx,
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
int psMonitorTable_can_deactivate(psMonitorTable_context *undo_ctx,
                        psMonitorTable_context *row_ctx,
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
int psMonitorTable_can_delete(psMonitorTable_context *undo_ctx,
                    psMonitorTable_context *row_ctx,
                    netsnmp_request_group * rg)
{
    /*
     * probably shouldn't delete a row that we can't
     * deactivate.
     */
    if(psMonitorTable_can_deactivate(undo_ctx,row_ctx,rg) != 1)
        return 0;

    /*
     * TODO: check for other deletion requirements here
     */
    return 1;
}

#ifdef psMonitorTable_ROW_CREATION
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
 * returns a newly allocated psMonitorTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
psMonitorTable_context *
psMonitorTable_create_row( netsnmp_index* hdr)
{
    psMonitorTable_context * ctx =
        SNMP_MALLOC_TYPEDEF(psMonitorTable_context);
    if(!ctx)
        return NULL;

    /*
     * TODO: check indexes, if necessary.
     */
    if(psMonitorTable_extract_index( ctx, hdr )) {
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
    /**
     ctx->psMonitorName = 0;
     ctx->psMonitorParameter = 0;
     ctx->psMonitorNumber = 0;
     ctx->psMonitorCheckInterval = 0;
     ctx->psMonitorFilter = 0;
     ctx->psMonitorDescription = 0;
     ctx->psMonitorCreatedDateTime = 0;
     ctx->psMonitorReaction = 0;
     ctx->psMonitorReactionPath = 0;
     ctx->psMonitorAlertMessage = 0;
     ctx->psMonitorStatus = 0;
    */

    return ctx;
}
#endif

/************************************************************
 * the *_duplicate row routine
 */
psMonitorTable_context *
psMonitorTable_duplicate_row( psMonitorTable_context * row_ctx)
{
    psMonitorTable_context * dup;

    if(!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(psMonitorTable_context);
    if(!dup)
        return NULL;

    if(psMonitorTable_row_copy(dup,row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index * psMonitorTable_delete_row( psMonitorTable_context * ctx )
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
void psMonitorTable_set_reserve1( netsnmp_request_group *rg )
{
    psMonitorTable_context *row_ctx =
            (psMonitorTable_context *)rg->existing_row;
    psMonitorTable_context *undo_ctx =
            (psMonitorTable_context *)rg->undo_info;
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

        case COLUMN_PSMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
            rc = netsnmp_check_vb_type_and_size(var, ASN_UNSIGNED,
                                                sizeof(row_ctx->psMonitorIndex));
        break;


        case COLUMN_PSMONITORNAME:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                        // <Start: WIP_NURI_ALL: Modified here>
                                               //sizeof(row_ctx->psMonitorName));
                        var->val_len);
                        // <End: WIP_NURI_ALL>

        break;

        case COLUMN_PSMONITORPARAMETER:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                        // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->psMonitorParameter));
                        var->val_len);
                        // <End: WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORNUMBER:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->psMonitorNumber));
        break;

        case COLUMN_PSMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->psMonitorCheckInterval));
            DEBUGMSGTL(("psMonitorTable_set_commit",
                    "<rc=%d>\n", rc));
        break;

        case COLUMN_PSMONITORFILTER:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                        // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->psMonitorFilter));
                        var->val_len);
                        // <End: WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                        // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->psMonitorDescription));
                        var->val_len);
                        // <End: WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                        // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->psMonitorCreatedDateTime));
                        var->val_len);
                        // <End: WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->psMonitorReaction));
        break;

        case COLUMN_PSMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                        // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->psMonitorReactionPath));
                        var->val_len);
                        // <End: WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                        // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->psMonitorAlertMessage));
                        var->val_len);
                        // <End: WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->psMonitorStatus));
        break;

        default: /** We shouldn't get here */
            rc = SNMP_ERR_GENERR;
            snmp_log(LOG_ERR, "unknown column in "
                     "psMonitorTable_set_reserve1\n");
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

void psMonitorTable_set_reserve2( netsnmp_request_group *rg )
{
    psMonitorTable_context *row_ctx = (psMonitorTable_context *)rg->existing_row;
    psMonitorTable_context *undo_ctx = (psMonitorTable_context *)rg->undo_info;
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

        case COLUMN_PSMONITORINDEX:
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

        case COLUMN_PSMONITORNAME:
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

        case COLUMN_PSMONITORPARAMETER:
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

        case COLUMN_PSMONITORNUMBER:
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

        case COLUMN_PSMONITORCHECKINTERVAL:
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

        case COLUMN_PSMONITORFILTER:
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

        case COLUMN_PSMONITORDESCRIPTION:
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

        case COLUMN_PSMONITORCREATEDDATETIME:
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

        case COLUMN_PSMONITORREACTION:
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

        case COLUMN_PSMONITORREACTIONPATH:
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

        case COLUMN_PSMONITORALERTMESSAGE:
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

        case COLUMN_PSMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
                rc = netsnmp_check_vb_rowstatus(current->ri->requestvb,
                                                undo_ctx ?
                                                undo_ctx->psMonitorStatus:0);
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
void psMonitorTable_set_action( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    psMonitorTable_context *row_ctx = (psMonitorTable_context *)rg->existing_row;
    psMonitorTable_context *undo_ctx = (psMonitorTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    int            row_err = 0;

    /*
     * TODO: loop through columns, copy varbind values
     * to context structure for the row.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_PSMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
            row_ctx->psMonitorIndex = *var->val.integer;
        break;

        case COLUMN_PSMONITORNAME:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->psMonitorName,var->val.string,var->val_len);
            row_ctx->psMonitorName_len = var->val_len;
            // <Start:WIP_NURI_ALL>
            row_ctx->psMonitorName[var->val_len]=0;
            // <End:WIP_NURI_ALL>

        break;

        case COLUMN_PSMONITORPARAMETER:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->psMonitorParameter,var->val.string,var->val_len);
            row_ctx->psMonitorParameter_len = var->val_len;
            // <Start:WIP_NURI_ALL>
            row_ctx->psMonitorParameter[var->val_len]=0;
            // <End:WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORNUMBER:
            /** INTEGER = ASN_INTEGER */
            row_ctx->psMonitorNumber = *var->val.integer;
        break;

        case COLUMN_PSMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
            row_ctx->psMonitorCheckInterval = *var->val.integer;
        break;

        case COLUMN_PSMONITORFILTER:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->psMonitorFilter,var->val.string,var->val_len);
            row_ctx->psMonitorFilter_len = var->val_len;
            // <Start:WIP_NURI_ALL>
            row_ctx->psMonitorFilter[var->val_len]=0;
            // <End:WIP_NURI_ALL>

        break;

        case COLUMN_PSMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->psMonitorDescription,var->val.string,var->val_len);
            row_ctx->psMonitorDescription_len = var->val_len;
            // <Start:WIP_NURI_ALL>
            row_ctx->psMonitorDescription[var->val_len]=0;
            // <End:WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->psMonitorCreatedDateTime,var->val.string,var->val_len);
            row_ctx->psMonitorCreatedDateTime_len = var->val_len;
            // <Start:WIP_NURI_ALL>
            row_ctx->psMonitorCreatedDateTime[var->val_len]=0;
            // <End:WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
            row_ctx->psMonitorReaction = *var->val.integer;
        break;

        case COLUMN_PSMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->psMonitorReactionPath,var->val.string,var->val_len);
            row_ctx->psMonitorReactionPath_len = var->val_len;
            // <Start:WIP_NURI_ALL>
            row_ctx->psMonitorReactionPath[var->val_len]=0;
            // <End:WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->psMonitorAlertMessage,var->val.string,var->val_len);
            row_ctx->psMonitorAlertMessage_len = var->val_len;
            // <Start:WIP_NURI_ALL>
            row_ctx->psMonitorAlertMessage[var->val_len]=0;
            // <End:WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
            row_ctx->psMonitorStatus = *var->val.integer;
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
#ifndef psMonitorTable_CAN_MODIFY_ACTIVE_ROW
    if( undo_ctx && RS_IS_ACTIVE(undo_ctx->psMonitorStatus) &&
        row_ctx && RS_IS_ACTIVE(row_ctx->psMonitorStatus) ) {
            row_err = 1;
    }
#endif

    /*
     * check activation/deactivation
     */
    row_err = netsnmp_table_array_check_row_status(&cb, rg,
                                  row_ctx ? &row_ctx->psMonitorStatus : NULL,
                                  undo_ctx ? &undo_ctx->psMonitorStatus : NULL);
    if(row_err) {
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
void psMonitorTable_set_commit( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    psMonitorTable_context *row_ctx = (psMonitorTable_context *)rg->existing_row;
    psMonitorTable_context *undo_ctx = (psMonitorTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;
    // <Start: WIP_NURI_ALL: Added code here>
    pthread_t       pPsThr;
    time_t          now;

    DEBUGMSGTL(("psMonitorTable_set_commit",
                    "Commit is being called\n"));

    if ( ProcMon_UpdateDataFile(row_ctx) == 0 )
    {

        // Row Created/ Changed Time
        row_ctx->psMonitorCreatedDateTime_len=MAX_DATE_TIME_LEN;

        // Get the Date-Time in time_t format
        // and update the created time for the newly
        // created row
        (void *) time(&now);

        strcpy(row_ctx->psMonitorCreatedDateTime,
                (u_char *) date_n_time(&now,
                (size_t*)&(row_ctx->psMonitorCreatedDateTime_len)));


        if ( pthread_create(&pPsThr, NULL, ProcMon_ProcMonThreadMain,
                        (void*)row_ctx) != 0 )
        {
            snmp_log_perror("Thread creation failed");
        }
    }

    // <End: WIP_NURI_ALL>


    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_PSMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
        break;

        case COLUMN_PSMONITORNAME:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORPARAMETER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORNUMBER:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORFILTER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORSTATUS:
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

/************************************************************
 * If either of the RESERVE calls fail, the write routines
 * are called again with the FREE action, to release any resources
 * that have been allocated. The agent will then return a failure
 * response to the requesting application.
 *
 * AFTER calling this routine, the agent will delete undo_info.
 */
void psMonitorTable_set_free( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    psMonitorTable_context *row_ctx = (psMonitorTable_context *)rg->existing_row;
    psMonitorTable_context *undo_ctx = (psMonitorTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_PSMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
        break;

        case COLUMN_PSMONITORNAME:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORPARAMETER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORNUMBER:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORFILTER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
        break;

        default: /** We shouldn't get here */
            /** should have been logged in reserve1 */
        // <Start: WIP_NURI_ALL: Modified to add break statment>
        break;
        // <End: WIP_NURI_ALL>
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
void psMonitorTable_set_undo( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    psMonitorTable_context *row_ctx = (psMonitorTable_context *)rg->existing_row;
    psMonitorTable_context *undo_ctx = (psMonitorTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_PSMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
        break;

        case COLUMN_PSMONITORNAME:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORPARAMETER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORNUMBER:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORFILTER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PSMONITORSTATUS:
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

#endif /** psMonitorTable_SET_HANDLING */


/************************************************************
 *
 * Initialize the psMonitorTable table by defining its contents and how it's structured
 */
void
initialize_table_psMonitorTable(void)
{
    netsnmp_table_registration_info *table_info;

    if(my_handler) {
        snmp_log(LOG_ERR, "initialize_table_psMonitorTable_handler called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /* if your table is read only, it's easiest to change the
       HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY */
    my_handler = netsnmp_create_handler_registration("psMonitorTable",
                                             netsnmp_table_array_helper_handler,
                                             psMonitorTable_oid,
                                             psMonitorTable_oid_len,
                                             HANDLER_CAN_RWRITE);

    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_psMonitorTable_handler\n");
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
        /** index: psMonitorIndex */
        netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

    table_info->min_column = psMonitorTable_COL_MIN;
    table_info->max_column = psMonitorTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = psMonitorTable_get_value;
    cb.container = netsnmp_container_find("psMonitorTable_primary:"
                                          "psMonitorTable:"
                                          "table_container");
#ifdef psMonitorTable_IDX2
    netsnmp_container_add_index(cb.container,
                                netsnmp_container_find("psMonitorTable_secondary:"
                                                       "psMonitorTable:"
                                                       "table_container"));
    cb.container->next->compare = psMonitorTable_cmp;
#endif
#ifdef psMonitorTable_SET_HANDLING
    cb.can_set = 1;
#ifdef psMonitorTable_ROW_CREATION
    cb.create_row = (UserRowMethod*)psMonitorTable_create_row;
#endif
    cb.duplicate_row = (UserRowMethod*)psMonitorTable_duplicate_row;
    cb.delete_row = (UserRowMethod*)psMonitorTable_delete_row;
    cb.row_copy = (Netsnmp_User_Row_Operation *)psMonitorTable_row_copy;

    cb.can_activate = (Netsnmp_User_Row_Action *)psMonitorTable_can_activate;
    cb.can_deactivate = (Netsnmp_User_Row_Action *)psMonitorTable_can_deactivate;
    cb.can_delete = (Netsnmp_User_Row_Action *)psMonitorTable_can_delete;

    cb.set_reserve1 = psMonitorTable_set_reserve1;
    cb.set_reserve2 = psMonitorTable_set_reserve2;
    cb.set_action = psMonitorTable_set_action;
    cb.set_commit = psMonitorTable_set_commit;
    cb.set_free = psMonitorTable_set_free;
    cb.set_undo = psMonitorTable_set_undo;
#endif
    DEBUGMSGTL(("initialize_table_psMonitorTable",
                "Registering table psMonitorTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);
}

/************************************************************
 * psMonitorTable_get_value
 */
int psMonitorTable_get_value(
            netsnmp_request_info *request,
            netsnmp_index *item,
            netsnmp_table_request_info *table_info )
{
    netsnmp_variable_list *var = request->requestvb;
    psMonitorTable_context *context = (psMonitorTable_context *)item;

    switch(table_info->colnum) {

        case COLUMN_PSMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->psMonitorIndex,
                         sizeof(context->psMonitorIndex) );
        break;

        case COLUMN_PSMONITORNAME:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->psMonitorName,
                         context->psMonitorName_len );
        break;

        case COLUMN_PSMONITORPARAMETER:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->psMonitorParameter,
                         context->psMonitorParameter_len );
        break;

        case COLUMN_PSMONITORNUMBER:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->psMonitorNumber,
                         sizeof(context->psMonitorNumber) );
        break;

        case COLUMN_PSMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->psMonitorCheckInterval,
                         sizeof(context->psMonitorCheckInterval) );
        break;

        case COLUMN_PSMONITORFILTER:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->psMonitorFilter,
                         context->psMonitorFilter_len );
        break;

        case COLUMN_PSMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->psMonitorDescription,
                         context->psMonitorDescription_len );
        break;

        case COLUMN_PSMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->psMonitorCreatedDateTime,
                         context->psMonitorCreatedDateTime_len );
        break;

        case COLUMN_PSMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->psMonitorReaction,
                         sizeof(context->psMonitorReaction) );
        break;

        case COLUMN_PSMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->psMonitorReactionPath,
                         context->psMonitorReactionPath_len );
        break;

        case COLUMN_PSMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->psMonitorAlertMessage,
                         context->psMonitorAlertMessage_len );
        break;

        case COLUMN_PSMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->psMonitorStatus,
                         sizeof(context->psMonitorStatus) );
        break;

    default: /** We shouldn't get here */
        snmp_log(LOG_ERR, "unknown column in "
                 "psMonitorTable_get_value\n");
        return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}

/************************************************************
 * psMonitorTable_get_by_idx
 */
const psMonitorTable_context *
psMonitorTable_get_by_idx(netsnmp_index * hdr)
{
    return (const psMonitorTable_context *)
        CONTAINER_FIND(cb.container, hdr );
}


// <Start: WIP_NURI_ALL: Added the new code from here>

/*****************************************************************************
 *
 * name             :   ProcMon_MasterThread
 * description      :   This is the master thread function which gets called
 *                      at the initialization of table
 * input parameters :   None
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   None
 */

void
ProcMon_MasterThread()
{
    int iIdx=0;
    pthread_t stProcMonThr[MAX_PROC_PTHREAD_NUM];
    psMonitorTable_context *pstPsRow=NULL;

    pstPsRow = CONTAINER_FIRST(cb.container);

    // As long as we get valid rows
    while(pstPsRow)
    {

        // Now, spawn thread for it
        if ( pthread_create(&stProcMonThr[iIdx], NULL,
                ProcMon_ProcMonThreadMain,
            (void*)pstPsRow) != 0 )
        {
            snmp_log_perror("Thread creation failed");
        }
        else
        {
            // Increment the thread count
            iIdx++;
        }

        pstPsRow = CONTAINER_NEXT(cb.container, pstPsRow);
    }

}


/*****************************************************************************
 *
 * name             :   ProcMon_ProcMonThreadMain
 * description      :   This is the main function for thread
 * input parameters :   void *psData - the Process Monitoring row context
 * output parameters:   None
 * return type      :   void *
 * global variables :   None
 * calls            :   None
 */

void *
ProcMon_ProcMonThreadMain(void *psData)
{
    int                     iCurPid=-1;
    int                     iOldPid=-1;
    int                     iSendAlertFlag = 0;
    psMonitorTable_context  *pstMycontext;


    DEBUGMSGTL(("ProcMon_ProcMonThreadMain",
            "Thread <%d> spawned \n",
            pthread_self()));

    pstMycontext = (psMonitorTable_context*)psData;

    while(1)
    {
        iCurPid=-1;

        // Before i start checking for process,
        // get the latest values of the data
        pstMycontext = CONTAINER_FIND(cb.container,&(pstMycontext->index));

        // If the row does not exist anymore
        if ( pstMycontext == NULL )
        {
            DEBUGMSGTL(("ProcMon_ProcMonThreadMain",
                "Exiting thread <%d>\n",pthread_self()));
            pthread_detach(pthread_self());
            pthread_exit(NULL);
        }

        // Check if the Row is active, else continue until
        // it becomes active
        if ( pstMycontext->psMonitorStatus == SNMP_ROW_ACTIVE )
        {
            // Do the value check, before proceeding
            if ( (strlen((char*)pstMycontext->psMonitorName) == 0 ) ||
                ( pstMycontext->psMonitorCheckInterval == 0 ) )
            {
#ifndef WIN32
                sleep(3);
#else
                Sleep(3000);
#endif
                continue;   // No need to process the request,
                            // until valid values are provided
            }

            // For reaction 2,3 check for reaction path also.
            if ( ( ( pstMycontext->psMonitorReaction == 2 ) ||
                ( pstMycontext->psMonitorReaction == 3 ) ) &&
                (strlen((char*)pstMycontext->psMonitorReactionPath) == 0 ) )
            {
#ifndef WIN32
                sleep(3);
#else
                Sleep(3000);
#endif
                continue;   // No need to process the request,
                            // until valid values are provided
            }
        }
        else
        {
            // since the row is not active
#ifndef WIN32
                sleep(3);
#else
                Sleep(3000);
#endif
                continue; 
        }

        DEBUGMSGTL(("ProcMon_ProcMonThreadMain",
            "Checking OID <%u> OID_LEN <%d> PROC <%s>\n",
            *(pstMycontext->index.oids),
            pstMycontext->index.len,
            pstMycontext->psMonitorName));


        // Check for the process status
        iCurPid = ProcMon_CheckProc(pstMycontext->psMonitorName);

        // If the process is running
        if ( iCurPid > 0 )
        {
            // Reset iSendAlertFlag
            iSendAlertFlag = 0;
        }

        DEBUGMSGTL(("ProcMon_ProcMonThreadMain",
                "iCurPid <%d> iOldPid <%d>\n",iCurPid, iOldPid));

        // Now, based on the reaction type, take appropriate action
        if ( pstMycontext->psMonitorReaction == 1 )
        {
            // i.e. the process state has changed
            // First time if the process is not running
            if ( ( iCurPid == -1 ) && ( iOldPid == -1 ) && ( iSendAlertFlag == 0 ) )
            {
                if (ProcMon_SendAlert(pstMycontext->psMonitorName,
                                    pstMycontext->psMonitorAlertMessage))
                    DEBUGMSGTL(("ProcMon_ProcMonThreadMain",
                        "Alert sending failed for <%s>\n",
                        pstMycontext->psMonitorName));
                else
                    iSendAlertFlag = 1;
            }
            else
            if ( iCurPid != iOldPid )
            {
                if (ProcMon_SendAlert(pstMycontext->psMonitorName,
                                    pstMycontext->psMonitorAlertMessage))
                DEBUGMSGTL(("ProcMon_ProcMonThreadMain",
                        "Alert sending failed for <%s>\n",
                        pstMycontext->psMonitorName));
                else
                    iSendAlertFlag = 1;

            }

        }
        else
        if ( pstMycontext->psMonitorReaction == 2 )
        {
            // Restart the process, if not running
            // and also send alert if the process was
            // restarted in the last cycle
            if ( iCurPid == -1 )
            {
                if ( ProcMon_RestartProcess(pstMycontext->psMonitorReactionPath)
                        == -1 )
                {
                    snmp_log_perror("Process restarting failed");
                }

                if( ProcMon_SendAlert(pstMycontext->psMonitorName,
                                    pstMycontext->psMonitorAlertMessage))
                    DEBUGMSGTL(("ProcMon_ProcMonThreadMain",
                        "Alert sending failed for <%s>\n",
                        pstMycontext->psMonitorName));
            }
        }
        else
        if ( pstMycontext->psMonitorReaction == 3 )
        {
            // Do both the above
            // restart the process, if not running, and send the
            // alert only once

            if ( iCurPid == -1  )
            {
                // Send an alert mesg
                if (ProcMon_SendAlert(pstMycontext->psMonitorName,
                                        pstMycontext-> psMonitorAlertMessage))
                    DEBUGMSGTL(("ProcMon_ProcMonThreadMain",
                        "Alert sending failed for <%s>\n",
                        pstMycontext->psMonitorName));

                if ( ProcMon_RestartProcess(pstMycontext->psMonitorReactionPath)
                        == -1 )
                {
                    snmp_log_perror("Process restarting failed");
                }

                // Send an alert mesg
                if (ProcMon_SendAlert(pstMycontext->psMonitorName,
                                        pstMycontext-> psMonitorAlertMessage))
                    DEBUGMSGTL(("ProcMon_ProcMonThreadMain",
                        "Alert sending failed for <%s>\n",
                        pstMycontext->psMonitorName));
            }
        }
        else
            DEBUGMSGTL(("ProcMon_ProcMonThreadMain",
                "Invalid Reaction type specified for <%s>\n",
                pstMycontext->psMonitorName));

        // Sleep until the next interval
        DEBUGMSGTL(("ProcMon_ProcMonThreadMain",
            "Thread <%u> is going to Sleep for <%u> now !!\n",
            pthread_self(),
            pstMycontext->psMonitorCheckInterval));

#if defined(WIN32)
        Sleep(pstMycontext->psMonitorCheckInterval*1000);
#else
        sleep(pstMycontext->psMonitorCheckInterval);
#endif

        DEBUGMSGTL(("ProcMon_ProcMonThreadMain",
            "Thread <%u> woke up !!\n",
            pthread_self()));

        // Assign the current pid to old pid
        iOldPid = iCurPid;

    } // End of infinite while loop

}


/*****************************************************************************
 *
 * name             :   ProcMon_SendAlert
 * description      :   This function is used to send the alert mesg to the
 *                      manager
 * input parameters :   char *name- Process Name
 *                      char *mesg - Alert message
 * output parameters:   None
 * return type      :   int - returns success (0) or failure (1)
 * global variables :   None
 * calls            :   None
 */

int
ProcMon_SendAlert(char *name,char *mesg)
{
    FILE *pfOutFd;
	nuriAlert stAlert;
	char acSysName[MAXHOSTNAMELEN+1];

#ifndef WIN32
	struct utsname sys;
#else
    int iNameLen=50;
    char* sys;
    WSADATA stWsData;
#endif

    // prepare the stAlert
    // Get the local machine IP
    ProcMon_GetLocalHostIp(stAlert.masterIp);

    // Get the current date and time string
    ProcMon_GetCurrDateTime(stAlert.dateTime);	//YYYY:MM:DD:HH:MI:SS

#ifndef WIN32
    uname(&sys);
	strcpy(acSysName , sys.nodename);
#else
    sys = (char*) malloc(iNameLen);
    WSAStartup(MAKEWORD( 2, 2 ),&stWsData);

    //This Statement can be unsucessful at times.
    gethostname(sys,iNameLen);
    strcpy(acSysName , sys);
#endif

	// Prepare the stAlert message and pass it to
    // ProcMon_SendMesg function
    strcpy(stAlert.modCode, "PSMN");
	strcpy(stAlert.sysName,acSysName);
	strcpy(stAlert.alertMesg,mesg);

    // If output file is specified
     // Now, log this in to the Proc Mon Output logfile too
    if ( strlen(gcInfoLogFileProc) == 0 )
        snmp_log(LOG_ERR,"No Process Log File specified in the MIB\n");
    else
    {
        if ( (pfOutFd = fopen( gcInfoLogFileProc, "a") ) == NULL )
        {
            snmp_log(LOG_ERR,"Error opening %s",gcInfoLogFileProc);
            snmp_log_perror("");
            fclose(pfOutFd);
        }
        else
        {

            // Write the Alert message into it
            fprintf(pfOutFd, "%s | %s | %s | %s | %s \n",
                                           stAlert.modCode,
                                           stAlert.masterIp,
                                           stAlert.dateTime,
                                           stAlert.sysName,
                                           stAlert.alertMesg);

            fclose(pfOutFd);
        } // End of Else
    } // End of Else

	// Now, send this stAlert mesg over UDP socket
    if ( ProcMon_SendMesg(stAlert) )
	{
		snmp_log(LOG_ERR,"UDP mesg sending to manager failed\n");
		return(1);
	}

	return (0);
}

/*****************************************************************************
 *
 * name             :   ProcMon_GetLocalHostIp
 * description      :   This function is used to get the IP of the localhost
 * input parameters :   char *pcSysIp
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   None
 */

void
ProcMon_GetLocalHostIp(char *pcSysIp)
{
    struct hostent *he;
    struct sockaddr_in in;
    char *pcIp;
#ifndef WIN32 // For unix
    struct utsname host_name;
#else  // For windows
    char* host_name;
    int iNameLen = 50;
    WSADATA stWsData;
#endif

#ifndef WIN32
    uname (&host_name);
#else
    host_name = (char*) malloc(iNameLen);
    // Modified for Windows
    WSAStartup(MAKEWORD( 2, 2 ),&stWsData);
    gethostname(host_name,iNameLen);
#endif
#ifndef WIN32

    if ((he = gethostbyname(host_name.nodename)) == NULL)
#else
    if ((he = gethostbyname(host_name)) == NULL)
#endif
    {
        snmp_log_perror("error resolving hostname..");
#ifdef WIN32
        free(host_name);
#endif
        return;
    }

    memcpy(&in.sin_addr, he->h_addr_list[0], he->h_length);

    pcIp=(char*)inet_ntoa(in.sin_addr);
    pcIp[21]=0;
    strcpy(pcSysIp,pcIp);

    DEBUGMSGTL(("ProcMon_GetLocalHostIp",
        "Localhost IP <%s>\n",
        pcSysIp));

#ifdef WIN32
        free(host_name);
#endif

    return;
}

/*****************************************************************************
 *
 * name             :   ProcMon_GetCurrDateTime
 * description      :   This function is used to get the current date-time
 *                      value string in format "YYYY:MM:DD:HH:MI:SS"
 * input parameters :   char *pcCurrTime
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   None
 */

void
ProcMon_GetCurrDateTime(char *pcCurrTime)
{
#ifndef WIN32 // For unix
    struct tm  *pstTimeStruct;
    struct timeval stTimeVal;
    struct timeval *pstTime=&stTimeVal;
    time_t stTimeValue;

    (void) time(&stTimeValue);

    pstTimeStruct = localtime(&stTimeValue);

    pstTime->tv_sec     = 0;
    pstTime->tv_usec    = 0;

    // Get the date-time of the day
    if (gettimeofday(pstTime,(struct timezone *)NULL) != 0)
    {
        snmp_log_perror("Error calling gettimeofday\n");
        return;
    }

    // Form the date-time string
    sprintf(pcCurrTime,"%.4d:%.2d:%.2d:%.2d:%.2d:%.2d",
                      1900+pstTimeStruct->tm_year,
                      1+pstTimeStruct->tm_mon,
                      pstTimeStruct->tm_mday,
                      pstTimeStruct->tm_hour,
                      pstTimeStruct->tm_min,
                      pstTimeStruct->tm_sec);
#else // For windows
    SYSTEMTIME pstTimeStruct;

    GetLocalTime(&pstTimeStruct);

    pcCurrTime = (char*)malloc(sizeof(char)*21);

    sprintf(pcCurrTime,"%.2d:%.2d:%.2d:%.2d:%.2d:%.2d",
                       pstTimeStruct.wYear,
                       pstTimeStruct.wMonth ,
                       pstTimeStruct.wDay,
                       pstTimeStruct.wHour,
                       pstTimeStruct.wMinute,
                       pstTimeStruct.wSecond);
#endif

    snmp_log(LOG_ERR,"Current Time <%s>\n",pcCurrTime);

    return;
}



/*****************************************************************************
 *
 * name             :   ProcMon_SendMesg
 * description      :   This function is used to send UDP message to the
 *                      manager
 * input parameters :   nuriAlert mesg - alert message struct
 * output parameters:   None
 * return type      :   int - returs sucess (0) or failure (1)
 * global variables :   None
 * calls            :   getIPFromASNFormat
 */

int
ProcMon_SendMesg(nuriAlert stProcAlert)
{
    int                 iSock=0;
    int                 iBufLen=0;
    char                aBuffer[16];
    struct sockaddr_in  stSrvName;
    struct hostent      *pstHostEnt;
    char                *pcUdpMesg;

    // Allocate memory
    pcUdpMesg = (char *) malloc(sizeof(char)*(sizeof(nuriAlert) + 5));

    // Reset it to zero
    memset(pcUdpMesg,0,sizeof(nuriAlert)+5);

    // Form the string
    sprintf(pcUdpMesg,"%s|%s|%s|%s|%s",
                        stProcAlert.modCode,
                        stProcAlert.masterIp,
                        stProcAlert.dateTime,
                        stProcAlert.sysName,
                        stProcAlert.alertMesg);

    // create socket for connecting to server
    iSock = socket(AF_INET, SOCK_DGRAM,0);

    if(iSock < 0)
    {
        snmp_log_perror("opening datagram socket");
        free(pcUdpMesg);
        return(1);
    }

    // construct stSrvName for connecting to server
    stSrvName.sin_family = AF_INET;
    stSrvName.sin_port = htons(glAgentInfoAlertMgrPort);

    // Change the Manager IP from ASN format to string
    getIPFromASNFormat(gcAgentInfoAlertMgrIp, aBuffer);

    // convert hostname to IP address and enter into stSrvName
    stSrvName.sin_addr.s_addr=inet_addr(aBuffer);

    // Check the buffer length
    iBufLen = sizeof(nuriAlert) +1;

    // send message to server
    if(sendto(iSock, (const void*)pcUdpMesg, iBufLen, 0,
                (struct sockaddr *)&stSrvName, sizeof(stSrvName)) <0)
    {
        snmp_log_perror("Error sending datagram message");
        free(pcUdpMesg);
        return(1);
    }
    else

        DEBUGMSGTL(("ProcMon_SendMesg",
            "Alert message <%s> \nSent to Manager IP <%s> \nManager Port <%d>\n",
            pcUdpMesg,
            aBuffer,
            glAgentInfoAlertMgrPort));

    // close connection
    close(iSock);

    // Free memory
    free(pcUdpMesg);

    return(0);
}

/*****************************************************************************
 *
 * name             :   ProcMon_RestartProcess
 * description      :   This function is used to restart the process
 * input parameters :   char *process - The proccess name
 * output parameters:   None
 * return type      :   int - PID of the process or -1 if fork failed
 * global variables :   None
 * calls            :   None
 */

int
ProcMon_RestartProcess(char *psProcName)
{
    int iPid=-1;

#ifndef WIN32
    int fds=0;
    char *psCmd[2]; // For command and its arguments

    DEBUGMSGTL(("ProcMon_RestartProcess",
        "Called\n"));

    psCmd[0] = psProcName;

    // ( Kept for future enhancement )
    psCmd[1] = 0; // NULL terminated arguments string

    // Fork the process in child
    if( (iPid = vfork() ) == 0)  // Child process
    {
        // Close all the parent
        // File descriptors
        fds=3;
        for(fds=3;fds<25;fds++)
        {
            close(fds);
        }

        // Execute the command
        if ( execv(psCmd[0],psCmd) != 0 )
        {

            DEBUGMSGTL(("ProcMon_RestartProcess",
                "Process <%s> could not be restarted\n",
                psCmd[0]));

            // Starting failed, so exit the child
            exit(1);
        }
    }

#else
 // TODO: Windows
    STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;
	BOOL bResult;
	

    ZeroMemory( &sInfo, sizeof(sInfo) );
    sInfo.cb = sizeof(sInfo);

	bResult = CreateProcess((LPTSTR) psProcName,NULL,NULL,NULL,
                TRUE,0,NULL,NULL,&sInfo,&pInfo);
    iPid = pInfo.dwProcessId;
#endif
    DEBUGMSGTL(("ProcMon_RestartProcess",
        "Returning....\n"));

    return(iPid);
}


/*****************************************************************************
 *
 * name             :   ProcMon_CheckProc
 * description      :   This function is used to send the alert mesg to the
 *                      manager
 * input parameters :   char *name- Process Name
 *                      char *mesg - Alert message
 * output parameters:   None
 * return type      :   int - returns success (0) or failure (1)
 * global variables :   None
 * calls            :   None
 */

#if defined(solaris2)  // Check proc for solaris
int
ProcMon_CheckProc(char *pcProcName)
{
    long                iPid=-1;
#if _SLASH_PROC_METHOD_
    psinfo_t stPsInfo;
    DIR                 *pstProcDir;
    struct dirent       *pstDirEnt;
    int                 iProcFd;
    char                acProcFn[sizeof "/proc/00000/psinfo"];

    DEBUGMSGTL(("ProcMon_CheckProc",
        "for solaris <SLASH_PROC_METHOD> called\n"));

    if ((pstProcDir = opendir ((const char*)"/proc")) == NULL)
    {
        snmp_log_perror ("Unable to open /proc dir\n");
        return (-2);
    }

    while (((pstDirEnt = readdir (pstProcDir)) != NULL) )
    {
        if ((!strcmp (pstDirEnt->d_name, ".")) ||
            (!strcmp (pstDirEnt->d_name, "..")))
        {
            continue;
        }

        iPid = atoi(pstDirEnt->d_name);

        sprintf(acProcFn, "/proc/%.5d/psinfo", iPid);

        if ((iProcFd = open(acProcFn, O_RDONLY)) != -1)
        {
            if (read(iProcFd, &stPsInfo, sizeof(stPsInfo)) !=
                                        sizeof(stPsInfo))
            {
                snmp_log_perror("Error while read /proc");
                return(-2);
            }

            close(iProcFd);
        }

        if ( strstr(stPsInfo.pr_fname,pcProcName) )
        {
            // Do the action here which is specified for
            // this process
            DEBUGMSGTL(("ProcMon_CheckProc",
                "Process Found <%s %s>\n",
                stPsInfo.pr_fname,
                pcProcName));
            break;
        }

        // Reset PID
        iPid=-1;

    } // End of while

    closedir(pstProcDir);

#else

    int iFndArg=0;
    int iCnt=0;
    struct user *pstUsrEnv;
    proc_t *pstCurProc;
    char ***pstCurProc;
    char ***pcEnv;
    kvm_t *pstKd;

    DEBUGMSGTL(("ProcMon_CheckProc",
        "for solaris <KVM_METHOD> called\n"));

    if ((pstKd = kvm_open(NULL, NULL, NULL, O_RDONLY, NULL)) == NULL)
    {
        snmp_log_perror("kvm_open failed");
        return (-2);
    }

    kvm_setproc(pstKd);

    // Iterate thru the proc list
    while ((pstCurProc = kvm_nextproc(pstKd)) != NULL)
    {
        if((pstUsrEnv = kvm_getu(pstKd, pstCurProc))!=NULL)
        {
            kvm_getcmd(pstKd, pstCurProc, pstUsrEnv, &pstCurProc,&pcEnv);

            if ( strstr((char*)pstCurProc[0],pcProcName) )
            {
                // Do the action here which is specified for
                // this process
                DEBUGMSGTL(("ProcMon_CheckProc",
                    "Process Found <%s>\n",
                    pstCurProc[0]));
                break;
            } // End of if
        }   // End of if

        ++iCnt;
    } // End of while

    kvm_close(pstKd);

#endif

    return iPid;
}
#endif // End of check proc for solaris

/*****************************************************************************
 *
 * name             :   ProcMon_CheckProc
 * description      :   This function is used to get the process status in
 *                      AIX machine
 * input parameters :   char *process - The proccess name
 * output parameters:   None
 * return type      :   int - PID of the process, if running, else -1
 * global variables :   None
 * calls            :   None
 */

#if defined(AIX_4_3)

int
ProcMon_CheckProc(char *process)
{
    int iIdx=0;
    int iMaxNumProc = 1024;
    int  iNumProc=0;
    int iPid=-1;
    int iIndex=0;
    struct procsinfo *pstProcTable=NULL;

    DEBUGMSGTL(("ProcMon_CheckProc",
        "Called for AIX 4.3\n"));
        // Reset the index
    do
    {
        //check whether we are here in the loop, then pstProcTable will
        //have some data. It will be NULL for the first time.
        if(pstProcTable != NULL)
            free(pstProcTable);

        pstProcTable = (struct procsinfo *)malloc(iMaxNumProc*sizeof(struct procsinfo));

        if(pstProcTable != NULL)
            memset(pstProcTable,0,sizeof(iMaxNumProc*sizeof(struct procsinfo)));
        else
        {
          // SNMP LOG MSG TO BE ADDED
          return -1;
        }

        iIdx = 0;

        // Now Get process table populated for the active processes.
        // Get the list of process currently active
        iNumProc = getprocs(pstProcTable, sizeof(struct procsinfo),
                                                NULL, 0,&iIdx,iMaxNumProc);

        if (iNumProc == -1)
        {
            // SNMP LOG MSG TO BE ADDED
            iNumProc = 0 ;
            if(pstProcTable != NULL)
               free(pstProcTable);

            return -1;
        }
        else
        if (iNumProc == iMaxNumProc)
        {
            //Since the number of processes is above iMaxNumProc.
            //We have to loop again after multiplying iMaxNumProc by 2.
            iMaxNumProc *= 2;
            iNumProc = -1;
        }

    } while (iNumProc == -1);

    // Iterate thru the process list
    // and get our process out of it, if it exists
    for(iIndex=0;iIndex<iNumProc;iIndex++)
    {
        /* Check for swapper process */
        if (pstProcTable[iIndex].pi_pid == 0)
        {
            strcpy(pstProcTable[iIndex].pi_comm,"swapper");
        }

        iPid = pstProcTable[iIndex].pi_pid;

        /* Check for zombie process */
        if(strcmp(pstProcTable[iIndex].pi_comm,"")==0)
        {
            strcpy(pstProcTable[iIndex].pi_comm,"unknown");
        }

        if (strstr(pstProcTable[iIndex].pi_comm,process))
        {
            // Do the action here which is specified for
            // this process
            DEBUGMSGTL(("ProcMon_CheckProc",
                "Process Found <%s %s>\n",
                pstProcTable[iIndex].pi_comm,
                process));

            break;
        } // End of if

        // Reset the PID
        iPid=-1;

    } // End of for

    if (pstProcTable != NULL)
        free(pstProcTable);

    return iPid;
}
#endif // End of hash def for AIX

/*****************************************************************************
 *
 * name             :   ProcMon_CheckProc
 * description      :   This function is used to get the process status in
 *                      HP-UX machine
 * input parameters :   char *process - The proccess name
 * output parameters:   None
 * return type      :   int - PID of the process, if running, else -1
 * global variables :   None
 * calls            :   None
 */

#if defined(hpux10) || defined(hpux11)

int
ProcMon_CheckProc(char *process)
{
    int iIdx=0;
    int iCount=0;
    int iUtil=0;
    int iPid=-1;
    int iNumProc = 0, bytes;
    //struct pst_status stPstat;
    struct pst_dynamic stPstDyn;
    struct pst_status *pstProcTable=NULL;

    DEBUGMSGTL(("ProcMon_CheckProc",
        "called for HPUX\n"));

    pstat_getdynamic(&stPstDyn, sizeof(struct pst_dynamic), 1, 0);
    iNumProc = stPstDyn.psd_activeprocs;

    bytes = iNumProc * sizeof(struct pst_status);

    if ((pstProcTable =
        (struct pst_status *) calloc(bytes, sizeof(struct pst_status))) == NULL)
    {
        return;
    }

    if (pstat_getproc (pstProcTable, sizeof (struct pst_status), iNumProc, 0) != -1)
    {

        for (iIdx=0;iIdx < iNumProc;iIdx++)
        {
            iCount += (pstProcTable[iIdx].pst_stat & PS_RUN);

            iUtil += pstProcTable[iIdx].pst_cpu;

            iPid = pstProcTable[iIdx].pst_pid;

            if ( strstr(pstProcTable[iIdx].pst_ucomm, process) )
            {
                DEBUGMSGTL(("ProcMon_CheckProc",
                    "Process Found <%s>\n",
                     pstProcTable[iIdx].pst_ucomm);
                break;
            } // End of If

            iPid=-1;

        } // End of for
    } // End of If

    free(pstProcTable);

    return iPid;
}

#endif // End of HP-UX hash defined

/*****************************************************************************
 *
 * name             :   ProcMon_CheckProc
 * description      :   This function is used to get the process status in
 *                      LINUX machine
 * input parameters :   char *process - The proccess name
 * output parameters:   None
 * return type      :   int - PID of the process, if running, else -1
 * global variables :   None
 * calls            :   None
 */

#ifdef linux
int ProcMon_CheckProc(char* szProcName){
 
	FILE            *fpProc = NULL;
	DIR             *pdProcDir = NULL;
 	struct dirent   *pstProcEntry = NULL;
 	static char  	*pcPos;
   	int    			 iPid;
	static char      szString[256];
    static char      szBuff[256];
  	
	snmp_log(LOG_INFO,"ProcMon_CheckProc: linux  called\n");
 	if (pdProcDir != NULL)
    	closedir(pdProcDir);
    pdProcDir = opendir("/proc");
 	if(pdProcDir == NULL){
  		snmp_log(LOG_INFO, "/proc open error. Exiting..\n");
 		return -1;
	}
    while ((pstProcEntry = readdir(pdProcDir)) != NULL){
  		iPid = atoi(pstProcEntry->d_name);
  		if(iPid){
        	sprintf(szString, "/proc/%d/status", iPid);
            if ((fpProc = fopen(szString, "r")) == NULL)
    			continue;
            fgets(szBuff, sizeof(szBuff), fpProc);   /* Name: Process name */
  			pcPos = (char *)strstr(szBuff, "Name:");
   			if(pcPos){
    			sscanf(pcPos,"Name : %s ", szString);
   			}
   			else{
    			continue;
   			}
 			if (strstr(szString, szProcName) != NULL){
		        snmp_log(LOG_INFO,"ProcMon_CheckProc: linux  process %s %d found. exiting..\n", szString, iPid);
				fclose(fpProc);
				closedir(pdProcDir);
			    return iPid;
			}	
  		}
 	}
 
    snmp_log(LOG_INFO,"ProcMon_CheckProc: linux  process: %s not found.exiting..\n", szProcName);
 	return -1;
}
#endif // End of LINUX hash defined


/*****************************************************************************
 *
 * name             :   ProcMon_CheckProc
 * description      :   This function is used to get the process status in
 *                      UnixWare machine
 * input parameters :   char *process - The proccess name
 * output parameters:   None
 * return type      :   int - PID of the process, if running, else -1
 * global variables :   None
 * calls            :   None
 */
#ifdef sysv4
int ProcMon_CheckProc (char* process)
{
    DIR * dirstream;
    struct psinfo pInfo;
    int procFD;
    char procBuff[255];
    int nread = 0;
    int psInfoSize =sizeof(struct psinfo);
    struct dirent *direntry = NULL;

    DEBUGMSGTL(("ProcMon_CheckProc",
        "called for sysv4\n"));

    // Open proc system
    dirstream = opendir("/proc");

    // Loop until we find the process
    while((direntry = readdir(dirstream))!= '\0')
    {
        if(!strcmp(direntry->d_name,".") || !strcmp(direntry->d_name,"..") ||
                        !strcmp(direntry->d_name,"0"))
        {
            continue;
        }

        sprintf(procBuff,"/proc/%d/psinfo",atoi(direntry->d_name));
        procFD = open(procBuff,O_RDONLY);

        if(procFD<0)
        {
            continue;
        }

        nread = read(procFD,&pInfo,psInfoSize);

        if(nread!=psInfoSize)
        {
            close(procFD);
            continue;
        }

        close(procFD);

        if(strstr(pInfo.pr_fname,process)!=NULL)
        {
            DEBUGMSGTL(("ProcMon_CheckProc",
                "Process Found <%s> <%d>\n",
                pInfo.pr_fname,
                pInfo.pr_pid));

            closedir(dirstream);
            return pInfo.pr_pid;
        }
    }

    closedir(dirstream);

    return -1;;
}
#endif /* sysv4 */

/*****************************************************************************
 *
 * name             :   ProcMon_CheckProc
 * description      :   This function is used to get the process status in
 *                      windows
 * input parameters :   char *process - The proccess name
 * output parameters:   None
 * return type      :   int - PID of the process, if running, else -1
 * global variables :   None
 * calls            :   None
 */

#if defined(WIN32)

int
ProcMon_CheckProc(char *process)
{
    //PDH related Inits
    HQUERY      hQuery        = NULL;
    HCOUNTER    hCounter      = NULL;
    DWORD       dwType        = 0;
    PDH_FMT_COUNTERVALUE    ProcID; //counter structures
    PDH_STATUS  status         = 0; //Init Vars for process count
	PDH_STATUS  pdhResult      = 0;
    char        aPathName [100];
    long        ProcessID;


    DEBUGMSGTL(("ProcMon_CheckProc",
        "called for windows\n"));

    strcpy(aPathName,"\\Process(");
    strcat(aPathName,process);
    strcat(aPathName,")\\ID Process");

    // Open query
    pdhResult = PdhOpenQuery( NULL, 0, &hQuery );

	// Add the counter to the query
	pdhResult = PdhAddCounter( hQuery,aPathName, 0, &hCounter );

	// Run the query to collect the performance data
    pdhResult = PdhCollectQueryData( hQuery );

    //Store the collected counter data into a formatted structure
    PdhGetFormattedCounterValue(hCounter,PDH_FMT_LONG,0, &ProcID);

    // Close the query
    PdhCloseQuery( hQuery );

    ProcessID =  ProcID.longValue;
    
    if( ProcessID == 0 )
    {
        if( _stricmp ( process,"Idle")==0 )
            ProcessID = 0;
        else
            ProcessID = -1;
    }
    
    return(ProcessID);

}
#endif

/*****************************************************************************
 *
 * name             :   ProcMon_LoadContainerFromFile
 * description      :   This function is used load the default configuration
 *                      from the data file
 * input parameters :   None
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   ProcMon_ProcMonThreadMain
 */

void
ProcMon_LoadContainerFromFile()
{
    int iFd=0;
    int iFdTmp=0;
    int iCnt=0;
    int iIdx=0;
    int iRewriteFlag=0;
    char acStrOid[MAX_OID_LEN] = { 0 };
    char acTmpStr[MAX_OID_LEN] = { 0 };
    psMonitorTable_context stReadCtx = {0};
    psMonitorTable_context *pstMyContext = NULL;

    // Open the Data file
    iFd = open (PROC_MON_DATA_FILE, O_RDONLY);

    if (iFd == -1)
    {
        snmp_log_perror("ProcMon_LoadContainerFromFile: \
            PROC_MON_DATA_FILE file open failed\n");
    }
    else
    {
        iIdx=0;

        while (read (iFd, &stReadCtx, sizeof(psMonitorTable_context)) > 0)
        {
            // If the row, is already destroyed, no need to create it again.
            if ( stReadCtx.psMonitorStatus == SNMP_ROW_DESTROY )
            {
                iRewriteFlag = 1;   // to re-write the mib data file
                continue; // Don't increment the row_count also
            }


            // Otherwise
            DEBUGMSGTL(("ProcMon_LoadContainerFromFile",
                "Loading MIB Row for Proc Name <%s>\n",
                stReadCtx.psMonitorName));

            pstMyContext =
             (psMonitorTable_context*)malloc (sizeof(psMonitorTable_context));

            memcpy (pstMyContext, &stReadCtx, sizeof (psMonitorTable_context));

            netsnmp_assert(pstMyContext->index.oids == NULL);

            if(snmp_clone_mem( (void*)&pstMyContext->index.oids,
                            &(stReadCtx.idx),
                            stReadCtx.index.len * sizeof(oid) ))
            {
                free(pstMyContext);
                return;
            }

            pstMyContext->index.len = stReadCtx.index.len;

            iCnt = CONTAINER_INSERT (cb.container, pstMyContext);

            if (iCnt)
            {
                DEBUGMSGTL(("initialize_table_psMonitorTable",
                "ERROR IN CONTAINER_INSERT FUNCION\n"));
            }

            DEBUGMSGTL(("ProcMon_LoadContainerFromFile",
                "OID in ProcMon_LoadContainerFromFile <%u>\n",
                *(pstMyContext->index.oids)));

        }
        // Close the file
        close (iFd);

        if ( iRewriteFlag )
        {

            DEBUGMSGTL(("ProcMon_LoadContainerFromFile",
                "Rewriting the data file\n"));

            if ( ( iFd = open (PROC_MON_DATA_FILE, O_RDONLY) ) == -1 )
            {
                snmp_log_perror("PROC_MON_DATA_FILE file open failed\n");
            }
            else
            {
                // open a new temporary file
                if ( ( iFdTmp = open (PROC_MON_DATA_FILE_TMP,
                                                 O_CREAT| O_RDWR) ) == -1 )
                {
                    snmp_log_perror(
                        "PROC_MON_DATA_FILE_TMP file open failed\n");
                }
                else
                {

                    // Reset the file pointers to begining of the files
                    lseek(iFd, 0,SEEK_SET);
                    lseek(iFdTmp, 0,SEEK_SET);

                    iIdx=0;

                    while (read (iFd, &stReadCtx,
                                sizeof (psMonitorTable_context)) > 0)
                    {
                        if ( stReadCtx.psMonitorStatus != SNMP_ROW_DESTROY )
                            if ( write(iFdTmp,&stReadCtx,
                                        sizeof(psMonitorTable_context)) < 0 )
                            {
                                snmp_log_perror(("Error while writing \
                                        data to PROC_MON_DATA_FILE"));
                            }

                    } // End of while
                } // End of else

                // Close the file
                close (iFd);
                close (iFdTmp);

                // Delete the old file
                unlink(PROC_MON_DATA_FILE);

                // rename the temp file to PROC_MON_DATA_FILE
                rename(PROC_MON_DATA_FILE_TMP,PROC_MON_DATA_FILE);

            } // End of else
        } // End of rewrite if


        DEBUGMSGTL(("initialize_table_psMonitorTable",
            "CONTAINER SIZE=%u\n",
            CONTAINER_SIZE (cb.container)));

    } // End of else

    // Don't do this !! this will delete the last row
    // from the container
    //free(pstMyContext);

}

/*****************************************************************************
 *
 * name             :   ProcMon_UpdateDataFile
 * description      :   This function is used to update the MIB data file with
 *                      the latest record
 * input parameters :   psMonitorTable_context *pstProcMonRow
 * output parameters:   None
 * return type      :   int - "0" for success in writing new record,
 *                          - "-1" for error,
 *                          - "1" for success in modifying existing record
 * global variables :   None
 * calls            :   None
 */


int
ProcMon_UpdateDataFile(psMonitorTable_context *pstProcMonRow)
{
    psMonitorTable_context stProcInFile;
    int iFd=-1;
    int iFoundFlag=0;
    int iRowNum=0;
    unsigned long ulPos=0;


    // Open the MIB data file
    // Added 'S_IREAD|S_IWRITE' for Windows
    if ( (iFd = open(PROC_MON_DATA_FILE,O_CREAT|O_RDWR,S_IREAD|S_IWRITE)) < 0 )
    {
        snmp_log_perror("Error opening LOG_MON_ROW_DATA_FILE");
        return(-1);
    }
    else
    {

        lseek(iFd, 0,SEEK_SET);

        while( read(iFd, &stProcInFile, sizeof(psMonitorTable_context)) > 0 )
        {
            if ( stProcInFile.idx == *pstProcMonRow->index.oids )
            {

                // Check if the row entry in the file
                // is actually deleted fromthe container
                if ( stProcInFile.psMonitorStatus == SNMP_ROW_DESTROY )
                    iFoundFlag =0;
                else
                    iFoundFlag =1;

                if ( lseek(iFd, ulPos,SEEK_SET) == (ulPos -1) )
                {
                    snmp_log_perror(
                        "Error while re-setting file descriptor position");
                }
                else
                {
                    DEBUGMSGTL(("LogMonCol_UpdateDataFile",
                        "OID <%u> found FoundFlag <%d>\n",
                            *pstProcMonRow->index.oids,
                            iFoundFlag));

                }
                break;
            }

            ulPos = lseek(iFd, 0, SEEK_CUR);
        }

        // Copy the oid, instead copying the pointer
        pstProcMonRow->idx = *(pstProcMonRow->index.oids);

        if ( write(iFd,pstProcMonRow,sizeof(psMonitorTable_context)) < 0 )
        {
            snmp_log_perror("Error while writing data to LOG_MON_ROW_DATA_FILE");
            return(-1);
        }
        else
            DEBUGMSGTL(("LogMonCol_UpdateDataFile",
                "Record written successfully to the data file\n"));
    }

    return(iFoundFlag);
}

// <End: WIP_NURI_ALL>

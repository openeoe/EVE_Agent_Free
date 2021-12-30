/*****************************************************************************
 *
 * logMonRowTable.c
 *
 *      Copyright (c) 2003 Wipro Technologies
 *      All rights Reserved
 *
 * author:
 *      Virendra Singh
 *
 * description :
 *      This file is a C source code for Log Monitoring - Row
 *
 * TODO:
 *      None
 *
 * CM Details:
 *
 * $Author: karthis $
 * $Log: logMonRowTable.c,v $
 * Revision 1.23  2003/11/25 06:48:43  karthis
 * Fixed the IP address problem
 *
 * Revision 1.22  2003/11/24 07:16:14  sig
 * free(pstHostEnt) has been removed from LogMonRow_GetLocalHostIp()
 *
 * Revision 1.21  2003/11/21 12:47:22  viren
 * latest
 *
 * Revision 1.20  2003/11/21 12:28:13  viren
 * latest
 *
 * Revision 1.18  2003/11/21 05:05:24  ashok
 * Added Windows Functionality.
 *
 * Revision 1.17  2003/11/20 16:38:41  viren
 * Hopefully stable
 *
 * Revision 1.15  2003/11/07 09:54:00  viren
 * Updated with the latest MIB
 *
 * Revision 1.13  2003/11/06 13:16:21  viren
 * Fixed all known bugs
 *
 * Revision 1.12  2003/11/06 07:00:23  sig
 * logMonRowTable.c has been modified for linux
 *
 * Revision 1.11  2003/11/04 13:15:00  viren
 * Header File name changed in include
 *
 * Revision 1.10  2003/11/04 13:11:54  viren
 * After beautification
 *
 * $State: Exp $
 * $Id: logMonRowTable.c,v 1.23 2003/11/25 06:48:43 karthis Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/

// <Start: WIP_NURI_ALL: Added REENTRANT>
/********************************* includes *********************************/

#define _REENTRANT

// <End: WIP_NURI_ALL>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

// <Start: WIP_NURI_ALL: Added REENTRANT>

#include <net-snmp/library/snmp.h>

// <End: WIP_NURI_ALL>

#include <net-snmp/library/snmp_assert.h>

#include "logMonRowTable.h"

static     netsnmp_handler_registration *my_handler = NULL;
static     netsnmp_table_array_callbacks cb;

oid logMonRowTable_oid[] = { logMonRowTable_TABLE_OID };
size_t logMonRowTable_oid_len = OID_LENGTH(logMonRowTable_oid);


#ifdef logMonRowTable_IDX2
/************************************************************
 * keep binary tree to find context by name
 */
static int logMonRowTable_cmp( const void *lhs, const void *rhs );

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int
logMonRowTable_cmp( const void *lhs, const void *rhs )
{
    logMonRowTable_context *context_l =
        (logMonRowTable_context *)lhs;
    logMonRowTable_context *context_r =
        (logMonRowTable_context *)rhs;

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
             "logMonRowTable_compare not implemented! Container order undefined\n" );
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
logMonRowTable_context *
logMonRowTable_get( const char *name, int len )
{
    logMonRowTable_context tmp;

    /** we should have a secondary index */
    netsnmp_assert(cb.container->next != NULL);

    /*
     * TODO: implement compare. Remove this ifdef code and
     * add your own code here.
     */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "logMonRowTable_get not implemented!\n" );
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
 * Initializes the logMonRowTable module
 */
void
init_logMonRowTable(void)
{
    initialize_table_logMonRowTable();

    /*
     * TODO: perform any startup stuff here
     */

    // Start: WIP_NURI_ALL: Added the code from here

    //init_agentInfo();
    LogMonRow_LoadContainerFromFile();

    // Create a Master Thread
    LogMonRow_MasterThread();

    // End: WIP_NURI_ALL

}

/************************************************************
 * the *_row_copy routine
 */
static int logMonRowTable_row_copy(logMonRowTable_context * dst,
                         logMonRowTable_context * src)
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
    dst->logMonRowIndex = src->logMonRowIndex;

    memcpy( dst->logMonRowLogFile, src->logMonRowLogFile, src->logMonRowLogFile_len );
    dst->logMonRowLogFile_len = src->logMonRowLogFile_len;

    dst->logMonRowReadInterval = src->logMonRowReadInterval;

    memcpy( dst->logMonRowFilter, src->logMonRowFilter, src->logMonRowFilter_len );
    dst->logMonRowFilter_len = src->logMonRowFilter_len;

    memcpy( dst->logMonRowDescription, src->logMonRowDescription, src->logMonRowDescription_len );
    dst->logMonRowDescription_len = src->logMonRowDescription_len;

    memcpy( dst->logMonRowCreatedTime, src->logMonRowCreatedTime, src->logMonRowCreatedTime_len );
    dst->logMonRowCreatedTime_len = src->logMonRowCreatedTime_len;

    memcpy( dst->logMonRowAlertMessage, src->logMonRowAlertMessage, src->logMonRowAlertMessage_len );
    dst->logMonRowAlertMessage_len = src->logMonRowAlertMessage_len;

    dst->logMonRowStatus = src->logMonRowStatus;

    memcpy( dst->logMonRowUpdatedTime, src->logMonRowUpdatedTime, src->logMonRowUpdatedTime_len );
    dst->logMonRowUpdatedTime_len = src->logMonRowUpdatedTime_len;

    dst->logMonRowLine = src->logMonRowLine;

    dst->logMonRowGetLine = src->logMonRowGetLine;

    dst->logMonRowFileSize = src->logMonRowFileSize;

    dst->logMonRowReadLine = src->logMonRowReadLine;

    return 0;
}

#ifdef logMonRowTable_SET_HANDLING

/*
 * the *_extract_index routine
 */
int
logMonRowTable_extract_index( logMonRowTable_context * ctx, netsnmp_index * hdr )
{
    /*
     * temporary local storage for extracting oid index
     */
    netsnmp_variable_list var_logMonRowIndex;
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
       memset( &var_logMonRowIndex, 0x00, sizeof(var_logMonRowIndex) );
       var_logMonRowIndex.type = ASN_UNSIGNED;
       /** TODO: link this index to the next, or NULL for the last one */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "logMonRowTable_extract_index index list not implemented!\n" );
    return 0;
#else
       // <Start: WIP_NURI_ALL: Commented the code here>

       //var_logMonRowIndex.next_variable = &var_XX;

      // <End: WIP_NURI_ALL>
#endif


    /*
     * parse the oid into the individual components
     */
    err = parse_oid_indexes( hdr->oids, hdr->len, &var_logMonRowIndex );
    if (err == SNMP_ERR_NOERROR) {
       /*
        * copy components into the context structure
        */
                ctx->logMonRowIndex = *var_logMonRowIndex.val.integer;


           /*
            * TODO: check index for valid values. For EXAMPLE:
            *
              * if ( *var_logMonRowIndex.val.integer != XXX ) {
          *    err = -1;
          * }
          */
    }

    /*
     * parsing may have allocated memory. free it.
     */
    snmp_reset_var_buffers( &var_logMonRowIndex );

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
int logMonRowTable_can_activate(logMonRowTable_context *undo_ctx,
                      logMonRowTable_context *row_ctx,
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
int logMonRowTable_can_deactivate(logMonRowTable_context *undo_ctx,
                        logMonRowTable_context *row_ctx,
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
int logMonRowTable_can_delete(logMonRowTable_context *undo_ctx,
                    logMonRowTable_context *row_ctx,
                    netsnmp_request_group * rg)
{
    /*
     * probably shouldn't delete a row that we can't
     * deactivate.
     */
    if(logMonRowTable_can_deactivate(undo_ctx,row_ctx,rg) != 1)
        return 0;

    /*
     * TODO: check for other deletion requirements here
     */
    return 1;
}

#ifdef logMonRowTable_ROW_CREATION
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
 * returns a newly allocated logMonRowTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
logMonRowTable_context *
logMonRowTable_create_row( netsnmp_index* hdr)
{
    logMonRowTable_context * ctx =
        SNMP_MALLOC_TYPEDEF(logMonRowTable_context);
    if(!ctx)
        return NULL;

    /*
     * TODO: check indexes, if necessary.
     */
    if(logMonRowTable_extract_index( ctx, hdr )) {
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
     ctx->logMonRowLogFile = 0;
     ctx->logMonRowReadInterval = 0;
     ctx->logMonRowFilter = 0;
     ctx->logMonRowDescription = 0;
     ctx->logMonRowCreatedTime = 0;
     ctx->logMonRowAlertMessage = 0;
     ctx->logMonRowStatus = 0;
     ctx->logMonRowUpdatedTime = 0;
     ctx->logMonRowLine = 0;
     ctx->logMonRowGetLine = 0;
     ctx->logMonRowFileSize = 0;
     ctx->logMonRowReadLine = 0;
    */

    return ctx;
}
#endif

/************************************************************
 * the *_duplicate row routine
 */
logMonRowTable_context *
logMonRowTable_duplicate_row( logMonRowTable_context * row_ctx)
{
    logMonRowTable_context * dup;

    if(!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(logMonRowTable_context);
    if(!dup)
        return NULL;

    if(logMonRowTable_row_copy(dup,row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index * logMonRowTable_delete_row( logMonRowTable_context * ctx )
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
void logMonRowTable_set_reserve1( netsnmp_request_group *rg )
{
    logMonRowTable_context *row_ctx =
            (logMonRowTable_context *)rg->existing_row;
    logMonRowTable_context *undo_ctx =
            (logMonRowTable_context *)rg->undo_info;
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

        case COLUMN_LOGMONROWLOGFILE:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
        // <Start:WIP_NURI_ALL:Modified>
                               //sizeof(row_ctx->logMonRowLogFile));
                               var->val_len);
        // <End:WIP_NURI_ALL>

        break;

        case COLUMN_LOGMONROWREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonRowReadInterval));
        break;

        case COLUMN_LOGMONROWFILTER:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
        // <Start:WIP_NURI_ALL: Modified>
                               //sizeof(row_ctx->logMonRowFilter));
                               var->val_len);
        // <End:WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONROWDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
        // <Start:WIP_NURI_ALL:Modified>
                               //sizeof(row_ctx->logMonRowDescription));
                               var->val_len);
        // <End:WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONROWCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
        // <Start:WIP_NURI_ALL:Modified>
                               //sizeof(row_ctx->logMonRowCreatedTime));
                               var->val_len);
        // <End:WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONROWALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
        // <Start:WIP_NURI_ALL:Modified>
                               //sizeof(row_ctx->logMonRowAlertMessage));
                               var->val_len);
        // <End:WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONROWSTATUS:
            /** RowStatus = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonRowStatus));
            // <Start: WIP_NURI_ALL: Added for time being>
	    /*
            if ( ( row_ctx->logMonRowStatus == SNMP_ROW_NONEXISTENT ) ||
                 ( row_ctx->logMonRowStatus == SNMP_ROW_NOTINSERVICE ) ||
                 ( row_ctx->logMonRowStatus == SNMP_ROW_NOTREADY ) ||
                 ( row_ctx->logMonRowStatus == SNMP_ROW_CREATEANDWAIT ) )
                rc = SNMP_ERR_BADVALUE;
		*/
           // <End: WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONROWUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                               //sizeof(row_ctx->logMonRowUpdatedTime));
                               var->val_len);
        break;

        case COLUMN_LOGMONROWLINE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonRowLine));
        break;

        case COLUMN_LOGMONROWGETLINE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonRowGetLine));
        break;

        case COLUMN_LOGMONROWFILESIZE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonRowFileSize));
        break;

        case COLUMN_LOGMONROWREADLINE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonRowReadLine));
        break;

        default: /** We shouldn't get here */
            rc = SNMP_ERR_GENERR;
            snmp_log(LOG_ERR, "unknown column in "
                     "logMonRowTable_set_reserve1\n");
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

void logMonRowTable_set_reserve2( netsnmp_request_group *rg )
{
    logMonRowTable_context *row_ctx = (logMonRowTable_context *)rg->existing_row;
    logMonRowTable_context *undo_ctx = (logMonRowTable_context *)rg->undo_info;
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

        case COLUMN_LOGMONROWLOGFILE:
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

        case COLUMN_LOGMONROWREADINTERVAL:
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

        case COLUMN_LOGMONROWFILTER:
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

        case COLUMN_LOGMONROWDESCRIPTION:
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

        case COLUMN_LOGMONROWCREATEDTIME:
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

        case COLUMN_LOGMONROWALERTMESSAGE:
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

        case COLUMN_LOGMONROWSTATUS:
            /** RowStatus = ASN_INTEGER */
                rc = netsnmp_check_vb_rowstatus(current->ri->requestvb,
                                                undo_ctx ?
                                                undo_ctx->logMonRowStatus:0);
                rg->rg_void = current->ri;
        break;

        case COLUMN_LOGMONROWUPDATEDTIME:
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

        case COLUMN_LOGMONROWLINE:
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

        case COLUMN_LOGMONROWGETLINE:
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

        case COLUMN_LOGMONROWFILESIZE:
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

        case COLUMN_LOGMONROWREADLINE:
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
void logMonRowTable_set_action( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    logMonRowTable_context *row_ctx = (logMonRowTable_context *)rg->existing_row;
    logMonRowTable_context *undo_ctx = (logMonRowTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    int            row_err = 0;

    /*
     * TODO: loop through columns, copy varbind values
     * to context structure for the row.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_LOGMONROWLOGFILE:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logMonRowLogFile,var->val.string,var->val_len);
            row_ctx->logMonRowLogFile_len = var->val_len;
            // <Start:WIP_NURI_ALL: Modified here to terminate it with NULL>
            row_ctx->logMonRowLogFile[var->val_len]=0;
            // <End:WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONROWREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonRowReadInterval = *var->val.integer;
        break;

        case COLUMN_LOGMONROWFILTER:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logMonRowFilter,var->val.string,var->val_len);
            row_ctx->logMonRowFilter_len = var->val_len;
            // <Start:WIP_NURI_ALL: Modified here to terminate it with NULL>
            row_ctx->logMonRowFilter[var->val_len]=0;
            // <End:WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONROWDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logMonRowDescription,var->val.string,var->val_len);
            row_ctx->logMonRowDescription_len = var->val_len;
            // <Start:WIP_NURI_ALL: Modified here to terminate it with NULL>
            row_ctx->logMonRowDescription[var->val_len]=0;
            // <End:WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONROWCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->logMonRowCreatedTime,var->val.string,var->val_len);
            row_ctx->logMonRowCreatedTime_len = var->val_len;
            // <Start:WIP_NURI_ALL: Modified here to terminate it with NULL>
            row_ctx->logMonRowCreatedTime[var->val_len]=0;
            // <End:WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONROWALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logMonRowAlertMessage,var->val.string,var->val_len);
            row_ctx->logMonRowAlertMessage_len = var->val_len;
            // <Start:WIP_NURI_ALL: Modified here to terminate it with NULL>
            row_ctx->logMonRowAlertMessage[var->val_len]=0;
            // <End:WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONROWSTATUS:
            /** RowStatus = ASN_INTEGER */
            row_ctx->logMonRowStatus = *var->val.integer;
        break;

        case COLUMN_LOGMONROWUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->logMonRowUpdatedTime,var->val.string,var->val_len);
            row_ctx->logMonRowUpdatedTime_len = var->val_len;
            // <Start:WIP_NURI_ALL: Modified here to terminate it with NULL>
            row_ctx->logMonRowUpdatedTime[var->val_len]=0;
            // <End:WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONROWLINE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonRowLine = *var->val.integer;
        break;

        case COLUMN_LOGMONROWGETLINE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonRowGetLine = *var->val.integer;
        break;

        case COLUMN_LOGMONROWFILESIZE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonRowFileSize = *var->val.integer;
        break;

        case COLUMN_LOGMONROWREADLINE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonRowReadLine = *var->val.integer;
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
#ifndef logMonRowTable_CAN_MODIFY_ACTIVE_ROW
    if( undo_ctx && RS_IS_ACTIVE(undo_ctx->logMonRowStatus) &&
        row_ctx && RS_IS_ACTIVE(row_ctx->logMonRowStatus) ) {
            row_err = 1;
    }
#endif

    /*
     * check activation/deactivation
     */
    row_err = netsnmp_table_array_check_row_status(&cb, rg,
                                  row_ctx ? &row_ctx->logMonRowStatus : NULL,
                                  undo_ctx ? &undo_ctx->logMonRowStatus : NULL);
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
void logMonRowTable_set_commit( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    logMonRowTable_context *row_ctx = (logMonRowTable_context *)rg->existing_row;
    logMonRowTable_context *undo_ctx = (logMonRowTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    // <Start: WIP_NURI_ALL: Modified the code from here>

    pthread_t pLogThr;
    time_t now;

    DEBUGMSGTL(("logMonRowTable_set_commit",
        "LogFile Name <%s>\n",
        row_ctx->logMonRowLogFile));

    if ( LogMonRow_UpdateDataFile(row_ctx) == 0 )
    {
        // Row Created/ Changed Time
        row_ctx->logMonRowCreatedTime_len=MAX_DATE_TIME_LEN;

        // Get the Date-Time in time_t format
        // and update the created time for the newly
        // created row
        (void *) time(&now);
        strcpy(row_ctx->logMonRowCreatedTime,
                (u_char *) date_n_time(&now,
                (size_t*)&(row_ctx->logMonRowCreatedTime_len)));

        // Create a thread for this newly created row
        if ( pthread_create(&pLogThr, NULL, LogMonRow_LogMonThreadMain,
                                  (void*)row_ctx) != 0 )
        {
            snmp_log_perror("logMonRowTable_set_commit \
                Thread creation failed\n");

        }
    }

    // <End: WIP_NURI_ALL>

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_LOGMONROWLOGFILE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWFILTER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWSTATUS:
            /** RowStatus = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWGETLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWFILESIZE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWREADLINE:
            /** INTEGER = ASN_INTEGER */
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
void logMonRowTable_set_free( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    logMonRowTable_context *row_ctx = (logMonRowTable_context *)rg->existing_row;
    logMonRowTable_context *undo_ctx = (logMonRowTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_LOGMONROWLOGFILE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWFILTER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWSTATUS:
            /** RowStatus = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWGETLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWFILESIZE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWREADLINE:
            /** INTEGER = ASN_INTEGER */
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
void logMonRowTable_set_undo( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    logMonRowTable_context *row_ctx = (logMonRowTable_context *)rg->existing_row;
    logMonRowTable_context *undo_ctx = (logMonRowTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_LOGMONROWLOGFILE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWFILTER:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWSTATUS:
            /** RowStatus = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONROWLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWGETLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWFILESIZE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONROWREADLINE:
            /** INTEGER = ASN_INTEGER */
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

#endif /** logMonRowTable_SET_HANDLING */


/************************************************************
 *
 * Initialize the logMonRowTable table by defining its contents and how it's structured
 */
void
initialize_table_logMonRowTable(void)
{
    netsnmp_table_registration_info *table_info;

    if(my_handler) {
        snmp_log(LOG_ERR, "initialize_table_logMonRowTable_handler called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /* if your table is read only, it's easiest to change the
       HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY */
    my_handler = netsnmp_create_handler_registration("logMonRowTable",
                                             netsnmp_table_array_helper_handler,
                                             logMonRowTable_oid,
                                             logMonRowTable_oid_len,
                                             HANDLER_CAN_RWRITE);

    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_logMonRowTable_handler\n");
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
        /** index: logMonRowIndex */
        netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

    table_info->min_column = logMonRowTable_COL_MIN;
    table_info->max_column = logMonRowTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = logMonRowTable_get_value;
    cb.container = netsnmp_container_find("logMonRowTable_primary:"
                                          "logMonRowTable:"
                                          "table_container");
#ifdef logMonRowTable_IDX2
    netsnmp_container_add_index(cb.container,
                                netsnmp_container_find("logMonRowTable_secondary:"
                                                       "logMonRowTable:"
                                                       "table_container"));
    cb.container->next->compare = logMonRowTable_cmp;
#endif
#ifdef logMonRowTable_SET_HANDLING
    cb.can_set = 1;
#ifdef logMonRowTable_ROW_CREATION
    cb.create_row = (UserRowMethod*)logMonRowTable_create_row;
#endif
    cb.duplicate_row = (UserRowMethod*)logMonRowTable_duplicate_row;
    cb.delete_row = (UserRowMethod*)logMonRowTable_delete_row;
    cb.row_copy = (Netsnmp_User_Row_Operation *)logMonRowTable_row_copy;

    cb.can_activate = (Netsnmp_User_Row_Action *)logMonRowTable_can_activate;
    cb.can_deactivate = (Netsnmp_User_Row_Action *)logMonRowTable_can_deactivate;
    cb.can_delete = (Netsnmp_User_Row_Action *)logMonRowTable_can_delete;

    cb.set_reserve1 = logMonRowTable_set_reserve1;
    cb.set_reserve2 = logMonRowTable_set_reserve2;
    cb.set_action = logMonRowTable_set_action;
    cb.set_commit = logMonRowTable_set_commit;
    cb.set_free = logMonRowTable_set_free;
    cb.set_undo = logMonRowTable_set_undo;
#endif
    DEBUGMSGTL(("initialize_table_logMonRowTable",
                "Registering table logMonRowTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);
}

/************************************************************
 * logMonRowTable_get_value
 */
int logMonRowTable_get_value(
            netsnmp_request_info *request,
            netsnmp_index *item,
            netsnmp_table_request_info *table_info )
{
    netsnmp_variable_list *var = request->requestvb;
    logMonRowTable_context *context = (logMonRowTable_context *)item;

    switch(table_info->colnum) {

        case COLUMN_LOGMONROWINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->logMonRowIndex,
                         sizeof(context->logMonRowIndex) );
        break;

        case COLUMN_LOGMONROWLOGFILE:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonRowLogFile,
                         context->logMonRowLogFile_len );
        break;

        case COLUMN_LOGMONROWREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonRowReadInterval,
                         sizeof(context->logMonRowReadInterval) );
        break;

        case COLUMN_LOGMONROWFILTER:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonRowFilter,
                         context->logMonRowFilter_len );
        break;

        case COLUMN_LOGMONROWDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonRowDescription,
                         context->logMonRowDescription_len );
        break;

        case COLUMN_LOGMONROWCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonRowCreatedTime,
                         context->logMonRowCreatedTime_len );
        break;

        case COLUMN_LOGMONROWALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonRowAlertMessage,
                         context->logMonRowAlertMessage_len );
        break;

        case COLUMN_LOGMONROWSTATUS:
            /** RowStatus = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonRowStatus,
                         sizeof(context->logMonRowStatus) );
        break;

        case COLUMN_LOGMONROWUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonRowUpdatedTime,
                         context->logMonRowUpdatedTime_len );
        break;

        case COLUMN_LOGMONROWLINE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonRowLine,
                         sizeof(context->logMonRowLine) );
        break;

        case COLUMN_LOGMONROWGETLINE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonRowGetLine,
                         sizeof(context->logMonRowGetLine) );
        break;

        case COLUMN_LOGMONROWFILESIZE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonRowFileSize,
                         sizeof(context->logMonRowFileSize) );
        break;

        case COLUMN_LOGMONROWREADLINE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonRowReadLine,
                         sizeof(context->logMonRowReadLine) );
        break;

    default: /** We shouldn't get here */
        snmp_log(LOG_ERR, "unknown column in "
                 "logMonRowTable_get_value\n");
        return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}

/************************************************************
 * logMonRowTable_get_by_idx
 */
const logMonRowTable_context *
logMonRowTable_get_by_idx(netsnmp_index * hdr)
{
    return (const logMonRowTable_context *)
        CONTAINER_FIND(cb.container, hdr );
}

// Start: WIP_NURI_SOL : Start of the newly added code here

/*****************************************************************************
 *
 * name             :   LogMonRow_MasterThread
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
LogMonRow_MasterThread()
{
    int iIdx=0;
    pthread_t stLogMonThr[MAX_LOG_PTHREAD_NUM];
    logMonRowTable_context *pstLogRow=NULL;

    pstLogRow = CONTAINER_FIRST(cb.container);

    while(pstLogRow)
    {

        // Now, spawn thread for it
        if ( pthread_create(&stLogMonThr[iIdx], NULL,
                LogMonRow_LogMonThreadMain,
            (void*)pstLogRow) != 0 )
        {
            snmp_log_perror("Thread creation failed");
        }
        else
        {
            DEBUGMSGTL(("LogMonRow_MasterThread",
                "Master Thread started for < %u %d %s >\n",
                pstLogRow->idx,
                pstLogRow->index.len,
                pstLogRow->logMonRowLogFile));
            iIdx++;
        }

        pstLogRow = CONTAINER_NEXT(cb.container, pstLogRow);
    }

}



/*****************************************************************************
 *
 * name             :   LogMonRow_LogMonThreadMain
 * description      :   This function is the main function of the thread, which
 *                      will keep on running
 * input parameters :   void *pstLogRowData - the argument to the thread, which will be
 *                      logMonRowTable_context data
 * output parameters:   None
 * return type      :   void * - Nothing
 * global variables :   1) cb
 * calls            :   1) CONTAINER_FIND
 *                      2) LogMonRow_GetLineCount
 *                      3) LogMonRow_SendAlert
 */

void *
LogMonRow_LogMonThreadMain(void *pstLogRowData)
{

    char acModCode[]="LGMN"; // Module Code
    logMonRowTable_context *logMonRowRow;


    DEBUGMSGTL(("LogMonRow_LogMonThreadMain",
          "Thread spawned for OID <%u>\n",
          ((logMonRowTable_context*)pstLogRowData)->idx));

    // we need the index value, so do this
    logMonRowRow = (logMonRowTable_context*)pstLogRowData;

    // Keep looping infinitely
    while(1)
    {
        // Get a context to my row in the MIB container
        // Get the latest values of the data
        logMonRowRow = CONTAINER_FIND(cb.container,&(logMonRowRow->index));

        // If the row does not exist anymore
        if ( logMonRowRow == NULL )
        {
            DEBUGMSGTL(("LogMonRow_LogMonThreadMain",
                "Exiting thread <%d>\n",pthread_self()));
            pthread_detach(pthread_self());
            pthread_exit(NULL);  // exit the thread
        }

        // Do the value check, before proceeding
        if ( ( strlen(logMonRowRow->logMonRowLogFile) == 0 ) ||
            ( logMonRowRow->logMonRowReadInterval == 0 ) ||
            ( strlen(logMonRowRow->logMonRowFilter) == 0 ) )
        {
#ifndef WIN32
            sleep(3);
#else
            Sleep(3000);
#endif
            // sleep for 3 sec until full row creation.
            continue; // No need to process the request,
                     // until valid values are provided
        }

        DEBUGMSGTL(("LogMonRow_LogMonThreadMain",
            "Checking OID <%u> OID_LEN <%d> LOGFILE <%s>\n",
            *(logMonRowRow->index.oids),
            logMonRowRow->index.len,
            logMonRowRow->logMonRowLogFile));

        // Check for the patterns
        if( LogMonRow_SearchPatterns(logMonRowRow) == 1 )
        {
            if (LogMonRow_SendAlert(logMonRowRow->logMonRowAlertMessage,
                acModCode))
            {
                snmp_log(LOG_ERR,
                    "Alert sending failed for <%s>\n",
                    logMonRowRow->logMonRowLogFile);
            }
        }

        // Update the data file with the latest row data
        if ( LogMonRow_UpdateDataFile(logMonRowRow) == -1 )
        {
            snmp_log_perror("Error while updating MIB file");
        }



       DEBUGMSGTL(("LogMonRow_LogMonThreadMain",
            "Thread # %d # going to sleep for logfile <%s>\n",
            pthread_self(),
            logMonRowRow->logMonRowLogFile));


       // Sleep until the next interval
#if defined(WIN32)
       Sleep(logMonRowRow->logMonRowReadInterval*1000);
#else
       sleep(logMonRowRow->logMonRowReadInterval);
#endif

       DEBUGMSGTL(("LogMonRow_LogMonThreadMain",
        "Thread # %d # woke up for logfile <%s>\n",
        pthread_self(),
        logMonRowRow->logMonRowLogFile));

    } // End of infinite while loop

}



/*****************************************************************************
 *
 * name             :   LogMonRow_GetLineCount
 * description      :   This function is used to get the total number of lines
 *                      in a text file
 * input parameters :   char *pcFileName - text file name
 * output parameters:   None
 * return type      :   int - Line count in the file
 * global variables :   None
 * calls            :   None
 */

int
LogMonRow_GetLineCount(char *pcFileName)
{
    FILE *psFileP;    // File descriptor
    int iLineCount=0; // To hold the line count value
    char pcBuffer[LINE_MAX_LEN+1];        // To hold the read line buffer

    // Open the text file, in read mode
    if ( (psFileP = fopen(pcFileName,"r")) == NULL )
    {
        return (-1);
    }

    // Read until fgets returns error
    while(fgets(pcBuffer,LINE_MAX_LEN,psFileP)!=NULL)
    {
        // Increment line count
        iLineCount++;
    }

    // Close the file
    fclose(psFileP);

    // Return the line count
    return(iLineCount);
}



/*****************************************************************************
 *
 * name             :   LogMonRow_SendAlert
 * description      :   This function is used to send the UDP alert to the
 *                      manager
 * input parameters :   char *mesg - The alert message
 *                      char *mod_code - The name of the module, which is
 *                                       sending the alert
 * output parameters:   None
 * return type      :   int - returns success (0) or failure (1)
 * global variables :   None
 * calls            :   1) LogMonRow_GetLocalHostIp
 *                      2) LogMonRow_GetCurrLogTime
 *                      3) LogMonRow_SendLogMesg
 */

int
LogMonRow_SendAlert(char *mesg,char *mod_code)
{

#ifndef WIN32 // For Unix Only
    struct utsname stSys;
#else  // For Windows Only
    char* sys;
    int namelen = 50;
    WSADATA wsadata;
#endif
    // Common delcaration
    nuriAlertLGRow stAlert;
    char acSysName[MAXHOSTNAMELEN+1];


#ifndef WIN32 // For Unix Only
    // Get the localhost structure
    uname(&stSys);
    strcpy(acSysName, stSys.nodename);
#else // For Windows Only
    sys = (char*) malloc(namelen);
    WSAStartup(MAKEWORD( 2, 2 ),&wsadata);

    if (gethostname(sys,namelen)==0)
    strcpy(acSysName, sys);
#endif

    // Get the localhost Ip address
    LogMonRow_GetLocalHostIp(stAlert.masterIp);

    // Get the current date-time string
    LogMonRow_GetCurrLogTime(stAlert.dateTime);

    // prepare the alert structure
    strcpy(stAlert.modCode, mod_code);
    strcpy(stAlert.sysName,acSysName);
    strcpy(stAlert.alertMesg,mesg);

    // Now, send this alert mesg over UDP socket
    if ( LogMonRow_SendLogMesg(stAlert) )
    {
        snmp_log(LOG_ERR,"UDP mesg sending to manager failed\n");
        return(1);
    }

    return (0);
}



/*****************************************************************************
 *
 * name             :   LogMonRow_GetLocalHostIp
 * description      :   This function is used to get the IP address of the local
                        host machine
 * input parameters :   None
 * output parameters:   None
 * return type      :   char * - returns the IP Address string
 * global variables :   None
 * calls            :   None
 */

void
LogMonRow_GetLocalHostIp(char *pcSysIp)
{
    struct sockaddr_in stInAddr; // Common variable for Windows & Unix
    struct hostent *pstHostEnt;

#ifndef WIN32 // For Unix

    struct utsname stHostName;

    // Get the localhost name
    uname (&stHostName);

    if ((pstHostEnt = gethostbyname(stHostName.nodename)) == NULL)
    {
        snmp_log_perror("error resolving hostname..");
        return;
    }

#else  // For windows

    WSADATA wsadata;
    char* stHostName;
    int namelen = 50;

    stHostName = (char*) malloc(namelen);

    WSAStartup(MAKEWORD( 2, 2 ),&wsadata);

    if (gethostname(stHostName,namelen)==0)
    {
        if ((pstHostEnt = gethostbyname(stHostName)) == NULL)
        {
            snmp_log_perror("error resolving hostname..");
            return;
        }
    }

#endif

    // Common code for Windows & Unix
    memcpy(&stInAddr.sin_addr, pstHostEnt->h_addr_list[0], pstHostEnt->h_length);

    // Convert it to IP string
    strcpy(pcSysIp,(char*)inet_ntoa(stInAddr.sin_addr));

    DEBUGMSGTL(("LogMonRow_GetLocalHostIp",
        "Local Host IP <%s>\n",
        pcSysIp));

    return;

}

/*****************************************************************************
 *
 * name             :   LogMonRow_GetCurrLogTime
 * description      :   This function is used to get the current time in the
                        "YYYY:MON:DD:HH:MI:SS" format
 * input parameters :   char *pcCurrTime
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   None
 */


void
LogMonRow_GetCurrLogTime(char *pcCurrTime)
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

    DEBUGMSGTL(("LogMonRow_GetCurrLogTime",
        "Current Time <%s>\n",pcCurrTime));


#else // For windows

        SYSTEMTIME pstTimeStruct;

    GetLocalTime(&pstTimeStruct);

    pcCurrTime = (char*)malloc(sizeof(char)*19);

    sprintf(pcCurrTime,"%.2d:%.2d:%.2d:%.2d:%.2d:%.2d",
                       pstTimeStruct.wYear,
                       pstTimeStruct.wMonth ,
                       pstTimeStruct.wDay,
                       pstTimeStruct.wHour,
                       pstTimeStruct.wMinute,
                       pstTimeStruct.wSecond);
#endif

    DEBUGMSGTL(("LogMonRow_GetCurrLogTime",
        "Current Time <%s>\n",
        pcCurrTime));

    return;

}

/*****************************************************************************
 *
 * name             :   LogMonRow_SendLogMesg
 * description      :   This function is used to send the UDP alert message to
 *                       the manager
 * input parameters :   nuriAlertLGRow stMesg - Alert message structure
 * output parameters:   None
 * return type      :   int - return status ( 1 for failure, 0 for success )
 * global variables :   1) gcAgentInfoAlertMgrIp
 *                       2) gcAgentInfoAlertMgrPort
 * calls            :   1) getIPFromASNFormat
 */

int
LogMonRow_SendLogMesg(nuriAlertLGRow stMesg)
{

    int iSock;
    int iBufLen;
    char aBuffer[16];
    char *pcAlertMesg;
    struct sockaddr_in stSrvName;
    struct hostent *pstHost;

    pcAlertMesg = (char *) calloc(sizeof(nuriAlertLGRow) + 5, sizeof(char));

    // Form the alert message
    sprintf(pcAlertMesg,"%s|%s|%s|%s|%s",
                                  stMesg.modCode,
                                  stMesg.masterIp,
                                  stMesg.dateTime,
                                  stMesg.sysName,
                                  stMesg.alertMesg);

    // create socket for connecting to server
    iSock = socket(AF_INET, SOCK_DGRAM,0);

    if(iSock < 0)
    {
        snmp_log_perror("opening datagram socket");
        free(pcAlertMesg);
        return(1);
    }

    // construct name for connecting to server
    stSrvName.sin_family = AF_INET;
    stSrvName.sin_port = htons(glAgentInfoAlertMgrPort);

    // Change the Ip from ASN format to character string
    getIPFromASNFormat(gcAgentInfoAlertMgrIp, aBuffer);

    // convert hostname to IP address and enter into name
    stSrvName.sin_addr.s_addr = inet_addr(aBuffer);

    // send message to server
    iBufLen = sizeof(nuriAlertLGRow) +5;

    if(sendto(iSock, (const void*)pcAlertMesg, iBufLen, 0,
                          (struct sockaddr *)&stSrvName, sizeof(stSrvName)) <0)
    {
        snmp_log_perror("Error sending datagram message");
        free(pcAlertMesg);
        return(1);
    }
    else
        DEBUGMSGTL(("LogMonRow_SendLogMesg",
            "Udp Alert Message <%s> \
            Sent to Manager IP <%s> \
            Port <%d> \n",
            pcAlertMesg,
            aBuffer,
            glAgentInfoAlertMgrPort));

    // close connection
    free(pcAlertMesg);

    close(iSock);

    return(0);

}




/*****************************************************************************
 *
 * name             :   LogMonRow_LoadContainerFromFile
 * description      :   This function is used to load the container with the
 *                      default values which were present when the last time
 *                      agent was up and running.
 * input parameters :   None
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   1) getIPFromASNFormat
 */

void
LogMonRow_LoadContainerFromFile()
{
    int iFd=0;
    int iFdTmp=0;
    int iCnt=0;
    int iRewriteFlag=0;
    logMonRowTable_context stReadCtx = {0};
    logMonRowTable_context *pstMyContext = NULL;

    iFd = open (LOG_MON_ROW_DATA_FILE, O_RDONLY,S_IREAD|S_IWRITE);

    if (iFd == -1)
    {
         snmp_log_perror
            ("LogMonRow_LoadContainerFromFile: \
                LOG_MON_ROW_DATA_FILE file open failed\n");
    }
    else
    {
        while (read (iFd, &stReadCtx, sizeof (logMonRowTable_context)) > 0)
        {
            // If the row, is already destroyed, no need to create it again.
            if ( stReadCtx.logMonRowStatus == SNMP_ROW_DESTROY )
            {
                 iRewriteFlag = 1;   // to re-write the mib data file
                 continue; // Don't increment the row_count also
            }

            DEBUGMSGTL(("LogMonRow_LoadContainerFromFile",
                "Loading MIB Row for Log File <%s>\n",
                stReadCtx.logMonRowLogFile));

            pstMyContext = (logMonRowTable_context*)malloc(sizeof (logMonRowTable_context));

            memcpy (pstMyContext, &stReadCtx, sizeof (logMonRowTable_context));

            netsnmp_assert(pstMyContext->index.oids == NULL);

            if(snmp_clone_mem( (void*)&pstMyContext->index.oids, &(stReadCtx.idx),
                            stReadCtx.index.len * sizeof(oid) ))
            {
                free(pstMyContext);
                return;
            }

            pstMyContext->index.len = stReadCtx.index.len;

            iCnt = CONTAINER_INSERT (cb.container, pstMyContext);

            if (iCnt)
            {
                DEBUGMSGTL(("LogMonRow_LoadContainerFromFile",
                    "ERROR IN CONTAINER_INSERT FUNCION\n"));
            }

            DEBUGMSGTL((" Load_Container_From_LogMON_File",
              "OID <%u>\n",*(pstMyContext->index.oids)));
        }

        DEBUGMSGTL(("LogMonRow_LoadContainerFromFile",
                "CONTAINER SIZE=%u\n", CONTAINER_SIZE (cb.container)));

        // Close the file

        close (iFd);

        if ( iRewriteFlag )
        {
            if ( ( iFd = open (LOG_MON_ROW_DATA_FILE, O_RDONLY,S_IREAD) ) == -1 )
            {
                snmp_log_perror("LOG_MON_ROW_DATA_FILE file open failed\n");
            }
            else
            {
                // Open a temp file
                if ( ( iFd = open (LOG_MON_ROW_DATA_FILE_TMP,
                                       O_CREAT|O_RDWR,S_IREAD|S_IWRITE) ) == -1 )
                {
                    snmp_log_perror(
                           "LOG_MON_ROW_DATA_FILE_TMP file open failed\n");
                }
                else
                {
                    // Reset the file pointer to begining of the file
                    lseek(iFd, 0,SEEK_SET);
                    lseek(iFdTmp, 0,SEEK_SET);

                    while (read (iFd, &stReadCtx,
                                       sizeof (logMonRowTable_context)) > 0)
                    {
                        if ( stReadCtx.logMonRowStatus == SNMP_ROW_ACTIVE )
                            if ( write(iFdTmp,&stReadCtx,
                                  sizeof(logMonRowTable_context)) < 0 )
                            {
                                snmp_log_perror(
                                 "Error writing data to PROC_MON_DATA_FILE");
                            }

                    } // End of while

                    // Close the file
                    close(iFd);
                    close(iFdTmp);

                    // Delete the old data file
                    unlink(LOG_MON_ROW_DATA_FILE);

                    // Rename the temp file to original
                    rename(LOG_MON_ROW_DATA_FILE_TMP,LOG_MON_ROW_DATA_FILE);

                } // End of else

            } // End of else

        } // End of rewrite if

    } // End of else

}


/*****************************************************************************
 *
 * name             :   LogMonRow_SearchPatterns
 * description      :   This function is used to search the patterns in the
 *                      input file
 * input parameters :   logMonRowTable_context *logMonRowRow
 * output parameters:   None
 * return type      :   int - 1 for pattern found, 0 for not found, -1 for error
 * global variables :   None
 * calls            :   LogMonRow_GetCurrLogTime
 */

int
LogMonRow_SearchPatterns(logMonRowTable_context *logMonRowRow)
{

    FILE *pfOutFd;         // Output file descriptor
    FILE *psInFd;          // Input file descriptor
    char acReadBuffer[LINE_MAX_LEN+1]; // To hold the line read
    struct stat *pstStatBuf;
    time_t now;
    int iStrFound=0;
    int iLineNum;        // Current line number
    int iLidx;

    // Open the input log file
    if( (psInFd = fopen(logMonRowRow->logMonRowLogFile,"r") ) == NULL)
    {
       // Error opening input log file
       DEBUGMSGTL(("LogMonRow_LogMonThreadMain",
                   "Error opening file <%s> <%d>\n",
                   logMonRowRow->logMonRowLogFile,errno));

       return (-1);
    }

    // Find the input log file size and check for error
    pstStatBuf = (struct stat *)malloc(sizeof(struct stat));

    if(stat(logMonRowRow->logMonRowLogFile,pstStatBuf)==-1)
    {

        fclose(psInFd);
        free(pstStatBuf);

        DEBUGMSGTL(("LogMonRow_LogMonThreadMain",
                    "Error opening logMonRowLogFile"));

#ifndef WIN32
        sleep(3);
#else
        Sleep(3000);
#endif

        return (-1);
    }

    // Open the output log file
    if( (pfOutFd = fopen(gcInfoLogFileMonRow,"a") ) == NULL)
    {

       // Error opening output log file
       fclose(psInFd);

       DEBUGMSGTL(("LogMonRow_LogMonThreadMain",
                    "Error opening Output Log File <%s>\n",
                    gcInfoLogFileMonRow));
    }


    // Set the file pointer to the matched line, last time
    iLineNum = logMonRowRow->logMonRowLine;

    // Skip all the lines read already
    for(iLidx=1;iLidx<=iLineNum;iLidx++)
    {
       if ( fgets(acReadBuffer,LINE_MAX_LEN,psInFd) == NULL )
           break;
    }


    // Read the Log file and find the pattern
    do
    {
       if(fgets(acReadBuffer,LINE_MAX_LEN,psInFd) == NULL)
       {
           // End of File reached or File read error
           // Free the memory and close files

           fclose(psInFd);
           fclose(pfOutFd);
           break;
       }
       else
       {
           iLineNum++;

           // Check if the match is present in
           // the current line read
           if(strstr(acReadBuffer,logMonRowRow->logMonRowFilter)!=NULL)
           {
               // No error opening output log file
               if(pfOutFd != NULL)
                   // Write the found details in the
                   // Outout Log file
                   fprintf(pfOutFd,"\n %d | %s | %s",
                              iLineNum,
                              logMonRowRow->logMonRowFilter,
                              acReadBuffer);

               // Free the memory and close files
               fclose(psInFd);
               fclose(pfOutFd);

               // Set the string found flag
               iStrFound = 1;

               break;
           }

       }

    } while(1); // Until we find a match, or
               // End-Of-File is reached

    // Now, update the container with the latest results
    logMonRowRow->logMonRowFileSize=pstStatBuf->st_size;

    // Row Modified Time
    logMonRowRow->logMonRowUpdatedTime_len=MAX_DATE_TIME_LEN;

    (void *) time(&now);

    strcpy(logMonRowRow->logMonRowUpdatedTime,
        (u_char *) date_n_time(&now,
        (size_t*)&(logMonRowRow->logMonRowUpdatedTime_len)));

    // Update the line number
    logMonRowRow->logMonRowGetLine =
          LogMonRow_GetLineCount(logMonRowRow->logMonRowLogFile);


    // Last line read
    logMonRowRow->logMonRowReadLine = iLineNum;

    // Line that matched
    if ( iStrFound )
       logMonRowRow->logMonRowLine = iLineNum;

    // Free the buffer
    free(pstStatBuf);

    // send an alert
    if( iStrFound )
    {
       if (LogMonRow_SendAlert(logMonRowRow->logMonRowAlertMessage,
                                            (char *)"LGMN"))
       {
           snmp_log(LOG_ERR,
                   "Alert sending failed for <%s>\n",
                   logMonRowRow->logMonRowLogFile);
       }
    }

    return (iStrFound);
}


/*****************************************************************************
 *
 * name             :   LogMonRow_UpdateDataFile
 * description      :   This function is used to update the MIB data file with
 *                      the latest record
 * input parameters :   logMonRowTable_context *pstLogRowRow
 * output parameters:   None
 * return type      :   int - "0" for success in writing new record,
 *                          - "-1" for error,
 *                          - "1" for success in modifying existing record
 * global variables :   None
 * calls            :   None
 */


int
LogMonRow_UpdateDataFile(logMonRowTable_context *pstLogRowRow)
{
    logMonRowTable_context stLogRowInFile;
    int iFd=-1;
    int iFoundFlag=0;
    int iRowNum=0;
    unsigned long ulPos=0;


    // Open the MIB data file
    if ( (iFd = open(LOG_MON_ROW_DATA_FILE,O_CREAT|O_RDWR,S_IREAD|S_IWRITE)) < 0 )
    {
        snmp_log_perror("Error opening LOG_MON_ROW_DATA_FILE");
        return(-1);
    }
    else
    {

        lseek(iFd, 0,SEEK_SET);

        while( read(iFd, &stLogRowInFile, sizeof(logMonRowTable_context)) > 0 )
        {
            if ( stLogRowInFile.idx == *pstLogRowRow->index.oids )
            {

                // Check if the row entry in the file
                // is actually deleted fromthe container
                if ( stLogRowInFile.logMonRowStatus == SNMP_ROW_DESTROY )
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
                    DEBUGMSGTL(("LogMonRow_UpdateDataFile",
                        "OID <%u> found FoundFlag <%d>\n",
                            *pstLogRowRow->index.oids,
                            iFoundFlag));

                }
                break;
            }

            ulPos = lseek(iFd, 0, SEEK_CUR);
        }

        // Copy the oid, instead copying the pointer
        pstLogRowRow->idx = *(pstLogRowRow->index.oids);

        if ( write(iFd,pstLogRowRow,sizeof(logMonRowTable_context)) < 0 )
        {
            snmp_log_perror("Error while writing data to LOG_MON_ROW_DATA_FILE");
            return(-1);
        }
        else
            DEBUGMSGTL(("LogMonRow_UpdateDataFile",
                "Record written\n"));
    }
    close(iFd);

    return(iFoundFlag);
}


// End: WIP_NURI_SOL : End of the newly added code here

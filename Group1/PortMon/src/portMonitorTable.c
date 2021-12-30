/*****************************************************************************
 *
 * portMonitorTable.c
 *
 *      Copyright (c) 2009 NURI Telecom Co., LTD. 
 *      All rights Reserved
 *
 * author:
 *      SungHae Kim 
 *
 * description :
 *      This file is a C source code for port status monitoring
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
#include "../inc/portMonitorTable.h"
#include <tchar.h>
#else
#include "psMonitorTable.h"
#include "portMonitorTable.h"
#endif

#ifdef linux
struct inpcb {
    struct inpcb   *inp_next;   /* pointers to other pcb's */
    struct in_addr  inp_faddr;  /* foreign host table entry */
    u_short         inp_fport;  /* foreign port */
    struct in_addr  inp_laddr;  /* local host table entry */
    u_short         inp_lport;  /* local port */
    int             inp_state;
    int             uid;        /* owner of the connection */
};
#endif

#define FCLOSE(x) fclose(x);\
                  x=NULL

#define CHECK_INTERVAL	10 // seconds
#define MAX(x,y) ((x)>(y)?(x):(y))

static int full_port_list = 0;

static port_cache_info *PortList = NULL;
static port_cache_info *PrePortList = NULL;

static netsnmp_handler_registration *my_handler = NULL;
static netsnmp_table_array_callbacks cb;

static oid objid_sysuptime[] = { 1, 3, 6, 1, 2, 1, 1, 3, 0 };
static oid objid_snmptrap[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };

oid portMonitorTable_oid[] = { portMonitorTable_TABLE_OID };
size_t portMonitorTable_oid_len = OID_LENGTH(portMonitorTable_oid);


#ifdef portMonitorTable_IDX2
/************************************************************
 * keep binary tree to find context by name
 */
static int portMonitorTable_cmp(const void *lhs, const void *rhs);

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int
portMonitorTable_cmp( const void *lhs, const void *rhs )
{
    portMonitorTable_context *context_l = (portMonitorTable_context*)lhs;
    portMonitorTable_context *context_r = (portMonitorTable_context*)rhs;

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
        "portMonitorTable_cmp not implemented! Container order undefined\n" );
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
portMonitorTable_context*
portMonitorTable_get(const char *name, int len)
{
    portMonitorTable_context tmp;

    /** we should have a secondary index */
    netsnmp_assert(cb.container->next != NULL);

    /*
     * TODO: implement compare. Remove this ifdef code and
     * add your own code here.
     */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "portMonitorTable_get not implemented!\n" );
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
static int portMonitorTable_row_copy(portMonitorTable_context* dst,
                         portMonitorTable_context* src)
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
    dst->portMonitorIndex = src->portMonitorIndex;

    dst->portMonitorNumber = src->portMonitorNumber;

    memcpy(dst->portMonitorType, 
			src->portMonitorType, 
			src->portMonitorType_len);
    dst->portMonitorType_len = src->portMonitorType_len;

    memcpy(dst->portMonitorState, 
			src->portMonitorState, 
			src->portMonitorState_len );
    dst->portMonitorState_len = src->portMonitorState_len;

    dst->portMonitorCheckInterval = src->portMonitorCheckInterval;

    memcpy(dst->portMonitorDescription, 
			src->portMonitorDescription, 
			src->portMonitorDescription_len );
    dst->portMonitorDescription_len = src->portMonitorDescription_len;

    memcpy(dst->portMonitorCreatedDateTime, 
			src->portMonitorCreatedDateTime, 
			src->portMonitorCreatedDateTime_len );
    dst->portMonitorCreatedDateTime_len = src->portMonitorCreatedDateTime_len;

    dst->portMonitorReaction = src->portMonitorReaction;

    memcpy(dst->portMonitorReactionPath, 
			src->portMonitorReactionPath, 
			src->portMonitorReactionPath_len );
    dst->portMonitorReactionPath_len = src->portMonitorReactionPath_len;

    memcpy(dst->portMonitorAlertMessage, 
			src->portMonitorAlertMessage, 
			src->portMonitorAlertMessage_len );
    dst->portMonitorAlertMessage_len = src->portMonitorAlertMessage_len;

    dst->portMonitorStatus = src->portMonitorStatus;

    return 0;
}

#ifdef portMonitorTable_SET_HANDLING

/*
 * the *_extract_index routine
 */
int portMonitorTable_extract_index(portMonitorTable_context* ctx, 
			netsnmp_index* hdr)
{
    /*
     * temporary local storage for extracting oid index
     */
    netsnmp_variable_list var_portMonitorIndex;
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
       memset( &var_portMonitorIndex, 0x00, sizeof(var_portMonitorIndex) );
       var_portMonitorIndex.type = ASN_UNSIGNED;

       /** TODO: link this index to the next, or NULL for the last one */

#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "portMonitorTable_extract_index index list not implemented!\n" );
    return 0;
#endif

    /*
     * parse the oid into the individual components
     */
    err = parse_oid_indexes( hdr->oids, hdr->len, &var_portMonitorIndex );
    if (err == SNMP_ERR_NOERROR) {
       /*
        * copy components into the context structure
        */
		ctx->portMonitorIndex = *var_portMonitorIndex.val.integer;

           /*
            * TODO: check index for valid values. For EXAMPLE:
            *
              * if ( *var_portMonitorIndex.val.integer != XXX ) {
          *    err = -1;
          * }
          */
    }

    /*
     * parsing may have allocated memory. free it.
     */
    snmp_reset_var_buffers(&var_portMonitorIndex);

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
int portMonitorTable_can_activate(portMonitorTable_context* undo_ctx,
                      portMonitorTable_context* row_ctx,
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
int portMonitorTable_can_deactivate(portMonitorTable_context* undo_ctx,
                        portMonitorTable_context* row_ctx,
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
int portMonitorTable_can_delete(portMonitorTable_context* undo_ctx,
                    portMonitorTable_context* row_ctx,
                    netsnmp_request_group* rg)
{
    /*
     * probably shouldn't delete a row that we can't
     * deactivate.
     */
    if(portMonitorTable_can_deactivate(undo_ctx, row_ctx, rg) != 1)
        return 0;

    /*
     * TODO: check for other deletion requirements here
     */
    return 1;
}

#ifdef portMonitorTable_ROW_CREATION
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
 * returns a newly allocated portMonitorTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
portMonitorTable_context* portMonitorTable_create_row(netsnmp_index* hdr)
{
    portMonitorTable_context* ctx =
        SNMP_MALLOC_TYPEDEF(portMonitorTable_context);

    if(ctx == NULL)
        return NULL;

    /*
     * TODO: check indexes, if necessary.
     */
    if(portMonitorTable_extract_index(ctx, hdr)) {
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
    ctx->portMonitorCheckInterval = 30;

    return ctx;
}
#endif

/************************************************************
 * the *_duplicate row routine
 */
portMonitorTable_context*
portMonitorTable_duplicate_row(portMonitorTable_context* row_ctx)
{
    portMonitorTable_context * dup;

    if(!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(portMonitorTable_context);
    if(!dup)
        return NULL;

    if(portMonitorTable_row_copy(dup, row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index* portMonitorTable_delete_row(portMonitorTable_context* ctx)
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
void portMonitorTable_set_reserve1( netsnmp_request_group *rg )
{
    portMonitorTable_context* row_ctx =
            (portMonitorTable_context *)rg->existing_row;
    portMonitorTable_context* undo_ctx =
            (portMonitorTable_context *)rg->undo_info;
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

        case COLUMN_PORTMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
            rc = netsnmp_check_vb_type_and_size(var, ASN_UNSIGNED, 
                    sizeof(row_ctx->portMonitorIndex));
        break;

        case COLUMN_PORTMONITORNUMBER:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->portMonitorNumber));
        break;

        case COLUMN_PORTMONITORTYPE:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    var->val_len);
        break;

        case COLUMN_PORTMONITORSTATE:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    var->val_len);
        break;

        case COLUMN_PORTMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->portMonitorCheckInterval));
        break;

        case COLUMN_PORTMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    var->val_len);
        break;

        case COLUMN_PORTMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    var->val_len);
        break;

        case COLUMN_PORTMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->portMonitorReaction));
        break;

        case COLUMN_PORTMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    var->val_len);
        break;

        case COLUMN_PORTMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
         		   var->val_len);
        break;

        case COLUMN_PORTMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->portMonitorStatus));
        break;

        default: /** We shouldn't get here */
            rc = SNMP_ERR_GENERR;
            snmp_log(LOG_ERR, "unknown column in "
                     "portMonitorTable_set_reserve1\n");
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

void portMonitorTable_set_reserve2(netsnmp_request_group *rg)
{
    portMonitorTable_context* row_ctx = 
		(portMonitorTable_context *)rg->existing_row;
    portMonitorTable_context* undo_ctx = 
		(portMonitorTable_context *)rg->undo_info;
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

        case COLUMN_PORTMONITORINDEX:
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

        case COLUMN_PORTMONITORNUMBER:
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

        case COLUMN_PORTMONITORTYPE:
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

        case COLUMN_PORTMONITORSTATE:
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

        case COLUMN_PORTMONITORCHECKINTERVAL:
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

        case COLUMN_PORTMONITORDESCRIPTION:
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

        case COLUMN_PORTMONITORCREATEDDATETIME:
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

        case COLUMN_PORTMONITORREACTION:
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

        case COLUMN_PORTMONITORREACTIONPATH:
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

        case COLUMN_PORTMONITORALERTMESSAGE:
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

        case COLUMN_PORTMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
            rc = netsnmp_check_vb_rowstatus(current->ri->requestvb,
                           undo_ctx ? undo_ctx->portMonitorStatus : 0);
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
void portMonitorTable_set_action(netsnmp_request_group* rg)
{
    portMonitorTable_context* row_ctx = 
		(portMonitorTable_context*)rg->existing_row;
    portMonitorTable_context* undo_ctx = 
		(portMonitorTable_context*)rg->undo_info;
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

        case COLUMN_PORTMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
            row_ctx->portMonitorIndex = *var->val.integer;
        break;

        case COLUMN_PORTMONITORNUMBER:
            /** INTEGER = ASN_INTEGER */
            row_ctx->portMonitorNumber = *var->val.integer;
        break;

        case COLUMN_PORTMONITORTYPE:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->portMonitorType, var->val.string, var->val_len);
            row_ctx->portMonitorType_len = var->val_len;
            row_ctx->portMonitorType[var->val_len] = 0;
        break;

        case COLUMN_PORTMONITORSTATE:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->portMonitorState, var->val.string, var->val_len);
            row_ctx->portMonitorState_len = var->val_len;
            row_ctx->portMonitorState[var->val_len] = 0;
        break;

        case COLUMN_PORTMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
            row_ctx->portMonitorCheckInterval = *var->val.integer;
        break;

        case COLUMN_PORTMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->portMonitorDescription, var->val.string, var->val_len);
            row_ctx->portMonitorDescription_len = var->val_len;
            row_ctx->portMonitorDescription[var->val_len]=0;
        break;

        case COLUMN_PORTMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->portMonitorCreatedDateTime, var->val.string, var->val_len);
            row_ctx->portMonitorCreatedDateTime_len = var->val_len;
            row_ctx->portMonitorCreatedDateTime[var->val_len]=0;
        break;

        case COLUMN_PORTMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
            row_ctx->portMonitorReaction = *var->val.integer;
        break;

        case COLUMN_PORTMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->portMonitorReactionPath, var->val.string, var->val_len);
            row_ctx->portMonitorReactionPath_len = var->val_len;
            row_ctx->portMonitorReactionPath[var->val_len]=0;
        break;

        case COLUMN_PORTMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->portMonitorAlertMessage, var->val.string, var->val_len);
            row_ctx->portMonitorAlertMessage_len = var->val_len;
            row_ctx->portMonitorAlertMessage[var->val_len]=0;
        break;

        case COLUMN_PORTMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
            row_ctx->portMonitorStatus = *var->val.integer;
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
#ifndef portMonitorTable_CAN_MODIFY_ACTIVE_ROW
    if (undo_ctx && RS_IS_ACTIVE(undo_ctx->portMonitorStatus) &&
        row_ctx && RS_IS_ACTIVE(row_ctx->portMonitorStatus)) {
        row_err = 1;
    }
#endif

    /*
     * check activation/deactivation
     */
    row_err = netsnmp_table_array_check_row_status(&cb, rg,
                      row_ctx ? &row_ctx->portMonitorStatus : NULL,
                      undo_ctx ? &undo_ctx->portMonitorStatus : NULL);
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
void portMonitorTable_set_commit(netsnmp_request_group* rg)
{
    portMonitorTable_context* row_ctx = 
		(portMonitorTable_context *)rg->existing_row;
    portMonitorTable_context *undo_ctx = 
		(portMonitorTable_context *)rg->undo_info;
    netsnmp_variable_list* var;
    netsnmp_request_group_item* current;
    pthread_t pt;
    time_t now;

	DEBUGMSGTL(("PortMon/portMonitorTable_set_commit",
		"Commit is being called\n"));

    if (PortMon_UpdateDataFile(row_ctx) == 0) {
        DEBUGMSGTL(("PortMon/portMonitorTable_set_commit",
            "Row updated in the PORT_MON_DATA_FILE\n"));
    }

    // Do the value check, before proceeding
    if ((strlen((char*)row_ctx->portMonitorType) != 0) &&
		(row_ctx->portMonitorNumber != 0) &&
        (row_ctx->portMonitorCheckInterval != 0) &&
        (row_ctx->portMonitorStatus == SNMP_ROW_ACTIVE )) {

        // Row Created/ Changed Time
        row_ctx->portMonitorCreatedDateTime_len = MAX_DATE_TIME_LEN;

        // Get the Date-Time in time_t format
        // and update the created time for the newly created row
        (void *)time(&now);

        strcpy(row_ctx->portMonitorCreatedDateTime,
               (u_char *)date_n_time(&now,
               (size_t*)&(row_ctx->portMonitorCreatedDateTime_len)));

		// shkim 2009.06.28
		// Check port state

        // i.e. if the thread has not been created yet
        if (row_ctx->threadActive == 0) {
            if (pthread_create(&pt, NULL, 
					PortCheckThread, (void*)row_ctx) != 0) {
                snmp_log_perror("Thread creation failed");
            } else {
                // Change it to active
                row_ctx->threadActive = 1;

                DEBUGMSGTL(("PortMon/portMonitorTable_set_commit",
                    "Thread started for < %u %d %d %s >\n",
                    row_ctx->idx,
                    row_ctx->index.len,
					row_ctx->portMonitorNumber,
                    row_ctx->portMonitorType));
            }
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
void portMonitorTable_set_free(netsnmp_request_group* rg)
{
    portMonitorTable_context* row_ctx = 
		(portMonitorTable_context *)rg->existing_row;
    portMonitorTable_context* undo_ctx = 
		(portMonitorTable_context *)rg->undo_info;
    netsnmp_variable_list* var;
    netsnmp_request_group_item* current;

    /*
     * loop through columns
     */
    for (current = rg->list; current; current = current->next) {

        var = current->ri->requestvb;

        switch (current->tri->colnum) {

        case COLUMN_PORTMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
        break;

        case COLUMN_PORTMONITORNUMBER:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PORTMONITORTYPE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PORTMONITORSTATE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PORTMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PORTMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PORTMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_PORTMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PORTMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PORTMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PORTMONITORSTATUS:
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
void portMonitorTable_set_undo(netsnmp_request_group *rg)
{
    portMonitorTable_context* row_ctx = 
		(portMonitorTable_context *)rg->existing_row;
    portMonitorTable_context* undo_ctx = 
		(portMonitorTable_context *)rg->undo_info;
    netsnmp_variable_list* var;
    netsnmp_request_group_item* current;

    /*
     * loop through columns
     */
    for (current = rg->list; current; current = current->next) {

        var = current->ri->requestvb;

        switch (current->tri->colnum) {

        case COLUMN_PORTMONITORINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
        break;

        case COLUMN_PORTMONITORNUMBER:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PORTMONITORTYPE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PORTMONITORSTATE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PORTMONITORCHECKINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PORTMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PORTMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_PORTMONITORREACTION:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PORTMONITORREACTIONPATH:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PORTMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_PORTMONITORSTATUS:
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

#endif /** portMonitorTable_SET_HANDLING */


/************************************************************
 *
 * Initialize the portMonitorTable table by defining 
 *
 * its contents and how it's structured
 */
void initialize_table_portMonitorTable(void)
{
    netsnmp_table_registration_info* table_info;

    if (my_handler) {
        snmp_log(LOG_ERR, "initialize_table_portMonitorTable_handler called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /* if your table is read only, it's easiest to change the
       HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY */
    my_handler = netsnmp_create_handler_registration("portMonitorTable",
                                  netsnmp_table_array_helper_handler,
                                  portMonitorTable_oid,
                                  portMonitorTable_oid_len,
                                  HANDLER_CAN_RWRITE);

    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_portMonitorTable_handler\n");
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

	/** index: portMonitorIndex */
	netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

    table_info->min_column = portMonitorTable_COL_MIN;
    table_info->max_column = portMonitorTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = portMonitorTable_get_value;
    cb.container = netsnmp_container_find("portMonitorTable_primary:"
                                          "portMonitorTable:"
                                          "table_container");
#ifdef portMonitorTable_IDX2
    netsnmp_container_add_index(cb.container,
            netsnmp_container_find("portMonitorTable_secondary:"
                                   "portMonitorTable:"
                                   "table_container"));
    cb.container->next->compare = portMonitorTable_cmp;
#endif

#ifdef portMonitorTable_SET_HANDLING
    cb.can_set = 1;
#ifdef portMonitorTable_ROW_CREATION
    cb.create_row = (UserRowMethod*)portMonitorTable_create_row;
#endif
    cb.duplicate_row = (UserRowMethod*)portMonitorTable_duplicate_row;
    cb.delete_row = (UserRowMethod*)portMonitorTable_delete_row;
    cb.row_copy = (Netsnmp_User_Row_Operation *)portMonitorTable_row_copy;

    cb.can_activate = (Netsnmp_User_Row_Action *)portMonitorTable_can_activate;
    cb.can_deactivate = (Netsnmp_User_Row_Action *)portMonitorTable_can_deactivate;
    cb.can_delete = (Netsnmp_User_Row_Action *)portMonitorTable_can_delete;

    cb.set_reserve1 = portMonitorTable_set_reserve1;
    cb.set_reserve2 = portMonitorTable_set_reserve2;
    cb.set_action = portMonitorTable_set_action;
    cb.set_commit = portMonitorTable_set_commit;
    cb.set_free = portMonitorTable_set_free;
    cb.set_undo = portMonitorTable_set_undo;
#endif

    DEBUGMSGTL(("PortMon/initialize_table_portMonitorTable",
                "Registering table portMonitorTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);
}

/************************************************************
 * portMonitorTable_get_value
 */
int portMonitorTable_get_value(
            netsnmp_request_info* request,
            netsnmp_index* item,
            netsnmp_table_request_info* table_info)
{
    netsnmp_variable_list* var = request->requestvb;
    portMonitorTable_context* context = (portMonitorTable_context *)item;

    switch (table_info->colnum) {

    case COLUMN_PORTMONITORINDEX:
        /** UNSIGNED32 = ASN_UNSIGNED */
        snmp_set_var_typed_value(var, ASN_UNSIGNED,
                     (char*)&context->portMonitorIndex,
                     sizeof(context->portMonitorIndex));
    break;

    case COLUMN_PORTMONITORNUMBER:
        /** INTEGER = ASN_INTEGER */
        snmp_set_var_typed_value(var, ASN_INTEGER,
                     (char*)&context->portMonitorNumber,
                     sizeof(context->portMonitorNumber));
    break;

    case COLUMN_PORTMONITORTYPE:
        /** DisplayString = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                     (char*)&context->portMonitorType,
                     context->portMonitorType_len);
    break;

    case COLUMN_PORTMONITORSTATE:
        /** DisplayString = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                     (char*)&context->portMonitorState,
                     context->portMonitorState_len);
    break;

    case COLUMN_PORTMONITORCHECKINTERVAL:
        /** INTEGER = ASN_INTEGER */
        snmp_set_var_typed_value(var, ASN_INTEGER,
                     (char*)&context->portMonitorCheckInterval,
                     sizeof(context->portMonitorCheckInterval));
    break;

    case COLUMN_PORTMONITORDESCRIPTION:
        /** DisplayString = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                 (char*)&context->portMonitorDescription,
                     context->portMonitorDescription_len);
    break;

    case COLUMN_PORTMONITORCREATEDDATETIME:
        /** DateAndTime = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                     (char*)&context->portMonitorCreatedDateTime,
                     context->portMonitorCreatedDateTime_len );
    break;

    case COLUMN_PORTMONITORREACTION:
        /** INTEGER = ASN_INTEGER */
        snmp_set_var_typed_value(var, ASN_INTEGER,
                     (char*)&context->portMonitorReaction,
                     sizeof(context->portMonitorReaction));
    break;

    case COLUMN_PORTMONITORREACTIONPATH:
        /** DisplayString = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                     (char*)&context->portMonitorReactionPath,
                     context->portMonitorReactionPath_len);
    break;

    case COLUMN_PORTMONITORALERTMESSAGE:
        /** DisplayString = ASN_OCTET_STR */
        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                     (char*)&context->portMonitorAlertMessage,
                     context->portMonitorAlertMessage_len);
    break;

    case COLUMN_PORTMONITORSTATUS:
        /** RowStatus = ASN_INTEGER */
        snmp_set_var_typed_value(var, ASN_INTEGER,
                     (char*)&context->portMonitorStatus,
                     sizeof(context->portMonitorStatus));
    break;

    default: /** We shouldn't get here */
        snmp_log(LOG_ERR, "unknown column in "
                 "portMonitorTable_get_value\n");
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

/************************************************************
 * portMonitorTable_get_by_idx
 */
const portMonitorTable_context*
portMonitorTable_get_by_idx(netsnmp_index* hdr)
{
    return (const portMonitorTable_context *)
        CONTAINER_FIND(cb.container, hdr);
}

/*****************************************************************************
 *
 * name             :   PortMon_MasterThread
 * description      :   This is the master thread function which gets called
 *                      at the initialization of table
 * input parameters :   None
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   None
 */

void PortMon_MasterThread()
{
    pthread_t portMonThread;
	pthread_t portListThread;
    portMonitorTable_context* portRow = NULL;

	if (pthread_create(&portListThread, NULL, PortListThread, NULL) != 0) {
		snmp_log_perror("Starting port list thread failed\n");
	} else {
		DEBUGMSGTL(("PortMon/PortMon_MasterThread",
			"Thr #%d# Port list checking thread started\n", MY_THR_ID));
	}

    // Get the first row in the container
    portRow = CONTAINER_FIRST(cb.container);

    // As long as we get valid rows

    while (portRow) {

        // Do the value check, before proceeding

        if ((strlen((char*)portRow->portMonitorType) != 0 ) &&
			(portRow->portMonitorNumber != 0) &&
            (portRow->portMonitorCheckInterval != 0 )) {

            // Now, spawn thread for it

            if (pthread_create(&portMonThread, NULL,
                    PortCheckThread, (void*)portRow) != 0 ) {
                snmp_log_perror("Thread creation failed");
            } else {
                // Change it to active
                portRow->threadActive = 1;

                DEBUGMSGTL(("PortMon/PortMon_MasterThread",
                    "#%d# Master Thread started for < %u %d %d %s >\n",
                    MY_THR_ID,
                    portRow->idx,
                    portRow->index.len,
					portRow->portMonitorNumber,
                    portRow->portMonitorType));
            }
        } else {
            // Change it to not-active
            portRow->threadActive = 0;

            snmp_log(LOG_ERR, "PortMon/PortMon_MasterThread \
                #%d# Thread could not be started for OID < %u >\n",
                MY_THR_ID, portRow->idx);
        }

        // Get the next row
        portRow = CONTAINER_NEXT(cb.container, portRow);
    }
}


/*****************************************************************************
 *
 * name             :   PortCheckThread
 * description      :   This is the main function for thread
 * input parameters :   void *psData - the Port Monitoring row context
 * output parameters:   None
 * return type      :   void *
 * global variables :   None
 * calls            :   None
 */

void* PortCheckThread(void *portData)
{
	int ret = 0;
	int flag = 0;
	stVarBindArgProc* varBinds = NULL;
    portMonitorTable_context* row = NULL;

	DEBUGMSGTL(("PortMon/PortCheckThread",
		"#%d# Thread spawned\n", MY_THR_ID));

    row = (portMonitorTable_context*)portData;

    while (1) {
        // Before i start checking for file, get the latest values of the data
        row = CONTAINER_FIND(cb.container, &(row->index));

        // If the row does not exist anymore
        if (row == NULL) {
            DEBUGMSGTL(("PortMon/PortCheckThread",
                "#%d# Thread exit\n", MY_THR_ID));
            pthread_detach(pthread_self());
            pthread_exit(NULL);
        }

        // Check, if the row status is not active, exit the thread
        if (row->portMonitorStatus != SNMP_ROW_ACTIVE) {
            DEBUGMSGTL(("PortMon/PortCheckThread",
                "#%d# Thread exit\n", MY_THR_ID));

            row->threadActive = 0;
            pthread_detach(pthread_self());
            pthread_exit(NULL);
        }

        // Check if the Row is active, else continue until it becomes active
        if (row->portMonitorStatus == SNMP_ROW_ACTIVE) {
            if ((strlen((char*)row->portMonitorType) == 0 ) ||
				(row->portMonitorNumber <= 0) ||
                (row->portMonitorCheckInterval == 0 )) {
                SLEEP(TIME_TO_SLEEP);
                continue;   // No need to process the request,
                            // until valid values are provided
            }

            // For reaction 2, 3 check for reaction path also.
            if ((row->portMonitorReaction == 2 ||
                row->portMonitorReaction == 3 ) &&
                strlen((char*)row->portMonitorReactionPath) == 0) {
                SLEEP(TIME_TO_SLEEP);
                continue;   // No need to process the request,
                            // until valid values are provided
            }
        } else {
            // since the row is not active
            SLEEP(TIME_TO_SLEEP);
            continue;
        }

        DEBUGMSGTL(("PortMon/PortCheckThread",
            "#%d# OID <%u> OID_LEN <%d> PORT <%s>\n",
            MY_THR_ID,
            *(row->index.oids),
            row->index.len,
            row->portMonitorType));

		pthread_mutex_lock(&portMonCheckLock);

		ret = checkListenPort(row);

		pthread_mutex_unlock(&portMonCheckLock);

		if (ret == -99) {			/* something wrong */
#ifdef WIN32
			SLEEP(MAX(row->portMonitorCheckInterval, CHECK_INTERVAL));
#else
			SLEEP(row->portMonitorCheckInterval);
#endif
			continue;
		}

		if (ret == 1) {				/* port listen */
			if (flag == 0) {		/* agent started */
				flag = 1;
				strcpy(row->portMonitorState, "LISTEN");
				row->portMonitorState_len = 6;
				row->portMonitorState[6] = 0;
			}

			if (strcmp(row->portMonitorState, "DOWN") == 0) {
				strcpy(row->portMonitorState, "LISTEN");
				row->portMonitorState_len = 6;
				row->portMonitorState[6] = 0;

				varBinds = makePortMonVarBind(row);

				if (Common_SendAlert_And_Vars("PTMN", gcInfoLogFileProc, 
					row->portMonitorDescription, 
					row->portMonitorIndex, varBinds, 5)) { 
					snmp_log(LOG_ERR, "Sending log failed for <%d:%s>\n", 
						row->portMonitorNumber, row->portMonitorState); 
				}

				if (varBinds != NULL) 
					free(varBinds);
			}
		} else if (ret == -1) {		/* port down */
			if (flag == 0) {
				flag = 1;
				strcpy(row->portMonitorState, "DOWN");
				row->portMonitorState_len = 4;
				row->portMonitorState[4] = 0;
			}

			if (strcmp(row->portMonitorState, "LISTEN") == 0) {
				strcpy(row->portMonitorState, "DOWN");
				row->portMonitorState_len = 4;
				row->portMonitorState[4] = 0;

				varBinds = makePortMonVarBind(row);

				if (Common_SendAlert_And_Vars("PTMN", gcInfoLogFileProc, 
					row->portMonitorDescription, 
					row->portMonitorIndex, varBinds, 5)) { 
					snmp_log(LOG_ERR, "Sending log failed for <%d:%s>\n", 
						row->portMonitorNumber, row->portMonitorState); 
				}

				if (varBinds != NULL) 
					free(varBinds);
			}
		}

        DEBUGMSGTL(("PortMon/PortCheckThread",
            "#%d# Thread check for <%d %s %s> port\n",
            MY_THR_ID, 
			row->portMonitorNumber,
			row->portMonitorType,
			row->portMonitorState));

#ifdef WIN32
        SLEEP(MAX(row->portMonitorCheckInterval, CHECK_INTERVAL));
#else
        SLEEP(row->portMonitorCheckInterval);
#endif

        DEBUGMSGTL(("PortMon/PortCheckThread",
            "#%d# Thread woke up!\n", MY_THR_ID));

    } // End of infinite while loop
}


int checkListenPort(portMonitorTable_context *row)
{
	port_cache_info *portList = NULL;
	int listen_port = 0;

    if (PortList == NULL) {			/* global variable */
    	return -99;
    }

	if (full_port_list == 0) {
		return -99;
	}

    listen_port = row->portMonitorNumber;
	
	for (portList = PortList; portList; portList = portList->next) {
		if (portList->listen_port == listen_port) {
			return (1);
		}
	}

	return (-1);
}


/*****************************************************************************
 *
 * name             :   PortListThread
 * description      :   This is a constructing port list fun for check thread
 * input parameters :   void *data - unused
 * output parameters:   None
 * return type      :   void *
 * global variables :   None
 * calls            :   None
 */

void* PortListThread(void *data)
{
	int ret = 0;
	char tbuf[30];
	port_cache_info* newList = NULL;
	port_cache_info* oldList = NULL;
	port_cache_info* portInfo = NULL;
	
    while(1) {
		ret = 0;

        newList = getTcpTable(&ret);

		if (ret == 0) {	/* made partial port list */
			getCurrentTime(tbuf);
/* aix4
            snmp_log(LOG_ERR, "[%s] PortListThread: ret=[%d]\n", tbuf, ret);
*/
			/* not swap old list, just free partial list */

			pthread_mutex_lock(&portMonCheckLock);		
			
			full_port_list = 0; 	/* constructing port list failed */

			pthread_mutex_unlock(&portMonCheckLock);

            while(newList != NULL) {	
                portInfo = newList->next;
                free(newList);
                newList = portInfo;
			}
		} else {		/* no error while constructing port list */
			if (newList != NULL) {
				pthread_mutex_lock(&portMonCheckLock);		

				oldList = PrePortList;
				PrePortList = PortList;
				PortList = newList;

				full_port_list = 1;

				pthread_mutex_unlock(&portMonCheckLock);

				while(oldList != NULL) {
					portInfo = oldList->next;
					free(oldList);
					oldList = portInfo;
				}
			} else {
				getCurrentTime(tbuf);
/* aix4
				snmp_log(LOG_ERR,
					" %s #%d# PortListThread failed\n", tbuf, MY_THR_ID);
*/
        	}
		}

        SLEEP(CHECK_INTERVAL);
    }
}


/*****************************************************************************
 *
 * name             :   PortMon_LoadContainerFromFile
 * description      :   This function is used load the default configuration
 *                      from the data file
 * input parameters :   None
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   
 */

#ifndef WIN32

void PortMon_LoadContainerFromFile()
{
    int fd = 0;
    int fdTmp = 0;
    int cnt = 0;
    int rewriteFlag = 0;
    portMonitorTable_context ctx = {0};
    portMonitorTable_context* row = NULL;

    // Open the Data file
    fd = open (PORT_MON_DATA_FILE, O_RDONLY);

    if (fd == -1) {
        snmp_log_perror("PortMon_LoadContainerFromFile: PORT_MON_DATA_FILE file open failed\n");
		return;
	}

	DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
		"#%d# Thread <%s> file opened\n",
		MY_THR_ID, PORT_MON_DATA_FILE));

	while (read(fd, &ctx, PORT_MON_CONTEXT_SIZE) > 0) {
		// If the row, is already destroyed, no need to create it again.
		if (ctx.portMonitorStatus == SNMP_ROW_DESTROY) {
			rewriteFlag = 1;   // to re-write the mib data file
			continue; // As we have to load the container
		}

		DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
			"Loading MIB Row for Port <%d %s>\n",
				ctx.portMonitorNumber,
				ctx.portMonitorType));

		row = (portMonitorTable_context*)malloc(PORT_MON_CONTEXT_SIZE);

		if (row == NULL) {
			snmp_log_perror("PortMon: Malloc failed");
			return;
		}

		memcpy(row, &ctx, PORT_MON_CONTEXT_SIZE);

		netsnmp_assert(row->index.oids == NULL);

		if (snmp_clone_mem((void*)&row->index.oids, &(ctx.idx), ctx.index.len * sizeof(oid) )) {
			DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
				"#%d# Thread <%s> file closed\n",
				MY_THR_ID, PORT_MON_DATA_FILE));

			free(row);
			close(fd);
			return;
		}

		row->index.len = ctx.index.len;

		cnt = CONTAINER_INSERT(cb.container, row);

		if (cnt) {
			DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
			"ERROR IN CONTAINER_INSERT FUNCION\n"));
		}

		DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
			"OID <%u>\n", *(row->index.oids)));
	} /* while */

	DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
		"CONTAINER SIZE=%u\n", CONTAINER_SIZE (cb.container)));

	if (rewriteFlag == 0) {
		close(fd);
		return;
	}

	DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
		"Rewriting the data file\n"));

	// open temporary file
	fdTmp = open(PORT_MON_DATA_FILE_TMP, O_CREAT|O_RDWR);

	if (fdTmp == -1) {
		snmp_log_perror("PORT_MON_DATA_FILE_TMP file open failed\n");

        DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
            "#%d# Thread <%s> file closed; open failed\n",
            MY_THR_ID, PORT_MON_DATA_FILE_TMP));

		close(fd);
		return;
	}

	DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
		"#%d# Thread <%s> file opened\n",
		MY_THR_ID, PORT_MON_DATA_FILE_TMP));

	// Reset the file pointers to begining of the files
	lseek(fd, 0, SEEK_SET);
	lseek(fdTmp, 0, SEEK_SET);

	while (read (fd, &ctx, PORT_MON_CONTEXT_SIZE) > 0) {
		if (ctx.portMonitorStatus != SNMP_ROW_DESTROY)
			if (write(fdTmp, &ctx, PORT_MON_CONTEXT_SIZE) < 0)
				snmp_log_perror(("Error while writing data to PORT_MON_DATA_FILE"));
	}

	close (fdTmp);
	close (fd);

	DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
		"#%d# Thread <%s> file closed\n",
		MY_THR_ID, PORT_MON_DATA_FILE_TMP));

	// Delete the old file and rename
	unlink(PORT_MON_DATA_FILE);
	rename(PORT_MON_DATA_FILE_TMP, PORT_MON_DATA_FILE);

	DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
		"#%d# Thread <%s> file closed\n",
		MY_THR_ID, PORT_MON_DATA_FILE));
}

#else

// For Windows

void PortMon_LoadContainerFromFile()
{
    HANDLE  hFd = NULL;
    HANDLE  hFdTmp = NULL;
    DWORD num;
    int iCnt=0;
    int iIdx=0;
    int iRewriteFlag=0;
    char acStrOid[MAX_OID_LEN] = { 0 };
    portMonitorTable_context ctx = {0};
    portMonitorTable_context* row = NULL;

    // Open the Data file
    hFd = CreateFile   (PORT_MON_DATA_FILE,
                        GENERIC_READ,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    if (hFd == INVALID_HANDLE_VALUE) {
        snmp_log_perror("PortMon_LoadContainerFromFile: \
            PORT_MON_DATA_FILE file open failed\n");
		return;
	}

	DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
		"#%d# Thread <%s> file opened\n",
		MY_THR_ID, PORT_MON_DATA_FILE));

	iIdx=0;

	while (ReadFile(hFd, &ctx, PORT_MON_CONTEXT_SIZE, &num, NULL)) {
		snmp_log(LOG_ERR, "Size <%d> <%d>\n", num, PORT_MON_CONTEXT_SIZE);
            
		if (num == 0) {
			snmp_log(LOG_ERR, " %d breaking out \n", GetLastError());
			break;
		}

		// If the row, is already destroyed, no need to create it again.
		if (ctx.portMonitorStatus == SNMP_ROW_DESTROY ) {
			iRewriteFlag = 1;   // to re-write the mib data file
			continue;			// As we have to load the container anyways
		}

		// Otherwise
		DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
			"Loading MIB Row for Port <%d %s>\n",
				ctx.portMonitorNumber,
				ctx.portMonitorType));

		row = (portMonitorTable_context*)malloc(PORT_MON_CONTEXT_SIZE);

		if (row == NULL) {
			snmp_log_perror("PortMon: Malloc failed");
			return;
		}

		memcpy(row, &ctx, PORT_MON_CONTEXT_SIZE);

		netsnmp_assert(row->index.oids == NULL);

		if (snmp_clone_mem( (void*)&row->index.oids,
                            &(ctx.idx), ctx.index.len * sizeof(oid) )) {
			free(row);
			CloseHandle(hFd);
			return;
		}

		row->index.len = ctx.index.len;

		iCnt = CONTAINER_INSERT(cb.container, row);

		if (iCnt) {
			DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
				"ERROR IN CONTAINER_INSERT FUNCION\n"));
		}

		DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
			"OID in PortMon_LoadContainerFromFile <%u>\n", 
				*(row->index.oids)));
	}

	DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
		"CONTAINER SIZE=%u\n", CONTAINER_SIZE(cb.container)));

	if (iRewriteFlag == 0) {
		CloseHandle(hFd);
		return;
	}


	DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
		"Rewriting the data file\n"));

	hFdTmp = CreateFile(PORT_MON_DATA_FILE_TMP,
					GENERIC_READ|GENERIC_WRITE,
					0,
					NULL,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL);

	if (hFdTmp == INVALID_HANDLE_VALUE) {
		snmp_log_perror("PORT_MON_DATA_FILE_TMP file open failed\n");

		CloseHandle(hFd);
		return;
	}

	DEBUGMSGTL(("PortMon/PortMon_LoadContainerFromFile",
		"#%d# Thread <%s> file opened\n",
		MY_THR_ID, PORT_MON_DATA_FILE_TMP));

	// Reset the file pointers to begining of the files
	SetFilePointer(hFd, 0, NULL, FILE_BEGIN);
	SetFilePointer(hFdTmp, 0, NULL, FILE_BEGIN);

	iIdx=0;
                
	while (ReadFile(hFd, &ctx, PORT_MON_CONTEXT_SIZE, &num, NULL)) {
		if (num == 0) {
			snmp_log(LOG_ERR," %d breaking out \n",GetLastError());
			break;
		}
            
		if (ctx.portMonitorStatus != SNMP_ROW_DESTROY)
			if (WriteFile(hFdTmp, &ctx, PORT_MON_CONTEXT_SIZE, &num, NULL) == 0 )
				snmp_log_perror(("Error while writing data to PORT_MON_DATA_FILE"));
	} // End of while

	// Close the file
	CloseHandle(hFdTmp);
	CloseHandle(hFd);

	// Delete the old file and rename
	unlink(PORT_MON_DATA_FILE);
	rename(PORT_MON_DATA_FILE_TMP, PORT_MON_DATA_FILE);

}

#endif

/*****************************************************************************
 *
 * name             :   PortMon_UpdateDataFile
 * description      :   This function is used to update the MIB data file with
 *                      the latest record
 * input parameters :   portMonitorTable_context* portMonRow
 * output parameters:   None
 * return type      :   int - "0" for success in writing new record,
 *                          - "-1" for error,
 *                          - "1" for success in modifying existing record
 * global variables :   None
 * calls            :   None
 */

#ifndef WIN32

// Function for Unix flavors
int PortMon_UpdateDataFile(portMonitorTable_context* portMonRow)
{
    portMonitorTable_context inFile;
    int iFd=-1;
    int iFoundFlag=0;
    int iRowNum=0;
    unsigned long ulPos=0;

    // Open the MIB data file
    // Added 'S_IREAD|S_IWRITE' for Windows
    if ((iFd = open(PORT_MON_DATA_FILE,O_CREAT|O_RDWR,S_IREAD|S_IWRITE)) < 0) {
        snmp_log_perror("Error opening PORT_MON_DATA_FILE");
        return (-1);
	}

	DEBUGMSGTL(("PortMon/PortMon_UpdateDataFile",
		"#%d# Thread <%s> file opened\n",
		MY_THR_ID, PORT_MON_DATA_FILE));

	lseek(iFd, 0,SEEK_SET);

	if (portMonRow->portMonitorStatus == SNMP_ROW_DESTROY)
		portMonRow->threadActive = 0;

	while (read(iFd, &inFile, PORT_MON_CONTEXT_SIZE) > 0) {
		if (inFile.idx == *portMonRow->index.oids) {
			// Check if the row entry in the file
			// is actually deleted fromthe container
			if (inFile.portMonitorStatus == SNMP_ROW_DESTROY)
				iFoundFlag =0;
			else
				iFoundFlag =1;

			if (lseek(iFd, ulPos, SEEK_SET) == (ulPos -1)) {
				snmp_log_perror("Error while re-setting file descriptor position");
			} else {
				DEBUGMSGTL(("PortMon/PortMon_UpdateDataFile",
					"#%d# Thread OID <%u> found FoundFlag <%d>\n",
					MY_THR_ID, *portMonRow->index.oids, iFoundFlag));
			}

			break;
		}

		ulPos = lseek(iFd, 0, SEEK_CUR);
	}

	// Copy the oid, instead copying the pointer
	portMonRow->idx = *(portMonRow->index.oids);

	if (write(iFd, portMonRow, PORT_MON_CONTEXT_SIZE) < 0) {
		snmp_log_perror("Error while writing data to PORT_MON_DATA_FILE");

		DEBUGMSGTL(("PortMon/PortMon_UpdateDataFile",
			"#%d# Thread <%s> file closed; write error\n",
			MY_THR_ID, PORT_MON_DATA_FILE));

		close(iFd);
		return(-1);
	}

	DEBUGMSGTL(("PortMon/PortMon_UpdateDataFile",
		"#%d# Thread Record written successfully to data file <%d>\n",
		MY_THR_ID, portMonRow->threadActive));

    close(iFd);

    DEBUGMSGTL(("PortMon/PortMon_UpdateDataFile",
        "#%d# Thread <%s> file closed\n",
        MY_THR_ID, PORT_MON_DATA_FILE));

    return(iFoundFlag);
}

#else

// Function for windows
int PortMon_UpdateDataFile(portMonitorTable_context *portMonRow)
{
    portMonitorTable_context inFile;
    HANDLE hFd = NULL;
    DWORD NumberOfBytesRead;
    int iFoundFlag=0;
    int iRowNum=0;
    unsigned long ulPos=0;
    static int iashtemp;

    // Open the MIB data file
    hFd = CreateFile   (PORT_MON_DATA_FILE,
                        GENERIC_READ|GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    if(hFd == INVALID_HANDLE_VALUE) {
        snmp_log_perror("Error opening PORT_MON_DATA_FILE");
        return(-1);
    } else {
        DEBUGMSGTL(("PortMon/PortMon_UpdateDataFile",
            "#%d# Thread <%s> file opened\n",
            MY_THR_ID, PORT_MON_DATA_FILE));

        SetFilePointer(hFd, 0, NULL, FILE_BEGIN);
    
        if (portMonRow->portMonitorStatus == SNMP_ROW_DESTROY)
            portMonRow->threadActive = 0;

        while (ReadFile(hFd, &inFile, PORT_MON_CONTEXT_SIZE, &NumberOfBytesRead, NULL)) {
            if (NumberOfBytesRead== 0) {
               snmp_log(LOG_ERR," %d breaking out \n", GetLastError());
               break;
            }

            if (inFile.idx == *portMonRow->index.oids) {
                // Check if the row entry in the file
                // is actually deleted fromthe container
                if (inFile.portMonitorStatus == SNMP_ROW_DESTROY)
                    iFoundFlag =0;
                else
                    iFoundFlag =1;

                if (SetFilePointer(hFd,ulPos,NULL,FILE_BEGIN) == (ulPos -1)) {
                    snmp_log_perror(
                        "Error while re-setting file descriptor position");
                } else {
                    DEBUGMSGTL(("PortMon/PortMon_UpdateDataFile",
                        "#%d# Thread OID <%u> found FoundFlag <%d>\n",
                        MY_THR_ID, *portMonRow->index.oids, iFoundFlag));

                }

                break;
            }

            ulPos = SetFilePointer(hFd, 0, NULL, FILE_CURRENT);
        }

        // Copy the oid, instead copying the pointer
        portMonRow->idx = *(portMonRow->index.oids);

        if (WriteFile (hFd,
                       portMonRow,
                       PORT_MON_CONTEXT_SIZE,
                       &NumberOfBytesRead,
                       NULL) == 0 ) {
            snmp_log_perror("Error while writing data to PORT_MON_DATA_FILE");
            DEBUGMSGTL(("PortMon/PortMon_UpdateDataFile",
                "#%d# Thread data file closed\n", MY_THR_ID));
            CloseHandle(hFd);
            return(-1);
        } else {
            DEBUGMSGTL(("PortMon/PortMon_UpdateDataFile",
                "#%d# Thread Record written successfully to the data file\n",
                MY_THR_ID));
		}
    }

    CloseHandle(hFd);
    return(iFoundFlag);
}

#endif


stVarBindArgProc *makePortMonVarBind(portMonitorTable_context *row)
{
	stVarBindArgProc* varBinds = (stVarBindArgProc *)NULL;
	int var_cnt = 5;

	varBinds = (stVarBindArgProc *)malloc(sizeof(stVarBindArgProc) * var_cnt);

	memset(varBinds, 0, sizeof(stVarBindArgProc) * var_cnt);

	strcpy(varBinds[0].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.2");
	varBinds[0].type='i';
	sprintf(varBinds[0].value, "%d", row->portMonitorNumber);

	strcpy(varBinds[1].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.3");
	varBinds[1].type='s';
	strcpy(varBinds[1].value, row->portMonitorType);

	strcpy(varBinds[2].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.4");
	varBinds[2].type='s';
	strcpy(varBinds[2].value, row->portMonitorState);

	strcpy(varBinds[3].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.6");
	varBinds[3].type='s';
	strcpy(varBinds[3].value, row->portMonitorDescription);

	strcpy(varBinds[4].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.10");
	varBinds[4].type='s';
	strcpy(varBinds[4].value, row->portMonitorAlertMessage);

	return varBinds;
}


/************************************************************
 * Initializes the portMonitorTable module
 */
void init_portMonitorTable(void)
{
    initialize_table_portMonitorTable();

    /*
     * TODO: perform any startup stuff here
     */

    PortMon_LoadContainerFromFile();

    PortMon_MasterThread();
}

/*

stVarBindArgProc *makePortMonitorVarBind_Down(portMonitorTable_context *myContext)
{
	stVarBindArgProc* vb = (stVarBindArgProc *)NULL;
	int var_cnt = 6;

	vb = (stVarBindArgProc *)malloc(sizeof(stVarBindArgProc) * var_cnt);
	memset(vb, 0x00, sizeof(stVarBindArgProc) * var_cnt);

	strcpy(vb[0].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.2");
	vb[0].type = 's';
	strcpy(vb[0].value, myContext->fileMonitorName);

	strcpy(vb[1].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.4");
	vb[1].type = 's';
	strcpy(vb[1].value, myContext->fileMonitorDescription);

	strcpy(vb[2].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.6");
	vb[2].type = 's';
	strcpy(vb[2].value, myContext->fileMonitorModifiedDateTime);

	strcpy(vb[3].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.7");
	vb[3].type = 'i';
	sprintf(vb[3].value, "%ld", myContext->fileMonitorFileSize);

	strcpy(vb[4].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.8");
	vb[4].type = 's';
	strcpy(vb[4].value, myContext->fileMonitorOwner);

	strcpy(vb[5].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.11");
	vb[5].type = 's';
	strcpy(vb[5].value, myContext->fileMonitorAlertMessage);

	return vb;
}

stVarBindArgProc *makePortMonitorVarBind_Up(portMonitorTable_context *myContext)
{
	stVarBindArgProc* vb = (stVarBindArgProc *)NULL;
	int var_cnt = 6;

	vb = (stVarBindArgProc *)malloc(sizeof(stVarBindArgProc) * var_cnt);
	memset(vb, 0x00, sizeof(stVarBindArgProc) * var_cnt);

	strcpy(vb[0].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.2");
	vb[0].type = 's';
	strcpy(vb[0].value, myContext->fileMonitorName);

	strcpy(vb[1].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.4");
	vb[1].type = 's';
	strcpy(vb[1].value, myContext->fileMonitorDescription);

	strcpy(vb[2].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.6");
	vb[2].type = 's';
	strcpy(vb[2].value, myContext->fileMonitorModifiedDateTime);

	strcpy(vb[3].variable, ".1.3.6.1.4.1.3204.1.3.35.7.1.7");
	vb[3].type = 'i';
	sprintf(vb[3].value, "%ld", myContext->fileMonitorFileSize);

	strcpy(vb[4].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.8");
	vb[4].type = 's';
	strcpy(vb[4].value, myContext->fileMonitorOwner);

	strcpy(vb[5].variable, ".1.3.6.1.4.1.3204.1.3.35.8.1.11");
	vb[5].type = 's';
	strcpy(vb[5].value, myContext->fileMonitorAlertMessage);

	return vb;
}

*/


#ifndef WIN32
#ifndef solaris2

port_cache_info* getTcpTable(int* ret)
{
    
    int			i = 0;
	int			local_port = 0;
	int			remote_port = 0;
	u_char		local_addr[64];
	u_char		remote_addr[64];
    u_char*		cp = NULL;

	port_cache_info* portHead = NULL;
	port_cache_info* portNew = NULL;
	port_cache_info* portTail = NULL;

	memset(local_addr, 0, sizeof(local_addr));
	memset(remote_addr, 0, sizeof(remote_addr));

	*ret = 0;				/* first, set 'partial list' */

#ifdef hpux11
    int             LowState, idx=0;
    static mib_tcpConnEnt tcp, Lowtcp;
#else
    int             LowState;
    static struct inpcb inpcb, Lowinpcb, idxcb;
#endif
    /*
     *  Allow for a kernel w/o TCP
     */
#ifdef TCPSTAT_SYMBOL
#if !defined(linux) && !defined(hpux11)
#ifdef KERNEL_64
	return NULL;
#else	
    if (auto_nlist_value(TCPSTAT_SYMBOL) == -1)
        return (NULL);
#endif
#endif
#endif

    /*
     * find "next" connection 
     */
#ifndef hpux11
  Again:
#endif
    LowState = -1;              /* Don't have one yet */

    tcpTable_load(NULL, NULL);
    #ifdef hpux11
    tcpTable_first_entry_scan(&idx);
    #else
    tcpTable_first_entry_scan(&idxcb);
    #endif
    
    for (;;) {
#ifdef hpux11
        if ((i = tcpTable_next_entry_scan(&idx, &tcp) == 0)
            break;              /* Done */
        cp = (u_char *) & tcp.LocalAddress;
#else                           /* hpux11 */
        if ((i = tcpTable_next_entry_scan(&idxcb, &inpcb)) < 0)
            goto Again;
        if (i == 0)
            break;              /* Done */
	#ifdef osf5
        cp = (u_char *) & inpcb.inp_laddr.s6_un.sa6_addr[12];
	#else
        cp = (u_char *) & inpcb.inp_laddr.s_addr;
	#endif
#endif                          /* hpux11 */

#ifdef hpux11
		LowState = tcp.State;
		Lowtcp = tcp;
#else                           /* hpux11 */
		LowState = inpcb.inp_state;
		Lowinpcb = inpcb;
#endif                          /* hpux11 */

		if (LowState != 2)		/* Listen state */
			continue;

		sprintf(local_addr, "%d.%d.%d.%d", cp[0], cp[1], cp[2], cp[3]);

#ifdef hpux11
		local_port =  (unsigned short) tcp.LocalPort;
#else
		local_port = ntohs(inpcb.inp_lport);
#endif


#ifdef hpux11
        cp = (u_char *) & tcp.RemAddress;
#else
	#ifdef osf5
        cp = (u_char *) & inpcb.inp_faddr.s6_un.sa6_addr[12];
	#else
        cp = (u_char *) & inpcb.inp_faddr.s_addr;
	#endif
#endif

		sprintf(remote_addr, "%d.%d.%d.%d", cp[0], cp[1], cp[2], cp[3]);

#ifdef hpux11
		remote_port = (unsigned short) tcp.RemPort;
#else
		remote_port = ntohs(inpcb.inp_fport);
#endif

		portNew = (port_cache_info*)malloc(sizeof(port_cache_info));
		if (portNew == NULL) {
			snmp_log(LOG_ERR, "PortListThread malloc failed");
			continue;
		}

		memset(portNew, 0, sizeof(port_cache_info));

		portNew->listen_port = local_port;
		portNew->next = NULL;

		if (portHead == NULL) {
			portHead = portNew;
			portTail = portNew;
		} else {
			portTail->next = portNew;	
			portTail = portNew;
		}
	/*
		DEBUGMSGTL(("PortMon/getTcpTable",
			"#%d# %15s:%4d %4d <- %15s:%4d\n",
			MY_THR_ID, local_addr, local_port, 
			LowState, remote_addr, remote_port));
	*/
	}

	*ret = 1;		/* full port list */

	return portHead;
}

#else                           /* solaris2 - tcp */


static int TCP_Cmp(void *addr, void *ep)
{
    if (memcmp((mib2_tcpConnEntry_t *) ep, (mib2_tcpConnEntry_t *) addr,
               sizeof(mib2_tcpConnEntry_t)) == 0)
        return (0);
    else
        return (1);
}

port_cache_info* getTcpTable(int* ret)
{
	int			tcp_state = 0;
	int			local_port = 0;
	int			remote_port = 0;
	u_char		local_addr[64];
	u_char		remote_addr[64];
    u_char*		cp = NULL;

	port_cache_info* portHead = NULL;
	port_cache_info* portNew = NULL;
	port_cache_info* portTail = NULL;

#define TCP_CONN_LENGTH	20
#define TCP_LOCADDR_OFF	10
#define TCP_LOCPORT_OFF	14
#define TCP_REMADDR_OFF	15
#define TCP_REMPORT_OFF	19
    mib2_tcpConnEntry_t Lowentry, Nextentry, entry;
    req_e           req_type;
    int             Found = 0;

    memset(&Lowentry, 0, sizeof(Lowentry));

	*ret = 0;				/* first, set 'partial list' */

    for (Nextentry.tcpConnLocalAddress = (u_long) - 1, req_type =
         GET_FIRST; ; Nextentry = entry, req_type = GET_NEXT) {
        if (getMibstat(MIB_TCP_CONN, &entry, sizeof(mib2_tcpConnEntry_t), 
			req_type, &TCP_Cmp, &entry) != 0)
            break;

		tcp_state = entry.tcpConnState;

		if (tcp_state != 2)			/* only listen port */
			continue;

		cp = (u_char *) & entry.tcpConnLocalAddress;
		sprintf(local_addr, "%d.%d.%d.%d", cp[0], cp[1], cp[2], cp[3]);
		local_port = entry.tcpConnLocalPort;

		cp = (u_char *) & entry.tcpConnRemAddress;
		sprintf(remote_addr, "%d.%d.%d.%d", cp[0], cp[1], cp[2], cp[3]);
		remote_port = entry.tcpConnRemPort;

		portNew = (port_cache_info*)malloc(sizeof(port_cache_info));
		if (portNew == NULL) {
			snmp_log(LOG_ERR, "PortListThread malloc failed");
			continue;
		}

		memset(portNew, 0, sizeof(port_cache_info));

		portNew->listen_port = local_port;
		portNew->next = NULL;

		if (portHead == NULL) {
			portHead = portNew;
			portTail = portNew;
		} else {
			portTail->next = portNew;	
			portTail = portNew;
		}
	/*
		DEBUGMSGTL(("PortMon/getTcpTable",
			"#%d# %15s:%4d %4d <- %15s:%4d\n",
			MY_THR_ID, local_addr, local_port, 
			tcp_state, remote_addr, remote_port));
	*/
    }

	*ret = 1;

	return portHead;
}

#endif                       /* solaris2 - tcp */


#else                           /* WIN32 */

#include <iphlpapi.h>
MIB_TCPROW      tcp_row;

port_cache_info* getTcpTable(int* ret)
{
	int			local_port = 0;
	int			remote_port = 0;
	u_char		local_addr[64];
	u_char		remote_addr[64];

    oid         newname[MAX_OID_LEN], lowest[MAX_OID_LEN], *op;
    u_char         *cp;
    int             LowState = -1;
    DWORD           status = NO_ERROR;
    DWORD           dwActualSize = 0;
    UINT            i;
    struct timeval  now;
    static long     Time_Of_Last_Reload = 0;
    static PMIB_TCPTABLE pTcpTable = NULL;

	port_cache_info* portHead = NULL;
	port_cache_info* portNew = NULL;
	port_cache_info* portTail = NULL;

    /*
     * save some cpu-cycles, and reload after 5 secs...
     */
    gettimeofday(&now, (struct timezone *) 0);
    if ((Time_Of_Last_Reload + 5 <= now.tv_sec) || (pTcpTable == NULL)) {
        if (pTcpTable != NULL)
            free(pTcpTable);
        Time_Of_Last_Reload = now.tv_sec;
        /*
         * query for buffer size needed 
         */
        status = GetTcpTable(pTcpTable, &dwActualSize, TRUE);
        if (status == ERROR_INSUFFICIENT_BUFFER) {
            pTcpTable = (PMIB_TCPTABLE) malloc(dwActualSize);
            if (pTcpTable != NULL) {
                /*
                 * Get the sorted TCP table 
                 */
                status = GetTcpTable(pTcpTable, &dwActualSize, TRUE);
            }
        }
    }

    if (status == NO_ERROR) {
        for (i = 0; i < pTcpTable->dwNumEntries; ++i) {
            cp = (u_char *) & pTcpTable->table[i].dwLocalAddr;
			sprintf(local_addr, "%d.%d.%d.%d", cp[0], cp[1], cp[2], cp[3]);
            local_port = ntohs((unsigned short)(0x0000FFFF & pTcpTable->table[i].dwLocalPort));

            cp = (u_char *) & pTcpTable->table[i].dwRemoteAddr;
			sprintf(remote_addr, "%d.%d.%d.%d", cp[0], cp[1], cp[2], cp[3]);
            remote_port = ntohs((unsigned short)(0x0000FFFF & pTcpTable->table[i].dwRemotePort));

			pTcpTable->table[i].dwState;

			portNew = (port_cache_info*)malloc(sizeof(port_cache_info));
			if (portNew == NULL) {
				snmp_log(LOG_ERR, "PortListThread malloc failed");
				continue;
			}

			memset(portNew, 0, sizeof(port_cache_info));

			portNew->listen_port = local_port;
			portNew->next = NULL;

			if (portHead == NULL) {
				portHead = portNew;
				portTail = portNew;
			} else {
				portTail->next = portNew;	
				portTail = portNew;
			}
        }

		*ret = 1;			/* full port list */
	}

	return portHead;
}

#endif                          /* WIN32 */




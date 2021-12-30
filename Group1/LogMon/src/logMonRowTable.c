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
 * $Author: viren $
 * $Log: logMonRowTable.c,v $
 * Revision 1.39  2004/01/23 05:11:15  viren
 * added macro for flcose, and null check after each malloc - reviewed by Vijay
 *
 * Revision 1.38  2004/01/20 12:59:59  buvanes
 * CR: Sending trap sequence implemented
 *
 * Revision 1.37  2004/01/20 08:11:46  buvanes
 * CR: File Name changes during run time handled
 *
 * Revision 1.36  2004/01/20 04:19:25  buvanes
 * Implemented CRs - searching till EOFand sending Traps
 *
 * Revision 1.35  2004/01/19 13:24:17  buvanes
 * Implemented CRs
 *
 * Revision 1.34  2004/01/13 09:18:42  buvanes
 * statbuffer has been changed to variable from pointer to avoid memory leak
 *
 * Revision 1.33  2004/01/07 09:01:59  viren
 * Search Pattern method pstStatBuf was not free in input file open failed - Fixed
 *
 * Revision 1.32  2004/01/05 04:32:38  saisk
 * Made all includes related to windows as relative path
 *
 * Revision 1.31  2004/01/03 13:13:40  viren
 * Resolved the AT Issue of Row Status
 *
 * Revision 1.29  2004/01/02 08:44:39  viren
 * "continue" got deleted somehow, too bad - fixed
 *
 * Revision 1.28  2004/01/01 10:37:10  viren
 * enclosed call to Common_CheckPattern func in "#ifdef" for Unix only
 *
 * Revision 1.27  2003/12/31 12:19:26  rajesh
 * * The regex_t is defined locally and is not malloced. regcomp() will fill the
 *   structure and we free it after regexec() using regfree
 *
 * Revision 1.26  2003/12/23 03:33:30  sig
 * *** empty log message ***
 *
 * Revision 1.25  2003/12/16 15:06:18  viren
 * MasterThread function was modified to check for valid data before spawning a thread
 *
 * Revision 1.24  2003/12/14 10:41:29  viren
 * fclose was removed from search file
 *
 * Revision 1.19  2003/12/12 15:45:17  viren
 * silly mistake
 *
 * Revision 1.18  2003/12/12 15:31:35  viren
 * LogMon thread was not creating - Fixed
 *
 * Revision 1.17  2003/12/11 07:38:58  viren
 * Added few more debug information
 *
 * Revision 1.16  2003/12/09 19:45:06  viren
 * file close problems fixed
 *
 * Revision 1.15  2003/12/09 18:27:42  viren
 * File transfer implemented - Working fine
 *
 * Revision 1.14  2003/12/09 11:18:42  ashok
 * File operations changed for windows & Line 1795. close() moved to Line. 1810
 *
 * Revision 1.13  2003/12/08 18:21:47  viren
 * Updated review comments
 *
 * Revision 1.12  2003/12/05 07:40:25  viren
 * Line 2299 : iFd changed to iFdTmp
 *
 * Revision 1.11  2003/12/02 10:54:43  vijaysp
 * Trap msg. format changed. TrapOID value changed from Nuri OID to table oid and arg. changed to alert message string alone
 *
 * Revision 1.10  2003/12/02 06:21:14  vijaysp
 * One bug found during windwos test solved. line no: 2005 changed
 *
 * Revision 1.9  2003/12/02 04:16:30  vijaysp
 * Overwriting Ashok(?)'s changes to give the code for system testing
 *
 * Revision 1.6  2003/12/01 10:32:56  vijaysp
 * Trap is sent to AlertMsg instead of UDP alert message
 *
 * Revision 1.5  2003/11/28 09:28:59  ashok
 * Minor Changes
 *
 * Revision 1.4  2003/11/28 05:14:53  buvanes
 * Checked file pointers for NULL before closing the files
 *
 * Revision 1.3  2003/11/25 11:29:09  ashok
 * Changes get_host_IP for Windows
 *
 * Revision 1.2  2003/11/24 09:08:12  sig
 * free(pstHostEnt) has been removed from LogMonRow_GetLocalHostIp()
 * LogMonRow_SendAlert() has been removed from LogMonRow_SearchPatterns()
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
 * $Id: logMonRowTable.c,v 1.39 2004/01/23 05:11:15 viren Exp $
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

#ifdef WIN32
#include "../inc/logMonRowTable.h"
#include "vbregexp_header.h"
#else
#include "logMonRowTable.h"
#endif

static     netsnmp_handler_registration *my_handler = NULL;
static     netsnmp_table_array_callbacks cb;

static oid  objid_sysuptime[] = { 1, 3, 6, 1, 2, 1, 1, 3, 0 };
static oid  objid_snmptrap[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };

oid logMonRowTable_oid[] = { logMonRowTable_TABLE_OID };
size_t logMonRowTable_oid_len = OID_LENGTH(logMonRowTable_oid);

#ifdef WIN32
static long
RTM_Get_Uptime_Win ();
#endif /* WIN32 */


#define FCLOSE(x) if(x != NULL) fclose(x);\
                  x=NULL

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

    /** added by D.J Park in 2005.02.22 [start] */
    dst->logMonRowFileSizeThreshold = src->logMonRowFileSizeThreshold;
    dst->logMonRowFilterCounter = src->logMonRowFilterCounter;
    dst->logMonRowTrapType = src->logMonRowTrapType;
    /** added by D.J Park in 2005.02.22 [end] */

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
    /** added by D.J Park in 2005.02.22 [start] */
     ctx->logMonRowFileSizeThreshold = 0;
     ctx->logMonRowFilterCounter = 1;
     ctx->logMonRowTrapType = 1;
    /** added by D.J Park in 2005.02.22 [end] */

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
        
        /** added by D.J Park in 2005.02.22 [start] */
        case COLUMN_LOGMONROWFILESIZETHRESHOLD:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->logMonRowFileSizeThreshold));
        break;

        case COLUMN_LOGMONROWFILTERCOUNTER:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->logMonRowFilterCounter));
        break;

        case COLUMN_LOGMONROWTRAPTYPE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->logMonRowTrapType));
        break;

        /** added by D.J Park in 2005.02.22 [end] */

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

        /** added by D.J Park in 2005.02.22 */

        case COLUMN_LOGMONROWFILESIZETHRESHOLD:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonRowFileSizeThreshold = *var->val.integer;
        break;

        case COLUMN_LOGMONROWFILTERCOUNTER:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonRowFilterCounter = *var->val.integer;
        break;

        case COLUMN_LOGMONROWTRAPTYPE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonRowTrapType = *var->val.integer;
        break;

        /** added by D.J Park in 2005.02.22 */

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

    DEBUGMSGTL(("LogMonRow/logMonRowTable_set_commit",
        "called\n"));

    DEBUGMSGTL(("LogMonRow/logMonRowTable_set_commit",
        "logMonRowLogFile <%s> logMonRowReadInterval <%d> logMonRowFilter <%s>\n",
        row_ctx->logMonRowLogFile,
        row_ctx->logMonRowReadInterval,
        row_ctx->logMonRowFilter));

    if ( LogMonRow_UpdateDataFile(row_ctx) == 0 )
    {
        DEBUGMSGTL(("LogMonRow/logMonRowTable_set_commit",
            "Row updated in the LOG_MON_ROW_DATA_FILE \n"));
    }

    DEBUGMSGTL(("LogMonRow/logMonRowTable_set_commit",
        "LogFile <%s> Read Interval <%d> Filter <%s>\n",
        row_ctx->logMonRowLogFile,
        row_ctx->logMonRowReadInterval,
        row_ctx->logMonRowFilter));

    // Do the value check, before proceeding
    /** modified by D.J Park in 2005.03.02 [start] */
    /*
    if ( ( strlen(row_ctx->logMonRowLogFile) != 0 ) &&
        ( row_ctx->logMonRowReadInterval != 0 ) &&
        ( row_ctx->logMonRowStatus == SNMP_ROW_ACTIVE ) &&
        ( strlen(row_ctx->logMonRowFilter) != 0 ) )
    */
    if ( ( strlen(row_ctx->logMonRowLogFile) != 0 ) &&
        ( row_ctx->logMonRowReadInterval != 0 ) &&
        ( row_ctx->logMonRowStatus == SNMP_ROW_ACTIVE ) &&
        ( (strlen(row_ctx->logMonRowFilter) != 0) ||
          (row_ctx->logMonRowFileSizeThreshold > 0)) )
    /** modified by D.J Park in 2005.03.02 [end] */
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

        if ( row_ctx->iThreadActiveFlag == 0 )
        {

            // Create a thread for this newly created row
            if ( pthread_create(&pLogThr, NULL, LogMonRow_LogMonThreadMain,
                                      (void*)row_ctx) != 0 )
            {
                snmp_log_perror("logMonRowTable_set_commit \
                    Thread creation failed\n");

            }
            else
            {
                row_ctx->iThreadActiveFlag = 1;

                DEBUGMSGTL(("LogMonRow/logMonRowTable_set_commit",
                    "Thread started for < %u %d %s >\n",
                    row_ctx->idx,
                    row_ctx->index.len,
                    row_ctx->logMonRowLogFile));
            }

        }
    }

    // <End: WIP_NURI_ALL>

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
    DEBUGMSGTL(("LogMonRow/initialize_table_logMonRowTable",
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

        /** added by D.J Park in 2005.02.22 [start] */
        case COLUMN_LOGMONROWFILESIZETHRESHOLD:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonRowFileSizeThreshold,
                         sizeof(context->logMonRowFileSizeThreshold) );
        break;

        case COLUMN_LOGMONROWFILTERCOUNTER:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonRowFilterCounter,
                         sizeof(context->logMonRowFilterCounter) );
        break;

        case COLUMN_LOGMONROWTRAPTYPE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonRowTrapType,
                         sizeof(context->logMonRowTrapType) );
        break;

        /** added by D.J Park in 2005.02.22 [end] */

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

// Start: WIP_NURI_ALL : Start of the newly added code here

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
    pthread_t           stLogMonThr;
    logMonRowTable_context *pstLogRow=NULL;

    pstLogRow = CONTAINER_FIRST(cb.container);

#ifdef WIN32
    InitializeRegExp();
#endif

    while(pstLogRow)
    {

        // Do the value check, before proceeding
        /** modified by D.J Park in 2005.03.02 [start] */
        /*
        if ( ( strlen(pstLogRow->logMonRowLogFile) != 0 ) &&
            ( pstLogRow->logMonRowReadInterval != 0 ) &&
            ( strlen(pstLogRow->logMonRowFilter) != 0 ) )
        */
        if ( ( strlen(pstLogRow->logMonRowLogFile) != 0 ) &&
            ( pstLogRow->logMonRowReadInterval != 0 ) &&
            ( (strlen(pstLogRow->logMonRowFilter) != 0) 
              || (pstLogRow->logMonRowFileSizeThreshold > 0)) )
        /** modified by D.J Park in 2005.03.02 [end] */
        {

            // Now, spawn thread for it
            if ( pthread_create(&stLogMonThr, NULL,
                    LogMonRow_LogMonThreadMain,
                (void*)pstLogRow) != 0 )
            {
                snmp_log_perror("Thread creation failed");
            }
            else
            {
                // Change it to active
                pstLogRow->iThreadActiveFlag = 1;

                DEBUGMSGTL(("LogMonRow/LogMonRow_MasterThread",
                    "Master Thread started for < %u %d %s >\n",
                    pstLogRow->idx,
                    pstLogRow->index.len,
                    pstLogRow->logMonRowLogFile));
            }
        }
        else
        {
            // Change it to not-active
            pstLogRow->iThreadActiveFlag = 0;

            snmp_log(LOG_ERR,"LogMonRow/LogMonRow_MasterThread \
                Thr #%d# Thread could not be started for OID < %u >\n",
                MY_THR_ID,
                pstLogRow->idx);
        }


        // Get the next row
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
 *                      3) Common_SendAlert
 */

void *
LogMonRow_LogMonThreadMain(void *pstLogRowData)
{

    logMonRowTable_context *logMonRowRow;
    /** added by D.J Park in 2005.02.22 [start] */
    int sendFileSizeTrap = -1;
	stVarBindArgProc    *varBinds = NULL;
    /** added by D.J Park in 2005.02.22 [end] */


    DEBUGMSGTL(("LogMonRow/LogMonRow_LogMonThreadMain",
          "Thr #%d# Thread spawned for OID <%u>\n",
          MY_THR_ID,
          ((logMonRowTable_context*)pstLogRowData)->idx));

    // we need the index value, so do this
    logMonRowRow = (logMonRowTable_context*)pstLogRowData;
    
    	
	// Copy the file name for later reference
	/** added by D.J Park in 2004.3.30 
	strcpy(logMonRowRow->acOldFileName,logMonRowRow->logMonRowLogFile);
	*/
	strtimeformat(logMonRowRow->acChangedFileName,
			sizeof(logMonRowRow->acChangedFileName),
			logMonRowRow->logMonRowLogFile);
	strcpy(logMonRowRow->acOldFileName,logMonRowRow->acChangedFileName);

	logMonRowRow->logMonRowReadLine = 0;
	logMonRowRow->isStart = 1;

    /** added by D.J Park in 2005.02.22 [start]*/
    if(logMonRowRow->logMonRowFilterCounter < 1)
        logMonRowRow->logMonRowFilterCounter = 1;
	logMonRowRow->filterCounter = 0;
    /** added by D.J Park in 2005.02.22 [end]*/
	
    // Keep looping infinitely
    while(1)
    {
        // Get a context to my row in the MIB container
        // Get the latest values of the data
        logMonRowRow = CONTAINER_FIND(cb.container,&(logMonRowRow->index));

        // If the row does not exist anymore
        if ( logMonRowRow == NULL )
        {
            DEBUGMSGTL(("LogMonRow/LogMonRow_LogMonThreadMain",
                "Thr #%d# Exiting thread\n",
                MY_THR_ID));

            pthread_detach(MY_THR_ID);
            pthread_exit(NULL);  // exit the thread
        }

        // Check, If the row status is not active, exit the thread
        if ( logMonRowRow->logMonRowStatus != SNMP_ROW_ACTIVE )
        {
            DEBUGMSGTL(("LogMonRow/LogMonRow_LogMonThreadMain",
                "Thr #%d# Exiting thread\n",
                MY_THR_ID));

            // Unset the thread active status
            logMonRowRow->iThreadActiveFlag = 0;

            pthread_detach(MY_THR_ID);
            pthread_exit(NULL);  // exit the thread
        }

        // Do the value check, before proceeding
        /** modified by D.J Park in 2005.03.02 [start] */
        /*
        if ( ( strlen(logMonRowRow->logMonRowLogFile) == 0 ) ||
            ( logMonRowRow->logMonRowReadInterval == 0 ) ||
            ( strlen(logMonRowRow->logMonRowFilter) == 0 ))
        */
        if ( ( strlen(logMonRowRow->logMonRowLogFile) == 0 ) ||
            ( logMonRowRow->logMonRowReadInterval == 0 ) ||
            ( (strlen(logMonRowRow->logMonRowFilter) == 0) && 
              (logMonRowRow->logMonRowFileSizeThreshold < 1) ) 
            )
        /** modified by D.J Park in 2005.03.02 [ned] */
        {
            SLEEP(TIME_TO_SLEEP);
            // SLEEP for 3 sec until full row creation is done.
            continue; // No need to process the request,
                     // until valid values are provided
        }


        // Check if the filename has been changed or not
        
        DEBUGMSGTL(("debug","File Name Cur : %s Old : %s \n",logMonRowRow->logMonRowLogFile,logMonRowRow->acOldFileName)); 


		/** added by D.J Park in 2004.3.30 [start]
		 */
		strtimeformat(logMonRowRow->acChangedFileName,
			sizeof(logMonRowRow->acChangedFileName),
			logMonRowRow->logMonRowLogFile);
		/** added by D.J Park in 2004.3.30 [end]*/

		/*
        snmp_log(LOG_INFO,"File Name Cur : %s Old : %s \n",
			logMonRowRow->acChangedFileName,logMonRowRow->acOldFileName); 
		*/

		/** modified by D.J Park in 2004.3.30
        if (strcmp(logMonRowRow->logMonRowLogFile,logMonRowRow->acOldFileName)!=0)
		*/
        if (strcmp(logMonRowRow->acChangedFileName,logMonRowRow->acOldFileName)!=0)
        {
        	DEBUGMSGTL(("debug","File Name Changed\n")); 
			/*
        	snmp_log(LOG_INFO,"debugFile Name Changed\n"); 
			*/
            logMonRowRow->logMonRowFileSize = 0;
            logMonRowRow->logMonRowReadLine = 0;
            logMonRowRow->logMonRowLine = 0;
            logMonRowRow->logMonRowGetLine = 0;
            /** added by D.J Park in 2005.02.22 [start] */
	        logMonRowRow->filterCounter = 0;
            /** added by D.J Park in 2005.02.22 [end] */
            strcpy(logMonRowRow->logMonRowUpdatedTime,"");
			/** modified by D.J Park in 2004.03.30 
            strcpy(logMonRowRow->acOldFileName,logMonRowRow->logMonRowLogFile);
			*/
            strcpy(logMonRowRow->acOldFileName,logMonRowRow->acChangedFileName);
        }
        
        
        // Register an alarm for log file transfer if
        // not registered already
        if ( (guiLogFileLogRowAlarmReg == 0) &&
             (glAgentInfoFileTrPort > 0) &&
             (strlen(gcAgentInfoFileTransferIP) > 0))
        {
            DEBUGMSGTL(("LogMonRow/LogMonRow_LogMonThreadMain",
                "Thr #%d# Registering the alarm for LogMonRow Log file transfer\n",
                MY_THR_ID));
            if (glAgentInfoFileTrInterval > 0 )
                guiLogFileLogRowAlarmReg = snmp_alarm_register (glAgentInfoFileTrInterval,
                (int)NULL,Common_SendFileThread, gcInfoLogFileMonRow);
            else
                guiLogFileLogRowAlarmReg = snmp_alarm_register (1,
                (int)NULL,Common_SendFileThread, gcInfoLogFileMonRow);

        }



        DEBUGMSGTL(("LogMonRow/LogMonRow_LogMonThreadMain",
            "Thr #%d# Checking OID <%u> OID_LEN <%d> LOGFILE <%s>\n",
            MY_THR_ID,
            *(logMonRowRow->index.oids),
            logMonRowRow->index.len,
            logMonRowRow->logMonRowLogFile));

        // Check for the patterns
        LogMonRow_SearchPatterns(logMonRowRow);


        /** added by D.J Park in 2005.02.22 [start] 
         * File Size 임계치 검사후 Trap 발생
         */
        if(sendFileSizeTrap < 0 && (logMonRowRow->logMonRowFileSize < logMonRowRow->logMonRowFileSizeThreshold))
            sendFileSizeTrap=1;
        if(logMonRowRow->logMonRowFileSizeThreshold > 0)
        {
            if( sendFileSizeTrap == 0 && (logMonRowRow->logMonRowFileSize > logMonRowRow->logMonRowFileSizeThreshold) )
            { 
                varBinds = makeThresholdLogMonRowVarBind(logMonRowRow,2); 
                if (Common_SendAlert_And_Vars("LGRM", gcInfoLogFileMonRow, 
                            logMonRowRow->logMonRowAlertMessage, 
                            logMonRowRow->logMonRowIndex,varBinds,6)) 
                { 
                    snmp_log(LOG_ERR,"Alert sending failed for <%s> threshold "
                            "fileSize[%ld] fileSizeThreshold[%ld]\n", 
                            logMonRowRow->acChangedFileName,
                            logMonRowRow->logMonRowFileSize,
                            logMonRowRow->logMonRowFileSizeThreshold); 
                }        

                if(varBinds != NULL) 
                    free(varBinds); 
                sendFileSizeTrap++;
            }
            else if(sendFileSizeTrap > 0 && (logMonRowRow->logMonRowFileSize <= logMonRowRow->logMonRowFileSizeThreshold))
            {
                varBinds = makeThresholdLogMonRowVarBind(logMonRowRow,3); 
                if (Common_SendAlert_And_Vars("LGRM", gcInfoLogFileMonRow, 
                            logMonRowRow->logMonRowAlertMessage, 
                            logMonRowRow->logMonRowIndex,varBinds,6)) 
                { 
                    snmp_log(LOG_ERR,"Alert sending failed for <%s> recovery "
                            "fileSize[%ld] fileSizeThreshold[%ld]\n", 
                            logMonRowRow->acChangedFileName,
                            logMonRowRow->logMonRowFileSize,
                            logMonRowRow->logMonRowFileSizeThreshold); 
                }        

                if(varBinds != NULL) 
                    free(varBinds); 
                sendFileSizeTrap = 0;
            }
        }
        /** added by D.J Park in 2005.02.22 [start] */
        
        #if 0
        if( LogMonRow_SearchPatterns(logMonRowRow) == 1 )
        {
        
            if (Common_SendAlert("LGRM",
                gcInfoLogFileMonRow,
                logMonRowRow->logMonRowAlertMessage,
                logMonRowRow->logMonRowIndex))
            {
                snmp_log(LOG_ERR,
                    "Alert sending failed for file<%s>\n",
                    logMonRowRow->logMonRowLogFile);
            }

        }
        else
            SLEEP(TIME_TO_SLEEP);
    	#endif            


        // Update the data file with the latest row data
        if ( LogMonRow_UpdateDataFile(logMonRowRow) == -1 )
        {
            snmp_log_perror("Error while updating MIB file");
        }



       DEBUGMSGTL(("LogMonRow/LogMonRow_LogMonThreadMain",
            "Thr #%d# going to SLEEP for logfile <%s>\n",
            MY_THR_ID,
            logMonRowRow->logMonRowLogFile));


       // Sleep until the next interval
       SLEEP(logMonRowRow->logMonRowReadInterval);

       DEBUGMSGTL(("LogMonRow/LogMonRow_LogMonThreadMain",
            "Thr #%d# woke up for logfile <%s>\n",
            MY_THR_ID,
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
    FILE                *psFileP=NULL; // File descriptor
    int                 iLineCount=0; // line count value
    char                pcBuffer[LINE_MAX_LEN+1]; // read line buffer

    // Open the text file, in read mode
    if ( (psFileP = fopen(pcFileName,"r")) == NULL )
    {
        return (-1);
    }

    DEBUGMSGTL(("LogMonRow/LogMonRow_GetLineCount",
        "Thr #%d# File <%s> opened\n",
        MY_THR_ID,
        pcFileName));

    // Read until fgets returns error
    while(fgets(pcBuffer,LINE_MAX_LEN,psFileP)!=NULL)
    {
        // Increment line count
        iLineCount++;
    }

    // Close the file
    FCLOSE(psFileP);

    DEBUGMSGTL(("LogMonRow/LogMonRow_GetLineCount",
        "Thr #%d# File <%s> closed\n",
        MY_THR_ID,
        pcFileName));


    // Return the line count
    return(iLineCount);
}

int
LogMonRow_GetLineCountAndSeek(char *pcFileName,long *readLineSeek)
{
    FILE                *psFileP=NULL; // File descriptor
    int                 iLineCount=0; // line count value
    char                pcBuffer[LINE_MAX_LEN+1]; // read line buffer

    // Open the text file, in read mode
    if ( (psFileP = fopen(pcFileName,"r")) == NULL )
    {
        return (-1);
    }

    DEBUGMSGTL(("LogMonRow/LogMonRow_GetLineCount",
        "Thr #%d# File <%s> opened\n",
        MY_THR_ID,
        pcFileName));

    // Read until fgets returns error
    while(fgets(pcBuffer,LINE_MAX_LEN,psFileP)!=NULL)
    {
        // Increment line count
        iLineCount++;
    }
	*readLineSeek=ftell(psFileP);

    // Close the file
    FCLOSE(psFileP);

    DEBUGMSGTL(("LogMonRow/LogMonRow_GetLineCount",
        "Thr #%d# File <%s> closed\n",
        MY_THR_ID,
        pcFileName));


    // Return the line count
    return(iLineCount);
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

#ifndef WIN32

void
LogMonRow_LoadContainerFromFile()
{
    logMonRowTable_context stReadCtx = {0}; // Read row record from the file
    logMonRowTable_context *pstMyContext = NULL; // Container row context
    int                 iFd=0; // Data file descriptor
    int                 iFdTmp=0; // Temp data file descriptor
    int                 iRewriteFlag=0; // Whether to rewrite the data file

    if ((iFd = open (LOG_MON_ROW_DATA_FILE, O_RDONLY,S_IREAD|S_IWRITE))== -1)
    {
         snmp_log_perror
            ("LogMonRow_LoadContainerFromFile: \
                LOG_MON_ROW_DATA_FILE file open failed\n");
    }
    else
    {
        DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            LOG_MON_ROW_DATA_FILE));

        while (read (iFd, &stReadCtx, sizeof (logMonRowTable_context)) > 0)
        {
            // If the row, is already destroyed, no need to create it again.
            if ( stReadCtx.logMonRowStatus == SNMP_ROW_DESTROY )
            {
                 DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                     "Thr #%d# Found row status as destroyed <%d>\n",
                     MY_THR_ID,
                     stReadCtx.logMonRowStatus));

                 iRewriteFlag = 1; // to re-write the mib data file
                 continue; // as we have to load the container
            }

            DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                "Loading MIB Row for Log File <%s>\n",
                stReadCtx.logMonRowLogFile));

            pstMyContext = (logMonRowTable_context*)malloc(sizeof (logMonRowTable_context));
            if ( !pstMyContext )
            {
                snmp_log_perror("Malloc failed");
                return;
            }

            memcpy (pstMyContext, &stReadCtx, sizeof (logMonRowTable_context));

            netsnmp_assert(pstMyContext->index.oids == NULL);

            if(snmp_clone_mem( (void*)&pstMyContext->index.oids, &(stReadCtx.idx),
                            stReadCtx.index.len * sizeof(oid) ))
            {
                free(pstMyContext);
                close(iFd);

                DEBUGMSGTL(("LogMonRow/LogMonRow_SearchPatterns",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_ROW_DATA_FILE));

                return;
            }

            pstMyContext->index.len = stReadCtx.index.len;

            if (CONTAINER_INSERT (cb.container, pstMyContext))
            {
                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "ERROR IN CONTAINER_INSERT FUNCION\n"));
            }

            DEBUGMSGTL(("LogMonRow/Load_Container_From_LogMON_File",
              "OID <%u>\n",*(pstMyContext->index.oids)));
        }

        DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                "CONTAINER SIZE=%u\n", CONTAINER_SIZE (cb.container)));

        if ( iRewriteFlag )
        {
            // Open a temp file
            if ( ( iFdTmp = open (LOG_MON_ROW_DATA_FILE_TMP,
                                   O_CREAT|O_RDWR,S_IREAD|S_IWRITE) ) == -1 )
            {
                snmp_log_perror(
                       "LOG_MON_ROW_DATA_FILE_TMP file open failed\n");
                                // Close the file
                close(iFd);

                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_ROW_DATA_FILE));

            }
            else
            {

                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "Thr #%d# File <%s> opened\n",
                    MY_THR_ID,
                    LOG_MON_ROW_DATA_FILE_TMP));

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
                             "Error writing data to LOG_MON_ROW_DATA_FILE");
                        }

                } // End of while

                // Close the file
                close(iFdTmp);

                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_ROW_DATA_FILE_TMP));


                // Delete the old data file
                unlink(LOG_MON_ROW_DATA_FILE);

                // Rename the temp file to original
                rename(LOG_MON_ROW_DATA_FILE_TMP,LOG_MON_ROW_DATA_FILE);

            } // End of else

        } // End of rewrite if

        // Close the file
        close(iFd);

        DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
            "Thr #%d# File <%s> closed\n",
            MY_THR_ID,
            LOG_MON_ROW_DATA_FILE));

    } // End of else

}

#else
/*
void
LogMonRow_LoadContainerFromFile()
{
    logMonRowTable_context stReadCtx = {0}; // Read row record from the file
    logMonRowTable_context *pstMyContext = NULL; // Container row context
    HANDLE                  hFd = NULL; // Data file descriptor
    HANDLE                  hFdTmp = NULL; // Temp data file descriptor
    DWORD                   NumberOfBytesRead;
    int                     iRewriteFlag=0; // Whether to rewrite the data file

    // Open the Data file
    hFd = CreateFile   (LOG_MON_ROW_DATA_FILE,
                        GENERIC_READ|GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    if (hFd == INVALID_HANDLE_VALUE)
    {
         snmp_log_perror
            ("LogMonRow_LoadContainerFromFile: \
                LOG_MON_ROW_DATA_FILE file open failed\n");
    }
    else
    {
        DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
            "Thr #%d# File <%s> Opened\n",
            MY_THR_ID,
            LOG_MON_ROW_DATA_FILE));


        ReadFile(hFd, &stReadCtx, sizeof (logMonRowTable_context),
                &NumberOfBytesRead, NULL);

        while (NumberOfBytesRead ==  sizeof (logMonRowTable_context))
        {
            // If the row, is already destroyed, no need to create it again.
            if ( stReadCtx.logMonRowStatus == SNMP_ROW_DESTROY )
            {
                 iRewriteFlag = 1; // to re-write the mib data file
                 continue; // As we have to load the container anyways
            }

            DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                "Loading MIB Row for Log File <%s>\n",
                stReadCtx.logMonRowLogFile));

            pstMyContext = (logMonRowTable_context*)malloc(sizeof (logMonRowTable_context));
            if ( !pstMyContext )
            {
                snmp_log_perror("Malloc failed");
                return;
            }

            memcpy (pstMyContext, &stReadCtx, sizeof (logMonRowTable_context));

            netsnmp_assert(pstMyContext->index.oids == NULL);

            if(snmp_clone_mem( (void*)&pstMyContext->index.oids, &(stReadCtx.idx),
                            stReadCtx.index.len * sizeof(oid) ))
            {
                free(pstMyContext);
                CloseHandle(hFd);
                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_ROW_DATA_FILE));

                return;
            }

            pstMyContext->index.len = stReadCtx.index.len;

            if (CONTAINER_INSERT (cb.container, pstMyContext))
            {
                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "ERROR IN CONTAINER_INSERT FUNCION\n"));
            }

            DEBUGMSGTL(("LogMonRow/Load_Container_From_LogMON_File",
              "OID <%u>\n",*(pstMyContext->index.oids)));

            ReadFile(hFd, &stReadCtx, sizeof (logMonRowTable_context),
                &NumberOfBytesRead, NULL);

        }

        DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                "CONTAINER SIZE=%u\n", CONTAINER_SIZE (cb.container)));

        if ( iRewriteFlag )
        {
            // Open a temp file
            hFdTmp = CreateFile(LOG_MON_ROW_DATA_FILE_TMP,
                                GENERIC_READ|GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

            if (hFdTmp == INVALID_HANDLE_VALUE)
            {
                snmp_log_perror(
                       "LOG_MON_ROW_DATA_FILE_TMP file open failed\n");

                // Close the file
                CloseHandle(hFd);

                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_ROW_DATA_FILE));

            }
            else
            {
                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "Thr #%d# File <%s> opened\n",
                    MY_THR_ID,
                    LOG_MON_ROW_DATA_FILE_TMP));

                // Reset the file pointers to begining of the files
                SetFilePointer(hFd,0,NULL,FILE_BEGIN);
                SetFilePointer(hFdTmp,0,NULL,FILE_BEGIN);

                ReadFile(hFd, &stReadCtx, sizeof (logMonRowTable_context),
                        &NumberOfBytesRead, NULL);

                while(NumberOfBytesRead == sizeof (logMonRowTable_context))
                {
                    if ( stReadCtx.logMonRowStatus == SNMP_ROW_ACTIVE )

                        if( WriteFile(hFdTmp, &stReadCtx,
                                    sizeof(logMonRowTable_context),
                                    &NumberOfBytesRead, NULL) == 0 )
                        {
                            snmp_log_perror(
                             "Error writing data to LOG_MON_ROW_DATA_FILE");
                        }

                } // End of while

                // Close the file
                CloseHandle(hFdTmp);
                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_ROW_DATA_FILE_TMP));

                // Delete the old data file
                unlink(LOG_MON_ROW_DATA_FILE);

                // Rename the temp file to original
                rename(LOG_MON_ROW_DATA_FILE_TMP,LOG_MON_ROW_DATA_FILE);

            } // End of else

        } // End of rewrite if

    } // End of else
    // Close the file
    CloseHandle(hFd);

    DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
        "Thr #%d# File <%s> closed\n",
        MY_THR_ID,
        LOG_MON_ROW_DATA_FILE));

}
*/

void
LogMonRow_LoadContainerFromFile()
{
    logMonRowTable_context stReadCtx = {0}; // Read row record from the file
    logMonRowTable_context *pstMyContext = NULL; // Container row context
    HANDLE                  hFd = NULL; // Data file descriptor
    HANDLE                  hFdTmp = NULL; // Temp data file descriptor
    DWORD                   NumberOfBytesRead;
    int                     iRewriteFlag=0; // Whether to rewrite the data file

    // Open the Data file
    hFd = CreateFile   (LOG_MON_ROW_DATA_FILE,
                        GENERIC_READ|GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    if (hFd == INVALID_HANDLE_VALUE)
    {
         snmp_log_perror
            ("LogMonRow_LoadContainerFromFile: \
                LOG_MON_ROW_DATA_FILE file open failed\n");
    }
    else
    {
        DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
            "Thr #%d# File <%s> Opened\n",
            MY_THR_ID,
            LOG_MON_ROW_DATA_FILE));


        while (ReadFile(hFd, &stReadCtx, sizeof (logMonRowTable_context),
                &NumberOfBytesRead, NULL) )
        {


         
           if (NumberOfBytesRead== 0)
                {
                 snmp_log(LOG_ERR," %d breaking out \n",GetLastError());
                break;
                }
           
            

            DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                "Status <%d>\n",stReadCtx.logMonRowStatus));
            // If the row, is already destroyed, no need to create it again.
            if (( stReadCtx.logMonRowStatus == SNMP_ROW_DESTROY ))
            {
                 iRewriteFlag = 1; // to re-write the mib data file
                 continue; // As we have to load the container anyways
            }

            DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                "Loading MIB Row for Log File <%s>\n",
                stReadCtx.logMonRowLogFile));

            pstMyContext = (logMonRowTable_context*)malloc(sizeof (logMonRowTable_context));
            if ( !pstMyContext )
            {
                snmp_log_perror("Malloc failed");
                return;
            }

            memcpy (pstMyContext, &stReadCtx, sizeof (logMonRowTable_context));

            netsnmp_assert(pstMyContext->index.oids == NULL);

            if(snmp_clone_mem( (void*)&pstMyContext->index.oids, &(stReadCtx.idx),
                            stReadCtx.index.len * sizeof(oid) ))
            {
                free(pstMyContext);
                CloseHandle(hFd);
                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_ROW_DATA_FILE));

                return;
            }

            pstMyContext->index.len = stReadCtx.index.len;

            if (CONTAINER_INSERT (cb.container, pstMyContext))
            {
                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "ERROR IN CONTAINER_INSERT FUNCION\n"));
            }

            DEBUGMSGTL(("LogMonRow/Load_Container_From_LogMON_File",
              "OID <%u>\n",*(pstMyContext->index.oids)));

        

        }

        DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                "CONTAINER SIZE=%u\n", CONTAINER_SIZE (cb.container)));

        if ( iRewriteFlag )
        {
            // Open a temp file
            hFdTmp = CreateFile(LOG_MON_ROW_DATA_FILE_TMP,
                                GENERIC_READ|GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

            if (hFdTmp == INVALID_HANDLE_VALUE)
            {
                snmp_log_perror(
                       "LOG_MON_ROW_DATA_FILE_TMP file open failed\n");

                // Close the file
                CloseHandle(hFd);

                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_ROW_DATA_FILE));

            }
            else
            {
                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "Thr #%d# File <%s> opened\n",
                    MY_THR_ID,
                    LOG_MON_ROW_DATA_FILE_TMP));

                // Reset the file pointers to begining of the files
                SetFilePointer(hFd,0,NULL,FILE_BEGIN);
                SetFilePointer(hFdTmp,0,NULL,FILE_BEGIN);

                

                while(ReadFile(hFd, &stReadCtx, sizeof (logMonRowTable_context),
                        &NumberOfBytesRead, NULL))
                {
                        if (NumberOfBytesRead== 0)
                        {
                         snmp_log(LOG_ERR," %d breaking out \n",GetLastError());
                        break;
                        }
           
            
                    if ( stReadCtx.logMonRowStatus == SNMP_ROW_ACTIVE )

                        if( WriteFile(hFdTmp, &stReadCtx,
                                    sizeof(logMonRowTable_context),
                                    &NumberOfBytesRead, NULL) == 0 )
                        {
                            snmp_log_perror(
                             "Error writing data to LOG_MON_ROW_DATA_FILE");
                        }

                } // End of while

                // Close the file
                CloseHandle(hFdTmp);
                DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_ROW_DATA_FILE_TMP));

                // Delete the old data file
                unlink(LOG_MON_ROW_DATA_FILE);

                // Rename the temp file to original
                rename(LOG_MON_ROW_DATA_FILE_TMP,LOG_MON_ROW_DATA_FILE);

            } // End of else

        } // End of rewrite if

    } // End of else
    // Close the file
    CloseHandle(hFd);

    DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
        "Thr #%d# File <%s> closed\n",
        MY_THR_ID,
        LOG_MON_ROW_DATA_FILE));

}

#endif


/*****************************************************************************
 *
 * name             :   LogMonRow_SearchPatterns
 * description      :   This function is used to search the patterns in the
 *                      input file
 * input parameters :   logMonRowTable_context *logMonRowRow
 * output parameters:   None
 * return type      :   int - 1 for pattern found, 0 for not found, -1 for error
 * global variables :   None
 * calls            :   LogMonRow_GetCurrDateTime
 */

int
LogMonRow_SearchPatterns(logMonRowTable_context *logMonRowRow)
{

   /*
    FILE                *pfOutFd = NULL; // Output file descriptor
   */
    FILE                *psInFd = NULL; // Input file descriptor
    char                acReadBuffer[LINE_MAX_LEN+1]; // the line read
    struct stat         pstStatBuf = {0};
    time_t              now={0};
    int                 iStrFound=0;
    int                 iLineNum=0; // Current line number
    int                 iLidx=0;
	stVarBindArgProc    *varBinds = NULL;


    // Find the input log file size and check for error

	/** modified by D.J Park in 2004.03.30
    if(stat(logMonRowRow->logMonRowLogFile,&pstStatBuf)==-1)
	*/
    if(stat(logMonRowRow->acChangedFileName,&pstStatBuf)==-1)
    {

        DEBUGMSGTL(("LogMonRow/LogMonRow_SearchPatterns",
            "Thr #%d# File <%s> closed\n",
            MY_THR_ID,
            logMonRowRow->logMonRowLogFile));

    //    free(pstStatBuf);

        DEBUGMSGTL(("LogMonRow/LogMonRow_SearchPatterns",
                    "Error doing stat on <%s>",
                    logMonRowRow->logMonRowLogFile));

        return (-1);
    }

    /* CR2 : Whenever the file name is changed, start monitoring from the beginning */
    /* CR1 : If the file size is lesser than last time, start the monitoring from begining */
    /* CR3 : When monitoring starts, Move to end-of-file and do not proceed with the monitoring for the first time */
    
	
	/**
	snmp_log(LOG_INFO,"logMonRowFileSize[%ld]\n",
		logMonRowRow->logMonRowFileSize);
	snmp_log(LOG_INFO,"pstStatBuf.st_size[%ld]\n",pstStatBuf.st_size);
	snmp_log(LOG_INFO,"logMonRowReadLine[%ld]\n",
		logMonRowRow->logMonRowReadLine);
	**/
	
    if(logMonRowRow->logMonRowFileSize > pstStatBuf.st_size || logMonRowRow->logMonRowReadLine == 0 )
    {
		/** modified by D.J Park in 2004.03.30
        logMonRowRow->logMonRowReadLine = LogMonRow_GetLineCount(logMonRowRow->logMonRowLogFile);    
		*/
		if(logMonRowRow->isStart)
		{
			/*
			snmp_log(LOG_INFO,"logMonRowTable THR<%ld> start and read line\n",
				MY_THR_ID);
			*/
			SLEEP(rand() % 60);

        	logMonRowRow->logMonRowReadLine = LogMonRow_GetLineCountAndSeek(
				logMonRowRow->acChangedFileName,
				&logMonRowRow->logMonRowReadLineSeek);
			logMonRowRow->isStart = 0;
			snmp_log(LOG_INFO,
				"START LOGMON :: THR<%ld> FILE[%s] LINE[%ld] SIZE[%ld]\n",
				MY_THR_ID,logMonRowRow->acChangedFileName,
				logMonRowRow->logMonRowReadLine, 
				logMonRowRow->logMonRowReadLineSeek);
		} 
		else
		{
        	logMonRowRow->logMonRowReadLine = 0;
			logMonRowRow->logMonRowReadLineSeek = 0;
		}
        
        // Now, update the container with the latest results
        logMonRowRow->logMonRowFileSize=pstStatBuf.st_size;
  
	
        // Row Modified Time
        logMonRowRow->logMonRowUpdatedTime_len=MAX_DATE_TIME_LEN;

        (void *) time(&now);

        strcpy(logMonRowRow->logMonRowUpdatedTime,
        (u_char *) date_n_time(&now,
        (size_t*)&(logMonRowRow->logMonRowUpdatedTime_len)));

        // Update the line number
        logMonRowRow->logMonRowGetLine = logMonRowRow->logMonRowReadLine;
        logMonRowRow->logMonRowLine = 0;
         
		if(logMonRowRow->logMonRowReadLineSeek==logMonRowRow->logMonRowFileSize)
        	return(0);
    }
    
       
    
    // Open the input log file
	/** modified by D.J Park in 2004.03.30
    if( (psInFd = fopen(logMonRowRow->logMonRowLogFile,"r") ) == NULL)
	*/
    if( (psInFd = fopen(logMonRowRow->acChangedFileName,"r") ) == NULL)
    {
        // Error opening input log file
        DEBUGMSGTL(("LogMonRow/LogMonRow_SearchPatterns",
           "Error opening file <%s> <%d>\n",
           logMonRowRow->logMonRowLogFile,errno));

     //   free(pstStatBuf);

        return (-1);
    }
    else
        DEBUGMSGTL(("LogMonRow/LogMonRow_SearchPatterns",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            logMonRowRow->logMonRowLogFile));


    // Check file size before opening file,
    // if its bigger transfer the file before opening
	/* modified by D.J Park in 2004.03.30
    Common_CheckFileSize(gcInfoLogFileMonRow);
	*/

    // Open the output log file
    /**
      * rewrite by D.J Park in 2004.01.31
    if( (pfOutFd = fopen(gcInfoLogFileMonRow,"a") ) == NULL)
    {

       // Error opening output log file
       FCLOSE(psInFd);

       DEBUGMSGTL(("LogMonRow/LogMonRow_SearchPatterns",
                    "Error opening Output Log File <%s>\n",
                    gcInfoLogFileMonRow));
    }
    else
        DEBUGMSGTL(("LogMonRow/LogMonRow_SearchPatterns",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            gcInfoLogFileMonRow));
    */



    // Set the file pointer to the matched line, last time - CR4: always search till the EOF
    iLineNum = logMonRowRow->logMonRowReadLine;

    // Skip all the lines read already
	/** modified by D.J Park in 2004.03.31
    for(iLidx=1;iLidx<=iLineNum;iLidx++)
    {
       if ( fgets(acReadBuffer,LINE_MAX_LEN,psInFd) == NULL )
           break;
    }
	*/
	if(pstStatBuf.st_size < logMonRowRow->logMonRowReadLineSeek)
	{
		snmp_log(LOG_ERR,"logFile[%s] size[%ld] readLineSeek[%ld]\n",
			logMonRowRow->acChangedFileName,pstStatBuf.st_size,
			logMonRowRow->logMonRowReadLineSeek);
		return 0;
	}
	/*
	snmp_log(LOG_INFO,"line[%ld] seek[%ld]\n",
		logMonRowRow->logMonRowReadLine,logMonRowRow->logMonRowReadLineSeek);
	*/
	fseek(psInFd,logMonRowRow->logMonRowReadLineSeek,SEEK_SET);

    iStrFound = 0;
    // Read the Log file and find the pattern
    do
    {
       if(fgets(acReadBuffer,LINE_MAX_LEN,psInFd) == NULL)
       {
           // End of File reached or File read error
           // Free the memory and close files

           if(psInFd)
		   {
				/** added by D.J Park in 2004.03.30 [start]*/
				logMonRowRow->logMonRowReadLineSeek = ftell(psInFd);
				/** added by D.J Park in 2004.03.30 [end]*/
                FCLOSE(psInFd);
		   }

            DEBUGMSGTL(("LogMonRow/LogMonRow_SearchPatterns",
                "Thr #%d# File <%s> closed\n",
                MY_THR_ID,
                logMonRowRow->logMonRowLogFile));

 	   /**
             * rewrite by D.J Park in 2004.01.30
           if(pfOutFd)
               FCLOSE(pfOutFd);
            DEBUGMSGTL(("LogMonRow/LogMonRow_SearchPatterns",
                "Thr #%d# File <%s> closed\n",
                MY_THR_ID,
                gcInfoLogFileMonRow));
	    */

           break;
       }
       else
       {
           iLineNum++;

           // Check if the match is present in
           // the current line read
           /** modified by D.J Park in 2005.03.02 [start] */
/*
#ifdef WIN32
           if(strstr(acReadBuffer,logMonRowRow->logMonRowFilter)!=NULL)
#else // Only for Unix
           if(Common_CheckPattern(acReadBuffer, logMonRowRow->logMonRowFilter)==1)
#endif
*/
#ifdef WIN32
           if((strlen(logMonRowRow->logMonRowFilter) > 0) && (strstr(acReadBuffer,logMonRowRow->logMonRowFilter)!=NULL))
#else // Only for Unix
           if((strlen(logMonRowRow->logMonRowFilter) > 0) && (Common_CheckPattern(acReadBuffer, logMonRowRow->logMonRowFilter)==1))
#endif
           /** modified by D.J Park in 2005.03.02 [end] */
           {
               /**added by D.J Park in 2005.02.22 [start] 
                * filterCounter에 설정된 횟수 만큼의 log pattern 발견시 
                * trap을 보낸다.
                */
               logMonRowRow->filterCounter++;
               logMonRowRow->filterCounter = (logMonRowRow->filterCounter)%(logMonRowRow->logMonRowFilterCounter);
               /**added by D.J Park in 2005.02.22 [end] */
               if(logMonRowRow->filterCounter == 0)
               {
			        varBinds = makeLogMonRowVarBind(logMonRowRow,
                            acReadBuffer,iLineNum);
     		        if (Common_SendAlert_And_Vars("LGRM", gcInfoLogFileMonRow, 
                                logMonRowRow->logMonRowAlertMessage, 
                                logMonRowRow->logMonRowIndex,varBinds,6)) 
                    { 
                        snmp_log(LOG_ERR,"Alert sending failed for <%s>\n", 
                                logMonRowRow->acChangedFileName); 
                    }        
                    if(varBinds != NULL) 
                        free(varBinds);
               }
               // Set the string found flag - 
               // atleast once the pattern got matched
               iStrFound = iLineNum;
           }

       }

    } while(1); // Until we find a match, or
               // End-Of-File is reached


    // close files
    if(psInFd)
	{
		/** added by D.J Park in 2004.03.30 [start]*/
		logMonRowRow->logMonRowReadLineSeek = ftell(psInFd);
		/** added by D.J Park in 2004.03.30 [end]*/
        FCLOSE(psInFd);
	}

    DEBUGMSGTL(("LogMonRow/LogMonRow_SearchPatterns",
         "Thr #%d# File <%s> closed\n",MY_THR_ID,logMonRowRow->logMonRowLogFile));

    /**
     * rewrite by D.J Park in 2004.01.30
    if(pfOutFd != NULL)
       FCLOSE(pfOutFd);
     */
    
    DEBUGMSGTL(("LogMonRow/LogMonRow_SearchPatterns",
         "Thr #%d# File <%s> closed\n",MY_THR_ID,gcInfoLogFileMonRow));

    /* Last row that matched */ 
    if(iStrFound)
    	logMonRowRow->logMonRowLine = iStrFound;
    
    // Now, update the container with the latest results
    logMonRowRow->logMonRowFileSize=pstStatBuf.st_size;
  
	
    // Row Modified Time
    logMonRowRow->logMonRowUpdatedTime_len=MAX_DATE_TIME_LEN;

    (void *) time(&now);

    strcpy(logMonRowRow->logMonRowUpdatedTime,
        (u_char *) date_n_time(&now,
        (size_t*)&(logMonRowRow->logMonRowUpdatedTime_len)));

    // Update the line number
    logMonRowRow->logMonRowGetLine = iLineNum;
          

    // Last line read
    logMonRowRow->logMonRowReadLine = iLineNum;

    // Free the buffer
    //free(pstStatBuf);

    return (iStrFound);
}

/** rewrite by D.J Park in 2004.1.30
 */
stVarBindArgProc *makeLogMonRowVarBind(logMonRowTable_context *logMonRowRow, 
	char *read_line, int read_line_num)
{
	stVarBindArgProc* varBinds = (stVarBindArgProc *)NULL;
	int var_cnt = 6;

	varBinds = (stVarBindArgProc *)malloc(sizeof(stVarBindArgProc) * var_cnt);

	memset(varBinds,0x00,sizeof(stVarBindArgProc) * var_cnt);

	strcpy(varBinds[0].variable,".1.3.6.1.4.1.3204.1.3.35.2.2.1.2");
	varBinds[0].type='s';
	/** modified by D.J Park in 2004.03.21 
	strcpy(varBinds[0].value,logMonRowRow->logMonRowLogFile);
	*/
	strcpy(varBinds[0].value,logMonRowRow->acChangedFileName);

	strcpy(varBinds[1].variable,".1.3.6.1.4.1.3204.1.3.35.2.2.1.4");
	varBinds[1].type='s';
	strcpy(varBinds[1].value,logMonRowRow->logMonRowFilter);

	strcpy(varBinds[2].variable,".1.3.6.1.4.1.3204.1.3.35.2.2.1.5");
	varBinds[2].type='s';
	strncpy(varBinds[2].value,read_line,299);

	strcpy(varBinds[3].variable,".1.3.6.1.4.1.3204.1.3.35.2.2.1.7");
	varBinds[3].type='s';
	strcpy(varBinds[3].value,logMonRowRow->logMonRowAlertMessage);

	strcpy(varBinds[4].variable,".1.3.6.1.4.1.3204.1.3.35.2.2.1.10");
	varBinds[4].type='i';
	sprintf(varBinds[4].value,"%d",read_line_num);

    /** added by D.J Park in 2005.02.22 [start] */
	strcpy(varBinds[5].variable,".1.3.6.1.4.1.3204.1.3.35.2.2.1.16");
	varBinds[5].type='i';
	sprintf(varBinds[5].value,"%d",1);
    /** added by D.J Park in 2005.02.22 [start] */

	return varBinds;
}

/** added by D.J Park in 2005.02.22
 */
stVarBindArgProc *makeThresholdLogMonRowVarBind(logMonRowTable_context *logMonRowRow, int trapType)
{
	stVarBindArgProc* varBinds = (stVarBindArgProc *)NULL;
	int var_cnt = 6;

	varBinds = (stVarBindArgProc *)malloc(sizeof(stVarBindArgProc) * var_cnt);

	memset(varBinds,0x00,sizeof(stVarBindArgProc) * var_cnt);

	strcpy(varBinds[0].variable,".1.3.6.1.4.1.3204.1.3.35.2.2.1.2");
	varBinds[0].type='s';
	strcpy(varBinds[0].value,logMonRowRow->acChangedFileName);

	strcpy(varBinds[1].variable,".1.3.6.1.4.1.3204.1.3.35.2.2.1.4");
	varBinds[1].type='s';
	strcpy(varBinds[1].value,logMonRowRow->logMonRowFilter);

	strcpy(varBinds[2].variable,".1.3.6.1.4.1.3204.1.3.35.2.2.1.7");
	varBinds[2].type='s';
	strcpy(varBinds[2].value,logMonRowRow->logMonRowAlertMessage);

	strcpy(varBinds[3].variable,".1.3.6.1.4.1.3204.1.3.35.2.2.1.12");
	varBinds[3].type='i';
	sprintf(varBinds[3].value,"%d",logMonRowRow->logMonRowFileSize);

	strcpy(varBinds[4].variable,".1.3.6.1.4.1.3204.1.3.35.2.2.1.14");
	varBinds[4].type='i';
	sprintf(varBinds[4].value,"%d",logMonRowRow->logMonRowFileSizeThreshold);

	strcpy(varBinds[5].variable,".1.3.6.1.4.1.3204.1.3.35.2.2.1.16");
	varBinds[5].type='i';
	sprintf(varBinds[5].value,"%d",trapType);

	return varBinds;
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

#ifndef WIN32

int
LogMonRow_UpdateDataFile(logMonRowTable_context *pstLogRowRow)
{
    logMonRowTable_context stLogRowInFile;
    int                 iFd=-1;
    int                 iFoundFlag=0;
    int                 iRowNum=0;
    unsigned long       ulPos=0;

    // Open the MIB data file
    if ( (iFd = open(LOG_MON_ROW_DATA_FILE,O_CREAT|O_RDWR,S_IREAD|S_IWRITE)) < 0 )
    {
        snmp_log_perror("Error opening LOG_MON_ROW_DATA_FILE");
        return(-1);

    }
    else
    {
        DEBUGMSGTL(("LogMonRow/LogMonRow_UpdateDataFile",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            LOG_MON_ROW_DATA_FILE));

        // Set the file descriptor to begining of the file
        lseek(iFd, 0,SEEK_SET);

        if ( pstLogRowRow->logMonRowStatus == SNMP_ROW_DESTROY )
        {
            DEBUGMSGTL(("LogMonRow/LogMonRow_LoadContainerFromFile",
                "Thr #%d# Found row status as destroyed <%d>\n",
                MY_THR_ID,
                pstLogRowRow->logMonRowStatus));

            pstLogRowRow->iThreadActiveFlag = 0;
        }

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
                    DEBUGMSGTL(("LogMonRow/LogMonRow_UpdateDataFile",
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
            // Close the file
            close(iFd);
            DEBUGMSGTL(("LogMonRow/LogMonRow_UpdateDataFile",
                "Thr #%d# File <%s> closed\n",
                MY_THR_ID,
                LOG_MON_ROW_DATA_FILE));

            return(-1);
        }
        else
            DEBUGMSGTL(("LogMonRow/LogMonRow_UpdateDataFile",
                "Record written\n"));
    }
    // Close the file
    close(iFd);

    DEBUGMSGTL(("LogMonRow/LogMonRow_UpdateDataFile",
        "Thr #%d# File <%s> closed\n",
        MY_THR_ID,
        LOG_MON_ROW_DATA_FILE));

    return(iFoundFlag);
}

/*****************************************************************************
 *
 * name             :   Common_CheckPattern
 * description      :   This function returns 1 if the pattern is found in string
 *                      else returns 0.
 * input parameters :   char *szString, char *szPattern
 * output parameters:   None
 * return type      :   int - "0" for pattern not found,
 *                          - "1" for pattern found
 * global variables :   None
 * calls            :   None
 */

int Common_CheckPattern(char *szString, char *szPattern) {
    int iPatternFound=0;
    int iPatternCompileResult=0;
    regex_t pstReg;
    regmatch_t stMatches[1];
    DEBUGMSGTL(("LogMonRow", "Inside LogMonRow_CheckPattern string:%s pattern:%s\n", szString, szPattern));
    if ((iPatternCompileResult = regcomp(&pstReg, szPattern, REG_EXTENDED)) == 0) {
	    if(regexec(&pstReg, szString, 1, stMatches,0)==0) {
	        DEBUGMSGTL(("LogMonRow", "Inside LogMonRow_CheckPattern found match\n"));
	        iPatternFound=1;
	    }
	    
	    regfree(&pstReg);
	} else {
		DEBUGMSGTL(("LogMonRow", "Inside LogMonRow_CheckPattern couldn't compile\n"));
		snmp_log(LOG_ERR, "Common_CheckPattern regular expression complie error, error_code=%d\n", iPatternCompileResult);
	}
    
    return iPatternFound;
}

#else

int
LogMonRow_UpdateDataFile(logMonRowTable_context *pstLogRowRow)
{
    logMonRowTable_context stLogRowInFile;
    HANDLE              hFd = NULL;
    DWORD               NumberOfBytesRead;
    int                 iFoundFlag=0;
    int                 iRowNum=0;
    unsigned long       ulPos=0;

    // Open the MIB data file
    hFd = CreateFile   (LOG_MON_ROW_DATA_FILE,
                        GENERIC_READ|GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    if(hFd == INVALID_HANDLE_VALUE)
    {
        snmp_log_perror("Error opening LOG_MON_ROW_DATA_FILE");
        return(-1);
    }
    else
    {
        DEBUGMSGTL(("LogMonRow/LogMonRow_UpdateDataFile",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            LOG_MON_ROW_DATA_FILE));

        // Set the file descriptor to begining of the file
        SetFilePointer(hFd,0,NULL,FILE_BEGIN);
        ReadFile(hFd, &stLogRowInFile, sizeof (logMonRowTable_context),
                &NumberOfBytesRead, NULL);

        if ( pstLogRowRow->logMonRowStatus == SNMP_ROW_DESTROY )
            pstLogRowRow->iThreadActiveFlag = 0;

        while (NumberOfBytesRead == sizeof (logMonRowTable_context))
        {
            if ( stLogRowInFile.idx == *pstLogRowRow->index.oids )
            {

                // Check if the row entry in the file
                // is actually deleted fromthe container
                if ( stLogRowInFile.logMonRowStatus == SNMP_ROW_DESTROY )
                    iFoundFlag =0;
                else
                    iFoundFlag =1;

                if ( SetFilePointer(hFd,ulPos,NULL,FILE_BEGIN) == (ulPos -1) )
                {
                    snmp_log_perror(
                        "Error while re-setting file descriptor position");
                }
                else
                {
                    DEBUGMSGTL(("LogMonRow/LogMonRow_UpdateDataFile",
                        "OID <%u> found FoundFlag <%d>\n",
                            *pstLogRowRow->index.oids,
                            iFoundFlag));

                }
                break;
            }
            ulPos = SetFilePointer(hFd,0,NULL,FILE_CURRENT);
            ReadFile(hFd, &stLogRowInFile, sizeof (logMonRowTable_context),
                &NumberOfBytesRead, NULL);
        }

        // Copy the oid, instead copying the pointer
        pstLogRowRow->idx = *(pstLogRowRow->index.oids);

        if ( WriteFile (hFd,
                        pstLogRowRow,
                        sizeof(logMonRowTable_context),
                        &NumberOfBytesRead,
                        NULL) == 0 )
        {
            snmp_log_perror("Error while writing data to LOG_MON_ROW_DATA_FILE");
            // Close the file
            CloseHandle(hFd);
            DEBUGMSGTL(("LogMonRow/LogMonRow_UpdateDataFile",
                "Thr #%d# File <%s> closed\n",
                MY_THR_ID,
                LOG_MON_ROW_DATA_FILE));

            // Unlock
            return(-1);
        }
        else
            DEBUGMSGTL(("LogMonRow/LogMonRow_UpdateDataFile",
                "Record written\n"));
    }
    // Close the file
    CloseHandle(hFd);
    DEBUGMSGTL(("LogMonRow/LogMonRow_UpdateDataFile",
        "Thr #%d# File <%s> closed\n",
        MY_THR_ID,
        LOG_MON_ROW_DATA_FILE));


    return(iFoundFlag);
}

#endif
/*****************************************************************************
 * name             :   RTM_Get_Uptime_Win
 * description      :   To get system uptime for windows
 * input parameters :
 * output parameters:
 * return type      :   long
 * global variables :
 * calls            :   send_snmptrap
 *****************************************************************************/

#ifdef WIN32
static long
RTM_Get_Uptime_Win ()
{
    long                uptim = 0;

    uptim = GetTickCount();
    uptim = (long)(uptim/10);

    return uptim;
}
#endif /* WIN32 */


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

    // Register an alarm for log file transfer if
    // not registered already
    if ( (glAgentInfoFileTrPort > 0) &&
         (strlen(gcAgentInfoFileTransferIP) > 0))
    {
        DEBUGMSGTL(("LogMonRow/init_logMonRowTable",
            "Registering the alarm for LogMonRow Log file transfer\n"));
        if (glAgentInfoFileTrInterval > 0 )
            guiLogFileLogRowAlarmReg = snmp_alarm_register (glAgentInfoFileTrInterval,
            (int)NULL,Common_SendFileThread, gcInfoLogFileMonRow);
        else
            guiLogFileLogRowAlarmReg = snmp_alarm_register (1,
            (int)NULL,Common_SendFileThread, gcInfoLogFileMonRow);

    }

    // End: WIP_NURI_ALL

}

// End: WIP_NURI_SOL : End of the newly added code here

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
size_t strtimeformat(char  * s, size_t maxsize, const char  * format)
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
	for(j = 0 ,i = 0 ; i < format_len ; i++)
	{
		if(format[i] == '%')
		{
			switch(format[i+1])
			{
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
		} else {
			tempbuf[j++]=format[i];			
		}
	}
	strcpy(s,tempbuf);

	if(tempbuf)
		free(tempbuf);
	
	return strlen(s);
}

/*****************************************************************************
 *
 * LogMonColTable.c
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
 * $Author: howonsuk $
 * $Log: logMonColumnTable.c,v $
 * Revision 1.1  2004/03/03 06:13:47  howonsuk
 * 로그모니터링: 정규식 추가
 * 프로세스모니터링 : 파라미터보여주기 및 파라미터에서 정규식으로 찾기 추가
 *
 * Revision 1.38  2004/01/30 11:06:44  karthis
 * createawaitandgo probelm fixed readfile moved to while and readfile in while loop is removed in windows part
 * (loadfromcontainerfile) for update and delete
 *
 * Revision 1.36  2004/01/23 05:06:40  viren
 * added macro for flcose, and null check after each malloc - reviewed by Vijay
 *
 * Revision 1.35  2004/01/21 04:15:31  buvanes
 * File size is updated after pattern monitoring
 *
 * Revision 1.34  2004/01/20 08:11:55  buvanes
 * CR: File Name changes during run time handled
 *
 * Revision 1.33  2004/01/20 04:02:37  buvanes
 * Implemented CRs - searching till EOFand sending Traps
 *
 * Revision 1.32  2004/01/13 09:26:19  viren
 * changed pstStatBuf to stStatBuf ( pointer to structure )
 *
 * Revision 1.31  2004/01/05 04:32:38  saisk
 * Made all includes related to windows as relative path
 *
 * Revision 1.30  2004/01/03 13:11:17  viren
 * Resolved the AT Issue of Row Status
 *
 * Revision 1.29  2004/01/02 08:01:14  viren
 * mistake while putting the "#ifdef"
 *
 * Revision 1.28  2004/01/01 10:48:25  viren
 * enclosed call to Common_CheckPattern func in "#ifdef" for Unix only
 *
 * Revision 1.27  2003/12/23 03:33:23  sig
 * *** empty log message ***
 *
 * Revision 1.26  2003/12/17 09:30:30  viren
 * closed a file descriptor
 *
 * Revision 1.25  2003/12/16 15:05:35  viren
 * MasterThread function was modified to check for valid data before spawning a thread
 *
 * Revision 1.24  2003/12/13 19:12:54  viren
 * line - 1386 - wrong debug mesg
 *
 * Revision 1.23  2003/12/13 19:04:18  viren
 * If col length was given 0, next col was not being considered for matching - Fixed
 *
 * Revision 1.18  2003/12/12 15:45:02  viren
 * silly mistake
 *
 * Revision 1.17  2003/12/12 15:31:59  viren
 * LogMon thread was not creating - Fixed
 *
 * Revision 1.16  2003/12/12 09:36:18  viren
 * opene logfile in append mode
 *
 * Revision 1.15  2003/12/11 07:38:50  viren
 * Added few more debug information
 *
 * Revision 1.14  2003/12/09 20:08:06  viren
 * Fixed close file issue
 *
 * Revision 1.13  2003/12/09 20:04:29  viren
 * Changed the location of init func.
 *
 * Revision 1.12  2003/12/09 19:49:47  viren
 * file close problems fixed
 *
 * Revision 1.11  2003/12/09 13:51:10  ashok
 * fclsoe is removed from LogMonCol_SearchPatterns
 *
 * Revision 1.8  2003/12/09 11:34:37  ashok
 * File operations changed for windows & Line 2079 close() moved to Line. 2094
 *
 * Revision 1.7  2003/12/08 18:21:41  viren
 * Updated review comments
 *
 * Revision 1.6  2003/12/02 10:54:43  vijaysp
 * Trap msg. format changed. TrapOID value changed from Nuri OID to table oid and arg. changed to alert message string alone
 *
 * Revision 1.5  2003/12/02 06:28:13  vijaysp
 * Bug found in windows testing solved. line 2290 changed
 *
 * Revision 1.4  2003/12/01 15:42:03  vijaysp
 * Trap Msg. is sent instread of AlertMsg. pthread_mutex_unlock added.
 *
 * Revision 1.3  2003/11/28 06:47:48  ashok
 * Minor Changes
 *
 * Revision 1.2  2003/11/26 07:45:13  ashok
 * Added Windows Functionality
 *
 * Revision 1.1  2003/11/24 06:41:59  viren
 * moved from old directories
 *
 * Revision 1.5  2003/11/21 12:32:47  viren
 * latest
 *
 * Revision 1.4  2003/11/20 16:38:35  viren
 * Hopefully stable
 *
 * Revision 1.15  2003/11/07 09:54:00  viren
 * Updated with the latest MIB
 *
 * Revision 1.13  2003/11/06 13:16:21  viren
 * Fixed all known bugs
 *
 * Revision 1.12  2003/11/06 07:00:23  sig
 * LogMonColTable.c has been modified for linux
 *
 * Revision 1.11  2003/11/04 13:15:00  viren
 * Header File name changed in include
 *
 * Revision 1.10  2003/11/04 13:11:54  viren
 * After beautification
 *
 * $State: Exp $
 * $Id: logMonColumnTable.c,v 1.1 2004/03/03 06:13:47 howonsuk Exp $
 * $Name:  $
 *
 ****************************************************************************/
// <Start: WIP_NURI_ALL: Added REENTRANT>
/********************************* includes *********************************/

#define _REENTRANT

// <End: WIP_NURI_ALL>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <net-snmp/library/snmp_assert.h>
#include <net-snmp/library/snmp.h>
#ifdef WIN32
#include "../inc/logMonColumnTable.h"
#else
#include "logMonColumnTable.h"
#endif

#define FCLOSE(x) fclose(x);\
                  x=NULL

static     netsnmp_handler_registration *my_handler = NULL;
static     netsnmp_table_array_callbacks cb;

static oid  objid_sysuptime[] = { 1, 3, 6, 1, 2, 1, 1, 3, 0 };
static oid  objid_snmptrap[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };

oid logMonColumnTable_oid[] = { logMonColumnTable_TABLE_OID };
size_t logMonColumnTable_oid_len = OID_LENGTH(logMonColumnTable_oid);


#ifdef logMonColumnTable_IDX2
/************************************************************
 * keep binary tree to find context by name
 */
static int logMonColumnTable_cmp( const void *lhs, const void *rhs );

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int
logMonColumnTable_cmp( const void *lhs, const void *rhs )
{
    logMonColumnTable_context *context_l =
        (logMonColumnTable_context *)lhs;
    logMonColumnTable_context *context_r =
        (logMonColumnTable_context *)rhs;

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
             "logMonColumnTable_compare not implemented! Container order undefined\n" );
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
logMonColumnTable_context *
logMonColumnTable_get( const char *name, int len )
{
    logMonColumnTable_context tmp;

    /** we should have a secondary index */
    netsnmp_assert(cb.container->next != NULL);

    /*
     * TODO: implement compare. Remove this ifdef code and
     * add your own code here.
     */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "logMonColumnTable_get not implemented!\n" );
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
 * Initializes the logMonColumnTable module
 */
void
init_logMonColumnTable(void)
{
    initialize_table_logMonColumnTable();

    /*
     * TODO: perform any startup stuff here
     */
    // Load the MIB from data file
    LogMonCol_LoadContainerFromFile();

    // Start the threads for default MIB
    LogMonCol_MasterThread();
    // Register an alarm for log file transfer if
    // not registered already
    if ( (glAgentInfoFileTrPort > 0) &&
         (strlen(gcAgentInfoFileTransferIP) > 0))
    {
        DEBUGMSGTL(("LogMonCol/init_logMonColumnTable",
            "Registering the alarm for LogMonCol Log file transfer\n"));
        if (glAgentInfoFileTrInterval > 0 )
            guiLogFileColRowAlarmReg = snmp_alarm_register (glAgentInfoFileTrInterval,
            (int)NULL,Common_SendFileThread, gcInfoLogFileMonCol);
        else
            guiLogFileColRowAlarmReg = snmp_alarm_register (1,
            (int)NULL,Common_SendFileThread, gcInfoLogFileMonCol);

    }

}


/************************************************************
 * the *_row_copy routine
 */
static int logMonColumnTable_row_copy(logMonColumnTable_context * dst,
                         logMonColumnTable_context * src)
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
    dst->logMonColumnIndex = src->logMonColumnIndex;

    memcpy( dst->logMonColumnLogFile, src->logMonColumnLogFile, src->logMonColumnLogFile_len );
    dst->logMonColumnLogFile_len = src->logMonColumnLogFile_len;

    dst->logMonColumnReadInterval = src->logMonColumnReadInterval;

    dst->logMonColumnCol1 = src->logMonColumnCol1;

    dst->logMonColumnCol2 = src->logMonColumnCol2;

    dst->logMonColumnCol3 = src->logMonColumnCol3;

    dst->logMonColumnCol4 = src->logMonColumnCol4;

    dst->logMonColumnCol5 = src->logMonColumnCol5;

    memcpy( dst->logMonColumnColFilter1, src->logMonColumnColFilter1, src->logMonColumnColFilter1_len );
    dst->logMonColumnColFilter1_len = src->logMonColumnColFilter1_len;

    memcpy( dst->logMonColumnColFilter2, src->logMonColumnColFilter2, src->logMonColumnColFilter2_len );
    dst->logMonColumnColFilter2_len = src->logMonColumnColFilter2_len;

    memcpy( dst->logMonColumnColFilter3, src->logMonColumnColFilter3, src->logMonColumnColFilter3_len );
    dst->logMonColumnColFilter3_len = src->logMonColumnColFilter3_len;

    memcpy( dst->logMonColumnColFilter4, src->logMonColumnColFilter4, src->logMonColumnColFilter4_len );
    dst->logMonColumnColFilter4_len = src->logMonColumnColFilter4_len;

    memcpy( dst->logMonColumnColFilter5, src->logMonColumnColFilter5, src->logMonColumnColFilter5_len );
    dst->logMonColumnColFilter5_len = src->logMonColumnColFilter5_len;

    memcpy( dst->logMonColumnCreatedTime, src->logMonColumnCreatedTime, src->logMonColumnCreatedTime_len );
    dst->logMonColumnCreatedTime_len = src->logMonColumnCreatedTime_len;

    memcpy( dst->logMonColumnDescription, src->logMonColumnDescription, src->logMonColumnDescription_len );
    dst->logMonColumnDescription_len = src->logMonColumnDescription_len;

    memcpy( dst->logMonColumnAlertMessage, src->logMonColumnAlertMessage, src->logMonColumnAlertMessage_len );
    dst->logMonColumnAlertMessage_len = src->logMonColumnAlertMessage_len;

    dst->logMonColumnStatus = src->logMonColumnStatus;

    memcpy( dst->logMonColumnUpdatedTime, src->logMonColumnUpdatedTime, src->logMonColumnUpdatedTime_len );
    dst->logMonColumnUpdatedTime_len = src->logMonColumnUpdatedTime_len;

    dst->logMonColumnFileSize = src->logMonColumnFileSize;

    dst->logMonColumnReadLine = src->logMonColumnReadLine;

    return 0;
}

#ifdef logMonColumnTable_SET_HANDLING

/*
 * the *_extract_index routine
 */
int
logMonColumnTable_extract_index( logMonColumnTable_context * ctx, netsnmp_index * hdr )
{
    /*
     * temporary local storage for extracting oid index
     */
    netsnmp_variable_list var_logMonColumnIndex;
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
       memset( &var_logMonColumnIndex, 0x00, sizeof(var_logMonColumnIndex) );
       var_logMonColumnIndex.type = ASN_UNSIGNED;
       /** TODO: link this index to the next, or NULL for the last one */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "logMonColumnTable_extract_index index list not implemented!\n" );
    return 0;
#else
       // <Start: WIP_NURI_ALL: Commented>
       //var_logMonColumnIndex.next_variable = &var_XX;
       // <End: WIP_NURI_ALL>
#endif


    /*
     * parse the oid into the individual components
     */
    err = parse_oid_indexes( hdr->oids, hdr->len, &var_logMonColumnIndex );
    if (err == SNMP_ERR_NOERROR) {
       /*
        * copy components into the context structure
        */
                ctx->logMonColumnIndex = *var_logMonColumnIndex.val.integer;


           /*
            * TODO: check index for valid values. For EXAMPLE:
            *
              * if ( *var_logMonColumnIndex.val.integer != XXX ) {
          *    err = -1;
          * }
          */
    }

    /*
     * parsing may have allocated memory. free it.
     */
    snmp_reset_var_buffers( &var_logMonColumnIndex );

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
int logMonColumnTable_can_activate(logMonColumnTable_context *undo_ctx,
                      logMonColumnTable_context *row_ctx,
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
int logMonColumnTable_can_deactivate(logMonColumnTable_context *undo_ctx,
                        logMonColumnTable_context *row_ctx,
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
int logMonColumnTable_can_delete(logMonColumnTable_context *undo_ctx,
                    logMonColumnTable_context *row_ctx,
                    netsnmp_request_group * rg)
{
    /*
     * probably shouldn't delete a row that we can't
     * deactivate.
     */
    if(logMonColumnTable_can_deactivate(undo_ctx,row_ctx,rg) != 1)
        return 0;

    /*
     * TODO: check for other deletion requirements here
     */
    return 1;
}

#ifdef logMonColumnTable_ROW_CREATION
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
 * returns a newly allocated logMonColumnTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
logMonColumnTable_context *
logMonColumnTable_create_row( netsnmp_index* hdr)
{
    logMonColumnTable_context * ctx =
        SNMP_MALLOC_TYPEDEF(logMonColumnTable_context);
    if(!ctx)
        return NULL;

    /*
     * TODO: check indexes, if necessary.
     */
    if(logMonColumnTable_extract_index( ctx, hdr )) {
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
     ctx->logMonColumnLogFile = 0;
     ctx->logMonColumnReadInterval = 0;
     ctx->logMonColumnCol1 = 0;
     ctx->logMonColumnCol2 = 0;
     ctx->logMonColumnCol3 = 0;
     ctx->logMonColumnCol4 = 0;
     ctx->logMonColumnCol5 = 0;
     ctx->logMonColumnColFilter1 = 0;
     ctx->logMonColumnColFilter2 = 0;
     ctx->logMonColumnColFilter3 = 0;
     ctx->logMonColumnColFilter4 = 0;
     ctx->logMonColumnColFilter5 = 0;
     ctx->logMonColumnCreatedTime = 0;
     ctx->logMonColumnDescription = 0;
     ctx->logMonColumnAlertMessage = 0;
     ctx->logMonColumnStatus = 0;
     ctx->logMonColumnUpdatedTime = 0;
     ctx->logMonColumnFileSize = 0;
     ctx->logMonColumnReadLine = 0;
    */

    return ctx;
}
#endif

/************************************************************
 * the *_duplicate row routine
 */
logMonColumnTable_context *
logMonColumnTable_duplicate_row( logMonColumnTable_context * row_ctx)
{
    logMonColumnTable_context * dup;

    if(!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(logMonColumnTable_context);
    if(!dup)
        return NULL;

    if(logMonColumnTable_row_copy(dup,row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index * logMonColumnTable_delete_row( logMonColumnTable_context * ctx )
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
void logMonColumnTable_set_reserve1( netsnmp_request_group *rg )
{
    logMonColumnTable_context *row_ctx =
            (logMonColumnTable_context *)rg->existing_row;
    logMonColumnTable_context *undo_ctx =
            (logMonColumnTable_context *)rg->undo_info;
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

        case COLUMN_LOGMONCOLUMNLOGFILE:
            /** DisplayString = ASN_OCTET_STR */

            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
            // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->logMonColumnLogFile));
                                                var->val_len);
            // <End: WIP_NURI_ALL>

        break;

        case COLUMN_LOGMONCOLUMNREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonColumnReadInterval));
        break;

        case COLUMN_LOGMONCOLUMNCOL1:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonColumnCol1));
        break;

        case COLUMN_LOGMONCOLUMNCOL2:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonColumnCol2));
        break;

        case COLUMN_LOGMONCOLUMNCOL3:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonColumnCol3));
        break;

        case COLUMN_LOGMONCOLUMNCOL4:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonColumnCol4));
        break;

        case COLUMN_LOGMONCOLUMNCOL5:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonColumnCol5));
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER1:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
            // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->logMonColumnColFilter1));
                                                var->val_len);
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER2:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
            // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->logMonColumnColFilter2));
                                                var->val_len);
            // <End: WIP_NURI_ALL>

        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER3:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
            // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->logMonColumnColFilter3));
                                                var->val_len);
            // <End: WIP_NURI_ALL>

        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER4:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
            // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->logMonColumnColFilter4));
                                                var->val_len);
            // <End: WIP_NURI_ALL>

        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER5:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
            // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->logMonColumnColFilter5));
                                                var->val_len);
            // <End: WIP_NURI_ALL>

        break;

        case COLUMN_LOGMONCOLUMNCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
            // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->logMonColumnCreatedTime));
                                                var->val_len);
            // <End: WIP_NURI_ALL>

        break;

        case COLUMN_LOGMONCOLUMNDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
            // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->logMonColumnDescription));
                                                var->val_len);
            // <End: WIP_NURI_ALL>


        break;

        case COLUMN_LOGMONCOLUMNALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
            // <Start: WIP_NURI_ALL: Modified here>
                                                //sizeof(row_ctx->logMonColumnAlertMessage));
                                                var->val_len);
            // <End: WIP_NURI_ALL>

        break;

        case COLUMN_LOGMONCOLUMNSTATUS:
            /** RowStatus = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonColumnStatus));
        break;

        case COLUMN_LOGMONCOLUMNUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
            // <Start: WIP_NURI_ALL: Modified here>
                                               //sizeof(row_ctx->logMonColumnUpdatedTime));
                                                var->val_len);
            // <End: WIP_NURI_ALL>

        break;

        case COLUMN_LOGMONCOLUMNFILESIZE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonColumnFileSize));
        break;

        case COLUMN_LOGMONCOLUMNREADLINE:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->logMonColumnReadLine));
        break;

        default: /** We shouldn't get here */
            rc = SNMP_ERR_GENERR;
            snmp_log(LOG_ERR, "unknown column in "
                     "logMonColumnTable_set_reserve1\n");
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

void logMonColumnTable_set_reserve2( netsnmp_request_group *rg )
{
    logMonColumnTable_context *row_ctx = (logMonColumnTable_context *)rg->existing_row;
    logMonColumnTable_context *undo_ctx = (logMonColumnTable_context *)rg->undo_info;
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

        case COLUMN_LOGMONCOLUMNLOGFILE:
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

        case COLUMN_LOGMONCOLUMNREADINTERVAL:
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

        case COLUMN_LOGMONCOLUMNCOL1:
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

        case COLUMN_LOGMONCOLUMNCOL2:
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

        case COLUMN_LOGMONCOLUMNCOL3:
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

        case COLUMN_LOGMONCOLUMNCOL4:
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

        case COLUMN_LOGMONCOLUMNCOL5:
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

        case COLUMN_LOGMONCOLUMNCOLFILTER1:
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

        case COLUMN_LOGMONCOLUMNCOLFILTER2:
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

        case COLUMN_LOGMONCOLUMNCOLFILTER3:
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

        case COLUMN_LOGMONCOLUMNCOLFILTER4:
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

        case COLUMN_LOGMONCOLUMNCOLFILTER5:
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

        case COLUMN_LOGMONCOLUMNCREATEDTIME:
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

        case COLUMN_LOGMONCOLUMNDESCRIPTION:
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

        case COLUMN_LOGMONCOLUMNALERTMESSAGE:
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

        case COLUMN_LOGMONCOLUMNSTATUS:
            /** RowStatus = ASN_INTEGER */
                rc = netsnmp_check_vb_rowstatus(current->ri->requestvb,
                                                undo_ctx ?
                                                undo_ctx->logMonColumnStatus:0);
                rg->rg_void = current->ri;
        break;

        case COLUMN_LOGMONCOLUMNUPDATEDTIME:
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

        case COLUMN_LOGMONCOLUMNFILESIZE:
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

        case COLUMN_LOGMONCOLUMNREADLINE:
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
void logMonColumnTable_set_action( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    logMonColumnTable_context *row_ctx = (logMonColumnTable_context *)rg->existing_row;
    logMonColumnTable_context *undo_ctx = (logMonColumnTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    int            row_err = 0;

    /*
     * TODO: loop through columns, copy varbind values
     * to context structure for the row.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_LOGMONCOLUMNLOGFILE:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logMonColumnLogFile,var->val.string,var->val_len);
            row_ctx->logMonColumnLogFile_len = var->val_len;
            // <Start: WIP_NURI_ALL: Modified here to terminate the string with NULL>
            row_ctx->logMonColumnLogFile[var->val_len]=0;
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONCOLUMNREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonColumnReadInterval = *var->val.integer;
        break;

        case COLUMN_LOGMONCOLUMNCOL1:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonColumnCol1 = *var->val.integer;
        break;

        case COLUMN_LOGMONCOLUMNCOL2:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonColumnCol2 = *var->val.integer;
        break;

        case COLUMN_LOGMONCOLUMNCOL3:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonColumnCol3 = *var->val.integer;
        break;

        case COLUMN_LOGMONCOLUMNCOL4:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonColumnCol4 = *var->val.integer;
        break;

        case COLUMN_LOGMONCOLUMNCOL5:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonColumnCol5 = *var->val.integer;
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER1:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logMonColumnColFilter1,var->val.string,var->val_len);
            row_ctx->logMonColumnColFilter1_len = var->val_len;
            // <Start: WIP_NURI_ALL: Modified here to terminate the string with NULL>
            row_ctx->logMonColumnColFilter1[var->val_len]=0;
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER2:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logMonColumnColFilter2,var->val.string,var->val_len);
            row_ctx->logMonColumnColFilter2_len = var->val_len;
            // <Start: WIP_NURI_ALL: Modified here to terminate the string with NULL>
            row_ctx->logMonColumnColFilter2[var->val_len]=0;
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER3:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logMonColumnColFilter3,var->val.string,var->val_len);
            row_ctx->logMonColumnColFilter3_len = var->val_len;
            // <Start: WIP_NURI_ALL: Modified here to terminate the string with NULL>
            row_ctx->logMonColumnColFilter3[var->val_len]=0;
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER4:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logMonColumnColFilter4,var->val.string,var->val_len);
            row_ctx->logMonColumnColFilter4_len = var->val_len;
            // <Start: WIP_NURI_ALL: Modified here to terminate the string with NULL>
            row_ctx->logMonColumnColFilter4[var->val_len]=0;
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER5:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logMonColumnColFilter5,var->val.string,var->val_len);
            row_ctx->logMonColumnColFilter5_len = var->val_len;
            // <Start: WIP_NURI_ALL: Modified here to terminate the string with NULL>
            row_ctx->logMonColumnColFilter5[var->val_len]=0;
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONCOLUMNCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->logMonColumnCreatedTime,var->val.string,var->val_len);
            row_ctx->logMonColumnCreatedTime_len = var->val_len;
            // <Start: WIP_NURI_ALL: Modified here to terminate the string with NULL>
            row_ctx->logMonColumnCreatedTime[var->val_len]=0;
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONCOLUMNDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logMonColumnDescription,var->val.string,var->val_len);
            row_ctx->logMonColumnDescription_len = var->val_len;
            // <Start: WIP_NURI_ALL: Modified here to terminate the string with NULL>
            row_ctx->logMonColumnDescription[var->val_len]=0;
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONCOLUMNALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            memcpy(row_ctx->logMonColumnAlertMessage,var->val.string,var->val_len);
            row_ctx->logMonColumnAlertMessage_len = var->val_len;
            // <Start: WIP_NURI_ALL: Modified here to terminate the string with NULL>
            row_ctx->logMonColumnAlertMessage[var->val_len]=0;
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONCOLUMNSTATUS:
            /** RowStatus = ASN_INTEGER */
            row_ctx->logMonColumnStatus = *var->val.integer;
            // <Start:WIP_NURI_ALL: Added the created date-time code here>
        break;

        case COLUMN_LOGMONCOLUMNUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            memcpy(row_ctx->logMonColumnUpdatedTime,var->val.string,var->val_len);
            row_ctx->logMonColumnUpdatedTime_len = var->val_len;
            // <Start: WIP_NURI_ALL: Modified here to terminate the string with NULL>
            row_ctx->logMonColumnUpdatedTime[var->val_len]=0;
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_LOGMONCOLUMNFILESIZE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonColumnFileSize = *var->val.integer;
        break;

        case COLUMN_LOGMONCOLUMNREADLINE:
            /** INTEGER = ASN_INTEGER */
            row_ctx->logMonColumnReadLine = *var->val.integer;
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
#ifndef logMonColumnTable_CAN_MODIFY_ACTIVE_ROW
    if( undo_ctx && RS_IS_ACTIVE(undo_ctx->logMonColumnStatus) &&
        row_ctx && RS_IS_ACTIVE(row_ctx->logMonColumnStatus) ) {
            row_err = 1;
    }
#endif

    /*
     * check activation/deactivation
     */
    row_err = netsnmp_table_array_check_row_status(&cb, rg,
                                  row_ctx ? &row_ctx->logMonColumnStatus : NULL,
                                  undo_ctx ? &undo_ctx->logMonColumnStatus : NULL);
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
void logMonColumnTable_set_commit( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    logMonColumnTable_context *row_ctx = (logMonColumnTable_context *)rg->existing_row;
    logMonColumnTable_context *undo_ctx = (logMonColumnTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    // <Start: WIP_NURI_ALL: Modified the code from here>

    pthread_t pLogThr;
    time_t now;

    DEBUGMSGTL(("LogMonCol/logMonColumnTable_set_commit",
        "LogFile Name <%s>\n",
        row_ctx->logMonColumnLogFile));

    if ( LogMonCol_UpdateDataFile(row_ctx) == 0 )
    {
        DEBUGMSGTL(("LogMonCol/logMonColumnTable_set_commit",
            "Row updated in LOG_MON_COL_DATA_FILE \n"));
    }

    // Do the value check, before proceeding
    if ( ( strlen(row_ctx->logMonColumnLogFile) != 0 ) &&
         ( row_ctx->logMonColumnReadInterval != 0 ) &&
         // check if not all the col are empty
         ( ( ( strlen(row_ctx->logMonColumnColFilter1) != 0 ) &&
            ( row_ctx->logMonColumnCol1 != 0 ) ) ||
           ( ( strlen(row_ctx->logMonColumnColFilter2) != 0 ) &&
            ( row_ctx->logMonColumnCol2 != 0 ) ) ||
           ( ( strlen(row_ctx->logMonColumnColFilter3) != 0 ) &&
            ( row_ctx->logMonColumnCol3 != 0 ) ) ||
           ( ( strlen(row_ctx->logMonColumnColFilter4) != 0 ) &&
            ( row_ctx->logMonColumnCol4 != 0 ) ) ||
           ( ( strlen(row_ctx->logMonColumnColFilter5) != 0 ) &&
            ( row_ctx->logMonColumnCol5 != 0 ) ) ) &&
           ( row_ctx->logMonColumnStatus == SNMP_ROW_ACTIVE ) )
    {
        // Row Created/ Changed Time
        row_ctx->logMonColumnCreatedTime_len=MAX_DATE_TIME_LEN;

        // Get the Date-Time in time_t format
        // and update the created time for the newly
        // created row
        (void *) time(&now);
        strcpy(row_ctx->logMonColumnCreatedTime,
                (u_char *) date_n_time(&now,
                (size_t*)&(row_ctx->logMonColumnCreatedTime_len)));
        if ( row_ctx->iThreadActiveFlag == 0 )
        {
            // Create a thread for this newly created row
            if ( pthread_create(&pLogThr, NULL, LogMonCol_LogMonThreadMain,
                                      (void*)row_ctx) != 0 )
            {
                snmp_log_perror("logMonColumnTable_set_commit \
                    Thread creation failed\n");

            }
            else
            {
                // Change it to active
                row_ctx->iThreadActiveFlag = 1;

                DEBUGMSGTL(("LogMonCol/logMonColumnTable_set_commit",
                    "Thread started for < %u %d %s >\n",
                    row_ctx->idx,
                    row_ctx->index.len,
                    row_ctx->logMonColumnLogFile));
            }

        }
    }

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_LOGMONCOLUMNCOL1:
        case COLUMN_LOGMONCOLUMNCOL2:
        case COLUMN_LOGMONCOLUMNCOL3:
        case COLUMN_LOGMONCOLUMNCOL4:
        case COLUMN_LOGMONCOLUMNCOL5:
        case COLUMN_LOGMONCOLUMNCOLFILTER1:
        case COLUMN_LOGMONCOLUMNCOLFILTER2:
        case COLUMN_LOGMONCOLUMNCOLFILTER3:
        case COLUMN_LOGMONCOLUMNCOLFILTER4:
        case COLUMN_LOGMONCOLUMNCOLFILTER5:
            /** DisplayString = ASN_OCTET_STR */
        row_ctx->logMonColumnReadLine=0; // Restart the searching
        DEBUGMSGTL(("LogMonCol/logMonColumnTable_set_commit",
                    "Line Count Reset for < %s >\n",
                    row_ctx->logMonColumnLogFile));

        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
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
void logMonColumnTable_set_free( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    logMonColumnTable_context *row_ctx = (logMonColumnTable_context *)rg->existing_row;
    logMonColumnTable_context *undo_ctx = (logMonColumnTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_LOGMONCOLUMNLOGFILE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNCOL1:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNCOL2:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNCOL3:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNCOL4:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNCOL5:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER1:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER2:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER3:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER4:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER5:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNSTATUS:
            /** RowStatus = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNFILESIZE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNREADLINE:
            /** INTEGER = ASN_INTEGER */
        break;

        default: /** We shouldn't get here */
        // <Start:WIP_NURI_ALL: Added break statement here>
        break;
        // <End:WIP_NURI_ALL>
            /** should have been logged in reserve1 */
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
void logMonColumnTable_set_undo( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    logMonColumnTable_context *row_ctx = (logMonColumnTable_context *)rg->existing_row;
    logMonColumnTable_context *undo_ctx = (logMonColumnTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_LOGMONCOLUMNLOGFILE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNCOL1:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNCOL2:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNCOL3:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNCOL4:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNCOL5:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER1:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER2:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER3:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER4:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER5:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNSTATUS:
            /** RowStatus = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
        break;

        case COLUMN_LOGMONCOLUMNFILESIZE:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_LOGMONCOLUMNREADLINE:
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

#endif /** logMonColumnTable_SET_HANDLING */


/************************************************************
 *
 * Initialize the logMonColumnTable table by defining its contents and how it's structured
 */
void
initialize_table_logMonColumnTable(void)
{
    netsnmp_table_registration_info *table_info;

    if(my_handler) {
        snmp_log(LOG_ERR, "initialize_table_logMonColumnTable_handler called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /* if your table is read only, it's easiest to change the
       HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY */
    my_handler = netsnmp_create_handler_registration("logMonColumnTable",
                                             netsnmp_table_array_helper_handler,
                                             logMonColumnTable_oid,
                                             logMonColumnTable_oid_len,
                                             HANDLER_CAN_RWRITE);

    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_logMonColumnTable_handler\n");
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
        /** index: logMonColumnIndex */
        netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

    table_info->min_column = logMonColumnTable_COL_MIN;
    table_info->max_column = logMonColumnTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = logMonColumnTable_get_value;
    cb.container = netsnmp_container_find("logMonColumnTable_primary:"
                                          "logMonColumnTable:"
                                          "table_container");
#ifdef logMonColumnTable_IDX2
    netsnmp_container_add_index(cb.container,
                                netsnmp_container_find("logMonColumnTable_secondary:"
                                                       "logMonColumnTable:"
                                                       "table_container"));
    cb.container->next->compare = logMonColumnTable_cmp;
#endif
#ifdef logMonColumnTable_SET_HANDLING
    cb.can_set = 1;
#ifdef logMonColumnTable_ROW_CREATION
    cb.create_row = (UserRowMethod*)logMonColumnTable_create_row;
#endif
    cb.duplicate_row = (UserRowMethod*)logMonColumnTable_duplicate_row;
    cb.delete_row = (UserRowMethod*)logMonColumnTable_delete_row;
    cb.row_copy = (Netsnmp_User_Row_Operation *)logMonColumnTable_row_copy;

    cb.can_activate = (Netsnmp_User_Row_Action *)logMonColumnTable_can_activate;
    cb.can_deactivate = (Netsnmp_User_Row_Action *)logMonColumnTable_can_deactivate;
    cb.can_delete = (Netsnmp_User_Row_Action *)logMonColumnTable_can_delete;

    cb.set_reserve1 = logMonColumnTable_set_reserve1;
    cb.set_reserve2 = logMonColumnTable_set_reserve2;
    cb.set_action = logMonColumnTable_set_action;
    cb.set_commit = logMonColumnTable_set_commit;
    cb.set_free = logMonColumnTable_set_free;
    cb.set_undo = logMonColumnTable_set_undo;
#endif
    DEBUGMSGTL(("LogMonCol/initialize_table_logMonColumnTable",
                "Registering table logMonColumnTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);
}

/************************************************************
 * logMonColumnTable_get_value
 */
int logMonColumnTable_get_value(
            netsnmp_request_info *request,
            netsnmp_index *item,
            netsnmp_table_request_info *table_info )
{
    netsnmp_variable_list *var = request->requestvb;
    logMonColumnTable_context *context = (logMonColumnTable_context *)item;

    switch(table_info->colnum) {

        case COLUMN_LOGMONCOLUMNINDEX:
            /** UNSIGNED32 = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->logMonColumnIndex,
                         sizeof(context->logMonColumnIndex) );
        break;

        case COLUMN_LOGMONCOLUMNLOGFILE:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonColumnLogFile,
                         context->logMonColumnLogFile_len );
        break;

        case COLUMN_LOGMONCOLUMNREADINTERVAL:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonColumnReadInterval,
                         sizeof(context->logMonColumnReadInterval) );
        break;

        case COLUMN_LOGMONCOLUMNCOL1:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonColumnCol1,
                         sizeof(context->logMonColumnCol1) );
        break;

        case COLUMN_LOGMONCOLUMNCOL2:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonColumnCol2,
                         sizeof(context->logMonColumnCol2) );
        break;

        case COLUMN_LOGMONCOLUMNCOL3:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonColumnCol3,
                         sizeof(context->logMonColumnCol3) );
        break;

        case COLUMN_LOGMONCOLUMNCOL4:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonColumnCol4,
                         sizeof(context->logMonColumnCol4) );
        break;

        case COLUMN_LOGMONCOLUMNCOL5:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonColumnCol5,
                         sizeof(context->logMonColumnCol5) );
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER1:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonColumnColFilter1,
                         context->logMonColumnColFilter1_len );
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER2:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonColumnColFilter2,
                         context->logMonColumnColFilter2_len );
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER3:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonColumnColFilter3,
                         context->logMonColumnColFilter3_len );
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER4:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonColumnColFilter4,
                         context->logMonColumnColFilter4_len );
        break;

        case COLUMN_LOGMONCOLUMNCOLFILTER5:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonColumnColFilter5,
                         context->logMonColumnColFilter5_len );
        break;

        case COLUMN_LOGMONCOLUMNCREATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonColumnCreatedTime,
                         context->logMonColumnCreatedTime_len );
        break;

        case COLUMN_LOGMONCOLUMNDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonColumnDescription,
                         context->logMonColumnDescription_len );
        break;

        case COLUMN_LOGMONCOLUMNALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonColumnAlertMessage,
                         context->logMonColumnAlertMessage_len );
        break;

        case COLUMN_LOGMONCOLUMNSTATUS:
            /** RowStatus = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonColumnStatus,
                         sizeof(context->logMonColumnStatus) );
        break;

        case COLUMN_LOGMONCOLUMNUPDATEDTIME:
            /** DateAndTime = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->logMonColumnUpdatedTime,
                         context->logMonColumnUpdatedTime_len );
        break;

        case COLUMN_LOGMONCOLUMNFILESIZE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonColumnFileSize,
                         sizeof(context->logMonColumnFileSize) );
        break;

        case COLUMN_LOGMONCOLUMNREADLINE:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->logMonColumnReadLine,
                         sizeof(context->logMonColumnReadLine) );
        break;

    default: /** We shouldn't get here */
        snmp_log(LOG_ERR, "unknown column in "
                 "logMonColumnTable_get_value\n");
        return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}

/************************************************************
 * logMonColumnTable_get_by_idx
 */
const logMonColumnTable_context *
logMonColumnTable_get_by_idx(netsnmp_index * hdr)
{
    return (const logMonColumnTable_context *)
        CONTAINER_FIND(cb.container, hdr );
}


// Start: WIP_NURI_ALL : Start of the newly added code here

/*****************************************************************************
 *
 * name             :   LogMonCol_MasterThread
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
LogMonCol_MasterThread()
{
    pthread_t               stLogMonThr;
    logMonColumnTable_context *pstLogColData=NULL;

    // Get the first row in the container
    pstLogColData = CONTAINER_FIRST(cb.container);

    while(pstLogColData)
    {

        // Do the value check, before proceeding
        if ( ( strlen(pstLogColData->logMonColumnLogFile) != 0 ) &&
             ( pstLogColData->logMonColumnReadInterval != 0 ) &&
             // check if not all the col are empty
             ( ( ( strlen(pstLogColData->logMonColumnColFilter1) != 0 ) &&
                ( pstLogColData->logMonColumnCol1 != 0 ) ) ||
               ( ( strlen(pstLogColData->logMonColumnColFilter2) != 0 ) &&
                ( pstLogColData->logMonColumnCol2 != 0 ) ) ||
               ( ( strlen(pstLogColData->logMonColumnColFilter3) != 0 ) &&
                ( pstLogColData->logMonColumnCol3 != 0 ) ) ||
               ( ( strlen(pstLogColData->logMonColumnColFilter4) != 0 ) &&
                ( pstLogColData->logMonColumnCol4 != 0 ) ) ||
               ( ( strlen(pstLogColData->logMonColumnColFilter5) != 0 ) &&
                ( pstLogColData->logMonColumnCol5 != 0 ) ) ) )
        {

            // Now, spawn thread for it
            if ( pthread_create(&stLogMonThr, NULL,
                    LogMonCol_LogMonThreadMain,
                (void*)pstLogColData) != 0 )
            {
                snmp_log_perror("Thread creation failed");
            }
            else
            {

                // Change it to active
                pstLogColData->iThreadActiveFlag = 1;

                DEBUGMSGTL(("LogMonCol/LogMonCol_MasterThread",
                    "Master Thread started for < %u %d %s >\n",
                    pstLogColData->idx,
                    pstLogColData->index.len,
                    pstLogColData->logMonColumnLogFile));

            }
        }
        else
        {
            // Change it to not-active
            pstLogColData->iThreadActiveFlag = 0;

            snmp_log(LOG_ERR,"LogMonCol/LogMonCol_MasterThread \
                Thr #%d# 1Thread could not be started for OID < %u >\n",
                MY_THR_ID,
                pstLogColData->idx);
        }


        // Get the next row
        pstLogColData = CONTAINER_NEXT(cb.container, pstLogColData);
    }

}



/*****************************************************************************
 *
 * name             :   LogMonCol_LogMonThreadMain
 * description      :   This function is the main function of the thread, which
 *                      will keep on running
 * input parameters :   void *pstLogColData - the argument to the thread, which will be
 *                      logMonColumnTable_context data
 * output parameters:   None
 * return type      :   void * - Nothing
 * global variables :   1) cb
 * calls            :   2) CONTAINER_FIND
 *                      3) Common_SendAlert
 *                      4) LogMonCol_UpdateDataFile
 */

void *
LogMonCol_LogMonThreadMain(void *pstLogColData)
{

    FILE *pfOutFd;         // Output file descriptor
    FILE *psInFd;          // Input file descriptor
    struct timeval stTime;
    char acReadBuffer[LINE_MAX_LEN+1];           // To hold the line read
    char *psCurrTime=NULL;
    int iStrFound=0;
    int iLineNum;        // Current line number
    int iLidx;
    int iRowNum=0;       // To hold the Global array index number
    logMonColumnTable_context *pstMycontext=NULL;

    DEBUGMSGTL(("LogMonCol/LogMonCol_LogMonThreadMain",
        "Thr #%d# Thread spawned for OID <%u>\n",
        MY_THR_ID,
        ((logMonColumnTable_context*)pstLogColData)->idx));

    // we need the index value, so do this
    pstMycontext = (logMonColumnTable_context*)pstLogColData;

    /* CR: When thread is created, init, old file name with the existing 
    file name and row read line should be zero */
    
    strcpy(pstMycontext->acOldFileName, pstMycontext->logMonColumnLogFile);
    pstMycontext->logMonColumnReadLine = 0;
    
    // Keep looping infinitely
    while(1)
    {
        // Get a context to my row in the MIB container
        // Get the latest values of the data
        pstMycontext = CONTAINER_FIND(cb.container,&(pstMycontext->index));

        // If the row does not exist anymore
        if ( pstMycontext == NULL )
        {
            DEBUGMSGTL(("LogMonCol/LogMonCol_LogMonThreadMain",
                "Thr #%d# Exiting thread\n",
                MY_THR_ID));

            pthread_detach(MY_THR_ID);
            pthread_exit(NULL);  // exit the thread
        }

        // Check if the row is not active exit the thread
        if ( pstMycontext->logMonColumnStatus != SNMP_ROW_ACTIVE )
        {
            DEBUGMSGTL(("LogMonCol/LogMonCol_LogMonThreadMain",
                "Thr #%d# Exiting thread\n",
                MY_THR_ID));
            // Unset the Thread Active status
            pstMycontext->iThreadActiveFlag = 0;

            pthread_detach(MY_THR_ID);
            pthread_exit(NULL);  // exit the thread
        }

        // Do the value check, before proceeding
        if ( ( strlen(pstMycontext->logMonColumnLogFile) == 0 ) ||
        ( pstMycontext->logMonColumnReadInterval == 0 ) )
        {
            SLEEP(TIME_TO_SLEEP);
            continue;   // No need to process the request
                        //until valid values are provided
        }

        /* CR: Whenever the file name is changed, initialize all data to 0 */
        if(strcmp(pstMycontext->acOldFileName, pstMycontext->logMonColumnLogFile) != 0)
        {
            pstMycontext->logMonColumnReadLine = 0;
   			strcpy(pstMycontext->logMonColumnUpdatedTime,"");            
            pstMycontext->logMonColumnFileSize = 0;
            strcpy(pstMycontext->acOldFileName, pstMycontext->logMonColumnLogFile);
        }

        DEBUGMSGTL(("LogMonCol/LogMonCol_LogMonThreadMain",
            "Thr #%d# Searching in LOGFILE <%s>\n",
            MY_THR_ID,
            pstMycontext->logMonColumnLogFile));

        // Register an alarm for log file transfer if
        // not registered already
        if ( (guiLogFileColRowAlarmReg == 0) &&
             (glAgentInfoFileTrPort > 0) &&
             (strlen(gcAgentInfoFileTransferIP) > 0))
        {
            DEBUGMSGTL(("LogMonCol/LogMonCol_LogMonThreadMain",
                "Thr #%d# Registering the alarm for LogMonCol Log file transfer\n",
                MY_THR_ID));
            if (glAgentInfoFileTrInterval > 0 )
                guiLogFileColRowAlarmReg = snmp_alarm_register (glAgentInfoFileTrInterval,
                (int)NULL,Common_SendFileThread, gcInfoLogFileMonCol);
            else
                guiLogFileColRowAlarmReg = snmp_alarm_register (1,
                (int)NULL,Common_SendFileThread, gcInfoLogFileMonCol);

        }


        // Check for the patterns
        LogMonCol_SearchPatterns(pstMycontext);
        /* CR: Searching is to be done till the end of file always and
               trap is to sent immediately for all the pattern match */
        #if 0
        if( LogMonCol_SearchPatterns(pstMycontext) == 1 )
        {
            if (Common_SendAlert("LGCM",
                gcInfoLogFileMonCol,
                pstMycontext->logMonColumnAlertMessage,
                pstMycontext->logMonColumnIndex))
            {
                snmp_log(LOG_ERR,
                    "Alert sending failed for <%s>\n",
                    pstMycontext->logMonColumnLogFile);
            }
        }
        else
            SLEEP(TIME_TO_SLEEP);
        #endif

        // Update the data file with the latest row data
        if ( LogMonCol_UpdateDataFile(pstMycontext) == -1 )
        {
            snmp_log_perror("Error while updating MIB file");
        }

        DEBUGMSGTL(("LogMonCol/LogMonCol_LogMonThreadMain",
            "Thr #%d# going to sleep for logfile <%s>\n",
            MY_THR_ID,
            pstMycontext->logMonColumnLogFile));

        // Sleep until the next interval
        SLEEP(pstMycontext->logMonColumnReadInterval);

        DEBUGMSGTL(("LogMonCol/LogMonCol_LogMonThreadMain",
            "Thr #%d# woke up for logfile <%s>\n",
            MY_THR_ID,
            pstMycontext->logMonColumnLogFile));

        // Re-set the Flag
        iStrFound=0;

    } // End of infinite while loop

}



/*****************************************************************************
 *
 * name             :   LogMonCol_LoadContainerFromFile
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

// For unix flavors
void
LogMonCol_LoadContainerFromFile()
{
    int iFd=0;
    int iFdTmp=0;
    int iCnt=0;
    int iRewriteFlag=0;
    logMonColumnTable_context stReadCtx = {0};
    logMonColumnTable_context *pstMyContext = NULL;

    iFd = open (LOG_MON_COL_DATA_FILE, O_RDONLY,S_IREAD);

    if (iFd == -1)
    {
         snmp_log_perror
            ("LogMonCol_LoadContainerFromFile: \
                LOG_MON_COL_DATA_FILE file open failed\n");
    }
    else
    {

        DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            LOG_MON_COL_DATA_FILE));

        while (read (iFd, &stReadCtx, sizeof (logMonColumnTable_context)) > 0)
        {
            // If the row, is already destroyed, no need to create it again.
            if ( stReadCtx.logMonColumnStatus == SNMP_ROW_DESTROY )
            {
	             iRewriteFlag = 1;   // to re-write the mib data file
                 continue; // as we have to load the container
            }

            DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
                "Mib being loaded for Log File <%s>\n",
                stReadCtx.logMonColumnLogFile));

            pstMyContext = (logMonColumnTable_context*)malloc(sizeof (logMonColumnTable_context));
            if ( !pstMyContext )
            {
                snmp_log_perror("LogMonCol: Malloc failed");
                return;
            }

            memcpy (pstMyContext, &stReadCtx, sizeof (logMonColumnTable_context));

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
                DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
                    "ERROR IN CONTAINER_INSERT FUNCION\n"));
            }

            DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
		      "OID <%u>\n",*(pstMyContext->index.oids)));
        }

        DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
                "CONTAINER SIZE=%u\n", CONTAINER_SIZE (cb.container)));

        if ( iRewriteFlag )
        {
            // Open a temp file
            if ( ( iFd = open (LOG_MON_COL_DATA_FILE_TMP,
                                   O_CREAT|O_RDWR,S_IREAD|S_IWRITE) ) == -1 )
            {
	            snmp_log_perror(
                       "LOG_MON_COL_DATA_FILE_TMP file open failed\n");
            }
            else
            {
                DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
                    "Thr #%d# File <%s> opened\n",
                    MY_THR_ID,
                    LOG_MON_COL_DATA_FILE_TMP));

                // Reset the file pointer to begining of the file
                lseek(iFd, 0,SEEK_SET);
                lseek(iFdTmp, 0,SEEK_SET);

                while (read (iFd, &stReadCtx,
                                   sizeof (logMonColumnTable_context)) > 0)
                {
	                if ( stReadCtx.logMonColumnStatus == SNMP_ROW_ACTIVE )
                        if ( write(iFdTmp,&stReadCtx,
                              sizeof(logMonColumnTable_context)) < 0 )
                        {
                            snmp_log_perror(
                             "Error writing data to LOG_MON_COL_DATA_FILE");
                        }

                } // End of while

                // Close the file
                close(iFdTmp);

                DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_COL_DATA_FILE_TMP));

                // Delete the old data file
                unlink(LOG_MON_COL_DATA_FILE);

	            // Rename the temp file to original
	            rename(LOG_MON_COL_DATA_FILE_TMP,LOG_MON_COL_DATA_FILE);

            } // End of else

        } // End of rewrite if

    } // End of else
    // Close the file
    close(iFd);

    DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
        "Thr #%d# File <%s> closed\n",
        MY_THR_ID,
        LOG_MON_COL_DATA_FILE));

}

#else

// For Windows
void
LogMonCol_LoadContainerFromFile()
{
    HANDLE  hFd = NULL;
    HANDLE  hFdTmp = NULL;
    DWORD NumberOfBytesRead;
    int iCnt=0;
    int iRewriteFlag=0;
    logMonColumnTable_context stReadCtx = {0};
    logMonColumnTable_context *pstMyContext = NULL;

    // Open the Data file
    hFd = CreateFile   (LOG_MON_COL_DATA_FILE,
                        GENERIC_READ,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    if (hFd == INVALID_HANDLE_VALUE)
    {
         snmp_log_perror
            ("LogMonCol_LoadContainerFromFile: \
                LOG_MON_COL_DATA_FILE file open failed\n");
    }
    else
    {
        

        DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            LOG_MON_COL_DATA_FILE));

        while (ReadFile(hFd, &stReadCtx, sizeof (logMonColumnTable_context),
                &NumberOfBytesRead, NULL))
        {
            
        
               if (NumberOfBytesRead== 0)
                {
                 snmp_log(LOG_ERR," %d breaking out \n",GetLastError());
                  break;
                }
        // If the row, is already destroyed, no need to create it again.
            if ( stReadCtx.logMonColumnStatus == SNMP_ROW_DESTROY )
            {
	             iRewriteFlag = 1;   // to re-write the mib data file
                 continue; // As we have to load the container
            }

            DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
                "Mib being loaded for Log File <%s>\n",
                stReadCtx.logMonColumnLogFile));

            pstMyContext = (logMonColumnTable_context*)malloc(sizeof (logMonColumnTable_context));
            if ( !pstMyContext )
            {
                snmp_log_perror("LogMonCol: Malloc failed");
                return;
            }

            memcpy (pstMyContext, &stReadCtx, sizeof (logMonColumnTable_context));

            netsnmp_assert(pstMyContext->index.oids == NULL);

            if(snmp_clone_mem( (void*)&pstMyContext->index.oids, &(stReadCtx.idx),
                            stReadCtx.index.len * sizeof(oid) ))
            {
                free(pstMyContext);
                CloseHandle(hFd);

                DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_COL_DATA_FILE));

                return;
            }

            pstMyContext->index.len = stReadCtx.index.len;

            iCnt = CONTAINER_INSERT (cb.container, pstMyContext);

            if (iCnt)
            {
                DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
                    "ERROR IN CONTAINER_INSERT FUNCION\n"));
            }

            DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
		      "OID <%u>\n",*(pstMyContext->index.oids)));

        
        }

        DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
                "CONTAINER SIZE=%u\n", CONTAINER_SIZE (cb.container)));

        if ( iRewriteFlag )
        {
            // Open a temp file
            hFdTmp = CreateFile(LOG_MON_COL_DATA_FILE_TMP,
                            GENERIC_READ|GENERIC_WRITE,
                            0,
                            NULL,
                            OPEN_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

            if (hFdTmp == INVALID_HANDLE_VALUE)
            {
	            snmp_log_perror(
                       "LOG_MON_COL_DATA_FILE_TMP file open failed\n");
            }
            else
            {
                DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_COL_DATA_FILE_TMP));

                // Reset the file pointers to begining of the files
                SetFilePointer(hFd,0,NULL,FILE_BEGIN);
                SetFilePointer(hFdTmp,0,NULL,FILE_BEGIN);

                DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
                    "Thr #%d# File <%s> opened\n",
                    MY_THR_ID,
                    LOG_MON_COL_DATA_FILE_TMP));

                

                while(ReadFile(hFd, &stReadCtx, sizeof (logMonColumnTable_context),
                    &NumberOfBytesRead, NULL))
                {
                    if (NumberOfBytesRead== 0)
                    {
                     snmp_log(LOG_ERR," %d breaking out \n",GetLastError());
                      break;
                    }
	                    if ( stReadCtx.logMonColumnStatus == SNMP_ROW_ACTIVE )

                        if( WriteFile(hFdTmp, &stReadCtx,
                                sizeof(logMonColumnTable_context),
                                &NumberOfBytesRead, NULL) == 0 )
                        {
                            snmp_log_perror(
                             "Error writing data to LOG_MON_COL_DATA_FILE");
                        }
                        

                } // End of while

                // Close the file
                CloseHandle(hFdTmp);

                DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    LOG_MON_COL_DATA_FILE_TMP));


                // Delete the old data file
                unlink(LOG_MON_COL_DATA_FILE);

	            // Rename the temp file to original
	            rename(LOG_MON_COL_DATA_FILE_TMP,LOG_MON_COL_DATA_FILE);

            } // End of else

        } // End of rewrite if

    } // End of else

    // Close the file
    CloseHandle(hFd);

    DEBUGMSGTL(("LogMonCol/LogMonCol_LoadContainerFromFile",
        "Thr #%d# File <%s> closed\n",
        MY_THR_ID,
        LOG_MON_COL_DATA_FILE));

}

#endif

/*****************************************************************************
 *
 * name             :   LogMonCol_SearchPatterns
 * description      :   This function is used to search the patterns in the
 *                      input file
 * input parameters :   logMonColumnTable_context *logMonColRow
 * output parameters:   None
 * return type      :   int - 1 for pattern found, 0 for not found, -1 for error
 * global variables :   None
 * calls            :   LogMonCol_GetCurrLogTime
 */


int
LogMonCol_SearchPatterns(logMonColumnTable_context *logMonColRow)
{

    FILE                *pfInFile=NULL;
    FILE *pfOutFile=NULL;
    struct stat stStatBuf={0};
    time_t now;
    int  iStrFound=0;
    int  iLineCount=0;
    int  iIdx=0;
    char acReadBuffer[LINE_MAX_LEN];
    char *acFilterField[5];
    char *acTmpFilterField[5];
    char acFormat[MAX_FORMAT_LEN];
    char acFilterStr[87];


    DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                "Thr #%d# Called\n",
                MY_THR_ID));
    // Check if the file is present, if yes get the stat
    if(stat(logMonColRow->logMonColumnLogFile,&stStatBuf)==-1)
    {

        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
            "Thr #%d# logMonColumnLogFile <%s> does not exist",
            MY_THR_ID,
            logMonColRow->logMonColumnLogFile));

        return(-1);
    }

    /* CR: When the file is lesser than last time or 
           whenever the file name is changed or
           when the thread is created first time
           go to EOF and do not monitor */
    if(logMonColRow->logMonColumnFileSize > stStatBuf.st_size || logMonColRow->logMonColumnReadLine == 0)
    {
    	logMonColRow->logMonColumnReadLine = LogMonRow_GetLineCount(logMonColRow->logMonColumnLogFile);
    	logMonColRow->logMonColumnFileSize = stStatBuf.st_size;

    	// Row Modified Time
        logMonColRow->logMonColumnUpdatedTime_len=MAX_DATE_TIME_LEN;

        (void *) time(&now);

        strcpy(logMonColRow->logMonColumnUpdatedTime,
            (u_char *) date_n_time(&now,
            (size_t*)&(logMonColRow->logMonColumnUpdatedTime_len)));
            
       return(0);

    }


    if (( pfInFile = fopen(logMonColRow->logMonColumnLogFile,"r")) == NULL )
    {
        snmp_log_perror("LogMonCol_SearchPatterns:Error opening input file");

        return(-1);
    }
    else
    {

        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            logMonColRow->logMonColumnLogFile));

        // Start from the line where we stopped found pattern last time
        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
            "Thr #%d# Starting search from line number <%d>\n",
            MY_THR_ID,
            logMonColRow->logMonColumnReadLine));




        iLineCount=logMonColRow->logMonColumnReadLine;

        for(iIdx=1; iIdx <= iLineCount ; iIdx++)
            if ( fgets(acReadBuffer,LINE_MAX_LEN,pfInFile) == NULL)
                break;

        acFilterField[0] =
         (char*) calloc(logMonColRow->logMonColumnCol1+1, sizeof(char));
        acFilterField[1] =
         (char*) calloc(logMonColRow->logMonColumnCol2+1, sizeof(char));
        acFilterField[2] =
         (char*) calloc(logMonColRow->logMonColumnCol3+1, sizeof(char));
        acFilterField[3] =
         (char*) calloc(logMonColRow->logMonColumnCol4+1, sizeof(char));
        acFilterField[4] =
         (char*) calloc(logMonColRow->logMonColumnCol5+1, sizeof(char));

        if ( !acFilterField[0] || !acFilterField[1] || !acFilterField[2] ||
             !acFilterField[3] || !acFilterField[4] )
        {
            snmp_log_perror("LogMonCol: Malloc failed");
            return(-1);
        }

        acTmpFilterField[0] =
         (char*) calloc(logMonColRow->logMonColumnCol1+1, sizeof(char));
        acTmpFilterField[1] =
         (char*) calloc(logMonColRow->logMonColumnCol2+1, sizeof(char));
        acTmpFilterField[2] =
         (char*) calloc(logMonColRow->logMonColumnCol3+1, sizeof(char));
        acTmpFilterField[3] =
         (char*) calloc(logMonColRow->logMonColumnCol4+1, sizeof(char));
        acTmpFilterField[4] =
         (char*) calloc(logMonColRow->logMonColumnCol5+1, sizeof(char));

        if ( !acTmpFilterField[0] || !acTmpFilterField[1] || !acTmpFilterField[2] ||
             !acTmpFilterField[3] || !acTmpFilterField[4] )
        {
            snmp_log_perror("LogMonCol: Malloc failed");
            return(-1);
        }

        do
        {
            /* CR: Flag is reset - this flag is used to decide to send 
                   trap or not for each match */
                   
            iStrFound = 0;
            if(fgets(acReadBuffer,LINE_MAX_LEN,pfInFile) == NULL)
            {
                // End of File reached or File read error
                DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                    "Thr #%d# %s : End of file or Read Error\n",
                    MY_THR_ID,
                    logMonColRow->logMonColumnLogFile));

                if (pfInFile)
                    FCLOSE(pfInFile);

                DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    logMonColRow->logMonColumnLogFile));

                break;
            }
            else
            {

                // Increment line count
                iLineCount++;

                strcpy(acFormat,"");

                // Prepare the format
                if ( logMonColRow->logMonColumnCol1 != 0 )
                {
                    sprintf(acFormat,"%s%%%dc",acFormat, logMonColRow->logMonColumnCol1);
                }

                if ( logMonColRow->logMonColumnCol2 != 0 )
                {
                    sprintf(acFormat,"%s%%%dc",acFormat, logMonColRow->logMonColumnCol2);
                }

                if ( logMonColRow->logMonColumnCol3 != 0 )
                {
                    sprintf(acFormat,"%s%%%dc",acFormat, logMonColRow->logMonColumnCol3);
                }

                if ( logMonColRow->logMonColumnCol4 != 0 )
                {
                    sprintf(acFormat,"%s%%%dc",acFormat, logMonColRow->logMonColumnCol4);
                }

                if ( logMonColRow->logMonColumnCol5 != 0 )
                {
                    sprintf(acFormat,"%s%%%dc",acFormat, logMonColRow->logMonColumnCol5);
                }

                if ( logMonColRow->logMonColumnCol1 != 0 )
                    DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                        "Thr #%d# Filter1 <%s>\n",
                        MY_THR_ID,
                        logMonColRow->logMonColumnColFilter1));

                if ( logMonColRow->logMonColumnCol2 != 0 )
                    DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                        "Thr #%d Filter2 <%s>\n",
                        MY_THR_ID,
                        logMonColRow->logMonColumnColFilter2));

                if ( logMonColRow->logMonColumnCol3 != 0 )
                    DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                        "Thr #%d# Filter3 <%s>\n",
                        MY_THR_ID,
                        logMonColRow->logMonColumnColFilter3));

                if ( logMonColRow->logMonColumnCol4 != 0 )
                    DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                        "Thr #%d# Filter4 <%s>\n",
                        MY_THR_ID,
                        logMonColRow->logMonColumnColFilter4));

                if ( logMonColRow->logMonColumnCol5 != 0 )
                    DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                        "Thr #%d# Filter5 <%s>\n",
                        MY_THR_ID,
                        logMonColRow->logMonColumnColFilter5));



                DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                    "Thr #%d# Format <%s> Line <%s>\n",
                    MY_THR_ID,
                    acFormat,
                    acReadBuffer));

                
                DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                    "Thr #%d# Log File Size <%d>\n",
                    MY_THR_ID,
                    logMonColRow->logMonColumnFileSize));


                // Copy the respective fields
                sscanf(acReadBuffer,acFormat,acTmpFilterField[0],
                                             acTmpFilterField[1],
                                             acTmpFilterField[2],
                                             acTmpFilterField[3],
                                             acTmpFilterField[4]);
                // Terminate the fields with NULL

                DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                    "Field1 <%s> Field2 <%s> Field3 <%s> Field4 <%s> Field5 <%s>\n",
                    acTmpFilterField[0],
                    acTmpFilterField[1],
                    acTmpFilterField[2],
                    acTmpFilterField[3],
                    acTmpFilterField[4]));

                acTmpFilterField[0][strlen(acTmpFilterField[0])] = 0;
                acTmpFilterField[1][strlen(acTmpFilterField[1])] = 0;
                acTmpFilterField[2][strlen(acTmpFilterField[2])] = 0;
                acTmpFilterField[3][strlen(acTmpFilterField[3])] = 0;
                acTmpFilterField[4][strlen(acTmpFilterField[4])] = 0;

                iIdx=0;

                if (logMonColRow->logMonColumnCol1 != 0)
                    strcpy(acFilterField[0], acTmpFilterField[iIdx++]);

                if (logMonColRow->logMonColumnCol2 != 0)
                    strcpy(acFilterField[1], acTmpFilterField[iIdx++]);

                if (logMonColRow->logMonColumnCol3  != 0)
                    strcpy(acFilterField[2], acTmpFilterField[iIdx++]);

                if (logMonColRow->logMonColumnCol4  != 0)
                    strcpy(acFilterField[3], acTmpFilterField[iIdx++]);

                if (logMonColRow->logMonColumnCol5 != 0)
                    strcpy(acFilterField[4], acTmpFilterField[iIdx++]);

                if ( logMonColRow->logMonColumnCol1 != 0 )
                    DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                        "Thr #%d# Log Col Field 1 <%s> Filter1 <%s>\n",
                        MY_THR_ID,
                        acFilterField[0],
                        logMonColRow->logMonColumnColFilter1));

                if ( logMonColRow->logMonColumnCol2 != 0 )
                    DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                        "Thr #%d# Log Col Field 2 <%s> Filter2 <%s>\n",
                        MY_THR_ID,
                        acFilterField[1],
                        logMonColRow->logMonColumnColFilter2));

                if ( logMonColRow->logMonColumnCol3 != 0 )
                    DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                        "Thr #%d# Log Col Field 3 <%s> Filter3 <%s>\n",
                        MY_THR_ID,
                        acFilterField[2],
                        logMonColRow->logMonColumnColFilter3));

                if ( logMonColRow->logMonColumnCol4 != 0 )
                    DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                        "Thr #%d# Log Col Field 4 <%s> Filter4 <%s>\n",
                        MY_THR_ID,
                        acFilterField[3],
                        logMonColRow->logMonColumnColFilter4));

                if ( logMonColRow->logMonColumnCol5 != 0 )
                    DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                        "Thr #%d# Log Col Field 5 <%s> Filter5 <%s>\n",
                        MY_THR_ID,
                        acFilterField[4],
                        logMonColRow->logMonColumnColFilter5));

                // Check if any pattern found
                // Check filter1 in field1
#ifdef WIN32
                if ((strstr(acFilterField[0],logMonColRow->logMonColumnColFilter1)!=NULL) &&
                         (strlen(logMonColRow->logMonColumnColFilter1) != 0 ) )

#else // Only for Unix
                if ((Common_CheckPattern(acFilterField[0],logMonColRow->logMonColumnColFilter1) == 1) &&
                         (strlen(logMonColRow->logMonColumnColFilter1) != 0 ) )
#endif
                {

                    // Check file size
                    Common_CheckFileSize(gcInfoLogFileMonCol);

                    if (( pfOutFile = fopen(gcInfoLogFileMonCol,"a")) == NULL )
                    {
                        snmp_log_perror("LogMonCol_SearchPatterns:\
                            Error opening output file");
                    }
                    else
                    {
                        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                            "Thr #%d# File <%s> opened\n",
                            MY_THR_ID,
                            gcInfoLogFileMonCol));

                        // Update the Output file
                        fprintf(pfOutFile,"%d | %s | %s\n",
                            iLineCount, logMonColRow->logMonColumnColFilter1,
                            acReadBuffer);

                        // Close the file
                        FCLOSE(pfOutFile);

                        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                            "Thr #%d# File <%s> closed\n",
                            MY_THR_ID,
                            gcInfoLogFileMonCol));

                    }

                    iStrFound = 1;
                   // break;
                }
#ifdef WIN32

                else
                // Check filter2 in field2
                if ((strstr(acFilterField[1],logMonColRow->logMonColumnColFilter2)!=NULL) &&
                    (strlen(logMonColRow->logMonColumnColFilter2) != 0 ) )

#else // Only for Unix

                else
                if ((Common_CheckPattern(acFilterField[1],logMonColRow->logMonColumnColFilter2) == 1) &&
                    (strlen(logMonColRow->logMonColumnColFilter2) != 0 ) )
#endif
                {

                    // Check file size
                    Common_CheckFileSize(gcInfoLogFileMonCol);

                    if (( pfOutFile = fopen(gcInfoLogFileMonCol,"a")) == NULL )
                    {
                        snmp_log_perror("LogMonCol_SearchPatterns:\
                            Error opening output file");
                    }
                    else
                    {
                        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                            "Thr #%d# File <%s> opened\n",
                            MY_THR_ID,
                            gcInfoLogFileMonCol));

                        // Update the Output file
                        fprintf(pfOutFile,"%d | %s | %s\n",
                            iLineCount, logMonColRow->logMonColumnColFilter2,
                            acReadBuffer);

                        // Close the file
                        FCLOSE(pfOutFile);

                        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                            "Thr #%d# File <%s> closed\n",
                            MY_THR_ID,
                            gcInfoLogFileMonCol));

                    }

                    iStrFound = 1;
             //       break;
                }
#ifdef WIN32
                else
                // Check filter3 in field3
                if ((strstr(acFilterField[2],logMonColRow->logMonColumnColFilter3)!=NULL) &&
                    (strlen(logMonColRow->logMonColumnColFilter3) != 0 ))
#else // Only for Unix
                else
                if ((Common_CheckPattern(acFilterField[2],logMonColRow->logMonColumnColFilter3) == 1) &&
                    (strlen(logMonColRow->logMonColumnColFilter3) != 0 ))
#endif
                {

                    // Check file size
                    Common_CheckFileSize(gcInfoLogFileMonCol);

                    if (( pfOutFile = fopen(gcInfoLogFileMonCol,"a")) == NULL )
                    {
                        snmp_log_perror("LogMonCol_SearchPatterns:\
                            Error opening output file");
                    }
                    else
                    {
                        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                            "Thr #%d# File <%s> opened\n",
                            MY_THR_ID,
                            gcInfoLogFileMonCol));

                        // Update the Output file
                        fprintf(pfOutFile,"%d | %s | %s\n",
                            iLineCount, logMonColRow->logMonColumnColFilter3,
                            acReadBuffer);

                        // Close the file
                        FCLOSE(pfOutFile);

                        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                            "Thr #%d# File <%s> closed\n",
                            MY_THR_ID,
                            gcInfoLogFileMonCol));

                    }

                    iStrFound = 1;
               //     break;
                }
#ifdef WIN32
                else
                // Check filter4 in field4
                if ((strstr(acFilterField[3],logMonColRow->logMonColumnColFilter4)!=NULL) &&
                    (strlen(logMonColRow->logMonColumnColFilter4) != 0 ))
#else
                else
                if ((Common_CheckPattern(acFilterField[3],logMonColRow->logMonColumnColFilter4) == 1) &&
                    (strlen(logMonColRow->logMonColumnColFilter4) != 0 ))
#endif
                {

                    // Check file size
                    Common_CheckFileSize(gcInfoLogFileMonCol);

                    if (( pfOutFile = fopen(gcInfoLogFileMonCol,"a")) == NULL )
                    {
                        snmp_log_perror("LogMonCol_SearchPatterns:\
                            Error opening output file");
                    }
                    else
                    {
                        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                            "Thr #%d# File <%s> opened\n",
                            MY_THR_ID,
                            gcInfoLogFileMonCol));

                        // Update the Output file
                        fprintf(pfOutFile,"%d | %s | %s\n",
                            iLineCount, logMonColRow->logMonColumnColFilter4,
                            acReadBuffer);

                        // Close the file
                        FCLOSE(pfOutFile);

                        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                            "Thr #%d# File <%s> closed\n",
                            MY_THR_ID,
                            gcInfoLogFileMonCol));

                    }

                    iStrFound = 1;
                 //   break;
                }
#ifdef WIN32
                else
                // Check filter5 in field5
                if ((strstr(acFilterField[4],logMonColRow->logMonColumnColFilter5)!=NULL) &&
                    (strlen(logMonColRow->logMonColumnColFilter5) != 0 ))
#else // Only for Unix
                else
                if ((Common_CheckPattern(acFilterField[4],logMonColRow->logMonColumnColFilter5) == 1) &&
                    (strlen(logMonColRow->logMonColumnColFilter5) != 0 ))
#endif
                {

                    // Check file size
                    Common_CheckFileSize(gcInfoLogFileMonCol);

                    if (( pfOutFile = fopen(gcInfoLogFileMonCol,"a")) == NULL )
                    {
                        snmp_log_perror("LogMonCol_SearchPatterns:\
                            Error opening output file");
                    }
                    else
                    {
                        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                            "Thr #%d# File <%s> opened\n",
                            MY_THR_ID,
                            gcInfoLogFileMonCol));

                        // Update the Output file
                        fprintf(pfOutFile,"%d | %s | %s\n",
                            iLineCount, logMonColRow->logMonColumnColFilter5,
                            acReadBuffer);

                        // Close the file
                        FCLOSE(pfOutFile);

                        DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                            "Thr #%d# File <%s> closed\n",
                            MY_THR_ID,
                            gcInfoLogFileMonCol));


                    }

                    iStrFound = 1;
                   // break;
                }

            }
            /* If the match occured last time, send the trap */
            if(iStrFound == 1)
            {
		if (Common_SendAlert("LGCM",
                gcInfoLogFileMonCol,
                logMonColRow->logMonColumnAlertMessage,
               logMonColRow->logMonColumnIndex))
            	{
                	snmp_log(LOG_ERR,
                    	"Alert sending failed for <%s>\n",
                    	logMonColRow->logMonColumnLogFile);
           		}                
            }
            

            // Reset the pointers values
            memset(acFilterField[0],0,sizeof(logMonColRow->logMonColumnCol1+1));
            memset(acFilterField[1],0,sizeof(logMonColRow->logMonColumnCol2+1));
            memset(acFilterField[2],0,sizeof(logMonColRow->logMonColumnCol3+1));
            memset(acFilterField[3],0,sizeof(logMonColRow->logMonColumnCol4+1));
            memset(acFilterField[4],0,sizeof(logMonColRow->logMonColumnCol5+1));

            memset(acTmpFilterField[0],0,sizeof(logMonColRow->logMonColumnCol1+1));
            memset(acTmpFilterField[1],0,sizeof(logMonColRow->logMonColumnCol2+1));
            memset(acTmpFilterField[2],0,sizeof(logMonColRow->logMonColumnCol3+1));
            memset(acTmpFilterField[3],0,sizeof(logMonColRow->logMonColumnCol4+1));
            memset(acTmpFilterField[4],0,sizeof(logMonColRow->logMonColumnCol5+1));

        }while(1); // Until the match is found or EOF is reached

        // Close the file, in case pattern was found
        /* CR: File is closed after searching for patterns till E-O-F */
        if (pfInFile)
        {
            FCLOSE(pfInFile);
            DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                "Thr #%d# File <%s> closed\n",
                MY_THR_ID,
                logMonColRow->logMonColumnLogFile));
        }

        // Update the log structure when the filter matches
        // Line that matched
        logMonColRow->logMonColumnReadLine=iLineCount;
        
		// Update the file size
        logMonColRow->logMonColumnFileSize=stStatBuf.st_size;
                
        if ( iStrFound == 1 )
            DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                "Thr #%d# Pattern found <%s> Line Number <%d>\n",
                MY_THR_ID,
                logMonColRow->logMonColumnColFilter1,
                logMonColRow->logMonColumnReadLine));

        // Row Modified Time
        logMonColRow->logMonColumnUpdatedTime_len=MAX_DATE_TIME_LEN;

        (void *) time(&now);

        strcpy(logMonColRow->logMonColumnUpdatedTime,
            (u_char *) date_n_time(&now,
            (size_t*)&(logMonColRow->logMonColumnUpdatedTime_len)));

        iIdx=0;
        while( iIdx < 5 )
        {
            DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
                "Thr #%d# freeing memory.....\n",
                MY_THR_ID));
            if (acFilterField[iIdx])
                free(acFilterField[iIdx]);
            if (acTmpFilterField[iIdx])
                free(acTmpFilterField[iIdx]);
            iIdx++;
        }
    }

    DEBUGMSGTL(("LogMonCol/LogMonCol_SearchPatterns",
        "Thr #%d# Returning\n",
        MY_THR_ID));

    return (iStrFound);
}

/*****************************************************************************
 *
 * name             :   LogMonCol_UpdateDataFile
 * description      :   This function is used to update the MIB data file with
 *                      the latest record
 * input parameters :   logMonColumnTable_context *pstLogColRow
 * output parameters:   None
 * return type      :   int - "0" for success in writing new record,
 *                          - "-1" for error,
 *                          - "1" for success in modifying existing record
 * global variables :   None
 * calls            :   None
 */

#ifndef WIN32

int
LogMonCol_UpdateDataFile(logMonColumnTable_context *pstLogColRow)
{
    logMonColumnTable_context stLogColInFile;
    int iFd=-1;
    int iFoundFlag=0;
    int iRowNum=0;
    unsigned long ulPos=0;


    // Open the MIB data file
    if ( (iFd = open(LOG_MON_COL_DATA_FILE,O_CREAT|O_RDWR,S_IREAD|S_IWRITE)) < 0 )
    {
        snmp_log_perror("Error opening LOG_MON_COL_DATA_FILE");
        return(-1);
    }
    else
    {

        lseek(iFd, 0,SEEK_SET);

        DEBUGMSGTL(("LogMonCol/LogMonCol_UpdateDataFile",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            LOG_MON_COL_DATA_FILE));

        if ( pstLogColRow->logMonColumnStatus == SNMP_ROW_DESTROY )
            pstLogColRow->iThreadActiveFlag = 0;

        while( read(iFd, &stLogColInFile, sizeof(logMonColumnTable_context)) > 0 )
        {
            if ( stLogColInFile.idx == *pstLogColRow->index.oids )
            {

                // Check if the row entry in the file
                // is actually deleted fromthe container
                if ( stLogColInFile.logMonColumnStatus == SNMP_ROW_DESTROY )
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
                    DEBUGMSGTL(("LogMonCol/LogMonCol_UpdateDataFile",
                        "Thr #%d# OID <%u> found FoundFlag <%d>\n",
                        MY_THR_ID,
                        *pstLogColRow->index.oids,
                        iFoundFlag));

                }
                break;
            }

            ulPos = lseek(iFd, 0, SEEK_CUR);
        }

        // Copy the oid, instead copying the pointer
        pstLogColRow->idx = *(pstLogColRow->index.oids);

        if ( write(iFd,pstLogColRow,sizeof(logMonColumnTable_context)) < 0 )
        {
            snmp_log_perror("Error while writing data to LOG_MON_COL_DATA_FILE");
            close(iFd);
            DEBUGMSGTL(("LogMonCol/LogMonCol_UpdateDataFile",
                "Thr #%d# File <%s> closed\n",
                MY_THR_ID,
                LOG_MON_COL_DATA_FILE));

            return(-1);
        }
        else
            DEBUGMSGTL(("LogMonCol/LogMonCol_UpdateDataFile",
                "Thr #%d# Record written\n",
                MY_THR_ID));
    }
    close(iFd);

    DEBUGMSGTL(("LogMonCol/LogMonCol_UpdateDataFile",
        "Thr #%d# File <%s> closed\n",
        MY_THR_ID,
        LOG_MON_COL_DATA_FILE));


    return(iFoundFlag);
}

#else

int
LogMonCol_UpdateDataFile(logMonColumnTable_context *pstLogColRow)
{
    logMonColumnTable_context stLogColInFile;
    HANDLE hFd = NULL;
    DWORD NumberOfBytesRead;
    int iFoundFlag=0;
    int iRowNum=0;
    unsigned long ulPos=0;

    // Open the MIB data file
    hFd = CreateFile   (LOG_MON_COL_DATA_FILE,
                        GENERIC_READ|GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    // Open the MIB data file
    if(hFd == INVALID_HANDLE_VALUE)
    {
        snmp_log_perror("Error opening LOG_MON_COL_DATA_FILE");
        return(-1);
    }
    else
    {
        DEBUGMSGTL(("LogMonCol/LogMonCol_UpdateDataFile",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            LOG_MON_COL_DATA_FILE));


        SetFilePointer(hFd,0,NULL,FILE_BEGIN);

        if ( pstLogColRow->logMonColumnStatus == SNMP_ROW_DESTROY )
            pstLogColRow->iThreadActiveFlag = 0;

        while (ReadFile(hFd, &stLogColInFile, sizeof (logMonColumnTable_context),
                &NumberOfBytesRead, NULL))
        {
                    if (NumberOfBytesRead== 0)
                    {
                    snmp_log(LOG_ERR," %d breaking out \n",GetLastError());
                    break;
                    }
            if ( stLogColInFile.idx == *pstLogColRow->index.oids )
            {

                // Check if the row entry in the file
                // is actually deleted fromthe container
                if ( stLogColInFile.logMonColumnStatus == SNMP_ROW_DESTROY )
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
                    DEBUGMSGTL(("LogMonCol/LogMonCol_UpdateDataFile",
                        "Thr #%d# OID <%u> found FoundFlag <%d>\n",
                        MY_THR_ID,
                        *pstLogColRow->index.oids,
                        iFoundFlag));

                }
                break;
            }

            ulPos = SetFilePointer(hFd,0,NULL,FILE_CURRENT);
            
        }

        // Copy the oid, instead copying the pointer
        pstLogColRow->idx = *(pstLogColRow->index.oids);

        if ( WriteFile (hFd,
                        pstLogColRow,
                        sizeof(logMonColumnTable_context),
                        &NumberOfBytesRead,
                        NULL) == 0 )
        {
            snmp_log_perror("Error while writing data to LOG_MON_COL_DATA_FILE");
            CloseHandle(hFd);
            return(-1);
        }
        else
            DEBUGMSGTL(("LogMonCol/LogMonCol_UpdateDataFile",
                "Thr #%d# Record written\n",
                MY_THR_ID));
    }
    CloseHandle(hFd);

    return(iFoundFlag);
}


#endif
// End: WIP_NURI_ALL : End of the newly added code here

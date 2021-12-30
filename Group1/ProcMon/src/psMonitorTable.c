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
 * $Author: viren $
 * $Log: psMonitorTable.c,v $
 * Revision 1.57  2004/01/23 09:19:51  viren
 * snmp_open, snmp_send and snmp_close changed to snmp_sess_open, snmp_sess_send and snmp_sess_close - review Aravind
 *
 * Revision 1.56  2004/01/23 05:03:36  viren
 * added macro for flcose, and null check after each malloc - reviewed by Vijay
 *
 * Revision 1.55  2004/01/05 04:38:01  saisk
 * Made all includes related to windows as relative path
 *
 * Revision 1.54  2004/01/03 13:22:35  viren
 * Resolved the AT Issue of Row Status
 *
 * Revision 1.53  2003/12/20 12:26:06  rajesh
 * * Fixed problem in checking the table() return value
 *
 * Revision 1.52  2003/12/20 10:35:54  rajesh
 * * Added check Process implementation for Tru-64
 *
 * Revision 1.51  2003/12/17 04:36:59  viren
 * removed a debug mesg before declaration in GetLocalHostIP
 *
 * Revision 1.50  2003/12/16 15:04:07  viren
 * RestartProc was modified to fix the known problems
 *
 * Revision 1.49  2003/12/15 14:13:49  viren
 * Introduced Signal handler to fix unixware start process problem - fixed
 *
 * Revision 1.48  2003/12/14 10:40:07  viren
 * Search File function has wrong condition - fixed
 *
 * Revision 1.42  2003/12/12 14:30:03  viren
 * proc comparision in all the OS's code was removed
 *
 * Revision 1.41  2003/12/12 08:37:08  viren
 * Fixed "defunct" processes problem
 *
 * Revision 1.40  2003/12/11 17:57:39  vijaysp
 * HP u_comm returns basename of process. so no need to parse for basename of process. Few codes are removed
 *
 * Revision 1.39  2003/12/11 04:26:36  viren
 * Fixed data file updation issue
 *
 * Revision 1.38  2003/12/10 11:02:01  ashok
 * added extra conversion()
 *
 * Revision 1.37  2003/12/10 08:55:52  ashok
 * Added funtion protype for conversion()
 *
 * Revision 1.36  2003/12/10 06:33:08  viren
 * check proc methods changed
 *
 * Revision 1.35  2003/12/10 06:07:20  viren
 * removed kvm meothod for solaris
 *
 * Revision 1.34  2003/12/10 06:03:24  viren
 * pcCurrTime for windows changed
 *
 * Revision 1.33  2003/12/10 06:00:06  viren
 * MIN_PARENT_FD and MAX_PARENT_FD introduced
 *
 * Revision 1.32  2003/12/09 19:44:26  viren
 * file close problems fixed
 *
 * Revision 1.31  2003/12/09 18:07:20  viren
 * File transfer implemented - Working fine
 *
 * Revision 1.30  2003/12/09 13:50:55  viren
 * File transfer implemented
 *
 * Revision 1.29  2003/12/09 06:47:25  ashok
 * Minor Changes
 *
 * Revision 1.28  2003/12/08 18:22:01  viren
 * Updated review comments
 *
 * Revision 1.27  2003/12/08 12:56:54  ashok
 * File operations changed for windows
 *
 * Revision 1.26  2003/12/07 15:08:27  viren
 * Review comments incorporated
 *
 * Revision 1.25  2003/12/06 14:46:46  buvanes
 * Removed comment within comment
 *
 * Revision 1.24  2003/12/05 10:48:33  sig
 *  Minor changes in ProcMon_CheckProc()
 *
 * Revision 1.22  2003/12/02 11:01:12  vijaysp
 * Trap msg. format changed. TrapOID value changed from Nuri OID to table oid and arg. changed to alert message string alone
 *
 * Revision 1.21  2003/12/02 06:31:07  vijaysp
 * Bug found in windows testing solved. line 2189 changed. mutex lock added
 *
 * Revision 1.19  2003/12/01 07:14:10  ashok
 * Minor Changes
 *
 * Revision 1.18  2003/11/28 12:16:39  ashok
 * Minor Changes
 *
 * Revision 1.17  2003/11/27 15:02:18  viren
 * compilation error fixed
 *
 * Revision 1.16  2003/11/27 09:21:39  viren
 * Reaction path check was wrong
 *
 * Revision 1.15  2003/11/27 08:37:48  viren
 * comment was given between another comment, which caused compilataion error - fixed
 *
 * Revision 1.14  2003/11/27 06:56:26  viren
 * HP -Check proc one ")" was missing at line 2575
 *
 * Revision 1.13  2003/11/27 06:27:25  kranti
 * unixware header file is added in psMonitorTable.c
 *
 * Revision 1.12  2003/11/27 06:22:10  ashok
 * Minor Changes
 *
 * Revision 1.10  2003/11/26 12:51:13  viren
 * DIR_SEPARATOR was given wronly for unix - fixed
 *
 * Revision 1.9  2003/11/26 12:11:48  viren
 * DIR_SEPARATOR was given wronly for unix - fixed
 *
 * Revision 1.8  2003/11/26 10:57:12  viren
 * Fixed reaction path problem
 *
 * Revision 1.6  2003/11/26 08:00:16  ashok
 * Minor Changes
 *
 * Revision 1.5  2003/11/26 06:39:01  ashok
 * Minor Changes
 *
 * Revision 1.4  2003/11/26 05:03:53  ashok
 * Minor Changes
 *
 * Revision 1.3  2003/11/25 11:51:11  ashok
 * Minor Changes
 *
 * Revision 1.2  2003/11/24 16:05:17  sig
 * ProcMon_CheckProc has been modified for linux
 *
 * Revision 1.1  2003/11/24 06:44:00  viren
 * moved from old dir
 *
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
 * $Id: psMonitorTable.c,v 1.57 2004/01/23 09:19:51 viren Exp $
 * $Name: NASCenterAgent_29Jan2004_Patch3_SrcRls $
 *
 ****************************************************************************/

/********************************* includes *********************************/
// <Start: WIP_NURI_TRU: Added Headers>
#ifdef osf5
#include <sys/table.h>
#include <sys/sysinfo.h>
#include <sys/user.h>
#include <unistd.h>
#endif
// <End: WIP_NURI_TRU>
// <Start: WIP_NURI_UXW: Added Headers>
/* added by HJ.Kwon. 2005.06.15. for compile */
#ifdef sysv4
#define _KMEMUSER
#include <sys/metrics.h>
#define METPTR_SYMBOL "met_localdata_ptrs_p"
#endif
// <End: WIP_NURI_UXW>
// <Start: WIP_NURI_SOL: Added REENTRANT>
#define _REENTRANT
#define MALLOC_CHECK_ 1
// <End: WIP_NURI_SOL>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
// <Start: WIP_NURI_ALL: Added snmp.h>
#include <net-snmp/library/snmp.h>
#include <net-snmp/library/snmp_assert.h>
// <End: WIP_NURI_ALL>

#ifdef WIN32
#include "../inc/psMonitorTable.h"
#include "vbregexp_header.h"
#include "getpscmdline.h"
#include <tchar.h>
#else
#include "psMonitorTable.h"
#include <arpa/inet.h>
#endif


#define FCLOSE(x) fclose(x);\
                  x=NULL

/** added by Corbin in 2004.02.03
 **/
#define CHECK_INTERVAL	10 // seconds
#define MAX(x,y) ((x)>(y)?(x):(y))
static ps_cache_info *gpstPsInfo = NULL;
static ps_cache_info *gpstPrePsInfo = NULL;

/** shkim 2008.04.01
 **/
static int full_process_list = 0;

#ifdef WIN32

#else
int ProcMon_CheckPattern(char *, char *);
int ProcMon_CompilePattern(char *);
#endif

#ifdef linux
void getPsStat(char *, ps_cache_info *);
int getProcessorNum();
#endif

static int cpuNo = 1;

/** added by D.J Park in 2005.02.23 [start]
 * cpu 정보를 주기적으로 구한다.
 */
psmon_cpuinfo_t psMonCpuInfo = {0};
/** added by D.J Park in 2005.02.23 [end]*/
	
static     netsnmp_handler_registration *my_handler = NULL;
static     netsnmp_table_array_callbacks cb;

static oid  objid_sysuptime[] = { 1, 3, 6, 1, 2, 1, 1, 3, 0 };
static oid  objid_snmptrap[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };


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

    /** added by D.J Park in 2004.10.15 */
    dst->psMonitorThreshold = src->psMonitorThreshold;
    dst->psMonitorCPU = src->psMonitorCPU;
    dst->psMonitorMEM = src->psMonitorMEM;
    /** added by D.J Park in 2004.03.02 */
    dst->psMonitorStartHour = src->psMonitorStartHour;
    dst->psMonitorEndHour = src->psMonitorEndHour;

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
     ctx->psMonitorNumber = 0;
    /**
     ctx->psMonitorName = 0;
     ctx->psMonitorParameter = 0;
     ctx->psMonitorCheckInterval = 0;
     ctx->psMonitorFilter = 0;
     ctx->psMonitorDescription = 0;
     ctx->psMonitorCreatedDateTime = 0;
     ctx->psMonitorReaction = 0;
     ctx->psMonitorReactionPath = 0;
     ctx->psMonitorAlertMessage = 0;
     ctx->psMonitorStatus = 0;
    */
     ctx->psMonitorStartHour = -1;
     ctx->psMonitorEndHour = -1;

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
                    // <Start: WIP_NURI_ALL: modified here>
                    //sizeof(row_ctx->psMonitorName));
                    var->val_len);
                    // <End: WIP_NURI_ALL>

        break;

        case COLUMN_PSMONITORPARAMETER:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                   // <Start: WIP_NURI_ALL: modified here>
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
        break;

        case COLUMN_PSMONITORFILTER:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                     // <Start: WIP_NURI_ALL: modified here>
                     //sizeof(row_ctx->psMonitorFilter));
                     var->val_len);
                     // <End: WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORDESCRIPTION:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    // <Start: WIP_NURI_ALL: modified here> 
                    //sizeof(row_ctx->psMonitorDescription));
                    var->val_len);
                    // <End: WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORCREATEDDATETIME:
            /** DateAndTime = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR, 
                    // <Start: WIP_NURI_ALL: modified here> 
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
                    // <Start: WIP_NURI_ALL: modified here>
                    //sizeof(row_ctx->psMonitorReactionPath));
                    var->val_len);
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORALERTMESSAGE:
            /** DisplayString = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
            // <Start: WIP_NURI_ALL: modified here>
            //sizeof(row_ctx->psMonitorAlertMessage));
            var->val_len);
            // <End: WIP_NURI_ALL>
        break;

        case COLUMN_PSMONITORSTATUS:
            /** RowStatus = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->psMonitorStatus));
        break;

        /** added by D.J Park in 2004.10.15 */
        case COLUMN_PSMONITORTHRESHOLD:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->psMonitorThreshold));
        break;

        case COLUMN_PSMONITORCPU:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->psMonitorCPU));
        break;

        case COLUMN_PSMONITORMEM:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->psMonitorMEM));
        break;

        /** added by D.J Park in 2004.03.02 */
        case COLUMN_PSMONITORSTARTHOUR:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->psMonitorStartHour));
        break;

        case COLUMN_PSMONITORENDHOUR:
            /** INTEGER = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER, 
                    sizeof(row_ctx->psMonitorEndHour));
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
#ifndef WIN32 // added from Hurwonsuck
            /** added by Corbin in 2004.02.05
             **/            
            if ( var != NULL && var->val.string != NULL && strlen(var->val.string) > 0 ) {
	            if ( ProcMon_CompilePattern(var->val.string) != 0 ) {
	            	rc = SNMP_ERR_BADVALUE;
	            }
	        }
#endif
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

        /** added by D.J Park in 2004.10.15 */
        case COLUMN_PSMONITORTHRESHOLD:
            /** INTEGER = ASN_INTEGER */
            /** TODO : check the validation of CPU(0-100)
        break;
        case COLUMN_PSMONITORCPU:
            /** INTEGER = ASN_INTEGER */
            /** TODO : check the validation of CPU(0-100)
        break;
        case COLUMN_PSMONITORMEM:
            /** INTEGER = ASN_INTEGER */
            /** TODO : check the validation of MEM
        break;

        /** added by D.J Park in 2005.03.02 */
        case COLUMN_PSMONITORSTARTHOUR:
            /** INTEGER = ASN_INTEGER */
            /** TODO : check the validation of MEM
        break;
        case COLUMN_PSMONITORENDHOUR:
            /** INTEGER = ASN_INTEGER */
            /** TODO : check the validation of MEM
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

        /** added by D.J Park in 2004.10.15 */
        case COLUMN_PSMONITORTHRESHOLD:
            /** RowStatus = ASN_INTEGER */
            row_ctx->psMonitorThreshold = *var->val.integer;
        break;

        case COLUMN_PSMONITORCPU:
            /** RowStatus = ASN_INTEGER */
            row_ctx->psMonitorCPU = *var->val.integer;
        break;

        case COLUMN_PSMONITORMEM:
            /** RowStatus = ASN_INTEGER */
            row_ctx->psMonitorMEM = *var->val.integer;
        break;

        /** added by D.J Park in 200l.03.02 */
        case COLUMN_PSMONITORSTARTHOUR:
            /** RowStatus = ASN_INTEGER */
            row_ctx->psMonitorStartHour = *var->val.integer;
        break;

        case COLUMN_PSMONITORENDHOUR:
            /** RowStatus = ASN_INTEGER */
            row_ctx->psMonitorEndHour = *var->val.integer;
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
    time_t          stNow;

    DEBUGMSGTL(("ProcMon/psMonitorTable_set_commit",
                    "Commit is being called\n"));

    if ( ProcMon_UpdateDataFile(row_ctx) == 0 )
    {
        DEBUGMSGTL(("ProcMon/psMonitorTable_set_commit",
            "Row updated in the PROC_MON_DATA_FILE\n"));
    }

    // Do the value check, before proceeding
    if ( (strlen((char*)row_ctx->psMonitorName) != 0 ) &&
        ( row_ctx->psMonitorCheckInterval != 0 ) &&
        ( row_ctx->psMonitorStatus == SNMP_ROW_ACTIVE ) )
    {

        // Row Created/ Changed Time
        row_ctx->psMonitorCreatedDateTime_len=MAX_DATE_TIME_LEN;

        // Get the Date-Time in time_t format
        // and update the created time for the newly
        // created row
        (void *) time(&stNow);

        strcpy(row_ctx->psMonitorCreatedDateTime,
                (u_char *) date_n_time(&stNow,
                (size_t*)&(row_ctx->psMonitorCreatedDateTime_len)));

        // i.e. if the thr has not been created yet
        if ( row_ctx->iThreadActiveFlag == 0 )
        {
            if ( pthread_create(&pPsThr, NULL, ProcMon_ProcMonThreadMain,
                            (void*)row_ctx) != 0 )
            {
                snmp_log_perror("Thread creation failed");
            }
            else
            {

                // Change it to active
                row_ctx->iThreadActiveFlag = 1;

                DEBUGMSGTL(("ProcMon/psMonitorTable_set_commit",
                    "Thread started for < %u %d %s >\n",
                    row_ctx->idx,
                    row_ctx->index.len,
                    row_ctx->psMonitorName));
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

        /** added by D.J Park in 2004.10.15 */
        case COLUMN_PSMONITORTHRESHOLD:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORCPU:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORMEM:
            /** INTEGER = ASN_INTEGER */
        break;

        /** added by D.J Park in 200l.03.02 */

        case COLUMN_PSMONITORSTARTHOUR:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORENDHOUR:
            /** INTEGER = ASN_INTEGER */
        break;

        default: /** We shouldn't get here */
            /** should have been logged in reserve1 */
        // <Start: WIP_NURI_ALL: modified to add break statment>
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

        /** added by D.J Park in 2004.10.15 */
        case COLUMN_PSMONITORTHRESHOLD:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORCPU:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORMEM:
            /** INTEGER = ASN_INTEGER */
        break;

        /** added by D.J Park in 200l.03.02 */

        case COLUMN_PSMONITORSTARTHOUR:
            /** INTEGER = ASN_INTEGER */
        break;

        case COLUMN_PSMONITORENDHOUR:
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
    DEBUGMSGTL(("ProcMon/initialize_table_psMonitorTable",
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

        /** added by D.J Park in 2004.10.15 */
        case COLUMN_PSMONITORTHRESHOLD:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->psMonitorThreshold,
                         sizeof(context->psMonitorThreshold) );
        break;

        case COLUMN_PSMONITORCPU:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->psMonitorCPU,
                         sizeof(context->psMonitorCPU) );
        break;

        case COLUMN_PSMONITORMEM:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->psMonitorMEM,
                         sizeof(context->psMonitorMEM) );
        break;

        /** added by D.J Park in 200l.03.02 */
        case COLUMN_PSMONITORSTARTHOUR:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->psMonitorStartHour,
                         sizeof(context->psMonitorStartHour) );
        break;

        case COLUMN_PSMONITORENDHOUR:
            /** INTEGER = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->psMonitorEndHour,
                         sizeof(context->psMonitorEndHour) );
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

/** Added by Corbin in 2004.02.02
 **/
 
/*****************************************************************************
 *
 * name             :   ProcMon_ProcCheckThreadMain
 * description      :   This is the main function for process checking thread
 * input parameters :   None
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   None
 */
void *
ProcMon_ProcCheckThreadMain(void *psData)
{
	ps_cache_info *pstNewList = NULL;
	ps_cache_info *pstOldList = NULL;
	ps_cache_info *pstPsInfo = NULL;
	
    /** added by D.J Park in 2005.02.24 [start] */
    int     cpuSetCounter = 0;
    /** added by D.J Park in 2005.02.24 [end] */

	/** shkim 2008.04.02 */
	char	tbuf[30];
	int		ret = 0;

#ifdef WIN32
    InitializeRegExp();
#endif
	
    while(1) {
		ret = 0;
        pstNewList = ProcMon_GetProcessList(&ret);

		if (ret == 0) {	/* made partial process list */
			getCurrentTime(tbuf);
            snmp_log(LOG_ERR, "[%s] CheckThreadMain: ret=[%d]\n", tbuf, ret);

			/* not swap old list, just free partial list */

			pthread_mutex_lock(&procMonProcessCheckLock);		
			
			full_process_list = 0; 	/* constructing process list failed */

			pthread_mutex_unlock(&procMonProcessCheckLock);

            while(pstNewList != NULL) {	
                pstPsInfo = pstNewList->next;
                free(pstNewList);
                pstNewList = pstPsInfo;
			}
		} else {		/* no error while reading /proc */
			if (pstNewList != NULL) {
				pthread_mutex_lock(&procMonProcessCheckLock);		
				pstOldList = gpstPrePsInfo;
				gpstPrePsInfo = gpstPsInfo;
				gpstPsInfo = pstNewList;

				full_process_list = 1;

				pthread_mutex_unlock(&procMonProcessCheckLock);

				while(pstOldList != NULL) {
					pstPsInfo = pstOldList->next;
					free(pstOldList);
					pstOldList = pstPsInfo;
				}
			} else {
				getCurrentTime(tbuf);
				snmp_log(LOG_ERR,
					" %s #%d# ProcCheckThread ProcMon_GetProcessList failed\n",
					tbuf, MY_THR_ID);
        	}

			/** added by D.J Park in 2005.02.24 [start] 
			* glAgentInfoSerialNo 값 설정에 따라 [glAgentInfoSerialNo * 10 second]
			* 일정 주기로 cpu 상태 값을 구한다.
			*/

			if(glAgentInfoSerialNo < 1)
				glAgentInfoSerialNo = 6;

			cpuSetCounter = cpuSetCounter%glAgentInfoSerialNo;

			if(cpuSetCounter == 0)
				setCpuAverage();

			cpuSetCounter++;
			/** added by D.J Park in 2005.02.24 [end] */
		}

        SLEEP(CHECK_INTERVAL);
    }
}

#if defined(solaris2)  // Check proc for solaris
void getProcessNameInCommand(char *command,char *pname)
{
    char combuf[200]={0};
    char *strp;
    int slen;
    int i;
    
    slen = strlen(command);
    for(i = 0 ; i < slen ; i++)
    {
        if(command[i] == ' ')
            break;
        combuf[i] = command[i];
    }
    slen = strlen(combuf);
    strp = &combuf[slen -1];
    for( i = 0 ; i < slen ; i++)
    {
        if(*strp == '/')
            break;
        strp--;
    }
    strp++;
    strcpy(pname,strp);
}


ps_cache_info*
ProcMon_GetProcessList(int* ret)
{
    ps_cache_info		*pstPsHead = NULL;
    ps_cache_info		*pstPsNew = NULL;
    ps_cache_info		*pstPsLast = NULL;
    long                iPid=-1;
#if _SLASH_PROC_METHOD_
    psinfo_t stPsInfo;
    DIR                 *pstProcDir;
    struct dirent       *pstDirEnt;
    int                 iProcFd;
    char                acProcFn[sizeof "/proc/00000/psinfo"];
	
    char 				buf[PROC_ARG_LEN];
    char				*strArgs;

	/* shkim 2008.04.02 */
	char				tbuf[30];

	*ret = 0;			/* first, set failed */

    DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
        "Thr #%d# for solaris <SLASH_PROC_METHOD> called\n",
        MY_THR_ID));

    if ((pstProcDir = opendir ((const char*)"/proc")) == NULL) {
        snmp_log_perror ("Unable to open /proc dir\n");
        return pstPsHead;
    }

    DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
        "Thr #%d# File </proc> opened\n",
        MY_THR_ID));

    while (((pstDirEnt = readdir (pstProcDir)) != NULL) ) {
        if ((!strcmp (pstDirEnt->d_name, ".")) ||
            (!strcmp (pstDirEnt->d_name, "..")))
        {
            continue;
        }

        iPid = atoi(pstDirEnt->d_name);

        sprintf(acProcFn, "/proc/%.5d/psinfo", iPid);

        if ((iProcFd = open(acProcFn, O_RDONLY)) != -1) 
		{
            DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                "Thr #%d# File <%s> opened\n",
                MY_THR_ID,
                acProcFn));

            if (read(iProcFd, &stPsInfo, sizeof(stPsInfo)) !=
                                        sizeof(stPsInfo))
            { 
				getCurrentTime(tbuf);
            	snmp_log(LOG_ERR, "[%s] Error while read /proc: *ret=[%d]\n", 
					tbuf, *ret);
                snmp_log_perror("Error while read /proc");
                DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                    "Thr #%d# Files <%s> and </proc> closed\n",
                    MY_THR_ID,
                    acProcFn));

                close(iProcFd);
                closedir(pstProcDir);
                return pstPsHead;
            }

            DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                "Thr #%d# File <%s> closed\n",
                MY_THR_ID,
                acProcFn));

            close(iProcFd);
        }
        else
        {
        	continue;
        }

        pstPsNew = (ps_cache_info*)malloc(sizeof(ps_cache_info));
        if (pstPsNew == NULL) {
            snmp_log(LOG_ERR, "ProcMon_GetProcessList ps_cache_info malloc failed");
            continue;
    	}
        memset(pstPsNew,0x00,sizeof(ps_cache_info));
		pstPsNew->pid = iPid;
        /** modified by D.J Park in 2005.02.28 [start]
		strcpy(pstPsNew->name, stPsInfo.pr_fname);
         */
        if(strlen(stPsInfo.pr_fname) > 14)
            getProcessNameInCommand(stPsInfo.pr_psargs,pstPsNew->name);
        else
            strcpy(pstPsNew->name, stPsInfo.pr_fname);
        /** modified by D.J Park in 2005.02.28 [end]*/
		
        strArgs = stPsInfo.pr_psargs;
        strArgs = strchr(strArgs, ' ');
		
        if (strArgs != NULL){
            if (strcmp(stPsInfo.pr_fname, "oracle") == 0) {
                strArgs[0] = '\0';       	
                strArgs = stPsInfo.pr_psargs;
            } else {
                strArgs++;
            }			
        } else {
            if (strcmp(stPsInfo.pr_fname, "oracle") == 0) {
                strcpy(buf, stPsInfo.pr_psargs);
            } else {
                strcpy(buf, "");
            }
            strArgs = buf;
        }
			
        if (strArgs[0] == ' ')
            strArgs++;
        	
        strcpy(pstPsNew->args, strArgs);

        /** added by D.J Park in 2004.10.14 */
        //pstPsNew->time = getcstime();
        //pstPsNew->cputime = stPsInfo.pr_time.tv_sec * 100 +
        //    stPsInfo.pr_time.tv_nsec / 10000000;
        pstPsNew->time = 0;
        pstPsNew->cputime = 0;
        pstPsNew->mem = stPsInfo.pr_rssize;
        pstPsNew->hcpu = (long)((double)stPsInfo.pr_pctcpu * 10000/0x8000);
        pstPsNew->cpu = pstPsNew->hcpu / 100;
        /*
        snmp_log(LOG_INFO,"PID[%d] NAME[%s] CPUT[%ld] CPU[%ld|%ld] "
               " MEM[%ld]\n",pstPsNew->pid,pstPsNew->name
                ,pstPsNew->cputime,pstPsNew->cpu,stPsInfo.pr_pctcpu
                ,pstPsNew->mem);
        */
		
        pstPsNew->next = NULL;
        
        if (pstPsHead != NULL)
            pstPsLast->next = pstPsNew;
        	
        if (pstPsHead == NULL)
            pstPsHead = pstPsNew;
        	
        pstPsLast = pstPsNew;

    } // End of while
    

    DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
        "Thr #%d# File </proc> closed\n",
        MY_THR_ID));

    closedir(pstProcDir);

	*ret = 1;		/* set success */
    
    return pstPsHead;        
#endif
}


/** added by D.J Park in 2005.02.23 [start]
 * cpu 정보를 주기적으로 구한다.
 */

void setCpuAverage()
{
    int             cpu_number=-1;
    char            *cpuname="cpu_stat";
    cpu_stat_t      cpustat;
    kstat_t         *ksp;
    kstat_ctl_t     *kc;
    int             i;
    int state[4] ={0};
    time_t curTime = 0;
    long total = 0;
    long pretotal = 0;
    long curtotal = 0;
   
    long userUsage = 0;
    long kernelUsage = 0;
    long waitUsage = 0;
    long idleUsage = 0;
    long utilUsage = 0;

    long userUsageFloat = 0;
    long kernelUsageFloat = 0;
    long waitUsageFloat = 0;
    long idleUsageFloat = 0;
    long utilUsageFloat = 0;

    char                    tbuf[30] ={0};

    // Get online processors
    psMonCpuInfo.ncpus = sysconf(_SC_NPROCESSORS_ONLN);

    // Open Kstat
    if ((kc = kstat_open()) == NULL)
    {
       fprintf(stderr,"get_cpu_average:kstat_open failed");
       return;
    }

    time(&curTime);
    for (ksp = kc->kc_chain; ksp != NULL; ksp = ksp->ks_next)
    {
        if ( (strncmp(ksp->ks_name, cpuname, strlen(cpuname)) == 0) )
          {
             sscanf(ksp->ks_name,"cpu_stat%d", &cpu_number);

             if (kstat_read(kc, ksp, &cpustat) < 0 )
             {
                fprintf(stderr,"get_cpu_average:kstat_read failed");
                continue;
             }
             state[CPU_USER] += cpustat.cpu_sysinfo.cpu[CPU_USER];
             state[CPU_KERNEL] += cpustat.cpu_sysinfo.cpu[CPU_KERNEL];
             state[CPU_WAIT] += cpustat.cpu_sysinfo.cpu[CPU_WAIT];
             state[CPU_IDLE] += cpustat.cpu_sysinfo.cpu[CPU_IDLE];
          }
    }
    kstat_close(kc);

	curtotal = state[CPU_USER] + state[CPU_KERNEL] 
        + state[CPU_WAIT] + state[CPU_IDLE];

	if(psMonCpuInfo.sampletime < 1)
	{
		psMonCpuInfo.stateUsage[NAS_CPU_USER] = 0;
		psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] = 0;
		psMonCpuInfo.stateUsage[NAS_CPU_WAIT] = 0;
		psMonCpuInfo.stateUsage[NAS_CPU_IDLE] = 0;
		psMonCpuInfo.stateUsage[NAS_CPU_UTIL] = 0;

		psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] = 0;
		psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] = 0;
		psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] = 0;
		psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] = 0;
		psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] = 0;
	} 
	else
	{
		pretotal = psMonCpuInfo.total;
		total = curtotal - pretotal;
		if(total > 0)
		{
			userUsageFloat =(long) ((state[CPU_USER]-psMonCpuInfo.state[NAS_CPU_USER]) * 10000.0 /(float)total);
			kernelUsageFloat = (long)((state[CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_KERNEL]) * 10000.0 /(float)total);
			waitUsageFloat = (long)((state[CPU_WAIT]-psMonCpuInfo.state[NAS_CPU_WAIT]) * 10000.0 /(float)total);
			idleUsageFloat = (long)((state[CPU_IDLE]-psMonCpuInfo.state[NAS_CPU_IDLE]) * 10000.0 /(float)total);
			utilUsageFloat = (long)((state[CPU_USER]+state[CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_UTIL]) * 10000.0 /(float)total);

			userUsage =(long) (userUsageFloat / 100.0);
			kernelUsage =(long) (kernelUsageFloat / 100.0);
			waitUsage =(long) (waitUsageFloat / 100.0);
			idleUsage =(long) (idleUsageFloat / 100.0);
			utilUsage =(long) (utilUsageFloat / 100.0);

			psMonCpuInfo.stateUsage[NAS_CPU_USER] = 
                userUsage > 100 ? 100 : userUsage;
			psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] = 
                kernelUsage > 100 ? 100 : kernelUsage;
			psMonCpuInfo.stateUsage[NAS_CPU_WAIT] = 
                waitUsage > 100 ? 100 : waitUsage;
			psMonCpuInfo.stateUsage[NAS_CPU_IDLE] = 
                idleUsage > 100 ? 100 : idleUsage;
			psMonCpuInfo.stateUsage[NAS_CPU_UTIL] = 
                utilUsage > 100 ? 100 : utilUsage;

			psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] = 
                userUsageFloat > 10000 ? 10000 : userUsageFloat;
			psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] = 
                kernelUsageFloat > 10000 ? 10000 : kernelUsageFloat;
			psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] = 
                waitUsageFloat > 10000 ? 10000 : waitUsageFloat;
			psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] = 
                idleUsageFloat > 10000 ? 10000 : idleUsageFloat;
			psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] = 
                utilUsageFloat > 10000 ? 10000 : utilUsageFloat;
		}
	}
	psMonCpuInfo.sampletime = curTime;
	psMonCpuInfo.total = curtotal;
	psMonCpuInfo.state[NAS_CPU_USER] = state[CPU_USER];
	psMonCpuInfo.state[NAS_CPU_KERNEL] = state[CPU_KERNEL];
	psMonCpuInfo.state[NAS_CPU_WAIT] = state[CPU_WAIT];
	psMonCpuInfo.state[NAS_CPU_IDLE] = state[CPU_IDLE];
	psMonCpuInfo.state[NAS_CPU_UTIL] = state[CPU_USER]+state[CPU_KERNEL];
    getCurrentTime(tbuf);
    DEBUGMSGTL(("ProcMon/setCpuAverage",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
            psMonCpuInfo.stateUsage[NAS_CPU_USER],
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL]));
    DEBUGMSGTL(("ProcMon/setCpuAverageFloat",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL]));
}
/** added by D.J Park in 2005.02.23 [end]*/

#elif defined(AIX_4_3)
ps_cache_info*
ProcMon_GetProcessList(int* ret) 
{	
    ps_cache_info		*pstPsHead = NULL;
    ps_cache_info		*pstPsNew = NULL;
    ps_cache_info		*pstPsLast = NULL;
	
    int iIdx=0;
    int iMaxNumProc = 1024;
    int  iNumProc=0;
    int iPid=-1;
    int iIndex=0;
    struct procsinfo *pstProcTable=NULL;
    /** added by Corbin in 2004.02.02
    **/
    int path_idx=0;
    int skip_idx=0;
    char buf[PROC_ARG_LEN];
    char string[PROC_ARG_LEN];
    char *strArgs;

	*ret = 0;
	
    DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
        "Thr #%d# Called for AIX 4.3\n",
        MY_THR_ID));
        // Reset the index
    do
    {
        //check whether we are here in the loop, then pstProcTable will
        //have some data. It will be NULL for the first time.
        if(pstProcTable != NULL)
            free(pstProcTable);

        pstProcTable = (struct procsinfo *)calloc(iMaxNumProc,sizeof(struct procsinfo));

        if(pstProcTable == NULL)
        {
            DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                "Thr #%d# Calloc failed\n",
                MY_THR_ID));
            return pstPsHead;
        }

        iIdx = 0;

        // Now Get process table populated for the active processes.
        // Get the list of process currently active
        iNumProc = getprocs(pstProcTable, sizeof(struct procsinfo),
                                                NULL, 0,&iIdx,iMaxNumProc);

        if (iNumProc == -1)
        {
            DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                "Thr #%d# Number of procs is -1\n",
                MY_THR_ID));
            iNumProc = 0 ;
            if(pstProcTable != NULL)
               free(pstProcTable);

            return pstPsHead;
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

        /* Check for zombie process */
        if(pstProcTable[iIndex].pi_state == SZOMB)
        {
            strcpy(pstProcTable[iIndex].pi_comm, "<defunct>"); /* Zombie process */
        }

        if(strcmp(pstProcTable[iIndex].pi_comm,"")==0)
        {
            strcpy(pstProcTable[iIndex].pi_comm,"unknown");
        }
        
        /* Get process command parameters */
        memset(buf,'\0',PROC_ARG_LEN);
		
		if(getargs(&pstProcTable[iIndex],sizeof(pstProcTable[iIndex]),buf,PROC_ARG_LEN)==0)
		{
			if (strcmp(pstProcTable[iIndex].pi_comm, "oracle") == 0) {
				strcpy(string, buf);
			} else {
				skip_idx = 0;
				do{
					if(buf[skip_idx] == ' ')
					{
						skip_idx++; /* Parameters present */
						break;
					}
					else if(buf[skip_idx] == '\0')
					{
						break; /* No parameters */
					}
					else
					{
						skip_idx++; /*	Path name*/
					}
				}while(1);
				for(path_idx=0;skip_idx<PROC_ARG_LEN;skip_idx++,path_idx++)
				{
					if(buf[skip_idx] == '\0' )
					{
						string[path_idx] = ' ';
					}
					else
					{
						string[path_idx] = buf[skip_idx];
					}
					if(buf[skip_idx] == '\0' && buf[(skip_idx+1)]=='\0')
					{
						string[path_idx]='\0';
						break;
					}
				}
			}
			strArgs = string;					
		}
		else
		{
			strcpy(buf, "unknown");
			strArgs = buf;
		}
		
		if (strArgs[0] == ' ')
			strArgs++;
			
    	pstPsNew = (ps_cache_info*)malloc(sizeof(ps_cache_info));
    	if (pstPsNew == NULL) {
    		snmp_log(LOG_ERR, "ProcMon_GetProcessList ps_cache_info malloc failed");
    		continue;
    	}
		pstPsNew->pid = pstProcTable[iIndex].pi_pid;
		strcpy(pstPsNew->name, pstProcTable[iIndex].pi_comm);
		strcpy(pstPsNew->args, strArgs);

        /** added by D.J Park in 2004.10.14 */
		pstPsNew->time = getcstime();
		if(pstProcTable[iIndex].pi_state == SZOMB)
		{
			pstPsNew->cputime = pstProcTable[iIndex].pi_utime * 100 
				+ pstProcTable[iIndex].pi_stime * 100;
		}
		else
		{
			pstPsNew->cputime = pstProcTable[iIndex].pi_ru.ru_utime.tv_sec*100 
				+ pstProcTable[iIndex].pi_ru.ru_stime.tv_sec * 100;
		}
		pstPsNew->mem = pstProcTable[iIndex].pi_trss 
			+ pstProcTable[iIndex].pi_drss;
        pstPsNew->hcpu = getPercentCPU(pstPsNew->pid,pstPsNew->time,
                pstPsNew->cputime,cpuNo);
        pstPsNew->cpu = pstPsNew->hcpu / 100;
        /*
        snmp_log(LOG_INFO,"PID[%d] NAME[%s] CPUT[%ld] CPU[%ld] " 
                " MEM[%ld]\n",pstPsNew->pid,pstPsNew->name 
                ,pstPsNew->cputime,pstPsNew->cpu,pstPsNew->mem);
        */
		      
        pstPsNew->next = NULL;
        
        if (pstPsHead != NULL)
        	pstPsLast->next = pstPsNew;
        	
        if (pstPsHead == NULL)
        	pstPsHead = pstPsNew;
        	
        pstPsLast = pstPsNew;
        
    } // End of for

    if (pstProcTable != NULL)
        free(pstProcTable);

	*ret = 1;

    return pstPsHead;
}

/** added by D.J Park in 2005.02.23 [start]
 * cpu 정보를 주기적으로 구한다.
 */

void setCpuAverage()
{
	long   curtotal;
	long   pretotal;
	long   total;
	
	long   userUsage;
	long   kernelUsage;
	long   waitUsage;
	long   idleUsage;
	long   utilUsage;

	long   userUsageFloat;
	long   kernelUsageFloat;
	long   waitUsageFloat;
	long   idleUsageFloat;
	long   utilUsageFloat;

	long   curTime;
	long   state[4];
    char                    tbuf[30] ={0};

#ifdef KERNEL_64
	struct sysinfo64 sysInfo;
#else
	struct sysinfo sysInfo;
#endif
	struct nlist nl;
	int kmem;
	char *sysinfo_symbol="sysinfo";
	int ssize;
	long ret = 0;

	psMonCpuInfo.ncpus = sysconf(_SC_NPROCESSORS_ONLN);	
	time(&curTime);
#ifdef KERNEL_64
	auto_nlist64("sysinfo", (char *)&sysInfo, sizeof(sysInfo)); 
#else
	auto_nlist("sysinfo", (char *)&sysInfo, sizeof(sysInfo)); 
#endif

    /*
	nl.n_name = sysinfo_symbol;
	ret = knlist(&nl, 1, sizeof(nl));
	if(ret)
	{
		fprintf(stderr,"kernel symbol[%s] not found\n",nl.n_name);
		exit(-1);
	}
	ssize = sizeof(struct sysinfo); 
	memset(&sysInfo,0x00,sizeof(struct sysinfo));

	kmem = open("/dev/kmem",O_RDONLY);
	if(kmem < 0)
	{
		fprintf(stderr,"/dev/kmem open failed\n");
		exit(-1);
	}
	fcntl(kmem,F_SETFD,1);

	ret = lseek(kmem,(off_t)nl.n_value,SEEK_SET);
	if(ret != nl.n_value)
	{
		fprintf(stderr,"/dev/kmem lseek failed \n");
		exit(-1);
	}

	time(&curTime);
	ret = read(kmem,(char *)&sysInfo,sizeof(struct sysinfo));

	if(ret != sizeof(struct sysinfo))
	{
		fprintf(stderr,"/dev/kmem read failed ret[%ld] sysinfo size[%d]\n",
			ret,ssize);
		exit(-1);
	}

	close(kmem);
    */

	state[CPU_USER] = sysInfo.cpu[CPU_USER];
	state[CPU_KERNEL] = sysInfo.cpu[CPU_KERNEL];
	state[CPU_WAIT] = sysInfo.cpu[CPU_WAIT];
	state[CPU_IDLE] = sysInfo.cpu[CPU_IDLE];
	curtotal = state[CPU_USER] + state[CPU_KERNEL] + state[CPU_WAIT] 
		+ state[CPU_IDLE];

    if(psMonCpuInfo.sampletime < 1)
    {
		psMonCpuInfo.stateUsage[NAS_CPU_USER] = 0;
 		psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] = 0;
 		psMonCpuInfo.stateUsage[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_UTIL] = 0;

		psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] = 0;
 		psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] = 0;
 		psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] = 0;
    }
    else
    {
        pretotal = psMonCpuInfo.total;
        total = curtotal - pretotal;
        if(total > 0)
        {
            userUsageFloat =(long) ((state[CPU_USER]-psMonCpuInfo.state[NAS_CPU_USER]) * 10000.0 /(float)total);
            kernelUsageFloat = (long)((state[CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_KERNEL]) * 10000.0 /(float)total);
            waitUsageFloat = (long)((state[CPU_WAIT]-psMonCpuInfo.state[NAS_CPU_WAIT]) * 10000.0 /(float)total);
            idleUsageFloat = (long)((state[CPU_IDLE]-psMonCpuInfo.state[NAS_CPU_IDLE]) * 10000.0 /(float)total);
            utilUsageFloat = (long)((state[CPU_USER]+state[CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_UTIL])* 10000.0 /(float)total);

            userUsage = (long)(userUsageFloat / 100.0);
            kernelUsage = (long)(kernelUsageFloat / 100.0);
            waitUsage = (long)(waitUsageFloat / 100.0);
            idleUsage = (long)(idleUsageFloat / 100.0);
            utilUsage = (long)(utilUsageFloat / 100.0);

            psMonCpuInfo.stateUsage[NAS_CPU_USER] =
                userUsage > 100 ? 100 : userUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] =
                kernelUsage > 100 ? 100 : kernelUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT] =
                waitUsage > 100 ? 100 : waitUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE] =
                idleUsage > 100 ? 100 : idleUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL] =
                utilUsage > 100 ? 100 : utilUsage;

            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] =
                userUsageFloat > 10000 ? 10000 : userUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] =
                kernelUsageFloat > 10000 ? 10000 : kernelUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] =
                waitUsageFloat > 10000 ? 10000 : waitUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] =
                idleUsageFloat > 10000 ? 10000 : idleUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] =
                utilUsageFloat > 10000 ? 10000 : utilUsageFloat;
        }
    }

    psMonCpuInfo.sampletime = curTime;
    psMonCpuInfo.total = curtotal;
    psMonCpuInfo.state[NAS_CPU_USER] = state[CPU_USER];
    psMonCpuInfo.state[NAS_CPU_KERNEL] = state[CPU_KERNEL];
    psMonCpuInfo.state[NAS_CPU_WAIT] = state[CPU_WAIT];
    psMonCpuInfo.state[NAS_CPU_IDLE] = state[CPU_IDLE];
    psMonCpuInfo.state[NAS_CPU_UTIL] = state[CPU_USER]+state[CPU_KERNEL];
    getCurrentTime(tbuf);
    DEBUGMSGTL(("ProcMon/setCpuAverage",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
            psMonCpuInfo.stateUsage[NAS_CPU_USER],
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL]));
    DEBUGMSGTL(("ProcMon/setCpuAverageFloat",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL]));
}
/** added by D.J Park in 2005.02.23 [end]*/

#elif defined(hpux11)
ps_cache_info*
ProcMon_GetProcessList(int* ret) 
{
	ps_cache_info		*pstPsHead = NULL;
	ps_cache_info		*pstPsNew = NULL;
	ps_cache_info		*pstPsLast = NULL;
	
	int iIdx=0;
    int iPid=-1;
    int iNumProc = 0;
    struct pst_dynamic stPstDyn;
    struct pst_status *pstProcTable=NULL;
	char buf[PROC_ARG_LEN];
	char *strArgs;

	*ret = 1;
	
    DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
        "Thr #%d# called for HPUX\n",
        MY_THR_ID));

    pstat_getdynamic(&stPstDyn, sizeof(struct pst_dynamic), 1, 0);
    iNumProc = stPstDyn.psd_activeprocs;
    iNumProc = iNumProc + 10;

    if ((pstProcTable =
        (struct pst_status *) calloc(iNumProc, sizeof(struct pst_status))) == NULL)
    {
        DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
            "Thr #%d# calloc failed\n",
            MY_THR_ID));
        return pstPsHead;
    }

    if (pstat_getproc (pstProcTable, sizeof (struct pst_status), iNumProc, 0) != -1)
    {
        for (iIdx=0;iIdx < iNumProc;iIdx++)
        {

            if(pstProcTable[iIdx].pst_cmd[0] == 0)
                break;
        	strArgs = pstProcTable[iIdx].pst_cmd;
        	
        	strArgs = strchr(strArgs, ' ');
	        		
    		if (strArgs != NULL) {
    			if (strcmp(pstProcTable[iIdx].pst_ucomm, "oracle") == 0) {
					strArgs[0] = '\0';       	
	       			strArgs = pstProcTable[iIdx].pst_cmd;
				} else {
					strArgs++;
				}				
			} else {
				if (strcmp(pstProcTable[iIdx].pst_ucomm, "oracle") == 0) {
					strcpy(buf, pstProcTable[iIdx].pst_cmd);
				} else {
					strcpy(buf, "");					
				}
				
				strArgs = buf;
			}
			
			if (strArgs[0] == ' ')
        		strArgs++;
        	
        	pstPsNew = (ps_cache_info*)malloc(sizeof(ps_cache_info));
        	if (pstPsNew == NULL) {
	    		snmp_log(LOG_ERR, "ProcMon_GetProcessList ps_cache_info malloc failed");
	    		continue;
	    	}
			pstPsNew->pid = pstProcTable[iIdx].pst_pid;
			strcpy(pstPsNew->name, pstProcTable[iIdx].pst_ucomm);
			strcpy(pstPsNew->args, strArgs);

            /** added by D.J Park in 2004.10.14 */
		    //pstPsNew->time = getcstime();
		    //pstPsNew->cputime =  pstProcTable[iIdx].pst_cptickstotal;
		    pstPsNew->time = 0;
		    pstPsNew->cputime =  0;
		    pstPsNew->mem = pstProcTable[iIdx].pst_rssize;
            pstPsNew->hcpu = (long)(pstProcTable[iIdx].pst_pctcpu * 10000);
            pstPsNew->cpu = pstPsNew->hcpu / 100;
            /*
            snmp_log(LOG_INFO,"PID[%d] NAME[%s] CPUT[%ld] CPU[%ld] " 
                    " MEM[%ld]\n",pstPsNew->pid,pstPsNew->name 
                    ,pstPsNew->cputime,pstPsNew->cpu,pstPsNew->mem);
            */
			      
	        pstPsNew->next = NULL;
	        
	        if (pstPsHead != NULL)
	        	pstPsLast->next = pstPsNew;
	        	
	        if (pstPsHead == NULL)
	        	pstPsHead = pstPsNew;
	        	
	        pstPsLast = pstPsNew;
	        
        } // End of for
    } // End of If

    free(pstProcTable);

	*ret = 1;

	return pstPsHead;
}

/** added by D.J Park in 2005.02.23 [start]
 * cpu 정보를 주기적으로 구한다.
 */

void setCpuAverage()
{
    int    i = 0;
    int    n = 0;
    long   ncpu = 0;
	long   curtotal;
	long   pretotal;
	long   total;
	
	long   userUsage;
	long   kernelUsage;
	long   waitUsage;
	long   idleUsage;
	long   utilUsage;

	long   userUsageFloat;
	long   kernelUsageFloat;
	long   waitUsageFloat;
	long   idleUsageFloat;
	long   utilUsageFloat;

	long   curTime;
    long   state[PST_MAX_CPUSTATES] = { 0 };
    struct pst_dynamic psd;
    struct pst_processor* psp=NULL;
    char                    tbuf[30] ={0};

    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) <0) {
        fprintf(stderr, "pstat_getdynamic failed. errno=%d\n", errno);
        return ;
    }
    ncpu = psd.psd_proc_cnt;
    psMonCpuInfo.ncpus = ncpu;

    psp = (struct pst_processor *) calloc (ncpu,sizeof(struct pst_processor));
    if (!psp)
    {
        fprintf(stderr,"pst_processor calloc failed \n");
        return ;
    }
    /* collect final system stats */
    time(&curTime);
    if (pstat_getprocessor(psp, sizeof(*psp), ncpu, 0) < 0)
    {
        fprintf(stderr,"pstat_getprocessor failed \n");
        if (psp)
            free (psp);
        return ;
    }

    if (psp)
        free (psp);

    for(i=0;i<PST_MAX_CPUSTATES;i++) {
        state[i] = 0;
    }
    for (n=0;n<ncpu;n++)
      for(i=0;i<PST_MAX_CPUSTATES;i++)
          state[i] += psp[n].psp_cpu_time[i];

    curtotal =0;
    for (i=0; i<PST_MAX_CPUSTATES; i++) {
        curtotal += state[i];
    }

    if(psMonCpuInfo.sampletime < 1)
    {
		psMonCpuInfo.stateUsage[NAS_CPU_USER] = 0;
 		psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] = 0;
 		psMonCpuInfo.stateUsage[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_UTIL] = 0;

		psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] = 0;
 		psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] = 0;
 		psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] = 0;
    }
    else
    {
        pretotal = psMonCpuInfo.total;
        total = curtotal - pretotal;
        if(total > 0)
        {
            userUsageFloat =
                (long) ((state[0]-psMonCpuInfo.state[NAS_CPU_USER]) 
                        * 10000.0 /(float)total);
            kernelUsageFloat = 
                (long)((state[2]-psMonCpuInfo.state[NAS_CPU_KERNEL]) 
                       * 10000.0 /(float)total);
            waitUsageFloat = 
                (long)((state[4]-psMonCpuInfo.state[NAS_CPU_WAIT]) 
                       * 10000.0 /(float)total);
            idleUsageFloat = 
                (long)((state[3]-psMonCpuInfo.state[NAS_CPU_IDLE]) 
                       * 10000.0 /(float)total);
            utilUsageFloat = 
                (long)((state[0]+state[2]-psMonCpuInfo.state[NAS_CPU_UTIL])
                       * 10000.0 /(float)total);

            userUsage = (long)(userUsageFloat / 100.0);
            kernelUsage = (long)(kernelUsageFloat / 100.0);
            waitUsage = (long)(waitUsageFloat / 100.0);
            idleUsage = (long)(idleUsageFloat / 100.0);
            utilUsage = (long)(utilUsageFloat / 100.0);

            psMonCpuInfo.stateUsage[NAS_CPU_USER] =
                userUsage > 100 ? 100 : userUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] =
                kernelUsage > 100 ? 100 : kernelUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT] =
                waitUsage > 100 ? 100 : waitUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE] =
                idleUsage > 100 ? 100 : idleUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL] =
                utilUsage > 100 ? 100 : utilUsage;

            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] =
                userUsageFloat > 10000 ? 10000 : userUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] =
                kernelUsageFloat > 10000 ? 10000 : kernelUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] =
                waitUsageFloat > 10000 ? 10000 : waitUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] =
                idleUsageFloat > 10000 ? 10000 : idleUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] =
                utilUsageFloat > 10000 ? 10000 : utilUsageFloat;
        }
    }

    psMonCpuInfo.sampletime = curTime;
    psMonCpuInfo.total = curtotal;
    psMonCpuInfo.state[NAS_CPU_USER] = state[0];
    psMonCpuInfo.state[NAS_CPU_KERNEL] = state[2];
    psMonCpuInfo.state[NAS_CPU_WAIT] = state[4];
    psMonCpuInfo.state[NAS_CPU_IDLE] = state[3];
    psMonCpuInfo.state[NAS_CPU_UTIL] = state[0]+state[2];
    getCurrentTime(tbuf);
    DEBUGMSGTL(("ProcMon/setCpuAverage",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
            psMonCpuInfo.stateUsage[NAS_CPU_USER],
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL]));
    DEBUGMSGTL(("ProcMon/setCpuAverageFloat",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL]));

}
/** added by D.J Park in 2005.02.23 [end]*/

#elif defined(linux)
void getPsStat(char *fname, ps_cache_info *pci)
{
    FILE *fp = NULL;
    char buf[256];
    int i;
    char *cp;
    pci->cputime = 0;
    pci->mem = 0;

    if((fp = fopen(fname,"r")) == NULL)
    {
        return;
    }
    fgets(buf, sizeof(buf), fp);
    cp = buf;

    for(i=0 ; i< 23; ++i)
    {
        while(*cp != ' ')
            ++cp;
        ++cp;
        if(i == 12 || i == 13)
            pci->cputime+=atoi(cp);
    }
    pci->mem = atoi(cp) * (getpagesize() / 1024);
    fclose(fp);
}

int getProcessorNum()
{
    FILE    *fpStat = NULL;
    char    szBuff[80];
    int     ncpu = 0;

    fpStat = fopen("/proc/stat", "r");
    if(fpStat != NULL)
    {
        memset(szBuff, '\0', 80);
        while(fgets(szBuff, 80, fpStat) != NULL)
        {
            if(!strncmp(szBuff, "cpu ", 4))
                continue;
            if(!strncmp(szBuff, "cpu", 3))
                ncpu++;
        }
    }
    if(fpStat) 
        fclose(fpStat);

    DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
        "Processor count : %d\n", ncpu));
    
    return ncpu;
}

ps_cache_info*
ProcMon_GetProcessList(int* ret) 
{
	ps_cache_info		*pstPsHead = NULL;
	ps_cache_info		*pstPsNew = NULL;
	ps_cache_info		*pstPsLast = NULL;
	
	FILE            *fpProc = NULL;
    DIR             *pdProcDir = NULL;
    struct dirent   *pstProcEntry = NULL;
    char            *pcPos;
    int    	     	iPid;
    char            szString[256];
    char            szBuff[256];
    char			*strArgs;
    char	   		*cp;

	*ret = 0;

    DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
        "Thr #%d# called\n",
        MY_THR_ID));

    if( (pdProcDir = opendir("/proc") ) == NULL)
    {
        snmp_log(LOG_INFO, "/proc open error. Returning from ProcMon_GetProcessList()\n");
 	    return pstPsHead;
    }

    DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
        "Thr #%d# File </proc> opened\n",
        MY_THR_ID));

    while ((pstProcEntry = readdir(pdProcDir)) != NULL)
    {
        /** added by HJ.Kwon. in 2005.07.06 */
        if ((!strcmp (pstProcEntry->d_name, ".")) ||
            (!strcmp (pstProcEntry->d_name, "..")))
        {
            continue;
        }
        iPid = atoi(pstProcEntry->d_name);
    	if(iPid)
    	{
            sprintf(szString, "/proc/%d/status", iPid);

	        if(fpProc != NULL)
                FCLOSE(fpProc);

            if ((fpProc = fopen(szString, "r")) == NULL)
    		    continue;

            DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                "Thr #%d# File <%s> opened\n",
                MY_THR_ID,
                szString));

            fgets(szBuff, sizeof(szBuff), fpProc);   /* Name: Process name */
  	        pcPos = (char *)strstr(szBuff, "Name:");

       	    if(pcPos)
       	    {
        	    sscanf(pcPos,"Name : %s ", szString);
       	    }
       	    else
       	    {
    		    if(fpProc != NULL)
    	            FCLOSE(fpProc);

                DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    szString));

        	    continue;
    	    }
    	    
    	    if (fpProc != NULL)
    	    	FCLOSE(fpProc);
    	    
    	    pstPsNew = (ps_cache_info*)malloc(sizeof(ps_cache_info));
    	    if (pstPsNew == NULL) {
	    		snmp_log(LOG_ERR, "ProcMon_GetProcessList ps_cache_info malloc failed");
	    		continue;
	    	}
			pstPsNew->pid = iPid;
			strcpy(pstPsNew->name, szString);
			
			/* Get process command parameters */
    	    memset(szString, 0, sizeof(szString));
	     	    	
 	    	sprintf(szString, "/proc/%d/cmdline", iPid);
			if ((fpProc = fopen(szString, "r")) == NULL)
			    continue;
			    
			DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                "Thr #%d# File <%s> opened\n",
                MY_THR_ID,
                szString));
            
            /* Get process command parameters */
			memset(szBuff, 0, sizeof(szBuff));
		
			/*
			 * argv[0] '\0' argv[1] '\0' ....
			 */
			if (!fgets(szBuff, sizeof(szBuff) - 2, fpProc)) {
			    /*
			     * maybe be empty (even argv[0] is missing)
			     */
			    szString[0] = '\0';				    				    
			}
			
			if(fpProc != NULL)
            {
                FCLOSE(fpProc);
	        
                DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                            "Thr #%d# File <%s> closed\n",
                            MY_THR_ID,
                            szString));
            }
            
            if (strcmp(pstPsNew->name, "oracle") == 0) {
            	strcpy(szString, szBuff);
            } else {
				/*
				 * Skip over argv[0]
				 */
				if (szString != NULL && strlen(szString) > 0)
				{
					cp = szBuff;
					while (*cp)
					    ++cp;
					++cp;
					/*
					 * Now join together separate arguments.
					 */
					while (1) {
					    while (*cp)
							++cp;
					    if (*(cp + 1) == '\0')
							break;		/* '\0''\0' => End of command line */
					    *cp = ' ';
					}
				
					cp = szBuff;
					while (*cp)
					    ++cp;
					++cp;
					strcpy(szString, cp);					
				}
			}

            strArgs = szString;
            
            if (strArgs[0] == ' ')
            	strArgs++;
	        		
			strcpy(pstPsNew->args, strArgs);

            /** added by D.J Park in 2004.10.14 */
 	    	sprintf(szString, "/proc/%d/stat", iPid);
		    pstPsNew->time = getcstime();
            getPsStat(szString,pstPsNew);
            pstPsNew->hcpu = getPercentCPU(pstPsNew->pid,pstPsNew->time,
                pstPsNew->cputime,cpuNo);
            pstPsNew->cpu = pstPsNew->hcpu / 100;
            /*
            snmp_log(LOG_INFO,"PID[%d] NAME[%s] CPUT[%ld] CPU[%ld] " 
                    " MEM[%ld]\n",pstPsNew->pid,pstPsNew->name 
                    ,pstPsNew->cputime,pstPsNew->cpu,pstPsNew->mem);
            */
			      
	        pstPsNew->next = NULL;
	        
	        if (pstPsHead != NULL)
	        	pstPsLast->next = pstPsNew;
	        	
	        if (pstPsHead == NULL)
	        	pstPsHead = pstPsNew;
	        	
	        pstPsLast = pstPsNew;
  	    }
        else
        {
            DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                        "Thr #%d# iPid = 0, Skipped\n",
                        MY_THR_ID));

            continue; 
        }
    }

    if(fpProc != NULL)
    {
        FCLOSE(fpProc);

        DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    szString));
    }

    if(pdProcDir != NULL)
    {
        closedir(pdProcDir);

        DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                    "Thr #%d# File </proc> closed\n",
                    MY_THR_ID));
    }

	*ret = 1;

	return pstPsHead;
}

/** added by D.J Park in 2005.02.23 [start]
 * cpu 정보를 주기적으로 구한다.
 */

#define STAT "/proc/stat"
void setCpuAverage()
{
    int    tmpIdx;
    long   ncpu = 0;
	long long int curtotal;
	long long int pretotal;
	long long int total;
	long long int userUsage;
	long long int kernelUsage;
	long long int waitUsage;
	long long int idleUsage;
	long long int utilUsage;

	long   userUsageFloat;
	long   kernelUsageFloat;
	long   waitUsageFloat;
	long   idleUsageFloat;
	long   utilUsageFloat;

	long   curTime;
    long long int state[4] = { 0 };
    long long int tmp_state[4] = { 0 };
    FILE   *fpStat = NULL;
	char 	szBuff[80];
    char                    tbuf[30] ={0};

    time(&curTime);
	fpStat = fopen(STAT, "r");
	if(fpStat != NULL)
    {
        memset(szBuff, '\0', 80);

        while(fgets(szBuff, 80, fpStat) != NULL)
        {
		    if(!strncmp(szBuff, "cpu ", 4))
			    continue;
		    if(!strncmp(szBuff, "cpu", 3))
            {
			    sscanf(szBuff, "cpu%d %Ld %Ld %Ld %Ld ",
			        &tmpIdx, 
			        &tmp_state[NAS_CPU_USER], 
			        &tmp_state[NAS_CPU_WAIT], 
			        &tmp_state[NAS_CPU_KERNEL], 
			        &tmp_state[NAS_CPU_IDLE]);
			    ncpu++;
                state[NAS_CPU_USER]+=tmp_state[NAS_CPU_USER];
                state[NAS_CPU_KERNEL]+=tmp_state[NAS_CPU_KERNEL];
                state[NAS_CPU_WAIT]+=tmp_state[NAS_CPU_WAIT];
                state[NAS_CPU_IDLE]+=tmp_state[NAS_CPU_IDLE];
                memset(szBuff, '\0', 80);
		    }
	    }
        if(fpStat)
	        fclose(fpStat); 
    }
    psMonCpuInfo.ncpus = ncpu;
    
    curtotal = state[NAS_CPU_USER] + state[NAS_CPU_KERNEL] + state[NAS_CPU_WAIT]
        + state[NAS_CPU_IDLE];
    if(psMonCpuInfo.sampletime < 1)
    {
		psMonCpuInfo.stateUsage[NAS_CPU_USER] = 0;
 		psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] = 0;
 		psMonCpuInfo.stateUsage[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_UTIL] = 0;

		psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] = 0;
 		psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] = 0;
 		psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] = 0;
    }
    else
    {
        pretotal = psMonCpuInfo.total;
        total = curtotal - pretotal;
        if(total > 0)
        {
            userUsageFloat =(long) ((state[NAS_CPU_USER]-psMonCpuInfo.state[NAS_CPU_USER]) * 10000.0 /(float)total);
            kernelUsageFloat = (long)((state[NAS_CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_KERNEL]) * 10000.0 /(float)total);
            waitUsageFloat = (long)((state[NAS_CPU_WAIT]-psMonCpuInfo.state[NAS_CPU_WAIT]) * 10000.0 /(float)total);
            idleUsageFloat = (long)((state[NAS_CPU_IDLE]-psMonCpuInfo.state[NAS_CPU_IDLE]) * 10000.0 /(float)total);
            utilUsageFloat = (long)((state[NAS_CPU_USER]+state[NAS_CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_UTIL])* 10000.0 /(float)total);


            userUsage = (long)(userUsageFloat/100.0);
            kernelUsage = (long)(kernelUsageFloat/100.0);
            waitUsage = (long)(waitUsageFloat/100.0);
            idleUsage = (long)(idleUsageFloat/100.0);
            utilUsage = (long)(utilUsageFloat/100.0);

            psMonCpuInfo.stateUsage[NAS_CPU_USER] =
                userUsage > 100 ? 100 : userUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] =
                kernelUsage > 100 ? 100 : kernelUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT] =
                waitUsage > 100 ? 100 : waitUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE] =
                idleUsage > 100 ? 100 : idleUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL] =
                utilUsage > 100 ? 100 : utilUsage;

            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] =
                userUsageFloat > 10000 ? 10000 : userUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] =
                kernelUsageFloat > 10000 ? 10000 : kernelUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] =
                waitUsageFloat > 10000 ? 10000 : waitUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] =
                idleUsageFloat > 10000 ? 10000 : idleUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] =
                utilUsageFloat > 10000 ? 10000 : utilUsageFloat;
        }
    }

    psMonCpuInfo.sampletime = curTime;
    psMonCpuInfo.total = curtotal;
    psMonCpuInfo.state[NAS_CPU_USER] = state[NAS_CPU_USER];
    psMonCpuInfo.state[NAS_CPU_KERNEL] = state[NAS_CPU_KERNEL];
    psMonCpuInfo.state[NAS_CPU_WAIT] = state[NAS_CPU_WAIT];
    psMonCpuInfo.state[NAS_CPU_IDLE] = state[NAS_CPU_IDLE];
    psMonCpuInfo.state[NAS_CPU_UTIL] = state[NAS_CPU_USER]+state[NAS_CPU_KERNEL];
    getCurrentTime(tbuf);
    DEBUGMSGTL(("ProcMon/setCpuAverage",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
            psMonCpuInfo.stateUsage[NAS_CPU_USER],
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL]));
    DEBUGMSGTL(("ProcMon/setCpuAverageFloat",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL]));
}
/** added by D.J Park in 2005.02.23 [end]*/

#elif defined(sysv4)
ps_cache_info*
ProcMon_GetProcessList(int* ret) 
{
	ps_cache_info		*pstPsHead = NULL;
	ps_cache_info		*pstPsNew = NULL;
	ps_cache_info		*pstPsLast = NULL;
	
	DIR                 *dirstream = {0};
    struct dirent       *direntry = NULL;
    struct psinfo       pInfo = {0};
    char                procBuff[MAXPATHLEN+1]="";
    int                 procFD = 0 ;
    int                 nread = 0;
    int                 psInfoSize =sizeof(struct psinfo);

	char				buf[PROC_ARG_LEN];
	char				*strArgs;

	*ret = 0;
	
    DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
        "Thr #%d# called for sysv4\n",
        MY_THR_ID));

    // Open proc system
    dirstream = opendir("/proc");

    if ( dirstream )
        DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
            "Thr #%d# File </proc> opened\n",
            MY_THR_ID));

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

        DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            procBuff));

        nread = read(procFD,&pInfo,psInfoSize);

        if(nread!=psInfoSize)
        {
            close(procFD);

            DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                "Thr #%d# File <%s> closed\n",
                MY_THR_ID,
                procBuff));

            continue;
        }

        DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
            "Thr #%d# File <%s> closed\n",
            MY_THR_ID,
            procBuff));

        close(procFD);
		
		pstPsNew = (ps_cache_info*)malloc(sizeof(ps_cache_info));
		if (pstPsNew == NULL) {
    		snmp_log(LOG_ERR, "ProcMon_GetProcessList ps_cache_info malloc failed");
    		continue;
    	}
		pstPsNew->pid = pInfo.pr_pid;
		strcpy(pstPsNew->name, pInfo.pr_fname);
		
		strArgs = pInfo.pr_psargs;
		
		if (strArgs[0] == ' ')
        	strArgs++;
        
		strArgs = strchr(strArgs, ' ');
       	
		if (strArgs != NULL){
			if (strcmp(pstPsNew->name, "oracle") == 0) {
			 	strArgs[0] = '\0';			 	
			 	strArgs = pInfo.pr_psargs;
			} else {
				strArgs++;
			}
		} else {
			if (strcmp(pstPsNew->name, "oracle") == 0) {
				strcpy(buf, pInfo.pr_psargs);
			} else {
				strcpy(buf, "");
			}
			
			strArgs = buf;        			
		}
		
		if (strArgs[0] == ' ')
        	strArgs++;
		
		strcpy(pstPsNew->args, strArgs);

        /** added by D.J Park in 2004.10.14 */
		pstPsNew->time = getcstime();
		pstPsNew->cputime = pInfo.pr_time.tv_sec * 100 + 
            pInfo.pr_time.tv_nsec / 10000000;
		pstPsNew->mem = pInfo.pr_rssize;
        pstPsNew->hcpu = getPercentCPU(pstPsNew->pid,pstPsNew->time,
                pstPsNew->cputime,cpuNo);
        pstPsNew->cpu = pstPsNew->hcpu / 100;
        /*
        snmp_log(LOG_INFO,"PID[%d] NAME[%s] CPUT[%ld] CPU[%ld] " 
                " MEM[%ld]\n",pstPsNew->pid,pstPsNew->name 
                ,pstPsNew->cputime,pstPsNew->cpu,pstPsNew->mem);
        */
		
        pstPsNew->next = NULL;
        
        if (pstPsHead != NULL)
        	pstPsLast->next = pstPsNew;
        	
        if (pstPsHead == NULL)
        	pstPsHead = pstPsNew;
        	
        pstPsLast = pstPsNew;        
	}

    DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
        "Thr #%d# File </proc> closed\n",
        MY_THR_ID));

    closedir(dirstream);

	*ret = 1;
    
	return pstPsHead;
}

/** added by D.J Park in 2005.02.23 [start]
 * cpu 정보를 주기적으로 구한다.
 */

void setCpuAverage()
{
    int    tmpIdx;
    long   ncpu = 0;
	long   curtotal=0;
	long   pretotal=0;
	long   total=0;

	long   userUsage=0;
	long   kernelUsage=0;
	long   waitUsage=0;
	long   idleUsage=0;
	long   utilUsage=0;

	long   userUsageFloat=0;
	long   kernelUsageFloat=0;
	long   waitUsageFloat=0;
	long   idleUsageFloat=0;
	long   utilUsageFloat=0;

	long   curTime=0;
    long   state[4] = { 0 };
    int    fd;
	int    i;
    struct mioc_rksym pm_rks;
    struct plocalmet pm_met;
    struct mets pm_mets;
    unsigned long pm_metpPtr;
    struct met_localdata_ptrs pm_metpPtrData;
    char                    tbuf[30] ={0};


    fd = open("/dev/kmem",O_RDONLY);
    if(fd < 0)
    {
        snmp_log(LOG_ERR,"open file[/dev/kmem] failed\n");
        return ;
    }
    pm_rks.mirk_symname=METPTR_SYMBOL;
    pm_rks.mirk_buf=&pm_metpPtr;
    pm_rks.mirk_buflen = sizeof(pm_metpPtr);

    if((ioctl(fd, MIOC_READKSYM, &pm_rks))<0)
    {
        snmp_log(LOG_INFO,"ioctl error: \n");
        return;
    }
    lseek(fd,pm_metpPtr,SEEK_SET);
    read(fd,&pm_metpPtrData,sizeof(pm_metpPtrData));

    lseek(fd,pm_metpPtr,SEEK_SET);
    read(fd,&pm_metpPtrData,sizeof(pm_metpPtrData)+((pm_metpPtrData.num_eng-1)*sizeof(vaddr_t)));

    psMonCpuInfo.ncpus = pm_metpPtrData.num_eng;
    time(&curTime);
    for(i =0 ; i < pm_metpPtrData.num_eng ; i++)
    {
        lseek(fd,pm_metpPtrData.localdata_p[i],SEEK_SET);
        read(fd,&pm_met,sizeof(pm_met));
        state[NAS_CPU_USER] += pm_met.metp_cpu.mpc_cpu[MET_CPU_USER];
        state[NAS_CPU_KERNEL] += pm_met.metp_cpu.mpc_cpu[MET_CPU_SYS];
        state[NAS_CPU_WAIT] += pm_met.metp_cpu.mpc_cpu[MET_CPU_WAIT];
        state[NAS_CPU_IDLE] += pm_met.metp_cpu.mpc_cpu[MET_CPU_IDLE];

       /* modified by HJ.Kwon. 2005.06.03
        * because UnixWare 7.0.1 doesn't have MET_CPU_INTR
        * in /usr/include/sys/metrics.h file
        */
#ifdef MET_CPU_INTR
        curtotal+= (pm_met.metp_cpu.mpc_cpu[MET_CPU_USER] + pm_met.metp_cpu.mpc_cpu[MET_CPU_SYS] + pm_met.metp_cpu.mpc_cpu[MET_CPU_WAIT] + pm_met.metp_cpu.mpc_cpu[MET_CPU_IDLE] + pm_met.metp_cpu.mpc_cpu[MET_CPU_INTR]);
#else
        curtotal+= (pm_met.metp_cpu.mpc_cpu[MET_CPU_USER] + pm_met.metp_cpu.mpc_cpu[MET_CPU_SYS] + pm_met.metp_cpu.mpc_cpu[MET_CPU_WAIT] + pm_met.metp_cpu.mpc_cpu[MET_CPU_IDLE]);
#endif
    }
	/* added by HJ.Kwon. 2005.07.07. */
    close(fd);
    
    if(psMonCpuInfo.sampletime < 1)
    {
		psMonCpuInfo.stateUsage[NAS_CPU_USER] = 0;
 		psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] = 0;
 		psMonCpuInfo.stateUsage[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_UTIL] = 0;

		psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] = 0;
 		psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] = 0;
 		psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] = 0;
    }
    else
    {
        pretotal = psMonCpuInfo.total;
        total = curtotal - pretotal;
        if(total > 0)
        {
            userUsageFloat =(long) ((state[NAS_CPU_USER]-psMonCpuInfo.state[NAS_CPU_USER]) * 10000.0 /(float)total);
            kernelUsageFloat = (long)((state[NAS_CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_KERNEL]) * 10000.0 /(float)total);
            waitUsageFloat = (long)((state[NAS_CPU_WAIT]-psMonCpuInfo.state[NAS_CPU_WAIT]) * 10000.0 /(float)total);
            idleUsageFloat = (long)((state[NAS_CPU_IDLE]-psMonCpuInfo.state[NAS_CPU_IDLE]) * 10000.0 /(float)total);
            utilUsageFloat = (long)((state[NAS_CPU_USER]+state[NAS_CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_UTIL])* 10000.0 /(float)total);

            userUsage = (long)(userUsageFloat/100.0);
            kernelUsage = (long)(kernelUsageFloat/100.0);
            waitUsage = (long)(waitUsageFloat/100.0);
            idleUsage = (long)(idleUsageFloat/100.0);
            utilUsage = (long)(utilUsageFloat/100.0);

            psMonCpuInfo.stateUsage[NAS_CPU_USER] =
                userUsage > 100 ? 100 : userUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] =
                kernelUsage > 100 ? 100 : kernelUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT] =
                waitUsage > 100 ? 100 : waitUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE] =
                idleUsage > 100 ? 100 : idleUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL] =
                utilUsage > 100 ? 100 : utilUsage;

            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] =
                userUsageFloat > 10000 ? 10000 : userUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] =
                kernelUsageFloat > 10000 ? 10000 : kernelUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] =
                waitUsageFloat > 10000 ? 10000 : waitUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] =
                idleUsageFloat > 10000 ? 10000 : idleUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] =
                utilUsageFloat > 10000 ? 10000 : utilUsageFloat;
        }
    }

    psMonCpuInfo.sampletime = curTime;
    psMonCpuInfo.total = curtotal;
    psMonCpuInfo.state[NAS_CPU_USER] = state[NAS_CPU_USER];
    psMonCpuInfo.state[NAS_CPU_KERNEL] = state[NAS_CPU_KERNEL];
    psMonCpuInfo.state[NAS_CPU_WAIT] = state[NAS_CPU_WAIT];
    psMonCpuInfo.state[NAS_CPU_IDLE] = state[NAS_CPU_IDLE];
    psMonCpuInfo.state[NAS_CPU_UTIL] = state[NAS_CPU_USER]+state[NAS_CPU_KERNEL];
    getCurrentTime(tbuf);
    DEBUGMSGTL(("ProcMon/setCpuAverage",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
            psMonCpuInfo.stateUsage[NAS_CPU_USER],
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL]));
    DEBUGMSGTL(("ProcMon/setCpuAverageFloat",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL]));
}
/** added by HJ.Kwon. 2005.07.12 */
int getProcessorNum()
{
    int fd;
    struct mioc_rksym pm_rks;
    struct met_localdata_ptrs pm_metpPtrData;
    unsigned long pm_metpPtr;

    fd = open("/dev/kmem",O_RDONLY);
    if(fd < 0)
    {
        return 1;
    }
    pm_rks.mirk_symname=METPTR_SYMBOL;
    pm_rks.mirk_buf=&pm_metpPtr;
    pm_rks.mirk_buflen = sizeof(pm_metpPtr);

    if((ioctl(fd, MIOC_READKSYM, &pm_rks))<0)
    {
        return 1;
    }
    lseek(fd,pm_metpPtr,SEEK_SET);
    read(fd,&pm_metpPtrData,sizeof(pm_metpPtrData));

    if (pm_metpPtrData.num_eng < 1)
        return 1;
    return pm_metpPtrData.num_eng;
}

/** added by D.J Park in 2005.02.23 [end]*/
#elif defined(osf5)
ps_cache_info*
ProcMon_GetProcessList(int* ret) 
{
    ps_cache_info		*pstPsHead = NULL;
    ps_cache_info		*pstPsNew = NULL;
    ps_cache_info		*pstPsLast = NULL;
	
    /** added by HJ.Kwon. in 2005.07.05. */ 
    struct user uarea;

    struct tbl_procinfo pinfo;
    DIR                 *dirstream = {0};
    struct dirent       *direntry = NULL;
    int                 nread = 0;
    int                 pInfoSize = sizeof(struct tbl_procinfo);

    char		buf[PROC_ARG_LEN];
    char		*strArgs;
	
    char 		argBuff[PROC_ARG_LEN];
    int 		argLen = 0;
    int			argTempLen = 0;
    int 		i = 0;
    int			bufIdx = 0;

	*ret = 0;
	
    DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
        "Thr #%d# called for tru64\n",
        MY_THR_ID));

    // Open proc system
    dirstream = opendir("/proc");

    if ( dirstream ) {
        DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
            "Thr #%d# File </proc> opened\n",
            MY_THR_ID));
    } else {
        DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
            "Thr #%d# File </proc> Failed to open\n",
            MY_THR_ID));
	return -1;
    }

    // Loop until we find the process
    while((direntry = readdir(dirstream))!= '\0')
    {
        if(!strcmp(direntry->d_name,".") || !strcmp(direntry->d_name,"..") ||
                        !strcmp(direntry->d_name,"0"))
        {
            continue;
        }
        if(table(TBL_PROCINFO,atoi(direntry->d_name),&pinfo,1,pInfoSize)>0)
        {
            DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
                        "Thr #%d# table call  Succeeded\n",
            		MY_THR_ID));
        } 
        else
        {
            DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
            		"Thr #%d# table call  Failed\n",
            		MY_THR_ID));
            continue;
        }
        
        bufIdx = 0;
        argLen = table(TBL_ARGUMENTS,pinfo.pi_pid,&argBuff,1,256); 
        
        if(argLen>=0) 
        {
            strcpy(buf, argBuff);
            argTempLen = strlen(argBuff);
			
            if(argLen>0 && strcmp(pinfo.pi_comm, "oracle") != 0)
            {
                for(i=argTempLen+1;i<256;i++)
                {
                    if(argBuff[i]=='\0' && argBuff[i+1]=='\0') 
                    {
                        break;
                    }
                    if(argBuff[i]=='\0')
                    {
                        buf[bufIdx]=' ';
                    } 
                    else 
                    {
                        buf[bufIdx]=argBuff[i];
                    }
                    bufIdx++;
                }
                buf[bufIdx+1] ='\0';	    		
            }			
            strArgs = buf;			

        } 
        else 
        {
            strcpy(buf, "");
            strArgs = buf;
        }

        if (strArgs[0] == ' ')
            strArgs++;

        pstPsNew = (ps_cache_info*)malloc(sizeof(ps_cache_info));
        if (pstPsNew == NULL) {
            snmp_log(LOG_ERR, "ProcMon_GetProcessList ps_cache_info malloc failed");
            continue;
    	}
	pstPsNew->pid = atoi(direntry->d_name);
	strcpy(pstPsNew->name, pinfo.pi_comm);
	strcpy(pstPsNew->args, strArgs);


	/** added by HJ.Kwon. in 2005.07.05. start 
         * it's for processes's memory and cpu usage on the system */ 
        if(table(TBL_UAREA,pinfo.pi_pid,&uarea,1,sizeof(uarea))<0)
        {
            DEBUGMSG(("ProcMon/ProcUser",
                    "Error Reading ProcUser from table() call\n"));
	    pstPsNew->time = getcstime();
            pstPsNew->mem = 0;
	    pstPsNew->cputime = 0;
            pstPsNew->hcpu = 0; 
            pstPsNew->cpu = 0;
        }
        else
        {
	    pstPsNew->time = getcstime();
            pstPsNew->mem = uarea.u_ru.ru_maxrss;
	    pstPsNew->cputime = uarea.u_ru.ru_utime.tv_sec*100 +
                                uarea.u_ru.ru_stime.tv_sec*100 +
                                uarea.u_ru.ru_utime.tv_usec/10000 +
                                uarea.u_ru.ru_stime.tv_usec/10000;
            pstPsNew->hcpu = getPercentCPU(pstPsNew->pid,pstPsNew->time,
                    pstPsNew->cputime,cpuNo);
            pstPsNew->cpu = pstPsNew->hcpu / 100;
        }
		      
        pstPsNew->next = NULL;
        
        if (pstPsHead != NULL)
        	pstPsLast->next = pstPsNew;
        	
        if (pstPsHead == NULL)
        	pstPsHead = pstPsNew;
        	
        pstPsLast = pstPsNew;        
    }

    closedir(dirstream);

	*ret = 1;

	return pstPsHead;
}

/** added by HJ.Kwon in 2005.07.08 [start]
 * cpu 정보를 주기적으로 구한다.
 */

void setCpuAverage()
{
    struct  tbl_sysinfo sinfo; 
    int     i;

    long    curtotal=0;
    long    pretotal=0;
    long    total=0;

    long    userUsage=0;
    long    kernelUsage=0;
    long    waitUsage=0;
    long    idleUsage=0;
    long    utilUsage=0;

    long    userUsageFloat=0;
    long    kernelUsageFloat=0;
    long    waitUsageFloat=0;
    long    idleUsageFloat=0;
    long    utilUsageFloat=0;

    long    state[4] = { 0 };
    long    tmp_state = 0;
    long    curTime = 0;

    char    tbuf[30] ={0};

    time(&curTime);

    //Get online processors
    psMonCpuInfo.ncpus = cpuNo;
    for(i=0; i<cpuNo; i++){
        memset(&sinfo,0,sizeof(sinfo));
        table(TBL_SYSINFO_MP,i,&sinfo,1,sizeof(sinfo));

        state[NAS_CPU_USER] += sinfo.si_user;
        state[NAS_CPU_KERNEL] += sinfo.si_sys;
        state[NAS_CPU_WAIT] += sinfo.wait;
        state[NAS_CPU_IDLE] += sinfo.si_idle;
        tmp_state += sinfo.si_nice;
    } 
    curtotal = state[NAS_CPU_USER] + state[NAS_CPU_KERNEL]
              + state[NAS_CPU_WAIT] + state[NAS_CPU_IDLE] + tmp_state;

    if(psMonCpuInfo.sampletime < 1)
    {
        psMonCpuInfo.stateUsage[NAS_CPU_USER] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_UTIL] = 0;

        psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] = 0;
    } 
    else
    {
        pretotal = psMonCpuInfo.total;
        total = curtotal - pretotal;
        if(total > 0)
        {
            userUsageFloat =(long) ((state[NAS_CPU_USER]-psMonCpuInfo.state[NAS_CPU_USER]) * 10000.0 /(float)total);
            kernelUsageFloat = (long)((state[NAS_CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_KERNEL]) * 10000.0 /(float)total);
            waitUsageFloat = (long)((state[NAS_CPU_WAIT]-psMonCpuInfo.state[NAS_CPU_WAIT]) * 10000.0 /(float)total);
            idleUsageFloat = (long)((state[NAS_CPU_IDLE]-psMonCpuInfo.state[NAS_CPU_IDLE]) * 10000.0 /(float)total);
            utilUsageFloat = (long)((state[NAS_CPU_USER]+state[NAS_CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_UTIL]) * 10000.0 /(float)total);

            userUsage =(long) (userUsageFloat / 100.0);
            kernelUsage =(long) (kernelUsageFloat / 100.0);
            waitUsage =(long) (waitUsageFloat / 100.0);
            idleUsage =(long) (idleUsageFloat / 100.0);
            utilUsage =(long) (utilUsageFloat / 100.0);

            psMonCpuInfo.stateUsage[NAS_CPU_USER] = 
                userUsage > 100 ? 100 : userUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] = 
                kernelUsage > 100 ? 100 : kernelUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT] = 
                waitUsage > 100 ? 100 : waitUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE] = 
                idleUsage > 100 ? 100 : idleUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL] = 
                utilUsage > 100 ? 100 : utilUsage;

            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] = 
                userUsageFloat > 10000 ? 10000 : userUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] = 
                kernelUsageFloat > 10000 ? 10000 : kernelUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] = 
                waitUsageFloat > 10000 ? 10000 : waitUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] = 
                idleUsageFloat > 10000 ? 10000 : idleUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] = 
                utilUsageFloat > 10000 ? 10000 : utilUsageFloat;
        }
    }
    psMonCpuInfo.sampletime = curTime;
    psMonCpuInfo.total = curtotal;
    psMonCpuInfo.state[NAS_CPU_USER] = state[NAS_CPU_USER];
    psMonCpuInfo.state[NAS_CPU_KERNEL] = state[NAS_CPU_KERNEL];
    psMonCpuInfo.state[NAS_CPU_WAIT] = state[NAS_CPU_WAIT];
    psMonCpuInfo.state[NAS_CPU_IDLE] = state[NAS_CPU_IDLE];
    psMonCpuInfo.state[NAS_CPU_UTIL] = state[NAS_CPU_USER]+state[NAS_CPU_KERNEL];
    getCurrentTime(tbuf);
    DEBUGMSGTL(("ProcMon/setCpuAverage",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
                psMonCpuInfo.stateUsage[NAS_CPU_USER],
                psMonCpuInfo.stateUsage[NAS_CPU_KERNEL],
                psMonCpuInfo.stateUsage[NAS_CPU_WAIT],
                psMonCpuInfo.stateUsage[NAS_CPU_IDLE],
                psMonCpuInfo.stateUsage[NAS_CPU_UTIL]));
    DEBUGMSGTL(("ProcMon/setCpuAverageFloat",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld]\n", tbuf,
                psMonCpuInfo.stateUsageFloat[NAS_CPU_USER],
                psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL],
                psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT],
                psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE],
                psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL]));

}
/** added by D.J Park in 2005.02.23 [end]*/

#elif defined(WIN32)

void
conversion(char * buff, char *ProcessName);

ps_cache_info*
ProcMon_GetProcessList(int* ret) 
{
    ps_cache_info		*pstPsHead = NULL;
    ps_cache_info		*pstPsNew = NULL;
    ps_cache_info		*pstPsLast = NULL;
	
    char szProcessName[MAX_PATH] = "unknown";
    char szCommandLine[MAX_PATH] = {0,};
    unsigned int i;
    char buff[MAX_PATH];

    // added by HJ.Kwon. in 2005.04.08.
    PROCESS_MEMORY_COUNTERS pmc;
    FILETIME	    ct, et, kt, ut;
	

    // Get a handle to the process.
    DWORD aProcesses[1024], cbNeeded, cProcesses;

	*ret = 0;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return 0;

    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);
    for ( i = 0; i < cProcesses; i++ )
    {
        //HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
        //                               PROCESS_VM_READ,
        //                               FALSE,  aProcesses[i]  );
        HANDLE hProcess = GetProcessHandleWithEnoughRights(aProcesses[i],PROCESS_QUERY_INFORMATION | PROCESS_VM_READ);

        // Get the process name.
        if (NULL != hProcess )
        {
            HMODULE hMod;
            DWORD cbNeeded;

            if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod),
                 &cbNeeded) )
            {
                GetModuleBaseName( hProcess, hMod, szProcessName,
                                   sizeof(szProcessName) );
                GetPsCommandLineEx(hProcess, sizeof(szCommandLine), szCommandLine);
                // added by HJ.Kwon. in 2005.04.08.
                GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc));
                GetProcessTimes(hProcess, &ct, &et, &kt, &ut);

            }

            //CloseHandle( hMod );
        }

//       conversion(buff,szProcessName);
//       strcpy(szProcessName,buff);

//       conversion(buff,process);
//       strcpy(process,buff);

        CloseHandle( hProcess );

        pstPsNew = (ps_cache_info*)malloc(sizeof(ps_cache_info));
        if (pstPsNew == NULL) {
            snmp_log(LOG_ERR, "ProcMon_GetProcessList ps_cache_info malloc failed");
            continue;
    	}
        DEBUGMSGTL(("ProcMon/ProcMon_GetProcessList",
            "GetProcessList %d : %s : %s\n", aProcesses[i], szProcessName, szCommandLine));
        pstPsNew->pid = aProcesses[i];
        strcpy(pstPsNew->name, szProcessName);
        //strcpy(pstPsNew->args, "");
        strcpy(pstPsNew->args, szCommandLine);


        /** added by HJ.Kwon. in 2005.04.08. */
        pstPsNew->mem = pmc.WorkingSetSize/1024;
        pstPsNew->time = getcstime();
        pstPsNew->cputime = (to_msec(&kt) + to_msec(&ut)) / 10;
        pstPsNew->hcpu = getPercentCPU(pstPsNew->pid,pstPsNew->time,
                                       pstPsNew->cputime,cpuNo);
        pstPsNew->cpu = pstPsNew->hcpu / 100;
        /*
        snmp_log(LOG_INFO,"PID[%d] NAME[%s] CPUT[%ld] CPU[%ld] " 
                " MEM[%ld]\n",pstPsNew->pid,pstPsNew->name 
                ,pstPsNew->cputime,pstPsNew->cpu,pstPsNew->mem);
        */
		      
        pstPsNew->next = NULL;
        
        if (pstPsHead != NULL)
            pstPsLast->next = pstPsNew;
        	
        if (pstPsHead == NULL)
            pstPsHead = pstPsNew;
        	
        pstPsLast = pstPsNew;       
    }

	*ret = 1;
    
    return pstPsHead;
}

/** added by D.J Park in 2005.02.23 [start]
 *  modified by HJ.Kwon. in 2005.04.11.
 * cpu 정보를 주기적으로 구한다.
 */

void setCpuAverage()
{
    LONGLONG   curtotal=0;
    LONGLONG   pretotal=0;
    LONGLONG   total=0;
	SYSTEM_INFO coreCount;

    long   userUsage=0;
    long   kernelUsage=0;
    long   waitUsage=0;
    long   idleUsage=0;
    long   utilUsage=0;
	long   cores=1;

    long   userUsageFloat=0;
    long   kernelUsageFloat=0;
    long   waitUsageFloat=0;
    long   idleUsageFloat=0;
    long   utilUsageFloat=0;

    LONGLONG   state[4] = {0};

    LONGLONG   curTime=0;
    int     i = 0;

    char    tbuf[30] ={0};
    char    pathName [MAX_PATH];
    PDH_RAW_COUNTER         PDHRawValue;

    psMonCpuInfo.ncpus = cpuNo;
    time(&curTime);

	GetSystemInfo(&coreCount);
	cores=coreCount.dwNumberOfProcessors;

    strcpy(pathName,"\\Processor(_Total)\\% Privileged Time");
    COMPDH_Collection_RAW(pathName, &PDHRawValue);
    state[NAS_CPU_KERNEL] = PDHRawValue.FirstValue/10000;
	
    /** 2000에서 idle time 성능카운터가 없음으로 
        같은 값인 프로세스(Idle)로 대체.*/
    strcpy(pathName,"\\Processor(_Total)\\% Idle Time");
    COMPDH_Collection_RAW(pathName, &PDHRawValue);
    state[NAS_CPU_IDLE] = PDHRawValue.FirstValue/10000;
    snmp_log(6,"idle=%I64d\n",state[NAS_CPU_IDLE]);
	/*
    strcpy(pathName,"\\Process(Idle)\\% Processor Time");
    COMPDH_Collection_RAW(pathName, &PDHRawValue);
    state[NAS_CPU_IDLE] = PDHRawValue.FirstValue/cores/10000;
	*/
    strcpy(pathName,"\\Processor(_Total)\\% User Time");
    COMPDH_Collection_RAW(pathName, &PDHRawValue);
    state[NAS_CPU_USER] = PDHRawValue.FirstValue/10000;

    strcpy(pathName,"\\Processor(_Total)\\% Interrupt Time");
    COMPDH_Collection_RAW(pathName, &PDHRawValue);
    state[NAS_CPU_WAIT] = PDHRawValue.FirstValue/10000;


    curtotal = state[NAS_CPU_USER] + state[NAS_CPU_KERNEL] 
             + state[NAS_CPU_WAIT] + state[NAS_CPU_IDLE];

    DEBUGMSGTL(("ProcMon/state",
                "[%s] usr[%I64d] sys[%I64d] wait[%I64d] idle[%I64d] curtotal[%I64d]\n", tbuf,
            state[NAS_CPU_USER],
            state[NAS_CPU_KERNEL],
            state[NAS_CPU_WAIT],
            state[NAS_CPU_IDLE],
            curtotal));

    if(psMonCpuInfo.sampletime < 1)
    {
        psMonCpuInfo.stateUsage[NAS_CPU_USER] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsage[NAS_CPU_UTIL] = 0;

        psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] = 0;
        psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] = 0;
    } 
    else
    {
        pretotal = psMonCpuInfo.total;
        total = curtotal - pretotal;
        if(total > 0)
        {	
            userUsageFloat =(long) ((state[NAS_CPU_USER]-psMonCpuInfo.state[NAS_CPU_USER]) * 10000.0 /(float)total);
			kernelUsageFloat = (long)((state[NAS_CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_KERNEL]) * 10000.0 /(float)total);
            waitUsageFloat = (long)((state[NAS_CPU_WAIT]-psMonCpuInfo.state[NAS_CPU_WAIT]) * 10000.0 /(float)total);
			idleUsageFloat = (long)((state[NAS_CPU_IDLE]-psMonCpuInfo.state[NAS_CPU_IDLE]) * 10000.0 /(float)total);
			utilUsageFloat = (long)((state[NAS_CPU_USER]+state[NAS_CPU_KERNEL]-psMonCpuInfo.state[NAS_CPU_UTIL]) * 10000.0 /(float)total);
	
            userUsage =(long) (userUsageFloat / 100.0);
            kernelUsage =(long) (kernelUsageFloat / 100.0);
            waitUsage =(long) (waitUsageFloat / 100.0);
            idleUsage =(long) (idleUsageFloat / 100.0);
            utilUsage =(long) (utilUsageFloat / 100.0);

            psMonCpuInfo.stateUsage[NAS_CPU_USER] = 
                userUsage > 100 ? 100 : userUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_KERNEL] = 
                kernelUsage > 100 ? 100 : kernelUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_WAIT] = 
                waitUsage > 100 ? 100 : waitUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_IDLE] = 
                idleUsage > 100 ? 100 : idleUsage;
            psMonCpuInfo.stateUsage[NAS_CPU_UTIL] = 
                utilUsage > 100 ? 100 : utilUsage;

            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER] = 
                userUsageFloat > 10000 ? 10000 : userUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL] = 
                kernelUsageFloat > 10000 ? 10000 : kernelUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT] = 
                waitUsageFloat > 10000 ? 10000 : waitUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE] = 
                idleUsageFloat > 10000 ? 10000 : idleUsageFloat;
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL] = 
                utilUsageFloat > 10000 ? 10000 : utilUsageFloat;
        }
    }
    psMonCpuInfo.sampletime = curTime;
    psMonCpuInfo.total = curtotal;
    psMonCpuInfo.state[NAS_CPU_USER] = state[NAS_CPU_USER];
    psMonCpuInfo.state[NAS_CPU_KERNEL] = state[NAS_CPU_KERNEL];
    psMonCpuInfo.state[NAS_CPU_WAIT] = state[NAS_CPU_WAIT];
    psMonCpuInfo.state[NAS_CPU_IDLE] = state[NAS_CPU_IDLE];
    psMonCpuInfo.state[NAS_CPU_UTIL] = state[NAS_CPU_USER]+state[NAS_CPU_KERNEL];
    getCurrentTime(tbuf);
    DEBUGMSGTL(("ProcMon/setCpuAverage",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld] core[%ld]\n", tbuf,
    psMonCpuInfo.stateUsage[NAS_CPU_USER],
    psMonCpuInfo.stateUsage[NAS_CPU_KERNEL],
    psMonCpuInfo.stateUsage[NAS_CPU_WAIT],
    psMonCpuInfo.stateUsage[NAS_CPU_IDLE],
    psMonCpuInfo.stateUsage[NAS_CPU_UTIL],
	coreCount.dwNumberOfProcessors));
    DEBUGMSGTL(("ProcMon/setCpuAverageFloat",
                "[%s] usr[%ld] sys[%ld] wait[%ld] idle[%ld] util[%ld] core[%ld]\n", tbuf,
            psMonCpuInfo.stateUsageFloat[NAS_CPU_USER],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_KERNEL],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_WAIT],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_IDLE],
            psMonCpuInfo.stateUsageFloat[NAS_CPU_UTIL],
			coreCount.dwNumberOfProcessors));
}
/** added by D.J Park in 2005.02.23 [end]
 *  modified by HJ.Kwon. in 2005.04.11. */

#endif

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
    pthread_t           stProcMonThr;
    psMonitorTable_context *pstPsRow=NULL;

    /** Added by Corbin in 2004.02.02
    **/
    pthread_t			stProcCheckThr;
 	
    // Now, spawn thread for process checking
    if ( pthread_create(&stProcCheckThr, NULL,
            ProcMon_ProcCheckThreadMain,
        (void*)pstPsRow) != 0 )
    {
        snmp_log_perror("Process checking thread creation failed\n");
    }
    else
    {
    	DEBUGMSGTL(("ProcMon/ProcMon_MasterThread",
            "Thr #%d# Process checking thread started\n",
            MY_THR_ID));
    }
    
    // Get the first row in the container
    pstPsRow = CONTAINER_FIRST(cb.container);


    // As long as we get valid rows
    while(pstPsRow)
    {
        // Do the value check, before proceeding
        if ( (strlen((char*)pstPsRow->psMonitorName) != 0 ) &&
            ( pstPsRow->psMonitorCheckInterval != 0 ) )
        {

            // Now, spawn thread for it
            if ( pthread_create(&stProcMonThr, NULL,
                    ProcMon_ProcMonThreadMain,
                (void*)pstPsRow) != 0 )
            {
                snmp_log_perror("Thread creation failed");
            }
            else
            {

                // Change it to active
                pstPsRow->iThreadActiveFlag = 1;

                DEBUGMSGTL(("ProcMon/ProcMon_MasterThread",
                    "Thr #%d# Master Thread started for < %u %d %s >\n",
                    MY_THR_ID,
                    pstPsRow->idx,
                    pstPsRow->index.len,
                    pstPsRow->psMonitorName));
            }
        }
        else
        {
            // Change it to not-active
            pstPsRow->iThreadActiveFlag = 0;

            snmp_log(LOG_ERR,"ProcMon/ProcMon_MasterThread \
                Thr #%d# Thread could not be started for OID < %u >\n",
                MY_THR_ID,
                pstPsRow->idx);
        }


        // Get the next row
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
    psMonitorTable_context  *pstMycontext;
    struct  stat            *pstStatBuf;
    char                    *acTmpStr=NULL;
    stVarBindArgProc	    *varBinds=NULL;
    char                    tbuf[30] ={0};
    char		    psNamePath[200]={0};

    /** added by HJ.Kwon. 2004.07.14. */
    int		iProcCount=0;
    int		iStatus=-1; //Unknown -1, Down 0, Up 1, Threshold 2
    ps_cache_info *pstNull = NULL;
    

    DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
            "Thr #%d# Thread spawned \n",
            MY_THR_ID));

    pstMycontext = (psMonitorTable_context*)psData;

    if(pstMycontext != NULL)
    {
        pstMycontext->thrcpu = 0;
        pstMycontext->thrmem = 0;
    }

    while(1)
    {
        // Before i start checking for process,
        // get the latest values of the data
        pstMycontext = CONTAINER_FIND(cb.container,&(pstMycontext->index));

        // If the row does not exist anymore
        if ( pstMycontext == NULL )
        {
            DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                "Thr #%d# Exiting thread\n",MY_THR_ID));

            pthread_detach(pthread_self());
            pthread_exit(NULL);
        }

        // Check, if the row status is not active, exit the thread
        if ( pstMycontext->psMonitorStatus != SNMP_ROW_ACTIVE )
        {
            DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                "Thr #%d# Exiting thread\n",MY_THR_ID));

            // Unset the thread active flag
            pstMycontext->iThreadActiveFlag = 0;

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

                SLEEP(TIME_TO_SLEEP);
                continue;   // No need to process the request,
                            // until valid values are provided
            }

            // For reaction 2,3 check for reaction path also.
            if ( ( ( pstMycontext->psMonitorReaction == 2 ) ||
                ( pstMycontext->psMonitorReaction == 3 ) ) &&
                (strlen((char*)pstMycontext->psMonitorReactionPath) == 0 ) )
            {
                SLEEP(TIME_TO_SLEEP);
                continue;   // No need to process the request,
                            // until valid values are provided
            }
        }
        else
        {
            // since the row is not active
                SLEEP(TIME_TO_SLEEP);
                continue;
        }

        if ( (guiTransLogFileAlarmReg == 0) &&
            (glAgentInfoFileTrPort > 0) &&
            (strlen(gcAgentInfoFileTransferIP) > 0))
        {
            DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                "Thr #%d# Registering the alarm for ProcMon Log file transfer\n",
                MY_THR_ID));
            if (glAgentInfoFileTrInterval > 0 )
                guiTransLogFileAlarmReg = snmp_alarm_register (glAgentInfoFileTrInterval,
                (int) NULL ,Common_SendFileThread, gcInfoLogFileProc);
            else
                guiTransLogFileAlarmReg = snmp_alarm_register (1,
                (int) NULL ,Common_SendFileThread, gcInfoLogFileProc);

        }

        /** added by D.J Park in 2005.03.02 [start] */
        if(getMonitoringTime(pstMycontext->psMonitorStartHour,pstMycontext->psMonitorEndHour) == 0)
        {
            getCurrentTime(tbuf);
            DEBUGMSGTL(("ProcMon/MonitoringTime",
                "Thr #%d# curtime[%s] startHour[%d] endHour[%d] "
                "not Monitoring\n",MY_THR_ID,tbuf, 
                pstMycontext->psMonitorStartHour,
                pstMycontext->psMonitorEndHour ));
            
            SLEEP(TIME_TO_SLEEP);
            continue;
        }
        /** added by D.J Park in 2005.03.02 [end] */


        DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
            "Thr #%d# Checking OID <%u> OID_LEN <%d> PROC <%s>\n",
            MY_THR_ID,
            *(pstMycontext->index.oids),
            pstMycontext->index.len,
            pstMycontext->psMonitorName));

#ifndef WIN32

        if ( pstMycontext->psMonitorReaction != 1 )
        {
            // Check if the executable really exists
            // and is not a directory

            pstStatBuf = (struct stat *)malloc(sizeof(struct stat));

            if ( !pstStatBuf )
            {
                snmp_log_perror("ProcMon: Malloc failed");
                return;
            }

            /**
             * modify by D.J Park in 2003.02.25
            if(stat(pstMycontext->psMonitorReactionPath,pstStatBuf) == -1)
            */
            getProcessName(pstMycontext->psMonitorReactionPath,psNamePath);	
            if(stat(psNamePath,pstStatBuf) == -1)
            {
                free(pstStatBuf);

                DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                            "Thr #%d# Given Reaction path does not exist\n",
                             MY_THR_ID));

                SLEEP(TIME_TO_SLEEP);

                continue;
            }
            else
                if ( pstStatBuf->st_size == 0 )
                {
                    free(pstStatBuf);

                    DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                                "Thr #%d# Given Reaction path is an EMPTY FILE\n",
                                MY_THR_ID));
                    SLEEP(TIME_TO_SLEEP);
                    continue;
                }
                else
                if ( S_ISDIR(pstStatBuf->st_mode) )
                {
                    free(pstStatBuf);

                    DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                                "Thr #%d# Given reaction Path is a directory\n",
                                MY_THR_ID));

                    SLEEP(TIME_TO_SLEEP);

                    continue;
                }
                else
                {
                    // Check if the process name and the reaction path
                    // executable name is same

                    // Get the last part of the Reaction Path

		/**
		 * modify by D.J Park in 2004.02.26
                    acTmpStr = strrchr(pstMycontext->psMonitorReactionPath,
		                          DIR_SEPARATOR);
                    acTmpStr = strrchr(psNamePath,DIR_SEPARATOR);

                    if ( acTmpStr  == NULL )
                    {
                        DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                                "Thr #%d# Invalid path\n",
                                MY_THR_ID));
                        SLEEP(TIME_TO_SLEEP);

                        continue;
                    }
                    else
                    {
                        acTmpStr++; // Get rid of the separator

                        if ( strcmp(acTmpStr,pstMycontext->psMonitorName) != 0 )
                        {

                            DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                                "Thr #%d# Process name and Reaction Path do not match\n",
                                MY_THR_ID));
                            SLEEP(TIME_TO_SLEEP);

                            continue;
                        }
                    }
                  */
                }

            // Free the stat buffer
            free(pstStatBuf);
        }
#else
// TODO: Add windows implementation
#endif
		
        // Check for the process status
		
        /**
         ** modified by Corbin in 2004.02.02
         ** modified by HJ.Kwon. in 2004.07.15
        iCurPid = ProcMon_CheckProc(pstMycontext->psMonitorName);
        */
        pthread_mutex_lock(&procMonProcessCheckLock);
        //iProcCount = ProcMon_CheckProc(pstMycontext->psMonitorName, pstMycontext->psMonitorParameter);
        iProcCount = ProcMon_CheckProc(pstMycontext);
        pthread_mutex_unlock(&procMonProcessCheckLock);

        if (iProcCount == -99) {
            SLEEP(TIME_TO_SLEEP);
            continue;
        }
			
        // Now, based on the reaction type, take appropriate action
        if ( pstMycontext->psMonitorReaction == 1 )
        {
            // i.e. the process state has changed
            // First time if the process is not running
            /*
             * modified by D.J Park in 2004.1.12
             * send alert when process is only died
            if ( ( iCurPid == -1 ) && ( iOldPid == -1 ) && ( iSendAlertFlag == 0 ) )
            {
                if (Common_SendAlert("PSMN",
                        gcInfoLogFileProc,
                        pstMycontext->psMonitorAlertMessage,
                        pstMycontext->psMonitorIndex))
                    DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                        "Thr #%d# Alert sending failed for <%s>\n",
                        MY_THR_ID,
                        pstMycontext->psMonitorName));
                else
                    iSendAlertFlag = 1;
            }
            else if ( iCurPid != iOldPid)
            {
                if (Common_SendAlert("PSMN",
                    gcInfoLogFileProc,
                    pstMycontext->psMonitorAlertMessage,
                    pstMycontext->psMonitorIndex))
                    DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                        "Thr #%d# Alert sending failed for <%s>\n",
                        MY_THR_ID,
                        pstMycontext->psMonitorName));
                else
                    iSendAlertFlag = 1;

            }
            */

            /** created by HJ.Kwon. 2004.07.14. start
             * It's for function of psNumber MIB 
             */
            if(pstMycontext->psMonitorNumber && 
                pstMycontext->psMonitorNumber < iProcCount &&
                iStatus != 2)
            {
                if(iStatus == 0)
                /* If istatus is changed 2 from 0 directly,
                 * it must send up-trap before sending threshold-trap.
                 */
                {
                    varBinds = makePsMonitorVarBind_Up(pstMycontext,
                                                       iProcCount);
                    Common_SendAlert_And_Vars("PSMN", gcInfoLogFileProc,
                                   pstMycontext->psMonitorAlertMessage,
                                   pstMycontext->psMonitorIndex,varBinds,6);
                    getCurrentTime(tbuf);
                    snmp_log(LOG_INFO,
                        "[%s] Thr #%d# Up Alert sending for <%s>\n",
                        tbuf,MY_THR_ID, pstMycontext->psMonitorName);
                        if(varBinds != NULL)
                            free(varBinds);
                }

                varBinds = makePsMonitorVarBind_Threshold(pstMycontext,
                                                         iProcCount);
                Common_SendAlert_And_Vars("PSMN", gcInfoLogFileProc,
                                pstMycontext->psMonitorAlertMessage,
                                pstMycontext->psMonitorIndex,varBinds,6);
                getCurrentTime(tbuf);
                snmp_log(LOG_INFO,
                    "[%s] Thr #%d# Threshold Alert sending for <%s>\n",
                    tbuf,MY_THR_ID, pstMycontext->psMonitorName);

                iStatus = 2;
                if(varBinds != NULL)
                    free(varBinds);
            }
            else if(pstMycontext->psMonitorNumber && 
                    pstMycontext->psMonitorNumber >= iProcCount &&
                    iStatus == 2)
            {
                varBinds = makePsMonitorVarBind_Recovery(pstMycontext,
                                            iProcCount);
                Common_SendAlert_And_Vars("PSMN", gcInfoLogFileProc,
                                    pstMycontext->psMonitorAlertMessage,
                                    pstMycontext->psMonitorIndex,varBinds,6);

                getCurrentTime(tbuf);
                snmp_log(LOG_INFO,
                    "[%s] Thr #%d# Recovery Alert sending for <%s>\n",
                    tbuf,MY_THR_ID, pstMycontext->psMonitorName);

                iStatus = 1;
                if(varBinds != NULL)
                    free(varBinds);

                if(iProcCount == 0)
                /* If istatus is changed 0 from 2 directly,
                 * it must send recovery-trap before sending down-trap.
                 * HJ.Kwon*/
                {
                    /** added by HJ.Kwon. 2005.09.07 
                     * it is going to send cpu or memory recovery trap
                     * before sending process down trap */
                    if(pstMycontext->thrcpu > 0)
                    {
                        pstMycontext->thrcpu = 0;
                        sendThresholdTrap(pstMycontext,pstNull,COLUMN_PSMONITORCPU,0);
                    }
                    if(pstMycontext->thrmem > 0)
                    {
                        pstMycontext->thrmem = 0;
                        sendThresholdTrap(pstMycontext,pstNull,COLUMN_PSMONITORMEM,0);
                    }
                    /* end : HJ.Kwon 2005.09.07. */

                    varBinds = makePsMonitorVarBind_Down(pstMycontext,
                                                        iProcCount);
                    Common_SendAlert_And_Vars("PSMN", gcInfoLogFileProc,
                                pstMycontext->psMonitorAlertMessage,
                                pstMycontext->psMonitorIndex,varBinds,6);

                    getCurrentTime(tbuf);
                    snmp_log(LOG_INFO,
                        "[%s] Thr #%d# Down Alert sending for <%s>\n",
                        tbuf,MY_THR_ID, pstMycontext->psMonitorName);

                    iStatus = 0;	
                    if(varBinds != NULL)
                        free(varBinds);
                }
            }
            else if (iStatus && !iProcCount) //down
            {
                /** added by HJ.Kwon. 2005.09.07 
                 * it is going to send cpu or memory recovery trap
                 * before sending process down trap */
                if(pstMycontext->thrcpu > 0)
                {
                    pstMycontext->thrcpu = 0;
                    sendThresholdTrap(pstMycontext,pstNull,COLUMN_PSMONITORCPU,0);
                }
                if(pstMycontext->thrmem > 0)
                {
                    pstMycontext->thrmem = 0;
                    sendThresholdTrap(pstMycontext,pstNull,COLUMN_PSMONITORMEM,0);
                }
                /* end : HJ.Kwon 2005.09.07. */

                varBinds = makePsMonitorVarBind_Down(pstMycontext,
                                                     iProcCount);
                Common_SendAlert_And_Vars("PSMN", gcInfoLogFileProc,
                                pstMycontext->psMonitorAlertMessage,
                                pstMycontext->psMonitorIndex,varBinds,6);

                getCurrentTime(tbuf);
                snmp_log(LOG_INFO,
                    "[%s] Thr #%d# Down Alert sending for <%s>\n",
                    tbuf,MY_THR_ID, pstMycontext->psMonitorName);

                iStatus = 0;
                if(varBinds != NULL)
                    free(varBinds);
            }
            else if (iStatus < 1 && iProcCount) //up
            {
                varBinds = makePsMonitorVarBind_Up(pstMycontext,
                                                   iProcCount);
                Common_SendAlert_And_Vars("PSMN", gcInfoLogFileProc,
                	pstMycontext->psMonitorAlertMessage,
                	pstMycontext->psMonitorIndex,varBinds,6);

                getCurrentTime(tbuf);
                snmp_log(LOG_INFO,
                    "[%s] Thr #%d# Up Alert sending for <%s>\n",
                    tbuf,MY_THR_ID, pstMycontext->psMonitorName);

                iStatus = 1;
                if(varBinds != NULL)
                    free(varBinds);
            }
       	}
        /** end : HJ.Kwon. in 2004.07.14. */

        else
        if ( pstMycontext->psMonitorReaction == 2 )
        {
            // Restart the process, if not running
            // and also send alert if the process was
            // restarted in the last cycle
            if (iStatus == -1)
            {
                if(iProcCount)
                {
                    varBinds = makePsMonitorVarBind_Up(pstMycontext,iProcCount);
                    if(Common_SendAlert_And_Vars("PSMN", gcInfoLogFileProc,
   	                    pstMycontext->psMonitorAlertMessage,
                            pstMycontext->psMonitorIndex,varBinds,6))
                    {
                        getCurrentTime(tbuf);
                        snmp_log(LOG_INFO,
                            "[%s] Thr #%d# Up Alert sending failed for <%s>\n",
                            tbuf,MY_THR_ID, pstMycontext->psMonitorName);
                    } else
                    {
                        getCurrentTime(tbuf);
                        snmp_log(LOG_INFO,
                            "[%s] Thr #%d# Up Alert sending for <%s>\n",
                            tbuf,MY_THR_ID, pstMycontext->psMonitorName);
                    }
                    iStatus = 1;	
                    if(varBinds != NULL)
                        free(varBinds);
                }
            }

            if (!iProcCount)
            {
                if ( ProcMon_RestartProcess(pstMycontext->psMonitorReactionPath)
                        == -1 )
                {
                    snmp_log_perror("Process restarting failed");
                }

                /** rewrite by D.J Park in 2004.1.14
                if( Common_SendAlert("PSMN",
                    gcInfoLogFileProc,
                    pstMycontext->psMonitorAlertMessage,
                    pstMycontext->psMonitorIndex))
                    DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                        "Thr #%d# Alert sending failed for <%s>\n",
                        MY_THR_ID,
                        pstMycontext->psMonitorName));
                 */
                varBinds = makePsMonitorVarBind_Up(pstMycontext,
                                                    iProcCount);
                if( Common_SendAlert_And_Vars("PSMN",
                    gcInfoLogFileProc,
                    pstMycontext->psMonitorAlertMessage,
                    pstMycontext->psMonitorIndex,varBinds,6))
                {
                    getCurrentTime(tbuf);
                    snmp_log(LOG_ERR,
                        "[%s] Thr #%d# Up Alert sending failed for <%s>\n",
                        tbuf,MY_THR_ID, pstMycontext->psMonitorName);
                }
                getCurrentTime(tbuf);
                snmp_log(LOG_ERR,
                    "[%s] Thr #%d# Up Alert sending for <%s>\n",
                    tbuf,MY_THR_ID, pstMycontext->psMonitorName);
                if(varBinds != NULL)
                    free(varBinds);
            }
        }
        else
        if ( pstMycontext->psMonitorReaction == 3 )
        {
            // Do both the above
            // restart the process, if not running, and send the
            // alert only once

            if (iStatus == -1)
            {
                if(iProcCount)
                {
                    varBinds = makePsMonitorVarBind_Up(pstMycontext,iProcCount);
                    if(Common_SendAlert_And_Vars("PSMN", gcInfoLogFileProc,
   	                    pstMycontext->psMonitorAlertMessage,
                        pstMycontext->psMonitorIndex,varBinds,6))
                    {
                        getCurrentTime(tbuf);
                        snmp_log(LOG_INFO,
                            "[%s] Thr #%d# Up Alert sending failed for <%s>\n",
                            tbuf,MY_THR_ID, pstMycontext->psMonitorName);
                    } else
                    {
                        getCurrentTime(tbuf);
                        snmp_log(LOG_INFO,
                            "[%s] Thr #%d# Up Alert sending for <%s>\n",
                            tbuf,MY_THR_ID, pstMycontext->psMonitorName);
                    }
				    iStatus = 1;	
				    if(varBinds != NULL)
				        free(varBinds);
                }
            }

            if (!iProcCount)
            {
                /* * rewrite by D.J Park in 2004.1.14
				 *
                // Send an alert pcMesg
                if (Common_SendAlert("PSMN",
                    gcInfoLogFileProc,
                    pstMycontext->psMonitorAlertMessage,
                    pstMycontext->psMonitorIndex))
                    DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                        "Thr #%d# Alert sending failed for <%s>\n",
                        MY_THR_ID,
                        pstMycontext->psMonitorName));

                if ( ProcMon_RestartProcess(pstMycontext->psMonitorReactionPath)
                        == -1 )
                {
                    snmp_log_perror("Process restarting failed");
                }

                // Send an alert pcMesg
                if (Common_SendAlert("PSMN",
                    gcInfoLogFileProc,
                    pstMycontext-> psMonitorAlertMessage,
                    pstMycontext->psMonitorIndex))
                    DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                        "Thr #%d# Alert sending failed for <%s>\n",
                        MY_THR_ID,
                        pstMycontext->psMonitorName));
				*/
                // Send an alert pcMesg
                if(iStatus != 0)
                {
                    /** added by HJ.Kwon. 2005.09.07 
                     * it is going to send cpu or memory recovery trap
                     * before sending process down trap */
                    if(pstMycontext->thrcpu > 0)
                    {
                        pstMycontext->thrcpu = 0;
                        sendThresholdTrap(pstMycontext,pstNull,COLUMN_PSMONITORCPU,0);
                    }
                    if(pstMycontext->thrmem > 0)
                    {
                        pstMycontext->thrmem = 0;
                        sendThresholdTrap(pstMycontext,pstNull,COLUMN_PSMONITORMEM,0);
                    }
                    /* end : HJ.Kwon 2005.09.07. */

                    varBinds = makePsMonitorVarBind_Down(pstMycontext,
                              iProcCount);
                    if (Common_SendAlert_And_Vars("PSMN",
                        gcInfoLogFileProc,
                        pstMycontext->psMonitorAlertMessage,
                        pstMycontext->psMonitorIndex,varBinds,6))
                    {
                        getCurrentTime(tbuf);
                        snmp_log(LOG_ERR, "[%s] Thr #%d# Down Alert sending "
                                "failed for <%s>\n", 
                                tbuf,MY_THR_ID, pstMycontext->psMonitorName);
                    }
                    getCurrentTime(tbuf);
                    snmp_log(LOG_ERR, "%s Thr #%d# Down Alert sending "
                            "for <%s>\n",
                         tbuf,MY_THR_ID, pstMycontext->psMonitorName);
				    if(varBinds != NULL)
					    free(varBinds);
                }

                if ( ProcMon_RestartProcess(pstMycontext->psMonitorReactionPath)
                        == -1 )
                {
                    snmp_log_perror("Process restarting failed");
                } 

                // Send an alert pcMesg
                /** modified by D.J Park in 2005.03.11 
                 * do not send process up trap 
                 * check process int next time
                 */

                iStatus = 0;
            } 
            else
            {
                if(iStatus == 0)
                {
				    varBinds = 
                        makePsMonitorVarBind_Up(pstMycontext, iProcCount);
                    if (Common_SendAlert_And_Vars("PSMN", gcInfoLogFileProc, 
                                pstMycontext-> psMonitorAlertMessage, 
                                pstMycontext->psMonitorIndex,varBinds,6)) 
				    { 
                        getCurrentTime(tbuf); 
                        snmp_log(LOG_ERR, "[%s] Thr #%d# Up Alert sending "
                                "failed for <%s>\n",
                                tbuf,MY_THR_ID, pstMycontext->psMonitorName); 
                    } 
                    getCurrentTime(tbuf); 
                    snmp_log(LOG_ERR, "[%s] Thr #%d# Up Alert sending "
                            "for <%s>\n", 
                            tbuf,MY_THR_ID, pstMycontext->psMonitorName);

                    if(varBinds != NULL) 
                        free(varBinds); 

                    iStatus =1;
                }
            }
        }
        else
            DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
                "Thr #%d# Invalid Reaction type specified for <%s>\n",
                MY_THR_ID,
                pstMycontext->psMonitorName));

        // Sleep until the next interval
        DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
            "Thr #%d# Thread is going to Sleep for <%u>!!\n",
            MY_THR_ID,
            pstMycontext->psMonitorCheckInterval));

#ifdef WIN32
        SLEEP(MAX(pstMycontext->psMonitorCheckInterval,CHECK_INTERVAL));
#else
        SLEEP(pstMycontext->psMonitorCheckInterval);
#endif

        DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain",
            "Thr #%d# Thread woke up !!\n",
            MY_THR_ID));


    } // End of infinite while loop

}


/*****************************************************************************
 *
 * name             :   Common_SendAlert
 * description      :   This function is used to send the alert pcMesg to the
 *                      manager
 * input parameters :   char *pcModCode - Module code which wants to sent alert
 *                      char *gcInfoLogFile - Log File
 *                      char *pcMesg - Alert message
 *                      unsigned long ulRowIndex - Row Context
 * output parameters:   None
 * return type      :   int - returns success (0) or failure (1)
 * global variables :   None
 * calls            :   None
 */

int
Common_SendAlert(char *pcModCode, char *pcInfoLogFile, char *pcMesg, unsigned long ulRowIndex)
{
    FILE                *pfOutFd = NULL;
    nuriAlert           stAlert = {0};
    char                acSysName[MAXHOSTNAMELEN+1];

#ifndef WIN32
    struct utsname      sys = {0};
#else
    char                sys[MAXHOSTNAMELEN + 1];
    WSADATA stWsData;
#endif

    DEBUGMSGTL(("Common/Common_SendAlert",
        "Received alert request from <%s> module Output Logfile <%s>\n",
        pcModCode,
        pcInfoLogFile));


    // prepare the stAlert
    // Get the local machine IP
    Common_GetLocalHostIp((stAlert.masterIp));

    // If the IP returned, is NULL
    if ( stAlert.masterIp == NULL )
        strcpy(stAlert.masterIp,"");

    // Get the current date and time string (YYYY:MM:DD:HH:MI:SS)
    Common_GetCurrDateTime(stAlert.dateTime);

    if ( stAlert.dateTime == NULL )
        strcpy(stAlert.dateTime,"");

#ifndef WIN32
    uname(&sys);

    if ( sys.nodename != NULL )
        strcpy(acSysName , sys.nodename);
    else
        strcpy(acSysName , "Unknown Host");

#else
    WSAStartup(MAKEWORD( 2, 2 ),&stWsData);

    //This Statement can be unsucessful at times.
    gethostname(sys,MAX_NAME_LEN);

    if ( sys != NULL )
        strcpy(acSysName , sys);
    else
        strcpy(acSysName , "Unknown Host");
#endif

    // Prepare the stAlert message and pass it to
    // Common_SendMesg function
    strcpy(stAlert.modCode, pcModCode);
    strcpy(stAlert.sysName,acSysName);

    if ( pcMesg != NULL )
        strcpy(stAlert.alertMesg,pcMesg);
    else
        strcpy(stAlert.alertMesg,"");

    stAlert.index = ulRowIndex;


    // Check the logfile size has reached maximum allowed or
    // not, if yes, transfer the file first
    Common_CheckFileSize(pcInfoLogFile);

    if ( strcmp(pcModCode,"PSMN") == 0 )
    {
        // If it is ProcMon
        // Write the Alert message into it

        // If output file is specified
        // Now, log this in to the Proc Mon Output logfile too
        if ( strlen(pcInfoLogFile) == 0 )
            snmp_log(LOG_ERR,"No Process Log File specified in the MIB\n");
        else
        {

            if ( (pfOutFd = fopen( pcInfoLogFile, "a") ) == NULL )
            {
                snmp_log(LOG_ERR,"Error opening %s",pcInfoLogFile);
                snmp_log_perror("fopen failed for pcInfoLogFile");
            }
            else
            {

                DEBUGMSGTL(("Common/Common_SendAlert",
                    "Thr #%d# File <%s> opened\n",
                    MY_THR_ID,
                    pcInfoLogFile));

                /* modified by Corbin in 2004.02.02
                fprintf(pfOutFd, "%s%s%s%s%s%s%s%s%s\n",
                               stAlert.modCode,
                               ALERT_SEPARATOR,
                               stAlert.masterIp,
                               ALERT_SEPARATOR,
                               stAlert.dateTime,
                               ALERT_SEPARATOR,
                               stAlert.sysName,
                               ALERT_SEPARATOR,
                               stAlert.alertMesg);
				*/
                FCLOSE(pfOutFd);

                DEBUGMSGTL(("Common/Common_SendAlert",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    pcInfoLogFile));


            } // End of Else

        } // End of Else
    }


	// Now, send this stAlert pcMesg over UDP socket
    if ( Common_SendMesg(stAlert) == -1 )
	{
		snmp_log(LOG_ERR,"UDP pcMesg sending to manager failed\n");
		return(1);
	}
	else
        DEBUGMSGTL(("Common/Common_SendAlert",
            "Thr #%d# Trap Mesg sent to the Alert Manager\n",
            MY_THR_ID));

	return (0);
}

/*****************************************************************************
 *
 * name             :   Common_GetLocalHostIp
 * description      :   This function is used to get the IP of the localhost
 * input parameters :   char *pcSysIp
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   None
 */

void
Common_GetLocalHostIp(char *pcSysIp)
{
    struct hostent      *he = NULL;
    struct sockaddr_in  in = {0};

#ifndef WIN32 // For unix
    struct utsname      host_name = {0};
#else  // For windows
    char                host_name[MAXHOSTNAMELEN + 1];
    WSADATA             stWsData = {0};
#endif

#ifndef WIN32 // For Unix
    uname (&host_name);
#else // For Windows

    WSAStartup(MAKEWORD( 2, 2 ),&stWsData);
    gethostname(host_name,MAX_NAME_LEN);

#endif

#ifndef WIN32 // For Unix
    if ((he = gethostbyname(host_name.nodename)) == NULL)
#else  // For Windows
    if ((he = gethostbyname(host_name)) == NULL)
#endif
    {
        snmp_log_perror("error resolving hostname..");
        return;
    }

    memcpy(&in.sin_addr, he->h_addr_list[0], he->h_length);
    strcpy(pcSysIp,(char*)inet_ntoa(in.sin_addr));

    // To make sure it is null terminated
    pcSysIp[strlen(pcSysIp)]=0;

    DEBUGMSGTL(("Common",
        "Thr #%d# Localhost IP <%s>\n",
        MY_THR_ID,
        pcSysIp));

    return;
}

/*****************************************************************************
 *
 * name             :   Common_GetCurrDateTime
 * description      :   This function is used to get the current date-time
 *                      value string in format "YYYY:MM:DD:HH:MI:SS"
 * input parameters :   char *pcCurrTime - Pointer to the Curr Time string
 * output parameters:   None
 * return type      :   void
 * global variables :   None
 * calls            :   None
 */

void
Common_GetCurrDateTime(char *pcCurrTime)
{
#ifndef WIN32 // For unix
    struct tm           *pstTimeStruct = NULL;
    struct timeval      stTimeVal = {0};
    struct timeval      *pstTime=&stTimeVal;
    time_t              stTimeValue = {0};

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

    sprintf(pcCurrTime,"%.4d:%.2d:%.2d:%.2d:%.2d:%.2d",
                       pstTimeStruct.wYear,
                       pstTimeStruct.wMonth ,
                       pstTimeStruct.wDay,
                       pstTimeStruct.wHour,
                       pstTimeStruct.wMinute,
                       pstTimeStruct.wSecond);
#endif

    DEBUGMSGTL(("Common",
        "Thr #%d# Current Time <%s>\n",
        MY_THR_ID,
        pcCurrTime));

    return;
}



/*****************************************************************************
 *
 * name             :   Common_SendMesg
 * description      :   This function is used to send UDP Trap to the
 *                      manager
 * input parameters :   nuriAlert stAlert - alert message struct
 * output parameters:   None
 * return type      :   int - returs sucess (0) or failure (-1)
 * global variables :   None
 * calls            :   getIPFromASNFormat
 */

int
Common_SendMesg(nuriAlert stAlert)
{
    struct sockaddr_in  stSrvName = {0};
    struct hostent      *pstHostEnt = NULL;
    stVarBindArgProc    args = { 0 };
    char                aAlertMesg[sizeof(nuriAlert) + 5*sizeof(ALERT_SEPARATOR)] = {0};
    char                aBuffer[MAX_HOST_IP_LEN +1] = { 0 };
    char                acTrapOid[]=".1.3.6.1.4.1.3204.1.3.35.2.1.1.16";

    if ( strcmp(stAlert.modCode,"PSMN") == 0 )
    {
        strcpy(acTrapOid,".1.3.6.1.4.1.3204.1.3.35.3.1.11");
    }
    else
    if ( strcmp(stAlert.modCode,"LGRM") == 0 )
    {
        strcpy(acTrapOid,".1.3.6.1.4.1.3204.1.3.35.2.2.1.7");
    }
    else
    if ( strcmp(stAlert.modCode,"LGCM") == 0 )
    {
        strcpy(acTrapOid,".1.3.6.1.4.1.3204.1.3.35.2.1.1.16");
    }


    // Form trap message argument
/*
 * modified hhyoon 2004.1.9
    sprintf (args.variable, "%s.%lu", acTrapOid, stAlert.index);
 */
    sprintf (args.variable, "%s",acTrapOid);
    args.type = 's';

/*
 * modified hhyoon 2004.1.9
    sprintf(aAlertMesg,"%s%s%s%s%s%s%s%s%s\n",
               stAlert.modCode,
               ALERT_SEPARATOR,
               stAlert.masterIp,
               ALERT_SEPARATOR,
               stAlert.dateTime,
               ALERT_SEPARATOR,
               stAlert.sysName,
               ALERT_SEPARATOR,
               stAlert.alertMesg);
 */

	sprintf(aAlertMesg, "%s\n", stAlert.alertMesg);

    strcpy (args.value, aAlertMesg);

    // Change the Ip from ASN format to character string
    getIPFromASNFormat(gcAgentInfoAlertMgrIp, aBuffer);

    DEBUGMSGTL(("Common/Common_SendMesg",
        "Thr #%d# trap Dest IP=%s port=%d\n",
        MY_THR_ID,
        aBuffer,
        glAgentInfoAlertMgrPort));

    if ( glAgentInfoAlertMgrPort <= 0 )
    {
        snmp_log(LOG_ERR,"Common_SendMesg: Invalid Alert Manager port number <%d>\n",
            glAgentInfoAlertMgrPort);
        return (-1);
    }

    // Send the trap message to Alert Manager
    Common_SendSnmptrap (1, &args, aBuffer, glAgentInfoAlertMgrPort,stAlert.modCode);

    return(0);
}

/** rewrite by D.J Park in 2003.12.13
 *	modified by HJ.Kwon in 2004.07.15
 */
stVarBindArgProc *makePsMonitorVarBind_Up(psMonitorTable_context *pstMycontext,int processNum)
{
	stVarBindArgProc* varBinds = (stVarBindArgProc *)NULL;
	int var_cnt = 6;

	varBinds = (stVarBindArgProc *)malloc(sizeof(stVarBindArgProc) * var_cnt);
	memset(varBinds,0x00,sizeof(stVarBindArgProc) * var_cnt);

	strcpy(varBinds[0].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.2");
	varBinds[0].type='s';
	strcpy(varBinds[0].value,pstMycontext->psMonitorName);

	strcpy(varBinds[1].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.3");
	varBinds[1].type='s';
	strcpy(varBinds[1].value,pstMycontext->psMonitorParameter);


	strcpy(varBinds[2].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.4");
	varBinds[2].type='i';
	sprintf(varBinds[2].value,"%d",processNum);

	strcpy(varBinds[3].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.6");
	varBinds[3].type='s';
	strcpy(varBinds[3].value,"up");

	strcpy(varBinds[4].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.11");
	varBinds[4].type='s';
	strcpy(varBinds[4].value,pstMycontext->psMonitorAlertMessage);

	strcpy(varBinds[5].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.13");
	varBinds[5].type='i';
	sprintf(varBinds[5].value,"%ld",pstMycontext->psMonitorNumber);

	return varBinds;
}

stVarBindArgProc *makePsMonitorVarBind_Down(psMonitorTable_context *pstMycontext,int processNum)
{
	stVarBindArgProc* varBinds = (stVarBindArgProc *)NULL;
	int var_cnt = 6;

	varBinds = (stVarBindArgProc *)malloc(sizeof(stVarBindArgProc) * var_cnt);
	memset(varBinds,0x00,sizeof(stVarBindArgProc) * var_cnt);

	strcpy(varBinds[0].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.2");
	varBinds[0].type='s';
	strcpy(varBinds[0].value,pstMycontext->psMonitorName);

	strcpy(varBinds[1].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.3");
	varBinds[1].type='s';
	strcpy(varBinds[1].value,pstMycontext->psMonitorParameter);


	strcpy(varBinds[2].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.4");
	varBinds[2].type='i';
	sprintf(varBinds[2].value,"%d",processNum);

	strcpy(varBinds[3].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.6");
	varBinds[3].type='s';
	strcpy(varBinds[3].value,"down");

	strcpy(varBinds[4].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.11");
	varBinds[4].type='s';
	strcpy(varBinds[4].value,pstMycontext->psMonitorAlertMessage);

	strcpy(varBinds[5].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.13");
	varBinds[5].type='i';
	sprintf(varBinds[5].value,"%ld",pstMycontext->psMonitorNumber);

	return varBinds;
}

/** added by HJ.Kwon. in 2004.07.14 
*/
stVarBindArgProc *makePsMonitorVarBind_Threshold(psMonitorTable_context *pstMycontext,int processNum)
{
	stVarBindArgProc* varBinds = (stVarBindArgProc *)NULL;
	int var_cnt = 6;

	varBinds = (stVarBindArgProc *)malloc(sizeof(stVarBindArgProc) * var_cnt);
	memset(varBinds,0x00,sizeof(stVarBindArgProc) * var_cnt);

	strcpy(varBinds[0].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.2");
	varBinds[0].type='s';
	strcpy(varBinds[0].value,pstMycontext->psMonitorName);

	strcpy(varBinds[1].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.3");
	varBinds[1].type='s';
	strcpy(varBinds[1].value,pstMycontext->psMonitorParameter);


	strcpy(varBinds[2].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.4");
	varBinds[2].type='i';
	sprintf(varBinds[2].value,"%d",processNum);

	strcpy(varBinds[3].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.6");
	varBinds[3].type='s';
	strcpy(varBinds[3].value,"threshold");

	strcpy(varBinds[4].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.11");
	varBinds[4].type='s';
	strcpy(varBinds[4].value,pstMycontext->psMonitorAlertMessage);

	strcpy(varBinds[5].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.13");
	varBinds[5].type='i';
	sprintf(varBinds[5].value,"%d",pstMycontext->psMonitorNumber);

	return varBinds;
}

stVarBindArgProc *makePsMonitorVarBind_Recovery(psMonitorTable_context *pstMycontext,int processNum)
{
	stVarBindArgProc* varBinds = (stVarBindArgProc *)NULL;
	int var_cnt = 6;

	varBinds = (stVarBindArgProc *)malloc(sizeof(stVarBindArgProc) * var_cnt);
	memset(varBinds,0x00,sizeof(stVarBindArgProc) * var_cnt);

	strcpy(varBinds[0].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.2");
	varBinds[0].type='s';
	strcpy(varBinds[0].value,pstMycontext->psMonitorName);

	strcpy(varBinds[1].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.3");
	varBinds[1].type='s';
	strcpy(varBinds[1].value,pstMycontext->psMonitorParameter);


	strcpy(varBinds[2].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.4");
	varBinds[2].type='i';
	sprintf(varBinds[2].value,"%d",processNum);

	strcpy(varBinds[3].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.6");
	varBinds[3].type='s';
	strcpy(varBinds[3].value,"recovery");

	strcpy(varBinds[4].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.11");
	varBinds[4].type='s';
	strcpy(varBinds[4].value,pstMycontext->psMonitorAlertMessage);

	strcpy(varBinds[5].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.13");
	varBinds[5].type='i';
	sprintf(varBinds[5].value,"%d",pstMycontext->psMonitorNumber);

	return varBinds;
}

/** end : HJ.Kwon */
int
Common_SendAlert_And_Vars(char *pcModCode, char *pcInfoLogFile, char *pcMesg, unsigned long ulRowIndex,stVarBindArgProc *varBinds, int var_cnt)
{
    FILE                *pfOutFd = NULL;
    nuriAlert           stAlert = {0};
    char                acSysName[MAXHOSTNAMELEN+1];

#ifndef WIN32
    struct utsname      sys = {0};
#else
    char                sys[MAXHOSTNAMELEN + 1];
    WSADATA stWsData;
#endif

    DEBUGMSGTL(("Common/Common_SendAlert",
        "Received alert request from <%s> module Output Logfile <%s>\n",
        pcModCode,
        pcInfoLogFile));


    /** rewrite by D.J Park in 2004.1.13
     */
    pthread_mutex_lock(&sendTrapLock);
    // prepare the stAlert
    // Get the local machine IP
    Common_GetLocalHostIp((stAlert.masterIp));

    // If the IP returned, is NULL
    if ( stAlert.masterIp == NULL )
        strcpy(stAlert.masterIp,"");

    // Get the current date and time string (YYYY:MM:DD:HH:MI:SS)
    Common_GetCurrDateTime(stAlert.dateTime);

    if ( stAlert.dateTime == NULL )
        strcpy(stAlert.dateTime,"");

#ifndef WIN32
    uname(&sys);

    if ( sys.nodename != NULL )
        strcpy(acSysName , sys.nodename);
    else
        strcpy(acSysName , "Unknown Host");

#else
    WSAStartup(MAKEWORD( 2, 2 ),&stWsData);

    //This Statement can be unsucessful at times.
    gethostname(sys,MAX_NAME_LEN);

    if ( sys != NULL )
        strcpy(acSysName , sys);
    else
        strcpy(acSysName , "Unknown Host");
#endif

    // Prepare the stAlert message and pass it to
    // Common_SendMesg function
    strcpy(stAlert.modCode, pcModCode);
    strcpy(stAlert.sysName,acSysName);

    if ( pcMesg != NULL )
        strcpy(stAlert.alertMesg,pcMesg);
    else
        strcpy(stAlert.alertMesg,"");

    stAlert.index = ulRowIndex;

	// Now, send this stAlert pcMesg over UDP socket
	/*
    if ( Common_SendMesg(stAlert) == -1 )
	*/
    if ( Common_SendMesg_And_Vars(stAlert,varBinds,var_cnt) == -1 )
	{
		snmp_log(LOG_ERR,"UDP pcMesg sending to manager failed\n");
		return(1);
	}
	else
        DEBUGMSGTL(("Common/Common_SendAlert",
            "Thr #%d# Trap Mesg sent to the Alert Manager\n",
            MY_THR_ID));
	/** rewrite by D.J Park in 2004.1.13
	 */
	pthread_mutex_unlock(&sendTrapLock);

	return (0);
}

int
Common_SendMesg_And_Vars(nuriAlert stAlert,stVarBindArgProc *varBinds,int var_cnt)
{
    struct sockaddr_in  stSrvName = {0};
    struct hostent      *pstHostEnt = NULL;
    stVarBindArgProc    args = { 0 };
    char                aAlertMesg[sizeof(nuriAlert) + 5*sizeof(ALERT_SEPARATOR)] = {0};
    char                aBuffer[MAX_HOST_IP_LEN +1] = { 0 };
    char                acTrapOid[]=".1.3.6.1.4.1.3204.1.3.35.2.1.1.16";

    if ( strcmp(stAlert.modCode,"PSMN") == 0 )
        strcpy(acTrapOid,".1.3.6.1.4.1.3204.1.3.35.3.1.11");
    else if ( strcmp(stAlert.modCode,"LGRM") == 0 )
        strcpy(acTrapOid,".1.3.6.1.4.1.3204.1.3.35.2.2.1.7");
    else if ( strcmp(stAlert.modCode,"LGCM") == 0 )
        strcpy(acTrapOid,".1.3.6.1.4.1.3204.1.3.35.2.1.1.16");
    else if ( strcmp(stAlert.modCode,"FLMN") == 0 )
        strcpy(acTrapOid,".1.3.6.1.4.1.3204.1.3.35.7.1.11"); /* alert msg */
    else if ( strcmp(stAlert.modCode,"PTMN") == 0 )
        strcpy(acTrapOid,".1.3.6.1.4.1.3204.1.3.35.8"); /* alert msg */
    else if ( strcmp(stAlert.modCode,"LGCL") == 0 )
        strcpy(acTrapOid,".1.3.6.1.4.1.3204.1.3.35.9"); /* description */

    // Form trap message argument
/*
 * modified hhyoon 2004.1.9
    sprintf (args.variable, "%s.%lu", acTrapOid, stAlert.index);
 */
    sprintf (args.variable, "%s",acTrapOid);
    args.type = 's';

/*
 * modified hhyoon 2004.1.9
    sprintf(aAlertMesg,"%s%s%s%s%s%s%s%s%s\n",
               stAlert.modCode,
               ALERT_SEPARATOR,
               stAlert.masterIp,
               ALERT_SEPARATOR,
               stAlert.dateTime,
               ALERT_SEPARATOR,
               stAlert.sysName,
               ALERT_SEPARATOR,
               stAlert.alertMesg);
 */

	sprintf(aAlertMesg, "%s\n", stAlert.alertMesg);

    strcpy (args.value, aAlertMesg);

    // Change the Ip from ASN format to character string
    getIPFromASNFormat(gcAgentInfoAlertMgrIp, aBuffer);

    DEBUGMSGTL(("Common/Common_SendMesg",
        "Thr #%d# trap Dest IP=%s port=%d\n",
        MY_THR_ID,
        aBuffer,
        glAgentInfoAlertMgrPort));

    if ( glAgentInfoAlertMgrPort <= 0 )
    {
        snmp_log(LOG_ERR,"Common_SendMesg: Invalid Alert Manager port number <%d>\n",
            glAgentInfoAlertMgrPort);
        return (-1);
    }

    // Send the trap message to Alert Manager
	/*
    Common_SendSnmptrap (1, &args, aBuffer, glAgentInfoAlertMgrPort,stAlert.modCode);
	*/
    Common_SendSnmptrap (var_cnt, varBinds, aBuffer, glAgentInfoAlertMgrPort,stAlert.modCode);

    return(0);
}

/*****************************************************************************
 * name             :   Common_SnmpInput
 * description      :   This is a callback function for sending trap
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :
 *****************************************************************************/

static int
Common_SnmpInput(int operation, netsnmp_session * session,
            int reqid, netsnmp_pdu *pdu, void *magic)
{
    /* No additional processing is required, just returning */
    return 1;
}

/*****************************************************************************
 * name             :   Common_SendSnmptrap
 * description      :   To send snmp trap message to Manager
 * input parameters :
 * output parameters:
 * return type      :   int
 * global variables :
 * calls            :
 *****************************************************************************/

int
Common_SendSnmptrap (int length, stVarBindArgProc *args, char *szDestIP, int iPort, char *modCode)
{
    void            *ss = NULL;
    netsnmp_session session = { 0 };
    netsnmp_pdu     *pdu = NULL;
    oid             name[MAX_OID_LEN] = { 0 };
    size_t          name_length = 0;
    struct in_addr  laddr = { 0 };
    int             status = 0;
    int             exitval = 0;
    int             cnt = 0;
    char            *trap = NULL;
    char            acNuriOid[]=".1.3.6.1.4.1.3204.1.3.35.2.2";

    /*
     * initialize session to default values
     */
    snmp_sess_init (&session);

    /* Initialize session values - DUMMY */
    session.version = SNMP_VERSION_2c;
    session.timeout = 1 * 1000000L;
    session.retries = 3;
    session.community = (unsigned char *) "public";
    session.community_len = strlen (session.community);

    if (szDestIP)
    {
        session.peername = szDestIP;
    }

    if ( strcmp(modCode,"PSMN") == 0 )
        strcpy(acNuriOid,".1.3.6.1.4.1.3204.1.3.35.3");
    else if ( strcmp(modCode,"LGRM") == 0 )
        strcpy(acNuriOid,".1.3.6.1.4.1.3204.1.3.35.2.2");
    else if ( strcmp(modCode,"LGCM") == 0 )
        strcpy(acNuriOid,".1.3.6.1.4.1.3204.1.3.35.2.1");
    else if ( strcmp(modCode,"FLMN") == 0 )
        strcpy(acNuriOid,".1.3.6.1.4.1.3204.1.3.35.7");
    else if ( strcmp(modCode,"PTMN") == 0 )
        strcpy(acNuriOid,".1.3.6.1.4.1.3204.1.3.35.8"); /* alert msg */
    else if ( strcmp(modCode,"LGCL") == 0 )
        strcpy(acNuriOid,".1.3.6.1.4.1.3204.1.3.35.9"); /* description */

    DEBUGMSGTL(("Common",
        "Thr #%d# peername=%s trapPort=%ld\n",
        MY_THR_ID,
        session.peername,
        iPort));

    SOCK_STARTUP;

    session.callback = Common_SnmpInput;
    session.callback_magic = NULL;

    if (iPort == 0)
    {
        DEBUGMSGTL(("Common",
            "Thr #%d# port value is zero for iPort=%lu",
            MY_THR_ID,
            iPort));
    }

    /* Assign the Remote port number in the session structure */
    netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_DEFAULT_PORT,
               (int) iPort);

    if (session.version == SNMP_VERSION_3) {
    /*
     * for traps, we use ourselves as the authoritative engine
     * which is really stupid since command line apps don't have a
     * notion of a persistent engine.  Hence, our boots and time
     * values are probably always really wacked with respect to what
     * a manager would like to see.
     *
     * The following should be enough to:
     *
     * 1) prevent the library from doing discovery for engineid & time.
     * 2) use our engineid instead of the remote engineid for
     * authoritative & privacy related operations.
     * 3) The remote engine must be configured with users for our engineID.
     *
     * -- Wes
     */

    /*
     * setup the engineID based on IP addr.     Need a different
     * algorthim here.  This will cause problems with agents on the
     * same machine sending traps.
     */
    setup_engineID(NULL, NULL);

    /*
     * pick our own engineID
     */
    if (session.securityEngineIDLen == 0 ||
        session.securityEngineID == NULL) {
        session.securityEngineID =
        snmpv3_generate_engineID(&session.securityEngineIDLen);
    }
    if (session.contextEngineIDLen == 0 ||
        session.contextEngineID == NULL) {
        session.contextEngineID =
        snmpv3_generate_engineID(&session.contextEngineIDLen);
    }

    /*
     * set boots and time, which will cause problems if this
     * machine ever reboots and a remote trap receiver has cached our
     * boots and time...  I'll cause a not-in-time-window report to
     * be sent back to this machine.
     */
    if (session.engineBoots == 0)
        session.engineBoots = 1;
    if (session.engineTime == 0)    /* not really correct, */
        session.engineTime = get_uptime();    /* but it'll work. Sort of. */
    }

    ss = snmp_sess_open(&session);
    if (ss == NULL) {
        /*
         * diagnose snmp_open errors with the input netsnmp_session pointer
         */
        snmp_sess_perror("snmptrap", &session);
        DEBUGMSGTL (("Common",
            "Thr #%d# snmp_open failed. errno=%d\n",
            MY_THR_ID,
            errno));

        SOCK_CLEANUP;
        return 1;
    }

    if (session.version == SNMP_VERSION_1) {
        DEBUGMSGTL(("Common",
            "Thr #%d# SNMP_VERSION_1 trap is not supported by this appln.\n",
            MY_THR_ID));
        return 1;
    } else {
    long        sysuptime;
    char        csysuptime[20];

    pdu = snmp_pdu_create(SNMP_MSG_TRAP2);

#ifdef WIN32
    sysuptime = RTM_Get_Uptime_Win ();
#else
    sysuptime = get_uptime();
#endif
    sprintf(csysuptime, "%ld", sysuptime);
    trap = csysuptime;

    if (snmp_add_var(pdu, objid_sysuptime,
             sizeof(objid_sysuptime) / sizeof(oid), 't', trap)!= 0) {
        // snmp_perror("objid_sysuptime");

        DEBUGMSGTL (("Common",
            "Thr #%d# snmp_add_var failed for "
            "objid_sysuptime\n",
            MY_THR_ID));

        SOCK_CLEANUP;

        return 1;
       }

    if (snmp_add_var
        (pdu, objid_snmptrap, sizeof(objid_snmptrap) / sizeof(oid),
         'o', acNuriOid) != 0) {
        // snmp_perror("objid_snmptrap");

        DEBUGMSGTL (("Common",
            "Thr #%d# snmp_add_var failed for "
            "objid_snmptrap\n",
            MY_THR_ID));
       }

    }

    name_length = MAX_OID_LEN;

    for (cnt = 0; cnt < length; cnt++)
    {
        DEBUGMSGTL (("Common",
            "Thr #%d# args[%d].variable=%s type=%c value=%s\n",
            MY_THR_ID,
            cnt,
            args[cnt].variable,
            args[cnt].type,
            args[cnt].value));

        if (!snmp_parse_oid(args[cnt].variable, name, &name_length)) {
            DEBUGMSGTL(("Common",
                "Thr #%d# * trap argument oid problem....\n",
                MY_THR_ID));

            SOCK_CLEANUP;
            return 1;
        }

        if (snmp_add_var
            (pdu, name, name_length, args[cnt].type, args[cnt].value) != 0) {
           DEBUGMSGTL(("Common",
                "Thr #%d# cnt=%d name=%s namelen=%d "
                "SNMPTRAP arg. type and value problem\n",
                MY_THR_ID,
                cnt,
                name,
                name_length));

           SOCK_CLEANUP;
           return 1;
        }
    }

    status = snmp_sess_send (ss, pdu) == 0;
    if (status) {
        snmp_sess_perror("snmptrap", ss);
        snmp_free_pdu(pdu);
        exitval = 1;
    }

    snmp_sess_close(ss);
    SOCK_CLEANUP;
    return exitval;
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
    /**
     * modify by D.J Park in 2004.02.26
    int fds=0;
    char **psCmd; // For command and its arguments

    DEBUGMSGTL(("ProcMon/ProcMon_RestartProcess",
        "Thr #%d# Called\n",
        MY_THR_ID));
    psCmd[0] = psProcName;

    // ( Kept for future enhancement )
    psCmd[1] = 0; // NULL terminated arguments string
	*/

    int fds=0;
	char reactionPath[255];
	char *psCmd[20] = {0,};
	int idx = 0;
	memset(reactionPath,0x00,sizeof(reactionPath));
	strcpy(reactionPath,psProcName);
	getSeperatedCmd(reactionPath,psCmd);

    // Ignore the signal from the child
    Common_SignalIgnore(SIGCHLD);

    // Fork the process in child
    if( (iPid = fork() ) == 0)  // Child process
    {

        // Close all the parent
        // File descriptors
        fds=MIN_PARENT_FD;
        for(fds=3;fds<MAX_PARENT_FD;fds++)
        {
            close(fds);
        }

        // Ignore all the signals from the parent
        Common_SignalIgnore(IGNORE_ALL_SIG);

        // Execute the command
        if ( (execv(psCmd[0],psCmd)) != 0 )
        {

            DEBUGMSGTL(("ProcMon/ProcMon_RestartProcess",
                "Thr #%d# Process <%s> could not be restarted\n",
                MY_THR_ID,
                psCmd[0]));

            // Starting failed, so exit the child
            _exit(1);
        }
    }
	while(psCmd[idx] != NULL)
		free(psCmd[idx++]);

#else
    STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;
	BOOL bResult;
	TCHAR psExecutable[_MAX_PATH] = {0,};
	TCHAR psParameter[_MAX_PATH] = {0,};
	
    ZeroMemory( &sInfo, sizeof(sInfo) );
    sInfo.cb = sizeof(sInfo);

//	bResult = CreateProcess((LPTSTR) psProcName,NULL,NULL,NULL,
	bResult = CreateProcess(NULL,psProcName,NULL,NULL,
                FALSE,0,NULL,NULL,&sInfo,&pInfo);
    iPid = pInfo.dwProcessId;
    CloseHandle(pInfo.hProcess);
#endif
    DEBUGMSGTL(("ProcMon/ProcMon_RestartProcess",
        "Thr #%d# Returning....\n",
        MY_THR_ID));

    return(iPid);
}


/*****************************************************************************
 *
 * name             :   ProcMon_CheckProc
 * description      :   This function is used to send the alert pcMesg to the
 *                      manager
 * input parameters :   char *name- Process Name
 *                      char *pcMesg - Alert message
 * output parameters:   None
 * return type      :   int - returns success (0) or failure (1)
 * global variables :   None
 * calls            :   None
 */

int
/** modified by D.J Park in 2004.10.15 */
//ProcMon_CheckProc(char *pcProcName, char *parameter)
ProcMon_CheckProc(psMonitorTable_context *pstMycontext)
{
	ps_cache_info *pstPsInfo = NULL;
	ps_cache_info *pstPsList = NULL;
    char *pcProcName = NULL; 
    char *parameter = NULL; 
	int iIndex = 0;
	/** added by HJ.Kwon */
	int iCount = 0;

    pcProcName = pstMycontext->psMonitorName;
    parameter = pstMycontext->psMonitorParameter;
	
    if (gpstPsInfo == NULL) {
    	return -99;
    }

	/** shkim 2008.04.01 */
	if (full_process_list == 0) {
		return -99;
	}
    	            
	pstPsList = gpstPsInfo;
	
	if (parameter[0] == ' ')
		parameter++;
		
	for (;pstPsList != NULL; pstPsList = pstPsList->next) {
		if ( strcmp(pstPsList->name, pcProcName) == 0 ) {
			if (strlen(parameter)==0) {
				
        		// Do the action here which is specified for
	            // this process
	            DEBUGMSGTL(("ProcMon/ProcMon_CheckProc",
	                "Thr #%d# Process Found <%s %s>\n",
	                MY_THR_ID,
	                pstPsList->name,
	                pcProcName));
                /** added by D.J Park in 2004.10.15 */
                checkThreshold(pstPsList,pstMycontext);
				iCount++;
        	} else {
        		if (strlen(pstPsList->args) > 0 ) 
        		{	
        			/*
        			snmp_log(LOG_ERR, "ProcMon/ProcMon_CheckPattern psname:%s," 
                        " args:%s, pattern:%s\n", pcProcName, 
                        pstPsList->args, parameter);
        			*/
	        		if (ProcMon_CheckPattern(pstPsList->args, parameter))
	        		{	        			
	        			// Do the action here which is specified for
			            // this process
			            DEBUGMSGTL(("ProcMon/ProcMon_CheckProc",
			                "Thr #%d# Process Found <%s %s>\n",
			                MY_THR_ID,
			                pstPsList->name,
			                pcProcName));
						iCount++;
                        /** added by D.J Park in 2004.10.15 */
                        checkThreshold(pstPsList,pstMycontext);
	        		}
	        	}
        	}   
		}
	} //end of for-loop 
	return iCount;    
}

#if defined(WIN32)

/*****************************************************************************
 *
 * name             :   conversion
 * description      :   This function is used to get the case changed
 * input parameters :   char *buff - the ouput
 *                      char *process - The proccess name
 * output parameters:   None
 * return type      :   None
 * global variables :   None
 * calls            :   None
 */

void
conversion(char * buff, char *ProcessName)
{
    int count;
    int i;
    char charValue[2];


    count = strlen(ProcessName);
    memset(buff,0,MAX_PATH);

    for(i=0;i<count;i++)
    {
        sprintf(charValue,"%c",toupper(ProcessName[i]));
        strcat(buff,charValue);
    }

    return buff;
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

#ifndef WIN32

// For unix flavors
void
ProcMon_LoadContainerFromFile()
{
    int iFd=0;
    int iFdTmp=0;
    int iCnt=0;
    int iRewriteFlag=0;
    char acStrOid[MAX_OID_LEN] = { 0 };
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
        DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            PROC_MON_DATA_FILE));

        while (read (iFd, &stReadCtx, sizeof(psMonitorTable_context)) > 0)
        {
            // If the row, is already destroyed, no need to create it again.
            if ( stReadCtx.psMonitorStatus == SNMP_ROW_DESTROY )
            {
                iRewriteFlag = 1;   // to re-write the mib data file
                continue; // As we have to load the container
            }


            // Otherwise
            DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                "Loading MIB Row for Proc Name <%s>\n",
                stReadCtx.psMonitorName));

            pstMyContext =
             (psMonitorTable_context*)malloc (sizeof(psMonitorTable_context));

            if ( !pstMyContext )
            {
                snmp_log_perror("ProcMon: Malloc failed");
                return;
            }

            memcpy (pstMyContext, &stReadCtx, sizeof (psMonitorTable_context));

            netsnmp_assert(pstMyContext->index.oids == NULL);

            if(snmp_clone_mem( (void*)&pstMyContext->index.oids,
                            &(stReadCtx.idx),
                            stReadCtx.index.len * sizeof(oid) ))
            {
                free(pstMyContext);
                close(iFd);

                DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    PROC_MON_DATA_FILE));

                return;
            }

            pstMyContext->index.len = stReadCtx.index.len;

            iCnt = CONTAINER_INSERT (cb.container, pstMyContext);

            if (iCnt)
            {
                DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                "ERROR IN CONTAINER_INSERT FUNCION\n"));
            }

            DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                "OID in ProcMon_LoadContainerFromFile <%u>\n",
                *(pstMyContext->index.oids)));

        }

        if ( iRewriteFlag )
        {

            DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                "Rewriting the data file\n"));

            // open a new temporary file
            if ( ( iFdTmp = open (PROC_MON_DATA_FILE_TMP,
                                             O_CREAT| O_RDWR) ) == -1 )
            {
                snmp_log_perror(
                    "PROC_MON_DATA_FILE_TMP file open failed\n");
            }
            else
            {

                DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                    "Thr #%d# File <%s> opened\n",
                    MY_THR_ID,
                    PROC_MON_DATA_FILE_TMP));

                // Reset the file pointers to begining of the files
                lseek(iFd, 0,SEEK_SET);
                lseek(iFdTmp, 0,SEEK_SET);

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

                // Close the file
                close (iFdTmp);

                DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                    "Thr #%d# File <%s> closed\n",
                    MY_THR_ID,
                    PROC_MON_DATA_FILE_TMP));


            } // End of else

            // Delete the old file
            unlink(PROC_MON_DATA_FILE);

            // rename the temp file to PROC_MON_DATA_FILE
            rename(PROC_MON_DATA_FILE_TMP,PROC_MON_DATA_FILE);

        } // End of rewrite if

        // Close the file
        close (iFd);

        DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
            "Thr #%d# File <%s> closed\n",
            MY_THR_ID,
            PROC_MON_DATA_FILE));


        DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
            "CONTAINER SIZE=%u\n",
            CONTAINER_SIZE (cb.container)));

    } // End of else

    // Don't do this !! this will delete the last row
    // from the container
    //free(pstMyContext);

}

/*****************************************************************************
 *
 * name             :   ProcMon_CheckPattern
 * description      :   This function returns 1 if the pattern is found in string
 *                      else returns 0.
 * input parameters :   char *szString, char *szPattern
 * output parameters:   None
 * return type      :   int - "0" for pattern not found,
 *                          - "1" for pattern found
 * global variables :   None
 * calls            :   None
 */

int ProcMon_CheckPattern(char *szString, char *szPattern) {
    int iPatternFound=0;
    int iPatternCompileResult=0;
    regex_t pstReg;
    regmatch_t stMatches[1];
    DEBUGMSGTL(("ProcMon", "Inside ProcMon_CheckPattern string:%s pattern:%s\n", szString, szPattern));
    if ((iPatternCompileResult = regcomp(&pstReg, szPattern, REG_EXTENDED)) == 0) {
	    if(regexec(&pstReg, szString, 1, stMatches,0)==0) {
	        DEBUGMSGTL(("ProcMon", "Inside ProcMon_CheckPattern found match\n"));
	        iPatternFound=1;
	    }
	    
	    regfree(&pstReg);	    
	} else {
		DEBUGMSGTL(("ProcMon", "Inside ProcMon_CheckPattern couldn't compile\n"));
	}
	
    return iPatternFound;
}

int ProcMon_CompilePattern(char *szPattern) {
	int iPatternCompileResult=0;
	regex_t pstReg;
	
	if ((iPatternCompileResult = regcomp(&pstReg, szPattern, REG_EXTENDED)) == 0) {
		regfree(&pstReg);
	}
	return iPatternCompileResult;
}

#else

// For Windows
void
ProcMon_LoadContainerFromFile()
{
    HANDLE  hFd = NULL;
    HANDLE  hFdTmp = NULL;
    DWORD NumberOfBytesRead;
    int iCnt=0;
    int iIdx=0;
    int iRewriteFlag=0;
    char acStrOid[MAX_OID_LEN] = { 0 };
    psMonitorTable_context stReadCtx = {0};
    psMonitorTable_context *pstMyContext = NULL;

    // Open the Data file
    hFd = CreateFile   (PROC_MON_DATA_FILE,
                        GENERIC_READ,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);


    if (hFd == INVALID_HANDLE_VALUE)
    {
        snmp_log_perror("ProcMon_LoadContainerFromFile: \
            PROC_MON_DATA_FILE file open failed\n");
    }
    else
    {

        DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            PROC_MON_DATA_FILE));

        iIdx=0;

        while(ReadFile(hFd, &stReadCtx, sizeof (psMonitorTable_context),
                &NumberOfBytesRead, NULL))
        {
          
            snmp_log(LOG_ERR,"Size <%d> <%d>\n",NumberOfBytesRead,
                sizeof (psMonitorTable_context));
            
            if (NumberOfBytesRead== 0)
                {
                 snmp_log(LOG_ERR," %d breaking out \n",GetLastError());
                break;
                }
            


            // If the row, is already destroyed, no need to create it again.
            if ( stReadCtx.psMonitorStatus == SNMP_ROW_DESTROY )
            {
                iRewriteFlag = 1;   // to re-write the mib data file
                continue; // As we have to load the container anyways
            }


            // Otherwise
            DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                "Loading MIB Row for Proc Name <%s>\n",
                stReadCtx.psMonitorName));

            pstMyContext =
             (psMonitorTable_context*)malloc (sizeof(psMonitorTable_context));
            if ( !pstMyContext )
            {
                snmp_log_perror("ProcMon: Malloc failed");
                return;
            }

            memcpy (pstMyContext, &stReadCtx, sizeof (psMonitorTable_context));

            netsnmp_assert(pstMyContext->index.oids == NULL);

            if(snmp_clone_mem( (void*)&pstMyContext->index.oids,
                            &(stReadCtx.idx),
                            stReadCtx.index.len * sizeof(oid) ))
            {
                free(pstMyContext);
                CloseHandle(hFd);
                return;
            }

            pstMyContext->index.len = stReadCtx.index.len;

            iCnt = CONTAINER_INSERT (cb.container, pstMyContext);

            if (iCnt)
            {
                DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                "ERROR IN CONTAINER_INSERT FUNCION\n"));
            }

            DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                "OID in ProcMon_LoadContainerFromFile <%u>\n",
                *(pstMyContext->index.oids)));

        }

        if ( iRewriteFlag )
        {

            DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                "Rewriting the data file\n"));

            hFdTmp = CreateFile(PROC_MON_DATA_FILE_TMP,
                                GENERIC_READ|GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

            if (hFdTmp == INVALID_HANDLE_VALUE)
            {
                snmp_log_perror(
                    "PROC_MON_DATA_FILE_TMP file open failed\n");
            }
            else
            {

                DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                    "Thr #%d# File <%s> opened\n",
                    MY_THR_ID,
                    PROC_MON_DATA_FILE_TMP));

                // Reset the file pointers to begining of the files
                SetFilePointer(hFd,0,NULL,FILE_BEGIN);
                SetFilePointer(hFdTmp,0,NULL,FILE_BEGIN);

                iIdx=0;

                
		while(ReadFile(hFd, &stReadCtx, sizeof (psMonitorTable_context),
                        &NumberOfBytesRead, NULL))
                {
                    
                        if (NumberOfBytesRead== 0)
                        {
                         snmp_log(LOG_ERR," %d breaking out \n",GetLastError());
                        break;
                        }
            
                     if ( stReadCtx.psMonitorStatus != SNMP_ROW_DESTROY )

                        if( WriteFile(hFdTmp, &stReadCtx,
                                    sizeof(psMonitorTable_context),
                                    &NumberOfBytesRead, NULL) == 0 )
                        {
                            snmp_log_perror(("Error while writing \
                                    data to PROC_MON_DATA_FILE"));
                        }
                        

                } // End of while

                // Close the file
                CloseHandle(hFdTmp);

            } // End of else

            // Delete the old file
            unlink(PROC_MON_DATA_FILE);

            // rename the temp file to PROC_MON_DATA_FILE
            rename(PROC_MON_DATA_FILE_TMP,PROC_MON_DATA_FILE);

        } // End of rewrite if

        DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
            "CONTAINER SIZE=%u\n",
            CONTAINER_SIZE (cb.container)));

    } // End of else

    // Close the file
    CloseHandle(hFd);

    // Don't do this !! this will delete the last row
    // from the container
    //free(pstMyContext);

}


static HMODULE hMod = NULL;
static const LPTSTR REGEXP_DLL_PATH = TEXT("vbregexp.dll");

static PFNPATTERN GetFuncPtr_Pattern()
{

   PFNPATTERN pPattern = NULL;

   // Load SHFolder.dll only once
   if (!hMod)
      hMod = LoadLibrary(REGEXP_DLL_PATH);

   if (hMod)
      // Obtain a pointer to the SHGetFolderPathA function
      pPattern= (PFNPATTERN)GetProcAddress(hMod, 
         "PatternForString");

   return pPattern;
} 

static PFNCHECKEXISTVBSCRIPT GetFuncPtr_CheckExistVBScript()
{
	PFNCHECKEXISTVBSCRIPT pFunc= NULL;

   // Load SHFolder.dll only once
   if (!hMod)
      hMod = LoadLibrary(REGEXP_DLL_PATH);

   if (hMod)
      // Obtain a pointer to the SHGetFolderPathA function
      pFunc = (PFNCHECKEXISTVBSCRIPT)GetProcAddress(hMod, 
         "CheckExistVBScript");

   return pFunc;
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

int ProcMon_CheckPattern(char *value, char *lpszPattern) {
    int iPatternFound=0;
	int nFound;

	DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain/ProcMon_CheckPattern", "Inside ProcMon_CheckPattern string:%s pattern:%s\n", value, lpszPattern));
	// regular expression을 사용하지 못할 경우
	if(pPattern == NULL)
	{
		DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain/ProcMon_CheckPattern","Cannot load CheckPattern function and replace strstr to match pattern.\n"));
		
			if(strstr(value, lpszPattern) != NULL)
				iPatternFound = 1;
			else
				iPatternFound = 0;
			return iPatternFound;
	}

	nFound = pPattern(lpszPattern, value);
	
	if(nFound == REGEXP_MATCHPATTERN)
	{
		DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain/ProcMon_CheckPattern","Found matched pattern\n[\"%s\":\"%s\"].\n",lpszPattern, value));
		iPatternFound = 1;
	}
	else if(nFound == REGEXP_INITERROR)
	{
		DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain/ProcMon_CheckPattern","Cannot load CheckPattern function and replace strstr to match pattern.\n[\"%s\":\"%s\"].\n",lpszPattern, value));
		iPatternFound = -1;
	}
	else if(nFound == REGEXP_NOTSUPPORTPATTERN)
	{
		DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain/ProcMon_CheckPattern","Target string to find pattern is invalid or empty.\n[\"%s\":\"%s\"].\n",lpszPattern, value));
		iPatternFound = -1;
	}
	else if(nFound == REGEXP_NOTMATCHPATTERN)
	{
		DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain/ProcMon_CheckPattern","Not found matched pattern.\n[\"%s\":\"%s\"].\n",lpszPattern, value));
		iPatternFound = -1;
	}
	else if(nFound == REGEXP_INVALIDOPERATION)
	{
		DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain/ProcMon_CheckPattern","Invalid Operation to find pattern.\n[\"%s\":\"%s\"].\n",lpszPattern, value));
		iPatternFound = -1;
	}
	else
	{
		DEBUGMSGTL(("ProcMon/ProcMon_ProcMonThreadMain/ProcMon_CheckPattern","Occured undefined error.\n[\"%s\":\"%s\"].\n",lpszPattern, value));
		iPatternFound = -1;
	}

    return iPatternFound;
}

#endif

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

#ifndef WIN32

// Function for Unix flavors
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
        snmp_log_perror("Error opening PROC_MON_DATA_FILE");
        return(-1);
    }
    else
    {
        DEBUGMSGTL(("ProcMon/ProcMon_UpdateDataFile",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            PROC_MON_DATA_FILE));

        lseek(iFd, 0,SEEK_SET);

        if ( pstProcMonRow->psMonitorStatus == SNMP_ROW_DESTROY )
            pstProcMonRow->iThreadActiveFlag = 0;

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
                    DEBUGMSGTL(("ProcMon/ProcMon_UpdateDataFile",
                        "Thr #%d# OID <%u> found FoundFlag <%d>\n",
                         MY_THR_ID,
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
            snmp_log_perror("Error while writing data to PROC_MON_DATA_FILE");

            DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
                "Thr #%d# File <%s> closed\n",
                MY_THR_ID,
                PROC_MON_DATA_FILE));

            close(iFd);
            return(-1);
        }
        else
            DEBUGMSGTL(("ProcMon/ProcMon_UpdateDataFile",
                "Thr #%d# Record written successfully to the data file <%d>\n",
                MY_THR_ID,
                pstProcMonRow->iThreadActiveFlag));
    }

    // Close file
    close(iFd);

    DEBUGMSGTL(("ProcMon/ProcMon_LoadContainerFromFile",
        "Thr #%d# File <%s> closed\n",
        MY_THR_ID,
        PROC_MON_DATA_FILE));

    return(iFoundFlag);
}
#else

// Function for windows
int
ProcMon_UpdateDataFile(psMonitorTable_context *pstProcMonRow)
{
    psMonitorTable_context stProcInFile;
    HANDLE hFd = NULL;
    DWORD NumberOfBytesRead;
    int iFoundFlag=0;
    int iRowNum=0;
    unsigned long ulPos=0;
    static int iashtemp;

    // Open the MIB data file
    hFd = CreateFile   (PROC_MON_DATA_FILE,
                        GENERIC_READ|GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    if(hFd == INVALID_HANDLE_VALUE)
    {
        snmp_log_perror("Error opening PROC_MON_DATA_FILE");
        return(-1);
    }
    else
    {
        DEBUGMSGTL(("ProcMon/ProcMon_UpdateDataFile",
            "Thr #%d# File <%s> opened\n",
            MY_THR_ID,
            PROC_MON_DATA_FILE));


        SetFilePointer(hFd,0,NULL,FILE_BEGIN);
    
        if ( pstProcMonRow->psMonitorStatus == SNMP_ROW_DESTROY )
            pstProcMonRow->iThreadActiveFlag = 0;

        while (ReadFile(hFd, &stProcInFile, sizeof (psMonitorTable_context),
                &NumberOfBytesRead, NULL))
            {
                if (NumberOfBytesRead== 0)
                {
                    snmp_log(LOG_ERR," %d breaking out \n",GetLastError());
                    break;
                }

            if ( stProcInFile.idx == *pstProcMonRow->index.oids )
            {

                // Check if the row entry in the file
                // is actually deleted fromthe container
                if ( stProcInFile.psMonitorStatus == SNMP_ROW_DESTROY )
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
                    DEBUGMSGTL(("ProcMon/ProcMon_UpdateDataFile",
                        "Thr #%d# OID <%u> found FoundFlag <%d>\n",
                        MY_THR_ID,
                        *pstProcMonRow->index.oids,
                        iFoundFlag));

                }
                break;
            }

            ulPos = SetFilePointer(hFd,0,NULL,FILE_CURRENT);
            
        }

        // Copy the oid, instead copying the pointer
        pstProcMonRow->idx = *(pstProcMonRow->index.oids);

        if ( WriteFile (hFd,
                        pstProcMonRow,
                        sizeof(psMonitorTable_context),
                        &NumberOfBytesRead,
                        NULL) == 0 )
        {
            snmp_log_perror("Error while writing data to PROC_MON_DATA_FILE");
            DEBUGMSGTL(("ProcMon/ProcMon_UpdateDataFile",
                "Thr #%d# File Closed\n",
                MY_THR_ID));
            CloseHandle(hFd);
            return(-1);
        }
        else
            DEBUGMSGTL(("ProcMon/ProcMon_UpdateDataFile",
                "Thr #%d# Record written successfully to the data file\n",
                MY_THR_ID));
    }

    CloseHandle(hFd);
    return(iFoundFlag);
}

#endif

/*****************************************************************************
 *
 * name             :   Common_SendFileThread
 * description      :   Call back for sending Log file to Manager
 * input parameters :   unsigned int guiReg - Registered Id
 *                      void *pcInfoLogFile - Log file to be transferred
 * output parameters:   void
 * return type      :   void
 * global variables :   (extern)glAgentInfoFileTrPort
 *                      (extern)gcAgentInfoFileTransferIP
 * calls            :
 */


void
Common_SendFileThread(unsigned int guiReg, void *pcInfoLogFile)
{
    pthread_t            stThr;

    // If the file transfer is in progress
    // return from here itself
    if ( giFileTransInProgress == 1 )
    {
        DEBUGMSGTL(("Common/Common_SendFileThread",
            "File transfer already in progress, so returning\n"));

	return;
    } // Else create the thread and set the Transfer flag to 1
    else
    {

        if ( pthread_create(&stThr, NULL,
                Common_SendLogFileToManager,
            (void*)pcInfoLogFile) != 0 )
        {
            snmp_log_perror("Common_SendFileThread:Thread creation failed");
        }
        else
        {
            giFileTransInProgress = 1;

            DEBUGMSGTL(("Common/Common_SendFileThread",
                "Send File to manager thread created\n"));
        }
    }

    return;
}


/*****************************************************************************
 *
 * name             :   Common_SendLogFileToManager
 * description      :   Sends Log file to Manager
 * input parameters :   void *pcInfoLogFile - Log file to be transferred
 * output parameters:   void
 * return type      :   void
 * global variables :   (extern)glAgentInfoFileTrPort
 *                      (extern)gcAgentInfoFileTransferIP
 *                      giFileTransInProgress
 * calls            :
 */

void *
Common_SendLogFileToManager (void *pcInfoLogFile)
{
    struct sockaddr_in  sock_addr = {0};
    struct stat         stStat = {0};
    char                acRemoteIP[MAX_HOST_IP_LEN+1]="";
    char                acBakFile[MAXPATHLEN+1]="";
    char                acDateTime[MAX_DATE_TIME_LEN]="";

    pthread_detach(MY_THR_ID);
    Common_GetCurrDateTime(acDateTime);

    DEBUGMSGTL(("Common",
        "Thr #%d# Called at <%s> \n",
        MY_THR_ID,
        acDateTime));

    // Convert the IP addr to string format
    getIPFromASNFormat(gcAgentInfoFileTransferIP, acRemoteIP);

    if ( ((char*)pcInfoLogFile == NULL) || ( strlen((char*)pcInfoLogFile) == 0 ))
    {
        DEBUGMSGTL(("Common",
            "Thr #%d# Invalid file \n",
            MY_THR_ID));

        giFileTransInProgress = 0;
        pthread_exit(NULL);
    }
    else
    {
        DEBUGMSGTL(("Common",
            "Thr #%d# Transferring <%s> \n",
            MY_THR_ID,
            pcInfoLogFile));
    }

    if((glAgentInfoFileTrPort < 0 )||(strlen(acRemoteIP) <= 0))
    {
        DEBUGMSGTL(("Common",
            "Thr #%d# Agent Port or IP is invalid\n",
            MY_THR_ID));

        giFileTransInProgress = 0;
        pthread_exit(NULL);
    }


    // Convert the IP in required format
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons((unsigned short)glAgentInfoFileTrPort);
    sock_addr.sin_addr.s_addr = inet_addr(acRemoteIP);

    // Assumption here is file transfer will complete before next interval
    // or file size limit reached

    if(strlen((char*)pcInfoLogFile) > 0)
    {

        // Check if statistics are valid: '0' - indicates success
        if( stat( pcInfoLogFile, &stStat ) == 0 )
        {
		/* 
            // Transfer the file
            if (FileTrans_SendFile ( (char*)pcInfoLogFile, &sock_addr , NULL , 512)
                                                != SUCCESS )
            {
                DEBUGMSGTL(("Common",
                    "Thr #%d# SendLogFileToManager() Failed to IP <%s> Port <%d>\n",
                    MY_THR_ID,
                    acRemoteIP,
                    glAgentInfoFileTrPort));
            }
            else
            {
                DEBUGMSGTL(("Common",
                    "Thr #%d# SendLogFileToManager() Success ....\n",
                    MY_THR_ID));

                // Now move this file to bak file
                strcpy(acBakFile,(char*)pcInfoLogFile);
                strcat(acBakFile,".bak");

                DEBUGMSGTL(("Common",
                    "Thr #%d# Renaming <%s> to <%s>\n",
                    MY_THR_ID,
                    (char*)pcInfoLogFile,
                    acBakFile));
                // Move the file to bak file
                rename((char*)pcInfoLogFile,acBakFile);

            }
		*/
        }
    }

    DEBUGMSGTL(("Common",
        "Thr #%d# Registering next alarm  for SendLogFileToManager....\n",
	MY_THR_ID));

    // Register an alarm for log file transfer for next iteration
    if ( (glAgentInfoFileTrPort > 0) &&
         (strlen(gcAgentInfoFileTransferIP) > 0))
    {

        if (glAgentInfoFileTrInterval > 0 )
            guiTransLogFileAlarmReg = snmp_alarm_register (glAgentInfoFileTrInterval,
            (int) NULL ,Common_SendFileThread, pcInfoLogFile);
        else
            guiTransLogFileAlarmReg = snmp_alarm_register (1,
            (int) NULL ,Common_SendFileThread, pcInfoLogFile);


    }

    giFileTransInProgress = 0;

    DEBUGMSGTL(("Common",
        "Thr #%d# Exiting SendLogFileToManager....\n",
	MY_THR_ID));
    pthread_exit(NULL);
}


/************************************************************
 * Initializes the psMonitorTable module
 */
void
init_psMonitorTable(void)
{
    /** added by HJ. Kwon. in 2005.07.08. */
#ifdef osf5
    int start;
#endif
    /** added by Corbin in 2004.02.03
     */
    if(pthread_mutex_init(&procMonProcessCheckLock, NULL) != 0)
        snmp_log(LOG_ERR,"psMonitorTable init mutex failed");
		
    initialize_table_psMonitorTable();

/** added by D.J Park in 2005.02.23 [start] */
#if defined(AIX_4_3) 
    cpuNo = sysconf(_SC_NPROCESSORS_ONLN);
/** added by HJ. Kwon. in 2005.07.13. */
#elif defined(sysv4)
    cpuNo = getProcessorNum();
#elif defined(WIN32)
    cpuNo = DetectNumProcessors();
#elif defined(osf5)
    if(getsysinfo(GSI_CPUS_IN_BOX, (caddr_t) &cpuNo, sizeof(cpuNo), &start, 0, 0) < 0)
        cpuNo = 1;
#elif defined(linux)
    cpuNo = getProcessorNum();
#endif
    if (cpuNo < 1)
        cpuNo = 1;
/** end : HJ. Kwon. in 2005.07.13. */

    psMonCpuInfo.ncpus = 1;
    psMonCpuInfo.sampletime = 0;
    psMonCpuInfo.total = 0;
    psMonCpuInfo.state[0] = 0;
    psMonCpuInfo.state[1] = 0;
    psMonCpuInfo.state[2] = 0;
    psMonCpuInfo.state[3] = 0;
    psMonCpuInfo.state[4] = 0;
    psMonCpuInfo.stateUsage[0] = 0;
    psMonCpuInfo.stateUsage[1] = 0;
    psMonCpuInfo.stateUsage[2] = 0;
    psMonCpuInfo.stateUsage[3] = 0;
    psMonCpuInfo.stateUsage[4] = 0;
    psMonCpuInfo.stateUsageFloat[0] = 0;
    psMonCpuInfo.stateUsageFloat[1] = 0;
    psMonCpuInfo.stateUsageFloat[2] = 0;
    psMonCpuInfo.stateUsageFloat[3] = 0;
    psMonCpuInfo.stateUsageFloat[4] = 0;
/** added by D.J Park in 2005.02.23 [end] */

    /*
     * TODO: perform any startup stuff here
     */

    // <Start: WIP_NURI_ALL: Added this code>
    ProcMon_LoadContainerFromFile();

    ProcMon_MasterThread();
    // <End: WIP_NURI_ALL>

    // Register an alarm for log file transfer if
    // not registered already
    if ( (glAgentInfoFileTrPort > 0) &&
         (strlen(gcAgentInfoFileTransferIP) > 0))
    {
        DEBUGMSGTL(("ProcMon/init_psMonitorTable",
            "Registering the alarm for ProcMon Log file transfer\n"));
        if (glAgentInfoFileTrInterval > 0 )
            guiTransLogFileAlarmReg = snmp_alarm_register (glAgentInfoFileTrInterval,
            (int) NULL ,Common_SendFileThread, gcInfoLogFileProc);
        else
            guiTransLogFileAlarmReg = snmp_alarm_register (1,
            (int) NULL ,Common_SendFileThread, gcInfoLogFileProc);
    }
}


/*****************************************************************************
 *
 * name             :   Common_CheckFileSize
 * description      :   Checks if the file is present and the size of the file,
 *                      is not bigger than the max allowed size for transfer, if
 *                      it is, it sends Log file to Manager
 * input parameters :   char *pcInfoLogFile
 * output parameters:   void
 * return type      :   int
 * global variables :   (extern)glAgentInfoLogFileLimitSize
 * calls            :   None
 */

int
Common_CheckFileSize(char *pcInfoLogFile)
{
    struct stat         stStat = {0};

    // Check if file exists and its
    // size has grown more than Max allowed size
    if( stat( pcInfoLogFile, &stStat ) == 0 )
    {

        // If yes, transfer the file first, and
        // then write in to the log file
        if ( stStat.st_size >= (glAgentInfoLogFileLimitSize*1024) )
        {
            DEBUGMSGTL(("Common/Common_CheckFileSize",
                "Thr #%d# Log File size is : <%s>, so transferring to manager... \n",
                MY_THR_ID,
                pcInfoLogFile));

            // Transfer the file
            (void) Common_SendFileThread(0,pcInfoLogFile);
        }
        else
        {
            DEBUGMSGTL(("Common/Common_CheckFileSize",
                "Thr #%d# Log File : <%s> exists but size is lesser than <%ld Kbytes>\n",
                MY_THR_ID,
                pcInfoLogFile,
                glAgentInfoLogFileLimitSize));
        }


        return (0);
    }
    else
    {
        DEBUGMSGTL(("Common",
            "Thr #%d# Log File <%s> does not exist \n",
            MY_THR_ID,
            pcInfoLogFile));

        return(-1);
    }
}

/*****************************************************************************
 *
 * name             :   Common_SignalIgnore
 * description      :   Ignores the specified signal
 * input parameters :   int iSigNum - The signal to be ignored
 * output parameters:   void
 * return type      :   void
 * global variables :   None
 * calls            :   None
 */

void
Common_SignalIgnore(int iSigNum)
{

    if(iSigNum == IGNORE_ALL_SIG) // Ignore all signals
    {
        if (signal(SIGINT, SIG_IGN) == SIG_ERR)
        {
            snmp_log_perror("Common_SignalIgnore: \
                error \"uncatching\" SIGINT\n");
            return;
        }

#ifndef WIN32 // Available only for Unix's
        if (signal(SIGALRM, SIG_IGN) == SIG_ERR)
        {
            snmp_log_perror("Common_SignalIgnore: \
                error \"uncatching\" SIGALRM\n");
            return;
        }
#endif
    }
    else
    {
        if(signal(iSigNum,SIG_IGN) == SIG_ERR)
        {
            snmp_log_perror("Common_SignalIgnore: \
                Error in signal handling\n");
            return;
        }
    }

    return;
}

/**
  * added by D.J Park in 2004.1.17
  * get current time. used to log when trap sended for psMonitor
  * param tbuf : time stamp buffer
  */
void getCurrentTime(char *tbuf)
{
    time_t t;
    struct tm *ptm;

    time(&t);
    ptm = localtime(&t);

    memset(tbuf,0x00,sizeof(tbuf));

    sprintf(tbuf,"%02d-%02d [%02d:%02d:%02d]",ptm->tm_mon+1,ptm->tm_mday,
            ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
}

/**
  * added by D.J Park in 2004.2.26
 * get the process name from reaction path
 * param  reactionPath : reaction path of process monitoring 
 * param  psName : buffer of process name
 */
void getProcessName(char *reactionPath, char *psName)
{
	char *param = NULL;
	int  reactlen = 0;
	int  paramlen = 0;

	memset(psName,0x00,sizeof(psName));

	if((param = strchr(reactionPath,' ')) == NULL)
		strcpy(psName,reactionPath);
	else
	{
		reactlen = strlen(reactionPath);
		paramlen = strlen(param);
		strncpy(psName,reactionPath,(reactlen - paramlen));
	}
}

/**
 * added by D.J Park in 2004.2.26
 * seperate command
 * param com: command(include argument)
 * param arg : buffer of command and argument
 */
void getSeperatedCmd(char *com,char *arg[])
{
	int num=0,len;
	char *ptr;

    ptr=(char *)strtok(com," \n");
	len=strlen(ptr);
	arg[num]=(char *)malloc(len+1);
	strcpy(arg[num++],ptr);

	while((ptr=(char *)strtok(NULL," \n")) != NULL) 
    { 
        len=strlen(ptr);
	    arg[num]=(char *)malloc(len+1);
	    strcpy(arg[num++],ptr);
    }
	
	arg[num]=NULL;
}

/** 
 * added by D.J Park in 2004.10.14
 * get current centi second 
 */
long getcstime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);

  	return ((tv.tv_sec%100000000) * 100) + (tv.tv_usec/10000); 

}

/**
 * added by D.J Park in 2004.10.14
 * caculate process percent cpu
 */
long getPercentCPU(int pid,long time,long cputime,int cpu_count)
{
    long pcpu = 0;
    double diffcpu = 0;
    double difftime = 0;
    ps_cache_info *psinfo = NULL;
    for(psinfo=gpstPsInfo ; psinfo != NULL ; psinfo=psinfo->next)
    {
        if(psinfo->pid == pid)
        {
            diffcpu = (double)cputime - psinfo->cputime;
            difftime = (double)time - psinfo->time;
            pcpu = (long)(diffcpu * 10000 / difftime); 
            pcpu = pcpu/cpu_count;
            break;
        }
    }
    return pcpu;
}

/**
 * added by D.J Parkin 2004.10.15
 * find process infomation previous process infomation same as pid
 * pid : Process ID
 */

ps_cache_info *searchPrePsInfo(int pid)
{
    ps_cache_info *pci = NULL;

    if(gpstPrePsInfo == NULL)
        return NULL;

    for(pci = gpstPrePsInfo ; pci != NULL ; pci=pci->next)
    {
        if(pci->pid == pid)
            return pci;
    }

    return NULL;
}

/**
 * added by D.J Park in 2004.10.15
 * make Trap Variable(Process CPU, Memory Threshold)
 * pstMycontext : process Monitoring Information
 * psinfo : Process Information
 * type : COLUMN_PSMONITORCPU or COLUMN_PSMONITORMEM
 * isAlert : threshold or recovery
 */
stVarBindArgProc *makePsMonitorVarBind_RThreshold(
        psMonitorTable_context *pstMycontext,
        ps_cache_info *psinfo, 
        int type, 
        int isAlert)
{
	stVarBindArgProc* varBinds = (stVarBindArgProc *)NULL;
	int var_cnt = 9;
        int iZero = 0;

	varBinds = (stVarBindArgProc *)malloc(sizeof(stVarBindArgProc) * var_cnt);
	memset(varBinds,0x00,sizeof(stVarBindArgProc) * var_cnt);

	strcpy(varBinds[0].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.2");
	varBinds[0].type='s';
	strcpy(varBinds[0].value,pstMycontext->psMonitorName);

	strcpy(varBinds[1].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.3");
	varBinds[1].type='s';
	strcpy(varBinds[1].value,pstMycontext->psMonitorParameter);

    if(type == COLUMN_PSMONITORCPU)
    {
        if(isAlert == 1)
        {
	        strcpy(varBinds[2].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.6");
	        varBinds[2].type='s';
	        strcpy(varBinds[2].value,"cpu_threshold");
        } else
        {
	        strcpy(varBinds[2].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.6");
	        varBinds[2].type='s';
	        strcpy(varBinds[2].value,"cpu_threshold_recovery");
        }
    } else if(type == COLUMN_PSMONITORMEM)
    {
        if(isAlert == 1)
        {
	        strcpy(varBinds[2].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.6");
	        varBinds[2].type='s';
	        strcpy(varBinds[2].value,"mem_threshold");
        } else
        {
	        strcpy(varBinds[2].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.6");
	        varBinds[2].type='s';
	        strcpy(varBinds[2].value,"mem_threshold_recovery");
        }
    }

	strcpy(varBinds[3].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.11");
	varBinds[3].type='s';
	strcpy(varBinds[3].value,pstMycontext->psMonitorAlertMessage);

	strcpy(varBinds[4].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.14");
	varBinds[4].type='i';
	sprintf(varBinds[4].value,"%ld",pstMycontext->psMonitorCPU);

	strcpy(varBinds[5].variable,".1.3.6.1.4.1.3204.1.3.35.3.1.15");
	varBinds[5].type='i';
	sprintf(varBinds[5].value,"%ld",pstMycontext->psMonitorMEM);

	strcpy(varBinds[6].variable,".1.3.6.1.4.1.3204.1.3.33.1.1.1");
	varBinds[6].type='i';
    if(psinfo == NULL)
        sprintf(varBinds[6].value,"%d", iZero);
    else
        sprintf(varBinds[6].value,"%d",psinfo->pid);

	strcpy(varBinds[7].variable,".1.3.6.1.4.1.3204.1.3.33.1.1.15");
	varBinds[7].type='i';
    if(psinfo == NULL)
        sprintf(varBinds[7].value,"%d", iZero);
    else
        sprintf(varBinds[7].value,"%ld",psinfo->cpu);

	strcpy(varBinds[8].variable,".1.3.6.1.4.1.3204.1.3.33.1.1.9");
	varBinds[8].type='i';
    if(psinfo == NULL)
        sprintf(varBinds[8].value,"%d", iZero);
    else
        sprintf(varBinds[8].value,"%ld",psinfo->mem);

	return varBinds;
}

/**
 * added by D.J Park in 2004.10.15
 * send Trap
 * pstMycontext : process Monitoring Information
 * psinfo : Process Information
 * type : COLUMN_PSMONITORCPU or COLUMN_PSMONITORMEM
 * isAlert : threshold or recovery
 */
void sendThresholdTrap(psMonitorTable_context *pstMycontext, 
        ps_cache_info *psinfo, 
        int type, 
        int isAlert)
{
    stVarBindArgProc* varBinds = NULL; 
    char              tbuf[30] ={0};

/*
    if(psinfo != NULL)
    {
        getCurrentTime(tbuf);
        snmp_log(LOG_INFO,"[%s] sendThresholdTrap:PID[%ld] PNAME[%s] CPU[%ld] "
                " MEM[%ld] TYPE[%d] ISALERT[%d] \n",tbuf,psinfo->pid,
       	         psinfo->name,psinfo->cpu,psinfo->mem,type,isAlert);
    }
*/

    varBinds = 
        makePsMonitorVarBind_RThreshold(pstMycontext,psinfo,type,isAlert);

 	Common_SendAlert_And_Vars("PSMN", gcInfoLogFileProc,
   	    pstMycontext->psMonitorAlertMessage,
        pstMycontext->psMonitorIndex,varBinds,9);

	if(varBinds != NULL)
		free(varBinds);
}

/**
 * added by D.J Park in 2004.10.15
 * check Threshold(CPU,Memory) and send Trap
 * psinfo : Process Information
 * pstMycontext : process Monitoring Information
 */
void checkThreshold(ps_cache_info *psinfo, psMonitorTable_context *pstMycontext)
{
    ps_cache_info *prepsinfo = NULL;
    long cpu = 0;
    long mem = 0;
    char              tbuf[30] ={0};

    cpu = pstMycontext->psMonitorCPU;
    mem = pstMycontext->psMonitorMEM;
    //getCurrentTime(tbuf);
    //snmp_log(LOG_INFO,"[%s] checkThreshold  PID[%ld] NAME[%s] CPU[%ld]"
    //        " MEM[%ld]\n",tbuf,psinfo->pid,psinfo->name,
    //        psinfo->cpu,psinfo->mem);

    // return if not set threshold(mem,cpu)
    if(cpu < 1 && mem < 1)
        return;

    prepsinfo = searchPrePsInfo(psinfo->pid);

    // case does not exist previous process data
    if(prepsinfo == NULL)
    {
        if(cpu > 0)
        {
            if(psinfo->cpu >= cpu)
            {
                pstMycontext->thrcpu = 1;
                sendThresholdTrap(pstMycontext,psinfo,COLUMN_PSMONITORCPU,1);
            }
        }

        if(mem > 0)
        {
            if(psinfo->mem >= mem)
            {
                pstMycontext->thrmem = 1;
                sendThresholdTrap(pstMycontext,psinfo,COLUMN_PSMONITORMEM,1);
            }
        }

        return;
    }

    // case exist previous process data
    if(cpu > 0)
    {
        // threshold and does not earlier send trap 
        if(psinfo->cpu >= cpu && pstMycontext->thrcpu < 1)
        {
            pstMycontext->thrcpu = 1;
            sendThresholdTrap(pstMycontext,psinfo,COLUMN_PSMONITORCPU,1);
        // does not threshold and earlier send trap 
        } else if( psinfo->cpu < cpu && pstMycontext->thrcpu > 0)
        {
            pstMycontext->thrcpu = 0;
            sendThresholdTrap(pstMycontext,psinfo,COLUMN_PSMONITORCPU,0);
        }
    }

    if(mem > 0)
    {
        // threshold and does not earlier send trap 
        if(psinfo->mem >= mem && pstMycontext->thrmem < 1)
        {
            pstMycontext->thrmem = 1;
            sendThresholdTrap(pstMycontext,psinfo,COLUMN_PSMONITORMEM,1);
        // does not threshold and earlier send trap 
        } else if( psinfo->mem < mem && pstMycontext->thrmem > 0)
        {
            pstMycontext->thrmem = 0;
            sendThresholdTrap(pstMycontext,psinfo,COLUMN_PSMONITORMEM,0);
        }
    }
}

/**
 * added by D.J Park in 2004.10.16
 * get process cpu load information(percent * 100) 
 * pid : process ID
 */
long getPsPercentCPU(int pid)
{
    long cpu = 0;
    ps_cache_info *pci;
    pthread_mutex_lock(&procMonProcessCheckLock);		
    for(pci = gpstPsInfo ; pci != NULL ; pci=pci->next)
    {
        if(pci->pid == pid)
        {
            cpu = pci->hcpu;
            break;
        }
    }
    pthread_mutex_unlock(&procMonProcessCheckLock);		

    return cpu;
}

/** 
 * added by D.J Park in 2005.02.24
 * 일정 주기의 cpu 사용량에 대한 값을 리턴한다.
 * cpuStateItem : CPU_USER(0), CPU_KERENL(1), CPU_WAIT(2), CPU_IDLE(3), 
 *                CPU_UTIL(4)
 *                cpuStateItem은 0~4 이어야 한다.
 */
long getPeriodCPUTime(int cpuStateItem)
{
    if( cpuStateItem < 0 || cpuStateItem > 4)
        return 0;

    return psMonCpuInfo.stateUsage[cpuStateItem];
}

/** 
 * added by D.J Park in 2005.02.24
 * 일정 주기의 cpu 사용량에 대한 값을 리턴한다.
 * cpuStateItem : CPU_USER(0), CPU_KERENL(1), CPU_WAIT(2), CPU_IDLE(3), 
 *                CPU_UTIL(4)
 *                cpuStateItem은 0~4 이어야 한다.
 */
long getPeriodFloatCPUTime(int cpuStateItem)
{
    if( cpuStateItem < 0 || cpuStateItem > 4)
        return 0;

    return psMonCpuInfo.stateUsageFloat[cpuStateItem];
}

/**
  * 프로세스 감시 시간을 얻어 온다.
  * 시간은 0~23만 유효한다.
  * param : startHour 시작 시간
  * param : endHour 끝 시간
  */
int getMonitoringTime(int startHour, int endHour)
{
    time_t t;
    struct tm *ptm;
    int starttime,endtime,curtime;

    if(startHour == endHour || startHour < 0 || endHour < 0 
            || startHour > 23 || endHour > 23)
       return 1; 

    starttime = startHour*60;
    endtime = endHour*60;
    if(startHour > endHour) 
        endtime += (23*60);
    time(&t);
    ptm = localtime(&t);
    curtime = ptm->tm_hour * 60 + ptm->tm_min;
    if(curtime < starttime)
        curtime += (23*60);
    if( curtime >= starttime && curtime <= endtime)
        return 1;

    return 0;
}

/** added by HJ.Kwon. in 2005.04.11. start. */
#ifdef WIN32
static unsigned long to_msec(PFILETIME ptr)
{
    unsigned long	    x =
	((long) ptr->dwHighDateTime << 32) +
	(unsigned) ptr->dwLowDateTime;
    x /= NSPERMSEC;
    return x;
}

int DetectNumProcessors()
{
    SYSTEM_INFO sinfo;
    GetSystemInfo(&sinfo);
    if(sinfo.dwNumberOfProcessors < 1)
        return 1;

    return (int)sinfo.dwNumberOfProcessors;
}
#endif
/** end. */

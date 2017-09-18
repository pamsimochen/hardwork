/*
 *  @file   NameServer.c
 *
 *  @brief      NameServer Manager
 *
 *              The NameServer module manages local name/value pairs that
 *              enables an application and other modules to store and retrieve
 *              values based on a name. The module supports different lengths of
 *              values. The add/get functions are for variable length values.
 *              The addUInt32 function is optimized for UInt32 variables and
 *              constants. Each NameServer instance manages a different
 *              name/value table. This allows each table to be customized to
 *              meet the requirements of user:
 *              @li Size differences: one table could allow long values
 *              (e.g. > 32 bits) while another table could be used to store
 *              integers. This customization enables better memory usage.
 *              @li Performance: improves search time when retrieving a
 *              name/value pair.
 *              @li Relax name uniqueness: names in a specific table must be
 *              unique, but the same name can be used in different tables.
 *              @li Critical region management: potentially different tables are
 *              used by different types of threads. The user can specify the
 *              type of critical region manager (i.e. xdc.runtime.IGateProvider)
 *              to be used for each instance.
 *              When adding a name/value pair, the name and value are copied
 *              into internal buffers in NameServer. To minimize runtime memory
 *              allocation these buffers can be allocated at creation time.
 *              The NameServer module can be used in a multiprocessor system.
 *              The module communicates to other processors via the RemoteProxy.
 *              The way the communication to the other processors is dependent
 *              on the RemoteProxy implementation.
 *              The NameServer module uses the MultiProc module for identifying
 *              the different processors. Which remote processors and the order
 *              they are quered is determined by the procId array in the get
 *              function.
 *              Currently there is no endian or wordsize conversion performed by
 *              the NameServer module.<br>
 *              Transport management:<br>
 *              #NameServer_setup API creates two NameServers internally. These
 *              NameServer are used for holding handles and names of other
 *              nameservers created by application or modules. This helps, when
 *              a remote processors wants to get data from a nameserver on this
 *              processor. In all modules, which can have instances, all created
 *              instances can be kept in a module specific nameserver. This
 *              reduces search operation if a single nameserver is used for all
 *              modules. Thus a module level nameserver helps.<br>
 *              When a module requires some information from a remote nameserver
 *              it passes a name in the following format:<br>
 *              "<module_name>:<instance_name>or<instance_info_name>"<br>
 *              For example: "GatePeterson:notifygate"<br>
 *              When transport gets this name it searches for <module_name> in
 *              the module nameserver (created by NameServer_setup). So, it gets
 *              the module specific NameServer handle, then it searchs for the
 *              <instance_name> or <instance_info_name> in the NameServer.
 *
 *
 *  ============================================================================
 *
 *  Copyright (c) 2008-2012, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information: 
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *  
 */


/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/ipc/NameServer.h>
#include <ti/syslink/inc/_NameServer.h>
#include <ti/syslink/inc/NameServerRemoteNotify.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/IGateProvider.h>


 /*
 *  The dynamic name/value table looks like the following. This approach allows
 *  each instance table to have different value and different name lengths.
 *  The names block is allocated on the create. The size of that block is
 *  (maxRuntimeEntries * maxNameLen). That block is sliced and diced up and
 *  given to each table entry.
 *  The same thing is done for the values block.
 *
 *     names                    table                  values
 *   -------------           -------------          -------------
 *   |           |<-\        |   elem    |   /----->|           |
 *   |           |   \-------|   name    |  /       |           |
 *   |           |           |   value   |-/        |           |
 *   |           |           |   len     |          |           |
 *   |           |<-\        |-----------|          |           |
 *   |           |   \       |   elem    |          |           |
 *   |           |    \------|   name    |  /------>|           |
 *   |           |           |   value   |-/        |           |
 *   -------------           |   len     |          |           |
 *                           -------------          |           |
 *                                                  |           |
 *                                                  |           |
 *                                                  -------------
 *
 *  There is an optimization for small values (e.g. <= sizeof(UInt32).
 *  In this case, there is no values block allocated. Instead the value
 *  field is used directly.  This optimization occurs and is managed when
 *  obj->maxValueLen <= sizeof(UInt32).
 *
 *  The static create is a little different. The static entries point directly
 *  to a name string (and value). Since it points directly to static items,
 *  this entries cannot be removed.
 *  If maxRuntimeEntries is non-zero, a names and values block is created.
 *  Here is an example of a table with 1 static entry and 2 dynamic entries
 *
 *                           ------------
 *                           |   elem   |
 *      "myName" <-----------|   name   |----------> someValue
 *                           |   value  |
 *     names                 |   len    |              values
 *   -------------           -------------          -------------
 *   |           |<-\        |   elem    |   /----->|           |
 *   |           |   \-------|   name    |  /       |           |
 *   |           |           |   value   |-/        |           |
 *   |           |           |   len     |          |           |
 *   |           |<-\        |-----------|          |           |
 *   |           |   \       |   elem    |          |           |
 *   |           |    \------|   name    |  /------>|           |
 *   |           |           |   value   |-/        |           |
 *   -------------           |   len     |          |           |
 *                           -------------          |           |
 *                                                  |           |
 *                                                  |           |
 *                                                  -------------
 *
 *  NameServerD uses a freeList and nameList to maintain the empty
 *  and filled-in entries. So when a name/value pair is added, an entry
 *  is pulled off the freeList, filled-in and placed on the nameList.
 *  The reverse happens on a remove.
 *
 *  For static adds, the entries are placed on the nameList statically.
 *
 *  For dynamic creates, the freeList is populated in postInt and there are no
 *  entries placed on the nameList (this happens when the add is called).
 *
 */

/* Macro to make a correct module magic number with refCount */
#define NameServer_MAKE_MAGICSTAMP(x) ((NameServer_MODULEID << 12u) | (x))

/* =============================================================================
 * Struct & Enums
 * =============================================================================
 */
/* Structure of entry in Name/Value table */
typedef struct NameServer_TableEntry_tag {
    List_Elem                 elem;
    /* List element */
    UInt32                    hash;
    /* Hash value */
    String                    name;
    /* Name portion of the name/value pair. */
    UInt                      len;
    /* Length of the value field. */
    Ptr                       value;
    /* Value portion of the name/value entry. */
    Bool                      collide;
    /* Does the hash collides? */
    struct NameServer_TableEntry_tag * next;
    /* Pointer to the next entry, used incase of collision only */
} NameServer_TableEntry;

/* structure for NameServer module state */
typedef struct NameServer_ModuleObject {
    List_Object               objList;
    /* List holding created objects */
    IGateProvider_Handle      gate;
    /* Handle to module gate */
    NameServerRemote_Handle * nsRemoteHandle;
    /* List of Remote driver handles for processors */
    Atomic                    refCount;
    /* Reference count */
    NameServer_Params         defInstParams;
    /* Default instance paramters */
    NameServer_Config         defCfg;
    /* Default module configuration */
    NameServer_Config         cfg;
    /* Module configuration */
} NameServer_ModuleObject;

/* Structure defining object for the NameServer */
struct NameServer_Object {
    List_Elem          elem;
    String             name;            /* name of the instance */
    List_Object        nameList;        /* list of entries */
    IGateProvider_Handle gate;          /* gate for critical section */
    NameServer_Params  params;          /* the parameter structure */
    UInt32             count;           /* count of entries */
} NameServer_Object;


/* =============================================================================
 * Globals
 * =============================================================================
 */
/*!
 *  @var    NameServer_state
 *
 *  @brief  Variable for holding state of the NameServer module.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
NameServer_ModuleObject NameServer_state =
{
    .defCfg.reserved                 = 0x0,
    .defInstParams.maxRuntimeEntries = 0u,
    .defInstParams.tableHeap         = NULL,
    .defInstParams.checkExisting     = TRUE,
    .defInstParams.maxValueLen       = 0u,
    .defInstParams.maxNameLen        = 16u
};

/*!
 *  @var    NameServer_module
 *
 *  @brief  Pointer to the NameServer module state.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
NameServer_ModuleObject * NameServer_module = &NameServer_state;


/* =============================================================================
 * APIs
 * =============================================================================
 */
/* Function to get the default configuration for the NameServer module. */
Void
NameServer_getConfig (NameServer_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "NameServer_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getConfig",
                             NameServer_E_INVALIDARG,
                             "Argument of type (NameServer_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                      NameServer_MAKE_MAGICSTAMP(0),
                                      NameServer_MAKE_MAGICSTAMP(1))
            == TRUE) {
            /* (If setup has not yet been called) */
            Memory_copy (cfg,
                         &NameServer_module->defCfg,
                         sizeof (NameServer_Config));
        }
        else {
            Memory_copy (cfg,
                         &NameServer_module->cfg,
                         sizeof (NameServer_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NameServer_getConfig");
}


/* Function to setup the nameserver module. */
Int
NameServer_setup (Void)
{
    Int status    = NameServer_S_SUCCESS;
    Error_Block   eb;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int tmpStatus = NameServer_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_ENTER, "NameServer_setup");
    Error_init (&eb);

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&NameServer_module->refCount,
                            NameServer_MAKE_MAGICSTAMP(0),
                            NameServer_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&NameServer_module->refCount)
        != NameServer_MAKE_MAGICSTAMP(1u)) {
        status = NameServer_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "NameServer Module already initialized!");
    }
    else {
        /* Construct the list object */
        List_construct (&NameServer_module->objList, NULL);
        /* Create a lock for protecting list object */
        NameServer_module->gate = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (NameServer_module->gate == NULL) {
            /*! @retval NameServer_E_FAIL Failed to create local gate */
            status = NameServer_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServer_setup",
                                 status,
                                 "Failed to create local gate!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            NameServer_module->nsRemoteHandle =
                 (NameServerRemote_Handle *) Memory_alloc (NULL,
                                                (  sizeof (Ptr)
                                                * MultiProc_getNumProcessors()),
                                                0u,
                                                NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (NameServer_module->nsRemoteHandle == NULL) {
                tmpStatus = GateMutex_delete ((GateMutex_Handle *)
                                              &NameServer_module->gate);
                GT_assert (curTrace, (tmpStatus >= 0));
                /*! @retval NameServer_E_MEMORY Failed allocate memory for remote
                 * driver handle lists*/
                status = NameServer_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NameServer_setup",
                                     status,
                                     "Failed allocate memory for remote driver "
                                     "handle lists!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Memory_set (NameServer_module->nsRemoteHandle,
                            0,
                            sizeof (Ptr) * MultiProc_getNumProcessors());
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "NameServer_setup", status);

    /*! @retval NameServer_S_SUCCESS operation was successful */
    return status;
}


/* Function to destroy the nameserver module. */
Int
NameServer_destroy (void)
{
    Int status    = NameServer_S_SUCCESS;
    Int tmpStatus = NameServer_S_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "NameServer_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval NameServer_E_INVALIDSTATE Module was not setup */
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_destroy",
                             status,
                             "Module was not setup!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&NameServer_module->refCount)
            == NameServer_MAKE_MAGICSTAMP(0)) {
            List_destruct (&NameServer_module->objList);

            tmpStatus = GateMutex_delete ((GateMutex_Handle *)
                                          &NameServer_module->gate);
            GT_assert (curTrace, (tmpStatus >= 0));
            if ((status >= 0) && (tmpStatus < 0)) {
                status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NameServer_destroy",
                                     status,
                                     "GateMutex_delete failed!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            Memory_free (NULL,
                         NameServer_module->nsRemoteHandle,
                         (sizeof (Ptr) * MultiProc_getNumProcessors()));
            NameServer_module->nsRemoteHandle = NULL;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_destroy", status);

    /*! @retval NameServer_S_SUCCESS operation was successful */
    return status;
}


/* Initialize this config-params structure with supplier-specified
 * defaults before instance creation.
 */
Void
NameServer_Params_init (NameServer_Params * params)
{
    GT_1trace (curTrace, GT_ENTER, "NameServer_Params_init", params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_Params_init",
                             NameServer_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_Params_init",
                             NameServer_E_INVALIDARG,
                             "Argument of type (NameServer_CreateParams *) is "
                             "NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_copy (params,
                     &(NameServer_module->defInstParams),
                     sizeof (NameServer_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NameServer_Params_init");
}


/* Function to create a name server. */
NameServer_Handle
NameServer_create (String name, const NameServer_Params * params)
{
    NameServer_Handle handle = NULL;
    IArg              key;
    Error_Block eb;

    GT_2trace (curTrace, GT_ENTER, "NameServer_create", name, params);

    GT_assert (curTrace, (params != NULL));
    GT_assert (curTrace, (name != NULL));
    Error_init (&eb);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_create",
                             NameServer_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_create",
                             NameServer_E_INVALIDARG,
                             "params passed is null!");
    }
    else if (name == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_create",
                             NameServer_E_INVALIDARG,
                             "name passed is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* check if the name is already created or not */
        if (NameServer_getHandle (name)) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServer_create",
                                 NameServer_E_INVALIDARG,
                                 "Name is in use!");
        }
        else {
            /* allocate memory for the handle */
            handle = (NameServer_Handle) Memory_calloc (params->tableHeap,
                                                     sizeof (NameServer_Object),
                                                     0,
                                                     NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (handle == NULL) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NameServer_create",
                                     NameServer_E_MEMORY,
                                     "Memory allocation failed for handle!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (handle != NULL) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Allocate memory for the name */
            handle->name = (String) Memory_alloc (params->tableHeap,
                                                  String_len (name) + 1u,
                                                  0,
                                                  NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (handle->name == NULL) {
                Memory_free (params->tableHeap,
                             handle,
                             sizeof (NameServer_Object));
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NameServer_create",
                                     NameServer_E_MEMORY,
                                     "Memory allocation failed for name!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Copy the name */
                String_ncpy (handle->name,
                            name,
                            String_len (name) + 1u);
                /* Copy the params */
                Memory_copy ((Ptr) &handle->params,
                             (Ptr) params,
                             sizeof (NameServer_Params));

                if (params->maxValueLen < sizeof(UInt32)) {
                    handle->params.maxValueLen = sizeof(UInt32);
                }
                else {
                    handle->params.maxValueLen = params->maxValueLen;
                }

                /* Construct the list */
                List_construct (&handle->nameList, NULL);

                handle->gate = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
                if (handle->gate == NULL) {
                    NameServer_delete (&handle);
                    /*! @retval NULL Failed to create GateMutex */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "NameServer_create",
                                         NameServer_E_INVALIDARG,
                                         "Failed to create GateMutex!");
                }

                /* Initialize the count */
                handle->count = 0u;

                /* Put in the local list */
                key = IGateProvider_enter (NameServer_module->gate);
                List_elemClear (&handle->elem);
                List_putHead ((List_Handle) &NameServer_module->objList,
                              &handle->elem);
                IGateProvider_leave (NameServer_module->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_create", handle);

    return handle;
}


/* Function to construct a name server. */
void
NameServer_construct (      NameServer_Handle   handle,
                            String              name,
                      const NameServer_Params * params)
{
    IArg              key;
    Error_Block       eb;

    GT_3trace (curTrace,
               GT_ENTER,
               "NameServer_construct",
               handle,
               name,
               params);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (name   != NULL));
    GT_assert (curTrace, (params != NULL));
    Error_init (&eb);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval NameServer_E_INVALIDSTATE Module was not setup */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_construct",
                             NameServer_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_construct",
                             NameServer_E_INVALIDARG,
                             "handle passed is null!");
    }
    else if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_construct",
                             NameServer_E_INVALIDARG,
                             "params passed is null!");
    }
    else if (name == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_construct",
                             NameServer_E_INVALIDARG,
                             "name passed is null!");
    }
    else if ((params != NULL) && (name != NULL)) {
        if (params->tableHeap) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServer_construct",
                                 NameServer_E_INVALIDARG,
                                 "Heap handle passed is null!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* check if the name is already registered or not */
            if (NameServer_getHandle (name)) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NameServer_construct",
                                     NameServer_E_ALREADYEXISTS,
                                     "Name is in use!");
            }
            else {
                /* Allocate memory for the name */
                handle->name = (String)Memory_alloc (params->tableHeap,
                                                     String_len (name) + 1u,
                                                     0,
                                                     NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (handle->name == NULL) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "NameServer_construct",
                                         NameServer_E_MEMORY,
                                         "Memory allocation failed for name!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Copy the name */
                    String_ncpy (handle->name,
                                name,
                                String_len (name) + 1u);
                    /* Copy the params */
                    Memory_copy ((Ptr) &handle->params,
                                 (Ptr) params,
                                 sizeof (NameServer_Params));

                    if (params->maxValueLen < sizeof(UInt32)) {
                        handle->params.maxValueLen = sizeof(UInt32);
                    }
                    else {
                        handle->params.maxValueLen = params->maxValueLen;
                    }

                    /* Construct the list */
                    List_construct (&handle->nameList, NULL);

                    /* Set up the gate for this instance */
                    handle->gate = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
                    if (handle->gate == NULL) {
                        NameServer_destruct (handle);
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "NameServer_construct",
                                             NameServer_E_INVALIDARG,
                                             "Failed to create GateMutex!");
                    }

                    /* Initialize the count */
                    handle->count = 0u;

                    /* Put in the local list */
                    key = IGateProvider_enter (NameServer_module->gate);
                    List_elemClear (&handle->elem);
                    List_putHead ((List_Handle) &NameServer_module->objList,
                                  &handle->elem);
                    IGateProvider_leave (NameServer_module->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NameServer_construct");
}


/* Function to delete a name server. */
Int
NameServer_delete (NameServer_Handle * handle)
{
    Int                 status = NameServer_S_SUCCESS;
    GateMutex_Handle    gate = NULL;
    IArg                key = 0;
    IArg                key1;

    GT_1trace (curTrace, GT_ENTER, "NameServer_delete", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_delete",
                             status,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_delete",
                             status,
                             "handle is null!");
    }
    else if (*handle == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_delete",
                             status,
                             "*handle is null!");
    }
    else {
        if (  ((*handle)->name != NULL)
            &&(   NameServer_getHandle ((*handle)->name)
               == NULL)) {
            status = NameServer_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServer_delete",
                                 status,
                                 "Handle is invalid!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            if ((*handle)->gate != NULL) {
                /* enter the critical section */
                key = IGateProvider_enter ((*handle)->gate);
            }

            GT_assert (curTrace, ((*handle)->count == 0));

            if ((*handle)->count == 0) {
                key1 = IGateProvider_enter (NameServer_module->gate);
                List_remove ((List_Handle) &NameServer_module->objList,
                             &(*handle)->elem);
                IGateProvider_leave (NameServer_module->gate, key1);

                gate = (GateMutex_Handle)(*handle)->gate;

                if ((*handle)->name != NULL) {
                    /* free the memory allocated for the name */
                    Memory_free ((*handle)->params.tableHeap,
                                 (*handle)->name,
                                 String_len ((*handle)->name) + 1u);
                    (*handle)->name = NULL;
                }

                /* Destruct the list */
                List_destruct (&(*handle)->nameList);

                /* Free the memory used for handle */
                Memory_free ((*handle)->params.tableHeap,
                             (*handle),
                             sizeof (NameServer_Object));
                (*handle) = NULL;

                /* leave the critical section */
                IGateProvider_leave ((IGateProvider_Handle)gate, key);

                if (gate != NULL) {
                    GateMutex_delete (&gate);
                }
            }
            else {
                if ((*handle)->gate != NULL) {
                    /* leave the critical section */
                    IGateProvider_leave ((*handle)->gate, key);
                }
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_delete", status);

    return status;
}


/* Function to delete a name server. */
void
NameServer_destruct (NameServer_Handle handle)
{
    IGateProvider_Handle gate = NULL;
    IArg        key;
    IArg        key1;

    GT_1trace (curTrace, GT_ENTER, "NameServer_destruct", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_destruct",
                             NameServer_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_destruct",
                             NameServer_E_INVALIDARG,
                             "handle is null!");
    }
    else {
        if (NameServer_getHandle (handle->name) == NULL) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServer_destruct",
                                 NameServer_E_INVALIDARG,
                                 "Handle is invalid!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* enter the critical section */
            key = IGateProvider_enter (handle->gate);

            GT_assert (curTrace, (handle->count > 0));
            if (handle->count == 0) {
                key1 = IGateProvider_enter (NameServer_module->gate);
                List_remove ((List_Handle) &NameServer_module->objList,
                             &handle->elem);
                IGateProvider_leave (NameServer_module->gate, key1);

                gate = handle->gate;

                /* free the memory allocated for the name */
                Memory_free (handle->params.tableHeap,
                             handle->name,
                             String_len (handle->name) + 1u);

                /* Destruct the list */
                List_destruct (&handle->nameList);

                /* Free the memory used for handle */
                Memory_set (handle, 0, sizeof (NameServer_Object));

                /* leave the critical section */
                IGateProvider_leave (gate, key);

                GateMutex_delete ((GateMutex_Handle *) &gate);
            }
            else {
                /* leave the critical section */
                IGateProvider_leave (handle->gate, key);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "NameServer_destruct");
}


/* Adds a variable length value into the local NameServer table */
Ptr
NameServer_add (NameServer_Handle handle, String name, Ptr buf, UInt len)
{
    Int                 status = NameServer_S_SUCCESS;
    List_Elem         * node   = NULL;
    NameServer_TableEntry * new_node = NULL;
    Bool                found = FALSE;
    UInt32              hash;
    IArg                key;

    GT_4trace (curTrace, GT_ENTER, "NameServer_add",
               handle, name, buf, len);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (name     != NULL));
    GT_assert (curTrace, (buf      != NULL));
    GT_assert (curTrace, (len      != 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_add",
                             status,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_add",
                             status,
                             "handle: Handle is null!");
    }
    else if (name == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_add",
                             status,
                             "name: name is null!");
    }
    else if (buf == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_add",
                             status,
                             "buf: buf is null!");
    }
    else if (len == 0u) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_add",
                             status,
                             "len: Length is zero!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* enter the critical section */
        key = IGateProvider_enter (handle->gate);

        /* Calculate the hash */
        hash = String_hash (name);

        /* Traverse the list to find duplicate check */
        List_traverse (node, (List_Handle) &handle->nameList) {
            /* Hash matches */
            if (((NameServer_TableEntry *)node)->hash == hash) {
                /* If the name matches, incase hash is duplicate */
                if (String_cmp (((NameServer_TableEntry *)node)->name,
                                name) == 0){
                    if (handle->params.checkExisting == TRUE) {
                        status = NameServer_E_INVALIDARG;
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "NameServer_add",
                                             status,
                                             "duplicate entry found!");
                        break;
                    }
                }
                else {
                    found = TRUE;
                    break;
                } /* name does not match */
            } /* hash does not match */
        } /* List_traverse */

        /* Now add the new entry. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* chain the entry */
            new_node = (NameServer_TableEntry *) Memory_alloc (
                                                handle->params.tableHeap,
                                                sizeof (NameServer_TableEntry),
                                                0,
                                                NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (new_node == NULL) {
                status = NameServer_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NameServer_add",
                                     status,
                                     "not able to allocate memory "
                                     "for the new node!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                new_node->hash    = hash;
                new_node->collide = found; /* Indicate if there is a collision*/
                new_node->len     = len;
                new_node->next    = NULL;
                new_node->name = (String)
                                    Memory_alloc (handle->params.tableHeap,
                                                  String_len (name) + 1u,
                                                  0,
                                                  NULL);
                new_node->value  = (Ptr)
                                    Memory_alloc (handle->params.tableHeap,
                                                  len,
                                                  0,
                                                  NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (new_node->name == NULL) {
                    if (new_node->value != NULL) {
                        Memory_free (handle->params.tableHeap,
                                     new_node->value,
                                     len);
                        new_node->value = NULL;
                    }
                    Memory_free (handle->params.tableHeap,
                                 new_node,
                                 sizeof (NameServer_TableEntry));
                    new_node = NULL;
                    status = NameServer_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "NameServer_add",
                                         status,
                                         "not able to allocate memory "
                                         "for the name!");
                }
                else if (new_node->value == NULL) {
                    Memory_free (handle->params.tableHeap,
                                 new_node->name,
                                 String_len (name) + 1u);
                    new_node->name = NULL;
                    Memory_free (handle->params.tableHeap,
                                 new_node,
                                 sizeof (NameServer_TableEntry));
                    new_node = NULL;
                    status = NameServer_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "NameServer_add",
                                         status,
                                         "not able to allocate memory "
                                         "for the value!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    String_ncpy (new_node->name,
                                 name,
                                 String_len (name) + 1u);
                    Memory_copy ((Ptr) new_node->value, (Ptr) buf, len);

                    if (found == TRUE) {
                        /* If hash is found, need to stitch the list to link the
                         * new node to the existing node with the same hash.
                         */
                        new_node->next = ((NameServer_TableEntry *)node)->next;
                        ((NameServer_TableEntry *)node)->next = new_node;
                    }
                    else {
                        /* put the new node into the list */
                        List_putHead ((List_Handle) &handle->nameList,
                                      (List_Elem *) new_node);
                    }

                    /* Hash does not match to any existing entry */
                    /* Increase the count */
                    handle->count++;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* leave the critical section */
        IGateProvider_leave (handle->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_add", new_node);

    return new_node;
}


/* Function to add a UInt32 value into a name server. */
Ptr
NameServer_addUInt32 (NameServer_Handle handle, String name, UInt32 value)
{
    Ptr entry = NULL;

    GT_3trace (curTrace,
               GT_ENTER,
               "NameServer_addUInt32",
               handle,
               name,
               value);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (name   != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_addUInt32",
                             NameServer_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_addUInt32",
                             NameServer_E_INVALIDARG,
                             "handle: Handle is null!");
    }
    else if (name == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_addUInt32",
                             NameServer_E_INVALIDARG,
                             "name: name is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        entry = NameServer_add (handle, name, &value, sizeof (UInt32));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_addUInt32", entry);

    return entry;
}


/* Function to remove a name/value pair from a name server. */
Int
NameServer_remove (NameServer_Handle handle, String name)
{
    Int                 status = NameServer_S_SUCCESS;
    NameServer_TableEntry *  prev   = NULL;
    NameServer_TableEntry *  temp   = NULL;
    List_Elem       *   node   = NULL;
    Bool                done   = FALSE;
    UInt32              hash;
    IArg                key;

    GT_2trace (curTrace, GT_ENTER, "NameServer_remove",
               handle, name);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (name   != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_remove",
                             status,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_remove",
                             status,
                             "Handle is null!");
    }
    else if (name == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_remove",
                             status,
                             "name: name is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* enter the critical section */
        key = IGateProvider_enter (handle->gate);

        /* Calculate the hash */
        hash = String_hash (name);

        /* Traverse the list to find duplicate check */
        List_traverse (node, (List_Handle) &handle->nameList) {
            /* Hash matchs */
            if (((NameServer_TableEntry *)node)->hash == hash) {
                if (((NameServer_TableEntry *)node)->collide == TRUE) {
                    if (String_cmp (((NameServer_TableEntry *)node)->name,
                                    name) == 0){
                        Memory_free (handle->params.tableHeap,
                                     ((NameServer_TableEntry *)node)->value,
                                     ((NameServer_TableEntry *)node)->len);
                        Memory_free (handle->params.tableHeap,
                                     ((NameServer_TableEntry *)node)->name,
                                     String_len (name) + 1u);
                        Memory_copy ((Ptr)node,
                                    (Ptr) ((NameServer_TableEntry *)node)->next,
                                    sizeof (NameServer_TableEntry));
                        ((NameServer_TableEntry *)node)->next =
                                    ((NameServer_TableEntry *)node)->next->next;
                        Memory_free (handle->params.tableHeap,
                                     ((NameServer_TableEntry *)node)->next,
                                     sizeof (NameServer_TableEntry));
                        handle->count--;
                        done = TRUE;
                        break;
                    }
                    else {
                        prev = ((NameServer_TableEntry *)node);
                        temp = ((NameServer_TableEntry *)node)->next;
                        while (temp) {
                            if (String_cmp (((NameServer_TableEntry*)temp)->name,
                                            name) == 0){
                                Memory_free (handle->params.tableHeap,
                                         temp->value,
                                         ((NameServer_TableEntry *)node)->len);
                                Memory_free (handle->params.tableHeap,
                                             temp->name,
                                             String_len (name) + 1u);
                                prev->next = temp->next;
                                Memory_free (handle->params.tableHeap,
                                             temp,
                                             sizeof (NameServer_TableEntry));
                                handle->count--;
                                done = TRUE;
                                break;
                            }
                            temp = temp->next;
                        }
                        break;
                    }
                }
                else {
                    Memory_free (handle->params.tableHeap,
                                 ((NameServer_TableEntry *)node)->value,
                                 ((NameServer_TableEntry *)node)->len);
                    Memory_free (handle->params.tableHeap,
                                 ((NameServer_TableEntry *)node)->name,
                                 String_len (name) + 1u);
                    List_remove ((List_Handle) &handle->nameList, node);
                    Memory_free (handle->params.tableHeap,
                                 ((NameServer_TableEntry *)node),
                                 sizeof (NameServer_TableEntry));
                    handle->count--;
                    done = TRUE;
                    break;
                }
            }
        }

        /* leave the critical section */
        IGateProvider_leave (handle->gate, key);

        if (done == FALSE) {
            status = NameServer_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServer_remove",
                                 status,
                                 "Entry not found!");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_remove", status);

    return (status);
}


/* Function to remove a name/value pair from a name server. */
Int
NameServer_removeEntry (NameServer_Handle handle, Ptr entry)
{
    Int  status = NameServer_S_SUCCESS;
    NameServer_TableEntry * node;
    IArg key;

    GT_2trace (curTrace, GT_ENTER, "NameServer_removeEntry",
               handle, entry);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (entry  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_removeEntry",
                             status,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_removeEntry",
                             status,
                             "Handle is null!");
    }
    else if (entry == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_removeEntry",
                             status,
                             "entry: entry is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* enter the critical section */
        key = IGateProvider_enter (handle->gate);

        node = (NameServer_TableEntry *) entry;

        Memory_free (handle->params.tableHeap, node->value, node->len);
        Memory_free (handle->params.tableHeap,
                     node->name,
                     (String_len (node->name) + 1u));
        List_remove ((List_Handle) &handle->nameList, (List_Elem *) node);
        Memory_free (handle->params.tableHeap,
                     node,
                     sizeof (NameServer_TableEntry));
        handle->count--;

        /* leave the critical section */
        IGateProvider_leave (handle->gate, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_removeEntry", status);

    return (status);
}


/* Function to retrieve the value portion of a name/value pair from
 * local table.
 */
Int
NameServer_get (NameServer_Handle handle,
                String            name,
                Ptr               value,
                UInt32 *          len,
                UInt16            procId[])
{
    /* Initialize status to not found. */
    Int    status = NameServer_E_NOTFOUND;
    UInt32 i;

    GT_5trace (curTrace, GT_ENTER, "NameServer_get",
               handle, name, value, len, procId);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (name   != NULL));
    GT_assert (curTrace, (value  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_get",
                             status,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_get",
                             status,
                             "handle: Handle is null!");
    }
    else if (name == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_get",
                             status,
                             "name: name is null!");
    }
    else if (value == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_get",
                             status,
                             "value: value is null!");
    }
    else if (len == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_get",
                             status,
                             "len: len is null!");
    }
    else if (*len == 0u) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_get",
                             status,
                             "*len: length is zero!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (procId == NULL) {
            /* Query the local one first */
            status = NameServer_getLocal(handle, name, value, len);
            if (status == NameServer_E_NOTFOUND) {
                /* To eliminate code if possible
                 * Query all the remote processors
                 */
                for (i = 0; i < MultiProc_getNumProcessors(); i++) {
                    /* Skip the local table. It was already searched */
                    if (   (i != MultiProc_self ())
                        && (NameServer_module->nsRemoteHandle [i] != NULL)) {
                        status = NameServerRemoteNotify_get (
                                          (NameServerRemoteNotify_Handle)
                                            NameServer_module->nsRemoteHandle [i],
                                          handle->name,
                                          name,
                                          value,
                                          len,
                                          NULL);
                        if (    (status >= 0)
                            ||  (   (status < 0)
                                 && (status != NameServer_E_NOTFOUND))) {
                            break;
                        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        else {
                            GT_0trace (curTrace,
                                  GT_3CLASS,
                                  "Failed to get value from remote NameServer");
                        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }
                }
            }
        }
        else {
            /*
             *  Search the query list. It might contain the local proc
             *  somewhere in the list.
             */
            i = 0;
            while (procId[i] != MultiProc_INVALIDID) {
                if (procId[i] == MultiProc_self()) {
                    /* Check local */
                    status = NameServer_getLocal(handle, name, value, len);
                }
                else {
                    status = NameServerRemoteNotify_get (
                              (NameServerRemoteNotify_Handle)
                                  NameServer_module->nsRemoteHandle [procId [i]],
                                  handle->name,
                                  name,
                                  value,
                                  len,
                                  NULL);
                }

                /* A count of non-zero means it was found...return */
                if (    (status > 0)
                    ||  (   (status < 0)
                         && (status != NameServer_E_NOTFOUND))) {
                    break;
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                else {
                    GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServer_get",
                                 status,
                                 "Failure from remote NameServer get!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                i++;
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_get", status);

    return status;
}


/* Function to Retrieve the value portion of a name/value pair from
 * local table.
 */
Int
NameServer_getLocal (NameServer_Handle handle,
                     String            name,
                     Ptr               value,
                     UInt32 *          len)
{
    /* Initialize status to not found. */
    Int                 status = NameServer_E_NOTFOUND;
    List_Elem       *   node   = NULL;
    NameServer_TableEntry *  temp   = NULL;
    Bool                done   = FALSE;
    UInt32              length;
    UInt32              hash;
    IArg                key;

    GT_4trace (curTrace, GT_ENTER, "NameServer_getLocal",
               handle, name, value, len);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (name   != NULL));
    GT_assert (curTrace, (value  != NULL));
    GT_assert (curTrace, (len    != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getLocal",
                             status,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getLocal",
                             status,
                             "handle: Handle is null!");
    }
    else if (name == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getLocal",
                             status,
                             "name: name is null!");
    }
    else if (value == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getLocal",
                             status,
                             "value: value is null!");
    }
    else if (len == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getLocal",
                             status,
                             "len: len is null!");
    }
    else if (*len == 0u) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getLocal",
                             status,
                             "*len: length is zero!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GT_1trace (curTrace,
                   GT_2CLASS,
                   "    NameServer_getLocal name [%s]",
                   name);
        length = *len;

        /* enter the critical section */
        key = IGateProvider_enter (handle->gate);

        /* Calculate the hash */
        hash = String_hash (name);

        /* Traverse the list to find duplicate check */
        List_traverse (node, (List_Handle) &handle->nameList) {
            /* Hash matchs */
            if (((NameServer_TableEntry *)node)->hash == hash) {
                if (((NameServer_TableEntry *)node)->collide == TRUE) {
                    temp = ((NameServer_TableEntry *)node);
                    while (temp) {
                        if (String_cmp (((NameServer_TableEntry*)temp)->name,
                                        name) == 0u){
                            if (length <= ((NameServer_TableEntry *)node)->len) {
                                Memory_copy (value,
                                             ((NameServer_TableEntry *)node)->value,
                                             length);
                                *len = length;
                            }
                            else {
                                Memory_copy (value,
                                            ((NameServer_TableEntry *)node)->value,
                                            ((NameServer_TableEntry *)node)->len);
                                *len = ((NameServer_TableEntry *)node)->len;
                            }
                            done = TRUE;
                            break;
                        }
                        temp = temp->next;
                    }
                    break;
                }
                else {
                    if (length <= ((NameServer_TableEntry *)node)->len) {
                        Memory_copy (value,
                                    ((NameServer_TableEntry *)node)->value,
                                    length);
                        *len = length;
                    }
                    else {
                        Memory_copy (value,
                                    ((NameServer_TableEntry *)node)->value,
                                    ((NameServer_TableEntry *)node)->len);
                        *len = ((NameServer_TableEntry *)node)->len;
                    }
                    done = TRUE;
                    break;
                }
            }
        }

        /* leave the critical section */
        IGateProvider_leave (handle->gate, key);

        if (done == FALSE) {
            GT_0trace (curTrace,
                       GT_2CLASS,
                       "    NameServer_getLocal: Entry not found!");
        }
        else {
            status = NameServer_S_SUCCESS;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_getLocal", status);

    return status;
}


/* Gets a 32-bit value by name */
Int
NameServer_getUInt32 (NameServer_Handle handle,
                      String            name,
                      Ptr               value,
                      UInt16            procId[])
{
    /* Initialize status to not found */
    Int  status = NameServer_E_NOTFOUND;
    UInt32 len = sizeof (UInt32);

    GT_4trace (curTrace, GT_ENTER, "NameServer_getUInt32",
               handle, name, value, procId);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (name   != NULL));
    GT_assert (curTrace, (value  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getUInt32",
                             status,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getUInt32",
                             status,
                             "handle: Handle is null!");
    }
    else if (name == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getUInt32",
                             status,
                             "name: name is null!");
    }
    else if (value == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getUInt32",
                             status,
                             "value: value is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        status = NameServer_get (handle, name, value, &len, procId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if ((status < 0) && (status != NameServer_E_NOTFOUND)) {
            /* NameServer_E_NOTFOUND is a valid run-time failure. */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServer_getUInt32",
                                 status,
                                 "NameServer_get failed!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_getUInt32", status);

    return status;
}


/*
 *  Gets a 32-bit value by name from the local table
 *
 *  If the name is found, the 32-bit value is copied into the value
 *  argument and a success status is returned.
 *
 *  If the name is not found, zero is returned in len and the contents
 *  of value are not modified. Not finding a name is not considered
 *  an error.
 *
 *  This function only searches the local name/value table.
 *
 */
Int
NameServer_getLocalUInt32 (NameServer_Handle handle, String name, Ptr value)
{
    Int                 status = NameServer_S_SUCCESS;
    UInt32              len    = sizeof (UInt32);

    GT_3trace (curTrace, GT_ENTER, "NameServer_getLocalUInt32",
               handle, name, value);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (name   != NULL));
    GT_assert (curTrace, (value  != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval NameServer_E_INVALIDSTATE Module was not setup */
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getLocalUInt32",
                             status,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval NameServer_E_INVALIDARG obj: Handle is null! */
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getLocalUInt32",
                             status,
                             "handle: Handle is null!");
    }
    else if (name == NULL) {
        /*! @retval NameServer_E_INVALIDARG name: name is null! */
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getLocalUInt32",
                             status,
                             "name: name is null!");
    }
    else if (value == NULL) {
        /*! @retval NameServer_E_INVALIDARG buf: buf is null! */
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getLocalUInt32",
                             status,
                             "value: value is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        status = NameServer_getLocal (handle, name, value, &len);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if ((status < 0) && (status != NameServer_E_NOTFOUND)) {
            /* NameServer_E_NOTFOUND is a valid run-time failure. */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServer_getLocalUInt32",
                                 status,
                                 "NameServer_getLocal failed!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_getLocal", status);

    /*! @retval NameServer_S_SUCCESS If the name is found */
    return status;
}


/*  Function to retrieve the value portion of a name/value pair from
 *  local table.
 */
Int
NameServer_match (NameServer_Handle handle,
                  String            name,
                  UInt32 *          value)
{
    UInt32      len      = 0;
    UInt32      foundLen = 0;
    List_Elem * node     = NULL;
    IArg        key;

    GT_3trace (curTrace, GT_ENTER, "NameServer_match", handle, name, value);

    GT_assert (curTrace, (name != NULL));
    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (value != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_match",
                             NameServer_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (name == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_match",
                             NameServer_E_INVALIDARG,
                             "name is null!");
    }
    else if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_match",
                             NameServer_E_INVALIDARG,
                             "handle is null!");
    }
    else if (value == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_match",
                             NameServer_E_INVALIDARG,
                             "value is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = IGateProvider_enter (handle->gate);

        /* Traverse the list to find duplicate check */
        List_traverse (node, (List_Handle) &handle->nameList) {
            len = String_len (((NameServer_TableEntry *)node)->name);

            /* Only check if the name in the table is going to potentially be
             * a better match.
             */
            if (len > foundLen) {
                if (String_ncmp (name,
                                 ((NameServer_TableEntry *)node)->name,
                                 len) == 0) {
                    *value = (UInt32)(((NameServer_TableEntry *)node)->value);
                    foundLen = len;
                }
            }
        }

        IGateProvider_leave (handle->gate, key);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_match", foundLen);

    return foundLen;
}


/* Function to retrieve a NameServer handle from name. */
NameServer_Handle
NameServer_getHandle (String name)
{
    NameServer_Handle handle = NULL;
    Bool              found = FALSE;
    List_Elem *       elem;

    GT_1trace (curTrace, GT_ENTER, "NameServer_getHandle", name);

    GT_assert (curTrace, (name != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getHandle",
                             NameServer_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (name == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_getHandle",
                             NameServer_E_INVALIDARG,
                             "name is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        List_traverse (elem, (List_Handle) &NameServer_module->objList) {
            handle = (NameServer_Handle) elem;
            if (String_cmp (handle->name, name) == 0) {
                found = TRUE;
                break;
            }
        }

        if (found == FALSE) {
            handle = NULL;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_getHandle", handle);

    return handle;
}


/* =============================================================================
 * Internal functions
 * =============================================================================
 */
/* Function to register a remote driver.*/
Int
NameServer_registerRemoteDriver (NameServerRemote_Handle handle,
                                 UInt16                  procId)
{
    Int status = NameServer_S_SUCCESS;

    GT_2trace (curTrace,
               GT_ENTER,
               "NameServer_registerRemoteDriver",
               handle,
               procId);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (procId < MultiProc_getNumProcessors()));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval NameServer_E_INVALIDSTATE Module was not setup */
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_registerRemoteDriver",
                             status,
                             "Module was not setup!");
    }
    else if (handle == NULL) {
        /*! @retval NameServer_E_INVALIDARG handle: Handle is null */
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_registerRemoteDriver",
                             status,
                             "handle: Handle is null!");
    }
    else if (procId >= MultiProc_getNumProcessors()) {
        /*! @retval NameServer_E_INVALIDARG Invalid procid */
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_registerRemoteDriver",
                             status,
                             "Invalid procid!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        NameServer_module->nsRemoteHandle [procId] = handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_registerRemoteDriver", status);

    /*! @retval NameServer_S_SUCCESS Operation is successful */
    return status;
}


/* Function to unregister a remote driver. */
Int
NameServer_unregisterRemoteDriver (UInt16 procId)
{
    Int status = NameServer_S_SUCCESS;

    GT_1trace (curTrace,
               GT_ENTER,
               "NameServer_unregisterRemoteDriver",
               procId);

    GT_assert (curTrace, (procId < MultiProc_getNumProcessors()));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval NameServer_E_INVALIDSTATE Module was not setup */
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_unregisterRemoteDriver",
                             status,
                             "Module was not setup!");
    }
    else if (procId >= MultiProc_getNumProcessors()) {
        /*! @retval NameServer_E_INVALIDARG Invalid procid */
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_unregisterRemoteDriver",
                             status,
                             "Invalid procid!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        NameServer_module->nsRemoteHandle [procId] = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_unregisterRemoteDriver", status);

    /*! @retval NameServer_S_SUCCESS Operation is successful */
    return status;
}


/* Determines if a remote driver is registered for the specified id. */
Bool
NameServer_isRegistered (UInt16 procId)
{
    Bool registered = FALSE;

    GT_1trace (curTrace, GT_ENTER, "NameServer_isRegistered", procId);

    GT_assert (curTrace, (procId < MultiProc_getNumProcessors ()));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(NameServer_module->refCount),
                                  NameServer_MAKE_MAGICSTAMP(0),
                                  NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_isRegistered",
                             NameServer_E_INVALIDSTATE,
                             "Module was not setup!");
    }
    else if (procId >= MultiProc_getNumProcessors()) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServer_isRegistered",
                             NameServer_E_INVALIDARG,
                             "Invalid procid!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        registered = (NameServer_module->nsRemoteHandle [procId] != NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServer_isRegistered", registered);

    return (registered);
}


/*
 *  ======== NameServer_open ========
 */
Int NameServer_open(String name, NameServer_Handle *handlePtr)
{
    Int status;

    GT_2trace(curTrace, GT_ENTER, "NameServer_open:", name, handlePtr);

    status = NameServer_S_SUCCESS;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(NameServer_module->refCount),
        NameServer_MAKE_MAGICSTAMP(0), NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "NameServer_open",
            status, "module was not setup");
    }
    else if (name == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "NameServer_open",
            status, "name passed is null");
    }
    else if (handlePtr == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "NameServer_open",
            status, "handlePtr passed is null");
    }
#endif

    /* get handle to named instance */
    if (status == NameServer_S_SUCCESS) {
        /* TODO: add refCount management */
        *handlePtr = NameServer_getHandle(name);

        if (*handlePtr == NULL) {
            status = NameServer_E_FAIL;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "NameServer_open:", status);

    return(status);
}

/*
 *  ======== NameServer_close ========
 */
Int NameServer_close(NameServer_Handle *handlePtr)
{
    Int status;

    GT_1trace(curTrace, GT_ENTER, "NameServer_close:", handlePtr);

    status = NameServer_S_SUCCESS;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Atomic_cmpmask_and_lt(&(NameServer_module->refCount),
        NameServer_MAKE_MAGICSTAMP(0), NameServer_MAKE_MAGICSTAMP(1))
        == TRUE) {
        status = NameServer_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "NameServer_close",
            status, "module was not setup");
    }
    else if (handlePtr == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "NameServer_close",
            status, "handlePtr is null");
    }
    else if (*handlePtr == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "NameServer_close",
            status, "*handlePtr is null");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    if (status == NameServer_S_SUCCESS) {
        if (NameServer_getHandle((*handlePtr)->name) == NULL) {
            status = NameServer_E_INVALIDARG;
            GT_setFailureReason(curTrace, GT_4CLASS, "NameServer_close",
                status, "handle not found");
        }
        else {
            /* TODO: add refCount management */
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "NameServer_close:", status);

    return(status);
}

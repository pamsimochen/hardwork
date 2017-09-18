/*
 *  @file   Ipc.c
 *
 *  @brief  This module is primarily used to configure IPC-wide settings and
 *           initialize IPC at runtime
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



/*  ----------------------------------- Standard headers   */
#if defined(SYSLINK_BUILDOS_LINUX)
#include <linux/string.h>
#else
#include <string.h>
#endif

#include <ti/syslink/Std.h>

/*  ----------------------------------- SysLink IPC module Headers   */
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/Notify.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/GateMP.h>
#include <ti/ipc/Ipc.h>
#include <ti/syslink/IpcHost.h>
#include <ti/syslink/inc/NameServerRemoteNotify.h>
#include <ti/syslink/inc/_GateMP.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/inc/_Notify.h>
#include <ti/syslink/inc/_MessageQ.h>
#include <ti/syslink/inc/_Ipc.h>
#include <ti/syslink/inc/knl/IpcKnl.h>
#include <ti/syslink/inc/knl/Platform.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/inc/knl/NotifySetupProxy.h>
#include <ti/syslink/inc/knl/TransportSetupProxy.h>

/*  ----------------------------------- SysLink utils Headers   */
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/inc/_NameServer.h>


/* =============================================================================
 * Macros
 * =============================================================================
 */
/* Macro to make a correct module magic number with refCount */
#define Ipc_MAKE_MAGICSTAMP(x) ((SharedRegion_MODULEID << 16u) | (x))

/* flag for starting processor synchronization */
#define Ipc_PROCSYNCSTART    1

/* flag for finishing processor synchronization */
#define Ipc_PROCSYNCFINISH   2

/* flag for detaching */
#define Ipc_PROCSYNCDETACH   3

/* =============================================================================
 * Enums & Structures
 * =============================================================================
 */

/* The structure used for reserving memory in SharedRegion */
typedef struct Ipc_Reserved {
    volatile Bits32    startedKey;
    SharedRegion_SRPtr notifySRPtr;
    SharedRegion_SRPtr nsrnSRPtr;
    SharedRegion_SRPtr transportSRPtr;
    SharedRegion_SRPtr configListHead;
} Ipc_Reserved;

/*!
 *   This structure captures Configuration details of a module/instance
 *   written by a slave to synchronize with a remote slave/HOST
 */
typedef struct Ipc_ConfigEntry {
    volatile Bits32 remoteProcId;
    /* Remote processor identifier */
    volatile Bits32 localProcId;
    /* Config Entry owner processor identifier */
    volatile Bits32 tag;
    /* Unique tag to distinguish config from other config entries */
    volatile Bits32 size;
    /* Size of the config pointer */
    volatile Bits32 next;
    /* Address of next config entry  (In SRPtr format) */
} Ipc_ConfigEntry;

/*
 *  This structure defines the fields that are to be configured
 *  between the executing processor and a remote processor.
 */
typedef struct Ipc_Entry {
    UInt16 remoteProcId;            /* the remote processor id   */
    Bool   setupNotify;             /* whether to setup Notify   */
    Bool   setupMessageQ;           /* whether to setup MessageQ */
} Ipc_Entry;

/*!
 *   Ipc instance structure.
 */
typedef struct Ipc_ProcEntry {
    SharedRegion_SRPtr *    localConfigList;
    SharedRegion_SRPtr *    remoteConfigList;
    Bool                    slave;
    Ipc_Entry               entry;
    UInt16                  isAttached;
    UInt16                  isNotifyAttached;
} Ipc_ProcEntry;

/*!
 *  Module state structure
 */
typedef struct Ipc_Module_State {
    Int32         refCount;
    Atomic        startRefCount;
    Ptr           ipcSharedAddr;
    Ptr           gateMPSharedAddr;
    Ipc_ProcSync  procSync;
    Ipc_Config    cfg;
    Ipc_ProcEntry procEntry [MultiProc_MAXPROCESSORS];
} Ipc_Module_State;


/* =============================================================================
 * Forward declaration
 * =============================================================================
 */
/*!
 *  ======== getMasterAddr () ========
 */
Ptr Ipc_getMasterAddr (UInt16 remoteProcId, Ptr sharedAddr);

/*!
 *  ======== getRegion0ReservedSize ========
 *  Returns the amount of memory to be reserved for Ipc in SharedRegion 0.
 *
 *  This is used for synchronizing processors.
 */
SizeT Ipc_getRegion0ReservedSize (Void);

/*!
 *  ======== getSlaveAddr () ========
 */
Ptr Ipc_getSlaveAddr (UInt16 remoteProcId, Ptr sharedAddr);

/*!
 *  ======== procSyncStart ========
 *  Starts the process of synchronizing processors.
 *
 *  Shared memory in region 0 will be used.  The processor which owns
 *  SharedRegion 0 writes its reserve memory address in region 0
 *  to let the other processors know it has started.  It then spins
 *  until the other processors start.  The other processors write their
 *  reserve memory address in region 0 to let the owner processor
 *  know they've started.  The other processors then spin until the
 *  owner processor writes to let them know its finished the process
 *  of synchronization before continuing.
 */
Int Ipc_procSyncStart (UInt16 remoteProcId, Ptr sharedAddr);

/*!
 *  ======== procSyncFinish ========
 *  Finishes the process of synchronizing processors.
 *
 *  Each processor writes its reserve memory address in SharedRegion 0
 *  to let the other processors know its finished the process of
 *  synchronization.
 */
Int Ipc_procSyncFinish (UInt16 remoteProcId, Ptr sharedAddr);

/*!
 *  ======== reservedSizePerProc ========
 *  The amount of memory required to be reserved per processor.
 */
SizeT Ipc_reservedSizePerProc (Void);


/* =============================================================================
 * Globals
 * =============================================================================
 */
static Ipc_Module_State Ipc_module_state = {
                                             .procSync = Ipc_ProcSync_ALL,
                                             .refCount = 0,
                                           };
static Ipc_Module_State * Ipc_module = &Ipc_module_state;


/* =============================================================================
 * APIs
 * =============================================================================
 */
/* attaches to a remote processor */
Int Ipc_attach (UInt16 remoteProcId)
{
    Int                     status = 0;
    SizeT                   reservedSize = Ipc_reservedSizePerProc ();
    Bool                    cacheEnabled = SharedRegion_isCacheEnabled (0);
    Ptr                     notifySharedAddr;
    Ptr                     msgqSharedAddr;
    Ptr                     nsrnSharedAddr;
    volatile Ipc_Reserved * slave;
    Ipc_ProcEntry *         ipc;
    UInt32                  memReq;
    SharedRegion_Entry      entry;
    IArg                    key;

    GT_1trace (curTrace, GT_ENTER, "Ipc_attach", remoteProcId);

    GT_assert(curTrace,remoteProcId < MultiProc_INVALIDID);
    GT_assert(curTrace,remoteProcId != MultiProc_self());

    key = Gate_enterSystem ();
   /* Make sure its not already attached */
    if (Ipc_module->procEntry[remoteProcId].isAttached) {
        Ipc_module->procEntry[remoteProcId].isAttached++;
        Gate_leaveSystem (key);
        status = Ipc_S_ALREADYSETUP;
    }
    else {
        Gate_leaveSystem (key);
            /* get region 0 information */
        SharedRegion_getEntry(0, &entry);

        /* Make sure we've attached to owner of SR0 if we're not owner */
        if ((MultiProc_self() != entry.ownerProcId) &&
            (remoteProcId != entry.ownerProcId) &&
            !(Ipc_module->procEntry[entry.ownerProcId].isAttached)) {
            status = Ipc_E_FAIL;
        }
        else {
            /* determine if self is master or slave */
            if  (MultiProc_self () < remoteProcId) {
                Ipc_module->procEntry[remoteProcId].slave = TRUE;
            }
            else {
                Ipc_module->procEntry[remoteProcId].slave = FALSE;
            }

            /* determine the slave's slot */
            slave = Ipc_getSlaveAddr (remoteProcId, Ipc_module->ipcSharedAddr);

            if  (cacheEnabled) {
                Cache_inv ( (Ptr)slave, reservedSize, Cache_Type_ALL, TRUE);
            }

            /* get the attach params associated with remoteProcId */
            ipc = &(Ipc_module->procEntry[remoteProcId]);

            /* Synchronize the processors. */
            status = Ipc_procSyncStart (remoteProcId, Ipc_module->ipcSharedAddr);
            if  (status < 0) {
                /* Do not set failure reason here, since this is a valid run-time
                 * failure.
                 */
                GT_0trace (curTrace,
                           GT_2CLASS,
                           "Ipc_attach: Ipc_procSyncStart failed!");
            }

            if (status >= 0) {
                /* must be called before SharedRegion_attach */
                status =
                     GateMP_attach (remoteProcId, Ipc_module->gateMPSharedAddr);
                if  (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "Ipc_attach",
                                         Ipc_E_FAIL,
                                         "GateMP_attach failed!");
                }
            }

            /* retrieves the SharedRegion Heap handles */
            if (status >= 0) {
                status = SharedRegion_attach (remoteProcId);
                if  (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "Ipc_attach",
                                         Ipc_E_FAIL,
                                         "SharedRegion_attach failed!");
                }
            }

            /* attach Notify if not yet attached and specified to set internal setup */
            if  (! (Notify_intLineRegistered (remoteProcId, 0)) &&
                 (ipc->entry.setupNotify)) {
                /* call Notify_attach */
                memReq = Notify_sharedMemReq(remoteProcId,
                                             Ipc_module->ipcSharedAddr);
                if (memReq != 0) {
                    if  (MultiProc_self () < remoteProcId) {
                        /*
                         *  calloc required here due to race condition.
                         *  Its possible that the slave, who creates the instance,
                         *  tries a sendEvent before the master has created its instance
                         *  because the state of memory was enabled from a previous run.
                         */
                        notifySharedAddr =
                                        Memory_calloc (SharedRegion_getHeap (0),
                                                  memReq,
                                                  SharedRegion_getCacheLineSize (0),
                                                  NULL);

                        if (notifySharedAddr == NULL) {
                            status = Ipc_E_MEMORY;
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "Ipc_attach",
                                                 status,
                                                 "Memory_calloc failed!");
                        }
                        else {
                            /* if cache enabled, wbInv the calloc above */
                            if (cacheEnabled) {
                                Cache_wbInv(notifySharedAddr, memReq,
                                            Cache_Type_ALL, TRUE);
                            }

                            slave->notifySRPtr = SharedRegion_getSRPtr (
                                                                   notifySharedAddr,
                                                                   0);
                            if (slave->notifySRPtr == SharedRegion_INVALIDSRPTR) {
                                status = Ipc_E_FAIL;
                                GT_setFailureReason (curTrace,
                                                     GT_4CLASS,
                                                     "Ipc_attach",
                                                     status,
                                                     "SharedRegion_getSRPtr failed!");
                            }
                        }
                    }
                    else {
                        notifySharedAddr = SharedRegion_getPtr (slave->notifySRPtr);
                        if (notifySharedAddr == NULL) {
                            status = Ipc_E_FAIL;
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "Ipc_attach",
                                                 status,
                                                 "SharedRegion_getPtr failed!");
                        }
                    }
                }
                else {
                    notifySharedAddr = NULL;
                    slave->notifySRPtr = 0;
                }
                if (status >= 0) {
                    status = Notify_attach (remoteProcId, notifySharedAddr);
                    if  (status < 0) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "Ipc_attach",
                                             Ipc_E_FAIL,
                                             "Notify_attach failed!");
                        if (   (MultiProc_self () < remoteProcId)
                            && (notifySharedAddr != NULL)) {
                            /* free the memory back to SharedRegion 0 heap */
                            Memory_free(SharedRegion_getHeap(0),
                                        notifySharedAddr,
                                        memReq);
                        }
                    }
                    else {
                        Ipc_module->procEntry[remoteProcId].isNotifyAttached =
                                                                           TRUE;
                    }
                }
            }

            /* Must come after GateMP_start because depends on default GateMP */
            if  (ipc->entry.setupNotify) {
                memReq = NameServerRemoteNotify_sharedMemReq(NULL);
                if  (memReq != 0) {
                    if  (MultiProc_self () < remoteProcId) {
                        nsrnSharedAddr = Memory_alloc (SharedRegion_getHeap (0),
                                                      memReq,
                                                      SharedRegion_getCacheLineSize (0),
                                                      NULL);

                        slave->nsrnSRPtr = SharedRegion_getSRPtr (nsrnSharedAddr, 0);
                        if (slave->nsrnSRPtr == SharedRegion_INVALIDSRPTR) {
                            status = Ipc_E_FAIL;
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "Ipc_attach",
                                                 status,
                                                 "SharedRegion_getSRPtr failed!");
                        }
                    }
                    else {
                        nsrnSharedAddr = SharedRegion_getPtr (slave->nsrnSRPtr);
                        if (nsrnSharedAddr == NULL) {
                            status = Ipc_E_FAIL;
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "Ipc_attach",
                                                 status,
                                                 "SharedRegion_getPtr failed!");
                        }
                    }
                }
                else {
                    nsrnSharedAddr = NULL;
                    slave->nsrnSRPtr = 0;
                }

                if (status >= 0) {
                    /* create the NameServerRemoteNotify instances */
                    status = NameServerRemoteNotify_attach (remoteProcId, nsrnSharedAddr);
                    if  (status < 0) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "Ipc_attach",
                                             Ipc_E_FAIL,
                                             "NameServerRemoteNotify_attach failed!");
                        if (   (MultiProc_self () < remoteProcId)
                            && (nsrnSharedAddr != NULL)) {
                            /* free the memory back to SharedRegion 0 heap */
                            Memory_free(SharedRegion_getHeap(0),
                                        nsrnSharedAddr,
                                        memReq);
                        }
                    }
                }
            }

            /* Must come after GateMP_start because depends on default GateMP */
            if (!(MessageQ_SetupTransportProxy_isRegistered(remoteProcId)) &&
                (ipc->entry.setupMessageQ)) {
                memReq = MessageQ_SetupTransportProxy_sharedMemReq (
                    Ipc_module->ipcSharedAddr);
                if (memReq != 0) {
                    if  (MultiProc_self () < remoteProcId) {
                        msgqSharedAddr = Memory_alloc (SharedRegion_getHeap (0),
                                                  memReq,
                                                  SharedRegion_getCacheLineSize (0),
                                                  NULL);

                        slave->transportSRPtr = SharedRegion_getSRPtr (
                                                                msgqSharedAddr,
                                                                0);
                        if (slave->transportSRPtr == SharedRegion_INVALIDSRPTR) {
                            status = Ipc_E_FAIL;
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "Ipc_attach",
                                                 status,
                                                 "SharedRegion_getSRPtr failed!");
                        }
                    }
                    else {
                        msgqSharedAddr = SharedRegion_getPtr (
                                                             slave->transportSRPtr);
                        if (msgqSharedAddr == NULL) {
                            status = Ipc_E_FAIL;
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "Ipc_attach",
                                                 status,
                                                 "SharedRegion_getPtr failed!");
                        }
                    }
                }
                else {
                    msgqSharedAddr = NULL;
                    slave->transportSRPtr = 0;
                }

                if (status >= 0) {
                    /* create the MessageQ Transport instances */
                    status = MessageQ_SetupTransportProxy_attach (remoteProcId,
                                                                msgqSharedAddr);
                    if  (status < 0) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "Ipc_attach",
                                             Ipc_E_FAIL,
                                             "Notify_attach failed!");
                        if (   (MultiProc_self () < remoteProcId)
                            && (msgqSharedAddr != NULL)) {
                            /* free the memory back to SharedRegion 0 heap */
                            Memory_free(SharedRegion_getHeap(0),
                                        msgqSharedAddr,
                                        memReq);
                        }
                    }
                }
            }

            if  (cacheEnabled) {
                if  (MultiProc_self () < remoteProcId) {
                    Cache_wbInv ( (Ptr)slave, reservedSize, Cache_Type_ALL, TRUE);
                }
            }

            if (status >= 0) {
                /* Finish the processor synchronization */
                status = Ipc_procSyncFinish (remoteProcId,
                                             Ipc_module->ipcSharedAddr);
                if  (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "Ipc_attach",
                                         Ipc_E_FAIL,
                                         "Notify_attach failed!");
                }
            }

            if (status >= 0) {
                key = Gate_enterSystem ();
                ipc->isAttached++;
                Gate_leaveSystem (key);
            }
        }
    }
    GT_1trace (curTrace, GT_LEAVE, "Ipc_attach", status);

    return  (status);
}


/* detaches from a remote processor */
Int Ipc_detach (UInt16 remoteProcId)
{
    Int                     status = 0;
    SizeT                   reservedSize = Ipc_reservedSizePerProc ();
    UInt16                  numProcessors = MultiProc_getNumProcessors();
    Bool                    cacheEnabled = SharedRegion_isCacheEnabled (0);
    Ptr                     notifySharedAddr;
    Ptr                     nsrnSharedAddr;
    Ptr                     msgqSharedAddr;
    volatile Ipc_Reserved * slave;
    volatile Ipc_Reserved * master;
    Ipc_ProcEntry *         ipc;
    SharedRegion_Entry      entry;
    Int                     i;
    IArg                    key;
    GT_1trace (curTrace, GT_ENTER, "Ipc_detach", remoteProcId);

    key = Gate_enterSystem ();
    if (Ipc_module->procEntry[remoteProcId].isAttached > 1) {
        /* Only detach if attach count reaches 1 */
        Ipc_module->procEntry[remoteProcId].isAttached--;
        Gate_leaveSystem (key);
        status = Ipc_S_BUSY;
    }
    else if (Ipc_module->procEntry[remoteProcId].isAttached == 0) {
        /* If already detached, then return fail */
        Gate_leaveSystem (key);
        status = Ipc_E_FAIL;
    }
    else {
        Gate_leaveSystem (key);
        /* get region 0 information */
        SharedRegion_getEntry(0, &entry);

        /* Make sure we detach from all other procs before owner of SR 0 */
        if (remoteProcId == entry.ownerProcId) {
            for (i = 0; i < numProcessors; i++) {
                if ((i != MultiProc_self()) && (i != entry.ownerProcId) &&
                    (Ipc_module->procEntry[i].isAttached)) {
                    status = Ipc_E_FAIL;
                }
            }
        }
        /* get the params associated with remoteProcId */
        ipc = & (Ipc_module->procEntry[remoteProcId]);

        if (ipc->isAttached == FALSE) {
            status = Ipc_E_INVALIDSTATE;
            GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "Ipc_detach",
                                    status,
                                    "Ipc not attached!");
        }
        else {
            /* determine the slave's slot */
            slave = Ipc_getSlaveAddr (remoteProcId, Ipc_module->ipcSharedAddr);
            /* determine the master's slot */
            master = Ipc_getMasterAddr(remoteProcId, Ipc_module->ipcSharedAddr);


            if (EXPECT_FALSE (cacheEnabled)) {
                Cache_inv((Ptr)master, reservedSize, Cache_Type_ALL, TRUE);
            }

            if (MultiProc_self () < remoteProcId) {
                /* check to make sure master is not trying to attach */
                if (master->startedKey == Ipc_PROCSYNCSTART) {
                    status = Ipc_E_NOTREADY;
                }
            }
            else {
                /* check to make sure slave is not trying to attach */
                if (slave->startedKey == Ipc_PROCSYNCSTART) {
                    status = Ipc_E_NOTREADY;
                }
            }
            /* The slave processor waits for master to finish its detach sequence */
            if (MultiProc_self() < remoteProcId) {
                if (master->startedKey != Ipc_PROCSYNCDETACH) {
                    status = Ipc_E_NOTREADY;
                }
            }

            if (status >= 0) {
                if (   ipc->entry.setupMessageQ
                    && MessageQ_SetupTransportProxy_isRegistered (remoteProcId)) {
                    /* call MessageQ_detach for remote processor */
                    status = MessageQ_SetupTransportProxy_detach (remoteProcId);
                    if  (status < 0) {
                        GT_setFailureReason (curTrace,
                                            GT_4CLASS,
                                            "Ipc_detach",
                                            Ipc_E_FAIL,
                                            "MessageQ_detach failed!");
                    }

                    if (slave->transportSRPtr) {
                        /* free the memory if slave processor */
                        if  (MultiProc_self () < remoteProcId) {
                            /* get the pointer to MessageQ transport instance */
                            msgqSharedAddr = SharedRegion_getPtr (
                                                         slave->transportSRPtr);

                            if (msgqSharedAddr != NULL) {
                                /* free the memory back to SharedRegion 0 heap */
                                Memory_free (SharedRegion_getHeap (0),
                                    msgqSharedAddr,
                                    MessageQ_SetupTransportProxy_sharedMemReq (
                                    msgqSharedAddr));
                            }

                            /* set the pointer for MessageQ transport instance
                             * back to invalid
                             */
                            slave->transportSRPtr = SharedRegion_INVALIDSRPTR;
                        }
                    }
                }

                if (   ipc->entry.setupNotify
                    && NameServer_isRegistered (remoteProcId)) {
                    /* call NameServerRemoteNotify_detach for remote processor */
                    status = NameServerRemoteNotify_detach (remoteProcId);
                    if  (status < 0) {
                        GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "Ipc_detach",
                                        Ipc_E_FAIL,
                                        "NameServerRemoteNotify_detach failed!");
                    }

                    if (slave->nsrnSRPtr) {
                        /* free the memory if slave processor */
                        if  (MultiProc_self () < remoteProcId) {
                            /* get the pointer to NSRN instance */
                            nsrnSharedAddr = SharedRegion_getPtr (
                                                            slave->nsrnSRPtr);

                            if (nsrnSharedAddr != NULL) {
                                /* free the memory back to SharedRegion 0 heap */
                                Memory_free (SharedRegion_getHeap (0),
                                    nsrnSharedAddr,
                                    NameServerRemoteNotify_sharedMemReq (NULL));
                            }

                            /* set the pointer for NSRN instance back to invalid */
                            slave->nsrnSRPtr = SharedRegion_INVALIDSRPTR;
                        }
                    }
                }

                if (   ipc->entry.setupNotify
                    && Notify_intLineRegistered (remoteProcId, 0)
                    && (Ipc_module->procEntry[remoteProcId].isNotifyAttached
                                                                     == TRUE)) {
                    /* call Notify_detach for remote processor */
                    status = Notify_detach (remoteProcId);
                    if  (status < 0) {
                        GT_setFailureReason (curTrace,
                                            GT_4CLASS,
                                            "Ipc_detach",
                                            Ipc_E_FAIL,
                                            "Notify_detach failed!");
                    }
                    else {
                        Ipc_module->procEntry[remoteProcId].isNotifyAttached =
                                                                         FALSE;
                    }

                    if (slave->notifySRPtr) {
                        /* free the memory if slave processor */
                        if  (MultiProc_self () < remoteProcId) {
                            /* get the pointer to Notify instance */
                            notifySharedAddr = SharedRegion_getPtr (
                                slave->notifySRPtr);

                            if (notifySharedAddr != NULL) {
                                /* free the memory back to SharedRegion 0 heap */
                                Memory_free (SharedRegion_getHeap (0),
                                             notifySharedAddr,
                                             Notify_sharedMemReq (remoteProcId,
                                                 notifySharedAddr));
                            }

                            /* set the pointer for Notify instance back to invalid */
                            slave->notifySRPtr = SharedRegion_INVALIDSRPTR;
                        }
                    }
                }

                if  (MultiProc_self () < remoteProcId) {
                    slave->startedKey = Ipc_PROCSYNCDETACH;
                    slave->configListHead = SharedRegion_INVALIDSRPTR;
                    if (EXPECT_FALSE (cacheEnabled)) {
                        Cache_wbInv ( (Ptr)slave,
                                      reservedSize,
                                      Cache_Type_ALL,
                                      TRUE);
                    }
                }
                else {
                    if (master != NULL) {
                        master->startedKey = Ipc_PROCSYNCDETACH;
                        master->configListHead = SharedRegion_INVALIDSRPTR;
                        if (EXPECT_FALSE (cacheEnabled)) {
                            Cache_wbInv ((Ptr) master,
                                        reservedSize,
                                        Cache_Type_ALL,
                                        TRUE);
                        }

                    }
                }

                /* Now detach the SharedRegion */
                status = SharedRegion_detach (remoteProcId);
                GT_assert (curTrace, (status >= 0));

                /* Now detach the GateMP */
                status = GateMP_detach (remoteProcId, Ipc_module->gateMPSharedAddr);
                GT_assert (curTrace, (status >= 0));
                ipc->isAttached--;
            }
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "Ipc_detach", status);

    return  (status);
}


/*! @brief Function to destroy a Ipc_ instance for a slave
 *
 *  @param      params  Parameters
 *
 *  @sa         Ipc__Params_init/Ipc__create
 */
Int
Ipc_control (UInt16 procId, Int cmdId, Ptr arg)
{
    Int status    = Ipc_S_SUCCESS;

    GT_3trace (curTrace, GT_ENTER, "Ipc_control", procId, cmdId, arg);

    switch (cmdId) {
    case Ipc_CONTROLCMD_LOADCALLBACK:
    {
        status = Platform_loadCallback (procId, arg);
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Ipc_control",
                                 status,
                                 "Platform_loadCallback failed!");
        }
    }
    break;

    case Ipc_CONTROLCMD_STARTCALLBACK:
    {
        status = Platform_startCallback (procId, arg);
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Ipc_control",
                                 status,
                                 "Platform_startCallback failed!");
        }
    }
    break;

    case Ipc_CONTROLCMD_STOPCALLBACK:
    {
        status = Platform_stopCallback (procId, arg);
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Ipc_control",
                                 status,
                                 "Platform_stopCallback failed!");
        }
    }
    break;

    default:
    {
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "Command Id (%x) not supported", cmdId);
    }
    break;
    }

    GT_1trace (curTrace, GT_LEAVE, "Ipc_control", status);

    /*! @retval Ipc_S_SUCCESS Operation was successful */
    return status;
}


/*
 *  ======== Ipc_getMasterAddr ========
 */
Ptr Ipc_getMasterAddr (UInt16 remoteProcId, Ptr sharedAddr)
{
    SizeT                   reservedSize = Ipc_reservedSizePerProc ();
    Int                     slot;
    UInt16                  masterId;
    volatile Ipc_Reserved * master;

    GT_2trace (curTrace, GT_ENTER, "Ipc_getMasterAddr",
               remoteProcId, sharedAddr);

    /* determine the master's procId and slot */
    if  (MultiProc_self () < remoteProcId) {
        masterId = remoteProcId;
        slot = MultiProc_self ();
    }
    else {
        masterId = MultiProc_self ();
        slot = remoteProcId;
    }

    /* determine the reserve address for master between self and remote */
    master =  (Ipc_Reserved *) ( (UInt32)sharedAddr +
              ( (masterId * reservedSize) +
              (slot * sizeof (Ipc_Reserved))));

    GT_1trace (curTrace, GT_LEAVE, "Ipc_getMasterAddr", master);

    return  ( (Ptr)master);
}

/*
 *  ======== Ipc_getRegion0ReservedSize ========
 */
SizeT Ipc_getRegion0ReservedSize(Void)
{
    SizeT reservedSize = Ipc_reservedSizePerProc();

    GT_0trace(curTrace, GT_ENTER, "Ipc_getRegion0ReservedSize");

    /* Calculate the total amount to reserve */
    reservedSize = reservedSize * MultiProc_getNumProcessors();

    GT_1trace(curTrace, GT_LEAVE, "Ipc_getRegion0ReservedSize", reservedSize);

    return (reservedSize);
}

/*
 *  ======== Ipc_getSlaveAddr ========
 */
Ptr Ipc_getSlaveAddr(UInt16 remoteProcId, Ptr sharedAddr)
{
    SizeT                   reservedSize = Ipc_reservedSizePerProc ();
    Int                     slot;
    UInt16                  slaveId;
    volatile Ipc_Reserved * slave;

    GT_2trace (curTrace, GT_ENTER, "Ipc_getSlaveAddr",
               remoteProcId, sharedAddr);

    /* determine the slave's procId and slot */
    if  (MultiProc_self () < remoteProcId) {
        slaveId = MultiProc_self ();
        slot = remoteProcId - 1;
    }
    else {
        slaveId = remoteProcId;
        slot = MultiProc_self () - 1;
    }

    /* determine the reserve address for slave between self and remote */
    slave =  (Ipc_Reserved *) ( (UInt32)sharedAddr +
             ( (slaveId * reservedSize) +
             (slot * sizeof (Ipc_Reserved))));

    GT_1trace (curTrace, GT_LEAVE, "Ipc_getSlaveAddr", slave);

    return  ( (Ptr)slave);
}

/*
 *  ======== Ipc_isAttached ========
 */
Bool Ipc_isAttached(UInt16 remoteProcId)
{
    UInt16 isAttached;

    isAttached = Ipc_module->procEntry[remoteProcId].isAttached;
    return(isAttached > 0 ? TRUE : FALSE);
}

/*
 *  ======== Ipc_procSyncStart ========
 *  The owner of SharedRegion 0 writes to its reserve memory address
 *  in region 0 to let the other processors know it has started.
 *  It then spins until the other processors start.
 *  The other processors write their reserve memory address in
 *  region 0 to let the owner processor know they've started.
 *  The other processors then spin until the owner processor writes
 *  to let them know that its finished the process of synchronization
 *  before continuing.
 */
Int Ipc_procSyncStart (UInt16 remoteProcId, Ptr sharedAddr)
{
    SizeT                   reservedSize = Ipc_reservedSizePerProc ();
    Bool                    cacheEnabled = SharedRegion_isCacheEnabled (0);
    Int                     status = 0;
    volatile Ipc_Reserved * self;
    volatile Ipc_Reserved * remote;
    Ipc_ProcEntry *         ipc;

    GT_2trace (curTrace, GT_ENTER, "Ipc_procSyncStart",
               remoteProcId, sharedAddr);

    /* don't do any synchronization if procSync is NONE */
    if  (Ipc_module->procSync != Ipc_ProcSync_NONE) {
        /* determine self and remote pointers */
        if  (MultiProc_self () < remoteProcId) {
            self = Ipc_getSlaveAddr (remoteProcId, sharedAddr);
            remote = Ipc_getMasterAddr (remoteProcId, sharedAddr);
        }
        else {
            self = Ipc_getMasterAddr (remoteProcId, sharedAddr);
            remote = Ipc_getSlaveAddr (remoteProcId, sharedAddr);
        }

        /* construct the config list */
        ipc = &(Ipc_module->procEntry[remoteProcId]);
        ipc->localConfigList = (SharedRegion_SRPtr *)&self->configListHead;
        ipc->remoteConfigList = (SharedRegion_SRPtr *)&remote->configListHead;
        *ipc->localConfigList = SharedRegion_INVALIDSRPTR;

        if  (cacheEnabled) {
            Cache_wbInv (ipc->localConfigList,
                         reservedSize,
                         Cache_Type_ALL,
                         TRUE);
        }

        if  (MultiProc_self () < remoteProcId) {
            /* set my processor's reserved key to start */
            self->startedKey = Ipc_PROCSYNCSTART;

            /* write back my processor's reserve key */
            if  (cacheEnabled) {
                Cache_wbInv ( (Ptr)self, reservedSize, Cache_Type_ALL, TRUE);
            }

            /* wait for remote processor to start */
            if  (cacheEnabled) {
                Cache_inv ( (Ptr)remote, reservedSize, Cache_Type_ALL, TRUE);
            }

            if  (remote->startedKey != Ipc_PROCSYNCSTART) {
                status = Ipc_E_NOTREADY;
            }
        }
        else {
            /*  wait for remote processor to start */

            if (cacheEnabled) {
                Cache_inv ( (Ptr)remote, reservedSize, Cache_Type_ALL, TRUE);
            }

            if  ( (self->startedKey != Ipc_PROCSYNCSTART) &&
                 (remote->startedKey != Ipc_PROCSYNCSTART)) {
                status = Ipc_E_NOTREADY;
            }

            if (status >= 0) {
                /* set my processor's reserved key to start */
                self->startedKey = Ipc_PROCSYNCSTART;

                /* write my processor's reserve key back */
                if  (cacheEnabled) {
                    Cache_wbInv ((Ptr)self, reservedSize, Cache_Type_ALL, TRUE);
                    /* wait for remote processor to finish */
                    Cache_inv ( (Ptr)remote, reservedSize, Cache_Type_ALL, TRUE);
                }

                if  (remote->startedKey != Ipc_PROCSYNCFINISH) {
                    status = Ipc_E_NOTREADY;
                }
            }
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "Ipc_procSyncStart", status);

    return  (status);
}

/*
 *  ======== Ipc_procSyncFinish ========
 *  Each processor writes its reserve memory address in SharedRegion 0
 *  to let the other processors know its finished the process of
 *  synchronization.
 */
Int Ipc_procSyncFinish (UInt16 remoteProcId, Ptr sharedAddr)
{
    SizeT                   reservedSize = Ipc_reservedSizePerProc ();
    Bool                    cacheEnabled = SharedRegion_isCacheEnabled (0);
    volatile Ipc_Reserved * self;
    volatile Ipc_Reserved *remote;

    GT_2trace (curTrace, GT_ENTER, "Ipc_procSyncFinish",
               remoteProcId, sharedAddr);

    /* don't do any synchronization if procSync is NONE */
    if  (Ipc_module->procSync != Ipc_ProcSync_NONE) {
        /* determine self pointer */
        if  (MultiProc_self () < remoteProcId) {
            self = Ipc_getSlaveAddr (remoteProcId, sharedAddr);
            remote = Ipc_getMasterAddr(remoteProcId, sharedAddr);
        }
        else {
            self = Ipc_getMasterAddr (remoteProcId, sharedAddr);
            remote = Ipc_getSlaveAddr(remoteProcId, sharedAddr);
        }

        /* set my processor's reserved key to finish */
        self->startedKey = Ipc_PROCSYNCFINISH;

        /* write back my processor's reserve key */
        if  (cacheEnabled) {
            Cache_wbInv ( (Ptr)self, reservedSize, Cache_Type_ALL, TRUE);
        }
        /* if slave processor, wait for remote to finish sync */
        if (MultiProc_self () < remoteProcId) {
            /* wait for remote processor to finish */
            do {
                if (cacheEnabled) {
                    Cache_inv((Ptr)remote, reservedSize, Cache_Type_ALL, TRUE);
                }
            } while (remote->startedKey != Ipc_PROCSYNCFINISH &&
                      remote->startedKey != Ipc_PROCSYNCDETACH);
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "Ipc_procSyncFinish", Ipc_S_SUCCESS);

    return  (Ipc_S_SUCCESS);
}

/*!
 *  ======== Ipc_readConfig ========
 */
Int Ipc_readConfig (UInt16 remoteProcId, UInt32 tag, Ptr cfg, SizeT size)
{
    Bool                       cacheEnabled = SharedRegion_isCacheEnabled (0);
    Int                        status = Ipc_E_FAIL;
    volatile Ipc_ConfigEntry * entry;

    GT_4trace (curTrace, GT_ENTER, "Ipc_readConfig",
               remoteProcId, tag, cfg, size);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Ipc_module->refCount == 0) {
        /*! @retval Ipc_E_INVALIDSTATE Module was not initialized */
        status = Ipc_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Ipc_readConfig",
                             status,
                             "Module was not initialized!");
    }
    else if (Ipc_module->procEntry[remoteProcId].isAttached == FALSE) {
        /*! @retval Ipc_E_INVALIDSTATE Ipc not attached */
        status = Ipc_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Ipc_readConfig",
                             status,
                             "Ipc not attached!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if  (cacheEnabled) {
            Cache_inv (Ipc_module->procEntry[remoteProcId].remoteConfigList,
                      SharedRegion_getCacheLineSize (0),
                      Cache_Type_ALL,
                      TRUE);
        }

        entry = (Ipc_ConfigEntry *)
                (*Ipc_module->procEntry[remoteProcId].remoteConfigList);

        while  ( (SharedRegion_SRPtr)entry != SharedRegion_INVALIDSRPTR) {
            entry =  (Ipc_ConfigEntry *)
                    SharedRegion_getPtr ( (SharedRegion_SRPtr)entry);
            if (entry == NULL) {
                status = Ipc_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_attach",
                                     status,
                                     "SharedRegion_getPtr failed!");
            }

            /* Traverse the list to find the tag */
            if  (cacheEnabled) {
                Cache_inv ( (Ptr)entry,
                          size + sizeof (Ipc_ConfigEntry),
                          Cache_Type_ALL,
                          TRUE);
            }

            if  ( (entry->remoteProcId == MultiProc_self ()) &&
                 (entry->localProcId == remoteProcId) &&
                 (entry->tag == tag)) {

                if  (size == entry->size) {
                    memcpy (cfg,
                            (Ptr) ( (UInt32)entry + sizeof (Ipc_ConfigEntry)),
                           entry->size);
                    return  (Ipc_S_SUCCESS);
                }
                else {
                    return  (Ipc_E_FAIL);
                }
            }

            entry =  (Ipc_ConfigEntry *)entry->next;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "Ipc_readConfig", status);

    return  (status);
}

/*
 *  ======== Ipc_reservedSizePerProc ========
 */
SizeT Ipc_reservedSizePerProc(Void)
{
    SizeT reservedSize = sizeof(Ipc_Reserved) * MultiProc_getNumProcessors();
    SizeT cacheLineSize = SharedRegion_getCacheLineSize(0);

    GT_0trace(curTrace, GT_ENTER, "Ipc_reservedSizePerProc");

    /* Calculate amount to reserve per processor */
    if (cacheLineSize > reservedSize) {
        /* Use cacheLineSize if larger than reservedSize */
        reservedSize = cacheLineSize;
    }
    else {
        /* Round reservedSize to cacheLineSize */
        reservedSize = _Ipc_roundup(reservedSize, cacheLineSize);
    }

    GT_1trace(curTrace, GT_LEAVE, "Ipc_reservedSizePerProc", reservedSize);

    return (reservedSize);
}

/*
 *  ======== Ipc_writeConfig ========
 */
Int Ipc_writeConfig(UInt16 remoteProcId, UInt32 tag, Ptr cfg, SizeT size)
{
    Int                 status = Ipc_S_SUCCESS;
    Bool                cacheEnabled;
    Ipc_ConfigEntry *   entry;
    SharedRegion_SRPtr  curSRPtr;
    SharedRegion_SRPtr *prevSRPtr;

    GT_4trace(curTrace, GT_ENTER, "Ipc_writeConfig", remoteProcId, tag, cfg,
            size);

    cacheEnabled = SharedRegion_isCacheEnabled(0);

    if (Ipc_module->refCount == 0) {
        /*! @retval Ipc_E_INVALIDSTATE Module was not initialized */
        status = Ipc_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_writeConfig",
                status, "Module was not initialized!");
    }

    if ((status == Ipc_S_SUCCESS) &&
            (remoteProcId >= MultiProc_MAXPROCESSORS)) {
        /*! @retval Ipc_E_INVALIDARG Argument passed to function is invalid */
        status = Ipc_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_writeConfig",
                status, "remoteProcId is invalid");
    }

    if ((status == Ipc_S_SUCCESS) &&
            (Ipc_module->procEntry[remoteProcId].isAttached == FALSE)) {
        /*! @retval Ipc_E_INVALIDSTATE Ipc not attached */
        status = Ipc_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_writeConfig",
                status, "Ipc not attached!");
    }

    /*  When cfg is NULL, the last memory allocated from a previous
     *  Ipc_writeConfig call with the same remoteProcId, tag, and size
     *  is freed.
     */
    if ((status == Ipc_S_SUCCESS) && (cfg == NULL)) {
        status = Ipc_E_FAIL;

        /* get head of local config list and set prevSRPtr to it */
        prevSRPtr = Ipc_module->procEntry[remoteProcId].localConfigList;

        /* set curSRPtr to first entry in list */
        curSRPtr = *prevSRPtr;

        /* loop through list of config entries until matching entry is found */
        while (curSRPtr != SharedRegion_INVALIDSRPTR) {

            /* convert Ptr associated with curSRPtr */
            entry = (Ipc_ConfigEntry *)SharedRegion_getPtr(curSRPtr);

            /* make sure entry matches remoteProcId, tag, and size */
            if ((entry->remoteProcId == remoteProcId) &&
                    (entry->tag == tag) && (entry->size == size)) {

                /* update the 'prev' next ptr */
                *prevSRPtr = (SharedRegion_SRPtr)(entry->next);

                /* writeback the 'prev' ptr */
                if (cacheEnabled) {
                    Cache_wb(prevSRPtr, sizeof(SharedRegion_SRPtr),
                            Cache_Type_ALL, FALSE);
                }

                /* free entry's memory back to shared heap */
                Memory_free(SharedRegion_getHeap(0), entry,
                        sizeof(Ipc_ConfigEntry) + size);

                /* set the status to success */
                status = Ipc_S_SUCCESS;
                break;
            }

            /* set the 'prev' to the 'cur' SRPtr */
            prevSRPtr = (SharedRegion_SRPtr *)(&entry->next);

            /* point to next config entry */
            curSRPtr = (SharedRegion_SRPtr)(entry->next);
        }

        /* return that status */
        return (status);
    }

    /* allocate the config entry object */
    if (status == Ipc_S_SUCCESS) {
        entry = Memory_alloc(SharedRegion_getHeap(0),
                sizeof(Ipc_ConfigEntry) + size,
                SharedRegion_getCacheLineSize(0), NULL);

        if (entry == NULL) {
            status = Ipc_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_writeConfig",
                    status, "Memory_alloc failed!");
        }
    }

    /* set the entry, writeback the cache */
    if (status == Ipc_S_SUCCESS) {
        entry->remoteProcId = remoteProcId;
        entry->localProcId = MultiProc_self();
        entry->tag = tag;
        entry->size = size;
        memcpy((Ptr)((UInt32)entry + sizeof(Ipc_ConfigEntry)), cfg, size);

        /* point the entry's next to the first entry in the list */
        entry->next = *Ipc_module->procEntry[remoteProcId].localConfigList;

        /* first write-back the entry if cache is enabled */
        if (cacheEnabled) {
            Cache_wbInv(entry, sizeof(Ipc_ConfigEntry) + size, Cache_Type_ALL,
                    FALSE);
        }

        /* set the entry as the new first in the list */
        *Ipc_module->procEntry[remoteProcId].localConfigList =
                SharedRegion_getSRPtr(entry, 0);

        /* write-back the config list */
        if (cacheEnabled) {
            Cache_wbInv(Ipc_module->procEntry[remoteProcId].localConfigList,
                    sizeof(SharedRegion_SRPtr), Cache_Type_ALL, FALSE);
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "Ipc_writeConfig", status);
    return(status);
}

/*
 *  ======== Ipc_start ========
 */
Int Ipc_start (Void)
{
    Int                status = 0;
    Int                i;
    SharedRegion_Entry entry;
    Ptr                ipcSharedAddr;
    Ptr                gateMPSharedAddr;
    GateMP_Params      gateMPParams;

    GT_0trace (curTrace, GT_ENTER, "Ipc_start");

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&(Ipc_module->startRefCount),
                            Ipc_MAKE_MAGICSTAMP(0),
                            Ipc_MAKE_MAGICSTAMP(0));
    if (   Atomic_inc_return (&(Ipc_module->startRefCount))
        != Ipc_MAKE_MAGICSTAMP(1u)) {

        /* Call sharedRegion_start in case platform load call back is for  next core
         * This is to  setup additional shared regions that next core might  have configured
         */
        status = Ipc_S_SUCCESS;
    }
    else {
        /* get region 0 information */
        SharedRegion_getEntry (0, &entry);

        /* if entry is not valid then return */
        if  (entry.isValid == FALSE) {
            status = Ipc_E_NOTREADY;
            GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_start", status,
                    "entry.isValid == FALSE!");
        }
        else {
            /*
             *  Need to reserve memory in region 0 for processor synchronization.
             *  This must done before SharedRegion_start().
             */
            ipcSharedAddr = SharedRegion_reserveMemory(0,
                    Ipc_getRegion0ReservedSize());

            /* must reserve memory for GateMP before SharedRegion_start() */
            gateMPSharedAddr = SharedRegion_reserveMemory(0,
                    GateMP_getRegion0ReservedSize());

            /* Init params for default gate (must match those in GateMP_start()) */
            GateMP_Params_init(&gateMPParams);
            gateMPParams.localProtect  = GateMP_LocalProtect_TASKLET;

            if (MultiProc_getNumProcessors () > 1) {
                gateMPParams.remoteProtect = GateMP_RemoteProtect_SYSTEM;
            }
            else {
                gateMPParams.remoteProtect = GateMP_RemoteProtect_NONE;
            }

            /* reserve memory for default gate before SharedRegion_start () */
            SharedRegion_reserveMemory(0, GateMP_sharedMemReq (&gateMPParams));

            /* clear the reserved memory */
            SharedRegion_clearReservedMemory();

            /* Set shared addresses */
            Ipc_module->ipcSharedAddr = ipcSharedAddr;
            Ipc_module->gateMPSharedAddr = gateMPSharedAddr;

            /* create default GateMP, must be called before SharedRegion start */
            status = GateMP_start(Ipc_module->gateMPSharedAddr);
            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_start", status,
                        "GateMP_start failed!");
            }
            else {
                /* create HeapMemMP in each SharedRegion */
                status = SharedRegion_start();
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "Ipc_start",
                                         status,
                                         "SharedRegion_start failed!");
                }
                else {
                    /* Call attach for all procs if procSync is ALL */
                    if  (Ipc_module->procSync == Ipc_ProcSync_ALL) {
                        /* Must attach to owner first to get default GateMP and
                         * HeapMemMP */
                        if (MultiProc_self() != entry.ownerProcId) {
                            do {
                                status = Ipc_attach(entry.ownerProcId);
                            } while (status == Ipc_E_NOTREADY);
                        }
                        if (status >= 0) {
                            /* Loop to attach to all other processors */
                            for  (i = 0; i < MultiProc_getNumProcessors(); i++) {
                                if  (   (i == MultiProc_self ())
                                     || (i == entry.ownerProcId)) {
                                    continue;
                                }
                                /* Skip the processor if there are no interrupt
                                 * lines to it
                                 */
                                if (Notify_numIntLines(i) == 0) {
                                    continue;
                                }
                                /* call Ipc_attach for every remote processor */
                                do {
                                    status = Ipc_attach(i);
                                } while (status == Ipc_E_NOTREADY);
                            }
                        }
                    }
                }
            }
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "Ipc_start", status);

    return  (status);
}


/*
 *  ======== Ipc_stop ========
 */
Int Ipc_stop (Void)
{
    Int                status = Ipc_S_SUCCESS;
    Int                tmpStatus = Ipc_S_SUCCESS;
    SharedRegion_Entry entry;
    GateMP_Params      gateMPParams;

    GT_0trace (curTrace, GT_ENTER, "Ipc_stop");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (EXPECT_FALSE (  Atomic_cmpmask_and_lt (&(Ipc_module->startRefCount),
                                   Ipc_MAKE_MAGICSTAMP(0),
                                   Ipc_MAKE_MAGICSTAMP(1)) == TRUE) ) {
        /*! @retval Ipc_E_FAIL Ipc_start not called */
        status = Ipc_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Ipc_stop",
                             status,
                             "Ipc_start not called!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (EXPECT_TRUE (   Atomic_dec_return (&Ipc_module->startRefCount)
                         == Ipc_MAKE_MAGICSTAMP(0))) {
            /* get region 0 information */
            SharedRegion_getEntry (0, &entry);

            /* if entry is not valid then return */
            if (entry.isValid == FALSE) {
                status = Ipc_E_FAIL;
                GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "Ipc_stop",
                                    status,
                                    "entry.isValid == FALSE!");
            }
            else {
                /*
                *  Need to unreserve memory in region 0 for processor
                *  synchronization. This must done before SharedRegion_stop ().
                */
                SharedRegion_unreserveMemory
                                    (0,
                                    Ipc_getRegion0ReservedSize ());

                /* must unreserve memory for GateMP before SharedRegion_stop () */
                SharedRegion_unreserveMemory
                                (0,
                                    GateMP_getRegion0ReservedSize ());

                /* Init params for default gate
                 * (must match those in GateMP_stop ()
                 */
                GateMP_Params_init (&gateMPParams);
                gateMPParams.localProtect  = GateMP_LocalProtect_TASKLET;

                if  (MultiProc_getNumProcessors () > 1) {
                    gateMPParams.remoteProtect = GateMP_RemoteProtect_SYSTEM;
                }
                else {
                    gateMPParams.remoteProtect = GateMP_RemoteProtect_NONE;
                }

                /* unreserve memory for default gate before
                 * SharedRegion_stop ()
                 */
                SharedRegion_unreserveMemory (0,
                                          GateMP_sharedMemReq (&gateMPParams));

                /* Delete HeapMemMP in each SharedRegion */
                status = SharedRegion_stop ();
                if (status < 0) {
                    status = Ipc_E_FAIL;
                    GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "Ipc_stop",
                                        status,
                                        "SharedRegion_stop failed!");
                }

                /* delete default GateMP, must be called before
                 * SharedRegion stop
                 */
                tmpStatus = GateMP_stop ();
                if  ((tmpStatus < 0) && (status >= 0)) {
                    status = Ipc_E_FAIL;
                    GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "Ipc_stop",
                                        status,
                                        "GateMP_stop failed!");
                }

                Ipc_module->gateMPSharedAddr = NULL;
                Ipc_module->ipcSharedAddr    = NULL;
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "Ipc_stop", status);

    return (status);
}


/*
 *  ======== Ipc_getConfig ========
 */
Void Ipc_getConfig (Ipc_Config * cfgParams)
{
    IArg key;

    GT_1trace (curTrace, GT_ENTER, "Ipc_getConfig", cfgParams);

    GT_assert (curTrace, (cfgParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfgParams == NULL) {
        /* No retVal since this is a Void function. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Ipc_getConfig",
                             Ipc_E_INVALIDARG,
                             "Argument of type (Ipc_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = Gate_enterSystem ();
        if (Ipc_module->refCount == 0) {
            cfgParams->procSync = Ipc_ProcSync_ALL;
        }
        else {
            Memory_copy ((Ptr) cfgParams,
                         (Ptr) &Ipc_module->cfg,
                         sizeof (Ipc_Config));
        }
        Gate_leaveSystem (key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "Ipc_getConfig");
}


/* Sets up Ipc for this processor. */
Int Ipc_setup (const Ipc_Config * cfg)
{
    Int             status = Ipc_S_SUCCESS;
    Ipc_Config      tmpCfg;
    IArg            key;
    Int             i;

    GT_1trace (curTrace, GT_ENTER, "Ipc_setup", cfg);

    if (cfg == NULL) {
        Ipc_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    key = Gate_enterSystem ();
    Ipc_module->refCount++;

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    if (Ipc_module->refCount > 1) {
        status = Ipc_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "Ipc Module already initialized!");
        Gate_leaveSystem (key);
    }
    else {
        Gate_leaveSystem (key);
        /* Copy the cfg */
        Memory_copy ((Ptr) &Ipc_module->cfg,
                     (Ptr) cfg,
                     sizeof (Ipc_Config));

        Ipc_module->procSync = cfg->procSync;

        status = Platform_setup ();
        if (status < 0) {
            key = Gate_enterSystem ();
            Ipc_module->refCount--;
            Gate_leaveSystem (key);
            status = Ipc_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Ipc_setup",
                                 status,
                                 "Platform_setup failed!");
        }

        /* Following can be done regardless of status */
        for (i = 0; i < MultiProc_getNumProcessors (); i++) {
            Ipc_module->procEntry [i].isAttached = FALSE;
            Ipc_module->procEntry[i].isNotifyAttached = FALSE;
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "Ipc_setup", status);

    /*! @retval Ipc_S_SUCCESS Operation successful */
    return status;
}


/* Destroys Ipc for this processor. */
Int
Ipc_destroy (Void)
{
    Int status = Ipc_S_SUCCESS;
    IArg  key;

    GT_0trace (curTrace, GT_ENTER, "Ipc_destroy");

    key = Gate_enterSystem ();
    Ipc_module->refCount--;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (Ipc_module->refCount < 0) {
        Gate_leaveSystem (key);
        /*! @retval Ipc_E_INVALIDSTATE Module was not initialized */
        status = Ipc_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Ipc_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (Ipc_module->refCount == 0) {
            Gate_leaveSystem (key);
            status = Platform_destroy ();
            if (status < 0) {
                status = Ipc_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "Platform_destroy failed!");
            }
        }
        else {
            Gate_leaveSystem (key);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "Ipc_destroy", status);

    /*! @retval Ipc_S_SUCCESS Operation successful */
    return status;
}

/*
 *  ======== ti_sdo_ipc_Ipc_getEntry ========
 */
Void Ipc_getEntry(Ipc_Entry *entry)
{
    /* Assert that the remoteProc is less than the number of processors */
    GT_assert(curTrace,entry->remoteProcId < MultiProc_getNumProcessors());

    /* Get the setupNotify flag */
    entry->setupNotify =
        Ipc_module->procEntry[entry->remoteProcId].entry.setupNotify;

    /* Get the setupMessageQ flag */
    entry->setupMessageQ =
        Ipc_module->procEntry[entry->remoteProcId].entry.setupMessageQ;
}

/*
 *  ======== Ipc_setEntry ========
 */
Void Ipc_setEntry(Ipc_Entry *entry)
{
    /* Set the setupNotify flag */
    Ipc_module->procEntry[entry->remoteProcId].entry.setupNotify =
        entry->setupNotify;

    /* Set the setupMessageQ flag */
    Ipc_module->procEntry[entry->remoteProcId].entry.setupMessageQ =
        entry->setupMessageQ;
}
/*!
 *  @brief Creates a IPC.
 *
 *  @retval Ipc_Status_ERROR    If operation had errors.
 *  @retval Ipc_Status_FAIL     If operation failed.
 *  @retval Ipc_Status_SUCCESS  If operation passed.
 */
Int Ipc_create (UInt16 remoteProcId, Ipc_Params * params)
{
//    Ipc_module->procEntry[remoteProcId].entry.slave = params->slave;
    Ipc_module->procEntry[remoteProcId].entry.setupMessageQ = params->setupMessageQ;
    Ipc_module->procEntry[remoteProcId].entry.setupNotify = params->setupNotify;
    Ipc_module->procEntry[remoteProcId].entry.remoteProcId = remoteProcId;

    /* Assert that the procSync is same as configured for the module. */
    GT_assert (curTrace, (Ipc_module->procSync == params->procSync));

    return Ipc_S_SUCCESS;
}

/*
 *  @file   KnlUtilsDrv.c
 *
 *  @brief      Linux OS-specific driver module code for Kernel utils
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



/*  Defined to include MACROS EXPORT_SYMBOL. This must be done before including
 *  module.h
 */
#if !defined (EXPORT_SYMTAB)
#define EXPORT_SYMTAB
#endif

/* OS-specific headers */
#include <linux/spinlock.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/pgtable.h>

/* Version Info*/
#include <ti/syslink/inc/Version.h>

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/OsalPrint.h>
#include <ti/syslink/inc/knl/OsalDriver.h>
#include <ti/syslink/inc/knl/OsalEvent.h>
#include <ti/syslink/inc/knl/OsalIsr.h>
#include <ti/syslink/inc/knl/OsalKfile.h>
#include <ti/syslink/inc/knl/OsalMutex.h>
#include <ti/syslink/utils/OsalSemaphore.h>
#include <ti/syslink/inc/knl/OsalThread.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/List.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/GateSpinlock.h>
#include <ti/syslink/RingIO.h>
#include <ti/syslink/RingIOShm.h>
/* Module headers */
#include <ti/syslink/inc/knl/Linux/LinuxClock.h>

#if !defined (SYSLINK_MULTIPLE_MODULES)
/* ProcMgr modules */
#include <ti/syslink/inc/knl/ProcMgrDrv.h>

/* IPC modules */
#include <ti/ipc/Ipc.h>
#include <ti/syslink/inc/knl/Linux/IpcDrv.h>
#include <ti/syslink/inc/_Ipc.h>
#include <ti/syslink/inc/knl/IpcKnl.h>
#include <ti/syslink/inc/knl/Linux/NotifyDrv.h>
#include <ti/syslink/inc/knl/Linux/NameServerDrv.h>
#include <ti/syslink/inc/knl/Linux/SyslinkMemMgrDrv.h>
#include <ti/syslink/inc/knl/Linux/MultiProcDrv.h>
#include <ti/syslink/inc/knl/Linux/NameServerDrv.h>
#include <ti/syslink/inc/knl/Linux/SharedRegionDrv.h>
#include <ti/syslink/inc/knl/Linux/HeapBufMPDrv.h>
#include <ti/syslink/inc/knl/Linux/HeapMemMPDrv.h>
#include <ti/syslink/inc/knl/Linux/ListMPDrv.h>
#include <ti/syslink/inc/knl/Linux/GateMPDrv.h>
#include <ti/syslink/inc/knl/Linux/MessageQDrv.h>
#include <ti/syslink/inc/knl/Linux/FrameQDrv.h>
#include <ti/syslink/inc/knl/Linux/FrameQBufMgrDrv.h>
#include <ti/syslink/inc/knl/Linux/RingIODrv.h>
#include <ti/syslink/inc/knl/Linux/RingIOShmDrv.h>
#include <ti/syslink/inc/knl/Linux/ClientNotifyMgrDrv.h>
#endif /* if !defined (SYSLINK_MULTIPLE_MODULES) */

#if defined(SYSLINK_PLATFORM_OMAP3530)
#include <ti/syslink/inc/knl/omap3530proc.h>
#include <ti/syslink/inc/knl/omap3530pwr.h>
#endif /* if defined(SYSLINK_PLATFORM_OMAP3530) */

#if defined(SYSLINK_PLATFORM_OMAPL1XX)
#include <ti/syslink/inc/knl/omapl1xxproc.h>
#include <ti/syslink/inc/knl/omapl1xxpwr.h>
#endif /* if defined(SYSLINK_PLATFORM_OMAP3530) */

#if defined(SYSLINK_PLATFORM_TI81XX)
#include <ti/syslink/inc/knl/_ArchIpcInt.h>
#include <ti/syslink/inc/knl/ArchIpcInt.h>
#endif /* if defined(SYSLINK_PLATFORM_OMAP3530) */

/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* OsalDriver functions */
EXPORT_SYMBOL(OsalDriver_setup);
EXPORT_SYMBOL(OsalDriver_destroy);
EXPORT_SYMBOL(OsalDriver_registerDriver);
EXPORT_SYMBOL(OsalDriver_unregisterDriver);

/* OsalEvent functions */
EXPORT_SYMBOL(OsalEvent_create);
EXPORT_SYMBOL(OsalEvent_delete);
EXPORT_SYMBOL(OsalEvent_reset);
EXPORT_SYMBOL(OsalEvent_set);
EXPORT_SYMBOL(OsalEvent_wait);

/* OsalIsr functions */
EXPORT_SYMBOL(OsalIsr_create);
EXPORT_SYMBOL(OsalIsr_delete);
EXPORT_SYMBOL(OsalIsr_install);
EXPORT_SYMBOL(OsalIsr_uninstall);
EXPORT_SYMBOL(OsalIsr_disableIsr);
EXPORT_SYMBOL(OsalIsr_enableIsr);
EXPORT_SYMBOL(OsalIsr_disable);
EXPORT_SYMBOL(OsalIsr_restore);
EXPORT_SYMBOL(OsalIsr_getState);
EXPORT_SYMBOL(OsalIsr_inIsr);

/* OsalKfile functions */
EXPORT_SYMBOL(OsalKfile_open);
EXPORT_SYMBOL(OsalKfile_close);
EXPORT_SYMBOL(OsalKfile_read);
EXPORT_SYMBOL(OsalKfile_seek);
EXPORT_SYMBOL(OsalKfile_tell);

/* OsalMutex functions */
EXPORT_SYMBOL(OsalMutex_create);
EXPORT_SYMBOL(OsalMutex_delete);
EXPORT_SYMBOL(OsalMutex_enter);
EXPORT_SYMBOL(OsalMutex_leave);

/* OsalSemaphore functions */
EXPORT_SYMBOL(OsalSemaphore_create);
EXPORT_SYMBOL(OsalSemaphore_delete);
EXPORT_SYMBOL(OsalSemaphore_pend);
EXPORT_SYMBOL(OsalSemaphore_post);

/* OsalThread functions */
EXPORT_SYMBOL(OsalThread_setup);
EXPORT_SYMBOL(OsalThread_destroy);
EXPORT_SYMBOL(OsalThread_create);
EXPORT_SYMBOL(OsalThread_delete);
EXPORT_SYMBOL(OsalThread_disableThread);
EXPORT_SYMBOL(OsalThread_enableThread);
EXPORT_SYMBOL(OsalThread_disable);
EXPORT_SYMBOL(OsalThread_enable);
EXPORT_SYMBOL(OsalThread_activate);
EXPORT_SYMBOL(OsalThread_yield);
EXPORT_SYMBOL(OsalThread_sleep);
EXPORT_SYMBOL(OsalThread_delay);
EXPORT_SYMBOL(OsalThread_inThread);
EXPORT_SYMBOL(OsalThread_waitForThread);

/* OsalMemory functions */
EXPORT_SYMBOL(Memory_alloc);
EXPORT_SYMBOL(Memory_calloc);
EXPORT_SYMBOL(Memory_free);
EXPORT_SYMBOL(MemoryOS_alloc);
EXPORT_SYMBOL(MemoryOS_calloc);
EXPORT_SYMBOL(MemoryOS_free);
EXPORT_SYMBOL(MemoryOS_map);
EXPORT_SYMBOL(MemoryOS_unmap);
EXPORT_SYMBOL(MemoryOS_copy);
EXPORT_SYMBOL(MemoryOS_set);

/* OsalPrint functions */
EXPORT_SYMBOL(Osal_printf);

/* Trace functions */
#if defined (SYSLINK_TRACE_ENABLE)
EXPORT_SYMBOL(_GT_setFailureReason);
EXPORT_SYMBOL(_GT_0trace);
EXPORT_SYMBOL(_GT_1trace);
EXPORT_SYMBOL(_GT_2trace);
EXPORT_SYMBOL(_GT_3trace);
EXPORT_SYMBOL(_GT_4trace);
EXPORT_SYMBOL(_GT_5trace);
EXPORT_SYMBOL(_GT_6trace);
EXPORT_SYMBOL(curTrace);
#endif /* if defined (SYSLINK_TRACE_ENABLE) */

/* String functions */
EXPORT_SYMBOL(String_cat);
EXPORT_SYMBOL(String_cmp);
EXPORT_SYMBOL(String_ncmp);
EXPORT_SYMBOL(String_cpy);
EXPORT_SYMBOL(String_ncpy);
EXPORT_SYMBOL(String_len);
EXPORT_SYMBOL(String_hexToStr);
EXPORT_SYMBOL(String_hash);

/* List functions */
EXPORT_SYMBOL(List_Params_init);
EXPORT_SYMBOL(List_create);
EXPORT_SYMBOL(List_delete);
EXPORT_SYMBOL(List_construct);
EXPORT_SYMBOL(List_destruct);
EXPORT_SYMBOL(List_empty);
EXPORT_SYMBOL(List_get);
EXPORT_SYMBOL(List_put);
EXPORT_SYMBOL(List_next);
EXPORT_SYMBOL(List_prev);
EXPORT_SYMBOL(List_insert);
EXPORT_SYMBOL(List_remove);
EXPORT_SYMBOL(List_elemClear);
EXPORT_SYMBOL(List_putHead);


/* List functions */
EXPORT_SYMBOL(MultiProc_setLocalId);
EXPORT_SYMBOL(MultiProc_getId);
EXPORT_SYMBOL(MultiProc_getName);
EXPORT_SYMBOL(MultiProc_getNumProcessors);
EXPORT_SYMBOL(MultiProc_setup);
EXPORT_SYMBOL(MultiProc_destroy);
EXPORT_SYMBOL(MultiProc_getConfig);
EXPORT_SYMBOL(MultiProc_self);

/* IGateProvider functions */
EXPORT_SYMBOL(IGateProvider_enter);
EXPORT_SYMBOL(IGateProvider_leave);

/* GateMutex functions */
EXPORT_SYMBOL(GateMutex_create);
EXPORT_SYMBOL(GateMutex_delete);

/* GateSpinlock functions */
EXPORT_SYMBOL(GateSpinlock_create);
EXPORT_SYMBOL(GateSpinlock_delete);

#if defined(SYSLINK_PLATFORM_OMAP3530)

EXPORT_SYMBOL(OMAP3530PROC_setup);
EXPORT_SYMBOL(OMAP3530PWR_destroy);
EXPORT_SYMBOL(OMAP3530PROC_Params_init);
EXPORT_SYMBOL(OMAP3530PWR_setup);
EXPORT_SYMBOL(OMAP3530PWR_getConfig);
EXPORT_SYMBOL(OMAP3530PWR_Params_init);
EXPORT_SYMBOL(OMAP3530PWR_delete);
EXPORT_SYMBOL(OMAP3530PROC_destroy);
EXPORT_SYMBOL(OMAP3530PWR_create);
EXPORT_SYMBOL(OMAP3530PROC_delete);
EXPORT_SYMBOL(OMAP3530PROC_create);
EXPORT_SYMBOL(OMAP3530PROC_getConfig);

#endif /* if defined(SYSLINK_PLATFORM_OMAP3530) */

#if defined(SYSLINK_PLATFORM_OMAPL1XX)

EXPORT_SYMBOL(OMAPL1XXPROC_setup);
EXPORT_SYMBOL(OMAPL1XXPWR_destroy);
EXPORT_SYMBOL(OMAPL1XXPROC_Params_init);
EXPORT_SYMBOL(OMAPL1XXPWR_setup);
EXPORT_SYMBOL(OMAPL1XXPWR_getConfig);
EXPORT_SYMBOL(OMAPL1XXPWR_Params_init);
EXPORT_SYMBOL(OMAPL1XXPWR_delete);
EXPORT_SYMBOL(OMAPL1XXPROC_destroy);
EXPORT_SYMBOL(OMAPL1XXPWR_create);
EXPORT_SYMBOL(OMAPL1XXPROC_delete);
EXPORT_SYMBOL(OMAPL1XXPROC_create);
EXPORT_SYMBOL(OMAPL1XXPROC_getConfig);

#endif /* if defined(SYSLINK_PLATFORM_OMAPL1XX) */


#if defined(SYSLINK_PLATFORM_TI81XX)
/* Clock Apis */
EXPORT_SYMBOL(LinuxClock_create);
EXPORT_SYMBOL(LinuxClock_enable);
EXPORT_SYMBOL(LinuxClock_disable);
EXPORT_SYMBOL(LinuxClock_put);
EXPORT_SYMBOL(LinuxClock_setRate);
EXPORT_SYMBOL(LinuxClock_get);

EXPORT_SYMBOL(ArchIpcInt_interruptDisable);
EXPORT_SYMBOL(ArchIpcInt_interruptEnable);
EXPORT_SYMBOL(ArchIpcInt_interruptUnregister);
EXPORT_SYMBOL(ArchIpcInt_sendInterrupt);
EXPORT_SYMBOL(ArchIpcInt_interruptRegister);

#endif /* if defined(SYSLINK_PLATFORM_TI81XX) */


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Module initialization function for Linux driver.
 */
static int __init KnlUtilsDrv_initializeModule (void);


/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit KnlUtilsDrv_finalizeModule (void);

/* Function to initialize the MemoryOS module */
Int MemoryOS_setup (void);

/* Function to Finalize the MemoryOS module */
Int MemoryOS_destroy (void);


/** ============================================================================
 *  Globals
 *  ============================================================================
 */
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool KnlUtilsDrv_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool KnlUtilsDrv_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool KnlUtilsDrv_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt KnlUtilsDrv_traceClass = 0;

#if !defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  OsalDriver handle for ProcMgr
 */
static Ptr KnlUtils_procMgrDrvHandle;

/*!
 *  @brief  OsalDriver handle for Ipc
 */
static Ptr KnlUtils_ipcDrvHandle;

/*!
 *  @brief  OsalDriver handle for Notify
 */
static Ptr KnlUtils_NotifyDrvHandle;

/*!
 *  @brief  OsalDriver handle for MultiProc
 */
static Ptr KnlUtils_MultiProcDrvHandle;

/*!
 *  @brief  OsalDriver handle for NameServer
 */
static Ptr KnlUtils_NameServerDrvHandle;

/*!
 *  @brief  OsalDriver handle for SharedRegion
 */
static Ptr KnlUtils_SharedRegionDrvHandle;

/*!
 *  @brief  OsalDriver handle for HeapBufMP
 */
static Ptr KnlUtils_HeapBufMPDrvHandle;

/*!
 *  @brief  OsalDriver handle for HeapMemMP
 */
static Ptr KnlUtils_HeapMemMPDrvHandle;

/*!
 *  @brief  OsalDriver handle for GateMP
 */
static Ptr KnlUtils_GateMPDrvHandle;
/*!
 *  @brief  OsalDriver handle for MessageQ
 */
static Ptr KnlUtils_MessageQDrvHandle;

/*!
 *  @brief  OsalDriver handle for ListMP
 */
static Ptr KnlUtils_ListMPDrvHandle;


/*!
 *  @brief  OsalDriver handle for SyslinkMemMgr
 */
static Ptr KnlUtils_SyslinkMemMgrDrvHandle;

/*!
 *  @brief  OsalDriver handle for ClientNotifyMgr
 */
static Ptr KnlUtils_ClientNotifyMgrDrvHandle;

/*!
 *  @brief  OsalDriver handle for FrameQBufMgr
 */
static Ptr KnlUtils_FrameQBufMgrDrvHandle;

/*!
 *  @brief  OsalDriver handle for FrameQ
 */
static Ptr KnlUtils_FrameQDrvHandle;

/*!
 *  @brief  OsalDriver handle for RingIO
 */
static Ptr KnlUtils_RingIODrvHandle;

/*!
 *  @brief  OsalDriver handle for RingIO
 */
static Ptr KnlUtils_RingIOShmDrvHandle;
#endif /* if !defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Module initialization  function for Linux driver.
 */
static
int
__init KnlUtilsDrv_initializeModule (void)
{
    int result = 0;
    Ipc_Config iCfg;
    UInt32 traceMask = 0;

    /* Display the version info and created date/time */
    Osal_printf ("SysLink version : %s\nSysLink module created on Date:%s Time:%s\n",
                 SYSLINK_VERSION,
                 __DATE__,
                 __TIME__);

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        KnlUtilsDrv_enableTrace = simple_strtol (TRACE, NULL, 16);
        if ((KnlUtilsDrv_enableTrace != 0) && (KnlUtilsDrv_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (KnlUtilsDrv_enableTrace == TRUE) {
            Osal_printf ("Trace enabled\n");
            traceMask = GT_TraceState_Enable;
        }
        else if (KnlUtilsDrv_enableTrace == FALSE) {
            Osal_printf ("Trace disabled\n");
            traceMask = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        KnlUtilsDrv_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (KnlUtilsDrv_enableTraceEnter != 0)
            &&  (KnlUtilsDrv_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (KnlUtilsDrv_enableTraceEnter == TRUE) {
            Osal_printf ("Trace entry/leave prints enabled\n");
            traceMask |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        KnlUtilsDrv_enableTraceFailure = simple_strtol (TRACEFAILURE, NULL, 16);
        if (    (KnlUtilsDrv_enableTraceFailure != 0)
            &&  (KnlUtilsDrv_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (KnlUtilsDrv_enableTraceFailure == TRUE) {
            Osal_printf ("Trace SetFailureReason enabled\n");
            traceMask |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        KnlUtilsDrv_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (KnlUtilsDrv_traceClass != 0)
            &&  (KnlUtilsDrv_traceClass != 1)
            &&  (KnlUtilsDrv_traceClass != 2)
            &&  (KnlUtilsDrv_traceClass != 3)) {
            Osal_printf ("Error! Only 0/1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            Osal_printf ("Trace class %s\n", TRACECLASS) ;
            KnlUtilsDrv_traceClass =
                           KnlUtilsDrv_traceClass << (32 - GT_TRACECLASS_SHIFT);
            traceMask |= KnlUtilsDrv_traceClass;
        }
    }

    /* Set the desired trace value. */
    GT_setTrace (traceMask, GT_TraceType_Kernel);

    /* Initialize the memoryOS module */
    MemoryOS_setup ();

    /* If a unified single kernel module is being built, need to register all
     * SysLink modules with OsalDriver to create the devices for each.
     */
#if !defined (SYSLINK_MULTIPLE_MODULES)
    /* Initialize the OsalDriver */
    OsalDriver_setup ();

    KnlUtils_ipcDrvHandle = IpcDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (KnlUtils_ipcDrvHandle == NULL) {
        /*! @retval OSALDRIVER_E_FAIL Failed to register Ipc
                                        driver with OS! */
        result = -EFAULT;
        GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "KnlUtilsDrv_initializeModule",
                          OSALDRIVER_E_FAIL,
                          "Failed to register ProcMgr driver with OS!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    /* -------------------------------------------------------------------------
     * ProcMgr modules
     * -------------------------------------------------------------------------
     */
    if (result == 0) {
        KnlUtils_procMgrDrvHandle = ProcMgrDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_procMgrDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register ProcMgr
                                            driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "KnlUtilsDrv_initializeModule",
                              OSALDRIVER_E_FAIL,
                              "Failed to register ProcMgr driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* -------------------------------------------------------------------------
     * IPC modules
     * -------------------------------------------------------------------------
     */

    if (result == 0) {
        KnlUtils_NotifyDrvHandle = NotifyDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_NotifyDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register Notify
                                            driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "KnlUtilsDrv_initializeModule",
                              OSALDRIVER_E_FAIL,
                              "Failed to register Notify driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (result == 0) {
        KnlUtils_MultiProcDrvHandle = MultiProcDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_MultiProcDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register MultiProc driver
             *                            with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "KnlUtilsDrv_initializeModule",
                          OSALDRIVER_E_FAIL,
                          "Failed to register MultiProc driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (result == 0) {
        KnlUtils_NameServerDrvHandle = NameServerDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_NameServerDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register NameServer
                                            driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "KnlUtilsDrv_initializeModule",
                          OSALDRIVER_E_FAIL,
                          "Failed to register NameServer driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (result == 0) {
        KnlUtils_SharedRegionDrvHandle = SharedRegionDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_SharedRegionDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register SharedRegion
                                            driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "KnlUtilsDrv_initializeModule",
                          OSALDRIVER_E_FAIL,
                          "Failed to register SharedRegion driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (result == 0) {
        KnlUtils_HeapBufMPDrvHandle = HeapBufMPDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_HeapBufMPDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register HeapBufMP
                                            driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "KnlUtilsDrv_initializeModule",
                          OSALDRIVER_E_FAIL,
                          "Failed to register HeapBufMP driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (result == 0) {
        KnlUtils_HeapMemMPDrvHandle = HeapMemMPDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_HeapMemMPDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register HeapMemMP
                                            driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "KnlUtilsDrv_initializeModule",
                          OSALDRIVER_E_FAIL,
                          "Failed to register HeapMemMP driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (result == 0) {
        KnlUtils_ListMPDrvHandle = ListMPDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_ListMPDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register ListMP
                                            driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "KnlUtilsDrv_initializeModule",
                          OSALDRIVER_E_FAIL,
                          "Failed to register ListMP driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (result == 0) {
        KnlUtils_GateMPDrvHandle = GateMPDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_GateMPDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register GateMP
                                            driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "KnlUtilsDrv_initializeModule",
                          OSALDRIVER_E_FAIL,
                          "Failed to register GateMP driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
    if (result == 0) {
        KnlUtils_MessageQDrvHandle = MessageQDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_MessageQDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register MessageQ
                                            driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "KnlUtilsDrv_initializeModule",
                          OSALDRIVER_E_FAIL,
                          "Failed to register MessageQ driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }


    if (result == 0) {
        KnlUtils_SyslinkMemMgrDrvHandle = SyslinkMemMgrDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_SyslinkMemMgrDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register NameServer
                                            driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "KnlUtilsDrv_initializeModule",
                          OSALDRIVER_E_FAIL,
                          "Failed to register SyslinkMemMgr driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (result == 0) {
         KnlUtils_ClientNotifyMgrDrvHandle =
                                     ClientNotifyMgrDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_ClientNotifyMgrDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register
                                NameServerRemoteNotify driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                   GT_4CLASS,
                   "KnlUtilsDrv_initializeModule",
                   OSALDRIVER_E_FAIL,
                   "Failed to register ClientNotifyMgr driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

   if (result == 0) {
        KnlUtils_FrameQBufMgrDrvHandle =
                                    FrameQBufMgrDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_FrameQBufMgrDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register
                                NameServerRemoteNotify driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "KnlUtilsDrv_initializeModule",
                                 OSALDRIVER_E_FAIL,
                                 "Failed to register KnlUtils_FrameQBufMgr"
                                 " Handle driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

   if (result == 0) {
        KnlUtils_FrameQDrvHandle = FrameQDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_FrameQDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register
                                NameServerRemoteNotify driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "KnlUtilsDrv_initializeModule",
                                 OSALDRIVER_E_FAIL,
                                 "Failed to register KnlUtils_FrameQDrv Handle"
                                 " driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (result == 0) {
        KnlUtils_RingIODrvHandle = RingIODrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_RingIODrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register
                                NameServerRemoteNotify driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "KnlUtilsDrv_initializeModule",
                                 OSALDRIVER_E_FAIL,
                                 "Failed to register KnlUtils_RingIODrv Handle"
                                 " driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
    if (result == 0) {
        KnlUtils_RingIOShmDrvHandle = RingIOShmDrv_registerDriver ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (KnlUtils_RingIOShmDrvHandle == NULL) {
            /*! @retval OSALDRIVER_E_FAIL Failed to register
                                NameServerRemoteNotify driver with OS! */
            result = -EFAULT;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "KnlUtilsDrv_initializeModule",
                                 OSALDRIVER_E_FAIL,
                                 "Failed to register KnlUtils_RingIOShmDrv Handle"
                                 " driver with OS!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
#endif /* if !defined (SYSLINK_MULTIPLE_MODULES) */

    iCfg.procSync = Ipc_ProcSync_PAIR;
    Ipc_setup (&iCfg);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void
__exit KnlUtilsDrv_finalizeModule (void)
{
    Ipc_destroy ();

    /* If a unified single kernel module is being built, need to unregister all
     * SysLink modules from OsalDriver.
     */
#if !defined (SYSLINK_MULTIPLE_MODULES)
    /* IPC modules */
    MessageQDrv_unregisterDriver (&(KnlUtils_MessageQDrvHandle));
    GateMPDrv_unregisterDriver (&(KnlUtils_GateMPDrvHandle));
    ListMPDrv_unregisterDriver (&(KnlUtils_ListMPDrvHandle));
    HeapMemMPDrv_unregisterDriver (&(KnlUtils_HeapMemMPDrvHandle));
    HeapBufMPDrv_unregisterDriver (&(KnlUtils_HeapBufMPDrvHandle));
    SharedRegionDrv_unregisterDriver (&(KnlUtils_SharedRegionDrvHandle));
    NameServerDrv_unregisterDriver (&(KnlUtils_NameServerDrvHandle));
    MultiProcDrv_unregisterDriver (&(KnlUtils_MultiProcDrvHandle));
    NotifyDrv_unregisterDriver (&(KnlUtils_NotifyDrvHandle));

    FrameQDrv_unregisterDriver (&(KnlUtils_FrameQDrvHandle));
    FrameQBufMgrDrv_unregisterDriver (
                                   &(KnlUtils_FrameQBufMgrDrvHandle));
    RingIODrv_unregisterDriver (&(KnlUtils_RingIODrvHandle));
    RingIOShmDrv_unregisterDriver (&(KnlUtils_RingIOShmDrvHandle));
    ClientNotifyMgrDrv_unregisterDriver (
                                    &(KnlUtils_ClientNotifyMgrDrvHandle));
    SyslinkMemMgrDrv_unregisterDriver(&(KnlUtils_SyslinkMemMgrDrvHandle));


    /* ProcMgr module */
    ProcMgrDrv_unregisterDriver (&(KnlUtils_procMgrDrvHandle));

    IpcDrv_unregisterDriver (&(KnlUtils_ipcDrvHandle));

    /* Finalize the OsalDriver */
    OsalDriver_destroy ();

#endif /* if !defined (SYSLINK_MULTIPLE_MODULES) */

    /* Finalize the memoryOS module */
    MemoryOS_destroy ();

    Osal_printf ("Removed syslink kernel module\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (KnlUtilsDrv_initializeModule);
module_exit (KnlUtilsDrv_finalizeModule);

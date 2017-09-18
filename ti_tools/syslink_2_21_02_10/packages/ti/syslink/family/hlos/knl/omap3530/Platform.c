/*
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
 */

/**
 *  @file   ti/syslink/family/hlos/knl/omap3530/Platform.c
 *
 *  @brief  Implementation of Platform initialization logic.
 */

/* Standard header files */
#include <ti/syslink/Std.h>

/* Utilities & Osal headers */
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/utils/OsalPrint.h>
#include <ti/syslink/inc/knl/OsalThread.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/Cfg.h>

/* SysLink device specific headers */
#include <ti/syslink/inc/knl/omap3530pwr.h>
#include <ti/syslink/inc/knl/omap3530proc.h>
#include <ti/syslink/inc/knl/Omap3530IpcInt.h>

/* Module level headers */
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/ListMP.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/Notify.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/ipc/NameServer.h>
#include <ti/ipc/SharedRegion.h>

#include <ti/syslink/inc/_FrameQ.h>
#include <ti/syslink/ipc/_FrameQBufMgr.h>
#include <ti/syslink/ProcMgr.h>
#include <ti/syslink/inc/_ProcMgr.h>
#include <ti/syslink/inc/knl/Platform.h>
#if defined (SYSLINK_LOADER_ELF)
#include <ti/syslink/inc/ElfLoader.h>
#else
#include <ti/syslink/inc/CoffLoader.h>
#endif

#include <ti/syslink/inc/_Notify.h>    /* for auto-setting of USE_SYSLINK_NOTIFY */

#if defined (USE_SYSLINK_NOTIFY)
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/syslink/inc/NotifyDriverShm.h>
#else
#include <syslink/multiproc.h>
#include <syslink/notify.h>
#include <syslink/_notify.h>
#include <syslink/notify_shm_drv.h>
#endif

#include <ti/syslink/inc/_GateMP.h>
#include <ti/syslink/inc/GatePeterson.h>

#include <ti/syslink/inc/TransportShm.h>
#if defined (SYSLINK_TRANSPORT_SHMNOTIFY)
#include <ti/syslink/inc/TransportShmNotify.h>
#endif
#if defined (SYSLINK_TRANSPORT_SHMCIRC)
#include <ti/syslink/inc/TransportShmCirc.h>
#endif

#include <ti/syslink/inc/NotifyDriverShm.h>
#include <ti/syslink/inc/NotifyDriverCirc.h>
#include <ti/syslink/inc/_NameServer.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/inc/_MessageQ.h>
#include <ti/syslink/inc/_HeapBufMP.h>
#include <ti/syslink/inc/_HeapMemMP.h>
#include <ti/syslink/inc/_ListMP.h>
#include <ti/syslink/inc/NameServerRemote.h>
#include <ti/syslink/inc/NameServerRemoteNotify.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/RingIO.h>
#include <ti/syslink/inc/_RingIO.h>
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/inc/_RingIOShm.h>
#include <ti/syslink/utils/IHeap.h>

#include <ti/ipc/Ipc.h>
#include <ti/syslink/inc/_Ipc.h>
#include <ti/syslink/inc/knl/IpcKnl.h>

/* ProcMgr friend function */
extern ProcMgr_Handle _ProcMgr_getHandle(UInt16 procId);


/** ============================================================================
 *  Macros.
 *  ============================================================================
 */
#define RESETVECTOR_SYMBOL          "_Ipc_ResetVector"


/* This tag is used as an identifier by Ipc_readConfig
 * to get different modules' configuration on slave
 */
#define SLAVE_CONFIG_TAG            0xDADA0000

/* Defines used for timeout value for start/stopCallback. Note that
 * the loop iterates LOOPCNT times but sleeps for SLEEPTIME only once
 * every 4096 iterations. Increase the DETACH_LOOPCNT if the remote
 * processor needs more time to shutdown.
 */
#define ATTACH_LOOPCNT          0x1388000   /* total sleep time = 5 sec */
#define ATTACH_SLEEPTIME        1           /* msec */
#define DETACH_LOOPCNT          0x2BC000    /* total sleep time = 0.7 sec */
#define DETACH_SLEEPTIME        1           /* msec */
#define CFG_LOOPCNT             40          /* total sleep time = 0.4 sec */
#define CFG_SLEEPTIME           10          /* msec */

/*!
 *  @brief  Interrupt ID of physical interrupt handled by the Notify driver to
 *          receive events.
 */
#define BASE_DSP2ARM_INTID          26u

/*!
 *  @brief  Interrupt ID of physical interrupt handled by the Notify driver to
 *          send events.
 */
#define BASE_ARM2DSP_INTID          55u

/** ============================================================================
 *  Application specific configuration, please change these value according to
 *  your application's need.
 *  ============================================================================
 */

/* this type must match the same in ti/syslink/ipc/rtos/Syslink.c */
typedef struct {
    UInt16      sharedRegionNumEntries;
    UInt16      terminateEventLineId;
    UInt32      terminateEventId;
} Syslink_SlaveModuleConfig;

typedef struct {
    UInt16      lineId;
    UInt32      eventId;
} TerminateEventConfig;

 /* This structure is used to get different modules' configuration on host
 * to match it with that of Slaves'
 */
typedef struct Platform_ModuleConfig {
    UInt16      sharedRegionNumEntries;
} Platform_ModuleConfig;

/*!
 *  @brief  Structure defining config parameters for overall System.
 */
typedef struct Platform_Config {
#if defined (USE_SYSLINK_NOTIFY)
    MultiProc_Config                multiProcConfig;
    /*!< Multiproc config parameter */
#else
    struct multiproc_config         multiProcConfig;
#endif
    GateMP_Config                   gateMPConfig;
    /*!< GateMP config parameter */

    GatePeterson_Config             gatePetersonConfig;
    /*!< Gatepeterson config parameter */

    SharedRegion_Config             sharedRegionConfig;
    /*!< SharedRegion config parameter */

    MessageQ_Config                 messageQConfig;
    /*!< MessageQ config parameter */

#if defined (USE_SYSLINK_NOTIFY)
    Notify_Config            notifyConfig;
#else
    struct notify_config            notifyConfig;
    /*!< Notify config parameter */
#endif

    ProcMgr_Config                  procMgrConfig;
    /*!< Processor manager config parameter */

#if defined (SYSLINK_LOADER_ELF)
    ElfLoader_Config                elfLoaderConfig;
    /*!< Elf loader config parameter */
#else
    CoffLoader_Config               coffLoaderConfig;
    /*!< Coff loader config parameter */
#endif

    HeapBufMP_Config                heapBufConfig;
    /*!< Heap Buf config parameter */

    HeapMemMP_Config                heapMemMPConfig;
    /*!< Heap MemMP config parameter */

    ListMP_Config                   listMPConfig;
    /*!<ListMP config parameter */

    TransportShm_Config             messageQTransportShmConfig;
    /*!< TransportShm config parameter */

#if defined (SYSLINK_TRANSPORT_SHMNOTIFY)
    TransportShmNotify_Config       messageQTransportShmNotifyConfig;
    /*!< TransportShmNotify config parameter */
#endif /* if defined (SYSLINK_TRANSPORT_SHMNOTIFY) */

#if defined (SYSLINK_TRANSPORT_SHMCIRC)
    TransportShmCirc_Config         messageQTransportShmCircConfig;
    /*!< TransportShmCirc config parameter */
#endif /* if defined (SYSLINK_TRANSPORT_SHMCIRC) */

    RingIOShm_Config                ringIOShmConfig;
    /*!< RingIOShm config parameter */

    RingIOShm_Params                ringIOShmParams [3];
    /*!< RingIOShm parameter */

#if defined (USE_SYSLINK_NOTIFY)
    NotifyDriverShm_Config          notifyDriverShmConfig;
    /*!< NotifyDriverShm config parameter */
#else
    struct notify_shm_drv_config    notifyDriverShmConfig;
#endif

    NotifyDriverCirc_Config         notifyDriverCircConfig;
    /*!< NotifyDriverCirc config parameter */

    NameServerRemoteNotify_Config   nameServerRemoteNotifyConfig;
    /*!< NameServerRemoteNotify config parameter */

    RingIO_Config                   ringIOConfig;
    /*!< RingIO config parameter */

    ClientNotifyMgr_Config          cliNotifyMgrCfgParams;
    /*!< ClientNotifyMgr config parameter */

    FrameQBufMgr_Config             frameQBufMgrCfgParams;
    /*!< FrameQBufMgr config parameter */

    FrameQ_Config                   frameQCfgParams;
    /*!< FrameQ config parameter */
} Platform_Config;


/* Struct embedded into slave binary */
typedef struct Platform_SlaveConfig {
    UInt32  cacheLineSize;
    UInt32  brOffset;
    UInt32  sr0MemorySetup;
    UInt32  setupMessageQ;
    UInt32  setupNotify;
    UInt32  procSync;
    UInt32  numSRs;
} Platform_SlaveConfig;

/* Shared region configuration */
typedef struct Platform_SlaveSRConfig {
    UInt32 entryBase;
    UInt32 entryLen;
    UInt32 ownerProcId;
    UInt32 id;
    UInt32 createHeap;
    UInt32 cacheLineSize;
} Platform_SlaveSRConfig;

/*! @brief structure for platform instance */
typedef struct Platform_Object {
    /*!< Flag to indicate platform initialization status */
    ProcMgr_Handle                pmHandle;
    /*!< Handle to the ProcMgr instance used */
    OMAP3530PROC_Handle           pHandle;
    /*!< Handle to the Processor instance used */
    OMAP3530PWR_Handle            pwrHandle;
    /*!< Handle to the PwrMgr instance used */
#if defined (SYSLINK_LOADER_ELF)
    ElfLoader_Handle              ldrHandle;
    /*!< Handle to the Loader instance used */
#else /* COFF */
    CoffLoader_Handle             ldrHandle;
    /*!< Handle to the Loader instance used */
#endif
    Platform_SlaveConfig          slaveCfg;
    /*!< Slave embedded config */
    Platform_SlaveSRConfig *      slaveSRCfg;
    /*!< Shared region details from slave */
} Platform_Object, *Platform_Handle;


/*! @brief structure for platform instance */
typedef struct Platform_Module_State {
    Bool              multiProcInitFlag;
    /*!< MultiProc Initialize flag */
    Bool              gateMPInitFlag;
    /*!< GateMP Initialize flag */
    Bool              gatePetersonInitFlag;
    /*!< Gatepeterson Initialize flag */
    Bool              sharedRegionInitFlag;
    /*!< SHAREDREGION Initialize flag */
    Bool              listMpInitFlag;
    /*!< ListMP Initialize flag */
    Bool              messageQInitFlag;
    /*!< MessageQ Initialize flag */
    Bool              ringIOInitFlag;
    /*!< ringIO Initialize flag */
    Bool              notifyInitFlag;
    /*!< Notify Initialize flag */
    Bool              procMgrInitFlag;
    /*!< Processor manager Initialize flag */
    Bool              loaderInitFlag;
    /*!< Loader initialize flag */
    Bool              heapBufInitFlag;
    /*!< HeapBufMP Initialize flag */
    Bool              heapMemMPInitFlag;
    /*!< HeapMemMP Initialize flag */
    Bool              nameServerInitFlag;
    /*!< nameServerRemoteNotify Initialize flag */
    Bool              listMPInitFlag;
    /*!< LISTMPSHAREDMEMORY Initialize flag */
    Bool              messageQTransportShmInitFlag;
    /*!< messageQTransportShm Initialize flag */
#if defined (SYSLINK_TRANSPORT_SHMNOTIFY)
    Bool              messageQTransportShmNotifyInitFlag;
    /*!< messageQTransportShmNotify Initialize flag */
#endif /* if defined (SYSLINK_TRANSPORT_SHMNOTIFY) */
#if defined (SYSLINK_TRANSPORT_SHMCIRC)
    Bool              messageQTransportShmCircInitFlag;
    /*!< messageQTransportShmCirc Initialize flag */
#endif /* if defined (SYSLINK_TRANSPORT_SHMCIRC) */
    Bool              ringIOShmInitFlag;
    /*!< ringIOShm Initialize flag */
    Bool              notifyDriverShmInitFlag;
    /*!< notifyDriverShm Initialize flag */
    Bool              notifyDriverCircInitFlag;
    /*!< notifyDriverCirc Initialize flag */
    Bool              nameServerRemoteNotifyInitFlag;
    /*!< nameServerRemoteNotify Initialize flag */
    Bool              clientNotifyMgrInitFlag;
    /*!< clientNotifierMgr Initialize flag */
    Bool              frameQBufMgrInitFlag;
    /*!< frameQBufMgr Initialize flag */
    Bool              frameQInitFlag;
    /*!< frameQ Initialize flag */
    Bool              platformInitFlag;
    /*!< Flag to indicate platform initialization status */
    Platform_ModuleConfig hostModuleConfig;
    /*!< Configuration of various Modules' parameters on host */
    TerminateEventConfig termEvtAry[3];
    /*!< Terminate Event configuration array */
} Platform_Module_State;


/* =============================================================================
 * GLOBALS
 * =============================================================================
 */
#if defined (USE_SYSLINK_NOTIFY)
static Platform_Object Platform_objects [MultiProc_MAXPROCESSORS];
#else
static Platform_Object Platform_objects [MULTIPROC_MAXPROCESSORS];
#endif
static Platform_Module_State Platform_Module_state;
static Platform_Module_State * Platform_module = &Platform_Module_state;
static UInt16 Platform_refCount = 0;


Int32 _Platform_setup  (void);
Int32 _Platform_destroy (void);

extern String ProcMgr_sysLinkCfgParams;

/** ============================================================================
 *  APIs.
 *  ============================================================================
 */
/* Function to read slave memory */
Int32
_Platform_readSlaveMemory (UInt16   procId,
                           UInt32   addr,
                           Ptr      value,
                           UInt32 * numBytes);

/* Function to write slave memory */
Int32
_Platform_writeSlaveMemory (UInt16   procId,
                            UInt32   addr,
                            Ptr      value,
                            UInt32 * numBytes);
/*!
 *  @brief      Function to get tyhe default values for confiurations.
 *
 *  @param      config   Configuration values.
 */
Void
Platform_getConfig (Platform_Config * config)
{
    GT_1trace (curTrace, GT_ENTER, "Platform_getConfig", config);

    GT_assert (curTrace, (config != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (config == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Platform_getConfig",
                             Platform_E_INVALIDARG,
                             "Argument of type (Platform_getConfig *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if defined (USE_SYSLINK_NOTIFY)
        /* Get the gatepeterson default config */
        MultiProc_getConfig (&config->multiProcConfig);
#else
        /* Get the gatepeterson default config */
        multiproc_get_config (&config->multiProcConfig);
#endif

        /* Get the gateMP default config */
        GateMP_getConfig (&config->gateMPConfig);

        /* Get the gatepeterson default config */
        GatePeterson_getConfig (&config->gatePetersonConfig);

        /* Get the sharedregion default config */
        SharedRegion_getConfig (&config->sharedRegionConfig);

        /* Get the MESSAGEQ default config */
        MessageQ_getConfig (&config->messageQConfig);

#if defined (USE_SYSLINK_NOTIFY)
        /* Get the NOTIFY default config */
        Notify_getConfig (&config->notifyConfig);
#else
        /* Get the NOTIFY default config */
        notify_get_config (&config->notifyConfig);
#endif

        /* Get the PROCMGR default config */
        ProcMgr_getConfig (&config->procMgrConfig);

#if defined (SYSLINK_LOADER_ELF)
        /* Get the ElfLoader default config */
        ElfLoader_getConfig (&config->elfLoaderConfig);
#else /* COFF */
        /* Get the CoffLoader default config */
        CoffLoader_getConfig (&config->coffLoaderConfig);
#endif

        /* Get the HeapBufMP default config */
        HeapBufMP_getConfig (&config->heapBufConfig);

        /* Get the HeapMemMP default config */
        HeapMemMP_getConfig (&config->heapMemMPConfig);

        /* Get the LISTMPSHAREDMEMORY default config */
        ListMP_getConfig (&config->listMPConfig);

        /* Get the MESSAGEQTRANSPORTSHM default config */
        TransportShm_getConfig (&config->messageQTransportShmConfig);

#if defined (SYSLINK_TRANSPORT_SHMNOTIFY)
        /* Get the MESSAGEQTRANSPORTSHMNOTIFY default config */
        TransportShmNotify_getConfig (&config->messageQTransportShmNotifyConfig);
#endif /* if defined (SYSLINK_TRANSPORT_SHMNOTIFY) */

#if defined (SYSLINK_TRANSPORT_SHMCIRC)
        /* Get the MESSAGEQTRANSPORTSHMCIRC default config */
        TransportShmCirc_getConfig (&config->messageQTransportShmCircConfig);
#endif /* if defined (SYSLINK_TRANSPORT_SHMCIRC) */

#if defined (USE_SYSLINK_NOTIFY)
        NotifyDriverShm_getConfig(&config->notifyDriverShmConfig);
#else
            /* Get the NOTIFYSHMDRIVER default config */
        notify_shm_drv_get_config (&config->notifyDriverShmConfig);
#endif

        /* Get the NOTIFYDRIVERCIRC default config */
        NotifyDriverCirc_getConfig (&config->notifyDriverCircConfig);

        /* Get the NAMESERVERREMOTENOTIFY default config */
        NameServerRemoteNotify_getConfig(&config->nameServerRemoteNotifyConfig);

        /* Get the ClientNotifyMgr default config */
        ClientNotifyMgr_getConfig (&config->cliNotifyMgrCfgParams) ;

        /*  Get the FrameQBufMgr default config */
        FrameQBufMgr_getConfig (&config->frameQBufMgrCfgParams) ;

        /*  Get the FrameQ default config */
        FrameQ_getConfig (&config->frameQCfgParams) ;

        /*  Get the RingIO default config */
        RingIO_getConfig (&config->ringIOConfig) ;

        /*  Get the RingIOShm default config */
        RingIOShm_getConfig (&config->ringIOShmConfig) ;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "Platform_getConfig");
}

/*!
 *  @brief      Function to override the default confiuration values.
 *
 *  @param      config   Configuration values.
 */
Int32
Platform_overrideConfig (Platform_Config * config)
{
    Int32  status = Platform_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "Platform_overrideConfig", config);

    GT_assert (curTrace, (config != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (config == NULL) {
        /*! @retval Platform_E_INVALIDARG Argument of type
         *  (Platform_Config *) passed is null*/
        status = Platform_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Platform_overrideConfig",
                             status,
                             "Argument of type (Platform_getConfig *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if defined (USE_SYSLINK_NOTIFY)
        /* Override the multiProcConfig default config */
        config->multiProcConfig.numProcessors = 2;
        config->multiProcConfig.id            = 1;
        String_cpy (config->multiProcConfig.nameList [0],
                    "DSP");
        String_cpy (config->multiProcConfig.nameList [1],
                    "HOST");
#else
        /* Currently multiproc in knl does not allow setting proc names */
#endif

        /* Override the gatepeterson default config */

        /* Override the Sharedregion default config */
        config->sharedRegionConfig.cacheLineSize = 128;

        /* Override the LISTMP default config */

        /* Override the MESSAGEQ default config */

        /* Override the NOTIFY default config */

        /* Override the PROCMGR default config */

        /* Override the HeapBuf default config */

        /* Override the LISTMPSHAREDMEMORY default config */

        /* Override the MESSAGEQTRANSPORTSHM default config */
        /* Override the MESSAGEQTRANSPORTSHMNOTIFY default config */
        /* Override the MESSAGEQTRANSPORTSHMCIRC default config */

        /* Override the NOTIFYSHMDRIVER default config */
        /* Override the NOTIFYDRIVERCIRC default config */

        /* Override the NAMESERVERREMOTENOTIFY default config */

        /* Override the  ClientNotifyMgr default config */
        /* Override the  FrameQBufMgr default config */

        /* Override the FrameQ default config */


#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_ENTER, "Platform_overrideConfig", status);

    /*! @retval Platform_S_SUCCESS operation was successful */
    return status;
}

/*!
 *  @brief      Function to setup platform.
 */
Int32
Platform_setup (void)
{
    Int32             status = Platform_S_SUCCESS;
    Platform_Config   _config;
    Platform_Config * config;
#if defined (USE_SYSLINK_NOTIFY)
    Omap3530IpcInt_Config intConfig;
#endif
    Platform_getConfig (&_config);
    config = &_config;

    Platform_overrideConfig (config);

#if defined (USE_SYSLINK_NOTIFY)
    status = MultiProc_setup (&(config->multiProcConfig));
#else
    status = multiproc_setup (&(config->multiProcConfig));
#endif
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Platform_setup",
                             status,
                             "MultiProc_setup failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

/* Initialize PROCMGR */
    if (status >= 0) {
        status = ProcMgr_setup (&(config->procMgrConfig));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "ProcMgr_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->procMgrInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

/* Initialize SharedRegion */
    if (status >= 0) {
        status = SharedRegion_setup (&config->sharedRegionConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "SharedRegion_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->sharedRegionInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

#if defined (USE_SYSLINK_NOTIFY)
    /* Initialize IpcInt required for Notify. */
    if (status >= 0) {
        intConfig.procId    = MultiProc_getId ("DSP");
        intConfig.recvIntId = BASE_DSP2ARM_INTID;
        Omap3530IpcInt_setup (&intConfig);
    }
#endif

/* Get the NOTIFYSHMDRIVER default config */
    if (status >= 0) {
#if defined (USE_SYSLINK_NOTIFY)
        status = NotifyDriverShm_setup (&config->notifyDriverShmConfig);
#else
        status = notify_shm_drv_setup ((struct notify_shm_drv_config *)
                                        &config->notifyDriverShmConfig);
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "NotifyDriverShm_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->notifyDriverShmInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

#if defined (SYSLINK_NOTIFYDRIVER_CIRC)
/* Get the NOTIFYDRIVERCIRC default config */
    if (status >= 0) {
        status = NotifyDriverCirc_setup (&config->notifyDriverCircConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "NotifyDriverCirc_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->notifyDriverCircInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
#endif /* if defined (SYSLINK_NOTIFYDRIVER_CIRC) */

/* Initialize NOTIFY */
    if (status >= 0) {
#if defined (USE_SYSLINK_NOTIFY)
        status = Notify_setup (&config->notifyConfig);
#else
        status = notify_setup (&config->notifyConfig);
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "Notify_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->notifyInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

/* Initialize NAMESERVER */
    if (status >= 0) {
        status = NameServer_setup ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "NameServer_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->nameServerInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

   if (status >= 0) {
        status = GateMP_setup (&config->gateMPConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "GateMP_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->gateMPInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

/* Initialize Platform */
    if (status >= 0) {
        status = GatePeterson_setup (&config->gatePetersonConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "GatePeterson_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->gatePetersonInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

/* Intialize MESSAGEQ */
    if (status >= 0) {
        status = MessageQ_setup (&config->messageQConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "MessageQ_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->messageQInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

/* Intialize RINGIO */
    if (status >= 0) {
        status = RingIO_setup (&config->ringIOConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "RingIO_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->ringIOInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Get the ringIOShm default config */
    if (status >= 0) {
        status = RingIOShm_setup (&config->ringIOShmConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "Platform_setup", status,
                    "RingIOShm_setup failed!");
        }
        else {
#endif
            Platform_module->ringIOShmInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif
    }


    /* Intialize Loader */
    if (status >= 0) {
#if defined (SYSLINK_LOADER_ELF)
        status = ElfLoader_setup(&config->elfLoaderConfig);
#else /* COFF */
        status = CoffLoader_setup(&config->coffLoaderConfig);
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "Platform_setup", status,
                    "Loader_setup failed!");
        }
        else {
#endif
            Platform_module->loaderInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif
    }

    /* Intialize heap buf */
    if (status >= 0) {
        status = HeapBufMP_setup (&config->heapBufConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "Platform_setup", status,
                    "HeapBufMP_setup failed!");
        }
        else {
#endif
            Platform_module->heapBufInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif
    }

    /* Intialize heap MemMP */
    if (status >= 0) {
        status = HeapMemMP_setup (&config->heapMemMPConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "HeapMemMP_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->heapMemMPInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Get the LISTMPSHAREDMEMORY default config */
    if (status >= 0) {
        status = ListMP_setup (&config->listMPConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "ListMP_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->listMPInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }


/* Get the MESSAGEQTRANSPORTSHM default config */
    if (status >= 0) {
        status = TransportShm_setup (&config->messageQTransportShmConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "TransportShm_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->messageQTransportShmInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }


#if defined (SYSLINK_TRANSPORT_SHMNOTIFY)
/* Get the MESSAGEQTRANSPORTSHMNOTIFY default config */
    if (status >= 0) {
        status = TransportShmNotify_setup (
                                    &config->messageQTransportShmNotifyConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "TransportShmNotify_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->messageQTransportShmNotifyInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
#endif /* if defined (SYSLINK_TRANSPORT_SHMNOTIFY) */


#if defined (SYSLINK_TRANSPORT_SHMCIRC)
/* Get the MESSAGEQTRANSPORTSHMCIRC default config */
    if (status >= 0) {
        status = TransportShmCirc_setup (
                                    &config->messageQTransportShmCircConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "TransportShmCirc_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->messageQTransportShmCircInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
#endif /* if defined (SYSLINK_TRANSPORT_SHMCIRC) */


/* Get the NAMESERVERREMOTENOTIFY default config */
    if (status >= 0) {
        status = NameServerRemoteNotify_setup (
                                     &config->nameServerRemoteNotifyConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "NameServerRemoteNotify_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->nameServerRemoteNotifyInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Get the ClientNotifyMgr default config */
    if (status >= 0) {
        status = ClientNotifyMgr_setup (&config->cliNotifyMgrCfgParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "ClientNotifyMgr_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->clientNotifyMgrInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Get the FrameQBufMgr default config */
    if (status >= 0) {
        status = FrameQBufMgr_setup (&config->frameQBufMgrCfgParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "FrameQBufMgr_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->frameQBufMgrInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
    /* Get the FrameQ default config */
    if (status >= 0) {
        status = FrameQ_setup (&config->frameQCfgParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "FrameQ_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->frameQInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (status >= 0) {
        Memory_set (Platform_objects,
                    0,
                    (sizeof (Platform_Object) * MultiProc_getNumProcessors()));
    }

    if (status >= 0) {
        status = _Platform_setup ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_setup",
                                 status,
                                 "_Platform_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->platformInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
    if (status >= 0) {
        /*
         * Store SharedRegion numEntries configuration on Host to compare with
         *  the value configured on slaves
         */
        Platform_module->hostModuleConfig.sharedRegionNumEntries =
                                          config->sharedRegionConfig.numEntries;

    }
    return status;
}


/*!
 *  @brief      Function to destroy the System.
 */
Int32
Platform_destroy (void)
{
    Int32  status = Platform_S_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "Platform_destroy");

    /* Finalize Platform-specific destroy */
    if (Platform_module->platformInitFlag == TRUE) {
        status = _Platform_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "Platform_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->platformInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize Frame module */
    if (Platform_module->frameQInitFlag == TRUE) {
        status = FrameQ_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "FrameQ_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->frameQInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize FrameQBufMgr module */
    if (Platform_module->frameQBufMgrInitFlag == TRUE) {
        status = FrameQBufMgr_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "FrameQBufMgr_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->frameQBufMgrInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize ClientNotifyMgr module */
    if (Platform_module->clientNotifyMgrInitFlag == TRUE) {
        status = ClientNotifyMgr_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "ClientNotifyMgr_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->clientNotifyMgrInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize ringIOShm */
    if (Platform_module->ringIOShmInitFlag == TRUE) {
        status = RingIOShm_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "RingIOShm_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->ringIOShmInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize RINGIO */
    if (Platform_module->ringIOInitFlag == TRUE) {
        status = RingIO_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "RingIO_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->ringIOInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize NAMESERVERREMOTENOTIFY */
    if (Platform_module->nameServerRemoteNotifyInitFlag == TRUE) {
        status = NameServerRemoteNotify_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "NameServerRemoteNotify_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->nameServerRemoteNotifyInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

#if defined (SYSLINK_TRANSPORT_SHMCIRC)
    /* Finalize MESSAGEQTRANSPORTSHMCIRC */
    if (Platform_module->messageQTransportShmCircInitFlag == TRUE) {
        status = TransportShmCirc_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "TransportShmCirc_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->messageQTransportShmCircInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
#endif /* if defined (SYSLINK_TRANSPORT_SHMCIRC) */

#if defined (SYSLINK_TRANSPORT_SHMNOTIFY)
    /* Finalize MESSAGEQTRANSPORTSHMNOTIFY */
    if (Platform_module->messageQTransportShmNotifyInitFlag == TRUE) {
        status = TransportShmNotify_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "TransportShmNotify_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->messageQTransportShmNotifyInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
#endif /* if defined (SYSLINK_TRANSPORT_SHMNOTIFY) */

    /* Finalize MESSAGEQTRANSPORTSHM */
    if (Platform_module->messageQTransportShmInitFlag == TRUE) {
        status = TransportShm_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "TransportShm_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->messageQTransportShmInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize LISTMPSHAREDMEMORY */
    if (Platform_module->listMPInitFlag == TRUE) {
        status = ListMP_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "ListMP_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->listMPInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize heap buf */
    if (Platform_module->heapBufInitFlag == TRUE) {
        status = HeapBufMP_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "HeapBufMP_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->heapBufInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize heap MemMP */
    if (Platform_module->heapMemMPInitFlag == TRUE) {
        status = HeapMemMP_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "HeapMemMP_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->heapMemMPInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize loader */
    if (Platform_module->loaderInitFlag == TRUE) {
#if defined (SYSLINK_LOADER_ELF)
        status = ElfLoader_destroy();
#else
        status = CoffLoader_destroy ();
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "Platform_destroy", status,
                    "ElfLoader_destroy failed!");
        }
        else {
#endif
            Platform_module->loaderInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif
    }

    /* Finalize MessageQ */
    if (Platform_module->messageQInitFlag == TRUE) {
        status = MessageQ_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "MessageQ_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->messageQInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize GatePeterson */
    if (Platform_module->gatePetersonInitFlag == TRUE) {
        status = GatePeterson_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "GatePeterson_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->gatePetersonInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (Platform_module->gateMPInitFlag == TRUE) {
        status = GateMP_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "GateMP_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->gateMPInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize NAMESERVER */
    if (Platform_module->nameServerInitFlag == TRUE) {
        status = NameServer_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "NameServer_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->nameServerInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize NOTIFY */
    if (Platform_module->notifyInitFlag == TRUE) {
#if defined (USE_SYSLINK_NOTIFY)
        status = Notify_destroy ();
#else
        status = notify_destroy ();
#endif
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "Notify_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->notifyInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize NOTIFYDRIVERCIRC */
    if (Platform_module->notifyDriverCircInitFlag == TRUE) {
            status = NotifyDriverCirc_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "NotifyDriverCirc_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->notifyDriverCircInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize NOTIFYSHMDRIVER */
    if (Platform_module->notifyDriverShmInitFlag == TRUE) {
#if defined (USE_SYSLINK_NOTIFY)
        status = NotifyDriverShm_destroy ();
#else
        status = notify_shm_drv_destroy ();
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "NotifyDriverShm_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->notifyDriverShmInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

#if defined (USE_SYSLINK_NOTIFY)
    /* Can be called only after Notify is destroyed */
    Omap3530IpcInt_destroy ();
#endif

    /* Finalize SharedRegion */
    if (Platform_module->sharedRegionInitFlag == TRUE) {
        status = SharedRegion_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "SharedRegion_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->sharedRegionInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    /* Finalize PROCMGR */
    if (Platform_module->procMgrInitFlag == TRUE) {
        status = ProcMgr_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Platform_destroy",
                                 status,
                                 "ProcMgr_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Platform_module->procMgrInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

#if defined (USE_SYSLINK_NOTIFY)
    /* Finalize MultiProc */
    status = MultiProc_destroy ();
#else
    /* Finalize MultiProc */
    status = multiproc_destroy ();
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Platform_destroy",
                             status,
                             "MultiProc_destroy failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    if (status >= 0) {
        Memory_set (Platform_objects,
                    0,
                    (sizeof (Platform_Object) * MultiProc_getNumProcessors()));
    }

    GT_1trace (curTrace, GT_LEAVE, "Platform_destroy", status);

    /*! @retval Platform_S_SUCCESS Operation successful */
    return status;
}

/*
 * union _Platform_setup_Local exists so that we don't waste stack or
 * alloc'ed memory on storage for things that exist for just a few
 * statements of the function _Platform_setup().  The *PROC_Params
 * elements are large and variably sized, depending on the macro
 * ProcMgr_MAX_MEMORY_REGIONS.
 */
typedef union _Platform_setup_Local {
    OMAP3530PROC_Config   procConfig;
    OMAP3530PWR_Config    pwrConfig;
    ProcMgr_Params        params;
    OMAP3530PROC_Params   procParams;
#if defined (SYSLINK_LOADER_ELF)
    ElfLoader_Params      loaderParams;
#else
    CoffLoader_Params     loaderParams;
#endif
    OMAP3530PWR_Params    pwrParams;
} _Platform_setup_Local;

/*!
 *  @brief      Function to setup platform.
 */
Int32
_Platform_setup (void)
{
    Int32                 status = Platform_S_SUCCESS;
    _Platform_setup_Local *lv;
    UInt16                procId;
    Platform_Handle       handle;

    GT_0trace (curTrace, GT_ENTER, "_Platform_setup");

    lv = Memory_alloc(NULL, sizeof(_Platform_setup_Local), 0, NULL);
    if (lv == NULL) {
        status = Platform_E_FAIL;
        GT_setFailureReason (curTrace,
                                GT_4CLASS,
                                "_Platform_setup",
                                status,
                                "Memory_alloc failed");
        goto ret;
    }

    handle = &Platform_objects [MultiProc_getId ("DSP")];
    OMAP3530PROC_getConfig (&lv->procConfig);
    status = OMAP3530PROC_setup (&lv->procConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Platform_setup",
                             status,
                             "OMAP3530PROC_setup failed!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        OMAP3530PWR_getConfig (&lv->pwrConfig);
        status = OMAP3530PWR_setup (&lv->pwrConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Platform_setup",
                                 status,
                                 "OMAP3530PWR_setup failed!");
        }
    }

    if (status >= 0) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Get MultiProc ID by name. */
        procId = MultiProc_getId ("DSP");

        /* Create an instance of the Processor object for
         * OMAP3530 */
        OMAP3530PROC_Params_init (NULL, &lv->procParams);
        handle->pHandle = OMAP3530PROC_create (procId,
                                               &lv->procParams);

#if defined (SYSLINK_LOADER_ELF)
        /* Create an instance of the ELF Loader object */
        ElfLoader_Params_init (NULL, &lv->loaderParams);
        handle->ldrHandle = ElfLoader_create (procId, &lv->loaderParams);
#else /* COFF */
        /* Create an instance of the COFF Loader object */
        CoffLoader_Params_init (NULL, &lv->loaderParams);
        handle->ldrHandle = CoffLoader_create (procId, &lv->loaderParams);
#endif /* if defined (SYSLINK_LOADER_ELF) */

        /* Create an instance of the PwrMgr object for OMAP3530 */
        OMAP3530PWR_Params_init (NULL, &lv->pwrParams);
        handle->pwrHandle = OMAP3530PWR_create (procId,
                                                &lv->pwrParams);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (handle->pHandle == NULL) {
            status = Platform_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Platform_setup",
                                 status,
                                 "OMAP3530PROC_create failed!");
        }
        else if (handle->ldrHandle ==  NULL) {
            status = Platform_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Platform_setup",
                                 status,
                                 "Failed to create loader instance!");
        }
        else if (handle->pwrHandle ==  NULL) {
            status = Platform_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Platform_setup",
                                 status,
                                 "OMAP3530PWR_create failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Initialize parameters */
            ProcMgr_Params_init (NULL, &lv->params);
            lv->params.procHandle = handle->pHandle;
            lv->params.loaderHandle = handle->ldrHandle;
            lv->params.pwrHandle = handle->pwrHandle;
            String_cpy (lv->params.rstVectorSectionName,
                        RESETVECTOR_SYMBOL);
            handle->pmHandle = ProcMgr_create (procId, &lv->params);
            if (handle->pmHandle == NULL) {
                status = Platform_E_FAIL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_Platform_setup",
                                     status,
                                     "ProcMgr_create failed!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    Memory_free(NULL, lv, sizeof(_Platform_setup_Local));

ret:
    GT_1trace (curTrace, GT_LEAVE, "_Platform_setup", status);

    /*! @retval Platform_S_SUCCESS operation was successful */
    return status;
}


/*!
 *  @brief      Function to setup platform.
 */
Int32
_Platform_destroy (void)
{
    Int32           status    = Platform_S_SUCCESS;
    Int32           tmpStatus = Platform_S_SUCCESS;
    Platform_Handle handle;

    GT_0trace (curTrace, GT_ENTER, "_Platform_destroy");

    handle = &Platform_objects [MultiProc_getId ("DSP")];
    if (handle->pmHandle != NULL) {
        status = ProcMgr_delete (&handle->pmHandle);
        GT_assert (curTrace, (status >= 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "_Platform_destroy",
                    status, "ProcMgr_delete failed!");
        }
#endif
    }

    /* Delete the Processor, Loader and PwrMgr instances */
    if (handle->pwrHandle != NULL) {
        tmpStatus = OMAP3530PWR_delete (&handle->pwrHandle);
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            GT_setFailureReason(curTrace, GT_4CLASS, "_Platform_destroy",
                    status, "OMAP3530PWR_delete failed!");
#endif
        }
    }

    if (handle->ldrHandle != NULL) {
#if defined (SYSLINK_LOADER_ELF)
        tmpStatus = ElfLoader_delete(&handle->ldrHandle);
#else /* COFF */
        tmpStatus = CoffLoader_delete(&handle->ldrHandle);
#endif
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            GT_setFailureReason(curTrace, GT_4CLASS, "_Platform_destroy",
                    status, "Failed to delete loader instance!");
#endif
        }
    }

    if (handle->pHandle != NULL) {
        tmpStatus = OMAP3530PROC_delete(&handle->pHandle);
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            GT_setFailureReason(curTrace, GT_4CLASS, "_Platform_destroy",
                    status, "OMAP3530PROC_delete failed!");
#endif
        }
    }

    tmpStatus = OMAP3530PWR_destroy();
    GT_assert (curTrace, (tmpStatus >= 0));
    if ((status >= 0) && (tmpStatus < 0)) {
        status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        GT_setFailureReason(curTrace, GT_4CLASS, "_Platform_destroy", status,
                "OMAP3530PWR_destroy failed!");
#endif
    }

    tmpStatus = OMAP3530PROC_destroy();
    GT_assert (curTrace, (tmpStatus >= 0));
    if ((status >= 0) && (tmpStatus < 0)) {
        status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        GT_setFailureReason(curTrace, GT_4CLASS, "_Platform_destroy", status,
                "OMAP3530PROC_destroy failed!");
#endif
    }

    GT_1trace (curTrace, GT_LEAVE, "_Platform_destroy", status);

    return (status);
}


/*
 * ======== Platform_loadCallback ========
 */
Int32 Platform_loadCallback(UInt16 procId, Ptr arg)
{
    Int32                   status = Platform_S_SUCCESS;
    Platform_Handle         handle;
    UInt32                  start;
    UInt32                  numBytes;
    SharedRegion_Entry      entry;
    UInt32                  mAddr;
    ProcMgr_AddrInfo        ai;
    Ipc_Params              iParams;
    Int                     i;
    UInt32                  mapMask;
    UInt32                  fileId;
    Char                    str[64];

    GT_2trace(curTrace, GT_ENTER, "Platform_loadCallback", procId, arg);

    handle = (Platform_Handle)&Platform_objects[procId];

    /* acquire the reset vector address */
    if (arg != NULL) {
        start = (*(UInt32 *)arg); /* arg is reset vector in slave va */
    }
    else {
        /* read the reset vector symbol from slave binary */
        fileId = ProcMgr_getLoadedFileId(handle->pmHandle);

        status = ProcMgr_getSymbolAddress(handle->pmHandle, fileId,
            RESETVECTOR_SYMBOL, &start);

        if (status < 0) {
            status = Platform_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "Platform_loadCallback",
                status, "ProcMgr_getSymbolAddress failed");
            goto leave;
        }
    }

    /* read the slave's embedded configuration structure */
    numBytes = sizeof(Platform_SlaveConfig);

    status = _Platform_readSlaveMemory(procId, start, &handle->slaveCfg,
        &numBytes);

    if (status < 0) {
        status = Platform_E_FAIL;
        GT_setFailureReason(curTrace, GT_4CLASS, "Platform_loadCallback",
            status, "_Platform_readSlaveMemory failed");
        goto leave;
    }

    /* read the slave shared region config */
    if (handle->slaveCfg.numSRs > 0) {

        /* allocate space for slave's shared region configuration */
        numBytes = handle->slaveCfg.numSRs * sizeof(Platform_SlaveSRConfig);
        handle->slaveSRCfg = Memory_alloc(NULL, numBytes, 0, NULL);

        if (handle->slaveSRCfg == NULL) {
            status = Platform_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS,
                "Platform_loadCallback", status, "Memory_alloc failed");
            goto leave;
        }

        /* read the slave's embedded shared region configuration */
        status = _Platform_readSlaveMemory(procId,
            start + sizeof(Platform_SlaveConfig), handle->slaveSRCfg,
            &numBytes);

        if (status < 0) {
            status = Platform_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS,
                "Platform_loadCallback", status,
                "_Platform_readSlaveMemory failed");
            goto leave;
        }
    }

    /* create and ipc instance */
    iParams.setupMessageQ = handle->slaveCfg.setupMessageQ;
    iParams.setupNotify   = handle->slaveCfg.setupNotify;
    iParams.procSync      = handle->slaveCfg.procSync;

    status = Ipc_create(procId, &iParams);

    if (status < 0) {
        status = Platform_E_FAIL;
        GT_setFailureReason(curTrace, GT_4CLASS, "Platform_loadCallback",
            status, "Ipc_create failed");
        goto leave;
    }

    /* add new shared region entries to table */
    for (i = 0; i < handle->slaveCfg.numSRs; i++) {
        GT_2trace(curTrace, GT_2CLASS, "handle->slaveSRCfg[%d].entryBase %p",
            i, handle->slaveSRCfg[i].entryBase);

        /* get entry from table */
        status = SharedRegion_getEntry(handle->slaveSRCfg[i].id, &entry);

        /* on omap3530, entry.isValid should always be false */
        GT_assert(curTrace, (!entry.isValid));

        if (status < 0) {
            status = Platform_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "Platform_loadCallback",
                status, "SharedRegion_getEntry failed");
            goto leave;
        }

        /* translate the slave address to master physical address */
        status = ProcMgr_translateAddr(handle->pmHandle, (Ptr *)&mAddr,
            ProcMgr_AddrType_MasterPhys, (Ptr)handle->slaveSRCfg[i].entryBase,
            ProcMgr_AddrType_SlaveVirt);

        if (status < 0) {
            status = Platform_E_FAIL;
            GT_setFailureReason (curTrace, GT_4CLASS, "Platform_loadCallback",
                status, "ProcMgr_translateAddr failed");
            goto leave;
        }

        /* setup address info structure for shared region */
        ai.addr[ProcMgr_AddrType_MasterPhys] = mAddr;
        ai.addr[ProcMgr_AddrType_SlaveVirt] = handle->slaveSRCfg[i].entryBase;
        ai.size = handle->slaveSRCfg[i].entryLen;
        ai.isCached = FALSE;

        /* check for shared region cache override param */
        sprintf(str, "SharedRegion.entry[%d].cacheEnable=",
                handle->slaveSRCfg[i].id);

        status = Cfg_propBool(str, ProcMgr_sysLinkCfgParams, &ai.isCached);

        if (!status) {
            GT_2trace(curTrace, GT_2CLASS, "Platform_loadCallback:\n"
                "    Found SharedRegion.entry[%d].cacheEnable"
                " configuration value %d", handle->slaveSRCfg[i].id,
                ai.isCached);
        }
        else {
            GT_1trace(curTrace, GT_2CLASS, "Platform_loadCallback:\n"
                "    No SharedRegion.entry[%d].cacheEnable configuration "
                "value found, using default FALSE",
                handle->slaveSRCfg[i].id);
        }

        GT_5trace(curTrace, GT_2CLASS, "Platform_loadCallback:\n"
            "    Mapping SharedRegion %d\n"
            "    addr[ProcMgr_AddrType_MasterPhys] [0x%x]\n"
            "    addr[ProcMgr_AddrType_SlaveVirt]  [0x%x]\n"
            "    size                              [0x%x]\n"
            "    isCached                          [%d]",
            handle->slaveSRCfg[i].id, ai.addr[ProcMgr_AddrType_MasterPhys],
            ai.addr[ProcMgr_AddrType_SlaveVirt], ai.size, ai.isCached);

        /* always map region into slave virtual address space */
        mapMask = ProcMgr_SLAVEVIRT;

        /* map region into kernel only if region has a heap */
        if ((Bool)handle->slaveSRCfg[i].createHeap) {
            mapMask |= ProcMgr_MASTERKNLVIRT;
        }

        /* map the shared region as requested */
        status = ProcMgr_map(handle->pmHandle, mapMask, &ai,
            ProcMgr_AddrType_MasterPhys);

        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS,
                "Platform_loadCallback", status, "ProcMgr_map failed!");
            goto leave;
        }

        /* If the entry has a heap, set the entry base address to the
         * kernel virtual address; otherwise use the master physical
         * address.
         */
        if (handle->slaveSRCfg[i].createHeap) {
            entry.base = (Ptr)ai.addr[ProcMgr_AddrType_MasterKnlVirt];
        }
        else {
            entry.base = (Ptr)mAddr;
        }
        entry.len = handle->slaveSRCfg[i].entryLen;
        entry.ownerProcId = handle->slaveSRCfg[i].ownerProcId;
        entry.cacheLineSize = handle->slaveSRCfg[i].cacheLineSize;
        entry.isValid = TRUE;
        entry.cacheEnable = ai.isCached;
        entry.createHeap = handle->slaveSRCfg[i].createHeap;

        /* on omap3530, shared region should never be started at this point */
        GT_assert(curTrace, (!_SharedRegion_isStarted()));

        /* set new entry in table */
        _SharedRegion_setEntry(handle->slaveSRCfg[i].id, &entry);

    } /* for (each shared region entry) */

    /* update slave's configuration */
    numBytes = sizeof(Platform_SlaveConfig);
    handle->slaveCfg.sr0MemorySetup = 1;  /* set ready flag */

    status = _Platform_writeSlaveMemory(procId, start, &handle->slaveCfg,
        &numBytes);

    if (status < 0) {
        status = Platform_E_FAIL;
        GT_setFailureReason(curTrace, GT_4CLASS, "Platform_loadCallback",
            status, "_Platform_writeSlaveMemory failed");
        goto leave;
    }

    /* start all ipc modules */
    status = Ipc_start();

    if (status < 0) {
        status = Platform_E_FAIL;
        GT_setFailureReason(curTrace, GT_4CLASS, "Platform_loadCallback",
            status, "Ipc_start failed");
        goto leave;
    }

    /* increase refCount for number of successful loadCallbacks */
    Platform_refCount++;

leave:
    /* if error, release resources in reverse order of acquisition */
    if (status < 0) {
        /* TODO: release map entries acquired in for loop */
        /* TODO: if Ipc_start failed, slave already running, what now? */
        if (handle->slaveSRCfg != NULL) {
            Memory_free(NULL, handle->slaveSRCfg,
                handle->slaveCfg.numSRs * sizeof(Platform_SlaveSRConfig));
            handle->slaveSRCfg = NULL;
        }
    }
    GT_1trace(curTrace, GT_LEAVE, "Platform_loadCallback", status);

    return (status);
}
EXPORT_SYMBOL(Platform_loadCallback);


/*
 * ======== Platform_startCallback ========
 *
 * In case of self boot mode, slave and host both have to create the
 * notifyDriver outside of SysMgr context. Since an event is sent to slave
 * to indicate that HOST has done initialization on its side and it is safe
 * for slave to do the rest of initialization.
 */
Int32
Platform_startCallback (UInt16 procId, Ptr arg)
{
    Int32 status = Platform_S_SUCCESS;
    Syslink_SlaveModuleConfig slaveModuleConfig;
    UInt32  startTimeout;

    GT_2trace (curTrace, GT_ENTER, "Platform_startCallback", procId, arg);

    /* TODO: enter gate */
    startTimeout = 0;

    /* Attach to slave cores */
    do {
        startTimeout++;
        status = Ipc_attach(procId);

        if ((status == Ipc_E_NOTREADY) && ((startTimeout & 0xFFF) == 0)) {
            OsalThread_sleep(ATTACH_SLEEPTIME);
        }
    } while ((status == Ipc_E_NOTREADY) && (startTimeout < ATTACH_LOOPCNT));

    /* Check timeout value */
    if (startTimeout >= ATTACH_LOOPCNT) {
        status = Platform_E_FAIL;
        GT_setFailureReason(curTrace, GT_4CLASS, "Platform_startCallback",
            status, "Ipc_attach timeout");
        goto end;
    }

    /* Reset timeout value */
    startTimeout = 0;

    /* Get shared region max numEntries from Slave */
    do {
        if (startTimeout > 0) {
            OsalThread_sleep(CFG_SLEEPTIME);
        }

        status = Ipc_readConfig(procId, SLAVE_CONFIG_TAG,
                (Ptr)&slaveModuleConfig, sizeof(Syslink_SlaveModuleConfig));

    } while ((status == Ipc_E_FAIL) && (++startTimeout < CFG_LOOPCNT));

    /* Check timeout value */
    if (startTimeout >= CFG_LOOPCNT) {
        status = Platform_E_FAIL;
        GT_setFailureReason(curTrace, GT_4CLASS, "Platform_startCallback",
            status, "Ipc_readConfig timeout");
        goto end;
    }

    GT_assert (curTrace,
              (    Platform_module->hostModuleConfig.sharedRegionNumEntries
               ==  slaveModuleConfig.sharedRegionNumEntries));

    if (    Platform_module->hostModuleConfig.sharedRegionNumEntries
        !=  slaveModuleConfig.sharedRegionNumEntries) {

        status = Platform_E_FAIL;
        GT_setFailureReason (curTrace,
                     GT_4CLASS,
                     "Platform_startCallback",
                     status,
                     "SharedRegion numEntries did not match with slave side");

        GT_4trace (curTrace,
                   GT_4CLASS,
                   "Platform_startCallback:\n"
                   "    Master procId = [%u]\n"
                   "    Slave  procId = [%u]\n"
                   "    SharedRegion numEntries on (Hlos)Master = [%u]\n"
                   "    SharedRegion numEntries on (Rtos)Slave = [%u]",
                   MultiProc_self(),
                   procId,
                   Platform_module->hostModuleConfig.sharedRegionNumEntries,
                   slaveModuleConfig.sharedRegionNumEntries);
    }

    /* save the terminate event configuration for this remote processor */
    Platform_module->termEvtAry[procId].eventId =
            slaveModuleConfig.terminateEventId;
    Platform_module->termEvtAry[procId].lineId =
            slaveModuleConfig.terminateEventLineId;

end:
    GT_1trace (curTrace, GT_LEAVE, "Platform_startCallback", status);

    return status;
}

EXPORT_SYMBOL(Platform_startCallback);

/*
 * ======== Platform_stopCallback ========
 */
Int32 Platform_stopCallback(UInt16 procId, Ptr arg)
{
    typedef struct {
        Ptr     basePhys;       /* MasterPhys */
        Ptr     baseKnl;        /* MasterKnlVirt */
        Ptr     baseSlave;      /* SlaveVirt */
        UInt32  mask;           /* unmap mask */
        Bool    cache;          /* cache enable */
    } UnmapInfo;

    Int32                   status = Platform_S_SUCCESS;
    UInt32                  i;
    UInt32                  mAddr;
    UInt32                  start;
    UInt32                  numBytes;
    UInt32                  detachTimeout;
    UInt32                  fileId;
    ProcMgr_AddrInfo        ai;
    Platform_Handle         handle;
    SharedRegion_Entry      entry;
    UnmapInfo *             uiAry = NULL;
    ProcMgr_State           procState;

    GT_2trace(curTrace, GT_ENTER, "Platform_stopCallback", procId, arg);

    handle = (Platform_Handle)&Platform_objects[procId];

    GT_assert(curTrace, (handle != NULL));

    /* acquire the reset vector address */
    if (arg != NULL) {
        start = (*(UInt32 *)arg); /* arg is reset vector in slave va */
    }
    else {
        /* read the reset vector symbol from slave binary */
        fileId = ProcMgr_getLoadedFileId(handle->pmHandle);

        status = ProcMgr_getSymbolAddress(handle->pmHandle, fileId,
            RESETVECTOR_SYMBOL, &start);

        if (status < 0) {
            status = Platform_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "Platform_stopCallback",
                status, "ProcMgr_getSymbolAddress failed");
            goto leave;
        }
    }

    /* cache region information, cannot access data afte Ipc_stop() */
    uiAry = Memory_alloc(NULL,
        (sizeof(UnmapInfo) * handle->slaveCfg.numSRs), 0, NULL);

    if (uiAry == NULL) {
        status = Platform_E_FAIL;
        GT_setFailureReason(curTrace, GT_4CLASS, "Platform_stopCallback",
            status, "Memory_alloc failed");
        goto leave;
    }

    for (i = 0; i < handle->slaveCfg.numSRs; i++) {
        status = SharedRegion_getEntry(handle->slaveSRCfg[i].id, &entry);

        if (status < 0) {
            status = Platform_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "Platform_stopCallback",
                status, "SharedRegion_getEntry failed");
            goto leave;
        }

        /* always unmap region from slave virtual address space */
        uiAry[i].baseSlave = (Ptr)handle->slaveSRCfg[i].entryBase;
        uiAry[i].mask = ProcMgr_SLAVEVIRT;

        /* if region has a heap, then also unmap from kernel address space */
        if (entry.createHeap) {
            uiAry[i].mask |= ProcMgr_MASTERKNLVIRT;
            uiAry[i].baseKnl = entry.base;

            /* translate the slave address to master physical address */
            status = ProcMgr_translateAddr(handle->pmHandle, (Ptr *)&mAddr,
                ProcMgr_AddrType_MasterPhys,
                (Ptr)handle->slaveSRCfg[i].entryBase,
                ProcMgr_AddrType_SlaveVirt);

            if (status < 0) {
                status = Platform_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS,
                    "Platform_stopCallback", status,
                    "ProcMgr_translateAddr failed");
                goto leave;
            }

            uiAry[i].basePhys = (Ptr)mAddr;
        }
        else {
            uiAry[i].basePhys = entry.base;  /* MasterPhys */
            uiAry[i].baseKnl = NULL;
        }

        /* save the region's cache property */
        uiAry[i].cache = entry.cacheEnable;
    }

    /* do IPC cleanup */
    detachTimeout = 0;

    do {
        detachTimeout++;
        status = Ipc_detach(procId);

        if ((status < 0) && ((detachTimeout & 0xFFF) == 0)) {
            OsalThread_sleep(DETACH_SLEEPTIME);
        }
    } while ((status < 0) && (detachTimeout < DETACH_LOOPCNT));

    if (detachTimeout >= DETACH_LOOPCNT) {
        GT_setFailureReason(curTrace, GT_4CLASS, "Platform_stopCallback",
            status, "Ipc_detach timeout");
        /* don't exit, keep going */
    }

    /* wait for remote processor if it is running */
    procState = ProcMgr_getState(handle->pmHandle);

    if ((Platform_refCount == 1) && (procState == ProcMgr_State_Running)) {

        /* check for IPC stop on slave until successful or times out */
        detachTimeout = 0;

        do {
            if (detachTimeout > 0) {
                OsalThread_sleep(CFG_SLEEPTIME);
            }

            /* read sr0MemorySetup */
            numBytes = sizeof(Platform_SlaveConfig);

            status = _Platform_readSlaveMemory(procId, start,
                    &handle->slaveCfg, &numBytes);

        } while ((handle->slaveCfg.sr0MemorySetup == 1)
                  && (++detachTimeout < CFG_LOOPCNT));

        if (detachTimeout >= CFG_LOOPCNT) {
            status = Platform_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "Platform_stopCallback",
                status, "Ipc_stop timeout - Ipc_stop not called by slave?");
            /* don't exit, keep going */
        }
    }

    Ipc_stop();

    /* now unmap slave's shared regions */
    for (i = 0; i < handle->slaveCfg.numSRs; i++) {

        /* setup address info structure for unmapping operation */
        ai.addr[ProcMgr_AddrType_MasterPhys] = (UInt32)uiAry[i].basePhys;
        ai.addr[ProcMgr_AddrType_SlaveVirt] = (UInt32)uiAry[i].baseSlave;
        ai.addr[ProcMgr_AddrType_MasterKnlVirt] = (UInt32)uiAry[i].baseKnl;
        ai.size = handle->slaveSRCfg[i].entryLen;
        ai.isCached = uiAry[i].cache;

        /* unmap the shared region as requested */
        status = ProcMgr_unmap(handle->pmHandle, uiAry[i].mask, &ai,
            ProcMgr_AddrType_MasterPhys);

        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "Platform_stopCallback",
                status, "ProcMgr_unmap failed!");
            goto leave;
        }

    } /* for (each shared region entry) */

    Platform_refCount--;

leave:
    /* release resources which are no longer needed */
    if (handle->slaveSRCfg != NULL) {
        Memory_free(NULL, handle->slaveSRCfg,
            handle->slaveCfg.numSRs * sizeof (Platform_SlaveSRConfig));
        handle->slaveSRCfg = NULL;
    }
    if (uiAry != NULL) {
        Memory_free(NULL, uiAry, handle->slaveCfg.numSRs * sizeof(UnmapInfo));
        uiAry = NULL;
    }

    GT_1trace(curTrace, GT_LEAVE, "Platform_stopCallback", status);

    return(status);
}

/* TBD: since application has to call this API for now */
EXPORT_SYMBOL(Platform_stopCallback);

/*  ----------------------------------------------------------------------------
 *  Internal functions
 *  ----------------------------------------------------------------------------
 */
/* Function to read slave memory */
Int32
_Platform_readSlaveMemory (UInt16   procId,
                           UInt32   addr,
                           Ptr      value,
                           UInt32 * numBytes)
{
    Int              status = 0;
    Bool             done = FALSE;
    Platform_Handle  handle;
    ProcMgr_AddrInfo aInfo;
    UInt32           mAddr;

    GT_4trace (curTrace, GT_ENTER, "_Platform_readSlaveMemory",
    procId, addr, value, numBytes);

    handle = (Platform_Handle) &Platform_objects [procId];

    /* Translate the slave address to master address */
    status = ProcMgr_translateAddr (handle->pmHandle,
                                    (Ptr *)&mAddr,
                                    ProcMgr_AddrType_MasterPhys,
                                    (Ptr)addr,
                                    ProcMgr_AddrType_SlaveVirt);
    if (status < 0) {
        status = Platform_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Platform_readSlaveMemory",
                             status,
                             "ProcMgr_translateAddr failed");
    }

    if (status >= 0) {
        status = ProcMgr_translateAddr (handle->pmHandle,
                                        (Ptr *)&mAddr,
                                        ProcMgr_AddrType_MasterKnlVirt,
                                        (Ptr)mAddr,
                                        ProcMgr_AddrType_MasterPhys);
       if (status >= 0) {
            Memory_copy (value, (Ptr) mAddr, *numBytes);
            done = TRUE;
       }
    }

    if ((status >= 0) && (done == FALSE)) {
        /* Map the address */
        aInfo.addr [ProcMgr_AddrType_MasterPhys] = mAddr;
        aInfo.addr [ProcMgr_AddrType_SlaveVirt]  = addr;
        aInfo.size = *numBytes;
        aInfo.isCached = FALSE;
        status = ProcMgr_map(handle->pmHandle,
                (ProcMgr_MASTERKNLVIRT | ProcMgr_SLAVEVIRT),
                &aInfo, ProcMgr_AddrType_MasterPhys);
        if (status < 0) {
            status = Platform_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Platform_readSlaveMemory",
                                 status,
                                 "ProcMgr_map failed");
        }
    }

    if ((status >= 0) && (done == FALSE)) {
        status = ProcMgr_read (handle->pmHandle,
                               addr,
                               numBytes,
                               value);
        if (status < 0) {
            status = Platform_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Platform_readSlaveMemory",
                                 status,
                                 "ProcMgr_read failed");
        }
    }

    if ((status >= 0) && (done == FALSE)) {
        /* Map the address */
        status = ProcMgr_unmap(handle->pmHandle,
                (ProcMgr_MASTERKNLVIRT | ProcMgr_SLAVEVIRT),
                &aInfo, ProcMgr_AddrType_MasterPhys);
        if (status < 0) {
            status = Platform_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Platform_readSlaveMemory",
                                 status,
                                 "ProcMgr_unmap failed");
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "_Platform_readSlaveMemory", status);

    return status;
}


/* Function to write slave memory */
Int32
_Platform_writeSlaveMemory (UInt16   procId,
                            UInt32   addr,
                            Ptr      value,
                            UInt32 * numBytes)
{
    Int              status = 0;
    Bool             done = FALSE;
    Platform_Handle  handle;
    ProcMgr_AddrInfo aInfo;
    UInt32           mAddr;

    GT_4trace (curTrace, GT_ENTER, "_Platform_writeSlaveMemory",
    procId, addr, value, numBytes);

    handle = (Platform_Handle) &Platform_objects [procId];

    /* Translate the slave address to master address */
    status = ProcMgr_translateAddr (handle->pmHandle,
                                    (Ptr *)&mAddr,
                                    ProcMgr_AddrType_MasterPhys,
                                    (Ptr)addr,
                                    ProcMgr_AddrType_SlaveVirt);
    if (status < 0) {
        status = Platform_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Platform_writeSlaveMemory",
                             status,
                             "ProcMgr_translateAddr failed");
    }

    if (status >= 0) {
        status = ProcMgr_translateAddr (handle->pmHandle,
                                        (Ptr *)&mAddr,
                                        ProcMgr_AddrType_MasterKnlVirt,
                                        (Ptr)mAddr,
                                        ProcMgr_AddrType_MasterPhys);
       if (status >= 0) {
            Memory_copy ((Ptr) mAddr, value, *numBytes);
            done = TRUE;
       }
    }

    if ((status >= 0) && (done == FALSE)) {
        /* Map the address */
        aInfo.addr[ProcMgr_AddrType_MasterPhys] = mAddr;
        aInfo.addr[ProcMgr_AddrType_SlaveVirt] = addr;
        aInfo.size = *numBytes;
        aInfo.isCached = FALSE;
        status = ProcMgr_map(handle->pmHandle,
                (ProcMgr_MASTERKNLVIRT | ProcMgr_SLAVEVIRT),
                &aInfo, ProcMgr_AddrType_MasterPhys);
        if (status < 0) {
            status = Platform_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Platform_writeSlaveMemory",
                                 status,
                                 "ProcMgr_map failed");
        }
    }

    if ((status >= 0) && (done == FALSE)) {
        status = ProcMgr_write(handle->pmHandle, addr, numBytes, value);
        if (status < 0) {
            status = Platform_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Platform_writeSlaveMemory",
                                 status,
                                 "ProcMgr_write failed");
        }
    }

    if ((status >= 0) && (done == FALSE))  {
        /* Map the address */
        status = ProcMgr_unmap(handle->pmHandle,
                (ProcMgr_MASTERKNLVIRT | ProcMgr_SLAVEVIRT), &aInfo,
                ProcMgr_AddrType_MasterPhys);
        if (status < 0) {
            status = Platform_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Platform_writeSlaveMemory",
                                 status,
                                 "ProcMgr_unmap failed");
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "_Platform_writeSlaveMemory", status);

    return status;
}

/*
 *  ======== Platform_terminateEventConfig ========
 */
Void Platform_terminateEventConfig(UInt16 procId, UInt32 *eventId,
        UInt16 *lineId)
{

    *eventId = Platform_module->termEvtAry[procId].eventId;
    *lineId = Platform_module->termEvtAry[procId].lineId;
}

Void Platform_terminateHandler(UInt16 procId)
{
    ProcMgr_Handle procHnd;

    procHnd = _ProcMgr_getHandle(procId);
    Platform_stopCallback(procId, NULL);
    ProcMgr_stop(procHnd);
}

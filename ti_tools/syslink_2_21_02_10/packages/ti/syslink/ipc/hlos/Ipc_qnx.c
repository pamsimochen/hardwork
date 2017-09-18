/*
 *  @file   Ipc_qnx.c
 *
 *  @brief      User side implementation of IPC.
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

/* Utilities & OSAL headers */
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/String.h>

/* Module headers */
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/IpcHost.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/syslink/inc/_GateMP_qnx.h>
#include <ti/syslink/SysLink.h>
#include <ti/syslink/inc/SysMemMgr.h>
#include <ti/ipc/Ipc.h>
#include <ti/syslink/inc/usr/IpcUsr.h>
#include <ti/syslink/inc/usr/Linux/IpcDrv.h>
#include <ti/syslink/inc/IpcDrvDefs.h>
#include <ti/syslink/inc/_Ipc.h>
#include <ti/syslink/utils/SyslinkMemMgr.h>
#include <ti/syslink/utils/IpcMemMgr.h>
#include <ti/syslink/inc/GateHWSpinlock_qnx.h>
#include <ti/syslink/inc/_RingIO.h>
#include <ti/syslink/inc/_RingIOShm.h>

#include <stdlib.h>  /* for getenv() */

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*!
 *  @brief  Ipc Module state object
 */
typedef struct Ipc_ModuleObject_tag {
    UInt32        setupRefCount;
    /*!< Reference count for number of times _setup/_destroy were called in this
         process. */
    /* Boot load page of the slaves */
    Bool          sysMemMgrInitFlag;
    /*!< SysMemMgr Initialize flag */
    Bool          ipcMemMgrInitFlag;
    /*!< IpcMemMgr Initialize flag */
    Bool          multiProcInitFlag;
    /*!< MultiProc Initialize flag */
    Bool          sharedRegionInitFlag;
    /*!< SHAREDREGION Initialize flag */
    Bool          gateHWSpinlockInitFlag;
    /*!< GateHWSpinlock Initialize flag */
    Bool          gateMPInitFlag;
    /*!< GateMP Initialize flag */
    Bool          listMpInitFlag;
    /*!< ListMP Initialize flag */
    Bool          messageQInitFlag;
    /*!< MessageQ Initialize flag */
    Bool          ringIOInitFlag;
    /*!< RingIO Initialize flag */
    Bool          notifyInitFlag;
    /*!< Notify Initialize flag */
    Bool          procMgrInitFlag;
    /*!< Processor manager Initialize flag */
    Bool          heapBufMPInitFlag;
    /*!< HeapBufMP Initialize flag */
    Bool          heapMemMPInitFlag;
    /*!< HeapBufMP Initialize flag */
    Bool          nameServerInitFlag;
    /*!< nameServerRemoteNotify Initialize flag */
    Bool          syslinkMemMgrInitFlag;
    /*!< syslinkMemMgrInitFlag Initialize flag */
    Bool          listMPInitFlag;
    /*!< LISTMP Initialize flag */
    Bool          clientNotifyMgrInitFlag;
    /*!< clientNotifierMgr Initialize flag */
    Bool          frameQBufMgrInitFlag;
    /*!< frameQBufMgr Initialize flag */
    Bool          frameQInitFlag;
    /*!< frameQ Initialize flag */
} Ipc_ModuleObject;


/* =============================================================================
 *  Globals
 * =============================================================================
 */

/*!
 *  @var    Ipc_state
 *
 *  @brief  Ipc state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
Ipc_ModuleObject Ipc_state =
{
    .setupRefCount = 0
};


/* TODO remove sysMemMap */
SysLink_MemoryMap SysLinkCfg_sysMemMap =
{
    .numBlocks = 0,
    .memBlocks = NULL
};


/* conf override string, set by application before calling SysLink_setup() */
String SysLink_params = NULL;


/* =============================================================================
 * APIS
 * =============================================================================
 */
/*!
 *  @brief      Function to get the default values for configurations.
 *
 *  @param      config   Configuration values.
 */
Void
Ipc_getConfig (Ipc_Config * config)
{
    char *pSL_PARAMS;

    GT_1trace (curTrace, GT_ENTER, "Ipc_getConfig", config);

    GT_assert (curTrace, (config != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (config == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Ipc_getConfig",
                             Ipc_E_INVALIDARG,
                             "Argument of type (Ipc_getConfig *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* add configuration overrides */
        config->sysMemMap = &SysLinkCfg_sysMemMap;

        /* assign config->params - overriding with SL_PARAMS env var, if set */
        pSL_PARAMS = getenv("SL_PARAMS");
        if (pSL_PARAMS != NULL) {
            GT_2trace(curTrace, GT_1CLASS, "Overriding SysLink_params \"%s\""
                    " with SL_PARAMS \"%s\"\n", SysLink_params, pSL_PARAMS);
            config->params = pSL_PARAMS;
        }
        else {
            config->params = SysLink_params;
        }

        /* get the IpcMemMgr default config */
        IpcMemMgr_getConfig(&config->ipcMemMgrConfig);

#if 0 /* TBD:Temporarily comment. */
        /* Get the SysMemMgr default config */
        SysMemMgr_getConfig (&config->sysMemMgrConfig);
#endif /* TBD: Temporarily comment. */

        /* Get the MultiProc default config */
        MultiProc_getConfig (&config->multiProcConfig);

        /* Get the sharedregion default config */
        SharedRegion_getConfig (&config->sharedRegionConfig);

        /* Get the GateMP default config */
//      GateMP_getConfig (&config->gateMPConfig);

        /* Get the MESSAGEQ default config */
        MessageQ_getConfig (&config->messageQConfig);

        /* Get the NOTIFY default config */
        Notify_getConfig (&config->notifyConfig);

        /* Get the PROCMGR default config */
        ProcMgr_getConfig (&config->procMgrConfig);

        /* Get the HeapBufMP default config */
        HeapBufMP_getConfig (&config->heapBufMPConfig);

        /* Get the HeapMemMP default config */
        HeapMemMP_getConfig (&config->heapMemMPConfig);

        /* Get the LISTMP default config */
        ListMP_getConfig (&config->listMPConfig);

        /* Get the ClientNotifyMgr default config */
        ClientNotifyMgr_getConfig (&config->cliNotifyMgrCfgParams) ;

#if !defined(SYSLINK_NOT_USE_FRAMEQ)
        /*  Get the FrameQBufMgr default config */
        FrameQBufMgr_getConfig (&config->frameQBufMgrCfgParams) ;

        /*  Get the FrameQ default config */
        FrameQ_getConfig (&config->frameQCfgParams) ;
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_ENTER, "Ipc_getConfig");
}

/*
 *  ======== Ipc_isAttached ========
 */
Bool Ipc_isAttached(UInt16 remoteProcId)
{
    Int status = Ipc_S_SUCCESS;
    Bool attached = FALSE;
    IpcDrv_CmdArgs cmdArgs;

    if (remoteProcId != MultiProc_self()) {
        cmdArgs.args.isAttached.remoteProcId = remoteProcId;
        cmdArgs.args.isAttached.attached = FALSE;

        status = IpcDrv_ioctl(CMD_IPC_ISATTACHED, &cmdArgs);

        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_isAttached",
                    status, "API (through ioctl) failed on kernel-side");
        }
        else {
            attached = cmdArgs.args.isAttached.attached;
        }
    }

    return(attached);
}

/*
 *  ======== Ipc_setup ========
 */
Int Ipc_setup(const Ipc_Config *cfg)
{
    Int            status = Ipc_S_SUCCESS;
    Ipc_Config *   config = NULL;
    Ipc_Config     tConfig;

    GT_1trace (curTrace, GT_ENTER, "Ipc_setup", cfg);

    if (cfg == NULL) {
        Ipc_getConfig (&tConfig);
        config = &tConfig;
    }
    else {
        config = (Ipc_Config *) cfg;
    }

    /* TBD: Protect from multiple threads. */
    Ipc_state.setupRefCount++;

    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (Ipc_state.setupRefCount > 1) {
        /*! @retval Ipc_S_ALREADYSETUP Success: Ipc module has been
                                           already setup in this process */
        status = Ipc_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "Ipc module has been already setup in this process.\n"
                   "    RefCount: [%d]\n",
                   Ipc_state.setupRefCount);
    }
    else {
        /* Open the driver handle. */
        status = IpcDrv_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Ipc_setup",
                                 status,
                                 "Failed to open driver handle!");
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        if (status >= 0) {
            status = IpcMemMgr_setup(&(config->ipcMemMgrConfig));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_setup",
                    status, "IpcMemMgr_setup failed");
            }
            else {
#endif
                Ipc_state.ipcMemMgrInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif
        }

        if (status >= 0) {
            status = MultiProc_setup (&(config->multiProcConfig));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "MultiProc_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.multiProcInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

#if 0 /* TBD:Temporarily comment. */
        if (status >= 0) {
            status = SysMemMgr_setup (&(config->sysMemMgrConfig));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "SysMemMgr_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.sysMemMgrInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#endif /* TBD: Temporarily comment. */

        /* Initialize NAMESERVER */
        if (status >= 0) {
            status = NameServer_setup ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "NameServer_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.nameServerInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

#if 0 /* TBD:Temporarily comment. */
        /* Initialize MEMMGR */
        if (status >= 0) {
            status = SyslinkMemMgr_setup ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "SyslinkMemMgr_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.syslinkMemMgrInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#endif /* TBD: Temporarily comment. */

        /* Initialize PROCMGR */
        if (status >= 0) {
            /* add system memory map and instance params */
            config->procMgrConfig.sysMemMap = config->sysMemMap;
            config->procMgrConfig.params = config->params;
            config->procMgrConfig.paramsLen =
                (config->params == NULL ? 0 : String_len(config->params) + 1);
            status = ProcMgr_setup (&(config->procMgrConfig));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "ProcMgr_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.procMgrInitFlag = TRUE;
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
                                     "Ipc_setup",
                                     status,
                                     "SharedRegion_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.sharedRegionInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* GateHWSpinlock initialize */
        if (status >= 0) {
            status = GateHWSpinlock_setup();
            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_setup",
                    status, "GateHWSpinlock_setup failed");
            }
            else {
                Ipc_state.gateHWSpinlockInitFlag = TRUE;
            }
        }

        /* GateMP initialize */
        if (status >= 0) {
            status = GateMP_setup();
            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_setup",
                    status, "GateMP_setup failed");
            }
            else {
                Ipc_state.gateMPInitFlag = TRUE;
            }
        }
    }

    /* Always initialize NOTIFY */
    if (status >= 0) {
        status = Notify_setup (&config->notifyConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Ipc_setup",
                                 status,
                                 "Notify_setup failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Ipc_state.notifyInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (Ipc_state.setupRefCount <= 1) {
        /* Intialize MESSAGEQ */
        if (status >= 0) {
            status = MessageQ_setup (&config->messageQConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "MessageQ_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.messageQInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

#if !defined(SYSLINK_NOT_USE_RINGIO)
        /* Intialize RINGIO */
        if (status >= 0) {
            status = RingIO_setup ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "RingIO_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.ringIOInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        /* Intialize RINGIOSHM */
        if (status >= 0) {
            status = RingIOShm_setup ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "RingIOShm_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.ringIOInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

#endif
        /* Intialize heap buf */
        if (status >= 0) {
            status = HeapBufMP_setup (&config->heapBufMPConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "HeapBufMP_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.heapBufMPInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Intialize HeapMemMP */
        if (status >= 0) {
            status = HeapMemMP_setup (&config->heapMemMPConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "HeapMemMP_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.heapMemMPInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Get the LISTMP default config */
        if (status >= 0) {
            status = ListMP_setup (&config->listMPConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "ListMpSharedMemory_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.listMPInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Get the ClientNotifyMgr default config */
        if (status >= 0) {
            status = ClientNotifyMgr_setup ( &config->cliNotifyMgrCfgParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                         status,
                                         "ClientNotifyMgr_setup failed!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    Ipc_state.clientNotifyMgrInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

#if !defined(SYSLINK_NOT_USE_FRAMEQ)
        /* Get the FrameQBufMgr default config */
        if (status >= 0) {
            status = FrameQBufMgr_setup ( &config->frameQBufMgrCfgParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "FrameQBufMgr_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.frameQBufMgrInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Get the FrameQ default config */
        if (status >= 0) {
            status = FrameQ_setup ( &config->frameQCfgParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "FrameQ_setup failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.frameQInitFlag = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#endif
    }

    GT_1trace (curTrace, GT_LEAVE, "Ipc_setup", status);

    /*! @retval Ipc_S_SUCCESS Operation successful */
    return status;
}


/* Function to destroy the System. */
Int
Ipc_destroy (void)
{
    Int             status = Ipc_S_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "Ipc_destroy");

    /* TBD: Protect from multiple threads. */
    Ipc_state.setupRefCount--;
    /* This is needed at runtime so should not be in SYSLINK_BUILD_OPTIMIZE. */
    if (Ipc_state.setupRefCount >= 1) {
        /*! @retval Ipc_S_ALREADYSETUP Success: Ipc module has been
                                           already setup in this process */
        status = Ipc_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "Ipc module has been already setup in this process.\n"
                   "    RefCount: [%d]\n",
                   Ipc_state.setupRefCount);
    }
    else {
        /* Finalize Frame module */
        if (Ipc_state.frameQInitFlag == TRUE) {
            status = FrameQ_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "FrameQ_destroy failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.frameQInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Finalize FrameQBufMgr module */
        if (Ipc_state.frameQBufMgrInitFlag == TRUE) {
            status = FrameQBufMgr_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "FrameQBufMgr_destroy failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.frameQBufMgrInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Finalize ClientNotifyMgr module */
        if (Ipc_state.clientNotifyMgrInitFlag == TRUE) {
            status = ClientNotifyMgr_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "ClientNotifyMgr_destroy failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.clientNotifyMgrInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Finalize LISTMP */
        if (Ipc_state.listMPInitFlag == TRUE) {
            status = ListMP_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_setup",
                                     status,
                                     "ListMpSharedMemory_destroy failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.listMPInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Finalize HeapMemMP */
        if (Ipc_state.heapMemMPInitFlag == TRUE) {
            status = HeapMemMP_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "HeapMemMP_destroy failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.heapMemMPInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Finalize heap buf */
        if (Ipc_state.heapBufMPInitFlag == TRUE) {
            status = HeapBufMP_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "HeapBufMP_destroy failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.heapBufMPInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Finalize MESSAGEQ */
        if (Ipc_state.messageQInitFlag == TRUE) {
            status = MessageQ_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "MessageQ_destroy failed!");
            }
            else {
    #endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.messageQInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Finalize RINGIO */
        if (Ipc_state.ringIOInitFlag == TRUE) {
            status = RingIO_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "RingIO_destroy failed!");
            }
            else {
    #endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.ringIOInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Finalize RINGIOSHM */
        if (Ipc_state.ringIOInitFlag == TRUE) {
            status = RingIOShm_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "RingIOShm_destroy failed!");
            }
            else {
    #endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.ringIOInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
    }

    /* Always finalize NOTIFY */
    if (Ipc_state.notifyInitFlag == TRUE) {
        status = Notify_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Ipc_destroy",
                                 status,
                                 "Notify_destroy failed!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (Ipc_state.setupRefCount < 1) {

        /* GateHWSpinlock finalize */
        if (Ipc_state.gateHWSpinlockInitFlag) {
            status = GateHWSpinlock_destroy();
            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_destroy",
                    status, "GateHWSpinlock_destroy failed");
            }
            Ipc_state.gateHWSpinlockInitFlag = FALSE;
        }

        /* GateMP finalize */
        if (Ipc_state.gateMPInitFlag) {
            status = GateMP_destroy();
            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_destroy",
                    status, "GateMP_destroy failed");
            }
            Ipc_state.gateMPInitFlag = FALSE;
        }

        /* Finalize SharedRegion */
        if (Ipc_state.sharedRegionInitFlag == TRUE) {
            status = SharedRegion_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "SharedRegion_destroy failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.sharedRegionInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* Finalize PROCMGR */
        if (Ipc_state.procMgrInitFlag == TRUE) {
            status = ProcMgr_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "ProcMgr_destroy failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.procMgrInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

#if 0 /* TBD:Temporarily comment. */
        /* Finalize MemMgr */
        if (Ipc_state.syslinkMemMgrInitFlag == TRUE) {
            status = SyslinkMemMgr_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "SyslinkMemMgr_destroy failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
               Ipc_state.syslinkMemMgrInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#endif /* TBD: Temporarily comment. */

        /* Finalize NAMESERVER */
        if (Ipc_state.nameServerInitFlag == TRUE) {
            status = NameServer_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "NameServer_destroy failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.nameServerInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

#if 0 /* TBD:Temporarily comment. */
        /* Finalize SysMemMgr */
        if (Ipc_state.sysMemMgrInitFlag == TRUE) {
            status = SysMemMgr_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "SysMemMgr_destroy failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.sysMemMgrInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#endif /* TBD: Temporarily comment. */

        /* Finalize MultiProc */
        if (Ipc_state.multiProcInitFlag == TRUE) {
            /* Destroy the multiProc */
            status = MultiProc_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_destroy",
                                     status,
                                     "MultiProc_destroy failed!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Ipc_state.multiProcInitFlag = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        /* finalize IpcMemMgr */
        if (Ipc_state.ipcMemMgrInitFlag == TRUE) {

            status = IpcMemMgr_destroy();

            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_destroy",
                    status, "IpcMemMgr_destroy failed");
            }
            else {
                Ipc_state.ipcMemMgrInitFlag = FALSE;
            }
        }

        /* Close the driver handle. */
        IpcDrv_close ();
    } /* Reference count check */

    GT_1trace (curTrace, GT_LEAVE, "Ipc_destroy", status);

    /*! @retval Ipc_S_SUCCESS Operation successful */
    return status;
}


/* Function to destroy a Ipc instance for a slave */
Int
Ipc_control (UInt16 procId, Int cmdId, Ptr arg)
{
    Int             status    = Ipc_S_SUCCESS;
    IpcDrv_CmdArgs cmdArgs;

    GT_3trace (curTrace, GT_ENTER, "Ipc_control", procId, cmdId, arg);

    cmdArgs.args.control.procId = procId;
    cmdArgs.args.control.cmdId  = cmdId;
    if (arg != NULL) {
        cmdArgs.args.control.arg   = (Ptr)(*(UInt32 *)arg);
    }
    else {
        cmdArgs.args.control.arg   = NULL;
    }
    status = IpcDrv_ioctl (CMD_IPC_CONTROL, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Ipc_control",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        switch (cmdId)
        {
//      case Ipc_CONTROLCMD_LOADCALLBACK:
//          GateMP_start();  /* currently started by SysLink Driver */
//          break;
//
        case Ipc_CONTROLCMD_STARTCALLBACK:
            status = _SharedRegion_setRegions ();
            if (status < 0) {
                status = Ipc_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_control",
                                     status,
                                     "_SharedRegion_setRegions API Failed!");
            }

            status = GateMP_attach(procId, NULL);

            if (status < 0) {
                status = Ipc_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_control",
                    status, "GateMP_attach failed");
            }
            break;

        case Ipc_CONTROLCMD_STOPCALLBACK:
            status = GateMP_detach(procId, NULL);

            if (status < 0) {
                status = Ipc_E_FAIL;
                GT_setFailureReason(curTrace, GT_4CLASS, "Ipc_control",
                    status, "GateMP_detach failed");
            }

            status = _SharedRegion_clearRegions ();
            if (status < 0) {
                status = Ipc_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Ipc_control",
                                     status,
                                     "_SharedRegion_clearRegions API Failed!");
            }
            break;
        default:
            break;

        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "Ipc_control", status);

    /*! @retval Ipc_S_SUCCESS Operation was successful */
    return status;
}


/* Function to read configuration information from Ipc module */
Int
Ipc_readConfig (UInt16 remoteProcId, UInt32 tag, Ptr cfg, SizeT size)
{
    Int             status    = Ipc_S_SUCCESS;
    IpcDrv_CmdArgs cmdArgs;

    GT_4trace (curTrace, GT_ENTER, "Ipc_readConfig",
               remoteProcId, tag, cfg, size);

    cmdArgs.args.readConfig.remoteProcId = remoteProcId;
    cmdArgs.args.readConfig.tag          = tag;
    cmdArgs.args.readConfig.cfg          = cfg;
    cmdArgs.args.readConfig.size         = size;
    status = IpcDrv_ioctl (CMD_IPC_READCONFIG, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Ipc_readConfig",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "Ipc_readConfig", status);

    /*! @retval Ipc_S_SUCCESS Operation was successful */
    return status;
}


/* Function to write configuration information to Ipc module */
Int
Ipc_writeConfig (UInt16 remoteProcId, UInt32 tag, Ptr cfg, SizeT size)
{
    Int             status    = Ipc_S_SUCCESS;
    IpcDrv_CmdArgs cmdArgs;

    GT_4trace (curTrace, GT_ENTER, "Ipc_writeConfig",
               remoteProcId, tag, cfg, size);

    cmdArgs.args.writeConfig.remoteProcId = remoteProcId;
    cmdArgs.args.writeConfig.tag          = tag;
    cmdArgs.args.writeConfig.cfg          = cfg;
    cmdArgs.args.writeConfig.size         = size;
    status = IpcDrv_ioctl (CMD_IPC_WRITECONFIG, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Ipc_writeConfig",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "Ipc_writeConfig", status);

    /*! @retval Ipc_S_SUCCESS Operation was successful */
    return status;
}

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
 *  @file   ti/syslink/family/rtos/bios6/ti814x/Platform.c
 *
 *  @brief  Implementation of Platform initialization logic for ti81xx.
 */

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>

#include <string.h>

#include <ti/ipc/MultiProc.h>

#include <ti/syslink/utils/OsalPrint.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/sdo/utils/List.h>

#include <ti/syslink/inc/knl/Dm8168IpcInt.h>
#include <ti/syslink/inc/knl/Dm8168DspPwr.h>
#include <ti/syslink/inc/knl/Dm8168DspProc.h>
#include <ti/syslink/inc/knl/Dm8168M3VideoProc.h>
#include <ti/syslink/inc/knl/Dm8168M3DssProc.h>
#include <ti/syslink/inc/knl/Dm8168DucatiPwr.h>
#include <ti/syslink/ProcMgr.h>
#include <ti/syslink/inc/knl/Platform.h>
#include <ti/syslink/inc/ElfLoader.h>
#include <ti/syslink/inc/_ProcMgr.h>
#include <ti/syslink/RingIO.h>
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/inc/knl/PwrDefs.h>
#include <ti/syslink/inc/_MultiProc.h>


/** ============================================================================
 *  Application specific configuration, please change these value according to
 *  your application's need.
 *  ============================================================================
 */

/*!
 *  @brief  Structure defining config parameters for overall System.
 */
typedef struct Platform_Config {
    ProcMgr_Config                  procMgrConfig;
    /*!< Processor manager config parameter */
    ElfLoader_Config                elfLoaderConfig;
    /*!< Elf loader config parameter */
} Platform_Config;

/*! @brief structure for platform instance */
typedef struct Platform_Object {
    /*!< Flag to indicate platform initialization status */
    ProcMgr_Handle                pmHandle;
    /*!< Handle to the ProcMgr instance used */
    union {
        struct {
            DM8168DSPPROC_Handle pHandle;
            /*!< Handle to the Processor instance used */
            DM8168DSPPWR_Handle  pwrHandle;
            /*!< Handle to the PwrMgr instance used */
            ElfLoader_Handle    ldrHandle;
            /*!< Handle to the Loader instance used */
        } dsp;
        struct {
            DM8168VIDEOM3PROC_Handle pHandle;
            /*!< Handle to the Processor instance used */
            DM8168DUCATIPWR_Handle  pwrHandle;
            /*!< Handle to the PwrMgr instance used */
            ElfLoader_Handle        ldrHandle;
            /*!< Handle to the Loader instance used */
        } m3video;
        struct {
            DM8168VPSSM3PROC_Handle pHandle;
            /*!< Handle to the Processor instance used */
            DM8168DUCATIPWR_Handle  pwrHandle;
            /*!< Handle to the PwrMgr instance used */
            ElfLoader_Handle      ldrHandle;
            /*!< Handle to the Loader instance used */
        } m3vpss;
    } sHandles;
} Platform_Object, *Platform_Handle;


/*! @brief structure for platform instance */
typedef struct Platform_Module_State {
    /*!< MessageQ Initialize flag */
    Bool              procMgrInitFlag;
    /*!< Processor manager Initialize flag */
    Bool              elfLoaderInitFlag;
    /*!< Elf loader Initialize flag */
    Bool              platformInitFlag;
    /*!< Flag to indicate platform initialization status */
} Platform_Module_State;


/* =============================================================================
 * GLOBALS
 * =============================================================================
 */
static Platform_Object Platform_objects [MultiProc_MAXPROCESSORS];
static Platform_Module_State Platform_Module_state;
static Platform_Module_State * Platform_module = &Platform_Module_state;
//static UInt16 Platform_refCount = 0;


Int32 _Platform_setup  (void);
Int32 _Platform_destroy (void);

/** ============================================================================
 *  APIs.
 *  ============================================================================
 */
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

    /* Get the PROCMGR default config */
    ProcMgr_getConfig (&config->procMgrConfig);

    /* Get the ElfLoader default config */
    ElfLoader_getConfig (&config->elfLoaderConfig);

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

    GT_1trace (curTrace, GT_ENTER, "Platform_overrideConfig", status);

    /*! @retval Platform_S_SUCCESS operation was successful */
    return status;
}

/*!
 *  @brief      Function to setup platform.
 *              TBD: logic would change completely in the final system.
 */
Int32
Platform_setup (void)
{
    Int32             status = Platform_S_SUCCESS;
    Platform_Config   _config;
    Platform_Config * config;

    Platform_getConfig (&_config);
    config = &_config;
    Platform_overrideConfig (config);

    /* Initialize PROCMGR */
    if (status >= 0) {
        status = ProcMgr_setup (&(config->procMgrConfig));
        Platform_module->procMgrInitFlag = TRUE;
    }

    /* Intialize Elf loader */
    if (status >= 0) {
        status = ElfLoader_setup (&config->elfLoaderConfig);
        Platform_module->elfLoaderInitFlag = TRUE;
    }
    if (status >= 0) {
        memset (Platform_objects,
                    0,
                    (sizeof (Platform_Object) * MultiProc_getNumProcessors()));
    }
    if (status >= 0) {
        status = _Platform_setup ();
        Platform_module->platformInitFlag = TRUE;
    }
    return status;
}


/*!
 *  @brief      Function to destroy the System.
 *
 *  @sa         Platform_setup
 */
Int32
Platform_destroy (void)
{
    Int32  status = Platform_S_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "Platform_destroy");

    /* Finalize Platform-specific destroy */
    if (Platform_module->platformInitFlag == TRUE) {
        status = _Platform_destroy ();
        Platform_module->platformInitFlag = FALSE;
    }

    /* Finalize ELF loader */
    if (Platform_module->elfLoaderInitFlag == TRUE) {
        status = ElfLoader_destroy ();
        Platform_module->elfLoaderInitFlag = FALSE;
    }

    /* Finalize PROCMGR */
    if (Platform_module->procMgrInitFlag == TRUE) {
        status = ProcMgr_destroy ();
        Platform_module->procMgrInitFlag = FALSE;
    }
    if (status >= 0) {
        memset (Platform_objects,
                    0,
                    (sizeof (Platform_Object) * MultiProc_getNumProcessors()));
    }

    GT_1trace (curTrace, GT_LEAVE, "Platform_destroy", status);
    /*! @retval Platform_S_SUCCESS Operation successful */
    return status;
}

/*!
 *  @brief      Function to setup platform.
 *              TBD: logic would change completely in the final system.
 */
Int32
_Platform_setup (void)
{
    Int32                    status = Platform_S_SUCCESS;
    ProcMgr_Params           params;
    DM8168DSPPROC_Config     dspProcConfig;
    DM8168VIDEOM3PROC_Config videoProcConfig;
    DM8168VPSSM3PROC_Config  vpssProcConfig;
    DM8168DSPPWR_Config      dspPwrConfig;
    DM8168DUCATIPWR_Config   videoPwrConfig;
    DM8168DUCATIPWR_Config   vpssPwrConfig;
    DM8168DSPPROC_Params     dspProcParams;
    DM8168VIDEOM3PROC_Params videoProcParams;
    DM8168VPSSM3PROC_Params  vpssProcParams;
    DM8168DSPPWR_Params      dspPwrParams;
    DM8168DUCATIPWR_Params   videoPwrParams;
    DM8168DUCATIPWR_Params   vpssPwrParams;
    ElfLoader_Params         elfLoaderParams;
    UInt16                   procId;
    Platform_Handle          handle;

    GT_0trace (curTrace, GT_ENTER, "_Platform_setup");

#if !defined (SYSLINK_NO_DSP)
    /* Get MultiProc ID by name. */
    procId = MultiProc_getId ("DSP");

    handle = &Platform_objects [procId];
    DM8168DSPPROC_getConfig (&dspProcConfig);
    status = DM8168DSPPROC_setup (&dspProcConfig);

    GT_assert (curTrace, (status >= 0));

    DM8168DSPPWR_getConfig (&dspPwrConfig);
    status = DM8168DSPPWR_setup (&dspPwrConfig);

    GT_assert (curTrace, (status >= 0));

    /* Create an instance of the Processor object for
     * DM8168DSP */
    DM8168DSPPROC_Params_init (NULL, &dspProcParams);
    handle->sHandles.dsp.pHandle = DM8168DSPPROC_create (procId,
                                                         &dspProcParams);

        /* Create an instance of the ELF Loader object */
        ElfLoader_Params_init (NULL, &elfLoaderParams);
        handle->sHandles.dsp.ldrHandle =
                                       ElfLoader_create (procId,
                                                         &elfLoaderParams);

        /* Create an instance of the PwrMgr object for DM8168DSP */
        DM8168DSPPWR_Params_init (&dspPwrParams);
        handle->sHandles.dsp.pwrHandle = DM8168DSPPWR_create (procId,
                                                              &dspPwrParams);
        if (handle->sHandles.dsp.pHandle == NULL) {
            status = Platform_E_FAIL;
            GT_assert (curTrace, (status >= 0));
        }
        else if (handle->sHandles.dsp.ldrHandle ==  NULL) {
            status = Platform_E_FAIL;
            GT_assert (curTrace, (status >= 0));
        }
        else if (handle->sHandles.dsp.pwrHandle ==  NULL) {
            status = Platform_E_FAIL;
            GT_assert (curTrace, (status >= 0));
        }
        else {
            /* Initialize parameters */
            ProcMgr_Params_init (NULL, &params);
            params.procHandle = handle->sHandles.dsp.pHandle;
            params.loaderHandle = handle->sHandles.dsp.ldrHandle;
            params.pwrHandle = handle->sHandles.dsp.pwrHandle;
            handle->pmHandle = ProcMgr_create (procId, &params);
            if (handle->pmHandle == NULL) {
                status = Platform_E_FAIL;
                GT_assert (curTrace, (status >= 0));
            }
        }
#endif /* #if !defined (SYSLINK_NO_DSP)   */

    if (status >= 0) {
        /* Get MultiProc ID by name. */
        procId = MultiProc_getId ("VIDEO-M3");

        handle = &Platform_objects [procId];
        DM8168VIDEOM3PROC_getConfig (&videoProcConfig);
        status = DM8168VIDEOM3PROC_setup (&videoProcConfig);
        if (status < 0) {
           GT_assert (curTrace, (status >= 0));
        }
        else {
            DM8168DUCATIPWR_getConfig (&videoPwrConfig);
            status = DM8168DUCATIPWR_setup (&videoPwrConfig);
            if (status < 0) {
                GT_assert (curTrace, (status >= 0));
            }
        }

        if (status >= 0) {
            /* Create an instance of the Processor object for
             * DM8168VIDEOM3 */
            DM8168VIDEOM3PROC_Params_init (NULL, &videoProcParams);
            handle->sHandles.m3video.pHandle = DM8168VIDEOM3PROC_create (
                                                          procId,
                                                          &videoProcParams);

            /* Create an instance of the ELF Loader object */
            ElfLoader_Params_init (NULL, &elfLoaderParams);
            handle->sHandles.m3video.ldrHandle = ElfLoader_create (procId,
                                                              &elfLoaderParams);

            /* Create an instance of the PwrMgr object for DM8168VIDEOM3 */
            DM8168DUCATIPWR_Params_init (&videoPwrParams);
            handle->sHandles.m3video.pwrHandle = DM8168DUCATIPWR_create (
                                                           procId,
                                                           &videoPwrParams);

            if (handle->sHandles.m3video.pHandle == NULL) {
                status = Platform_E_FAIL;
                GT_assert (curTrace, (status >= 0));
            }
            else if (handle->sHandles.m3video.ldrHandle ==  NULL) {
                status = Platform_E_FAIL;
                GT_assert (curTrace, (status >= 0));
            }
            else if (handle->sHandles.m3video.pwrHandle ==  NULL) {
                status = Platform_E_FAIL;
                GT_assert (curTrace, (status >= 0));
            }
            else {
                /* Initialize parameters */
                ProcMgr_Params_init (NULL, &params);
                params.procHandle = handle->sHandles.m3video.pHandle;
                params.loaderHandle = handle->sHandles.m3video.ldrHandle;
                params.pwrHandle = handle->sHandles.m3video.pwrHandle;
                handle->pmHandle = ProcMgr_create (procId, &params);
                if (handle->pmHandle == NULL) {
                    status = Platform_E_FAIL;
                    GT_assert (curTrace, (status >= 0));
                }
            }
        }
    }
    if (status >= 0) {
        /* Get MultiProc ID by name. */
        procId = MultiProc_getId ("VPSS-M3");

        handle = &Platform_objects [procId];
        DM8168VPSSM3PROC_getConfig (&vpssProcConfig);
        status = DM8168VPSSM3PROC_setup (&vpssProcConfig);
        if (status < 0) {
                GT_assert (curTrace, (status >= 0));
        }
        else {
            DM8168DUCATIPWR_getConfig (&vpssPwrConfig);
            status = DM8168DUCATIPWR_setup (&vpssPwrConfig);
            if (status < 0) {
                GT_assert (curTrace, (status >= 0));
            }
        }

        if (status >= 0) {
            /* Create an instance of the Processor object for
             * DM8168VPSSM3 */
            DM8168VPSSM3PROC_Params_init(NULL, &vpssProcParams);
            handle->sHandles.m3vpss.pHandle = DM8168VPSSM3PROC_create(procId,
                    &vpssProcParams);

            /* Create an instance of the ELFLoader object */
            ElfLoader_Params_init (NULL, &elfLoaderParams);
            handle->sHandles.m3vpss.ldrHandle = ElfLoader_create(procId,
                    &elfLoaderParams);

            /* Create an instance of the PwrMgr object for DM8168VPSSM3 */
            DM8168DUCATIPWR_Params_init(&vpssPwrParams);
            handle->sHandles.m3vpss.pwrHandle = DM8168DUCATIPWR_create(procId,
                    &vpssPwrParams);

            if (handle->sHandles.m3vpss.pHandle == NULL) {
                status = Platform_E_FAIL;
                GT_assert (curTrace, (status >= 0));
            }
            else if (handle->sHandles.m3vpss.ldrHandle ==  NULL) {
                status = Platform_E_FAIL;
                GT_assert (curTrace, (status >= 0));
            }
            else if (handle->sHandles.m3vpss.pwrHandle ==  NULL) {
                status = Platform_E_FAIL;
                GT_assert (curTrace, (status >= 0));
            }
            else {
                /* Initialize parameters */
                ProcMgr_Params_init (NULL, &params);
                params.procHandle = handle->sHandles.m3vpss.pHandle;
                params.loaderHandle = handle->sHandles.m3vpss.ldrHandle;
                params.pwrHandle = handle->sHandles.m3vpss.pwrHandle;
                handle->pmHandle = ProcMgr_create (procId, &params);
                if (handle->pmHandle == NULL) {
                    status = Platform_E_FAIL;
                    GT_assert (curTrace, (status >= 0));
                }
            }
        }
    }
    GT_1trace (curTrace, GT_LEAVE, "_Platform_setup", status);

    /*! @retval Platform_S_SUCCESS operation was successful */
    return status;
}

/*!
 *  @brief      Function to setup platform.
 *              TBD: logic would change completely in the final system.
 */
Int32
_Platform_destroy (void)
{
    Int32           status    = Platform_S_SUCCESS;
    Int32           tmpStatus = Platform_S_SUCCESS;
    Platform_Handle handle;

    GT_0trace (curTrace, GT_ENTER, "_Platform_destroy");

    /* ------------------------- DSP cleanup -------------------------------- */
    handle = &Platform_objects [MultiProc_getId ("DSP")];
    if (handle->pmHandle != NULL) {
        status = ProcMgr_delete (&handle->pmHandle);
        GT_assert (curTrace, (status >= 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
           GT_assert (curTrace, (status >= 0));
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    if (handle->sHandles.dsp.pwrHandle != NULL) {
        tmpStatus = DM8168DSPPWR_delete (&handle->sHandles.dsp.pwrHandle);
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
        }
    }

    if (handle->sHandles.dsp.ldrHandle != NULL) {
        tmpStatus = ElfLoader_delete (&handle->sHandles.dsp.ldrHandle);

        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
        }
    }

    if (handle->sHandles.dsp.pHandle != NULL) {
        tmpStatus = DM8168DSPPROC_delete (&handle->sHandles.dsp.pHandle);
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
        }
    }

    tmpStatus = DM8168DSPPWR_destroy ();
    GT_assert (curTrace, (tmpStatus >= 0));
    if ((status >= 0) && (tmpStatus < 0)) {
        status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
    }

    tmpStatus = DM8168DSPPROC_destroy ();
    GT_assert (curTrace, (tmpStatus >= 0));
    if ((status >= 0) && (tmpStatus < 0)) {
        status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
    }
    /* ------------------------- VIDEOM3 cleanup ------------------------------ */
    handle = &Platform_objects [MultiProc_getId ("VIDEO-M3")];
    if (handle->pmHandle != NULL) {
        tmpStatus = ProcMgr_delete (&handle->pmHandle);
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
        }
    }

    /* Delete the Processor, Loader and PwrMgr instances */
    if (handle->sHandles.m3video.pwrHandle != NULL) {
        tmpStatus = DM8168DUCATIPWR_delete (&handle->sHandles.m3video.pwrHandle);
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
        }
    }

    if (handle->sHandles.m3video.ldrHandle != NULL) {
        tmpStatus = ElfLoader_delete (&handle->sHandles.m3video.ldrHandle);
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
        }
    }

    if (handle->sHandles.m3video.pHandle != NULL) {
        tmpStatus = DM8168VIDEOM3PROC_delete (&handle->sHandles.m3video.pHandle);
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
        }
    }

    tmpStatus = DM8168DUCATIPWR_destroy ();
    GT_assert (curTrace, (tmpStatus >= 0));
    if ((status >= 0) && (tmpStatus < 0)) {
        status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
    }

    tmpStatus = DM8168VIDEOM3PROC_destroy ();
    GT_assert (curTrace, (tmpStatus >= 0));
    if ((status >= 0) && (tmpStatus < 0)) {
        status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
    }

/* ------------------------- VPSSM3 cleanup --------------------------------- */
    handle = &Platform_objects [MultiProc_getId ("VPSS-M3")];
    if (handle->pmHandle != NULL) {
        tmpStatus = ProcMgr_delete (&handle->pmHandle);
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
        }
    }

    /* Delete the Processor, Loader and PwrMgr instances */
    if (handle->sHandles.m3vpss.pwrHandle != NULL) {
        tmpStatus = DM8168DUCATIPWR_delete (&handle->sHandles.m3vpss.pwrHandle);
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
        }
    }

    if (handle->sHandles.m3vpss.ldrHandle != NULL) {
        tmpStatus = ElfLoader_delete (&handle->sHandles.m3vpss.ldrHandle);
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
        }
    }

    if (handle->sHandles.m3vpss.pHandle != NULL) {
        tmpStatus = DM8168VPSSM3PROC_delete (&handle->sHandles.m3vpss.pHandle);
        GT_assert (curTrace, (tmpStatus >= 0));
        if ((status >= 0) && (tmpStatus < 0)) {
            status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
        }
    }

    tmpStatus = DM8168DUCATIPWR_destroy ();
    GT_assert (curTrace, (tmpStatus >= 0));
    if ((status >= 0) && (tmpStatus < 0)) {
        status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
    }

    tmpStatus = DM8168VPSSM3PROC_destroy ();
    GT_assert (curTrace, (tmpStatus >= 0));
    if ((status >= 0) && (tmpStatus < 0)) {
        status = tmpStatus;
           GT_assert (curTrace, (status >= 0));
    }

    GT_1trace (curTrace, GT_LEAVE, "_Platform_destroy", status);

    /*! @retval Platform_S_SUCCESS operation was successful */
    return status;
}

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

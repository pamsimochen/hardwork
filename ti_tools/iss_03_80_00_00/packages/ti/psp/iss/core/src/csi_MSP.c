/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright 2012, Texas Instruments Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file camera_xxx.c
*
* XXX: Description
*
* @path iss/drivers/drv_camera/src
*
* @rev nn.mm
*/
/* -------------------------------------------------------------------------- */

#include <ti/psp/iss/core/msp_types.h>
#include <ti/psp/iss/core/inc/csi_MSP.h>

/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
#define GOTO_EXIT_IF(C,N) if (C) {                                                                      \
                                    goto EXIT_##N;                                                      \
                                    }
									//TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_DRIVERS, "\nError in %s, line:%d\n",__FILE__, __LINE__);     \
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/* ========================================================================== */
/**
* Csi2RegisterCB  
*
* @param    **ppQueue:
* @param    *pCsiDevStruct:
* @param    *pCsiLineNumIsr:
* @param    tConfigIndex:  -  
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
*
* @pre There is no pre conditions.
*
* @post There is no post conditions.
*
* @see
*
*/
/* ========================================================================== */
MSP_ERROR_TYPE Csi2RegisterCB(CSI2_IsrHandle **ppQueue, MSP_CsiDev *pCsiDevStruct,
                                IssCsi2rxIsrHandleLineNumT *pCsiLineNumIsr, MSP_CSI2CfgIndex tConfigIndex)
{
    CSI2_IsrHandle*  pQueue;
    
    pQueue = (CSI2_IsrHandle *)MSP_MEMALLOC(sizeof(CSI2_IsrHandle));
    GOTO_EXIT_IF((pQueue==NULL),1);

    pQueue->pNext = pCsiDevStruct->pCsi2IsrHdlFirst;
    pCsiDevStruct->pCsi2IsrHdlFirst = pQueue;

    pQueue->eIrqIdx = tConfigIndex;
    pQueue->tCsi2IsrHdl.tIsrHandle.private = pCsiLineNumIsr->tIsrHandle.private;
    pQueue->tCsi2IsrHdl.tIsrHandle.callback = pCsiLineNumIsr->tIsrHandle.callback;
    pQueue->tCsi2IsrHdl.tIsrHandle.arg1 = pCsiLineNumIsr->tIsrHandle.arg1;
    pQueue->tCsi2IsrHdl.tIsrHandle.arg2 = pCsiLineNumIsr->tIsrHandle.arg2;
    pQueue->tCsi2IsrHdl.nVsize = pCsiLineNumIsr->nVsize;

    *ppQueue = pQueue;
    return MSP_ERROR_NONE;
EXIT_1:
    return MSP_ERROR_FAIL;
}

/* ========================================================================== */
/**
* Csi2UnRegisterCB  
*
* @param    **ppQueue:
* @param    *pCsiDevStruct:
* @param    tConfigIndex:  -  
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
*
* @pre There is no pre conditions.
*
* @post There is no post conditions.
*
* @see
*
*/
/* ========================================================================== */
MSP_ERROR_TYPE Csi2UnRegisterCB(CSI2_IsrHandle **ppQueue, MSP_CsiDev *pCsiDevStruct, MSP_CSI2CfgIndex tConfigIndex)
{
    CSI2_IsrHandle*         pTempQueue = NULL;
    CSI2_IsrHandle*         pQueue = NULL;

    pQueue = pCsiDevStruct->pCsi2IsrHdlFirst;
    while((pQueue != NULL) && (pQueue->eIrqIdx != tConfigIndex)){
        pTempQueue = pQueue;
        pQueue = pQueue->pNext;
    }
    GOTO_EXIT_IF(((NULL == pTempQueue) && (NULL == pQueue)),1);
    if(pQueue != NULL){
        if(NULL == pTempQueue){
            pCsiDevStruct->pCsi2IsrHdlFirst = pQueue->pNext;//// Element for unregistering is at first place
        }else{
            pTempQueue->pNext = pQueue->pNext;// Element for unregistering is after first element
        }
    }else{
        pQueue = pTempQueue;// Element for unregistering is alone in the queue
        pCsiDevStruct->pCsi2IsrHdlFirst = NULL;
    }

    *ppQueue = pQueue;
    return MSP_ERROR_NONE;
EXIT_1:
    return MSP_ERROR_FAIL;
}

/* ========================================================================== */
/**
* MSP_CSI_query  
*
* @param    handle:
* @param    tQueryIndex:
* @param    pQueryParam:  -  
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
*
* @pre There is no pre conditions.
*
* @post There is no post conditions.
*
* @see
*
*/
/* ========================================================================== */
MSP_ERROR_TYPE MSP_CSI_query(MSP_HANDLE handle, MSP_INDEXTYPE tQueryIndex,
                                MSP_PTR pQueryParam)
{

    //MSP_ERROR_TYPE          retCode = MSP_ERROR_NONE;
    IssCSI2DevT             eCsiInstance;
    MSP_COMPONENT_TYPE*     hMSP;
    MSP_CsiDev*             pCsiDevStruct;

    GOTO_EXIT_IF(((handle == NULL) || (pQueryParam == NULL)), 1);
    hMSP  = (MSP_COMPONENT_TYPE*) handle;
    pCsiDevStruct = (MSP_CsiDev *)hMSP->pCompPrivate;
    eCsiInstance = pCsiDevStruct->eCsiInstance;

    switch((MSP_CsiQueryIndexT)tQueryIndex) {
        case MSP_CSI2_GET_LATEST_BUFFER:
            {
                MSP_U32 *buff_idx;
                buff_idx = (MSP_U32 *)pQueryParam;
                *buff_idx = issCsi2rxGetLatestBuffer(eCsiInstance, 0);
            }
            break;
        default:
            return MSP_ERROR_FAIL;

    }

    return MSP_ERROR_NONE;
EXIT_1:
    return MSP_ERROR_FAIL;
}

/* ========================================================================== */
/**
* MSP_CSI_config  
*
* @param    handle:
* @param    tConfigIndex:
* @param    pConfigParam:  -  
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
*
* @pre There is no pre conditions.
*
* @post There is no post conditions.
*
* @see
*
*/
/* ========================================================================== */
MSP_ERROR_TYPE MSP_CSI_config(MSP_HANDLE handle, MSP_INDEXTYPE tConfigIndex,
                                 MSP_PTR pConfigParam)
{
    MSP_ERROR_TYPE          retCode = MSP_ERROR_NONE;
    MSP_COMPONENT_TYPE*     hMSP;
    CSI2_IsrHandle*         pQueue;
    MSP_CsiDev*             pCsiDevStruct;
    IssCSI2DevT             eCsiInstance;

    GOTO_EXIT_IF((handle == NULL) , 1);

    hMSP  = (MSP_COMPONENT_TYPE*) handle;
    pCsiDevStruct = (MSP_CsiDev *)hMSP->pCompPrivate;
    eCsiInstance = pCsiDevStruct->eCsiInstance;

    switch((MSP_CSI2CfgIndex)tConfigIndex) {
        case MSP_CSI2_HOOK_LINENUM_ISR:
            {
                IssCsi2rxIsrHandleLineNumT *pCsiLineNumIsr;
                pCsiLineNumIsr = (IssCsi2rxIsrHandleLineNumT *)pConfigParam;

                retCode = Csi2RegisterCB(&pQueue, pCsiDevStruct, pCsiLineNumIsr, (MSP_CSI2CfgIndex)tConfigIndex);
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
                retCode = issCsi2rxHookLineNumIsr (eCsiInstance, 0, &(pQueue->tCsi2IsrHdl.tIsrHandle), pQueue->tCsi2IsrHdl.nVsize);
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 2);
            }
            break;
        case MSP_CSI2_UNHOOK_LINENUM_ISR:
            {
                //IssCsi2rxIsrHandleLineNumT *pCsiLineNumIsr;
                //pCsiLineNumIsr = (IssCsi2rxIsrHandleLineNumT *)pConfigParam;
                if(pCsiDevStruct->pCsi2IsrHdlFirst != NULL){
                    retCode = Csi2UnRegisterCB(&pQueue, pCsiDevStruct, MSP_CSI2_HOOK_LINENUM_ISR);
                    GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);

                    retCode = issCsi2rxUnhookLineNumIsr(eCsiInstance, 0, &(pQueue->tCsi2IsrHdl.tIsrHandle));
                    GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
                    MSP_MEMFREE(pQueue);
                }else{
                    Vps_rprintf("\n CSI2 IRQ Already unregistered  !.\n");
                }
            }
            break;
        case MSP_CSI2_ENABLE_LINENUM_ISR:
                issCsi2rxLineNumIrqEnable (eCsiInstance, *(MSP_U32*)pConfigParam);
            break;
        case MSP_CSI2_DISABLE_LINENUM_ISR:
                issCsi2rxLineNumIrqDisable(eCsiInstance, *(MSP_U32*)pConfigParam);
            break;

        case MSP_CSI2_HOOK_FRAME_END_ISR:
            {
                IssCsi2rxIsrHandleLineNumT *pCsiLineNumIsr;
                pCsiLineNumIsr = (IssCsi2rxIsrHandleLineNumT *)pConfigParam;

                retCode = Csi2RegisterCB(&pQueue, pCsiDevStruct, pCsiLineNumIsr, (MSP_CSI2CfgIndex)tConfigIndex);
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);

                retCode = issCsi2rxHookFrameEndIsr(eCsiInstance, 0, &(pQueue->tCsi2IsrHdl.tIsrHandle));
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 2);
            }
            break;
        case MSP_CSI2_UNHOOK_FRAME_END_ISR:
            {
                if(pCsiDevStruct->pCsi2IsrHdlFirst != NULL){
                    retCode = Csi2UnRegisterCB(&pQueue, pCsiDevStruct, MSP_CSI2_HOOK_FRAME_END_ISR);
                    GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);

                    retCode = issCsi2rxUnhookFrameEndIsr(eCsiInstance, 0, &(pQueue->tCsi2IsrHdl.tIsrHandle));
                    GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
                    MSP_MEMFREE(pQueue);
                }else{
                    Vps_rprintf("\n CSI2 IRQ Already unregistered  !.\n");
                }
            }
            break;

        case MSP_CSI2_ENABLE_FRAMEEND_ISR:
                issCsi2rxFrameEndIrqEnable(eCsiInstance, *(MSP_U32*)pConfigParam);
            break;
        case MSP_CSI2_DISABLE_FRAMEEND_ISR:
                issCsi2rxFrameEndIrqDisable(eCsiInstance, *(MSP_U32*)pConfigParam);
            break;
        case MSP_CSI2_ENABLE_START_FRAME_ISR:
        case MSP_CSI2_ENABLE_FRAMESTART_ISR:
                issCsi2rxFrameStartIrqEnable(eCsiInstance, *(MSP_U32*)pConfigParam);
            break;
        case MSP_CSI2_DISABLE_START_FRAME_ISR:
                issCsi2rxFrameStartIrqDisable(eCsiInstance, *(MSP_U32*)pConfigParam);
            break;
        case MSP_CSI2_HOOK_START_FRAME_ISR:{
                IssCsi2rxIsrHandleLineNumT *pCsiLineNumIsr;
                pCsiLineNumIsr = (IssCsi2rxIsrHandleLineNumT *)pConfigParam;
                retCode = Csi2RegisterCB(&pQueue, pCsiDevStruct, pCsiLineNumIsr, (MSP_CSI2CfgIndex)tConfigIndex);
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
                retCode = issCsi2rxHookFrameStartIsr(eCsiInstance, 0, &(pQueue->tCsi2IsrHdl.tIsrHandle));
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 2);
            }
            break;
        case MSP_CSI2_UNHOOK_START_FRAME_ISR:{
                if(pCsiDevStruct->pCsi2IsrHdlFirst != NULL){
                    retCode = Csi2UnRegisterCB(&pQueue, pCsiDevStruct, MSP_CSI2_HOOK_START_FRAME_ISR);
                    GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
                    retCode = issCsi2rxUnhookFrameStartIsr(eCsiInstance, 0, &(pQueue->tCsi2IsrHdl.tIsrHandle));
                    GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
                    MSP_MEMFREE(pQueue);
                }else{
                    Vps_rprintf("\n CSI2 IRQ Already unregistered  !.\n");
                }
            }
            break;
        case MSP_CSI2_CONFIG_PING_PONG:
            {
                IssCsi2rxConfigPingPong *pConfigPingPong;
                pConfigPingPong = (IssCsi2rxConfigPingPong *)pConfigParam;
                retCode = issCsi2rxConfigPingPong(pConfigPingPong->buff_idx, pConfigPingPong->pBuff, eCsiInstance, 0);
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
            }
            break;
        case MSP_CSI2_CONFIG_ENABLE_CONTEXT:{
                MSP_U32 context = *(MSP_U32*)pConfigParam;
                csi2_enable_context ((CSI2_DEVICE_T)eCsiInstance, (csi2_context_number)context); 
            }
            break;
        case MSP_CSI2_CONFIG_DISABLE_CONTEXT:{
                MSP_U32 context = *(MSP_U32*)pConfigParam;
                csi2_disable_context ((CSI2_DEVICE_T)eCsiInstance, (csi2_context_number)context); 
            }
            break;
        case MSP_CSI2_CONFIG_RESET:
                issCsi2rxReset(eCsiInstance);
            break;
        case MSP_CSI2_CONFIG_CTX:{
                CAM_CSI_Configuration *pCamCsiCfg;
                pCamCsiCfg = (CAM_CSI_Configuration *)pConfigParam;
                issCsi2rxConfigCtx (eCsiInstance,
                                    pCamCsiCfg->nContextIdx,
                                    &pCsiDevStruct->tIssCsi2Config,
                                    pCamCsiCfg->nContextBuffPrm[pCamCsiCfg->nContextIdx].pCsiBuffs[0],
                                    pCamCsiCfg->nContextBuffPrm[pCamCsiCfg->nContextIdx].pCsiBuffs[1],
                                    pCamCsiCfg->nContextBuffPrm[pCamCsiCfg->nContextIdx].bufferLineSizeBytes,
                                    pCamCsiCfg->numLanes,
                                    pCamCsiCfg->csi2_speed_MHzs);
            }
            break;
        case MSP_CSI2_CONFIG_RX:{
                CAM_CSI_Configuration *pCamCsiCfg;
                pCamCsiCfg = (CAM_CSI_Configuration *)pConfigParam;
                issCsi2rxConfig (eCsiInstance, &pCsiDevStruct->tIssCsi2Config, &pCamCsiCfg->transcode);
            }
            break;
        case MSP_CSI2_CONFIG_INIT:
                issCsi2rxInit();
            break;
        case MSP_CSI2_CONFIG_TRANSCODE:
            {
                IssCsi2rxTranscodeParams_t* sizes;
                sizes = (IssCsi2rxTranscodeParams_t *)pConfigParam;
                retCode = issCsi2rxTranscodeConfig(eCsiInstance,sizes, 0);
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
            }
            break;
        case MSP_CSI2_CONFIG_INTLINENUM:
            {
                MSP_U32 v_size;
                v_size = *(MSP_U32 *)pConfigParam;
                retCode = issCsi2rxSetIntLineNum(eCsiInstance, 0, v_size);
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
            }
            break;
        case MSP_CSI2_CONFIG_HOOK_FIFO_OVF:
            {
                IssCsi2rxIsrHandleLineNumT *pCsiLineNumIsr;
                pCsiLineNumIsr = (IssCsi2rxIsrHandleLineNumT *)pConfigParam;
                retCode = Csi2RegisterCB(&pQueue, pCsiDevStruct, pCsiLineNumIsr, (MSP_CSI2CfgIndex)tConfigIndex);
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
                retCode = issCsi2rxHookFifoOvfIsr(eCsiInstance, &(pQueue->tCsi2IsrHdl.tIsrHandle));
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 2);
            }
            break;
        case MSP_CSI2_CONFIG_UNHOOK_FIFO_OVF:
            {
                if(pCsiDevStruct->pCsi2IsrHdlFirst != NULL){
                    retCode = Csi2UnRegisterCB(&pQueue, pCsiDevStruct, MSP_CSI2_CONFIG_HOOK_FIFO_OVF);
                    GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
                    retCode = issCsi2rxUnhookFifoOvfIsr(eCsiInstance, &(pQueue->tCsi2IsrHdl.tIsrHandle));
                    GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
                    MSP_MEMFREE(pQueue);
                } else {
                    Vps_rprintf("\n CSI2 IRQ Already unregistered  !.\n");
                }
            }
            break;
        case MSP_CSI2_CONFIG_ENABLE_FIFO_OVF_ISR:
            {
                retCode = issCsi2rxFifoOvfEnable(eCsiInstance);
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
            }
            break;
        case MSP_CSI2_CONFIG_DISABLE_FIFO_OVF_ISR:
            {
                retCode = issCsi2rxFifoOvfDisable(eCsiInstance);
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
            }
            break;
        case MSP_CSI2_CONFIG_USER_PARAMS_COMPLEXIO:
            {
               // retCode = CSI2_UserParamsCfgIO(eCsiInstance, pCsiDevStruct, pCsiDevStruct->pModulsCsi->tHwHdls[CAM_CSI_INTERFACE].pHwCfg->pCfgParam, *(MSP_U32 *)pConfigParam);
                GOTO_EXIT_IF((retCode != MSP_ERROR_NONE) , 1);
            }
            break;
        default:
            return MSP_ERROR_FAIL;
    }
    return MSP_ERROR_NONE;
EXIT_2:
    MSP_MEMFREE(pQueue);
EXIT_1:
    return MSP_ERROR_FAIL;

}


/* ========================================================================== */
/**
* MSP_CSI_control  
*
* @param    handle:
* @param    tCmd:
* @param    pCmdParam:  -  
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
*
* @pre There is no pre conditions.
*
* @post There is no post conditions.
*
* @see
*
*/
/* ========================================================================== */
MSP_ERROR_TYPE MSP_CSI_control(MSP_HANDLE handle, MSP_CTRLCMD_TYPE tCmd,
                                  MSP_PTR pCmdParam)
{
    IssCSI2DevT eCsiInstance;
    MSP_COMPONENT_TYPE* hMSP;

    GOTO_EXIT_IF((NULL == handle), 1);
    hMSP = (MSP_COMPONENT_TYPE*)handle;
    eCsiInstance = ((MSP_CsiDev*)(hMSP->pCompPrivate))->eCsiInstance;
    switch(tCmd)
    {
        case MSP_CTRLCMD_START:
            issCsi2rxStart (eCsiInstance, *(MSP_U32*)pCmdParam);
            break;
        case MSP_CTRLCMD_STOP:
            issCsi2rxStop (eCsiInstance);
            break;
        case MSP_CTRLCMD_PAUSE:
        case MSP_CTRLCMD_RESUME:
        case MSP_CTRLCMD_FLUSH:
        case MSP_CTRLCMD_EXTENDED:
            break;
        default:
            return MSP_ERROR_FAIL;;
    }
    return MSP_ERROR_NONE;
EXIT_1:
    return MSP_ERROR_FAIL;
}

/* ========================================================================== */
/**
* MSP_CSI_open  
*
* @param    handle:
* @param    pCreateParam:  -  
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
*
* @pre There is no pre conditions.
*
* @post There is no post conditions.
*
* @see
*
*/
/* ========================================================================== */
/*MSP_ERROR_TYPE MSP_CSI_open( MSP_HANDLE handle, MSP_PTR pCreateParam )
{
    CAM_Module* pModuleHDL;
    MSP_COMPONENT_TYPE* hMSP;
    MSP_CsiDev* pCsiDevStruct;

    GOTO_EXIT_IF(((handle==NULL)||(pCreateParam==NULL)), 1);

    hMSP = (MSP_COMPONENT_TYPE *)handle;
    pCsiDevStruct = (MSP_CsiDev*)(hMSP->pCompPrivate);

    pCsiDevStruct->pModulsCsi = (CAM_Module*)pCreateParam;//link to Csi params!!;

    pModuleHDL = pCsiDevStruct->pModulsCsi;

	pCsiDevStruct->eCsiInstance = ISS_CSI_DEV_A;
    
    return MSP_ERROR_NONE;
EXIT_1:
    return MSP_ERROR_FAIL;
}*/

/* ========================================================================== */
/**
* MSP_CSI_close  
*
* @param    handle:  -  
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
*
* @pre There is no pre conditions.
*
* @post There is no post conditions.
*
* @see
*
*/
/* ========================================================================== */
MSP_ERROR_TYPE MSP_CSI_close(MSP_HANDLE handle)
{
    MSP_ERROR_TYPE retCode = MSP_ERROR_NONE;
    MSP_COMPONENT_TYPE* hMSP = handle;

    issCsi2rxStop (((MSP_CsiDev *)(hMSP->pCompPrivate))->eCsiInstance);
    ((MSP_CsiDev *)(hMSP->pCompPrivate))->eCsiInstance = (IssCSI2DevT)0xBAD0;//TODO
    return (retCode);
}


/* ========================================================================== */
/**
* MSP_CSI_process  
*
* @param    handle:
* @param    pArg:
* @param    *ptBufHdr:  -  
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
*
* @pre There is no pre conditions.
*
* @post There is no post conditions.
*
* @see
*
*/
/* ========================================================================== */
MSP_ERROR_TYPE MSP_CSI_process(MSP_HANDLE handle, MSP_PTR pArg, MSP_BUFHEADER_TYPE *ptBufHdr )
{
    MSP_ERROR_TYPE retCode = MSP_ERROR_NONE;
    return retCode;
}


/* ========================================================================== */
/**
* MSP_CSI_deInit  
*
* @param    handle:  -  
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
*
* @pre There is no pre conditions.
*
* @post There is no post conditions.
*
* @see
*
*/
/* ========================================================================== */
MSP_ERROR_TYPE MSP_CSI_deInit(MSP_HANDLE handle)
{
    MSP_COMPONENT_TYPE* hMSP;
    MSP_CsiDev*         pCsiDevStruct;

    GOTO_EXIT_IF((handle == NULL), 1);

    issCsi2rxDeinit();

    hMSP  = (MSP_COMPONENT_TYPE*) handle;
    pCsiDevStruct = (MSP_CsiDev*)hMSP->pCompPrivate;
    MSP_MEMFREE(pCsiDevStruct);

    return MSP_ERROR_NONE;
EXIT_1:
    return MSP_ERROR_FAIL;
}


/* ========================================================================== */
/**
* MSP_CSI_init  
*
* @param    *hMSP:
* @param    tProfile:  -  
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
*
* @pre There is no pre conditions.
*
* @post There is no post conditions.
*
* @see
*
*/
/* ========================================================================== */
#if 0
MSP_ERROR_TYPE MSP_CSI_init(MSP_COMPONENT_TYPE *hMSP, MSP_PROFILE_TYPE tProfile)
{
    MSP_COMPONENT_TYPE* pComponent = hMSP;
    MSP_CsiDev* pCsiDevStruct;

    /* Allocate memory to the Csi Device Struct */
    pComponent->pCompPrivate = (MSP_CsiDev *)MSP_MEMALLOC(sizeof(MSP_CsiDev));
    GOTO_EXIT_IF((pComponent->pCompPrivate == NULL), 1);
    MSP_MEMSET(pComponent->pCompPrivate, sizeof(MSP_CsiDev));

    pCsiDevStruct = pComponent->pCompPrivate;

    //pCsiDevStruct->devHandle = MSP_CSI_INVALID_HANDLE;
    pCsiDevStruct->inUse = MSP_FALSE;
    
    /* Populate the MSP function pointers */
    pComponent->open = MSP_CSI_open;
    pComponent->control = MSP_CSI_control;
    pComponent->close = MSP_CSI_close;
    pComponent->deInit = MSP_CSI_deInit;
    pComponent->process = MSP_CSI_process;
    pComponent->config = MSP_CSI_config;
    pComponent->query = MSP_CSI_query;

    return MSP_ERROR_NONE;
EXIT_1:
    return MSP_ERROR_FAIL;
}
#endif


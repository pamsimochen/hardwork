/* ============================================================================ 
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved.  Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ========================================================================== 
 */
/**
 * @file    isp_msp.c
 *
 * This file contains MSP interface for CSI2 transmitter driver. Uses functions
 * defined in csi2tx_interface.c
 *
 * @path    iss/drivers/drv_isp/src
 *
 * @rev     2.0
*/
/* -------------------------------------------------------------------------- 
 */
/* 
 *! Revision History
 *! ===================================
 *! 30-Jun-2009  Author: Pavel Nedev - All MSP functions revised and modified.
 *!              This is the first working driver version
 *!
 *! 05-Jun-2009  Author: Pavel Nedev - First release
 *!
 * ========================================================================== */
// TODO Comment ALL !!!
/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
// #include <ti/sysbios/BIOS.h>
#include <stdlib.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/psp/vps/common/vps_config.h>
// #include <msp/msp_utils.h>
#include <ti/psp/iss/core/inc/iss_comp.h>
#include <ti/psp/iss/core/isp_msp.h>
#include <ti/psp/iss/core/inc/isp_bsc.h>
#include "../isp_msp.h"
// #include <ti/iss/drivers/drv_camera/sensor_drv/inc/sensor_config.h>

#include <ti/psp/iss/core/inc/iss_drv.h>

#define GOTO_EXIT(N)                                                                                        \
    do {                                                                                                    \
        goto EXIT_##N;                                                                                      \
    } while(0)

    // TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_DRIVERS, "\nError in %s,
    // line:%d\n",__FILE__, __LINE__);

#define GOTO_EXIT_IF(_CONDITION,_ERROR) { \
            if((_CONDITION)) { \
			    Vps_rprintf ("Error :: %s : %s : %d :: ", __FILE__, __FUNCTION__, \
                        __LINE__); \
                Vps_rprintf (" Exiting because: %s \n", #_CONDITION); \
                goto EXIT_##_ERROR; \
            } \
}

#define MSP_MEMALLOC(SIZE)		malloc(SIZE, 32);
#define MSP_MEMFREE(PTR)		free(PTR);

#include <ti/psp/iss/hal/iss/csi2/csi2.h>
#include <ti/psp/iss/hal/iss/isp/common/inc/isp_common.h>

/******************************************************************************
 * GLOBAL DECLARATIONS
 ******************************************************************************/

extern ccp2_regs_ovly ccp2_reg;

extern csi2_regs_ovly csi2A_regs;

extern iss_regs_ovly iss_regs;

extern isp_regs_ovly isp_regs;

extern rsz_A_regs_ovly rszA_reg;

extern isif_regs_ovly isif_reg;

extern rsz_regs_ovly rsz_reg;

#define ISP_WAIT_QUEUE_LEN          (10)

typedef struct {
    MSP_BOOL bThisIsStarted;
    MSP_BOOL bWaitForCcpRx;
    MSP_IspIntHandleQueueT *pIspIntQueueFirst;
    iss_drv_config_t iss_drv_config;
} ispDriverInstanceT;

MSP_U32 Invocations = 0;

MSP_U32 HwiState;

MSP_BOOL Started;

MSP_HANDLE WaitQueue[ISP_WAIT_QUEUE_LEN];

MSP_U32 WaitQueueIdxCurr;

MSP_U32 WaitQueueIdxEnd;

MSP_U32 WaitQueueIdxElems;

// ====================== Local Routines
// ======================================
/* ===================================================================
 *  @func     ispLock                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static inline void ispLock(void)
{
    HwiState = Hwi_disable();
}

/* ===================================================================
 *  @func     ispUnlock                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static inline void ispUnlock(void)
{
    Hwi_restore(HwiState);
}

/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* ===================================================================
 *  @func     isp_test_isif_linenum_handle                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void isp_test_isif_linenum_handle(ISP_RETURN status, uint32 arg1, void *arg2);

/* ===================================================================
 *  @func     MSP_IspIsrCallback                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void MSP_IspIsrCallback(ISS_RETURN status, uint32 arg1, void *arg2)
{
    MSP_COMPONENT_TYPE *hMSP;

    hMSP = (MSP_COMPONENT_TYPE *) arg2;

    hMSP->tAppCBParam.MSP_callback(hMSP,
                                   hMSP->tAppCBParam.pAppData,
                                   MSP_DATA_EVENT, arg1, 0);
};

/* ===================================================================
 *  @func     MSP_IssIsrCallback                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void MSP_IssIsrCallback(ISS_RETURN status, uint32 arg1, void *arg2)
{
    MSP_COMPONENT_TYPE *hMSP;

    hMSP = (MSP_COMPONENT_TYPE *) arg2;

    issDrvDisableIssModuleIrq((MSP_IssInterruptIdT) arg1);

    hMSP->tAppCBParam.MSP_callback(hMSP,
                                   hMSP->tAppCBParam.pAppData,
                                   MSP_DATA_EVENT, arg1, 0);
};

/* ========================================================================== 
 */
/**
 * @fn  MSP_ISP_open()    MSP ISP device driver open function.
 *
 * @see  isp_msp.h
*/
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     MSP_ISP_open                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MSP_ERROR_TYPE MSP_ISP_open(MSP_HANDLE handle, MSP_PTR pCreateParam)
{
    MSP_ERROR_TYPE eResult = MSP_ERROR_NONE;

    return (eResult);
}

/* ========================================================================== 
 */
/**
 * @fn  MSP_ISP_control()    MSP ISP device driver control function.
 * Issues commands to ISP device
 *
 * @see  isp_msp.h
*/
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     MSP_ISP_control                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MSP_ERROR_TYPE MSP_ISP_control(MSP_HANDLE handle, MSP_CTRLCMD_TYPE tCmd,
                               MSP_PTR pCmdParam)
{
    MSP_ERROR_TYPE eResult = MSP_ERROR_NONE;

    MSP_COMPONENT_TYPE *hMSP;

    MSP_COMPONENT_TYPE *hNextMSP;

    ispDriverInstanceT *ispDev;

    ispDriverInstanceT *ispDevNext;

    GOTO_EXIT_IF((NULL == handle), 3);
    hMSP = (MSP_COMPONENT_TYPE *) handle;
    ispDev = (ispDriverInstanceT *) hMSP->pCompPrivate;
    switch (tCmd)
    {
        case MSP_CTRLCMD_START:
            ispLock();
            if (MSP_TRUE == Started)
            {
                GOTO_EXIT_IF((MSP_TRUE == ispDev->bThisIsStarted), 2);
                GOTO_EXIT_IF((ISP_WAIT_QUEUE_LEN == WaitQueueIdxElems), 2);
                WaitQueue[WaitQueueIdxEnd] = handle;
                WaitQueueIdxElems++;
                WaitQueueIdxEnd++;
                if (WaitQueueIdxEnd == ISP_WAIT_QUEUE_LEN)
                {
                    WaitQueueIdxEnd = 0;
                }
            }
            else
            {
                Started = MSP_TRUE;
                ispDev->bThisIsStarted = MSP_TRUE;
                hMSP->tAppCBParam.MSP_callback(hMSP,
                                               hMSP->tAppCBParam.pAppData,
                                               MSP_CTRLCMD_EVENT,
                                               MSP_CALLBACK_CMD_START, 0);
                /* hMSP->tAppCBParam.MSP_callback(NULL, NULL, NULL, NULL, 0); */
            }
            ispUnlock();
            break;

        case MSP_CTRLCMD_STOP:
            ispLock();

            GOTO_EXIT_IF((MSP_TRUE != Started), 2);
            GOTO_EXIT_IF((MSP_FALSE == ispDev->bThisIsStarted), 2);
            if (0 == WaitQueueIdxElems)
            {
                Started = MSP_FALSE;
                hMSP->tAppCBParam.MSP_callback(hMSP,
                                               hMSP->tAppCBParam.pAppData,
                                               MSP_CTRLCMD_EVENT,
                                               MSP_CALLBACK_CMD_STOP, 0);
            }
            else
            {
                hNextMSP = (MSP_COMPONENT_TYPE *) WaitQueue[WaitQueueIdxCurr];
                WaitQueueIdxElems--;
                WaitQueueIdxCurr++;
                if (WaitQueueIdxCurr == ISP_WAIT_QUEUE_LEN)
                {
                    WaitQueueIdxCurr = 0;
                }
                ispDev->bThisIsStarted = MSP_FALSE;
                hMSP->tAppCBParam.MSP_callback(hMSP,
                                               hMSP->tAppCBParam.pAppData,
                                               MSP_CTRLCMD_EVENT,
                                               MSP_CALLBACK_CMD_STOP, 0);

                ispDevNext = (ispDriverInstanceT *) hNextMSP->pCompPrivate;
                ispDevNext->bThisIsStarted = MSP_TRUE;
                hNextMSP->tAppCBParam.MSP_callback(hNextMSP,
                                                   hNextMSP->tAppCBParam.
                                                   pAppData, MSP_CTRLCMD_EVENT,
                                                   MSP_CALLBACK_CMD_START, 0);
            }
            ispUnlock();

            break;

        case MSP_CTRLCMD_PAUSE:
        case MSP_CTRLCMD_RESUME:
        case MSP_CTRLCMD_FLUSH:
        case MSP_CTRLCMD_EXTENDED:
            break;

        default:
            eResult = MSP_ERROR_INVALIDCMD;
            break;
    }

    return (eResult);
  EXIT_2:
    ispUnlock();
    eResult = MSP_ERROR_FAIL;
    return (eResult);
  EXIT_3:
    eResult = MSP_ERROR_NULLPTR;
    return (eResult);
}

/* ========================================================================== 
 */
/**
 * @fn  MSP_ISP_process()    MSP ISP device driver process function.
 * Empty (not used)
 *
 * @see  isp_msp.h
*/
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     MSP_ISP_process                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MSP_ERROR_TYPE MSP_ISP_process(MSP_HANDLE handle, MSP_PTR pArg,
                               MSP_BUFHEADER_TYPE * ptBufHdr)
{
    MSP_ERROR_TYPE eResult = MSP_ERROR_NONE;

    ISP_RETURN eTempResult = ISP_SUCCESS;

    MSP_COMPONENT_TYPE *hMSP;

    MSP_IspProcessParamT *IspProcessParams;

    ispDriverInstanceT *ptIspDev;

    volatile int cnt;

    GOTO_EXIT_IF(((handle == NULL) || (pArg == NULL)), 2);

    hMSP = (MSP_COMPONENT_TYPE *) handle;
    ptIspDev = (ispDriverInstanceT *) hMSP->pCompPrivate;
    IspProcessParams = (MSP_IspProcessParamT *) pArg;

    GOTO_EXIT_IF((MSP_FALSE == ptIspDev->bThisIsStarted), 4);

    switch (IspProcessParams->eCmd)
    {

        case MSP_ISP_CMD_H3A_START:
            GOTO_EXIT_IF((ISP_SUCCESS != issDrvH3aStart()), 4);
            break;

        case MSP_ISP_CMD_H3A_STOP:
            GOTO_EXIT_IF((ISP_SUCCESS != issDrvH3aStop()), 4);
            break;

        case MSP_ISP_CMD_STOP_PROCESSING:
            if (ptIspDev->bWaitForCcpRx)
            {
                ptIspDev->bWaitForCcpRx = MSP_FALSE;
                while (!CCP2_get_irq_status(EOF_IRQ))
                {

                    // CASH_WA_ENB_INTERRUPTS();
                    Task_yield();
                }
                CCP2_clear_irq_status(EOF_IRQ);
            }
            issDrvProcessingStop();
            break;

        case MSP_ISP_CMD_STOP_ISIF:
        {
            MSP_IspStopIsifCmdParamT *pStopIsif =
                (MSP_IspStopIsifCmdParamT *) IspProcessParams->pCmdData;
            issDrvStopIsif(pStopIsif->nFrames, pStopIsif->nWidth,
                           pStopIsif->nHeight, pStopIsif->pInBuffer);
        }
            break;

        case MSP_ISP_CMD_CCP_TO_ISIF_STOP:
            issDrvCcpToIsifStop();
            break;

        case MSP_ISP_CMD_MEM_TO_MEM_START:
        {
            MSP_IspMemToMemCmdParamT *pIspMemToMemCmdParam;

            pIspMemToMemCmdParam =
                (MSP_IspMemToMemCmdParamT *) IspProcessParams->pCmdData;
            GOTO_EXIT_IF((NULL == pIspMemToMemCmdParam), 2);

            if (MSP_IPIPE_IN_FORMAT_BAYER == pIspMemToMemCmdParam->eInFormat)
            {
                ptIspDev->bWaitForCcpRx = MSP_TRUE;
            }
            // TODO: Remove this when select nontiled space

            eTempResult = issDrvMemToMemStart(&ptIspDev->iss_drv_config,
                                              pIspMemToMemCmdParam->pInBuffer,
                                              pIspMemToMemCmdParam->
                                              pOutbuffRszA,
                                              pIspMemToMemCmdParam->
                                              pOutbuffRszB,
                                              pIspMemToMemCmdParam->
                                              pOutbuffRszACr,
                                              pIspMemToMemCmdParam->
                                              pOutbuffRszBCr,
                                              (IPIPE_IN_FORMAT)
                                              pIspMemToMemCmdParam->eInFormat,
                                              pIspMemToMemCmdParam->
                                              pOutRawBuffer);
            GOTO_EXIT_IF((ISP_SUCCESS != eTempResult), 4);

        }
            break;

        case MSP_ISP_CMD_SENSOR_TO_MEM_START:
        {
            MSP_IspSenToMemCmdParamT *pIspMemToMemCmdParam;

            pIspMemToMemCmdParam =
                (MSP_IspSenToMemCmdParamT *) IspProcessParams->pCmdData;
            GOTO_EXIT_IF((NULL == pIspMemToMemCmdParam), 2);
            eTempResult =
                issDrvVpToIsifStart(&ptIspDev->iss_drv_config,
                                    pIspMemToMemCmdParam->pOutBuffer);
            GOTO_EXIT_IF((ISP_SUCCESS != eTempResult), 4);
        }
            break;

        case MSP_ISP_CMD_SENSOR_SYNC:
        {
            MSP_IspSenToMemCmdParamT *pIspSenToMemCmdParam;

            pIspSenToMemCmdParam =
                (MSP_IspSenToMemCmdParamT *) IspProcessParams->pCmdData;
            issDrvVpToIsifStart(&ptIspDev->iss_drv_config,
                                pIspSenToMemCmdParam->pOutBuffer);
        }
            break;

        case MSP_ISP_CMD_CCP_TO_ISIF_START:
        {
            MSP_IspCcpToIsifCmdParamT *pIspSenToMemCmdParam;

            pIspSenToMemCmdParam =
                (MSP_IspCcpToIsifCmdParamT *) IspProcessParams->pCmdData;
            issDrvCcpToIsifStart(&ptIspDev->iss_drv_config,
                                 pIspSenToMemCmdParam->pInBuffer,
                                 pIspSenToMemCmdParam->pOutBuffer);
        }
            break;

        case MSP_ISP_CMD_DARK_FRAME_START:
        {
            MSP_IspDarkFrameCmdParamT *pDarkFrameCmdParam;

            pDarkFrameCmdParam =
                (MSP_IspDarkFrameCmdParamT *) IspProcessParams->pCmdData;
            GOTO_EXIT_IF((NULL == pDarkFrameCmdParam), 2);
            eTempResult = issDrvDarkToRawStart(pDarkFrameCmdParam->pOutBuffer,
                                               pDarkFrameCmdParam->pDfsAddress);
            GOTO_EXIT_IF((ISP_SUCCESS != eTempResult), 4);
        }
            break;

        default:
            GOTO_EXIT(3);
    }

    return (eResult);
  EXIT_2:
    eResult = MSP_ERROR_NULLPTR;
    return (eResult);
  EXIT_3:
    eResult = MSP_ERROR_INVALIDCMD;
    return (eResult);
  EXIT_4:
    eResult = MSP_ERROR_FAIL;
    return (eResult);
}

/* Hack for DCC Integration */
iss_drv_config_t *iss_drv_config;

/* ========================================================================== 
 */
/**
 * @fn  MSP_ISP_config()    MSP ISP device driver config function.
 * Writes different config settings of the ISP device
 *
 * @see  isp_msp.h
*/
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     MSP_ISP_config                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MSP_ERROR_TYPE MSP_ISP_config(MSP_HANDLE handle, MSP_INDEXTYPE tConfigIndex,
                              MSP_PTR pConfigParam)
{
    MSP_ERROR_TYPE eResult = MSP_ERROR_NONE;

    MSP_COMPONENT_TYPE *hMSP;

    ispDriverInstanceT *ptIspDev;

    MSP_IspIntHandleQueueT *pQueue = NULL;

    GOTO_EXIT_IF(((handle == NULL) || (pConfigParam == NULL)), 2);

    hMSP = (MSP_COMPONENT_TYPE *) handle;
    ptIspDev = (ispDriverInstanceT *) hMSP->pCompPrivate;

    GOTO_EXIT_IF((MSP_FALSE == ptIspDev->bThisIsStarted), 4);

    switch ((MSP_CfgIndexT) tConfigIndex)
    {

        case MSP_ISP_CFG_VD_ISR:
        {
            MSP_IspVdLineNumberCfgT *MSP_IspVdLineNumberCfg;

            MSP_IspVdLineNumberCfg = (MSP_IspVdLineNumberCfgT *) pConfigParam;

            issDrvSetVdLineNumber(MSP_IspVdLineNumberCfg->nVdId,
                                  MSP_IspVdLineNumberCfg->nLineNumber);
        }
            break;

        case MSP_ISP_CFG_2DLSC_EN:
        {
            MSP_Isp2DLscCfgT *pMSP_Isp2DLscCfg;

            pMSP_Isp2DLscCfg = (MSP_Isp2DLscCfgT *) pConfigParam;

            isif_2dlsc_enable(pMSP_Isp2DLscCfg->bEnable);
        }
            break;
        case MSP_ISP_CFG_SET_LSC_TABLE:
        {
            MSP_Isp2DLscTableCfgT *pMSP_Isp2DLscTableCfg;

            pMSP_Isp2DLscTableCfg = (MSP_Isp2DLscTableCfgT *) pConfigParam;

            issDrvSetNew2DLscTable(pMSP_Isp2DLscTableCfg->pNew2DLscTable,
                                   (iss_config_processing_t *)
                                   pMSP_Isp2DLscTableCfg->ptIssConfig,
                                   &ptIspDev->iss_drv_config);

        }
            break;
        case MSP_ISP_CFG_INT:
        {
            MSP_IspInterruptCfgT *pIspInterruptCfg;

            pIspInterruptCfg = (MSP_IspInterruptCfgT *) pConfigParam;

            if (MSP_TRUE == pIspInterruptCfg->bEnable)
            {
                GOTO_EXIT_IF((ISP_SUCCESS !=
                              issDrvEnableIspIrq(pIspInterruptCfg->
                                                 eInterruptId)), 4);
            }
            else
            {
                GOTO_EXIT_IF((ISP_SUCCESS !=
                              issDrvDisableIspIrq(pIspInterruptCfg->
                                                  eInterruptId)), 4);
            }
        }
            break;

        case MSP_ISS_CFG_IRQ:
        {
            MSP_IspInterruptCfgT *pIssInterruptCfg;

            pIssInterruptCfg = (MSP_IspInterruptCfgT *) pConfigParam;

            if (MSP_TRUE == pIssInterruptCfg->bEnable)
            {
                GOTO_EXIT_IF((ISP_SUCCESS !=
                              issDrvEnableIssIrq(pIssInterruptCfg->
                                                 eInterruptId)), 4);
            }
            else
            {
                GOTO_EXIT_IF((ISP_SUCCESS !=
                              issDrvDisableIssIrq(pIssInterruptCfg->
                                                  eInterruptId)), 4);
            }
        }
            break;

        case MSP_ISP_CFG_DMA_REQ:
        {
            MSP_IspDmaReqCfgT *pIspDmaReqCfg;

            pIspDmaReqCfg = (MSP_IspDmaReqCfgT *) pConfigParam;
            if (MSP_TRUE == pIspDmaReqCfg->bEnable)
            {
                GOTO_EXIT_IF((ISP_SUCCESS !=
                              issDrvEnableDma((IPIPE_DMA_REQ_T) pIspDmaReqCfg->
                                              eDmaReqId)), 4);
            }
            else
            {
                GOTO_EXIT_IF((ISP_SUCCESS !=
                              issDrvDisableDma((IPIPE_DMA_REQ_T) pIspDmaReqCfg->
                                               eDmaReqId)), 4);
            }

        }
            break;

        case MSP_ISP_CFG_REG_CALLBACK:
        {
            MSP_IspCallbackRegCfgT *pCallbackRegCfg;

            pCallbackRegCfg = (MSP_IspCallbackRegCfgT *) pConfigParam;
            // TODO - Check for duplicated registration - may cause memory
            // leak.
            pQueue =
                (MSP_IspIntHandleQueueT *)
                malloc(sizeof(MSP_IspIntHandleQueueT));
            GOTO_EXIT_IF((NULL == pQueue), 3);
            pQueue->pNext = ptIspDev->pIspIntQueueFirst;
            ptIspDev->pIspIntQueueFirst = pQueue;
            pQueue->pHandle =
                (Interrupt_Handle_T *) malloc(sizeof(Interrupt_Handle_T));
            GOTO_EXIT_IF((NULL == pQueue->pHandle), 3);
            pQueue->pHandle->arg1 = (MSP_U32) pCallbackRegCfg->eInterruptId;
            pQueue->pHandle->arg2 = hMSP;
            pQueue->pHandle->callback = MSP_IspIsrCallback;
            pQueue->pHandle->context =
                (ISS_CALLBACK_CONTEXT_T) pCallbackRegCfg->nContext;
            pQueue->pHandle->priority = pCallbackRegCfg->nPriority;
            if (ISP_SUCCESS !=
                issDrvRegisterIspCallback(pCallbackRegCfg->eInterruptId,
                                          pQueue->pHandle))
            {
                free(pQueue->pHandle);
                free(pQueue);
                GOTO_EXIT(4);
            }
        }
            break;

        case MSP_ISP_CFG_UNREG_CALLBACK:
        {
            MSP_IspCallbackUnregCfgT *pCallbackUnregCfg;

            MSP_IspIntHandleQueueT *pTempQueue = NULL;

            pCallbackUnregCfg = (MSP_IspCallbackUnregCfgT *) pConfigParam;

            // Search for handle
            pQueue = ptIspDev->pIspIntQueueFirst;
            while ((pQueue != NULL) &&
                   (pQueue->pHandle->arg1 != pCallbackUnregCfg->eInterruptId))
            {
                pTempQueue = pQueue;
                pQueue = (MSP_IspIntHandleQueueT *) pQueue->pNext;
            }

            GOTO_EXIT_IF(((NULL == pTempQueue) && (NULL == pQueue)), 3);

            if (pQueue != NULL)
            {
                if (NULL == pTempQueue)
                {
                    // Element for unregistering is at first place
                    ptIspDev->pIspIntQueueFirst =
                        (MSP_IspIntHandleQueueT *) pQueue->pNext;
                }
                else
                {
                    // Element for unregistering is after first element
                    pTempQueue->pNext = pQueue->pNext;
                }
            }
            else
            {
                // Element for unregistering is alone in the queue
                pQueue = pTempQueue;
                ptIspDev->pIspIntQueueFirst = NULL;
            }
            GOTO_EXIT_IF((ISP_SUCCESS !=
                          issDrvUnregisterIspCallback((MSP_IssInterruptIdT)
                                                      pQueue->pHandle->arg1,
                                                      pQueue->pHandle)), 4);
            free(pQueue->pHandle);
            free(pQueue);
        }
            break;

        case MSP_ISS_CFG_REG_CALLBACK:
        {
            iss_callback_info_t iss_callback;

            MSP_IssCallbackRegCfgT *pCallbackRegCfg;

            pCallbackRegCfg = (MSP_IssCallbackRegCfgT *) pConfigParam;

            iss_callback.cbk_func = MSP_IssIsrCallback;
            iss_callback.arg1 = (MSP_U32) pCallbackRegCfg->eInterruptId;
            iss_callback.arg2 = hMSP;
            issDrvRegisterIssCallback(pCallbackRegCfg->eInterruptId,
                                      &iss_callback);
        }
            break;

        case MSP_ISS_CFG_UNREG_CALLBACK:
        {
            MSP_IspCallbackUnregCfgT *pCallbackRegCfg;

            pCallbackRegCfg = (MSP_IspCallbackUnregCfgT *) pConfigParam;
            issDrvUnregisterIssCallback(pCallbackRegCfg->eInterruptId);
        }
            break;

        case MSP_ISP_CFG_YUV_RANGE:
        {
            IPIPE_YUV_RANGE eRange;

            MSP_IspYuvRangeCfgT *peYuvRangeCfg;

            peYuvRangeCfg = ((MSP_IspYuvRangeCfgT *) pConfigParam);

            switch (peYuvRangeCfg->eIspYuvRange)
            {
                case MSP_ISS_YUV_RANGE_FULL:
                    eRange = IPIPE_YUV_RANGE_FULL;
                    break;
                case MSP_ISS_YUV_RANGE_RESTRICTED:
                    eRange = IPIPE_YUV_RANGE_RESTRICTED;
                    break;
                default:
                    GOTO_EXIT(3);
            }
            GOTO_EXIT_IF((ISP_SUCCESS != issDrvSetYuvRange(eRange)), 4);
        }
            break;

        case MSP_ISP_CFG_H3A:
        {
            MSP_IspH3aCfgT *ptIspH3aCfg;

            ptIspH3aCfg = (MSP_IspH3aCfgT *) pConfigParam;

            GOTO_EXIT_IF((ISP_SUCCESS !=
                          issDrvH3aConfig((iss_config_processing_t *)
                                          ptIspH3aCfg->ptIssConfig,
                                          ptIspH3aCfg->pAewbBuff,
                                          ptIspH3aCfg->pAfBuff,
                                          &ptIspDev->iss_drv_config)), 4);
        }
            break;

        case MSP_ISP_CFG_BOXCAR:
        {
            MSP_IpipeBoxcarCfgT *pIpipeBoxcarCfg;

            pIpipeBoxcarCfg = (MSP_IpipeBoxcarCfgT *) pConfigParam;

            GOTO_EXIT_IF((ISP_SUCCESS !=
                          ipipe_config_boxcar((ipipe_boxcar_cfg_t *)
                                              pIpipeBoxcarCfg)), 4);
        }
            break;

        case MSP_ISP_CFG_HIST:
        {
            MSP_IpipeHistCfgT *pIpipeHistCfg;

            pIpipeHistCfg = (MSP_IpipeHistCfgT *) pConfigParam;

            GOTO_EXIT_IF((ISP_SUCCESS !=
                          ipipe_config_histogram((ipipe_hist_cfg_t *)
                                                 pIpipeHistCfg)), 4);
        }
            break;

        case MSP_ISP_CFG_BSC:
        {
            MSP_IpipeBscCfgT *pIpipeBscCfg;

            pIpipeBscCfg = (MSP_IpipeBscCfgT *) pConfigParam;

            GOTO_EXIT_IF((ISP_SUCCESS !=
                          ipipe_config_bsc((ipipe_bsc_cfg_t *) pIpipeBscCfg)),
                         4);
        }
            break;

        case MSP_ISP_CFG_IPIPE:
        {
            MSP_IspProcCfgT *ptIspIpipeCfg;

            ptIspIpipeCfg = (MSP_IspProcCfgT *) pConfigParam;

            GOTO_EXIT_IF((ISP_SUCCESS !=
                          issDrvIpipeProcessingConfig((iss_config_processing_t
                                                       *) ptIspIpipeCfg->
                                                      ptIssConfig)), 4);
        }
            break;

        case MSP_ISP_CFG_PROCESSING:
        {
            MSP_IspProcCfgT *ptIspProcCfg;

            ptIspProcCfg = (MSP_IspProcCfgT *) pConfigParam;

            GOTO_EXIT_IF((ISP_SUCCESS !=
                          issDrvProcessingConfig((iss_config_processing_t *)
                                                 ptIspProcCfg->ptIssConfig)),
                         4);
        }
            break;

        case MSP_ISP_CFG_PROC_MEM_TO_MEM:
        {
            MSP_IspParamCfgT *ptIspParamCfg;

            ptIspParamCfg = (MSP_IspParamCfgT *) pConfigParam;

            /* Hack for DCC Integration */
            iss_drv_config = &ptIspDev->iss_drv_config;

            GOTO_EXIT_IF((ISP_SUCCESS !=
                          issDrvMemToMemConfig((iss_config_params_t *)
                                               ptIspParamCfg->ptIssParams,
                                               &ptIspDev->iss_drv_config)), 4);

            isp_regs->ISP5_CTRL |= (1 << 22);              // 22
#ifdef IMGS_PANASONIC_MN34041
            isp_regs->ISP5_CTRL &= ~(1 << 22); 
#endif
#ifdef IMGS_SONY_IMX136
            isp_regs->ISP5_CTRL &= ~(1 << 22); 
#endif
#ifdef IMGS_SONY_IMX140
            isp_regs->ISP5_CTRL &= ~(1 << 22); 
#endif
#ifdef IMGS_MICRON_AR0331
            //isp_regs->ISP5_CTRL &= ~(1 << 22); 
#endif	
#ifdef IMGS_OMNIVISION_OV2715
            isp_regs->ISP5_CTRL &= ~(1 << 22); 
#endif	
#ifdef IMGS_MICRON_MT9M034
            isp_regs->ISP5_CTRL &= ~(1 << 22); 
#endif	
#ifdef  IMGS_MICRON_MT9M034_DUAL_HEAD_BOARD
            isp_regs->ISP5_CTRL &= ~(1 << 22); 
#endif		
            rsz_reg->SRC_HPS = 0;

        }
            break;
        case MSP_ISP_CFG_PROC_SENSOR_TO_MEM:
        {
            MSP_IspParamCfgT *ptIspParamCfg;

            ptIspParamCfg = (MSP_IspParamCfgT *) pConfigParam;

            GOTO_EXIT_IF((ISP_SUCCESS !=
                          issDrvRawStatisticsConfig((iss_config_params_t *)
                                                    ptIspParamCfg->ptIssParams,
                                                    &ptIspDev->iss_drv_config)),
                         4);
        }
            break;

        case MSP_ISP_CFG_PROC_CCP_TO_ISIF:
        {
            MSP_IspParamCfgT *ptIspParamCfg;

            ptIspParamCfg = (MSP_IspParamCfgT *) pConfigParam;

            GOTO_EXIT_IF((ISP_SUCCESS !=
                          issDrvCCPRawStatisticsConfig((iss_config_params_t *)
                                                       ptIspParamCfg->
                                                       ptIssParams,
                                                       &ptIspDev->
                                                       iss_drv_config)), 4);
        }
            break;

        case MSP_ISP_CFG_PROC_DARK_FRAME:
        {
            MSP_IspParamCfgT *ptIspParamCfg;

            ptIspParamCfg = (MSP_IspParamCfgT *) pConfigParam;

            GOTO_EXIT_IF((ISP_SUCCESS !=
                          issDrvDarkToRawConfig((iss_config_params_t *)
                                                ptIspParamCfg->ptIssParams,
                                                &ptIspDev->iss_drv_config)), 4);
        }
            break;

        default:
            GOTO_EXIT(3);
    }

    return (eResult);
  EXIT_2:
    eResult = MSP_ERROR_NULLPTR;
    return (eResult);
  EXIT_3:
    eResult = MSP_ERROR_INVALIDCONFIG;
    return (eResult);
  EXIT_4:
    eResult = MSP_ERROR_FAIL;
    return (eResult);

}

/* ========================================================================== 
 */
/**
 * @fn  MSP_ISP_query()    MSP ISP device driver query function.
 * Reads different config settings of the ISP device
 *
 * @see  isp_msp.h
*/
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     MSP_ISP_query                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MSP_ERROR_TYPE MSP_ISP_query(MSP_HANDLE handle, MSP_INDEXTYPE tQueryIndex,
                             MSP_PTR pQueryParam)
{

    MSP_ERROR_TYPE eResult = MSP_ERROR_NONE;

    GOTO_EXIT_IF(((handle == NULL) || (pQueryParam == NULL)), 3);

    switch ((MSP_QueryIndexT) tQueryIndex)
    {

        case MSP_ISP_QUERY_BSC_BUFF_SIZES:
        {
            MSP_IspQueryBscSizesT *ptBscSizes;

            ptBscSizes = (MSP_IspQueryBscSizesT *) pQueryParam;
            ptBscSizes->pRowsBufferSize = ptBscSizes->ptBscCfg->nRowPos.nVNum *
                ptBscSizes->ptBscCfg->nRowPos.nVectors * 2;
            ptBscSizes->pColsBufferSize = ptBscSizes->ptBscCfg->nColPos.nHNum *
                ptBscSizes->ptBscCfg->nColPos.nVectors * 2;
        }
            break;
        case MSP_ISP_QUERY_GET_BSC:
        {
            MSP_U32 nBscRowSize;

            MSP_U32 nBscColSize;

            MSP_IspQueryBscDataT *pIspQueryBscData;

            pIspQueryBscData = (MSP_IspQueryBscDataT *) pQueryParam;
            nBscRowSize = pIspQueryBscData->ptBscCfg->nRowPos.nVNum *
                pIspQueryBscData->ptBscCfg->nRowPos.nVectors * 2;
            nBscColSize = pIspQueryBscData->ptBscCfg->nColPos.nHNum *
                pIspQueryBscData->ptBscCfg->nColPos.nVectors * 2;
            GOTO_EXIT_IF((MSP_ERROR_NONE !=
                          bscDataGet(pIspQueryBscData->pRowsBuffer,
                                     pIspQueryBscData->pColsBuffer, nBscRowSize,
                                     nBscColSize)), 1);
        }
            break;
        case MSP_ISP_QUERY_BSC_ADDR:
        {
            ISP_RETURN eIspResult;

            MSP_IspQueryBscAddrT *pIspQueryBscAddr;

            pIspQueryBscAddr = (MSP_IspQueryBscAddrT *) pQueryParam;

            eIspResult =
                ipipe_get_bsc_address((MSP_U32 *) &
                                      (pIspQueryBscAddr->pRowsData),
                                      IPIPE_BSC_MEMORY_0);
            GOTO_EXIT_IF((ISP_SUCCESS != eIspResult), 1);
            eIspResult =
                ipipe_get_bsc_address((MSP_U32 *) &
                                      (pIspQueryBscAddr->pColsData),
                                      IPIPE_BSC_MEMORY_1);
            GOTO_EXIT_IF((ISP_SUCCESS != eIspResult), 1);
        }
            break;
        case MSP_ISP_QUERY_GET_HIST:
        {
            MSP_IspQueryHistDataT *pIspHistData;

            pIspHistData = (MSP_IspQueryHistDataT *) pQueryParam;
            pIspHistData->nOutHistSize =
                ipipe_get_histogram((MSP_U32 *) pIspHistData->pHistBuffer,
                                    pIspHistData->nHistRegion,
                                    (IPIPE_HIST_COLOR_T) pIspHistData->
                                    eHistColor);
        }
            break;
        case MSP_ISP_QUERY_H3A_STATE:
        {
            MSP_IspQueryH3aStateT *pIspH3aState;

            pIspH3aState = (MSP_IspQueryH3aStateT *) pQueryParam;
            pIspH3aState->eH3aState = (MSP_IspH3aStatesT) h3aGetState();
        }
            break;
        case MSP_ISP_QUERY_CROP_SIZES:
            if (ISP_SUCCESS !=
                issDrvCalcCropDims((config_sizes_t *) pQueryParam))
            {
                eResult = MSP_ERROR_INVALIDCONFIG;
            }
            break;
        case MSP_ISP_QUERY_MAX_WIDTH:
            if (ISP_SUCCESS !=
                issDrvCheckMaxWidth((config_sizes_t *) pQueryParam))
            {
                eResult = MSP_ERROR_INVALIDCONFIG;
            }
            break;
        case MSP_ISP_QUERY_CLOCK_DIV:
            if (ISP_SUCCESS !=
                issDrvGetDivider((iss_config_params_t *) pQueryParam))
            {
                eResult = MSP_ERROR_INVALIDCONFIG;
            }
            break;
        default:
            GOTO_EXIT(2);
    }

    return (eResult);
  EXIT_1:
    eResult = MSP_ERROR_FAIL;
    return (eResult);
  EXIT_2:
    eResult = MSP_ERROR_INVALIDINDEX;
    return (eResult);
  EXIT_3:
    eResult = MSP_ERROR_NULLPTR;
    return (eResult);
}

/* ========================================================================== 
 */
/**
 * @fn  MSP_ISP_close()    MSP ISP device driver close function.
 * Closes ISP device - power off and reset
 *
 * @see  isp_msp.h
*/
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     MSP_ISP_close                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MSP_ERROR_TYPE MSP_ISP_close(MSP_HANDLE handle)
{
    MSP_ERROR_TYPE eResult = MSP_ERROR_NONE;

    return (eResult);
}

/* ========================================================================== 
 */
/**
 * @fn  MSP_ISP_deInit()    MSP ISP device driver deinit function.
 * Frees ISP device driver resources
 *
 * @see  isp_msp.h
*/
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     MSP_ISP_deInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MSP_ERROR_TYPE MSP_ISP_deInit(MSP_HANDLE handle)
{
    MSP_ERROR_TYPE eResult = MSP_ERROR_NONE;

    MSP_COMPONENT_TYPE *hMSP;

    ispDriverInstanceT *tIspDev;

    GOTO_EXIT_IF((handle == NULL), 2);

    hMSP = (MSP_COMPONENT_TYPE *) handle;
    tIspDev = (ispDriverInstanceT *) hMSP->pCompPrivate;

    free(tIspDev);
	
	hMSP->pCompPrivate = NULL;
	
    //Invocations--;
    if (!Invocations)
    {
        // TODO: RECOVER ME GOTO_EXIT_IF(ISP_SUCCESS != issDrvDeinit(), 3);

        /* iss_module_clk_ctrl(ISS_SIMCOP_CLK, ISS_CLK_DISABLE);
         * iss_module_clk_ctrl(ISS_ISP_CLK, ISS_CLK_DISABLE);
         * iss_module_clk_ctrl(ISS_CCP2_CLK, ISS_CLK_DISABLE);
         * iss_module_clk_ctrl(ISS_CSI2B_CLK, ISS_CLK_DISABLE);
         * iss_module_clk_ctrl(ISS_CSI2A_CLK, ISS_CLK_DISABLE); */
    }

    return (eResult);
  EXIT_2:
    eResult = MSP_ERROR_NULLPTR;
    return (eResult);
    /* EXIT_3: eResult = MSP_ERROR_HARDWARE; return (eResult); */
}

/* ========================================================================== 
 */
/**
 * @fn  MSP_ISP_init()    MSP ISP device driver initlization function. Allocates
 * memory for the MSP ISP device driver handle and initializes the MSP function
 * pointers
 *
 * @see  isp_msp.h
*/
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     MSP_ISP_init                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MSP_ERROR_TYPE MSP_ISP_init(MSP_COMPONENT_TYPE * hMSP,
                            MSP_PROFILE_TYPE tProfile)
{
    MSP_ERROR_TYPE eResult = MSP_ERROR_NONE;

    ispDriverInstanceT *tIspDev;

    GOTO_EXIT_IF((hMSP == NULL), 2);

    /* Allocate memory for the ISP Device Struct */
	if (!hMSP->pCompPrivate)
	{
    	tIspDev = (ispDriverInstanceT *) malloc(sizeof(ispDriverInstanceT));

    	hMSP->pCompPrivate = tIspDev;

    	/* Allocation successful? */
    	GOTO_EXIT_IF((hMSP->pCompPrivate == NULL), 3);
	}

	tIspDev = hMSP->pCompPrivate;
    tIspDev->bThisIsStarted = MSP_FALSE;
    tIspDev->bWaitForCcpRx = MSP_FALSE;
    tIspDev->pIspIntQueueFirst = NULL;

    /* Initialize the MSP functions pointers */
    hMSP->open = MSP_ISP_open;
    hMSP->control = MSP_ISP_control;
    hMSP->process = MSP_ISP_process;
    hMSP->config = MSP_ISP_config;
    hMSP->query = MSP_ISP_query;
    hMSP->close = MSP_ISP_close;
    hMSP->deInit = MSP_ISP_deInit;

    if (!Invocations)
    {                                                      /* 
                                                            * // Enable ISP
                                                            * clock and
                                                            * initialize ISP
                                                            * GOTO_EXIT_IF(ISS_SUCCESS 
                                                            * != iss_init(),
                                                            * 4);
                                                            * GOTO_EXIT_IF(ISP_SUCCESS 
                                                            * !=
                                                            * isp_common_init(), 
                                                            * 4);
                                                            * 
                                                            * iss_module_clk_ctrl(ISS_SIMCOP_CLK, 
                                                            * ISS_CLK_ENABLE);
                                                            * iss_module_clk_ctrl(ISS_ISP_CLK, 
                                                            * ISS_CLK_ENABLE);
                                                            * iss_module_clk_ctrl(ISS_CCP2_CLK, 
                                                            * ISS_CLK_ENABLE);
                                                            * iss_module_clk_ctrl(ISS_CSI2B_CLK, 
                                                            * ISS_CLK_ENABLE);
                                                            * iss_module_clk_ctrl(ISS_CSI2A_CLK, 
                                                            * ISS_CLK_ENABLE); */
        // TODO: RECOVER ME GOTO_EXIT_IF(ISP_SUCCESS != issDrvInit(), 4);

        // Perform global variables initialization.
        Started = MSP_FALSE;
        WaitQueueIdxCurr = 0;
        WaitQueueIdxEnd = 0;
        WaitQueueIdxElems = 0;
    }
    //Invocations++;

    return (eResult);
  EXIT_2:
    eResult = MSP_ERROR_NULLPTR;
    return (eResult);
  EXIT_3:
    eResult = MSP_ERROR_NOTENOUGHRESOURCES;
    return (eResult);
    /* EXIT_4: MSP_MEMFREE(tIspDev); eResult = MSP_ERROR_HARDWARE; return
     * (eResult); */

}

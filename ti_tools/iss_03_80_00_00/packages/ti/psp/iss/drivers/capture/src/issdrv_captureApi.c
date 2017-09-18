
/** ==================================================================
 *  @file   issdrv_captureApi.c
 *
 *  @path   /ti/psp/iss/drivers/capture/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <xdc/std.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif.h>
#include <ti/psp/iss/hal/iss/isp/rsz/inc/rsz.h>
#include <ti/psp/iss/hal/iss/isp/ipipe/inc/ipipe.h>
#include <ti/psp/iss/core/inc/iss_drv_common.h>
#include <ti/psp/iss/core/inc/iss_drv.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/iss/drivers/alg/vstab/alg_vstab.h>
#include <ti/psp/iss/core/isp_msp.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_alg2APriv.h>
#include <ti/psp/iss/drivers/iss_init.h>
#include <ti/psp/iss/core/inc/csi2rx_api.h>
#include <ti/psp/iss/core/inc/csi_MSP.h>
#include <ti/psp/iss/common/iss_evtMgr.h>
#include <ti/psp/iss/drivers/capture/src/issdrv_capturePriv.h>
#include <ti/psp/iss/drivers/capture/src/issdrv_ispPriv.h>
#include <ti/psp/iss/drivers/alg/vstab/src/issdrv_algVstabPriv.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define VS_floor(value, align)   (( (value) / (align) ) * (align) )

#ifndef ISS_EXAMPLES
extern Ptr Utils_memAlloc(UInt32 size, UInt32 align);
extern Int32 Utils_memFree(Ptr addr, UInt32 size);
#endif

#define BOXCAR_ELEMENT_SIZE         (8)
#define BOXCAR_MAX_LINE_8           (4096)

#define VIDEO_ISR_EVENT_RSZ         (1 << 0)
#define VIDEO_ISR_EVENT_H3A         (1 << 1)
#define VIDEO_ISR_EVENT_BSC         (1 << 2)
#define VIDEO_ISR_EVENT_CCP         (1 << 3)

#define EXPECT_EVENT(x,y)           (x->nRemainIsrEvents |= (y))
#define CLEAR_EVENT(x,y)            (x->nRemainIsrEvents &= (~(y)))
#define NO_EVENTS_REMAIN(x)         (!(x->nRemainIsrEvents))
#define CLEAR_ALL_EVENTS(x)         (x->nRemainIsrEvents = 0)

#define MSP_MEMALLOC(SIZE)          malloc(SIZE, 32);
#define MSP_MEMFREE(PTR)

#define ISS_VDINT_NUM_LINES_LESS        (5u)

#define ISS_CAPT_RESET_DELAY            (10000u)

#ifndef ENABLE_TILER
#undef ENABLE_BTE
#endif


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* none */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Void Iss_ispWriteReg(
        volatile UInt32 *pRegAddr,
        UInt32 value,
        UInt32 bitPosn,
        UInt32 numBits);

UInt32 Utils_tilerGetOriAddr(
            UInt32 tilerAddr,
            UInt32 cntMode,
            UInt32 oriFlag,
            UInt32 width,
            UInt32 height);

UInt32 Utils_tilerAddr2CpuAddr(UInt32 tilerAddr);

/* Interrupt Handlers */
void isp_h3a_dma_end_handle();
Void Issdrv_IspIsifVdInt(const UInt32 event, Ptr arg);
void Issdrv_IspRszDmaEndInt(const UInt32 event, Ptr arg);
void video_bsc_handle();
void isp_test_csi2rx_line_end_handle(MSP_ERROR_TYPE status, uint32 arg1, void*arg2);
void isp_test_csi2rx_line_end_wait(void);


Int32 bufSwitchFull(UInt32 streamId);
int VIDEO_vsTskDelete();
Int32 Iss_Disable_RszB();
Int32 IssAlg_capt2AGetFocus(Ptr pCmdArgs);
Int32 Issdrv_captSetRszCfg(
            Iss_CaptObj *pObj,
            Iss_IspResizerParams *scPrms);
static MSP_ERROR_TYPE drvIspCallback(MSP_PTR hMSP, MSP_PTR pAppData,
                                     MSP_EVENT_TYPE tEvent,
                                     MSP_OPAQUE nEventData1,
                                     MSP_OPAQUE nEventData2);

Int32 Issdrv_setH3aConfig(Iss_IspH3aCfg *h3aCfg, UInt32 algInit);
Int32 Issdrv_getH3aConfig(Iss_IspH3aCfg *h3aCfg);
Void Issdrv_captSetDefaultH3APrms();
Int32 Issdrv_getIsifConfig(Iss_IspIsifCfg *isifCfg);
Int32 Issdrv_setIsifConfig(Iss_IspIsifCfg *isifCfg);
Void IssCdrv_setIpipeCfg();
Int32 Issdrv_setIpipeIfDfsParams(Iss_IspIpipeifDfs *);
Int32 Iss_captSetOutDataFmt(Iss_CaptObj *pObj,Iss_CaptOutDataFormat *pOutDataFmt);

/* ========================================================================== */
/*                          Global Variables                                  */
/* ========================================================================== */

const int anIirCoef1[11] = {8, -95, -49, 38, 76, 38, -111, -54, 17, -34, 17};
const int anIirCoef2[11] = {11, -72, -50, 26, 51, 26, -92, -53, 19, -38, 19};
volatile int raw_data_done;
extern isif_regs_ovly isif_reg;
extern rsz_A_regs_ovly rszA_reg;
extern rsz_B_regs_ovly rszB_reg;
extern ipipe_regs_ovly ipipe_reg;
extern ipipeif_regs_ovly ipipeif_reg;
extern rsz_regs_ovly rsz_reg;
//extern h3a_regs_ovly h3a_reg;
extern iss_regs_ovly iss_regs;
extern isp_regs_ovly isp_regs;
extern bte_regs_ovly bte_reg;
extern csi2_regs_ovly csi2A_regs;
Iss_CaptCommonObj gIss_captCommonObj = {.tskUpdate = NULL};
FVID2_Frame *PrimaryFrame_RszA;
FVID2_Frame *PrimaryFrame_RszB;
tStreamBuf curStreamBuf[2];
void *h3a_AEWBBuff;
void *h3a_AFBuff;
extern ti2a_output ti2a_output_params;
static Int32 gRawCaptureTriggerd;
static Int32 gYUVCaptureTriggerd;
static Int32 gTiler_xw_swap = 0;
static Int32 gMirrorMode;
Int32 isp_reset_trigger = 0;
Int32 trigger_wait_cnt = 0;
IssCsi2rxIsrHandleT *pIssCsiHandle;
//MSP_CamCfgTranscodeSize tCtxTranscode;



/* ========================================================================== */
/*                          Function Defination                               */
/* ========================================================================== */


Int32 isYUV422ILEFormat(Int32 format)
{
    if((format == FVID2_DF_YUV422I_UYVY) ||
       (format == FVID2_DF_YUV422I_YUYV) ||
       (format == FVID2_DF_YUV422I_YVYU) ||
       (format == FVID2_DF_YUV422I_VYUY))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

Int32 isYUV420SPFormat(Int32 format)
{
    if((format == FVID2_DF_YUV420SP_UV) ||
       (format == FVID2_DF_YUV420SP_VU))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

#ifdef ENABLE_BTE
#define BTE_BWL 56

#define DIV_UP(n, alignment) ((n + alignment - 1)/alignment)

static Int32 configureBteContext(bteHandle *handleBTE, int streamNumber, int width, int height, int format, int rotationMode)
{
	bte_config_t *contextHandle;
	int i;
	int pitch;

    GT_assert(GT_DEFAULT_MASK, handleBTE != NULL);

	GT_assert(GT_DEFAULT_MASK, streamNumber < handleBTE->contextNumber/2);

	i = streamNumber * 2;
	contextHandle = &handleBTE->bte_config[i];

	/* First context */
	if(isYUV422ILEFormat(format) == 1)	
	{
		pitch = width * 2;
	}
	else
	{
		pitch = width;
	}

	contextHandle->context_end_addr = ((DIV_UP(contextHandle->context_start_addr + pitch, 16) - 1) << CSL_BTE_BTE_CONTEXT_END_0_X_SHIFT) +
		((height - 1) << CSL_BTE_BTE_CONTEXT_END_0_Y_SHIFT);

	if (rotationMode < 4)
	{
		if(isYUV422ILEFormat(format) == 1)
			contextHandle->context_ctrl.grid = 0x2;
		else
			contextHandle->context_ctrl.grid = 0x0;
	}
	else
	{
		contextHandle->context_ctrl.grid = 0x1;
	}

	/* 4 lines */
	contextHandle->context_ctrl.trigger_threshold = pitch * 3 / 16 + 2;

	/* Second context for YUV420 */
	if(isYUV422ILEFormat(format) == 0)
	{
		contextHandle++;

		contextHandle->context_end_addr = ((DIV_UP(contextHandle->context_start_addr + pitch, 16) - 1) << CSL_BTE_BTE_CONTEXT_END_0_X_SHIFT) +
			((height/2 - 1) << CSL_BTE_BTE_CONTEXT_END_0_Y_SHIFT);

		if (rotationMode < 4)
		{
			contextHandle->context_ctrl.grid = 0x2;
		}
		else
		{
			contextHandle->context_ctrl.grid = 0x1;
		}

		contextHandle->context_ctrl.trigger_threshold = pitch * 3 / 16 + 2;
	}

	return 0;
}

static Int32 startBteContext(bte_config_t *contextHandle, uint32 tilerAddress)
{
    GT_assert(GT_DEFAULT_MASK, contextHandle != NULL);

	contextHandle->frame_buffer_addr = tilerAddress;

	contextHandle->context_ctrl.start = 1;

	GT_assert(GT_DEFAULT_MASK, bte_config_context_ctrl(contextHandle) == BTE_SUCCESS);

	return 0;
}

static Int32 stopBteContext(bte_config_t *contextHandle)
{
    GT_assert(GT_DEFAULT_MASK, contextHandle != NULL);

	contextHandle->context_ctrl.start = 0;
	contextHandle->context_ctrl.flush = 0;
	contextHandle->context_ctrl.stop = 1;

	GT_assert(GT_DEFAULT_MASK, bte_config_ctrl(&contextHandle->context_ctrl, contextHandle->context_num) == BTE_SUCCESS);

	contextHandle->context_ctrl.start = 0;
	contextHandle->context_ctrl.flush = 1;
	contextHandle->context_ctrl.stop = 0;

	GT_assert(GT_DEFAULT_MASK, bte_config_ctrl(&contextHandle->context_ctrl, contextHandle->context_num) == BTE_SUCCESS);

	while ((bte_reg->BTE_HL_IRQSTATUS_RAW & (CSL_BTE_BTE_HL_IRQSTATUS_RAW_IRQ_CTX0_DONE_MASK << (contextHandle->context_num))) == 0);

	return 0;
}
#endif

/*========================================================================= */

/*========================================================================= */

void isp_test_csi2rx_line_end_handle(MSP_ERROR_TYPE status, uint32 arg1, void*arg2)
{
    //Implementation for M2M mode
}


void isp_test_csi2rx_line_end_wait(void)
{
    //Implementation for M2M mode
}


Void Issdrv_IspH3aEndInt(const UInt32 event, Ptr arg)
{
    gIss_captCommonObj.intCounter[ISS_CAPT_INT_H3A] ++;

    IssAlg_capt2AIsrCallBack((Iss_2AObj*)gIss_captCommonObj.pModuleInstance->p2AObj);

    CLEAR_EVENT(gIss_captCommonObj.pModuleInstance, VIDEO_ISR_EVENT_H3A);
}

Void Issdrv_IspBscInt(const UInt32 event, Ptr arg)
{
    gIss_captCommonObj.intCounter[ISS_CAPT_INT_BSC] ++;

    video_bsc_copyBuffers();

    CLEAR_EVENT(gIss_captCommonObj.pModuleInstance, VIDEO_ISR_EVENT_BSC);
}
Void Issdrv_IspRszOvfl(const UInt32 event, Ptr arg)
{
    gIss_captCommonObj.intCounter[ISS_CAPT_INT_RSZ_OVFL] ++;

    isp_reset_trigger = 1;
}

Void IssCDrv_rszResetTask(UArg arg1, UArg arg2)
{
    while(1)
    {
        /* Wait for BSC semaphore */
        Semaphore_pend(gIss_captCommonObj.ovflWait, BIOS_WAIT_FOREVER);

        if(TRUE == gIss_captCommonObj.exitTask)
        {
            break;
        }

        Iss_captResetAndRestart(NULL);
    }
}


/* ===================================================================
 *  @func     bufSwitchFull
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
Int32 bufSwitchFull(UInt32 streamId)
{
    Int32 status = FVID2_SOK;
    FVID2_Frame *pframe;
    Iss_CaptObj *pObj;
    Iss_CaptChObj *pChObj;
    Iss_CaptRtParams *pRtParams = NULL;

    pObj = &gIss_captCommonObj.captureObj[0];
    pChObj = &pObj->chObj[streamId][0];
    pChObj->pTmpFrame = NULL;
	int i;
	char *bufAddr;
	int startX, startY, pitchY, pitchC, width, height;

    status = VpsUtils_queGet(&pChObj->emptyQue, (Ptr *) & pframe, 1, BIOS_NO_WAIT);

    /* Set first RT Params */
    if (FVID2_SOK == status)
    {
        if ((streamId == 0) &&
            (pObj->updateResPrms) &&
            (pObj->createArgs.videoCaptureMode == ISS_CAPT_INMODE_ISIF))
        {
            /* Resizer should be configured in ISIF mode only */
            Issdrv_captSetRszCfg(pObj, &pObj->createArgs.rszPrms);
        }
        pObj->updateResPrms = 0;
    }

    if (status == FVID2_SOK)
    {
        if (curStreamBuf[streamId].ptmpFrame != NULL)
        {
            FVID2_Frame *pframeSave;

            if(streamId == 0)
            {
                if (gYUVCaptureTriggerd == 1 && trigger_wait_cnt == 0 )
                {

                    pframeSave = curStreamBuf[streamId].ptmpFrame;
                    trigger_wait_cnt++;
                }

                if(trigger_wait_cnt == 1) {
					startX = pObj->createArgs.rszPrms.rPrms[streamId].posPrms.startX;
					startY = pObj->createArgs.rszPrms.rPrms[streamId].posPrms.startY;
					pitchY = pObj->createArgs.outStreamInfo[streamId].pitch[0];
					pitchC = pObj->createArgs.outStreamInfo[streamId].pitch[1];
					width = pObj->createArgs.rszPrms.rPrms[streamId].outWidth;
					height = pObj->createArgs.rszPrms.rPrms[streamId].outHeight;
					
                    if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[streamId].dataFormat) == 1)
                    {
						bufAddr = (char *)pframeSave->addr[1][0] + pitchY * startY + startX * 2;		
						for (i = 0; i < height; i++)
							memcpy((char *)gIss_captCommonObj.YUVcaptureAddr + width * 2 * i,
								bufAddr + pitchY * i, width * 2);
                    }
					else
					{
						bufAddr = (char *)pframeSave->addr[1][0] + pitchY * startY + startX;		
						for (i = 0; i < height; i++)
							memcpy((char *)gIss_captCommonObj.YUVcaptureAddr + width * i,
								bufAddr + pitchY * i, width);
						bufAddr = (char *)pframeSave->addr[1][1] + pitchC * startY/2 + startX;	
						for (i = 0; i < height/2; i++)
							memcpy((char *)gIss_captCommonObj.YUVcaptureAddr + width * height + width * i,
								bufAddr + pitchC * i, width);
					}
					
					gIss_captCommonObj.gIttParams->Raw_Data_Ready = 1;
                    gYUVCaptureTriggerd = 0;
                    trigger_wait_cnt = 0;
                }
            }

            if(pObj->createArgs.captureMode == ISS_CAPT_INMODE_ISIF)
            {
                if(NULL != curStreamBuf[streamId].ptmpFrame->perFrameCfg)
                {
                    pRtParams = (Iss_CaptRtParams *)curStreamBuf[streamId].ptmpFrame->perFrameCfg;
                    pRtParams->captureOutWidth = (pObj->createArgs.rszPrms.rPrms[streamId].outWidth & 0xFFFFFFF0);
                    pRtParams->captureOutHeight = pObj->createArgs.rszPrms.rPrms[streamId].outHeight;
                    pRtParams->captureOutPitch = pObj->createArgs.pitch[streamId];
                    if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[streamId].dataFormat) == 1)
                        pRtParams->captureOutPitch = pObj->createArgs.pitch[streamId] * 2;

                    if(gTiler_xw_swap == 1)
                    {
                        pRtParams->captureOutWidth = (pObj->createArgs.rszPrms.rPrms[streamId].outHeight & 0xFFFFFFF0);
                        pRtParams->captureOutHeight = pObj->createArgs.rszPrms.rPrms[streamId].outWidth;
                    }
                }
                status = VpsUtils_quePut(&pObj->chObj[streamId][0].tmpQue,
                                         curStreamBuf[streamId].ptmpFrame,
                                         BIOS_WAIT_FOREVER);
            }
            else
            {
                // ISS_CAPT_INMODE_DDR mode
                curStreamBuf[streamId].ptmpFrame->addr[0][0] = curStreamBuf[streamId].ptmpFrame->addr[1][0];
                status = VpsUtils_quePut(&pObj->fullQue[streamId],
                                         curStreamBuf[streamId].ptmpFrame,
                                         BIOS_WAIT_FOREVER);
            }

            if (status == FVID2_SOK)
            {
                curStreamBuf[streamId].ptmpFrame = pframe;
            }
            else
            {
                // Output queue is full.
                // Send the filled buffer back to empty pool.
                status = VpsUtils_quePut(&pObj->chObj[streamId][0].emptyQue,
                                         curStreamBuf[streamId].ptmpFrame,
                                         BIOS_WAIT_FOREVER);

                if (status == FVID2_SOK)
                {
                    curStreamBuf[streamId].ptmpFrame = pframe;
                }
                else
                {
                    //Vps_printf("\n Input/Output queues FULL!!!! \n");
                }
            }
        }
        else
        {
            curStreamBuf[streamId].ptmpFrame = pframe;
        }
    }
    else
    {
#ifdef ISS_DEBUG_RT
        Vps_printf("Stream ID %d: Input queue empty condition.", streamId);
#endif
    }

    return status;
}



/* ===================================================================
 *  @func     isp_h3a_dma_end_handle
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
void isp_h3a_dma_end_handle()
{

    // Buffer switch logic implementation - TODO

}

/* ===================================================================
 *  @func     Issdrv_IspRszDmaEndInt
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
void Issdrv_IspRszDmaEndInt(const UInt32 event, Ptr arg)
{
    Iss_CaptObj *pObj = &gIss_captCommonObj.captureObj[0];
#ifdef ENABLE_BTE
    UInt32 oriBufOffsetY, oriBufOffsetC;
#endif

    gIss_captCommonObj.intCounter[ISS_CAPT_INT_RSZ] ++;

    if (pObj->state  == ISS_CAPT_STATE_STOPPED)
        return ;

    if (gMirrorMode == 1)
    {
		if (curStreamBuf[0].memType == ISS_NONTILEDMEM)
		{
        	rsz_cfg_flip(RESIZER_A, (RSZ_FLIP_CFG_T) curStreamBuf[0].mirrorMode);
    	}

    	if (curStreamBuf[1].memType == ISS_NONTILEDMEM)
    	{
        	rsz_cfg_flip(RESIZER_B, (RSZ_FLIP_CFG_T) curStreamBuf[1].mirrorMode);
    	}

        gMirrorMode = 0;
    }

#ifdef ENABLE_BTE
/*
    checkBteStatus(0);
    if (pObj->createArgs.outStreamInfo[0].dataFormat != FVID2_DF_YUV422I_UYVY)
        checkBteStatus(1);
*/
    stopBteContext(&gIss_captCommonObj.handleBTE.bte_config[0]);
    if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[0].dataFormat) == 0)
        stopBteContext(&gIss_captCommonObj.handleBTE.bte_config[1]);

    configureBteContext(&gIss_captCommonObj.handleBTE, 0,
        pObj->createArgs.rszPrms.rPrms[0].outWidth, pObj->createArgs.rszPrms.rPrms[0].outHeight,
        pObj->createArgs.outStreamInfo[0].dataFormat, curStreamBuf[0].mirrorMode);

    oriBufOffsetY = Utils_tilerGetOriAddr((uint32)curStreamBuf[0].ptmpFrame->addr[1][0], 0, curStreamBuf[0].mirrorMode,
        pObj->createArgs.rszPrms.rPrms[0].outWidth, pObj->createArgs.rszPrms.rPrms[0].outHeight);
    startBteContext(&gIss_captCommonObj.handleBTE.bte_config[0], oriBufOffsetY);

    if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[0].dataFormat) == 0)
    {
        oriBufOffsetC = Utils_tilerGetOriAddr((uint32)curStreamBuf[0].ptmpFrame->addr[1][1], 1, curStreamBuf[0].mirrorMode,
            pObj->createArgs.rszPrms.rPrms[0].outWidth, pObj->createArgs.rszPrms.rPrms[0].outHeight/2);
        startBteContext(&gIss_captCommonObj.handleBTE.bte_config[1], oriBufOffsetC);
    }
#endif
    // PP DRV callback
    Iss_ispDrvCallBack();

    CLEAR_EVENT(gIss_captCommonObj.pModuleInstance, VIDEO_ISR_EVENT_RSZ);
}



/* ===================================================================
 *  @func     calc_flip_offsets
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

void calc_flip_offsets(Int16 dataFormat, Int16 streamId, Int32 pitch, Int32 height, Int32 width)

{
	if (curStreamBuf[streamId].memType == ISS_NONTILEDMEM)
	{
	    if (dataFormat == 0)/*YUV422*/
	    {
	        curStreamBuf[streamId].flipVOffsetY = (pitch * 2 * (height - 1)) / 4;	
	        curStreamBuf[streamId].flipVOffsetC = (pitch * (height / 2 - 1)) / 4;	
	        curStreamBuf[streamId].flipHOffsetYC = (width * 2 - 1) / 4;
	    }
	    else if (dataFormat == 1) /*420SP*/
	    {
	        curStreamBuf[streamId].flipVOffsetY = (pitch * (height - 1)) / 4;
	        curStreamBuf[streamId].flipVOffsetC = (pitch * (height / 2 - 1)) / 4;
	        curStreamBuf[streamId].flipHOffsetYC = (width - 1) / 4;
	    }
	    else
	    {
	        curStreamBuf[streamId].flipVOffsetY = 0;
	        curStreamBuf[streamId].flipVOffsetC = 0;
	        curStreamBuf[streamId].flipHOffsetYC = 0;
	    }

	    if (curStreamBuf[streamId].flipV == TRUE)
	    {
	        gIss_captCommonObj.buffOffsetY += curStreamBuf[streamId].flipVOffsetY;
	        gIss_captCommonObj.buffOffsetYC += curStreamBuf[streamId].flipVOffsetC;
	    }

	    if (curStreamBuf[streamId].flipH == TRUE)
	    {
	        gIss_captCommonObj.buffOffsetY += curStreamBuf[streamId].flipHOffsetYC;
	        gIss_captCommonObj.buffOffsetYC += curStreamBuf[streamId].flipHOffsetYC;
	    }
	}
	else
	{
    	Int32 oriBufOffsetY, oriBufOffsetC;
    	Int32 widthYC, heightYC;

    	if (curStreamBuf[streamId].mirrorMode <= 3)
    	{
    	    gTiler_xw_swap = 0;
    	    gIss_captCommonObj.pitch[0] =  16u*1024u;
    	    gIss_captCommonObj.pitch[1] =  32u*1024u;
    	}
    	else
    	{
    	    gTiler_xw_swap = 1;
    	    gIss_captCommonObj.pitch[0] =  8u*1024u;
    	    gIss_captCommonObj.pitch[1] =  8u*1024u;
    	}

    	widthYC = width;
    	heightYC = height/2;

    	switch (curStreamBuf[streamId].mirrorMode)
    	{
    	    case 1:
    	        *(volatile unsigned *)0x4E000224 = 0x9000;
    	        break;
    	    case 2:
    	        *(volatile unsigned *)0x4E000224 = 0xA000;
    	        break;
    	    case 3:
    	        *(volatile unsigned *)0x4E000224 = 0xB000;
    	        break;
    	    case 4:
    	        *(volatile unsigned *)0x4E000224 = 0xC000;
    	        break;
    	    case 5:
    	        *(volatile unsigned *)0x4E000224 = 0xD000;
    	        break;
    	    case 6:
    	        *(volatile unsigned *)0x4E000224 = 0xE000;
    	        break;
    	    default:
    	        *(volatile unsigned *)0x4E000224 = 0x8000;
    	        break;
    	}

    	oriBufOffsetY = Utils_tilerGetOriAddr((Int32)(gIss_captCommonObj.buffOffsetY), 0, curStreamBuf[streamId].mirrorMode, width, height);
    	oriBufOffsetC = Utils_tilerGetOriAddr((Int32)(gIss_captCommonObj.buffOffsetYC), 1, curStreamBuf[streamId].mirrorMode, widthYC, heightYC);

    	gIss_captCommonObj.buffOffsetY = (Ptr)Utils_tilerAddr2CpuAddr((Int32)oriBufOffsetY);
    	gIss_captCommonObj.buffOffsetYC = (Ptr)Utils_tilerAddr2CpuAddr((Int32)oriBufOffsetC);
	}

}


/* ===================================================================
 *  @func     Issdrv_IspIsifVdInt
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
Void Issdrv_IspIsifVdInt(const UInt32 event, Ptr arg)
{
    Int32 status = FVID2_SOK;
    Int32 streamId;
    Iss_CaptObj *pObj;
    Int32 imgHeight, imgWidth;
    MSP_IspQueryHistDataT histData;

    gIss_captCommonObj.intCounter[ISS_CAPT_INT_VD_INT0] ++;

    pObj = &gIss_captCommonObj.captureObj[0];

    if (pObj->state == ISS_CAPT_STATE_STOPPED)
        return ;

    if (isp_reset_trigger)
    {
        Semaphore_post(gIss_captCommonObj.ovflWait);

        isp_reset_trigger = 0;

        /* Reprogram Buffer Address in resetandrestart api,
           returning from here makes sure that the buffer is not moved to output queue and
           current buffer is not replaced with new buffer */
        return ;
    }

    for (streamId = 0; streamId < pObj->numStream; streamId++)
    {
        if (((pObj->chObj[streamId][0].frameCount < 30) && (((pObj->chObj[streamId][0].frameSkipMask >> pObj->chObj[streamId][0].frameCount) & 0x1) == 0u)) ||
            ((pObj->chObj[streamId][0].frameCount >= 30) && (((pObj->chObj[streamId][0].frameSkipMaskHigh >> (pObj->chObj[streamId][0].frameCount - 30)) & 0x1) == 0u)) ||
            (pObj->createArgs.captureMode == ISS_CAPT_INMODE_DDR))
        {
            status = bufSwitchFull(streamId);

            if(pObj->vsEnable == 0)
            {
                imgHeight = pObj->createArgs.rszPrms.rPrms[streamId].outHeight;
                imgWidth = pObj->createArgs.rszPrms.rPrms[streamId].outWidth;
            }
            else
            {
                imgHeight = (pObj->createArgs.rszPrms.rPrms[streamId].outHeight *
                                VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
                imgWidth = (pObj->createArgs.rszPrms.rPrms[streamId].outWidth *
                                VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
            }

            if (status == FVID2_SOK)
            {
	            pObj->chObj[streamId][0].captureFrameCount ++;

                if(pObj->createArgs.captureMode == ISS_CAPT_INMODE_ISIF)
                {
                    gIss_captCommonObj.buffOffsetY = curStreamBuf[streamId].ptmpFrame->addr[1][0];
                    gIss_captCommonObj.buffOffsetYC = curStreamBuf[streamId].ptmpFrame->addr[1][1];
                    gIss_captCommonObj.pitch[0] =  pObj->createArgs.outStreamInfo[streamId].pitch[0];
                    gIss_captCommonObj.pitch[1] =  pObj->createArgs.outStreamInfo[streamId].pitch[1];
                    gIss_captCommonObj.buffOffsetY +=
                        (gIss_captCommonObj.pitch[0] *
                            pObj->createArgs.rszPrms.rPrms[streamId].posPrms.startY +
                            pObj->createArgs.rszPrms.rPrms[streamId].posPrms.startX)/4;
                    gIss_captCommonObj.buffOffsetYC +=
                        (gIss_captCommonObj.pitch[1] *
                            pObj->createArgs.rszPrms.rPrms[streamId].posPrms.startY/2 +
                            pObj->createArgs.rszPrms.rPrms[streamId].posPrms.startX)/4;

                    if (streamId == 0)
                    {
                        /* This flag can be enabled only for RAW input */
                        if(pObj->glbcEnable == 1)
                        {
                            ipipe_config_boxcar_addr((UInt32)curStreamBuf[streamId].ptmpFrame->blankData);
                        }

                        if ((pObj->createArgs.outStreamInfo[streamId].dataFormat == FVID2_DF_YUV420SP_UV) ||
                            (pObj->createArgs.outStreamInfo[streamId].dataFormat == FVID2_DF_YUV420SP_VU))
                        {
                            calc_flip_offsets(1, streamId, pObj->createArgs.pitch[0], imgHeight, imgWidth);
#ifndef ENABLE_BTE
						    if (NULL == gIss_captCommonObj.buffOffsetY || NULL == gIss_captCommonObj.buffOffsetYC)
						    {
							    Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
							    GT_assert(GT_DEFAULT_MASK, FALSE);
						    }

                            status = issSetRszOutAddress(RESIZER_A, RSZ_YUV420_Y_OP, gIss_captCommonObj.buffOffsetY, gIss_captCommonObj.pitch[0]);
                            status = issSetRszOutAddress(RESIZER_A, RSZ_YUV420_C_OP, gIss_captCommonObj.buffOffsetYC, gIss_captCommonObj.pitch[1]);
#else
						    if ((NULL == gIss_captCommonObj.handleBTE.baseAddress + gIss_captCommonObj.handleBTE.bte_config[0].context_start_addr) ||
						    	(NULL == gIss_captCommonObj.handleBTE.baseAddress + gIss_captCommonObj.handleBTE.bte_config[1].context_start_addr))
						    {
							    Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
							    GT_assert(GT_DEFAULT_MASK, FALSE);
						    }

                            status = issSetRszOutAddress(RESIZER_A, RSZ_YUV420_Y_OP,
                                (void *)(gIss_captCommonObj.handleBTE.baseAddress + gIss_captCommonObj.handleBTE.bte_config[0].context_start_addr), BTE_PITCH);
                            status = issSetRszOutAddress(RESIZER_A, RSZ_YUV420_C_OP,
                                (void *)(gIss_captCommonObj.handleBTE.baseAddress + gIss_captCommonObj.handleBTE.bte_config[1].context_start_addr), BTE_PITCH);
#endif
                        }

                        else if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[streamId].dataFormat) == 1)
                        {
                            calc_flip_offsets(0, streamId, pObj->createArgs.pitch[0], imgHeight, imgWidth);
#ifndef ENABLE_BTE
						    if (NULL == gIss_captCommonObj.buffOffsetY)
						    {
							    Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
							    GT_assert(GT_DEFAULT_MASK, FALSE);
						    }

                            status = issSetRszOutAddress(RESIZER_A, RSZ_YUV422_RAW_RGB_OP, gIss_captCommonObj.buffOffsetY, gIss_captCommonObj.pitch[0]);
#else
						    if (NULL == gIss_captCommonObj.handleBTE.baseAddress + gIss_captCommonObj.handleBTE.bte_config[0].context_start_addr)
						    {
							    Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
							    GT_assert(GT_DEFAULT_MASK, FALSE);
						    }

                            status = issSetRszOutAddress(RESIZER_A, RSZ_YUV422_RAW_RGB_OP,
                                (void *)(gIss_captCommonObj.handleBTE.baseAddress + gIss_captCommonObj.handleBTE.bte_config[0].context_start_addr), BTE_PITCH);
#endif
                        }
                    }
                    else
                    {
                        if ((pObj->createArgs.outStreamInfo[streamId].dataFormat == FVID2_DF_YUV420SP_UV) ||
                            (pObj->createArgs.outStreamInfo[streamId].dataFormat == FVID2_DF_YUV420SP_VU))
                        {
                            calc_flip_offsets(1, streamId, pObj->createArgs.pitch[1], imgHeight, imgWidth);

						    if (NULL == gIss_captCommonObj.buffOffsetY || NULL == gIss_captCommonObj.buffOffsetYC)
						    {
							    Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
							    GT_assert(GT_DEFAULT_MASK, FALSE);
						    }

                            status = issSetRszOutAddress(RESIZER_B, RSZ_YUV420_Y_OP, gIss_captCommonObj.buffOffsetY, gIss_captCommonObj.pitch[0]);
                            status = issSetRszOutAddress(RESIZER_B, RSZ_YUV420_C_OP, gIss_captCommonObj.buffOffsetYC, gIss_captCommonObj.pitch[1]);

                        }
                        else if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[streamId].dataFormat) == 1)
                        {
                            calc_flip_offsets(0, streamId, pObj->createArgs.pitch[1], imgHeight, imgWidth);

						    if (NULL == gIss_captCommonObj.buffOffsetY)
						    {
							    Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
							    GT_assert(GT_DEFAULT_MASK, FALSE);
						    }
                            status = issSetRszOutAddress(RESIZER_B, RSZ_YUV422_RAW_RGB_OP, gIss_captCommonObj.buffOffsetY, gIss_captCommonObj.pitch[0]);
                        }
                    }
                }
                else
                {
					if (NULL == curStreamBuf[streamId].ptmpFrame->addr[1][0])
					{
					    Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
					    GT_assert(GT_DEFAULT_MASK, FALSE);
					}
                    // ISS_CAPT_INMODE_DDR mode
                    isif_config_sdram_address((UInt32)curStreamBuf[streamId].ptmpFrame->addr[1][0]);

                    /* This flag can be enabled only for RAW input */
                    if(pObj->glbcEnable == 1)
                    {
						if (NULL == curStreamBuf[streamId].ptmpFrame->blankData)
						{
						    Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
						    GT_assert(GT_DEFAULT_MASK, FALSE);
						}

                        ipipe_config_boxcar_addr((UInt32)curStreamBuf[streamId].ptmpFrame->blankData);
                    }

                }
            }
            else
            {
	            /* No Buffer in the driver's input queue */
	            pObj->chObj[streamId][0].dropFrameCount ++;
            }
        }
        else
        {
	        pObj->chObj[streamId][0].skipFrameCount ++;

	        /* Program NULL Address so that frames will not be written at all */
			if(pObj->createArgs.captureMode != ISS_CAPT_INMODE_ISIF)
			{
				isif_config_sdram_address(NULL);
			}
			else
			{
				if (streamId == 0)
				{
					if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[streamId].dataFormat) == 1)
					{
						status = issSetRszOutAddress(RESIZER_A, RSZ_YUV422_RAW_RGB_OP, NULL, gIss_captCommonObj.pitch[0]);
					}
					else
					{
						status = issSetRszOutAddress(RESIZER_A, RSZ_YUV420_Y_OP, NULL, gIss_captCommonObj.pitch[0]);
						status = issSetRszOutAddress(RESIZER_A, RSZ_YUV420_C_OP, NULL, gIss_captCommonObj.pitch[1]);
					}
				}
				else
				{
					if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[streamId].dataFormat) == 1)
					{
						status = issSetRszOutAddress(RESIZER_B, RSZ_YUV422_RAW_RGB_OP, NULL, gIss_captCommonObj.pitch[0]);
					}
					else
					{
						status = issSetRszOutAddress(RESIZER_B, RSZ_YUV420_Y_OP, NULL, gIss_captCommonObj.pitch[0]);
						status = issSetRszOutAddress(RESIZER_B, RSZ_YUV420_C_OP, NULL, gIss_captCommonObj.pitch[1]);
					}
				}
			}
        }

        pObj->chObj[streamId][0].frameCount = (pObj->chObj[streamId][0].frameCount + 1u) % 60u;
    }

    if (gRawCaptureTriggerd > 0)
    {
        gRawCaptureTriggerd++;

        if (gRawCaptureTriggerd > 2)
        {
            gRawCaptureTriggerd = 0;

            issTriggerRawCapture(NULL, ISIF_STOP_WRITE);

            raw_data_done = 1;
            gIss_captCommonObj.gIttParams->Raw_Data_Ready = 1;
        }
    }

	/* status may be KO when frame drop happens on one of the streams,
	we still need to post the signal otherwise buffers of the other stream will be accumulated in the tmpQ */
    //if (FVID2_SOK == status)
    {
        if (pObj->vsEnable == 0)
        {
            Semaphore_post(gIss_captCommonObj.semUpdate);
        }
    }

    if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
        (pObj->inFmt.dataFormat == FVID2_DF_RAW))
    {
        /* Get the Histogram data */
        histData.pHistBuffer = pObj->histData;
        histData.nHistRegion = 0;
        histData.eHistColor  = MSP_IPIPE_HIST_Y;
        histData.nOutHistSize = 0;

        MSP_ISP_query(gIss_captCommonObj.pModuleInstance->hIspHandle,(MSP_INDEXTYPE) MSP_ISP_QUERY_GET_HIST,&histData);

        pObj->histSize = histData.nOutHistSize;
    }

    if (pObj->state == ISS_CAPT_STATE_DO_STOP)
    {
        Semaphore_post(pObj->semStopDone);
        return;
    }
}


/* ===================================================================
 *  @func     Iss_captInit
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
Int32 Iss_captInit()
{
    Int32 status = FVID2_SOK, instId;
    Task_Params tskParams;
    Semaphore_Params semParams;

    /* set to 0's */
    memset(&gIss_captCommonObj, 0, sizeof(gIss_captCommonObj));

    /* create driver locks */
    for (instId = 0; instId < ISS_CAPT_INST_MAX; instId++)
        status |= Iss_captLockCreate(&gIss_captCommonObj.captureObj[instId]);

    if (status != FVID2_SOK)
        return status;

    gRawCaptureTriggerd = 0;
    gYUVCaptureTriggerd = 0;

    gMirrorMode = 0;

    if (status == FVID2_SOK)
    {
        /* register driver to FVID2 layer */
        gIss_captCommonObj.fvidDrvOps.create = (FVID2_DrvCreate) Iss_captCreate;
        gIss_captCommonObj.fvidDrvOps.delete = Iss_captDelete;
        gIss_captCommonObj.fvidDrvOps.control = Iss_captControl;
        gIss_captCommonObj.fvidDrvOps.queue = Iss_captQueue;
        gIss_captCommonObj.fvidDrvOps.dequeue = Iss_captDequeue;
        gIss_captCommonObj.fvidDrvOps.processFrames = NULL;
        gIss_captCommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gIss_captCommonObj.fvidDrvOps.drvId = FVID2_ISS_CAPT_DRV;

        status = FVID2_registerDriver(&gIss_captCommonObj.fvidDrvOps);
        if (status != FVID2_SOK)
        {
            Iss_captDeInit();
        }
    }

    if (FVID2_SOK == status)
    {
        status = IssCdrv_registerInt();
        if (status != FVID2_SOK)
        {
            Iss_captDeInit();
        }
        else
        {
            /* Disable All ISR at init time */
            Iem_disableInt(gIss_captCommonObj.intHandle[ISS_CAPT_INT_VD_INT0]);
            Iem_disableInt(gIss_captCommonObj.intHandle[ISS_CAPT_INT_RSZ]);
            Iem_disableInt(gIss_captCommonObj.intHandle[ISS_CAPT_INT_H3A]);
            Iem_disableInt(gIss_captCommonObj.intHandle[ISS_CAPT_INT_BSC]);
        }
    }


    if (FVID2_SOK == status)
    {
        Semaphore_Params_init(&semParams);

        semParams.mode = Semaphore_Mode_BINARY;

        gIss_captCommonObj.ovflWait =
            Semaphore_create(0u, &semParams, NULL);

        if (NULL == gIss_captCommonObj.ovflWait)
        {
            status = FVID2_EFAIL;
        }
    }

    /* Create Task for the Resetting ISS in case of Resizer Overflow */
    if (FVID2_SOK == status)
    {
        gIss_captCommonObj.exitTask = FALSE;

        Task_Params_init(&tskParams);
        tskParams.priority = 15;
        gIss_captCommonObj.ovflTask = Task_create(
                                        IssCDrv_rszResetTask,
                                        &tskParams,
                                        NULL);

        if (NULL == gIss_captCommonObj.ovflTask)
        {
            status = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK != status)
    {
        Iss_captDeInit();
    }

    return status;
}

/* ===================================================================
 *  @func     Iss_captDeInit
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
Int32 Iss_captDeInit()
{
    Int32 instId;

    IssCdrv_unRegisterInt();

    /* unregister from FVID2 layer */
    FVID2_unRegisterDriver(&gIss_captCommonObj.fvidDrvOps);

    /* delete driver locks */
    for (instId = 0; instId < ISS_CAPT_INST_MAX; instId++)
        Iss_captLockDelete(&gIss_captCommonObj.captureObj[instId]);

    gIss_captCommonObj.exitTask = TRUE;
    Semaphore_post(gIss_captCommonObj.ovflWait);
    while(Task_Mode_TERMINATED != Task_getMode(gIss_captCommonObj.ovflTask))
    {
        Task_sleep(1u);
    }
    Task_delete(&gIss_captCommonObj.ovflTask);

    Semaphore_delete(&gIss_captCommonObj.ovflWait);

    return FVID2_SOK;

}



/* ===================================================================
 *  @func     Iss_captCreate
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
Fdrv_Handle Iss_captCreate(UInt32 drvId,
                           UInt32 instanceId,
                           Iss_CaptCreateParams *createArgs,
                           Iss_CaptCreateStatus *createStatus,
                           const FVID2_CbParams *cbPrm)
{
    Int32                status;
    Iss_CaptObj         *pObj;
    Semaphore_Params     semParams;
    MSP_U32              eMSP = MSP_ERROR_NONE;
    MSP_IspConfigSizesT *pConfigSizes;
    MSP_IspParamCfgT     tIspParamCfg;
    MSP_IspQueryBscSizesT tIspBscSizes;
    MSP_IspYuvRangeCfgT  tIspYuvRangeCfg;

#ifdef ENABLE_BTE
    int i;
    bte_config_t *contextHandle;
#endif

    if (instanceId == ISS_CAPT_INST_ALL)
    {
        /*
         * gloabl handle open requested, no action required,
         * just return a special handle ID
         */
        return (Fdrv_Handle) ISS_CAPT_INST_ALL;
    }

    if (createStatus == NULL)
        return NULL;

    pConfigSizes = NULL;
    curStreamBuf[0].bufIsNew = FALSE;
    curStreamBuf[0].ptmpFrame = NULL;
    curStreamBuf[0].flipH = FALSE;
    curStreamBuf[0].flipV = FALSE;
    curStreamBuf[0].flipVOffsetY = 0;
    curStreamBuf[0].flipVOffsetC = 0;
    curStreamBuf[0].flipHOffsetYC = 0;
    curStreamBuf[0].mirrorMode = 0;
    curStreamBuf[0].memType = ISS_NONTILEDMEM;

    curStreamBuf[1].bufIsNew = FALSE;
    curStreamBuf[1].ptmpFrame = NULL;
    curStreamBuf[1].flipH = FALSE;
    curStreamBuf[1].flipV = FALSE;
    curStreamBuf[1].flipVOffsetY = 0;
    curStreamBuf[1].flipVOffsetC = 0;
    curStreamBuf[1].flipHOffsetYC = 0;
    curStreamBuf[1].mirrorMode = 0;
    curStreamBuf[1].memType = ISS_NONTILEDMEM;

    /* parameter checking */
    if (instanceId >= ISS_CAPT_INST_MAX
        || createArgs == NULL || (drvId != FVID2_ISS_CAPT_DRV))
    {
        /* invalid parameters - return NULL */
        createStatus->retVal = FVID2_EBADARGS;

        return NULL;
    }

    /* get instance specific handle */
    pObj = &gIss_captCommonObj.captureObj[instanceId];

    /* lock driver instance */
    Iss_captLock(pObj);

    /* check if object is already opended */
    if (pObj->state != ISS_CAPT_STATE_IDLE)
    {
        createStatus->retVal = FVID2_EDEVICE_INUSE;
        Iss_captUnlock(pObj);
        return NULL;
    }

    /* copy create arguments */
    memcpy(&pObj->createArgs, createArgs, sizeof(pObj->createArgs));
    memcpy(&pObj->inFmt, &createArgs->inFmt, sizeof(FVID2_Format));

    /* set instance Id */
    pObj->instanceId = instanceId;
    pObj->updateResPrms = 0;

    /* copy callback */
    if (cbPrm != NULL)
        memcpy(&pObj->cbPrm, cbPrm, sizeof(pObj->cbPrm));

    /* set number of streams */
    pObj->numStream = createArgs->numStream;

    pObj->histSize = 0;
    pObj->vsEnable = createArgs->vsEnable;
    pObj->vsDemoEnable = createArgs->vsDemoEnable;

    if ((createArgs->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
        (createArgs->inFmt.dataFormat == FVID2_DF_RAW))
    {
        pObj->glbcEnable = createArgs->glbcEnable;
    }
    else
    {
        pObj->glbcEnable = 0u;
    }

    if(pObj->createArgs.captureMode == ISS_CAPT_INMODE_DDR)
    {
        pObj->numStream = 1;
        pObj->vsEnable  = 0;
    }

    /* create start and stop sync semaphores */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    pObj->semStopDone = Semaphore_create(0, &semParams, NULL);
    if (pObj->semStopDone == NULL)
    {
        createStatus->retVal = FVID2_EALLOC;
        Iss_captUnlock(pObj);
        return NULL;
    }

    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    pObj->semStartDone = Semaphore_create(0, &semParams, NULL);
    if (pObj->semStartDone == NULL)
    {
        Semaphore_delete(&pObj->semStopDone);
        createStatus->retVal = FVID2_EALLOC;
        Iss_captUnlock(pObj);
        return NULL;
    }

    /* create channel specific objects */
    status = Iss_captCreateChObj(pObj);
    if (status == FVID2_SOK)
    {
        /* create input and output queues */
        status = Iss_captCreateQueues(pObj);
    }

    createStatus->retVal = status;
    if (status != FVID2_SOK)
    {
        /* Error - free previously allocated objects */
        Semaphore_delete(&pObj->semStartDone);
        Semaphore_delete(&pObj->semStopDone);
        Iss_captUnlock(pObj);
        return NULL;
    }

    pObj->createArgs.pCompPrivate = (MSP_PTR) malloc(sizeof(VideoModuleInstanceT));
    if (pObj->createArgs.pCompPrivate == NULL)
    {
        return NULL;
    }

    gIss_captCommonObj.pModuleInstance = (VideoModuleInstanceT *) (pObj->createArgs.pCompPrivate);
    gIss_captCommonObj.pModuleInstance->pH3aAewbBuff = NULL;
    gIss_captCommonObj.pModuleInstance->pH3aAfBuff = NULL;
    gIss_captCommonObj.pModuleInstance->pH3aAewbBuffNoPad = NULL;
    gIss_captCommonObj.pModuleInstance->hInit = NULL;
    gIss_captCommonObj.pModuleInstance->hIspHandle = NULL;
    gIss_captCommonObj.pModuleInstance->nLscEnable = 0;
    gIss_captCommonObj.pModuleInstance->nLscTableIsValid = MSP_FALSE;
    gIss_captCommonObj.pModuleInstance->bBscFromFD = MSP_TRUE;
    gIss_captCommonObj.pModuleInstance->bAllocateOutBuff = MSP_TRUE;
    gIss_captCommonObj.pModuleInstance->nAlterSeqNum = 0;
    gIss_captCommonObj.pModuleInstance->nBscAlterSeqNum = 0;
    gIss_captCommonObj.pModuleInstance->nIssInX = 0;
    gIss_captCommonObj.pModuleInstance->nIssInY = 0;
    gIss_captCommonObj.pModuleInstance->nARPrvAlterSeqNum = 0;
    gIss_captCommonObj.pModuleInstance->nARCapAlterSeqNum = 0;
    gIss_captCommonObj.pModuleInstance->bFilledBuffsInvalidate = MSP_FALSE;
    gIss_captCommonObj.pModuleInstance->bVidStab = MSP_FALSE;
    gIss_captCommonObj.pModuleInstance->nLscEnable = 0;
    gIss_captCommonObj.pModuleInstance->nLscTableIsValid = MSP_FALSE;
    gIss_captCommonObj.pModuleInstance->bAllocateOutBuff = MSP_FALSE;
    gIss_captCommonObj.pModuleInstance->tIspAppParam.pAppData = gIss_captCommonObj.pModuleInstance;
    gIss_captCommonObj.pModuleInstance->tIspAppParam.MSP_callback = drvIspCallback;

    CLEAR_ALL_EVENTS(gIss_captCommonObj.pModuleInstance);

    gIss_captCommonObj.pModuleInstance->hIspHandle =
        (MSP_COMPONENT_TYPE *) malloc(sizeof(MSP_COMPONENT_TYPE));
    if (gIss_captCommonObj.pModuleInstance->hIspHandle == NULL)
    {
        goto exit0;
    }

    ((MSP_COMPONENT_TYPE *) (gIss_captCommonObj.pModuleInstance->hIspHandle))->tAppCBParam =
        gIss_captCommonObj.pModuleInstance->tIspAppParam;

    ((MSP_COMPONENT_TYPE*)gIss_captCommonObj.pModuleInstance->hIspHandle)->pCompPrivate = NULL;
    MSP_ISP_init(gIss_captCommonObj.pModuleInstance->hIspHandle, MSP_PROFILE_REMOTE);

    pConfigSizes = (MSP_PTR) malloc(sizeof(MSP_IspConfigSizesT));
    if (pConfigSizes == NULL)
    {
        goto exit0;
    }

    gIss_captCommonObj.pIssConfig = (MSP_PTR) calloc(1, sizeof(MSP_IspConfigProcessingT));
    if (gIss_captCommonObj.pIssConfig == NULL)
    {
        goto exit0;
    }

    MSP_ISP_control(gIss_captCommonObj.pModuleInstance->hIspHandle, MSP_CTRLCMD_START, NULL);


#ifdef USE_MIPI_MODE
   /* pIssCsiHandle = (IssCsi2rxIsrHandleT*)malloc(sizeof(IssCsi2rxIsrHandleT));
    pIssCsiHandle->arg1 = 1;
    pIssCsiHandle->arg2 = NULL;
    pIssCsiHandle->callback = isp_test_csi2rx_line_end_handle;
    issCsi2rxHookLineNumIsr(
        ISS_CSI_DEV_A,
        0,
        pIssCsiHandle,
        (pObj->inFmt.height) / 2);*/
#endif

    pConfigSizes->nInSizeX = pObj->inFmt.width;
    pConfigSizes->nInSizeY = pObj->inFmt.height;
    pConfigSizes->nInSizePpln = pObj->createArgs.pitch[0];
    pConfigSizes->nInStartX = pObj->createArgs.rszPrms.cropPrms.cropStartX;
    pConfigSizes->nInStartY = pObj->createArgs.rszPrms.cropPrms.cropStartY;

    if(pObj->vsEnable == 0)
    {
        pConfigSizes->nOutSizeAX =
            pObj->createArgs.rszPrms.rPrms[0].outWidth;
        pConfigSizes->nOutSizeAY =
        pObj->createArgs.rszPrms.rPrms[0].outHeight;
    }
    else
    {
        pConfigSizes->nOutSizeAX =
            (pObj->createArgs.rszPrms.rPrms[0].outWidth *
                VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
        pConfigSizes->nOutSizeAY =
            (pObj->createArgs.rszPrms.rPrms[0].outHeight *
                VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;;
    }

    if((pConfigSizes->nOutSizeAX > pConfigSizes->nInSizeX) ||
       (pConfigSizes->nOutSizeAY > pConfigSizes->nInSizeY))
    {
        goto exit0;
    }

    pConfigSizes->nOutSizeABpln = pObj->createArgs.pitch[0];
    pConfigSizes->nOutSizeABplnC = 0;

    if (pObj->numStream == 2)
    {
        if((pObj->vsEnable == 0) || (pObj->vsDemoEnable == 1))
        {
            pConfigSizes->nOutSizeBX =
                pObj->createArgs.rszPrms.rPrms[1].outWidth;
            pConfigSizes->nOutSizeBY =
                pObj->createArgs.rszPrms.rPrms[1].outHeight;
        }
        else
        {
            pConfigSizes->nOutSizeBX =
                (pObj->createArgs.rszPrms.rPrms[1].outWidth *
                    VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
            pConfigSizes->nOutSizeBY =
                (pObj->createArgs.rszPrms.rPrms[1].outHeight *
                    VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
        }

        if((pConfigSizes->nOutSizeBX > pConfigSizes->nInSizeX) ||
           (pConfigSizes->nOutSizeBY > pConfigSizes->nInSizeY))
        {
            goto exit0;
        }

        pConfigSizes->nOutSizeBBpln = pObj->createArgs.pitch[1];
        pConfigSizes->nOutSizeBBplnC = 0;
    }
    else
    {
        pConfigSizes->nOutSizeBX = 720;
        pConfigSizes->nOutSizeBY = 480;
        pConfigSizes->nOutSizeBBpln = 720;
    }

    pConfigSizes->nCropX = pObj->createArgs.rszPrms.cropPrms.cropStartX;
    pConfigSizes->nCropY = pObj->createArgs.rszPrms.cropPrms.cropStartY;

    pConfigSizes->nClockPercents = MSP_ISS_SPEED_MAX;

    eMSP =
        MSP_ISP_query(gIss_captCommonObj.pModuleInstance->hIspHandle,
                      (MSP_INDEXTYPE) MSP_ISP_QUERY_MAX_WIDTH, pConfigSizes);
    eMSP =
        MSP_ISP_query(gIss_captCommonObj.pModuleInstance->hIspHandle,
                      (MSP_INDEXTYPE) MSP_ISP_QUERY_CROP_SIZES, pConfigSizes);
    if (eMSP != MSP_ERROR_NONE)
    {
        // Vps_printf("Error MSP_ISP_query failed!!!\n");
    }

    gIss_captCommonObj.pModuleInstance->nLscTableIsValid = MSP_FALSE;
    gIss_captCommonObj.pModuleInstance->nOutStartX = 0;
    gIss_captCommonObj.pModuleInstance->nOutStartY = 0;
    gIss_captCommonObj.pModuleInstance->bVidStab = MSP_FALSE;
    gIss_captCommonObj.pModuleInstance->bAllocateOutBuff = MSP_FALSE;

    IssFillDefaultParams((iss_config_processing_t *) gIss_captCommonObj.pIssConfig);

#if defined(APPRO_SENSOR_VENDOR) && defined(IMGS_SONY_IMX136)
	gIss_captCommonObj.pIssConfig->eMsbPos = MSP_IPIPE_BAYER_MSB_BIT13;
	isif_reg->CLDCOFST = 0;
#else
    gIss_captCommonObj.pIssConfig->eMsbPos = MSP_IPIPE_BAYER_MSB_BIT11;
#endif
    gIss_captCommonObj.pIssConfig->ptBsc = &(gIss_captCommonObj.pModuleInstance->tBscCfg);
    gIss_captCommonObj.pIssConfig->ptLsc2D->nHDirDataOffset = 16;
#ifdef IMGS_OMNIVISION_OV7740
    gIss_captCommonObj.pIssConfig->eColorPattern = MSP_IPIPE_BAYER_PATTERN_BGGR;
#else
    gIss_captCommonObj.pIssConfig->eColorPattern = MSP_IPIPE_BAYER_PATTERN_GRBG;
#endif
    gIss_captCommonObj.pIssConfig->nVpDevice = MSP_IPIPE_VP_DEV_PI;
    gIss_captCommonObj.pIssConfig->eFH3aValidity =
        (MSP_PROC_H3A_VALID_ID) (MSP_PROC_H3A_VALID_AEWB |
                                 MSP_PROC_H3A_VALID_H3A |
                                 MSP_PROC_H3A_VALID_AF);

    gIss_captCommonObj.pModuleInstance->tConfigParams.eInFormat = MSP_IPIPE_IN_FORMAT_BAYER;

    if ((pObj->createArgs.outStreamInfo[0].dataFormat == FVID2_DF_YUV420SP_UV)
        || (pObj->createArgs.outStreamInfo[0].dataFormat ==
            FVID2_DF_YUV420SP_VU))
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.eOutFormatA =
            MSP_IPIPE_OUT_FORMAT_YUV420;
    }
    else if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[0].dataFormat) == 1)
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.eOutFormatA =
            MSP_IPIPE_OUT_FORMAT_YUV422;
    }
    else if(pObj->createArgs.outStreamInfo[0].dataFormat == FVID2_DF_BAYER_RAW)
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.eOutFormatA =
            MSP_IPIPE_OUT_FORMAT_YUV422;
    }
    else
    {
        Vps_printf("Stream 0: Invalid output format configured!!!\n");
    }

    if ((pObj->createArgs.outStreamInfo[1].dataFormat == FVID2_DF_YUV420SP_UV)
        || (pObj->createArgs.outStreamInfo[1].dataFormat ==
            FVID2_DF_YUV420SP_VU))
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.eOutFormatB =
            MSP_IPIPE_OUT_FORMAT_YUV420;
    }
    else if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[1].dataFormat) == 1)
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.eOutFormatB =
            MSP_IPIPE_OUT_FORMAT_YUV422;
    }
    else
    {
        Vps_printf("Stream 1: Invalid output format configured!!!\n");
    }

    gIss_captCommonObj.pModuleInstance->tConfigParams.eCompresIn = MSP_IPIPE_PROC_COMPR_NO;
    gIss_captCommonObj.pModuleInstance->tConfigParams.eCompresOut = MSP_IPIPE_PROC_COMPR_NO;
    gIss_captCommonObj.pModuleInstance->tConfigParams.nFFlipB = 0;
    gIss_captCommonObj.pModuleInstance->tConfigParams.nFMirrorB = 0;
    gIss_captCommonObj.pModuleInstance->tConfigParams.nFFlipA = 0;
    gIss_captCommonObj.pModuleInstance->tConfigParams.nFMirrorA = 0;
    gIss_captCommonObj.pModuleInstance->tConfigParams.nVidstabEnb = 0;
    gIss_captCommonObj.pModuleInstance->tConfigParams.ptIssConfig = gIss_captCommonObj.pIssConfig;
    gIss_captCommonObj.pModuleInstance->tConfigParams.pLsc2DTableBuffer =
        gIss_captCommonObj.pModuleInstance->p2DLscTableBuff;

    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nInSizeX =
        //gIss_captCommonObj.pModuleInstance->nIssInX;
        pObj->inFmt.width;
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nInSizeY =
        //gIss_captCommonObj.pModuleInstance->nIssInY;
        pObj->inFmt.height;

    gIss_captCommonObj.pModuleInstance->tConfigParams.ptIssConfig->eRszIpPortSel =
        MSP_RSZ_IP_IPIPEIF;

    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeAX =
        pConfigSizes->nOutSizeAX;
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeAY =
        pConfigSizes->nOutSizeAY;

#ifndef ENABLE_BTE
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeABpln =
        pObj->createArgs.pitch[0];
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeABplnC =
        pObj->createArgs.pitch[0];
#else
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeABpln =
        BTE_PITCH;
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeABplnC =
        BTE_PITCH;
#endif

    if (pObj->numStream == 2)
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBX =
            pConfigSizes->nOutSizeBX;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBY =
            pConfigSizes->nOutSizeBY;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBBpln =
            pObj->createArgs.pitch[1];
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBBplnC =
            pObj->createArgs.pitch[1];
    }
    else
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBX = 720;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBY = 480;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBBpln = 720;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBBplnC = 720;
    }

    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nClockPercents =
        MSP_ISS_SPEED_MAX;

    tIspParamCfg.ptIssParams = &gIss_captCommonObj.pModuleInstance->tConfigParams;
    // RAJAT - need to correct the configuration being sent out to remove the
    // error

    MSP_ISP_config(gIss_captCommonObj.pModuleInstance->hIspHandle,
                   (MSP_INDEXTYPE) MSP_ISP_CFG_PROC_MEM_TO_MEM, &tIspParamCfg);
    Vps_rprintf("\n%s:%d\n", __func__, __LINE__);

    if (pObj->createArgs.videoCaptureMode == ISS_CAPT_INMODE_ISIF)
    {
        if(pObj->numStream == 1)
        {
            status = Iss_Disable_RszB();

            if(status == FVID2_EFAIL)
                Vps_rprintf("Iss_Disable_RszB Failed \n");
        }

        /* Resizer should be configured in ISIF mode only */
        Issdrv_captSetRszCfg(pObj, &pObj->createArgs.rszPrms);
    }

    gIss_captCommonObj.pModuleInstance->nLscTableIsValid = MSP_TRUE;
    ipipe_reg->SRC_VSZ = pObj->inFmt.height;
    ipipe_reg->SRC_HSZ = pObj->inFmt.width;

    IssCdrv_setIpipeCfg();

    gIss_captCommonObj.pModuleInstance->p2AObj = (Iss_2AObj*)malloc(sizeof(Iss_2AObj));
    if ( gIss_captCommonObj.pModuleInstance->p2AObj == NULL)
    {
        goto exit0;
    }

    ((Iss_2AObj*)(gIss_captCommonObj.pModuleInstance->p2AObj))->taskHndl = NULL;

    Issdrv_captSetDefaultH3APrms();
    Issdrv_getH3aConfig(&pObj->h3aCfg);
    Issdrv_setH3aConfig(&pObj->h3aCfg, 1);
    Issdrv_getIsifConfig(&pObj->isifCfg);
    Issdrv_setIsifConfig(&pObj->isifCfg);

    gIss_captCommonObj.pIssConfig->ptLsc2D->nGainTableAddress = (MSP_U32)memalign(256, 40240);
    GT_assert(GT_DEFAULT_MASK, gIss_captCommonObj.pIssConfig->ptLsc2D->nGainTableAddress != NULL);

    gIss_captCommonObj.pIssConfig->ptLsc2D->nGainTableLength = 40240;
    gIss_captCommonObj.pIssConfig->ptLsc2D->nOffsetTableAddress  = (MSP_U32)memalign(256, 40240);
    GT_assert(GT_DEFAULT_MASK, gIss_captCommonObj.pIssConfig->ptLsc2D->nOffsetTableAddress != NULL);

    gIss_captCommonObj.pIssConfig->ptLsc2D->nOffsetTableLength = 40240;

    // //////////////////////////////////////////////////////////////////////////////////////////////////////

    tIspBscSizes.pRowsBufferSize = 0;
    tIspBscSizes.pColsBufferSize = 0;

    tIspBscSizes.ptBscCfg = gIss_captCommonObj.pModuleInstance->tConfigParams.ptIssConfig->ptBsc;

    MSP_ISP_query(gIss_captCommonObj.pModuleInstance->hIspHandle,
                  (MSP_INDEXTYPE) MSP_ISP_QUERY_BSC_BUFF_SIZES, &tIspBscSizes);

    gIss_captCommonObj.pModuleInstance->nBscRowBuffSize = tIspBscSizes.pRowsBufferSize;
    gIss_captCommonObj.pModuleInstance->nBscColBuffSize = tIspBscSizes.pColsBufferSize;

    tIspYuvRangeCfg.eIspYuvRange = MSP_ISS_YUV_RANGE_FULL;

    MSP_ISP_config(gIss_captCommonObj.pModuleInstance->hIspHandle,
                   (MSP_INDEXTYPE) MSP_ISP_CFG_YUV_RANGE, &tIspYuvRangeCfg);

    if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
        (pObj->inFmt.dataFormat == FVID2_DF_RAW))
    {
        /* Histogram Config */
        {
            MSP_IpipeHistCfgT histogramCfg;
            MSP_IpipeHistDimT histogramDim[4];
            MSP_U8            gainTbl[4];

            memcpy(&histogramCfg,gIss_captCommonObj.pIssConfig->ptHistogram,sizeof(MSP_IpipeHistCfgT));

            histogramCfg.nOst  = 0;     // Free Run
            histogramCfg.nSel  = 1;         // Input Selection - 0. From noise filter 1.From RGBtoYUV
            histogramCfg.nType = 2;     // G selection in Bayer mode - (Gb + Gr)/2

            histogramDim[0].nVPos  = 0;
            histogramDim[0].nVSize = pObj->inFmt.height;
            histogramDim[0].nHPos  = 0;
            histogramDim[0].nHSize = pObj->inFmt.width;

            gainTbl[0] = 6;             // R
            gainTbl[1] = 6;             // Gr
            gainTbl[2] = 6;             // Gb
            gainTbl[3] = 6;             // B

            histogramCfg.ptHistDim = histogramDim;
            histogramCfg.pnGainTbl = gainTbl;

            MSP_ISP_config(
                gIss_captCommonObj.pModuleInstance->hIspHandle,
                (MSP_INDEXTYPE)MSP_ISP_CFG_HIST,&histogramCfg);
        }
        if(pObj->glbcEnable == 1)
        {
            MSP_IpipeBoxcarCfgT BoxCarCfg;
            memcpy(
                &BoxCarCfg,
                gIss_captCommonObj.pIssConfig->ptBoxcar,
                sizeof(MSP_IpipeBoxcarCfgT));

            BoxCarCfg.nEnable = TRUE;
            if (pObj->createArgs.captureMode == ISS_CAPT_INMODE_ISIF)
            {
                BoxCarCfg.nOst = 0;
            }
            else
            {
                BoxCarCfg.nOst = 1;
            }
                BoxCarCfg.nBoxSize = 1;
                BoxCarCfg.nShift = 3;
                BoxCarCfg.pAddr = NULL;

            MSP_ISP_config(
                gIss_captCommonObj.pModuleInstance->hIspHandle,
                (MSP_INDEXTYPE)MSP_ISP_CFG_BOXCAR,
                &BoxCarCfg);
        }
    }

    /*
     * mark state as created
     */
    pObj->state = ISS_CAPT_STATE_CREATED;

    /*
     * unlock driver instance
     */

    Iss_captUnlock(pObj);

    IssAlg_captInit();

#ifdef ENABLE_BTE
    GT_assert(GT_DEFAULT_MASK, bte_init() == BTE_SUCCESS);
    GT_assert(GT_DEFAULT_MASK, bte_open(1) == BTE_SUCCESS);/*0x20000000~0x3FFFFFFF*/

    gIss_captCommonObj.handleBTE.baseAddress = 0x20000000;

    gIss_captCommonObj.handleBTE.contextNumber =
        2 << BTE_FEXT(bte_reg->BTE_HL_HWINFO, CSL_BTE_BTE_HL_HWINFO_CONTEXTS_SHIFT, CSL_BTE_BTE_HL_HWINFO_CONTEXTS_MASK);

    gIss_captCommonObj.handleBTE.bwLimiter = BTE_BWL;

    BTE_SET32(bte_reg->BTE_CTRL, gIss_captCommonObj.handleBTE.bwLimiter, CSL_BTE_BTE_CTRL_BW_LIMITER_SHIFT,
              CSL_BTE_BTE_CTRL_BW_LIMITER_MASK);
    BTE_SET32(bte_reg->BTE_CTRL, 1, CSL_BTE_BTE_CTRL_POSTED_SHIFT,
              CSL_BTE_BTE_CTRL_POSTED_MASK);

    for (i = 0; i < gIss_captCommonObj.handleBTE.contextNumber; i++)
    {
        contextHandle = &gIss_captCommonObj.handleBTE.bte_config[i];

        contextHandle->context_num = (BTE_CONTEXT)i;

        contextHandle->context_ctrl.init_sx = 0;
        contextHandle->context_ctrl.init_sy = 0;

        contextHandle->context_ctrl.mode = 0x0;/*write*/
        contextHandle->context_ctrl.one_shot = 0x1;/*one shot*/
        contextHandle->context_ctrl.addr32 = 1;/*TILER*/
        contextHandle->context_ctrl.autoflush = 0;

        contextHandle->context_ctrl.start = 0;
        contextHandle->context_ctrl.stop = 0;
        contextHandle->context_ctrl.flush = 0;
    }
#endif

    if (pConfigSizes != NULL)
    {
        free(pConfigSizes);
    }

    return pObj;

  exit0:
    if (gIss_captCommonObj.pModuleInstance->hIspHandle != NULL)
    {
        free(gIss_captCommonObj.pModuleInstance->hIspHandle);
    }
    if (pConfigSizes != NULL)
    {
        free(pConfigSizes);
    }
    if (gIss_captCommonObj.pIssConfig != NULL)
    {
        free(gIss_captCommonObj.pIssConfig);
    }

    return NULL;
}



/* ===================================================================
 *  @func     Iss_captDelete
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
Int32 Iss_captDelete(Fdrv_Handle handle, Ptr reserved)
{
    Int32                       status = FVID2_EFAIL;
    Iss_CaptObj                *pObj = (Iss_CaptObj *) handle;

    if (handle == (Fdrv_Handle) ISS_CAPT_INST_ALL)
        return FVID2_SOK;

    GT_assert(GT_DEFAULT_MASK, pObj != NULL);

    /* lock driver instance */
    Iss_captLock(pObj);

    /* if driver is created or driver is ready for deletion, i.e stopped */
    if (pObj->state == ISS_CAPT_STATE_STOPPED
        || pObj->state == ISS_CAPT_STATE_CREATED)
    {
        /* Stop H3A */
        IssAlg_capt2ADeInit(
            (Iss_2AObj*)gIss_captCommonObj.pModuleInstance->p2AObj);

		pObj->dbgLineNo = __LINE__;	
			
        free(gIss_captCommonObj.pModuleInstance->p2AObj);

        /* stop Isp processing */
        MSP_ISP_control(
            gIss_captCommonObj.pModuleInstance->hIspHandle,
            MSP_CTRLCMD_STOP,
            NULL);

		pObj->dbgLineNo = __LINE__;
			
        /* Soft resetting ISS */
        ipipe_config_boxcar_addr(NULL);
		
		pObj->dbgLineNo = __LINE__;
		
        Iss_ispWriteReg(&iss_regs->ISS_HL_SYSCONFIG,1,0,1);
		
		pObj->dbgLineNo = __LINE__;
				
        /* delete semaphores */
        Semaphore_delete(&pObj->semStopDone);
        Semaphore_delete(&pObj->semStartDone);

        /*
         * delete queues accosciated with this driver instance
         */

        Iss_captDeleteQueues(pObj);

        /*
         * free path associated with this driver
         */
        Iss_captFreePath(pObj);

        /* MSP ISP DeInit */
        ((MSP_COMPONENT_TYPE*)gIss_captCommonObj.pModuleInstance->hIspHandle)->
            deInit((MSP_HANDLE)gIss_captCommonObj.pModuleInstance->hIspHandle);

		pObj->dbgLineNo = __LINE__;
			
        /*
         * free buffer allocations
         */
        free((void *)gIss_captCommonObj.pIssConfig->ptLsc2D->nGainTableAddress);
        free((void *)gIss_captCommonObj.pIssConfig->ptLsc2D->nOffsetTableAddress);
        free(gIss_captCommonObj.pModuleInstance->hIspHandle);
        free(gIss_captCommonObj.pModuleInstance);
        free(gIss_captCommonObj.pIssConfig);

        /*
         * mark state as idle
         */
        pObj->state = ISS_CAPT_STATE_IDLE;
    }

    /*
     * unlock driver instance
     */
    Iss_captUnlock(pObj);
    IssAlg_captDeInit();

	pObj->dbgLineNo = __LINE__;
	
    return status;
}



/* ===================================================================
 *  @func     Iss_captQueue
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
Int32 Iss_captQueue(Fdrv_Handle handle,
                    FVID2_FrameList * frameList, UInt32 streamId)
{
    Iss_CaptObj *pObj = (Iss_CaptObj *) handle;
    Iss_CaptChObj *pChObj;
    Int32 status = FVID2_SOK, queStatus;
    FVID2_Frame *pFrame;
    UInt16 frameId, chId, instId, lChannelNum, drvStreamId;

    /*
     * if global handle then make handle as NULL, this indicates
     * we need to check over all driver instances
     */
    if (handle == (Fdrv_Handle) ISS_CAPT_INST_ALL)
        handle = NULL;

    /*
     * parameter checking
     */
    if (frameList == NULL || frameList->numFrames == 0 ||
        frameList->numFrames > FVID2_MAX_FVID_FRAME_PTR)
    {
        return FVID2_EFAIL;
    }

    /*
     * for all frames that need to be queued
     */
    for (frameId = 0; frameId < frameList->numFrames; frameId++)
    {
        /* get FVID2 frame pointer */
        pFrame = frameList->frames[frameId];

        if (pFrame == NULL)
        {
            /*
             * invalid FVID2 frame ID
             */
            status = FVID2_EFAIL;
            continue;
        }

        /*
         * map user channel number to driver channel number
         */
        lChannelNum = gIss_captCommonObj.fvidChannelNum2lChannelNumMap
            [pFrame->channelNum];

        /*
         * extract driver instance ID from driver channel number
         */
        instId = Iss_captGetInstId(lChannelNum);

        if (instId >= ISS_CAPT_INST_MAX)
        {
            /* invalid instance ID */
            status = FVID2_EFAIL;
            continue;
        }

        if (handle == NULL)
        {
            /*
             * if global handle mode, the get instance handle
             * from driver channel number extract instance ID
             */
            pObj = &gIss_captCommonObj.captureObj[instId];
        }

        if (instId != pObj->instanceId)
        {
            /*
             * if mismatch then channelNum in FVID2 Frame is not correct,
             * skip this frame que
             */

            status = FVID2_EFAIL;
            continue;
        }

        if (pObj->state == ISS_CAPT_STATE_IDLE)
        {
            /*
             * if driver handle is not open then skip this frame que
             */
            status = FVID2_EFAIL;
            continue;
        }

        if (streamId == ISS_CAPT_STREAM_ID_ANY)
            drvStreamId = Iss_captGetStreamId(lChannelNum);
        else
            drvStreamId = streamId;

        if (drvStreamId >= pObj->numStream ||
            drvStreamId != Iss_captGetStreamId(lChannelNum))
        {
            /*
             * invalid stream ID skip this frame que
             */
            status = FVID2_EFAIL;
            continue;
        }

        /*
         * get channel ID
         */
        chId = Iss_captGetChId(lChannelNum);
        if (chId >= pObj->numCh)
        {
            /*
             * invalid channel ID skip this frame que
             */
            status = FVID2_EFAIL;
            continue;
        }

        /*
         * valid instance, stream and channel
         */

        /*
         * get channel specific object in the required instance
         */
        pChObj = &pObj->chObj[drvStreamId][chId];

        /*
         * que the frame to the channel empty frame que
         */
        queStatus = VpsUtils_quePut(&pChObj->emptyQue, pFrame, BIOS_NO_WAIT);

        /*
         * this assert should never happen
         */
        GT_assert(GT_DEFAULT_MASK, queStatus == FVID2_SOK);

        /* Mark frame in frameList as NULL */
        frameList->frames[frameId] = NULL;
    }

    return status;
}



/* ===================================================================
 *  @func     Iss_captDequeue
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
Int32 Iss_captDequeue(Fdrv_Handle handle,
                      FVID2_FrameList * frameList,
                      UInt32 streamId,
                      UInt32 timeout)
{
    Iss_CaptObj *pObj = (Iss_CaptObj *) handle;
    Int32 status = FVID2_SOK, dequeStatus, instId;
    Ptr pData;

    /*
     * if global handle then make handle as NULL, this indicates
     * we need to check over all driver instances
     */
    if (handle == (Fdrv_Handle) ISS_CAPT_INST_ALL)
        handle = NULL;

    /*
     * parameter checking
     */
    if (frameList == NULL || streamId >= ISS_CAPT_STREAM_ID_MAX)
    {
        return FVID2_EFAIL;
    }

    /*
     * init frame list fields
     */
    frameList->numFrames = 0;
    frameList->perListCfg = NULL;
    frameList->drvData = NULL;
    frameList->reserved = NULL;

    /*
     * for every instance
     */
    for (instId = 0; instId < ISS_CAPT_INST_MAX; instId++)
    {
        if (handle == NULL)
        {
            /*
             * if handle is NULL, i.e global handle, then get handle using 'instId',
             * else use user supplied handle
             */
            pObj = &gIss_captCommonObj.captureObj[instId];
        }

        /*
         * check if driver instance is not idle, i.e driver is opened
         */
        if (pObj->state != ISS_CAPT_STATE_IDLE)
        {
            /*
             * validate stream ID
             */
            if (streamId >= pObj->numStream)
            {
                status = FVID2_EFAIL;
            }

            if (status == FVID2_SOK)
            {
                /*
                 * deque for current handle
                 */
                do
                {
                    /*
                     * deque from handle, could be blocking for the first time
                     * based on 'timeout'
                     */
                    dequeStatus = VpsUtils_queGet(
                                    &pObj->fullQue[streamId],
                                    &pData,
                                    1,
                                    timeout);
                    if (dequeStatus == FVID2_SOK)
                    {
                        /*
                         * deque is successful, add to frame list
                         */
                        frameList->frames[frameList->numFrames] =
                            (FVID2_Frame *) pData;

                        frameList->numFrames++;
                    }
                    /*
                     * second iteration onwards do non-blocking deque
                     */
                    timeout = BIOS_NO_WAIT;

                    /*
                     * Max frames limit exceeded exit
                     */
                    if (frameList->numFrames >= FVID2_MAX_FVID_FRAME_PTR)
                        break;
                }
                while (dequeStatus == FVID2_SOK);

                /*
                 * all frames deque from current driver handle
                 */
            }
        }

        if (handle != NULL)
            break;                                         /* if not global
                                                            * handle, then we
                                                            * are done, so
                                                            * exit */

        /*
         * Max frames limit exceeded exit
         */
        if (frameList->numFrames >= FVID2_MAX_FVID_FRAME_PTR)
            break;

        /*
         * global handle mode, so check other driver instances also
         */
    }

    return status;
}



/* ===================================================================
 *  @func     Iss_captControl
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
Int32 Iss_captControl(Fdrv_Handle handle, UInt32 cmd,
                      Ptr cmdArgs, Ptr cmdStatusArgs)
{
    Int32 status = FVID2_SOK;
    Iss_CaptObj *pObj = &gIss_captCommonObj.captureObj[0];
    UInt32 value = 0;
    Iss_2AObj *p2AObj;
    p2AObj = (Iss_2AObj*)gIss_captCommonObj.pModuleInstance->p2AObj;

    /*
     * switch to correct CMD handler
     */
    switch (cmd)
    {
        case FVID2_START:
            status = Iss_captStart(handle);
            break;

        case FVID2_STOP:
            status = Iss_captStop(handle);
            break;

        case IOCTL_ISS_CAPT_SET_SC_PARAMS:
        {
            Iss_IspResizerParams *rszPrms = (Iss_IspResizerParams *)cmdArgs;

            /* check For Errors */
            if (rszPrms->numOutput > pObj->numStream)
            {
                rszPrms->numOutput = pObj->numStream;
            }

            memcpy(
                &pObj->createArgs.rszPrms,
                rszPrms,
                sizeof(Iss_IspResizerParams));

            pObj->updateResPrms = 1;
            break;
        }

        case IOCTL_ISS_CAPT_RESET_AND_RESTART:
            status = Iss_captResetAndRestart(cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_FRAME_SKIP:
            status = Iss_captSetFrameSkip(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_MIRROR_MODE:
            status = Iss_captSetMirrorMode(handle, cmdArgs);
            break;

        case IOCTL_ISS_ALG_2A_UPDATE:
            status = Iss_capt2AUpdate(handle);
            break;

        case IOCTL_ISS_CAPT_SET_RESOLUTION:
            status = Iss_captSetResolution(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_CHANGE_RESOLUTION:
            status = Iss_captSetInResolution(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_FRAMERATE:
            status = Iss_captSetFrameRate(handle);
            status = IssAlg_capt2ASetFrameRate(handle, cmdArgs);
            break;

        case IOCTL_ISS_ALG_ITT_CONTROL:
            status = Iss_captITTControl(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_COLOR:
            status = IssAlg_capt2ASetColor(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_AEWBVENDOR:
            status = IssAlg_capt2ASetAEWBVendor(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_AEWBMODE:
            status = IssAlg_capt2ASetAEWBMode(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_IRIS:
            status = IssAlg_capt2ASetIris(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_AEWBPRI:
            status = IssAlg_capt2ASetAEWBPri(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_SHARPNESS:
            status = IssAlg_capt2ASetSharpness(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_BLC:
            status = IssAlg_capt2ASetBlc(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_AWBMODE:
            status = IssAlg_capt2ASetAwbMode(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_AEMODE:
            status = IssAlg_capt2ASetAeMode(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_ENV:
            status = IssAlg_capt2ASetEnv(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_BINNING:
            status = IssAlg_capt2ASetBinning(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_SET_DCCPRM:
            status = IssAlg_capt2ASetDccPrm(handle, cmdArgs);
            break;

        case IOCTL_ISS_CAPT_GET_HISTADDR:
        {
            if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
                (pObj->inFmt.dataFormat == FVID2_DF_RAW))
            {
                *((UInt32*)cmdArgs) = (UInt32)pObj->histData;
                status = FVID2_SOK;
            }
            else
            {
                status = FVID2_EUNSUPPORTED_CMD;
            }
            break;
        }

        case IOCTL_ISS_CAPT_GET_AFDATA:
        {
            if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
                (pObj->inFmt.dataFormat == FVID2_DF_RAW))
            {
                *((UInt32*)cmdArgs) = (UInt32)p2AObj->AFValue;
                status = FVID2_SOK;
            }
            else
            {
                status = FVID2_EUNSUPPORTED_CMD;
            }
            break;
        }

        case IOCTL_ISS_CAPT_GET_AEWBDATA:
        {
            if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
                (pObj->inFmt.dataFormat == FVID2_DF_RAW))
            {
                value = (1 << 10 ); value -= 1;
                p2AObj->AEWBValue1 &= (UInt32)value; //low 10 bit
                value = (1 << 20 ); value -= 1;
                p2AObj->AEWBValue2 &= (UInt32)value; //high 20 bit
                value = (UInt32)p2AObj->AEWBValue1 + ( (UInt32)p2AObj->AEWBValue2 << 10 ) ;
                *((UInt32*)cmdArgs) = (UInt32) value;
                status = FVID2_SOK;
            }
            else
            {
                status = FVID2_EUNSUPPORTED_CMD;
            }
            break;
        }

        case IOCTL_ISS_CAPT_GET_FOCUSVALUE:
        {
            if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
                (pObj->inFmt.dataFormat == FVID2_DF_RAW))
            {
                status = IssAlg_capt2AGetFocus(cmdArgs);
            }
            else
            {
                status = FVID2_EUNSUPPORTED_CMD;
            }
            break;
        }

        case IOCTL_ISS_CAPT_GET_H3A_CFG:
        {
            Iss_IspH3aCfg *h3aCfg = (Iss_IspH3aCfg *)cmdArgs;

            if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
                (pObj->inFmt.dataFormat == FVID2_DF_RAW))
            {
                if (NULL != h3aCfg)
                {
                    memcpy(h3aCfg, &pObj->h3aCfg, sizeof(Iss_IspH3aCfg));
                }
                else
                {
                    status = FVID2_EFAIL;
                }
            }
            else
            {
                status = FVID2_EUNSUPPORTED_CMD;
            }

            break;
        }

        case IOCTL_ISS_CAPT_SET_H3A_CFG:
        {
            Iss_IspH3aCfg *h3aCfg = (Iss_IspH3aCfg *)cmdArgs;

            if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
                (pObj->inFmt.dataFormat == FVID2_DF_RAW))
            {
                if (NULL != h3aCfg)
                {
                    status = Issdrv_setH3aConfig(h3aCfg, 0);
                }
                else
                {
                    status = FVID2_EFAIL;
                }
            }
            else
            {
                status = FVID2_EUNSUPPORTED_CMD;
            }

            break;
        }

        case IOCTL_ISS_CAPT_GET_ISIF_PARAMS:
        {
            Iss_IspIsifCfg *isifCfg = (Iss_IspIsifCfg *)cmdArgs;

            if (NULL != isifCfg)
            {
                memcpy(isifCfg, &pObj->isifCfg, sizeof(Iss_IspIsifCfg));
            }
            else
            {
                status = FVID2_EFAIL;
            }

            break;
        }

        case IOCTL_ISS_CAPT_SET_ISIF_PARAMS:
        {
            Iss_IspIsifCfg *isifCfg = (Iss_IspIsifCfg *)cmdArgs;

            if (NULL != isifCfg)
            {
                status = Issdrv_setIsifConfig(isifCfg);
            }
            else
            {
                status = FVID2_EFAIL;
            }

            break;
        }

        case IOCTL_ISS_CAPT_SET_DFS_PARAMS:
        {
            Iss_IspIpipeifDfs *dfsPrm = (Iss_IspIpipeifDfs *)cmdArgs;

            if (NULL != dfsPrm)
            {
                status = Issdrv_setIpipeIfDfsParams(dfsPrm);
            }
            else
            {
                status = FVID2_EFAIL;
            }

            break;
        }

		case IOCTL_ISS_CAPT_SET_OUTDATAFMT:
		{
			Iss_CaptOutDataFormat *pOutDataFmt = (Iss_CaptOutDataFormat*)cmdArgs;
			status = Iss_captSetOutDataFmt(handle, pOutDataFmt);
			break;
		}
		
        default:

            /*
             * illegal IOCTL
             */
            status = 0;
            break;

    }



    return status;

}



/*
 * Starts a drvier instance */
/* ===================================================================
 *  @func     Iss_captStart
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
Int32 Iss_captStart(Fdrv_Handle handle)
{
    Iss_CaptObj *pObj = (Iss_CaptObj *) handle;
    Iss_CaptRtParams *pRtParams = NULL;
    Int32 status = FVID2_EFAIL;
    MSP_IspMemToMemCmdParamT tIspMemToMemCmdParam;
    MSP_IspProcessParamT tIspProcessParam;
    MSP_IspQueryH3aStateT tQueryH3aState;
    //MSP_IspInterruptCfgT tIspInterruptCfg;
    MSP_ERROR_TYPE eResult = MSP_ERROR_NONE;
#ifdef ENABLE_BTE
    UInt32 oriBufOffsetY, oriBufOffsetC;
#endif
    Iss_CaptChObj *pChObj;
    isif_vd_interrupt_t vdIntCfg;

    /*
     * check if global handle, this operation is not supportted
     * for global handles
     */
    if (handle == (Fdrv_Handle) ISS_CAPT_INST_ALL)
        return FVID2_EFAIL;

    GT_assert(GT_DEFAULT_MASK, pObj != NULL);

    /*
     * lock driver instance
     */
    Iss_captLock(pObj);

    /* Get Buffers from Empty Queue to register it to Resizer Driver */
    pChObj = &pObj->chObj[0][0];

    if (VpsUtils_queIsEmpty(&pChObj->emptyQue))
    {
        Vps_printf("\nNo Elements in the emptyQueue\n");
        return FVID2_EFAIL;
    }

    status =
        VpsUtils_queGet(&pChObj->emptyQue, (Ptr *) & PrimaryFrame_RszA, 1,
                        BIOS_NO_WAIT);

    if(pObj->numStream == 2)
    {
        pChObj = &pObj->chObj[1][0];

        if (VpsUtils_queIsEmpty(&pChObj->emptyQue))
        {
            Vps_printf("\n Single Element in the emptyQueue\n");
            VpsUtils_quePut(&pChObj->emptyQue, PrimaryFrame_RszA, BIOS_WAIT_FOREVER);
            return FVID2_EFAIL;
        }

        status =
            VpsUtils_queGet(&pChObj->emptyQue, (Ptr *) & PrimaryFrame_RszB, 1,
                            BIOS_NO_WAIT);
    }

    pRtParams = (Iss_CaptRtParams *)PrimaryFrame_RszA->perFrameCfg;

    /* udpate width x height in run time per frame config */

    pRtParams->captureOutWidth = pObj->createArgs.rszPrms.rPrms[0].outWidth;
    pRtParams->captureOutHeight = pObj->createArgs.rszPrms.rPrms[0].outHeight;
    pRtParams->captureOutPitch = pObj->createArgs.pitch[0];

    if(pObj->numStream == 2)
    {
        pRtParams = (Iss_CaptRtParams *)PrimaryFrame_RszB->perFrameCfg;

        /* udpate width x height in run time per frame config */

        pRtParams->captureOutWidth = pObj->createArgs.rszPrms.rPrms[1].outWidth;
        pRtParams->captureOutHeight = pObj->createArgs.rszPrms.rPrms[1].outHeight;
        pRtParams->captureOutPitch = pObj->createArgs.pitch[1];
    }

#ifdef ENABLE_BTE
    gIss_captCommonObj.handleBTE.bte_config[0].context_start_addr = 0;
    if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[0].dataFormat) == 0)
        gIss_captCommonObj.handleBTE.bte_config[1].context_start_addr =
            (DIV_UP(pObj->createArgs.rszPrms.rPrms[0].outWidth, 128) << CSL_BTE_BTE_CONTEXT_START_0_X_SHIFT);

    configureBteContext(&gIss_captCommonObj.handleBTE, 0,
        pObj->createArgs.rszPrms.rPrms[0].outWidth, pObj->createArgs.rszPrms.rPrms[0].outHeight,
        pObj->createArgs.outStreamInfo[0].dataFormat, curStreamBuf[0].mirrorMode);

    oriBufOffsetY = Utils_tilerGetOriAddr((Int32)PrimaryFrame_RszA->addr[1][0], 0, curStreamBuf[0].mirrorMode,
        pObj->createArgs.rszPrms.rPrms[0].outWidth, pObj->createArgs.rszPrms.rPrms[0].outHeight);
    startBteContext(&gIss_captCommonObj.handleBTE.bte_config[0], oriBufOffsetY);

    if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[0].dataFormat) == 0)
    {
        oriBufOffsetC = Utils_tilerGetOriAddr((Int32)PrimaryFrame_RszA->addr[1][1], 1, curStreamBuf[0].mirrorMode,
            pObj->createArgs.rszPrms.rPrms[0].outWidth, pObj->createArgs.rszPrms.rPrms[0].outHeight/2);
        startBteContext(&gIss_captCommonObj.handleBTE.bte_config[1], oriBufOffsetC);
    }
#endif

    if (pObj->state == ISS_CAPT_STATE_CREATED ||
        pObj->state == ISS_CAPT_STATE_STOPPED)
    {

        gIss_captCommonObj.pIssConfig->eFH3aValidity = MSP_PROC_H3A_VALID_NO;
        gIss_captCommonObj.pModuleInstance->tConfigParams.ptIssConfig = gIss_captCommonObj.pIssConfig;
        EXPECT_EVENT(gIss_captCommonObj.pModuleInstance, VIDEO_ISR_EVENT_RSZ);

#ifndef ENABLE_BTE
        tIspMemToMemCmdParam.pOutbuffRszA = PrimaryFrame_RszA->addr[1][0];
        tIspMemToMemCmdParam.pOutbuffRszACr = PrimaryFrame_RszA->addr[1][1];
#else
        tIspMemToMemCmdParam.pOutbuffRszA = (void *)(gIss_captCommonObj.handleBTE.baseAddress +
            gIss_captCommonObj.handleBTE.bte_config[0].context_start_addr);

        tIspMemToMemCmdParam.pOutbuffRszACr = (void *)(gIss_captCommonObj.handleBTE.baseAddress +
            gIss_captCommonObj.handleBTE.bte_config[1].context_start_addr);
#endif
        curStreamBuf[0].ptmpFrame = PrimaryFrame_RszA;

        if(pObj->numStream == 2)
        {
            tIspMemToMemCmdParam.pOutbuffRszB = PrimaryFrame_RszB->addr[1][0];
            tIspMemToMemCmdParam.pOutbuffRszBCr = PrimaryFrame_RszB->addr[1][1];
            curStreamBuf[1].ptmpFrame = PrimaryFrame_RszB;
        }
        else if(pObj->numStream == 1)
        {
            tIspMemToMemCmdParam.pOutbuffRszB = NULL;
            tIspMemToMemCmdParam.pOutbuffRszBCr = NULL;
            curStreamBuf[1].ptmpFrame = NULL;
        }
        if(pObj->glbcEnable == 1)
            ipipe_config_boxcar_addr((UInt32)PrimaryFrame_RszA->blankData);

        if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
            (pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW))
        {
            tIspMemToMemCmdParam.eInFormat = MSP_IPIPE_IN_FORMAT_BAYER;
        }
        else
        {
            tIspMemToMemCmdParam.eInFormat = MSP_IPIPE_IN_FORMAT_YUV422;
        }

        tIspMemToMemCmdParam.pOutRawBuffer = (void *) NULL;

        vdIntCfg.vd0_intr_timing_lineno =
            pObj->inFmt.height - ISS_VDINT_NUM_LINES_LESS;
        vdIntCfg.vd1_intr_timing_lineno =
            pObj->inFmt.height - ISS_VDINT_NUM_LINES_LESS;
        vdIntCfg.vd2_intr_timing_lineno =
            pObj->inFmt.height - ISS_VDINT_NUM_LINES_LESS;
        isif_config_vd_line_numbers(&vdIntCfg);

        Iem_enableInt(gIss_captCommonObj.intHandle[ISS_CAPT_INT_VD_INT0]);
        Iem_enableInt(gIss_captCommonObj.intHandle[ISS_CAPT_INT_RSZ]);
        Iem_enableInt(gIss_captCommonObj.intHandle[ISS_CAPT_INT_H3A]);
        Iem_enableInt(gIss_captCommonObj.intHandle[ISS_CAPT_INT_BSC]);

        tIspMemToMemCmdParam.pOutRawBuffer = NULL;
        tIspProcessParam.eCmd = MSP_ISP_CMD_MEM_TO_MEM_START;
        tIspProcessParam.pCmdData = &tIspMemToMemCmdParam;
        if(pObj->createArgs.captureMode == ISS_CAPT_INMODE_ISIF)
        {
            eResult =
                MSP_ISP_process(
                    gIss_captCommonObj.pModuleInstance->hIspHandle,
                    &tIspProcessParam,
                    NULL);

            if (MSP_ERROR_NONE == eResult)
            {
                /* TODO TIMM_OSAL_SemaphoreObtain(pModuleInstance->pIssSem,
                 TIMM_OSAL_SUSPEND); */
            }
        }
        else
        {
            /* ISS_CAPT_INMODE_DDR mode */
            isif_config_sdram_address((UInt32)PrimaryFrame_RszA->addr[1][0]);
            isif_start(ISIF_START_WRITE);
        }

        if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
            (pObj->inFmt.dataFormat == FVID2_DF_RAW))
        {
            tQueryH3aState.eH3aState = MSP_H3A_STOPPED;

            issDrvH3aStart();

            MSP_ISP_query(gIss_captCommonObj.pModuleInstance->hIspHandle,
                          (MSP_INDEXTYPE) MSP_ISP_QUERY_H3A_STATE, &tQueryH3aState);

            if (tQueryH3aState.eH3aState & (MSP_H3A_AEWB_ENABLED))
            {
                EXPECT_EVENT(gIss_captCommonObj.pModuleInstance, VIDEO_ISR_EVENT_H3A);

                /*tIspInterruptCfg.eInterruptId = MSP_ISS_DRV_H3A_END_ISR;
                tIspInterruptCfg.bEnable = MSP_TRUE;

                MSP_ISP_config(gIss_captCommonObj.pModuleInstance->hIspHandle,
                               (MSP_INDEXTYPE) MSP_ISP_CFG_INT,
                               (MSP_PTR) & tIspInterruptCfg);*/
            }
        }
    }
	
#ifdef USE_MIPI_MODE
	isif_reg->CGAMMAWD = 0x7708;
	iss_regs->ISS_CTRL &= ~0x0000000C;
	csi2A_regs->CSI2_CTRL |= 0x00000800;
	isp_regs ->ISP5_CTRL |= 0x00C00000;
#endif
	
#if defined(IMGS_OMNIVISION_OV10630) || defined(IMGS_OMNIVISION_OV2710)
    isif_reg->SYNCEN = 0x3;
#endif

    pObj->state = ISS_CAPT_STATE_RUNNING;
    /*
     * unlock driver instance
     */

    Iss_captUnlock(pObj);

    return status;
}



/*
 * Stops a driver instance */
/* ===================================================================
 *  @func     Iss_captStop
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

Int32 Iss_captStop(Fdrv_Handle handle)

{
    UInt32 cnt, numFrames = 0;
    FVID2_Frame *pFrame = NULL;
    Iss_CaptObj *pObj = (Iss_CaptObj *) handle;
    Int32 status = FVID2_EFAIL;

    /* check if global handle, this operation is not supportted
       for global handles */

    if (handle == (Fdrv_Handle) ISS_CAPT_INST_ALL)
        return FVID2_SOK;

    GT_assert(GT_DEFAULT_MASK, pObj != NULL);

    /* lock driver instance */
    Iss_captLock(pObj);

    if (pObj->state == ISS_CAPT_STATE_RUNNING)
    {
        pObj->state = ISS_CAPT_STATE_DO_STOP;

        Semaphore_pend(pObj->semStopDone, BIOS_WAIT_FOREVER);

        Iem_disableInt(gIss_captCommonObj.intHandle[ISS_CAPT_INT_VD_INT0]);
        Iem_disableInt(gIss_captCommonObj.intHandle[ISS_CAPT_INT_RSZ]);
        Iem_disableInt(gIss_captCommonObj.intHandle[ISS_CAPT_INT_H3A]);
        Iem_disableInt(gIss_captCommonObj.intHandle[ISS_CAPT_INT_BSC]);

        issDrvH3aStop();

        rsz_submodule_start(RSZ_STOP, RESIZER_A);

        if (pObj->numStream == 2)
            rsz_submodule_start(RSZ_STOP, RESIZER_B);
        rsz_start(RSZ_STOP);

        ipipe_start(IPIPE_STOP);
        isif_start(ISIF_STOP);
        ipipeif_start(IPIPEIF_STOP);

        pObj->state = ISS_CAPT_STATE_STOPPED;
    }

    /* Move All frames from tmpQ, fullQ, curStreamBuf to emptyQue */
    for (cnt = 0; cnt < pObj->numStream; cnt ++)
    {
        numFrames = 0;
        while (TRUE)
        {
            status = VpsUtils_queGet(&pObj->fullQue[cnt],
                                     (Ptr *) &pFrame,
                                     1,
                                     BIOS_NO_WAIT);
            if (FVID2_SOK != status || pFrame == NULL)
                break;

            status = VpsUtils_quePut(&pObj->chObj[cnt][0].emptyQue,
                                     pFrame,
                                     BIOS_WAIT_FOREVER);
            numFrames ++;
        }

        numFrames = 0;
        while (TRUE)
        {
            status = VpsUtils_queGet(&pObj->chObj[cnt][0].tmpQue,
                                     (Ptr *) &pFrame,
                                     1,
                                     BIOS_NO_WAIT);
            if (FVID2_SOK != status || pFrame == NULL)
                break;

            status = VpsUtils_quePut(&pObj->chObj[cnt][0].emptyQue,
                                     pFrame,
                                     BIOS_WAIT_FOREVER);
            numFrames ++;
        }

        if (curStreamBuf[cnt].ptmpFrame != NULL)
        {
            status = VpsUtils_quePut(&pObj->chObj[cnt][0].emptyQue,
                                     curStreamBuf[cnt].ptmpFrame,
                                     BIOS_WAIT_FOREVER);
        }
    }

    /* unlock driver instance */
    Iss_captUnlock(pObj);

    return FVID2_SOK;
}



/* ===================================================================
 *  @func     Iss_captFreePath
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
Int32 Iss_captFreePath(Iss_CaptObj * pObj)
{
    /*
     * free VP resources used in this driver path
     */
    return 0;
}



/* ===================================================================
 *  @func     Iss_captAllocPath
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
Int32 Iss_captAllocPath(Iss_CaptObj * pObj)
{
    Int32 status = FVID2_SOK;

    return status;
}



/*
 * Create and init channel specific driver objects */
/* ===================================================================
 *  @func     Iss_captCreateChObj
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
Int32 Iss_captCreateChObj(Iss_CaptObj * pObj)
{
    UInt16 streamId, chId;
    Iss_CaptChObj *pChObj;
    Int32 status;

    pObj->resetStatistics = TRUE;
    pObj->addMultiChDummyDesc = FALSE;

    /*
     * get channel properties like num channels,
     * in width x in height, scan format - interlaced or progressive
     */
    pObj->numCh = pObj->createArgs.numCh;

    status = Iss_captAllocPath(pObj);
    if (status != FVID2_SOK)
        return status;

    /*
     * for every stream and ever channel do ..
     */
    for (streamId = 0; streamId < pObj->numStream; streamId++)
    {
        for (chId = 0; chId < pObj->numCh; chId++)
        {
            /*
             * get channel object
             */
            pChObj = &pObj->chObj[streamId][chId];

            /*
             * mark VPDMA received channels to 0
             */
            pChObj->vChannelRecv = 0;

            /*
             * mark memory type as non-tiled by default
             */
            pChObj->memType = ISS_NONTILEDMEM;

            /*
             * make driver channel number from instance ID, stream ID,
             * thus given driver channel number we can know which
             * instance, stream, channel it belongs to
             * chID
             */
            pChObj->lChannelNum =
                Iss_captMakeChannelNum(pObj->instanceId, streamId, chId);

            /*
             * copy user channel number to FVID2 Format structure
             */
            pChObj->channelNum = pObj->createArgs.channelNumMap[streamId][chId];

            /*
             * make user channel number to driver channel number mapping
             */
            gIss_captCommonObj.
                fvidChannelNum2lChannelNumMap[pChObj->channelNum] =
                pChObj->lChannelNum;

            /*
             * fill FVID2 Format with data format
             */
            pChObj->dataFormat =
                pObj->createArgs.outStreamInfo[streamId].dataFormat;

            /*
             * set memory type for data format's which could support tiled mode
             */
            if (pChObj->dataFormat == FVID2_DF_YUV420SP_UV
                || pChObj->dataFormat == FVID2_DF_YUV422SP_UV)
            {
                pChObj->memType
                    = pObj->createArgs.outStreamInfo[streamId].memType;
            }

            pChObj->memType = pObj->createArgs.outStreamInfo[streamId].memType;

            curStreamBuf[streamId].memType = pChObj->memType;

            /* invert FID for YUV420 and in discrete sync mode */
            pChObj->invertFid = FALSE;

            pChObj->maxOutHeight
                = pObj->createArgs.outStreamInfo[streamId].maxOutHeight;

            /* unlimited output width */
            pChObj->maxOutWidth = 0;

            /* fill pitch based on user supplied value */
            pChObj->pitch[0]
                = pObj->createArgs.outStreamInfo[streamId].pitch[0];

			pChObj->dropFrameCount = 0;
			pChObj->captureFrameCount = 0;
			pChObj->skipFrameCount = 0;
        }
    }

    return FVID2_SOK;
}



/*
 * Create queues for all channels and streams
 *
 * For each channel and each stream there is one input or free or empty
 * buffer queue
 *
 * Each stream there is one output or full or completed buffer queue
 *
 * Queue depth for empty buffer queue is ISS_CAPT_FRAME_QUE_LEN_PER_CH_MAX
 *
 * Queue depth for full buffer is ISS_CAPT_FRAME_QUE_LEN_PER_CH_MAX*numCh
 *
 * Inaddition to this per channel and stream there is tmp que. This que is
 * for keeping track of buffer submmited to the CLM, i.e buffers which are in
 * the process of getting captured
 *
 * In case or errors in creation, previously created queues are deleted and
 * error is returned */
/* ===================================================================
 *  @func     Iss_captCreateQueues
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
Int32 Iss_captCreateQueues(Iss_CaptObj * pObj)
{
    UInt16 chId, maxElements;
    Int16 streamId;
    Iss_CaptChObj *pChObj;
    Int32 status;

    /*
     * create full que
     */
    for (streamId = 0; streamId < pObj->numStream; streamId++)
    {
        maxElements = pObj->numCh * ISS_CAPT_FRAME_QUE_LEN_PER_CH_MAX;

        status = VpsUtils_queCreate(&pObj->fullQue[streamId],
                                    maxElements,
                                    &pObj->fullQueMem[streamId][0],
                                    VPSUTILS_QUE_FLAG_BLOCK_QUE);
        if (status != FVID2_SOK)
        {
            for (streamId--; streamId >= 0; streamId--)
            {
                status = VpsUtils_queDelete(&pObj->fullQue[streamId]);
                GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
            }

            return FVID2_EFAIL;
        }
    }

    /*
     * create empty que and tmp que
     */
    for (streamId = 0; streamId < pObj->numStream; streamId++)
    {
        for (chId = 0; chId < pObj->numCh; chId++)
        {
            pChObj = &pObj->chObj[streamId][chId];
            maxElements = ISS_CAPT_FRAME_QUE_LEN_PER_CH_MAX;
            status = VpsUtils_queCreate(&pChObj->emptyQue,
                                        maxElements,
                                        pChObj->emptyQueMem,
                                        VPSUTILS_QUE_FLAG_NO_BLOCK_QUE);

            GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

            status =
                VpsUtils_queCreate(&pChObj->tmpQue, maxElements,
                                   pChObj->tmpQueMem,
                                   VPSUTILS_QUE_FLAG_NO_BLOCK_QUE);

            GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
        }
    }

    return FVID2_SOK;
}



/*
 * Delete previously created queues */
/* ===================================================================
 *  @func     Iss_captDeleteQueues
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
Int32 Iss_captDeleteQueues(Iss_CaptObj * pObj)
{
    UInt16 streamId, chId;
    Iss_CaptChObj *pChObj;
    Int32 status;

    for (streamId = 0; streamId < pObj->numStream; streamId++)
    {
        status = VpsUtils_queDelete(&pObj->fullQue[streamId]);

        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        for (chId = 0; chId < pObj->numCh; chId++)
        {
            pChObj = &pObj->chObj[streamId][chId];

            status = VpsUtils_queDelete(&pChObj->emptyQue);
            GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

            status = VpsUtils_queDelete(&pChObj->tmpQue);
            GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
        }
    }

    return FVID2_SOK;
}



/*
 * Check if data is available in any of the 'full' queues */
/* ===================================================================
 *  @func     Iss_captIsDataAvailable
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
UInt32 Iss_captIsDataAvailable(Iss_CaptObj * pObj)
{
    UInt32 isDataAvailable = FALSE;
    UInt16 streamId;

    /*
     * check is done for all streams
     */
    for (streamId = 0; streamId < pObj->numStream; streamId++)
    {
        if (!VpsUtils_queIsEmpty(&pObj->fullQue[streamId]))
        {
            isDataAvailable = TRUE;
            break;
        }
    }

    return isDataAvailable;
}



/* ===================================================================
 *  @func     Iss_capt2AUpdate
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
Int32 Iss_capt2AUpdate(Iss_CaptObj * pObj)
{
    Int32 status = FVID2_SOK;
    Int32 dGain;
    int rGain;
    int grGain;
    int gbGain;
    int bGain;
    Int32 aewbVendor;
    isif_gain_offset_cfg_t isifgain;

    aewbVendor = IssAlg_capt2AGetAEWBVendor();
    if (aewbVendor == AEWB_ID_NONE)
    {
        return status;
    }

    Iss_captLock(pObj);

    isifgain.gain_offset_featureflag =
        ISIF_H3A_WHITE_BALANCE_FLAG | ISIF_H3A_OFFSET_CTRL_FLAG |
        ISIF_IPIPE_WHITE_BALANCE_FLAG | ISIF_IPIPE_OFFSET_CTRL_FLAG |
        ISIF_SDRAM_WHITE_BALANCE_FLAG | ISIF_SDRAM_OFFSET_CTRL_FLAG;

    if (ti2a_output_params.mask)
    {
        rGain = ti2a_output_params.ipipe_awb_gain.rGain * 4;
        grGain = ti2a_output_params.ipipe_awb_gain.grGain * 4;
        gbGain = ti2a_output_params.ipipe_awb_gain.gbGain * 4;
        bGain = ti2a_output_params.ipipe_awb_gain.bGain * 4;
        dGain = ti2a_output_params.ipipe_awb_gain.dGain * 2;
        ipipe_reg->WB2_OFT_R = 0;
        ipipe_reg->WB2_OFT_GR = 0;
        ipipe_reg->WB2_OFT_GB = 0;
        ipipe_reg->WB2_OFT_B = 0;
        ipipe_reg->WB2_WGN_R = rGain;
        ipipe_reg->WB2_WGN_GR = grGain;
        ipipe_reg->WB2_WGN_GB = gbGain;
        ipipe_reg->WB2_WGN_B = bGain;

        isifgain.gain_r = isifgain.gain_gr =
            isifgain.gain_gb = isifgain.gain_bg = dGain;
        isifgain.offset = 0;

        isif_config_gain_offset(&isifgain);
        //isif_reg->CGAMMAWD |= 0x7700;
    }

    Iss_captUnlock(pObj);

    return status;
}



/* ===================================================================
 *  @func     Iss_captSetFrameRate
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
Int32 Iss_captSetFrameRate(Iss_CaptObj * pObj)
{
    Int32 status = FVID2_SOK;

    return status;
}



/*
 * Set Itt Capture For RAW and YUV Frames
 *
 * RAW and YUV Capture will happen and store in the Buffer. */
/* ===================================================================
 *  @func     Iss_captITTControl
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
Int32 Iss_captITTControl(Iss_CaptObj * pObj, Iss_IttParams * ittParams)
{
    Int32 status = FVID2_EFAIL;

    /*
     * parameter checfing, not supported for global handle
     */
    if (pObj == (Iss_CaptObj *) ISS_CAPT_INST_ALL ||
        pObj == NULL ||
        ittParams == NULL)
    {
        return FVID2_EFAIL;
    }

    if (pObj->state != ISS_CAPT_STATE_IDLE)
    {
        if (ittParams->ittCommand == 4)
        {
            status = IssCaptureRawFrame(pObj, ittParams);
        }
        else if (ittParams->ittCommand == 9)
        {
            status = IssCaptureYUVFrame(pObj, ittParams);
        }
    }

    return status;
}



/* ===================================================================
 *  @func     IssCaptureRawFrame
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
Int32 IssCaptureRawFrame(Iss_CaptObj * pObj, Iss_IttParams * ittParams)
{
    Int32 status = FVID2_SOK;

    gIss_captCommonObj.gIttParams = ittParams;
    Vps_rprintf("\nRAW capture started to address %x", ittParams->isif_dump);
    if (ittParams->isif_dump != NULL)
    {
        gIss_captCommonObj.gIttParams->Raw_Data_Ready = ittParams->Raw_Data_Ready;
        issTriggerRawCapture((Int32 *) ittParams->isif_dump, ISIF_START_WRITE);
        gRawCaptureTriggerd = 1;
    }
    else
    {
        status = FVID2_EFAIL;
    }

    return status;
}



/* ===================================================================
 *  @func     IssCaptureYUVFrame
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
Int32 IssCaptureYUVFrame(Iss_CaptObj * pObj, Iss_IttParams * ittParams)
{
    Int32 status = FVID2_SOK;

    gIss_captCommonObj.gIttParams = ittParams;
    Vps_printf("\nYUV captured to address %x", ittParams->isif_dump);
    gIss_captCommonObj.YUVcaptureAddr = (Int32 *) ittParams->isif_dump;

    gYUVCaptureTriggerd = 1;

    return status;
}



/*
 * Set frame skip mask
 *
 * Frame skip itself will happen from next list processing onwards
 *
 * This functionc can get called while the driver is running. i.e run time
 * changing of frame skip mask is supported */
/* ===================================================================
 *  @func     Iss_captSetFrameSkip
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
Int32 Iss_captSetFrameSkip(Iss_CaptObj * pObj, Iss_CaptFrameSkip * frameSkip)
{
    UInt16 lChannelNum, instId, streamId, chId;
    Int32 status = FVID2_EFAIL;

    /*
     * parameter checfing, not supported for global handle
     */
    if (pObj == (Iss_CaptObj *) ISS_CAPT_INST_ALL ||
        pObj == NULL ||
        frameSkip == NULL)
    {
        return FVID2_EFAIL;
    }

    /*
     * lock driver instance
     */
    Iss_captLock(pObj);

    if (pObj->state != ISS_CAPT_STATE_IDLE)
    {
        /*
         * driver needs to atleast be in open state for this call to work
         */

        /*
         * map from user channel number of driver channel number
         */
        lChannelNum = gIss_captCommonObj.fvidChannelNum2lChannelNumMap
            [frameSkip->channelNum];

        instId = Iss_captGetInstId(lChannelNum);
        streamId = Iss_captGetStreamId(lChannelNum);
        chId = Iss_captGetChId(lChannelNum);

        if (instId < ISS_CAPT_INST_MAX
            && streamId < pObj->numStream
            && chId < pObj->numCh && instId == pObj->instanceId)
        {
            pObj->chObj[streamId][chId].frameSkipMask =
                frameSkip->frameSkipMask[streamId];

            pObj->chObj[streamId][chId].frameSkipMaskHigh =
                frameSkip->frameSkipMaskHigh[streamId];

            status = FVID2_SOK;
        }
    }

    /*
     * unlock driver instance
     */
    Iss_captUnlock(pObj);

    return status;
}



/* ===================================================================
 *  @func     Iss_captSetMirrorMode
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
Int32 Iss_captSetMirrorMode(Iss_CaptObj * pObj, Int32 * mirrorMode)
{
    Int32 status = FVID2_SOK;

    curStreamBuf[0].mirrorMode = *mirrorMode;
    curStreamBuf[1].mirrorMode = *mirrorMode;

    switch (*mirrorMode)
    {
        case 1:
            curStreamBuf[0].flipH = TRUE;
            curStreamBuf[0].flipV = FALSE;
            curStreamBuf[1].flipH = TRUE;
            curStreamBuf[1].flipV = FALSE;
            break;
        case 2:
            curStreamBuf[0].flipH = FALSE;
            curStreamBuf[0].flipV = TRUE;
            curStreamBuf[1].flipH = FALSE;
            curStreamBuf[1].flipV = TRUE;
            break;
        case 3:
            curStreamBuf[0].flipH = TRUE;
            curStreamBuf[0].flipV = TRUE;
            curStreamBuf[1].flipH = TRUE;
            curStreamBuf[1].flipV = TRUE;
            break;
        case 0:
        default:
            curStreamBuf[0].flipH = FALSE;
            curStreamBuf[0].flipV = FALSE;
            curStreamBuf[1].flipH = FALSE;
            curStreamBuf[1].flipV = FALSE;
            break;
    }

    gMirrorMode = 1;

    return status;
}



/*
 * Set Resolution
 *
 * Frame skip itself will happen from next list processing onwards
 *
 * This functionc can get called while the driver is running. i.e run time
 * changing of frame skip mask is supported */
/* ===================================================================
 *  @func     Iss_captSetResolution
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
Int32 Iss_captSetInResolution(Iss_CaptObj * pObj,
                              Iss_CaptResParams *resolutionParams)
{
    Int32 status = FVID2_SOK;
    UInt32 cnt;
    MSP_IspConfigSizesT *pConfigSizes;
    MSP_U32 eMSP = MSP_ERROR_NONE;
    MSP_IspParamCfgT tIspParamCfg;
    isif_vd_interrupt_t vdIntCfg;

    /*
     * parameter checfing, not supported for global handle
     */
    if (pObj == (Iss_CaptObj *) ISS_CAPT_INST_ALL ||
        pObj == NULL ||
        resolutionParams == NULL ||
        ISS_CAPT_STATE_RUNNING == pObj->state)
    {
        return FVID2_EFAIL;
    }

    /*
     * lock driver instance
     */
    Iss_captLock(pObj);

    pConfigSizes = NULL;

    /* Do Error check */
    pObj->inFmt.width = resolutionParams->scParams[0].inWidth;
    pObj->inFmt.height = resolutionParams->scParams[0].inHeight;
    pObj->createArgs.inFmt.width = pObj->inFmt.width;
    pObj->createArgs.inFmt.height = pObj->inFmt.height;
    pObj->createArgs.rszPrms.cropPrms.cropWidth = resolutionParams->scParams[0].inWidth;
    pObj->createArgs.rszPrms.cropPrms.cropHeight = resolutionParams->scParams[0].inHeight;

    for (cnt = 0u; cnt < resolutionParams->numStreams; cnt ++)
    {
        pObj->createArgs.rszPrms.rPrms[cnt].outWidth = resolutionParams->scParams[cnt].outWidth;
        pObj->createArgs.rszPrms.rPrms[cnt].outHeight = resolutionParams->scParams[cnt].outHeight;
    }

    pConfigSizes = (MSP_PTR) malloc(sizeof(MSP_IspConfigSizesT));
    if (pConfigSizes == NULL)
    {
        Iss_captUnlock(pObj);
        return FVID2_EALLOC;
    }

    vdIntCfg.vd0_intr_timing_lineno =
        pObj->inFmt.width - ISS_VDINT_NUM_LINES_LESS;
    vdIntCfg.vd1_intr_timing_lineno =
        pObj->inFmt.width - ISS_VDINT_NUM_LINES_LESS;
    vdIntCfg.vd2_intr_timing_lineno =
        pObj->inFmt.width - ISS_VDINT_NUM_LINES_LESS;
    isif_config_vd_line_numbers(&vdIntCfg);

    *(MSP_U32 *) (0x5505002C) |= 1;                        // isp IRQ set

    pConfigSizes->nInSizeX = pObj->inFmt.width;
    pConfigSizes->nInSizeY = pObj->inFmt.height;
    pConfigSizes->nInSizePpln = pObj->createArgs.pitch[0];
    pConfigSizes->nInStartX = pObj->createArgs.rszPrms.cropPrms.cropStartX;
    pConfigSizes->nInStartY = pObj->createArgs.rszPrms.cropPrms.cropStartY;

    if(pObj->vsEnable == 0)
    {
        pConfigSizes->nOutSizeAX =
            pObj->createArgs.rszPrms.rPrms[0].outWidth;
        pConfigSizes->nOutSizeAY =
        pObj->createArgs.rszPrms.rPrms[0].outHeight;
    }
    else
    {
        pConfigSizes->nOutSizeAX =
            (pObj->createArgs.rszPrms.rPrms[0].outWidth *
                VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
        pConfigSizes->nOutSizeAY =
            (pObj->createArgs.rszPrms.rPrms[0].outHeight *
                VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;;
    }

    if((pConfigSizes->nOutSizeAX > pConfigSizes->nInSizeX) ||
       (pConfigSizes->nOutSizeAY > pConfigSizes->nInSizeY))
    {
        Iss_captUnlock(pObj);
        free(pConfigSizes);
        return FVID2_EFAIL;
    }

    pConfigSizes->nOutSizeABpln = pObj->createArgs.pitch[0];
    pConfigSizes->nOutSizeABplnC = 0;

    if (pObj->numStream == 2)
    {
        if((pObj->vsEnable == 0) || (pObj->vsDemoEnable == 1))
        {
            pConfigSizes->nOutSizeBX =
                pObj->createArgs.rszPrms.rPrms[1].outWidth;
            pConfigSizes->nOutSizeBY =
                pObj->createArgs.rszPrms.rPrms[1].outHeight;
        }
        else
        {
            pConfigSizes->nOutSizeBX =
                (pObj->createArgs.rszPrms.rPrms[1].outWidth *
                    VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
            pConfigSizes->nOutSizeBY =
                (pObj->createArgs.rszPrms.rPrms[1].outHeight *
                    VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
        }

        if((pConfigSizes->nOutSizeBX > pConfigSizes->nInSizeX) ||
           (pConfigSizes->nOutSizeBY > pConfigSizes->nInSizeY))
        {
            Iss_captUnlock(pObj);
            free(pConfigSizes);
            return FVID2_EFAIL;
        }

        pConfigSizes->nOutSizeBBpln = pObj->createArgs.pitch[1];
        pConfigSizes->nOutSizeBBplnC = 0;
    }
    else
    {
        pConfigSizes->nOutSizeBX = 720;
        pConfigSizes->nOutSizeBY = 480;
        pConfigSizes->nOutSizeBBpln = 720;
    }

    pConfigSizes->nCropX = pObj->createArgs.rszPrms.cropPrms.cropStartX;
    pConfigSizes->nCropY = pObj->createArgs.rszPrms.cropPrms.cropStartY;

    pConfigSizes->nClockPercents = MSP_ISS_SPEED_MAX;
    eMSP =
        MSP_ISP_query(gIss_captCommonObj.pModuleInstance->hIspHandle,
                      (MSP_INDEXTYPE) MSP_ISP_QUERY_MAX_WIDTH, pConfigSizes);
    eMSP =
        MSP_ISP_query(gIss_captCommonObj.pModuleInstance->hIspHandle,
                      (MSP_INDEXTYPE) MSP_ISP_QUERY_CROP_SIZES, pConfigSizes);

    if (eMSP != MSP_ERROR_NONE)
    {
        // Vps_printf("Error MSP_ISP_query failed!!!\n");
    }

    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nInSizeX =
        //gIss_captCommonObj.pModuleInstance->nIssInX;
        pObj->inFmt.width;
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nInSizeY =
        //gIss_captCommonObj.pModuleInstance->nIssInY;
        pObj->inFmt.height;

    gIss_captCommonObj.pModuleInstance->tConfigParams.ptIssConfig->eRszIpPortSel =
        MSP_RSZ_IP_IPIPEIF;

    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeAX =
        pConfigSizes->nOutSizeAX; //pObj->createArgs.scParams[0].outWidth;
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeAY =
        pConfigSizes->nOutSizeAY; //pObj->createArgs.scParams[0].outHeight;
#ifndef ENABLE_BTE
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeABpln =
        pObj->createArgs.pitch[0];
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeABplnC =
        pObj->createArgs.pitch[0];
#else
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeABpln =
        BTE_PITCH;
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeABplnC =
        BTE_PITCH;
#endif

    if (pObj->numStream == 2)
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBX =
            pConfigSizes->nOutSizeBX;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBY =
            pConfigSizes->nOutSizeBY;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBBpln =
            pObj->createArgs.pitch[1];
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBBplnC =
            pObj->createArgs.pitch[1];
    }
    else
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBX = 720;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBY = 480;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBBpln = 720;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBBplnC = 720;
    }

    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nClockPercents =
        MSP_ISS_SPEED_MAX;

    tIspParamCfg.ptIssParams = &gIss_captCommonObj.pModuleInstance->tConfigParams;

    // RAJAT - need to correct the configuration being sent out to remove the
    // error
    MSP_ISP_config(gIss_captCommonObj.pModuleInstance->hIspHandle,
                   (MSP_INDEXTYPE) MSP_ISP_CFG_PROC_MEM_TO_MEM, &tIspParamCfg);
    Vps_rprintf("\n%s:%d\n", __func__, __LINE__);

    if (pObj->createArgs.videoCaptureMode == ISS_CAPT_INMODE_ISIF)
    {
        if(pObj->numStream == 1)
        {
            status = Iss_Disable_RszB();

            if(status == FVID2_EFAIL)
                Vps_rprintf("Iss_Disable_RszB Failed \n");
        }

        /* Resizer should be configured in ISIF mode only */
        Issdrv_captSetRszCfg(pObj, &pObj->createArgs.rszPrms);
    }

    gIss_captCommonObj.pModuleInstance->nLscTableIsValid = MSP_TRUE;
    ipipe_reg->SRC_VSZ = pObj->inFmt.height;
    ipipe_reg->SRC_HSZ = pObj->inFmt.width;

    IssCdrv_setIpipeCfg();

    Issdrv_setIsifConfig(&pObj->isifCfg);

    if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
        (pObj->inFmt.dataFormat == FVID2_DF_RAW))
    {
        /* Histogram Config */
        {
            MSP_IpipeHistCfgT histogramCfg;
            MSP_IpipeHistDimT histogramDim[4];
            MSP_U8            gainTbl[4];

            memcpy(&histogramCfg,gIss_captCommonObj.pIssConfig->ptHistogram,sizeof(MSP_IpipeHistCfgT));

            histogramCfg.nOst  = 0;     // Free Run
            histogramCfg.nSel  = 1;         // Input Selection - 0. From noise filter 1.From RGBtoYUV
            histogramCfg.nType = 2;     // G selection in Bayer mode - (Gb + Gr)/2

            histogramDim[0].nVPos  = 0;
            histogramDim[0].nVSize = pObj->inFmt.height;
            histogramDim[0].nHPos  = 0;
            histogramDim[0].nHSize = pObj->inFmt.width;

            gainTbl[0] = 6;             // R
            gainTbl[1] = 6;             // Gr
            gainTbl[2] = 6;             // Gb
            gainTbl[3] = 6;             // B

            histogramCfg.ptHistDim = histogramDim;
            histogramCfg.pnGainTbl = gainTbl;

            MSP_ISP_config(gIss_captCommonObj.pModuleInstance->hIspHandle,(MSP_INDEXTYPE)MSP_ISP_CFG_HIST,&histogramCfg);
        }
        if(pObj->glbcEnable == 1)
        {
            MSP_IpipeBoxcarCfgT BoxCarCfg;
            memcpy(&BoxCarCfg,gIss_captCommonObj.pIssConfig->ptBoxcar,sizeof(MSP_IpipeBoxcarCfgT));

            BoxCarCfg.nEnable = TRUE;
            if(pObj->createArgs.captureMode == ISS_CAPT_INMODE_ISIF)
            {
                BoxCarCfg.nOst = 0;
            }
            else
            {
                BoxCarCfg.nOst = 1;
            }
                BoxCarCfg.nBoxSize = 1;
                BoxCarCfg.nShift = 3;
                BoxCarCfg.pAddr = NULL;

            MSP_ISP_config(
                gIss_captCommonObj.pModuleInstance->hIspHandle,
                (MSP_INDEXTYPE)MSP_ISP_CFG_BOXCAR,
                &BoxCarCfg);
        }
    }

    if (pConfigSizes != NULL)
    {
        free(pConfigSizes);
    }

    /*
     * unlock driver instance
     */
    Iss_captUnlock(pObj);

    return status;
}

/*
 * Set Resolution
 *
 * Frame skip itself will happen from next list processing onwards
 *
 * This functionc can get called while the driver is running. i.e run time
 * changing of frame skip mask is supported */
/* ===================================================================
 *  @func     Iss_captSetResolution
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
Int32 Iss_captSetResolution(Iss_CaptObj * pObj,
                            Iss_CaptResolution * resolutionParams)
{
    Int32 status = FVID2_SOK;

    /*
     * parameter checfing, not supported for global handle
     */
    if (pObj == (Iss_CaptObj *) ISS_CAPT_INST_ALL ||
        pObj == NULL ||
        resolutionParams == NULL)
    {
        return FVID2_EFAIL;
    }

    /*
     * lock driver instance
     */
    Iss_captLock(pObj);

    pObj->createArgs.rszPrms.rPrms[resolutionParams->channelNum].outWidth = resolutionParams->ResolutionWidth;
    pObj->createArgs.rszPrms.rPrms[resolutionParams->channelNum].outHeight = resolutionParams->ResolutionHeight;

    if(resolutionParams->ResolutionPitch[0] != 0)
	{
		if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[resolutionParams->channelNum].dataFormat) == 1)
		{
			pObj->createArgs.pitch[resolutionParams->channelNum] = resolutionParams->ResolutionPitch[0]/2;
		}	
		else
		{
			pObj->createArgs.pitch[resolutionParams->channelNum] = resolutionParams->ResolutionPitch[0];
		}
		
		pObj->createArgs.outStreamInfo[resolutionParams->channelNum].pitch[0] = resolutionParams->ResolutionPitch[0];
		pObj->createArgs.outStreamInfo[resolutionParams->channelNum].pitch[1] = resolutionParams->ResolutionPitch[1];
	}
	
    Issdrv_captSetRszCfg(pObj, &pObj->createArgs.rszPrms);

    /* unlock driver instance */
    Iss_captUnlock(pObj);

    return status;
}



Int32 Iss_captGetChStatus(Iss_CaptObj * pObj,
                          Iss_CaptChGetStatusArgs * args,
                          Iss_CaptChStatus * chStatus)
{
    UInt16 lChannelNum, instId, streamId, chId;
    Iss_CaptChObj *pChObj;
    UInt32 curTime;
    Int32 status = FVID2_EFAIL;

    /*
     * parameter checking, not supported for global handle
     */
    if (pObj == (Iss_CaptObj *) ISS_CAPT_INST_ALL
        || pObj == NULL || args == NULL || chStatus == NULL)
        return FVID2_EFAIL;

    /*
     * init output structure
     */
    chStatus->isVideoDetected = FALSE;
    chStatus->outWidth = 0;
    chStatus->outHeight = 0;
    chStatus->droppedFrameCount = 0;
    chStatus->captureFrameCount = 0;

    /*
     * lock driver instance
     */
    Iss_captLock(pObj);

    if (pObj->state != ISS_CAPT_STATE_IDLE)
    {
        /*
         * driver needs to atleast be in open state for this call to work
         */

        /*
         * map from user channel number of driver channel number
         */
        lChannelNum = gIss_captCommonObj.fvidChannelNum2lChannelNumMap
            [args->channelNum];

        instId = Iss_captGetInstId(lChannelNum);
        streamId = Iss_captGetStreamId(lChannelNum);
        chId = Iss_captGetChId(lChannelNum);

        if (instId < ISS_CAPT_INST_MAX
            && streamId < pObj->numStream
            && chId < pObj->numCh && instId == pObj->instanceId)
        {
            /*
             * get channel object
             */
            pChObj = &pObj->chObj[streamId][chId];

            /*
             * get latest frame width x height
             */
            chStatus->outWidth = pChObj->lastFrameWidth;
            chStatus->outHeight = pChObj->lastFrameHeight;

            /*
             * get dropped frame count
             */
            chStatus->droppedFrameCount = pChObj->skipFrameCount + pChObj->dropFrameCount;
            chStatus->captureFrameCount = pChObj->captureFrameCount;

            /*
             * get current
             */
            curTime = Clock_getTicks();

            /*
             * if current time exceeds expected frame interval,
             * then its considered that video is not detected
             */
            if (curTime < (pChObj->lastFrameTimestamp
                           + args->frameInterval + 0))
            {
                chStatus->isVideoDetected = TRUE;
            }

            status = FVID2_SOK;
        }
    }

    /*
     * unlock driver instance
     */
    Iss_captUnlock(pObj);

    return status;
}

/* ===================================================================
 *  @func     Iss_captSetOutDataFmt
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
Int32 Iss_captSetOutDataFmt(Iss_CaptObj *pObj,Iss_CaptOutDataFormat *pOutDataFmt)
{
    Int32 status = FVID2_SOK;
	UInt32 streamId;

    /*
     * parameter checking, not supported for global handle
     */
    if (pObj == (Iss_CaptObj *)ISS_CAPT_INST_ALL ||
		pObj == NULL ||
        pOutDataFmt == NULL)
    {
        return FVID2_EFAIL;
    }
	
	streamId = pOutDataFmt->streamId;
	
	/* Check if data format change is required */
	if(((isYUV422ILEFormat(pOutDataFmt->dataFmt) == 1) && (isYUV422ILEFormat(pObj->createArgs.outStreamInfo[streamId].dataFormat) == 1)) || 
	   ((isYUV420SPFormat(pOutDataFmt->dataFmt) == 1) && (isYUV420SPFormat(pObj->createArgs.outStreamInfo[streamId].dataFormat) == 1)))
	{
		return FVID2_SOK;
	}
	
    /* lock driver instance */
    Iss_captLock(pObj);
		
	pObj->createArgs.outStreamInfo[streamId].dataFormat = pOutDataFmt->dataFmt;
	
	if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[streamId].dataFormat) == 1)
	{
		pObj->createArgs.outStreamInfo[streamId].pitch[0] *= 2;
		pObj->createArgs.outStreamInfo[streamId].pitch[1] *= 2; 
	}
	else
	{
		pObj->createArgs.outStreamInfo[streamId].pitch[0] /= 2;
		pObj->createArgs.outStreamInfo[streamId].pitch[1] /= 2; 	
	}
	
	pObj->createArgs.rszPrms.rPrms[streamId].posPrms.startX = pOutDataFmt->startX;
	pObj->createArgs.rszPrms.rPrms[streamId].posPrms.startY = pOutDataFmt->startY;
	
    /* unlock driver instance */
    Iss_captUnlock(pObj);	
		
	/* Reset and Restart capture */
	Iss_captResetAndRestart(NULL);
		
    return status;	
} 

/* Driver object lock */
Int32 Iss_captLock(Iss_CaptObj * pObj)
{
    /*
     * take semaphore for locking
     */
    Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);

    return FVID2_SOK;
}



/* Driver object unlock */
Int32 Iss_captUnlock(Iss_CaptObj * pObj)
{
    /*
     * release semaphore for un-locking
     */
    Semaphore_post(pObj->lock);

    return FVID2_SOK;
}



/* create driver semaphore */
Int32 Iss_captLockCreate(Iss_CaptObj * pObj)
{
    Int32 retVal = FVID2_SOK;

    Semaphore_Params semParams;
    /*
     * create locking semaphore
     */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    pObj->lock = Semaphore_create(1u, &semParams, NULL);

    if (pObj->lock == NULL)
    {
        Vps_printf(" CAPTURE:%s:%d: Semaphore_create() failed !!!\n",
                   __FUNCTION__, __LINE__);
        retVal = FVID2_EALLOC;
    }

    return retVal;
}



Int32 Iss_captLockDelete(Iss_CaptObj * pObj)
{
    /*
     * delete semaphore
     */
    Semaphore_delete(&pObj->lock);

    return FVID2_SOK;
}



/* ===================================================================
 *  @func     Iss_captCheckOverflow
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
Int32 Iss_captCheckOverflow(Iss_CaptOverFlowStatus * overFlowStatus)
{
    Int32 status = FVID2_SOK;

    return (status);
}


/* ===================================================================
 *  @func     Iss_captResetAndRestart
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

Int32 Iss_captResetAndRestart(Iss_CaptOverFlowStatus * overFlowStatus)
{

    Iss_CaptObj *pObj;
    Int32 status = 0;
    MSP_U32 eMSP;
    //MSP_IspInterruptCfgT tIspInterruptCfg;
    MSP_IspConfigSizesT *pConfigSizes;
    MSP_IspParamCfgT tIspParamCfg;
    MSP_IspQueryBscSizesT tIspBscSizes;
    MSP_IspYuvRangeCfgT tIspYuvRangeCfg;

    MSP_IspMemToMemCmdParamT tIspMemToMemCmdParam;
    MSP_IspProcessParamT tIspProcessParam;
    MSP_IspQueryH3aStateT tQueryH3aState;
    MSP_ERROR_TYPE eResult = MSP_ERROR_NONE;
    isif_vd_interrupt_t vdIntCfg;

#ifdef ENABLE_BTE
    int i;
    bte_config_t *contextHandle;
    UInt32 oriBufOffsetY, oriBufOffsetC;
#endif
    volatile UInt32 delay = 0;
    UInt32 key;

    /*
     * get instance specific handle
     */
    pObj = &gIss_captCommonObj.captureObj[0];

    Iss_captLock(pObj);

    key = Task_disable();

    Iem_disableAll();
    IssCdrv_unRegisterInt();

    //Clear the trigger
    isp_reset_trigger = 0;

    rsz_submodule_start(RSZ_STOP, RESIZER_A);
    rsz_submodule_start(RSZ_STOP, RESIZER_B);
    rsz_start(RSZ_STOP);

    /* Reset the ISP h/w */

    isp_regs->ISP5_SYSCONFIG |= 0x2;
    for (delay = 0u; delay < ISS_CAPT_RESET_DELAY; delay ++);
    while (isp_regs->ISP5_SYSCONFIG & 0x2 == 0x1);
    Iss_reInit(NULL);

    pConfigSizes = NULL;

    gIss_captCommonObj.pModuleInstance = (VideoModuleInstanceT *) ((pObj->createArgs).pCompPrivate);
    gIss_captCommonObj.pModuleInstance->tIspAppParam.pAppData = gIss_captCommonObj.pModuleInstance;
    gIss_captCommonObj.pModuleInstance->tIspAppParam.MSP_callback = drvIspCallback;

    ((MSP_COMPONENT_TYPE *) (gIss_captCommonObj.pModuleInstance->hIspHandle))->tAppCBParam = gIss_captCommonObj.pModuleInstance->tIspAppParam;

    CLEAR_ALL_EVENTS(gIss_captCommonObj.pModuleInstance);

    MSP_ISP_init(gIss_captCommonObj.pModuleInstance->hIspHandle, MSP_PROFILE_REMOTE);

    pConfigSizes = (MSP_PTR) malloc(sizeof(MSP_IspConfigSizesT));
    GT_assert(GT_DEFAULT_MASK, pConfigSizes != NULL);

    MSP_ISP_control(gIss_captCommonObj.pModuleInstance->hIspHandle, MSP_CTRLCMD_START, NULL);

    pConfigSizes->nInSizeX = pObj->inFmt.width;
    pConfigSizes->nInSizeY = pObj->inFmt.height;
    pConfigSizes->nInSizePpln = pObj->createArgs.pitch[0];
    pConfigSizes->nInStartX = pObj->createArgs.rszPrms.cropPrms.cropStartX;
    pConfigSizes->nInStartY = pObj->createArgs.rszPrms.cropPrms.cropStartY;

    if(pObj->vsEnable == 0)
    {
        pConfigSizes->nOutSizeAX =
            pObj->createArgs.rszPrms.rPrms[0].outWidth;
        pConfigSizes->nOutSizeAY =
        pObj->createArgs.rszPrms.rPrms[0].outHeight;
    }
    else
    {
        pConfigSizes->nOutSizeAX =
            (pObj->createArgs.rszPrms.rPrms[0].outWidth *
                VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
        pConfigSizes->nOutSizeAY =
            (pObj->createArgs.rszPrms.rPrms[0].outHeight *
                VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;;
    }

    if((pConfigSizes->nOutSizeAX > pConfigSizes->nInSizeX) ||
       (pConfigSizes->nOutSizeAY > pConfigSizes->nInSizeY))
    {
        GT_assert(GT_DEFAULT_MASK, 0);
    }

    pConfigSizes->nOutSizeABpln = pObj->createArgs.pitch[0];
    pConfigSizes->nOutSizeABplnC = 0;

    if (pObj->numStream == 2)
    {
        if((pObj->vsEnable == 0) || (pObj->vsDemoEnable == 1))
        {
            pConfigSizes->nOutSizeBX =
                pObj->createArgs.rszPrms.rPrms[1].outWidth;
            pConfigSizes->nOutSizeBY =
                pObj->createArgs.rszPrms.rPrms[1].outHeight;
        }
        else
        {
            pConfigSizes->nOutSizeBX =
                (pObj->createArgs.rszPrms.rPrms[1].outWidth *
                    VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
            pConfigSizes->nOutSizeBY =
                (pObj->createArgs.rszPrms.rPrms[1].outHeight *
                    VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
        }

        if((pConfigSizes->nOutSizeBX > pConfigSizes->nInSizeX) ||
           (pConfigSizes->nOutSizeBY > pConfigSizes->nInSizeY))
        {
            GT_assert(GT_DEFAULT_MASK, 0);
        }

        pConfigSizes->nOutSizeBBpln = pObj->createArgs.pitch[1];
        pConfigSizes->nOutSizeBBplnC = 0;
    }
    else
    {
        pConfigSizes->nOutSizeBX = 720;
        pConfigSizes->nOutSizeBY = 480;
        pConfigSizes->nOutSizeBBpln = 720;
    }

    pConfigSizes->nCropX = pObj->createArgs.rszPrms.cropPrms.cropStartX;
    pConfigSizes->nCropY = pObj->createArgs.rszPrms.cropPrms.cropStartY;
    pConfigSizes->nClockPercents = MSP_ISS_SPEED_MAX;

    eMSP = MSP_ISP_query(gIss_captCommonObj.pModuleInstance->hIspHandle,
                      (MSP_INDEXTYPE) MSP_ISP_QUERY_MAX_WIDTH, pConfigSizes);

    eMSP = MSP_ISP_query(gIss_captCommonObj.pModuleInstance->hIspHandle,
                      (MSP_INDEXTYPE) MSP_ISP_QUERY_CROP_SIZES, pConfigSizes);
    if (eMSP != MSP_ERROR_NONE)

    {
        // Vps_printf("Error MSP_ISP_query failed!!!\n");
    }

    gIss_captCommonObj.pModuleInstance->nLscTableIsValid = MSP_FALSE;
    gIss_captCommonObj.pModuleInstance->nOutStartX = 0;
    gIss_captCommonObj.pModuleInstance->nOutStartY = 0;
    gIss_captCommonObj.pModuleInstance->bVidStab = MSP_FALSE;
    gIss_captCommonObj.pModuleInstance->bAllocateOutBuff = MSP_FALSE;

    IssFillDefaultParams((iss_config_processing_t *) gIss_captCommonObj.pIssConfig);

#if defined(APPRO_SENSOR_VENDOR) && defined(IMGS_SONY_IMX136)
		gIss_captCommonObj.pIssConfig->eMsbPos = MSP_IPIPE_BAYER_MSB_BIT13;
		isif_reg->CLDCOFST = 0;
#else
    gIss_captCommonObj.pIssConfig->eMsbPos = MSP_IPIPE_BAYER_MSB_BIT11;
#endif	

    gIss_captCommonObj.pIssConfig->ptBsc = &(gIss_captCommonObj.pModuleInstance->tBscCfg);
    gIss_captCommonObj.pIssConfig->ptLsc2D->nHDirDataOffset = 16;

#ifdef IMGS_OMNIVISION_OV7740
    gIss_captCommonObj.pIssConfig->eColorPattern = MSP_IPIPE_BAYER_PATTERN_BGGR;
#else
    gIss_captCommonObj.pIssConfig->eColorPattern = MSP_IPIPE_BAYER_PATTERN_GRBG;
#endif

    gIss_captCommonObj.pIssConfig->nVpDevice = MSP_IPIPE_VP_DEV_PI;
    gIss_captCommonObj.pIssConfig->eFH3aValidity =
        (MSP_PROC_H3A_VALID_ID) (MSP_PROC_H3A_VALID_AEWB |
                                 MSP_PROC_H3A_VALID_H3A |
                                 MSP_PROC_H3A_VALID_AF);

    gIss_captCommonObj.pModuleInstance->tConfigParams.eInFormat = MSP_IPIPE_IN_FORMAT_BAYER;
    if ((pObj->createArgs.outStreamInfo[0].dataFormat == FVID2_DF_YUV420SP_UV)
        || (pObj->createArgs.outStreamInfo[0].dataFormat ==
            FVID2_DF_YUV420SP_VU))
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.eOutFormatA =
            MSP_IPIPE_OUT_FORMAT_YUV420;
    }
    else if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[0].dataFormat) == 1)
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.eOutFormatA =
            MSP_IPIPE_OUT_FORMAT_YUV422;
    }
    else if(pObj->createArgs.outStreamInfo[0].dataFormat == FVID2_DF_BAYER_RAW)
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.eOutFormatA = MSP_IPIPE_OUT_FORMAT_YUV422;
    }
    else
    {
        Vps_printf("Stream 0: Invalid output format configured!!!\n");
    }

    if ((pObj->createArgs.outStreamInfo[1].dataFormat == FVID2_DF_YUV420SP_UV)
        || (pObj->createArgs.outStreamInfo[1].dataFormat ==
            FVID2_DF_YUV420SP_VU))
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.eOutFormatB =
            MSP_IPIPE_OUT_FORMAT_YUV420;
    }

    else if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[1].dataFormat) == 1)
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.eOutFormatB =
            MSP_IPIPE_OUT_FORMAT_YUV422;
    }
    else
    {
        Vps_printf("Stream 1: Invalid output format configured!!!\n");
    }

    gIss_captCommonObj.pModuleInstance->tConfigParams.eCompresIn = MSP_IPIPE_PROC_COMPR_NO;
    gIss_captCommonObj.pModuleInstance->tConfigParams.eCompresOut = MSP_IPIPE_PROC_COMPR_NO;
    gIss_captCommonObj.pModuleInstance->tConfigParams.nFFlipB = 0;
    gIss_captCommonObj.pModuleInstance->tConfigParams.nFMirrorB = 0;
    gIss_captCommonObj.pModuleInstance->tConfigParams.nFFlipA = 0;
    gIss_captCommonObj.pModuleInstance->tConfigParams.nFMirrorA = 0;
    gIss_captCommonObj.pModuleInstance->tConfigParams.nVidstabEnb = 0;
    gIss_captCommonObj.pModuleInstance->tConfigParams.ptIssConfig = gIss_captCommonObj.pIssConfig;
    gIss_captCommonObj.pModuleInstance->tConfigParams.pLsc2DTableBuffer =
    gIss_captCommonObj.pModuleInstance->p2DLscTableBuff;
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nInSizeX =
        //gIss_captCommonObj.pModuleInstance->nIssInX;
        pObj->inFmt.width;
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nInSizeY =
        //gIss_captCommonObj.pModuleInstance->nIssInY;
        pObj->inFmt.height;
    gIss_captCommonObj.pModuleInstance->tConfigParams.ptIssConfig->eRszIpPortSel =  MSP_RSZ_IP_IPIPEIF;
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeAX =
    pConfigSizes->nOutSizeAX;
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeAY =
    pConfigSizes->nOutSizeAY;
#ifndef ENABLE_BTE
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeABpln =
        pObj->createArgs.pitch[0];
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeABplnC =
        pObj->createArgs.pitch[0];
#else
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeABpln =
        BTE_PITCH;
    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeABplnC =
        BTE_PITCH;
#endif

    if (pObj->numStream == 2)
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBX =
            pConfigSizes->nOutSizeBX;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBY =
            pConfigSizes->nOutSizeBY;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBBpln =
            pObj->createArgs.pitch[1];
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBBplnC =
            pObj->createArgs.pitch[1];
    }
    else
    {
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBX = 720;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBY = 480;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBBpln = 720;
        gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nOutSizeBBplnC = 720;
    }


    gIss_captCommonObj.pModuleInstance->tConfigParams.tConfigSizes.nClockPercents =
        MSP_ISS_SPEED_MAX;

    tIspParamCfg.ptIssParams = &gIss_captCommonObj.pModuleInstance->tConfigParams;

    MSP_ISP_config(gIss_captCommonObj.pModuleInstance->hIspHandle,
                   (MSP_INDEXTYPE) MSP_ISP_CFG_PROC_MEM_TO_MEM, &tIspParamCfg);

    if(pObj->createArgs.captureMode == ISS_CAPT_INMODE_ISIF)
    {
        if(pObj->numStream == 1)
        {
            status = Iss_Disable_RszB();

            if(status == FVID2_EFAIL)
                Vps_rprintf("Iss_Disable_RszB Failed \n");
        }

        /* Resizer should be configured in ISIF mode only */
        Issdrv_captSetRszCfg(pObj, &pObj->createArgs.rszPrms);
    }

    Issdrv_setIsifConfig(&pObj->isifCfg);
    Vps_rprintf("\n%s:%d\n", __func__, __LINE__);

    ipipe_reg->SRC_VSZ = pObj->inFmt.height;
    ipipe_reg->SRC_HSZ = pObj->inFmt.width;

    gIss_captCommonObj.pModuleInstance->nLscTableIsValid = MSP_TRUE;

    IssCdrv_setIpipeCfg();

    Issdrv_setH3aConfig(&pObj->h3aCfg, 0);

    tIspBscSizes.pRowsBufferSize = 0;
    tIspBscSizes.pColsBufferSize = 0;
    tIspBscSizes.ptBscCfg = gIss_captCommonObj.pModuleInstance->tConfigParams.ptIssConfig->ptBsc;
    MSP_ISP_query(gIss_captCommonObj.pModuleInstance->hIspHandle,
                  (MSP_INDEXTYPE) MSP_ISP_QUERY_BSC_BUFF_SIZES, &tIspBscSizes);

    tIspYuvRangeCfg.eIspYuvRange = MSP_ISS_YUV_RANGE_FULL;
    MSP_ISP_config(gIss_captCommonObj.pModuleInstance->hIspHandle,
                   (MSP_INDEXTYPE) MSP_ISP_CFG_YUV_RANGE, &tIspYuvRangeCfg);

    if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
        (pObj->inFmt.dataFormat == FVID2_DF_RAW))
    {
        /* Histogram Config */
        {
            MSP_IpipeHistCfgT histogramCfg;
            MSP_IpipeHistDimT histogramDim[4];
            MSP_U8            gainTbl[4];

            memcpy(&histogramCfg, gIss_captCommonObj.pIssConfig->ptHistogram, sizeof(MSP_IpipeHistCfgT));

            histogramCfg.nOst  = 0;     // Free Run
            histogramCfg.nSel  = 1;     // Input Selection - From noise filter
            histogramCfg.nType = 2;     // G selection in Bayer mode - (Gb + Gr)/2

            histogramDim[0].nVPos  = 0;
            histogramDim[0].nVSize = pObj->inFmt.height;
            histogramDim[0].nHPos  = 0;
            histogramDim[0].nHSize = pObj->inFmt.width;

            gainTbl[0] = 6;             // R
            gainTbl[1] = 6;             // Gr
            gainTbl[2] = 6;             // Gb
            gainTbl[3] = 6;             // B

            histogramCfg.ptHistDim = histogramDim;
            histogramCfg.pnGainTbl = gainTbl;

            MSP_ISP_config(gIss_captCommonObj.pModuleInstance->hIspHandle,(MSP_INDEXTYPE)MSP_ISP_CFG_HIST,&histogramCfg);
        }

        if(pObj->glbcEnable == 1)
        {
            MSP_IpipeBoxcarCfgT BoxCarCfg;
            memcpy(&BoxCarCfg,gIss_captCommonObj.pIssConfig->ptBoxcar,sizeof(MSP_IpipeBoxcarCfgT));

            BoxCarCfg.nEnable = TRUE;
            if(pObj->createArgs.captureMode == ISS_CAPT_INMODE_ISIF)
            {
                BoxCarCfg.nOst = 0;
            }
            else
            {
                BoxCarCfg.nOst = 1;
            }
            BoxCarCfg.nBoxSize = 1;
            BoxCarCfg.nShift = 3;
            BoxCarCfg.pAddr = NULL;

            MSP_ISP_config(gIss_captCommonObj.pModuleInstance->hIspHandle,(MSP_INDEXTYPE)MSP_ISP_CFG_BOXCAR,&BoxCarCfg);
        }
    }

    IssAlg_captInit();

#ifdef ENABLE_BTE
        GT_assert(GT_DEFAULT_MASK, bte_init() == BTE_SUCCESS);

        GT_assert(GT_DEFAULT_MASK, bte_open(1) == BTE_SUCCESS);/*0x20000000~0x3FFFFFFF*/

        gIss_captCommonObj.handleBTE.baseAddress = 0x20000000;

        gIss_captCommonObj.handleBTE.contextNumber =
            2 << BTE_FEXT(bte_reg->BTE_HL_HWINFO, CSL_BTE_BTE_HL_HWINFO_CONTEXTS_SHIFT, CSL_BTE_BTE_HL_HWINFO_CONTEXTS_MASK);

        gIss_captCommonObj.handleBTE.bwLimiter = BTE_BWL;

        BTE_SET32(bte_reg->BTE_CTRL, gIss_captCommonObj.handleBTE.bwLimiter, CSL_BTE_BTE_CTRL_BW_LIMITER_SHIFT,
                  CSL_BTE_BTE_CTRL_BW_LIMITER_MASK);
        BTE_SET32(bte_reg->BTE_CTRL, 1, CSL_BTE_BTE_CTRL_POSTED_SHIFT,
                  CSL_BTE_BTE_CTRL_POSTED_MASK);

        for (i = 0; i < gIss_captCommonObj.handleBTE.contextNumber; i++)
        {
            contextHandle = &gIss_captCommonObj.handleBTE.bte_config[i];

            contextHandle->context_num = (BTE_CONTEXT)i;

            contextHandle->context_ctrl.init_sx = 0;
            contextHandle->context_ctrl.init_sy = 0;

            contextHandle->context_ctrl.mode = 0x0;/*write*/
            contextHandle->context_ctrl.one_shot = 0x1;/*one shot*/
            contextHandle->context_ctrl.addr32 = 1;/*TILER*/
            contextHandle->context_ctrl.autoflush = 0;

            contextHandle->context_ctrl.start = 0;
            contextHandle->context_ctrl.stop = 0;
            contextHandle->context_ctrl.flush = 0;
        }

        /* Assuming here that output is from RSZ when BTE is used */
        gIss_captCommonObj.handleBTE.bte_config[0].context_start_addr = 0;
        if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[0].dataFormat) == 0)
            gIss_captCommonObj.handleBTE.bte_config[1].context_start_addr =
                (DIV_UP(pObj->createArgs.rszPrms.rPrms[0].outWidth, 128) << CSL_BTE_BTE_CONTEXT_START_0_X_SHIFT);

        configureBteContext(&gIss_captCommonObj.handleBTE, 0,
            pObj->createArgs.rszPrms.rPrms[0].outWidth, pObj->createArgs.rszPrms.rPrms[0].outHeight,
            pObj->createArgs.outStreamInfo[0].dataFormat, curStreamBuf[0].mirrorMode);

        oriBufOffsetY = Utils_tilerGetOriAddr((Int32)PrimaryFrame_RszA->addr[1][0], 0, curStreamBuf[0].mirrorMode,
            pObj->createArgs.rszPrms.rPrms[0].outWidth, pObj->createArgs.rszPrms.rPrms[0].outHeight);
        startBteContext(&gIss_captCommonObj.handleBTE.bte_config[0], oriBufOffsetY);

        if(isYUV422ILEFormat(pObj->createArgs.outStreamInfo[0].dataFormat) == 0)
        {
            oriBufOffsetC = Utils_tilerGetOriAddr((Int32)PrimaryFrame_RszA->addr[1][1], 1, curStreamBuf[0].mirrorMode,
                pObj->createArgs.rszPrms.rPrms[0].outWidth, pObj->createArgs.rszPrms.rPrms[0].outHeight/2);
            startBteContext(&gIss_captCommonObj.handleBTE.bte_config[1], oriBufOffsetC);
        }
#endif

    PrimaryFrame_RszA = curStreamBuf[0].ptmpFrame;
    PrimaryFrame_RszB = curStreamBuf[1].ptmpFrame;

    EXPECT_EVENT(gIss_captCommonObj.pModuleInstance, VIDEO_ISR_EVENT_RSZ);

#ifndef ENABLE_BTE
    tIspMemToMemCmdParam.pOutbuffRszA = PrimaryFrame_RszA->addr[1][0];
    tIspMemToMemCmdParam.pOutbuffRszACr = PrimaryFrame_RszA->addr[1][1];
#else
    tIspMemToMemCmdParam.pOutbuffRszA = (void *)(gIss_captCommonObj.handleBTE.baseAddress +
        gIss_captCommonObj.handleBTE.bte_config[0].context_start_addr);

    tIspMemToMemCmdParam.pOutbuffRszACr = (void *)(gIss_captCommonObj.handleBTE.baseAddress +
        gIss_captCommonObj.handleBTE.bte_config[1].context_start_addr);
#endif
    curStreamBuf[0].ptmpFrame = PrimaryFrame_RszA;

    if(pObj->numStream == 2)
    {
        tIspMemToMemCmdParam.pOutbuffRszB = PrimaryFrame_RszB->addr[1][0];
        tIspMemToMemCmdParam.pOutbuffRszBCr = PrimaryFrame_RszB->addr[1][1];
        curStreamBuf[1].ptmpFrame = PrimaryFrame_RszB;
    }
    else if(pObj->numStream == 1)
    {
        tIspMemToMemCmdParam.pOutbuffRszB = NULL;
        tIspMemToMemCmdParam.pOutbuffRszBCr = NULL;
        curStreamBuf[1].ptmpFrame = NULL;
    }
    if(pObj->glbcEnable == 1)
        ipipe_config_boxcar_addr((UInt32)PrimaryFrame_RszA->blankData);

    vdIntCfg.vd0_intr_timing_lineno =
        pObj->inFmt.height - ISS_VDINT_NUM_LINES_LESS;
    vdIntCfg.vd1_intr_timing_lineno =
        pObj->inFmt.height - ISS_VDINT_NUM_LINES_LESS;
    vdIntCfg.vd2_intr_timing_lineno =
        pObj->inFmt.height - ISS_VDINT_NUM_LINES_LESS;
    isif_config_vd_line_numbers(&vdIntCfg);
    *(MSP_U32 *) (0x5505002C) |= 1;                        // isp IRQ set register

    status = IssCdrv_registerInt();

    if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
        (pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW))
    {
        tIspMemToMemCmdParam.eInFormat = MSP_IPIPE_IN_FORMAT_BAYER;
    }
    else
    {
        tIspMemToMemCmdParam.eInFormat = MSP_IPIPE_IN_FORMAT_YUV422;
    }

    tIspMemToMemCmdParam.pOutRawBuffer = (void *) NULL;
    tIspMemToMemCmdParam.pOutRawBuffer = NULL;
    tIspProcessParam.eCmd = MSP_ISP_CMD_MEM_TO_MEM_START;
    tIspProcessParam.pCmdData = &tIspMemToMemCmdParam;
    if(pObj->createArgs.captureMode == ISS_CAPT_INMODE_ISIF)
    {
        eResult =  MSP_ISP_process(
                    gIss_captCommonObj.pModuleInstance->hIspHandle,
                    &tIspProcessParam,
                    NULL);
        if (MSP_ERROR_NONE == eResult)
        {

        }
    }
    else
    {
	    if (NULL == PrimaryFrame_RszA->addr[1][0])
	    {
		    Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
		    GT_assert(GT_DEFAULT_MASK, FALSE);
	    }
        // ISS_CAPT_INMODE_DDR mode
        isif_config_sdram_address((UInt32)(UInt32)PrimaryFrame_RszA->addr[1][0]);
        isif_start(ISIF_START_WRITE);
    }

    if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
        (pObj->inFmt.dataFormat == FVID2_DF_RAW))
    {
        tQueryH3aState.eH3aState = MSP_H3A_STOPPED;
        issDrvH3aStart();

        MSP_ISP_query(
            gIss_captCommonObj.pModuleInstance->hIspHandle,
            (MSP_INDEXTYPE) MSP_ISP_QUERY_H3A_STATE,
            &tQueryH3aState);


        if (tQueryH3aState.eH3aState & (MSP_H3A_AEWB_ENABLED))
        {
            EXPECT_EVENT(gIss_captCommonObj.pModuleInstance, VIDEO_ISR_EVENT_H3A);
            /*tIspInterruptCfg.eInterruptId = MSP_ISS_DRV_H3A_END_ISR;
            tIspInterruptCfg.bEnable = MSP_TRUE;

            MSP_ISP_config(gIss_captCommonObj.pModuleInstance->hIspHandle,
                           (MSP_INDEXTYPE) MSP_ISP_CFG_INT,
                           (MSP_PTR) & tIspInterruptCfg);*/
        }
        //("%s:%d\n", __func__, __LINE__);
    }

    /* Need to set the Mirror mode also as resizer registers are reset */
    if (curStreamBuf[0].memType == ISS_NONTILEDMEM)
    {
        rsz_cfg_flip(RESIZER_A, (RSZ_FLIP_CFG_T) curStreamBuf[0].mirrorMode);
    }
    if (curStreamBuf[1].memType == ISS_NONTILEDMEM)
    {
        rsz_cfg_flip(RESIZER_B, (RSZ_FLIP_CFG_T) curStreamBuf[1].mirrorMode);
    }

    if(pObj->createArgs.captureMode == ISS_CAPT_INMODE_DDR)
    {
        Issdrv_ispDrvRszOvfl();
    }

    Task_restore(key);

    /* unlock driver instance */
    Iss_captUnlock(pObj);

    if (pConfigSizes != NULL)
    {
        free(pConfigSizes);
    }

    if(pObj->createArgs.captureMode != ISS_CAPT_INMODE_DDR)
    {
        Iss_ispDrvCallBack();
    }

    return status;
}



/* ===================================================================
 *  @func     Iss_captPrintAdvancedStatistics
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
Int32 Iss_captPrintAdvancedStatistics(UInt32 totalTimeInMsecs)
{
    Iss_CaptObj *pDrvObj;
    UInt32 streamId, chId, drvId, totalFieldCount;
    Iss_CaptChObj *pChObj;

    Vps_rprintf(" \r\n");
    Vps_rprintf(" *** ISS Capture Driver Advanced Statistics *** \r\n");
    Vps_rprintf(" \r\n");
    Vps_rprintf(" ISS Parser Reset Count : %d\r\n", gIss_captCommonObj.resetCount);
    Vps_rprintf(" \r\n");

    Vps_rprintf("     |   Total    Even     Odd  Total  Even   Odd  Min /  Max  Min /  Max        Total   (No In Buffer / \r\n");
    Vps_rprintf(" CH  |  Fields  Fields  Fields    FPS   FPS   FPS       Width      Height  Drop Fields    Skip Frames )\r\n");
    Vps_rprintf(" ------------------------------------------------------------------------------------------------------------\r\n");

    for(drvId=0; drvId<ISS_CAPT_INST_MAX; drvId++)
    {
        pDrvObj = &gIss_captCommonObj.captureObj[drvId];

        if( pDrvObj->state == ISS_CAPT_STATE_IDLE)
            continue;

        for(streamId=0; streamId<pDrvObj->createArgs.numStream; streamId++)
        {
            for(chId=0; chId<pDrvObj->createArgs.numCh; chId++)
            {
                pChObj = &pDrvObj->chObj[streamId][chId];

                totalFieldCount = pChObj->skipFrameCount + pChObj->dropFrameCount + pChObj->captureFrameCount;

                if(totalFieldCount)
                {
                    Vps_rprintf(" %d%d%d | %7d %7d %7d %6d %5d %5d %4d / %4d %7d %d/%d\r\n",
                        drvId,
                        streamId,
                        chId,
                        totalFieldCount,
                        pChObj->fieldCount[0],
                        pChObj->fieldCount[1],
                        totalFieldCount/(totalTimeInMsecs/1000),
                        pChObj->fieldCount[0]/(totalTimeInMsecs/1000),
                        pChObj->fieldCount[1]/(totalTimeInMsecs/1000),
                        pChObj->lastFrameWidth,
                        pChObj->lastFrameHeight,
                        pChObj->skipFrameCount + pChObj->dropFrameCount,
                        pChObj->dropFrameCount,
                        pChObj->skipFrameCount);
                }
            }
        }
    }


    Vps_rprintf(" \r\n");

    return 0;
}



/* ===================================================================
 *  @func     Iss_captDequeueFromQueToFrameList
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
Int32 Iss_captDequeueFromQueToFrameList(VpsUtils_QueHandle * queHandle,
                                        FVID2_FrameList * frameList)
{
    Int32 dequeStatus;
    volatile FVID2_Frame *pData;

    do
    {
        /*
         * Max frames limit exceeded exit
         */
        if (frameList->numFrames >= FVID2_MAX_FVID_FRAME_PTR)
            break;

        pData = NULL;

        /*
         * deque from handle, could be blocking for the first time
         * based on 'timeout'
         */
        dequeStatus = VpsUtils_queGet(
                        queHandle,
                        (Ptr *) & pData,
                        1,
                        BIOS_NO_WAIT);

        if (dequeStatus == FVID2_SOK && pData != NULL &&
            pData->channelNum != ISS_CAPT_DROP_FRAME_CH_ID)
        {
            /*
             * deque is successful, add to frame list
             */
            frameList->frames[frameList->numFrames] = (FVID2_Frame *) pData;
            frameList->numFrames++;
        }

        /*
         * Max frames limit exceeded exit
         */
        if (frameList->numFrames >= FVID2_MAX_FVID_FRAME_PTR)
            break;
    } while (dequeStatus == FVID2_SOK);

    return FVID2_SOK;
}



/* ===================================================================
 *  @func     Iss_captFlush
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

Int32 Iss_captFlush(Fdrv_Handle handle, FVID2_FrameList * frameList)
{
    Iss_CaptObj *pObj = (Iss_CaptObj *) handle;
    Int32 status = FVID2_SOK, instId, streamId, chId;
    Iss_CaptChObj *pChObj;

    /*
     * if global handle then make handle as NULL, this indicates
     * we need to check over all driver instances
     */

    if (handle == (Fdrv_Handle) ISS_CAPT_INST_ALL)
        handle = NULL;

    /*
     * parameter checking
     */
    if (frameList == NULL)
    {
        return FVID2_EFAIL;
    }

    /*
     * init frame list fields
     */
    frameList->numFrames = 0;
    frameList->perListCfg = NULL;
    frameList->drvData = NULL;
    frameList->reserved = NULL;

    /*
     * for every instance
     */
    for (instId = 0; instId < ISS_CAPT_INST_MAX; instId++)
    {
        if (handle == NULL)
        {
            /*
             * if handle is NULL, i.e global handle, then get handle using 'instId',
             * else use user supplied handle
             */
            pObj = &gIss_captCommonObj.captureObj[instId];
        }
        if (pObj->state == ISS_CAPT_STATE_CREATED
            || pObj->state == ISS_CAPT_STATE_STOPPED)
        {
            /* dequeue frames to frame list for this driver inst */
            for (streamId = 0u; streamId < pObj->numStream; streamId++)
            {
                Iss_captDequeueFromQueToFrameList(&pObj->fullQue[streamId],
                                                  frameList);

                for (chId = 0u; chId < pObj->numCh; chId++)
                {
                    pChObj = &pObj->chObj[streamId][chId];
                    Iss_captDequeueFromQueToFrameList(&pChObj->tmpQue,
                                                      frameList);
                    Iss_captDequeueFromQueToFrameList(&pChObj->emptyQue,
                                                      frameList);
                }
            }
        }
        else
        {
            /* invalid state for dequeue all */
            if (handle != NULL)
                status = FVID2_EFAIL;
        }

        if (handle != NULL)
            break;                                         /* if not global
                                                            * handle, then we
                                                            * are done, so
                                                            * exit */
        /*
         * Max frames limit exceeded exit
         */
        if (frameList->numFrames >= FVID2_MAX_FVID_FRAME_PTR)
            break;

        /*
         * global handle mode, so check other driver instances also
         */
    }

    return status;
}



/* ===================================================================
 *  @func     drvIspCallback
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
static MSP_ERROR_TYPE drvIspCallback(MSP_PTR hMSP, MSP_PTR pAppData,
                                     MSP_EVENT_TYPE tEvent,
                                     MSP_OPAQUE nEventData1,
                                     MSP_OPAQUE nEventData2)
{
    MSP_ERROR_TYPE eResult = MSP_ERROR_NONE;
    MSP_U32 nRemainIsrEventsBefore;

    gIss_captCommonObj.pModuleInstance = (VideoModuleInstanceT *) pAppData;

    // Save initial events to check if the event is expected
    nRemainIsrEventsBefore = gIss_captCommonObj.pModuleInstance->nRemainIsrEvents;

    switch (tEvent)
    {
        case MSP_ERROR_EVENT:
            break;

        case MSP_DATA_EVENT:
            switch ((MSP_U32) nEventData1)
            {
                case MSP_ISS_DRV_CCP_LCM:
                    CLEAR_EVENT(gIss_captCommonObj.pModuleInstance, VIDEO_ISR_EVENT_CCP);
                    break;

                case MSP_ISS_DRV_ISIF_2DLSC_ISR:
                    // TODO isif2DLscSofIsr(pAppData);
                    break;

                default:
                    break;
            };
            break;

        case MSP_PROFILE_EVENT:
        case MSP_OPEN_EVENT:
        case MSP_CLOSE_EVENT:
            break;

        case MSP_CTRLCMD_EVENT:
            switch ((MSP_U32) nEventData1)
            {
                case MSP_CALLBACK_CMD_START:
                    // TODO - ISP is ready for use
                    break;

                case MSP_CALLBACK_CMD_STOP:
                    break;

                default:
                    break;
            };
            break;

        default:
            break;
    };

    if ((nRemainIsrEventsBefore != gIss_captCommonObj.pModuleInstance->nRemainIsrEvents) &&
        (tEvent == MSP_DATA_EVENT) && (NO_EVENTS_REMAIN(gIss_captCommonObj.pModuleInstance)))
    {
        EXPECT_EVENT(gIss_captCommonObj.pModuleInstance, VIDEO_ISR_EVENT_H3A);
        EXPECT_EVENT(gIss_captCommonObj.pModuleInstance, VIDEO_ISR_EVENT_RSZ);
    }

    return (eResult);
}

Int32 Iss_Disable_RszB()
{
    Int32 retval = 0;
    RESIZER_T resizer = RESIZER_B;

    retval = rsz_clock_enable(resizer, RSZ_CLK_DISABLE);

    return retval;
}


Int32 Issdrv_captSetRszCfg(
            Iss_CaptObj *pObj,
            Iss_IspResizerParams *scPrms)
{
    rsz_global_crop_dims_t cropDims;
    Int32 imgHeight, imgWidth;

    GT_assert(GT_DEFAULT_MASK, (NULL != pObj));
    GT_assert(GT_DEFAULT_MASK, (NULL != scPrms));

    cropDims.h_pos = scPrms->cropPrms.cropStartX;
    cropDims.v_pos = scPrms->cropPrms.cropStartX;
    cropDims.h_size = scPrms->cropPrms.cropWidth;
    cropDims.v_size = scPrms->cropPrms.cropHeight;

    rsz_cfg_global_crop(&cropDims);

	if(pObj->vsEnable == 0)
	{
		imgHeight = scPrms->rPrms[0].outHeight;
		imgWidth = scPrms->rPrms[0].outWidth;
	}
	else
	{
		imgHeight = (scPrms->rPrms[0].outHeight *
						VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
		imgWidth = (scPrms->rPrms[0].outWidth *
						VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
	}
	
    /* RSZ A configuration */
    rszASetOutConfig(scPrms->cropPrms.cropWidth - 2,
                     scPrms->cropPrms.cropHeight - 4,
                     imgWidth,
                     imgHeight);

	if(pObj->vsEnable == 0)
	{
		imgHeight = scPrms->rPrms[1].outHeight;
		imgWidth = scPrms->rPrms[1].outWidth;
	}
	else
	{
		imgHeight = (scPrms->rPrms[1].outHeight *
						VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
		imgWidth = (scPrms->rPrms[1].outWidth *
						VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
	}

    if (pObj->numStream > 1)
    {
        rszBSetOutConfig(scPrms->cropPrms.cropWidth - 2,
                         scPrms->cropPrms.cropHeight - 4,
                         imgWidth,
                         imgHeight);
    }

    return FVID2_SOK;
}

Int32 IssCdrv_registerInt()
{
    Int32 status = FVID2_SOK;

    /* Register Callback for VD interrupt */
    gIss_captCommonObj.intHandle[ISS_CAPT_INT_VD_INT0] =
        Iem_register(
              IEM_IRQ_NUM_0,
              IEM_IRQ_BANK_NUM_ISP_IRQ0,
              IEM_EVENT_ISIF_VDINT0,
              IEM_PRIORITY0,
              Issdrv_IspIsifVdInt,
              NULL);
    if (NULL == gIss_captCommonObj.intHandle[ISS_CAPT_INT_VD_INT0])
    {
        Vps_printf("\n\n Cannot register VD ISR\n");
        status = FVID2_EFAIL;
    }

    /* Register Callback for RSZ interrupt */
    if (FVID2_SOK == status)
    {
        /* Register Callback for VD Interrupt interrupt */
        gIss_captCommonObj.intHandle[ISS_CAPT_INT_RSZ] =
            Iem_register(
                    IEM_IRQ_NUM_0,
                    IEM_IRQ_BANK_NUM_ISP_IRQ0,
                    IEM_EVENT_RSZ_INT_DMA,
                    IEM_PRIORITY0,
                    Issdrv_IspRszDmaEndInt,
                    NULL);
        if (NULL == gIss_captCommonObj.intHandle[ISS_CAPT_INT_RSZ])
        {
            Vps_printf("\n\n Cannot register RSZ ISR\n");
            status = FVID2_EFAIL;
        }
   }

    /* Register Callback for RSZ interrupt */
    if (FVID2_SOK == status)
    {
        /* Register Callback for VD Interrupt interrupt */
        gIss_captCommonObj.intHandle[ISS_CAPT_INT_H3A] =
            Iem_register(
                    IEM_IRQ_NUM_0,
                    IEM_IRQ_BANK_NUM_ISP_IRQ0,
                    IEM_EVENT_H3A_INT,
                    IEM_PRIORITY0,
                    Issdrv_IspH3aEndInt,
                    NULL);
        if (NULL == gIss_captCommonObj.intHandle[ISS_CAPT_INT_H3A])
        {
            Vps_printf("\n\n Cannot register H3A ISR\n");
            status = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == status)
    {
        /* Register Callback for VD Interrupt interrupt */
        gIss_captCommonObj.intHandle[ISS_CAPT_INT_BSC] =
            Iem_register(
                    IEM_IRQ_NUM_0,
                    IEM_IRQ_BANK_NUM_ISP_IRQ0,
                    IEM_EVENT_IPIPE_INT_BSC,
                    IEM_PRIORITY0,
                    Issdrv_IspBscInt,
                    NULL);
        if (NULL == gIss_captCommonObj.intHandle[ISS_CAPT_INT_BSC])
        {
            Vps_printf("\n\n Cannot register BSC ISR\n");
            status = FVID2_EFAIL;
        }
   }

    if (FVID2_SOK == status)
    {
        /* Register Callback for VD Interrupt interrupt */
        gIss_captCommonObj.intHandle[ISS_CAPT_INT_RSZ_OVFL] =
            Iem_register(
                    IEM_IRQ_NUM_0,
                    IEM_IRQ_BANK_NUM_ISP_IRQ0,
                    IEM_EVENT_RSZ_INT_FIFO_OVF,
                    IEM_PRIORITY0,
                    Issdrv_IspRszOvfl,
                    NULL);
        if (NULL == gIss_captCommonObj.intHandle[ISS_CAPT_INT_RSZ_OVFL])
        {
            Vps_printf("\n\n Cannot register Resizer Overflow ISR\n");
            status = FVID2_EFAIL;
        }
    }
    return (status);
}

Void IssCdrv_unRegisterInt()
{
    UInt32 cnt;

    for (cnt = 0u; cnt < ISS_CAPT_MAX_INT; cnt ++)
    {
        if (NULL != gIss_captCommonObj.intHandle[cnt])
        {
            Iem_unRegister(gIss_captCommonObj.intHandle[cnt]);
            gIss_captCommonObj.intHandle[cnt] = NULL;
        }
    }
}


Int32 Issdrv_getIsifConfig(Iss_IspIsifCfg *isifCfg)
{
    Int32 status = FVID2_SOK;
    Iss_CaptObj *pObj = NULL;

    GT_assert(GT_DEFAULT_MASK, (NULL != isifCfg));

    pObj = &gIss_captCommonObj.captureObj[0];

    isifCfg->hdVdDir = 1;
    isifCfg->hdPolarity = ISS_ISIF_SYNC_POLARITY_NEGATIVE;
    isifCfg->vdPolarity = ISS_ISIF_SYNC_POLARITY_NEGATIVE;

    isifCfg->hdWidth = pObj->inFmt.width;
    isifCfg->vdWidth = pObj->inFmt.height;
    isifCfg->width = isifCfg->hdWidth;
    isifCfg->height = (isifCfg->vdWidth * 2) + 1;
    isifCfg->outWidth = pObj->inFmt.width;
    isifCfg->outHeight = pObj->inFmt.height;
    isifCfg->startX = 14;

    isifCfg->cfaPattern = ISP_ISIF_CGA_PATTERN_MOSAIC;

    isifCfg->blankDcOffset = 0;

#if defined(APPRO_SENSOR_VENDOR) && defined(IMGS_SONY_IMX136)
	isifCfg->colorPattern[FVID2_FIELD_TOP_ADDR_IDX][1] =
		ISS_ISIF_COLOR_COMP_R_YE;
	isifCfg->colorPattern[FVID2_FIELD_TOP_ADDR_IDX][0] =
		ISS_ISIF_COLOR_COMP_GR_CY;
	isifCfg->colorPattern[FVID2_FIELD_TOP_ADDR_IDX][3] =
		ISS_ISIF_COLOR_COMP_GB_G;
	isifCfg->colorPattern[FVID2_FIELD_TOP_ADDR_IDX][2] =
		ISS_ISIF_COLOR_COMP_B_MG;

	isifCfg->colorPattern[FVID2_FIELD_BOTTOM_ADDR_IDX][1] =
		ISS_ISIF_COLOR_COMP_R_YE;
	isifCfg->colorPattern[FVID2_FIELD_BOTTOM_ADDR_IDX][0] =
		ISS_ISIF_COLOR_COMP_GR_CY;
	isifCfg->colorPattern[FVID2_FIELD_BOTTOM_ADDR_IDX][3] =
		ISS_ISIF_COLOR_COMP_GB_G;
	isifCfg->colorPattern[FVID2_FIELD_BOTTOM_ADDR_IDX][2] =
		ISS_ISIF_COLOR_COMP_B_MG;
#else
    isifCfg->colorPattern[FVID2_FIELD_TOP_ADDR_IDX][0] =
        ISS_ISIF_COLOR_COMP_R_YE;
    isifCfg->colorPattern[FVID2_FIELD_TOP_ADDR_IDX][1] =
        ISS_ISIF_COLOR_COMP_GR_CY;
    isifCfg->colorPattern[FVID2_FIELD_TOP_ADDR_IDX][2] =
        ISS_ISIF_COLOR_COMP_GB_G;
    isifCfg->colorPattern[FVID2_FIELD_TOP_ADDR_IDX][3] =
        ISS_ISIF_COLOR_COMP_B_MG;

    isifCfg->colorPattern[FVID2_FIELD_BOTTOM_ADDR_IDX][0] =
        ISS_ISIF_COLOR_COMP_R_YE;
    isifCfg->colorPattern[FVID2_FIELD_BOTTOM_ADDR_IDX][1] =
        ISS_ISIF_COLOR_COMP_GR_CY;
    isifCfg->colorPattern[FVID2_FIELD_BOTTOM_ADDR_IDX][2] =
        ISS_ISIF_COLOR_COMP_GB_G;
    isifCfg->colorPattern[FVID2_FIELD_BOTTOM_ADDR_IDX][3] =
        ISS_ISIF_COLOR_COMP_B_MG;
#endif	// modify by jem

    isifCfg->enableDecrAddr = 0;
    isifCfg->srcPack = ISS_ISIF_SRC_PACK_16BITS;

    if (ISS_ISIF_SRC_PACK_8BITS == isifCfg->srcPack)
        isifCfg->horzOffset = pObj->inFmt.width;
    else
        isifCfg->horzOffset = pObj->inFmt.width*2;

#if defined(APPRO_SENSOR_VENDOR) && defined(IMGS_SONY_IMX136)
	isifCfg->msbBitPos = ISS_ISIF_BAYER_MSB_POS_BIT13;
#else
    isifCfg->msbBitPos = ISS_ISIF_BAYER_MSB_POS_BIT11;
#endif	

    return (status);
}

Int32 Issdrv_setIsifConfig(Iss_IspIsifCfg *isifCfg)
{
    Int32 status = FVID2_SOK;
    UInt32 cnt;
    isif_ip_src_cfg_t isifSrcCfg;
    isif_sdram_op_cfg_t isifOpCfg;
    Iss_CaptObj *pObj = NULL;

    GT_assert(GT_DEFAULT_MASK, (NULL != isifCfg));

    pObj = &gIss_captCommonObj.captureObj[0];

    isifSrcCfg.sync_enable = ISIF_HD_VD_DISABLE;

    if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
        (pObj->inFmt.dataFormat == FVID2_DF_RAW))
    {
        isifSrcCfg.ip_pix_fmt = ISIF_DATA_INPUT_MODE_CCD_RAW;
    }
    else
    {
        if (pObj->createArgs.videoIfMode == ISS_CAPT_YUV_8BIT)
            isifSrcCfg.ip_pix_fmt = ISIF_DATA_INPUT_MODE_YUV8;
        else
            isifSrcCfg.ip_pix_fmt = ISIF_DATA_INPUT_MODE_YUV16;
    }

    isifSrcCfg.data_polarity = ISIF_DATA_NORMAL;
    isifSrcCfg.field_pol = ISIF_NEGATIVE_POLARITY;
    if (ISS_ISIF_SYNC_POLARITY_POSITIVE == isifCfg->hdPolarity)
    {
        isifSrcCfg.hd_pol = ISIF_POSITIVE_POLARITY;
    }
    else
    {
        isifSrcCfg.hd_pol = ISIF_NEGATIVE_POLARITY;
    }
    if (ISS_ISIF_SYNC_POLARITY_POSITIVE == isifCfg->vdPolarity)
    {
        isifSrcCfg.vd_pol = ISIF_POSITIVE_POLARITY;
    }
    else
    {
        isifSrcCfg.vd_pol = ISIF_NEGATIVE_POLARITY;
    }
    isifSrcCfg.field_dir = (ISIF_SIGNAL_DIRECTION) isifCfg->hdVdDir;
    isifSrcCfg.hd_vd_dir = (ISIF_SIGNAL_DIRECTION) isifCfg->hdVdDir;
    isifSrcCfg.hd_width = isifCfg->hdWidth;
    isifSrcCfg.vd_width = isifCfg->vdWidth;
    isifSrcCfg.ppln_hs_interval = isifCfg->width;
    isifSrcCfg.lpfr_vs_interval = isifCfg->height;
    isifSrcCfg.yc_in_swap = ISIF_YCINSWAP_DISABLE;
    isifSrcCfg.msb_inverse_cin = ISIF_MSB_INVERSE_CIN_DISABLE;
    isifSrcCfg.ip_data_msb_pos = (ISIF_GAIN_MSB_POS) isifCfg->msbBitPos;

    for (cnt = 0u; cnt < ISS_ISP_ISIF_MAX_PIX_POS; cnt ++)
    {
        isifSrcCfg.field0_pixel_pos[cnt] =
            (ISIF_COLOR_PATTERN_POSITION) isifCfg->colorPattern[0][cnt];
    }
    for (cnt = 0u; cnt < ISS_ISP_ISIF_MAX_PIX_POS; cnt ++)
    {
        isifSrcCfg.field1_pixel_pos[cnt] =
            (ISIF_COLOR_PATTERN_POSITION) isifCfg->colorPattern[1][cnt];
    }

    isifSrcCfg.cfa_pattern = (ISIF_CFA_PATTERN)isifCfg->cfaPattern;
    isifSrcCfg.r656_inferface_mode = ISIF_REC656IF_MODE_DISABLE;
    isifSrcCfg.err_correction_mode = ISIF_ERR_CORRECTION_FVC_DISABLE;
    isifSrcCfg.ccir656_bit_width = ISIF_BW656_DISABLE;

    status = isif_config_input_src(&isifSrcCfg);

    isifOpCfg.sdram_hpos = isifCfg->startX;
    isifOpCfg.sdram_hsize = isifCfg->outWidth;
    isifOpCfg.sdram_vpos0 = 0;
    isifOpCfg.sdram_vpos1 = 0;
    isifOpCfg.sdram_vsize = isifCfg->outHeight;
    isifOpCfg.memory_addr_decrement =
        (ISIF_HLINE_ADDR) isifCfg->enableDecrAddr;
	isifOpCfg.memory_addr_offset = isifCfg->horzOffset;
    isifOpCfg.field_offset_params.fid_polarity = ISIF_FID_NO_CHANGE;
    isifOpCfg.field_offset_params.odd_field_line_offset = ISIF_FIELD_LINE_OFFSET_PLUS_1;
    isifOpCfg.field_offset_params.even_line_even_field_offset = ISIF_LINE_OFFSET_PLUS_1;
    isifOpCfg.field_offset_params.odd_line_even_field_offset = ISIF_LINE_OFFSET_PLUS_1;
    isifOpCfg.field_offset_params.even_line_odd_field_offset = ISIF_LINE_OFFSET_PLUS_1;
    isifOpCfg.field_offset_params.odd_line_odd_field_offset = ISIF_LINE_OFFSET_PLUS_1;
    isifOpCfg.memory_addr = 0;
    isifOpCfg.alias_filter_enable = ISIF_LPF_ANTI_ALIAS_DISABLE;
    isifOpCfg.dpcm_predictor = ISIF_DPCM_ENCODE_PREDICTOR2;
    isifOpCfg.dpcm_enable = ISIF_DPCM_ENCODER_ENABLE_OFF;
    isifOpCfg.ccd_raw_shift_value = ISIF_CCD_DATA_NO_SHIFT;
    isifOpCfg.ccd_field_mode = ISIF_CCD_MODE_NON_INTERLACED;
    isifOpCfg.wen_selection_mode = ISIF_NO_WEN;
    isifOpCfg.data_write_enable = ISIF_DATA_WRITE_DISABLE;
    isifOpCfg.msb_inverse_cout = ISIF_MSB_INVERSE_COUT_DISABLE;
    isifOpCfg.sdram_byte_swap = ISIF_BYTE_SWAP_DISABLE;
    isifOpCfg.y_position = ISIF_SELECT_Y_POS_EVEN_PX;
    isifOpCfg.sdram_ext_trig = ISIF_NO_EXTRG;
    isifOpCfg.sdram_trig_select = ISIF_SDRAM_TRGSEL_DWEN_REG;
    isifOpCfg.ccd_valid_log = ISIF_WENLOG_SIGNAL_LOGICAL_AND_INTERNAL;
    isifOpCfg.fid_detection_mode = ISIF_FID_LATCH_VSYNC;
    isifOpCfg.yc_out_swap = ISIF_YCOUTSWAP_DISABLE;
    isifOpCfg.sdram_pack_fmt = (ISIF_SDRAM_PACK)isifCfg->srcPack;
    isifOpCfg.vsync_enable = ISIF_VLDC_RESYNC_VSYNC_ENABLE;
    isifOpCfg.culling_params.horizontal_odd_culling = 0xFF;
    isifOpCfg.culling_params.horizontal_even_culling = 0xFF;
    isifOpCfg.culling_params.vertical_culling = 0xFF;

    status |= isif_config_sdram_format(&isifOpCfg);

    if (FVID2_SOK == status)
    {
        memcpy(&pObj->isifCfg, isifCfg, sizeof(pObj->isifCfg));
    }

    return (status);
}

Int32 Issdrv_getH3aConfig(Iss_IspH3aCfg *h3aCfg)
{
    Int32 status = FVID2_SOK;
    UInt32 cnt;
    MSP_H3aAfParamT     *mH3aAfPrms;
    MSP_H3aAewbParamT   *mH3aAewPrms;
    Iss_IspH3aAfCfg *h3aAfCfg;
    Iss_IspH3aAewbCfg *h3aAewCfg;

    GT_assert(GT_DEFAULT_MASK, (NULL != h3aCfg));
    GT_assert(GT_DEFAULT_MASK, (NULL != gIss_captCommonObj.pIssConfig));
    GT_assert(GT_DEFAULT_MASK, (NULL != gIss_captCommonObj.pIssConfig->ptH3aCommonParams));
    GT_assert(GT_DEFAULT_MASK, (NULL != gIss_captCommonObj.pIssConfig->ptH3aAewbParams));
    GT_assert(GT_DEFAULT_MASK, (NULL != gIss_captCommonObj.pIssConfig->ptH3aAfParams));

    /* Check for errors */
    h3aCfg->afCfg.enableALowComp = (UInt32) gIss_captCommonObj.pIssConfig->ptH3aCommonParams->eAfAlawEn;
    h3aCfg->aewbCfg.enableALowComp = (UInt32) gIss_captCommonObj.pIssConfig->ptH3aCommonParams->eAfAlawEn;
    //(UInt32) gIss_captCommonObj.pIssConfig->ptH3aCommonParams.eAfAlawEn.eH3aDecimEnable;
    //(UInt32) gIss_captCommonObj.pIssConfig->ptH3aCommonParams.eAfAlawEn.eIpipeifAveFiltEn;
    h3aCfg->aewbCfg.midFiltThreshold = (UInt32) gIss_captCommonObj.pIssConfig->ptH3aCommonParams->nMedianFilterThreshold;
    h3aCfg->afCfg.midFiltThreshold = h3aCfg->aewbCfg.midFiltThreshold;

    h3aCfg->afCfg.enableMedFilt = (UInt32) gIss_captCommonObj.pIssConfig->ptH3aCommonParams->eAfMedianEn;
    h3aCfg->aewbCfg.enableMedFilt = (UInt32) gIss_captCommonObj.pIssConfig->ptH3aCommonParams->eAewbMedianEn;

    h3aAfCfg = &h3aCfg->afCfg;
    h3aAewCfg = &h3aCfg->aewbCfg;
    mH3aAfPrms = gIss_captCommonObj.pIssConfig->ptH3aAfParams;
    mH3aAewPrms = gIss_captCommonObj.pIssConfig->ptH3aAewbParams;

    /* Copy AF Params*/
    h3aAfCfg->enable = mH3aAfPrms->eAfEnable;
    h3aAfCfg->rgbPos = (Iss_IspH3aAfRgbPos)mH3aAfPrms->eRgbPos;

    h3aAfCfg->fvMode = (Iss_IspH3aAfFvMode)mH3aAfPrms->ePeakModeEn;
    h3aAfCfg->vfMode = (Iss_IspH3aAfVfMode)mH3aAfPrms->eVerticalFocusEn;
    h3aAfCfg->paxelCfg.startX = mH3aAfPrms->ptAfPaxelWin->nHPos;
    h3aAfCfg->paxelCfg.startY = mH3aAfPrms->ptAfPaxelWin->nVPos;
    h3aAfCfg->paxelCfg.width = mH3aAfPrms->ptAfPaxelWin->nHSize;
    h3aAfCfg->paxelCfg.height = mH3aAfPrms->ptAfPaxelWin->nVSize;
    h3aAfCfg->paxelCfg.horzCount = mH3aAfPrms->ptAfPaxelWin->nHCount;
    h3aAfCfg->paxelCfg.vertCount = mH3aAfPrms->ptAfPaxelWin->nVCount;
    h3aAfCfg->paxelCfg.horzIncr = mH3aAfPrms->ptAfPaxelWin->nHIncr;
    h3aAfCfg->paxelCfg.vertIncr = mH3aAfPrms->ptAfPaxelWin->nVIncr;
    //nIirStartPos;                              // AFIIRSH IIRSH
    h3aAfCfg->iirCfg1.threshold = mH3aAfPrms->ptIir1->nHfvThres;
    for (cnt = 0; cnt < ISS_ISP_H3A_AF_IIR_COEFF_MAX; cnt ++)
    {
        h3aAfCfg->iirCfg1.coeff[cnt] = mH3aAfPrms->ptIir1->anIirCoef[cnt];
    }
    h3aAfCfg->iirCfg2.threshold = mH3aAfPrms->ptIir2->nHfvThres;
    for (cnt = 0; cnt < ISS_ISP_H3A_AF_IIR_COEFF_MAX; cnt ++)
    {
        h3aAfCfg->iirCfg2.coeff[cnt] = mH3aAfPrms->ptIir2->anIirCoef[cnt];
    }

    /* Copy AEW Params */
    h3aAewCfg->outMode = (Iss_IspH3aAewbOutMode)mH3aAewPrms->eAeOpFmt;
    h3aAewCfg->sumShift = mH3aAewPrms->nShiftValue;
    h3aAewCfg->satMax = mH3aAewPrms->nSaturationLimit;
    h3aAewCfg->blackRowStartX = mH3aAewPrms->ptBlkWinDims->nHPos;
    h3aAewCfg->blackRowStartY = mH3aAewPrms->ptBlkWinDims->nVPos;

    h3aAewCfg->winCfg.startX = mH3aAewPrms->ptAewbPaxelWin->nHPos;
    h3aAewCfg->winCfg.startY = mH3aAewPrms->ptAewbPaxelWin->nVPos;
    h3aAewCfg->winCfg.width = mH3aAewPrms->ptAewbPaxelWin->nHSize;
    h3aAewCfg->winCfg.height = mH3aAewPrms->ptAewbPaxelWin->nVSize;
    h3aAewCfg->winCfg.horzCount = mH3aAewPrms->ptAewbPaxelWin->nHCount;
    h3aAewCfg->winCfg.vertCount = mH3aAewPrms->ptAewbPaxelWin->nVCount;
    h3aAewCfg->winCfg.horzIncr = mH3aAewPrms->ptAewbPaxelWin->nHIncr;
    h3aAewCfg->winCfg.vertIncr = mH3aAewPrms->ptAewbPaxelWin->nVIncr;

    h3aAewCfg->firCfg1.threshold = mH3aAfPrms->ptFir1->nVfvThres;
    for (cnt = 0; cnt < ISS_ISP_H3A_AF_FIR_COEFF_MAX; cnt ++)
    {
        h3aAewCfg->firCfg1.coeff[cnt] = mH3aAfPrms->ptFir1->anFirCoef[cnt];
    }
    h3aAewCfg->firCfg2.threshold = mH3aAfPrms->ptFir2->nVfvThres;
    for (cnt = 0; cnt < ISS_ISP_H3A_AF_FIR_COEFF_MAX; cnt ++)
    {
        h3aAewCfg->firCfg2.coeff[cnt] = mH3aAfPrms->ptFir2->anFirCoef[cnt];
    }

    //Iss_IspH3aAfMode

    return (status);
}

Int32 Issdrv_setH3aConfig(Iss_IspH3aCfg *h3aCfg, UInt32 algInit)
{
    Int32 status = FVID2_SOK;
    MSP_H3aAfParamT *mH3aAfPrms;
    MSP_H3aAewbParamT *mH3aAewPrms;
    Iss_IspH3aAfCfg *h3aAfCfg;
    Iss_IspH3aAewbCfg *h3aAewCfg;
    Iss_2AObj *p2AObj;
    UInt32 numPaxels;
    UInt32 AEWBBuffSize;
    UInt32 pixCtWin;
    UInt32 cnt;
    Iss_CaptObj *pObj;

    /* Check for errors */
    GT_assert(GT_DEFAULT_MASK, (NULL != h3aCfg));
    GT_assert(GT_DEFAULT_MASK, (NULL != gIss_captCommonObj.pIssConfig));
    GT_assert(GT_DEFAULT_MASK, (NULL != gIss_captCommonObj.pIssConfig->ptH3aCommonParams));
    GT_assert(GT_DEFAULT_MASK, (NULL != gIss_captCommonObj.pIssConfig->ptH3aAewbParams));
    GT_assert(GT_DEFAULT_MASK, (NULL != gIss_captCommonObj.pIssConfig->ptH3aAfParams));

    pObj = &gIss_captCommonObj.captureObj[0];

    p2AObj = (Iss_2AObj*)gIss_captCommonObj.pModuleInstance->p2AObj;

    gIss_captCommonObj.pIssConfig->ptH3aCommonParams->eAfAlawEn = (MSP_H3A_FEATURE_ENABLE_T)h3aCfg->afCfg.enableALowComp;
    gIss_captCommonObj.pIssConfig->ptH3aCommonParams->eAfAlawEn = (MSP_H3A_FEATURE_ENABLE_T)h3aCfg->aewbCfg.enableALowComp;
    //(UInt32) gIss_captCommonObj.pIssConfig->ptH3aCommonParams.eAfAlawEn.eH3aDecimEnable;
    //(UInt32) gIss_captCommonObj.pIssConfig->ptH3aCommonParams.eAfAlawEn.eIpipeifAveFiltEn;
    gIss_captCommonObj.pIssConfig->ptH3aCommonParams->nMedianFilterThreshold = h3aCfg->aewbCfg.midFiltThreshold;
    h3aCfg->aewbCfg.midFiltThreshold = h3aCfg->afCfg.midFiltThreshold;

    gIss_captCommonObj.pIssConfig->ptH3aCommonParams->eAfMedianEn = (MSP_H3A_FEATURE_ENABLE_T)h3aCfg->afCfg.enableMedFilt;
    gIss_captCommonObj.pIssConfig->ptH3aCommonParams->eAewbMedianEn = (MSP_H3A_FEATURE_ENABLE_T)h3aCfg->aewbCfg.enableMedFilt;

    h3aAfCfg = &h3aCfg->afCfg;
    h3aAewCfg = &h3aCfg->aewbCfg;
    mH3aAfPrms = gIss_captCommonObj.pIssConfig->ptH3aAfParams;
    mH3aAewPrms = gIss_captCommonObj.pIssConfig->ptH3aAewbParams;

    /* Copy AF Params*/
    mH3aAfPrms->eAfEnable = (MSP_H3A_FEATURE_ENABLE_T)h3aAfCfg->enable;
    mH3aAfPrms->eRgbPos = (MSP_H3A_RGB_POS_T)h3aAfCfg->rgbPos;

    mH3aAfPrms->ePeakModeEn = (MSP_H3A_FEATURE_ENABLE_T)h3aAfCfg->fvMode;
    mH3aAfPrms->eVerticalFocusEn = (MSP_H3A_FEATURE_ENABLE_T)h3aAfCfg->vfMode;
    mH3aAfPrms->ptAfPaxelWin->nHPos = h3aAfCfg->paxelCfg.startX;
    mH3aAfPrms->ptAfPaxelWin->nVPos = h3aAfCfg->paxelCfg.startY;
    mH3aAfPrms->ptAfPaxelWin->nHSize = h3aAfCfg->paxelCfg.width;
    mH3aAfPrms->ptAfPaxelWin->nVSize = h3aAfCfg->paxelCfg.height;
    mH3aAfPrms->ptAfPaxelWin->nHCount = h3aAfCfg->paxelCfg.horzCount;
    mH3aAfPrms->ptAfPaxelWin->nVCount = h3aAfCfg->paxelCfg.vertCount;
    mH3aAfPrms->ptAfPaxelWin->nHIncr = h3aAfCfg->paxelCfg.horzIncr;
    mH3aAfPrms->ptAfPaxelWin->nVIncr = h3aAfCfg->paxelCfg.vertIncr;
    //nIirStartPos;                              // AFIIRSH IIRSH
    mH3aAfPrms->ptIir1->nHfvThres = h3aAfCfg->iirCfg1.threshold;
    for (cnt = 0; cnt < ISS_ISP_H3A_AF_IIR_COEFF_MAX; cnt ++)
    {
        mH3aAfPrms->ptIir1->anIirCoef[cnt] = h3aAfCfg->iirCfg1.coeff[cnt];
    }
    mH3aAfPrms->ptIir2->nHfvThres = h3aAfCfg->iirCfg2.threshold;
    for (cnt = 0; cnt < ISS_ISP_H3A_AF_IIR_COEFF_MAX; cnt ++)
    {
        mH3aAfPrms->ptIir2->anIirCoef[cnt] = h3aAfCfg->iirCfg2.coeff[cnt];
    }

    /* Copy AEW Params */
    mH3aAewPrms->eAeOpFmt = (MSP_H3A_AEWB_OP_FMT_T)h3aAewCfg->outMode;
    mH3aAewPrms->nShiftValue = h3aAewCfg->sumShift;
    mH3aAewPrms->nSaturationLimit = h3aAewCfg->satMax;
    mH3aAewPrms->ptBlkWinDims->nHPos = h3aAewCfg->blackRowStartX;
    mH3aAewPrms->ptBlkWinDims->nVPos = h3aAewCfg->blackRowStartY;

    mH3aAewPrms->ptAewbPaxelWin->nHPos = h3aAewCfg->winCfg.startX;
    mH3aAewPrms->ptAewbPaxelWin->nVPos = h3aAewCfg->winCfg.startY;
    mH3aAewPrms->ptAewbPaxelWin->nHSize = h3aAewCfg->winCfg.width;
    mH3aAewPrms->ptAewbPaxelWin->nVSize = h3aAewCfg->winCfg.height;
    mH3aAewPrms->ptAewbPaxelWin->nHCount = h3aAewCfg->winCfg.horzCount;
    mH3aAewPrms->ptAewbPaxelWin->nVCount = h3aAewCfg->winCfg.vertCount;
    mH3aAewPrms->ptAewbPaxelWin->nHIncr = h3aAewCfg->winCfg.horzIncr;
    mH3aAewPrms->ptAewbPaxelWin->nVIncr = h3aAewCfg->winCfg.vertIncr;

    mH3aAfPrms->ptFir1->nVfvThres = h3aAewCfg->firCfg1.threshold;
    for (cnt = 0; cnt < ISS_ISP_H3A_AF_FIR_COEFF_MAX; cnt ++)
    {
        mH3aAfPrms->ptFir1->anFirCoef[cnt] = h3aAewCfg->firCfg1.coeff[cnt];
    }
    mH3aAfPrms->ptFir2->nVfvThres = h3aAewCfg->firCfg2.threshold;
    for (cnt = 0; cnt < ISS_ISP_H3A_AF_FIR_COEFF_MAX; cnt ++)
    {
        mH3aAfPrms->ptFir2->anFirCoef[cnt] = h3aAewCfg->firCfg2.coeff[cnt];
    }

    //Iss_IspH3aAfMode


#ifdef IMGS_OMNIVISION_OV7740
    pixCtWin =
        ((gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVSize+ gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVIncr -1 ) /
         gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVIncr) *
        ((gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHSize+  gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHIncr - 1) /
         gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHIncr);
#else
    pixCtWin =
        (gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVSize /
         gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVIncr) *
        (gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHSize /
         gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHIncr);
#endif

    numPaxels =
        (gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVCount +
         1) * gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHCount;
    AEWBBuffSize = (numPaxels * 32) + (((numPaxels + 7) / 8) * 32);

    gIss_captCommonObj.gH3ABufParams.h3aUpdate = 1;
    gIss_captCommonObj.gH3ABufParams.emptyFrame = 1;
    gIss_captCommonObj.gH3ABufParams.aewbNumPix = pixCtWin;
    gIss_captCommonObj.gH3ABufParams.aewbNumWinH =
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHCount;
    gIss_captCommonObj.gH3ABufParams.aewbNumWinV =
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVCount;

    p2AObj->pIspHandle      = gIss_captCommonObj.pModuleInstance->hIspHandle;
    p2AObj->pIssConfig      = gIss_captCommonObj.pIssConfig;
    p2AObj->h3aBufSize      = AEWBBuffSize;
    p2AObj->aewbNumPix      = pixCtWin;
    p2AObj->aewbNumWinH     = gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHCount;
    p2AObj->aewbNumWinV     = gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVCount;
    p2AObj->aewbVendor      = (AEWB_VENDOR)pObj->createArgs.aewbVendor;
    p2AObj->aewbMode        = pObj->createArgs.aewbMode;
    p2AObj->aewbModeFlag    = 1;
    p2AObj->aewbPriority    = pObj->createArgs.aewbPriority;
    p2AObj->SensorHandle    = pObj->createArgs.SensorHandle;
    p2AObj->cameraVipHandle = pObj;

    gIss_captCommonObj.pIssConfig->eFH3aValidity =
        (MSP_PROC_H3A_VALID_ID) (MSP_PROC_H3A_VALID_AEWB |
                                 MSP_PROC_H3A_VALID_H3A |
                                 MSP_PROC_H3A_VALID_AF);

    if (algInit)
        IssAlg_capt2AInit((Iss_2AObj*)gIss_captCommonObj.pModuleInstance->p2AObj , TRUE);
    else
        IssAlg_capt2AInit((Iss_2AObj*)gIss_captCommonObj.pModuleInstance->p2AObj, FALSE);


    memcpy(&pObj->h3aCfg, h3aCfg, sizeof(pObj->h3aCfg));

    return (status);
}

#if 0
Void *IssCdrv_init2AObj(UInt32 sensorId, Fdrv_Handle SensorHandle, UInt32 isIsifCapt)
{
	UInt32 numPaxels;
	Iss_2AObj *p2AObj = NULL;

	p2AObj = malloc(sizeof(Iss_2AObj));

	if (NULL != p2AObj)
	{
	    numPaxels = (32 + 1) * 16;

	    p2AObj->pIspHandle      = gIss_captCommonObj.pModuleInstance->hIspHandle;
	    p2AObj->pIssConfig      = gIss_captCommonObj.pIssConfig;
	    p2AObj->h3aBufSize      = (numPaxels * 32) + (((numPaxels + 7) / 8) * 32);
	    p2AObj->aewbNumPix      = (32 / 8) * (120 / 8);
	    p2AObj->aewbNumWinH     = 16;
	    p2AObj->aewbNumWinV     = 32;
	    p2AObj->aewbVendor      = AEWB_ID_TI;
	    p2AObj->aewbMode        = 3;
	    p2AObj->aewbModeFlag    = 1;
	    p2AObj->aewbPriority    = 0;
	    p2AObj->SensorHandle    = SensorHandle;

	    if (isIsifCapt)
	    	p2AObj->cameraVipHandle = &gIss_captCommonObj.captureObj[0];
		else
	    	p2AObj->cameraVipHandle = NULL;

	    p2AObj->sensorId 		= sensorId;

	    gIss_captCommonObj.pIssConfig->eFH3aValidity =
	        (MSP_PROC_H3A_VALID_ID) (MSP_PROC_H3A_VALID_AEWB |
	                                 MSP_PROC_H3A_VALID_H3A |
	                                 MSP_PROC_H3A_VALID_AF);

		IssAlg_capt2AInit(p2AObj, TRUE);
	}

	return (Void *)p2AObj;
}

Void IssCdrv_reInit2AObj(Void *pObj)
{
	Iss_2AObj *p2AObj = (Iss_2AObj *)pObj;
	if (NULL != p2AObj)
	{
	    gIss_captCommonObj.pIssConfig->eFH3aValidity =
	        (MSP_PROC_H3A_VALID_ID) (MSP_PROC_H3A_VALID_AEWB |
	                                 MSP_PROC_H3A_VALID_H3A |
	                                 MSP_PROC_H3A_VALID_AF);

		IssAlg_capt2AInit(p2AObj, FALSE);
	}
}

Void IssCdrv_deInit2AObj(Void *pObj)
{
	Iss_2AObj *p2AObj = (Iss_2AObj *)pObj;
	if (NULL != p2AObj)
	{
		IssAlg_capt2ADeInit(p2AObj);

		free(p2AObj);
	}
}
#endif

Void Issdrv_captSetDefaultH3APrms()
{
    UInt32 numPaxels, pixCtWin, AEWBBuffSize;
    UInt32 count;
    Iss_CaptObj *pObj;
    Iss_2AObj *p2AObj;

    pObj = &gIss_captCommonObj.captureObj[0];

    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVCount = 32;
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHCount = 16;
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVSize = 32;
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHSize = 120;

    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHIncr = 8;
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVIncr = 8;
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVPos = 30;

#ifdef IMGS_PANASONIC_MN34041
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHPos = 96;
#elif defined(APPRO_SENSOR_VENDOR) && defined(IMGS_SONY_IMX136)
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHPos = 31;
#elif defined (IMGS_SONY_IMX136) || defined (IMGS_SONY_IMX140)
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHPos = 32;
#elif defined IMGS_SONY_IMX104
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHPos = 8;
#else
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHPos = 0;
#endif

    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptBlkWinDims->nVPos = 0;
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptBlkWinDims->nHPos = 2;

    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->nShiftValue = 0;
    gIss_captCommonObj.pIssConfig->ptH3aAewbParams->nSaturationLimit = 0x3FE;

    if ((pObj->inFmt.dataFormat == FVID2_DF_BAYER_RAW) ||
        (pObj->inFmt.dataFormat == FVID2_DF_RAW))
    {
        gIss_captCommonObj.pIssConfig->ptH3aAfParams->eAfEnable = MSP_H3A_FEATURE_ENABLE;
        gIss_captCommonObj.pIssConfig->ptH3aAfParams->eRgbPos = MSP_H3A_AF_RGBPOS_RG_GB_BAYER;
        gIss_captCommonObj.pIssConfig->ptH3aAfParams->ePeakModeEn = MSP_H3A_FEATURE_ENABLE;
        gIss_captCommonObj.pIssConfig->ptH3aAfParams->eVerticalFocusEn = MSP_H3A_FEATURE_ENABLE;
        gIss_captCommonObj.pIssConfig->ptH3aAfParams->nIirStartPos = 30;
        gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptAfPaxelWin->nVPos = 34;
        gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptAfPaxelWin->nHPos = 32;

        gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptAfPaxelWin->nVCount = 16;
        gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptAfPaxelWin->nVIncr = 8;
        gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptAfPaxelWin->nHCount = 6;
        gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptAfPaxelWin->nHIncr = 8;

        for(count=0;count<ISS_ISP_H3A_AF_IIR_COEFF_MAX;count++)
        {
            gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptIir1->anIirCoef[count] = anIirCoef1[count];
            gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptIir2->anIirCoef[count] = anIirCoef2[count];
        }

        gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptIir1->nHfvThres = 100;
        gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptIir2->nHfvThres = 100;

        for(count=0;count<ISS_ISP_H3A_AF_FIR_COEFF_MAX;count++)
        {
            gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptFir1->anFirCoef[count] = 2;
            gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptFir2->anFirCoef[count] = 2;
        }

        gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptFir1->nVfvThres = 500;
        gIss_captCommonObj.pIssConfig->ptH3aAfParams->ptFir2->nVfvThres = 500;
    }

    numPaxels =
        (gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVCount +
         1) * gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHCount;

    AEWBBuffSize = (numPaxels * 32) + (((numPaxels + 7) / 8) * 32);

#ifdef IMGS_OMNIVISION_OV7740
    pixCtWin =
        ((gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVSize+ gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVIncr -1 ) /
         gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVIncr) *
        ((gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHSize+  gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHIncr - 1) /
         gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHIncr);
#else
    pixCtWin =
        (gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVSize /
         gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVIncr) *
        (gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHSize /
         gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHIncr);
#endif

    gIss_captCommonObj.gH3ABufParams.h3aUpdate = 1;
    gIss_captCommonObj.gH3ABufParams.emptyFrame = 1;
    gIss_captCommonObj.gH3ABufParams.aewbNumPix = pixCtWin;
    gIss_captCommonObj.gH3ABufParams.aewbNumWinH =
        gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHCount;
    gIss_captCommonObj.gH3ABufParams.aewbNumWinV =
        gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVCount;

    p2AObj = (Iss_2AObj*)gIss_captCommonObj.pModuleInstance->p2AObj;

    p2AObj->pIspHandle      = gIss_captCommonObj.pModuleInstance->hIspHandle;
    p2AObj->pIssConfig      = gIss_captCommonObj.pIssConfig;
    p2AObj->h3aBufSize      = AEWBBuffSize;
    p2AObj->aewbNumPix      = pixCtWin;
    p2AObj->aewbNumWinH     = gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nHCount;
    p2AObj->aewbNumWinV     = gIss_captCommonObj.pIssConfig->ptH3aAewbParams->ptAewbPaxelWin->nVCount;
    p2AObj->aewbVendor      = (AEWB_VENDOR)pObj->createArgs.aewbVendor;
    p2AObj->aewbMode        = pObj->createArgs.aewbMode;
    p2AObj->aewbModeFlag    = 1;
    p2AObj->aewbPriority    = pObj->createArgs.aewbPriority;
    p2AObj->SensorHandle    = pObj->createArgs.SensorHandle;
    p2AObj->cameraVipHandle = pObj;
    //p2AObj->sensorId = pObj->createArgs.sensorId;
}


Void IssCdrv_setIpipeCfg()
{
#ifdef IMGS_OMNIVISION_OV7740
    ipipe_reg->SRC_COL  = 27; //OV7740
    ipipe_reg->SRC_VSZ  = 481;
    ipipe_reg->SRC_HSZ  = 641;

    ipipe_reg->WB2_WGN_R = 600;//OV7740
    ipipe_reg->WB2_WGN_B = 750;//OV7740
    ipipe_reg->RGB1_MUL_RR = 256;
    ipipe_reg->RGB1_MUL_GR = 0;
    ipipe_reg->RGB1_MUL_BR = 0;
    ipipe_reg->RGB1_MUL_RG = 0;
    ipipe_reg->RGB1_MUL_GG = 256;
    ipipe_reg->RGB1_MUL_BG = 0;
    ipipe_reg->RGB1_MUL_RB = 0;
    ipipe_reg->RGB1_MUL_GB = 0;
    ipipe_reg->RGB1_MUL_BB = 256;
    *(MSP_U32 *)(0x5505006C) &= 0xBFFFFF;
#else

#ifdef IMGS_PANASONIC_MN34041
    ipipe_reg->SRC_COL = 0xE4;
    #ifdef MN34041_DATA_OP_LVDS324
    ipipe_reg->SRC_COL = 0xB1;
    #endif
#elif defined IMGS_OMNIVISION_OV2715
    ipipe_reg->SRC_COL = 0xE4;
#elif defined IMGS_SONY_IMX036
    ipipe_reg->SRC_COL = 0x4E;
#elif defined IMGS_SONY_IMX136
    ipipe_reg->SRC_COL = 0x1B;
#elif defined IMGS_SONY_IMX104
    ipipe_reg->SRC_COL = 0x4E;
#elif defined IMGS_SONY_IMX140
    ipipe_reg->SRC_COL = 0x4E;
#elif defined IMGS_ALTASENS_AL30210
    ipipe_reg->SRC_COL = 0x1B;
#else
    ipipe_reg->SRC_COL = 177;
#endif
#endif

#if defined(IMGS_OMNIVISION_OV10630) || defined(IMGS_OMNIVISION_OV2710)
    ipipeif_reg->CFG2 = 0xCE;

    ipipe_reg->SRC_COL  = 27;
    ipipe_reg->SRC_VSZ  = 721;
    ipipe_reg->SRC_HSZ  = 1281;

    gIss_captCommonObj.pModuleInstance->nLscTableIsValid       = MSP_TRUE;

    ipipe_reg->WB2_WGN_R = 600;
    ipipe_reg->WB2_WGN_B = 750;
    ipipe_reg->RGB1_MUL_RR = 256;
    ipipe_reg->RGB1_MUL_GR = 0;
    ipipe_reg->RGB1_MUL_BR = 0;
    ipipe_reg->RGB1_MUL_RG = 0;
    ipipe_reg->RGB1_MUL_GG = 256;
    ipipe_reg->RGB1_MUL_BG = 0;
    ipipe_reg->RGB1_MUL_RB = 0;
    ipipe_reg->RGB1_MUL_GB = 0;
    ipipe_reg->RGB1_MUL_BB = 256;
    *(MSP_U32 *)(0x5505006C) &= 0xBFFFFF;

#endif
#ifdef IMGS_SONY_IMX104
    #define VSTAB_MULT_COEFFICIENT  1.2
    ////////////////Ipipe////////////////////
    //ipipe_reg->SRC_COL = 177;
    //ipipe_reg->SRC_HSZ = 1297;//1937;
    //ipipe_reg->SRC_HSZ = 1937;
    //ipipe_reg->SRC_VSZ  = 976;
    ////////////////RSZ////////////////////
    gIss_captCommonObj.pModuleInstance->nLscTableIsValid       = MSP_TRUE;
#endif

    // ///////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef IMGS_MICRON_MT9M034_DUAL_HEAD_BOARD
    rsz_reg->SRC_VSZ    = 715;
    rsz_reg->SRC_HSZ    = 1917;
#endif

#ifdef IMGS_MICRON_MT9M034
#define VSTAB_MULT_COEFFICIENT  1.2
    isif_reg->VDINT0 = 731;//715;// 720;
    isif_reg->VDINT1 = 731;//715;//720;
    isif_reg->VDINT2 = 731;//715;//720;
    ////////////////Ipipe////////////////////
    //ipipe_reg->SRC_COL = 177;
    ipipe_reg->SRC_HSZ = 1297;//1937;
    //ipipe_reg->SRC_HSZ = 1937;
    ipipe_reg->SRC_VSZ  = 739;//723;
    ipipe_reg->SRC_VPS = 16;
    ////////////////RSZ////////////////////
    gIss_captCommonObj.pModuleInstance->nLscTableIsValid       = MSP_TRUE;
#endif

#ifdef IMGS_SONY_IMX036
    gIss_captCommonObj.pIssConfig->eMsbPos = MSP_IPIPE_BAYER_MSB_BIT13;
#endif

#ifdef IMGS_SONY_IMX122
    /* SONY IMX122 uses the following Bayer pattern

            +-----+-----+
            |  Gb |  B  |
            +-----+-----+
            |  R  |  Gr |
            +-----+-----+
    */

    ipipe_reg->SRC_COL = 0xE4;

    isif_reg->VDINT0 = 1000;// 720;
    isif_reg->VDINT1 = 1000;//720;
    isif_reg->VDINT2 = 1000;//720;

    ipipe_reg->SRC_HPS = 140;
    ipipe_reg->SRC_VPS = 16;
#endif

#if defined(APPRO_SENSOR_VENDOR) && defined(IMGS_SONY_IMX136)
	gIss_captCommonObj.pIssConfig->eMsbPos = MSP_IPIPE_BAYER_MSB_BIT13;
	isif_reg->CLDCOFST = 0;
#endif	

}


Int32 Issdrv_setIpipeIfDfsParams(Iss_IspIpipeifDfs *dfsPrms)
{
    Int32 status = FVID2_SOK;
    ipipeif_ip_dfs_t dfsCfg;

    dfsCfg.enable = dfsPrms->enable;

    if (ISS_ISP_IPIPEIF_DFS_INP_ISIF == dfsPrms->inpSource)
    {
        dfsCfg.muxSel = IPIPEIF_ISIF_MUX;
    }
    else
    {
        dfsCfg.muxSel = IPIPEIF_IPIPE_MUX;
    }

    dfsCfg.data_shift = DATASFT_BITS11_0;
    dfsCfg.unpack = UNPACK_NORMAL_16_BITS_PIXEL;

    dfsCfg.crop_dims.ppln_hs_interval = gIss_captCommonObj.captureObj[0].isifCfg.startX;
    dfsCfg.crop_dims.lpfr_vs_interval = 1;
    dfsCfg.crop_dims.hnum = gIss_captCommonObj.captureObj[0].isifCfg.outWidth;
    dfsCfg.crop_dims.vnum = gIss_captCommonObj.captureObj[0].isifCfg.outHeight;

    dfsCfg.address = dfsPrms->addr;
    dfsCfg.adofs = dfsPrms->pitch;

    if (dfsPrms->direction == ISS_ISP_IPIPEIF_DFS_DIR_SENSOR_MINUS_DF)
        dfsCfg.dfs_dir = DFSDIR_SENSOR_MINUS_SDRAM;
    else
        dfsCfg.dfs_dir = DFSDIR_SDRAM_MINUS_SENSOR;

    status = ipipeif_set_dfs_config(&dfsCfg);

    if (ISP_SUCCESS != status)
    {
        status = FVID2_EINVALID_PARAMS;
    }

    return (status);
}

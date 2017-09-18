/** ==================================================================
 *  @file   issdrv_algVstabApi.c
 *
 *  @path   /ti/psp/iss/drivers/alg/vstab/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <xdc/std.h>
#include <ti/psp/iss/drivers/alg/vstab/alg_vstab.h>
#include <ti/psp/iss/drivers/capture/src/issdrv_capturePriv.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif.h>
#include <ti/psp/iss/hal/iss/isp/rsz/inc/rsz.h>
#include <ti/psp/iss/hal/iss/isp/ipipe/inc/ipipe.h>
#include <ti/psp/iss/core/inc/iss_drv_common.h>
#include <ti/psp/iss/core/inc/iss_drv.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/iss/drivers/alg/vstab/src/issdrv_algVstabPriv.h>

#define VS_floor(value, align)   (( (value) / (align) ) * (align) )
/* Stack for capture Alg processing task */
#pragma DATA_ALIGN(gIssAlg_captTskStackUpdate, 32)
#pragma DATA_SECTION(gIssAlg_captTskStackUpdate, ".bss:taskStackSection")

UInt8 gIssAlg_captTskStackUpdate[ISSALG_CAPT_TSK_STACK_UPDATE];

//#define VS_PROFILE

#ifdef VS_PROFILE
    extern UInt32 Utils_getCurTimeInMsec();
#endif

Int32 Iss_ispPostBscSem(UInt32 prevBscBufAddr,UInt32 curBscBufAddr);
Bool Iss_ispGetVsRunStatus();

/*========================================================================= */
/*========================================================================= */

VIDEO_VsStream gVsStream = {.algVsHndl = NULL};

tStreamBuf tempStreamBuf[2];

/* ===================================================================
 *  @func     IssAlg_ProcessFunction
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
Int32 IssAlg_ProcessFunction();

IssAlg_CaptObj gIssAlg_captObj;

/* ===================================================================
 *  @func     VIDEO_vsTskCreate
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
int VIDEO_vsTskCreate()
{
    Iss_CaptObj *pObj = &gIss_captCommonObj.captureObj[0];
    ipipe_bsc_cfg_t bscConfig;

    gVsStream.vsCreatePrm.totalFrameWidth = pObj->inFmt.width;      //1920;
    gVsStream.vsCreatePrm.totalFrameHeight = pObj->inFmt.height;    //1080;

    if (!gVsStream.algVsHndl)
        gVsStream.algVsHndl = ALG_vstabCreate(&gVsStream.vsCreatePrm);

    // Configure BSC with the parameters returned by the VS alg create
    bscConfig.row_pos.vectors = gVsStream.vsCreatePrm.bsc_row_vct;
    bscConfig.row_pos.v_pos = gVsStream.vsCreatePrm.bsc_row_vpos;
    bscConfig.row_pos.v_num = gVsStream.vsCreatePrm.bsc_row_vnum;
    bscConfig.row_pos.v_skip = gVsStream.vsCreatePrm.bsc_row_vskip;
    bscConfig.row_pos.h_pos = gVsStream.vsCreatePrm.bsc_row_hpos;
    bscConfig.row_pos.h_num = gVsStream.vsCreatePrm.bsc_row_hnum;
    bscConfig.row_pos.h_skip = gVsStream.vsCreatePrm.bsc_row_hskip;
    bscConfig.row_pos.shift = gVsStream.vsCreatePrm.bsc_row_shf;

    bscConfig.col_pos.vectors = gVsStream.vsCreatePrm.bsc_col_vct;
    bscConfig.col_pos.v_pos = gVsStream.vsCreatePrm.bsc_col_vpos;
    bscConfig.col_pos.v_num = gVsStream.vsCreatePrm.bsc_col_vnum;
    bscConfig.col_pos.v_skip = gVsStream.vsCreatePrm.bsc_col_vskip;
    bscConfig.col_pos.h_pos = gVsStream.vsCreatePrm.bsc_col_hpos;
    bscConfig.col_pos.h_num = gVsStream.vsCreatePrm.bsc_col_hnum;
    bscConfig.col_pos.h_skip = gVsStream.vsCreatePrm.bsc_col_hskip;
    bscConfig.col_pos.shift = gVsStream.vsCreatePrm.bsc_col_shf;

    bscConfig.enable = TRUE;
    bscConfig.mode = 0;
    bscConfig.col_sample = 1;
    bscConfig.row_sample = 1;
    bscConfig.element = 0;

    if (ipipe_config_bsc(&bscConfig) != FVID2_SOK)
    {
        Vps_rprintf("DRV_ipipeBscUpdateParams()\n");
        return FVID2_EFAIL;
    }

    if (gVsStream.algVsHndl == NULL)
    {
        Vps_rprintf("ALG_vstabCreate(%dx%d)\n",
                    gVsStream.vsCreatePrm.totalFrameWidth,
                    gVsStream.vsCreatePrm.totalFrameHeight);
        return FVID2_EFAIL;
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VIDEO_vsTskDelete
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
int VIDEO_vsTskDelete()
{
    int status;

    status = ALG_vstabDelete(gVsStream.algVsHndl);

    gVsStream.algVsHndl = NULL;

    return status;
}

/* ===================================================================
 *  @func     video_bsc_copyBuffers
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
void video_bsc_copyBuffers()
{
    Iss_CaptObj *pObj  = &gIss_captCommonObj.captureObj[0];;
    ALG_VsObj *pAlgObj = (ALG_VsObj *) gVsStream.algVsHndl;

	if(pObj->vsEnable == 1)
	{
        memcpy((void *) (pAlgObj->bscBufAddr[pAlgObj->curBscBufIdx]),
               (const void *) 0x55054000, DRV_IPIPE_BSC_BUFFER_MAX);

        pAlgObj->prevBscBufAddr = pAlgObj->curBscBufAddr;
        pAlgObj->curBscBufAddr = pAlgObj->bscBufAddr[pAlgObj->curBscBufIdx];

        /* swap the buffer index */
        pAlgObj->curBscBufIdx = pAlgObj->curBscBufIdx ? 0 : 1;

        Semaphore_post(gIss_captCommonObj.semUpdate);
	}
	
    if(Iss_ispGetVsRunStatus() == TRUE)
    {
        Iss_ispPostBscSem(pAlgObj->prevBscBufAddr,pAlgObj->curBscBufAddr);
    }
}

/* ===================================================================
 *  @func     video_bsc_handle
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
void video_bsc_handle()
{
    ALG_VstabStatus vstabStatus;

    ALG_VstabRunPrm runPrm;

    Int32 offset, offset1, offset2, offset3;

    Int32 startX0, startY0;

    Int32 startX1, startY1;

    Iss_CaptObj *pObj;

    ALG_VsObj *pAlgObj = (ALG_VsObj *) gVsStream.algVsHndl;

    UInt32 nonStabilizedWidth0,nonStabilizedHeight0;
    UInt32 nonStabilizedWidth1,nonStabilizedHeight1;

    pObj = &gIss_captCommonObj.captureObj[0];

    if (pObj->vsEnable == 0)
    {
        if (tempStreamBuf[0].ptmpFrame != NULL)
        {
	        tempStreamBuf[0].ptmpFrame->addr[0][0] =
	            tempStreamBuf[0].ptmpFrame->addr[1][0];
	        tempStreamBuf[0].ptmpFrame->addr[0][1] =
	            tempStreamBuf[0].ptmpFrame->addr[1][1];
        }
        if (tempStreamBuf[1].ptmpFrame != NULL)
        {
            tempStreamBuf[1].ptmpFrame->addr[0][0] =
                tempStreamBuf[1].ptmpFrame->addr[1][0];
            tempStreamBuf[1].ptmpFrame->addr[0][1] =
                tempStreamBuf[1].ptmpFrame->addr[1][1];
        }
    }
    else
    {
        {

            runPrm.curBscDataVirtAddr = (Uint8 *) pAlgObj->curBscBufAddr;
            runPrm.curBscDataPhysAddr = NULL;
            runPrm.prevBscDataVirtAddr = (Uint8 *) pAlgObj->prevBscBufAddr;
            runPrm.prevBscDataPhysAddr = NULL;

            ALG_vstabRun(gVsStream.algVsHndl, &runPrm, &vstabStatus);

            nonStabilizedWidth0  = (pObj->createArgs.rszPrms.rPrms[0].outWidth * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
            nonStabilizedHeight0 = (pObj->createArgs.rszPrms.rPrms[0].outHeight * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
            nonStabilizedWidth1  = (pObj->createArgs.rszPrms.rPrms[1].outWidth * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
            nonStabilizedHeight1 = (pObj->createArgs.rszPrms.rPrms[1].outHeight * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;

            #if 0
            {
                static Uint16 sx = 0,sy=0; 

                if ((sx != vstabStatus.startX)||(sy != vstabStatus.startY))
                {
                    sx = vstabStatus.startX;sy = vstabStatus.startY;
                    Vps_rprintf("startXY %d %d\n",vstabStatus.startX,vstabStatus.startY);
                }
            }
            #endif
            
            startX0 = (nonStabilizedWidth0 * vstabStatus.startX)/gVsStream.vsCreatePrm.totalFrameWidth;
            startX0 = VS_floor(startX0,4);

            startY0 = (nonStabilizedHeight0 * vstabStatus.startY)/gVsStream.vsCreatePrm.totalFrameHeight;
            startY0 = VS_floor(startY0,2);


            startX1 = (nonStabilizedWidth1 * vstabStatus.startX)/gVsStream.vsCreatePrm.totalFrameWidth;
            startX1 = VS_floor(startX1,4);

            startY1 = (nonStabilizedHeight1 * vstabStatus.startY)/gVsStream.vsCreatePrm.totalFrameHeight;
            startY1 = VS_floor(startY1,2);

            offset  = (startY1 * pObj->createArgs.outStreamInfo[1].pitch[0] + startX1);
            offset1 = (startY1 * (pObj->createArgs.outStreamInfo[1].pitch[1] >> 1) + startX1);

            offset2 = (startY0 * pObj->createArgs.outStreamInfo[0].pitch[0] + startX0);
            offset3 = (startY0 * (pObj->createArgs.outStreamInfo[0].pitch[1] >> 1) + startX0);

            if (tempStreamBuf[0].ptmpFrame != NULL)
            if((pObj->createArgs.outStreamInfo[0].dataFormat == FVID2_DF_YUV420SP_UV) ||
               (pObj->createArgs.outStreamInfo[0].dataFormat == FVID2_DF_YUV420SP_VU))
            {
                tempStreamBuf[0].ptmpFrame->addr[0][0] = (Ptr) (((Int32) tempStreamBuf[0].ptmpFrame->addr[1][0] + offset2) & 0xFFFFFFE0);
                tempStreamBuf[0].ptmpFrame->addr[0][1] = (Ptr) (((Int32) tempStreamBuf[0].ptmpFrame->addr[1][1] + (offset3)) & 0xFFFFFFE0);
            }
            else
            {
                tempStreamBuf[0].ptmpFrame->addr[0][0] = (Ptr) (((Int32) tempStreamBuf[0].ptmpFrame->addr[1][0] + (offset2)) & 0xFFFFFFE0);
            }

            if (tempStreamBuf[1].ptmpFrame != NULL)
            {
                if (pObj->vsDemoEnable == 1)
                {
                    tempStreamBuf[1].ptmpFrame->addr[0][0] =
                        tempStreamBuf[1].ptmpFrame->addr[1][0];
                    tempStreamBuf[1].ptmpFrame->addr[0][1] =
                        tempStreamBuf[1].ptmpFrame->addr[1][1];
                }
                else{
                    if((pObj->createArgs.outStreamInfo[1].dataFormat == FVID2_DF_YUV420SP_UV) ||
                       (pObj->createArgs.outStreamInfo[1].dataFormat == FVID2_DF_YUV420SP_VU))
                    {
                        tempStreamBuf[1].ptmpFrame->addr[0][0] = (Ptr) (((Int32) tempStreamBuf[1].ptmpFrame->addr[1][0] + offset) & 0xFFFFFFE0);
                        tempStreamBuf[1].ptmpFrame->addr[0][1] = (Ptr) (((Int32) tempStreamBuf[1].ptmpFrame->addr[1][1] + (offset1)) & 0xFFFFFFE0);
                    }
                    else
                    {
                        tempStreamBuf[1].ptmpFrame->addr[0][0] = (Ptr) (((Int32) tempStreamBuf[1].ptmpFrame->addr[1][0] + (offset)) & 0xFFFFFFE0);
                    }
                }
            }
#ifdef ISS_DEBUG_RT
            Vps_rprintf
                ("BSC: startx = %d \tcurStreamBuf[0].addr[0][0] = %x \t tempStreamBuf[0].addr[1][0] = %x",
                 vstabStatus.startX, tempStreamBuf[0].ptmpFrame->addr[0][0],
                 tempStreamBuf[0].ptmpFrame->addr[1][0]);
            Vps_rprintf
                ("BSC: startx = %d \tcurStreamBuf[1].addr[0][0] = %x \t tempStreamBuf[1].addr[1][0] = %x",
                 startX1, tempStreamBuf[1].ptmpFrame->addr[0][0],
                 tempStreamBuf[1].ptmpFrame->addr[1][0]);
#endif

        }
    }
}

// VS ALG create
/* ===================================================================
 *  @func     VS_ALG_create
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
IALG_Handle VS_ALG_create(IALG_Fxns * fxns, IALG_Handle p, IALG_Params * params)
{
    IALG_MemRec memTab[2];

    IALG_Handle alg;

    IALG_Fxns *fxnsPtr;

    int n;

    n = fxns->algAlloc(params, &fxnsPtr, memTab);

    if (n <= 0)
    {
        return (NULL);
    }

    // Allocate memory requested by VS algorithm
    memTab[0].base = memalign(32, memTab[0].size);
    memTab[1].base = memalign(32, memTab[1].size);

    alg = (IALG_Handle) memTab[0].base;
    alg->fxns = fxns;

    if (fxns->algInit(alg, memTab, p, params) == IALG_EOK)
    {
        return (alg);
    }
    else
    {
        VS_ALG_delete(alg);
        return (NULL);
    }
}

/* ===================================================================
 *  @func     ALG_vstabCreate
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
void *ALG_vstabCreate(ALG_VstabCreate * create)
{
    ALG_VsObj *pObj;

    IVS1_Params vsParams;

    pObj = malloc(sizeof(ALG_VsObj));
    if (pObj == NULL)
    {
        return NULL;
    }

    memset(pObj, 0, sizeof(*pObj));
    memcpy(&pObj->createPrm, create, sizeof(pObj->createPrm));

    vsParams.vertImgSizeBeforeBSC = create->totalFrameHeight;
    vsParams.horzImgSizeBeforeBSC = create->totalFrameWidth;
    vsParams.vertDownsampleRatioInImgSensor = 1;
    vsParams.horzDownSampleRatioInImgSensor = 1;
    vsParams.minPanCoeffHorz = 29846;                      // should be
                                                           // between 0 - 1
                                                           // in Q15 format
                                                           // 0.91 * 2^15 =
                                                           // 29846
    vsParams.maxPanCoeffHorz = (1 * (1 << 15));            // should be
                                                           // between 0 - 1
                                                           // in Q15 format
    vsParams.minPanCoeffVert = 29846;                      // should be
                                                           // between 0 - 1
                                                           // in Q15 format
                                                           // 0.91 * 2^15 =
                                                           // 29846
    vsParams.maxPanCoeffVert = (1 * (1 << 15));            // should be
                                                           // between 0 - 1
                                                           // in Q15 format


    vsParams.panningCaseMotionThreshold = 0;
    vsParams.bounPels = vsParams.horzImgSizeBeforeBSC / 10;
    vsParams.bounLines = vsParams.vertImgSizeBeforeBSC / 10;
    
    
    pObj->vsHandle = (IALG_Handle) VS_ALG_create((IALG_Fxns *) & VS_TI_IVS,
                                                 (IALG_Handle) NULL,
                                                 (IALG_Params *) & vsParams);

    // copy the BSC params
    create->bsc_row_vct = vsParams.bsc_row_vct;
    create->bsc_row_vpos = vsParams.bsc_row_vpos;
    create->bsc_row_vnum = vsParams.bsc_row_vnum;
    create->bsc_row_vskip = vsParams.bsc_row_vskip;
    create->bsc_row_hpos = vsParams.bsc_row_hpos;
    create->bsc_row_hnum = vsParams.bsc_row_hnum;
    create->bsc_row_hskip = vsParams.bsc_row_hskip;
    create->bsc_row_shf = vsParams.bsc_row_shf;

    create->bsc_col_vct = vsParams.bsc_col_vct;
    create->bsc_col_vpos = vsParams.bsc_col_vpos;
    create->bsc_col_vnum = vsParams.bsc_col_vnum;
    create->bsc_col_vskip = vsParams.bsc_col_vskip;
    create->bsc_col_hpos = vsParams.bsc_col_hpos;
    create->bsc_col_hnum = vsParams.bsc_col_hnum;
    create->bsc_col_hskip = vsParams.bsc_col_hskip;
    create->bsc_col_shf = vsParams.bsc_col_shf;

    // Allocate bsc buffers
    pObj->bscBufAddr[0] = (Uint32) memalign(32, DRV_IPIPE_BSC_BUFFER_MAX);
    pObj->bscBufAddr[1] = (Uint32) memalign(32, DRV_IPIPE_BSC_BUFFER_MAX);
    pObj->curBscBufIdx = 0;
    pObj->curBscBufAddr = NULL;
    pObj->prevBscBufAddr = NULL;

    return pObj;
}

Uint32 prevStartX = 0;

Uint32 prevStartY = 0;

/* ===================================================================
 *  @func     ALG_vstabRun
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
int ALG_vstabRun(void *hndl, ALG_VstabRunPrm * prm, ALG_VstabStatus * status)
{
    int retVal;

    ALG_VsObj *pObj = (ALG_VsObj *) hndl;

    IVS1_InArgs inArgs;

    IVS1_OutArgs outArgs;

    IVS1_Fxns *ivsFxns;

    Uint32 bscSize;

    Iss_CaptObj *pCaptObj = &gIss_captCommonObj.captureObj[0];

    Iss_CaptVsProcessPrm vsProcessPrm;

#ifdef VS_PROFILE
    Uint32 start, end;
#endif

    if (pObj->vsHandle == NULL || prm == NULL)
    {
        return FVID2_EFAIL;
    }

    bscSize = (ALG_BSC_DATA_SIZE_MAX >> 1);

    ivsFxns = (IVS1_Fxns *) (pObj->vsHandle)->fxns;
#ifdef VS_PROFILE
    start = Utils_getCurTimeInMsec();
#endif

    if(pCaptObj->createArgs.vsProcessFunc == NULL)
    {
        // Activate the alg
        (pObj->vsHandle)->fxns->algActivate(pObj->vsHandle);

        // Apply VS
        inArgs.bscBufferCurrV = (XDAS_UInt32) prm->curBscDataVirtAddr;
        inArgs.bscBufferCurrH = (XDAS_UInt32) prm->curBscDataVirtAddr + bscSize;
        inArgs.bscBufferPrevV = (XDAS_UInt32) prm->prevBscDataVirtAddr;
        inArgs.bscBufferPrevH = (XDAS_UInt32) prm->prevBscDataVirtAddr + bscSize;

        retVal = ivsFxns->process((IVS1_Handle) pObj->vsHandle, &inArgs, &outArgs);

        // Deactivate the alg
        (pObj->vsHandle)->fxns->algDeactivate(pObj->vsHandle);
    }
    else
    {
        vsProcessPrm.bscBufferCurrV = (XDAS_UInt32) prm->curBscDataVirtAddr;
        vsProcessPrm.bscBufferCurrH = (XDAS_UInt32) prm->curBscDataVirtAddr + bscSize;
        vsProcessPrm.bscBufferPrevV = (XDAS_UInt32) prm->prevBscDataVirtAddr;
        vsProcessPrm.bscBufferPrevH = (XDAS_UInt32) prm->prevBscDataVirtAddr + bscSize;

        retVal = pCaptObj->createArgs.vsProcessFunc(&vsProcessPrm);

        outArgs.startX = vsProcessPrm.startX;
        outArgs.startY = vsProcessPrm.startY;
    }
#ifdef VS_PROFILE
    end = Utils_getCurTimeInMsec();
#endif
    if (retVal != IVS1_EOK)
    {
        status->startX = 0;
        status->startY = 0;
        return FVID2_SOK;
    }
    else
    {
        /*
         * The stabilized frame start co-ordinates output by the VSTAB algorithm are w.r.t 80% of input frame
         * But actually we are considering 83% of the input frame.So scaling the startX and startY accordingly
         */

        status->startX = (outArgs.startX);// * 83)/100;
        status->startY = (outArgs.startY);// * 83)/100;

        if ((prevStartX != status->startX) || (prevStartY != status->startY))
        {
#ifdef VS_PROFILE
            Vps_rprintf("(%3d,%3d) - %d msec\n", status->startX, status->startY,
                        (end - start));
#endif
            prevStartX = status->startX;
            prevStartY = status->startY;
        }
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     ALG_vstabDelete
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
int ALG_vstabDelete(void *hndl)
{
    ALG_VsObj *pObj = (ALG_VsObj *) hndl;

    if (pObj->vsHandle == NULL)
    {
        return FVID2_EFAIL;
    }

    // delete the VS alg
    VS_ALG_delete(pObj->vsHandle);

    // Free the prev bsc buf
    free((void *) pObj->bscBufAddr[0]);
    free((void *) pObj->bscBufAddr[1]);

    pObj->curBscBufIdx = 0;
    pObj->curBscBufAddr = NULL;
    pObj->prevBscBufAddr = NULL;

    free(pObj);

    return FVID2_SOK;
}

// VS ALG delete
/* ===================================================================
 *  @func     VS_ALG_delete
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
Void VS_ALG_delete(IALG_Handle alg)
{
    IALG_MemRec memTab[2];

    IALG_Fxns *fxns;

    fxns = alg->fxns;

    memTab[0].base = alg;

    fxns->algFree(alg, memTab);

    free(memTab[0].base);
    free(memTab[1].base);
}

/* Init capture Alg processing */
/* ===================================================================
 *  @func     IssAlg_captInit
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
Int32 IssAlg_captInit()
{
    Semaphore_Params semParams;

    Task_Params tskParams;

    Int32 status = FVID2_SOK;

    if (!gIss_captCommonObj.tskUpdate)
    {
        /* set to 0's for Alg object and descriptor memory */
        memset(&gIssAlg_captObj, 0, sizeof(gIssAlg_captObj));
    }

    /* set state to IDLE */
    gIssAlg_captObj.state = ISSALG_CAPT_STATE_IDLE;

    gIssAlg_captObj.dropFrameCount = 0;

    /* set driver callback */
    gIssAlg_captObj.completeCb = IssAlg_captIsrComplete;

    if (!gIss_captCommonObj.tskUpdate)
    {
        /* create timer object for periodic trigger of driver processing */

        /* create locking semaphore */
        Semaphore_Params_init(&semParams);

        semParams.mode = Semaphore_Mode_BINARY;

        gIssAlg_captObj.lock = Semaphore_create(1u, &semParams, NULL);

        if (gIssAlg_captObj.lock == NULL)
        {
            Vps_printf(" CAPTURE:%s:%d: Semaphore_create() failed !!!\n",
                       __FUNCTION__, __LINE__);
            status = FVID2_EALLOC;
        }
    }

    VIDEO_vsTskCreate();

    if ((!gIss_captCommonObj.tskUpdate) && (status == FVID2_SOK))
    {
        /* create semaphore and task for Alg processing task */
        Semaphore_Params_init(&semParams);
        Task_Params_init(&tskParams);

        semParams.mode = Semaphore_Mode_BINARY;

        gIss_captCommonObj.semUpdate = Semaphore_create(0, &semParams, NULL);

        if (gIss_captCommonObj.semUpdate == NULL)
        {
            /* Error - free previoulsy allocated resources */
            VIDEO_vsTskDelete();
            /* delete semaphore */
            Semaphore_delete(&gIssAlg_captObj.lock);
            return FVID2_EFAIL;
        }

        /* mark to not exit in Alg processing */
        gIss_captCommonObj.exitTsk = FALSE;

        tskParams.priority = ISSALG_CAPT_TSK_PRI_UPDATE;
        tskParams.stack = gIssAlg_captTskStackUpdate;
        tskParams.stackSize = sizeof(gIssAlg_captTskStackUpdate);

        gIss_captCommonObj.tskUpdate = Task_create(IssAlg_captTskUpdate,
                                                   &tskParams, NULL);

        if (gIss_captCommonObj.tskUpdate == NULL)
        {
            /* Error - free previoulsy allocated resources */
            VIDEO_vsTskDelete();
            /* delete semaphore */
            Semaphore_delete(&gIssAlg_captObj.lock);

            return (FVID2_EFAIL);
        }
    }

    return status;
}

/* Driver callback called by CLM with timer expires */
/* ===================================================================
 *  @func     IssAlg_captIsrComplete
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
Void IssAlg_captIsrComplete()
{
    /* get current time */
    gIssAlg_captObj.clockTicks = Clock_getTicks();

    /* trigger Alg processing task */
    Semaphore_post(gIss_captCommonObj.semUpdate);

}

/* CAPT ALG lock */
/* ===================================================================
 *  @func     IssAlg_captLock
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
Int32 IssAlg_captLock()
{
    /* take semaphore for locking */
    Semaphore_pend(gIssAlg_captObj.lock, BIOS_WAIT_FOREVER);

    return FVID2_SOK;
}

/* CAPT ALG unlock */
/* ===================================================================
 *  @func     IssAlg_captUnlock
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
Int32 IssAlg_captUnlock()
{
    /*
     * release semaphore for un-locking
     */
    Semaphore_post(gIssAlg_captObj.lock);

    return FVID2_SOK;
}

/* Alg processing task */
/* ===================================================================
 *  @func     IssAlg_captTskUpdate
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
Void IssAlg_captTskUpdate(UArg arg1, UArg arg2)
{
    Int32 status;

    Iss_CaptObj *pObj = &gIss_captCommonObj.captureObj[0];

    /* loop for ever */
    while (1u)
    {

        /* wait from trigger from CLM callback, i.e when timer expires */
        status = Semaphore_pend(gIss_captCommonObj.semUpdate,
                                BIOS_WAIT_FOREVER);

        if (gIss_captCommonObj.exitTsk)
        {
#ifdef ISS_CAPT_DEBUG
            Vps_rprintf(" %s:%d: Exiting Alg Task !!!\n", __FUNCTION__,
                        __LINE__);
#endif

            return;
        }

        if (!status)
        {
            /* error - continue looping */
            Vps_rprintf(" CAPTURE Alg: %s:%d: ERROR !!! \n",
                        __FUNCTION__, __LINE__);
            continue;
        }

        if(pObj->createArgs.captureMode == ISS_CAPT_INMODE_ISIF)
        {
            IssAlg_ProcessFunction();
            video_bsc_handle();
        }

        Iss_captTskDoCallback();
    }
}

/* ===================================================================
 *  @func     Iss_captTskDoCallback
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
Int32 Iss_captTskDoCallback()
{
    Iss_CaptObj *pObj;

    pObj = &gIss_captCommonObj.captureObj[0];

    if (pObj->fullQue[0].count != 0)
    {
        pObj->cbPrm.fdmCbFxn(pObj->cbPrm.fdmData, pObj);
    }

    return FVID2_SOK;
}

/* De-Init capture Alg processing */
/* ===================================================================
 *  @func     IssAlg_captDeInit
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
Int32 IssAlg_captDeInit()
{
    UInt32 sleepTime = 16;                                 /* in OS ticks */

    /* mark to exit Alg processing task */
    gIss_captCommonObj.exitTsk = TRUE;

    /* Wake up task to exit it */
    Semaphore_post(gIss_captCommonObj.semUpdate);

    /* wait for Alg processing task to get exited */
    Task_sleep(1);

    /* confirm task termination */
    while (Task_Mode_TERMINATED != Task_getMode(gIss_captCommonObj.tskUpdate))
    {
        Task_sleep(sleepTime);
        sleepTime >>= 1u;
        if (sleepTime == 0u)
        {
            GT_assert(GT_DEFAULT_MASK, 0u);
        }
    }

    /* delete the task */
    Task_delete(&gIss_captCommonObj.tskUpdate);

    VIDEO_vsTskDelete();

    /* delete semaphore */
    Semaphore_delete(&gIssAlg_captObj.lock);

    /* delete semaphore */
    Semaphore_delete(&gIss_captCommonObj.semUpdate);

    gIss_captCommonObj.tskUpdate = NULL;

    return (FVID2_SOK);
}

/* ===================================================================
 *  @func     IssAlg_ProcessFunction
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
Int32 IssAlg_ProcessFunction()
{
    Int32 status = FVID2_SOK;

    Int32 streamId = 0;

    FVID2_Frame *pframe_tmp;

    Iss_CaptObj *pObj;

    pObj = &gIss_captCommonObj.captureObj[0];
    // Vps_rprintf("Stream ID %d: Input queue empty condition.", streamId);
    for (streamId = 0; streamId < pObj->numStream; streamId++)
    {
        status =
            VpsUtils_queGet(&pObj->chObj[streamId][0].tmpQue,
                            (Ptr *) & pframe_tmp, 1, BIOS_NO_WAIT);
        if (status == FVID2_SOK)
        {
            tempStreamBuf[streamId].ptmpFrame = pframe_tmp;
            status =
                VpsUtils_quePut(&pObj->fullQue[streamId], pframe_tmp,
                                BIOS_WAIT_FOREVER);
            if (status != FVID2_SOK)
            {
                // Output queue is full.
                // Send the filled buffer back to empty pool.
                status =
                    VpsUtils_quePut(&pObj->chObj[streamId][0].emptyQue,
                                    pframe_tmp, BIOS_WAIT_FOREVER);
                if (status != FVID2_SOK)
                {
                    Vps_rprintf(" Intput and Output queues FULL!!!!");
                }
            }
        }
        else
        {
            tempStreamBuf[streamId].ptmpFrame = NULL;
#ifdef ISS_DEBUG_RT
            Vps_rprintf("Stream ID %d: Input queue empty condition.", streamId);
#endif
        }
    }
    return status;
}


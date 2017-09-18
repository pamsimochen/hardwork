/** ==================================================================

 *  @file   issdrv_ispApi.c

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

#include <xdc/std.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif.h>
#include <ti/psp/iss/hal/iss/isp/rsz/inc/rsz.h>
#include <ti/psp/iss/hal/iss/isp/ipipe/inc/ipipe.h>
#include <ti/psp/iss/core/inc/iss_drv_common.h>
#include <ti/psp/iss/core/inc/iss_drv.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/iss/drivers/alg/vstab/alg_vstab.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_alg2APriv.h>
#include <ti/psp/iss/drivers/capture/src/issdrv_capturePriv.h>
#include <ti/psp/iss/drivers/capture/src/issdrv_ispPriv.h>

extern isif_regs_ovly isif_reg;
extern ipipe_regs_ovly ipipe_reg;
extern rsz_regs_ovly rsz_reg;
extern rsz_A_regs_ovly rszA_reg;
extern rsz_B_regs_ovly rszB_reg;
extern ipipeif_regs_ovly ipipeif_reg;
extern VIDEO_VsStream gVsStream;

Iss_IspCommonObj gIss_ispCommonObj;

/* Stack for process task */
#pragma DATA_ALIGN(gIssIsp_processTskStack,32)
#pragma DATA_SECTION(gIssIsp_processTskStack, ".bss:taskStackSection")

UInt8 gIssIsp_processTskStack[ISS_ISP_INST_MAX][ISS_ISP_PROCESS_TASK_STACK];

Int32 IspDrv_processReq(Iss_IspObj *pObj);
Void ispDrvCalcFlipOffset(Iss_IspObj *pObj);
Void ispDrvSetOneShotMode(Iss_IspObj *pObj);
Void ispDrvSetCfg(Iss_IspObj *pObj);
Iss_IspObj *issIspAllocObj();
Void issIspFreeObj(Iss_IspObj *pObj);
Int32 Iss_isplock();
Int32 Iss_ispUnlock();

UInt32 Utils_tilerGetOriAddr(
            UInt32 tilerAddr,
            UInt32 cntMode,
            UInt32 oriFlag,
            UInt32 width,
            UInt32 height);

UInt32 Utils_tilerAddr2CpuAddr(UInt32 tilerAddr);

Int32 ispDrvIsYUV422ILEFormat(Int32 format)
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

/* ISS Post Process Write reg */
Void Iss_ispWriteReg(volatile UInt32 *pRegAddr,UInt32 value,UInt32 bitPosn,UInt32 numBits)
{
    UInt32 i,mask = 0;

    for(i = 0;i < numBits;i ++)
    {
        mask |= (1 << (bitPosn + i));
    }
    mask = ~mask;

    *pRegAddr &= mask;
    *pRegAddr |= (value << bitPosn);
}

Void IssDrv_ispH3aCallback()
{
#ifdef ISPDRV_ENABLE_2A_ISP
    Iss_IspObj *pObj;

    pObj = gIss_ispCommonObj.curActIspObj;

    if ((NULL != pObj) && (pObj->state == ISS_ISP_STATE_CREATED) && (NULL != pObj->p2AObj))
    {
    	//IssAlg_capt2AIsrCallBack(pObj->p2AObj);
	}
#endif
}


Void Issdrv_ispDrvRszOvfl()
{
    Iss_IspObj *pObj;
    FVID2_ProcessList *errList;

    pObj = gIss_ispCommonObj.curActIspObj;

    gIss_ispCommonObj.numRszOvfl ++;
    Vps_rprintf("\n\n ============ RESIZER OVERFLOW %d============= \n\n", gIss_ispCommonObj.numRszOvfl);

    if ((NULL != pObj) && (pObj->state == ISS_ISP_STATE_CREATED))
    {
        if (1 == pObj->usrCbFlag)
        {
            /* Main Task is waiting for the Luma to finish */

            /* So upate use flag to 2 and post the semaphore,
               main task should process further if the use flag is set to 2*/
            pObj->isRszOvfl = 1u;
            Semaphore_post(pObj->sem420);
        }

        gIss_ispCommonObj.isBteInitDone = 0;
        ispDrvSetOneShotMode(pObj);

        ispDrvSetCfg(pObj);

#ifdef ISPDRV_ENABLE_2A_ISP
		IssCdrv_reInit2AObj((Void *)pObj->p2AObj);
#endif

        if (pObj->cbPrm.fdmErrCbFxn)
        {
            errList = (FVID2_ProcessList *)pObj->cbPrm.errList;

            errList->numInLists = 1u;
            errList->numOutLists = 1u;

            /* Copy in and out frame Lists */
            errList->inFrameList[0] =
                pObj->procList.inFrameList[0];
            errList->outFrameList[0] =
                pObj->procList.outFrameList[0];

            pObj->cbPrm.fdmErrCbFxn(
                pObj->cbPrm.fdmData,
                errList,
                pObj);
        }

        gIss_ispCommonObj.curActIspObj = NULL;

	    Iss_ispUnlock();
    }
}

/* ISS Post Process Callback */
Int32 Iss_ispDrvCallBack()
{
    Iss_IspObj *pObj;
    VpsUtils_QueHandle *pQueueHndl;

	pObj = gIss_ispCommonObj.curActIspObj;

	if ((NULL == pObj) || (pObj->state == ISS_ISP_STATE_IDLE))
	{
		return FVID2_SOK;
	}

	if(pObj->cbPrm.fdmCbFxn != NULL)
	{
	    if(pObj->usrCbFlag == 0)
	    {
	        if(pObj->vsEnable == TRUE)
	        {
	            pQueueHndl = &pObj->processQueue;
	        }
	        else
	        {
	            pQueueHndl = &pObj->outQueue;
	        }

	         /* Queue the buffers in in queue */
	        if(pObj->pOutFrame[0] != NULL)
	        {
	            VpsUtils_quePut(pQueueHndl,
	                            pObj->pOutFrame[0],
	                            BIOS_WAIT_FOREVER);
	        }

	        if(pObj->pOutFrame[1] != NULL)
	        {
	            VpsUtils_quePut(pQueueHndl,
	                            pObj->pOutFrame[1],
	                            BIOS_WAIT_FOREVER);
	        }

	        if(pObj->runVs == FALSE)
	        {
	            Semaphore_post(gIss_ispCommonObj.bscSync);
	        }

#ifdef ISPDRV_ENABLE_2A_ISP
			IssAlg_capt2AIsrCallBack(pObj->p2AObj);
#endif

	        Semaphore_post(gIss_ispCommonObj.process);

	        pObj->cbPrm.fdmCbFxn(
	                        pObj->cbPrm.fdmData,
	                        pObj);
	    }
	    else
	    {
	        pObj->usrCbFlag = 0;
	        Semaphore_post(pObj->sem420);
	    }
	}
	gIss_ispCommonObj.curActIspObj = NULL;

    return FVID2_SOK;
}

/* ISS Post Process lock */
Int32 Iss_ispLock()
{
    /*
     * take semaphore for locking
     */
    Semaphore_pend(gIss_ispCommonObj.lock, BIOS_WAIT_FOREVER);

    return FVID2_SOK;
}

/* ISS Post Process unlock */
Int32 Iss_ispUnlock()
{
    /*
     * release semaphore for un-locking
     */
    Semaphore_post(gIss_ispCommonObj.lock);

    return FVID2_SOK;
}

/* ISS ISP Post BSC sync semaphore */
Int32 Iss_ispPostBscSem(UInt32 prevBscBufAddr,UInt32 curBscBufAddr)
{
    Iss_IspObj *pObj;

    pObj = gIss_ispCommonObj.curActIspObj;

    if((pObj->state == ISS_ISP_STATE_CREATED) && (pObj->vsEnable == TRUE))
    {
        pObj->prevBscBufAddr = prevBscBufAddr;
        pObj->curBscBufAddr  = curBscBufAddr;
        Semaphore_post(gIss_ispCommonObj.bscSync);
    }

    return FVID2_SOK;
}

/* ISS ISP Get VS run status */
Bool Iss_ispGetVsRunStatus()
{
    Iss_IspObj *pObj;

    pObj = gIss_ispCommonObj.curActIspObj;

    return (pObj->runVs);
}

/* ISS ISP Process Task */
Void Iss_ispProcessTask(UArg arg1, UArg arg2)
{
    Int32 retVal = FVID2_SOK;
    Iss_IspObj *pObj;
    FVID2_Frame *pFrame;
    ALG_VstabRunPrm runPrm;
    ALG_VstabStatus vstabStatus;
    Iss_IspPerFrameCfg *pPerFrameCfg;
    UInt32 nonStabilizedWidth,nonStabilizedHeight;
    UInt32 startX = 0,startY = 0;
    UInt32 offsetY = 0,offsetUV = 0;

    pObj = &gIss_ispCommonObj.ispObj[0];

    Vps_printf("Entered ISS ISP Process task\n");

    while(1)
    {
        /* Wait for BSC semaphore */
        Semaphore_pend(gIss_ispCommonObj.bscSync,BIOS_WAIT_FOREVER);

        if(gIss_ispCommonObj.exitTask == TRUE)
        {
            break;
        }

        /* Apply VS algorithm */
        if(pObj->runVs == TRUE)
        {
            runPrm.curBscDataVirtAddr  = (Uint8*)pObj->curBscBufAddr;
            runPrm.curBscDataPhysAddr  = NULL;
            runPrm.prevBscDataVirtAddr = (Uint8*)pObj->prevBscBufAddr;
            runPrm.prevBscDataPhysAddr = NULL;

            ALG_vstabRun(gVsStream.algVsHndl, &runPrm, &vstabStatus);
        }

        /* Wait for process semaphore */
        Semaphore_pend(gIss_ispCommonObj.process,BIOS_WAIT_FOREVER);

        do
        {
            /* Get the frames from process queue */
            retVal = VpsUtils_queGet(&pObj->processQueue,(Ptr *)&pFrame,1,BIOS_NO_WAIT);

            if(retVal == FVID2_SOK)
            {
                /* Stabilize the frame */
                pPerFrameCfg = (Iss_IspPerFrameCfg*)(pFrame->perFrameCfg);

                nonStabilizedWidth  = (pPerFrameCfg->frameWidth * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
                nonStabilizedHeight = (pPerFrameCfg->frameHeight * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;

                startX = (nonStabilizedWidth * vstabStatus.startX)/pObj->bscInWidth;
                startX = ISP_floor(startX,4);

                startY = (nonStabilizedHeight * vstabStatus.startY)/pObj->bscInHeight;
                startY = ISP_floor(startY,2);

                if (pPerFrameCfg->queueId == 0)
                {
                    offsetY  = (startY * pObj->pitch[0][0]) + startX;
                    offsetUV = (startY * (pObj->pitch[0][1] >> 1)) + startX;
                }
                else
                {
                    offsetY  = (startY * pObj->pitch[1][0]) + startX;
                    offsetUV = (startY * (pObj->pitch[0][1] >> 1)) + startX;
                }

                if((pPerFrameCfg->dataFormat == FVID2_DF_YUV420SP_UV) ||
                   (pPerFrameCfg->dataFormat == FVID2_DF_YUV420SP_VU))
                {
                    pFrame->addr[0][0] = (Ptr)(((Int32)pFrame->addr[1][0] + offsetY) & 0xFFFFFFE0);
                    pFrame->addr[0][1] = (Ptr)(((Int32)pFrame->addr[1][1] + offsetUV) & 0xFFFFFFE0);
                }
                else
                {
                    pFrame->addr[0][0] = (Ptr)(((Int32)pFrame->addr[1][0] + (offsetY)) & 0xFFFFFFE0);
                }

                /* Put it in out queue */
                VpsUtils_quePut(&pObj->outQueue,
                                pFrame,
                                BIOS_WAIT_FOREVER);
            }
        }while(retVal == FVID2_SOK);
    }

    Vps_printf("Exited ISS ISP Process task\n");
}

/* ISS Post Process Create */
Fdrv_Handle Iss_ispCreate(UInt32 drvId,
                         UInt32 instanceId,
                         Iss_IspCreateParams *pCreateArgs,
                         Iss_IspCreateStatus *pCreateStatus,
                         const FVID2_CbParams *pCbPrm)
{
    Iss_IspObj *pObj;
    Semaphore_Params semParams;
    Task_Params tskParams;

    if (instanceId == ISS_ISP_INST_ALL)
    {
        /*
         * gloabl handle open requested, no action required,
         * just return a special handle ID
         */
        return (Fdrv_Handle)ISS_ISP_INST_ALL;
    }

    if (pCreateStatus == NULL)
    {
        return NULL;
    }

    if ((instanceId >= ISS_ISP_INST_MAX) || (pCreateArgs == NULL) || (drvId != FVID2_ISS_M2M_ISP_DRV))
    {
        /*
         * invalid parameters - return NULL
         */
        pCreateStatus->retVal = FVID2_EBADARGS;
        return NULL;
    }

    /*
     * lock driver instance
     */
    Iss_ispLock();

    pObj = issIspAllocObj();
    if (NULL == pObj)
    {
	    Vps_rprintf("Could not allocate handle object\n");
	    return NULL;
    }

    /* Semaphore for the YUV420 -> YUV420 resizing */
    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;
    pObj->sem420 = Semaphore_create(0u, &semParams, NULL);

    /*
     * check if object is already opended
     */

    if (pObj->state != ISS_ISP_STATE_IDLE)
    {
        pCreateStatus->retVal = FVID2_EDEVICE_INUSE;

        issIspFreeObj(pObj);

        Iss_ispUnlock();
        return NULL;
    }

    memcpy(&pObj->createArgs, pCreateArgs, sizeof(pObj->createArgs));

    pObj->instanceId = instanceId;
    pObj->vsEnable   = pCreateArgs->vsEnable;

    if (pCbPrm != NULL)
    {
        memcpy(&pObj->cbPrm, pCbPrm, sizeof(pObj->cbPrm));
    }

    ispDrvSetOneShotMode(pObj);

    pObj->state = ISS_ISP_STATE_CREATED;

    VpsUtils_queCreate(&pObj->outQueue,                 // handle
                       ISS_ISP_MAX_ELEMENTS_QUEUE,      // maxElements
                       &pObj->outQueueMem[0],           // queueMem
                       VPSUTILS_QUE_FLAG_NO_BLOCK_QUE); // flags

    VpsUtils_queCreate(&pObj->processQueue,             // handle
                       ISS_ISP_MAX_ELEMENTS_QUEUE,      // maxElements
                       &pObj->processQueueMem[0],       // queueMem
                       VPSUTILS_QUE_FLAG_NO_BLOCK_QUE); // flags

    /* Create process task */
    if (gIss_ispCommonObj.processTask == NULL)
    {
    	Task_Params_init(&tskParams);

    	tskParams.priority  = ISS_ISP_PROCESS_TASK_PRI;
    	tskParams.stack     = gIssIsp_processTskStack[instanceId];
    	tskParams.stackSize = ISS_ISP_PROCESS_TASK_STACK;

    	gIss_ispCommonObj.processTask = Task_create(Iss_ispProcessTask,
    	                                			&tskParams,
    	                                			NULL);
	}

    pObj->pOutFrame[0] = NULL;
    pObj->pOutFrame[1] = NULL;

    gIss_ispCommonObj.numObjUsed ++;

#ifdef ISPDRV_ENABLE_2A_ISP
    pObj->p2AObj = NULL;

	pObj->p2AObj = (Iss_2AObj *)IssCdrv_init2AObj(pCreateArgs->sensorId,
												  pCreateArgs->SensorHandle,
												  pCreateArgs->isIsifCapt);

	if (NULL == pObj->p2AObj)
	{
	    pCreateStatus->retVal = FVID2_EDEVICE_INUSE;

	    Iss_ispUnlock();

	    Iss_ispDelete(pObj, NULL);

	    return NULL;
	}
#endif

    /*
     * unlock driver instance
     */
    Iss_ispUnlock();

    return pObj;
}

/* ISS Post Process Process Frames */
Int32 Iss_ispProcessFrames(FVID2_Handle handle,
                           FVID2_ProcessList *procList)
{
    Int32 retVal = FVID2_SOK;
    Iss_IspObj *pObj = (Iss_IspObj *)handle;

    if (handle == NULL || procList == NULL)
    {
        retVal = FVID2_EBADARGS;
        return retVal;
    }

    Iss_ispLock();

    pObj->pOutFrame[0] = NULL;
    pObj->pOutFrame[1] = NULL;

    pObj->usrCbFlag = 0;
    pObj->isRszOvfl = 0u;

    if(pObj->numStream == 0)
    {
	    Iss_ispUnlock();
        Iss_ispDrvCallBack();
        return retVal;
    }

    // Wait for IPIPE and IPIPEIF to be free
    while(ipipeif_reg->IPIPEIF_ENABLE == 1);
    while(ipipe_reg->SRC_EN == 1);

    pObj->procList.inFrameList[0] = procList->inFrameList[0];
    pObj->procList.outFrameList[0] = procList->outFrameList[0];

    pObj->inFrame = procList->inFrameList[0]->frames[0];
    pObj->pOutFrame[0] = procList->outFrameList[0]->frames[0];
	if (pObj->numStream == 2)
    	pObj->pOutFrame[1] = procList->outFrameList[0]->frames[1];

    if (pObj->inFrame == NULL || pObj->pOutFrame[0] == NULL)
    {
	    Iss_ispUnlock();
        Iss_ispDrvCallBack();
        return FVID2_EFAIL;
    }
    if (pObj->numStream > 1 && (pObj->pOutFrame[1] == NULL))
    {
	    Iss_ispUnlock();
        Iss_ispDrvCallBack();
        return FVID2_EFAIL;
    }

    gIss_ispCommonObj.curActIspObj = pObj;

#ifdef ISPDRV_ENABLE_2A_ISP
	IssAlg_capt2AApplySettings(pObj->p2AObj, 1);
	IssAlg_capt2ASetBufAddr(pObj->p2AObj);
#endif

    retVal = IspDrv_processReq(pObj);

    Iss_ispUnlock();

    return retVal;
}

/* ISS Get Processed Frames */
Int32 Iss_ispGetProcessedFrames(Fdrv_Handle handle,
                                FVID2_ProcessList *processList,
                                UInt32 timeout)
{
    Int32 retVal = FVID2_SOK;
    Iss_IspObj *pObj = (Iss_IspObj *)handle;
    FVID2_Frame *pFrame;

    processList->outFrameList[0]->numFrames = 0;

    do
    {
        retVal = VpsUtils_queGet(&pObj->outQueue,(Ptr *)&pFrame,1,BIOS_NO_WAIT);

        if(retVal == FVID2_SOK)
        {
            processList->outFrameList[0]->frames[processList->outFrameList[0]->numFrames] = pFrame;
            processList->outFrameList[0]->numFrames ++;
        }
    }while(retVal == FVID2_SOK);

    return FVID2_SOK;
}

/* ISS Post Process Control */
Int32 Iss_ispControl(Fdrv_Handle handle, UInt32 cmd,
                    Ptr cmdArgs, Ptr cmdStatusArgs)
{
    Int32 status = FVID2_SOK;
    UInt32 pCnt;
    Iss_IspObj *pObj = (Iss_IspObj *)handle;
    Iss_IspRszConfig *pRszCfg = (Iss_IspRszConfig*)cmdArgs;

    switch(cmd)
    {
        case IOCTL_ISS_ISP_RSZ_CONFIG:
            {
                pObj->inDataFmt   = pRszCfg->inDataFmt;
                pObj->outDataFmt0 = pRszCfg->outDataFmt0;
                pObj->outDataFmt1 = pRszCfg->outDataFmt1;
                pObj->numStream   = (pRszCfg->numStream >= 2)?2:1;
                pObj->inWidth     = pRszCfg->inWidth;
                pObj->inHeight    = pRszCfg->inHeight;
                pObj->outWidth0   = pRszCfg->outWidth0;
                pObj->outHeight0  = pRszCfg->outHeight0;
                pObj->outWidth1   = pRszCfg->outWidth1;
                pObj->outHeight1  = pRszCfg->outHeight1;
                pObj->mirrorMode  = pRszCfg->mirrorMode;
                pObj->outStartX   = pRszCfg->outStartX;
                pObj->outStartY   = pRszCfg->outStartY;
                pObj->inPitch     = pRszCfg->inPitch;

				for (pCnt = 0; pCnt < 3; pCnt ++)
				{
					pObj->pitch[0][pCnt] = pRszCfg->pitch[0][pCnt];
					pObj->pitch[1][pCnt] = pRszCfg->pitch[1][pCnt];
				}

                if(pObj->numStream == 0)
                {
                    break;
                }

                pObj->runVs = pRszCfg->runVs;

                if(pObj->runVs == TRUE)
                {
                    pObj->bscInWidth  = pObj->inWidth;
                    pObj->bscInHeight = pObj->inHeight;
                }

                ispDrvCalcFlipOffset(pObj);

                //ispDrvSetCfg(pObj);
            }
            break;

        default:
            break;
    }

    return status;
}

/* ISS Post Process Delete */
Int32 Iss_ispDelete(Fdrv_Handle handle, Ptr reserved)
{
    Int32 status = FVID2_SOK;
    Iss_IspObj *pObj = (Iss_IspObj *)handle;
    UInt32 sleepTime = 16;

    if (handle == (Fdrv_Handle) ISS_ISP_INST_ALL)
    {
        return FVID2_SOK;
    }

    GT_assert(GT_DEFAULT_MASK, pObj != NULL);

    /*
     * lock driver instance
     */
    Iss_ispLock();

    if (pObj->state == ISS_ISP_STATE_STOPPED || pObj->state == ISS_ISP_STATE_CREATED)
    {
        // Stop the rsz and ipipeif
        rsz_reg->SRC_EN = 0;
        ipipeif_reg->IPIPEIF_ENABLE = 0;
        pObj->state = ISS_ISP_STATE_IDLE;
    }

    VpsUtils_queDelete(&pObj->outQueue);
    VpsUtils_queDelete(&pObj->processQueue);

    issIspFreeObj(pObj);

    gIss_ispCommonObj.numObjUsed --;

    if (0 == gIss_ispCommonObj.numObjUsed)
    {
	    /* Process task exit sequence */
	    gIss_ispCommonObj.exitTask = TRUE;
	    Semaphore_post(gIss_ispCommonObj.bscSync);
	    while(Task_Mode_TERMINATED != Task_getMode(gIss_ispCommonObj.processTask))
	    {
	        Task_sleep(sleepTime);
	    }
	    Task_delete(&gIss_ispCommonObj.processTask);

	    gIss_ispCommonObj.curActIspObj = NULL;
    }

    /*
     * unlock driver instance
     */
    Iss_ispUnlock();

    Semaphore_delete(&pObj->sem420);

    return status;
}

/* ISS Post Process Init */
Int32 Iss_ispInit()
{
    Int32 status = FVID2_SOK, instId;
    Semaphore_Params semParams;

    memset(&gIss_ispCommonObj, 0, sizeof(gIss_ispCommonObj));

    /* create locking semaphore */
    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;
    gIss_ispCommonObj.lock = Semaphore_create(1u, &semParams, NULL);

    if (gIss_ispCommonObj.lock == NULL)
    {
        Vps_printf(" ISP:%s:%d: Semaphore_create() failed !!!\n",
                   __FUNCTION__, __LINE__);
        status = FVID2_EALLOC;
    }

    /* Semaphore for BSC sync */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    gIss_ispCommonObj.bscSync = Semaphore_create(0u, &semParams, NULL);
    if (gIss_ispCommonObj.bscSync == NULL)
    {
        Vps_printf(" ISP:%s:%d: Semaphore_create() failed !!!\n",
                   __FUNCTION__, __LINE__);
        status = FVID2_EALLOC;
    }

    /* Semaphore for process */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    gIss_ispCommonObj.process = Semaphore_create(0u, &semParams, NULL);
    if (gIss_ispCommonObj.process == NULL)
    {
        Vps_printf(" ISP:%s:%d: Semaphore_create() failed !!!\n",
                   __FUNCTION__, __LINE__);
        status = FVID2_EALLOC;
    }

    for (instId = 0; instId < ISS_ISP_INST_HANDLES; instId++)
    {
		gIss_ispCommonObj.isAllocated[instId] = FALSE;
	}

	gIss_ispCommonObj.exitTask = FALSE;
	gIss_ispCommonObj.numRszOvfl = 0;
	gIss_ispCommonObj.isBteInitDone = 0;

    if (status == FVID2_SOK)
    {
        /* register driver to FVID2 layer */
        gIss_ispCommonObj.fvidDrvOps.create             = (FVID2_DrvCreate)Iss_ispCreate;
        gIss_ispCommonObj.fvidDrvOps.delete             = Iss_ispDelete;
        gIss_ispCommonObj.fvidDrvOps.control            = Iss_ispControl;
        gIss_ispCommonObj.fvidDrvOps.queue              = NULL;
        gIss_ispCommonObj.fvidDrvOps.dequeue            = NULL;
        gIss_ispCommonObj.fvidDrvOps.processFrames      = Iss_ispProcessFrames;
        gIss_ispCommonObj.fvidDrvOps.getProcessedFrames = Iss_ispGetProcessedFrames;
        gIss_ispCommonObj.fvidDrvOps.drvId              = FVID2_ISS_M2M_ISP_DRV;

        status = FVID2_registerDriver(&gIss_ispCommonObj.fvidDrvOps);

        if (status != FVID2_SOK)
        {
            Vps_printf(" ISP:%s:%d: FVID2_registerDriver() failed !!!\n",
                       __FUNCTION__, __LINE__);
        }
    }

    if (status != FVID2_SOK)
    {
        Iss_ispDeInit();

        return status;
    }

    return status;
}

/* ISS Post Process Deinit */
Int32 Iss_ispDeInit()
{
    /*
     * unregister from FVID2 layer
     */
    FVID2_unRegisterDriver(&gIss_ispCommonObj.fvidDrvOps);

    /*
     * delete driver locks
     */

	/* delete semaphore */
    if (NULL != gIss_ispCommonObj.lock)
        Semaphore_delete(&gIss_ispCommonObj.lock);
    if (NULL != gIss_ispCommonObj.bscSync)
        Semaphore_delete(&gIss_ispCommonObj.bscSync);
    if (NULL != gIss_ispCommonObj.process)
        Semaphore_delete(&gIss_ispCommonObj.process);


    return FVID2_SOK;
}

Int32 IspDrv_processReq(Iss_IspObj *pObj)
{
    Int32 retVal = FVID2_SOK;
    Ptr inAddr,inAddrC;
    Ptr outAddrAY = NULL,outAddrAC = NULL,outAddrBY = NULL,outAddrBC = NULL;

    // set the input address
    inAddr = pObj->inFrame->addr[0][0];
    inAddrC = pObj->inFrame->addr[0][1];

    if(pObj->numStream == 2)
    {
        outAddrAY = pObj->pOutFrame[0]->addr[1][0];
        outAddrAC = pObj->pOutFrame[0]->addr[1][1];
        outAddrBY = pObj->pOutFrame[1]->addr[1][0];
        outAddrBC = pObj->pOutFrame[1]->addr[1][1];

		if (!ispDrvIsYUV422ILEFormat(pObj->outDataFmt0))
			if (NULL == outAddrAC)
			{
				Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
				return FVID2_EBADARGS;
			}
				
		if (!ispDrvIsYUV422ILEFormat(pObj->outDataFmt1))
			if (NULL == outAddrBC)
			{
				Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
				return FVID2_EBADARGS;
			}

        if ((NULL == outAddrAY) || (NULL == outAddrBY))
		{
			Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
			return FVID2_EBADARGS;
		}
    }
    else if(pObj->numStream == 1)
    {
        outAddrAY = pObj->pOutFrame[0]->addr[1][0];
        outAddrAC = pObj->pOutFrame[0]->addr[1][1];

		if (!ispDrvIsYUV422ILEFormat(pObj->outDataFmt0))
			if (NULL == outAddrAC)
			{
				Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
				return FVID2_EBADARGS;
			}

        if (NULL == outAddrAY)
		{
			Vps_rprintf("\n%s:%d\n", __func__, __LINE__, "Address is NULL\n");
			return FVID2_EBADARGS;
		}
    }
    else
    {
        Vps_printf(" ISP:%s:%d: Invalid numstream !!!\n",
                   __FUNCTION__, __LINE__);
        retVal = FVID2_EBADARGS;
        return retVal;
    }

    outAddrAY = (UInt32 *)outAddrAY + (pObj->pitch[0][0] * pObj->outStartY + pObj->outStartX)/4;
    outAddrAC = (UInt32 *)outAddrAC + (pObj->pitch[0][1] * pObj->outStartY/2 + pObj->outStartX)/4;

    if ((ISS_TILEDMEM == pObj->createArgs.memType0) ||
    	(ISS_TILEDMEM == pObj->createArgs.memType1))
    	*(volatile unsigned *)0x4E000224 = 0x8000;

    // Add Offset
    if (ISS_TILEDMEM == pObj->createArgs.memType0)
    {
	    if (pObj->createArgs.enableBte0)
	    {
		    IssCdrv_stopBteContext(&gIss_ispCommonObj.handleBTE.bte_config[0]);
		    IssCdrv_stopBteContext(&gIss_ispCommonObj.handleBTE.bte_config[1]);

		    gIss_ispCommonObj.handleBTE.bte_config[0].context_start_addr = 0;
			gIss_ispCommonObj.handleBTE.bte_config[1].context_start_addr =
		            (DIV_UP(pObj->outWidth0, 128) << CSL_BTE_BTE_CONTEXT_START_0_X_SHIFT);

		    IssCdrv_configureBteContext(&gIss_ispCommonObj.handleBTE, 0,
		        pObj->outWidth0, pObj->outHeight0,
		        pObj->outDataFmt0, pObj->mirrorMode);

	    	/* Assuming this is enabled for YUV420 output */
	    	outAddrAY = (Ptr)Utils_tilerGetOriAddr((UInt32)outAddrAY, 0, pObj->mirrorMode, pObj->outWidth0, pObj->outHeight0);
	    	outAddrAC = (Ptr)Utils_tilerGetOriAddr((UInt32)outAddrAC, 1, pObj->mirrorMode, pObj->outWidth0, pObj->outHeight0/2);

		    IssCdrv_startBteContext(&gIss_ispCommonObj.handleBTE.bte_config[0], (UInt32)outAddrAY);
			IssCdrv_startBteContext(&gIss_ispCommonObj.handleBTE.bte_config[1], (UInt32)outAddrAC);

			outAddrAY =
				(Ptr)(gIss_ispCommonObj.handleBTE.baseAddress + gIss_captCommonObj.handleBTE.bte_config[0].context_start_addr);
			outAddrAC =
				(Ptr)(gIss_ispCommonObj.handleBTE.baseAddress + gIss_captCommonObj.handleBTE.bte_config[1].context_start_addr);
			pObj->pitch[0][0] = pObj->pitch[0][1] = BTE_PITCH;
	    }
	    else
	    {
	    	/* Assuming this is enabled for YUV420 output */
	    	outAddrAY = (Ptr)Utils_tilerGetOriAddr((UInt32)outAddrAY, 0, pObj->mirrorMode, pObj->outWidth0, pObj->outHeight0);
	    	outAddrAC = (Ptr)Utils_tilerGetOriAddr((UInt32)outAddrAC, 1, pObj->mirrorMode, pObj->outWidth0, pObj->outHeight0/2);

	    	outAddrAY = (Ptr)Utils_tilerAddr2CpuAddr((UInt32)outAddrAY);
	    	outAddrAC = (Ptr)Utils_tilerAddr2CpuAddr((UInt32)outAddrAC);
    	}
    }
    else
    {
    	outAddrAY = (UInt32 *)outAddrAY + pObj->flipOffset[0][0]/4;
    	outAddrAC = (UInt32 *)outAddrAC + pObj->flipOffset[0][1]/4;
	}

    if (ISS_TILEDMEM == pObj->createArgs.memType1)
    {
	    if (pObj->createArgs.enableBte1)
	    {
		    gIss_ispCommonObj.handleBTE.bte_config[0].context_start_addr = 0;
			gIss_ispCommonObj.handleBTE.bte_config[1].context_start_addr =
		            (DIV_UP(pObj->outWidth1, 128) << CSL_BTE_BTE_CONTEXT_START_0_X_SHIFT);

		    IssCdrv_configureBteContext(&gIss_ispCommonObj.handleBTE, 0,
		        pObj->outWidth1, pObj->outHeight1,
		        pObj->outDataFmt1, pObj->mirrorMode);

	    	/* Assuming this is enabled for YUV420 output */
	    	outAddrBY = (Ptr)Utils_tilerGetOriAddr((UInt32)outAddrBY, 0, pObj->mirrorMode, pObj->outWidth1, pObj->outHeight1);
	    	outAddrBC = (Ptr)Utils_tilerGetOriAddr((UInt32)outAddrBC, 1, pObj->mirrorMode, pObj->outWidth1, pObj->outHeight1/2);

		    IssCdrv_startBteContext(&gIss_ispCommonObj.handleBTE.bte_config[0], (UInt32)outAddrBY);
			IssCdrv_startBteContext(&gIss_ispCommonObj.handleBTE.bte_config[1], (UInt32)outAddrBC);

			outAddrBY =
				(Ptr)(gIss_ispCommonObj.handleBTE.baseAddress + gIss_captCommonObj.handleBTE.bte_config[0].context_start_addr);
			outAddrBC =
				(Ptr)(gIss_ispCommonObj.handleBTE.baseAddress + gIss_captCommonObj.handleBTE.bte_config[1].context_start_addr);
			pObj->pitch[1][0] = pObj->pitch[1][1] = BTE_PITCH;
	    }
	    else
	    {
	    	/* Assuming this is enabled for YUV420 output */
	    	outAddrBY = (Ptr)Utils_tilerGetOriAddr((UInt32)outAddrBY, 0, pObj->mirrorMode, pObj->outWidth1, pObj->outHeight1);
	    	outAddrBC = (Ptr)Utils_tilerGetOriAddr((UInt32)outAddrBC, 1, pObj->mirrorMode, pObj->outWidth1, pObj->outHeight1/2);

	    	outAddrBY = (Ptr)Utils_tilerAddr2CpuAddr((UInt32)outAddrBY);
	    	outAddrBC = (Ptr)Utils_tilerAddr2CpuAddr((UInt32)outAddrBC);
    	}
    }
    else
    {
	    outAddrBY = (UInt32 *)outAddrBY + pObj->flipOffset[1][0]/4;
    	outAddrBC = (UInt32 *)outAddrBC + pObj->flipOffset[1][1]/4;
	}

    ispDrvSetCfg(pObj);

    ipipeif_set_sdram_in_addr((UInt32)inAddr);

    if (ispDrvIsYUV422ILEFormat(pObj->inDataFmt) ||
        (FVID2_DF_BAYER_RAW == pObj->inDataFmt))
    {
        if (ispDrvIsYUV422ILEFormat(pObj->outDataFmt0))
        {
            retVal = issSetRszOutAddress(RESIZER_A,
                                         RSZ_YUV422_RAW_RGB_OP,
                                         outAddrAY,
                                         pObj->pitch[0][0]);
        }
        else
        {
            retVal = issSetRszOutAddress(RESIZER_A,
                                         RSZ_YUV420_Y_OP,
                                         outAddrAY,
                                         pObj->pitch[0][0]);

            retVal = issSetRszOutAddress(RESIZER_A,
                                         RSZ_YUV420_C_OP,
                                         outAddrAC,
                                         pObj->pitch[0][1]);
        }

        if (2 == pObj->numStream)
        {
            if (ispDrvIsYUV422ILEFormat(pObj->outDataFmt1))
            {
                retVal = issSetRszOutAddress(RESIZER_B,
                                             RSZ_YUV422_RAW_RGB_OP,
                                             outAddrBY,
                                             pObj->pitch[1][0]);
            }
            else
            {
                retVal = issSetRszOutAddress(RESIZER_B,
                                             RSZ_YUV420_Y_OP,
                                             outAddrBY,
                                             pObj->pitch[1][0]);

                retVal = issSetRszOutAddress(RESIZER_B,
                                             RSZ_YUV420_C_OP,
                                             outAddrBC,
                                             pObj->pitch[1][1]);
            }
        }
    }
    else
    {
        // Y resizing
        Iss_ispWriteReg(&rszA_reg->RZA_420,1,0,1);                          // YEN -> Y output enable and 422to420 conversion enabled
        Iss_ispWriteReg(&rszA_reg->RZA_420,0,1,1);                          // CEN -> C output disable

        retVal = issSetRszOutAddress(RESIZER_A,
                                     RSZ_YUV420_Y_OP,
                                     outAddrAY,
                                     pObj->pitch[0][0]);
        retVal = issSetRszOutAddress(RESIZER_B,
                                     RSZ_YUV420_Y_OP,
                                     outAddrBY,
                                     pObj->pitch[1][0]);

        pObj->usrCbFlag = 1;

        // Wait for IPIPE and IPIPEIF to be free
        //while(ipipeif_reg->IPIPEIF_ENABLE == 1);
        //while(ipipe_reg->SRC_EN == 1);

        // Trigger RSZ and IPIPEIF
        Iss_ispWriteReg(&rsz_reg->SRC_EN,1,0,1);

        Iss_ispWriteReg(&rszA_reg->RZA_EN,1,0,1);
        if (2 == pObj->numStream)
        {
            Iss_ispWriteReg(&rszB_reg->RZB_EN,1,0,1);
        }
        else
        {
            Iss_ispWriteReg(&rszB_reg->RZB_EN,0,0,1);
        }

        Iss_ispWriteReg(&ipipe_reg->SRC_EN,1,0,1);
        Iss_ispWriteReg(&ipipeif_reg->IPIPEIF_ENABLE,1,0,1);

        Semaphore_pend(pObj->sem420,BIOS_WAIT_FOREVER);

        if (!pObj->isRszOvfl)
        {
            ipipeif_set_sdram_in_addr((UInt32)inAddrC);

            // UV resizing
            Iss_ispWriteReg(&rszA_reg->RZA_420,0,0,1); // YEN -> Y output disable
            Iss_ispWriteReg(&rszA_reg->RZA_420,1,1,1); // CEN -> C output enable and 422to420 conversion enabled

            rszASetOutConfig(pObj->inWidth - 2,
                             (pObj->inHeight >> 1) - 2,
                             pObj->outWidth0,
                             pObj->outHeight0 >> 1);
            rszBSetOutConfig(pObj->inWidth - 2,
                            (pObj->inHeight >> 1) - 2,
                            pObj->outWidth1,
                            pObj->outHeight1 >> 1);

            retVal = issSetRszOutAddress(RESIZER_A,
                                         RSZ_YUV420_C_OP,
                                         outAddrAC,
                                         pObj->pitch[0][1]);
            retVal = issSetRszOutAddress(RESIZER_B,
                                         RSZ_YUV420_C_OP,
                                         outAddrBC,
                                         pObj->pitch[1][1]);
        }
    }

    if (!pObj->isRszOvfl)
    {
        // Trigger RSZ and IPIPEIF
        Iss_ispWriteReg(&rsz_reg->SRC_EN,1,0,1);

        if(pObj->numStream == 2)
        {
            Iss_ispWriteReg(&rszA_reg->RZA_EN,1,0,1);
            Iss_ispWriteReg(&rszB_reg->RZB_EN,1,0,1);
        }
        else
        {
            Iss_ispWriteReg(&rszA_reg->RZA_EN,1,0,1);
            Iss_ispWriteReg(&rszB_reg->RZB_EN,0,0,1);
        }

        Iss_ispWriteReg(&ipipe_reg->SRC_EN,1,0,1);
        Iss_ispWriteReg(&ipipeif_reg->IPIPEIF_ENABLE,1,0,1);
    }

    return (retVal);
}


Void ispDrvCalcFlipOffset(Iss_IspObj *pObj)
{
    Bool flipH, flipV;

    switch (pObj->mirrorMode)
    {
        case 1:
            flipH = TRUE;
            flipV = FALSE;
            break;

        case 2:
            flipH = FALSE;
            flipV = TRUE;
            break;

        case 3:
            flipH = TRUE;
            flipV = TRUE;
            break;

        case 0:
        default:
            flipH = FALSE;
            flipV = FALSE;
            break;
    }

    pObj->flipOffset[0][0] = 0;
    pObj->flipOffset[0][1] = 0;
    pObj->flipOffset[1][0] = 0;
    pObj->flipOffset[1][1] = 0;

    if (flipV)
    {
        switch(pObj->outDataFmt0)
        {
            case FVID2_DF_YUV422I_UYVY:
            case FVID2_DF_YUV422I_YUYV:
            case FVID2_DF_YUV422I_YVYU:
            case FVID2_DF_YUV422I_VYUY:
                pObj->flipOffset[0][0] += pObj->pitch[0][0] * (pObj->outHeight0 - 1);
                break;

            default:
            case FVID2_DF_YUV420SP_UV:
            case FVID2_DF_YUV420SP_VU:
                pObj->flipOffset[0][0] += pObj->pitch[0][0] * (pObj->outHeight0 - 1);
                pObj->flipOffset[0][1] += pObj->pitch[0][1] * (pObj->outHeight0/2 - 1);
                break;
        }

        switch(pObj->outDataFmt1)
        {
            case FVID2_DF_YUV422I_UYVY:
            case FVID2_DF_YUV422I_YUYV:
            case FVID2_DF_YUV422I_YVYU:
            case FVID2_DF_YUV422I_VYUY:
                pObj->flipOffset[1][0] += pObj->pitch[1][0] * (pObj->outHeight1 - 1);
                break;

            default:
            case FVID2_DF_YUV420SP_UV:
            case FVID2_DF_YUV420SP_VU:
                pObj->flipOffset[1][0] += pObj->pitch[1][0] * (pObj->outHeight1 - 1);
                pObj->flipOffset[1][1] += pObj->pitch[1][1] * (pObj->outHeight1/2 - 1);
                break;
        }
    }

    if (flipH)
    {
        switch(pObj->outDataFmt0)
        {
            case FVID2_DF_YUV422I_UYVY:
            case FVID2_DF_YUV422I_YUYV:
            case FVID2_DF_YUV422I_YVYU:
            case FVID2_DF_YUV422I_VYUY:
                pObj->flipOffset[0][0] += (pObj->outWidth0 * 2 - 1);
                break;

            default:
            case FVID2_DF_YUV420SP_UV:
            case FVID2_DF_YUV420SP_VU:
                pObj->flipOffset[0][0] += (pObj->outWidth0 - 1);
                pObj->flipOffset[0][1] += (pObj->outWidth0 - 1);
                break;
        }

        switch(pObj->outDataFmt1)
        {
            case FVID2_DF_YUV422I_UYVY:
            case FVID2_DF_YUV422I_YUYV:
            case FVID2_DF_YUV422I_YVYU:
            case FVID2_DF_YUV422I_VYUY:
                pObj->flipOffset[1][0] += (pObj->outWidth1 * 2 - 1);
                break;

            default:
            case FVID2_DF_YUV420SP_UV:
            case FVID2_DF_YUV420SP_VU:
                pObj->flipOffset[1][0] += (pObj->outWidth1 - 1);
                pObj->flipOffset[1][1] += (pObj->outWidth1 - 1);
                break;
        }
    }
}

Void ispDrvSetOneShotMode(Iss_IspObj *pObj)
{
	UInt32 cnt;
	bte_config_t *contextHandle = NULL;

    // IPIPEIF register setting
    Iss_ispWriteReg(&ipipeif_reg->CFG1,1,0,1);   // ONESHOT -> One shot mode
    Iss_ispWriteReg(&ipipeif_reg->CFG1,1,10,1);  // CLKSEL -> Selects the pixel clock from the fractional clock divider

    Iss_ispWriteReg(&ipipeif_reg->CLKDIV,(pObj->createArgs.clkDivN - 1),0,8);   // CLKDIV -> N = CLKDIV[7:0] + 1
    Iss_ispWriteReg(&ipipeif_reg->CLKDIV,(pObj->createArgs.clkDivM - 1),8,8);   // CLKDIV -> M = CLKDIV[15:8] + 1

    // IPIPE register setting
    Iss_ispWriteReg(&ipipe_reg->SRC_MODE,1,0,1); // OST -> One shot
    Iss_ispWriteReg(&ipipe_reg->SRC_MODE,1,1,1); // WRT -> Enable
#ifdef IMGS_PANASONIC_MN34041
    /*
        +-----+-----+
        |  Gr |  R  |
        +-----+-----+
        |  B  |  Gb |
        +-----+-----+
    */

    Iss_ispWriteReg(&ipipe_reg->SRC_COL,3,0,2);                                                                                                                                  // EE -> R
    Iss_ispWriteReg(&ipipe_reg->SRC_COL,2,2,2);                                                                                                                                  // EO -> Gr
    Iss_ispWriteReg(&ipipe_reg->SRC_COL,1,4,2);                                                                                                                                  // OE -> Gb
    Iss_ispWriteReg(&ipipe_reg->SRC_COL,0,6,2);                                                                                                                                  // OO -> B
    ipipe_reg->SRC_COL = 0x1B;
#elif defined(IMGS_SONY_IMX104)
	ipipe_reg->SRC_COL = 0x4E;
#elif defined(IMGS_SONY_IMX136)
	ipipe_reg->SRC_COL = 0xE4;
#else
    Iss_ispWriteReg(&ipipe_reg->SRC_COL,3,0,2);                                 // EE -> B
    Iss_ispWriteReg(&ipipe_reg->SRC_COL,2,2,2);                                 // EO -> Gb
    Iss_ispWriteReg(&ipipe_reg->SRC_COL,1,4,2);                                 // OE -> Gr
    Iss_ispWriteReg(&ipipe_reg->SRC_COL,0,6,2);                                 // OO -> R
#endif

#ifdef IMGS_MICRON_MT9M034
	isif_reg->SLV0 = 2;
#elif defined IMGS_SONY_IMX104
	isif_reg->SLV0 = 21;
#elif defined IMGS_SONY_IMX136
	isif_reg->SLV0 = 13;
#elif defined IMGS_PANASONIC_MN34041
	isif_reg->SLV0 = 8;
#elif defined IMGS_MICRON_AR0331
	isif_reg->SLV0 = 2;
#endif

    // RSZ register setting
    Iss_ispWriteReg(&rsz_reg->SRC_MODE,1,0,1);                                  // OST -> One shot
    Iss_ispWriteReg(&rsz_reg->SRC_MODE,1,1,1);                                  // WRT -> Enable
    Iss_ispWriteReg(&rszA_reg->RZA_MODE,1,0,1);                                 // MODE -> One shot
    Iss_ispWriteReg(&rszB_reg->RZB_MODE,1,0,1);                                 // MODE -> One shot

    rsz_clock_enable(RESIZER_A,RSZ_CLK_ENABLE);
    rsz_clock_enable(RESIZER_B,RSZ_CLK_ENABLE);
    isp_enable_interrupt(ISP_RSZ_INT_DMA);

	/* configure BTE and its context if it is enabled */
	if (!gIss_ispCommonObj.isBteInitDone)
	{
		gIss_ispCommonObj.isBteInitDone = 1;
		if (pObj->createArgs.enableBte0 || pObj->createArgs.enableBte1)
		{
		    GT_assert(GT_DEFAULT_MASK, bte_init() == BTE_SUCCESS);
		    GT_assert(GT_DEFAULT_MASK, bte_open(1) == BTE_SUCCESS);/*0x20000000~0x3FFFFFFF*/

		    gIss_ispCommonObj.handleBTE.baseAddress = 0x20000000;

		    gIss_ispCommonObj.handleBTE.contextNumber = IssCdrv_getBteCtxNum();

		    gIss_ispCommonObj.handleBTE.bwLimiter = BTE_BWL;

		    IssCdrv_setBteBwLimiter(gIss_ispCommonObj.handleBTE.bwLimiter);
		    IssCdrv_setBteCtrlPosted(1);

		    for (cnt = 0; cnt < gIss_ispCommonObj.handleBTE.contextNumber; cnt++)
		    {
		        contextHandle = &gIss_ispCommonObj.handleBTE.bte_config[cnt];

		        contextHandle->context_num = (BTE_CONTEXT)cnt;

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
		}
	}
}

Void ispDrvSetCfg(Iss_IspObj *pObj)
{
    Iss_ispWriteReg(&ipipeif_reg->PPLN, ((pObj->inWidth + 16) - 1),0,13);
    Iss_ispWriteReg(&ipipeif_reg->LPFR, ((pObj->inHeight + 16) - 1),0,13);
    Iss_ispWriteReg(&ipipeif_reg->HNUM, (pObj->inWidth - 1),0,13);
    Iss_ispWriteReg(&ipipeif_reg->VNUM, (pObj->inHeight - 1),0,13);
    Iss_ispWriteReg(&ipipeif_reg->ADOFS, (pObj->inPitch >> 5),0,12);

    // IPIPE size setting
	Iss_ispWriteReg(&ipipe_reg->SRC_HPS,0,0,16);
	Iss_ispWriteReg(&ipipe_reg->SRC_HSZ,(pObj->inWidth - 1),0,13);
	Iss_ispWriteReg(&ipipe_reg->SRC_VPS,0,0,16);
	Iss_ispWriteReg(&ipipe_reg->SRC_VSZ,(pObj->inHeight - 1),0,13);

#ifdef ISPDRV_ENABLE_2A_ISP
    if (0x10000513u == pObj->createArgs.sensorId)
    {
    	/* SONY IMX122 uses the following Bayer pattern

    	        +-----+-----+
    	        |  Gb |  B  |
    	        +-----+-----+
    	        |  R  |  Gr |
    	        +-----+-----+
    	*/

		ipipe_reg->SRC_COL = 0xE4;
	}
    if (0x10000512u == pObj->createArgs.sensorId)
	{
    	/* OV2710 uses the following Bayer pattern

    	        +-----+-----+
    	        |  B  |  Gb |
    	        +-----+-----+
    	        |  Gr |  R  |
    	        +-----+-----+
    	*/
		ipipe_reg->SRC_COL = 0x1B;
	}
#endif

    switch(pObj->inDataFmt)
    {
        default:
        case FVID2_DF_BAYER_RAW:
        {
            /* RAW input YUV output */

            // IPIPEIF reg
            Iss_ispWriteReg(&ipipeif_reg->CFG1,1,2,2);                      // INPSRC2 -> SDRAM_RAW
            Iss_ispWriteReg(&ipipeif_reg->CFG1,0,8,2);                      // UNPACK -> 16 bits / pixel

            // RSZ reg
            Iss_ispWriteReg(&rsz_reg->SRC_FMT0,0,0,1);                      // SEL -> IPIPE
            Iss_ispWriteReg(&rsz_reg->SRC_FMT1,0,0,1);                      // RAW -> Flipping preserves YCbCr format
            Iss_ispWriteReg(&rsz_reg->SRC_FMT1,0,1,1);                      // IN420 -> YUV422
            Iss_ispWriteReg(&rsz_reg->SRC_VPS, 0, 0, 16);                      // IN420 -> YUV422

            switch(pObj->outDataFmt0)
            {
                case FVID2_DF_YUV422I_UYVY:
                case FVID2_DF_YUV422I_YUYV:
                case FVID2_DF_YUV422I_YVYU:
                case FVID2_DF_YUV422I_VYUY:
                {
                    // RSZA reg
                    Iss_ispWriteReg(&rszA_reg->RZA_420,0,0,1);              // YEN -> Y output disable
                    Iss_ispWriteReg(&rszA_reg->RZA_420,0,1,1);              // CEN -> C output disable
                    break;
                }

                default:
                case FVID2_DF_YUV420SP_UV:
                case FVID2_DF_YUV420SP_VU:
                {
                    // RSZA reg
                    Iss_ispWriteReg(&rszA_reg->RZA_420,1,0,1);              // YEN -> Y output enable and 422to420 conversion enabled
                    Iss_ispWriteReg(&rszA_reg->RZA_420,1,1,1);              // CEN -> C output enable and 422to420 conversion enabled
                    break;
                }
            }

            switch(pObj->outDataFmt1)
            {
                case FVID2_DF_YUV422I_UYVY:
                case FVID2_DF_YUV422I_YUYV:
                case FVID2_DF_YUV422I_YVYU:
                case FVID2_DF_YUV422I_VYUY:
                {
                    // RSZB reg
                    Iss_ispWriteReg(&rszB_reg->RZB_420,0,0,1);              // YEN -> Y output disable
                    Iss_ispWriteReg(&rszB_reg->RZB_420,0,1,1);              // CEN -> C output disable
                    break;
                }

                default:
                case FVID2_DF_YUV420SP_UV:
                case FVID2_DF_YUV420SP_VU:
                {
                    // RSZB reg
                    Iss_ispWriteReg(&rszB_reg->RZB_420,1,0,1);              // YEN -> Y output enable and 422to420 conversion enabled
                    Iss_ispWriteReg(&rszB_reg->RZB_420,1,1,1);              // CEN -> C output enable and 422to420 conversion enabled
                    break;
                }
            }

            break;
        }

        case FVID2_DF_YUV422I_UYVY:
        case FVID2_DF_YUV422I_YUYV:
        case FVID2_DF_YUV422I_YVYU:
        case FVID2_DF_YUV422I_VYUY:
        {
            /* YUV 422 input and YUV output */

            // IPIPEIF reg
            Iss_ispWriteReg(&ipipeif_reg->CFG1,3,2,2);                      // INPSRC2 -> SDRAM_YUV
            Iss_ispWriteReg(&ipipeif_reg->CFG1,0,8,2);                      // UNPACK -> 16 bits / pixel

            // RSZ reg
            Iss_ispWriteReg(&rsz_reg->SRC_FMT0,1,0,1);                      // SEL -> IPIPEIF
            Iss_ispWriteReg(&rsz_reg->SRC_FMT1,0,0,1);                      // RAW -> Flipping preserves YCbCr format
            Iss_ispWriteReg(&rsz_reg->SRC_FMT1,0,1,1);                      // IN420 -> YUV422

            switch(pObj->outDataFmt0)
            {
                case FVID2_DF_YUV422I_UYVY:
                case FVID2_DF_YUV422I_YUYV:
                case FVID2_DF_YUV422I_YVYU:
                case FVID2_DF_YUV422I_VYUY:
                {
                    // RSZA reg
                    Iss_ispWriteReg(&rszA_reg->RZA_420,0,0,1);              // YEN -> Y output disable
                    Iss_ispWriteReg(&rszA_reg->RZA_420,0,1,1);              // CEN -> C output disable
                    break;
                }

                default:
                case FVID2_DF_YUV420SP_UV:
                case FVID2_DF_YUV420SP_VU:
                {
                    // RSZA reg
                    Iss_ispWriteReg(&rszA_reg->RZA_420,1,0,1);              // YEN -> Y output enable and 422to420 conversion enabled
                    Iss_ispWriteReg(&rszA_reg->RZA_420,1,1,1);              // CEN -> C output enable and 422to420 conversion enabled
                    break;
                }
            }

            switch(pObj->outDataFmt1)
            {
                case FVID2_DF_YUV422I_UYVY:
                case FVID2_DF_YUV422I_YUYV:
                case FVID2_DF_YUV422I_YVYU:
                case FVID2_DF_YUV422I_VYUY:
                {
                    // RSZB reg
                    Iss_ispWriteReg(&rszB_reg->RZB_420,0,0,1);              // YEN -> Y output disable
                    Iss_ispWriteReg(&rszB_reg->RZB_420,0,1,1);              // CEN -> C output disable
                    break;
                }

                default:
                case FVID2_DF_YUV420SP_UV:
                case FVID2_DF_YUV420SP_VU:
                {
                    // RSZB reg
                    Iss_ispWriteReg(&rszB_reg->RZB_420,1,0,1);              // YEN -> Y output enable and 422to420 conversion enabled
                    Iss_ispWriteReg(&rszB_reg->RZB_420,1,1,1);              // CEN -> C output enable and 422to420 conversion enabled
                    break;
                }
            }

            break;
        }

        case FVID2_DF_YUV420SP_UV:
        case FVID2_DF_YUV420SP_VU:
        {
            /* YUV 420 input and YUV 420 output */

            // IPIPEIF reg
            Iss_ispWriteReg(&ipipeif_reg->CFG1,3,2,2);                      // INPSRC2 -> SDRAM_YUV
            Iss_ispWriteReg(&ipipeif_reg->CFG1,1,8,2);                      // UNPACK -> 8 bits / pixel

            // RSZ reg
            Iss_ispWriteReg(&rsz_reg->SRC_FMT0,1,0,1);                      // SEL -> IPIPEIF
            Iss_ispWriteReg(&rsz_reg->SRC_FMT1,0,0,1);                      // RAW -> Flipping preserves YCbCr format
            Iss_ispWriteReg(&rsz_reg->SRC_FMT1,1,1,1);                      // IN420 -> YUV4:2:0 is input

            break;
        }
    }

    if(pObj->numStream == 2)
    {
        rszASetOutConfig(pObj->inWidth - 2,
                         pObj->inHeight - 4,
                         pObj->outWidth0,
                         pObj->outHeight0);

        rszBSetOutConfig(pObj->inWidth - 2,
                         pObj->inHeight - 4,
                         pObj->outWidth1,
                         pObj->outHeight1);
    }
    else
    {
        rszASetOutConfig(pObj->inWidth - 2,
                         pObj->inHeight - 4,
                         pObj->outWidth0,
                         pObj->outHeight0);

        /* Set the resolution params even if RSZ B is going to be disabled */
        rszBSetOutConfig(pObj->inWidth - 2,
                         pObj->inHeight - 4,
                         pObj->outWidth0,
                         pObj->outHeight0);
    }
}

Iss_IspObj *issIspAllocObj()
{
	UInt32 cnt;
	Iss_IspObj *pObj = NULL;

	for (cnt = 0; cnt < ISS_ISP_INST_HANDLES; cnt ++)
	{
		if (gIss_ispCommonObj.isAllocated[cnt] == FALSE)
		{
			pObj = &gIss_ispCommonObj.ispObj[cnt];
			gIss_ispCommonObj.isAllocated[cnt] = TRUE;
			break;
		}
	}

	return (pObj);
}

Void issIspFreeObj(Iss_IspObj *pObj)
{
	UInt32 cnt;

	for (cnt = 0; cnt < ISS_ISP_INST_HANDLES; cnt ++)
	{
		if (&gIss_ispCommonObj.ispObj[cnt] == pObj)
		{
			gIss_ispCommonObj.isAllocated[cnt] = FALSE;
		}
	}
}


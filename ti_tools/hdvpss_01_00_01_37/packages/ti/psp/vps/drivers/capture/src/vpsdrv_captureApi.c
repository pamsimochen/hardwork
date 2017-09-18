/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/vps/drivers/capture/src/vpsdrv_capturePriv.h>

Vps_CaptCommonObj gVps_captCommonObj;
/**< Q objects that would used to track the current frame under reception */
extern Vps_CaptSubFrameQObj gVps_captSubFrameQObj[VPS_CAPT_STREAM_ID_MAX] \
                                        [VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX];
extern UInt32 gVps_captSubFrameQIndex;

#ifdef VPS_CAPTURE_TS_DEBUG
UInt32 gVps_CaptResetTsStats = 0;
#endif /* VPS_CAPTURE_TS_DEBUG */

/* Local Functions */
static Int32 Vps_captSubFrameCheckArgs( Vps_CaptCreateParams * createArgs );
static Int32 Vps_captSubFrameInitArgs(  Vps_CaptObj * pObj );
static Int32 Vps_captDeInitTimeStamp( Vps_CaptObj * pObj );
static Int32 Vps_captInitTimeStamp(  Vps_CaptObj * pObj, UInt32 enBestEffort, 
                Vps_CaptFrameTimeStamp fncPtr, UInt32 expectFps);
static Int32 Vps_configFrameTimeStamp ( Fdrv_Handle handle,
                                        Vps_CaptFrameTimeStampParms *tsPrms );
Int32 Vps_captInit (  )
{
    Int32 status = FVID2_SOK, instId;
    Vps_PlatformCpuRev cpuRev;
    /**< Version of the silicon being used */
    Vps_PlatformId platform;
    /**< Current pltform */

    /*
     * set to 0's
     */
    memset ( &gVps_captCommonObj, 0, sizeof ( gVps_captCommonObj ) );
    
    /* Get the platform and the CPU version */
    platform = Vps_platformGetId();

    if ((platform <= VPS_PLATFORM_ID_UNKNOWN) ||
        (platform >= VPS_PLATFORM_ID_MAX))
    {
        GT_assert( GT_DEFAULT_MASK, FALSE);
    }
    cpuRev = Vps_platformGetCpuRev();
    if (cpuRev >= VPS_PLATFORM_CPU_REV_UNKNOWN)
    {
        GT_assert( GT_DEFAULT_MASK,  FALSE);
    }
    
    /* When VIP parser overflows, disabling of VIP port is a workaround for the 
       memory flush issue that exists only on Netra PG1.1.*/
    if ((platform == VPS_PLATFORM_ID_EVM_TI816x) &&
        (cpuRev == VPS_PLATFORM_CPU_REV_1_1)) 
    {
        gVps_captCommonObj.disPortonOverflow = TRUE;
    }
    else
    {
        gVps_captCommonObj.disPortonOverflow = FALSE;
    }

        /*
     * create driver locks
     */
    for (instId = 0u; instId < VPS_CAPT_INST_MAX; instId++)
    {
        status |= Vps_captLockCreate ( &gVps_captCommonObj.captureObj[instId] );
        /*
         * derive VIP inst ID and VIP port ID
         */
        gVps_captCommonObj.captureObj[instId].vipPortId = instId % VPSHAL_VIP_INST_MAX;
        gVps_captCommonObj.captureObj[instId].vipInstId = instId / VPSHAL_VIP_INST_MAX;
    }

    if ( status != FVID2_SOK )
        return status;

    /*
     * init capture list manager
     */
    status = Vps_captInitList (  );
    if ( status != FVID2_SOK )
    {
        /*
         * Error - de-init previously inited stuff's
         */
        return status;
    }
    
    if (gVps_captCommonObj.disPortonOverflow)
    {
        status = Vem_registerVipIntr(&Vps_CaptOverflowCallback);
        if (status != FVID2_SOK)
        {
           return status;
        }
    
    }

    if ( status == FVID2_SOK )
    {
        /*
         * register driver to FVID2 layer
         */

        gVps_captCommonObj.fvidDrvOps.create = Vps_captCreate;
        gVps_captCommonObj.fvidDrvOps.delete = Vps_captDelete;
        gVps_captCommonObj.fvidDrvOps.control = Vps_captControl;
        gVps_captCommonObj.fvidDrvOps.queue = Vps_captQueue;
        gVps_captCommonObj.fvidDrvOps.dequeue = Vps_captDequeue;
        gVps_captCommonObj.fvidDrvOps.processFrames = NULL;
        gVps_captCommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gVps_captCommonObj.fvidDrvOps.drvId = FVID2_VPS_CAPT_VIP_DRV;

        status = FVID2_registerDriver ( &gVps_captCommonObj.fvidDrvOps );

        if ( status != FVID2_SOK )
        {
            /*
             * Error - de-init previously inited stuff's
             */
            Vps_captDeInitList (  );
        }
    }

    return status;
}

Int32 Vps_captDeInit (  )
{
    Int32 instId;

    /*
     * unregister from FVID2 layer
     */
    FVID2_unRegisterDriver ( &gVps_captCommonObj.fvidDrvOps );

    /*
     * De-init capture list manager
     */
    Vps_captDeInitList (  );

    /*
     * Unregister the VIP Interrupt
     */
    Vem_unRegisterVipIntr( );

    /*
     * delete driver locks
     */
    for ( instId = 0; instId < VPS_CAPT_INST_MAX; instId++ )
        Vps_captLockDelete ( &gVps_captCommonObj.captureObj[instId] );

    return FVID2_SOK;
}

Fdrv_Handle Vps_captCreate (UInt32 drvId,
                            UInt32 instanceId,
                            Ptr createArgsVps,
                            Ptr createStatusArgs,
                            const FVID2_DrvCbParams *cbPrm)
{
    Vps_CaptCreateParams   *createArgs = (Vps_CaptCreateParams *)createArgsVps;
    Vps_CaptCreateStatus   *createStatus =
                                (Vps_CaptCreateStatus *)createStatusArgs;
    Vps_CaptObj *pObj;
    Semaphore_Params semParams;
    Int32 status;
    Vps_CaptVipCropParams vipCropPrms;

    if ( instanceId == VPS_CAPT_INST_VIP_ALL )
    {
        /*
         * gloabl handle open requested, no action required,
         * just return a special handle ID
         */
        return ( Fdrv_Handle ) VPS_CAPT_INST_VIP_ALL;
    }

    if ( createStatus == NULL )
        return NULL;

    /*
     * parameter checking
     */
    if ( instanceId >= VPS_CAPT_INST_MAX
         || createArgs == NULL || ( drvId != FVID2_VPS_CAPT_VIP_DRV ) )
    {
        /*
         * invalid parameters - return NULL
         */
        createStatus->retVal = FVID2_EBADARGS;
        return NULL;
    }

    /*
     * get instance specific handle
     */
    pObj = &gVps_captCommonObj.captureObj[instanceId];

    /*
     * lock driver instance
     */
    Vps_captLock ( pObj );

    /*
     * check if object is already opended
     */
    if ( pObj->state != VPS_CAPT_STATE_IDLE )
    {
        createStatus->retVal = FVID2_EDEVICE_INUSE;
        Vps_captUnlock ( pObj );
        return NULL;
    }

    /* Get the platform and the CPU version */
    pObj->platform = Vps_platformGetId();

    if ((pObj->platform <= VPS_PLATFORM_ID_UNKNOWN) ||
        (pObj->platform >= VPS_PLATFORM_ID_MAX))
    {
        GT_assert( GT_DEFAULT_MASK, FALSE);
    }
    pObj->cpuRev = Vps_platformGetCpuRev();
    if (pObj->cpuRev >= VPS_PLATFORM_CPU_REV_UNKNOWN)
    {
        GT_assert( GT_DEFAULT_MASK,  FALSE);
    }

    if (Vps_captSubFrameCheckArgs( createArgs ) != FVID2_SOK)
    {
        createStatus->retVal = FVID2_EBADARGS;
        Vps_captUnlock ( pObj );
        return NULL;
    }

    /*
     * copy create arguments
     */
    memcpy ( &pObj->createArgs, createArgs, sizeof ( pObj->createArgs ) );

    /*
     * set instance Id
     */
    pObj->instanceId = instanceId;

    /*
     * By default set for field capture
     */
    pObj->frameCapture = FVID2_BUF_FMT_FIELD;
    /*
     * By default queueback error frame back to driver queue.
     */
    pObj->returnErrFrm = FALSE;
    pObj->oneCallBackPerFrm = FALSE;

    /*
     * copy callback
     */
    if ( cbPrm != NULL )
        memcpy ( &pObj->cbPrm, cbPrm, sizeof ( pObj->cbPrm ) );

    /*
     * derive VIP inst ID and VIP port ID
     */
    pObj->vipPortId = instanceId % VPSHAL_VIP_INST_MAX;
    pObj->vipInstId = instanceId / VPSHAL_VIP_INST_MAX;

    /*
     * set number of streams
     */
    pObj->numStream = createArgs->numStream;
    /*
     * create start and stop sync semaphores
     */
    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    pObj->semStopDone = Semaphore_create ( 0, &semParams, NULL );

    if ( pObj->semStopDone == NULL )
    {
        createStatus->retVal = FVID2_EALLOC;
        Vps_captUnlock ( pObj );
        return NULL;
    }

    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    pObj->semStartDone = Semaphore_create ( 0, &semParams, NULL );

    if ( pObj->semStartDone == NULL )
    {
        Semaphore_delete ( &pObj->semStopDone );
        createStatus->retVal = FVID2_EALLOC;
        Vps_captUnlock ( pObj );
        return NULL;
    }

    /*
     * create channel specific objects
     */
    status = Vps_captCreateChObj ( pObj );

    if ( status == FVID2_SOK )
    {

        /*
         * setup VIP HW blocks
         */
        status = Vps_captCreateVip ( pObj );

        if ( status == FVID2_SOK )
        {

            /*
             * create input and output queues
             */
            status = Vps_captCreateQueues ( pObj );
        }

        if ( status == FVID2_SOK )
        {
            status = Vps_captSubFrameInitArgs ( pObj );
        }
        
        if (status == FVID2_SOK)
        {
            status = Vps_captInitTimeStamp ( pObj, FALSE, NULL, 60 );
        }

    }

    createStatus->retVal = status;

    if ( status != FVID2_SOK )
    {
        /*
         * Error - free previously allocated objects
         */
        Semaphore_delete ( &pObj->semStartDone );
        Semaphore_delete ( &pObj->semStopDone );
        Vps_captUnlock ( pObj );
        return NULL;
    }

    /*
     * mark state as created
     */
    pObj->state = VPS_CAPT_STATE_CREATED;

    /* Disable VIP cropping */
    if (((pObj->platform == VPS_PLATFORM_ID_EVM_TI816x) &&
            (pObj->cpuRev >= VPS_PLATFORM_CPU_REV_2_0)) ||
        ((pObj->platform == VPS_PLATFORM_ID_EVM_TI814x) &&
            (pObj->cpuRev >= VPS_PLATFORM_CPU_REV_2_1)) ||
        (VPS_PLATFORM_ID_EVM_TI8107 == pObj->platform))
    {
        status = Vps_captGetVipCropCfg((Fdrv_Handle) pObj, &vipCropPrms);
        GT_assert(GT_DEFAULT_MASK, (status == FVID2_SOK));

        vipCropPrms.vipCropEnable = FALSE;
        status = Vps_captSetVipCropCfg((Fdrv_Handle) pObj, &vipCropPrms);
        GT_assert(GT_DEFAULT_MASK, (status == FVID2_SOK));
    }

    /*
     * unlock driver instance
     */
    Vps_captUnlock ( pObj );

    return pObj;
}

Int32 Vps_captDelete ( Fdrv_Handle handle, Ptr reserved )
{
    Int32 status = FVID2_EFAIL;
    Vps_CaptObj *pObj = ( Vps_CaptObj * ) handle;

    if ( handle == ( Fdrv_Handle ) VPS_CAPT_INST_VIP_ALL )
        return FVID2_SOK;

    GT_assert( GT_DEFAULT_MASK,  pObj != NULL );

    /*
     * lock driver instance
     */
    Vps_captLock ( pObj );

    /*
     * if driver is created or driver is ready for deletion, i.e stopped
     */
    if ( pObj->state == VPS_CAPT_STATE_STOPPED
         || pObj->state == VPS_CAPT_STATE_CREATED )
    {
        /*
         * driver can be deleted
         */

        status = FVID2_SOK;

        /*
         * Clear Time Stamping ISRs.
         */
        Vps_captDeInitTimeStamp ( pObj );

        /*
         * delete semaphores
         */
        Semaphore_delete ( &pObj->semStopDone );
        Semaphore_delete ( &pObj->semStartDone );

        /*
         * delete VIP handles associated with this driver instance
         */
        Vps_captDeleteVip ( pObj );

        /*
         * delete queues accosciated with this driver instance
         */
        Vps_captDeleteQueues ( pObj );

        /*
         * free path associated with this driver
         */
        Vps_captFreePath ( pObj );

        /* Check if sub frame based capture is enabled, if so un register ISRs*/
        if ( Vps_captSubFrameIsEnabled ( pObj ) == TRUE )
        {
            status = Vps_captVipSubFrameUnRegisterIsr ( pObj );
        }

        /*
         * mark state as idle
         */
        pObj->state = VPS_CAPT_STATE_IDLE;
    }

    /*
     * unlock driver instance
     */
    Vps_captUnlock ( pObj );

    return status;
}

Int32 Vps_captQueue ( Fdrv_Handle handle,
                      FVID2_FrameList * frameList, UInt32 streamId )
{
    Vps_CaptObj *pObj = ( Vps_CaptObj * ) handle;
    Vps_CaptChObj *pChObj;
    Int32 status = FVID2_SOK, queStatus;
    FVID2_Frame *pFrame;
    UInt16 frameId, chId, instId, lChannelNum, drvStreamId;

    /*
     * if global handle then make handle as NULL, this indicates
     * we need to check over all driver instances
     */
    if ( handle == ( Fdrv_Handle ) VPS_CAPT_INST_VIP_ALL )
        handle = NULL;

    /*
     * parameter checking
     */
    if (frameList == NULL || frameList->numFrames == 0
        || frameList->numFrames > FVID2_MAX_FVID_FRAME_PTR
        )
    {
        return FVID2_EFAIL;
    }

    /*
     * for all frames that need to be queued
     */
    for ( frameId = 0; frameId < frameList->numFrames; frameId++ )
    {

        /*
         * get FVID2 frame pointer
         */
        pFrame = frameList->frames[frameId];
        if ( pFrame == NULL )
        {
            /*
             * invalid FVID2 frame ID
             */
            status = FVID2_EFAIL;
            continue;
        }

        /* Reset the time stamp */
        pFrame->timeStamp = 0;

        /*
         * map user channel number to driver channel number
         */
        lChannelNum = gVps_captCommonObj.fvidChannelNum2lChannelNumMap
            [pFrame->channelNum];

        /*
         * extract driver instance ID from driver channel number
         */
        instId = Vps_captGetInstId ( lChannelNum );
        if ( instId >= VPS_CAPT_INST_MAX )
        {
            /*
             * invalid instance ID
             */
            status = FVID2_EFAIL;
            continue;
        }

        if ( handle == NULL )
        {
            /*
             * if global handle mode, the get instance handle
             * from driver channel number extract instance ID
             */
            pObj = &gVps_captCommonObj.captureObj[instId];
        }

        if ( instId != pObj->instanceId )
        {

            /*
             * if mismatch then channelNum in FVID2 Frame is not correct,
             * skip this frame que
             */
            status = FVID2_EFAIL;
            continue;
        }

        if ( pObj->state == VPS_CAPT_STATE_IDLE )
        {
            /*
             * if driver handle is not open then skip this frame que
             */
            status = FVID2_EFAIL;
            continue;
        }

        if(streamId==VPS_CAPT_STREAM_ID_ANY)
            drvStreamId = Vps_captGetStreamId ( lChannelNum );
        else
            drvStreamId = streamId;

        if ( drvStreamId >= pObj->numStream
             || drvStreamId != Vps_captGetStreamId ( lChannelNum ) )
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
        chId = Vps_captGetChId ( lChannelNum );
        if ( chId >= pObj->numCh )
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
        queStatus = VpsUtils_quePut ( &pChObj->emptyQue, pFrame, BIOS_NO_WAIT );

        /*
         * this assert should never happen
         */
        GT_assert( GT_DEFAULT_MASK,  queStatus == FVID2_SOK );

        /* Mark frame in frameList as NULL  */
        frameList->frames[frameId] = NULL;
    }

    return status;
}

Int32 Vps_captDequeue ( Fdrv_Handle handle,
                        FVID2_FrameList * frameList, UInt32 streamId,
                        UInt32 timeout )
{
    Vps_CaptObj *pObj = ( Vps_CaptObj * ) handle;
    Int32 status = FVID2_SOK, dequeStatus, instId;
    Ptr pData;

    /*
     * if global handle then make handle as NULL, this indicates
     * we need to check over all driver instances
     */
    if ( handle == ( Fdrv_Handle ) VPS_CAPT_INST_VIP_ALL )
        handle = NULL;

    /*
     * parameter checking
     */
    if (frameList == NULL || streamId >= VPS_CAPT_STREAM_ID_MAX )
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
    for ( instId = 0; instId < VPS_CAPT_INST_MAX; instId++ )
    {

        if ( handle == NULL )
        {
            /*
             * if handle is NULL, i.e global handle, then get handle using 'instId',
             * else use user supplied handle
             */
            pObj = &gVps_captCommonObj.captureObj[instId];
        }

        /*
         * check if driver instance is not idle, i.e driver is opened
         */
        if ( pObj->state != VPS_CAPT_STATE_IDLE )
        {

            /*
             * validate stream ID
             */
            if ( streamId >= pObj->numStream )
            {
                status = FVID2_EFAIL;
            }

            if ( status == FVID2_SOK )
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
                    if (FALSE == pObj->oneCallBackPerFrm)
                    {
                        dequeStatus = VpsUtils_queGet ( &pObj->fullQue[streamId],
                                                        &pData, 1, timeout );
                    }
                    else
                    {
                        dequeStatus = VpsUtils_queGet ( &pObj->fullQue1[streamId],
                                                        &pData, 1, timeout );
                    }
                    if ( dequeStatus == FVID2_SOK )
                    {
                        /*
                         * deque is successful, add to frame list
                         */
                        frameList->frames[frameList->numFrames] =
                            ( FVID2_Frame * ) pData;
                        frameList->numFrames++;
                        if (pObj->oneCallBackPerFrm)
                        {
                            break;
                        }
                    }
                    /*
                     * second iteration onwards do non-blocking deque
                     */
                    timeout = BIOS_NO_WAIT;
                    /*
                     * Max frames limit exceeded exit
                     */
                    if(frameList->numFrames >= FVID2_MAX_FVID_FRAME_PTR)
                       break;
                }
                while ( dequeStatus == FVID2_SOK );
                /*
                 * all frames deque from current driver handle
                 */
            }
        }

        if ( handle != NULL )
            break;  /* if not global handle, then we are done, so exit */

        /*
         * Max frames limit exceeded exit
         */
        if(frameList->numFrames >= FVID2_MAX_FVID_FRAME_PTR)
           break;

        /*
         * global handle mode, so check other driver instances also
         */
    }

    return status;
}

Int32 Vps_captControl ( Fdrv_Handle handle, UInt32 cmd,
                        Ptr cmdArgs, Ptr cmdStatusArgs )
{
    Int32 status;
    UInt32 disPortonOverflow;

    /*
     * check if global handle, IOCTLs not support for global handles
     */
    if ( handle == ( Fdrv_Handle ) VPS_CAPT_INST_VIP_ALL )
    {
        /*
         * switch to correct CMD handler
         */
        switch ( cmd )
        {
            case IOCTL_VPS_CAPT_CHECK_OVERFLOW:
                status = Vps_captCheckOverflow(
                            (Vps_CaptOverFlowStatus *) cmdStatusArgs);
                break;
            case IOCTL_VPS_CAPT_RESET_AND_RESTART:
                status = Vps_captResetAndRestart(
                            (Vps_CaptOverFlowStatus *) cmdArgs);
                break;
            case IOCTL_VPS_CAPT_RESET_VIP0:
                status  = Vps_captVipBlockReset(&gVps_captCommonObj.captureObj[VPS_CAPT_INST_VIP0_PORTA]);
                break;
            case IOCTL_VPS_CAPT_RESET_VIP1:
                status  = Vps_captVipBlockReset(&gVps_captCommonObj.captureObj[VPS_CAPT_INST_VIP1_PORTA]);
                break;
            case IOCTL_VPS_CAPT_PRINT_ADV_STATISTICS:
                status = Vps_captPrintAdvancedStatistics((UInt32)cmdArgs);
                break;
            case IOCTL_VPS_CAPT_FLUSH:
                status = Vps_captFlush(handle, (FVID2_FrameList*)cmdStatusArgs);
                break;
            case IOCTL_VPS_CAPT_DROP_DATA_BUFFER:
                gVps_captCommonObj.dropDataAddr = cmdArgs;
                status = FVID2_SOK;
                break;

            case IOCTL_VPS_CAPT_DISABLE_PORT_ON_OVERFLOW:
                disPortonOverflow = (UInt32 )cmdArgs;
                if (gVps_captCommonObj.disPortonOverflow != disPortonOverflow)
                {
                    gVps_captCommonObj.disPortonOverflow = disPortonOverflow;
                    if (disPortonOverflow)
                    {
                        /**Register VIP Interrupt on Overflow*/
                        status = Vem_registerVipIntr(&Vps_CaptOverflowCallback);
                    }
                    else
                    {
                        /**Unregister VIP Interrupt on Overflow*/
                        Vem_unRegisterVipIntr();
                        status = FVID2_SOK;
                    }
                }
                break;

            default:
                /*
                 * illegal IOCTL
                 */
                status = FVID2_EUNSUPPORTED_CMD;
                break;
        }

        return status;
    }

    /*
     * switch to correct CMD handler
     */
    switch ( cmd )
    {
        case FVID2_START:
            status = Vps_captStart ( handle );
            break;
        case FVID2_STOP:
            status = Vps_captStop ( handle, 1u );
            break;
        case IOCTL_VPS_CAPT_SET_FRAME_SKIP:
            status = Vps_captSetFrameSkip ( handle, cmdArgs );
            break;
        case IOCTL_VPS_CAPT_SET_SC_PARAMS:
            status = Vps_captVipSetScParams ( handle, cmdArgs );
            break;
        case IOCTL_VPS_CAPT_GET_CH_STATUS:
            status = Vps_captGetChStatus ( handle, cmdArgs, cmdStatusArgs );
            break;
        case IOCTL_VPS_CAPT_RESET_VIP0:
            status  = Vps_captVipBlockReset(&gVps_captCommonObj.captureObj[VPS_CAPT_INST_VIP0_PORTA]);
            break;
        case IOCTL_VPS_CAPT_RESET_VIP1:
            status  = Vps_captVipBlockReset(&gVps_captCommonObj.captureObj[VPS_CAPT_INST_VIP1_PORTA]);
            break;
        case IOCTL_VPS_CAPT_RESET_VIP:
            status = Vps_captVipReset(handle);
            break;
        case IOCTL_VPS_CAPT_CHECK_OVERFLOW:
            status = Vps_captCheckOverflow(
                        (Vps_CaptOverFlowStatus *) cmdStatusArgs);
            break;
        case IOCTL_VPS_CAPT_RESET_AND_RESTART:
            status = Vps_captResetAndRestart(
                        (Vps_CaptOverFlowStatus *) cmdArgs);
            break;
        case IOCTL_VPS_CAPT_PRINT_ADV_STATISTICS:
            status = Vps_captPrintAdvancedStatistics((UInt32)cmdArgs);
            break;
        case IOCTL_VPS_CAPT_FLUSH:
            status = Vps_captFlush(handle, (FVID2_FrameList*)cmdStatusArgs);
            break;
        case IOCTL_VPS_CAPT_SET_STORAGE_FMT:
            status = Vps_captSetStorageFmt(
                         handle,
                        (Vps_CaptStorageParams *) cmdArgs);
            break;
        case IOCTL_VPS_CAPT_SET_VIP_CROP_CFG:
            status = Vps_captSetVipCropCfg(
                         handle,
                         (const Vps_CaptVipCropParams *) cmdArgs);
            break;
        case IOCTL_VPS_CAPT_GET_VIP_CROP_CFG:
            status = Vps_captGetVipCropCfg(
                         handle,
                         (Vps_CaptVipCropParams *) cmdArgs);
            break;
        case  IOCTL_VPS_CAPT_RETURN_ERR_FRM:
            status = Vps_returnErrFrm(handle, (Vps_CaptReturnErrFrms *)cmdArgs);
            break;
        case  IOCTL_VPS_CAPT_ONE_CB_PER_FRM:
            status = Vps_oneCbPerFrm(handle,
                (Vps_CaptOneCallBackPerFrm *)cmdArgs);
            break;

        case IOCTL_VPS_CAPT_CFG_TIME_STAMPING_FRAMES:
            status = Vps_configFrameTimeStamp (handle, 
                (Vps_CaptFrameTimeStampParms *)cmdArgs);
            break;

        default:
            /*
             * illegal IOCTL
             */
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    return status;
}

/*
  Starts a drvier instance
*/
Int32 Vps_captStart ( Fdrv_Handle handle )
{
    Vps_CaptObj *pObj = ( Vps_CaptObj * ) handle;
    Int32 status = FVID2_EFAIL;

    /*
     * check if global handle, this operation is not supportted
     * for global handles
     */
    if ( handle == ( Fdrv_Handle ) VPS_CAPT_INST_VIP_ALL )
        return FVID2_EFAIL;

    GT_assert( GT_DEFAULT_MASK,  pObj != NULL );

    /*
     * lock driver instance
     */
    Vps_captLock ( pObj );

    if ( pObj->state == VPS_CAPT_STATE_CREATED
         || pObj->state == VPS_CAPT_STATE_STOPPED )
    {
        /*
         * Driver is not running so we can start it
         */
        status = Vps_captStartList ( pObj );
    }

    /*
     * unlock driver instance
     */
    Vps_captUnlock ( pObj );

    return status;
}

/*
  Stops a driver instance
*/
Int32 Vps_captStop ( Fdrv_Handle handle, UInt32 doReset )
{
    Vps_CaptObj *pObj = ( Vps_CaptObj * ) handle;
    Int32 status = FVID2_EFAIL;

    /*
     * check if global handle, this operation is not supportted
     * for global handles
     */
    if ( handle == ( Fdrv_Handle ) VPS_CAPT_INST_VIP_ALL )
        return FVID2_SOK;

    GT_assert( GT_DEFAULT_MASK,  pObj != NULL );

    /*
     * lock driver instance
     */
    Vps_captLock ( pObj );

    if ( pObj->state == VPS_CAPT_STATE_RUNNING )
    {

        /*
         * stop, only if driver is running
         */
        status = Vps_captStopList ( pObj );

        /* This API is called from FVId2_stop, so do sw reset of vip parser */
        if (doReset &&
            (pObj->cpuRev > VPS_PLATFORM_CPU_REV_1_1))
        {
            /* Waiting for current frame complete */
            Vps_captVipWaitForComplete(pObj);

            /* Reset VIP and other modules in the path */
            Vps_captVipPortAndModulesReset (pObj);
        }
    }

    /*
     * unlock driver instance
     */
    Vps_captUnlock ( pObj );

    return status;
}

Int32 Vps_captFreePath ( Vps_CaptObj * pObj )
{
    /*
     * free VIP resources used in this driver path
     */
    return Vcore_vipResFreePath ( &pObj->resObj );
}

Int32 Vps_captAllocPath ( Vps_CaptObj * pObj )
{
    Int32 status = FVID2_SOK;
    UInt16 streamId;

    /*
     * VIP capture path is not shared across driver
     * hence allocate path in exclusive mode
     */
    pObj->resAllocParams.openMode = VCORE_VIP_RES_ALLOC_EXCLUSIVE;

    /*
     * driver name
     */
    pObj->resAllocParams.drvName = "fvid2_vps_capt_vip_ti";

    /*
     * in exclusive mode allocation driver ID is ignored,
     * but assigned for convinience
     */
    pObj->resAllocParams.drvId
        =
        0x0DCA0000 | ( pObj->vipInstId * VPSHAL_VIP_PORT_MAX +
                       pObj->vipPortId );

    pObj->resAllocParams.vipInstId = pObj->vipInstId;

    /*
     * based on capture mode set input source for resource manager
     */

    /*
     * to being with mark input source as invalid
     */
    pObj->resAllocParams.inSrc = VCORE_VIP_IN_SRC_INVALID;

    pObj->resAllocParams.muxModeStartChId = pObj->createArgs.muxModeStartChId;

    switch ( pObj->createArgs.videoCaptureMode )
    {
        default:
            status = FVID2_EFAIL;
            break;

        case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VBLK:
        case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VSYNC:
        case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK:
        case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC:
        case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC:
            /*
             * single channel modes
             */

            switch ( pObj->createArgs.inDataFormat )
            {
                case FVID2_DF_YUV422P:

                    switch ( pObj->createArgs.videoIfMode )
                    {
                        case VPS_CAPT_VIDEO_IF_MODE_8BIT:

                            if ( pObj->vipPortId == 0 )
                                pObj->resAllocParams.inSrc =
                                    VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_8_BIT;
                            else
                                pObj->resAllocParams.inSrc =
                                    VCORE_VIP_IN_SRC_PARSER_PORT_B_YUV422_8_BIT;
                            break;

                        case VPS_CAPT_VIDEO_IF_MODE_16BIT:
                            if ( pObj->vipPortId == 0 )
                                pObj->resAllocParams.inSrc =
                                    VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_16_BIT;
                            break;
                    }
                    break;

                case FVID2_DF_RGB24_888:
                    if ( pObj->createArgs.videoIfMode
                         == VPS_CAPT_VIDEO_IF_MODE_24BIT
                         && pObj->vipPortId == 0 )
                    {
                        pObj->resAllocParams.inSrc =
                            VCORE_VIP_IN_SRC_PARSER_PORT_A_RGB888_24_BIT;
                    }
                    break;

                case FVID2_DF_YUV444I:
                    if ( pObj->createArgs.videoIfMode
                         == VPS_CAPT_VIDEO_IF_MODE_24BIT
                         && pObj->vipPortId == 0 )
                    {
                        pObj->resAllocParams.inSrc =
                            VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV444_24_BIT;
                    }
                    break;
            }
            break;

        case VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_SPLIT_LINE_EMBEDDED_SYNC:
        case VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_PIXEL_MUX_EMBEDDED_SYNC:
        case VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_EMBEDDED_SYNC:
            /*
             * multi channel modes
             */

            if ( pObj->createArgs.inDataFormat == FVID2_DF_YUV422P )
            {
                switch ( pObj->createArgs.videoIfMode )
                {
                    case VPS_CAPT_VIDEO_IF_MODE_8BIT:

                        if ( pObj->vipPortId == 0 )
                            pObj->resAllocParams.inSrc =
                                VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_8_BIT_MULTI_CH;
                        else
                            pObj->resAllocParams.inSrc =
                                VCORE_VIP_IN_SRC_PARSER_PORT_B_YUV422_8_BIT_MULTI_CH;
                        break;

                    case VPS_CAPT_VIDEO_IF_MODE_16BIT:
                        if ( pObj->vipPortId == 0 )
                            pObj->resAllocParams.inSrc =
                                VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_16_BIT_MULTI_CH;
                        break;
                }
            }
            break;
    }

    pObj->resAllocParams.numOutParams = pObj->numStream;

    for ( streamId = 0; streamId < pObj->resAllocParams.numOutParams;
          streamId++ )
    {
        switch ( pObj->createArgs.outStreamInfo[streamId].dataFormat )
        {
            case FVID2_DF_YUV422I_YUYV:
                pObj->resAllocParams.outParams[streamId].format
                    = VCORE_VIP_OUT_FORMAT_YUV422;
                break;

            case FVID2_DF_YUV420SP_UV:
                pObj->resAllocParams.outParams[streamId].format
                    = VCORE_VIP_OUT_FORMAT_YUV420;
                break;

            case FVID2_DF_RGB24_888:
                pObj->resAllocParams.outParams[streamId].format
                    = VCORE_VIP_OUT_FORMAT_RGB888;
                break;

            case FVID2_DF_YUV444I:
                pObj->resAllocParams.outParams[streamId].format
                    = VCORE_VIP_OUT_FORMAT_YUV444;
                break;

            case FVID2_DF_RAW_VBI:
                pObj->resAllocParams.outParams[streamId].format
                    = VCORE_VIP_OUT_FORMAT_ANC;
                break;

            case FVID2_DF_YUV422SP_UV:
                pObj->resAllocParams.outParams[streamId].format
                    = VCORE_VIP_OUT_FORMAT_YUV422SP_UV;
                break;

            default:
                status = FVID2_EFAIL;
                break;
        }

        pObj->resAllocParams.outParams[streamId].scEnable =
            pObj->createArgs.outStreamInfo[streamId].scEnable;
    }

    if ( pObj->resAllocParams.inSrc == VCORE_VIP_IN_SRC_INVALID )
        status = FVID2_EFAIL;

    if ( status == FVID2_SOK )
    {
        status = Vcore_vipResAllocPath ( &pObj->resObj, &pObj->resAllocParams );
    }

    return status;
}

/*
  Create and init channel specific driver objects
*/
Int32 Vps_captCreateChObj ( Vps_CaptObj * pObj )
{
    UInt16 streamId, chId;
    Vps_CaptChObj *pChObj;
    UInt16 vpdmaMultiChOffset, vChId;
    Int32 status;

    pObj->resetStatistics = TRUE;

    /*
     * VPDMA channel mapping offset in actual Si
     */
    if(pObj->createArgs.videoCaptureMode==VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_PIXEL_MUX_EMBEDDED_SYNC)
        vpdmaMultiChOffset = 1;
    else
        vpdmaMultiChOffset = 2;

    pObj->multiChDummyDescVchannelId = VPSHAL_VPDMA_CHANNEL_INVALID;

    if ((pObj->createArgs.videoCaptureMode ==
            VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC) ||
        (Vps_captIsSingleChDiscSyncMode(pObj->createArgs.videoCaptureMode)))
    {

        switch(pObj->instanceId)
        {
            case VPS_CAPT_INST_VIP0_PORTA:
                pObj->multiChDummyDescVchannelId = VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTA_SRC2;
                break;
            case VPS_CAPT_INST_VIP0_PORTB:
                pObj->multiChDummyDescVchannelId = VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTB_SRC2;
                break;
            case VPS_CAPT_INST_VIP1_PORTA:
                pObj->multiChDummyDescVchannelId = VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTA_SRC2;
                break;
            case VPS_CAPT_INST_VIP1_PORTB:
                pObj->multiChDummyDescVchannelId = VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTB_SRC2;
                break;
        }
    }

    /*
     * get channel properties like num channels,
     * in width x in height, scan format - interlaced or progressive
     */
    pObj->numCh = pObj->createArgs.numCh;

    status = Vps_captAllocPath ( pObj );
    if ( status != FVID2_SOK )
        return status;

    /*
     * for every stream and ever channel do ..
     */
    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        for ( chId = 0; chId < pObj->numCh; chId++ )
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
            pChObj->memType = VPS_VPDMA_MT_NONTILEDMEM;
            /*
             * Set for field Capture by default
             */
            pChObj->frameCapture = FVID2_BUF_FMT_FIELD;
            /*
             * This is dont care for fieldMerged
             */
            pChObj->fieldsMerged = FALSE;
            /*
             * make driver channel number from instance ID, stream ID,
             * thus given driver channel number we can know which
             * instance, stream, channel it belongs to
             * chID
             */
            pChObj->lChannelNum =
                Vps_captMakeChannelNum ( pObj->instanceId, streamId, chId );

            /*
             * copy user channel number to FVID2 Format structure
             */
            pChObj->channelNum =
                pObj->createArgs.channelNumMap[streamId][chId];

            /*
             * make user channel number to driver channel number mapping
             */
            gVps_captCommonObj.fvidChannelNum2lChannelNumMap[pChObj->
                                                             channelNum] =
                pChObj->lChannelNum;

            /*
             * fill FVID2 Format with data format
             */
            pChObj->dataFormat =
                pObj->createArgs.outStreamInfo[streamId].dataFormat;

            /*
             * set memory type for data format's which could support tiled mode
             */
            if ( pChObj->dataFormat == FVID2_DF_YUV420SP_UV
                 || pChObj->dataFormat == FVID2_DF_YUV422SP_UV )
            {
                pChObj->memType
                    = pObj->createArgs.outStreamInfo[streamId].memType;

            }

            /* invert FID for YUV420 and in discrete sync mode */
            pChObj->invertFid = FALSE;
            #if 0
            if ((pChObj->dataFormat == FVID2_DF_YUV420SP_UV) &&
                 Vps_captIsSingleChDiscSyncMode(pObj->createArgs.videoCaptureMode))
            {
                pChObj->invertFid = TRUE;
            }
            #endif

            pChObj->maxOutHeight
                = pObj->createArgs.outStreamInfo[streamId].maxOutHeight;
            if (pChObj->maxOutHeight > VPS_CAPT_MAX_OUT_HEIGHT_1080_LINES ||
                pChObj->maxOutHeight < VPS_CAPT_MAX_OUT_HEIGHT_288_LINES)
            {
                pChObj->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED;
            }

            pChObj->maxOutWidth
                = pObj->createArgs.outStreamInfo[streamId].maxOutWidth;
            if (pChObj->maxOutWidth > VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_1920_PIXELS ||
                pChObj->maxOutWidth < VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_352_PIXELS)
            {
                pChObj->maxOutWidth = VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_UNLIMITED;
            }

            /* fill pitch based on user supplied value */
            pChObj->pitch[0]
                = pObj->createArgs.outStreamInfo[streamId].pitch[0];
            pChObj->pitch[1]
                = pObj->createArgs.outStreamInfo[streamId].pitch[1];
            pChObj->pitch[2]
                = pObj->createArgs.outStreamInfo[streamId].pitch[2];
            /*
             * depending on data format and channel ID fill in the associated
             * VPDMA channel ID, VPDMA data types, how many VPDMA channels
             * are associated with this driver logical channel, and bits per pixel
             */
            pChObj->vChannelNum = pObj->resObj.vpdmaInfo[streamId].vpdmaChNum;
            GT_assert(GT_DEFAULT_MASK,
                (pChObj->vChannelNum <= VPS_CAPT_VCH_PER_LCH_MAX));

            for ( vChId = 0; vChId < pChObj->vChannelNum; vChId++ )
            {

                pChObj->vChannelDataType[vChId]
                    = pObj->resObj.vpdmaInfo[streamId].vpdmaChDataType[vChId];

                pChObj->vChannelId[vChId]
                    = pObj->resObj.vpdmaInfo[streamId].vpdmaChId[vChId];

                if ( pObj->resObj.vpdmaInfo[streamId].isMultiCh )
                {
                    /*
                     * pObj->resObj.vpdmaInfo[streamId].vpdmaChId[vChId]
                     * stores CH0 VPDMA CH ID,
                     * offset to current CH ID is calculated below
                     */

                    pChObj->vChannelId[vChId] += chId * vpdmaMultiChOffset;
                }
            }
        }
    }

    return FVID2_SOK;
}

/*
  Create queues for all channels and streams

  For each channel and each stream there is one input or free
  or empty buffer queue

  Each stream there is one output or full or completed buffer queue

  Queue depth for empty buffer queue is VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX

  Queue depth for full buffer is VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX*numCh

  Inaddition to this per channel and stream there is tmp que.
  This que is for keeping track of buffer submmited to the CLM, i.e
  buffers which are in the process of getting captured

  In case or errors in creation, previously created queues are
  deleted and error is returned
*/
Int32 Vps_captCreateQueues ( Vps_CaptObj * pObj )
{
    UInt16 chId, maxElements;
    Int16 streamId;
    Vps_CaptChObj *pChObj;
    Int32 status;
    Vps_CaptOutInfo *outInfo;
    Vps_CaptSubFrameChObj *pSubFrameObj;

    /*
     * create full que
     */
    for (streamId = 0; streamId < pObj->numStream; streamId ++)
    {

        maxElements = pObj->numCh * VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX;
        status = VpsUtils_queCreate ( &pObj->fullQue[streamId],
                                      maxElements,
                                      &pObj->fullQueMem[streamId][0],
                                      VPSUTILS_QUE_FLAG_BLOCK_QUE );

        if ( status != FVID2_SOK )
        {

            for ( streamId--; streamId >= 0; streamId-- )
            {
                status = VpsUtils_queDelete ( &pObj->fullQue[streamId] );

                GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );
            }
            return FVID2_EFAIL;
        }
    }
    /*
     * create full que
     */
    for (streamId = 0; streamId < pObj->numStream; streamId ++)
    {

        maxElements = pObj->numCh * VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX;
        status = VpsUtils_queCreate ( &pObj->fullQue1[streamId],
                                      maxElements,
                                      &pObj->fullQueMem1[streamId][0],
                                      VPSUTILS_QUE_FLAG_BLOCK_QUE );

        if ( status != FVID2_SOK )
        {

            for ( streamId--; streamId >= 0; streamId-- )
            {
                status = VpsUtils_queDelete ( &pObj->fullQue1[streamId] );

                GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );
            }
            return FVID2_EFAIL;
        }
    }


    /*
     * create empty que, tmp que and time stamp que
     */
    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        for ( chId = 0; chId < pObj->numCh; chId++ )
        {
            pChObj = &pObj->chObj[streamId][chId];

            maxElements = VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX;
            status = VpsUtils_queCreate ( &pChObj->emptyQue,
                                          maxElements,
                                          pChObj->emptyQueMem,
                                          VPSUTILS_QUE_FLAG_NO_BLOCK_QUE );

            GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

            status = VpsUtils_queCreate ( &pChObj->tmpQue,
                                          maxElements,
                                          pChObj->tmpQueMem,
                                          VPSUTILS_QUE_FLAG_NO_BLOCK_QUE );

            GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

            status = VpsUtils_queCreate ( &pChObj->tsFrameQue,
                                          maxElements,
                                          pChObj->tsFrameQueMem,
                                          VPSUTILS_QUE_FLAG_NO_BLOCK_QUE );

            GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );
        }
    }

    /*
     * For each channel / stream create a sub frame Q
     * Note that since we support sub framed based processing only for
     * non muxed mode of capture. Each stream will have only one channel
     */
    for ( streamId = 0; ((streamId < pObj->numStream) && (status == FVID2_SOK));
          streamId++ )
    {
        outInfo = &pObj->createArgs.outStreamInfo[streamId];
        if (outInfo->subFrameModeEnable == TRUE)
        {
            for ( chId = 0; ((chId < pObj->numCh) && (status == FVID2_SOK));
                  chId++ )
            {
                GT_assert( GT_DEFAULT_MASK,  chId == NULL );

                pSubFrameObj = &(pObj->chObj[streamId][chId].subFrameObj);
                /* Due to descriptor pacing mechanisim, at most we could have 3
                   elements in the sub frame Q.
                   SUBFRAME_TBD reduce the maxElements count to 3 */
                maxElements = VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX;
                status = VpsUtils_queCreate ( &pSubFrameObj->subFrameQ,
                                              maxElements,
                                              pSubFrameObj->subFrameQMem,
                                              VPSUTILS_QUE_FLAG_NO_BLOCK_QUE );
            }
        }
    }
    return FVID2_SOK;
}

/*
  Delete previously created queues
*/
Int32 Vps_captDeleteQueues ( Vps_CaptObj * pObj )
{
    UInt16 streamId, chId;
    Vps_CaptChObj *pChObj;
    Int32 status;

    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {

        status = VpsUtils_queDelete ( &pObj->fullQue[streamId] );

        GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

        status = VpsUtils_queDelete ( &pObj->fullQue1[streamId] );

        GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

        for ( chId = 0; chId < pObj->numCh; chId++ )
        {

            pChObj = &pObj->chObj[streamId][chId];

            status = VpsUtils_queDelete ( &pChObj->emptyQue );

            GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

            status = VpsUtils_queDelete ( &pChObj->tmpQue );

            GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

            status = VpsUtils_queDelete ( &pChObj->tsFrameQue );

            GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

            status = VpsUtils_queDelete ( &pChObj->subFrameObj.subFrameQ );

            GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

            /* Clear SubFrame Config */
            pChObj->isSubFrameEnabled = FALSE;

        }
    }

    return FVID2_SOK;
}

/*
  Check if data is available in any of the 'full' queues
*/
UInt32 Vps_captIsDataAvailable ( Vps_CaptObj * pObj )
{
    UInt32 isDataAvailable = FALSE;
    UInt16 streamId;

    /*
     * check is done for all streams
     */
    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        if ( !VpsUtils_queIsEmpty ( &pObj->fullQue[streamId] ) )
        {
            isDataAvailable = TRUE;
            break;
        }
    }

    return isDataAvailable;
}


/*
  Check if data is available in any of the 'full' queues
*/
UInt32 Vps_captIsDataAvailabeInOutQue1( Vps_CaptObj * pObj )
{
    UInt32 isDataAvailable = FALSE;
    UInt16 streamId;
    FVID2_Frame *pFrame;

    /*
     * check is done for all streams
     */
    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        if ( !VpsUtils_queIsEmpty ( &pObj->fullQue[streamId] ) )
        {
            VpsUtils_queGet(&pObj->fullQue[streamId],
                (Ptr *)&pFrame,
                1,
                BIOS_WAIT_FOREVER);
            VpsUtils_quePut(&pObj->fullQue1[streamId],
                pFrame,
                BIOS_WAIT_FOREVER);
            isDataAvailable = TRUE;
        }
    }

    return isDataAvailable;
}
/*
  Set frame skip mask

  Frame skip itself will happen from next list processing onwards

  This functionc can get called while the driver is running.
  i.e run time changing of frame skip mask is supported
*/
Int32 Vps_captSetFrameSkip ( Vps_CaptObj * pObj, Vps_CaptFrameSkip * frameSkip )
{
    UInt16 lChannelNum, instId, streamId, chId;
    Vps_CaptChObj *pChObj;
    Int32 status = FVID2_EFAIL;

    /*
     * parameter checfing, not supported for global handle
     */
    if ( pObj == ( Vps_CaptObj * ) VPS_CAPT_INST_VIP_ALL
         || pObj == NULL || frameSkip == NULL )
    {
        return FVID2_EFAIL;
    }

    /*
     * lock driver instance
     */
    Vps_captLock ( pObj );

    if ( pObj->state != VPS_CAPT_STATE_IDLE )
    {
        /*
         * driver needs to atleast be in open state for this call to work
         */

        /*
         * map from user channel number of driver channel number
         */
        lChannelNum = gVps_captCommonObj.fvidChannelNum2lChannelNumMap
            [frameSkip->channelNum];

        instId = Vps_captGetInstId ( lChannelNum );
        streamId = Vps_captGetStreamId ( lChannelNum );
        chId = Vps_captGetChId ( lChannelNum );

        if ( instId < VPS_CAPT_INST_MAX
             && streamId < pObj->numStream
             && chId < pObj->numCh && instId == pObj->instanceId )
        {

            /*
             * get channel object
             */
            pChObj = &pObj->chObj[streamId][chId];

            /*
             * lock list processing, since this value is checking
             * during list processing
             */
            Vps_captLmLock (  );

            /*
             * change frame skip mask
             */
            pChObj->frameSkipMask = frameSkip->frameSkipMask;

            /*
             * unlock list processing
             */
            Vps_captLmUnlock (  );

            status = FVID2_SOK;
        }
    }

    /*
     * unlock driver instance
     */
    Vps_captUnlock ( pObj );

    return status;
}

Int32 Vps_captGetChStatus ( Vps_CaptObj * pObj, Vps_CaptChGetStatusArgs * args,
                            Vps_CaptChStatus * chStatus )
{
    UInt16 lChannelNum, instId, streamId, chId;
    Vps_CaptChObj *pChObj;
    UInt32 curTime;
    Int32 status = FVID2_EFAIL;

    /*
     * parameter checking, not supported for global handle
     */
    if ( pObj == ( Vps_CaptObj * ) VPS_CAPT_INST_VIP_ALL
         || pObj == NULL || args == NULL || chStatus == NULL )
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
    Vps_captLock ( pObj );

    if ( pObj->state != VPS_CAPT_STATE_IDLE )
    {
        /*
         * driver needs to atleast be in open state for this call to work
         */

        /*
         * map from user channel number of driver channel number
         */
        lChannelNum = gVps_captCommonObj.fvidChannelNum2lChannelNumMap
            [args->channelNum];

        instId = Vps_captGetInstId ( lChannelNum );
        streamId = Vps_captGetStreamId ( lChannelNum );
        chId = Vps_captGetChId ( lChannelNum );

        if ( instId < VPS_CAPT_INST_MAX
             && streamId < pObj->numStream
             && chId < pObj->numCh && instId == pObj->instanceId )
        {

            /*
             * get channel object
             */
            pChObj = &pObj->chObj[streamId][chId];

            /*
             * lock list processing, since these value iare updated
             * during list processing
             */
            Vps_captLmLock (  );

            /*
             * get latest frame width x height
             */
            chStatus->outWidth = pChObj->lastFrameWidth;
            chStatus->outHeight = pChObj->lastFrameHeight;

            /*
             * get dropped frame count
             */
            chStatus->droppedFrameCount = pChObj->droppedFrameCount;
            chStatus->captureFrameCount = pChObj->captureFrameCount;

            /*
             * get current
             */
            curTime = Clock_getTicks (  );

            /*
             * if current time exceeds expected frame interval,
             * then its considered that video is not detected
             */
            if ( curTime < ( pChObj->lastFrameTimestamp
                             + args->frameInterval
                             + VPS_CAPT_LIST_UPDATE_TIME_IN_TICKS ) )
            {
                chStatus->isVideoDetected = TRUE;
            }

            /*
             * unlock list processing
             */
            Vps_captLmUnlock (  );

            status = FVID2_SOK;
        }
    }

    /*
     * unlock driver instance
     */
    Vps_captUnlock ( pObj );

    return status;
}

/* Driver object lock */
Int32 Vps_captLock ( Vps_CaptObj * pObj )
{
    /*
     * take semaphore for locking
     */
    Semaphore_pend ( pObj->lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/* Driver object unlock */
Int32 Vps_captUnlock ( Vps_CaptObj * pObj )
{
    /*
     * release semaphore for un-locking
     */
    Semaphore_post ( pObj->lock );

    return FVID2_SOK;
}

/* create driver semaphore  */
Int32 Vps_captLockCreate ( Vps_CaptObj * pObj )
{
    Int32 retVal = FVID2_SOK;
    Semaphore_Params semParams;

    /*
     * create locking semaphore
     */
    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    pObj->lock = Semaphore_create ( 1u, &semParams, NULL );

    if ( pObj->lock == NULL )
    {
        Vps_printf ( " CAPTURE:%s:%d: Semaphore_create() failed !!!\n",
                        __FUNCTION__, __LINE__ );
        retVal = FVID2_EALLOC;
    }

    return retVal;
}

/* delete driver semaphore  */
Int32 Vps_captLockDelete ( Vps_CaptObj * pObj )
{
    /*
     * delete semaphore
     */
    Semaphore_delete ( &pObj->lock );

    return FVID2_SOK;
}

Int32 Vps_captCheckOverflow(Vps_CaptOverFlowStatus *overFlowStatus)
{
    Int32 status = FVID2_SOK;
    UInt32 platform, cpuRev;

    platform = Vps_platformGetId();
    cpuRev   = Vps_platformGetCpuRev();

    VpsHal_vipInstPortIsOverflow(
        VPSHAL_VIP_INST_0,
        &overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTA],
        &overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTB]);
    VpsHal_vipInstPortIsOverflow(
        VPSHAL_VIP_INST_1,
        &overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTA],
        &overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTB]);

    if (!((platform == VPS_PLATFORM_ID_EVM_TI816x) &&
         (cpuRev > VPS_PLATFORM_CPU_REV_1_1)))
    {
        if (overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTA] ||
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTB])
        {
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTB] = TRUE;
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTA] = TRUE;
        }
        if (overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTA] ||
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTB])
        {
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTB] = TRUE;
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTA] = TRUE;
        }
    }

    return status;
}

Int32 Vps_captResetAndRestart(Vps_CaptOverFlowStatus *overFlowStatus)
{
    Int32 status = FVID2_SOK, instId, resetDelay;
    UInt32 streamId, chCnt;
    Vps_CaptObj *pObj = NULL;
    UInt32 platform, cpuRev;

    platform = Vps_platformGetId();
    cpuRev   = Vps_platformGetCpuRev();

    resetDelay = 0;

    if (!((platform == VPS_PLATFORM_ID_EVM_TI816x) &&
          (cpuRev > VPS_PLATFORM_CPU_REV_1_1)))
    {
        if (overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTA] ||
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTB])
        {
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTB] = TRUE;
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTA] = TRUE;
        }

        if (overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTA] ||
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTB])
        {
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTB] = TRUE;
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTA] = TRUE;
        }
    }

    /*
     * stop instance if required
     */
    for (instId = 0u; instId < VPS_CAPT_INST_MAX; instId++ )
    {
        if (overFlowStatus->isPortOverFlowed[instId])
        {
            gVps_captCommonObj.captureObj[instId].resetStatistics = FALSE;

            /* Since this is called as part of reset and reset ioctl,
               there is no need to do reset as part of captStop. */
            Vps_captStop(&gVps_captCommonObj.captureObj[instId], 0u);
        }
    }

    if ((gVps_captCommonObj.captureObj[0].platform == VPS_PLATFORM_ID_EVM_TI816x) &&
        (gVps_captCommonObj.captureObj[0].cpuRev > VPS_PLATFORM_CPU_REV_1_1))
    {
        /*
         * Reset
         */
        for (instId = 0u; instId < VPS_CAPT_INST_MAX; instId++ )
        {
            if (overFlowStatus->isPortOverFlowed[instId])
            {
                Task_sleep(resetDelay);

                gVps_captCommonObj.captureObj[instId].resetStatistics = FALSE;

                Vps_captVipPortAndModulesReset(&gVps_captCommonObj.captureObj[instId]);

                gVps_captCommonObj.resetCount++;
                Task_sleep(resetDelay);
            }
        }
    }
    else
    {
        if (overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTA] ||
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTB])
        {
            Task_sleep(resetDelay);
            Vps_captVipBlockReset(&gVps_captCommonObj.captureObj[VPS_CAPT_INST_VIP0_PORTA]);
            gVps_captCommonObj.resetCount++;
            Task_sleep(resetDelay);
        }
        if (overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTA] ||
            overFlowStatus->isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTB])
        {
            Task_sleep(resetDelay);
            Vps_captVipBlockReset(&gVps_captCommonObj.captureObj[VPS_CAPT_INST_VIP1_PORTA]);
            gVps_captCommonObj.resetCount++;
            Task_sleep(resetDelay);
        }
    }

    /*
     * restart instance if required
     */
    for ( instId = 0; instId < VPS_CAPT_INST_MAX; instId++ )
    {
        pObj = &gVps_captCommonObj.captureObj[instId];

        if(overFlowStatus->isPortOverFlowed[instId])
        {
            for (streamId = 0u; streamId < pObj->numStream; streamId ++)
            {
                for (chCnt = 0u; chCnt < pObj->numCh; chCnt ++)
                {
                    pObj->chObj[streamId][chCnt].numDropData =
                            VPS_CAPT_MAX_DROP_FRAMES;
                }
            }

            Vps_captStart(pObj);
        }
    }

    return status;
}

Int32 Vps_captPrintAdvancedStatistics(UInt32 totalTimeInMsecs)
{
    Vps_CaptObj *pDrvObj;
    UInt32 streamId, chId, drvId, totalFieldCount;
    Vps_CaptChObj *pChObj;

#ifdef VPS_CPAT_ERR_TIME_TRACK_ENABLE
    UInt32 errIndex;
#endif /* VPS_CPAT_ERR_TIME_TRACK_ENABLE */

#ifdef VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE
    UInt32 fidIndex;
#endif /* VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE */

    Vps_printf(" \r\n");
    Vps_printf(" *** Capture Driver Advanced Statistics *** \r\n");
    Vps_printf(" \r\n");
    Vps_printf(" VIP Parser Reset Count : %d\r\n", gVps_captCommonObj.resetCount);
    Vps_printf(" \r\n");

#ifdef VPS_CAPT_PRINT_TS_STATS
    Vps_printf("     |   Total    Even     Odd  Total  Even   Odd  Min /  Max  Min /  Max Average Time     Dropped Fid Repeat Frame Error Y/C\r\n");
    Vps_printf(" CH  |  Fields  Fields  Fields    FPS   FPS   FPS       Width      Height Period (Min/Max) Fields      Count (Desc Error Y/C)\r\n");
    Vps_printf(" -----------------------------------------------------------------------------------------------------------------------\r\n");
#else
    Vps_printf("     |   Total    Even     Odd  Total  Even   Odd  Min /  Max  Min /  Max Dropped Fid Repeat Frame Error Y/C\r\n");
    Vps_printf(" CH  |  Fields  Fields  Fields    FPS   FPS   FPS       Width      Height  Fields      Count (Desc Error Y/C)\r\n");
    Vps_printf(" ------------------------------------------------------------------------------------------------------------\r\n");
#endif /* VPS_CAPT_PRINT_TS_STATS */

    for(drvId=0; drvId<VPS_CAPT_INST_MAX; drvId++)
    {
        pDrvObj = &gVps_captCommonObj.captureObj[drvId];

        if( pDrvObj->state == VPS_CAPT_STATE_IDLE)
            continue;

        for(streamId=0; streamId<pDrvObj->createArgs.numStream; streamId++)
        {
            for(chId=0; chId<pDrvObj->createArgs.numCh; chId++)
            {
                pChObj = &pDrvObj->chObj[streamId][chId];

                totalFieldCount = pChObj->droppedFrameCount + pChObj->captureFrameCount;

                if(totalFieldCount)
                {
#ifdef VPS_CAPT_PRINT_TS_STATS
                    Vps_printf(" %d%d%d | %7d %7d %7d %6d %5d %5d %4d / %-4d %4d / %-4d %-4d (%2d / %-4d)   %-7d %-10d %d/%d (%d/%d)\r\n",
#else
                    Vps_printf(" %d%d%d | %7d %7d %7d %6d %5d %5d %4d / %4d %4d / %4d %7d %10d %d/%d (%d/%d)\r\n",
#endif /* VPS_CAPT_PRINT_TS_STATS */
                        drvId,
                        streamId,
                        chId,
                        totalFieldCount,
                        pChObj->fieldCount[0],
                        pChObj->fieldCount[1],
                        totalFieldCount/(totalTimeInMsecs/1000),
                        pChObj->fieldCount[0]/(totalTimeInMsecs/1000),
                        pChObj->fieldCount[1]/(totalTimeInMsecs/1000),
                        pChObj->minRecvFrameWidth,
                        pChObj->maxRecvFrameWidth,
                        pChObj->minRecvFrameHeight,
                        pChObj->maxRecvFrameHeight,
#ifdef VPS_CAPT_PRINT_TS_STATS
                        pChObj->tsCummulative / totalFieldCount,
                        pChObj->tsMinDiff,
                        pChObj->tsMaxDiff,
#endif /* VPS_CAPT_PRINT_TS_STATS */
                        pChObj->droppedFrameCount,
                        pChObj->fidRepeatCount,
                        pChObj->frameNotWrittenCount,
                        pChObj->secFrameNotWrittenCount,
                        pChObj->descNotWrittenCount,
                        pChObj->secDescNotWrittenCount);
                }
            }
        }
    }


#ifdef VPS_CPAT_ERR_TIME_TRACK_ENABLE
    Vps_printf("CH No || Frame NotWritten || 2 Frame NotWritten || Desc NotWritten||\n");
    for(drvId=0; drvId<VPS_CAPT_INST_MAX; drvId++)
    {
        pDrvObj = &gVps_captCommonObj.captureObj[drvId];

        if( pDrvObj->state == VPS_CAPT_STATE_IDLE)
            continue;

        for(streamId=0; streamId<pDrvObj->createArgs.numStream; streamId++)
        {
            for(chId=0; chId<pDrvObj->createArgs.numCh; chId++)
            {
                pChObj = &pDrvObj->chObj[streamId][chId];

                if (pChObj->errTimeStampIndex > 2000u)
                {
                    continue;
                }
                for (errIndex = 0x0; errIndex <= pChObj->errTimeStampIndex; errIndex++)
                {
                    Vps_printf("%5d || %16d || %18d || %14d ||\n",
                        pChObj->errTimeStamps[errIndex].chanNo,
                        pChObj->errTimeStamps[errIndex].frameNotWrittenTimeStamp,
                        pChObj->errTimeStamps[errIndex].secFrameNotWrittenTimeStamp,
                        pChObj->errTimeStamps[errIndex].descNotWrittenTimeStamp);
                }
            }
        }
    }
#endif /* VPS_CPAT_ERR_TIME_TRACK_ENABLE */

#ifdef VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE
    Vps_printf("CH No || FID Repetation @ \n");
    for(drvId=0; drvId<VPS_CAPT_INST_MAX; drvId++)
    {
        pDrvObj = &gVps_captCommonObj.captureObj[drvId];

        if( pDrvObj->state == VPS_CAPT_STATE_IDLE)
            continue;

        for(streamId=0; streamId<pDrvObj->createArgs.numStream; streamId++)
        {
            for(chId=0; chId<pDrvObj->createArgs.numCh; chId++)
            {
                pChObj = &pDrvObj->chObj[streamId][chId];

                if (pChObj->fidRptIndex > 2000u)
                {
                    continue;
                }
                for (fidIndex = 0x0; fidIndex <= pChObj->fidRptIndex; fidIndex++)
                {
                    Vps_printf("%5d || %16d \n",
                        pChObj->fidRptTimeStamps[fidIndex].chanNo,
                        pChObj->fidRptTimeStamps[fidIndex].fidRptTimeStamp);
                }
            }
        }
    }
#endif /* VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE */

    Vps_printf(" \r\n");
    for(drvId=0; drvId<VPS_CAPT_INST_MAX; drvId++)
    {
        pDrvObj = &gVps_captCommonObj.captureObj[drvId];

        if( pDrvObj->state == VPS_CAPT_STATE_IDLE)
            continue;
        Vps_printf(" VIP Capture Port %d | DescMissMatch1 = %d, DescMissMatch2 = %d , DescMissMatch3 = %d \n",
            drvId,
            pDrvObj->firstDescMissMatch,
            pDrvObj->secDescMissMatch,
            pDrvObj->thirdDescMissMatch);
    }

    #ifdef VPS_CAPT_LIST_ENABLE_ADV_STATISTICS_LOG
    Vps_captLmPrintAdvancedStatistics();
    #endif

    Vps_printf(" \r\n");

    return 0;
}

Int32 Vps_captDequeueFromQueToFrameList(VpsUtils_QueHandle *queHandle, FVID2_FrameList *frameList)
{
    Int32 dequeStatus;
    volatile FVID2_Frame *pData;

    do
    {
        /*
         * Max frames limit exceeded exit
         */
        if(frameList->numFrames >= FVID2_MAX_FVID_FRAME_PTR)
           break;

        pData = NULL;

        /*
         * deque from handle, could be blocking for the first time
         * based on 'timeout'
         */
        dequeStatus = VpsUtils_queGet ( queHandle,
                                        (Ptr*)&pData, 1, BIOS_NO_WAIT );

        if ( dequeStatus == FVID2_SOK && pData != NULL && pData->channelNum != VPS_CAPT_DROP_FRAME_CH_ID)
        {
            /*
             * deque is successful, add to frame list
             */
            frameList->frames[frameList->numFrames] =
                ( FVID2_Frame * ) pData;
            frameList->numFrames++;
        }

        /*
         * Max frames limit exceeded exit
         */
        if(frameList->numFrames >= FVID2_MAX_FVID_FRAME_PTR)
           break;
    }
    while ( dequeStatus == FVID2_SOK );

    return FVID2_SOK;
}

Int32 Vps_captSetStorageFmt(Fdrv_Handle handle,
                            const Vps_CaptStorageParams *storagePrms)
{
    Int32               retVal = FVID2_SOK;
    Vps_CaptObj         *pObj = ( Vps_CaptObj * ) handle;
    Vps_CaptChObj       *pChObj;
    UInt32              streamId, chId;

    /* Return error if any of the required parameters is NULL */
    if ((NULL == pObj) || (NULL == storagePrms))
    {
       Vps_printf( " %s:%d: ERROR: Null Pointer !!!\n",__FUNCTION__, __LINE__);
       retVal = FVID2_EBADARGS;
       return retVal;
    }
    /* IOCTL cannot be called when capture is in progress */
    if (VPS_CAPT_STATE_RUNNING == pObj->state)
    {
        Vps_printf( " %s:%d: ERROR: Streamin On !!!\n",__FUNCTION__, __LINE__);
        retVal = FVID2_EFAIL;
        return retVal;
    }
    /*
     * for every stream and ever channel do ..
     */
    pObj->frameCapture = (FVID2_BufferFormat) storagePrms->bufferFmt;
    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        for ( chId = 0; chId < pObj->numCh; chId++ )
        {
            /*
             * get channel object
             */
            pChObj = &pObj->chObj[streamId][chId];

            pChObj->fieldsMerged = storagePrms->fieldMerged;
            pChObj->frameCapture =
                (FVID2_BufferFormat) storagePrms->bufferFmt;
            if ((pChObj->memType == VPSHAL_VPDMA_MT_TILEDMEM) &&
                (FVID2_BUF_FMT_FRAME == storagePrms->bufferFmt))
            {
                retVal = FVID2_EINVALID_PARAMS;
                Vps_printf( " %s:%d: ERROR: Frame capture not supported "
                    "for Tiled Mem\n",
                    __FUNCTION__, __LINE__);
                break;
            }
            if (pObj->createArgs.inScanFormat == FVID2_SF_PROGRESSIVE)
            {
                pChObj->fieldsMerged = TRUE;
                pChObj->frameCapture = FVID2_BUF_FMT_FIELD;
                pObj->frameCapture = FVID2_BUF_FMT_FIELD;
            }
        }
    }
    /* If we are not able to set the correct storage format because of tiler
     * then set the storage format as field for all the channels for the
     * capture instance
     */
    if (FVID2_SOK != retVal)
    {
        pObj->frameCapture = FVID2_BUF_FMT_FIELD;
        for ( streamId = 0; streamId < pObj->numStream; streamId++ )
        {
            for ( chId = 0; chId < pObj->numCh; chId++ )
            {
                /*
                 * get channel object
                 */
                pChObj = &pObj->chObj[streamId][chId];
                pChObj->fieldsMerged = TRUE;
                pChObj->frameCapture = FVID2_BUF_FMT_FIELD;
            }
        }
    }
    return (retVal);
}

Int32 Vps_captSetVipCropCfg(Fdrv_Handle handle,
                            const Vps_CaptVipCropParams *vipCropPrms)
{
    Int32                   retVal = FVID2_SOK;
    Vps_CaptObj            *pObj = (Vps_CaptObj *) handle;
    VpsHal_VipCropConfig    vipHalCropCfg;

    /* Return error if any of the required parameters is NULL */
    if ((NULL == pObj) || (NULL == vipCropPrms))
    {
       Vps_printf( " %s:%d: ERROR: Null Pointer !!!\n",__FUNCTION__, __LINE__);
       retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        /* IOCTL cannot be called when capture is in progress */
        if (VPS_CAPT_STATE_RUNNING == pObj->state)
        {
            Vps_printf( " %s:%d: ERROR: Stream is On !!!\n",
                __FUNCTION__, __LINE__);
            retVal = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (!(((VPS_PLATFORM_ID_EVM_TI816x == pObj->platform) &&
                (pObj->cpuRev >= VPS_PLATFORM_CPU_REV_2_0)) ||
              ((VPS_PLATFORM_ID_EVM_TI814x == pObj->platform) &&
                (pObj->cpuRev >= VPS_PLATFORM_CPU_REV_2_1)) ||
              (VPS_PLATFORM_ID_EVM_TI8107 == pObj->platform)))
        {
            /* VIP trimmer is supported only in TI816X ES2.0 and TI814x ES2.1 */
            Vps_printf(" %s:%d: VIP trimmer not supported for this CPU!!!\n",
                __FUNCTION__, __LINE__);
            retVal = FVID2_EUNSUPPORTED_CMD;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Currently only channel 0 of a PORT is supported */
        vipHalCropCfg.channelNum = 0u;
        vipHalCropCfg.vipCropEnable = vipCropPrms->vipCropEnable;
        memcpy(
            &vipHalCropCfg.vipCropCfg,
            &vipCropPrms->vipCropCfg,
            sizeof(Vps_CropConfig));
        retVal = Vcore_vipSetVipActCropCfg(&pObj->resObj, &vipHalCropCfg);
    }

    return (retVal);
}

Int32 Vps_captGetVipCropCfg(Fdrv_Handle handle,
                            Vps_CaptVipCropParams *vipCropPrms)
{
    Int32                   retVal = FVID2_SOK;
    Vps_CaptObj            *pObj = ( Vps_CaptObj * ) handle;
    VpsHal_VipCropConfig    vipHalCropCfg;

    /* Return error if any of the required parameters is NULL */
    if ((NULL == pObj) || (NULL == vipCropPrms))
    {
       Vps_printf( " %s:%d: ERROR: Null Pointer !!!\n",__FUNCTION__, __LINE__);
       retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        /* IOCTL cannot be called when capture is in progress */
        if (VPS_CAPT_STATE_RUNNING == pObj->state)
        {
            Vps_printf( " %s:%d: ERROR: Stream is On !!!\n",
                __FUNCTION__, __LINE__);
            retVal = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (!(((VPS_PLATFORM_ID_EVM_TI816x == pObj->platform) &&
                (pObj->cpuRev >= VPS_PLATFORM_CPU_REV_2_0)) ||
              ((VPS_PLATFORM_ID_EVM_TI814x == pObj->platform) &&
                (pObj->cpuRev >= VPS_PLATFORM_CPU_REV_2_1)) ||
              (VPS_PLATFORM_ID_EVM_TI8107 == pObj->platform)))
        {
            /* VIP trimmer is supported only in TI816X ES2.0 and TI814x ES2.1 */
            Vps_printf(" %s:%d: VIP trimmer not supported for this CPU!!!\n",
                __FUNCTION__, __LINE__);
            retVal = FVID2_EUNSUPPORTED_CMD;
        }
    }

    if (FVID2_SOK == retVal)
    {
        retVal = Vcore_vipGetVipActCropCfg(&pObj->resObj, &vipHalCropCfg);
        if (FVID2_SOK == retVal)
        {
            /* Currently only channel 0 of a PORT is supported */
            vipCropPrms->channelNum = 0u;
            vipCropPrms->vipCropEnable = vipHalCropCfg.vipCropEnable;
            memcpy(
                &vipCropPrms->vipCropCfg,
                &vipHalCropCfg.vipCropCfg,
                sizeof(Vps_CropConfig));

        }
    }

    return (retVal);
}
Int32 Vps_returnErrFrm(Fdrv_Handle handle, Vps_CaptReturnErrFrms *cmdArgs)
{
    Vps_CaptObj *pObj = ( Vps_CaptObj * ) handle;

    pObj->returnErrFrm = cmdArgs->enable;
    return FVID2_SOK;
}

Int32 Vps_oneCbPerFrm(Fdrv_Handle handle, Vps_CaptOneCallBackPerFrm *cmdArgs)
{
    Vps_CaptObj *pObj = ( Vps_CaptObj * ) handle;

    pObj->oneCallBackPerFrm = cmdArgs->enable;
    return FVID2_SOK;
}

Int32 Vps_captFlush( Fdrv_Handle handle, FVID2_FrameList * frameList )
{
    Vps_CaptObj *pObj = ( Vps_CaptObj * ) handle;
    Int32 status = FVID2_SOK, instId, streamId, chId;
    Vps_CaptChObj *pChObj;


    /*
     * if global handle then make handle as NULL, this indicates
     * we need to check over all driver instances
     */
    if ( handle == ( Fdrv_Handle ) VPS_CAPT_INST_VIP_ALL )
        handle = NULL;

    /*
     * parameter checking
     */
    if (frameList == NULL )
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
    for ( instId = 0; instId < VPS_CAPT_INST_MAX; instId++ )
    {

        if ( handle == NULL )
        {
            /*
             * if handle is NULL, i.e global handle, then get handle using 'instId',
             * else use user supplied handle
             */
            pObj = &gVps_captCommonObj.captureObj[instId];
        }

        if ( pObj->state == VPS_CAPT_STATE_CREATED
                ||
             pObj->state == VPS_CAPT_STATE_STOPPED
            )
        {
            /* dequeue frames to frame list for this driver inst */
            for (streamId = 0u; streamId < pObj->numStream; streamId ++)
            {
                Vps_captDequeueFromQueToFrameList(&pObj->fullQue[streamId], frameList);

                for (chId = 0u; chId < pObj->numCh; chId ++)
                {
                    pChObj = &pObj->chObj[streamId][chId];

                    Vps_captDequeueFromQueToFrameList(&pChObj->tmpQue, frameList);
                    Vps_captDequeueFromQueToFrameList(&pChObj->emptyQue, frameList);
                    if (pChObj->tsUseBestEffort == TRUE)
                    {
                        Vps_captTimeStampFlushQue( pChObj );
                    }
                }
            }
        }
        else
        {
            /* invalid state for dequeue all */
            if ( handle != NULL )
                status = FVID2_EFAIL;
        }

        if ( handle != NULL )
            break;  /* if not global handle, then we are done, so exit */

        /*
         * Max frames limit exceeded exit
         */
        if(frameList->numFrames >= FVID2_MAX_FVID_FRAME_PTR)
           break;

        /*
         * global handle mode, so check other driver instances also
         */
    }

    return status;
}


Int32 Vps_captVipReset(Fdrv_Handle handle)
{
    Vps_CaptObj *pObj = (Vps_CaptObj *)handle;
    Int32 status = FVID2_SOK;

    if (handle == (Fdrv_Handle )VPS_CAPT_INST_VIP_ALL)
    {
        /* This ioctl cannot be used with the global handle */
        status = FVID2_EFAIL;
    }

    if (!((VPS_PLATFORM_ID_EVM_TI816x == pObj->platform) &&
          (pObj->cpuRev > VPS_PLATFORM_CPU_REV_1_1)))
    {
        /* This ioctl can be used only for TI816X ES2.0 */
        status = FVID2_EUNSUPPORTED_CMD;
    }

    if (FVID2_SOK == status)
    {
        status = Vps_captVipPortAndModulesReset(pObj);
    }

    return status;
}


/**
 *  Vps_captSubFrameCheckArgs
 *  \brief This function performs pre-creation checkf in case sub frame based
 *         capturing is enabled.
 *
 *  \param createArgs   Create arguments supplied by the applications.
 *
 *  \return             Returns 0 on success else returns FVID2_EBADARGS.
 */

static Int32 Vps_captSubFrameCheckArgs( Vps_CaptCreateParams * createArgs )
{
    Int32 rtnValue = FVID2_EBADARGS;
    Vps_CaptOutInfo *outInfo;
    UInt32 numStream, numLines;

    while ( TRUE )
    {
        /* If subframe processing is enabled following conditions should be met
         * 1. subFrame call back should be provided
         * 2. N lines should be greater than total number of lines
         * 3. Maximum output height should not be set to UN LIMITED
         * 4. Field merged mode should not be enabled
         * 5. Should be configured to operate in single channel mode only
         */
        for ( numStream = 0x0; numStream < createArgs->numStream; numStream++ )
        {
            outInfo = &createArgs->outStreamInfo[numStream];
            rtnValue = FVID2_EBADARGS;
            if ( outInfo->subFrameModeEnable == TRUE )
            {
                if ( outInfo->subFrameCb == NULL )
                {
                    break;
                }

                if ( outInfo->numLinesInSubFrame == 0x0u )
                {
                    break;
                }

                if ( outInfo->scEnable == TRUE )
                {
                    numLines = createArgs->scParams.outHeight;
                }
                else
                {
                    switch ( outInfo->maxOutHeight )
                    {
                        case VPS_CAPT_MAX_OUT_HEIGHT_288_LINES:
                            numLines = 288u;
                            break;
                        case VPS_CAPT_MAX_OUT_HEIGHT_576_LINES:
                            numLines = 576;
                            break;
                        case VPS_CAPT_MAX_OUT_HEIGHT_720_LINES:
                            numLines = 720;
                            break;
                        case VPS_CAPT_MAX_OUT_HEIGHT_1080_LINES:
                            numLines = 1080;
                            break;
                        case VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED:
                            numLines = VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED;
                            break;
                        default :
                            GT_assert( GT_DEFAULT_MASK,  FALSE );
                    }
                }
                if ( VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED != numLines )
                {
                    if ( outInfo->maxOutHeight >= numLines )
                    {
                        break;
                    }
                }

                /* SUBFRAME_TBD Add a check in frame based capture to ensure
                   frame based capture is not enabled when sub frame based
                   capture is enabled */
                if ( ( createArgs->videoCaptureMode !=
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC)
                    &&
                     (  createArgs->videoCaptureMode !=
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VBLK
                    )
                    &&
                     (  createArgs->videoCaptureMode !=
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VSYNC
                    )
                    &&
                     (  createArgs->videoCaptureMode !=
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK
                    )
                    &&
                     (  createArgs->videoCaptureMode !=
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC
                    ) )
                {
                    break;
                }
            }
            rtnValue = FVID2_SOK;
        }
        break;
    }
    return (rtnValue);
}

/**
 *  Vps_captSubFrameInitArgs
 *  \brief This function initializes sub frame variables and allocates memory
 *         sub frame configuration per channel / stream.
 *
 *  \param createArgs   Create arguments supplied by the applications.
 *
 *  \return             Returns 0 on success else returns FVID2_EBADARGS.
 */

static Int32 Vps_captSubFrameInitArgs(  Vps_CaptObj * pObj )
{
    UInt32 streamId, chId, index, temp;
    Vps_CaptOutInfo *outInfo;
    Vps_CaptChObj *pChObj;
    Vps_CaptSubFrameQObj *subFrameQInst;

    gVps_captSubFrameQIndex = 0x0;

    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        outInfo = &pObj->createArgs.outStreamInfo[streamId];
        if (outInfo->subFrameModeEnable == TRUE)
        {
            for ( chId = 0; chId < pObj->numCh; chId++ )
            {
                GT_assert( GT_DEFAULT_MASK,  chId == NULL );

                pChObj = &pObj->chObj[streamId][chId];

                /* Initialize field queue objects */
                for (index = 0x0;  index < VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX;
                     index++)
                {
                    subFrameQInst = &gVps_captSubFrameQObj[streamId][index];
                    subFrameQInst->status       = FVID2_SOK;
                    subFrameQInst->pFrame       = NULL;
                    subFrameQInst->pChObj       = pChObj;
                    subFrameQInst->handle       = pObj;
                    subFrameQInst->totalNoOfLinesPChan  =   0x0;
                    subFrameQInst->totalNoOfLinesSChan  =   0x0;
                    subFrameQInst->allocStatus  = FALSE;
                    subFrameQInst->subFrameNo   = 0x0;
                }

                /* Update sub frame channel configurations */
                pChObj->isSubFrameEnabled = TRUE;
                pChObj->subFrameObj.subFrameCb = outInfo->subFrameCb;
                pChObj->subFrameObj.streamId = streamId;
                pChObj->subFrameObj.nLineCount = outInfo->numLinesInSubFrame;
                pChObj->subFrameObj.subFrameSize = outInfo->numLinesInSubFrame
                                    + VPS_CAPT_SUBFRAME_ADDITIONAL_NO_OF_LINES;

                if ( outInfo->scEnable == TRUE )
                {
                    pChObj->subFrameObj.maxFrameHeight
                            = pObj->createArgs.scParams.outHeight;
                }
                else
                {
                    switch (outInfo->maxOutHeight)
                    {
                        case VPS_CAPT_MAX_OUT_HEIGHT_288_LINES :
                            pChObj->subFrameObj.maxFrameHeight = 288u;
                            break;
                        case VPS_CAPT_MAX_OUT_HEIGHT_576_LINES :
                            pChObj->subFrameObj.maxFrameHeight = 576u;
                            break;
                        case VPS_CAPT_MAX_OUT_HEIGHT_720_LINES :
                            pChObj->subFrameObj.maxFrameHeight = 720u;
                            break;
                        case VPS_CAPT_MAX_OUT_HEIGHT_1080_LINES :
                            pChObj->subFrameObj.maxFrameHeight = 1080u;
                            break;
                        case VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED :
                            pChObj->subFrameObj.maxFrameHeight =
                                VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED;
                            break;
                        default :
                            GT_assert( GT_DEFAULT_MASK, FALSE);
                    }
                }
                if ( VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED !=
                            pChObj->subFrameObj.maxFrameHeight )
                {
                    /* expectSFCount =  max height / Actual sub frame size
                       if max height % Actual sub frame size != 0
                        expectSFCount++  */
                    if ( pObj->createArgs.inScanFormat == FVID2_SF_INTERLACED )
                    {
                        pChObj->subFrameObj.maxFrameHeight /= 2;
                    }

                    temp = (pChObj->subFrameObj.maxFrameHeight /
                            pChObj->subFrameObj.subFrameSize);

                    pChObj->subFrameObj.expectSFCount = temp;

                    temp = (pChObj->subFrameObj.maxFrameHeight %
                             pChObj->subFrameObj.subFrameSize);
                    if (temp)
                    {
                        pChObj->subFrameObj.expectSFCount++;
                    }
                }
                else
                {
                    /* Some very large number */
                    pChObj->subFrameObj.expectSFCount =
                            VPS_CAPT_SUBFRAME_HEIGHT_UNLIM_EXPECTED_SF_COUNT;
                }
                /* Get the register addresses of FID and size, will be used
                   in ISR to determine FID and size */
                VpsHal_vipGetFidRegAddr ( pObj->resObj.resInfo.vipHalHandle,
                                          &(pChObj->subFrameObj.fidReg),
                                          &(pChObj->subFrameObj.maskFid),
                                          &(pChObj->subFrameObj.shiftFid));

                if ( outInfo->scEnable == TRUE )
                {
                    pChObj->subFrameObj.srcSizeHeightReg =
                                        &pObj->createArgs.scParams.outHeight;
                                        pChObj->subFrameObj.maskHeight = 0xFFFF;
                                        pChObj->subFrameObj.shiftHeight = 0x0;
                    pChObj->subFrameObj.srcSizeWidthReg =
                                        &pObj->createArgs.scParams.outWidth;
                                        pChObj->subFrameObj.maskWidth = 0xFFFF;
                                        pChObj->subFrameObj.shiftWidth = 0x0;
                }
                else
                {
                    VpsHal_vipGetSrcHeightRegAddr (
                                        pObj->resObj.resInfo.vipHalHandle,
                                        &(pChObj->subFrameObj.srcSizeHeightReg),
                                        &(pChObj->subFrameObj.maskHeight),
                                        &(pChObj->subFrameObj.shiftHeight));
                    VpsHal_vipGetSrcWidthRegAddr (
                                        pObj->resObj.resInfo.vipHalHandle,
                                        &(pChObj->subFrameObj.srcSizeWidthReg),
                                        &(pChObj->subFrameObj.maskWidth),
                                        &(pChObj->subFrameObj.shiftWidth));
                }
            }
        }
    }

    return (FVID2_SOK);
}

void Vps_CaptOverflowCallback( )
{

    UInt32 platform, cpuRev;
    Bool isBlockRst = TRUE;

    platform = Vps_platformGetId();
    cpuRev   = Vps_platformGetCpuRev();

    if ((platform == VPS_PLATFORM_ID_EVM_TI816x) &&
                (cpuRev > VPS_PLATFORM_CPU_REV_1_1))
    {
        isBlockRst = FALSE;
    }

    VpsHal_vipInstChkOverflowAndDisable(VPSHAL_VIP_INST_0 , isBlockRst);
    VpsHal_vipInstChkOverflowAndDisable(VPSHAL_VIP_INST_1 , isBlockRst);

}

/**
 *  Vps_captClockGetTicks
 *  \brief A simple wrapper function for DSP/BIOS clock get ticks.
 *
 *  \param unUsed   Un used argument
 *
 *  \return  Returns an UINT32 value.
 */
static UInt32 Vps_captClockGetTicks(Ptr unUsed)
{
    return Clock_getTicks();
}

/**
 *  Vps_captDeInitTimeStamp
 *  \brief This function clear time stamp configurations.
 *
 *  \param pObj         Pointer to instance object.
 *
 *  \return  Returns FVID2_SOK
 */

static Int32 Vps_captDeInitTimeStamp( Vps_CaptObj * pObj )
{
    UInt32 streamId, chId;

    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        for ( chId = 0; chId < pObj->numCh; chId++ )
        {
            if (pObj->chObj[streamId][chId].tsUseBestEffort == TRUE)
            {
                /* Un-Register interrupts */
                Vps_captTimeStampCfgEofInt(pObj, FALSE);
            }

            pObj->chObj[streamId][chId].tsUseBestEffort = FALSE;
            pObj->chObj[streamId][chId].tsEofIsrHndl = NULL;
        }
    }
    
    return FVID2_SOK;
}

                
/**
 *  Vps_captInitTimeStamp
 *  \brief This function initializes function to be used to determine time.
 *
 *  \param pObj         Pointer to instance object.
 *  \param enBestEffort Configuration to use, either best effort or default.
 *  \param fncPtr       Custome time stampper, if NULL, standard DSP/BIOS
 *                          Clock_getTicks would be used.
 *
 *  \return  Returns FVID2_SOK
 */

static Int32 Vps_captInitTimeStamp( Vps_CaptObj * pObj, UInt32 enBestEffort, 
                Vps_CaptFrameTimeStamp fncPtr, UInt32 expectFps )
{
    UInt32 streamId, chId;
    Vps_CaptChObj *pChObj;
    Vps_CaptFrameTimeStamp tsFunction;

    tsFunction = fncPtr;
    if (tsFunction == NULL)
    {
        tsFunction = &Vps_captClockGetTicks;
    }

    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        for ( chId = 0; chId < pObj->numCh; chId++ )
        {
            pChObj = &pObj->chObj[streamId][chId];

            pChObj->tsUseBestEffort = enBestEffort;
            pChObj->getCurrTime = tsFunction;
            pChObj->tsExpectTime = 1000u / expectFps;
        }
    }
    
    return FVID2_SOK;
}


/**
 *  Vps_configFrameTimeStamp
 *  \brief This function configures the driver to support required mode for time
 *          stamping of catpured frames.
 *
 *  \param handle       Pointer to instance object.
 *  \param tsPrms       Time stamping configurations prameters.
 *
 *  \return  Returns 0 on success else negative number.
 */
static Int32 Vps_configFrameTimeStamp ( Fdrv_Handle handle,
            Vps_CaptFrameTimeStampParms *tsPrms )
{
    Int32               retVal = FVID2_SOK;
    Vps_CaptObj         *pObj = ( Vps_CaptObj * ) handle;

    /* Return error if any of the required parameters is NULL */
    if ((NULL == pObj) || (NULL == tsPrms))
    {
       Vps_printf( " %s:%d: ERROR: Null Pointer !!!\n",__FUNCTION__, __LINE__);
       retVal = FVID2_EBADARGS;
       return retVal;
    }
    /* IOCTL cannot be called when capture is in progress */
    if (VPS_CAPT_STATE_RUNNING == pObj->state)
    {
        Vps_printf( " %s:%d: ERROR: Streamin On !!!\n",__FUNCTION__, __LINE__);
        retVal = FVID2_EFAIL;
        return retVal;
    }
    
    Vps_captLock ( pObj );

    /* 1. If enabled, 
            a. Check if custom time stamper is required
            b. Enable the interrupts
       2. Else
            b. Disable the interrupts
    */
    if (tsPrms->enableBestEffort == TRUE)
    {
        retVal = Vps_captInitTimeStamp(pObj, TRUE, tsPrms->timeStamper, 
                    tsPrms->expectedFps);
        Vps_captTimeStampCfgEofInt(pObj, TRUE);
    }
    else
    {
        retVal = Vps_captInitTimeStamp(pObj, FALSE, tsPrms->timeStamper, 60);
        Vps_captTimeStampCfgEofInt(pObj, FALSE);
    }

    Vps_captUnlock ( pObj );

    return retVal;
}


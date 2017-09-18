/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/vps/drivers/m2mNsf/src/vpsdrv_m2mNsfPriv.h>

/*
  NSF Common glabal object
*/
Vps_NsfCommonObj gVps_nsfCommonObj;

/*
  Reset NSF HW to known state
*/
Int32 Vps_nsfHwReset (  )
{
    /*
     * Reset HW
     */
    VpsHal_nfReset ( gVps_nsfCommonObj.nsfHalHandle );

    /*
     * Set NF mux to non-bypass mode
     * Noise filter in noise filtering mode is not working on TI816x 1.0.
     */
    if ((VPS_PLATFORM_ID_EVM_TI816x == Vps_platformGetId()) &&
        (VPS_PLATFORM_CPU_REV_1_0 == Vps_platformGetCpuRev()))
    {
        VpsHal_vpsModuleBypass(VPSHAL_VPS_MODULE_NF, VPSHAL_VPS_BYPASS_MODULE);
    }
    else
    {
        VpsHal_vpsModuleBypass(VPSHAL_VPS_MODULE_NF, VPSHAL_VPS_SELECT_MODULE);
    }

    /*
     * Set channel active as start event for all NF channels
     */

    VpsHal_vpdmaSetFrameStartEvent ( VPSHAL_VPDMA_CHANNEL_NF_WR_LUMA, VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE, VPSHAL_VPDMA_LM_0,    /* NOT USED */
                                     0, NULL, 0 );
    VpsHal_vpdmaSetFrameStartEvent ( VPSHAL_VPDMA_CHANNEL_NF_WR_CHROMA, VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE, VPSHAL_VPDMA_LM_0,  /* NOT USED */
                                     0, NULL, 0 );

    VpsHal_vpdmaSetFrameStartEvent ( VPSHAL_VPDMA_CHANNEL_NF_PREV_LUMA, VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE, VPSHAL_VPDMA_LM_0, /* NOT USED */
                                     0, NULL, 0 );
    VpsHal_vpdmaSetFrameStartEvent ( VPSHAL_VPDMA_CHANNEL_NF_PREV_CHROMA, VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE, VPSHAL_VPDMA_LM_0,   /* NOT USED */
                                     0, NULL, 0 );
    VpsHal_vpdmaSetFrameStartEvent ( VPSHAL_VPDMA_CHANNEL_NF_RD, VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE, VPSHAL_VPDMA_LM_0,  /* NOT USED */
                                     0, NULL, 0 );

    return 0;
}

/*
  NSF driver system init, gets called as part of FVID2 init
*/
Int32 Vps_nsfInit (  )
{

    Semaphore_Params semParams;
    Int32 status = FVID2_SOK;

    GT_assert( GT_DEFAULT_MASK, VPS_NSF_MAX_REQ_PER_LIST<=FVID2_MAX_FVID_FRAME_PTR);
    /*
     * Set to 0's for global object, descriptor memory
     */
    memset ( &gVps_nsfCommonObj, 0, sizeof ( gVps_nsfCommonObj ) );

    /*
     * Create global NSF lock
     */
    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    gVps_nsfCommonObj.lock = Semaphore_create ( 1u, &semParams, NULL );

    if ( gVps_nsfCommonObj.lock == NULL )
        status = FVID2_EALLOC;

    gVps_nsfCommonObj.openCnt = 0;


    if ( status == FVID2_SOK )
    {
        /*
         * Register NSF driver with FVID2 sub-system
         */

        gVps_nsfCommonObj.fvidDrvOps.create = ( FVID2_DrvCreate ) Vps_nsfCreate;
        gVps_nsfCommonObj.fvidDrvOps.delete = Vps_nsfDelete;
        gVps_nsfCommonObj.fvidDrvOps.control = Vps_nsfControl;
        gVps_nsfCommonObj.fvidDrvOps.queue = NULL;
        gVps_nsfCommonObj.fvidDrvOps.dequeue = NULL;
        gVps_nsfCommonObj.fvidDrvOps.processFrames = Vps_nsfProcessFrames;
        gVps_nsfCommonObj.fvidDrvOps.getProcessedFrames =
            Vps_nsfGetProcessedFrames;
        gVps_nsfCommonObj.fvidDrvOps.drvId = FVID2_VPS_M2M_NSF_DRV;

        status = FVID2_registerDriver ( &gVps_nsfCommonObj.fvidDrvOps );

        if ( status != FVID2_SOK )
        {
            /*
             * Error - free acquired resources
             */
            Semaphore_delete ( &gVps_nsfCommonObj.lock );
        }
    }

    /*
     * Open HAL handle for NSF HW
     */
    gVps_nsfCommonObj.nsfHalHandle = VpsHal_nfOpen ( VPSHAL_NF_INST_0 );

    GT_assert( GT_DEFAULT_MASK,  gVps_nsfCommonObj.nsfHalHandle != NULL );

    /*
     * Reset NSF HW to known state
     */
    Vps_nsfHwReset (  );

    if ( status != FVID2_SOK )
    {
        Vps_printf ( " ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__,
                        __LINE__ );
    }

    return status;
}

/*
  NSF sub-system de-init, gets called during FVID2_deInit()
*/
Int32 Vps_nsfDeInit (  )
{
    /*
     * Unregister FVID2 driver
     */
    FVID2_unRegisterDriver ( &gVps_nsfCommonObj.fvidDrvOps );

    /*
     * Delete semaphore's
     */
    Semaphore_delete ( &gVps_nsfCommonObj.lock );

    /*
     * Close HAL handle
     */
    VpsHal_nfClose ( gVps_nsfCommonObj.nsfHalHandle );

    return 0;
}

/*
  Global NSF lock
*/
Int32 Vps_nsfLock (  )
{
    Semaphore_pend ( gVps_nsfCommonObj.lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/*
  Global NSF unlock
*/
Int32 Vps_nsfUnlock (  )
{
    Semaphore_post ( gVps_nsfCommonObj.lock );

    return FVID2_SOK;
}

/*
  Alloc NSF drier object instance data structure
*/
Vps_NsfObj *Vps_nsfAllocObj (  )
{
    UInt32 handleId;
    Vps_NsfObj *pObj;
    Semaphore_Params semParams;
    UInt32 found = FALSE;

    /*
     * Take global lock to avoid race condition
     */
    Vps_nsfLock (  );

    /*
     * find a unallocated object in pool
     */
    for ( handleId = 0; handleId < VPS_NSF_MAX_HANDLES; handleId++ )
    {

        pObj = &gVps_nsfCommonObj.nsfObj[handleId];

        if ( pObj->state == VPS_NSF_OBJ_STATE_UNUSED )
        {
            /*
             * free object found
             */

            /*
             * init to 0's
             */
            memset ( pObj, 0, sizeof ( *pObj ) );

            /*
             * init state and handle ID
             */
            pObj->state = VPS_NSF_OBJ_STATE_IDLE;
            pObj->handleId = handleId;

            /*
             * create driver object specific semaphore lock
             */
            Semaphore_Params_init ( &semParams );

            semParams.mode = Semaphore_Mode_BINARY;

            pObj->lock = Semaphore_create ( 1u, &semParams, NULL );

            /*
             * init object parameters to default
             */
            pObj->blockOnChQueFull = FALSE;
            pObj->blockOnReqQueFull = TRUE;

            found = TRUE;

            if ( pObj->lock == NULL )
            {
                /*
                 * Error - release object
                 */
                found = FALSE;
                pObj->state = VPS_NSF_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /*
     * Release global lock
     */
    Vps_nsfUnlock (  );

    if ( TRUE == found )
        return pObj;    /* Free object found return it */

    /*
     * free object not found, return NULL
     */
    return NULL;
}

/*
  Release previously allocated object
*/
Int32 Vps_nsfFreeObj ( Vps_NsfObj * pObj )
{
    /*
     * take global lock
     */
    Vps_nsfLock (  );

    if ( pObj->state != VPS_NSF_OBJ_STATE_UNUSED )
    {
        /*
         * mark state as unsued
         */
        pObj->state = VPS_NSF_OBJ_STATE_UNUSED;

        /*
         * delete object locking semaphore
         */
        Semaphore_delete ( &pObj->lock );
    }

    /*
     * release global lock
     */
    Vps_nsfUnlock (  );

    return FVID2_SOK;
}

/*
  Allocate a free HW frame noise index

  autoMode - TRUE: frame noise is updated automatically
             FALSE: frame noise is updated statically

  All static-update frame noise index's can be shared across channels
  All auto-update frame noise index's need to be unique for a channel
*/
Int16 Vps_nsfHwFrameIdxAlloc ( UInt32 autoMode )
{
    Int16 tmpFrameNoiseIdx;
    UInt32 isFound = FALSE;

    /*
     * take global lock
     */
    Vps_nsfLock (  );

    /*
     * search for a free index
     */
    for ( tmpFrameNoiseIdx = 0;
          tmpFrameNoiseIdx < VPS_NSF_MAX_FRAME_NOISE_IDX; tmpFrameNoiseIdx++ )
    {
        if ( TRUE == autoMode )
        {
            /*
             * auto mode uses exclusive allocation
             */
            if ( gVps_nsfCommonObj.
                 hwFrameNoiseIdxAllocStatus[tmpFrameNoiseIdx] == 0 )
            {
                /*
                 * not previously allocated
                 */

                /*
                 * mark as allocated in auto mode - 0xFFFF
                 */
                gVps_nsfCommonObj.hwFrameNoiseIdxAllocStatus[tmpFrameNoiseIdx] =
                    0xFFFF;
                isFound = TRUE;
                break;
            }
        }
        else
        {
            /*
             * static mode uses shared allocation
             */

            if ( gVps_nsfCommonObj.hwFrameNoiseIdxAllocStatus[tmpFrameNoiseIdx]
                 < 0xFFFF )
            {
                /*
                 * when < 0xFFFF, not allocated in auto mode
                 * either it not allocated at all, i.e = 0 or
                 * allocated in shared mode, i.e > 0 and < 0xFFF
                 */

                /*
                 * increment user count for this index
                 */
                gVps_nsfCommonObj.
                    hwFrameNoiseIdxAllocStatus[tmpFrameNoiseIdx]++;
                isFound = TRUE;
                break;
            }
        }
    }

    if ( FALSE == isFound )
        tmpFrameNoiseIdx = -1;  /* not found, return -1 */

    /*
     * release global lock
     */
    Vps_nsfUnlock (  );

    /*
     * return frame noise index
     */
    return tmpFrameNoiseIdx;
}

/*
  Free prevously allocated frame noise index
*/
Void Vps_nsfHwFrameIdxFree ( Int16 hwFrameIdx )
{
    /*
     * parameter check
     */
    if ( hwFrameIdx < 0 || hwFrameIdx >= VPS_NSF_MAX_FRAME_NOISE_IDX )
        return;

    /*
     * take global lock
     */
    Vps_nsfLock (  );

    if ( gVps_nsfCommonObj.hwFrameNoiseIdxAllocStatus[hwFrameIdx] )
    {
        /*
         * frame noise index is allocated
         */

        if ( gVps_nsfCommonObj.hwFrameNoiseIdxAllocStatus[hwFrameIdx] ==
             0xFFFF )
        {
            /*
             * exclusive mode allocation, free it
             */
            gVps_nsfCommonObj.hwFrameNoiseIdxAllocStatus[hwFrameIdx] = 0;
        }
        else
        {
            /*
             * shared allocation, decrement user count
             * index gets free-ed only when count reaches zero
             */
            gVps_nsfCommonObj.hwFrameNoiseIdxAllocStatus[hwFrameIdx]--;
        }
    }

    /*
     * release global lock
     */
    Vps_nsfUnlock (  );
}

/* Allocate CH descriptor set */
Ptr Vps_nsfChDescSetAlloc(void)
{
    Ptr memPtr = NULL;

    memPtr = VpsUtils_allocDescMem(
                 (VPS_NSF_MAX_DESC_SET_PER_CH * VPS_NSF_MAX_DESC_SET_SIZE),
                 (VPSHAL_VPDMA_DESC_BYTE_ALIGN * 2u));
    if (memPtr != NULL)
    {
#ifdef VPS_CLEAR_ALL_MEM
        memset(
            memPtr,
            0,
            (VPS_NSF_MAX_DESC_SET_PER_CH * VPS_NSF_MAX_DESC_SET_SIZE));
#endif
    }

    return (memPtr);
}

/* Free previously allocated CH descriptor set  */
Void Vps_nsfChDescSetFree(Ptr memPtr)
{
    if (memPtr != NULL)
    {
        VpsUtils_freeDescMem(
            memPtr,
            (VPS_NSF_MAX_DESC_SET_PER_CH * VPS_NSF_MAX_DESC_SET_SIZE));
    }
}

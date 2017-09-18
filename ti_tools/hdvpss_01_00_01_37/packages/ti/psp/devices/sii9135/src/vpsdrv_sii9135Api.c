/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/devices/sii9135/src/vpsdrv_sii9135Priv.h>

/* Global object storing all information related to all
  SII9135 driver handles */
Vps_Sii9135CommonObj gVps_sii9135CommonObj;

/* Control API that gets called when FVID2_control is called

  This API does handle level semaphore locking

  handle - SII9135 driver handle
  cmd - command
  cmdArgs - command arguments
  cmdStatusArgs - command status

  returns error in case of
  - illegal parameters
  - I2C command RX/TX error
*/
Int32 Vps_sii9135Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
    Vps_Sii9135Obj *pObj = ( Vps_Sii9135Obj * ) handle;
    Int32 status;

    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /*
     * lock handle
     */
    Vps_sii9135LockObj ( pObj );

    switch ( cmd )
    {
        case FVID2_START:
            status = Vps_sii9135Start ( pObj );
            break;

        case FVID2_STOP:
            status = Vps_sii9135Stop ( pObj );
            break;

        case IOCTL_VPS_VIDEO_DECODER_GET_CHIP_ID:
            status = Vps_sii9135GetChipId ( pObj, cmdArgs, cmdStatusArgs );
            break;

        case IOCTL_VPS_VIDEO_DECODER_RESET:
            status = Vps_sii9135Reset ( pObj );
            break;

        case IOCTL_VPS_VIDEO_DECODER_SET_VIDEO_MODE:
            status = Vps_sii9135SetVideoMode ( pObj, cmdArgs );
            break;

        case IOCTL_VPS_VIDEO_DECODER_GET_VIDEO_STATUS:
            status = Vps_sii9135GetVideoStatus ( pObj, cmdArgs, cmdStatusArgs );
            break;

        case IOCTL_VPS_VIDEO_DECODER_REG_WRITE:
            status = Vps_sii9135RegWrite ( pObj, cmdArgs );
            break;

        case IOCTL_VPS_VIDEO_DECODER_REG_READ:
            status = Vps_sii9135RegRead ( pObj, cmdArgs );
            break;

        default:
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    /*
     * unlock handle
     */
    Vps_sii9135UnlockObj ( pObj );

    return status;
}

/*
  Create API that gets called when FVID2_create is called

  This API does not configure the SII9135 is any way.

  This API
  - validates parameters
  - allocates driver handle
  - stores create arguments in its internal data structure.

  Later the create arguments will be used when doing I2C communcation with
  SII9135

  drvId - driver ID, must be FVID2_VPS_VID_DEC_SII9135_DRV
  instanceId - must be 0
  createArgs - create arguments
  createStatusArgs - create status
  fdmCbParams  - NOT USED, set to NULL

  returns NULL in case of any error
*/
Fdrv_Handle Vps_sii9135Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams )
{
    Vps_Sii9135Obj *pObj;
    Vps_VideoDecoderCreateParams *vidDecCreateArgs
        = ( Vps_VideoDecoderCreateParams * ) createArgs;

    Vps_VideoDecoderCreateStatus *vidDecCreateStatus
        = ( Vps_VideoDecoderCreateStatus * ) createStatusArgs;

    /*
     * check parameters
     */
    if ( vidDecCreateStatus == NULL )
        return NULL;

    vidDecCreateStatus->retVal = FVID2_SOK;

    if ( drvId != FVID2_VPS_VID_DEC_SII9135_DRV
         || instanceId != 0 || vidDecCreateArgs == NULL )
    {
        vidDecCreateStatus->retVal = FVID2_EBADARGS;
        return NULL;
    }

    if ( vidDecCreateArgs->deviceI2cInstId >= VPS_DEVICE_I2C_INST_ID_MAX )
    {
        vidDecCreateStatus->retVal = FVID2_EINVALID_PARAMS;
        return NULL;
    }

    /*
     * allocate driver handle
     */
    pObj = Vps_sii9135AllocObj (  );
    if ( pObj == NULL )
    {
        vidDecCreateStatus->retVal = FVID2_EALLOC;
        return NULL;
    }

    /*
     * copy parameters to allocate driver handle
     */
    memcpy ( &pObj->createArgs, vidDecCreateArgs,
             sizeof ( *vidDecCreateArgs ) );

    Vps_sii9135ResetRegCache(pObj);

    /*
     * return driver handle object pointer
     */
    return pObj;
}

/*
  Delete function that is called when FVID2_delete is called

  This API
  - free's driver handle object

  handle - driver handle
  deleteArgs - NOT USED, set to NULL

*/
Int32 Vps_sii9135Delete ( Fdrv_Handle handle, Ptr deleteArgs )
{
    Vps_Sii9135Obj *pObj = ( Vps_Sii9135Obj * ) handle;

    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /*
     * free driver handle object
     */
    Vps_sii9135FreeObj ( pObj );

    return FVID2_SOK;
}

/*
  System init for SII9135 driver

  This API
  - create semaphore locks needed
  - registers driver to FVID2 sub-system
  - gets called as part of Vps_deviceInit()
*/
Int32 Vps_sii9135Init (  )
{
    Semaphore_Params semParams;
    Int32 status = FVID2_SOK;

    /*
     * Set to 0's for global object, descriptor memory
     */
    memset ( &gVps_sii9135CommonObj, 0, sizeof ( gVps_sii9135CommonObj ) );

    /*
     * Create global SII9135 lock
     */
    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    gVps_sii9135CommonObj.lock = Semaphore_create ( 1u, &semParams, NULL );

    if ( gVps_sii9135CommonObj.lock == NULL )
        status = FVID2_EALLOC;

    if ( status == FVID2_SOK )
    {
        /*
         * Register SII9135 driver with FVID2 sub-system
         */

        gVps_sii9135CommonObj.fvidDrvOps.create =
            ( FVID2_DrvCreate ) Vps_sii9135Create;
        gVps_sii9135CommonObj.fvidDrvOps.delete = Vps_sii9135Delete;
        gVps_sii9135CommonObj.fvidDrvOps.control = Vps_sii9135Control;
        gVps_sii9135CommonObj.fvidDrvOps.queue = NULL;
        gVps_sii9135CommonObj.fvidDrvOps.dequeue = NULL;
        gVps_sii9135CommonObj.fvidDrvOps.processFrames = NULL;
        gVps_sii9135CommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gVps_sii9135CommonObj.fvidDrvOps.drvId = FVID2_VPS_VID_DEC_SII9135_DRV;

        status = FVID2_registerDriver ( &gVps_sii9135CommonObj.fvidDrvOps );

        if ( status != FVID2_SOK )
        {
            /*
             * Error - free acquired resources
             */
            Semaphore_delete ( &gVps_sii9135CommonObj.lock );
        }
    }

    if ( status != FVID2_SOK )
    {
        Vps_printf ( " ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__,
                        __LINE__ );
    }

    return status;
}

/*
  System de-init for SII9135 driver

  This API
  - de-registers driver from FVID2 sub-system
  - delete's allocated semaphore locks
  - gets called as part of Vps_deviceDeInit()
*/
Int32 Vps_sii9135DeInit (  )
{
    /*
     * Unregister FVID2 driver
     */
    FVID2_unRegisterDriver ( &gVps_sii9135CommonObj.fvidDrvOps );

    /*
     * Delete semaphore's
     */
    Semaphore_delete ( &gVps_sii9135CommonObj.lock );

    return 0;
}

/*
  Handle level lock
*/
Int32 Vps_sii9135LockObj ( Vps_Sii9135Obj * pObj )
{
    Semaphore_pend ( pObj->lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/*
  Handle level unlock
*/
Int32 Vps_sii9135UnlockObj ( Vps_Sii9135Obj * pObj )
{
    Semaphore_post ( pObj->lock );

    return FVID2_SOK;
}

/*
  Global driver level lock
*/
Int32 Vps_sii9135Lock (  )
{
    Semaphore_pend ( gVps_sii9135CommonObj.lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/*
  Global driver level unlock
*/
Int32 Vps_sii9135Unlock (  )
{
    Semaphore_post ( gVps_sii9135CommonObj.lock );

    return FVID2_SOK;
}

/*
  Allocate driver object

  Searches in list of driver handles and allocate's a 'NOT IN USE' handle
  Also create's handle level semaphore lock

  return NULL in case handle could not be allocated
*/
Vps_Sii9135Obj *Vps_sii9135AllocObj (  )
{
    UInt32 handleId;
    Vps_Sii9135Obj *pObj;
    Semaphore_Params semParams;
    UInt32 found = FALSE;

    /*
     * Take global lock to avoid race condition
     */
    Vps_sii9135Lock (  );

    /*
     * find a unallocated object in pool
     */
    for ( handleId = 0; handleId < VPS_DEVICE_MAX_HANDLES; handleId++ )
    {

        pObj = &gVps_sii9135CommonObj.sii9135Obj[handleId];

        if ( pObj->state == VPS_SII9135_OBJ_STATE_UNUSED )
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
            pObj->state = VPS_SII9135_OBJ_STATE_IDLE;
            pObj->handleId = handleId;

            /*
             * create driver object specific semaphore lock
             */
            Semaphore_Params_init ( &semParams );

            semParams.mode = Semaphore_Mode_BINARY;

            pObj->lock = Semaphore_create ( 1u, &semParams, NULL );

            found = TRUE;

            if ( pObj->lock == NULL )
            {
                /*
                 * Error - release object
                 */
                found = FALSE;
                pObj->state = VPS_SII9135_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /*
     * Release global lock
     */
    Vps_sii9135Unlock (  );

    if ( found )
        return pObj;    /* Free object found return it */

    /*
     * free object not found, return NULL
     */
    return NULL;
}

/*
  De-Allocate driver object

  Marks handle as 'NOT IN USE'
  Also delete's handle level semaphore lock
*/
Int32 Vps_sii9135FreeObj ( Vps_Sii9135Obj * pObj )
{
    /*
     * take global lock
     */
    Vps_sii9135Lock (  );

    if ( pObj->state != VPS_SII9135_OBJ_STATE_UNUSED )
    {
        /*
         * mark state as unused
         */
        pObj->state = VPS_SII9135_OBJ_STATE_UNUSED;

        /*
         * delete object locking semaphore
         */
        Semaphore_delete ( &pObj->lock );
    }

    /*
     * release global lock
     */
    Vps_sii9135Unlock (  );

    return FVID2_SOK;
}

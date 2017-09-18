/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/devices/tvp5158/src/vpsdrv_tvp5158Priv.h>

/* Global object storing all information related to all
  TVP5158 driver handles */
Vps_Tvp5158CommonObj gVps_tvp5158CommonObj;

/* Control API that gets called when FVID2_control is called

  This API does handle level semaphore locking

  handle - TVP5158 driver handle
  cmd - command
  cmdArgs - command arguments
  cmdStatusArgs - command status

  returns error in case of
  - illegal parameters
  - I2C command RX/TX error
*/
Int32 Vps_tvp5158Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
    Vps_Tvp5158Obj *pObj = ( Vps_Tvp5158Obj * ) handle;
    Int32 status;

    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /*
     * lock handle
     */
    Vps_tvp5158LockObj ( pObj );

    switch ( cmd )
    {
        case FVID2_START:
            status = Vps_tvp5158Start ( pObj );
            break;

        case FVID2_STOP:
            status = Vps_tvp5158Stop ( pObj );
            break;

        case IOCTL_VPS_VIDEO_DECODER_GET_CHIP_ID:
            status = Vps_tvp5158GetChipId ( pObj, cmdArgs, cmdStatusArgs );
            break;

        case IOCTL_VPS_VIDEO_DECODER_RESET:
            status = Vps_tvp5158Reset ( pObj );
            break;

        case IOCTL_VPS_VIDEO_DECODER_SET_VIDEO_MODE:
            status = Vps_tvp5158SetVideoMode ( pObj, cmdArgs );
            break;

        case IOCTL_VPS_VIDEO_DECODER_GET_VIDEO_STATUS:
            status = Vps_tvp5158GetVideoStatus ( pObj, cmdArgs, cmdStatusArgs );
            break;

        case IOCTL_VPS_VIDEO_DECODER_SET_VIDEO_COLOR:
            status = Vps_tvp5158SetVideoColor ( pObj, cmdArgs );
            break;

        case IOCTL_VPS_TVP5158_SET_VIDEO_NF:
            status = Vps_tvp5158SetVideoNf ( pObj, cmdArgs );
            break;

        case IOCTL_VPS_VIDEO_DECODER_REG_WRITE:
            status = Vps_tvp5158RegWrite ( pObj, cmdArgs );
            break;

        case IOCTL_VPS_VIDEO_DECODER_REG_READ:
            status = Vps_tvp5158RegRead ( pObj, cmdArgs );
            break;

        case IOCTL_VPS_TVP5158_SET_AUDIO_MODE:
            status = Vps_tvp5158SetAudioMode ( pObj, cmdArgs );
            break;

        default:
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    /*
     * unlock handle
     */
    Vps_tvp5158UnlockObj ( pObj );

    return status;
}

/*
  Create API that gets called when FVID2_create is called

  This API does not configure the TVP5158 is any way.

  This API
  - validates parameters
  - allocates driver handle
  - stores create arguments in its internal data structure.

  Later the create arguments will be used when doing I2C communcation with
  TVP5158

  drvId - driver ID, must be FVID2_VPS_VID_DEC_TVP5158_DRV
  instanceId - must be 0
  createArgs - create arguments
  createStatusArgs - create status
  fdmCbParams  - NOT USED, set to NULL

  returns NULL in case of any error
*/
Fdrv_Handle Vps_tvp5158Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams )
{
    Vps_Tvp5158Obj *pObj;
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

    if ( drvId != FVID2_VPS_VID_DEC_TVP5158_DRV
         || instanceId != 0 || vidDecCreateArgs == NULL )
    {
        vidDecCreateStatus->retVal = FVID2_EBADARGS;
        return NULL;
    }

    if ( vidDecCreateArgs->deviceI2cInstId >= VPS_DEVICE_I2C_INST_ID_MAX
         || vidDecCreateArgs->numDevicesAtPort > VPS_TVP5158_DEV_PER_PORT_MAX )
    {
        vidDecCreateStatus->retVal = FVID2_EINVALID_PARAMS;
        return NULL;
    }

    /*
     * allocate driver handle
     */
    pObj = Vps_tvp5158AllocObj (  );
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
Int32 Vps_tvp5158Delete ( Fdrv_Handle handle, Ptr deleteArgs )
{
    Vps_Tvp5158Obj *pObj = ( Vps_Tvp5158Obj * ) handle;

    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /*
     * free driver handle object
     */
    Vps_tvp5158FreeObj ( pObj );

    return FVID2_SOK;
}

/*
  System init for TVP5158 driver

  This API
  - create semaphore locks needed
  - registers driver to FVID2 sub-system
  - gets called as part of Vps_deviceInit()

*/
Int32 Vps_tvp5158Init (  )
{
    Semaphore_Params semParams;
    Int32 status = FVID2_SOK;

    /*
     * Set to 0's for global object, descriptor memory
     */
    memset ( &gVps_tvp5158CommonObj, 0, sizeof ( gVps_tvp5158CommonObj ) );

    /*
     * Create global TVP5158 lock
     */
    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    gVps_tvp5158CommonObj.lock = Semaphore_create ( 1u, &semParams, NULL );

    if ( gVps_tvp5158CommonObj.lock == NULL )
        status = FVID2_EALLOC;

    if ( status == FVID2_SOK )
    {
        /*
         * Register TVP5158 driver with FVID2 sub-system
         */

        gVps_tvp5158CommonObj.fvidDrvOps.create
            = ( FVID2_DrvCreate ) Vps_tvp5158Create;
        gVps_tvp5158CommonObj.fvidDrvOps.delete = Vps_tvp5158Delete;
        gVps_tvp5158CommonObj.fvidDrvOps.control = Vps_tvp5158Control;
        gVps_tvp5158CommonObj.fvidDrvOps.queue = NULL;
        gVps_tvp5158CommonObj.fvidDrvOps.dequeue = NULL;
        gVps_tvp5158CommonObj.fvidDrvOps.processFrames = NULL;
        gVps_tvp5158CommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gVps_tvp5158CommonObj.fvidDrvOps.drvId = FVID2_VPS_VID_DEC_TVP5158_DRV;

        status = FVID2_registerDriver ( &gVps_tvp5158CommonObj.fvidDrvOps );

        if ( status != FVID2_SOK )
        {
            /*
             * Error - free acquired resources
             */
            Semaphore_delete ( &gVps_tvp5158CommonObj.lock );
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
  System de-init for TVP5158 driver

  This API
  - de-registers driver from FVID2 sub-system
  - delete's allocated semaphore locks
  - gets called as part of Vps_deviceDeInit()
*/
Int32 Vps_tvp5158DeInit (  )
{
    /*
     * Unregister FVID2 driver
     */
    FVID2_unRegisterDriver ( &gVps_tvp5158CommonObj.fvidDrvOps );

    /*
     * Delete semaphore's
     */
    Semaphore_delete ( &gVps_tvp5158CommonObj.lock );

    return 0;
}

/*
  Handle level lock
*/
Int32 Vps_tvp5158LockObj ( Vps_Tvp5158Obj * pObj )
{
    Semaphore_pend ( pObj->lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/*
  Handle level unlock
*/
Int32 Vps_tvp5158UnlockObj ( Vps_Tvp5158Obj * pObj )
{
    Semaphore_post ( pObj->lock );

    return FVID2_SOK;
}

/*
  Global driver level lock
*/
Int32 Vps_tvp5158Lock (  )
{
    Semaphore_pend ( gVps_tvp5158CommonObj.lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/*
  Global driver level unlock
*/
Int32 Vps_tvp5158Unlock (  )
{
    Semaphore_post ( gVps_tvp5158CommonObj.lock );

    return FVID2_SOK;
}

/*
  Allocate driver object

  Searches in list of driver handles and allocate's a 'NOT IN USE' handle
  Also create's handle level semaphore lock

  return NULL in case handle could not be allocated
*/
Vps_Tvp5158Obj *Vps_tvp5158AllocObj (  )
{
    UInt32 handleId;
    Vps_Tvp5158Obj *pObj;
    Semaphore_Params semParams;
    UInt32 found = FALSE;

    /*
     * Take global lock to avoid race condition
     */
    Vps_tvp5158Lock (  );

    /*
     * find a unallocated object in pool
     */
    for ( handleId = 0; handleId < VPS_DEVICE_MAX_HANDLES; handleId++ )
    {

        pObj = &gVps_tvp5158CommonObj.tvp5158Obj[handleId];

        if ( pObj->state == VPS_TVP5158_OBJ_STATE_UNUSED )
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
            pObj->state = VPS_TVP5158_OBJ_STATE_IDLE;
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
                pObj->state = VPS_TVP5158_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /*
     * Release global lock
     */
    Vps_tvp5158Unlock (  );

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
Int32 Vps_tvp5158FreeObj ( Vps_Tvp5158Obj * pObj )
{
    /*
     * take global lock
     */
    Vps_tvp5158Lock (  );

    if ( pObj->state != VPS_TVP5158_OBJ_STATE_UNUSED )
    {
        /*
         * mark state as unused
         */
        pObj->state = VPS_TVP5158_OBJ_STATE_UNUSED;

        /*
         * delete object locking semaphore
         */
        Semaphore_delete ( &pObj->lock );
    }

    /*
     * release global lock
     */
    Vps_tvp5158Unlock (  );

    return FVID2_SOK;
}

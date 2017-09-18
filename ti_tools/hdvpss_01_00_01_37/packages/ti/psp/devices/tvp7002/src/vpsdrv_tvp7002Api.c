/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_tvp7002Api.c
 *
 *  \brief TVP7002 video encoder FVID2 driver API file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <ti/psp/devices/vps_videoDecoder.h>
#include <ti/psp/devices/tvp7002/vpsdrv_tvp7002.h>
#include <ti/psp/devices/tvp7002/src/vpsdrv_tvp7002Priv.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Fdrv_Handle Vps_tvp7002Create(UInt32 drvId,
                                     UInt32 instId,
                                     Ptr createArgs,
                                     Ptr createStatusArgs,
                                     const FVID2_DrvCbParams *fdmCbPrms);
static Int32 Vps_tvp7002Delete(Fdrv_Handle handle, Ptr deleteArgs);
static Int32 Vps_tvp7002Control(Fdrv_Handle handle,
                                UInt32 cmd,
                                Ptr cmdArgs,
                                Ptr cmdStatusArgs);

static Vps_Tvp7002HandleObj *Vps_tvp7002AllocObj(void);
static Int32 Vps_tvp7002FreeObj(Vps_Tvp7002HandleObj *pObj);

static Int32 Vps_tvp7002LockObj(Vps_Tvp7002HandleObj *pObj);
static Int32 Vps_tvp7002UnlockObj(Vps_Tvp7002HandleObj *pObj);
static Int32 Vps_tvp7002Lock(void);
static Int32 Vps_tvp7002Unlock(void);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief TVP7002 driver function pointer. */
static const FVID2_DrvOps gVpsTvp7002DrvOps =
{
    FVID2_VPS_VID_DEC_TVP7002_DRV,  /* Driver ID */
    Vps_tvp7002Create,              /* Create */
    Vps_tvp7002Delete,              /* Delete */
    Vps_tvp7002Control,             /* Control */
    NULL,                           /* Queue */
    NULL,                           /* Dequeue */
    NULL,                           /* ProcessFrames */
    NULL                            /* GetProcessedFrames */
};

/**
 *  \brief Global object storing all information related to all TVP7002 driver
 *  handles.
 */
Vps_Tvp7002Obj gVpsTvp7002Obj;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  \brief System init for TVP7002 driver
 *
 *  This API
 *      - create semaphore locks needed
 *      - registers driver to FVID2 sub-system
 *      - gets called as part of Vps_deviceInit()
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.
 */
Int32 Vps_tvp7002Init(void)
{
    Int32               retVal = FVID2_SOK;
    Semaphore_Params    semPrms;

    /* Memset global object */
    VpsUtils_memset(&gVpsTvp7002Obj, 0, sizeof (gVpsTvp7002Obj));

    /* Create global TVP7002 lock */
    Semaphore_Params_init(&semPrms);
    semPrms.mode = Semaphore_Mode_BINARY;
    gVpsTvp7002Obj.lock = Semaphore_create(1u, &semPrms, NULL);
    if (NULL == gVpsTvp7002Obj.lock)
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Global semaphore create failed\n");
        retVal = FVID2_EALLOC;
    }

    if (FVID2_SOK == retVal)
    {
        /* Register TVP7002 driver with FVID2 sub-system */
        retVal = FVID2_registerDriver(&gVpsTvp7002DrvOps);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDeviceTrace, GT_ERR,
                "Registering to FVID2 driver manager failed\n");
            /* Error - free acquired resources */
            Semaphore_delete(&gVpsTvp7002Obj.lock);
        }
    }

    return (retVal);
}



/**
 *  \brief System de-init for TVP7002 driver
 *
 *  This API
 *      - de-registers driver from FVID2 sub-system
 *      - delete's allocated semaphore locks
 *      - gets called as part of Vps_deviceDeInit()
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.
 */
Int32 Vps_tvp7002DeInit(void)
{
    /* Unregister FVID2 driver */
    FVID2_unRegisterDriver(&gVpsTvp7002DrvOps);

    /* Delete semaphore's. */
    Semaphore_delete(&gVpsTvp7002Obj.lock);

    return (FVID2_SOK);
}



/**
 *  \brief TVP7002 create API that gets called when FVID2_create is called.
 *
 *  This API does not configure the TVP7002 is any way.
 *
 *  This API
 *      - validates parameters
 *      - allocates driver handle
 *      - stores create arguments in its internal data structure.
 *
 *  Later the create arguments will be used when doing I2C communcation with
 *  TVP7002.
 *
 *  \param drvId            [IN] Driver ID, must be
 *                          FVID2_VPS_VID_DEC_TVP7002_DRV.
 *  \param instId           [IN] Must be 0.
 *  \param createArgs       [IN] Create arguments - pointer to valid
 *                          Vps_VideoDecoderCreateParams. This parameter should
 *                          be non-NULL.
 *  \param createStatusArgs [OUT] TVP7002 driver return parameter -
 *                          pointer to Vps_VideoDecoderCreateStatus.
 *                          This parameter could be NULL and the driver fills
 *                          the retVal information only if this is not NULL.
 *  \param fdmCbPrms        [IN] Not used. Set to NULL
 *
 *  \return                 Returns NULL in case of any error. Otherwise
 *                          returns a valid handle.
 */
static Fdrv_Handle Vps_tvp7002Create(UInt32 drvId,
                                     UInt32 instId,
                                     Ptr createArgs,
                                     Ptr createStatusArgs,
                                     const FVID2_DrvCbParams *fdmCbPrms)
{
    Int32                           retVal = FVID2_SOK;
    Vps_Tvp7002HandleObj           *pObj = NULL;
    Vps_VideoDecoderCreateParams   *vidDecCreateArgs;
    Vps_VideoDecoderCreateStatus   *vidDecCreateStatus;

    /* Check parameters */
    if ((NULL == createArgs) ||
        (drvId != FVID2_VPS_VID_DEC_TVP7002_DRV) ||
        (instId != 0u))
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Null pointer/Invalid parameters\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        vidDecCreateArgs = (Vps_VideoDecoderCreateParams *) createArgs;
        if (vidDecCreateArgs->deviceI2cInstId >= VPS_DEVICE_I2C_INST_ID_MAX)
        {
            GT_0trace(VpsDeviceTrace, GT_ERR, "Invalid I2C instance ID\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate driver handle */
        pObj = Vps_tvp7002AllocObj();
        if (NULL == pObj)
        {
            GT_0trace(VpsDeviceTrace, GT_ERR, "Alloc object failed\n");
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Copy parameters to allocate driver handle */
        VpsUtils_memcpy(
            &pObj->createArgs,
            vidDecCreateArgs,
            sizeof (*vidDecCreateArgs));
    }

    /* Fill the retVal if possible */
    if (NULL != createStatusArgs)
    {
        vidDecCreateStatus = (Vps_VideoDecoderCreateStatus *) createStatusArgs;
        vidDecCreateStatus->retVal = retVal;
    }

    return (pObj);
}



/**
 *  \brief Delete function that is called when FVID2_delete is called.
 *
 *  This API
 *      - free's driver handle object
 *
 *  \param handle           [IN] Driver handle.
 *  \param deleteArgs       [IN] Not used currently. Meant for future purpose.
 *                          Set this to NULL.
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code. *
 */
static Int32 Vps_tvp7002Delete(Fdrv_Handle handle, Ptr deleteArgs)
{
    Int32                   retVal = FVID2_SOK;
    Vps_Tvp7002HandleObj   *pObj;

    /* Check parameters */
    if (NULL == handle)
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Null pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        /* Free driver handle object */
        pObj = (Vps_Tvp7002HandleObj *) handle;
        retVal = Vps_tvp7002FreeObj(pObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDeviceTrace, GT_ERR, "Free object failed\n");
        }
    }

    return (retVal);
}



/**
 *  \brief Control API that gets called when FVID2_control is called.
 *
 *  This API does handle level semaphore locking
 *
 *  \param handle           [IN] TVP7002 driver handle returned by
 *                          create function.
 *  \param cmd              [IN] Supports the standard video decoder interface
 *                          commands.
 *  \param cmdArgs          [IN] Depending on the command this will vary.
 *  \param cmdStatusArgs    [OUT] Depending on the command this will vary.
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error codes for illegal parameters and
 *                          I2C command RX/TX error.
 */
static Int32 Vps_tvp7002Control(Fdrv_Handle handle,
                                UInt32 cmd,
                                Ptr cmdArgs,
                                Ptr cmdStatusArgs)
{
    Int32                   retVal = FVID2_SOK;
    Vps_Tvp7002HandleObj   *pObj = NULL;

    /* Check parameters */
    if (NULL == handle)
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Null pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        pObj = (Vps_Tvp7002HandleObj *) handle;

        /* lock handle */
        Vps_tvp7002LockObj(pObj);

        switch (cmd)
        {
            case IOCTL_VPS_VIDEO_DECODER_RESET:
                retVal = Vps_tvp7002ResetIoctl(pObj);
                break;

            case IOCTL_VPS_VIDEO_DECODER_GET_CHIP_ID:
                retVal = Vps_tvp7002GetChipIdIoctl(
                             pObj,
                             cmdArgs,
                             cmdStatusArgs);
                break;

            case IOCTL_VPS_VIDEO_DECODER_SET_VIDEO_MODE:
                retVal = Vps_tvp7002SetVideoModeIoctl(pObj, cmdArgs);
                break;

            case IOCTL_VPS_VIDEO_DECODER_GET_VIDEO_STATUS:
                retVal = Vps_tvp7002GetVideoStatusIoctl(
                             pObj,
                             cmdArgs,
                             cmdStatusArgs);
                break;

            case FVID2_START:
                retVal = Vps_tvp7002StartIoctl(pObj);
                break;

            case FVID2_STOP:
                retVal = Vps_tvp7002StopIoctl(pObj);
                break;

            case IOCTL_VPS_VIDEO_DECODER_REG_WRITE:
                retVal = Vps_tvp7002RegWriteIoctl(pObj, cmdArgs);
                break;

            case IOCTL_VPS_VIDEO_DECODER_REG_READ:
                retVal = Vps_tvp7002RegReadIoctl(pObj, cmdArgs);
                break;

            default:
                GT_0trace(VpsDeviceTrace, GT_ERR, "Unsupported command\n");
                retVal = FVID2_EUNSUPPORTED_CMD;
                break;
        }
    }

    if (NULL != pObj)
    {
        /* Unlock handle */
        Vps_tvp7002UnlockObj(pObj);
    }

    return (retVal);
}



/**
 *  \brief Allocates driver object.
 *
 *  Searches in list of driver handles and allocate's a 'NOT IN USE' handle
 *  Also create's handle level semaphore lock.
 *
 *  Returns NULL in case handle could not be allocated.
 */
static Vps_Tvp7002HandleObj *Vps_tvp7002AllocObj(void)
{
    UInt32                  handleId;
    Semaphore_Params        semPrms;
    Vps_Tvp7002HandleObj   *pObj = NULL;

    /* Take global lock to avoid race condition */
    Vps_tvp7002Lock();

    /* Find a unallocated object in pool */
    for (handleId = 0u; handleId < VPS_DEVICE_MAX_HANDLES; handleId++)
    {
        if (VPS_TVP7002_OBJ_STATE_UNUSED ==
            gVpsTvp7002Obj.handlePool[handleId].state)
        {
            /* Free object found */
            pObj = &gVpsTvp7002Obj.handlePool[handleId];

            /* Init state and handle ID */
            VpsUtils_memset(pObj, 0, sizeof (*pObj));
            pObj->state = VPS_TVP7002_OBJ_STATE_IDLE;
            pObj->handleId = handleId;

            /* Create driver object specific semaphore lock */
            Semaphore_Params_init(&semPrms);
            semPrms.mode = Semaphore_Mode_BINARY;
            pObj->lock = Semaphore_create(1u, &semPrms, NULL);
            if (NULL == pObj->lock)
            {
                GT_0trace(VpsDeviceTrace, GT_ERR,
                    "Handle semaphore create failed\n");
                /* Error - release object */
                pObj->state = VPS_TVP7002_OBJ_STATE_UNUSED;
                pObj = NULL;
            }
            break;
        }
    }

    /* Release global lock */
    Vps_tvp7002Unlock();

    return (pObj);
}



/**
 *  \brief De-Allocate driver object.
 *
 *  Marks handle as 'NOT IN USE'.
 *  Also delete's handle level semaphore lock.
 */
static Int32 Vps_tvp7002FreeObj(Vps_Tvp7002HandleObj *pObj)
{
    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));

    /* Take global lock to avoid race condition */
    Vps_tvp7002Lock();

    if (pObj->state != VPS_TVP7002_OBJ_STATE_UNUSED)
    {
        /* Mark state as unused */
        pObj->state = VPS_TVP7002_OBJ_STATE_UNUSED;

        /* Delete object locking semaphore */
        Semaphore_delete(&pObj->lock);
    }

    /* Release global lock */
    Vps_tvp7002Unlock();

    return (FVID2_SOK);
}



/**
 *  \brief Handle level lock.
 */
static Int32 Vps_tvp7002LockObj(Vps_Tvp7002HandleObj *pObj)
{
    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));

    Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);

    return (FVID2_SOK);
}



/**
 *  \brief Handle level unlock
 */
static Int32 Vps_tvp7002UnlockObj(Vps_Tvp7002HandleObj *pObj)
{
    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));

    Semaphore_post(pObj->lock);

    return (FVID2_SOK);
}



/**
 *  \brief Global driver level lock.
 */
static Int32 Vps_tvp7002Lock(void)
{
    Semaphore_pend(gVpsTvp7002Obj.lock, BIOS_WAIT_FOREVER);

    return (FVID2_SOK);
}



/**
 *  \brief Global driver level unlock.
 */
static Int32 Vps_tvp7002Unlock(void)
{
    Semaphore_post(gVpsTvp7002Obj.lock);

    return (FVID2_SOK);
}

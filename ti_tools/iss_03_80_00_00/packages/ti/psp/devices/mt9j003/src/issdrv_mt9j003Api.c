/** ==================================================================
 *  @file   issdrv_mt9j003Api.c
 *
 *  @path   /ti/psp/devices/mt9j003/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/devices/mt9j003/src/issdrv_mt9j003Priv.h>
#include <ti/psp/devices/mt9j003/issdrv_mt9j003_config.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_alg2APriv.h>

#define LONG_EXPO                       0x3012
#define LONG_GAIN                       0x305e

/* Global object storing all information related to all MT9J003 driver
 * handles */
static Iss_Mt9j003CommonObj gIss_Mt9j003CommonObj;

static I2c_Ctrl gMt9j003_I2c_ctrl;

extern ti2a_output ti2a_output_params;

/* Control API that gets called when FVID2_control is called
 *
 * This API does handle level semaphore locking
 *
 * handle - MT9J003 driver handle cmd - command cmdArgs - command arguments
 * cmdStatusArgs - command status
 *
 * returns error in case of - illegal parameters - I2C command RX/TX error */
/* ===================================================================
 *  @func     Iss_Mt9j003Control
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
Int32 Iss_Mt9j003Control(Fdrv_Handle handle,
                         UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs)
{
    Iss_Mt9j003Obj *pObj = (Iss_Mt9j003Obj *) handle;

    Int32 status;

    if (pObj == NULL)
        return FVID2_EBADARGS;

    /*
     * lock handle
     */
    Iss_Mt9j003LockObj(pObj);

    switch (cmd)
    {
        case FVID2_START:
            status = Iss_Mt9j003Start(pObj);
            break;

        case FVID2_STOP:
            status = Iss_Mt9j003Stop(pObj);
            break;

        case IOCTL_ISS_SENSOR_GET_CHIP_ID:
            status = Iss_Mt9j003GetChipId(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_ISS_SENSOR_RESET:
            status = Iss_Mt9j003Reset(pObj);
            break;

        case IOCTL_ISS_SENSOR_REG_WRITE:
            status = Iss_Mt9j003RegWrite(pObj, cmdArgs);
            break;

        case IOCTL_ISS_SENSOR_REG_READ:
            status = Iss_Mt9j003RegRead(pObj, cmdArgs);
            break;

        case IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN:
            status = Iss_Mt9j003UpdateExpGain(pObj, cmdArgs);
            break;

        case IOCTL_ISS_SENSOR_UPDATE_ITT:
            status = Iss_Mt9j003UpdateItt(pObj, cmdArgs);
            break;

        case IOCTL_ISS_SENSOR_UPDATE_FRAMERATE:
            status = Iss_Mt9j003UpdateFrameRate(pObj, cmdArgs);
            break;

        case IOCTL_ISS_SENSOR_FRAME_RATE_SET:
            status = Iss_Mt9j003FrameRateSet(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_ISS_SENSOR_PWM_CONFIG:
            status = FVID2_SOK;
            break;
        case IOCTL_ISS_SENSOR_SET_VIDEO_MODE:
        {
            Iss_SensorVideoModeParams *modePrms = (Iss_SensorVideoModeParams *)cmdArgs;

            if (NULL != modePrms)
            {
                status = Iss_mt9t003SetMode(pObj, (FVID2_Standard)modePrms->standard, MT9J_003_ADDR, 1);
            }
            else
            {
                status = FVID2_EBADARGS;
            }

            if (FVID2_SOK == status)
                pObj->createArgs.InputStandard = modePrms->standard;
            break;
        }
        default:
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    /*
     * unlock handle
     */
    Iss_Mt9j003UnlockObj(pObj);

    return status;
}

/*
 * Create API that gets called when FVID2_create is called
 *
 * This API does not configure the MT9J003 is any way.
 *
 * This API - validates parameters - allocates driver handle - stores create
 * arguments in its internal data structure.
 *
 * Later the create arguments will be used when doing I2C communcation with
 * MT9J003
 *
 * drvId - driver ID, must be FVID2_ISS_VID_DEC_MT9J003_DRV instanceId - must
 * be 0 createArgs - create arguments createStatusArgs - create status
 * fdmCbParams - NOT USED, set to NULL
 *
 * returns NULL in case of any error */
/* ===================================================================
 *  @func     Iss_Mt9j003Create
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
Fdrv_Handle Iss_Mt9j003Create(UInt32 drvId,
                              UInt32 instanceId,
                              Ptr createArgs,
                              Ptr createStatusArgs,
                              const FVID2_DrvCbParams * fdmCbParams)
{
    Iss_Mt9j003Obj *pObj;

    Int32 status = FVID2_SOK;

    Iss_SensorCreateParams *sensorCreateArgs
        = (Iss_SensorCreateParams *) createArgs;

    Iss_SensorCreateStatus *sensorCreateStatus
        = (Iss_SensorCreateStatus *) createStatusArgs;

    /*
     * check parameters
     */
    if (sensorCreateStatus == NULL)
        return NULL;

    sensorCreateStatus->retVal = FVID2_SOK;

    if (drvId != FVID2_ISS_SENSOR_MT9J003_DRV
        || instanceId != 0 || sensorCreateArgs == NULL)
    {
        sensorCreateStatus->retVal = FVID2_EBADARGS;
        return NULL;
    }

    if (sensorCreateArgs->deviceI2cInstId >= ISS_DEVICE_I2C_INST_ID_MAX)
    {
        sensorCreateStatus->retVal = FVID2_EINVALID_PARAMS;
        return NULL;
    }

    /*
     * allocate driver handle
     */
    pObj = Iss_Mt9j003AllocObj();
    if (pObj == NULL)
    {
        sensorCreateStatus->retVal = FVID2_EALLOC;
        return NULL;
    }

    gMt9j003_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();

    status = Iss_mt9t003SetMode(
                pObj,
                (FVID2_Standard)sensorCreateArgs->InputStandard,
                MT9J_003_ADDR,
                0);

    if (FVID2_SOK == status)
    {
        //if (sensorCreateArgs->vstabEnabled == 0)
        /*
         * copy parameters to allocate driver handle
         */
        memcpy(&pObj->createArgs, sensorCreateArgs, sizeof(*sensorCreateArgs));

        Iss_Mt9j003ResetRegCache(pObj);
    }
    else
    {
        Iss_Mt9j003Delete((Fdrv_Handle)pObj, NULL);

        pObj = NULL;
    }

    /*
     * return driver handle object pointer
     */
    return pObj;
}

/*
 * Delete function that is called when FVID2_delete is called
 *
 * This API - free's driver handle object
 *
 * handle - driver handle deleteArgs - NOT USED, set to NULL
 *
 */
/* ===================================================================
 *  @func     Iss_Mt9j003Delete
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
Int32 Iss_Mt9j003Delete(Fdrv_Handle handle, Ptr deleteArgs)
{
    Iss_Mt9j003Obj *pObj = (Iss_Mt9j003Obj *) handle;

    if (pObj == NULL)
        return FVID2_EBADARGS;

    /*
     * free driver handle object
     */
    Iss_Mt9j003FreeObj(pObj);

    return FVID2_SOK;
}

/*
 * System init for MT9J003 driver
 *
 * This API - create semaphore locks needed - registers driver to FVID2
 * sub-system - gets called as part of Iss_deviceInit() */
/* ===================================================================
 *  @func     Iss_Mt9j003Init
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
Int32 Iss_Mt9j003Init()
{
    Semaphore_Params semParams;

    Int32 status = FVID2_SOK;

    /*
     * Set to 0's for global object, descriptor memory
     */
    memset(&gIss_Mt9j003CommonObj, 0, sizeof(gIss_Mt9j003CommonObj));

    /*
     * Create global MT9J003 lock
     */
    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;

    gIss_Mt9j003CommonObj.lock = Semaphore_create(1u, &semParams, NULL);

    if (gIss_Mt9j003CommonObj.lock == NULL)
        status = FVID2_EALLOC;

    if (status == FVID2_SOK)
    {
        /*
         * Register MT9J003 driver with FVID2 sub-system
         */
        gIss_Mt9j003CommonObj.fvidDrvOps.create =
            (FVID2_DrvCreate) Iss_Mt9j003Create;
        gIss_Mt9j003CommonObj.fvidDrvOps.delete = Iss_Mt9j003Delete;
        gIss_Mt9j003CommonObj.fvidDrvOps.control = Iss_Mt9j003Control;
        gIss_Mt9j003CommonObj.fvidDrvOps.queue = NULL;
        gIss_Mt9j003CommonObj.fvidDrvOps.dequeue = NULL;
        gIss_Mt9j003CommonObj.fvidDrvOps.processFrames = NULL;
        gIss_Mt9j003CommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gIss_Mt9j003CommonObj.fvidDrvOps.drvId = FVID2_ISS_SENSOR_MT9J003_DRV;

        status = FVID2_registerDriver(&gIss_Mt9j003CommonObj.fvidDrvOps);

        if (status != FVID2_SOK)
        {
            /*
             * Error - free acquired resources
             */
            Semaphore_delete(&gIss_Mt9j003CommonObj.lock);
        }
    }

    if (status != FVID2_SOK)
    {
        Vps_printf(" ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__, __LINE__);
    }

    return status;
}

/*
 * System de-init for MT9J003 driver
 *
 * This API - de-registers driver from FVID2 sub-system - delete's allocated
 * semaphore locks - gets called as part of Iss_deviceDeInit() */
/* ===================================================================
 *  @func     Iss_Mt9j003DeInit
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
Int32 Iss_Mt9j003DeInit()
{
    /*
     * Unregister FVID2 driver
     */
    FVID2_unRegisterDriver(&gIss_Mt9j003CommonObj.fvidDrvOps);

    /*
     * Delete semaphore's
     */
    Semaphore_delete(&gIss_Mt9j003CommonObj.lock);

    return 0;
}

/*
 * Handle level lock */
/* ===================================================================
 *  @func     Iss_Mt9j003LockObj
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
Int32 Iss_Mt9j003LockObj(Iss_Mt9j003Obj * pObj)
{
    Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);

    return FVID2_SOK;
}

/*
 * Handle level unlock */
/* ===================================================================
 *  @func     Iss_Mt9j003UnlockObj
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
Int32 Iss_Mt9j003UnlockObj(Iss_Mt9j003Obj * pObj)
{
    Semaphore_post(pObj->lock);

    return FVID2_SOK;
}

/*
 * Global driver level lock */
/* ===================================================================
 *  @func     Iss_Mt9j003Lock
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
Int32 Iss_Mt9j003Lock()
{
    Semaphore_pend(gIss_Mt9j003CommonObj.lock, BIOS_WAIT_FOREVER);

    return FVID2_SOK;
}

/*
 * Global driver level unlock */
/* ===================================================================
 *  @func     Iss_Mt9j003Unlock
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
Int32 Iss_Mt9j003Unlock()
{
    Semaphore_post(gIss_Mt9j003CommonObj.lock);

    return FVID2_SOK;
}

/*
 * Allocate driver object
 *
 * Searches in list of driver handles and allocate's a 'NOT IN USE' handle
 * Also create's handle level semaphore lock
 *
 * return NULL in case handle could not be allocated */
/* ===================================================================
 *  @func     Iss_Mt9j003AllocObj
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
Iss_Mt9j003Obj *Iss_Mt9j003AllocObj()
{
    UInt32 handleId;

    Iss_Mt9j003Obj *pObj;

    Semaphore_Params semParams;

    UInt32 found = FALSE;

    /*
     * Take global lock to avoid race condition
     */
    Iss_Mt9j003Lock();

    /*
     * find a unallocated object in pool
     */
    for (handleId = 0; handleId < ISS_DEVICE_MAX_HANDLES; handleId++)
    {

        pObj = &gIss_Mt9j003CommonObj.Mt9j003Obj[handleId];

        if (pObj->state == ISS_MT9J003_OBJ_STATE_UNUSED)
        {
            /*
             * free object found
             */

            /*
             * init to 0's
             */
            memset(pObj, 0, sizeof(*pObj));

            /*
             * init state and handle ID
             */
            pObj->state = ISS_MT9J003_OBJ_STATE_IDLE;
            pObj->handleId = handleId;

            /*
             * create driver object specific semaphore lock
             */
            Semaphore_Params_init(&semParams);

            semParams.mode = Semaphore_Mode_BINARY;

            pObj->lock = Semaphore_create(1u, &semParams, NULL);

            found = TRUE;

            if (pObj->lock == NULL)
            {
                /*
                 * Error - release object
                 */
                found = FALSE;
                pObj->state = ISS_MT9J003_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /*
     * Release global lock
     */
    Iss_Mt9j003Unlock();

    if (found)
        return pObj;                                       /* Free object
                                                            * found return it
                                                            */

    /*
     * free object not found, return NULL
     */
    return NULL;
}

/*
 * De-Allocate driver object
 *
 * Marks handle as 'NOT IN USE' Also delete's handle level semaphore lock */
/* ===================================================================
 *  @func     Iss_Mt9j003FreeObj
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
Int32 Iss_Mt9j003FreeObj(Iss_Mt9j003Obj * pObj)
{
    /*
     * take global lock
     */
    Iss_Mt9j003Lock();

    if (pObj->state != ISS_MT9J003_OBJ_STATE_UNUSED)
    {
        /*
         * mark state as unused
         */
        pObj->state = ISS_MT9J003_OBJ_STATE_UNUSED;

        /*
         * delete object locking semaphore
         */
        Semaphore_delete(&pObj->lock);
    }

    /*
     * release global lock
     */
    Iss_Mt9j003Unlock();

    return FVID2_SOK;
}

UInt32 Iss_Mt9j003GainCalc(int gain)
{
    int Gain1 = 0;
    int Gain2 = 0;
    int GainSend = 0;

    if( gain >= 640000 )
        gain = 635000;

    if( gain >= 320000 )
    {
        Gain1 = 0x2DC0;
        Gain2 = gain*100/32;
    }
    else if( gain >= 160000 )
    {
        Gain1 = 0x1DC0;
        Gain2 = gain*100/16;
    }
    else if( gain >= 80000 )
    {
        Gain1 = 0x1CC0;
        Gain2 = gain*100/8;
    }
    else if( gain >= 40000 )
    {
        Gain1 = 0x1C40;
        Gain2 = gain*100/4;

    }
    else if( gain >= 20000 )
    {
        Gain1 = 0x1840;
        Gain2 = gain*100/2;

    }
    else if( gain >= 10000 )
    {
        Gain1 = 0x1040;
        Gain2 = gain*100/1;
    }

    GainSend = (Gain2*64/1000000)|Gain1;


    return (UInt32)GainSend;
}

/*
 * Update exposure and gain value from the 2A */
/* ===================================================================
 *  @func     Iss_Mt9j003UpdateExpGain
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
Int32 Iss_Mt9j003UpdateExpGain(Iss_Mt9j003Obj * pObj, Ptr createArgs)
{
    Int32 status = FVID2_SOK;

    Int32 g_int;

    Int32 c_int;

    Int32 PixelClock;

    Int32 LineLength;

    UInt32 i2cInstId = Iss_platformGetI2cInstId();

    Int32 devAddr = MT9J_003_ADDR;

    Int32 count = 0;

    gMt9j003_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
    devAddr = MT9J_003_ADDR;

    PixelClock = 160;                                      // MT9J_003_PIXEL_CLOCK;
    LineLength = 2177;

    g_int = MT9J003_GainTableMap(ti2a_output_params.sensorGain);

    c_int = (ti2a_output_params.sensorExposure * PixelClock) / LineLength;  // exposure
                                                                            // is
                                                                            // in
                                                                            // unit
                                                                            // of
                                                                            // 1/16
                                                                            // line

    /*
     * take global lock
     */
    Iss_Mt9j003Lock();
    {
        Int32 aewbVendor;

        aewbVendor = IssAlg_capt2AGetAEWBVendor();

        if (aewbVendor == 1)
        {
            if (ti2a_output_params.mask)
            {
                gMt9j003_I2c_ctrl.regAddr[count] = LONG_EXPO;
                gMt9j003_I2c_ctrl.regValue[count] =
                    ti2a_output_params.sensorExposure;
                gMt9j003_I2c_ctrl.numRegs = 1;
                status =
                    Iss_deviceWrite16(gMt9j003_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9j003_I2c_ctrl.regAddr[count],
                                      &gMt9j003_I2c_ctrl.regValue[count],
                                      gMt9j003_I2c_ctrl.numRegs);
            }
            if (ti2a_output_params.mask)
            {
                gMt9j003_I2c_ctrl.regAddr[count] = LONG_GAIN;
                gMt9j003_I2c_ctrl.regValue[count] = Iss_Mt9j003GainCalc(ti2a_output_params.sensorGain);
                gMt9j003_I2c_ctrl.numRegs = 1;
                status =
                    Iss_deviceWrite16(gMt9j003_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9j003_I2c_ctrl.regAddr[count],
                                      &gMt9j003_I2c_ctrl.regValue[count],
                                      gMt9j003_I2c_ctrl.numRegs);
            }
        }
        else if (aewbVendor == 2)
        {
            if (ti2a_output_params.mask)
            {
                gMt9j003_I2c_ctrl.regAddr[count] = LONG_EXPO;
                gMt9j003_I2c_ctrl.regValue[count] = c_int;
                gMt9j003_I2c_ctrl.numRegs = 1;
                status =
                    Iss_deviceWrite16(gMt9j003_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9j003_I2c_ctrl.regAddr[count],
                                      &gMt9j003_I2c_ctrl.regValue[count],
                                      gMt9j003_I2c_ctrl.numRegs);
            }
            if (ti2a_output_params.mask)
            {
                gMt9j003_I2c_ctrl.regAddr[count] = LONG_GAIN;
                gMt9j003_I2c_ctrl.regValue[count] = g_int;
                gMt9j003_I2c_ctrl.numRegs = 1;
                status =
                    Iss_deviceWrite16(gMt9j003_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9j003_I2c_ctrl.regAddr[count],
                                      &gMt9j003_I2c_ctrl.regValue[count],
                                      gMt9j003_I2c_ctrl.numRegs);
            }
        }
    }
    /*
     * release global lock
     */
    Iss_Mt9j003Unlock();

    return status;
}

/*
 * Update ITT Values */
/* ===================================================================
 *  @func     Iss_Mt9j003UpdateItt
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
Int32 Iss_Mt9j003UpdateItt(Iss_Mt9j003Obj * pObj,
                           Itt_RegisterParams * gItt_RegisterParams)
{
    int status = 0;

    int devAddr, count = 0;

    I2c_Ctrl mt9j003_I2c_ctrl;

    mt9j003_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
    devAddr = MT9J_003_ADDR;
    if (gItt_RegisterParams->Control == 1)
    {
        mt9j003_I2c_ctrl.regAddr[count] = gItt_RegisterParams->regAddr;
        mt9j003_I2c_ctrl.regValue[count] = gItt_RegisterParams->regValue;
        mt9j003_I2c_ctrl.numRegs = 1;
        status =
            Iss_deviceWrite16(mt9j003_I2c_ctrl.i2cInstId, devAddr,
                              &mt9j003_I2c_ctrl.regAddr[count],
                              &mt9j003_I2c_ctrl.regValue[count],
                              mt9j003_I2c_ctrl.numRegs);
    }
    else if (gItt_RegisterParams->Control == 0)
    {
        mt9j003_I2c_ctrl.regAddr[count] = gItt_RegisterParams->regAddr;
        mt9j003_I2c_ctrl.regValue[count] = 0;
        mt9j003_I2c_ctrl.numRegs = 1;
        status =
            Iss_deviceRead16(mt9j003_I2c_ctrl.i2cInstId, devAddr,
                             &mt9j003_I2c_ctrl.regAddr[count],
                             &mt9j003_I2c_ctrl.regValue[count],
                             mt9j003_I2c_ctrl.numRegs);
        count = 0;
        gItt_RegisterParams->regValue = mt9j003_I2c_ctrl.regValue[count];
    }
    return status;
}

/*
 * Update Frame Rate Control forMT9J003 driver
 *
 * This API - Checks the New Framerate and configures the sensor accordingly
 * - gets called from Application FVID2_Control statement */
/* ===================================================================
 *  @func     Iss_Mt9j003UpdateFrameRate
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
Int32 Iss_Mt9j003UpdateFrameRate(Iss_Mt9j003Obj * pObj,
                                 Iss_CaptFrameRate * framerateParams)
{
    Int32 status = FVID2_SOK;

    UInt32 i2cInstId = Iss_platformGetI2cInstId();

    Int32 devAddr = MT9J_003_ADDR;

    Int32 count = 0;

    gMt9j003_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
    devAddr = MT9J_003_ADDR;

    Vps_rprintf("framerateParams->FrameRate = %d\n",
                framerateParams->FrameRate);

    if (framerateParams->FrameRate == 60)
    {
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0342;
        gMt9j003_I2c_ctrl.regValue[count] = 0x0881;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0340;
        gMt9j003_I2c_ctrl.regValue[count] = 0x04c9;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x3012;
        gMt9j003_I2c_ctrl.regValue[count] = 1175;
        count++;
    }
    else if (framerateParams->FrameRate == 30)
    {
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0342;
        gMt9j003_I2c_ctrl.regValue[count] = 0x0880;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0340;
        gMt9j003_I2c_ctrl.regValue[count] = 0x0992;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x3012;
        gMt9j003_I2c_ctrl.regValue[count] = 2450;
        count++;
    }
    else
    {
        status = FVID2_EFAIL;
        return status;
    }
    /*
     * take global lock
     */
    Iss_Mt9j003Lock();

    gMt9j003_I2c_ctrl.numRegs = count;
    status =
        Iss_deviceWrite16(gMt9j003_I2c_ctrl.i2cInstId, devAddr,
                          &gMt9j003_I2c_ctrl.regAddr[0],
                          &gMt9j003_I2c_ctrl.regValue[0],
                          gMt9j003_I2c_ctrl.numRegs);

    /*
     * release global lock
     */
    Iss_Mt9j003Unlock();

    return status;
}

/* ===================================================================
 *  @func     Iss_Mt9j003FrameRateSet
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
Int32 Iss_Mt9j003FrameRateSet(Iss_Mt9j003Obj * pObj, Ptr createArgs,
                              Ptr cmdStatusArgs)
{
    Int32 status = FVID2_SOK;

    UInt32 i2cInstId = Iss_platformGetI2cInstId();

    Int32 devAddr = MT9J_003_ADDR;

    Int32 count = 0;

    Int32 framerate = *(Int32 *) createArgs;

    gMt9j003_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
    devAddr = MT9J_003_ADDR;

    if (framerate == 60)
    {
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0342;
        gMt9j003_I2c_ctrl.regValue[count] = 0x0881;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0340;
        gMt9j003_I2c_ctrl.regValue[count] = 0x04c9;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x3012;
        gMt9j003_I2c_ctrl.regValue[count] = 1225;
        count++;
        *(Int32 *) cmdStatusArgs = 1225;
    }
    else if (framerate == 50)
    {
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0342;
        gMt9j003_I2c_ctrl.regValue[count] = 0x0881;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0340;
        gMt9j003_I2c_ctrl.regValue[count] = 0x05BE;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x3012;
        gMt9j003_I2c_ctrl.regValue[count] = 1470;
        count++;
        *(Int32 *) cmdStatusArgs = 1470;
    }
    else if (framerate == 30)
    {
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0342;
        gMt9j003_I2c_ctrl.regValue[count] = 0x0880;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0340;
        gMt9j003_I2c_ctrl.regValue[count] = 0x0992;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x3012;
        gMt9j003_I2c_ctrl.regValue[count] = 2450;
        count++;
        *(Int32 *) cmdStatusArgs = 2450;
    }
    else if (framerate == 25)
    {
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0342;
        gMt9j003_I2c_ctrl.regValue[count] = 0x0880;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0340;
        gMt9j003_I2c_ctrl.regValue[count] = 0x0B7C;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x3012;
        gMt9j003_I2c_ctrl.regValue[count] = 2940;
        count++;
        *(Int32 *) cmdStatusArgs = 2940;
    }

    else
    {
        status = FVID2_EFAIL;
        return status;
    }

    /** take global lock **/
    Iss_Mt9j003Lock();

    gMt9j003_I2c_ctrl.numRegs = count;
    status =
        Iss_deviceWrite16(gMt9j003_I2c_ctrl.i2cInstId, devAddr,
                          &gMt9j003_I2c_ctrl.regAddr[0],
                          &gMt9j003_I2c_ctrl.regValue[0],
                          gMt9j003_I2c_ctrl.numRegs);

    /** release global lock **/
    Iss_Mt9j003Unlock();

    return status;
}


Int32 Iss_mt9t003SetMode(Iss_Mt9j003Obj *pObj, FVID2_Standard standard, UInt32 devAddr, UInt32 doReset)
{
    Int32 status = FVID2_SOK;
    UInt32 count = 0;

    Iss_Mt9j003Stop(pObj);

    switch (standard)
    {
    case FVID2_STD_5MP_2560_1920:
        for (count = 0; count < 69; count++)
        {
            gMt9j003_I2c_ctrl.regAddr[count] = SensorConfigScript5MP[count][0];
            gMt9j003_I2c_ctrl.regValue[count] = SensorConfigScript5MP[count][2];
            gMt9j003_I2c_ctrl.numRegs = 1;
            if (SensorConfigScript5MP[count][1] == 2)
            {
                status =
                    Iss_deviceWrite16(gMt9j003_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9j003_I2c_ctrl.regAddr[count],
                                      &gMt9j003_I2c_ctrl.regValue[count],
                                      gMt9j003_I2c_ctrl.numRegs);
            }
        }
        break;
    case FVID2_STD_8MP_3264_2448:
        for (count = 0; count < 69; count++)
        {
            gMt9j003_I2c_ctrl.regAddr[count] = SensorConfigScript8MP[count][0];
            gMt9j003_I2c_ctrl.regValue[count] = SensorConfigScript8MP[count][2];
            gMt9j003_I2c_ctrl.numRegs = 1;
            if (SensorConfigScript8MP[count][1] == 2)
            {
                status =
                    Iss_deviceWrite16(gMt9j003_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9j003_I2c_ctrl.regAddr[count],
                                      &gMt9j003_I2c_ctrl.regValue[count],
                                      gMt9j003_I2c_ctrl.numRegs);
            }
        }
        break;
    case FVID2_STD_10MP_3648_2736:
        for (count = 0; count < 69; count++)
        {
            gMt9j003_I2c_ctrl.regAddr[count] = SensorConfigScript10MP[count][0];
            gMt9j003_I2c_ctrl.regValue[count] = SensorConfigScript10MP[count][2];
            gMt9j003_I2c_ctrl.numRegs = 1;
            if (SensorConfigScript10MP[count][1] == 2)
            {
                status =
                    Iss_deviceWrite16(gMt9j003_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9j003_I2c_ctrl.regAddr[count],
                                      &gMt9j003_I2c_ctrl.regValue[count],
                                      gMt9j003_I2c_ctrl.numRegs);
            }
        }
        break;
    default:
        for (count = 0; count < 69; count++)
        {
            gMt9j003_I2c_ctrl.regAddr[count] = SensorConfigScript[count][0];
            gMt9j003_I2c_ctrl.regValue[count] = SensorConfigScript[count][2];
            gMt9j003_I2c_ctrl.numRegs = 1;
            if (SensorConfigScript[count][1] == 2)
            {
                status =
                    Iss_deviceWrite16(gMt9j003_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9j003_I2c_ctrl.regAddr[count],
                                      &gMt9j003_I2c_ctrl.regValue[count],
                                      gMt9j003_I2c_ctrl.numRegs);
            }
        }
        break;
    }
#if 0
    else
    {
        for (count = 0; count < 65; count++)
        {
            gMt9j003_I2c_ctrl.regAddr[count] = SensorConfigScriptVS[count][0];
            gMt9j003_I2c_ctrl.regValue[count] = SensorConfigScriptVS[count][2];
            gMt9j003_I2c_ctrl.numRegs = 1;
            if (SensorConfigScriptVS[count][1] == 2)
            {
                status =
                    Iss_deviceWrite16(gMt9j003_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9j003_I2c_ctrl.regAddr[count],
                                      &gMt9j003_I2c_ctrl.regValue[count],
                                      gMt9j003_I2c_ctrl.numRegs);
            }
        }
    }
#endif

    count = 0;
    if (standard == FVID2_STD_1080P_60)
    {
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0342;
        gMt9j003_I2c_ctrl.regValue[count] = 0x0881;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0340;
        gMt9j003_I2c_ctrl.regValue[count] = 0x04c9;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x3012;
        gMt9j003_I2c_ctrl.regValue[count] = 1175;
        count++;
    }
    else if (standard == FVID2_STD_1080P_30)
    {
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0342;
        gMt9j003_I2c_ctrl.regValue[count] = 0x0880;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x0340;
        gMt9j003_I2c_ctrl.regValue[count] = 0x0992;
        count++;
        gMt9j003_I2c_ctrl.regAddr[count] = 0x3012;
        gMt9j003_I2c_ctrl.regValue[count] = 2450;
        count++;
    }

    if (count > 0)
    {
        Iss_Mt9j003Lock();

        gMt9j003_I2c_ctrl.numRegs = count;
        status =
            Iss_deviceWrite16(gMt9j003_I2c_ctrl.i2cInstId, devAddr,
                              &gMt9j003_I2c_ctrl.regAddr[0],
                              &gMt9j003_I2c_ctrl.regValue[0],
                              gMt9j003_I2c_ctrl.numRegs);
        /*
         * release global lock
         */
        Iss_Mt9j003Unlock();
    }

    if (FVID2_SOK == status)
    {
        pObj->createArgs.InputStandard = standard;
        Iss_Mt9j003ResetRegCache(pObj);

        Iss_Mt9j003Start(pObj);
    }

    return (status);
}

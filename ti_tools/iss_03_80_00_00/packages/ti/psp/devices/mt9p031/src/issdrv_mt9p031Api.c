/** ==================================================================
 *  @file   issdrv_mt9p031Api.c                                                  
 *                                                                    
 *  @path   /ti/psp/devices/mt9p031/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/devices/mt9p031/src/issdrv_mt9p031Priv.h>
#include <ti/psp/devices/mt9p031/issdrv_mt9p031_config.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_alg2APriv.h>
#define LONG_EXPO                       0x3012
#define LONG_GAIN                       0x305e

/* Global object storing all information related to all MT9P031 driver
 * handles */
static Iss_Mt9p031CommonObj gIss_Mt9p031CommonObj;

static I2c_Ctrl gMt9p031_I2c_ctrl;

extern ti2a_output ti2a_output_params;

/* Control API that gets called when FVID2_control is called
 * 
 * This API does handle level semaphore locking
 * 
 * handle - MT9P031 driver handle cmd - command cmdArgs - command arguments
 * cmdStatusArgs - command status
 * 
 * returns error in case of - illegal parameters - I2C command RX/TX error */
/* ===================================================================
 *  @func     Iss_Mt9p031Control                                               
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
Int32 Iss_Mt9p031Control(Fdrv_Handle handle,
                         UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs)
{
    Iss_Mt9p031Obj *pObj = (Iss_Mt9p031Obj *) handle;

    Int32 status;

    if (pObj == NULL)
        return FVID2_EBADARGS;

    /* 
     * lock handle
     */
    Iss_Mt9p031LockObj(pObj);

    switch (cmd)
    {
        case FVID2_START:
            status = Iss_Mt9p031Start(pObj);
            break;

        case FVID2_STOP:
            status = Iss_Mt9p031Stop(pObj);
            break;

        case IOCTL_ISS_SENSOR_GET_CHIP_ID:
            status = Iss_Mt9p031GetChipId(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_ISS_SENSOR_RESET:
            status = Iss_Mt9p031Reset(pObj);
            break;

        case IOCTL_ISS_SENSOR_REG_WRITE:
            status = Iss_Mt9p031RegWrite(pObj, cmdArgs);
            break;

        case IOCTL_ISS_SENSOR_REG_READ:
            status = Iss_Mt9p031RegRead(pObj, cmdArgs);
            break;

        case IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN:
           // status = Iss_Mt9p031UpdateExpGain(pObj, cmdArgs);
            break;

        default:
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    /* 
     * unlock handle
     */
    Iss_Mt9p031UnlockObj(pObj);

    return status;
}

/* 
 * Create API that gets called when FVID2_create is called
 * 
 * This API does not configure the MT9P031 is any way.
 * 
 * This API - validates parameters - allocates driver handle - stores create
 * arguments in its internal data structure.
 * 
 * Later the create arguments will be used when doing I2C communcation with
 * MT9P031
 * 
 * drvId - driver ID, must be FVID2_ISS_VID_DEC_MT9P031_DRV instanceId - must 
 * be 0 createArgs - create arguments createStatusArgs - create status
 * fdmCbParams - NOT USED, set to NULL
 * 
 * returns NULL in case of any error */
/* ===================================================================
 *  @func     Iss_Mt9p031Create                                               
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
Fdrv_Handle Iss_Mt9p031Create(UInt32 drvId,
                              UInt32 instanceId,
                              Ptr createArgs,
                              Ptr createStatusArgs,
                              const FVID2_DrvCbParams * fdmCbParams)
{
    Iss_Mt9p031Obj *pObj;

    Int32 devAddr;

    Int32 count;

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

    if (drvId != FVID2_ISS_SENSOR_MT9P031_DRV
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
    pObj = Iss_Mt9p031AllocObj();
    if (pObj == NULL)
    {
        sensorCreateStatus->retVal = FVID2_EALLOC;
        return NULL;
    }
    gMt9p031_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
    devAddr = MT9P_031_ADDR;

 /*   for (count = 0; count < 21; count++)
    {
        gMt9p031_I2c_ctrl.regAddr[count] = SensorConfigScript_Mt9p031[count][0];
        gMt9p031_I2c_ctrl.regValue[count] = 0x00;
        gMt9p031_I2c_ctrl.numRegs = 1;
        if (SensorConfigScript_Mt9p031[count][1] == 2)
        {
            status =
                Iss_deviceRead16(gMt9p031_I2c_ctrl.i2cInstId, devAddr,
                                 &gMt9p031_I2c_ctrl.regAddr[count],
                                 &gMt9p031_I2c_ctrl.regValue[count],
                                 gMt9p031_I2c_ctrl.numRegs);
        }
        else if (SensorConfigScript_Mt9p031[count][1] == 0)
        {
            // DM814x_usecDelay(SensorConfigScript_Mt9p031[count][2]);
        }
    }*/
    for (count = 0; count < 21; count++)
    {
        gMt9p031_I2c_ctrl.regAddr[count] = SensorConfigScript_Mt9p031[count][0];
        gMt9p031_I2c_ctrl.regValue[count] =
            SensorConfigScript_Mt9p031[count][2];
        gMt9p031_I2c_ctrl.numRegs = 1;
        if (SensorConfigScript_Mt9p031[count][1] == 2)
        {
            status =
                Iss_deviceWrite16(gMt9p031_I2c_ctrl.i2cInstId, devAddr,
                                  &gMt9p031_I2c_ctrl.regAddr[count],
                                  &gMt9p031_I2c_ctrl.regValue[count],
                                  gMt9p031_I2c_ctrl.numRegs);
        }
        else if (SensorConfigScript_Mt9p031[count][2] == 1)
        {
            // status =
            // Iss_deviceWrite8(gMt9p031_I2c_ctrl.i2cInstId,devAddr,&gMt9p031_I2c_ctrl.regAddr[count],&gMt9p031_I2c_ctrl.regValue[count],gMt9p031_I2c_ctrl.numRegs);
        }
        else if (SensorConfigScript_Mt9p031[count][2] == 0)
        {
            // DM814x_usecDelay(SensorConfigScript_Mt9p031[count][2]);
        }
    }
  /*  for (count = 0; count < 21; count++)
    {
        gMt9p031_I2c_ctrl.regAddr[count] = SensorConfigScript_Mt9p031[count][0];
        gMt9p031_I2c_ctrl.regValue[count] = 0x00;
        gMt9p031_I2c_ctrl.numRegs = 1;
        if (SensorConfigScript_Mt9p031[count][1] == 2)
        {
            status =
                Iss_deviceRead16(gMt9p031_I2c_ctrl.i2cInstId, devAddr,
                                 &gMt9p031_I2c_ctrl.regAddr[count],
                                 &gMt9p031_I2c_ctrl.regValue[count],
                                 gMt9p031_I2c_ctrl.numRegs);
        }
        else if (SensorConfigScript_Mt9p031[count][2] == 0)
        {
            // DM814x_usecDelay(SensorConfigScript_Mt9p031[count][2]);
        }
    }*/
	if (status != 0)
    {
        Vps_rprintf("Capture : Sensor Create Failed \n");

    }
    /* 
     * copy parameters to allocate driver handle
     */
    memcpy(&pObj->createArgs, sensorCreateArgs, sizeof(*sensorCreateArgs));

    Iss_Mt9p031ResetRegCache(pObj);

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
 *  @func     Iss_Mt9p031Delete                                               
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
Int32 Iss_Mt9p031Delete(Fdrv_Handle handle, Ptr deleteArgs)
{
    Iss_Mt9p031Obj *pObj = (Iss_Mt9p031Obj *) handle;

    if (pObj == NULL)
        return FVID2_EBADARGS;

    /* 
     * free driver handle object
     */
    Iss_Mt9p031FreeObj(pObj);

    return FVID2_SOK;
}

/* 
 * System init for MT9P031 driver
 * 
 * This API - create semaphore locks needed - registers driver to FVID2
 * sub-system - gets called as part of Iss_deviceInit() */

/* ===================================================================
 *  @func     Iss_Mt9p031Init                                               
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
Int32 Iss_Mt9p031Init()
{
    Semaphore_Params semParams;

    Int32 status = FVID2_SOK;

    /* 
     * Set to 0's for global object, descriptor memory
     */
    memset(&gIss_Mt9p031CommonObj, 0, sizeof(gIss_Mt9p031CommonObj));

    /* 
     * Create global MT9P031 lock
     */
    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;

    gIss_Mt9p031CommonObj.lock = Semaphore_create(1u, &semParams, NULL);

    if (gIss_Mt9p031CommonObj.lock == NULL)
        status = FVID2_EALLOC;

    if (status == FVID2_SOK)
    {
        /* 
         * Register MT9P031 driver with FVID2 sub-system
         */

        gIss_Mt9p031CommonObj.fvidDrvOps.create =
            (FVID2_DrvCreate) Iss_Mt9p031Create;
        gIss_Mt9p031CommonObj.fvidDrvOps.delete = Iss_Mt9p031Delete;
        gIss_Mt9p031CommonObj.fvidDrvOps.control = Iss_Mt9p031Control;
        gIss_Mt9p031CommonObj.fvidDrvOps.queue = NULL;
        gIss_Mt9p031CommonObj.fvidDrvOps.dequeue = NULL;
        gIss_Mt9p031CommonObj.fvidDrvOps.processFrames = NULL;
        gIss_Mt9p031CommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gIss_Mt9p031CommonObj.fvidDrvOps.drvId = FVID2_ISS_SENSOR_MT9P031_DRV;

        status = FVID2_registerDriver(&gIss_Mt9p031CommonObj.fvidDrvOps);

        if (status != FVID2_SOK)
        {
            /* 
             * Error - free acquired resources
             */
            Semaphore_delete(&gIss_Mt9p031CommonObj.lock);
        }
    }

    if (status != FVID2_SOK)
    {
        Vps_printf(" ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__, __LINE__);
    }

    return status;
}

/* 
 * System de-init for MT9P031 driver
 * 
 * This API - de-registers driver from FVID2 sub-system - delete's allocated
 * semaphore locks - gets called as part of Iss_deviceDeInit() */
/* ===================================================================
 *  @func     Iss_Mt9p031DeInit                                               
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
Int32 Iss_Mt9p031DeInit()
{
    /* 
     * Unregister FVID2 driver
     */
    FVID2_unRegisterDriver(&gIss_Mt9p031CommonObj.fvidDrvOps);

    /* 
     * Delete semaphore's
     */
    Semaphore_delete(&gIss_Mt9p031CommonObj.lock);

    return 0;
}

/* ===================================================================
 *  @func     Iss_Mt9p031PinMux                                               
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
Int32 Iss_Mt9p031PinMux()
{
    /* setup CAM input pin mux */
    *PINCNTL156 = 0x00050002;                              // select function 
                                                           // 2 with receiver 
                                                           // enabled and
                                                           // pullup/down
                                                           // disabled - only 
                                                           // works in
                                                           // supervisor mode
    *PINCNTL157 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL158 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL159 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL160 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL161 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL162 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL163 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL164 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL165 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL166 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL167 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL168 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL169 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL170 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL171 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL172 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL173 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL174 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL175 = 0x00050002;                              // select function 
                                                           // 2 - only works
                                                           // in supervisor
                                                           // mode

    /* setup I2C2 pin mux */
    *PINCNTL74 |= 0x00000020;                              // select function 
                                                           // 6 - only works
                                                           // in supervisor
                                                           // mode
    *PINCNTL75 |= 0x00000020;                              // select function 
                                                           // 6 - only works
                                                           // in supervisor
                                                           // mode

    *(UInt32 *) 0x48140AAC = 0x00060002;                   // CAM_HSYNC
                                                           // PINCNTL172[1]
    *(UInt32 *) 0x48140AB0 = 0x00060002;                   // CAM_VSYNC
                                                           // PINCNTL173[1]
    *(UInt32 *) 0x48140AB8 = 0x00060002;                   // CAM_PCLK
                                                           // PINCNTL175[1]
    *(UInt32 *) 0x48140A58 = 0x00060020;                   // CAM_WEn
                                                           // PINCNTL151[5]
                                                           // cam_de_mux1
    *(UInt32 *) 0x48140A60 = 0x00060080;                   // gpio2[18]
                                                           // PINCNTL153[7]
                                                           // // 0x00060080
                                                           // CAM_RST
                                                           // PINCNTL153[5]
    *(UInt32 *) 0x48140A64 = 0x00060020;                   // CAM_STROBE
                                                           // PINCNTL154[5]
    *(UInt32 *) 0x48140A68 = 0x00060020;                   // CAM_SHTR
                                                           // PINCNTL155[5]
    *(UInt32 *) 0x48140AA8 = 0x00060002;                   // CAM_D0
                                                           // PINCNTL171[1]
    *(UInt32 *) 0x48140AA4 = 0x00060002;                   // CAM_D1
                                                           // PINCNTL170[1]
    *(UInt32 *) 0x48140AA0 = 0x00060002;                   // CAM_D2
                                                           // PINCNTL169[1]
    *(UInt32 *) 0x48140A9C = 0x00060002;                   // CAM_D3
                                                           // PINCNTL168[1]
    *(UInt32 *) 0x48140A98 = 0x00060002;                   // CAM_D4
                                                           // PINCNTL167[1]
    *(UInt32 *) 0x48140A94 = 0x00060002;                   // CAM_D5
                                                           // PINCNTL166[1]
    *(UInt32 *) 0x48140A90 = 0x00060002;                   // CAM_D6
                                                           // PINCNTL165[1]
    *(UInt32 *) 0x48140A8C = 0x00060002;                   // CAM_D7
                                                           // PINCNTL164[1]
    *(UInt32 *) 0x48140A6C = 0x00060002;                   // CAM_D8
                                                           // PINCNTL156[1]
    *(UInt32 *) 0x48140A70 = 0x00060002;                   // CAM_D9
                                                           // PINCNTL157[1]
    *(UInt32 *) 0x48140A74 = 0x00060002;                   // CAM_D10
                                                           // PINCNTL158[1]
    *(UInt32 *) 0x48140A78 = 0x00060002;                   // CAM_D11
                                                           // PINCNTL159[1]

    *(UInt32 *) 0x48140A7C = 0x00060002;                   // CAM_D12
                                                           // PINCNTL160[1]
    *(UInt32 *) 0x48140A80 = 0x00060002;                   // CAM_D13
                                                           // PINCNTL161[1]
    *(UInt32 *) 0x48140A84 = 0x00060002;                   // CAM_D14
                                                           // PINCNTL162[1]
    *(UInt32 *) 0x48140A88 = 0x00060002;                   // CAM_D15
                                                           // PINCNTL163[1]

    return 0;
}

/* 
 * Handle level lock */
/* ===================================================================
 *  @func     Iss_Mt9p031LockObj                                               
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
Int32 Iss_Mt9p031LockObj(Iss_Mt9p031Obj * pObj)
{
    Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);

    return FVID2_SOK;
}

/* 
 * Handle level unlock */
/* ===================================================================
 *  @func     Iss_Mt9p031UnlockObj                                               
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
Int32 Iss_Mt9p031UnlockObj(Iss_Mt9p031Obj * pObj)
{
    Semaphore_post(pObj->lock);

    return FVID2_SOK;
}

/* 
 * Global driver level lock */
/* ===================================================================
 *  @func     Iss_Mt9p031Lock                                               
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
Int32 Iss_Mt9p031Lock()
{
    Semaphore_pend(gIss_Mt9p031CommonObj.lock, BIOS_WAIT_FOREVER);

    return FVID2_SOK;
}

/* 
 * Global driver level unlock */
/* ===================================================================
 *  @func     Iss_Mt9p031Unlock                                               
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
Int32 Iss_Mt9p031Unlock()
{
    Semaphore_post(gIss_Mt9p031CommonObj.lock);

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
 *  @func     Iss_Mt9p031AllocObj                                               
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
Iss_Mt9p031Obj *Iss_Mt9p031AllocObj()
{
    UInt32 handleId;

    Iss_Mt9p031Obj *pObj;

    Semaphore_Params semParams;

    UInt32 found = FALSE;

    /* 
     * Take global lock to avoid race condition
     */
    Iss_Mt9p031Lock();

    /* 
     * find a unallocated object in pool
     */
    for (handleId = 0; handleId < ISS_DEVICE_MAX_HANDLES; handleId++)
    {

        pObj = &gIss_Mt9p031CommonObj.Mt9p031Obj[handleId];

        if (pObj->state == ISS_MT9P031_OBJ_STATE_UNUSED)
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
            pObj->state = ISS_MT9P031_OBJ_STATE_IDLE;
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
                pObj->state = ISS_MT9P031_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /* 
     * Release global lock
     */
    Iss_Mt9p031Unlock();

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
 *  @func     Iss_Mt9p031FreeObj                                               
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
Int32 Iss_Mt9p031FreeObj(Iss_Mt9p031Obj * pObj)
{
    /* 
     * take global lock
     */
    Iss_Mt9p031Lock();

    if (pObj->state != ISS_MT9P031_OBJ_STATE_UNUSED)
    {
        /* 
         * mark state as unused
         */
        pObj->state = ISS_MT9P031_OBJ_STATE_UNUSED;

        /* 
         * delete object locking semaphore
         */
        Semaphore_delete(&pObj->lock);
    }

    /* 
     * release global lock
     */
    Iss_Mt9p031Unlock();

    return FVID2_SOK;
}

/* 
 * Update exposure and gain value from the 2A */
/* ===================================================================
 *  @func     Iss_Mt9p031UpdateExpGain                                               
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
Int32 Iss_Mt9p031UpdateExpGain(Iss_Mt9p031Obj * pObj, Ptr createArgs)
{
    Int32 status = FVID2_SOK;

    Int32 exp_time_rows = 0;

    Int32 exp_time_rows_max;

    Int32 g_int;

    Int32 c_int;

  //  Int32 f_int;

    Int32 PixelClock;

    Int32 LineLength;

    UInt32 i2cInstId = Iss_platformGetI2cInstId();

    Int32 devAddr = MT9P_031_ADDR;

    Int32 count = 0;

    gMt9p031_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
    devAddr = MT9P_031_ADDR;

    PixelClock = 160;                                      // MT9P_031_PIXEL_CLOCK;
    LineLength = 2177;
    exp_time_rows_max = 2200;                              // MT9P_031_COARSE_INT_TIME_MAX;

    g_int = MT9P031_GainTableMap(ti2a_output_params.sensorGain);

    c_int = (ti2a_output_params.sensorExposure * PixelClock) / LineLength;  // exposure 
                                                                            // is 
                                                                            // in 
                                                                            // unit 
                                                                            // of 
                                                                            // 1/16 
                                                                            // line
  //  f_int =
  //      PixelClock - (c_int * LineLength) / (ti2a_output_params.sensorExposure);

    // clamp the calculated exposure time to its maximum value
    if (exp_time_rows > exp_time_rows_max)
    {
        exp_time_rows = exp_time_rows_max;
    }

    /* 
     * take global lock
     */
    Iss_Mt9p031Lock();
    {
        Int32 aewbVendor;
		aewbVendor = IssAlg_capt2AGetAEWBVendor();
        if (aewbVendor == 1)
        {
            if (ti2a_output_params.mask)
            {
                gMt9p031_I2c_ctrl.regAddr[count] = LONG_EXPO;
                gMt9p031_I2c_ctrl.regValue[count] =
                    ti2a_output_params.sensorExposure;
                gMt9p031_I2c_ctrl.numRegs = 1;
                status =
                    Iss_deviceWrite16(gMt9p031_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9p031_I2c_ctrl.regAddr[count],
                                      &gMt9p031_I2c_ctrl.regValue[count],
                                      gMt9p031_I2c_ctrl.numRegs);
            }
            if (ti2a_output_params.mask)
            {
                gMt9p031_I2c_ctrl.regAddr[count] = LONG_GAIN;
                gMt9p031_I2c_ctrl.regValue[count] =
                    ti2a_output_params.sensorGain;
                gMt9p031_I2c_ctrl.numRegs = 1;
                status =
                    Iss_deviceWrite16(gMt9p031_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9p031_I2c_ctrl.regAddr[count],
                                      &gMt9p031_I2c_ctrl.regValue[count],
                                      gMt9p031_I2c_ctrl.numRegs);
            }
        }
        else if (aewbVendor == 2)
        {
            if (ti2a_output_params.mask)
            {
                gMt9p031_I2c_ctrl.regAddr[count] = LONG_EXPO;
                gMt9p031_I2c_ctrl.regValue[count] = c_int;
                gMt9p031_I2c_ctrl.numRegs = 1;
                status =
                    Iss_deviceWrite16(gMt9p031_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9p031_I2c_ctrl.regAddr[count],
                                      &gMt9p031_I2c_ctrl.regValue[count],
                                      gMt9p031_I2c_ctrl.numRegs);
            }
            if (ti2a_output_params.mask)
            {
                gMt9p031_I2c_ctrl.regAddr[count] = LONG_GAIN;
                gMt9p031_I2c_ctrl.regValue[count] = g_int;
                gMt9p031_I2c_ctrl.numRegs = 1;
                status =
                    Iss_deviceWrite16(gMt9p031_I2c_ctrl.i2cInstId, devAddr,
                                      &gMt9p031_I2c_ctrl.regAddr[count],
                                      &gMt9p031_I2c_ctrl.regValue[count],
                                      gMt9p031_I2c_ctrl.numRegs);
            }
        }
    }
    /* 
     * release global lock
     */
    Iss_Mt9p031Unlock();

    return status;
}

/** ==================================================================
 *  @file   issdrv_ov7740Api.c                                                 
 *                                                                    
 *  @path   /ti/psp/devices/ov7740/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/devices/ov7740/src/issdrv_ov7740Priv.h>
#include <ti/psp/devices/ov7740/issdrv_ov7740_config.h>
#include <ti/psp/platforms/iss_platform.h>
#define LONG_EXPO                       0x3012
#define LONG_GAIN                       0x305e

/* Global object storing all information related to all
  OV7740 driver handles */
static Iss_Ov7740CommonObj 	gIss_Ov7740CommonObj;

static I2c_Ctrl 				gOv7740_I2c_ctrl;

extern ti2a_output 		ti2a_output_params;

/* Control API that gets called when FVID2_control is called
 * 
 * This API does handle level semaphore locking
 * 
 * handle - OV7740 driver handle cmd - command cmdArgs - command arguments
 * cmdStatusArgs - command status
 * 
 * returns error in case of - illegal parameters - I2C command RX/TX error */
/* ===================================================================
 *  @func     Iss_Ov7740Control                                               
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
Int32 Iss_Ov7740Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
    Iss_Ov7740Obj *pObj = ( Iss_Ov7740Obj * ) handle;
    Int32 status;

    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /*
     * lock handle
     */
    Iss_Ov7740LockObj ( pObj );

    switch ( cmd )
    {
        case FVID2_START:
       //     status = Iss_Ov7740Start ( pObj );
            break;

        case FVID2_STOP:
      //      status = Iss_Ov7740Stop ( pObj );
            break;

        case IOCTL_ISS_SENSOR_GET_CHIP_ID:
      //      status = Iss_Ov7740GetChipId ( pObj, cmdArgs, cmdStatusArgs );
            break;

        case IOCTL_ISS_SENSOR_RESET:
      //      status = Iss_Ov7740Reset ( pObj );
            break;

        case IOCTL_ISS_SENSOR_REG_WRITE:
            status = Iss_Ov7740RegWrite ( pObj, cmdArgs );
            break;

        case IOCTL_ISS_SENSOR_REG_READ:
            status = Iss_Ov7740RegRead ( pObj, cmdArgs );
            break;

        case IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN:
             status = Iss_Ov7740UpdateExpGain ( pObj, cmdArgs);
             break;

		case IOCTL_ISS_SENSOR_UPDATE_ITT:
          //   status = Iss_Ov7740UpdateItt ( pObj, cmdArgs);
             break;

        default:
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    /*
     * unlock handle
     */
    Iss_Ov7740UnlockObj ( pObj );

    return status;
}

/* 
 * Create API that gets called when FVID2_create is called
 * 
 * This API does not configure the OV7740 is any way.
 * 
 * This API - validates parameters - allocates driver handle - stores create
 * arguments in its internal data structure.
 * 
 * Later the create arguments will be used when doing I2C communcation with
 * OV7740
 * 
 * drvId - driver ID, must be FVID2_ISS_SENSOR_OV7740_DRV instanceId - must 
 * be 0 createArgs - create arguments createStatusArgs - create status
 * fdmCbParams - NOT USED, set to NULL
 * 
 * returns NULL in case of any error */
/* ===================================================================
 *  @func     Iss_Ov7740Create                                               
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
Fdrv_Handle Iss_Ov7740Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams )
{
    Iss_Ov7740Obj *pObj;
    Iss_SensorCreateParams *sensorCreateArgs
        = ( Iss_SensorCreateParams * ) createArgs;

    Iss_SensorCreateStatus *sensorCreateStatus
        = ( Iss_SensorCreateStatus * ) createStatusArgs;

    /*
     * check parameters
     */
    if ( sensorCreateStatus == NULL )
        return NULL;

    sensorCreateStatus->retVal = FVID2_SOK;

    if ( drvId != FVID2_ISS_SENSOR_OV7740_DRV
         || instanceId != 0 || sensorCreateArgs == NULL )
    {
        sensorCreateStatus->retVal = FVID2_EBADARGS;
        return NULL;
    }

    if ( sensorCreateArgs->deviceI2cInstId >= ISS_DEVICE_I2C_INST_ID_MAX )
    {
        sensorCreateStatus->retVal = FVID2_EINVALID_PARAMS;
        return NULL;
    }

    /*
     * allocate driver handle
     */
    pObj = Iss_Ov7740AllocObj (  );
    if ( pObj == NULL )
    {
        sensorCreateStatus->retVal = FVID2_EALLOC;
        return NULL;
    }

    /*
     * copy parameters to allocate driver handle
     */
    memcpy ( &pObj->createArgs, sensorCreateArgs, sizeof ( *sensorCreateArgs ) );

    Iss_Ov7740ResetRegCache(pObj);

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
 *  @func     Iss_Ov7740Delete                                               
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
Int32 Iss_Ov7740Delete ( Fdrv_Handle handle, Ptr deleteArgs )
{
    Iss_Ov7740Obj *pObj = ( Iss_Ov7740Obj * ) handle;

    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /*
     * free driver handle object
     */
    Iss_Ov7740FreeObj ( pObj );

    return FVID2_SOK;
}

/* 
 * System init for OV7740 driver
 * 
 * This API - create semaphore locks needed - registers driver to FVID2
 * sub-system - gets called as part of Iss_deviceInit() */

/* ===================================================================
 *  @func     Iss_Ov7740Init                                               
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
Int32 Iss_Ov7740Init (  )
{
    Semaphore_Params semParams;

    Int32 status = FVID2_SOK;

    Int32 count;

    Int32 devAddr;
    /*
     * Set to 0's for global object, descriptor memory
     */
    memset ( &gIss_Ov7740CommonObj, 0, sizeof ( gIss_Ov7740CommonObj ) );

    /*
     * Create global OV7740 lock
     */
    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    gIss_Ov7740CommonObj.lock = Semaphore_create ( 1u, &semParams, NULL );

    if ( gIss_Ov7740CommonObj.lock == NULL )
        status = FVID2_EALLOC;

    if ( status == FVID2_SOK )
    {
        /*
         * Register OV7740 driver with FVID2 sub-system
         */
		gOv7740_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
		Vps_printf("\ngOv7740_I2c_ctrl.i2cInstId is %d\n",gOv7740_I2c_ctrl.i2cInstId);
		devAddr = OV_7740_ADDR;

		for(count=0; count<sizeof(SensorConfigScript_rgb)/3/*122*/; count++){
			gOv7740_I2c_ctrl.regAddr[count] = SensorConfigScript_rgb[count][0];
			gOv7740_I2c_ctrl.regValue[count] = SensorConfigScript_rgb[count][2];
			gOv7740_I2c_ctrl.numRegs = 1;

			if(SensorConfigScript_rgb[count][1] == 2){
				status = Iss_deviceWrite8(gOv7740_I2c_ctrl.i2cInstId,devAddr,&gOv7740_I2c_ctrl.regAddr[count],&gOv7740_I2c_ctrl.regValue[count],gOv7740_I2c_ctrl.numRegs);
                                {
                                UInt8 value = 0;
                                status = Iss_deviceRead8(gOv7740_I2c_ctrl.i2cInstId,devAddr,&gOv7740_I2c_ctrl.regAddr[count],&value,gOv7740_I2c_ctrl.numRegs);
                                }
			}
		}
		
#if 0 //dm385 evm		
		gOv7740_I2c_ctrl.regAddr[0] = 0x80;
		gOv7740_I2c_ctrl.regValue[0] = 0x00;
		gOv7740_I2c_ctrl.numRegs = 1;
		Iss_deviceWrite8(gOv7740_I2c_ctrl.i2cInstId,0x20,&gOv7740_I2c_ctrl.regAddr[0],&gOv7740_I2c_ctrl.regValue[0],gOv7740_I2c_ctrl.numRegs);
#endif		
		

        gIss_Ov7740CommonObj.fvidDrvOps.create = ( FVID2_DrvCreate ) Iss_Ov7740Create;
        gIss_Ov7740CommonObj.fvidDrvOps.delete = Iss_Ov7740Delete;
        gIss_Ov7740CommonObj.fvidDrvOps.control = Iss_Ov7740Control;
        gIss_Ov7740CommonObj.fvidDrvOps.queue = NULL;
        gIss_Ov7740CommonObj.fvidDrvOps.dequeue = NULL;
        gIss_Ov7740CommonObj.fvidDrvOps.processFrames = NULL;
        gIss_Ov7740CommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gIss_Ov7740CommonObj.fvidDrvOps.drvId = FVID2_ISS_SENSOR_OV7740_DRV;

        status = FVID2_registerDriver ( &gIss_Ov7740CommonObj.fvidDrvOps );

        if ( status != FVID2_SOK )
        {
            /*
             * Error - free acquired resources
             */
            Semaphore_delete ( &gIss_Ov7740CommonObj.lock );
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
 * System de-init for OV7740 driver
 * 
 * This API - de-registers driver from FVID2 sub-system - delete's allocated
 * semaphore locks - gets called as part of Iss_deviceDeInit() */
/* ===================================================================
 *  @func     Iss_Ov7740DeInit                                               
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
Int32 Iss_Ov7740DeInit (  )
{
    /*
     * Unregister FVID2 driver
     */
    FVID2_unRegisterDriver ( &gIss_Ov7740CommonObj.fvidDrvOps );

    /*
     * Delete semaphore's
     */
    Semaphore_delete ( &gIss_Ov7740CommonObj.lock );

    return 0;
}

/* ===================================================================
 *  @func     Iss_Ov7740PinMux                                               
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

Int32 Iss_Ov7740PinMux (  )
{
	/* setup CAM input pin mux */
	*PINCNTL156 = 0x00050002;				// select function 2 with receiver enabled and pullup/down disabled  - only works in supervisor mode
	*PINCNTL157 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL158 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL159 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL160 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL161 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL162 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL163 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL164 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL165 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL166 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL167 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL168 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL169 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL170 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL171 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL172 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL173 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL174 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL175 = 0x00050002;				// select function 2  - only works in supervisor mode

   *(UInt32 *)0x48140AAC = 0x00060002;    // CAM_HSYNC        PINCNTL172[1]
    *(UInt32 *)0x48140AB0 = 0x00060002;    // CAM_VSYNC        PINCNTL173[1]
    *(UInt32 *)0x48140AB8 = 0x00060002;    // CAM_PCLK         PINCNTL175[1]
    *(UInt32 *)0x48140A60 = 0x00060080;    // gpio2[18]        PINCNTL153[7] // 0x00060080   CAM_RST          PINCNTL153[5]
   // *(UInt32 *)0x48140A58 = 0x00060020;    // CAM_WEn          PINCNTL151[5] cam_de_mux1
   *(UInt32 *)0x48140A64 = 0x00060020;    // CAM_STROBE       PINCNTL154[5]
    *(UInt32 *)0x48140A68 = 0x00060020;    // CAM_SHTR         PINCNTL155[5]
    *(UInt32 *)0x48140AA8 = 0x00060002;    // CAM_D0           PINCNTL171[1]
    *(UInt32 *)0x48140AA4 = 0x00060002;    // CAM_D1           PINCNTL170[1]
    *(UInt32 *)0x48140AA0 = 0x00060002;    // CAM_D2           PINCNTL169[1]
    *(UInt32 *)0x48140A9C = 0x00060002;    // CAM_D3           PINCNTL168[1]
    *(UInt32 *)0x48140A98 = 0x00060002;    // CAM_D4           PINCNTL167[1]
    *(UInt32 *)0x48140A94 = 0x00060002;    // CAM_D5           PINCNTL166[1]
    *(UInt32 *)0x48140A90 = 0x00060002;    // CAM_D6           PINCNTL165[1]
    *(UInt32 *)0x48140A8C = 0x00060002;    // CAM_D7           PINCNTL164[1]
    *(UInt32 *)0x48140A6C = 0x00060002;    // CAM_D8           PINCNTL156[1]
    *(UInt32 *)0x48140A70 = 0x00060002;    // CAM_D9           PINCNTL157[1]
    *(UInt32 *)0x48140A74 = 0x00060002;    // CAM_D10          PINCNTL158[1]
    *(UInt32 *)0x48140A78 = 0x00060002;    // CAM_D11          PINCNTL159[1]

	/* setup I2C2 pin mux */
	*PINCNTL74 |= 0x00000020;				// select function 6  - only works in supervisor mode
	*PINCNTL75 |= 0x00000020;				// select function 6  - only works in supervisor mode
	
	return 0;
}
/* 
 * Handle level lock */
/* ===================================================================
 *  @func     Iss_Ov7740LockObj                                               
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
Int32 Iss_Ov7740LockObj ( Iss_Ov7740Obj * pObj )
{
    Semaphore_pend ( pObj->lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/* 
 * Handle level unlock */
/* ===================================================================
 *  @func     Iss_Ov7740UnlockObj                                               
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
Int32 Iss_Ov7740UnlockObj ( Iss_Ov7740Obj * pObj )
{
    Semaphore_post ( pObj->lock );

    return FVID2_SOK;
}

/* 
 * Global driver level lock */
/* ===================================================================
 *  @func     Iss_Ov7740Lock                                               
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
Int32 Iss_Ov7740Lock (  )
{
    Semaphore_pend ( gIss_Ov7740CommonObj.lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/* 
 * Global driver level unlock */
/* ===================================================================
 *  @func     Iss_Ov7740Unlock                                               
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
Int32 Iss_Ov7740Unlock (  )
{
    Semaphore_post ( gIss_Ov7740CommonObj.lock );

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
 *  @func     Iss_Ov7740AllocObj                                               
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
Iss_Ov7740Obj *Iss_Ov7740AllocObj (  )
{
    UInt32 handleId;

    Iss_Ov7740Obj *pObj;

    Semaphore_Params semParams;

    UInt32 found = FALSE;

    /*
     * Take global lock to avoid race condition
     */
    Iss_Ov7740Lock (  );

    /*
     * find a unallocated object in pool
     */
    for ( handleId = 0; handleId < ISS_DEVICE_MAX_HANDLES; handleId++ )
    {

        pObj = &gIss_Ov7740CommonObj.Ov7740Obj[handleId];

        if ( pObj->state == ISS_OV7740_OBJ_STATE_UNUSED )
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
            pObj->state = ISS_OV7740_OBJ_STATE_IDLE;
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
                pObj->state = ISS_OV7740_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /*
     * Release global lock
     */
    Iss_Ov7740Unlock (  );

    if ( found )
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
 *  @func     Iss_Ov7740FreeObj                                               
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
Int32 Iss_Ov7740FreeObj ( Iss_Ov7740Obj * pObj )
{
    /*
     * take global lock
     */
    Iss_Ov7740Lock (  );

    if ( pObj->state != ISS_OV7740_OBJ_STATE_UNUSED )
    {
        /*
         * mark state as unused
         */
        pObj->state = ISS_OV7740_OBJ_STATE_UNUSED;

        /*
         * delete object locking semaphore
         */
        Semaphore_delete ( &pObj->lock );
    }

    /*
     * release global lock
     */
    Iss_Ov7740Unlock (  );

    return FVID2_SOK;
}

/* 
 * Update exposure and gain value from the 2A */
/* ===================================================================
 *  @func     Iss_Ov7740UpdateExpGain                                               
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
Int32 Iss_Ov7740UpdateExpGain ( Iss_Ov7740Obj * pObj, Ptr createArgs )
{
	static int first = 1;
	
	if (first)
	{
		*(unsigned int *)(0x5505144C) = 0x60267CF;
	}
  //      Vps_printf("Exp gain: %d Exp: %d",ti2a_output_params.sensorGain,ti2a_output_params.sensorExposure);

	Int32 status = FVID2_SOK;
	Int32 exp_time_rows = 0;
	Int32 exp_time_rows_max;
	Int32 g_int;
	Int32 c_int;
	UInt32 i2cInstId = Iss_platformGetI2cInstId();
	Int32 devAddr = OV_7740_ADDR;
	Int32 count = 0;


	gOv7740_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
	devAddr = OV_7740_ADDR;

	//PixelClock = 160;
	//LineLength = 2177;
	exp_time_rows_max = 960;

	g_int = OV7740_GainTableMap(ti2a_output_params.sensorGain);

	c_int = (ti2a_output_params.sensorExposure)/70;  // exposure is in unit of 1/16 line
	//f_int = PixelClock - (c_int * LineLength)/(ti2a_output_params.sensorExposure);


	// clamp the calculated exposure time to its maximum value
	if( exp_time_rows > exp_time_rows_max )
	{
		exp_time_rows = exp_time_rows_max;
	}

	/*
     * take global lock
     */
    Iss_Ov7740Lock (  );
    {
    	extern Int32 aewbVendor;
		   	if (ti2a_output_params.mask) 
			{
				gOv7740_I2c_ctrl.regAddr[count] = 0x10;
				gOv7740_I2c_ctrl.regValue[count] = (c_int & 0xFF);
				gOv7740_I2c_ctrl.numRegs = 1;
				//Rfile_printf("sensor Exposure Time reg 0 value   :  < %5d, %5d > \n",gOv7740_I2c_ctrl.regValue[count],c_int );
				status = Iss_deviceWrite8(gOv7740_I2c_ctrl.i2cInstId,devAddr,&gOv7740_I2c_ctrl.regAddr[count],&gOv7740_I2c_ctrl.regValue[count],gOv7740_I2c_ctrl.numRegs);

				gOv7740_I2c_ctrl.regAddr[count] = 0xF;
				gOv7740_I2c_ctrl.regValue[count] = ((c_int >> 8) & (0xFF));
				gOv7740_I2c_ctrl.numRegs = 1;
				//Rfile_printf("sensor Exposure Time reg 1 value   :  < %5d, %5d> \n",gOv7740_I2c_ctrl.regValue[count] ,c_int );
				status = Iss_deviceWrite8(gOv7740_I2c_ctrl.i2cInstId,devAddr,&gOv7740_I2c_ctrl.regAddr[count],&gOv7740_I2c_ctrl.regValue[count],gOv7740_I2c_ctrl.numRegs);
			}
		    if (ti2a_output_params.mask) 
			{
				gOv7740_I2c_ctrl.regAddr[count] = 0x0;
				gOv7740_I2c_ctrl.regValue[count] = g_int;
				gOv7740_I2c_ctrl.numRegs = 1;
			    //Rfile_printf("sensor Analog gain reg value  :  < 0x%2x > \n",gOv7740_I2c_ctrl.regValue[count]  );
				status = Iss_deviceWrite8(gOv7740_I2c_ctrl.i2cInstId,devAddr,&gOv7740_I2c_ctrl.regAddr[count],&gOv7740_I2c_ctrl.regValue[count],gOv7740_I2c_ctrl.numRegs);
			}
		}
     /*
     * release global lock
     */
    Iss_Ov7740Unlock (  );

	return status;

}
#if 0
/*
  Update ITT Values
*/
Int32 Iss_Ov7740UpdateItt ( Iss_Ov7740Obj * pObj, Ptr createArgs )
{
	int status,devAddr,count  = 0;
	I2c_Ctrl 				ov7740_I2c_ctrl;

	ov7740_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
	devAddr = OV_7740_ADDR;
	if(gItt_RegisterParams.Control == 1){
	ov7740_I2c_ctrl.regAddr[count]  = gItt_RegisterParams.regAddr;
	ov7740_I2c_ctrl.regValue[count] = gItt_RegisterParams.regValue;
	ov7740_I2c_ctrl.numRegs = 1;

	status = Iss_deviceWrite8(ov7740_I2c_ctrl.i2cInstId,devAddr,&ov7740_I2c_ctrl.regAddr[count],&ov7740_I2c_ctrl.regValue[count],ov7740_I2c_ctrl.numRegs);
	}
	else if(gItt_RegisterParams.Control == 0){

	ov7740_I2c_ctrl.regAddr[count]  = gItt_RegisterParams.regAddr;
	ov7740_I2c_ctrl.regValue[count] = 0;
	ov7740_I2c_ctrl.numRegs = 1;
	status = Iss_deviceRead8(ov7740_I2c_ctrl.i2cInstId,devAddr,&ov7740_I2c_ctrl.regAddr[count],&ov7740_I2c_ctrl.regValue[count],ov7740_I2c_ctrl.numRegs);
	count  = 0;
	gItt_RegisterParams.regValue = ov7740_I2c_ctrl.regValue[count];
	}
	return status;
}
#endif

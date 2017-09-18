/** ==================================================================
 *  @file   issdrv_mn34041Api.c                                                  
 *                                                                    
 *  @path   /ti/psp/devices/mn34041/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/devices/mn34041/src/issdrv_mn34041Priv.h>
#include <ti/psp/devices/mn34041/issdrv_mn34041_config.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif_reg.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_alg2APriv.h>
#include <math.h>
#define LONG_EXPO                       0x3012
#define LONG_GAIN                       0x305e

#define SET_REG_IN_CREATERFUN      0

extern isif_regs_ovly isif_reg;

static int FrameTime = 1125;

/* Global object storing all information related to all MN34041 driver
 * handles */
static Iss_Mn34041CommonObj gIss_Mn34041CommonObj;

extern ti2a_output ti2a_output_params;

/* ===================================================================
 *  @func     mn34_041_setcs                                               
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
void mn34_041_setcs()
{
    *GIO_WRITE_DATA |= 0x1 << 16;
}

/* ===================================================================
 *  @func     mn34_041_clrcs                                               
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
void mn34_041_clrcs()
{
    *GIO_CLEAR_DATA = 0x1 << 16;
}

/* ===================================================================
 *  @func     mn34_041_setclk                                               
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
void mn34_041_setclk()
{
    *GIO_WRITE_DATA |= 0x1 << 17;
}

/* ===================================================================
 *  @func     mn34_041_clrclk                                               
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
void mn34_041_clrclk()
{
    *GIO_CLEAR_DATA = 0x1 << 17;
}

/* ===================================================================
 *  @func     mn34_041_setdata                                               
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
void mn34_041_setdata()
{
    *GIO_WRITE_DATA |= 0x1 << 26;
}

/* ===================================================================
 *  @func     mn34_041_clrdata                                               
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
void mn34_041_clrdata()
{
    *GIO_CLEAR_DATA = 0x1 << 26;
}

/* ===================================================================
 *  @func     mn34_041_setgio                                               
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
void mn34_041_setgio(unsigned short addr, unsigned short reg)
{
    int i;

    mn34_041_clrclk();
    mn34_041_clrcs();

    for (i = 0; i < 16; i++)
    {
        if (addr & 0x00000001)
            mn34_041_setdata();
        else
            mn34_041_clrdata();
        addr = addr >> 1;

        mn34_041_setclk();
        mn34_041_clrclk();
    }
/***********************************/
    for (i = 0; i < 16; i++)
    {
        if (reg & 0x00000001)
            mn34_041_setdata();
        else
            mn34_041_clrdata();
        reg = reg >> 1;

        mn34_041_setclk();
        mn34_041_clrclk();
    }

    // CS high 
    mn34_041_setcs();
}

/* Control API that gets called when FVID2_control is called
 * 
 * This API does handle level semaphore locking
 * 
 * handle - MN34041 driver handle cmd - command cmdArgs - command arguments
 * cmdStatusArgs - command status
 * 
 * returns error in case of - illegal parameters - I2C command RX/TX error */
/* ===================================================================
 *  @func     Iss_Mn34041Control                                               
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
Int32 Iss_Mn34041Control(Fdrv_Handle handle,
                         UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs)
{
    Iss_Mn34041Obj *pObj = (Iss_Mn34041Obj *) handle;

    Int32 status;

    if (pObj == NULL)
        return FVID2_EBADARGS;

    /* 
     * lock handle
     */
    Iss_Mn34041LockObj(pObj);

    switch (cmd)
    {
        case FVID2_START:
            // status = Iss_Mn34041Start ( pObj );
            break;

        case FVID2_STOP:
            // status = Iss_Mn34041Stop ( pObj );
            break;

        case IOCTL_ISS_SENSOR_GET_CHIP_ID:
            // status = Iss_Mn34041GetChipId ( pObj, cmdArgs, cmdStatusArgs
            // );
            break;

        case IOCTL_ISS_SENSOR_RESET:
            // status = Iss_Mn34041Reset ( pObj );
            break;

        case IOCTL_ISS_SENSOR_REG_WRITE:
            status = Iss_Mn34041RegWrite ( pObj, cmdArgs );
            break;

        case IOCTL_ISS_SENSOR_REG_READ:
            status = Iss_Mn34041RegRead ( pObj, cmdArgs );
            break;

        case IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN:
        {
        	Int32 aewbVendor = IssAlg_capt2AGetAEWBVendor();
        	if( aewbVendor !=1)
	            Transplant_DRV_imgsSetEshutter();
	            
            status = Iss_Mn34041UpdateExpGain(pObj, cmdArgs);

            break;
		}
            
        case IOCTL_ISS_SENSOR_PWM_CONFIG:
        {
        	Iss_SensorPwmParm *pPwmParm = (Iss_SensorPwmParm *)cmdArgs;
			if( pPwmParm->Id == ISS_SENSOR_PWM_VIDEO )
			{
        		Iss_Mn34041PwmVideoSet(pPwmParm->period, pPwmParm->duty);
        	}else if( pPwmParm->Id == ISS_SENSOR_PWM_DC ){
				Iss_Mn34041PwmDcSet(pPwmParm->period, pPwmParm->duty);
        	}
			status = FVID2_SOK;
        	break;
		}
		case IOCTL_ISS_SENSOR_UPDATE_FRAMERATE:
        	{
	        	Ptr createArgs;
	        	Iss_CaptFrameRate *framerateParams = (Iss_CaptFrameRate *)cmdArgs;
	        	createArgs = &(framerateParams->FrameRate);
            	status = Iss_Mn34041FrameRateSet(pObj, createArgs, cmdStatusArgs);
        	}
            break;
            
        case IOCTL_ISS_SENSOR_FRAME_RATE_SET:
            status = Iss_Mn34041FrameRateSet(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_ISS_SENSOR_UPDATE_ITT:
            status = Iss_Mn34041UpdateItt(pObj, cmdArgs);
            break;    
        default:
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    /* 
     * unlock handle
     */
    Iss_Mn34041UnlockObj(pObj);

    return status;
}

/* 
 * Create API that gets called when FVID2_create is called
 * 
 * This API does not configure the MN34041 is any way.
 * 
 * This API - validates parameters - allocates driver handle - stores create
 * arguments in its internal data structure.
 * 
 * Later the create arguments will be used when doing I2C communcation with
 * MN34041
 * 
 * drvId - driver ID, must be FVID2_ISS_VID_DEC_MN34041_DRV instanceId - must 
 * be 0 createArgs - create arguments createStatusArgs - create status
 * fdmCbParams - NOT USED, set to NULL
 * 
 * returns NULL in case of any error */
/* ===================================================================
 *  @func     Iss_Mn34041Create                                               
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
Fdrv_Handle Iss_Mn34041Create(UInt32 drvId,
                              UInt32 instanceId,
                              Ptr createArgs,
                              Ptr createStatusArgs,
                              const FVID2_DrvCbParams * fdmCbParams)
{
    Iss_Mn34041Obj *pObj;

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

    if (drvId != FVID2_ISS_SENSOR_MN34041_DRV
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
    pObj = Iss_Mn34041AllocObj();
    if (pObj == NULL)
    {
        sensorCreateStatus->retVal = FVID2_EALLOC;
        return NULL;
    }
#if SET_REG_IN_CREATERFUN

    int i = 0, j = 10000;

    for (i = 0; i < 382; i++)
    {
        mn34_041_setgio(MN34041_ADDR[i], MN34041_REG[i]);

        if (i == 6)
        {
            while (j--)
            {
            }
            j = 100000;
            // usleep(10);
        }

        if (i == 380)
        {
            while (j--)
            {
            }
            // usleep(100);
        }
    }
#endif
    /* 
     * copy parameters to allocate driver handle
     */
    memcpy(&pObj->createArgs, sensorCreateArgs, sizeof(*sensorCreateArgs));

    // Iss_Mn34041ResetRegCache(pObj);

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
 *  @func     Iss_Mn34041Delete                                               
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
Int32 Iss_Mn34041Delete(Fdrv_Handle handle, Ptr deleteArgs)
{
    Iss_Mn34041Obj *pObj = (Iss_Mn34041Obj *) handle;

    if (pObj == NULL)
        return FVID2_EBADARGS;

    /* 
     * free driver handle object
     */
    Iss_Mn34041FreeObj(pObj);

    return FVID2_SOK;
}

/* 
 * System init for MN34041 driver
 * 
 * This API - create semaphore locks needed - registers driver to FVID2
 * sub-system - gets called as part of Iss_deviceInit() */
//static int theTimes = 100;

/* ===================================================================
 *  @func     Transplant_DRV_imgsSetEshutter                                               
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
int Transplant_DRV_imgsSetEshutter()
{
    Int32 status = FVID2_SOK;

    int shtpos;

    // static int theTimes=100;
    status = Iss_Mn34041Lock();
    // if( FVID2_SOK==status)
    if (FVID2_SOK == status)
    {
        shtpos = FRAME_LENGTH - (ti2a_output_params.sensorExposure) / 30;   // (ROW_TIME);
        /* 
         * if(theTimes>0) { theTimes--; } else { Vps_printf (
         * "ti2a_output_params.sensorExposure=%x, shtpos=
         * %x",ti2a_output_params.sensorExposure,shtpos); theTimes=100; } */
        mn34_041_setgio(0x00A1, shtpos);

    }
    Iss_Mn34041Unlock();
    return status;
}

/* ===================================================================
 *  @func     Iss_Mn34041Init                                               
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
int gSensor_init_flag = 0; 
Int32 Iss_Mn34041Init()
{
    Semaphore_Params semParams;

    Int32 status = FVID2_SOK;

    /* 
     * Set to 0's for global object, descriptor memory
     */
    memset(&gIss_Mn34041CommonObj, 0, sizeof(gIss_Mn34041CommonObj));

    /* 
     * Create global MN34041 lock
     */
    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;

    gIss_Mn34041CommonObj.lock = Semaphore_create(1u, &semParams, NULL);

    if (gIss_Mn34041CommonObj.lock == NULL)
        status = FVID2_EALLOC;

    if (status == FVID2_SOK)
    {
#if !(SET_REG_IN_CREATERFUN)

	    int i, j = 10000; 

        /* 
         * Register MN34041 driver with FVID2 sub-system
         */
		if(gSensor_init_flag != 1){ 
			gSensor_init_flag =1;
        for (i = 0; i < 382; i++)
        {
            mn34_041_setgio(MN34041_ADDR[i], MN34041_REG[i]);

            if (i == 6)
            {
                while (j--)
                {
                }
                j = 100000;
                // usleep(10);
            }

            if (i == 380)
            {
                while (j--)
                {
                }
                // usleep(100);
            }
            }
        }
#endif
		Iss_Mn34041PwmVideoSet(83320, 83320);
        Iss_Mn34041PwmDcSet(83320, 41660);
        gIss_Mn34041CommonObj.fvidDrvOps.create =
            (FVID2_DrvCreate) Iss_Mn34041Create;
        gIss_Mn34041CommonObj.fvidDrvOps.delete = Iss_Mn34041Delete;
        gIss_Mn34041CommonObj.fvidDrvOps.control = Iss_Mn34041Control;
        gIss_Mn34041CommonObj.fvidDrvOps.queue = NULL;
        gIss_Mn34041CommonObj.fvidDrvOps.dequeue = NULL;
        gIss_Mn34041CommonObj.fvidDrvOps.processFrames = NULL;
        gIss_Mn34041CommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gIss_Mn34041CommonObj.fvidDrvOps.drvId = FVID2_ISS_SENSOR_MN34041_DRV;

        status = FVID2_registerDriver(&gIss_Mn34041CommonObj.fvidDrvOps);

        if (status != FVID2_SOK)
        {
            /* 
             * Error - free acquired resources
             */
            Semaphore_delete(&gIss_Mn34041CommonObj.lock);
        }
    }

    if (status != FVID2_SOK)
    {
        Vps_printf(" ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__, __LINE__);
    }

    return status;
}

/* 
 * System de-init for MN34041 driver
 * 
 * This API - de-registers driver from FVID2 sub-system - delete's allocated
 * semaphore locks - gets called as part of Iss_deviceDeInit() */
/* ===================================================================
 *  @func     Iss_Mn34041DeInit                                               
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
Int32 Iss_Mn34041DeInit()
{
    /* 
     * Unregister FVID2 driver
     */
    FVID2_unRegisterDriver(&gIss_Mn34041CommonObj.fvidDrvOps);

    /* 
     * Delete semaphore's
     */
    Semaphore_delete(&gIss_Mn34041CommonObj.lock);

    return 0;
}

/* ===================================================================
 *  @func     Iss_Mn34041PinMux                                               
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
Int32 Iss_Mn34041PinMux()
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

    *PINCNTL85 = 0x00000080;
    *PINCNTL86 = 0x00000080;
    *PINCNTL88 = 0x00000080;

    *GIO_INPUT_OUTPUT_DIR &= ~((0x1 << 16) + (0x1 << 17) + (0x1 << 26));
    *GIO_ENABLE_DISABLE_WAKEUP &= ~((0x1 << 16) + (0x1 << 17) + (0x1 << 26));

    return 0;
}

/* 
 * Handle level lock */
/* ===================================================================
 *  @func     Iss_Mn34041LockObj                                               
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
Int32 Iss_Mn34041LockObj(Iss_Mn34041Obj * pObj)
{
    Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);

    return FVID2_SOK;
}

/* 
 * Handle level unlock */
/* ===================================================================
 *  @func     Iss_Mn34041UnlockObj                                               
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
Int32 Iss_Mn34041UnlockObj(Iss_Mn34041Obj * pObj)
{
    Semaphore_post(pObj->lock);

    return FVID2_SOK;
}

/* 
 * Global driver level lock */
/* ===================================================================
 *  @func     Iss_Mn34041Lock                                               
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
Int32 Iss_Mn34041Lock()
{
    Semaphore_pend(gIss_Mn34041CommonObj.lock, BIOS_WAIT_FOREVER);

    return FVID2_SOK;
}

/* 
 * Global driver level unlock */
/* ===================================================================
 *  @func     Iss_Mn34041Unlock                                               
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
Int32 Iss_Mn34041Unlock()
{
    Semaphore_post(gIss_Mn34041CommonObj.lock);

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
 *  @func     Iss_Mn34041AllocObj                                               
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
Iss_Mn34041Obj *Iss_Mn34041AllocObj()
{
    UInt32 handleId;

    Iss_Mn34041Obj *pObj;

    Semaphore_Params semParams;

    UInt32 found = FALSE;

    /* 
     * Take global lock to avoid race condition
     */
    Iss_Mn34041Lock();

    /* 
     * find a unallocated object in pool
     */
    for (handleId = 0; handleId < ISS_DEVICE_MAX_HANDLES; handleId++)
    {

        pObj = &gIss_Mn34041CommonObj.Mn34041Obj[handleId];

        if (pObj->state == ISS_MN34041_OBJ_STATE_UNUSED)
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
            pObj->state = ISS_MN34041_OBJ_STATE_IDLE;
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
                pObj->state = ISS_MN34041_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /* 
     * Release global lock
     */
    Iss_Mn34041Unlock();

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
 *  @func     Iss_Mn34041FreeObj                                               
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
Int32 Iss_Mn34041FreeObj(Iss_Mn34041Obj * pObj)
{
    /* 
     * take global lock
     */
    Iss_Mn34041Lock();

    if (pObj->state != ISS_MN34041_OBJ_STATE_UNUSED)
    {
        /* 
         * mark state as unused
         */
        pObj->state = ISS_MN34041_OBJ_STATE_UNUSED;

        /* 
         * delete object locking semaphore
         */
        Semaphore_delete(&pObj->lock);
    }

    /* 
     * release global lock
     */
    Iss_Mn34041Unlock();

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     pintISIFRegDirectly                                               
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
void pintISIFRegDirectly()
{
    static int tmpIndex = 0x064;

    int i = 0;

    if (tmpIndex > 0)
    {
        tmpIndex--;
    }
    else
    {

        uint32 *pData = NULL;

        // pData=(uint32 *)(&(isp_regs->SYNCEN));
        pData = (uint32 *) (isif_reg);
        if (NULL != pData)
        {
            for (i = 0; i < 127; i++)
            {
                Vps_printf
                    ("Debug : global structure  isp_regs [%d] = 0x%08x ",
                     i, *pData);
                pData++;
            }

        }
        else
        {
            Vps_printf(" ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__,
                       __LINE__);
        }
        int i = 0;

        volatile uint32 *pdata;

        // volatile
        for (i = 0; i < 127; i++)
        {
            pdata = (uint32 *)(ISIF_BASE_ADDRESS + i * 4);
            Vps_printf
                ("Debug : Directly print ISIFReg [%d] = 0x%08x ", i,
                 *pdata);
        }

        tmpIndex = 0x64;

    }

}

UInt32 Iss_Mn34041GainCalc(int gain,int type)
{
	unsigned int Gain1 = 0;
	unsigned int Gain2 = 0;
	unsigned int GainSend = 0;
	
	if( gain > 320000 )
		gain = 320000;

	if( gain >= 320000 )
	{
		Gain1 = 0xC0C0;
		Gain2 = (unsigned int )gain/8;
	
	}
	else if( gain >= 160000 )
	{
		Gain1 = 0xC0C0;
		Gain2 = (unsigned int )gain/8;
	
	}
	else if( gain >= 80000 )
	{
		Gain1 = 0xC0C0;
		Gain2 = (unsigned int )gain/8;
	
	}
	else if( gain >= 40000 )
	{
		Gain1 = 0xC080;
		Gain2 = (unsigned int )gain/4;
	

	}
	else if( gain >= 20000 )
	{
		Gain1 = 0x8080;
		Gain2 = (unsigned int )gain/2;
		

	}
	else if( gain >= 10000 )
	{
		Gain1 = 0x0080;
		Gain2 = (unsigned int )gain/1;
	}

	Gain2 = (((log((double)Gain2/(double)10000.0)/log((double)10.0))*(double)20.0f)/(double)0.09375f)+0x80;

	if( type == 0 )
		GainSend = Gain1;
	else
		GainSend = Gain2;

	//Vps_printf("Debug : type%d Gain = GainSend = 0x%X\n",type,GainSend);
	

	return (UInt32)GainSend;
}

/* 
 * Update exposure and gain value from the 2A */
/* ===================================================================
 *  @func     Iss_Mn34041UpdateExpGain                                               
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
Int32 Iss_Mn34041UpdateExpGain(Iss_Mn34041Obj * pObj, Ptr createArgs)
{
    Int32 status = FVID2_SOK;

    Int16 g_int = 0;

	Int32 aewbVendor = IssAlg_capt2AGetAEWBVendor();
    Int16 regAddr = 0x0020;
    Int16 regAddrD = 0x0021;

    status = Iss_Mn34041Lock();
    // gMn34041_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();

    if (FVID2_SOK == status)
    {
		if( aewbVendor == 1 )
		{
			if (ti2a_output_params.mask&(0x1<<0))
			{
				g_int = Iss_Mn34041GainCalc(ti2a_output_params.sensorGain,0);
				mn34_041_setgio(regAddr, (g_int&0xFFFF));	
				g_int = Iss_Mn34041GainCalc(ti2a_output_params.sensorGain,1);
				mn34_041_setgio(regAddrD, (g_int&0xFFFF));
			}
			
			if (ti2a_output_params.mask&(0x1<<1))
			{
				int shtpos = 0;
				
				shtpos = FrameTime - (ti2a_output_params.sensorExposure);   // (ROW_TIME);
				if(shtpos < 0)
					shtpos = 0;
					
		        mn34_041_setgio(0x00A1, shtpos);
		          //{ Vps_printf (
		          //"ti2a_output_params.sensorExposure=%d, shtpos=
		          //%d",ti2a_output_params.sensorExposure,shtpos); } 
		        
			}

		}else{
        	g_int = Mn34041_GainTableMap(ti2a_output_params.sensorGain);
	        mn34_041_setgio(regAddr, g_int);
        	g_int = Mn34041_DGainTableMap(ti2a_output_params.sensorGain);
	        mn34_041_setgio(regAddr + 1, g_int);
	    }
    }
    Iss_Mn34041Unlock();

    return status;
}

#define DM_TIMER_LOAD_MIN		0xFFFFFFFE
inline Uint32 TimerBaseAddr( Int32 timerId )
{
	Uint32 timerAddr[] = {
		0x4802E000,// 1
		0x48040000,// 2
		0x48042000,// 3
		0x48044000,// 4
		0x48046000,// 5
		0x48048000,// 6
		0x4804A000,// 7
		0x481C1000 // 8
	};
	return timerAddr[(timerId-1)];
}
Uint32 Iss_Mn34041PwmVideoSet(Uint32 load_value, Uint32 match_value)
{
	Uint32	TimerID = 6;
	Uint32  *TimerLoadReg = (Uint32  *)(TimerBaseAddr(TimerID)+0x40);
	Uint32  *TimerMatchReg = (Uint32  *)(TimerBaseAddr(TimerID)+0x4C);

	*TimerLoadReg = DM_TIMER_LOAD_MIN-load_value;
	*TimerMatchReg = DM_TIMER_LOAD_MIN-(load_value-match_value);

	return 0;
}


Uint32 Iss_Mn34041PwmDcSet(Uint32 load_value, Uint32 match_value)
{
	Uint32	TimerID = 7;
	Uint32  *TimerLoadReg = (Uint32  *)(TimerBaseAddr(TimerID)+0x40);
	Uint32  *TimerMatchReg = (Uint32  *)(TimerBaseAddr(TimerID)+0x4C);

	*TimerLoadReg = DM_TIMER_LOAD_MIN-load_value;
	*TimerMatchReg = DM_TIMER_LOAD_MIN-(load_value-match_value);
	
	return 0;
}
typedef struct {
    UInt32 i2cInstId;

    UInt32 numArgs;
    UInt8 regAddr[0x200];
    UInt8 regValue[0x200];
    UInt32 numRegs;
} I2c_Ctrl;
static I2c_Ctrl gMn34041_I2c_ctrl;

Int32 Iss_Mn34041ReciverReSet(void)
{
	Int32 devAddr = (0x2D);
	Int32 status;
	int i=0;
	gMn34041_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();

	for( i =0; i<10;i++)
		gMn34041_I2c_ctrl.regAddr[i] = i;
		
	status =Iss_deviceRead8(gMn34041_I2c_ctrl.i2cInstId, devAddr, gMn34041_I2c_ctrl.regAddr,
                              gMn34041_I2c_ctrl.regValue, 1);
	Vps_printf("status = %d\n",status);
	for( i =0; i<10;i++)
    Vps_printf("gMn34041_I2c_ctrl.regValue[%d] = %x \n",i,gMn34041_I2c_ctrl.regValue[i]);  
	
	return status;
}

Int32 Iss_Mn34041FrameRateSet(Iss_Mn34041Obj * pObj, Ptr createArgs,
                              Ptr cmdStatusArgs)
{
    Int32 status = FVID2_SOK;
	int shtpos = 0;
    Int32 framerate = *(Int32 *) createArgs;

	Vps_printf("framerate1 = %d \n",framerate);
    /** take global lock **/
    Iss_Mn34041Lock();
    
    if (framerate == 60)
    {
    	//FrameTime = 0x465;
		//mn34_041_setgio(0x1, 0x30);
		mn34_041_setgio(0x1, 0x30);
		shtpos = FRAME_LENGTH - (ti2a_output_params.sensorExposure) / 15;   // (ROW_TIME);
        /* 
         * if(theTimes>0) { theTimes--; } else { Vps_printf (
         * "ti2a_output_params.sensorExposure=%x, shtpos=
         * %x",ti2a_output_params.sensorExposure,shtpos); theTimes=100; } */
        mn34_041_setgio(0x00A1, shtpos);
        *(Int32 *) cmdStatusArgs = FrameTime;
        
    }
    else if (framerate == 50)
    {

    	//FrameTime = 0x546;
		//Iss_Mn34041ReciverReSet();
		//mn34_041_setgio(0x1, 0x28);
        *(Int32 *) cmdStatusArgs = FrameTime;

    }
    else if (framerate == 30)
    {
		mn34_041_setgio(0x1, 0x18);
	    shtpos = FRAME_LENGTH - (ti2a_output_params.sensorExposure) / 30;   // (ROW_TIME);
        /* 
         * if(theTimes>0) { theTimes--; } else { Vps_printf (
         * "ti2a_output_params.sensorExposure=%x, shtpos=
         * %x",ti2a_output_params.sensorExposure,shtpos); theTimes=100; } */
        mn34_041_setgio(0x00A1, shtpos);
        *(Int32 *) cmdStatusArgs = FrameTime;
    }
    else if (framerate == 25)
    {
        *(Int32 *) cmdStatusArgs = FrameTime;
    }
    else
    {
        status = FVID2_EFAIL;
        return status;
    }

    Vps_printf("framerate2 = %d \n",framerate);

    /** release global lock **/
    Iss_Mn34041Unlock();

    return status;
}

Int32 Iss_Mn34041UpdateItt(Iss_Mn34041Obj * pObj,
                           Itt_RegisterParams * gItt_RegisterParams)
{
    int status = 0;

    if (gItt_RegisterParams->Control == 1)
    {
    	mn34_041_setgio( gItt_RegisterParams->regAddr,gItt_RegisterParams->regValue);
    	Vps_printf("addr = %x val=%x \n",gItt_RegisterParams->regAddr,gItt_RegisterParams->regValue);
       
    }
    else if (gItt_RegisterParams->Control == 0)
    {
        
        gItt_RegisterParams->regValue = 0;
    }
    return status;
}


/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/devices/imx136_LIMG/src/issdrv_imx136Priv.h>
#include <ti/psp/devices/imx136_LIMG/issdrv_imx136_config.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif_reg.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif.h>
#include <ti/psp/devices/imx136_LIMG/src/imx136_reg_setting.h>
#define IMGS_SENSOR_LINEWIDTH		(1125)
#define IMGS_SENSOR_LINEWIDTH_3MP	(1608)

#ifdef IMX136_DATA_OP_LVDS324
#define  IMX136_LVDS324
#endif

#ifdef IMX136_LVDS324
#include <ti/psp/devices/src/vpsdrv_devicePriv.h>
#include <math.h>

static I2c_Ctrl   gImx136Lvds_I2c_ctrl;
extern Vps_DeviceObj gVps_deviceObj;
extern ti2aControlParams_t gTi2aControlParams;
#endif
Transplant_DRV_imgsFrameTime gFrameTime;
int imxWDRmode ;
int gFramerate = 30;

int InitRegArrary()
{
	int i=0;

#ifdef  IMX136_SPI_TEST
	do{
		imx136_setgio(IMX136_SETTING[8],IMX136_SETTING[9]);
	}while(1);
#endif

	//for (i = 0; i < IMX136_REG_COUNT; i = i+2)
	for (i = 0; i < 375*2; i = i+2)
	{
		imx136_setgio(IMX136_SETTING[i],IMX136_SETTING[i+1]);
	}

	imx136_setgio(0x0200,0); //enable the streaming
	return 0;
}


extern isif_regs_ovly		isif_reg;
/* Global object storing all information related to all
  IMX136 driver handles */
static Iss_Imx136CommonObj 	gIss_Imx136CommonObj;
extern ti2a_output 			ti2a_output_params;

void UpdateInitReg(int *indx);

void imx136_setcs()
{
	*GIO_WRITE_DATA |= 0x1 << 16;
}

void imx136_clrcs()
{
	*GIO_CLEAR_DATA |= 0x1 << 16;
}

void imx136_setclk()
{
	*GIO_WRITE_DATA |= 0x1 << 17;
}

void imx136_clrclk()
{
	*GIO_CLEAR_DATA |= 0x1 << 17;
}

void imx136_setdata()
{
	*GIO_WRITE_DATA |= 0x1 << 26;
}

void imx136_clrdata()
{
	*GIO_CLEAR_DATA |= 0x1 << 26;
}

void imx136_setgio(unsigned short addr, unsigned short reg)
{
	int i;
	unsigned char imx136_value;
	unsigned char spi_chipid;
	unsigned char page_addr;

	spi_chipid = (unsigned char)(addr >> 8);
	page_addr = (unsigned char)(addr&0xFF);
	imx136_value =( unsigned char) reg;

	imx136_clrclk();
	imx136_clrcs();
	/*spi_chipid*/
	for (i=0; i<8; i++) {
		if (spi_chipid & 0x01)
			imx136_setdata();
		else
			imx136_clrdata();
		spi_chipid = spi_chipid >> 1;
		imx136_setclk();
		imx136_clrclk();
	}
	/*page address*/
	for (i=0; i<8; i++) {
		if (page_addr & 0x01)
			imx136_setdata();
		else
			imx136_clrdata();
		page_addr = page_addr >> 1;

		imx136_setclk();
		imx136_clrclk();
	}
	/*data*/
	for (i=0; i<8; i++) {
		if (imx136_value & 0x01)
			imx136_setdata();
		else
			imx136_clrdata();
		imx136_value = imx136_value >> 1;

		imx136_setclk();
		imx136_clrclk();
	}

  // CS high
    imx136_setcs();
}

Int32 Iss_Imx136_HDR_Enable(int enable)
{
    if(enable == 2)
    {
        imxWDRmode = 1;
        Vps_printf("IMX136 WDR enabled\n");
		imx136_setgio(0x020a,0x0);
        imx136_setgio(0x20C,0x02);  //0: Normal mode 2:WDR mode
        imx136_setgio(0x20F,0x05);  //Compressed Output enable 1:enable
        imx136_setgio(0x210,0x00);  //Fixed
        imx136_setgio(0x212,0x2D);   //Fixed
        imx136_setgio(0x220,0x1F);  //SHS1
        imx136_setgio(0x221,0x04);  //SHS1
        imx136_setgio(0x222,0x00);  //SHS1  002BD = 701 lines
        imx136_setgio(0x223,0x05);  //SHS2
        imx136_setgio(0x224,0x00);  //SHS2
        imx136_setgio(0x265,0x00);  //Fixed
        imx136_setgio(0x284,0x10);  //Midpoint potential setting
        imx136_setgio(0x286,0x10);  //Fixed
        imx136_setgio(0x2CF,0xE1);   //Fixed
        imx136_setgio(0x2D0,0x30);   //Fixed
        imx136_setgio(0x2D2,0xC4);  //Fixed
        imx136_setgio(0x2D3,0x01);   //Fixed


    } else {
     imxWDRmode = 0;
        Vps_printf("IMX136 WDR disabled\n");
		imx136_setgio(0x020a,0xf0);
        imx136_setgio(0x20C,0x00);
        imx136_setgio(0x20F,0x01);
        imx136_setgio(0x210,0x01);
        imx136_setgio(0x212,0x00);
        imx136_setgio(0x220,0x0);
        imx136_setgio(0x221,0x00);
        imx136_setgio(0x222,0x00);
        imx136_setgio(0x223,0x00);
        imx136_setgio(0x224,0x00);
        imx136_setgio(0x265,0x20);
        imx136_setgio(0x284,0x0);
        imx136_setgio(0x286,0x1);
        imx136_setgio(0x2CF,0xd1);
        imx136_setgio(0x2D0,0x1b);
        imx136_setgio(0x2D2,0x5f);
        imx136_setgio(0x2D3,0x0);
    }
    return 1;
}

#include <ti/psp/iss/drivers/capture/src/issdrv_capturePriv.h>
extern Iss_CaptCommonObj gIss_captCommonObj;
#define __raw_readl(a)      (*(volatile unsigned int *)(a))
#define __raw_writel(v, a)  (*(volatile unsigned int *)(a) = (v))
int DRV_imgsSetRegs();

Int32 Iss_imx136_Reset(Iss_Imx136Obj* pObj)
{
    Int32 status = FVID2_SOK;

#if 1
	UInt32  add, val;
	//..GPIO2 18,...sensor
    /* GPIO0 base 0x48032000 GPIO1 base 0x4804C000 GPIO2 base 0x481AC000 GPIO3 base 0x481AE000 */
    //GPIO2[] group
    add= 0x481ac134;                     //GPIO_OE Output Enable Register
    val = __raw_readl(add);
    val &=~(1<<18);                     //GP2_18-CAM_RST output
    __raw_writel(val, add);

    /* reset sensor */
	Vps_printf("RESET IMX136");
    __raw_writel((1<<18), 0x481ac190);  //output low
    Task_sleep(900);
    __raw_writel((1<<18), 0x481ac194);  //output high
    Task_sleep(1000);
#endif

    //reconfigure all of the Register after 1 sec
	Vps_printf("configure IMX136 register");
	DRV_imgsSetRegs();
	Vps_printf("configure IMX136 register end");

	Semaphore_post(gIss_captCommonObj.ovflWait);
    return status;
}

Int32 Iss_Imx136FrameRateSet(Iss_Imx136Obj * pObj,  Int32 framerate)
{
	Int32 status = FVID2_SOK;
	gFramerate = framerate;

#if defined(WDR_ON)
	if(framerate> 30)
	{
		framerate = 30;
	}
#endif

    unsigned short regAddr[3],regData[3];
	//Frame select = 1/0 HMAX 2200/4400
    regAddr[0]=0x0209; regAddr[1]=0x021b; regAddr[2]=0x021c;

	Vps_printf("IMX136 FrameRateSet %d", framerate);

    if (framerate == 60) {
		gTi2aControlParams.maxExposure=16667;
		regData[0]=1; regData[1]=0x98; regData[2]=0x08;
    } else if (framerate == 30) {
		gTi2aControlParams.maxExposure=33333;
        regData[0]=2; regData[1]=0x30; regData[2]=0x11;
    } else if (framerate == 15) {
		gTi2aControlParams.maxExposure=40000;
        regData[0]=2; regData[1]=0x60; regData[2]=0x22;
	}

    Iss_Imx136Lock(); int i=0;
    for(i=0;i<3;i++) {
        imx136_setgio(regAddr[i], regData[i]);
    }
    Iss_Imx136Unlock();

    return status;
}
/* Control API that gets called when FVID2_control is called

  This API does handle level semaphore locking

  handle - Imx136 driver handle
  cmd - command
  cmdArgs - command arguments
  cmdStatusArgs - command status

  returns error in case of
  - illegal parameters
  - I2C command RX/TX error
*/
Int32 Iss_Imx136Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
	Iss_Imx136Obj *pObj = ( Iss_Imx136Obj * ) handle;
    Int32 status = FVID2_SOK;
	//int  loopNum=1;

	if ( pObj == NULL )
		return FVID2_EBADARGS;

	Int32 enable = *(Int32 *) cmdArgs;
    Iss_CaptFrameRate *framerateParams = (Iss_CaptFrameRate *)cmdArgs;

    /*
     * lock handle
     */
    Iss_Imx136LockObj ( pObj );

	switch ( cmd )
    {
        case FVID2_START:
            break;

        case FVID2_STOP:
            break;

        case IOCTL_ISS_SENSOR_GET_CHIP_ID:
            break;

        case IOCTL_ISS_SENSOR_RESET:
            Iss_imx136_Reset(pObj);
            break;

        case IOCTL_ISS_SENSOR_REG_WRITE:
            break;

        case IOCTL_ISS_SENSOR_REG_READ:
            break;

		case IOCTL_ISS_SENSOR_WDR_MODE:
            Iss_Imx136_HDR_Enable(enable);
            break;
        case IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN:
            status = Iss_Imx136UpdateExpGain ( pObj, cmdArgs);
            break;
		case IOCTL_ISS_SENSOR_UPDATE_FRAMERATE:
            status = Iss_Imx136FrameRateSet(pObj, framerateParams->FrameRate);
			break;

        default:
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    /*
     * unlock handle
     */
    Iss_Imx136UnlockObj ( pObj );

    return status;
}


/*
  Create API that gets called when FVID2_create is called

  This API does not configure the Imx136 is any way.

  This API
  - validates parameters
  - allocates driver handle
  - stores create arguments in its internal data structure.

  Later the create arguments will be used when doing I2C communcation with
  Imx136

  drvId - driver ID, must be FVID2_ISS_VID_DEC_Imx136_DRV
  instanceId - must be 0
  createArgs - create arguments
  createStatusArgs - create status
  fdmCbParams  - NOT USED, set to NULL

  returns NULL in case of any error
*/
Fdrv_Handle Iss_Imx136Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams )
{
	Iss_Imx136Obj *pObj;
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

    if ( drvId != FVID2_ISS_SENSOR_IMX136_DRV
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
    pObj = Iss_Imx136AllocObj (  );
    if ( pObj == NULL )
    {
        sensorCreateStatus->retVal = FVID2_EALLOC;
        return NULL;
    }

    /*
     * copy parameters to allocate driver handle
     */
    memcpy ( &pObj->createArgs, sensorCreateArgs,
             sizeof ( *sensorCreateArgs ) );

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
Int32 Iss_Imx136Delete ( Fdrv_Handle handle, Ptr deleteArgs )
{
    Iss_Imx136Obj *pObj = ( Iss_Imx136Obj * ) handle;

    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /*
     * free driver handle object
     */
    Iss_Imx136FreeObj ( pObj );

    return FVID2_SOK;
}



/*
  System init for Imx136 driver

  This API
  - create semaphore locks needed
  - registers driver to FVID2 sub-system
  - gets called as part of Iss_deviceInit()
*/
//static int theTimes=100;




int DRV_imgsSetRegs()
{
	int j = 0;
    /*************lvds324 I2C configuration *********************/
#ifdef IMX136_LVDS324
	int delays=100000;
	int status = FVID2_SOK;
	unsigned char lvdsRegAddr[12];
	unsigned char lvdsRegValue[12];
	//int delays = 100000;

	gVps_deviceObj.i2cDebugEnable=1;
	gImx136Lvds_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();

	lvdsRegAddr[0]=0x00;
	status = Iss_deviceRead8(gImx136Lvds_I2c_ctrl.i2cInstId,0x2d,&lvdsRegAddr[0],&lvdsRegValue[0], 1);

	lvdsRegAddr[0]=0x09; lvdsRegValue[0]=0x21; /*for sony wide channel 12bpp*/
	lvdsRegAddr[1]=0x0A; lvdsRegValue[1]=0x72; /*0x62--->SCLK:58MHz~75MHz;OUTCLK:116~150MHz*/
										      /*0x61--->SCLK:31MHz~60MHz;OUTCLK:62~120MHz*/
	delays = 100000;
	while(delays--){}

	status = Iss_deviceWrite8(gImx136Lvds_I2c_ctrl.i2cInstId,0x2d,&lvdsRegAddr[0],&lvdsRegValue[0], 1);

	delays = 100000;
	while(delays--){}
	status = Iss_deviceWrite8(gImx136Lvds_I2c_ctrl.i2cInstId,0x2d,&lvdsRegAddr[1],&lvdsRegValue[1], 1);
	delays = 100000;
	while(delays--){}

	status = Iss_deviceRead8(gImx136Lvds_I2c_ctrl.i2cInstId,0x2d,&lvdsRegAddr[0],&lvdsRegValue[1], 1);
	delays = 100000;
	while(delays--){}

#endif
	j = InitRegArrary();

	Vps_printf("With IMX136 SETTING_-_-,%s,%d %d \n",__FUNCTION__,__LINE__, j);

	return status;

}

Int32 Iss_Imx136Init (  )
{
    Semaphore_Params semParams;
    Int32 status = FVID2_SOK;

    /*
     * Set to 0's for global object, descriptor memory
     */
    memset ( &gIss_Imx136CommonObj, 0, sizeof ( gIss_Imx136CommonObj ) );

    /*
     * Create global Imx136 lock
     */
    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    gIss_Imx136CommonObj.lock = Semaphore_create ( 1u, &semParams, NULL );

    if ( gIss_Imx136CommonObj.lock == NULL )
        status = FVID2_EALLOC;

    if ( status == FVID2_SOK )
    {
        DRV_imgsSetRegs();

        gIss_Imx136CommonObj.fvidDrvOps.create = ( FVID2_DrvCreate ) Iss_Imx136Create;
        gIss_Imx136CommonObj.fvidDrvOps.delete = Iss_Imx136Delete;
        gIss_Imx136CommonObj.fvidDrvOps.control = Iss_Imx136Control;
        gIss_Imx136CommonObj.fvidDrvOps.queue = NULL;
        gIss_Imx136CommonObj.fvidDrvOps.dequeue = NULL;
        gIss_Imx136CommonObj.fvidDrvOps.processFrames = NULL;
        gIss_Imx136CommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gIss_Imx136CommonObj.fvidDrvOps.drvId = FVID2_ISS_SENSOR_IMX136_DRV;

        status = FVID2_registerDriver ( &gIss_Imx136CommonObj.fvidDrvOps );

        if ( status != FVID2_SOK )
        {
            /*
             * Error - free acquired resources
             */
            Semaphore_delete ( &gIss_Imx136CommonObj.lock );
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
  System de-init for Imx136 driver

  This API
  - de-registers driver from FVID2 sub-system
  - delete's allocated semaphore locks
  - gets called as part of Iss_deviceDeInit()
*/
Int32 Iss_Imx136DeInit (  )
{
    /*
     * Unregister FVID2 driver
     */
    FVID2_unRegisterDriver ( &gIss_Imx136CommonObj.fvidDrvOps );

    /*
     * Delete semaphore's
     */
    Semaphore_delete ( &gIss_Imx136CommonObj.lock );

    return 0;
}

Int32 Iss_Imx136PinMux (  )
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

    #if defined(TI_8107_BUILD)
    *PINCNTL74 = 0xE0040;   /* i2c2_scl_mux0 */
    *PINCNTL75 = 0xE0040;   /* i2c2_sda_mux0 */
    *PINCNTL135 = 0xE0040;   /* i2c2_scl_mux0 */
    *PINCNTL136 = 0xE0040;   /* i2c2_sda_mux0 */
    #endif
    #if defined(TI_814X_BUILD)
    *PINCNTL74 = 0x00020;
    *PINCNTL75 = 0x00020;
    #endif

    *PINCNTL85 = 0x00000080;
    *PINCNTL86 = 0x00000080;
    *PINCNTL88 = 0x00000080;

    *GIO_INPUT_OUTPUT_DIR &= ~((0x1 << 16) + (0x1 << 17) + (0x1 << 26));
    *GIO_ENABLE_DISABLE_WAKEUP &= ~((0x1 << 16) + (0x1 << 17) + (0x1 << 26));

    //Vps_printf (" spi gio enable in %s\n",__FUNCTION__);
	return 0;
}
/*
  Handle level lock
*/
Int32 Iss_Imx136LockObj ( Iss_Imx136Obj * pObj )
{
    Semaphore_pend ( pObj->lock, BIOS_WAIT_FOREVER );
    return FVID2_SOK;
}

/*
  Handle level unlock
*/
Int32 Iss_Imx136UnlockObj ( Iss_Imx136Obj * pObj )
{
    Semaphore_post ( pObj->lock );
    return FVID2_SOK;
}

/*
  Global driver level lock
*/
Int32 Iss_Imx136Lock (  )
{
    Semaphore_pend ( gIss_Imx136CommonObj.lock, BIOS_WAIT_FOREVER );
    return FVID2_SOK;
}

/*
  Global driver level unlock
*/
Int32 Iss_Imx136Unlock (  )
{
    Semaphore_post ( gIss_Imx136CommonObj.lock );
    return FVID2_SOK;
}

/*
  Allocate driver object

  Searches in list of driver handles and allocate's a 'NOT IN USE' handle
  Also create's handle level semaphore lock

  return NULL in case handle could not be allocated
*/
Iss_Imx136Obj *Iss_Imx136AllocObj (  )
{
    UInt32 handleId;
    Iss_Imx136Obj *pObj;
    Semaphore_Params semParams;
    UInt32 found = FALSE;

    /*
     * Take global lock to avoid race condition
     */
    Iss_Imx136Lock (  );

    /*
     * find a unallocated object in pool
     */
    for ( handleId = 0; handleId < ISS_DEVICE_MAX_HANDLES; handleId++ )
    {
        pObj = &gIss_Imx136CommonObj.Imx136Obj[handleId];
        if ( pObj->state == ISS_IMX136_OBJ_STATE_UNUSED )
        {
            /* free object found */

            /* init to 0's */
            memset ( pObj, 0, sizeof ( *pObj ) );

            /* init state and handle ID */
            pObj->state = ISS_IMX136_OBJ_STATE_IDLE;
            pObj->handleId = handleId;

            /* create driver object specific semaphore lock */
            Semaphore_Params_init ( &semParams );
            semParams.mode = Semaphore_Mode_BINARY;
            pObj->lock = Semaphore_create ( 1u, &semParams, NULL );
            found = TRUE;

            if ( pObj->lock == NULL )
            {
                /* Error - release object */
                found = FALSE;
                pObj->state = ISS_IMX136_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /* Release global lock */
    Iss_Imx136Unlock (  );

    if ( found )
        return pObj;    /* Free object found return it */

    /* free object not found, return NULL */
    return NULL;
}

/*
  De-Allocate driver object

  Marks handle as 'NOT IN USE'
  Also delete's handle level semaphore lock
*/
Int32 Iss_Imx136FreeObj ( Iss_Imx136Obj * pObj )
{
    /* take global lock */
    Iss_Imx136Lock (  );

    if ( pObj->state != ISS_IMX136_OBJ_STATE_UNUSED )
    {
        /* mark state as unused */
        pObj->state = ISS_IMX136_OBJ_STATE_UNUSED;

        /* delete object locking semaphore */
        Semaphore_delete ( &pObj->lock );
    }

    /* release global lock */
    Iss_Imx136Unlock( );

    return FVID2_SOK;
}

void pintISIFRegDirectly()
{
	static int tmpIndex=0x064;
    int i=0;
    if (tmpIndex>0) {
		tmpIndex--;
	} else {
       	uint32 *pData = NULL;
       	//pData=(uint32 *)(&(isp_regs->SYNCEN));
		pData=(uint32 *)(isif_reg);
       	if(NULL!=pData)
		{
			for(i=0;i<127;i++)
			{
				Vps_printf ("global structure  isp_regs [%d] = 0x%08x ",i,*pData);
				pData++;
			}
        } else {
			Vps_printf ( " ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__, __LINE__ );
        }
		int i=0;
		volatile uint32 *pdata;

		//volatile
		for(i=0;i<127;i++)
		{
			pdata= (uint32 *)(ISIF_BASE_ADDRESS+i*4);
			Vps_printf ("Debug : Directly print ISIFReg [%d] = 0x%08x ",i, *pdata);
		}
		tmpIndex=0x64;
	}
}


int Transplant_DRV_imgsCalcSW(int exposureTimeInUsecs)
{

	int Lw;
	Transplant_DRV_imgsFrameTime *pFrame = &gFrameTime;

	Lw = IMGS_SENSOR_LINEWIDTH;
	//pFrame->SW = (int)(((double)exposureTimeInUsecs*(double)Lw)/((double)33333) );
	//pFrame->SW = (int)(((double)exposureTimeInUsecs*0.8*(double)Lw)/((double)33333) );

	if(gFramerate == 60)
		pFrame->SW = (int)(((double)exposureTimeInUsecs*(double)Lw)/((double)16666) );
	else
		pFrame->SW = (int)(((double)exposureTimeInUsecs*(double)Lw)/((double)33333) );

	if(pFrame->SW<1)
	{
		pFrame->SW = 1;
	}

	return pFrame->SW;

}

int Transplant_DRV_imgsSetEshutter(Uint32 eshutterInUsec, int setRegDirect)
{
	int	status = FVID2_SOK;
	//Uint32 SUB_Control = 0;
	Uint32	SetValue,tempdata;
	Uint8	Send_HH,Send_H,Send_L;
	Transplant_DRV_imgsFrameTime *pFrame = &gFrameTime;
	//int count=0;
	//Uint8 regset[5];

	if(eshutterInUsec==0){
		Vps_printf("STError:\t%d\n",eshutterInUsec);
		return status;
	}

	Transplant_DRV_imgsCalcSW(eshutterInUsec);
	SetValue = pFrame->SW;
	/*************************************************
	if(SetValue<IMGS_SENSOR_LINEWIDTH){
		SUB_Control = IMGS_SENSOR_LINEWIDTH - SetValue;
	}else{
		SUB_Control = IMGS_SENSOR_LINEWIDTH - SetValue;
	}
	Send_HH=(Uint8)(SUB_Control>>16);
	Send_H=(Uint8)((SUB_Control>>8)&0x00FF);
	Send_L=(Uint8)(SUB_Control & 0x00FF);
		imx136_setgio(0x220,Send_L);
		imx136_setgio(0x221,Send_H);
		imx136_setgio(0x222,Send_HH);
	* long exposure == > 1024*128/1152/30= 3.8 sec !!*/
	if(SetValue<IMGS_SENSOR_LINEWIDTH){
		tempdata = IMGS_SENSOR_LINEWIDTH  - SetValue;
	}
	else
	{
		//tempdata = SetValue;
		tempdata = 0;
	}
	Send_HH=(Uint8)(tempdata>>16);
	Send_H=(Uint8)((tempdata>>8)&0x00FF);
	Send_L=(Uint8)(tempdata & 0x00FF);
        if(imxWDRmode == 0)
        {
            imx136_setgio(0x220,Send_L);
            imx136_setgio(0x221,Send_H);
            imx136_setgio(0x222,Send_HH);
        } else //(imxWDRmode == 1)
        {
            imx136_setgio(0x223,Send_L);
            imx136_setgio(0x224,Send_H);
            imx136_setgio(0x225,Send_HH);
            /*  Long integration 80H to 736H (SHS2 = 965 to 5d)
             Short integration 5H to 46H (SHS1 1115 to 1055d) */

        //Vps_printf("Transplant_DRV_imgsSetEshutter %d %x %d %d %d %d\n",
        //      ti2a_output_params.sensorExposure, SetValue , tempdata , Send_HH, Send_H, Send_L);
            SetValue = Transplant_DRV_imgsCalcSW(ti2a_output_params.sensorExposure/16);
            if(SetValue<IMGS_SENSOR_LINEWIDTH){
                tempdata = IMGS_SENSOR_LINEWIDTH  - SetValue;
            } else {
                //tempdata = SetValue;
                tempdata =0;
            }
      //  Vps_printf("Transplant_DRV_imgsSetEshutter short: %d %x\n", ti2a_output_params.sensorExposure/16, tempdata);
            Send_HH=(Uint8)(tempdata>>16);
            Send_H=(Uint8)((tempdata>>8)&0x00FF);
            Send_L=(Uint8)(tempdata& 0x00FF);

            imx136_setgio(0x220,Send_L);
            imx136_setgio(0x221,Send_H);
            imx136_setgio(0x222,Send_HH);

        }


	return status;
}



int Transplant_DRV_imgsSetAgain(int again, int setRegDirect)
{
	int	status = FVID2_SOK;
	//int count=0;
	Uint8 regset[5];
	//int SetIdx;
	//unsigned long tmp;
	//unsigned short SetVal1;
	unsigned short SetVal2;
	double temp;

	//SetVal2 = (200*log((double)again));
	//temp = log((((double)again)/1000))/log(10);
	temp = (200*log10(((double)again)/1000.0));
	if (imxWDRmode == 1)
		temp = temp - 45;

	SetVal2=(unsigned short)temp;

	if (SetVal2>484)
	{
		SetVal2 = 484;
	}

	//Vps_printf("Again reg value : 0x%x, again : %d \n",SetVal2,again);
	regset[0]=SetVal2&0xff;
	regset[1]=(SetVal2>>8)&0x01;

	//register hold
	imx136_setgio(0x201,1);
	imx136_setgio(0x214,regset[0]);
	imx136_setgio(0x215,regset[1]);
	//register hold
	imx136_setgio(0x201,0);
	return status;
}


/*
  Update exposure and gain value from the 2A
*/
Int32 Iss_Imx136UpdateExpGain ( Iss_Imx136Obj * pObj, Ptr createArgs )
{
	Int32 status = FVID2_SOK;

	status=Iss_Imx136Lock (  );
	//gImx136_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
	if( FVID2_SOK==status)
	{
		Transplant_DRV_imgsSetAgain(ti2a_output_params.sensorGain,1);
		Transplant_DRV_imgsSetEshutter(ti2a_output_params.sensorExposure,0);
		//g_int = Imx136_GainTableMap(ti2a_output_params.sensorGain);
		//imx136_setgio(regAddr,g_int);
	}
	Iss_Imx136Unlock (  );
	return status;
}

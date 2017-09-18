/** ==================================================================
 *  @file   issdrv_imx122Api.c
 *
 *  @path   /ti/psp/devices/imx122/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/devices/imx122/src/issdrv_imx122Priv.h>
#include <ti/psp/devices/imx122/issdrv_imx122_config.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif_reg.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_alg2APriv.h>
#include <math.h>

#define SET_REG_IN_CREATERFUN      0

extern isif_regs_ovly isif_reg;

/* Global object storing all information related to all IMX122 driver
 * handles */
static Iss_Imx122CommonObj gIss_Imx122CommonObj;

extern ti2a_output ti2a_output_params;

/* ===================================================================
 *  @func     imx122_setcs
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
void imx122_setcs()
{
    //*(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0194) |= 0x1 << 16;
	*(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0194) = 0x1 << 16;
}

/* ===================================================================
 *  @func     imx122_clrcs
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
void imx122_clrcs()
{
    //*(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0190) |= 0x1 << 16;
	*(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0190) = 0x1 << 16;
}

/* ===================================================================
 *  @func     imx122_setclk
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
void imx122_setclk()
{
    //*(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0194) |= 0x1 << 17;
	*(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0194) = 0x1 << 17;
}

/* ===================================================================
 *  @func     imx122_clrclk
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
void imx122_clrclk()
{
    //*(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0190) |= 0x1 << 17;
	*(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0190) = 0x1 << 17;
}

/* ===================================================================
 *  @func     imx122_setdata
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
void imx122_setdata()
{
    //*(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0194) |= 0x1 << 26;
	*(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0194) = 0x1 << 26;
}

/* ===================================================================
 *  @func     imx122_clrdata
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
void imx122_clrdata()
{
    //*(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0190) |= 0x1 << 26;
	*(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0190) = 0x1 << 26;
}

/* ===================================================================
 *  @func     imx122_reset
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
void imx122_reset()
{

    //*(volatile UInt32*)(GP3_MODULE_BASE_ADDR + 0x190) |= 0x1 << 17;
	*(volatile UInt32*)(GP3_MODULE_BASE_ADDR + 0x190) = 0x1 << 17;


	Task_sleep(100);


	//*(volatile UInt32*)(GP3_MODULE_BASE_ADDR + 0x194) |= 0x1 << 17;
	*(volatile UInt32*)(GP3_MODULE_BASE_ADDR + 0x194) = 0x1 << 17;

	Task_sleep(10);

}





/* ===================================================================
 *  @func     imx122_getdata
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
	/* SPI_SDI is not connected to Image sensor so we can't read data from it */
#if 1

int imx122_getdata()
{
	UInt32 gpio, value;

	value = *(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0138);
	gpio = ((value>>18) & 0x1);
	//Vps_printf("value of GP1 = %d\n",gpio);

	return gpio;
}


/* ===================================================================
 *  @func     imx122_spiRead
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

int imx122_spiRead(UInt8 *regAddr, UInt8 *regValue, Uint8 count)
{
	int status = FVID2_SOK;
	UInt8 data;
	int i, j;
	UInt32 delay;

	if(!regAddr || !regValue || !count )
	{
		Vps_printf(" imx122 SPI parameter error.\n");
		return FVID2_EBADARGS;
	}

	/* Clock signal to "Low"*/
	imx122_clrclk();

	for(i = 0; i < count; i++)
	{
		/* CS to Low*/
		imx122_clrcs();

		/* LSB First, Write Read Command Byte */
		data = IMX122_READ_CMD;
		for(j = 0; j < 8; j++)
		{
			if(data & 0x01)		imx122_setdata();
			else				imx122_clrdata();
			imx122_setclk();
			imx122_clrclk();
			data = data >> 1;
		}
		
		delay = 1000;
		while(delay--);
		
		/* Write Register address to Read */
		data = regAddr[i];
		for(j = 0; j < 8; j++)
		{
			if(data & 0x01)		imx122_setdata();
			else				imx122_clrdata();
			imx122_setclk();
			imx122_clrclk();
			data = data >> 1;
		}

		data = 0;
		
		delay = 1000;
		while(delay--);

		for(j = 0; j < 8; j++)
		{
			imx122_setclk();
			if(imx122_getdata())	data |= (1 << j);
			imx122_clrclk();
		}

		regValue[i] = data;

		/* CS to High */
		imx122_setcs();

	}
	
	delay = 2000;
	while(delay--);

	/* Clock Signal to "High" */
	imx122_setclk();

	return status;
}
#endif

/* ===================================================================
 *  @func     imx122_spiWrite
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
int imx122_spiWrite(UInt8 *regAddr, UInt8 *regValue, Uint8 count)
{
	int status = FVID2_SOK;
	Uint16 data;
	int i, j;
	UInt32 delay;

	if(!regAddr || !regValue || !count)
	{
		Vps_printf(" SPI1 parameter error.\n");
		return FVID2_EBADARGS;
	}

	/* Clock signal to "Low"*/
	imx122_clrclk();

	for(i = 0; i < count; i++)
	{
		/* CS to Low*/
		imx122_clrcs();

		data = IMX122_WRITE_CMD;
		for(j = 0; j < 8; j++)
		{
			if(data & 0x01)		imx122_setdata();
			else				imx122_clrdata();
			imx122_setclk();
			imx122_clrclk();
			data = data >> 1;
		}

		delay = 1000;
		while(delay--);

		data = regAddr[i];
		for(j = 0; j < 8; j++)
		{
			if(data & 0x01)		imx122_setdata();
			else				imx122_clrdata();
			imx122_setclk();
			imx122_clrclk();
			data = data >> 1;
		}


		delay = 1000;
		while(delay--);

		data = regValue[i];

		for(j = 0; j < 8; j++)
		{
			if(data & 0x01)		imx122_setdata();
			else				imx122_clrdata();
			imx122_setclk();
			imx122_clrclk();
			data = data >> 1;
		}


		delay = 2000;
		while(delay--);

		/* CS to High */
		imx122_setcs();

	}

	/* Clock Signal to "High" */
	imx122_setclk();

	return status;

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
 *  @func     Iss_Imx122Control
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
Int32 Iss_Imx122Control(Fdrv_Handle handle,
                         UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs)
{
    Iss_Imx122Obj *pObj = (Iss_Imx122Obj *) handle;

    Int32 status;

    if (pObj == NULL)
        return FVID2_EBADARGS;

    /*
     * lock handle
     */
    Iss_Imx122LockObj(pObj);

    switch (cmd)
    {
        case FVID2_START:
            // status = Iss_Imx122Start ( pObj );
            break;

        case FVID2_STOP:
            // status = Iss_Imx122Stop ( pObj );
            break;

        case IOCTL_ISS_SENSOR_GET_CHIP_ID:
            // status = Iss_Imx122GetChipId ( pObj, cmdArgs, cmdStatusArgs
            // );
            break;

        case IOCTL_ISS_SENSOR_RESET:
            // status = Iss_Imx122Reset ( pObj );
            break;

        case IOCTL_ISS_SENSOR_REG_WRITE:
            status = Iss_Imx122RegWrite ( pObj, cmdArgs );
            break;

        case IOCTL_ISS_SENSOR_REG_READ:
            status = Iss_Imx122RegRead ( pObj, cmdArgs );
            break;
		
        case IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN:
            status = Transplant_DRV_imgsSetEshutter();
            status |= Iss_Imx122UpdateExpGain( pObj, cmdArgs );
            break;
		
		case IOCTL_ISS_SENSOR_UPDATE_ITT:
            status = Iss_Imx122UpdateItt(pObj, cmdArgs);
            break;
			
        default:
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    /*
     * unlock handle
     */
    Iss_Imx122UnlockObj(pObj);

    return status;
}

/*
 * Create API that gets called when FVID2_create is called
 *
 * This API does not configure the IMX122 is any way.
 *
 * This API - validates parameters - allocates driver handle - stores create
 * arguments in its internal data structure.
 *
 * Later the create arguments will be used when doing I2C communcation with
 * IMX122
 *
 * drvId - driver ID, must be FVID2_ISS_VID_DEC_IMX122_DRV instanceId - must
 * be 0 createArgs - create arguments createStatusArgs - create status
 * fdmCbParams - NOT USED, set to NULL
 *
 * returns NULL in case of any error */
/* ===================================================================
 *  @func     Iss_Imx122Create
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
Fdrv_Handle Iss_Imx122Create(UInt32 drvId,
                              UInt32 instanceId,
                              Ptr createArgs,
                              Ptr createStatusArgs,
                              const FVID2_DrvCbParams * fdmCbParams)
{

    Iss_Imx122Obj *pObj;
//	UInt8 regValue=0;

    Iss_SensorCreateParams *sensorCreateArgs
        = (Iss_SensorCreateParams *) createArgs;

    Iss_SensorCreateStatus *sensorCreateStatus
        = (Iss_SensorCreateStatus *) createStatusArgs;

#if SET_REG_IN_CREATERFUN
	Iss_RegPair *pReg = IMX_122_INIT_DATA;
	int i = 0;
	int j = 0;
#endif



	Vps_printf( "++Iss_Imx122Create()\n");


    /*
     * check parameters
     */
    if (sensorCreateStatus == NULL)
        return NULL;

    sensorCreateStatus->retVal = FVID2_SOK;

    if (drvId != FVID2_ISS_SENSOR_IMX122_DRV
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
    pObj = Iss_Imx122AllocObj();
    if (pObj == NULL)
    {
        sensorCreateStatus->retVal = FVID2_EALLOC;
        return NULL;
    }

#if SET_REG_IN_CREATERFUN

	/* Hardware Reset */
	imx122_reset();
	
    for (i = 0; i < sizeof(IMX_122_INIT_DATA)/sizeof(Iss_RegPair); i++)
    {
        imx122_spiWrite(&(pReg->regAddr), &(pReg->regValue), 1);

		pReg++;

		#if 0
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
		#endif

	}
#endif
    /*
     * copy parameters to allocate driver handle
     */
    memcpy(&pObj->createArgs, sensorCreateArgs, sizeof(*sensorCreateArgs));

    // Iss_Imx122ResetRegCache(pObj);

    /*
     * return driver handle object pointer
     */



	Vps_printf( "--Iss_Imx122Create()\n");


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
 *  @func     Iss_Imx122Delete
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
Int32 Iss_Imx122Delete(Fdrv_Handle handle, Ptr deleteArgs)
{
    Iss_Imx122Obj *pObj = (Iss_Imx122Obj *) handle;

    if (pObj == NULL)
        return FVID2_EBADARGS;

    /*
     * free driver handle object
     */
    Iss_Imx122FreeObj(pObj);

    return FVID2_SOK;
}

/*
 * System init for IMX122 driver
 *
 * This API - create semaphore locks needed - registers driver to FVID2
 * sub-system - gets called as part of Iss_deviceInit() */
//static int theTimes = 100;



/* ===================================================================
 *  @func     Iss_Imx122Init
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

Int32 Iss_Imx122Init()
{
    Semaphore_Params semParams;

    Int32 status = FVID2_SOK;

    //Int32 count, delay = 100000;

	Int32 cha = 0;
	while(!cha)
	{
		cha = 1;
	}

#if !(SET_REG_IN_CREATERFUN)
	Iss_RegPair *pReg = (Iss_RegPair*)IMX_122_INIT_DATA;
    int i, j = 10000;
    // k = 3;
#endif

    /*
     * Set to 0's for global object, descriptor memory
     */
    memset(&gIss_Imx122CommonObj, 0, sizeof(gIss_Imx122CommonObj));

    /*
     * Create global IMX122 lock
     */
    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;

    gIss_Imx122CommonObj.lock = Semaphore_create(1u, &semParams, NULL);

    if (gIss_Imx122CommonObj.lock == NULL)
        status = FVID2_EALLOC;

    if (status == FVID2_SOK)
    {
#if !(SET_REG_IN_CREATERFUN)

		/* Hardware Reset */
		imx122_reset();

		/*
         * Register IMX122 driver with FVID2 sub-system
         */


        for (i = 0; i < sizeof(IMX_122_INIT_DATA)/sizeof(Iss_RegPair); i++)
        {
            imx122_spiWrite(&(pReg->regAddr), &(pReg->regValue), 1);

			//Task_sleep(100);

			pReg++;

			j = 10000;
			while(--j);

        }
#endif

		Vps_rprintf("\n\n.........INSIDE IMX122 INIT.........\n\n");

        gIss_Imx122CommonObj.fvidDrvOps.create =     (FVID2_DrvCreate) Iss_Imx122Create;
        gIss_Imx122CommonObj.fvidDrvOps.delete = Iss_Imx122Delete;
        gIss_Imx122CommonObj.fvidDrvOps.control = Iss_Imx122Control;
        gIss_Imx122CommonObj.fvidDrvOps.queue = NULL;
        gIss_Imx122CommonObj.fvidDrvOps.dequeue = NULL;
        gIss_Imx122CommonObj.fvidDrvOps.processFrames = NULL;
        gIss_Imx122CommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gIss_Imx122CommonObj.fvidDrvOps.drvId = FVID2_ISS_SENSOR_IMX122_DRV;

        status = FVID2_registerDriver(&gIss_Imx122CommonObj.fvidDrvOps);

        if (status != FVID2_SOK)
        {
            /*
             * Error - free acquired resources
             */
            Semaphore_delete(&gIss_Imx122CommonObj.lock);
			
			Vps_rprintf("\n\nSENSOR REGISTER FAILED...\n\n");
			
        }
    }

	#if	1// No Camera ---ERROR
    {
    	UInt8 regAddr;
		UInt8 regValue=0;
		regAddr = 0x20;// Black level
    	imx122_spiRead(&regAddr, &regValue, 1);
		if(regValue!=0x3c){
			imx122_spiRead(&regAddr, &regValue, 1);
			if(regValue!=0x3c) {
				imx122_spiRead(&regAddr, &regValue, 1);
				if(regValue!=0x3c) {
					Vps_printf("**ERROR %d\n",regValue);
					status = FVID2_EFAIL;
				}
			}
		}
	}
	#endif

    if (status == FVID2_SOK) {
    	Vps_printf( " %s ok!\n", __FUNCTION__);
	} else {
        Vps_printf(" ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__, __LINE__);
    }

    return FVID2_SOK;//status;
}

/*
 * System de-init for IMX122 driver
 *
 * This API - de-registers driver from FVID2 sub-system - delete's allocated
 * semaphore locks - gets called as part of Iss_deviceDeInit() */
/* ===================================================================
 *  @func     Iss_Imx122DeInit
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
Int32 Iss_Imx122DeInit()
{
    /*
     * Unregister FVID2 driver
     */
    FVID2_unRegisterDriver(&gIss_Imx122CommonObj.fvidDrvOps);

    /*
     * Delete semaphore's
     */
    Semaphore_delete(&gIss_Imx122CommonObj.lock);

    return 0;
}

/* ===================================================================
 *  @func     Iss_Imx122PinMux
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
Int32 Iss_Imx122PinMux()
{
    /* setup CAM input pin mux */
    *PINCNTL156 = 0x00050002;		// CAM_D8
    *PINCNTL157 = 0x00050002;		// CAM_D9
    *PINCNTL158 = 0x00050002;		// CAM_D10
    *PINCNTL159 = 0x00050002;		// CAM_D11
	*PINCNTL160 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL161 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL162 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL163 = 0x00050002;				// select function 2  - only works in supervisor mode
    *PINCNTL164 = 0x00050002;		// CAM_D7
    *PINCNTL165 = 0x00050002;		// CAM_D6
    *PINCNTL166 = 0x00050002;		// CAM_D5
    *PINCNTL167 = 0x00050002;		// CAM_D4
    *PINCNTL168 = 0x00050002;		// CAM_D3
    *PINCNTL169 = 0x00050002;		// CAM_D2
    *PINCNTL170 = 0x00050002;		// CAM_D1
    *PINCNTL171 = 0x00050002;		// CAM_D0
    *PINCNTL172 = 0x00050002;		// CAM_HS
    *PINCNTL173 = 0x00050002;		// CAM_VS
    *PINCNTL174 = 0x00050002;		// CAM_FLD
    *PINCNTL175 = 0x00050002;		// CAM_PCLK

	/**PINCNTL160 = 0x00000080;		// SPI_CS  : GP0[14]		
	*PINCNTL161 = 0x00000080;		// SPI_CLK : GP0[15]		
	*PINCNTL162 = 0x00000080;		// SPI_DIN : GP0[16]		
	*PINCNTL163 = 0x00000080;		// SPI_DOUT: GP0[17]		
	*PINCNTL153 = 0x00000080;		// CAM_RESET : GP2[18]
	*/
	
	*(volatile UInt32*)(CTRL_MODULE_BASE_ADDR+0x950) = 0x00000080;		// SPI[1]_CS0: GP1[16]		 [F8]
	*(volatile UInt32*)(CTRL_MODULE_BASE_ADDR+0x954) = 0x00000080;		// SPI[1]:CLK: GP1[17]		 [F8]
	*(volatile UInt32*)(CTRL_MODULE_BASE_ADDR+0x958) = 0x00000080;		// SPI[1]:DIN: GP1[18]		 [F8]
	*(volatile UInt32*)(CTRL_MODULE_BASE_ADDR+0x95C) = 0x00000080;		// SPI[1]:DOUT: GP1[26]		 [F8]	
	//*(volatile UInt32*)(CTRL_MODULE_BASE_ADDR+0x95C) = 0x00000080;		// CAM_RESET: GP3[17]		 [F8]

    /* setup I2C2 pin mux */
    //*PINCNTL74 |= 0x00000020;		// I2C_SCL
    //*PINCNTL75 |= 0x00000020;		// I2C_SCL

	/* CAM_CS, CAM_SCLK, CAM_SDI, CAM_SDO */
	*(volatile UInt32*)(GP1_MODULE_BASE_ADDR+0x134) &= ~((0x1 << 16) + (0x1 << 17) + (0x1 << 26));
	*(volatile UInt32*)(GP1_MODULE_BASE_ADDR+0x134) |= 0x1 << 18 ;
	//*(volatile UInt32*)(GP0_MODULE_BASE_ADDR+0x044) &= ~((0x1 << 14) + (0x1 << 15) + (0x1 << 17));

	/* CAM_RESET as output */
	*(volatile UInt32*)(GP3_MODULE_BASE_ADDR+0x134) &= ~(0x1 << 17);
	//*(volatile UInt32*)(GP2_MODULE_BASE_ADDR+0x044) &= ~(0x1 << 18);

    return 0;

}

/*
 * Handle level lock */
/* ===================================================================
 *  @func     Iss_Imx122LockObj
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
Int32 Iss_Imx122LockObj(Iss_Imx122Obj * pObj)
{
    Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);

    return FVID2_SOK;
}

/*
 * Handle level unlock */
/* ===================================================================
 *  @func     Iss_Imx122UnlockObj
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
Int32 Iss_Imx122UnlockObj(Iss_Imx122Obj * pObj)
{
    Semaphore_post(pObj->lock);

    return FVID2_SOK;
}

/*
 * Global driver level lock */
/* ===================================================================
 *  @func     Iss_Imx122Lock
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
Int32 Iss_Imx122Lock()
{
    Semaphore_pend(gIss_Imx122CommonObj.lock, BIOS_WAIT_FOREVER);

    return FVID2_SOK;
}

/*
 * Global driver level unlock */
/* ===================================================================
 *  @func     Iss_Imx122Unlock
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
Int32 Iss_Imx122Unlock()
{
    Semaphore_post(gIss_Imx122CommonObj.lock);

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
 *  @func     Iss_Imx122AllocObj
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
Iss_Imx122Obj *Iss_Imx122AllocObj()
{
    UInt32 handleId;

    Iss_Imx122Obj *pObj;

    Semaphore_Params semParams;

    UInt32 found = FALSE;

    /*
     * Take global lock to avoid race condition
     */
    Iss_Imx122Lock();

    /*
     * find a unallocated object in pool
     */
    for (handleId = 0; handleId < ISS_DEVICE_MAX_HANDLES; handleId++)
    {

        pObj = &gIss_Imx122CommonObj.Imx122Obj[handleId];

        if (pObj->state == ISS_IMX122_OBJ_STATE_UNUSED)
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
            pObj->state = ISS_IMX122_OBJ_STATE_IDLE;
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
                pObj->state = ISS_IMX122_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /*
     * Release global lock
     */
    Iss_Imx122Unlock();

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
 *  @func     Iss_Imx122FreeObj
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
Int32 Iss_Imx122FreeObj(Iss_Imx122Obj * pObj)
{
    /*
     * take global lock
     */
    Iss_Imx122Lock();

    if (pObj->state != ISS_IMX122_OBJ_STATE_UNUSED)
    {
        /*
         * mark state as unused
         */
        pObj->state = ISS_IMX122_OBJ_STATE_UNUSED;

        /*
         * delete object locking semaphore
         */
        Semaphore_delete(&pObj->lock);
    }

    /*
     * release global lock
     */
    Iss_Imx122Unlock();

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

#define IMGS_SENSOR_LINEWIDTH       (1125)

int Transplant_DRV_imgsCalcSW(int exposureTimeInUsecs)
{
    int Lw, sw;
    float expInuSec; 
    int indb = 1;
    /*Convert db exposure into milli sec*/
    if (indb)
    {
      float expIndB = exposureTimeInUsecs; 
      
      float ratio; 
      
      ratio = (expIndB)/(float)((20*1000));
      ratio = ratio*2.3026;
      expInuSec = exp(ratio);
      //Vps_printf("%d %f %f",exposureTimeInUsecs, ratio, expInuSec );
    }
    else 
    {
      expInuSec = exposureTimeInUsecs;
    }
    Lw = IMGS_SENSOR_LINEWIDTH;
    //pFrame->SW = (int)(((double)exposureTimeInUsecs*(double)Lw)/((double)33333) );
    //pFrame->SW = (int)(((double)exposureTimeInUsecs*0.8*(double)Lw)/((double)33333) );
    #ifdef IMX122_60fps
    sw = (int)(((double)expInuSec*(double)Lw)/((double)66666) );
    #else
    sw = (int)(((double)expInuSec*(double)Lw)/((double)33333) );
    #endif
    if(sw<1)
    {
        sw = 1;
    }

    return sw;
}

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
	//int shtpos;
	UInt8 regAddr[2];
	UInt8 regValue[2];
	Uint32  SetValue, tempdata;

	regAddr[0]		= SHS1_L;
	regAddr[1]		= SHS1_H;
	//regValue[0]		= (UInt8)(ti2a_output_params.sensorExposure & 0xFF);
	//regValue[1]		= (UInt8)((ti2a_output_params.sensorExposure >> 8) & 0xFF);

	status = Iss_Imx122Lock();

	if( FVID2_SOK==status)
	{
		SetValue = Transplant_DRV_imgsCalcSW(ti2a_output_params.sensorExposure);

		if(SetValue<IMGS_SENSOR_LINEWIDTH){
			tempdata = IMGS_SENSOR_LINEWIDTH  - SetValue;
		}
		else
		{
			tempdata =0;
		}

    if (tempdata >= IMGS_SENSOR_LINEWIDTH)
      tempdata = IMGS_SENSOR_LINEWIDTH - 1;
		regValue[1]=(Uint8)((tempdata>>8)&0x00FF);
		regValue[0]=(Uint8)(tempdata & 0x00FF);
		
		//Vps_printf("Transplant_DRV_imgsSetEshutter %d %d %d %d %d\n", 
		//	ti2a_output_params.sensorExposure, SetValue , tempdata , regValue[1], regValue[0]);
		
		imx122_spiWrite(regAddr, regValue, 2);

	}	

	Iss_Imx122Unlock();



	return status;
}

/* ===================================================================
 *  @func     Imx122_GainTableMap
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
#if 0
static UInt8 Imx122_GainTableMap(int again)
{
	UInt8 theRetGain;
	double temp;
	temp = (66.66667*log10(((double)again/1000.0)));
    theRetGain = (UInt8) temp;
	return theRetGain;
}
#endif
static UInt8 Imx122_GainTableMap(int again)
{
	UInt8 theRetGain;
	double temp;
	  
	  temp = 3.3333*(again/1000.0);
    theRetGain = (UInt8) temp;
	return theRetGain;
}


/*
 * Update exposure and gain value from the 2A */
/* ===================================================================
 *  @func     Iss_Imx122UpdateExpGain
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
Int32 Iss_Imx122UpdateExpGain(Iss_Imx122Obj * pObj, Ptr createArgs)
{
    Int32 status = FVID2_SOK;

    UInt8 gain = 0;

    UInt8 regAddr = GAIN;

    status = Iss_Imx122Lock();

    if (FVID2_SOK == status)
    {
        gain = Imx122_GainTableMap(ti2a_output_params.sensorGain);
        if(gain > IMX122_GAIN_MAX)
			gain = IMX122_GAIN_MAX;
			
		//Vps_printf("Exposure Gain %d %d\n", 
		//	ti2a_output_params.sensorGain, gain);
		
        imx122_spiWrite(&regAddr, &gain, 1);
    }
    Iss_Imx122Unlock();

    return status;
}

/*
 * Update ITT Values */
/* ===================================================================
 *  @func     Iss_Imx122UpdateItt
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
Int32 Iss_Imx122UpdateItt(Iss_Imx122Obj * pObj,
                           Itt_RegisterParams * gItt_RegisterParams)
{
    int status = FVID2_SOK;

    int regValue = 0;

    if (gItt_RegisterParams->Control == 1)
    {
        status = imx122_spiWrite((UInt8*)&gItt_RegisterParams->regAddr,(UInt8*) &gItt_RegisterParams->regValue, 2);
		//Vps_printf("RegAddr = 0x%x, RegData = 0x%x\n",gItt_RegisterParams->regAddr,gItt_RegisterParams->regValue);
                             
    }
    else if (gItt_RegisterParams->Control == 0)
    {
        status = imx122_spiRead((UInt8*)&gItt_RegisterParams->regAddr, (UInt8*)&regValue, 2);
        gItt_RegisterParams->regValue = regValue;
		//Vps_printf("RegAddr = 0x%x, RegData = 0x%x\n",gItt_RegisterParams->regAddr,regValue);
    }
    return status;
}

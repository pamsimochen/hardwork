/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/devices/imx140/src/issdrv_imx140Priv.h>
#include <ti/psp/devices/imx140/issdrv_imx140_config.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif_reg.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif.h>
#include <ti/psp/devices/imx140/src/imx140_reg_setting.h>
//#include <ti/psp/devices/iss_sensorID.h>
#define IMGS_SENSOR_LINEWIDTH		(1125)
#define IMGS_SENSOR_LINEWIDTH_3MP	(1608)
#include <math.h>

#include <ti/psp/devices/src/vpsdrv_devicePriv.h>

//#define IMX140_SPI_TEST 1
//#define IMX140_SPI_READ_TEST
//#define SPI_TEST_ADDR  0x05cb

#define USE_D15_AS_SPI_READ_PIN

#define GPIO_MODULE_BASE_ADDR0  (0x48032000u)

#define GIO_READ_DATA0                    		(volatile UInt32*)(GPIO_MODULE_BASE_ADDR0 + 0x0138)
#define GIO_INPUT_OUTPUT_DIR0                   (volatile UInt32*)(GPIO_MODULE_BASE_ADDR0 + 0x0134)
#define GIO_WRITE_DATA0                         (volatile UInt32*)(GPIO_MODULE_BASE_ADDR0 + 0x013c)


//static I2c_Ctrl   gImx140Lvds_I2c_ctrl;
extern Vps_DeviceObj gVps_deviceObj;
Transplant_DRV_imgsFrameTime gImx140_FrameTime;
//int loc_imx140_FrameRate =60;

int imxWDRmode = 0;


Int32 Iss_Imx140UpdateItt(Iss_Imx140Obj * pObj,
                           Itt_RegisterParams * gItt_RegisterParams)
{
    int status = 0;
    return status;
}

int loc_imx140_FrameRate =30;
int Imx140_InitRegArray()
{
	int j,i=0;
   
#ifdef  IMX140_SPI_TEST
	do{
		imx140_setgio(0x0203,0x5a);      
	}while(1);	 
#endif
	j = sizeof(IMX140_SETTING_ID2)/sizeof(unsigned short);
	Vps_printf("ID2 REGs:%d\n",j);
	for (i = 0; i <j; i = i+2)
	{
		imx140_setgio(IMX140_SETTING_ID2[i],IMX140_SETTING_ID2[i+1]);
	}
	j = sizeof(IMX140_SETTING_ID3)/sizeof(unsigned short);
	Vps_printf("ID3 REGs:%d\n",j);	
		for (i = 0; i < j; i = i+2)
	{
			  imx140_setgio(IMX140_SETTING_ID3[i],IMX140_SETTING_ID3[i+1]);

	}
	j = sizeof(IMX140_SETTING_ID4)/sizeof(unsigned short);
	Vps_printf("ID4 REGs:%d\n",j);	
			for (i = 0; i < j; i = i+2)
	{
			  imx140_setgio(IMX140_SETTING_ID4[i],IMX140_SETTING_ID4[i+1]);

	}
	j = sizeof(IMX140_SETTING_ID5)/sizeof(unsigned short);
	Vps_printf("ID5 REGs:%d\n",j);			
	for (i = 0; i < j; i = i+2)
	{
			  imx140_setgio(IMX140_SETTING_ID5[i],IMX140_SETTING_ID5[i+1]);

	}
	  
#ifdef IMX140_SPI_READ_TEST
	Uint8 tmp;
	do{
		tmp = imx140_getgio(SPI_TEST_ADDR);
		Vps_printf("Read back and check:0x%x:0x%x\n",SPI_TEST_ADDR,tmp);
	}while(1);
#endif	

	 return 0;

}


extern isif_regs_ovly		isif_reg;
/* Global object storing all information related to all
  IMX140 driver handles */
static Iss_Imx140CommonObj 	gIss_Imx140CommonObj;
extern ti2a_output 			ti2a_output_params;

void UpdateInitReg(int *indx);
Int32 Iss_Imx140FrameRateSet(Iss_Imx140Obj * pObj, Ptr createArgs, Ptr cmdStatusArgs);


void imx140_setcs()
{
	*GIO_WRITE_DATA |= 0x1 << 16;

}

void imx140_clrcs()
{
	*GIO_CLEAR_DATA |= 0x1 << 16;
	//*GIO_CLEAR_DATA = 0x1 << 16;
}

void imx140_setclk()
{
	*GIO_WRITE_DATA |= 0x1 << 17;
	//*GIO_WRITE_DATA = 0x1 << 17;
}

void imx140_clrclk()
{
	*GIO_CLEAR_DATA |= 0x1 << 17;
	//*GIO_CLEAR_DATA = 0x1 << 17;
}

void imx140_setdata()
{
	//*GIO_WRITE_DATA |= 0x1 << 26;
	*GIO_WRITE_DATA = 0x1 << 26;
}

void imx140_clrdata()
{
	*GIO_CLEAR_DATA |= 0x1 << 26;
	//*GIO_CLEAR_DATA = 0x1 << 26;
}
char  imx140_getdata()
{
	char ret= 0;
#ifdef USE_D15_AS_SPI_READ_PIN
	if(((*GIO_READ_DATA0)&(0x1 << 17))>0)
	{
		ret= 1;
	}
#else

	if(((*GIO_READ_DATA)&(0x1 << 18))>0)
	{
		ret= 1;
	}
#endif
	return ret;
}
void imx140_setgio(unsigned short addr, unsigned short reg)
{
	int i;
	unsigned char imx140_value;
	unsigned char spi_chipid;
	unsigned char page_addr;
	
	spi_chipid = (unsigned char)(addr >> 8);
	page_addr = (unsigned char)(addr&0xFF);
	imx140_value =( unsigned char) reg;

	imx140_clrclk();
	imx140_clrcs();
	/*spi_chipid*/
	for (i=0; i<8; i++) {         
		if (spi_chipid & 0x01)
			imx140_setdata();       
		else                         
			imx140_clrdata();  
		spi_chipid = spi_chipid >> 1; 
		imx140_setclk();   
		imx140_clrclk();              
	}
	/*page address*/	  
	for (i=0; i<8; i++) {         
		if (page_addr & 0x01)
			imx140_setdata();       
		else                         
			imx140_clrdata();  
		page_addr = page_addr >> 1; 
                          
		imx140_setclk();   
		imx140_clrclk();              
	}
	/*data*/  
	for (i=0; i<8; i++) {         
		if (imx140_value & 0x01)  
			imx140_setdata(); 
		else                         
			imx140_clrdata(); 
		imx140_value = imx140_value >> 1; 
                                          
		imx140_setclk();   
		imx140_clrclk();   
	}  
                        
  // CS high                     
    imx140_setcs(); 
}
Uint8 imx140_getgio(Uint16 regaddr)
{

  Uint8 i;
  Uint8 data = 0;
  unsigned char spi_chipid,page_addr,dummy_data=0xff;

 imx140_clrclk();
 imx140_clrcs();

spi_chipid = (unsigned char)(regaddr >> 8)|0x80;
page_addr = (unsigned char)(regaddr&0xFF);

	/*spi_chipid*/
	for (i=0; i<8; i++) {         
		if (spi_chipid & 0x01)
			imx140_setdata();       
		else                         
			imx140_clrdata();  
		spi_chipid = spi_chipid >> 1; 
		imx140_setclk();   
		imx140_clrclk();              
	}
	/*page address*/	  
	for (i=0; i<8; i++) {         
		if (page_addr & 0x01)
			imx140_setdata();       
		else                         
			imx140_clrdata();  
		page_addr = page_addr >> 1; 
                          
		imx140_setclk();   
		imx140_clrclk();              
	}	
  // 8 bits output,assuming LSB first
	for (i=0; i<8; i++) {         
		if (dummy_data & 0x01)
			imx140_setdata();       
		else                         
			imx140_clrdata();  
		dummy_data = dummy_data >> 1; 
                          
		imx140_setclk();
		
//		                  data=data>>1;
						  
		if(imx140_getdata())
			data = data |(1<<i);
		else
			data = data&(~(1<<i));
/*			
			{
		data =data |0x80;
		Vps_printf("always 000001\n");
		}
		else
		{
		data = data & 0xef;
		Vps_printf("always 00000\n");
		}
*/		
		imx140_clrclk();              
	}

  // CS high
  imx140_setcs(); 

return data;
}
/* Control API that gets called when FVID2_control is called

  This API does handle level semaphore locking

  handle - Imx140 driver handle
  cmd - command
  cmdArgs - command arguments
  cmdStatusArgs - command status

  returns error in case of
  - illegal parameters
  - I2C command RX/TX error
*/
Int32 Iss_Imx140Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
	Iss_Imx140Obj *pObj = ( Iss_Imx140Obj * ) handle;
	Int32 status;
	//int  loopNum=1;

	if ( pObj == NULL )
		return FVID2_EBADARGS;

    /*
     * lock handle
     */
    Iss_Imx140LockObj ( pObj );

	switch ( cmd )
    {
        case FVID2_START:
            break;

        case FVID2_STOP:
            break;

        case IOCTL_ISS_SENSOR_GET_CHIP_ID:
            break;

        case IOCTL_ISS_SENSOR_RESET:
            break;

        case IOCTL_ISS_SENSOR_REG_WRITE:
            break;

        case IOCTL_ISS_SENSOR_REG_READ:
            break;

        case IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN:

	     
             status = Iss_Imx140UpdateExpGain ( pObj, cmdArgs);
	 
             break;
        case IOCTL_ISS_SENSOR_UPDATE_FRAMERATE:
        	{
#if 0
        		if(g_issCurrentSensorID!=FVID2_ISS_SENSOR_IMX140_LVDS_DRV)
        		{
        			status = FVID2_EUNSUPPORTED_CMD;
        			break;
        		}
        		else
        		{
        			Ptr createArgs;
    	        	Iss_CaptFrameRate *framerateParams = (Iss_CaptFrameRate *)cmdArgs;
    	        	createArgs = &(framerateParams->FrameRate);
                	status = Iss_Imx140FrameRateSet(pObj, createArgs, cmdStatusArgs);
        		}

#endif
        	}
            break;

        case IOCTL_ISS_SENSOR_FRAME_RATE_SET:
            status = Iss_Imx140FrameRateSet(pObj, cmdArgs, cmdStatusArgs);
            break;

		case IOCTL_ISS_SENSOR_UPDATE_ITT:
            status = Iss_Imx140UpdateItt(pObj, cmdArgs);
            break;
        default:
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    /*
     * unlock handle
     */
    Iss_Imx140UnlockObj ( pObj );

    return status;
}
Int32 Iss_Imx140FrameRateSet(Iss_Imx140Obj * pObj, Ptr createArgs,
                              Ptr cmdStatusArgs)
{
    Int32 status = FVID2_SOK;

    UInt32 i2cInstId = Iss_platformGetI2cInstId();

    Int32 count = 0;

    Int32 framerate = *(Int32 *) createArgs;
    unsigned short regAddr[3],regData[3];
    regAddr[0]=0x0209;
    regAddr[1]=0x021b;
    regAddr[2]=0x021c;

    if (framerate == 60)
    {
    #if 0
    	if(FVID2_ISS_SENSOR_IMX140_LVDS_DRV==g_issCurrentSensorID)
    	{
			regData[0]=1;
			regData[1]=0x98;
			regData[2]=0x08;
			 loc_imx140_FrameRate =60;
			count++;
			*(Int32 *) cmdStatusArgs = 1100;
    	}
    	else
    	{
    		status = FVID2_EFAIL;
    		return status;
    	}
#endif
        //loc_RealRowTime=ROW_TIME;
    }
    else if (framerate == 30)
    {
    	regData[0]=2;
    	regData[1]=0x30;
    	regData[2]=0x11;
    	 loc_imx140_FrameRate =30;
        count++;
        *(Int32 *) cmdStatusArgs = 1100*2;
        //loc_RealRowTime=ROW_TIME*6/5;
    }

    else
    {
        status = FVID2_EFAIL;
        return status;
    }
    /** take global lock **/
    Iss_Imx140Lock();
    //imx140_setgio(0x0200,1); //enable the streaming
    //Task_sleep(40);
    int i=0;
    for(i=0;i<3;i++)
    {
    	imx140_setgio(regAddr[i], regData[i]);
    }
    //imx140_setgio(0x0200,1); //enable the streaming
   // Task_sleep(40);
    /** release global lock **/
    Iss_Imx140Unlock();

    return status;
}


void Iss_IMX140_EnableStreaming()
{
	Vps_printf("inside Iss_IMX140_EnableStreaming ");
    	// Task_sleep(10);
	imx140_setgio(0x0200,0); //enable the streaming,normal operation
	Task_sleep(1);
	imx140_setgio(0x0202,0);//master mode start
	Task_sleep(100);
}
/*
  Create API that gets called when FVID2_create is called

  This API does not configure the Imx140 is any way.

  This API
  - validates parameters
  - allocates driver handle
  - stores create arguments in its internal data structure.

  Later the create arguments will be used when doing I2C communcation with
  Imx140

  drvId - driver ID, must be FVID2_ISS_VID_DEC_Imx140_DRV
  instanceId - must be 0
  createArgs - create arguments
  createStatusArgs - create status
  fdmCbParams  - NOT USED, set to NULL

  returns NULL in case of any error
*/
Fdrv_Handle Iss_Imx140Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams )
{
	Iss_Imx140Obj *pObj;
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

#if 0
    if ( drvId != g_issCurrentSensorID
         || instanceId != 0 || sensorCreateArgs == NULL )
    {
        sensorCreateStatus->retVal = FVID2_EBADARGS;
        return NULL;
    }
#endif

    if ( sensorCreateArgs->deviceI2cInstId >= ISS_DEVICE_I2C_INST_ID_MAX )
    {
        sensorCreateStatus->retVal = FVID2_EINVALID_PARAMS;
        return NULL;
    }

    /*
     * allocate driver handle
     */
    pObj = Iss_Imx140AllocObj (  );
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
Int32 Iss_Imx140Delete ( Fdrv_Handle handle, Ptr deleteArgs )
{
    Iss_Imx140Obj *pObj = ( Iss_Imx140Obj * ) handle;

    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /*
     * free driver handle object
     */
    Iss_Imx140FreeObj ( pObj );

    return FVID2_SOK;
}



/*
  System init for Imx140 driver

  This API
  - create semaphore locks needed
  - registers driver to FVID2 sub-system
  - gets called as part of Iss_deviceInit()
*/
//static int theTimes=100;


int Imx140_DRV_imgsSetRegs()
{
	//int delays=100000;
	//int status = 0;
	int j = 0;
	//Uint8 tmp;

	j=Imx140_InitRegArray();

	Vps_printf("With IMX140 SETTING,1080P@60FPS,%s,%d %d\n",__FUNCTION__,__LINE__,j);

	return FVID2_SOK;

}

Int32 Iss_Imx140Init (  )
{
    Semaphore_Params semParams;
    Int32 status = FVID2_SOK;
	
    /*
     * Set to 0's for global object, descriptor memory
     */
    memset ( &gIss_Imx140CommonObj, 0, sizeof ( gIss_Imx140CommonObj ) );

    /*
     * Create global Imx140 lock
     */
    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    gIss_Imx140CommonObj.lock = Semaphore_create ( 1u, &semParams, NULL );

    if ( gIss_Imx140CommonObj.lock == NULL )
        status = FVID2_EALLOC;

    if ( status == FVID2_SOK )
    {
	
    	Imx140_DRV_imgsSetRegs();
        gIss_Imx140CommonObj.fvidDrvOps.create = ( FVID2_DrvCreate ) Iss_Imx140Create;
        gIss_Imx140CommonObj.fvidDrvOps.delete = Iss_Imx140Delete;
        gIss_Imx140CommonObj.fvidDrvOps.control = Iss_Imx140Control;
        gIss_Imx140CommonObj.fvidDrvOps.queue = NULL;
        gIss_Imx140CommonObj.fvidDrvOps.dequeue = NULL;
        gIss_Imx140CommonObj.fvidDrvOps.processFrames = NULL;
        gIss_Imx140CommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gIss_Imx140CommonObj.fvidDrvOps.drvId = FVID2_ISS_SENSOR_IMX140_DRV;
        //gIss_Imx140CommonObj.fvidDrvOps.drvId =g_issCurrentSensorID;
        status = FVID2_registerDriver ( &gIss_Imx140CommonObj.fvidDrvOps );

        if ( status != FVID2_SOK )
        {
            /*
             * Error - free acquired resources
             */
            Semaphore_delete ( &gIss_Imx140CommonObj.lock );
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
  System de-init for Imx140 driver

  This API
  - de-registers driver from FVID2 sub-system
  - delete's allocated semaphore locks
  - gets called as part of Iss_deviceDeInit()
*/
Int32 Iss_Imx140DeInit (  )
{
    /*
     * Unregister FVID2 driver
     */
    FVID2_unRegisterDriver ( &gIss_Imx140CommonObj.fvidDrvOps );

    /*
     * Delete semaphore's
     */
    Semaphore_delete ( &gIss_Imx140CommonObj.lock );

    return 0;
}

Int32 Iss_Imx140PinMux (  )
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
	
#ifdef USE_D15_AS_SPI_READ_PIN
	*PINCNTL163 = 0x00000080;
	*GIO_INPUT_OUTPUT_DIR0 |=(0x1 << 17);
#else
	*PINCNTL87 = 0x00000080;
	*GIO_INPUT_OUTPUT_DIR |=(0x01<<18);
#endif

    //Vps_printf (" spi gio enable in %s\n",__FUNCTION__);
	return 0;
}
/*
  Handle level lock
*/
Int32 Iss_Imx140LockObj ( Iss_Imx140Obj * pObj )
{
    Semaphore_pend ( pObj->lock, BIOS_WAIT_FOREVER );
    return FVID2_SOK;
}

/*
  Handle level unlock
*/
Int32 Iss_Imx140UnlockObj ( Iss_Imx140Obj * pObj )
{
    Semaphore_post ( pObj->lock );
    return FVID2_SOK;
}

/*
  Global driver level lock
*/
Int32 Iss_Imx140Lock (  )
{
    Semaphore_pend ( gIss_Imx140CommonObj.lock, BIOS_WAIT_FOREVER );
    return FVID2_SOK;
}

/*
  Global driver level unlock
*/
Int32 Iss_Imx140Unlock (  )
{
    Semaphore_post ( gIss_Imx140CommonObj.lock );
    return FVID2_SOK;
}

/*
  Allocate driver object

  Searches in list of driver handles and allocate's a 'NOT IN USE' handle
  Also create's handle level semaphore lock

  return NULL in case handle could not be allocated
*/
Iss_Imx140Obj *Iss_Imx140AllocObj (  )
{
    UInt32 handleId;
    Iss_Imx140Obj *pObj;
    Semaphore_Params semParams;
    UInt32 found = FALSE;

    /*
     * Take global lock to avoid race condition
     */
    Iss_Imx140Lock (  );

    /*
     * find a unallocated object in pool
     */
    for ( handleId = 0; handleId < ISS_DEVICE_MAX_HANDLES; handleId++ )
    {
        pObj = &gIss_Imx140CommonObj.Imx140Obj[handleId];
        if ( pObj->state == ISS_IMX140_OBJ_STATE_UNUSED )
        {
            /* free object found */

            /* init to 0's */
            memset ( pObj, 0, sizeof ( *pObj ) );

            /* init state and handle ID */
            pObj->state = ISS_IMX140_OBJ_STATE_IDLE;
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
                pObj->state = ISS_IMX140_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /* Release global lock */
    Iss_Imx140Unlock (  );

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
Int32 Iss_Imx140FreeObj ( Iss_Imx140Obj * pObj )
{
    /* take global lock */
    Iss_Imx140Lock (  );

    if ( pObj->state != ISS_IMX140_OBJ_STATE_UNUSED )
    {
        /* mark state as unused */
        pObj->state = ISS_IMX140_OBJ_STATE_UNUSED;

        /* delete object locking semaphore */
        Semaphore_delete ( &pObj->lock );
    }

    /* release global lock */
    Iss_Imx140Unlock( );

    return FVID2_SOK;
}

void Imx140_pintISIFRegDirectly()
{
	static int tmpIndex=0x064;
    int i=0;
    if (tmpIndex>0)
	{
		tmpIndex--;
	}
	else
	{
         	
       	uint32 *pData = NULL;
       	//pData=(uint32 *)(&(isp_regs->SYNCEN));
		pData=(uint32 *)(isif_reg);
       	if(NULL!=pData)
		{
			for(i=0;i<127;i++)
			{
				Vps_printf ("Debug : global structure  isp_regs [%d] = 0x%08x ",i,*pData);
				pData++;
			}

        }
		else
		{
                	 Vps_printf ( " ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__,
                        __LINE__ );
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


int Imx140_Transplant_DRV_imgsCalcSW(int exposureTimeInUsecs)
{

	int Lw;
	Transplant_DRV_imgsFrameTime *pFrame = &gImx140_FrameTime;

	Lw = IMGS_SENSOR_LINEWIDTH;
	//pFrame->SW = (int)(((double)exposureTimeInUsecs*(double)Lw)/((double)33333) );
	//pFrame->SW = (int)(((double)exposureTimeInUsecs*0.8*(double)Lw)/((double)33333) );
	if(loc_imx140_FrameRate==60)
	{
		pFrame->SW = (int)(((double)exposureTimeInUsecs*(double)Lw)/((double)66666) );

	}
	else if(loc_imx140_FrameRate==30)
	{
		pFrame->SW = (int)(((double)exposureTimeInUsecs*(double)Lw)/((double)33333) );
	}

	if(pFrame->SW<1)
	{
		pFrame->SW = 1;	
	}
	
	return FVID2_SOK;

}

int Imx140_Transplant_DRV_imgsSetEshutter(Uint32 eshutterInUsec, int setRegDirect)
{
    int status = FVID2_SOK;
    Uint32  SetValue,tempdata;
    Uint8   Send_HH,Send_H,Send_L;
    Transplant_DRV_imgsFrameTime *pFrame = &gImx140_FrameTime;

    if(eshutterInUsec==0){
        Vps_printf("STError:\t%d\n",eshutterInUsec);
        return status;
    }


    Imx140_Transplant_DRV_imgsCalcSW(eshutterInUsec);
    SetValue = pFrame->SW;
    if(SetValue<IMGS_SENSOR_LINEWIDTH){
        tempdata = IMGS_SENSOR_LINEWIDTH  - SetValue;
    }
    else
    {
        tempdata = 0;
    }
    Send_HH=(Uint8)(tempdata>>16);
    Send_H=(Uint8)((tempdata>>8)&0x00FF);
    Send_L=(Uint8)(tempdata & 0x00FF);
    if(imxWDRmode == 0)
    {
        imx140_setgio(0x220,Send_L);
        imx140_setgio(0x221,Send_H);
        imx140_setgio(0x222,Send_HH);
    }

    return status;
}




int Imx140_Transplant_DRV_imgsSetAgain(int again, int setRegDirect)
{
	int	status = FVID2_SOK;
	double gain_times;
	double gain_db;
	unsigned int gain_reg;


//42db:125000
	if (again < 1000)
		again = 1000;
	if (again > 125892)
		again = 125892;

	gain_times = again / 1000.0; //floor
	gain_db = 20.0 * (double) (log10(gain_times));
	gain_reg = 10.0 * gain_db;

//	fprintf(stderr,"again=%d gain_reg=%d\n",again, gain_reg);

	imx140_setgio(0x0214, (gain_reg & 0xff));
	imx140_setgio(0x0215, ((gain_reg >> 8) & 0xff));
	return status;
}


/*
  Update exposure and gain value from the 2A
*/
Int32 Iss_Imx140UpdateExpGain ( Iss_Imx140Obj * pObj, Ptr createArgs )
{
	Int32 status = FVID2_SOK;
	
	status=Iss_Imx140Lock (  );
	//gImx140_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
	if( FVID2_SOK==status)
	{
		Imx140_Transplant_DRV_imgsSetAgain(ti2a_output_params.sensorGain,1);
		Imx140_Transplant_DRV_imgsSetEshutter(ti2a_output_params.sensorExposure,0);
		//g_int = Imx140_GainTableMap(ti2a_output_params.sensorGain);
		//imx140_setgio(regAddr,g_int);
	}
	Iss_Imx140Unlock (  );
	return status;
}

/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/devices/imx104/src/issdrv_imx104Priv.h>
#include <ti/psp/devices/imx104/issdrv_imx104_config.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif_reg.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif.h>
#include <ti/psp/devices/imx104/src/imx104_reg_setting.h>
#include <ti/psp/devices/imx104/issdrv_imx104.h>

//#define IMX104_SPI_TEST
//#define LVDS324_720P_TEST_MODE

//#define IMX104_LVDS324
#ifdef IMX104_LVDS324

#include <ti/psp/devices/src/vpsdrv_devicePriv.h>
static I2c_Ctrl   gImx104Lvds_I2c_ctrl;
extern Vps_DeviceObj gVps_deviceObj;
#endif

int imxWDRmode = 0;
static void  Transplant_OSA_waitMsecs(Uint32 msecs)
{
	Uint32 i=0; 
	Uint32 tmpDelayNum=360000;
	for(i=0;i<msecs;i++)
	{
		while(tmpDelayNum>2)
		{tmpDelayNum--;}	
	}
}	

static int Imx104_GainTableMap(int again)	
{
	int theRetGain;
	double temp;
	temp = (200*log10(((double)again)/1000.0));
	if (imxWDRmode == 1)
		temp = temp - 45;

	theRetGain =(unsigned short)temp / 3;
	return theRetGain;
}

int InitRegArrary()
{
	int i=0;
   
#ifdef  IMX104_SPI_TEST
	do{
		imx104_setgio(IMX104_SETTING[0],IMX104_SETTING[1]);      
	}while(1);	 
#endif

	Vps_printf("Current FRAME RATE SEL:%d\n",IMX104_LVDS_FPS);

	Transplant_OSA_waitMsecs(200);
	imx104_setgio(0x0200,0x01); //standy  mode
	Transplant_OSA_waitMsecs(200);

	for (i = 0; i < IMX104_REG_COUNT; i = i+2)
	{
		imx104_setgio(IMX104_SETTING[i],IMX104_SETTING[i+1]);      
	}
 
	Transplant_OSA_waitMsecs(200);
	imx104_setgio(0x0200,0); //cancel standy mode
 
	Transplant_OSA_waitMsecs(200);
	imx104_setgio(0x0202,0); //start master mode
	return 0;
}


extern isif_regs_ovly isif_reg;

/* Global object storing all information related to all
  IMX104 driver handles */
static Iss_Imx104CommonObj 	gIss_Imx104CommonObj;
extern ti2a_output 		ti2a_output_params;
void UpdateInitReg(int *indx);

void imx104_setcs()
{
	*GIO_WRITE_DATA |= 0x1 << 16;
}

void imx104_clrcs()
{
	*GIO_CLEAR_DATA |= 0x1 << 16;
}

void imx104_setclk()
{
	*GIO_WRITE_DATA |= 0x1 << 17;
}

void imx104_clrclk()
{
	*GIO_CLEAR_DATA |= 0x1 << 17;
}

void imx104_setdata()
{
	*GIO_WRITE_DATA |= 0x1 << 26;
}

void imx104_clrdata()
{
	*GIO_CLEAR_DATA |= 0x1 << 26;
}

void imx104_setgio(unsigned short addr, unsigned short reg)
{
	int i;
	unsigned char imx104_value;
	unsigned char spi_chipid;
	unsigned char page_addr;
	
	spi_chipid = (unsigned char)(addr >> 8);
	page_addr = (unsigned char)(addr&0xFF);
	imx104_value =( unsigned char) reg;

	imx104_clrclk();
	imx104_clrcs();
	/*spi_chipid*/
	for (i=0; i<8; i++) {         
		if (spi_chipid & 0x01)
			imx104_setdata();       
		else                         
			imx104_clrdata();  
		spi_chipid = spi_chipid >> 1; 
                          
		imx104_setclk();   
		imx104_clrclk();              
	}
	/*page address*/	  
	for (i=0; i<8; i++) {         
		if (page_addr & 0x01)
			imx104_setdata();       
		else                         
			imx104_clrdata();  
		page_addr = page_addr >> 1; 
                          
		imx104_setclk();   
		imx104_clrclk();              
	}
	/*data*/  
	for (i=0; i<8; i++) {         
		if (imx104_value & 0x01)  
			imx104_setdata(); 
		else                         
			imx104_clrdata(); 
		imx104_value = imx104_value >> 1; 
                                          
		imx104_setclk();   
		imx104_clrclk();   
	}  
                        
	// CS high                     
	imx104_setcs(); 
}

Int32 Iss_Imx104_HDR_Enable(int enable)
{
	if(enable == 2)
	{
		imxWDRmode = 1;
		Vps_printf("IMX104 WDR enabled\n");
		imx104_setgio(0x20a,0x00);
		imx104_setgio(0x20C,0x02);  //0: Normal mode 2:WDR mode
		imx104_setgio(0x20F,0x05);  //Compressed Output enable 1:enable
		imx104_setgio(0x210,0x38);  //Fixed
		imx104_setgio(0x212,0x0F);   //Fixed
		imx104_setgio(0x220,0xBF);  //SHS1
		imx104_setgio(0x221,0x02);  //SHS1
		imx104_setgio(0x222,0x00);  //SHS1  002BD = 701 lines
		imx104_setgio(0x223,0x0E);  //SHS2
		imx104_setgio(0x224,0x00);  //SHS2
		imx104_setgio(0x265,0x00);  //Fixed
		imx104_setgio(0x284,0x0F);  //Midpoint potential setting
		imx104_setgio(0x286,0x10);  //Fixed
		imx104_setgio(0x2CF,0xE1);   //Fixed
		imx104_setgio(0x2D0,0x29);   //Fixed
		imx104_setgio(0x2D2,0x9B);  //Fixed
		imx104_setgio(0x2D3,0x01);   //Fixed

		imx104_setgio(0x461,0x9B);
		imx104_setgio(0x466,0xD0);
		imx104_setgio(0x467,0x08);

	} else {
		imxWDRmode = 0;
		Vps_printf("IMX104 WDR disabled\n");
		imx104_setgio(0x20a,0x30);
		imx104_setgio(0x20C,0x00);
		imx104_setgio(0x20F,0x01);
		imx104_setgio(0x210,0x39);
		imx104_setgio(0x212,0x50);
		imx104_setgio(0x220,0x0);
		imx104_setgio(0x221,0x00);
		imx104_setgio(0x222,0x00);
		imx104_setgio(0x223,0x00);
		imx104_setgio(0x224,0x00);
		imx104_setgio(0x265,0x20);
		imx104_setgio(0x284,0x0);
		imx104_setgio(0x286,0x1);
		imx104_setgio(0x2CF,0xd1);
		imx104_setgio(0x2D0,0x1b);
		imx104_setgio(0x2D2,0x5f);
		imx104_setgio(0x2D3,0x0);

		imx104_setgio(0x461,0x9b);
		imx104_setgio(0x466,0xD0);
		imx104_setgio(0x467,0x08);
	}
	return 1;
}
/* Control API that gets called when FVID2_control is called

  This API does handle level semaphore locking

  handle - Imx104 driver handle
  cmd - command
  cmdArgs - command arguments
  cmdStatusArgs - command status

  returns error in case of
  - illegal parameters
  - I2C command RX/TX error
*/
Int32 Iss_Imx104Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
	Iss_Imx104Obj *pObj = ( Iss_Imx104Obj * ) handle;
	Int32 status;
	//int  loopNum=1;

	if ( pObj == NULL )
		return FVID2_EBADARGS;

	Int32 enable = *(Int32 *) cmdArgs;
	/*
 	 * lock handle
	 */
	Iss_Imx104LockObj ( pObj );

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
			Transplant_DRV_imgsSetEshutter();
			status = Iss_Imx104UpdateExpGain ( pObj, cmdArgs);
			break;

		case IOCTL_ISS_SENSOR_WDR_MODE:
			Iss_Imx104_HDR_Enable(enable);
			break;

		default:
			status = FVID2_EUNSUPPORTED_CMD;
			break;
	}

	/*
	 * unlock handle
	 */
	Iss_Imx104UnlockObj ( pObj );

	return status;
}

/*
  Create API that gets called when FVID2_create is called

  This API does not configure the Imx104 is any way.

  This API
  - validates parameters
  - allocates driver handle
  - stores create arguments in its internal data structure.

  Later the create arguments will be used when doing I2C communcation with
  Imx104

  drvId - driver ID, must be FVID2_ISS_VID_DEC_Imx104_DRV
  instanceId - must be 0
  createArgs - create arguments
  createStatusArgs - create status
  fdmCbParams  - NOT USED, set to NULL

  returns NULL in case of any error
*/
Fdrv_Handle Iss_Imx104Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams )
{

	Iss_Imx104Obj *pObj;
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

	if ( drvId != FVID2_ISS_SENSOR_IMX104_DRV
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
	pObj = Iss_Imx104AllocObj (  );
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
Int32 Iss_Imx104Delete ( Fdrv_Handle handle, Ptr deleteArgs )
{
	Iss_Imx104Obj *pObj = ( Iss_Imx104Obj * ) handle;

	if ( pObj == NULL )
		return FVID2_EBADARGS;

    /*
     * free driver handle object
     */
	Iss_Imx104FreeObj ( pObj );

	return FVID2_SOK;
}

#define IMGS_SENSOR_LINEWIDTH      (750)  //(1082) 
int Transplant_DRV_imgsCalcSW(int exposureTimeInUsecs)
{

	int Lw, sw;

	Lw = IMGS_SENSOR_LINEWIDTH;
	//pFrame->SW = (int)(((double)exposureTimeInUsecs*(double)Lw)/((double)33333) );
	//pFrame->SW = (int)(((double)exposureTimeInUsecs*0.8*(double)Lw)/((double)33333) );
	#ifdef IMX104_60fps
	sw = (int)(((double)exposureTimeInUsecs*(double)Lw)/((double)16666) );
	#else
	sw = (int)(((double)exposureTimeInUsecs*(double)Lw)/((double)33333) );
	#endif
	if(sw<1)
	{
		sw = 1;
	}

	return sw;
}


/*
  System init for Imx104 driver

  This API
  - create semaphore locks needed
  - registers driver to FVID2 sub-system
  - gets called as part of Iss_deviceInit()
*/
//static int theTimes=100;


int Transplant_DRV_imgsSetEshutter()
{
	Int32 status = FVID2_SOK;
	Uint32  SetValue, tempdata;
	Uint8   Send_HH,Send_H,Send_L;
	//static int theTimes=100;
	status=Iss_Imx104Lock (  );
	//if( FVID2_SOK==status)
	if( FVID2_SOK==status)
	{

		//Normal & HDR long exposure
		SetValue = Transplant_DRV_imgsCalcSW(ti2a_output_params.sensorExposure);

		if(SetValue<IMGS_SENSOR_LINEWIDTH){
			tempdata = IMGS_SENSOR_LINEWIDTH  - SetValue;
		}
		else
		{
			//tempdata = SetValue;
			tempdata =0;
		}
		Send_HH=(Uint8)(tempdata>>16);
		Send_H=(Uint8)((tempdata>>8)&0x00FF);
		Send_L=(Uint8)(tempdata & 0x00FF);


		if(imxWDRmode == 0)
		{
			imx104_setgio(0x220,Send_L);
			imx104_setgio(0x221,Send_H);
			imx104_setgio(0x222,Send_HH);
		} else //(imxWDRmode == 1)
		{
			//Vps_printf("Transplant_DRV_imgsSetEshutter Long %d %d \n", ti2a_output_params.sensorExposure, tempdata);
			imx104_setgio(0x223,Send_L);
			imx104_setgio(0x224,Send_H);
			imx104_setgio(0x225,Send_HH);
			/*	Long integration 160H to 736H (SHS2 = 589 to 13d) 
			 Short integration 10H to 46H (SHS1 739 to 703d) */

			//Vps_printf("Transplant_DRV_imgsSetEshutter %d %d %d %d %d %d\n", 
			//	ti2a_output_params.sensorExposure, SetValue , tempdata , Send_HH, Send_H, Send_L);
			SetValue = Transplant_DRV_imgsCalcSW(ti2a_output_params.sensorExposure/16);
			if(SetValue<IMGS_SENSOR_LINEWIDTH){
        		tempdata = IMGS_SENSOR_LINEWIDTH  - SetValue;
			} else {
				//tempdata = SetValue;
				tempdata =0;
			}
			//Vps_printf("Transplant_DRV_imgsSetEshutter short: %d %d\n", ti2a_output_params.sensorExposure/16, tempdata);
			Send_HH=(Uint8)(tempdata>>16);
			Send_H=(Uint8)((tempdata>>8)&0x00FF);
			Send_L=(Uint8)(tempdata& 0x00FF);

			imx104_setgio(0x220,Send_L);
			imx104_setgio(0x221,Send_H);
			imx104_setgio(0x222,Send_HH);

		}
	}	
	Iss_Imx104Unlock() ;
	return status;
}


int DRV_imgsSetRegs()
{
	//int delays=100000;
	int j = 0;
/*************for lvds324 *********************/
#ifdef IMX104_LVDS324
	unsigned char lvdsRegAddr[12];
	unsigned char lvdsRegValue[12];
	//int delays = 100000;
	gVps_deviceObj.i2cDebugEnable=1;
	gImx104Lvds_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
	
	lvdsRegAddr[0]=0x00;//first chip id address
	lvdsRegAddr[1]=0x0b;//FRAME_WIDTH_MSB
	lvdsRegAddr[2]=0x0c;//FRAME_WIDTH_LSB
	lvdsRegAddr[3]=0x0d;//FRAME_HEIGHT_MSB
	lvdsRegAddr[4]=0x0e;//FRAME_HEIGHT_LSB
	//do
	//	{
	Iss_deviceRead8(gImx104Lvds_I2c_ctrl.i2cInstId,0x2d,lvdsRegAddr,lvdsRegValue, 5);
	Vps_printf("lvds324 chip id 1st:0x%x\n",lvdsRegValue[0]);
	Vps_printf("defaut width:0x%x,height:0x%x",(lvdsRegValue[1]<<8)|lvdsRegValue[2],(lvdsRegValue[3]<<8)|lvdsRegValue[4]);
	//	}while(1);
	
#endif
/***************************************************************/
#ifndef LVDS324_720P_TEST_MODE
	lvdsRegAddr[0]=0x09; lvdsRegValue[0]=0x21; /*for 0x11:output data lsb first;0x31:output msb first,both for  sony wide channel 12bpp*/
	lvdsRegAddr[1]=0x0A; lvdsRegValue[1]=0x62; /*0x62--->SCLK:58MHz~75MHz;OUTCLK:116~150MHz*/
										      /*0x61--->SCLK:31MHz~60MHz;OUTCLK:62~120MHz*/
	Iss_deviceWrite8(gImx104Lvds_I2c_ctrl.i2cInstId,0x2d,&lvdsRegAddr[0],&lvdsRegValue[0], 1);
	Iss_deviceWrite8(gImx104Lvds_I2c_ctrl.i2cInstId,0x2d,&lvdsRegAddr[1],&lvdsRegValue[1], 1);
	
	j=InitRegArrary();
	Vps_printf("With IMX104 SETTING %d\n", j);
#else
	//test mode for 720p@60fps
	lvdsRegAddr[0]=0x1f;  lvdsRegValue[0]=0x05;
	lvdsRegAddr[1]=0x20; lvdsRegValue[1]=0xdc;
	lvdsRegAddr[2]=0x21; lvdsRegValue[2]=0x03;
	lvdsRegAddr[3]=0x22; lvdsRegValue[3]=0x39;
	/*active area*/
	lvdsRegAddr[4]=0x0b;  lvdsRegValue[4]=0x05;
	lvdsRegAddr[5]=0x0c;  lvdsRegValue[5]=0x00;
	lvdsRegAddr[6]=0x0d;  lvdsRegValue[6]=0x02;
	lvdsRegAddr[7]=0x0e;  lvdsRegValue[7]=0xd0;
	/* for clock*/
	lvdsRegAddr[8]=0x0a;  lvdsRegValue[8]=0x62;
	/*enable test mode*/		  
	lvdsRegAddr[9]=0x09; lvdsRegValue[9]=0x70;//enable test mode in lvds324									      
	Iss_deviceWrite8(gImx104Lvds_I2c_ctrl.i2cInstId,0x2d,lvdsRegAddr,lvdsRegValue, 10);  
	
	Vps_printf("ENABLED 720P@60FPS TEST MODE\n");
#endif
	
	return FVID2_SOK;
}

Int32 Iss_Imx104Init (  )
{
	Semaphore_Params semParams;
	Int32 status = FVID2_SOK;
	
	/*
	 * Set to 0's for global object, descriptor memory
	 */
	memset ( &gIss_Imx104CommonObj, 0, sizeof ( gIss_Imx104CommonObj ) );

	/*
	 * Create global Imx104 lock
	 */
	Semaphore_Params_init ( &semParams );
	semParams.mode = Semaphore_Mode_BINARY;

	gIss_Imx104CommonObj.lock = Semaphore_create ( 1u, &semParams, NULL );

	if ( gIss_Imx104CommonObj.lock == NULL )
		status = FVID2_EALLOC;

	if ( status == FVID2_SOK )
	{
		DRV_imgsSetRegs();
	
		gIss_Imx104CommonObj.fvidDrvOps.create = ( FVID2_DrvCreate ) Iss_Imx104Create;
		gIss_Imx104CommonObj.fvidDrvOps.delete = Iss_Imx104Delete;
		gIss_Imx104CommonObj.fvidDrvOps.control = Iss_Imx104Control;
		gIss_Imx104CommonObj.fvidDrvOps.queue = NULL;
		gIss_Imx104CommonObj.fvidDrvOps.dequeue = NULL;
		gIss_Imx104CommonObj.fvidDrvOps.processFrames = NULL;
		gIss_Imx104CommonObj.fvidDrvOps.getProcessedFrames = NULL;
		gIss_Imx104CommonObj.fvidDrvOps.drvId = FVID2_ISS_SENSOR_IMX104_DRV;

		status = FVID2_registerDriver ( &gIss_Imx104CommonObj.fvidDrvOps );

		if ( status != FVID2_SOK )
		{
			/*
			 * Error - free acquired resources
			 */
			Semaphore_delete ( &gIss_Imx104CommonObj.lock );
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
  System de-init for Imx104 driver

  This API
  - de-registers driver from FVID2 sub-system
  - delete's allocated semaphore locks
  - gets called as part of Iss_deviceDeInit()
*/
Int32 Iss_Imx104DeInit (  )
{
	/*
	 * Unregister FVID2 driver
	 */
	FVID2_unRegisterDriver ( &gIss_Imx104CommonObj.fvidDrvOps );

	/*
	 * Delete semaphore's
	 */
	Semaphore_delete ( &gIss_Imx104CommonObj.lock );

	return 0;
}

Int32 Iss_Imx104PinMux (  )
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

	
//	*PINCNTL74 |= 0x00000020;				// select function 6  - only works in supervisor mode
//	*PINCNTL75 |= 0x00000020;				// select function 6  - only works in supervisor mode

	*PINCNTL74 = 0xE0040;   /* i2c2_scl_mux0 */
    *PINCNTL75 = 0xE0040;   /* i2c2_sda_mux0 */
    *PINCNTL135 = 0xE0040;   /* i2c2_scl_mux0 */
    *PINCNTL136 = 0xE0040;   /* i2c2_sda_mux0 */

	*PINCNTL85 = 0x00000080;
	*PINCNTL86 = 0x00000080;
	*PINCNTL88 = 0x00000080;


	
        //gio dir for spi
	*GIO_INPUT_OUTPUT_DIR &= ~((0x1 << 16) + (0x1 << 17) +(0x1 << 26));
	*GIO_ENABLE_DISABLE_WAKEUP &= ~((0x1 << 16) + (0x1 << 17) +(0x1 << 26));
	/*resize src 0:IPIPE;1:IPIPEIF*/
	//Vps_printf("Resize input src:0x%x",*RSZ_SRC_FMT0);
	Vps_printf (" spi gio enable in %s\n",__FUNCTION__);
	return 0;
}
/*
  Handle level lock
*/
Int32 Iss_Imx104LockObj ( Iss_Imx104Obj * pObj )
{
	Semaphore_pend ( pObj->lock, BIOS_WAIT_FOREVER );
	return FVID2_SOK;
}

/*
  Handle level unlock
*/
Int32 Iss_Imx104UnlockObj ( Iss_Imx104Obj * pObj )
{
	Semaphore_post ( pObj->lock );
	return FVID2_SOK;
}

/*
  Global driver level lock
*/
Int32 Iss_Imx104Lock (  )
{
	Semaphore_pend ( gIss_Imx104CommonObj.lock, BIOS_WAIT_FOREVER );
	return FVID2_SOK;
}

/*
  Global driver level unlock
*/
Int32 Iss_Imx104Unlock (  )
{
	Semaphore_post ( gIss_Imx104CommonObj.lock );
	return FVID2_SOK;
}

/*
  Allocate driver object

  Searches in list of driver handles and allocate's a 'NOT IN USE' handle
  Also create's handle level semaphore lock

  return NULL in case handle could not be allocated
*/
Iss_Imx104Obj *Iss_Imx104AllocObj (  )
{
	UInt32 handleId;
	Iss_Imx104Obj *pObj;
	Semaphore_Params semParams;
	UInt32 found = FALSE;

	/*
	 * Take global lock to avoid race condition
	 */
	Iss_Imx104Lock (  );

    /*
     * find a unallocated object in pool
     */
	for ( handleId = 0; handleId < ISS_DEVICE_MAX_HANDLES; handleId++ )
	{
		pObj = &gIss_Imx104CommonObj.Imx104Obj[handleId];
		if ( pObj->state == ISS_IMX104_OBJ_STATE_UNUSED )
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
			pObj->state = ISS_IMX104_OBJ_STATE_IDLE;
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
				pObj->state = ISS_IMX104_OBJ_STATE_UNUSED;
			}

			break;
		}
	}

	/*
	 * Release global lock
	 */
	Iss_Imx104Unlock (  );

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
Int32 Iss_Imx104FreeObj ( Iss_Imx104Obj * pObj )
{
	/*
	 * take global lock
	 */
	Iss_Imx104Lock (  );

	if ( pObj->state != ISS_IMX104_OBJ_STATE_UNUSED )
	{
		/*
		 * mark state as unused
		 */
		pObj->state = ISS_IMX104_OBJ_STATE_UNUSED;

		/*
		 * delete object locking semaphore
		 */
		Semaphore_delete ( &pObj->lock );
	}

	/*
	 * release global lock
	 */
	Iss_Imx104Unlock (  );

	return FVID2_SOK;
}

void pintISIFRegDirectly()
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
			pdata=(uint32*) (ISIF_BASE_ADDRESS+i*4);
			Vps_printf ("Debug : Directly print ISIFReg [%d] = 0x%08x ",i, *pdata); 
		}
		tmpIndex=0x64;
	}
}
/*
  Update exposure and gain value from the 2A
*/
Int32 Iss_Imx104UpdateExpGain ( Iss_Imx104Obj * pObj, Ptr createArgs )
{
	Int32 status = FVID2_SOK;
	Int16 g_int=0;
	Uint8 regset[5];

	status=Iss_Imx104Lock (  );
	//gImx104_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();

	if( FVID2_SOK==status)
	{
		g_int = Imx104_GainTableMap(ti2a_output_params.sensorGain);
		//Vps_printf("Iss_Imx104UpdateExpGain %d %d\n", ti2a_output_params.sensorGain, g_int);

		regset[0]=g_int&0xff;
		regset[1]=(g_int>>8)&0x01;

		//imx104_setgio(regAddr,g_int);
		imx104_setgio(0x214,regset[0]);
		imx104_setgio(0x215,regset[1]);
	}
	Iss_Imx104Unlock (  );
	return status;
}


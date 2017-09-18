/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/devices/imx136_APPRO/src/issdrv_imx136Priv.h>
#include <ti/psp/devices/imx136_APPRO/issdrv_imx136_config.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif_reg.h>
#include <ti/psp/iss/hal/iss/isp/isif/inc/isif.h>
#include <ti/psp/devices/imx136_APPRO/src/imx136_reg_setting.h>
#define IMGS_SENSOR_LINEWIDTH		(1125)
//#define IMGS_SENSOR_LINEWIDTH_3MP	(1608)

#include <math.h>

#ifdef IMX136_LVDS324
#include <ti/psp/devices/src/vpsdrv_devicePriv.h>
#endif

#define WDR_MODE_SET 1

#ifdef IMX136_DATA_OP_LVDS324
//	#define  IMX136_LVDS324
#endif

static int WDR_MODE_ENABLE = 1;	//WDR_MODE_SET;

static int PreShutter = -1;
static int PreGain = -1;

#ifdef IMX136_LVDS324
static I2c_Ctrl   gImx136Lvds_I2c_ctrl;
extern Vps_DeviceObj gVps_deviceObj;
#endif

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_alg2APriv.h>

static int sensor_linewidth = IMGS_SENSOR_LINEWIDTH;

//static int gCurrsensorMode = DRV_IMGS_SENSOR_MODE_1920x1080;

Transplant_DRV_imgsFrameTime gFrameTime;

int Transplant_DRV_imgsSetEshutter(Uint32 eshutterInUsec, int setRegDirect);
int Transplant_DRV_imgsSetAgain(int again, int setRegDirect);
Int32 Iss_imx136FrameRateSet(Iss_Imx136Obj * pObj, Ptr createArgs, Ptr cmdStatusArgs);
Uint32 Iss_Imx136PwmVideoSet(Uint32 load_value, Uint32 match_value);
Uint32 Iss_Imx136PwmDcSet(Uint32 load_value, Uint32 match_value);

#if WDR_MODE_SET	// for imx136 wdr mode
int DRV_imgs_SensorMode(int IsWDR);
#endif

static void mdelay(int delay_msec)
{
    if (delay_msec == 0)
        delay_msec = 1;
    Task_sleep(delay_msec);
}

void IMX136_REG_ID2_SET(unsigned short addr, unsigned short reg)
{
	imx136_setgio( 0x0200|(0x00ff&addr), reg );
	return;
}

void IMX136_REG_ID3_SET(unsigned short addr, unsigned short reg)
{
	imx136_setgio( 0x0300|(0x00ff&addr), reg );
	return;
}

void IMX136_REG_ID4_SET(unsigned short addr, unsigned short reg)
{
	imx136_setgio( 0x0400|(0x00ff&addr), reg );
	return;
}

int DRV_imgsSetOB(int Level)
{
	//Vps_printf (" ~~~~~~ %s, Level %d ~~~~~~ \n", __FUNCTION__, Level );

	imx136_setgio( 0x20A, (Level & 0xFF) );
	imx136_setgio( 0x20B, ((Level>>8) & 0xFF) );

	return 0;
}

int InitRegArrary()
{
#if 0	// modify by jem, copy from dm368 imx136
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
#else
	//Vps_printf (" ~~~~~~ %s ~~~~~~ \n", __FUNCTION__ );

	/* Init ID2 */
	IMX136_REG_ID2_SET(0x00,0x01);// STANDBY
	IMX136_REG_ID2_SET(0x01,0x00);// REG NOT HOLD
	IMX136_REG_ID2_SET(0x02,0x01);// MASTER MODE STOP
	IMX136_REG_ID2_SET(0x03,0x01);// SW RESET START
	mdelay(50);
	IMX136_REG_ID2_SET(0x03,0x00);// SW RESET STOP
	IMX136_REG_ID2_SET(0x04,0x10);// FIXED
	IMX136_REG_ID2_SET(0x05,0x01);// AD 12BIT, NO SHIFT
	IMX136_REG_ID2_SET(0x06,0x00);// DRIVE MODE = ALL PIXEL SCAN MODE
	IMX136_REG_ID2_SET(0x07,0x10);// NO H,V REVERSE , WIN MODE = 1080p mode
	IMX136_REG_ID2_SET(0x08,0x00);// FIXED
	IMX136_REG_ID2_SET(0x09,0x02);// FRAME RATE = 30 FPS
	IMX136_REG_ID2_SET(0x0A,0xF0);// 12BIT BLACK LEVEL[0:7]
	IMX136_REG_ID2_SET(0x0B,0x00);// 12BIT BLACK LEVEL[8]
	IMX136_REG_ID2_SET(0x0C,0x00);// FIXED
	IMX136_REG_ID2_SET(0x0D,0x20);// FIXED
	IMX136_REG_ID2_SET(0x0E,0x01);// FIXED
	IMX136_REG_ID2_SET(0x0F,0x05);// FIXED

	IMX136_REG_ID2_SET(0x10,0x01);// FIXED
	IMX136_REG_ID2_SET(0x11,0x00);// FIXED
	IMX136_REG_ID2_SET(0x12,0x0F);// FIXED
	IMX136_REG_ID2_SET(0x13,0x00);// FIXED
	IMX136_REG_ID2_SET(0x14,0x00);// GAIN = 0 DB
	IMX136_REG_ID2_SET(0x15,0x00);// FIXED
	IMX136_REG_ID2_SET(0x16,0x08);// FIXED
	IMX136_REG_ID2_SET(0x17,0x00);// FIXED
	IMX136_REG_ID2_SET(0x18,0x65);// VMAX[0:7]
	IMX136_REG_ID2_SET(0x19,0x04);// VMAX[8:15] ==>1125
	IMX136_REG_ID2_SET(0x1A,0x00);// VMAX[16]
	IMX136_REG_ID2_SET(0x1B,0x30);// HMAX[0:7]
	IMX136_REG_ID2_SET(0x1C,0x11);// HMAX[8:15] ==>4400
	IMX136_REG_ID2_SET(0x1D,0x26);// FIXED
	IMX136_REG_ID2_SET(0x1E,0x02);// FIXED
	IMX136_REG_ID2_SET(0x1F,0x00);// FIXED
	IMX136_REG_ID2_SET(0x20,0x00);// SUB CONTROL [0:7]
	IMX136_REG_ID2_SET(0x21,0x00);// SUB CONTROL [8:15]
	IMX136_REG_ID2_SET(0x22,0x00);// SUB CONTROL [16]

	IMX136_REG_ID2_SET(0x38,0x3C);// WINPV[0:7] , V CROPPING POSITION
	IMX136_REG_ID2_SET(0x39,0x00);// WINPV[8:10]
	IMX136_REG_ID2_SET(0x3A,0x50);// WINWV[0:7] , V CROPPING SIZE
	IMX136_REG_ID2_SET(0x3B,0x04);// WINWV[8:10]
	IMX136_REG_ID2_SET(0x3C,0x00);// WINPH[0:7] , H CROPPING POSITION
	IMX136_REG_ID2_SET(0x3D,0x00);// WINPV[8:10]
	IMX136_REG_ID2_SET(0x3E,0x9C);// WINWH[0:7] , H CROPPING SIZE
	IMX136_REG_ID2_SET(0x3F,0x07);// WINWH[8:10]
	IMX136_REG_ID2_SET(0x40,0x00);// FIXED
	IMX136_REG_ID2_SET(0x41,0x00);// FIXED
	IMX136_REG_ID2_SET(0x42,0x00);// FIXED
	IMX136_REG_ID2_SET(0x43,0x00);// FIXED
	IMX136_REG_ID2_SET(0x44,0x01);// 12 BIT NUMBER,PARALLEL CMOS O/P
	IMX136_REG_ID2_SET(0x45,0x01);
	IMX136_REG_ID2_SET(0x46,0x00);// XVS PULSE WIDTH
	IMX136_REG_ID2_SET(0x47,0x08);// XHS PULSE MIN. O/P
	IMX136_REG_ID2_SET(0x49,0x00);// XVS AND XHS O/P FIXED AT HI

	IMX136_REG_ID2_SET(0x54,0x63);// FIXED
	IMX136_REG_ID2_SET(0x5B,0x01);// INCKSEL1
	IMX136_REG_ID2_SET(0x5C,0x20);// FIXED
	IMX136_REG_ID2_SET(0x5D,0x06);// INCKSEL2
	IMX136_REG_ID2_SET(0x5E,0x30);// FIXED
	IMX136_REG_ID2_SET(0x5F,0x04);// INCKSEL3

	/* Init ID3 */
	IMX136_REG_ID3_SET(0x0F,0x0E);
	IMX136_REG_ID3_SET(0x16,0x02);

	/* Init ID4 */
	IMX136_REG_ID4_SET(0x36,0x71);
	IMX136_REG_ID4_SET(0x39,0xF1);
	IMX136_REG_ID4_SET(0x41,0xF2);
	IMX136_REG_ID4_SET(0x42,0x21);
	IMX136_REG_ID4_SET(0x43,0x21);
	IMX136_REG_ID4_SET(0x48,0xF2);
	IMX136_REG_ID4_SET(0x49,0x21);
	IMX136_REG_ID4_SET(0x4A,0x21);
	IMX136_REG_ID4_SET(0x52,0x01);
	IMX136_REG_ID4_SET(0x54,0xB1);

	{
		/* 1080p 30fps */
		Vps_printf ("Sensor in 1080p Mode...\n");
		IMX136_REG_ID2_SET(0x5B,0x1);// INCLK1
		IMX136_REG_ID2_SET(0x5C,0x20);// INCLK2
		IMX136_REG_ID2_SET(0x5D,0x0A);// INCLK3
		IMX136_REG_ID2_SET(0x5E,0x30);// INCLK4
		IMX136_REG_ID2_SET(0x5F,0x08);//INCLK5
		IMX136_REG_ID2_SET(0x46,0x30);// XVS PULSE WIDTH=8H
		IMX136_REG_ID2_SET(0x47,0x18);// XHS PULSE =128 pixel
		IMX136_REG_ID2_SET(0x05,0x1);// AD bit 12
		IMX136_REG_ID2_SET(0x06,0x00);// all pixel
		IMX136_REG_ID2_SET(0x07,0x10);// window 1080P
		IMX136_REG_ID2_SET(0x09,0x2);// FRSEL=30 F/S
		IMX136_REG_ID2_SET(0x18,0x65);
		IMX136_REG_ID2_SET(0x19,0x04); // V = 1125
		IMX136_REG_ID2_SET(0x1B,0x30);

		IMX136_REG_ID2_SET(0x1C,0x11); // H =4400
		IMX136_REG_ID2_SET(0x44,0x1);//ODBIT 12 & Parallel CMOS
		IMX136_REG_ID2_SET(0x00,0x00);// STANDBY CANCEL
		mdelay(50);		// wait for stable
		IMX136_REG_ID2_SET(0x02,0x00);// Master mode start
		IMX136_REG_ID2_SET(0x49,0x0a);// XVS AND XHS O/P start
		mdelay(50);		// wait for stable

		Transplant_DRV_imgsSetEshutter(IMGS_SENSOR_LINEWIDTH, 1);	// exposure 1 frame ( 1/30sec)
		Transplant_DRV_imgsSetAgain(1000,1);	//1000=0db
#if WDR_MODE_SET	// for imx136 wdr mode
		DRV_imgsSetOB(0);
		DRV_imgs_SensorMode(1);
#else
		DRV_imgsSetOB(240); 	// 12bit:240, 10bit:64
#endif
	}

#endif

#if WDR_MODE_SET	// for imx136 wdr mode
	IMX136_REG_ID2_SET(0x0C,0x02);// WDMODE[1:0]
	IMX136_REG_ID2_SET(0x0F,0x05);// WDC_CMPEN

	IMX136_REG_ID2_SET(0x56,0x99);// WDC_CCMP1/WDC_CCMP2
	IMX136_REG_ID2_SET(0x57,0x55);// WDC_ACMP1/WDC_ACMP2

	IMX136_REG_ID2_SET(0x65,0x00);// Fixed

	IMX136_REG_ID2_SET(0x84,0x0F);// Fixed
	IMX136_REG_ID2_SET(0x86,0x10);// Fixed

	IMX136_REG_ID2_SET(0xCF,0xE1);// Fixed

	IMX136_REG_ID2_SET(0xD0,0x30);// Fixed
	IMX136_REG_ID2_SET(0xD2,0xc4);// Fixed
	IMX136_REG_ID2_SET(0xD3,0x01);// Fixed

	DRV_imgs_SensorMode(0);
#endif

 return 0;

}


 extern isif_regs_ovly isif_reg;

/* Global object storing all information related to all
  IMX136 driver handles */
static Iss_Imx136CommonObj 	gIss_Imx136CommonObj;
extern ti2a_output 		ti2a_output_params;

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
            break;

        case IOCTL_ISS_SENSOR_REG_WRITE:
            break;

        case IOCTL_ISS_SENSOR_REG_READ:
            break;

		case IOCTL_ISS_SENSOR_UPDATE_FRAMERATE:
		{
//			int *tmp_val = (int*)cmdArgs;
//			int *tmp_val1 = (int*)cmdStatusArgs;
			//status = Iss_imx036UpdateFrameRate(pObj, cmdArgs);
//			Vps_printf( " ~~~~~~ %s ( IOCTL_ISS_SENSOR_UPDATE_FRAMERATE, bIsNight %d (%d) ) ~~~~~~ \n", __FUNCTION__, *tmp_val, *tmp_val1 );
//			Iss_Imx136Control_setNDShutterOnOff((int)*tmp_val);
			break;
		}
        case IOCTL_ISS_SENSOR_FRAME_RATE_SET:
            status = Iss_imx136FrameRateSet(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN:
             status = Iss_Imx136UpdateExpGain ( pObj, cmdArgs);
             break;

        case IOCTL_ISS_SENSOR_PWM_CONFIG:
        {
        	Iss_SensorPwmParm *pPwmParm = (Iss_SensorPwmParm *)cmdArgs;

			//Vps_printf( " ~~~~~~ %s ( IOCTL_ISS_SENSOR_PWM_CONFIG, pPwmParm->Id %d, period %d, duty %d ) ~~~~~~ \n", __FUNCTION__, pPwmParm->Id, pPwmParm->period, pPwmParm->duty );

			if( pPwmParm->Id == ISS_SENSOR_PWM_VIDEO ) {
        		status = Iss_Imx136PwmVideoSet(pPwmParm->period, pPwmParm->duty);
        	}else if( pPwmParm->Id == ISS_SENSOR_PWM_DC ){
				status = Iss_Imx136PwmDcSet(pPwmParm->period, pPwmParm->duty);
        	}
        	break;
		}
        default:
            status = FVID2_EUNSUPPORTED_CMD;
			Vps_printf( " !!!!!! FAIL, %s ( FVID2_EUNSUPPORTED_CMD ) !!!!!! \n", __FUNCTION__ );
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
	int status = 0;
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

	return FVID2_SOK;

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
        Iss_Imx136PwmVideoSet(83320, 83320);
        Iss_Imx136PwmDcSet(83320, 41660);

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
            pObj->state = ISS_IMX136_OBJ_STATE_IDLE;
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
                pObj->state = ISS_IMX136_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /*
     * Release global lock
     */
    Iss_Imx136Unlock (  );

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
Int32 Iss_Imx136FreeObj ( Iss_Imx136Obj * pObj )
{
    /*
     * take global lock
     */
    Iss_Imx136Lock (  );

    if ( pObj->state != ISS_IMX136_OBJ_STATE_UNUSED )
    {
        /*
         * mark state as unused
         */
        pObj->state = ISS_IMX136_OBJ_STATE_UNUSED;

        /*
         * delete object locking semaphore
         */
        Semaphore_delete ( &pObj->lock );
    }

    /*
     * release global lock
     */
    Iss_Imx136Unlock (  );

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
				Vps_printf ("Debug by windsorm : global structure  isp_regs [%d] = 0x%08x ",i,*pData);
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
				Vps_printf ("Debug by windsorm : Directly print ISIFReg [%d] = 0x%08x ",i, *pdata);
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
	#ifdef IMX136_60fps

	pFrame->SW = (int)(((double)exposureTimeInUsecs*(double)Lw)/((double)66666) );
	#else
	pFrame->SW = (int)(((double)exposureTimeInUsecs*(double)Lw)/((double)33333) );
	#endif
	if(pFrame->SW<1)
	{
		pFrame->SW = 1;
	}

	return FVID2_SOK;

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

	static Uint32 pre_eshutterInUsec = 0;

	if ( pre_eshutterInUsec == eshutterInUsec )
		return status;
	pre_eshutterInUsec = eshutterInUsec;

	//Vps_printf( " ~~~~~~ %s ( %d, %d ) ~~~~~~ \n", __FUNCTION__, eshutterInUsec, setRegDirect );


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
		tempdata =0;
	}
	Send_HH=(Uint8)(tempdata>>16);
	Send_H=(Uint8)((tempdata>>8)&0x00FF);
	Send_L=(Uint8)(tempdata & 0x00FF);
		imx136_setgio(0x220,Send_L);
		imx136_setgio(0x221,Send_H);
		imx136_setgio(0x222,Send_HH);

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

	static int pre_again = -1;

	if ( pre_again == again )
		return status;
	pre_again = again;

	//Vps_printf( " ~~~~~~ %s ( %d, %d ) ~~~~~~ \n", __FUNCTION__, again, setRegDirect );

	//SetVal2 = (200*log((double)again));
	//temp = log((((double)again)/1000))/log(10);
	 temp = (200*log10(((double)again)/1000.0));
	SetVal2=(unsigned short)temp;
	//SetVal2 = temp * 100;
	/*
	if(again <= 1000)
	{
	   SetVal2 = 0;
	}
	else if(again <= 2000)
	{
	  SetVal2 = ((again - 1000) * 60)/1000;
	}
	else if(again <= 4000)
	{
	  SetVal2 = 60 + ((again - 2000) * 60)/2000;
	}
	else if(again <= 8000)
	{
	  SetVal2 = 120 + ((again - 4000) * 60)/4000;
	}
	else if(again <= 16000)
	{
	  SetVal2 = 180 + ((again - 8000) * 60)/8000;
	}
	else if(again <= 16000)
	{
	  SetVal2 = 180 + ((again - 8000) * 60)/8000;
	}



	else
	{
	  SetVal2 = 240;
	}

	if(SetVal2>240)
	{
		SetVal2=240;
	}
	*/
	if (SetVal2>420)
	{
		SetVal2=420;
	}

	//Vps_printf("Again reg value :%f, 0x%x, again : %d \n",temp,SetVal2,again);
	//Vps_printf("Again reg value : 0x%x, again : %d \n",SetVal2,again);
	regset[0]=SetVal2&0xff;
	regset[1]=(SetVal2>>8)&0x01;

	imx136_setgio(0x214,regset[0]);
	imx136_setgio(0x215,regset[1]);


	return status;
}

//for appro 2A
#define IMX136_REG_ID2_SET_Multi(X,pY,YC)		IMX136_REG_SET_Multi(0x02,X,pY,YC)
#define IMX136_REG_ID3_SET_Multi(X,pY,YC)		IMX136_REG_SET_Multi(0x03,X,pY,YC)
#define IMX136_REG_ID4_SET_Multi(X,pY,YC)		IMX136_REG_SET_Multi(0x04,X,pY,YC)

#define LIMIT(XL,X,XH)	(((X)>(XL))?((X<XH)?(X):(XH)):(XL))

//for appro 2A
int IMX136_REG_SET_Multi(unsigned char IdAddress, unsigned char RegAddr, unsigned char *pVal, unsigned char ValCnt)
{
	unsigned short tmp_reg_addr;
	int	status = FVID2_SOK, tmp_cnt;

	if ( (RegAddr+ValCnt) > 0x100 ) {
		Vps_printf( "\n !!!!!! FAIL, %s ( IdAddress %d, RegAddr %d, ValCnt %d ) !!!!!! \n\n", __FUNCTION__, IdAddress, RegAddr, ValCnt );
		return FVID2_EFAIL;
	}

	tmp_reg_addr = (IdAddress<<8)|(RegAddr);

	for ( tmp_cnt = 0; tmp_cnt<ValCnt; tmp_cnt++ ) {
		//Vps_printf( " ~~~~~~ %s ( reg_addr 0x%x, val %d ) ~~~~~~ \n", __FUNCTION__, (tmp_reg_addr+tmp_cnt), *(pVal+tmp_cnt) );
		imx136_setgio( (tmp_reg_addr+tmp_cnt), *(pVal+tmp_cnt) );
	}

	return status;
}

//for appro 2A
int DRV_imgsSetAgain ( int again )
{
	int	status = FVID2_SOK;
	Uint8 regset[8];
	int SetVal;

	if ( PreGain == again )
		return 0;

	PreGain = again;

//	Vps_printf( " ( %s ( %d ) ) \n", __FUNCTION__, again );

	/* 0db~42db , 0.1db/step */
	//printf("AG:\t%d\n",again);
	if(again<10000 || again>1280000){
		Vps_printf ( "\nXXXXX DRV_imgsSetAgain() Error(%d)\n\n", again );
		return status;
	}
	SetVal = (int)(60*((log10((float)again/10000))/(float)log10(2))+0.001);
	//printf("GainX10 = %d\n",SetVal);
	SetVal = LIMIT(0,SetVal,420);

	regset[0] = (SetVal & 0xFF);
	regset[1] = (SetVal>>8) & 0xFF;
	status = IMX136_REG_ID2_SET_Multi(0x14,regset,2);

	return status;
}

//for appro 2A
int DRV_imgsSetEshutter ( Uint32 Eshutter )
{
	Uint32 SetValue,SUB_Control=0;
	Uint32 tempdata;
	int count=0, Line;
	Uint8 regset[8];

	if ( PreShutter == Eshutter )
		return 0;

	PreShutter = Eshutter;

//	Vps_printf( " ( %s ( %d ) ) \n", __FUNCTION__, Eshutter );

	SetValue = Eshutter;
	//Line = IMGS_SENSOR_LINEWIDTH;
	Line = sensor_linewidth;

	if(SetValue<Line){
		SUB_Control = Line - SetValue;
	}else{
		SUB_Control = 0;
	}

#if WDR_MODE_SET	// for imx136 wdr mode
	{
		static int pre_wdr_mode_enable = -1;

		if ( pre_wdr_mode_enable != WDR_MODE_ENABLE && WDR_MODE_ENABLE == 0 ) {
			pre_wdr_mode_enable = WDR_MODE_ENABLE;

			regset[0] =  0;
			regset[1] =  0;
			regset[2] =  0;

			IMX136_REG_ID2_SET_Multi(0x23,regset,3);//long exposure
		}
	}
	if ( WDR_MODE_ENABLE )
	{
		Uint8 Send_HH, Send_H, Send_L;
		Uint32 SetValue2 = 0, SUB_Control2 = 0;

		Send_HH=(Uint8)(SUB_Control>>16);
		Send_H=(Uint8)((SUB_Control>>8)&0x00FF);
		Send_L=(Uint8)(SUB_Control & 0x00FF);

		regset[0] = Send_L;
		regset[1] = Send_H;
		regset[2] = Send_HH;

		IMX136_REG_ID2_SET_Multi(0x23,regset,3);//long exposure

		SetValue2 = (SetValue+15)/16;
		if ( SetValue < Line ) {
			SUB_Control2 = Line - SetValue2;
		} else {
			SUB_Control2 = SetValue - SetValue2;
		}

		Send_HH=(Uint8)((SUB_Control2)>>16);
		Send_H=(Uint8)(((SUB_Control2)>>8)&0x00FF);
		Send_L=(Uint8)((SUB_Control2) & 0x00FF);

		regset[0] = Send_L;
		regset[1] = Send_H;
		regset[2] = Send_HH;

		IMX136_REG_ID2_SET_Multi(0x20,regset,3);//short exposure

		//Vps_printf(" %s, SUB_Control = 0x%x SUB_Control2 = 0x%x Line = %d \n", __FUNCTION__, SUB_Control,SUB_Control2,Line);
		//Vps_printf(" %s, Eshutter = %d SetValue = %d   \n", __FUNCTION__, Eshutter,SetValue);
	} else
#endif	// for imx136 wdr mode
	{
		count = 0;
		regset[count++] =  (Uint8)(SUB_Control & 0x00FF);
		regset[count++] =  (Uint8)((SUB_Control>>8)&0x00FF);
		regset[count++] =  (Uint8)(SUB_Control>>16);

		IMX136_REG_ID2_SET_Multi(0x20, regset, count );
	}

	if(SetValue<Line){
		tempdata = Line;
	}else{
		tempdata = SetValue;
	}
	count = 0;
	regset[count++] =  (Uint8)(tempdata & 0x00FF);
	regset[count++] =  (Uint8)((tempdata>>8)&0x00FF);
	regset[count++] =  (Uint8)(tempdata>>16);

	IMX136_REG_ID2_SET_Multi(0x18, regset, count );

	return 0;
}

/*
  Update exposure and gain value from the 2A
*/
Int32 Iss_Imx136UpdateExpGain ( Iss_Imx136Obj * pObj, Ptr createArgs )
{
	Int32 status = FVID2_SOK;

	//Int16 tmpGain=0;
	status=Iss_Imx136Lock (  );
	//gImx136_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();


	if( FVID2_SOK==status)
	{

        Int32 aewbVendor;

        aewbVendor = (Int32)IssAlg_capt2AGetAEWBVendor();

        if (aewbVendor == 1) {

			DRV_imgsSetAgain ( ti2a_output_params.sensorGain );
			DRV_imgsSetEshutter ( ti2a_output_params.sensorExposure );

        } else if (aewbVendor == 2) {
			//Int16 tmpEshutter=0 ;
			//Int16 regAddr= 0x0020;
			//tmpGain=ti2a_output_params.sensorGain;
			//tmpEshutter=ti2a_output_params.sensorExposure;
			//if(tmpGain>1000)
			//{
				//tmpEshutter=tmpGain/1000;
			//}

			//Vps_printf("Iss_Imx136UpdateExpGain %d\n", tmpEshutter);
			Transplant_DRV_imgsSetAgain(ti2a_output_params.sensorGain,1);
			//Transplant_DRV_imgsSetEshutter(ti2a_output_params.sensorExposure*tmpEshutter,0);
			Transplant_DRV_imgsSetEshutter(ti2a_output_params.sensorExposure,0);
			//g_int = Imx136_GainTableMap(ti2a_output_params.sensorGain);
			//imx136_setgio(regAddr,g_int);
        }
	}
	Iss_Imx136Unlock (  );

	return status;
}

/* ===================================================================
 *  @func     Iss_imx136FrameRateSet
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
Int32 Iss_imx136FrameRateSet(Iss_Imx136Obj * pObj, Ptr createArgs,
                              Ptr cmdStatusArgs)
{
    Int32 status = FVID2_SOK;


    Int32 framerate = *(Int32 *) createArgs;


    if (framerate == 60)
    {

        *(Int32 *) cmdStatusArgs = IMGS_SENSOR_LINEWIDTH/2;
    }
    else if (framerate == 50)
    {

        *(Int32 *) cmdStatusArgs = IMGS_SENSOR_LINEWIDTH*30/50;
    }
    else if (framerate == 30)
    {

        *(Int32 *) cmdStatusArgs = IMGS_SENSOR_LINEWIDTH;
    }
    else if (framerate == 25)
    {

        *(Int32 *) cmdStatusArgs = IMGS_SENSOR_LINEWIDTH*30/25;
    }

    else
    {
    	*(Int32 *) cmdStatusArgs = IMGS_SENSOR_LINEWIDTH;
        status = FVID2_EFAIL;
    }

#if 1
	if ( status == FVID2_SOK ) {
	    /** take global lock **/
	    Iss_Imx136Lock();

	    sensor_linewidth = *(Int32 *) cmdStatusArgs;

	    /** release global lock **/
	    Iss_Imx136Unlock();
	}
#endif

	//Vps_printf( " ~~~~~~ %s ( framerate %d, LINE WIDTH %d, status %d ) ~~~~~~ \n", __FUNCTION__, *(Int32 *) cmdStatusArgs,framerate, status );

    return status;
}

//for autoiris control (pwm).
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

Uint32 Iss_Imx136PwmVideoSet(Uint32 load_value, Uint32 match_value)
{
	Uint32	TimerID = 6;
	Uint32  *TimerLoadReg = (Uint32  *)(TimerBaseAddr(TimerID)+0x40);
	Uint32  *TimerMatchReg = (Uint32  *)(TimerBaseAddr(TimerID)+0x4C);

#if defined(TI_814X_BUILD)
	*TimerLoadReg = DM_TIMER_LOAD_MIN-load_value;
	*TimerMatchReg = DM_TIMER_LOAD_MIN-(match_value);
#else
	*TimerLoadReg = DM_TIMER_LOAD_MIN-load_value;
	*TimerMatchReg = DM_TIMER_LOAD_MIN-(load_value-match_value);
#endif
	return FVID2_SOK;
}

Uint32 Iss_Imx136PwmDcSet(Uint32 load_value, Uint32 match_value)
{
	Uint32	TimerID = 7;
	Uint32  *TimerLoadReg = (Uint32  *)(TimerBaseAddr(TimerID)+0x40);
	Uint32  *TimerMatchReg = (Uint32  *)(TimerBaseAddr(TimerID)+0x4C);

#if defined(TI_814X_BUILD)
	*TimerLoadReg = DM_TIMER_LOAD_MIN-load_value;
	*TimerMatchReg = DM_TIMER_LOAD_MIN-(match_value);
#else
	*TimerLoadReg = DM_TIMER_LOAD_MIN-load_value;
	*TimerMatchReg = DM_TIMER_LOAD_MIN-(load_value-match_value);
#endif

	return FVID2_SOK;
}

#if WDR_MODE_SET	// for imx136 wdr mode
int DRV_imgs_SensorMode(int IsWDR)
{
	int tmp_est, tmp_agc;

	//Vps_printf ( " ~~~~~~ %s, IsWDR %d ~~~~~ \n", __FUNCTION__, IsWDR );

	if(IsWDR == 1)
	{
		IMX136_REG_ID2_SET(0x0C,0x02);// WDC_CMPEN
		IMX136_REG_ID2_SET(0x0F,0x05);// WDC_CMPEN

		IMX136_REG_ID2_SET(0x10,0x00);// FIXED
		IMX136_REG_ID2_SET(0x11,0x00);// FIXED
		IMX136_REG_ID2_SET(0x12,0x2d);// FIXED

		IMX136_REG_ID2_SET(0x65,0x00);// WDC_CMPEN

		WDR_MODE_ENABLE = 1;
	}else{
		IMX136_REG_ID2_SET(0x0C,0x00);// WDC_CMPEN
		IMX136_REG_ID2_SET(0x0F,0x00);// WDC_CMPEN

		IMX136_REG_ID2_SET(0x10,0x01);// FIXED
		IMX136_REG_ID2_SET(0x11,0x00);// FIXED
		IMX136_REG_ID2_SET(0x12,0xF0);// FIXED

		IMX136_REG_ID2_SET(0x65,0x20);// WDC_CMPEN

		WDR_MODE_ENABLE = 0;
	}
 	if ( PreShutter >= 0 && PreShutter<(IMGS_SENSOR_LINEWIDTH*16) )
 		tmp_est = PreShutter;
	else
		tmp_est = IMGS_SENSOR_LINEWIDTH;
	PreShutter = -1;
	DRV_imgsSetEshutter(tmp_est);

	if ( PreGain >= 1000 && PreGain<128000 )
		tmp_agc = PreGain;
	else
		tmp_agc = 1000;
	PreGain = -1;
	DRV_imgsSetAgain(tmp_agc);

	return 0;
}
#endif



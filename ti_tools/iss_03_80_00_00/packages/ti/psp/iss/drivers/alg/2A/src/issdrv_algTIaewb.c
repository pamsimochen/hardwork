/** ==================================================================
 *  @file   issdrv_algTIaewb.c
 *
 *  @path    /proj/vsi/users/venu/DM812x/IPNetCam_rel_1_8/ti_tools/iss_02_bkup/packages/ti/psp/iss/drivers/alg/2A/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/


#include <string.h>

#include <stdlib.h>
#include <stdio.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algAewbPriv.h>
#include "alg_ti_aewb_priv.h"
#include "ae_ti.h"
#include "awb_ti.h"
#include "TI_aewb.h"
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>
//#include "Rfile.h"
#include <ti/psp/vps/common/vps_config.h>
#include "alg_ti_flicker_detect.h"
ALG_AewbObj gALG_TI_aewbObj;

ti2aControlParams_t gTi2aControlParams;

int new_awb_data_available;

int *g_flickerMem 		= NULL; //algorithm persistent memory
IAEWB_Rgb *rgbData     	= NULL;
aewDataEntry *aew_data 	= NULL;
int aew_enable         	= AEW_ENABLE;
int aewbFrames         	= 0;
int awb_alg 			= TI_DSPRND_AWB;//TI_VSP_AWB; //TI_DSPRND_AWB;

typedef enum{
	DRV_IMGS_SENSOR_MODE_640x480 = 0,
	DRV_IMGS_SENSOR_MODE_720x480,
	DRV_IMGS_SENSOR_MODE_800x600,
	DRV_IMGS_SENSOR_MODE_1024x768,
	DRV_IMGS_SENSOR_MODE_1280x720,
	DRV_IMGS_SENSOR_MODE_1280x960,
	DRV_IMGS_SENSOR_MODE_1280x1024,
	DRV_IMGS_SENSOR_MODE_1600x1200,
	DRV_IMGS_SENSOR_MODE_1920x1080,
	DRV_IMGS_SENSOR_MODE_2048x1536,
	DRV_IMGS_SENSOR_MODE_2592x1920
} DRV_IMGS_SENSOR_MODE;


#define FDC_ENABLED  1
#define FDC_DISABLED 0

#define DO_2A 1
#define NO_2A 0

//#define _PROFILE_AWB_ALGO_
#ifdef _PROFILE_AWB_ALGO_
  volatile uint32              awb_start;
  volatile uint32              awb_end;
  extern volatile uint32       overhead;
  extern volatile uint32       numTicksPerMilSec;
  #include <xdc/runtime/Timestamp.h>
#endif

ti2a_output ti2a_output_params = {
  1000,
  2,
  5000,
  50,
  1,
  0,
  {
    128,
    128,
    128,
    128,
    0,
    0,
    0,
    0,
    512
  },
  {
        256, 0,    0,
        0,   256,  0,
        0,   0,    256,
        0,   0,    0
  },
  {
        256, 0,    0,
        0,   256,  0,
        0,   0,    256,
        0,   0,    0
  }
};

ti2a_output ti2a_output_params_prev = {
  1000,
  2,
  5000,
  50,
  1,
  0,
  {
    128,
    128,
    128,
    128,
    0,
    0,
    0,
    0,
   512
  },
  {
        256, 0,    0,
        0,   256,  0,
        0,   0,    256,
        0,   0,    0
  },
  {
        256, 0,    0,
        0,   256,  0,
        0,   0,    256,
        0,   0,    0
  }
};

/* ===================================================================
 *  @func     ALG_aewbCreate
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
void *ALG_aewbCreate(int aewbNumWinH,int aewbNumWinV,int aewbNumPix)
{
  IAE_Params aeParams;
  IAWB_Params awbParams;

  int numMem;
  int retval;

  memset(&gALG_TI_aewbObj, 0, sizeof(gALG_TI_aewbObj));

  if(rgbData == NULL)
	rgbData  = calloc(sizeof(IAEWB_Rgb), aewbNumWinH * aewbNumWinV);
  if(aew_data == NULL)
	aew_data = calloc(sizeof(aewDataEntry), (aewbNumWinH * aewbNumWinV + 7) >> 3);
  if(g_flickerMem == NULL)
      g_flickerMem = calloc(sizeof(int), 6*1024);

  gALG_TI_aewbObj.reduceShutter          = 100;
  gALG_TI_aewbObj.saldre                 = 0;
  gALG_TI_aewbObj.aewbType               = ALG_AEWB_AEWB;
  gALG_TI_aewbObj.env_50_60Hz            =  VIDEO_PAL;
  gALG_TI_aewbObj.flicker_detect         = FDC_DISABLED;

  gTi2aControlParams.update              = 0;
  gTi2aControlParams.flicker_sel         = 0;

  gTi2aControlParams.flickerFreq         = 0;
  gTi2aControlParams.minExposure         = 5000;
  gTi2aControlParams.maxExposure         = 16667;
  gTi2aControlParams.stepSize            = 200;
  gTi2aControlParams.aGainMin            = 1000;
  gTi2aControlParams.dGainMin            = 128;		//1024;		//128
  gTi2aControlParams.dGainMax            = 4092;
  gTi2aControlParams.targetBrightnessMin = 30;
  gTi2aControlParams.targetBrightnessMax = 50;
  gTi2aControlParams.targetBrightness    = 40;

#ifdef IMGS_ALTASENS_AL30210
  gTi2aControlParams.maxExposure         = 33333;
#elif defined IMGS_PANASONIC_MN34041
  gTi2aControlParams.minExposure         = 1000;
  gTi2aControlParams.maxExposure         = 32000;
  gTi2aControlParams.stepSize            = 200;
  gTi2aControlParams.aGainMin            = 1000;
  gTi2aControlParams.aGainMax            = 32000;
  gTi2aControlParams.dGainMax            = 4092 * 2;
#elif defined IMGS_MICRON_AR0331_WDR
  gTi2aControlParams.aGainMax            = 2000;
  gTi2aControlParams.minExposure         = 10000;		//5000;		//100
  gTi2aControlParams.stepSize            = 50;		//200;		//50
  gTi2aControlParams.aGainMin            = 100;		//1000;		//100
  gTi2aControlParams.maxExposure         = 20000; //WDR
#elif defined (IMGS_MICRON_AR0331) | defined (IMGS_MICRON_AR0330)
  gTi2aControlParams.aGainMax            = 8000;
  gTi2aControlParams.minExposure         = 100;		//5000;		//100
  gTi2aControlParams.stepSize            = 50;		//200;		//50
  gTi2aControlParams.aGainMin            = 100;		//1000;		//100
  gTi2aControlParams.maxExposure         = 33333; //16667;
#ifdef WDR_ON
	gTi2aControlParams.dGainMin 	   = 1024;
	gTi2aControlParams.dGainMax 	   = 1024;
	gTi2aControlParams.minExposure     = 237;
	gTi2aControlParams.maxExposure     = 33185;
#endif
#elif defined IMGS_ALTASENS_AL30210
  gTi2aControlParams.aGainMax            = 5600;
#elif defined IMGS_SONY_IMX104
  gTi2aControlParams.dGainMin            = 1024;		//1024;		//128
  gTi2aControlParams.minExposure         = 100;		//5000;		//100
  gTi2aControlParams.maxExposure         = 33333;
  gTi2aControlParams.dGainMax            = 8092;
  gTi2aControlParams.aGainMax            = 251000;
#ifdef WDR_ON
  gTi2aControlParams.minExposure         = 7155;
  gTi2aControlParams.maxExposure         = 32755;
  gTi2aControlParams.dGainMin            = 1024;
  gTi2aControlParams.dGainMax 		     = 1024;
  gTi2aControlParams.aGainMin            = 1679;//4.5dB
  gTi2aControlParams.aGainMax            = 6683;//16.5dB
#endif
#elif defined (IMGS_SONY_IMX136) | defined (IMGS_SONY_IMX140)
  gTi2aControlParams.dGainMin            = 1024;		//128
  gTi2aControlParams.aGainMax            = 256000; //251000;
  gTi2aControlParams.dGainMax            = 1024; //4092 * 2;
  gTi2aControlParams.maxExposure         = 33333;
#ifdef WDR_ON
  gTi2aControlParams.minExposure         = 4740;
  gTi2aControlParams.maxExposure         = 33184;
  gTi2aControlParams.dGainMin 		   = 1024;
  gTi2aControlParams.dGainMax 		   = 1024;
  gTi2aControlParams.aGainMin            = 1679;//4.5dB
  gTi2aControlParams.aGainMax            = 6683;//16.5dB
#endif
#elif defined IMGS_SONY_IMX122
  gTi2aControlParams.dGainMin            = 1024;		//128
  gTi2aControlParams.aGainMax            = 251000;
  gTi2aControlParams.dGainMax            = 4092 * 2;
  gTi2aControlParams.maxExposure         = 33333;
  gTi2aControlParams.flickerFreq         = 50000;
#elif defined IMGS_MICRON_MT9M034
  gTi2aControlParams.dGainMin            = 1024;		//1024;		//128
  gTi2aControlParams.dGainMax            = 8092;
#ifdef WDR_ON
  gTi2aControlParams.minExposure         = 4300;//100;		//5000;		//100
  gTi2aControlParams.maxExposure         = 22100;//33333;
  gTi2aControlParams.dGainMin 		   = 1024;
  gTi2aControlParams.dGainMax 		   = 1024;
#else
  gTi2aControlParams.minExposure         = 100;		//5000;		//100
  gTi2aControlParams.maxExposure         = 33333;
#endif
  gTi2aControlParams.aGainMax            = 230000;  // 2.88(DCG) * 7.96875(D) * 8(A)
#else
  gTi2aControlParams.aGainMax            = 32000;
#endif


  gTi2aControlParams.aewbType            = ALG_AEWB_AEWB;

  gTi2aControlParams.day_night           = AE_DAY;
  gTi2aControlParams.blc                 = BACKLIGHT_LOW;
  gTi2aControlParams.brightness          = 128;
  gTi2aControlParams.contrast            = 128;
  gTi2aControlParams.sharpness           = 128;
  gTi2aControlParams.saturation          = 128;
  gTi2aControlParams.wbSceneMode         = AWB_AUTO;

  new_awb_data_available = 0;


  TI_2A_init_tables(aewbNumWinH, aewbNumWinV);
  //Initial AE
  gALG_TI_aewbObj.weight = TI_WEIGHTING_MATRIX;
  aeParams.size = sizeof(aeParams);
  aeParams.numHistory = 10;
  aeParams.numSmoothSteps = 1;
  numMem = AE_TI_AE.ialg.algAlloc((IALG_Params *)&aeParams, NULL, gALG_TI_aewbObj.memTab_ae);
  while(numMem > 0){
    gALG_TI_aewbObj.memTab_ae[numMem-1].base = malloc(gALG_TI_aewbObj.memTab_ae[numMem-1].size);
    numMem --;
  }

  gALG_TI_aewbObj.handle_ae = (IALG_Handle)gALG_TI_aewbObj.memTab_ae[0].base;
  retval = AE_TI_AE.ialg.algInit(gALG_TI_aewbObj.handle_ae, gALG_TI_aewbObj.memTab_ae, NULL, (IALG_Params *)&aeParams);
  if(retval == -1) {
    OSA_ERROR("AE_TI_AE.ialg.algInit()\n");
    return NULL;
  }

  //Initial AWB
  awbParams.size = sizeof(awbParams);
  awbParams.numHistory = 6;
  numMem = AWB_TI_AWB.ialg.algAlloc((IALG_Params *)&awbParams, NULL, gALG_TI_aewbObj.memTab_awb);
  while(numMem > 0){
    gALG_TI_aewbObj.memTab_awb[numMem-1].base = malloc(gALG_TI_aewbObj.memTab_awb[numMem-1].size);
    numMem --;
  }

  gALG_TI_aewbObj.handle_awb = (IALG_Handle)gALG_TI_aewbObj.memTab_awb[0].base;
  retval = AWB_TI_AWB.ialg.algInit(gALG_TI_aewbObj.handle_awb, gALG_TI_aewbObj.memTab_awb, NULL, (IALG_Params *)&awbParams);
  if(retval == -1) {
    OSA_ERROR("AWB_TI_AWB.ialg.algInit()\n");
    return NULL;
  }

  gALG_TI_aewbObj.IAEWB_StatMatdata.winCtVert  = aewbNumWinV;
  gALG_TI_aewbObj.IAEWB_StatMatdata.winCtHorz  = aewbNumWinH;
  gALG_TI_aewbObj.IAEWB_StatMatdata.pixCtWin   = aewbNumPix;
  gALG_TI_aewbObj.sensorMode = DRV_IMGS_SENSOR_MODE_1920x1080;

  retval = TI_2A_config(gALG_TI_aewbObj.flicker_detect, gALG_TI_aewbObj.saldre);
  if(retval == -1) {
    return NULL;
  }

  return &gALG_TI_aewbObj;
}

IAE_DynamicParams aeDynamicParams;

/* ===================================================================
 *  @func     TI_Increase_Exposure
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
void TI_Increase_Exposure()
{
  if(((ti2a_output_params.sensorExposure + 500) <= aeDynamicParams.exposureTimeRange[0].max))
  {
    ti2a_output_params.sensorExposure += 500;
    ti2a_output_params.mask = 1;
  }
  else if(((ti2a_output_params.sensorGain + 100) <= aeDynamicParams.sensorGainRange[1].max))
  {
    ti2a_output_params.sensorGain += 100;
    ti2a_output_params.mask = 2;
  }
  else if((((ti2a_output_params.ipipe_awb_gain.dGain + 8) << 2) <= aeDynamicParams.ipipeGainRange[2].max))
  {
    ti2a_output_params.ipipe_awb_gain.dGain += 8;
    ti2a_output_params.mask = 4;
  }
#ifdef ALG_AEWB_DEBUG
  Vps_printf("sensor Exposure Time   :  < %5d > \n",ti2a_output_params.sensorExposure  );
  Vps_printf("Sensor Analog Gain     :  < %5d > \n",ti2a_output_params.sensorGain );
  Vps_printf("IPIPE Digital Gain     :  < %5d > \n",(ti2a_output_params.ipipe_awb_gain.dGain * 2) );
#endif
}
/* ===================================================================
 *  @func     TI_Decrease_Exposure
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
void TI_Decrease_Exposure()
{
  if((((ti2a_output_params.ipipe_awb_gain.dGain - 8) << 2) >= aeDynamicParams.ipipeGainRange[2].min))
  {
    ti2a_output_params.ipipe_awb_gain.dGain -= 8;
    ti2a_output_params.mask = 4;
  }
  else if(((ti2a_output_params.sensorGain - 100) >= aeDynamicParams.sensorGainRange[1].min))
  {
    ti2a_output_params.sensorGain -= 100;
    ti2a_output_params.mask = 2;
  }
  else if(((ti2a_output_params.sensorExposure - 500) >= aeDynamicParams.exposureTimeRange[0].min))
  {
    ti2a_output_params.sensorExposure -= 500;
    ti2a_output_params.mask = 1;
  }
#ifdef ALG_AEWB_DEBUG
  Vps_printf("sensor Exposure Time   :  < %5d > \n",ti2a_output_params.sensorExposure  );
  Vps_printf("Sensor Analog Gain     :  < %5d > \n",ti2a_output_params.sensorGain );
  Vps_printf("IPIPE Digital Gain     :  < %5d > \n",(ti2a_output_params.ipipe_awb_gain.dGain * 2) );
#endif
}

/* ===================================================================
 *  @func     TI_2A_config
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
int TI_2A_config(int flicker_detection, int saldre)
{

  int i, stepSize, min_exp;
  int retval;
  aeDynamicParams.size = sizeof(aeDynamicParams);
  aeDynamicParams.numRanges = 0;

  i = 0; aewbFrames = 0;


  /* set stepSize based on input from Flicker detectiom and PAL/NTSC environment */
  if(flicker_detection == 1)
  {
    if(gALG_TI_aewbObj.env_50_60Hz == VIDEO_NTSC)
      stepSize = 8333;
    else
      stepSize = 10000;

    min_exp  = stepSize;
  }
  else
  {
    stepSize = 200;
    min_exp  = 5000;
  }


  if(gALG_TI_aewbObj.env_50_60Hz == VIDEO_NTSC && flicker_detection == 3){
    min_exp = 8333;
    stepSize = (8333*gALG_TI_aewbObj.reduceShutter)/100;
  }
  else if(gALG_TI_aewbObj.env_50_60Hz == VIDEO_PAL && flicker_detection == 2){
    min_exp = 10000;
    stepSize = 10000;
  }

  if (flicker_detection == 0)
  {
    if (gTi2aControlParams.flickerFreq == 0)
    {
      min_exp = gTi2aControlParams.minExposure;
      stepSize = gTi2aControlParams.stepSize ;
    }
    else
    {
      min_exp = ((500000)/gTi2aControlParams.flickerFreq);

      if (min_exp < gTi2aControlParams.minExposure)
      {
          min_exp =  gTi2aControlParams.minExposure + 1;
      }

      if (min_exp > gTi2aControlParams.maxExposure)
      {
          min_exp =  gTi2aControlParams.maxExposure - 1;
      }
      stepSize = min_exp;

    }
  }


#ifdef ALG_AEWB_DEBUG
  Vps_printf("min_exp = %d, stepSize = %d\n", min_exp, stepSize);
#endif
  if (flicker_detection != 0)
  {
    ti2a_output_params.sensorExposure = min_exp;
  }
  aeDynamicParams.numRanges ++;
  aeDynamicParams.exposureTimeRange[i].min    = min_exp;
  aeDynamicParams.exposureTimeRange[i].max    = gTi2aControlParams.maxExposure;
  aeDynamicParams.apertureLevelRange[i].min   = 0;
  aeDynamicParams.apertureLevelRange[i].max   = 0;
  aeDynamicParams.sensorGainRange[i].min      = 1000;
  aeDynamicParams.sensorGainRange[i].max      = 1000;
  aeDynamicParams.ipipeGainRange[i].min       = 1024;
  aeDynamicParams.ipipeGainRange[i].max       = 1024;
  i++;

  aeDynamicParams.numRanges ++;
  aeDynamicParams.exposureTimeRange[i].min    = 0;
  aeDynamicParams.exposureTimeRange[i].max    = 0;
  aeDynamicParams.apertureLevelRange[i].min   = 0;
  aeDynamicParams.apertureLevelRange[i].max   = 0;
  aeDynamicParams.sensorGainRange[i].min      = gTi2aControlParams.aGainMin ;
  aeDynamicParams.sensorGainRange[i].max      = gTi2aControlParams.aGainMax ;
  aeDynamicParams.ipipeGainRange[i].min       = 0;
  aeDynamicParams.ipipeGainRange[i].max       = 0;
  i++;
  aeDynamicParams.numRanges ++ ;
  aeDynamicParams.exposureTimeRange[i].min    = 0;
  aeDynamicParams.exposureTimeRange[i].max    = 0;
  aeDynamicParams.apertureLevelRange[i].min   = 0;
  aeDynamicParams.apertureLevelRange[i].max   = 0;
  aeDynamicParams.sensorGainRange[i].min      = 0;
  aeDynamicParams.sensorGainRange[i].max      = 0;
  aeDynamicParams.ipipeGainRange[i].min       = gTi2aControlParams.dGainMin;// 4
  aeDynamicParams.ipipeGainRange[i].max       = gTi2aControlParams.dGainMax ; //saldre?1024:4092;
  aeDynamicParams.targetBrightnessRange.min   = gTi2aControlParams.targetBrightnessMin ;
  aeDynamicParams.targetBrightnessRange.max   = gTi2aControlParams.targetBrightnessMax ;
  aeDynamicParams.targetBrightness            = gTi2aControlParams.targetBrightness;
  aeDynamicParams.thrld                       = 10;
  aeDynamicParams.exposureTimeStepSize        = stepSize;

  memcpy( (void *)&gALG_TI_aewbObj.AE_InArgs.statMat,
    (void *)&gALG_TI_aewbObj.IAEWB_StatMatdata,
    sizeof(IAEWB_StatMat) );

  memcpy( (void *)&gALG_TI_aewbObj.AWB_InArgs.statMat,
    (void *)&gALG_TI_aewbObj.IAEWB_StatMatdata,
    sizeof(IAEWB_StatMat) );

  retval = AE_TI_AE.control((IAE_Handle)gALG_TI_aewbObj.handle_ae,
                      IAE_CMD_SET_CONFIG, &aeDynamicParams, NULL);
  if(retval == -1) {
    OSA_ERROR("AE_TI_AE.control()\n");
    return retval;
  }

  if(flicker_detection == 1) ti2a_output_params.sensorExposure = 5000;

  /* Pass calibration data to TI AWB */
  retval = AWB_TI_AWB.control((IAWB_Handle)gALG_TI_aewbObj.handle_awb,
                          TIAWB_CMD_CALIBRATION, (IAWB_DynamicParams*)&awb_calc_data, NULL);

  if(retval == -1) {
    OSA_ERROR("AWB_TI_AWB.control()\n");
    return retval;
  }

  return 0;
}

#ifdef ALG_AEWB_DEBUG
static void H3A_DEBUG_PRINT()
{
	int i,j,cnt;
	static int ShowCnt=0;
	char line_buffer[200];
	char word_buffer[20];
 
	ShowCnt ++;
	if(ShowCnt == 600)
	{	
		Vps_printf("pixCtWin=%d,WinVNum=%d,WinHNum=%d,\n",
			gALG_TI_aewbObj.IAEWB_StatMatdata.pixCtWin,
			gALG_TI_aewbObj.IAEWB_StatMatdata.winCtVert,
			gALG_TI_aewbObj.IAEWB_StatMatdata.winCtHorz);
		ShowCnt = 0;
		Vps_printf("\n******* H3A DUMP Start ********\n");
		Vps_printf("=====R Data=====\n");
		cnt=0;
		for(i=0; i< gALG_TI_aewbObj.IAEWB_StatMatdata.winCtVert; i++)
		{
			sprintf(line_buffer,"[%3d]",i);
			for(j=0;j< gALG_TI_aewbObj.IAEWB_StatMatdata.winCtHorz; j++) {
				sprintf(word_buffer,"%5u, ",rgbData[cnt+j ].r);
				strcat(line_buffer,word_buffer);
			}
			Vps_printf("%s",line_buffer);
			cnt=cnt+gALG_TI_aewbObj.IAEWB_StatMatdata.winCtHorz; 
		}

		Vps_printf("=====G Data=====\n");
		cnt=0;
		for(i=0; i< gALG_TI_aewbObj.IAEWB_StatMatdata.winCtVert; i++)
		{
			sprintf(line_buffer,"[%3d]",i);
			for(j=0;j< gALG_TI_aewbObj.IAEWB_StatMatdata.winCtHorz; j++) {
				sprintf(word_buffer,"%5u, ",rgbData[cnt+j].g);
				strcat(line_buffer,word_buffer);
			}
			Vps_printf("%s",line_buffer);
			cnt=cnt+gALG_TI_aewbObj.IAEWB_StatMatdata.winCtHorz; 
		}
		Vps_printf("=====B Data=====\n");
		cnt=0;
		for(i=0; i< gALG_TI_aewbObj.IAEWB_StatMatdata.winCtVert; i++)
		{
			sprintf(line_buffer,"[%3d]",i);
			for(j=0;j< gALG_TI_aewbObj.IAEWB_StatMatdata.winCtHorz; j++) {
				sprintf(word_buffer,"%5u, ",rgbData[cnt+j].b);
				strcat(line_buffer,word_buffer);
			}
			Vps_printf("%s",line_buffer);
			cnt=cnt+gALG_TI_aewbObj.IAEWB_StatMatdata.winCtHorz; 
		}
		Vps_printf("\n******* H3A DUMP END ********\n");
	}	
	
}
#endif

int accValue[4];
/* ===================================================================
 *  @func     GETTING_RGB_BLOCK_VALUE
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
static void GETTING_RGB_BLOCK_VALUE(unsigned short * BLOCK_DATA_ADDR,
            IAEWB_Rgb *rgbData, aewDataEntry *aew_data, int shift)
{
  unsigned short i,j,k, numWin, idx1, idx2;
  Uint8 *curAewbAddr;
  H3aAewbOutUnsatBlkCntOverlay *pAewbUnsatBlk;
  H3aAewbOutSumModeOverlay *pAewbWinData;
  int aew_win_vt_cnt = gALG_TI_aewbObj.IAEWB_StatMatdata.winCtVert;
  int aew_win_hz_cnt = gALG_TI_aewbObj.IAEWB_StatMatdata.winCtHorz;

  curAewbAddr = (Uint8*)BLOCK_DATA_ADDR;
  numWin=0;

#ifdef ALG_AEWB_DEBUG
  accValue[0]=accValue[1]=accValue[2]=accValue[3]=0;
#endif
  /*skip the first row of black window*/
  curAewbAddr += (aew_win_hz_cnt/8) * (sizeof(H3aAewbOutSumModeOverlay) * 8 + sizeof(H3aAewbOutUnsatBlkCntOverlay));

  for(i=0;i<aew_win_vt_cnt; i++) {
    for(j=0;j<aew_win_hz_cnt; j++) {

      pAewbWinData = (H3aAewbOutSumModeOverlay *)curAewbAddr;

      idx1 = numWin/8;
      idx2 = numWin%8;

      aew_data[idx1].window_data[idx2][0] = pAewbWinData->subSampleAcc[0];
      aew_data[idx1].window_data[idx2][1] = pAewbWinData->subSampleAcc[1];
      aew_data[idx1].window_data[idx2][2] = pAewbWinData->subSampleAcc[2];
      aew_data[idx1].window_data[idx2][3] = pAewbWinData->subSampleAcc[3];

#ifdef ALG_AEWB_DEBUG
      accValue[0] += pAewbWinData->subSampleAcc[0];
      accValue[1] += pAewbWinData->subSampleAcc[1];
      accValue[2] += pAewbWinData->subSampleAcc[2];
      accValue[3] += pAewbWinData->subSampleAcc[3];
#endif
      curAewbAddr += sizeof(H3aAewbOutSumModeOverlay);

      numWin++;

      if(numWin%8==0) {
        pAewbUnsatBlk = (H3aAewbOutUnsatBlkCntOverlay*)curAewbAddr;

        for(k=0; k<8;k++)
          aew_data[idx1].unsat_block_ct[k] = pAewbUnsatBlk->unsatCount[k];

        curAewbAddr += sizeof(H3aAewbOutUnsatBlkCntOverlay);
      }
    }
    //curAewbAddr = (Uint8*)OSA_align( (Uint32)curAewbAddr, 32);
    //curAewbAddr = (Uint8*)(( (Uint32)curAewbAddr+ 31) & (~32));

  }

  for(i = 0; i < (aew_win_hz_cnt * aew_win_vt_cnt)>>3;i ++){
    for(j = 0; j < 8; j ++){
      rgbData[i * 8 + j].r = aew_data[i].window_data[j][1] >> shift;
      rgbData[i * 8 + j].b = aew_data[i].window_data[j][2] >> shift;
      rgbData[i * 8 + j].g = (aew_data[i].window_data[j][0]
      + aew_data[i].window_data[j][3]+ 1) >> (1 + shift) ;
    }
  }
#ifdef IMGS_SONY_IMX104
  for(i = 0; i < (aew_win_hz_cnt * aew_win_vt_cnt)>>3;i ++){
    for(j = 0; j < 8; j ++){
      rgbData[i * 8 + j].r = aew_data[i].window_data[j][2] >> shift;
      rgbData[i * 8 + j].b = aew_data[i].window_data[j][1] >> shift;
      rgbData[i * 8 + j].g = (aew_data[i].window_data[j][0]
      + aew_data[i].window_data[j][3]+ 1) >> (1 + shift) ;
    }
  }
#endif
#ifdef IMGS_PANASONIC_MN34041
 #ifndef MN34041_DATA_OP_LVDS324
  for(i = 0; i < (aew_win_hz_cnt * aew_win_vt_cnt)>>3;i ++){
    for(j = 0; j < 8; j ++){
      rgbData[i * 8 + j].r = aew_data[i].window_data[j][0] >> shift;
      rgbData[i * 8 + j].b = aew_data[i].window_data[j][3] >> shift;
      rgbData[i * 8 + j].g = (aew_data[i].window_data[j][1]
      + aew_data[i].window_data[j][2]+ 1) >> (1 + shift) ;

    }
  }
 #endif
#endif
#ifdef IMGS_ALTASENS_AL30210
  for(i = 0; i < (aew_win_hz_cnt * aew_win_vt_cnt)>>3;i ++){
    for(j = 0; j < 8; j ++){
      rgbData[i * 8 + j].r = aew_data[i].window_data[j][0] >> shift;
      rgbData[i * 8 + j].b = aew_data[i].window_data[j][3] >> shift;
      rgbData[i * 8 + j].g = (aew_data[i].window_data[j][1]
      + aew_data[i].window_data[j][2]+ 1) >> (1 + shift) ;
    	}
  }
#endif
#ifdef IMGS_OMNIVISION_OV2715
  for(i = 0; i < (aew_win_hz_cnt * aew_win_vt_cnt)>>3;i ++){
    for(j = 0; j < 8; j ++){
      rgbData[i * 8 + j].r = aew_data[i].window_data[j][3] >> shift;
      rgbData[i * 8 + j].b = aew_data[i].window_data[j][0] >> shift;
      rgbData[i * 8 + j].g = (aew_data[i].window_data[j][1]
      + aew_data[i].window_data[j][2]+ 1) >> (1 + shift) ;
    	}
  }
#endif
#if defined(IMGS_SONY_IMX136) 
  for(i = 0; i < (aew_win_hz_cnt * aew_win_vt_cnt)>>3;i ++){
    for(j = 0; j < 8; j ++){
      rgbData[i * 8 + j].r = aew_data[i].window_data[j][0] >> shift;
      rgbData[i * 8 + j].b = aew_data[i].window_data[j][3] >> shift;
      rgbData[i * 8 + j].g = (aew_data[i].window_data[j][1]
      + aew_data[i].window_data[j][2]+ 1) >> (1 + shift) ;
    	}
  }
#endif
#if defined(IMGS_SONY_IMX140)
  for(i = 0; i < (aew_win_hz_cnt * aew_win_vt_cnt)>>3;i ++){
    for(j = 0; j < 8; j ++){
      rgbData[i * 8 + j].r = aew_data[i].window_data[j][2] >> shift;
      rgbData[i * 8 + j].b = aew_data[i].window_data[j][1] >> shift;
      rgbData[i * 8 + j].g = (aew_data[i].window_data[j][0]
      + aew_data[i].window_data[j][3]+ 1) >> (1 + shift) ;
    	}
  }
#endif
#ifdef IMGS_SONY_IMX122 
  for(i = 0; i < (aew_win_hz_cnt * aew_win_vt_cnt)>>3;i ++){
    for(j = 0; j < 8; j ++){
      rgbData[i * 8 + j].r = aew_data[i].window_data[j][0] >> shift;
      rgbData[i * 8 + j].b = aew_data[i].window_data[j][3] >> shift;
      rgbData[i * 8 + j].g = (aew_data[i].window_data[j][1]
      + aew_data[i].window_data[j][2]+ 1) >> (1 + shift) ;
    	}
  }
#endif
#ifdef ALG_AEWB_DEBUG
  accValue[0] /= numWin*gALG_TI_aewbObj.IAEWB_StatMatdata.pixCtWin;
  accValue[1] /= numWin*gALG_TI_aewbObj.IAEWB_StatMatdata.pixCtWin;
  accValue[2] /= numWin*gALG_TI_aewbObj.IAEWB_StatMatdata.pixCtWin;
  accValue[3] /= numWin*gALG_TI_aewbObj.IAEWB_StatMatdata.pixCtWin;
  if(aewbFrames % (NUM_STEPS*32) == 0)
  {
    Vps_printf("AEWB: Avg Color      :  < %5d, %5d, %5d, %5d >\n",
    accValue[0], accValue[1], accValue[2], accValue[3]);
  }
#endif
}

//convert H3A RGB data into the luma image (int16) the FD algorithm needed
/* ===================================================================
 *  @func     GETTING_RGB_BLOCK_VALUE_Y
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
static void GETTING_RGB_BLOCK_VALUE_Y(unsigned short * BLOCK_DATA_ADDR, short *y, int shift)
{
  unsigned short i,j, numWin;
  Uint8 *curAewbAddr;
#ifndef MN34041_DATA_OP_LVDS324
  H3aAewbOutSumModeOverlay *pAewbWinData;
#endif
  int aew_win_vt_cnt = gALG_TI_aewbObj.IAEWB_StatMatdata.winCtVert;
  int aew_win_hz_cnt = gALG_TI_aewbObj.IAEWB_StatMatdata.winCtHorz;
  int r, g, b;

  curAewbAddr = (Uint8*)BLOCK_DATA_ADDR;
  numWin=0;

  /*skip the first row of black window*/
  curAewbAddr += (aew_win_hz_cnt/8) * (sizeof(H3aAewbOutSumModeOverlay) * 8 + sizeof(H3aAewbOutUnsatBlkCntOverlay));

  for(i=0;i<aew_win_vt_cnt; i++)
  {
    for(j=0;j<aew_win_hz_cnt; j++)
    {
#ifndef MN34041_DATA_OP_LVDS324
      pAewbWinData = (H3aAewbOutSumModeOverlay *)curAewbAddr;
#endif

#ifdef IMGS_PANASONIC_MN34041
#ifdef MN34041_DATA_OP_FPGA
      g = (pAewbWinData->subSampleAcc[1] + pAewbWinData->subSampleAcc[2]) >> (1+shift);
      r = pAewbWinData->subSampleAcc[3] >> shift;
      b = pAewbWinData->subSampleAcc[0] >> shift;
#endif
/*#ifdef MN34041_DATA_OP_LVDS324
      g = (pAewbWinData->subSampleAcc[0] + pAewbWinData->subSampleAcc[3]) >> (1+shift);
      r = pAewbWinData->subSampleAcc[1] >> shift;
      b = pAewbWinData->subSampleAcc[2] >> shift;
#endif*/
#elif defined (IMGS_SONY_IMX136) | defined(IMGS_SONY_IMX140)
      g = (pAewbWinData->subSampleAcc[1] + pAewbWinData->subSampleAcc[2]) >> (1+shift);
      r = pAewbWinData->subSampleAcc[0] >> shift;
      b = pAewbWinData->subSampleAcc[3] >> shift;
#elif defined IMGS_ALTASENS_AL30210
      g = (pAewbWinData->subSampleAcc[1] + pAewbWinData->subSampleAcc[2]) >> (1+shift);
      r = pAewbWinData->subSampleAcc[0] >> shift;
      b = pAewbWinData->subSampleAcc[3] >> shift;
#elif defined IMGS_OMNIVISION_OV2715
      g = (pAewbWinData->subSampleAcc[1] + pAewbWinData->subSampleAcc[2]) >> (1+shift);
      r = pAewbWinData->subSampleAcc[3] >> shift;
      b = pAewbWinData->subSampleAcc[0] >> shift;
#elif defined IMGS_SONY_IMX104
      g = (pAewbWinData->subSampleAcc[0] + pAewbWinData->subSampleAcc[3]) >> (1+shift);
      r = pAewbWinData->subSampleAcc[2] >> shift;
      b = pAewbWinData->subSampleAcc[1] >> shift;
#elif defined IMGS_SONY_IMX122
      g = (pAewbWinData->subSampleAcc[1] + pAewbWinData->subSampleAcc[2]) >> (1+shift);
      r = pAewbWinData->subSampleAcc[0] >> shift;
      b = pAewbWinData->subSampleAcc[3] >> shift;
#else
      g = (pAewbWinData->subSampleAcc[0] + pAewbWinData->subSampleAcc[3]) >> (1+shift);
      r = pAewbWinData->subSampleAcc[1] >> shift;
      b = pAewbWinData->subSampleAcc[2] >> shift;
#endif
      *y++ = ((0x4D * r) + (0x96 * g) + (0x1D * b) + 128 ) / 256;

      curAewbAddr += sizeof(H3aAewbOutUnsatBlkCntOverlay);

      numWin++;

      if(numWin%8==0) {
        curAewbAddr += sizeof(H3aAewbOutUnsatBlkCntOverlay);
      }
    }
    //curAewbAddr = (Uint8*)(( (Uint32)curAewbAddr+ 31) & (~32));
  }
}


TI2A_FDC_Create()
{
}
static int flicker_detect_complete = 0;

/* ===================================================================
 *  @func     TI2A_FDC_Process
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
int TI2A_FDC_Process(void *h3aDataVirtAddr)
{

  /*-------------------------------------------------------------------------*/
  /*Some of the code has to be create time.                                  */
  /*-------------------------------------------------------------------------*/
  static int frame_cnt = -1;
  frame_cnt++;

  flicker_ret_st ret_st;
  static int exp;
  /*-------------------------------------------------------------------------*/
  /*hard coding of window size of h3a need to check with kumar               */
  /*-------------------------------------------------------------------------*/
  int w=16, h=32; //H3A AEWB has w x h windows
  static int row_time;
  static int pinp;
  static int h3aWinHeight;
  static int fd_res_support = 1;
  Int16 h3a[512]; //h3a luma image for FD algorithm
  /*-------------------------------------------------------------------------*/
  /*pass h3a virtual address                                                 */
  /*-------------------------------------------------------------------------*/
  Int16 *pAddr = (Int16*)h3aDataVirtAddr;
  Int32 expG;
  int   fd_gain;
  int   dGain;
  static int stab_count = 0;
  static int flicker_detect_enable = 0;
  static int frame_cnt_fd = 0;
  static int orig_exp = 0;
  static int orig_gain = 0;
  static int orig_dGain = 0;
  static int fd_count = 0;

  //Rfile_printf ("Flicker Detect trace begin ..... \n");
  if (frame_cnt == 0)
  {
    /* multi-resolution flicker detection support */
    if(gALG_TI_aewbObj.sensorMode == DRV_IMGS_SENSOR_MODE_720x480)
    {
      /* D1 */
      row_time = 47;
      pinp = 60;
      h3aWinHeight = 14;
    }
    else if (gALG_TI_aewbObj.sensorMode  == DRV_IMGS_SENSOR_MODE_1280x720)
    {
      /* 720p30 & 720p60*/
      row_time = (44*100)/gALG_TI_aewbObj.reduceShutter;
      pinp = 72;
      h3aWinHeight = 22;
    }
    else if (gALG_TI_aewbObj.sensorMode == DRV_IMGS_SENSOR_MODE_1920x1080)
    {
      /* 1080p */
      row_time = 29/2;
      pinp = 64;
      h3aWinHeight = 32;
    }
    else if (gALG_TI_aewbObj.sensorMode == DRV_IMGS_SENSOR_MODE_1280x960)
    {
      /* SXVGA */
      row_time = 34;
      pinp = 56;
      h3aWinHeight = 30;
    }
    else if (gALG_TI_aewbObj.sensorMode == DRV_IMGS_SENSOR_MODE_1600x1200)
    {
      /* 2MP */
      row_time = 26;
      pinp = 44;
      h3aWinHeight = 38;
    }
    else if (gALG_TI_aewbObj.sensorMode == DRV_IMGS_SENSOR_MODE_2048x1536)
    {
      /* 3MP */
      row_time = 31;
      pinp = 30;
      h3aWinHeight = 48;
    }
    else if (gALG_TI_aewbObj.sensorMode == DRV_IMGS_SENSOR_MODE_2592x1920)
    {
      /* 5MP */
      row_time = 36;
      pinp = 24;
      h3aWinHeight = 60;
    }
    else
    {
      /* FD resolution not currently supported, turn off FD */
      fd_res_support = 0;
    }

    //Rfile_printf("row_time = %d, pinp = %d, h3aWinHeight = %d\n", row_time, pinp, h3aWinHeight);
    if(fd_res_support)
    {
      //(1) first API call to get persistent memory size
      int ms = flicker_alloc(w, h);

      //(2) second API call; fail if ret!=0
      int ret = flicker_init((int32*)g_flickerMem, w, h,
          row_time,
          h3aWinHeight,   	//H3A window height is 22 for 720p, 14 for D1
          100);   			//threshold (100 is default)
      //Rfile_printf("FD init is done ret = %d\n", ret);
    }
  }

  /*-------------------------------------------------------------------------*/
  /* Wait for 2A to stabailize to enable FD                                  */
  /* Waits for the five frames of stable 2A                                  */
  /*-------------------------------------------------------------------------*/
  if (gALG_TI_aewbObj.AE_OutArgs.nextAe.exposureTime == gALG_TI_aewbObj.AE_InArgs.curAe.exposureTime &&
      gALG_TI_aewbObj.AE_OutArgs.nextAe.sensorGain == gALG_TI_aewbObj.AE_InArgs.curAe.sensorGain &&
      (flicker_detect_enable==0) && (flicker_detect_complete==0))
  {
    stab_count++;
    if(stab_count==5)
    {
      //Rfile_printf("2A is stabilized ... Enabling flicker detect ... \n");
      /*---------------------------------------------------------------------*/
      /*enable flicker detection only when 2A is stable                      */
      /*---------------------------------------------------------------------*/
      flicker_detect_enable = 1;
      frame_cnt_fd = 0;
    }
  }
  else
  {
    if (stab_count)
    {
      stab_count = 0;
      //Rfile_printf("waiting for 2A to stabilize ....  \n");
    }
  }


  /* begin Flicker Detection process */
  if (fd_res_support && flicker_detect_enable && (flicker_detect_complete==0))
  {

    //Rfile_printf("Running the flicker detection .... 2A will be disabled ...\n");
    /*---------------------------------------------------------------------*/
    /*set the exposure to the rounded to a 10ms to avoid any phase diff      */
    /*---------------------------------------------------------------------*/
    if(frame_cnt_fd == 0)
    {
      orig_exp = gALG_TI_aewbObj.AE_OutArgs.nextAe.exposureTime;
      orig_gain = gALG_TI_aewbObj.AE_OutArgs.nextAe.sensorGain;
      expG = orig_exp * orig_gain;

      //Rfile_printf("first stage ....org exp = %d, org gain = %d \n", orig_exp,orig_gain);
      //1st exposure
      exp = 10000*((gALG_TI_aewbObj.AE_OutArgs.nextAe.exposureTime + 5000)/10000);

      if(exp <10000) exp = 10000;

      fd_gain = expG / exp;

      orig_dGain = gALG_TI_aewbObj.AE_OutArgs.nextAe.ipipeGain;

      if(fd_gain<1000)
      {
        dGain = 256*fd_gain/1000;  //nextAe->ipipeGain>> 2;1
        fd_gain = 1000;

        //ALG_aewbSetIpipeWb(&ipipe_awb_gain);
        ti2a_output_params.ipipe_awb_gain.dGain = dGain;
      }

      /* Set sensor exposure time and analog gain for 1st stage FD */
      //ALG_aewbSetSensorExposure(exp);
      //ALG_aewbSetSensorGain(fd_gain);

      ti2a_output_params.sensorGain           = fd_gain;
      ti2a_output_params.sensorExposure       = exp;
      ti2a_output_params.mask  = 3; //Enable for exp and aGain
      //Rfile_printf("first stage complete .... modified exp = %d, modified gain = %d \n", exp,fd_gain);
    }
    /*---------------------------------------------------------------------*/
    /*increase the exposure by 5ms to get 2nd sample point                 */
    /*---------------------------------------------------------------------*/
    else if(frame_cnt_fd == FD_FRAME_STEPS)
    {
      expG = gALG_TI_aewbObj.AE_OutArgs.nextAe.exposureTime * gALG_TI_aewbObj.AE_OutArgs.nextAe.sensorGain;
      //Rfile_printf("secound stage \n");

      //2nd exposure
      exp += 5000;

      fd_gain = expG / exp;

      //(3) API call for detection
      /* Pass H3A buffer to data conversion function */
      GETTING_RGB_BLOCK_VALUE_Y((unsigned short*)pAddr, h3a, 2);
      ret_st = flicker_detect((int32*)g_flickerMem, h3a, pinp, FLICKER_STATE_STAT);

      if(fd_gain<1000)
      {
        dGain = 256*fd_gain/1000;  //nextAe->ipipeGain>> 2;
        fd_gain = 1000;

        //ALG_aewbSetIpipeWb(&ipipe_awb_gain);
        ti2a_output_params.ipipe_awb_gain.dGain = dGain;
      }

      /* Set sensor exposure time and analog gain for 2nd stage FD (add 5 ms to 1st stage exposure time) */
      ti2a_output_params.sensorGain           =  fd_gain;
      ti2a_output_params.sensorExposure       = exp;
      ti2a_output_params.mask  = 3; //Enable for exp and aGain
      //Rfile_printf("secound stage complete .... modified exp = %d, modified gain = %d \n", exp,fd_gain);
      //ALG_aewbSetSensorExposure(exp);
      //ALG_aewbSetSensorGain(fd_gain);
    }
    /*---------------------------------------------------------------------*/
    /*set the sensor exp to 8.33 ms(120Hz) multiple                       */
    /*---------------------------------------------------------------------*/
    else if(frame_cnt_fd == FD_FRAME_STEPS*2)
    {
      expG = gALG_TI_aewbObj.AE_OutArgs.nextAe.exposureTime * gALG_TI_aewbObj.AE_OutArgs.nextAe.sensorGain;

      //Rfile_printf("third stage \n");
      //3rd exposure
      exp = 8333*((gALG_TI_aewbObj.AE_OutArgs.nextAe.exposureTime + 4167)/8333);

      if(exp <8333) exp = 8333;

      fd_gain = expG / exp;

      //(3) API call for detection
      /* Pass H3A buffer to data conversion function */
      GETTING_RGB_BLOCK_VALUE_Y((unsigned short*)pAddr, h3a, 2);
      ret_st = flicker_detect((int32*)g_flickerMem, h3a, pinp, FLICKER_STATE_STAT);

      if(fd_gain<1000)
      {
        dGain = 256*fd_gain/1000;  //nextAe->ipipeGain>> 2;
        fd_gain = 1000;

        //ALG_aewbSetIpipeWb(&ipipe_awb_gain);
        ti2a_output_params.ipipe_awb_gain.dGain = dGain;
      }

      /* Set sensor exposure time and analog gain for 3rd stage FD */
      ti2a_output_params.sensorGain           =fd_gain;
      ti2a_output_params.sensorExposure       = exp;
      ti2a_output_params.mask  = 3; //Enable for exp and aGain
      //Rfile_printf("third stage complete .... modified exp = %d, modified gain = %d \n", exp,fd_gain);
      //ALG_aewbSetSensorExposure(exp);
      //ALG_aewbSetSensorGain(fd_gain);
    }
    /*---------------------------------------------------------------------*/
    /*increase the sensor gain by 8.33/2 ms                                */
    /*---------------------------------------------------------------------*/
    else if(frame_cnt_fd == FD_FRAME_STEPS*3)
    {
      expG = gALG_TI_aewbObj.AE_OutArgs.nextAe.exposureTime * gALG_TI_aewbObj.AE_OutArgs.nextAe.sensorGain;

      //Rfile_printf("fourth stage \n");
      //4th exposure
      exp += 4167;

      fd_gain = expG / exp;

      //(3) API call for detection
      /* Pass H3A buffer to data conversion function */
      GETTING_RGB_BLOCK_VALUE_Y((unsigned short*)pAddr, h3a, 2);
      ret_st = flicker_detect((int32*)g_flickerMem, h3a, pinp, FLICKER_STATE_STAT);

      if(fd_gain<1000)
      {
        dGain = 256*fd_gain/1000;  //nextAe->ipipeGain>> 2;
        fd_gain = 1000;

        //ALG_aewbSetIpipeWb(&ipipe_awb_gain);
        ti2a_output_params.ipipe_awb_gain.dGain = dGain;
      }

      ti2a_output_params.sensorGain           = fd_gain;
      ti2a_output_params.sensorExposure       = exp;
      //ALG_aewbSetSensorExposure(exp);
      ti2a_output_params.mask  = 3; //Enable for exp and aGain
      //ALG_aewbSetSensorGain(fd_gain);
      //Rfile_printf("fourth stage complete .... modified exp = %d, modified gain = %d \n", exp,fd_gain);
    }
    else if(frame_cnt_fd == FD_FRAME_STEPS*4)
    {

      //Rfile_printf("Final statge of detection .... ");
      expG = gALG_TI_aewbObj.AE_OutArgs.nextAe.exposureTime * gALG_TI_aewbObj.AE_OutArgs.nextAe.sensorGain;

      //(3) API call for detection
      /* Pass H3A buffer to data conversion function */
      GETTING_RGB_BLOCK_VALUE_Y((unsigned short*)pAddr, h3a, 2);
      ret_st = flicker_detect((int32*)g_flickerMem, h3a, pinp, FLICKER_STATE_CALC);

      ti2a_output_params.ipipe_awb_gain.dGain = orig_dGain;
      ti2a_output_params.sensorGain           = orig_gain;
      ti2a_output_params.sensorExposure       = orig_exp;
      ti2a_output_params.mask  = 3; //Enable for exp and aGain
      //ALG_aewbSetSensorExposure(orig_exp);
      //ALG_aewbSetSensorGain(orig_gain);

      //ipipe_awb_gain.dGain = orig_dGain;
      //ALG_aewbSetIpipeWb(&ipipe_awb_gain);

      /* Configure 2A based on results of FD */
#ifdef ALG_AEWB_DEBUG
      Vps_printf("Detection status ... %d \n", ret_st);
#endif

      //Rfile_printf("Doing config call ... \n");
      TI_2A_config(ret_st, gALG_TI_aewbObj.saldre);
#ifdef FD_DEBUG_MSG
      OSA_printf("\n ret_st=%dti2a_output_params.sensorExposure\n", ret_st);
#endif
      if(((ret_st == 2)&&(gALG_TI_aewbObj.env_50_60Hz == VIDEO_PAL))  ||
          ((ret_st == 3)&&(gALG_TI_aewbObj.env_50_60Hz == VIDEO_NTSC)) ||
          (fd_count==6))
      {
#ifdef ALG_AEWB_DEBUG
        Vps_printf("Flicker detection completed ... ");
#endif
        flicker_detect_complete = 1;
        flicker_detect_enable = 0;

        fd_count = 0;
      }
      else
      {
        flicker_detect_enable = 0;
        fd_count++;

      }
    }

    frame_cnt_fd++;

    /*do not call 2A if the flicker detection is in progress*/
    return NO_2A;
  }
  else
  {
    return DO_2A;
  }
}

/* ===================================================================
 *  @func     TI2AFunc
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
void TI2AFunc(void *pAddr)
{
  //GETTING_RGB_BLOCK_VALUE(pAddr, rgbData, aew_data, 2);

  if(aew_enable == AEW_ENABLE && !(aewbFrames % NUM_STEPS) )
  {
    GETTING_RGB_BLOCK_VALUE(pAddr, rgbData, aew_data, 2);

    gALG_TI_aewbObj.AE_InArgs.curAe.exposureTime = ti2a_output_params.sensorExposure;
    gALG_TI_aewbObj.AE_InArgs.curAe.sensorGain   = ti2a_output_params.sensorGain;
    gALG_TI_aewbObj.AE_InArgs.curAe.ipipeGain    = ti2a_output_params.ipipe_awb_gain.dGain << 2;
    gALG_TI_aewbObj.AE_InArgs.curWb.rGain        = ti2a_output_params.ipipe_awb_gain.rGain << 3;
    gALG_TI_aewbObj.AE_InArgs.curWb.gGain        = ti2a_output_params.ipipe_awb_gain.grGain << 3;
    gALG_TI_aewbObj.AE_InArgs.curWb.bGain        = ti2a_output_params.ipipe_awb_gain.bGain << 3;

    if(gALG_TI_aewbObj.aewbType == ALG_AEWB_AE || gALG_TI_aewbObj.aewbType == ALG_AEWB_AEWB)
    {
      AE_TI_AE.process(
        (IAE_Handle)gALG_TI_aewbObj.handle_ae,
        &gALG_TI_aewbObj.AE_InArgs,
        &gALG_TI_aewbObj.AE_OutArgs,
        rgbData,
        gALG_TI_aewbObj.weight,
        NULL
        );

#ifdef ALG_AEWB_DEBUG
        Vps_printf("DSP AE: \n");
        Vps_printf("nextExposure: %d, curExposure:%d\n",
           gALG_TI_aewbObj.AE_OutArgs.nextAe.exposureTime,
           gALG_TI_aewbObj.AE_InArgs.curAe.exposureTime);
        Vps_printf("nextGain: %d, curGain:%d\n",
           gALG_TI_aewbObj.AE_OutArgs.nextAe.sensorGain,
           gALG_TI_aewbObj.AE_InArgs.curAe.sensorGain);
		Vps_printf("nextIGain: %d, curIGain:%d\n\n",
           gALG_TI_aewbObj.AE_OutArgs.nextAe.ipipeGain,
           gALG_TI_aewbObj.AE_InArgs.curAe.ipipeGain);
#endif

    }
    else
    {
      gALG_TI_aewbObj.AE_OutArgs.nextAe = gALG_TI_aewbObj.AE_InArgs.curAe;
    }

    if(gALG_TI_aewbObj.AE_OutArgs.nextAe.exposureTime == gALG_TI_aewbObj.AE_InArgs.curAe.exposureTime &&
      gALG_TI_aewbObj.AE_OutArgs.nextAe.sensorGain == gALG_TI_aewbObj.AE_InArgs.curAe.sensorGain &&
      (gALG_TI_aewbObj.aewbType == ALG_AEWB_AWB || gALG_TI_aewbObj.aewbType == ALG_AEWB_AEWB) )
    {

       /* calling awb only we AE has converged */
      gALG_TI_aewbObj.AWB_InArgs.curWb = gALG_TI_aewbObj.AE_InArgs.curWb;
      gALG_TI_aewbObj.AWB_InArgs.curAe = gALG_TI_aewbObj.AE_InArgs.curAe;

#ifdef _PROFILE_AWB_ALGO_
       awb_start    = Timestamp_get32();
#endif

       AWB_TI_AWB.process(
        (IAWB_Handle)gALG_TI_aewbObj.handle_awb,
        &gALG_TI_aewbObj.AWB_InArgs,
        &gALG_TI_aewbObj.AWB_OutArgs,
        rgbData,
        NULL
        );
#ifdef _PROFILE_AWB_ALGO_
      awb_end    = Timestamp_get32();
      Rfile_printf("DSP AWB execution time: %d, %f ms \n",(awb_end - awb_start - overhead), (float)((((awb_end - awb_start - overhead) * 100 )/numTicksPerMilSec))/100.0);
#endif

      ti2a_output_params.ipipe_awb_gain.rGain  = gALG_TI_aewbObj.AWB_OutArgs.nextWb.rGain >> 3;
      ti2a_output_params.ipipe_awb_gain.grGain = gALG_TI_aewbObj.AWB_OutArgs.nextWb.gGain >> 3;
      ti2a_output_params.ipipe_awb_gain.gbGain = gALG_TI_aewbObj.AWB_OutArgs.nextWb.gGain >> 3;
      ti2a_output_params.ipipe_awb_gain.bGain  = gALG_TI_aewbObj.AWB_OutArgs.nextWb.bGain >> 3;

          ti2a_output_params.ipipe_awb_gain.rOffset   = 0;
          ti2a_output_params.ipipe_awb_gain.grOffset  = 0;
          ti2a_output_params.ipipe_awb_gain.gbOffset  = 0;
          ti2a_output_params.ipipe_awb_gain.bOffset   = 0;


      ti2a_output_params.colorTemparaure = gALG_TI_aewbObj.AWB_OutArgs.nextWb.colorTemp;
#ifdef ALG_AEWB_DEBUG
      if(aewbFrames % (NUM_STEPS*32) == 0)
      {
          Vps_printf("DSP AWB : CT : %5d, R : %4d, G : %4d B : %4d \n",
				gALG_TI_aewbObj.AWB_OutArgs.nextWb.colorTemp,
				ti2a_output_params.ipipe_awb_gain.rGain,
				ti2a_output_params.ipipe_awb_gain.grGain,
				ti2a_output_params.ipipe_awb_gain.bGain);
          //ti2a_output_params.colorTemparaure = Illum.temperature;
          Vps_printf(" AWB %d %d %d\n", aew_enable , gALG_TI_aewbObj.aewbType , gTi2aControlParams.wbSceneMode );
      }
#endif

    }

    TI2A_applySettings(&gALG_TI_aewbObj.AE_InArgs.curAe,
        &gALG_TI_aewbObj.AE_OutArgs.nextAe, NUM_STEPS-1, 0);

  }
  else if(aew_enable == AEW_ENABLE && (gALG_TI_aewbObj.aewbType == ALG_AEWB_AE || gALG_TI_aewbObj.aewbType == ALG_AEWB_AWB || gALG_TI_aewbObj.aewbType == ALG_AEWB_AEWB )){
    TI2A_applySettings(&gALG_TI_aewbObj.AE_InArgs.curAe, &gALG_TI_aewbObj.AE_OutArgs.nextAe, NUM_STEPS-1, (aewbFrames % NUM_STEPS));
  }
  /* remove the count and put it into the process */
  aewbFrames ++;

}

/* ===================================================================
 *  @func     TI2A_applySettings
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
static void TI2A_applySettings(IAEWB_Ae *curAe, IAEWB_Ae *nextAe, int numSmoothSteps, int step)
{
  int delta_sensorgain = ((int)nextAe->sensorGain - (int)curAe->sensorGain)/numSmoothSteps;
  int delta_exposure = ((int)nextAe->exposureTime - (int)curAe->exposureTime)/numSmoothSteps;
  int delta_ipipe = ((int)nextAe->ipipeGain - (int)curAe->ipipeGain)/numSmoothSteps;

  step ++;

  ti2a_output_params.sensorGain           = delta_sensorgain * step + curAe->sensorGain;
  ti2a_output_params.sensorExposure       = delta_exposure * step + curAe->exposureTime;
  ti2a_output_params.ipipe_awb_gain.dGain = (delta_ipipe * step +curAe->ipipeGain) >> 2;

  if(step >= numSmoothSteps) {
    ti2a_output_params.sensorGain = nextAe->sensorGain;
    ti2a_output_params.sensorExposure = nextAe->exposureTime;
    ti2a_output_params.ipipe_awb_gain.dGain = nextAe->ipipeGain>> 2;
  }
  if(ti2a_output_params_prev.sensorGain != ti2a_output_params.sensorGain)
  {
    ti2a_output_params_prev.sensorGain = ti2a_output_params.sensorGain;
    ti2a_output_params.mask |= 2;
#ifdef ALG_AEWB_DEBUG
    Vps_printf("Sensor Analog Gain   :  < %5d, %5d ,%5d > \n",
    ti2a_output_params.sensorGain,curAe->sensorGain,nextAe->sensorGain);
    Vps_printf("AEWB: Avg Color      :  < %5d, %5d, %5d, %5d >\n",
    accValue[0], accValue[1], accValue[2], accValue[3]);
#endif
  }
  else
  {
    ti2a_output_params.mask &= ~(2);
  }
  if(ti2a_output_params_prev.sensorExposure != ti2a_output_params.sensorExposure)
  {
    ti2a_output_params_prev.sensorExposure = ti2a_output_params.sensorExposure;
    ti2a_output_params.mask |= 1;
#ifdef ALG_AEWB_DEBUG
    Vps_printf("Sensor Shutter Speed :  < %5d, %5d ,%5d > \n",
    ti2a_output_params.sensorExposure,curAe->exposureTime,nextAe->exposureTime);
    Vps_printf("AEWB: Avg Color      :  < %5d, %5d, %5d, %5d > \n",
    accValue[0], accValue[1], accValue[2], accValue[3]);
#endif
  }
  else
  {
    ti2a_output_params.mask &= ~(1);
  }

#if 1
  if(ti2a_output_params_prev.ipipe_awb_gain.dGain != ti2a_output_params.ipipe_awb_gain.dGain)
  {
    ti2a_output_params_prev.ipipe_awb_gain.dGain = ti2a_output_params.ipipe_awb_gain.dGain;
#ifdef ALG_AEWB_DEBUG
    Vps_printf("AEWB Digital Gain    :  < %5d, %5d ,%5d > \n",
    ti2a_output_params.ipipe_awb_gain.dGain,curAe->ipipeGain,nextAe->ipipeGain);
    Vps_printf("AEWB: Avg Color      :  < %5d, %5d, %5d, %5d > \n",
    accValue[0], accValue[1], accValue[2], accValue[3]);
#endif
  }
  ti2a_output_params.mask |= (4);
#else

  if(memcmp(&(ti2a_output_params_prev.ipipe_awb_gain),&(ti2a_output_params.ipipe_awb_gain),(sizeof(AWB_PARAM))))
  {
    ti2a_output_params.mask |= 4;
    ti2a_output_params_prev.ipipe_awb_gain.rGain  = ti2a_output_params.ipipe_awb_gain.rGain;
    ti2a_output_params_prev.ipipe_awb_gain.grGain = ti2a_output_params.ipipe_awb_gain.grGain;
    ti2a_output_params_prev.ipipe_awb_gain.gbGain = ti2a_output_params.ipipe_awb_gain.gbGain;
    ti2a_output_params_prev.ipipe_awb_gain.bGain  = ti2a_output_params.ipipe_awb_gain.bGain;
    ti2a_output_params_prev.ipipe_awb_gain.dGain  = ti2a_output_params.ipipe_awb_gain.dGain;

#ifdef ALG_AEWB_DEBUG
    Rfile_printf("AE Digital Gain      :  < %5d, %5d ,%5d > \n",
    ti2a_output_params.ipipe_awb_gain.dGain,(curAe->ipipeGain>> 2), (nextAe->ipipeGain>> 2));
    Rfile_printf("AWB:                 :  < %5d, %5d, %5d> \n",
    ti2a_output_params.ipipe_awb_gain.rGain,ti2a_output_params.ipipe_awb_gain.grGain,ti2a_output_params.ipipe_awb_gain.bGain);
    Rfile_printf("AEWB: Avg Color      :  < %5d, %5d, %5d, %5d > \n",
    accValue[0], accValue[1], accValue[2], accValue[3]);
#endif
  }
  else
  {
    ti2a_output_params.mask &= ~(4);
  }
#endif

}




/* ===================================================================
 *  @func     ALG_aewbRun
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
int ALG_aewbRun(void *h3aDataVirtAddr)
{


  int do2a = DO_2A;
  /*Debug*/



  /*if flicker detection is enabled*/
  if (gALG_TI_aewbObj.flicker_detect == FDC_ENABLED)
  {
      do2a = TI2A_FDC_Process((void*)h3aDataVirtAddr);
  }

  if (do2a == DO_2A)
  {
      TI2AFunc( (void *)h3aDataVirtAddr );
#ifdef ALG_AEWB_DEBUG
      H3A_DEBUG_PRINT();
#endif
  }
  return 0;
}

/* ===================================================================
 *  @func     ALG_aewbDelete
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
int ALG_aewbDelete(void *hndl)
{
  int numMem;

  numMem = AE_TI_AE.ialg.algFree(gALG_TI_aewbObj.handle_ae, gALG_TI_aewbObj.memTab_ae) + 1;
  while(numMem > 0){
    free( gALG_TI_aewbObj.memTab_ae[numMem-1].base );
    numMem --;
  }

  numMem = AWB_TI_AWB.ialg.algFree(gALG_TI_aewbObj.handle_awb, gALG_TI_aewbObj.memTab_awb) + 1;
  while(numMem > 0){
    free( gALG_TI_aewbObj.memTab_awb[numMem-1].base );
    numMem --;
  }

  if(rgbData!=NULL) {
  	free(rgbData);
  	rgbData = NULL;
  }

  if(aew_data!=NULL) {
  	free(aew_data);
  	aew_data = NULL;
  }

  if(g_flickerMem!=NULL) {
  	free(g_flickerMem);
  	g_flickerMem = NULL;
  }

  return 0;
}


/* ===================================================================
 *  @func     ALG_aewbControl
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
int ALG_aewbControl(void)
{

    gALG_TI_aewbObj.aewbType = gTi2aControlParams.aewbType;
    if (gTi2aControlParams.update & TI2A_UPDATE_AE_DAY_NIGHT)
    {
      if(gTi2aControlParams.day_night == AE_DAY)
      {
        gTi2aControlParams.maxExposure         = 16666;
      }
      else
      {
        gTi2aControlParams.maxExposure         = 33330;
#if defined(IMGS_MICRON_MT9M034) & defined(WDR_ON)
        gTi2aControlParams.maxExposure         = 22100;//33330;
#endif
#if defined(IMGS_SONY_IMX104) & defined(WDR_ON)
		gTi2aControlParams.maxExposure		 = 32755;//33330;
#endif
#if (defined(IMGS_SONY_IMX136) | defined(IMGS_SONY_IMX140) ) & defined(WDR_ON)
	  gTi2aControlParams.maxExposure		 = 33184;
#endif
      }
      gTi2aControlParams.update |= TI2A_UPDATE_CONTROL_PARAMS_2A;
      gTi2aControlParams.update &= (~TI2A_UPDATE_AE_DAY_NIGHT);
    }

  if ( (gTi2aControlParams.update & TI2A_UPDATE_CONTROL_PARAMS_2A) || (new_awb_data_available == 1))
    {
	  new_awb_data_available = 0;
#ifdef ALG_AEWB_DEBUG
      Vps_printf("Doing 2A config Call ...\n ");
#endif
      gTi2aControlParams.update &= (~TI2A_UPDATE_CONTROL_PARAMS_2A);
	  if (gTi2aControlParams.flicker_sel == 1)
      {
        gALG_TI_aewbObj.env_50_60Hz = VIDEO_PAL;
      }
      else if (gTi2aControlParams.flicker_sel == 2)
      {
        gALG_TI_aewbObj.env_50_60Hz = VIDEO_NTSC;
      }
      TI_2A_config(0, gALG_TI_aewbObj.saldre);
    }

    if (gTi2aControlParams.update & TI2A_UPDATE_BLC)
    {

        if(gTi2aControlParams.blc==BACKLIGHT_LOW ||
            gTi2aControlParams.blc==BACKLIGHT_LOW2 )
        {
            gALG_TI_aewbObj.weight= TI_WEIGHTING_MATRIX;

        }
        else if(gTi2aControlParams.blc==BACKLIGHT_HIGH ||
                gTi2aControlParams.blc==BACKLIGHT_HIGH2 )
        {
            gALG_TI_aewbObj.weight=TI_WEIGHTING_SPOT;
        }
        else
        {
            gALG_TI_aewbObj.weight=TI_WEIGHTING_CENTER;
        }
      gTi2aControlParams.update &= (~TI2A_UPDATE_BLC);
   }

    if (gTi2aControlParams.wbSceneMode != TI2A_WB_SCENE_MODE_AUTO)
    {
        int R = 0, G = 0, B = 0;
        int base;

      if(gTi2aControlParams.wbSceneMode == TI2A_WB_SCENE_MODE_D65)
      {
        ti2a_output_params.colorTemparaure = awb_calc_data.default_T_H;
        R = awb_calc_data.ref_gray_R_1[awb_calc_data.default_T_H_index][0];
        G = awb_calc_data.ref_gray_G_1[awb_calc_data.default_T_H_index][0];
        B = awb_calc_data.ref_gray_B_1[awb_calc_data.default_T_H_index][0];
      }
      else if(gTi2aControlParams.wbSceneMode == TI2A_WB_SCENE_MODE_D55)
      {
        ti2a_output_params.colorTemparaure = awb_calc_data.default_T_MH;
        R = awb_calc_data.ref_gray_R_1[awb_calc_data.default_T_MH_index][0];
        G = awb_calc_data.ref_gray_G_1[awb_calc_data.default_T_MH_index][0];
        B = awb_calc_data.ref_gray_B_1[awb_calc_data.default_T_MH_index][0];
      }
      else if(gTi2aControlParams.wbSceneMode == TI2A_WB_SCENE_MODE_FLORESCENT)
      {
        ti2a_output_params.colorTemparaure = awb_calc_data.default_T_ML;
        R = awb_calc_data.ref_gray_R_1[awb_calc_data.default_T_ML_index][0];
        G = awb_calc_data.ref_gray_G_1[awb_calc_data.default_T_ML_index][0];
        B = awb_calc_data.ref_gray_B_1[awb_calc_data.default_T_ML_index][0];
      }
      else if(gTi2aControlParams.wbSceneMode == TI2A_WB_SCENE_MODE_INCANDESCENT)
      {
        ti2a_output_params.colorTemparaure = awb_calc_data.default_T_L;
        R = awb_calc_data.ref_gray_R_1[awb_calc_data.default_T_L_index][0];
        G = awb_calc_data.ref_gray_G_1[awb_calc_data.default_T_L_index][0];
        B = awb_calc_data.ref_gray_B_1[awb_calc_data.default_T_L_index][0];
      }

      base = 0;



	    if (base < R )  base = R;

	    if (base < G )  base = G;

	    if (base < B )  base = B;



      ti2a_output_params.ipipe_awb_gain.rGain    = (( base * 512 + R / 2 ) / R)/4;
      ti2a_output_params.ipipe_awb_gain.grGain   = (( base * 512 + G / 2 ) / G)/4;
      ti2a_output_params.ipipe_awb_gain.gbGain   = (( base * 512 + G / 2 ) / G)/4;
      ti2a_output_params.ipipe_awb_gain.bGain    = (( base * 512 + B / 2 ) / B)/4;
    }

    return 0;
}





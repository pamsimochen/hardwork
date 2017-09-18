/** ==================================================================
 *  @file   issdrv_algAewbPriv.h                                                  
 *                                                                    
 *  @path    /proj/vsi/users/venu/DM812x/IPNetCam_rel_1_8/ti_tools/iss_02_bkup/packages/ti/psp/iss/drivers/alg/2A/inc/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ALG_AEWB_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ALG_AEWB_PRIV_H_

#include <iae.h>
#include <iawb.h>
#include <aewb_xdm.h>
//#include <alg_aewb.h>
#include <alg_aewb_ctrl_priv.h>

#include "alg_ti_aewb_priv.h"
#include "ae_ti.h"
#include "awb_ti.h"
#include "TI_aewb.h"


#define ALG_AEWB_ID_NONE    0
#define ALG_AEWB_ID_APPRO   1
#define ALG_AEWB_ID_TI    	2

#define ALG_AEWB_OFF    	0
#define ALG_AEWB_AE   		1
#define ALG_AEWB_AWB    	2
#define ALG_AEWB_AEWB    	3

#define ALG_VIDEO_MODE_NTSC   0
#define ALG_VIDEO_MODE_PAL    1

#define ALG_AWB_MODE_INDOOR   0
#define ALG_AWB_MODE_OUTDOOR  1

#define ALG_AE_MODE_DAY      0
#define ALG_AE_MODE_NIGHT    1

#define IMAGE_TUNE_AWB_RGB_SIZE    1024
#define IMAGE_TUNE_AWB_YUV_SIZE    1024   /* If modified, change the same in imageTunePreview.h */


#define OSA_ERROR(a) //

/**
  \brief  AE/AWB packet format for sum-only mode
*/
  typedef struct {

    unsigned short  subSampleAcc[4];
    unsigned short  saturatorAcc[4];
    //unsigned int    sumOfSquares[4];    ///< AE/AWB packet format for sum of square mode

  } H3aAewbOutSumModeOverlay;

/**
  \brief  AE/AWB unsaturated block count
*/
  typedef struct {

    unsigned short  unsatCount[8];

  } H3aAewbOutUnsatBlkCntOverlay;

typedef struct {

  IAE_InArgs    AE_InArgs;
  IAE_OutArgs   AE_OutArgs;
  IAWB_InArgs   AWB_InArgs;
  IAWB_OutArgs  AWB_OutArgs;

  IALG_Handle   handle_ae;
  IALG_Handle   handle_awb;

  IALG_MemRec   memTab_ae[4];
  IALG_MemRec   memTab_awb[4];

  unsigned char *weight;

  IAEWB_StatMat IAEWB_StatMatdata;

  int vsEnable;
  int vnfDemoCfg;
  int aewbType;
  int aewbVendor;
  int aewbPriority;
  int reduceShutter;
  int saldre;
  int flgAWBCalcDataUpdate;
  int sensorMode;
  int env_50_60Hz;
  int flicker_detect;
} ALG_AewbObj;

extern ALG_AewbObj gALG_TI_aewbObj;

extern IAE_DynamicParams  IAE_DynamicParam;
extern IAWB_DynamicParams IAWB_DynamicParam;

/* ===================================================================
 *  @func     ALG_aewbGetAEValues                                               
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
void ALG_aewbGetAEValues(Int32 *exposureTime, Int32 *apertureLevel, Int32 *sensorGain, Int32 *ipipeGain);
/* ===================================================================
 *  @func     ALG_aewbGetAWBGains                                               
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
void ALG_aewbGetAWBGains(Uint16 *rGain, Uint16 *grGain, Uint16 *gbGain, Uint16 *bGain);
/* ===================================================================
 *  @func     ALG_aewbGetRgb2Rgb                                               
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
void ALG_aewbGetRgb2Rgb(Int16*matrix);

/* ===================================================================
 *  @func     ALG_aewbSetSensorGain                                               
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
short ALG_aewbSetSensorGain(int gain);
/* ===================================================================
 *  @func     ALG_aewbSetSensorExposure                                               
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
short ALG_aewbSetSensorExposure(int shutter);
/* ===================================================================
 *  @func     ALG_aewbSetIpipeWb                                               
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
short ALG_aewbSetIpipeWb(AWB_PARAM  *pAwb_Data );
short ALG_aewbSetIpipeWb2(AWB_PARAM  *pAwb_Data );
/* ===================================================================
 *  @func     ALG_aewbSetSensorDcsub                                               
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
short ALG_aewbSetSensorDcsub(int dcsub);
/* ===================================================================
 *  @func     ALG_aewbSetSensorBin                                               
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
short ALG_aewbSetSensorBin(int bin);
/* ===================================================================
 *  @func     ALG_aewbSetRgb2Rgb                                               
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
short ALG_aewbSetRgb2Rgb(RGB2RGB_PARAM  *pRgb2Rgb );
short ALG_aewbSetRgb2Rgb2(RGB2RGB_PARAM  *pRgb2Rgb );
/* ===================================================================
 *  @func     ALG_aewbSetOtfCorrect                                               
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
short ALG_aewbSetOtfCorrect( int level );
/* ===================================================================
 *  @func     ALG_aewbSetEdgeEnhancement                                               
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
short ALG_aewbSetEdgeEnhancement(EDGE_PARAM  *pParm );
/* ===================================================================
 *  @func     ALG_aewbSetContrastBrightness                                               
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
short ALG_aewbSetContrastBrightness(BRT_CRT_PARAM  *pParm );
/* ===================================================================
 *  @func     ALG_aewbSetSensorBinSkip                                               
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
short ALG_aewbSetSensorBinSkip(int Is_binning);
/* ===================================================================
 *  @func     ALG_aewbSetSensor50_60Hz                                               
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
short ALG_aewbSetSensor50_60Hz(int Is50Hz);
/* ===================================================================
 *  @func     ALG_aewbSetSensorFrameRate                                               
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
short ALG_aewbSetSensorFrameRate(int frame_rate_mode);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _ALG_AEWB_PRIV_H_ */


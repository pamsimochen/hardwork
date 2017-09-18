/*
********************************************************************************
 * AEWB API
 *
 * "AEWB API" is software module developed for TI's ISS based SOCs.
 * This module provides APIs for auto exposure and aoto white balance algorith
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
********************************************************************************
*/
/**
********************************************************************************
 * @file  issdrv_algTIaewb.h
 *
 * @brief This file contains interface function anf struture required for
 * using AEWB algorith.
 *
********************************************************************************
*/
#ifndef _ALG_TI_AEWB_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ALG_TI_AEWB_H_


/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/* Macro to enable debug cpabilites in AEWB algorith */
//#define ALG_AEWB_DEBUG


#define TI_DSPRND_AWB (0)
#define TI_VSP_AWB    (1)

/*---------------------- data declarations -----------------------------------*/
/**
 *******************************************************************************
 *  @struct awb_params
 *  @brief  This structure defines the gain values WB module and
 *          digital gain module in ISIF
 *  @param   rGain	 : 	WB gain for R Pixels
 *  @param   grGain	 : 	WB gain for GR Pixels
 *  @param   gbGain	 : 	WB gain for GB Pixels
 *  @param   bGain	 : 	WB gain for B Pixels
 *  @param   rOffset : 	WB Offset for R Pixels
 *  @param   grOffset: 	WB Offset for GR Pixels
 *  @param   gbOffset: 	WB Offset for GB Pixels
 *  @param   bOffset : 	WB Offset for B Pixels
 *  @param   dGain	 : 	ISIF digital gain
 *
 *******************************************************************************
*/
typedef struct {
unsigned int rGain;
unsigned int grGain;
unsigned int gbGain;
unsigned int bGain;
unsigned int rOffset;
unsigned int grOffset;
unsigned int gbOffset;
unsigned int bOffset;
unsigned int dGain;
}awb_params;

/**
 *******************************************************************************
 *  @struct rgb2rgb_params
 *  @brief  This structure defines the gain and offset values RGB to RGB
 *          conversion module.
 *  @param   rgb_mul_rr	: 	R gain for R pixels
 *  @param   rgb_mul_rg	: 	G gain for R pixels
 *  @param   rgb_mul_rb	: 	B gain for R pixels
 *  @param   rgb_mul_gr	: 	R gain for G pixels
 *  @param   rgb_mul_gg	: 	G gain for G pixels
 *  @param   rgb_mul_gb	: 	B gain for G pixels
 *  @param   rgb_mul_br	: 	R gain for B pixels
 *  @param   rgb_mul_bg	: 	G gain for B pixels
 *  @param   rgb_mul_bb	: 	B gain for B pixels
 *  @param   rgb_oft_or	: 	Offset for R pixels
 *  @param   rgb_oft_og	: 	Offset for G pixels
 *  @param   rgb_oft_ob	: 	Offset for B pixels
 *
 *******************************************************************************
*/
typedef struct {
unsigned int rgb_mul_rr;
unsigned int rgb_mul_gr;
unsigned int rgb_mul_br;
unsigned int rgb_mul_rg;
unsigned int rgb_mul_gg;
unsigned int rgb_mul_bg;
unsigned int rgb_mul_rb;
unsigned int rgb_mul_gb;
unsigned int rgb_mul_bb;
unsigned int rgb_oft_or;
unsigned int rgb_oft_og;
unsigned int rgb_oft_ob;
}rgb2rgb_params;

/**
 *******************************************************************************
 *  @struct awb_params
 *  @brief  This structure defines the output parameters of AEWB algorith
 *  @param   colorTemparaure : 	Color temperature calculated by the AWB
 *  @param   sensorGain      : 	Sensor analog gain calculated by AE
 *  @param   sensorExposure  : 	Exposure time calculated by AE
 *  @param   apertureLevel   : 	Aperture level. Currently not used
 *  @param   rgbMatrixIndex  : 	The RGB to RGB selection matrix index
 *  @param   Mask            : 	Mask for setting drivers
 *  @param   ipipe_awb_gain  : 	White balance gain values
 *  @param   rgb2rgbparam    : 	RGB to RGB 1 modules gain matrix
 *  @param   rgb2rgbparam    : 	RGB to RGB 2 modules gain matrix
 *
 *******************************************************************************
*/

typedef struct  {
int colorTemparaure;
int sensorGain;
int sensorExposure;
int apertureLevel;
int rgbMatrixIndex;
int mask;
awb_params ipipe_awb_gain;
rgb2rgb_params rgb2rgbparam;
rgb2rgb_params rgb2rgb2param;
}ti2a_output;


typedef enum {
    TI2A_UPDATE_CLEAR_COMMAND              = 0,
    TI2A_UPDATE_CONTROL_PARAMS_2A          = 1,
    TI2A_UPDATE_BLC                        = 2,
    TI2A_UPDATE_AE_DAY_NIGHT               = 4,
    TI2A_UPDATE_CNT_BRGT                   = 8,
    TI2A_UPDATE_SHARPNESS                  = 16,
    TI2A_UPDATE_SATURATION                 = 32,
}ti2aControlCommand;

typedef enum {
    TI2A_WB_SCENE_MODE_AUTO                = 0,
    TI2A_WB_SCENE_MODE_D65                 = 1,
    TI2A_WB_SCENE_MODE_D55                 = 2,
    TI2A_WB_SCENE_MODE_FLORESCENT          = 3,
    TI2A_WB_SCENE_MODE_INCANDESCENT        = 4,
}ti2aWBSceneMode;



typedef struct{
    int update;
    int flicker_sel; /* <TBR: Uday>Sel between 50/60Hz flicker*/
    int flickerFreq; /*Select custum flicker*/
    int minExposure; /*Mininum Sensor exposure*/
    int maxExposure; /*Maximum Sensor exposure*/
    int stepSize;    /*step size for the exposure variation */
    int aGainMin;    /*Minimum analog Gain*/
    int aGainMax;    /*Maximum Analog gain */
    int dGainMin;    /*Minimum digital gain*/
    int dGainMax;    /*Maximum Digital gain */
    int targetBrightnessMin; /*Minimum target bright ness */
    int targetBrightnessMax; /*Maximum target bright ness */
    int targetBrightness;    /*target brightness to be achieved*/

    int	day_night;
    int	blc;
    int	brightness;
    int	contrast;
    int	sharpness;
    int	saturation;
    int wbSceneMode;
    int aewbType;


}ti2aControlParams_t;


/*******************************************************************************
*                         FUNCTION DEFINITIONS
*******************************************************************************/

/**
********************************************************************************
 * @fn      void *ALG_aewbCreate(int aewbNumWinH,int aewbNumWinV,int aewbNumPix);
 *
 * @brief   This function Need to be called only once application init
 *          time for memory allocations
 *
 * @param   aewbNumWinH
 *          Number of H3A paxel in Horizontal direction
 *
 * @param   aewbNumWinV
 *          Number of H3A paxel in Vertical direction
 *
 * @param   aewbNumPix
 *          Number of pixels per Paxel
 *
 * @return  void *
 *          Pointer to algorithm instance
********************************************************************************
*/
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
void *ALG_aewbCreate(int aewbNumWinH,int aewbNumWinV,int aewbNumPix);
/**
********************************************************************************
 * @fn      int ALG_aewbRun(void *h3aDataVirtAddr);
 *
 * @brief   Need to be called to perform AEWB for a given Picture
 *
 * @param   *h3aDataVirtAddr
 *           Pointer to the address where H3A data for
 *           current Picyure is stored in DDR
 *
 * @return  SUCESS/FAIL
********************************************************************************
*/
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
int ALG_aewbRun(void *h3aDataVirtAddr);
/**
********************************************************************************
 * @fn      int ALG_aewbDelete(void *hndl);
 *
 * @brief   Need to be called while closing the application
 *
 * @param   *hndl  Object handle of algorithm handle to be deleted
 *
 * @return  SUCESS/FAIL
********************************************************************************
*/
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
int ALG_aewbDelete(void *hndl);
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
int ALG_aewbControl(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _ALG_TI_AEWB_H_ */

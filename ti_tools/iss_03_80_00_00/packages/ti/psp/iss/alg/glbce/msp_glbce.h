/* ==============================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * =========================================================================== */
/**
 * @file msp_glbce.h
 *
 * This header file defines the public interface of MSP GLBCE module.
 * 
 *
 * @path  \WTSD_DucatiMMSW\alg\glbce\
 *
 * @rev  1.0
 */
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 20-08-2010 Anoop K P: Initial Release
 *!
 *!Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */
#ifndef _MSP_GLBCE_H_
#define _MSP_GLBCE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

   /* User code goes here */
   /* ------compilation control switches ----------------------------------------*/
   /****************************************************************
    * INCLUDE FILES
    ****************************************************************/
   /* ----- system and platform files --------------------------*/
   /*-------program files ----------------------------------------*/
   #include "../jpeg_enc/inc/msp.h"
    /*--------------------------- END -------------------------------*/
    /****************************************************************
     * EXTERNAL REFERENCES NOTE : only use if not found in header file
     ****************************************************************/
    /*--------data declarations -----------------------------------*/
    /*--------function prototypes --------------------------------*/
    /*--------------------------- END -------------------------------*/
    /****************************************************************
     * PUBLIC DECLARATIONS Defined here, used elsewhere
     ****************************************************************/
    /*--------data declarations ----------------------------------*/

/*--------macros -----------------------------------------------*/

/*--------------------------- END ------------------------------*/

/*-----------------------------------------------------------------------------*/

/* ======================================================================= */
/*! \enum MSP_INDEXTYPE_GLBCE
 *  Index types for configuring MSP GLBCE component (used in MSP_config call)
 * ]
 * @param MSP_INDEXTYPE_COMPUTE_WEIGHT   : Compute the LBCE weight
 * @param MSP_INDEXTYPE_FREE_WEIGHT      : Free the weight memory
 */
    typedef enum {
        MSP_INDEXTYPE_COMPUTE_WEIGHT,
        MSP_INDEXTYPE_FREE_WEIGHT,
    } MSP_INDEXTYPE_GLBCE;


/* ======================================================================= */
/*! \enum MSP_GLBCE_PORT_TYPE
 *  Port definition for input and output ports of MSP GLBCE component
 *  (used in MSP_process call)
 * ]
 * @param MSP_GLBCE_INPUT_PORT     : Input port
 * @param MSP_GLBCE_OUTPUT_PORT    : Output port
 */
    typedef enum {
        MSP_GLBCE_INPUT_PORT = 0,
        MSP_GLBCE_OUTPUT_PORT = 1
    } MSP_GLBCE_PORT_TYPE;


/* ======================================================================= */
/*! \enum MSP_GLBCE_OPERATE_MODE
 *  Mode of operation of algorithm; can be Preview or Capture
 *
 * @param MSP_GLBCE_PREVIEW_MODE    : Preview mode
 * @param MSP_GLBCE_CAPTURE_MODE    : Capture mode
 */
    typedef enum {
        MSP_GLBCE_PREVIEW_MODE = 0,
        MSP_GLBCE_CAPTURE_MODE = 1,
    } MSP_GLBCE_OPERATE_MODE;


/* ======================================================================= */
/*! \enum MSP_GLBCE_BAYER_CFA_PHASE
 *  Enumeration for Sensor type based on the phase of the CFA
 *
 * @param MSP_GLBCE_CFA_PHASE_R  : RGr GbB in alternate rows RGrRGr... GbBGbB...
 * @param MSP_GLBCE_CFA_PHASE_Gr : GrR BGb in alternate rows GrRGrR... BGbBGb...
 * @param MSP_GLBCE_CFA_PHASE_Gb : GbB RGr in alternate rows GbBGbB... RGrRGr...
 * @param MSP_GLBCE_CFA_PHASE_B  : BGb GrR in alternate rows BGbBGb... GrRGrR...
 */

    typedef enum {

        MSP_GLBCE_CFA_PHASE_R = 0,
        MSP_GLBCE_CFA_PHASE_Gr,
        MSP_GLBCE_CFA_PHASE_Gb,
        MSP_GLBCE_CFA_PHASE_B,

    } MSP_GLBCE_BAYER_CFA_PHASE;


/*Weight table parameters for LBCE computation*/
    typedef struct {
        MSP_S16 wImgVertSize;
        MSP_S16 wImgHorzSize;
        MSP_S16 wDownsampleRatio;
        MSP_U8 *pWeightTableLBCE;
        MSP_PTR pHeapInst;
    } MSP_GLBCE_WEIGHT_PARAMS;


/*Capture parameters for the sensor*/
    typedef struct {

        MSP_S32 nExposureTime;
        MSP_S16 wAnalogGain;
        MSP_S16 wDigitalGain;
        MSP_S16 wAnalogGainDivider;
        MSP_S16 wDigitalGainDivider;
        MSP_S16 wAperture;
        MSP_S16 wAperturedivider;


    } MSP_GLBCE_CAPTURE_PARAMS;


/*White balance parameters from AWB algorithm*/
    typedef struct {

        MSP_S16 wWB_RGain;
        MSP_S16 wWB_GrGain;
        MSP_S16 wWB_GbGain;
        MSP_S16 wWB_BGain;

    } MSP_GLBCE_WHITE_BALANCE_PARAMS;


/*RGB2RGB conversion coefficients from AWB algorithm*/
    typedef struct {

        MSP_S16 wRRCoef;
        MSP_S16 wRGCoef;
        MSP_S16 wRBCoef;
        MSP_S16 wGRCoef;
        MSP_S16 wGGCoef;
        MSP_S16 wGBCoef;
        MSP_S16 wBRCoef;
        MSP_S16 wBGCoef;
        MSP_S16 wBBCoef;

    } MSP_GLBCE_RGB2RGB_COEFFS;


/*Noise Filter Tuning parameters*/
    typedef struct {

        MSP_S16 *pwAllBlkMeans;
        MSP_S16 *pwLocalGains;
        MSP_S16  wMeanLocalGains;
        MSP_S16  wMeanBlkMean;
        MSP_S16  wGainAtZero;
        MSP_S16  wGainAtMax;
        MSP_S8   bLbceBlks;

    } MSP_GLBCE_NOISE_TUNING_PARAMS;


/* CreationTime parameter structure for GLBCE */
    typedef struct {

        MSP_S16                wImgVertSize;
        MSP_S16                wImgHorzSize;
        MSP_S16                wNumBits;
        MSP_S16                wDownsampleRatio;
        MSP_GLBCE_OPERATE_MODE eOperateMode;
        MSP_BOOL               fUseInputGammaTable;
        MSP_S16               *pInputGammaTable;
        MSP_S16                wNumGainLimitPoints;
        MSP_S16               *pGainLimitXPoints;
        MSP_S16               *pGainLimitYPoints;
        MSP_S8                *pLimitTable;
        MSP_S16                wExternalGamma_XRange;
        MSP_S16                wExternalGamma_YRange;
        MSP_BOOL               fLSCCorrectedBoxcar; // This flag is currently IGNORED by the library

    } MSP_GLBCE_CREATE_PARAMS;


/*GBCE input parameters during a run for one image */
    typedef struct {

        MSP_GLBCE_CAPTURE_PARAMS       stCaptureParams;
        MSP_GLBCE_WHITE_BALANCE_PARAMS stWB_params;
        MSP_GLBCE_RGB2RGB_COEFFS       stRgb2RgbCoeffs;
        MSP_GLBCE_BAYER_CFA_PHASE      eSensorType;
        MSP_S16                        wGBEStrength;
        MSP_S16                        wGCEStrength;

        MSP_BOOL fEnableLBCE;
        MSP_U8  *pWeightTableLBCE;

        MSP_S16 wLBEStrength; //Q4.12 format
        MSP_S16 wLCEStrength; //Q4.12 format

        MSP_S8 *pLSCTable;
        MSP_S8  bLSCDownSampleRatio;


    } MSP_GLBCE_RUNTIME_INPUT_PARAMS;


/*GLBCE output parameters during a run for one image*/
    typedef struct {

        MSP_S8                        bSToneValue;
        MSP_S8                        bMToneValue;
        MSP_S8                        bHToneValue;
        MSP_S8                        bLowerCutoffValue;
        MSP_GLBCE_NOISE_TUNING_PARAMS stNSFTuningParams;

    } MSP_GLBCE_RUNTIME_OUTPUT_PARAMS;


#ifdef __cplusplus
        }
#endif /* __cplusplus */

#endif /* !_MSP_GLBCE_H_ */


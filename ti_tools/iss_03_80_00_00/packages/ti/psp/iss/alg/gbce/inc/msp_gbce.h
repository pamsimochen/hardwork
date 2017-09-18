/* ============================================================================== 
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved.  Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * =========================================================================== 
 */
/**
 * @file msp_gbce.h
 *
 * This header file defines the public interface of MSP GBCE module.
 * 
 *
 * @path  $(DUCATIVOB)\alg\gbce\
 *
 * @rev  1.0
 */
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 15-11-2009 Sanish Mahadik: Initial Release
 *!
 *!Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */
#ifndef _MSP_GBCE_H_
#define _MSP_GBCE_H_

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

    /* User code goes here */
    /* ------compilation control switches
     * ---------------------------------------- */
   /****************************************************************
    * INCLUDE FILES
    ****************************************************************/
    /* ----- system and platform files -------------------------- */
   /*-------program files ----------------------------------------*/
#include "../../framework/msp/msp.h"
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

    /* ======================================================================= 
     */
    /* ! \enum MSP_GBCE_PORT_TYPE Port definition for input and output ports
     * of MSP component (used in MSP_process call) @param
     * MSP_GBCE_INPUT_PORT : Input port @param MSP_GBCE_OUTPUT_PORT : Output
     * port */
    typedef enum {
        MSP_GBCE_INPUT_PORT = 0,
        MSP_GBCE_OUTPUT_PORT = 1
    } MSP_GBCE_PORT_TYPE;

    /* Mode of operation of algorithm; can be Preview or Capture */
    typedef enum {
        MSP_GBCE_PREVIEW_MODE = 0,
        MSP_GBCE_CAPTURE_MODE = 1,
    } MSP_GBCE_OPERATE_MODE;

    /* Size of the GBCE/Gamma table output by the algorithm */
    typedef enum {
        MSP_GBCE_TABLESIZE_256 = 8,
        MSP_GBCE_TABLESIZE_512 = 9,
        MSP_GBCE_TABLESIZE_1024 = 10,
    } MSP_GBCE_TABLESIZE;

    /* Enumeration for Strength of algorithm. Higher value implies higher
     * contribution of GBCE algorithm */
    typedef enum {
        MSP_GBCE_STRENGTH_0 = 0,
        MSP_GBCE_STRENGTH_1 = 512,
        MSP_GBCE_STRENGTH_2 = 1024,
        MSP_GBCE_STRENGTH_3 = 1536,
        MSP_GBCE_STRENGTH_4 = 2048,
    } MSP_GBCE_ALGO_STRENGTH;

    /* CreationTime parameter structure for GBCE */
    typedef struct {

        MSP_GBCE_OPERATE_MODE OperateMode;
        MSP_GBCE_TABLESIZE TableSize;

        MSP_BOOL UseInputGammaTable;
        MSP_S32 *InputGammaTable;

        /* GBCE Strength */
        MSP_GBCE_ALGO_STRENGTH StregnthOutdoor;
        MSP_GBCE_ALGO_STRENGTH StregnthIndoor;
        MSP_GBCE_ALGO_STRENGTH StregnthDark;

    } MSP_GBCE_CREATE_PARAMS;

    /* GBCE input parameters during a run for one image */
    typedef struct {
        MSP_S32 ExposureTime;
        MSP_S32 AnalogGain;
        MSP_S32 DigitalGain;
        MSP_S32 Aperture;

        /* For Future Use */
        MSP_U8 BacklitDetect;
        MSP_U8 SkyDetect;
        MSP_U8 FoliageDetect;

    } MSP_GBCE_RUNTIME_INPUT_PARAMS;

    /* GBCE output parameters during a run for one image */
    typedef struct {
        MSP_S16 GbceLowerCutoff;
    } MSP_GBCE_RUNTIME_OUTPUT_PARAMS;

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* !_MSP_GBCE_H_ */

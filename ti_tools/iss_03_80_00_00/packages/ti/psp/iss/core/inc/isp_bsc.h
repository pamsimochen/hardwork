/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file bsc_sdma.h
*
* This file contains declarations for BSC SDMA driver
*
* @path iss/drivers/bsc_sdma/src
*
* @rev 01.01
*/
/* -------------------------------------------------------------------------- 
 */
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *! 22-Jul-2009 
 * Petar Sivenov - Created *! *
 * =========================================================================== */
#ifndef _ISP_BSC_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISP_BSC_H_

#include <ti/psp/iss/core/msp_types.h>
/* User code goes here */
/* ------compilation control switches --------------------------------------- 
 */
/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
/* ----- system and platform files ---------------------------- */
/*-------program files ----------------------------------------*/

/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/

// #define BS_BLANK_LINES 32 // blanking lines for BSC (atleast 16 lines)
#define   BS_NUM_VECT                   (3)                // Divide image
                                                           // into M
                                                           // horizontal
                                                           // parts
#define   BSC_BUFF_SIZE                 (1920)             // max shorts in
                                                           // BSC output
                                                           // buffers
#define   BSC_ELEM_SIZE                 (2)                // max shorts in
                                                           // BSC output
                                                           // buffers
#define   BSC_BUFF_SIZE_BYTES           (BSC_BUFF_SIZE * BSC_ELEM_SIZE) // max 
                                                                        // shorts 
                                                                        // in 
                                                                        // BSC 
                                                                        // output 
                                                                        // buffers

typedef enum {
    DRV_BSC_ENABLE_OFF,
    DRV_BSC_ENABLE_ON
} drvBscEnableT;

typedef enum {
    DRV_BSC_MODE_FREE_RUN,
    DRV_BSC_MODE_SINGLE
} drvBscModeT;

typedef enum {
    DRV_BSC_SAMPLING_DISABLED,
    DRV_BSC_SAMPLING_ENABLED
} drvBscSamplingEnableT;

typedef enum {
    DRV_BSC_ELEMENT_Y,
    DRV_BSC_ELEMENT_Cb,
    DRV_BSC_ELEMENT_Cr
} drvBscSamplingTypeT;

typedef struct {
    MSP_U16 nInImgWidth;
    MSP_U16 nInImgHeight;
    MSP_U16 nInImgStartX;
    MSP_U16 nInImgStartY;
    MSP_U16 nNumVectors;
} bscInputCfgT;

/*--------function prototypes ---------------------------------*/
// MSP_ERROR_TYPE bscSdmaInit(MSP_APPCBPARAM_TYPE *ptAppCBParam,
// MSP_Sdma_ChannelEventCallback sdmaCallback);
// bscSdmaDeinit();
// bscSdmaConfig(MSP_U32 nBscSdmaChanId, MSP_Sdma_ChannelEventCallback
// sdmaCallback);
// bscSdmaStart();
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/* ===================================================================
 *  @func     bscDataGet                                               
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
MSP_ERROR_TYPE bscDataGet(MSP_PTR pRowSums, MSP_PTR pColSums, MSP_U32 row_size,
                          MSP_U32 col_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _ISP_BSC_H_ */

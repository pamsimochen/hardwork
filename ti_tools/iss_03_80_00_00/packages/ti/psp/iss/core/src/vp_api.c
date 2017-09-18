/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file vp_api.c
*
* This file contains routines for Parallel video interface driver
*
* @path iss/drivers/drv_vp/src
*
* @rev 01.00
*/
/* -------------------------------------------------------------------------- 
 */
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *!
 * 05-Apr-2011 LVasilev - Created *! *
 * =========================================================================== */

/* User code goes here */
/* ------compilation control switches --------------------------------------- 
 */
/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
/* ----- system and platform files ---------------------------- */
#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>
#include <cram.h>
#include <baseaddress.h>

/*-------program files ----------------------------------------*/
#include "../../../iss/framework/msp/msp_utils.h"
#include "../../../framework/msp/msp_types.h"
#include "csl/iss/iss_common/iss_common.h"
// #include <drv_vp/vp_msp.h"
#include "../../../drivers/drv_vp/inc/vp_api.h"
// #include <drv_vp/inc/vp_private.h"
// #include "../../../framework/msp/inc/msp.h"

/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/

// #define OMAP4430_CONTROL_CORE DEVICE_CORE_CONTROL_MODULE

/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/

/* ===================================================================
 *  @func     vp_init                                               
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
MSP_ERROR_TYPE vp_init(void)
{
    MSP_ERROR_TYPE ret = MSP_ERROR_NONE;

    iss_module_clk_ctrl(ISS_CSI2A_CLK, ISS_CLK_DISABLE);
    iss_module_clk_ctrl(ISS_CSI2B_CLK, ISS_CLK_DISABLE);
    iss_isp_input_config(ISS_ISP_INPUT_PI);

    return ret;
}

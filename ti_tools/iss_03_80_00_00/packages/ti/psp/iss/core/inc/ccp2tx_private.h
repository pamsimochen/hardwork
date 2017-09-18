/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file ccp2tx_api.h
*
* This file contains private routines declarations and defines for CCP
*       transmitter driver
*
* @path ti/iss/drivers/drv_ccp2tx/inc
*
* @rev 01.00
*/
/* -------------------------------------------------------------------------- 
 */

#ifndef _CCP2_TX_PRIVATE_H
#define _CCP2_TX_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

    /* User code goes here */
    /* ------compilation control switches
     * --------------------------------------- */
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
/*--------macros ----------------------------------------------*/
#define CCPTX_CORE_INT      (60)
/*--------data declarations -----------------------------------*/
    /* ============================================================================================== 
     */
/**
* enum CCP2TX_CORE_CCPT_CONFIG_BIT_SEL_T - describes possible values of CCP bit shift select
*
* @see
**/
    /* ============================================================================================== 
     */
    typedef enum {
        CCPT_CONFIG_LSB_TO_BIT0,
        CCPT_CONFIG_LSB_TO_BIT1,
        CCPT_CONFIG_LSB_TO_BIT2,
        CCPT_CONFIG_LSB_TO_BIT3,
        CCPT_CONFIG_LSB_TO_BIT4,
        CCPT_CONFIG_LSB_TO_BIT5,
        CCPT_CONFIG_LSB_TO_BIT6,
        CCPT_CONFIG_LSB_TO_BIT7,
        CCPT_CONFIG_LSB_TO_BIT8
    } CCP2TX_CORE_CCPT_CONFIG_BIT_SEL_T;

    /* ============================================================================================== 
     */
/**
* enum CCP2TX_CORE_CCPT_CONFIG_BURST_LENGTH_T - describes possible values of OCP burst length
*
* @see
**/
    /* ============================================================================================== 
     */
    typedef enum {
        CCPT_CONFIG_BURST_LENGTH_4,
        CCPT_CONFIG_BURST_LENGTH_8,
        CCPT_CONFIG_BURST_LENGTH_16
    } CCP2TX_CORE_CCPT_CONFIG_BURST_LENGTH_T;

    /* ============================================================================================== 
     */
/**
* enum CCP2TX_CORE_CCPT_CONFIG_CLK_STRB_T - describes possible values of CCP signaling
*
* @see
**/
    /* ============================================================================================== 
     */
    typedef enum {
        CCPT_CONFIG_CLK_STROBE,
        CCPT_CONFIG_CLK_CLOCK
    } CCP2TX_CORE_CCPT_CONFIG_CLK_STRB_T;

    /* ============================================================================================== 
     */
/**
* enum CCP2TX_CORE_CCPT_CONTROL_FORMAT_I_T - describes possible values of CCP input format
*
* @see
**/
    /* ============================================================================================== 
     */
    typedef enum {
        CCPT_CONTROL_FORMAT_I_YUV422,
        CCPT_CONTROL_FORMAT_I_RAW_1BYTE,
        CCPT_CONTROL_FORMAT_I_RAW_2BYTE,
        CCPT_CONTROL_FORMAT_I_JPEG8,
        CCPT_CONTROL_FORMAT_I_EDATA
    } CCP2TX_CORE_CCPT_CONTROL_FORMAT_I_T;

    /* ============================================================================================== 
     */
/**
* enum CCP2TX_CORE_CCPT_CONTROL_FORMAT_O_T - describes possible values of CCP output format
*
* @see
**/
    /* ============================================================================================== 
     */
    typedef enum {
        CCPT_CONTROL_FORMAT_O_YUV422,
        CCPT_CONTROL_FORMAT_O_YUV420,
        CCPT_CONTROL_FORMAT_O_RGB888,
        CCPT_CONTROL_FORMAT_O_RGB565,
        CCPT_CONTROL_FORMAT_O_RGB444,
        CCPT_CONTROL_FORMAT_O_RAW8,
        CCPT_CONTROL_FORMAT_O_RAW8_DPCM,
        CCPT_CONTROL_FORMAT_O_RAW10,
        CCPT_CONTROL_FORMAT_O_RAW12,
        CCPT_CONTROL_FORMAT_O_JPEG8,
        CCPT_CONTROL_FORMAT_O_EDATA
    } CCP2TX_CORE_CCPT_CONTROL_FORMAT_O_T;

/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _CCP2_TX_PRIVATE_H 
                                                            */

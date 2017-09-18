/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file ccp2tx_msp.h
*
* This file contains types and defines for CCP transmitter driver MSP Interface
*
* @path ti/iss/drivers/drv_ccp2tx/src
*
* @rev nn.mm
*/
/* -------------------------------------------------------------------------- 
 */

#ifndef _CCP2_MSP_H
#define _CCP2_MSP_H

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
#include <ti/psp/iss/core/msp_types.h>

/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/

    /* ========================================================================== 
     */
/**
* struct MSP_drvCcp2txIntMaskT - describes CCP interrupt events
*
* @see
**/
    /* ========================================================================== 
     */
    typedef struct {
    /** Flag disabling CCP2 Tx End interrupt*/
        MSP_U32 drvCcp2txMaskCcp2txEnd:1;
    /** Flag disabling CCP2 FIFO Underflow interrupt*/
        MSP_U32 drvCcp2txMaskFifoUnf:1;
    /** Flag disabling CCP2 DMA End interrupt*/
        MSP_U32 drvCcp2txMaskDmaEnd:1;
    /** Flag disabling CCP2 All End interrupt*/
        MSP_U32 drvCcp2txMaskAllEnd:1;
        MSP_U32 reserved:28;
    } MSP_drvCcp2txIntMaskT;

    /* ========================================================================== 
     */
/**
* enum MSP_drvCcp2txModesT - describes possible CCP modes
*
* @see
**/
    /* ========================================================================== 
     */
    typedef enum {
        /* CCP1 Mode */
        DRV_CCPT_CCP1,
        /* CCP2 Class 0 Mode */
        DRV_CCPT_CCP2_MODE_CLASS0,
        /* CCP2 Class 1 Mode */
        DRV_CCPT_CCP2_MODE_CLASS1,
        /* CCP2 Class 2 Mode */
        DRV_CCPT_CCP2_MODE_CLASS2,
        /* CCP1 Mode */
        DRV_CCPT_CCP2_MODE_NUM
    } MSP_drvCcp2txModesT;

    /* ========================================================================== 
     */
/**
* enum MSP_drvCcp2txModesT - describes possible CCP modes
*
* @see
**/
    /* ========================================================================== 
     */
    typedef enum {
        /* Burst length 4 words */
        DRV_CCPT_BURST_LENGTH_4,
        /* Burst length 8 words */
        DRV_CCPT_BURST_LENGTH_8,
        /* Burst length 16 words */
        DRV_CCPT_BURST_LENGTH_16
    } MSP_drvCcp2txBurstWidthT;

    /* ========================================================================== 
     */
/**
* enum MSP_drvCcp2txModesT - describes possible CCP modes
*
* @see
**/
    /* ========================================================================== 
     */
    typedef enum {
        /* LSB to bit0 in 2byte */
        DRV_CCPT_LSB_TO_BIT0,
        /* LSB to bit1 in 2byte */
        DRV_CCPT_LSB_TO_BIT1,
        /* LSB to bit2 in 2byte */
        DRV_CCPT_LSB_TO_BIT2,
        /* LSB to bit3 in 2byte */
        DRV_CCPT_LSB_TO_BIT3,
        /* LSB to bit4 in 2byte */
        DRV_CCPT_LSB_TO_BIT4,
        /* LSB to bit5 in 2byte */
        DRV_CCPT_LSB_TO_BIT5,
        /* LSB to bit6 in 2byte */
        DRV_CCPT_LSB_TO_BIT6,
        /* LSB to bit7 in 2byte */
        DRV_CCPT_LSB_TO_BIT7,
        /* LSB to bit8 in 2byte */
        DRV_CCPT_LSB_TO_BIT8
    } MSP_drvCcp2txBitSelT;

    /* ========================================================================== 
     */
/**
* struct MSP_drvCcp2txParamsT - describes CCP general configuration parameters
*
* @see
**/
    /* ========================================================================== 
     */
    typedef struct {
    /** CCP Tx Mode*/
        MSP_drvCcp2txModesT ccp2txMode;
    /** CCP Tx Burst mode*/
        MSP_drvCcp2txBurstWidthT burstWidth;
    /** CCP Tx Bit select */
        MSP_drvCcp2txBitSelT bitSelect;
    } MSP_drvCcp2txGeneralT;

    /* ========================================================================== 
     */
/**
* struct MSP_drvCcp2txCodesT - describes CCP codes
*
* @see
**/
    /* ========================================================================== 
     */
    typedef struct {
    /** Code Frame Start */
        MSP_U32 drvCcp2txCodeFsOn:1;
    /** Code Frame End */
        MSP_U32 drvCcp2txCodeFeOn:1;
    /** Code Line Start */
        MSP_U32 drvCcp2txCodeLsOn:1;
    /** Code Line End */
        MSP_U32 drvCcp2txCodeLeOn:1;
        MSP_U32 reserved:28;
    } MSP_drvCcp2txCodesT;

    /* ========================================================================== 
     */
/**
* enum MSP_drvCcp2txFormatInT - describes possible values of CCP input formats
*
* @see
**/
    /* ========================================================================== 
     */
    typedef enum {
    /** Input format YUV 422 */
        DRV_CCPT_FORMAT_IN_YUV422,
    /** Input format 8bit RAW */
        DRV_CCPT_FORMAT_IN_RAW_1BYTE,
    /** Input format 16bit RAW */
        DRV_CCPT_FORMAT_IN_RAW_2BYTE,
    /** Input format JPEG */
        DRV_CCPT_FORMAT_IN_JPEG8,
    /** Input format data */
        DRV_CCPT_FORMAT_IN_EDATA,
    /** Total number of input formats */
        DRV_CCPT_FORMAT_IN_NUM
    } MSP_drvCcp2txFormatInT;

    /* ========================================================================== 
     */
/**
* enum MSP_drvCcp2txFormatOutT - describes possible values of CCP output formats
*
* @see
**/
    /* ========================================================================== 
     */
    typedef enum {
    /** Output format YUV 422 */
        DRV_CCPT_FORMAT_OUT_YUV422,
    /** Output format YUV 420 */
        DRV_CCPT_FORMAT_OUT_YUV420,
    /** Output format RGB 888 */
        DRV_CCPT_FORMAT_OUT_RGB888,
    /** Output format RGB 565 */
        DRV_CCPT_FORMAT_OUT_RGB565,
    /** Output format RGB 444 */
        DRV_CCPT_FORMAT_OUT_RGB444,
    /** Output format 8bit RAW */
        DRV_CCPT_FORMAT_OUT_RAW8,
    /** Output format 8bit DPCM RAW */
        DRV_CCPT_FORMAT_OUT_RAW8_DPCM,
    /** Output format 10bit RAW */
        DRV_CCPT_FORMAT_OUT_RAW10,
    /** Output format 12bit RAW */
        DRV_CCPT_FORMAT_OUT_RAW12,
    /** Output format JPEG */
        DRV_CCPT_FORMAT_OUT_JPEG8,
    /** Output format data */
        DRV_CCPT_FORMAT_OUT_EDATA,
    /** Total number of output formats */
        DRV_CCPT_FORMAT_OUT_NUM
    } MSP_drvCcp2txFormatOutT;

    /* ========================================================================== 
     */
/**
* struct MSP_drvCcp2txTransferT - describes CCP transfer parameters
*
* @see
**/
    /* ========================================================================== 
     */
    typedef struct {
    /** Pointer to input data */
        MSP_PTR buf;
    /** Total bytes per line */
        MSP_U16 lineLenTotal;
    /** Valid bytes per line */
        MSP_U16 lineLenValid;
    /** Total number of lines */
        MSP_U16 lineCount;
    /** CCP Tx Logical channel */
        MSP_U16 logicalChannel;
    /** CCP Tx code */
        MSP_drvCcp2txCodesT code;
    /** CCP Tx input format */
        MSP_drvCcp2txFormatInT formatIn;
    /** CCP Tx output format */
        MSP_drvCcp2txFormatOutT formatOut;
    } MSP_drvCcp2txTransferT;

    typedef struct {
        // MSP_drvCcp2txNotifyTransferEndT handle;
    /** CCP Tx Interrupt mask */
        MSP_drvCcp2txIntMaskT isrMask;
    } MSP_drvCcp2txIsrT;

    /* ========================================================================== 
     */
/**
* struct MSP_drvCcp2txMatrixT - describes CCP matrix coefficients
*
* @see
**/
    /* ========================================================================== 
     */
    typedef struct {
    /** Y coefficient */
        MSP_U16 matCoefgY;
    /** V coefficient */
        MSP_U16 matCoefrV;
    /** gU coefficient */
        MSP_U16 matCoefgU;
    /** gV coefficient */
        MSP_U16 matCoefgV;
    /** bU coefficient */
        MSP_U16 matCoefbU;
    /** Offset level */
        MSP_U16 offsetLvl;
    /** Upper clip */
        MSP_U16 upperClip;
    } MSP_drvCcp2txMatrixT;

    /* ========================================================================== 
     */
/**
* enum MSP_CCP_INDEXTYPE - describes possible values of MSP Config and Query
*                           indices.
*
* @see
**/
    /* ========================================================================== 
     */
    typedef enum {

    /** Get Config slot ID query */
        MSP_CCP_INDEXTYPE_GET_CSID,
    /** Get Transfer ID query */
        MSP_CCP_INDEXTYPE_GET_TID,
    /** Get General configuration query */
        MSP_CCP_INDEXTYPE_GET_GENERAL_CFG,
    /** Get ISR configuration query */
        MSP_CCP_INDEXTYPE_GET_ISR_CFG,
    /** Get YUV2RGB matrix configuration query */
        MSP_CCP_INDEXTYPE_GET_RGB_MATRIX_CFG,
    /** Get transfer configuration query */
        MSP_CCP_INDEXTYPE_GET_TRANSFER_CFG,
    /** Get line count query */
        MSP_CCP_INDEXTYPE_GET_LINE_COUNT_CFG,
    /** Get address query */
        MSP_CCP_INDEXTYPE_GET_ADDRESS_CFG,
    /** Set general configuration */
        MSP_CCP_INDEXTYPE_SET_GENERAL_CFG,
    /** Set ISR configuration */
        MSP_CCP_INDEXTYPE_SET_ISR_CFG,
    /** Set YUV2RGB matrix configuration */
        MSP_CCP_INDEXTYPE_SET_RGB_MATRIX_CFG,
    /** Set transfer configuration */
        MSP_CCP_INDEXTYPE_SET_TRANSFER_CFG,
    /** Set line count */
        MSP_CCP_INDEXTYPE_SET_LINE_COUNT_CFG,
    /** Set address */
        MSP_CCP_INDEXTYPE_SET_ADDRESS_CFG,
    /** Total indexes number */
        MSP_CCP_INDEXTYPE_NUM_MAX
    } MSP_CCP_INDEXTYPE;

    /* ========================================================================== 
     */
/**
* struct MSP_drvCcp2txConfigT - describes CCP transmitter configuration
*
* @see
**/
    /* ========================================================================== 
     */
    typedef struct {
    /** General configuration parameters */
        MSP_drvCcp2txGeneralT generalParams;
    /** ISR configuration parameters */
        MSP_drvCcp2txIsrT isrParams;
    /** YUV2RGB matrix configuration parameters */
        MSP_drvCcp2txMatrixT matrixParams;
    /** Transfer configuration parameters */
        MSP_drvCcp2txTransferT transferParams;

    /** Transfer ID */
        MSP_U32 transferID;
    } MSP_drvCcp2txConfigT;

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
#endif                                                     /* _CCP2_MSP_H */

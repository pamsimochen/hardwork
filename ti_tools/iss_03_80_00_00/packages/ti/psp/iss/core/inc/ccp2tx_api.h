/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file ccp2tx_api.h
*
* This file contains routines declarations and defines for CCP transmitter
*       driver core
*
* @path ti/iss/drivers/drv_ccp2tx/inc
*
* @rev 01.01
*/
/* -------------------------------------------------------------------------- 
 */
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *!
 * 13-Feb-2009 Petar Sivenov - Created *! *! 27-Mar-2009 Petar Sivenov - MSP
 * interface added *! *
 * =========================================================================== */
#ifndef _CCP2_TX_API_H
#define _CCP2_TX_API_H

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
#include <ti/psp/iss/core/msp_types.h>
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

/** @def CCP_TXBLNKPAD_LEN
* CCP transmitter blanking length */
#define CCP_TXBLNKPAD_LEN   (0x10)

/** @def CCP_TXBLNKPAD_VAL
* CCP transmitter blanking data value */
#define CCP_TXBLNKPAD_VAL   (0x80000001)

/** @def CCP_TXBLNKPAD_VAL_CRC
* CCP transmitter CRC padding value */
#define CCP_TXBLNKPAD_VAL_CRC    (0xAA)

/** @def CCP_TXBLNKPAD_VAL_JPEG
* CCP transmitter JPEG padding value */
#define CCP_TXBLNKPAD_VAL_JPEG   (0xA5)

/*--------data declarations -----------------------------------*/

    /* ========================================================================= 
     */
/**
* enum drvCcp2txReturnT - describes possible error messages returned by CCP
*                           driver API
*
* @see
**/
    /* ========================================================================== 
     */
    typedef enum {
        DEV_CCPT_ERROR_OK,                                 // !< No error
        DEV_CCPT_ERROR_DEV,                                // !< Incorrect
                                                           // CCP Device
                                                           // instance
        DEV_CCPT_ERROR_CONFIG,                             // !< Incorrect
                                                           // configuration
        DEV_CCPT_ERROR_NUM                                 // !< Total number 
                                                           // of errors
            // TODO: Add possible errors here
    } drvCcp2txReturnT;

    /* ========================================================================== 
     */
/**
* struct drvCcp2txDevT - describes CCP device handle
*
* @see
**/
    /* ========================================================================== 
     */
    typedef struct {
        MSP_U8 instance;                                   // !< CCP device
                                                           // instance
    } drvCcp2txDevT;

/** CCP Tx ISR type */
    typedef void (*drvCcp2txNotifyTransferEndT) (void);

    /* ========================================================================== 
     */
/**
* struct drvCcp2txRevT - describes CCP revision type
*
* @see
**/
    /* ========================================================================== 
     */
    typedef struct {
        MSP_U16 majorRevision;                             // !< CCP Tx Core
                                                           // HW major
                                                           // revision
        MSP_U16 minorRevision;                             // !< CCP Tx Core
                                                           // HW minor
                                                           // revision
    } drvCcp2txRevT;

/*--------function prototypes ---------------------------------*/

    /* ========================================================================== 
     */
/**
* drvCcp2txInit() - Initializes CCP device
*
* @param ccp2txDevHandle - drvCcp2txDevT* - Pointer to CCP device handle
*
* @return
*
* @pre There is no pre conditions.
*
* @post There is no post conditions.
*
* @see
*
*/
    /* ========================================================================== 
     */
    void drvCcp2txInit(drvCcp2txDevT * ccp2txDevHandle);

    /* ========================================================================== 
     */
/**
* drvCcp2txIntEnable() - Enables CCP interrupt
*
* @param ccp2txDevHandle - drvCcp2txDevT* - Pointer to CCP device handle
*
* @param ccp2txIntMask - drvCcp2txIntMaskT* - Pointer to CCP interrupt masks
*
* @return   DEV_CCPT_ERROR_OK       - Device configured successfully
*           DEV_CCPT_ERROR_DEV      - Uninitialized/wrong device
*
* @pre There is no pre conditions.
*
* @post There is no post conditions.
*
* @see
*
*/
    /* ========================================================================== 
     */
    drvCcp2txReturnT drvCcp2txIntEnable(drvCcp2txDevT * ccp2txDevHandle,
                                        MSP_drvCcp2txIntMaskT ccp2txIntMask);

    /* ========================================================================== 
     */
/**
* drvCcp2txGenConfigSet() - Configures CCP device
*
* @param ccp2txDevHandle - drvCcp2txDevT* - Pointer to CCP device handle
*
* @param ccp2txParams - drvCcp2txParamsT* - Pointer to CCP configuration
*                                           parameters
*
* @return   DEV_CCPT_ERROR_OK       - Device configured successfully
*           DEV_CCPT_ERROR_DEV      - Uninitialized/wrong device
*           DEV_CCPT_ERROR_CONFIG   - Incorrect configuration parameters
*
* @pre CCP device must be initialized
*
* @post There is no post conditions.
*
* @see
*
*/
    /* ========================================================================== 
     */
    drvCcp2txReturnT drvCcp2txGenConfigSet(drvCcp2txDevT * ccp2txDevHandle,
                                           MSP_drvCcp2txGeneralT *
                                           ccp2txParams);

    /* ========================================================================== 
     */
/**
* drvCcp2txInit() - Gets CCP device configuration parameters
*
* @param ccp2txDevHandle - drvCcp2txDevT* - CCP device handle
*
* @param ccp2txParams - drvCcp2txParamsT* - Pointer to CCP configuration
*                                           parameters
*
* @return   DEV_CCPT_ERROR_OK       - Device configuration retrieved
*                                       successfully
*           DEV_CCPT_ERROR_DEV      - Uninitialized/wrong device
*
* @pre CCP device must be initialized
*
* @post There is no post conditions.
*
* @see
*
*/
    /* ========================================================================== 
     */
    drvCcp2txReturnT drvCcp2txConfigGet(drvCcp2txDevT * ccp2txDevHandle,
                                        MSP_drvCcp2txGeneralT * ccp2txParams);

    /* ========================================================================== 
     */
/**
* drvCcp2txInit() - Configures CCP transmitter input address
*
* @param ccp2txDevHandle - drvCcp2txDevT* - CCP device handle
*
* @param bufferAddress - MSP_PTR - Pointer to input data buffer
*
* @return   DEV_CCPT_ERROR_OK       - Device configuration retrieved
*                                       successfully
*           DEV_CCPT_ERROR_DEV      - Uninitialized/wrong device
*
* @pre CCP device must be initialized
*
* @post There is no post conditions.
*
* @see
*
*/
    /* ========================================================================== 
     */
    drvCcp2txReturnT drvCcp2txConfigAddress(drvCcp2txDevT * ccp2txDevHandle,
                                            MSP_PTR bufferAddress);

    /* ========================================================================== 
     */
/**
* drvCcp2txInit() - Configures CCP transmitter input address
*
* @param ccp2txDevHandle - drvCcp2txDevT* - CCP device handle
*
* @param lineCount - MSP_U32 - Input lines count
*
* @return   DEV_CCPT_ERROR_OK       - Device configuration retrieved
*                                       successfully
*           DEV_CCPT_ERROR_DEV      - Uninitialized/wrong device
*
* @pre CCP device must be initialized
*
* @post There is no post conditions.
*
* @see
*
*/
    /* ========================================================================== 
     */
    drvCcp2txReturnT drvCcp2txConfigLineCount(drvCcp2txDevT * ccp2txDevHandle,
                                              MSP_U32 lineCount);

    /* ========================================================================== 
     */
/**
* drvCcp2txInit() - Sets CCP device transfer configuration parameters
*
* @param ccp2txDevHandle - drvCcp2txDevT* - CCP device handle
*
* @param ccp2txTransfer - MSP_drvCcp2txTransferT* - Pointer to CCP transfer configuration
*
* @return   DEV_CCPT_ERROR_OK       - Device configuration retrieved
*                                       successfully
*           DEV_CCPT_ERROR_DEV      - Uninitialized/wrong device
*
* @pre CCP device must be initialized
*
* @post There is no post conditions.
*
* @see
*
*/
    /* ========================================================================== 
     */
    drvCcp2txReturnT drvCcp2txConfigTransfer(drvCcp2txDevT * ccp2txDevHandle,
                                             MSP_drvCcp2txTransferT *
                                             ccp2txTransfer);

    /* ========================================================================== 
     */
/**
* drvCcp2txSubmitTransfer() - Initiates CCP data transfer
*
* @param ccp2txDevHandle - drvCcp2txDevT* - Pointer to CCP device handle
*
* @param ccp2txTransfer - MSP_drvCcp2txTransferT* - Pointer to CCP transfer
                                                parameters
*
* @param notifyTransferEndHandle - drvCcp2txNotifyTransferEndT - Pointer
                                to user routine called upon transfer completion
*
* @return   DEV_CCPT_ERROR_OK       - Device configured successfully
*           DEV_CCPT_ERROR_DEV      - Uninitialized/wrong device
*           DEV_CCPT_ERROR_CONFIG   - Incorrect transfer parameters
*
* @pre CCP device must be initialized end configured
*
* @post There is no post conditions.
*
* @see
*
*/
    /* ========================================================================== 
     */
    drvCcp2txReturnT drvCcp2txSubmitTransfer(drvCcp2txDevT * ccp2txDevHandle);

    /* ========================================================================== 
     */
/**
* drvCcp2txConfigRgbMatrix() - Configures CCP YUV-to-RGB conversion matrix
*                               coefficients
*
* @param ccp2txDevHandle - drvCcp2txDevT* - Pointer to CCP device handle
*
* @param ccp2txMatrix - MSP_drvCcp2txMatrixT* - Pointer to YUV-to-RGB conversion
*                                           matrix coefficients
*
* @return   DEV_CCPT_ERROR_OK       - Device configured successfully
*           DEV_CCPT_ERROR_DEV      - Uninitialized/wrong device
*
* @pre CCP device must be initialized
*
* @post There is no post conditions.
*
* @see
*
*/
    /* ========================================================================== 
     */
    drvCcp2txReturnT drvCcp2txConfigRgbMatrix(drvCcp2txDevT * ccp2txDevHandle,
                                              MSP_drvCcp2txMatrixT *
                                              ccp2txMatrix);

    /* ========================================================================== 
     */
/**
* drvCcp2txIsComplete() - Checks if CCP transfer is completed
*
* @param ccp2txDevHandle - drvCcp2txDevT* - Pointer to CCP device handle
*
* @return   0       - Transfer is in progress
*           1       - Transfer is completed
*
* @pre CCP device must be initialized
*
* @post There is no post conditions.
*
* @see
*
*/
    /* ========================================================================== 
     */
    MSP_U16 drvCcp2txIsComplete(drvCcp2txDevT * ccp2txDevHandle);

    /* ========================================================================== 
     */
/**
* drvCcp2txGetRevision() - Gets CCP Tx core revision
*
* @param ccp2txDevHandle - drvCcp2txDevT* - Pointer to CCP device handle
*
* @param ccp2txRevision - drvCcp2txRevT* - Pointer to CCP revision struct
*
* @return   DEV_CCPT_ERROR_OK       - Revision retrieved successfully
*           DEV_CCPT_ERROR_DEV      - Uninitialized/wrong device
*
* @pre CCP device must be initialized
*
* @post There is no post conditions.
*
* @see
*
*/
    /* ========================================================================== 
     */
    MSP_U16 drvCcp2txGetRevision(drvCcp2txDevT * ccp2txDevHandle,
                                 drvCcp2txRevT * ccp2txRevision);

    /* ========================================================================== 
     */
/**
 * ccp2_print() Print some text using XDC
 *
 * @param buf Pointer to text buffer
 *
 * @pre none
 *
 * @post none
 */
    /* ========================================================================== 
     */
    void ccp2_print(void *buf);

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _CCP2_TX_API_H */

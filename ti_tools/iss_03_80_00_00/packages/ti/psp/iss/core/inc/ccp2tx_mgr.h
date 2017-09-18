/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file ccp2tx_mgr.h
*
* This file contains routines declarations and defines for CCP transmitter
*       driver manager
*
* @path ti/iss/drivers/drv_ccp2tx/inc
*
* @rev 01.00
*/
/* -------------------------------------------------------------------------- 
 */
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *! 27-Mar-2009 
 * Petar Sivenov: - Created *! *
 * =========================================================================== */

#ifndef _CCP_MGR_H
#define _CCP_MGR_H

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
    // #include <msp/msp.h>
#include "ti/psp/iss/core/inc/ccp2tx_api.h"
#include "ti/psp/iss/core/inc/ccp2tx_private.h"
/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*-------- data declarations -----------------------------------*/
/*-------- function prototypes ---------------------------------*/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------macros ----------------------------------------------*/

    /* ========================================================================== 
     */
/**
 * @def CCP_MGR_INVALID_VALUE - Indicates error when returned
 */
    /* ========================================================================== 
     */
#define CCP_MGR_INVALID_VALUE           (~((MSP_U32)(0)))

    /* ========================================================================== 
     */
/**
 * @def DEV_CCPTX_INSTANCE_MAX_NUM - Maximum number of CCPTx Driver instances.
 *                                  Only one CCP Transmitter is available.
 */
    /* ========================================================================== 
     */
#define DEV_CCPTX_INSTANCE_MAX_NUM      (1)
    /* ========================================================================== 
     */
/**
 * @def DEV_CCPTX_QUEUES_LENGTH - Maximum number of CCPTx transfers queued.
 */
    /* ========================================================================== 
     */
#define DEV_CCPTX_QUEUES_LENGTH         (10)

/** Configuration mask - general parameters */
#define CCP_CFG_MASK_GEN                (1 << 0)
/** Configuration mask - ISR parameters */
#define CCP_CFG_MASK_ISR                (1 << 1)
/** Configuration mask - YUV2RGB matrix parameters */
#define CCP_CFG_MASK_MATRIX             (1 << 2)
/** Configuration mask - transfer parameters */
#define CCP_CFG_MASK_TRANSFER           (1 << 3)
/** Configuration mask - line count parameters */
#define CCP_CFG_MASK_LINE_COUNT         (1 << 4)
/** Configuration mask - address parameters */
#define CCP_CFG_MASK_ADDRESS            (1 << 5)

/** Configuration mask - all parameters */
#define CCP_CFG_MASK_ALL                ((1<<6)-1)

/*--------data declarations -----------------------------------*/
    typedef struct {

    /** Next CCP transfer general parameters */
        MSP_drvCcp2txGeneralT generalParams;
    /** Next CCP transfer ISR parameters */
        MSP_drvCcp2txIsrT isrParams;
    /** Next CCP transfer YUV to RGB matrix parameters */
        MSP_drvCcp2txMatrixT rgbMatrixParams;
    /** Next CCP transfer specific parameters */
        MSP_drvCcp2txTransferT transferParams;

    /** Next CCP transfer configuration slot */
        MSP_U32 configSlot;
    /** Next CCP transfer configuration mask */
        MSP_U8 configMask;
    } drvCcp2txTransferElemT;

    typedef struct {
        MSP_PTR elemData;
    /** Is this queue element used (valid only if can free non first element)*/
        MSP_U8 isUsed;
    } drvCcp2txQueueElemT;

    // transferQueue[DEV_CCPTX_QUEUES_LENGTH];

    typedef struct {
    /** Pointer to queue root */
        drvCcp2txQueueElemT *queueRoot;
    /** First element index */
        MSP_U32 firstElem;
    /** Last element index */
        MSP_U32 lastElem;
    /** Total queue length */
        MSP_U32 totalElems;
    /** Valid elements in queue */
        MSP_U32 validElems;
    /** Can be non-first element freed */
        MSP_U32 canFreeNonFirst;
    } drvCcp2txQueueControlT;

    typedef struct {
    /** Queue containing configuration slots */
        drvCcp2txQueueControlT configSlotQueue;
    /** Queue containing currently configured transfers */
        drvCcp2txQueueControlT transferQueue;
    /** Queue containing prepared transfers submitted by client for sending */
        drvCcp2txQueueControlT readyQueue;
    /** Low level CCP tx driver handle */
        drvCcp2txDevT *ccp2txDevHandle;
    /** Configuration slot for current transfer */
        MSP_U32 currentActiveSlot;
    /** Flag indicating that a transfer is running */
        MSP_U8 startedFlag;
    } ccp2txInstanceT;

/*--------function prototypes ---------------------------------*/

    /* ========================================================================== 
     */
/**
* ccp2txMgrQueueInit() - Initializes queue
*
* @param ctrl - drvCcp2txQueueControlT* - Pointer to queue control structure
*
* @param size - MSP_U32 - Queue length
*
* @param canFreeNonFirst - MSP_U32 - Flag indicating that a non first element
*                                       can be freed
*
* @return   CCP_MGR_INVALID_VALUE       - Error
*           Number                      - Size of queue
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
    MSP_U32 ccp2txMgrQueueInit(drvCcp2txQueueControlT * ctrl,
                               MSP_U32 qLength,
                               MSP_U32 elemSize, MSP_U32 canFreeNonFirst);

    /* ========================================================================== 
     */
/**
* ccp2txMgrQueueDeinit() - Deinitializes queue
*
* @param ctrl - drvCcp2txQueueControlT* - Pointer to queue control structure
*
* @return   CCP_MGR_INVALID_VALUE       - Error
*           Number                      - Size of queue
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
    MSP_U32 ccp2txMgrQueueDeinit(drvCcp2txQueueControlT * ctrl);

    /* ========================================================================== 
     */
/**
* ccp2txMgrQueueGetNode() - Gets a free node from queue
*
* @param ctrl - drvCcp2txQueueControlT* - Pointer to queue control structure
*
* @return   CCP_MGR_INVALID_VALUE       - Error
*           Number                      - Node index
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
    MSP_U32 ccp2txMgrQueueGetNode(drvCcp2txQueueControlT * ctrl);

    /* ========================================================================== 
     */
/**
* ccp2txMgrQueueFreeNode() - Frees a node from queue
*
* @param ctrl - drvCcp2txQueueControlT* - Pointer to queue control structure
*
* @return   CCP_MGR_INVALID_VALUE       - Error
*           Number                      - Node index
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
    MSP_U32 ccp2txMgrQueueFreeNode(drvCcp2txQueueControlT * ctrl, MSP_U32 node);

    /* ========================================================================== 
     */
/**
* ccp2txMgrInit() - Initializes CCP Tx driver manager
*
* @param pInstance - ccp2txInstanceT* - Pointer to driver instance
*
* @return - None
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
    void ccp2txMgrInit(ccp2txInstanceT * pInstance);

    /* ========================================================================== 
     */
/**
* ccp2txMgrDeinit() - Deinitializes CCP Tx driver manager
*
* @param pInstance - ccp2txInstanceT* - Pointer to driver instance
*
* @return - None
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
    void ccp2txMgrDeinit(ccp2txInstanceT * pInstance);

    /* ========================================================================== 
     */
/**
* ccp2txMgrGetConfigSlot() - Gets Free configuration slot
*
* @param hMSP - MSP_HANDLE - Pointer to MSP handle
*
* @return   CCP_MGR_INVALID_VALUE       - Error
*           Otherwise   - configuration slot number
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
    MSP_U32 ccp2txMgrGetConfigSlot(MSP_HANDLE hMSP);

    /* ========================================================================== 
     */
/**
* ccp2txMgrGetTransferID() - Gets Free transfer ID
*
* @param hMSP - MSP_HANDLE - Pointer to MSP handle
*
* @param configSlot - MSP_U32 - Configuration slot
*
* @return   CCP_MGR_INVALID_VALUE       - Error
*           Otherwise   - Transfer ID
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
    MSP_U32 ccp2txMgrGetTransferID(MSP_HANDLE hMSP, MSP_U32 configSlot);

    /* ========================================================================== 
     */
/**
* ccp2txMgrSetConfiguration() - Sets configuration
*
* @param hMSP - MSP_HANDLE - Pointer to MSP handle
*
* @param cfg -  MSP_drvCcp2txConfigT* - Pointer to configuration structure
*
* @param config - MSP_CCP_INDEXTYPE - Configuration type
*
* @return   MSP_ERROR_NONE          - Successful operation
*           MSP_ERROR_INVALIDCONFIG - Wrong configuration
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
    MSP_ERROR_TYPE ccp2txMgrSetConfiguration(MSP_HANDLE hMSP,
                                             MSP_drvCcp2txConfigT * cfg,
                                             MSP_CCP_INDEXTYPE config);

    /* ========================================================================== 
     */
/**
* ccp2txMgrGetConfiguration() - Gets configuration
*
* @param hMSP - MSP_HANDLE - Pointer to MSP handle
*
* @param cfg -  MSP_drvCcp2txConfigT* - Pointer to configuration structure
*
* @param config - MSP_CCP_INDEXTYPE - Configuration type
*
* @return   MSP_ERROR_NONE          - Successful operation
*           MSP_ERROR_INVALIDCONFIG - Wrong configuration
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
    MSP_ERROR_TYPE ccp2txMgrGetConfiguration(MSP_HANDLE hMSP,
                                             MSP_drvCcp2txConfigT * cfg,
                                             MSP_CCP_INDEXTYPE config);

    /* ========================================================================== 
     */
/**
* ccp2txMgrCopyConfig() - Copies configuration between slots
*
* @param destConfig - drvCcp2txTransferElemT* - Pointer to destination data
*
* @param sourceConfig -  drvCcp2txTransferElemT* - Pointer to source data
*
* @param maskFromSource - MSP_U8 - Flag indicating if source or destination mask is used
*
* @return   None
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
    void ccp2txMgrCopyConfig(drvCcp2txTransferElemT * destConfig,
                             drvCcp2txTransferElemT * sourceConfig,
                             MSP_U8 maskFromSource);

    /* ========================================================================== 
     */
/**
* ccp2txMgrApplyConfig() - Copies configuration between slots
*
* @param destConfig - drvCcp2txTransferElemT* - Pointer to destination data
*
* @param sourceConfig -  drvCcp2txTransferElemT* - Pointer to source data
*
* @param maskFromSource - MSP_U8 - Flag indicating if source or destination mask is used
*
* @return   None
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
    void ccp2txMgrApplyConfig(ccp2txInstanceT * ccp2txInstance,
                              drvCcp2txTransferElemT * cfg, MSP_U8 full);

    /* ========================================================================== 
     */
/**
* ccp2txMgrTransferEndCB() - Interrupt Service Routine
*
* @return   None
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
    void ccp2txMgrTransferEndCB(void);

    /* ========================================================================== 
     */
/**
* ccp2txMgrProcessNextElem() - Starts transfer with next configuration ID
*
* @param hMSP - MSP_HANDLE - Pointer to MSP handle
*
* @return   CCP_MGR_INVALID_VALUE       - Error
*           Otherwise   - configuration slot number
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
    MSP_U32 ccp2txMgrProcessNextElem(MSP_HANDLE hMSP);

    /* ========================================================================== 
     */
/**
* ccp2txMgrStartTransfer() - Initiate the transfer
*
* @param hMSP - MSP_HANDLE - Pointer to MSP handle
*
* @param transferID - MSP_U32 - Transfer ID
*
* @return   CCP_MGR_INVALID_VALUE       - Error
*           Otherwise   - configuration slot number
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
    MSP_ERROR_TYPE ccp2txMgrStartTransfer(MSP_HANDLE hMSP, MSP_U32 transferID);

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _CCP_MGR_H */

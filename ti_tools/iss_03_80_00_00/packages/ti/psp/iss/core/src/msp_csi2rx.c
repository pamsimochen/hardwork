/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file ccp2tx_api.c
*
* This file contains routines for CCP transmitter driver MSP Interface
*
* @path iss/drivers/drv_ccp2tx/src
*
* @rev 01.00
*/
/* -------------------------------------------------------------------------- 
 */
/* --------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *!
 * 13-Feb-2009 Petar Sivenov *! *
 * =========================================================================== 
 */

/* User code goes here */
/* ------compilation control switches --------------------------------------- 
 */
/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
/* ----- system and platform files ---------------------------- */
#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>
#include <stdlib.h>
/*-------program files ----------------------------------------*/
#include <ti/psp/iss/core/ccp2tx_msp.h>
#include <ti/psp/iss/core/msp_types.h>
// #include <msp/msp.h>
#include <ti/psp/iss/core/inc/ccp2tx_api.h>
#include <ti/psp/iss/core/inc/ccp2tx_private.h>
#include <ti/psp/iss/core/inc/ccp2tx_mgr.h>

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
/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/

/* ========================================================================== 
 */
/**
* MSP_CCPTxOpen() - Opens MSP CCPTx Driver Component for use
*
* @param hMSP - MSP_COMPONENT_TYPE* - Pointer to CCP device handle
*
* @param tProfile - MSP_PROFILE_TYPE* - Pointer to CCP revision struct
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
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
/* ===================================================================
 *  @func     MSP_CCPTxOpen                                               
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
MSP_ERROR_TYPE MSP_CCPTxOpen(MSP_HANDLE hMSP, MSP_PTR pCreateParam)
{
    return (MSP_ERROR_NONE);
}

/* ========================================================================== 
 */
/**
* MSP_CCPTxControl() - Controls MSP CCPTx Driver Component
*
* @param hMSP - MSP_COMPONENT_TYPE* - Pointer to CCP device handle
*
* @param tProfile - MSP_PROFILE_TYPE* - Pointer to CCP revision struct
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
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
/* ===================================================================
 *  @func     MSP_CCPTxControl                                               
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
MSP_ERROR_TYPE MSP_CCPTxControl(MSP_HANDLE hMSP,
                                MSP_CTRLCMD_TYPE tCmd, MSP_PTR pCmdParam)
{
    MSP_U32 result = MSP_ERROR_NONE;

    // MSP_COMPONENT_TYPE *pComponent = (MSP_COMPONENT_TYPE *)hMSP;

    if ((hMSP == NULL) || (pCmdParam == NULL))
    {
        result = MSP_ERROR_NULLPTR;
    }
    else
    {
        switch ((MSP_CTRLCMD_TYPE) tCmd)
        {
            case MSP_CTRLCMD_START:
                result = ccp2txMgrStartTransfer(hMSP, *((MSP_U32 *) pCmdParam));
                break;
            case MSP_CTRLCMD_STOP:
            case MSP_CTRLCMD_PAUSE:
            case MSP_CTRLCMD_RESUME:
            case MSP_CTRLCMD_FLUSH:
            case MSP_CTRLCMD_EXTENDED:
            default:
                result = MSP_ERROR_INVALIDCMD;
                break;
        }
    }

    return (result);
}

/* ========================================================================== 
 */
/**
* MSP_CCPTxConfig() - Configures MSP CCPTx Driver Component
*
* @param hMSP - MSP_COMPONENT_TYPE* - Pointer to CCP device handle
*
* @param tProfile - MSP_PROFILE_TYPE* - Pointer to CCP revision struct
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
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
/* ===================================================================
 *  @func     MSP_CCPTxConfig                                               
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
MSP_ERROR_TYPE MSP_CCPTxConfig(MSP_HANDLE hMSP, MSP_INDEXTYPE tConfigIndex,
                               MSP_PTR pConfigParam)
{
    MSP_U32 result = MSP_ERROR_NONE;

    MSP_drvCcp2txConfigT *cfg = (MSP_drvCcp2txConfigT *) pConfigParam;

    if ((hMSP == NULL) || (pConfigParam == NULL))
    {
        result = MSP_ERROR_NULLPTR;
    }
    else
    {
        result =
            ccp2txMgrSetConfiguration(hMSP, cfg,
                                      (MSP_CCP_INDEXTYPE) tConfigIndex);
    }

    return (result);
}

/* ========================================================================== 
 */
/**
* MSP_CCPTxQuery() - Queries MSP CCPTx Driver Component
*
* @param hMSP - MSP_COMPONENT_TYPE* - Pointer to CCP device handle
*
* @param tProfile - MSP_PROFILE_TYPE* - Pointer to CCP revision struct
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
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
/* ===================================================================
 *  @func     MSP_CCPTxQuery                                               
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
MSP_ERROR_TYPE MSP_CCPTxQuery(MSP_HANDLE hMSP, MSP_INDEXTYPE tQueryIndex,
                              MSP_PTR pQueryParam)
{

    MSP_U32 result = MSP_ERROR_NONE;

    MSP_drvCcp2txConfigT *cfg = (MSP_drvCcp2txConfigT *) pQueryParam;

    MSP_U32 retNumber;

    if ((hMSP == NULL) || (pQueryParam == NULL))
    {
        result = MSP_ERROR_NULLPTR;
    }
    else
    {
        switch ((MSP_CCP_INDEXTYPE) tQueryIndex)
        {
                // Requests user ID
            case MSP_CCP_INDEXTYPE_GET_CSID:
                retNumber = ccp2txMgrGetConfigSlot(hMSP);
                if (CCP_MGR_INVALID_VALUE != retNumber)
                {
                    *((MSP_U32 *) pQueryParam) = retNumber;
                }
                else
                {
                    result = MSP_ERROR_FAIL;
                }
                break;

                // Requests transfer ID
            case MSP_CCP_INDEXTYPE_GET_TID:
                retNumber =
                    ccp2txMgrGetTransferID(hMSP, *((MSP_U32 *) pQueryParam));
                if (CCP_MGR_INVALID_VALUE != retNumber)
                {
                    *((MSP_U32 *) pQueryParam) = retNumber;
                }
                else
                {
                    result = MSP_ERROR_FAIL;
                }
                break;

                // Requests configuration
            default:
                result =
                    ccp2txMgrGetConfiguration(hMSP, cfg,
                                              (MSP_CCP_INDEXTYPE) tQueryIndex);
                break;
        }
    }

    return (result);
}

/* ========================================================================== 
 */
/**
* MSP_CCPTxDeInit() - Deinitializes MSP CCPTx Driver Component
*
* @param hMSP - MSP_COMPONENT_TYPE* - Pointer to CCP device handle
*
* @param tProfile - MSP_PROFILE_TYPE* - Pointer to CCP revision struct
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
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
/* ===================================================================
 *  @func     MSP_CCPTxDeInit                                               
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
MSP_ERROR_TYPE MSP_CCPTxDeInit(MSP_HANDLE hMSP)
{
    MSP_U32 result = MSP_ERROR_NONE;

    MSP_COMPONENT_TYPE *pComponent;

    ccp2txInstanceT *pInstance;

    pComponent = (MSP_COMPONENT_TYPE *) hMSP;
    pInstance = (ccp2txInstanceT *) pComponent->pCompPrivate;

    // Deinitialize CCP TX manager
    ccp2txMgrDeinit(pInstance);

    // Deallocate driver instance data
    free(pComponent->pCompPrivate);

    result = MSP_ERROR_NONE;

    return (result);
}

/* ========================================================================== 
 */
/**
* MSP_CCPTxProcess() - Initializes MSP CCPTx Driver Component
*
* @param hMSP - MSP_COMPONENT_TYPE* - Pointer to CCP device handle
*
* @param tProfile - MSP_PROFILE_TYPE* - Pointer to CCP revision struct
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
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
/* ===================================================================
 *  @func     MSP_CCPTxProcess                                               
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
MSP_ERROR_TYPE MSP_CCPTxProcess(MSP_HANDLE hMSP,
                                MSP_PTR pArg, MSP_BUFHEADER_TYPE * ptBufHdr)
{
    MSP_U32 result = MSP_ERROR_NONE;

    return (result);
}

/* ========================================================================== 
 */
/**
* MSP_CCPTxClose() - Initializes MSP CCPTx Driver Component
*
* @param hMSP - MSP_COMPONENT_TYPE* - Pointer to CCP device handle
*
* @param tProfile - MSP_PROFILE_TYPE* - Pointer to CCP revision struct
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
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
/* ===================================================================
 *  @func     MSP_CCPTxClose                                               
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
MSP_ERROR_TYPE MSP_CCPTxClose(MSP_HANDLE hMSP)
{
    MSP_ERROR_TYPE result;

    result = MSP_ERROR_NONE;
    return (result);
}

/* ========================================================================== 
 */
/**
* MSP_CCPTxInit() - Initializes MSP CCPTx Driver Component
*
* @param hMSP - MSP_COMPONENT_TYPE* - Pointer to CCP device handle
*
* @param tProfile - MSP_PROFILE_TYPE* - Pointer to CCP revision struct
*
* @return   MSP_ERROR_NONE      - Initialization successful
*           MSP_ERROR_FAIL      - Initialization failed
*
* @post There is no post conditions.
*
* @see
*
*/
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     MSP_CCPTxInit                                               
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
MSP_ERROR_TYPE MSP_CCPTxInit(MSP_COMPONENT_TYPE * hMSP,
                             MSP_PROFILE_TYPE tProfile)
{
    MSP_ERROR_TYPE result;

    MSP_COMPONENT_TYPE *pComponent;

    ccp2txInstanceT *pInstance;

    result = MSP_ERROR_NONE;

    pComponent = hMSP;

    // Allocate driver instance data
    pComponent->pCompPrivate =
        (ccp2txInstanceT *) malloc(sizeof(ccp2txInstanceT));
    if (pComponent->pCompPrivate == NULL)
    {
        return MSP_ERROR_FAIL;
    }

    pInstance = (ccp2txInstanceT *) pComponent->pCompPrivate;

    // Initialize CCP TX manager
    ccp2txMgrInit(pInstance);

    pComponent->open = MSP_CCPTxOpen;
    pComponent->config = MSP_CCPTxConfig;
    pComponent->control = MSP_CCPTxControl;
    pComponent->query = MSP_CCPTxQuery;
    pComponent->process = MSP_CCPTxProcess;
    pComponent->close = MSP_CCPTxClose;
    pComponent->deInit = MSP_CCPTxDeInit;

    // Initialize CCP TX Low level
    drvCcp2txInit(pInstance->ccp2txDevHandle);

    return (result);

}

/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file vp_api.c
*
* This file contains routines for Parallel video interface MSP Interface
*
* @path iss/drivers/drv_vp/src
*
* @rev 01.00
*/
/* -------------------------------------------------------------------------- 
 */
/* --------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *!
 * 05-Apr-2011 LVasilev - Created *! *
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

/*-------program files ----------------------------------------*/
#include <drv_vp/vp_msp.h>
#include <msp/msp_utils.h>
#include <msp/msp.h>

#include <drv_vp/inc/vp_api.h>
#include <drv_vp/inc/vp_private.h>

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
* MSP_VPInterfaceOpen() - Opens MSP CCPTx Driver Component for use
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
 *  @func     MSP_VPInterfaceOpen                                               
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
MSP_ERROR_TYPE MSP_VPInterfaceOpen(MSP_HANDLE hMSP, MSP_PTR pCreateParam)
{
    return vp_init();
}

/* ========================================================================== 
 */
/**
* MSP_VPInterfaceControl() - Controls MSP CCPTx Driver Component
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
 *  @func     MSP_VPInterfaceControl                                               
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
MSP_ERROR_TYPE MSP_VPInterfaceControl(MSP_HANDLE hMSP,
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
        // switch((MSP_VP_INDEXTYPE)tCmd) {
        // }
    }

    return (result);
}

/* ========================================================================== 
 */
/**
* MSP_VPInterfaceConfig() - Configures MSP CCPTx Driver Component
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
 *  @func     MSP_VPInterfaceConfig                                               
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
MSP_ERROR_TYPE MSP_VPInterfaceConfig(MSP_HANDLE hMSP,
                                     MSP_INDEXTYPE tConfigIndex,
                                     MSP_PTR pConfigParam)
{
    MSP_U32 result = MSP_ERROR_NONE;

    if ((hMSP == NULL) || (pConfigParam == NULL))
    {
        result = MSP_ERROR_NULLPTR;
    }
    else
    {

    }

    return (result);
}

/* ========================================================================== 
 */
/**
* MSP_VPInterfaceQuery() - Queries MSP CCPTx Driver Component
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
 *  @func     MSP_VPInterfaceQuery                                               
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
MSP_ERROR_TYPE MSP_VPInterfaceQuery(MSP_HANDLE hMSP, MSP_INDEXTYPE tQueryIndex,
                                    MSP_PTR pQueryParam)
{

    MSP_U32 result = MSP_ERROR_NONE;

    MSP_U32 retNumber;

    if ((hMSP == NULL) || (pQueryParam == NULL))
    {
        result = MSP_ERROR_NULLPTR;
    }
    else
    {
        // switch((MSP_VP_INDEXTYPE)tQueryIndex) {
        // }
    }

    return (result);
}

/* ========================================================================== 
 */
/**
* MSP_VPInterfaceDeInit() - Deinitializes MSP CCPTx Driver Component
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
 *  @func     MSP_VPInterfaceDeInit                                               
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
MSP_ERROR_TYPE MSP_VPInterfaceDeInit(MSP_HANDLE hMSP)
{
    MSP_U32 result = MSP_ERROR_NONE;

    MSP_COMPONENT_TYPE *pComponent;

    pComponent = (MSP_COMPONENT_TYPE *) hMSP;

    // Deallocate driver instance data
    MSP_MEMFREE(pComponent->pCompPrivate);

    result = MSP_ERROR_NONE;

    return (result);
}

/* ========================================================================== 
 */
/**
* MSP_VPInterfaceProcess() - Initializes MSP CCPTx Driver Component
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
 *  @func     MSP_VPInterfaceProcess                                               
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
MSP_ERROR_TYPE MSP_VPInterfaceProcess(MSP_HANDLE hMSP,
                                      MSP_PTR pArg,
                                      MSP_BUFHEADER_TYPE * ptBufHdr)
{
    MSP_ERROR_TYPE result = MSP_ERROR_NONE;

    return (result);
}

/* ========================================================================== 
 */
/**
* MSP_VPInterfaceClose() - Initializes MSP CCPTx Driver Component
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
 *  @func     MSP_VPInterfaceClose                                               
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
MSP_ERROR_TYPE MSP_VPInterfaceClose(MSP_HANDLE hMSP)
{
    MSP_ERROR_TYPE result = MSP_ERROR_NONE;

    return (result);
}

/* ========================================================================== 
 */
/**
* MSP_VPInterfaceInit() - Initializes MSP CCPTx Driver Component
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
 *  @func     MSP_VPInterfaceInit                                               
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
MSP_ERROR_TYPE MSP_VPInterfaceInit(MSP_COMPONENT_TYPE * hMSP,
                                   MSP_PROFILE_TYPE tProfile)
{
    MSP_ERROR_TYPE result;

    MSP_COMPONENT_TYPE *pComponent;

    result = MSP_ERROR_NONE;

    pComponent = hMSP;

    // Allocate driver instance data
    // pComponent->pCompPrivate =
    // (ccp2txInstanceT*)MSP_MEMALLOC(sizeof(ccp2txInstanceT));

    // pInstance = (ccp2txInstanceT*)pComponent->pCompPrivate;

    // Initialize CCP TX manager
    // ccp2txMgrInit(pInstance);

    pComponent->open = MSP_VPInterfaceOpen;
    pComponent->config = MSP_VPInterfaceConfig;
    pComponent->control = MSP_VPInterfaceControl;
    pComponent->query = MSP_VPInterfaceQuery;
    pComponent->process = MSP_VPInterfaceProcess;
    pComponent->close = MSP_VPInterfaceClose;
    pComponent->deInit = MSP_VPInterfaceDeInit;

    return (result);

}

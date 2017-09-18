/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file bte_drv.h
*
* Bte driver public header file containing function declarations.
*
* @path WTSD_DucatiMMSW/drivers/drv_bte/inc
*
* @rev 00.01
*/
/* -------------------------------------------------------------------------- 
 */

#ifndef _BTE_DRV_H
#define _BTE_DRV_H

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
#include "../inc/bte_def.h"

/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------function prototypes ---------------------------------*/

    /* ========================================================================== 
     */
/**
 *  bteContextManagementInit()
 *
 * @brief  Initialize the BTE virtual contexts manager.
 *
 * @param bteCtxMngr - bteCtxMngrT* - [in] pointer to the context manager
 * handle for the BTE driver instance.
 *
 * @param bteBaseAddrOfst - MSP_Bte_BaseAddressT - [in] Virtual base address param.
 *
 * @return MSP_ERROR_TYPE
 *
 * @pre Driver MSP handle is initialized and opened.
 *
 * @post There is no post conditions.
 *
 * @see MSP_ERROR_TYPE
 */
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE bteContextManagementInit(bteCtxMngrT * bteCtxMngr,
                                            MSP_Bte_BaseAddressT
                                            bteBaseAddrOfst);

    /* ========================================================================== 
     */
/**
 *  bteContextManagementDeinit()
 *
 * @brief  Deinitialize the BTE virtual contexts manager.
 *
 * @param bteCtxMngr - bteCtxMngrT* - [in] pointer to the context manager
 * handle for the BTE driver instance.
 *
 * @return MSP_ERROR_TYPE
 *
 * @pre Driver MSP handle is initialized and opened.
 *
 * @post There is no post conditions.
 *
 * @see MSP_ERROR_TYPE
 */
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE bteContextManagementDeinit(bteCtxMngrT * bteCtxMngr);

    /* ========================================================================== 
     */
/**
 *  bteContextDefine()
 *
 * @brief  Defines a BTE virtual context buffer.
 *
 * @param bteCtxMngr - bteCtxMngrT* - [in] pointer to the context manager
 * handle for the BTE driver instance.
 *
 * @param contxId - MSP_Bte_ContextIdT - [in] Id of the virtual context.
 *
 * @param contxCreateParams - MSP_Bte_ContextDefinitionT* - [in] Pointer to 
 * virtual context create params.
 *
 * @param contxSpacePtr - MSP_U32* - [out] Context space pointer returned through 
 * this pointer.
 *
 * @return MSP_ERROR_TYPE
 *
 * @pre Driver MSP handle is initialized and opened.
 *
 * @post There is no post conditions.
 *
 * @see MSP_ERROR_TYPE
 */
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE bteContextDefine(bteCtxMngrT * bteCtxMngr,
                                    MSP_Bte_ContextIdT contxId,
                                    MSP_Bte_ContextDefinitionT *
                                    contxCreateParams, MSP_U32 * contxSpacePtr);

    /* ========================================================================== 
     */
/**
 *  bteContextRelease()
 *
 * @brief  Releases a BTE virtual context buffer.
 *
 * @param bteCtxMngr - bteCtxMngrT* - [in] pointer to the context manager
 * handle for the BTE driver instance.
 *
 * @param contxId - MSP_Bte_ContextIdT - [in] Id of the virtual context.
 *
 * @return MSP_ERROR_TYPE
 *
 * @pre Driver MSP handle is initialized and opened.
 *
 * @post There is no post conditions.
 *
 * @see MSP_ERROR_TYPE
 */
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE bteContextRelease(bteCtxMngrT * bteCtxMngr,
                                     MSP_Bte_ContextIdT contxId);

    /* ========================================================================== 
     */
/**
 *  bteContextReleaseByPtr()
 *
 * @brief  Releases a BTE virtual context buffer.
 *
 * @param bteCtxMngr - bteCtxMngrT* - [in] pointer to the context manager
 * handle for the BTE driver instance.
 *
 * @param contxPtr - MSP_U32 - [in] Buffer pointer of the virtual context.
 *
 * @return MSP_ERROR_TYPE
 *
 * @pre Driver MSP handle is initialized and opened.
 *
 * @post There is no post conditions.
 *
 * @see MSP_ERROR_TYPE
 */
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE bteContextReleaseByPtr(bteCtxMngrT * bteCtxMngr,
                                          MSP_U32 contxPtr);

    /* ========================================================================== 
     */
/**
 *  bteContextControlSetByPtr()
 *
 * @brief  Control settings for a BTE virtual context buffer.
 *
 * @param bteCtxMngr - bteCtxMngrT* - [in] pointer to the context manager
 * handle for the BTE driver instance.
 *
 * @param contxPtr - MSP_U32 - [in] Buffer pointer of the virtual context.
 *
 * @param ctxCtrlPrms - MSP_Bte_ContextControlT - [in] Control parameters.
 *
 * @return MSP_ERROR_TYPE
 *
 * @pre Driver MSP handle is initialized and opened.
 *
 * @post There is no post conditions.
 *
 * @see MSP_ERROR_TYPE
 */
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE bteContextControlSetByPtr(bteCtxMngrT * bteCtxMngr,
                                             MSP_U32 contxPtr,
                                             MSP_Bte_ContextControlT
                                             ctxCtrlPrms);

    /* ========================================================================== 
     */
/**
 *  bteContextConfig()
 *
 * @brief  Configure a BTE virtual context.
 *
 * @param bteCtxMngr - bteCtxMngrT* - [in] pointer to the context manager
 * handle for the BTE driver instance.
 *
 * @param contxId - MSP_Bte_ContextIdT - [in] Id of the virtual context.
 *
 * @param contxConf - MSP_Bte_VirtContextT* - [in] Pointer to virtual context configuration.
 *
 * @return MSP_ERROR_TYPE
 *
 * @pre Driver MSP handle is initialized and opened.
 *
 * @post There is no post conditions.
 *
 * @see MSP_ERROR_TYPE
 */
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE bteContextConfig(bteCtxMngrT * bteCtxMngr,
                                    MSP_Bte_ContextIdT contxId,
                                    MSP_Bte_VirtContextT * contxConf);

    /* ========================================================================== 
     */
/**
 *  bteContextGetConfig()
 *
 * @brief  Gets a BTE virtual context configuration.
 *
 * @param bteCtxMngr - bteCtxMngrT* - [in] pointer to the context manager
 * handle for the BTE driver instance.
 *
 * @param contxId - MSP_Bte_ContextIdT - [in] Id of the virtual context.
 *
 * @param contxConf - MSP_Bte_VirtContextT* - [out] Pointer to virtual context 
 * configuration structure to return values to.
 *
 * @return MSP_ERROR_TYPE
 *
 * @pre Driver MSP handle is initialized and opened.
 *
 * @post There is no post conditions.
 *
 * @see MSP_ERROR_TYPE
 */
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE bteContextGetConfig(bteCtxMngrT * bteCtxMngr,
                                       MSP_Bte_ContextIdT contxId,
                                       MSP_Bte_VirtContextT * contxConf);

    /* ========================================================================== 
     */
/**
 *  bteContextControlSet()
 *
 * @brief  Set the control of a BTE virtual context.
 *
 * @param bteCtxMngr - bteCtxMngrT* - [in] pointer to the context manager
 * handle for the BTE driver instance.
 *
 * @param contxId - MSP_Bte_ContextIdT - [in] Id of the virtual context.
 *
 * @param ctxCtrlPrms - MSP_Bte_ContextControlT - [in] Context control to set.
 *
 * @return MSP_ERROR_TYPE
 *
 * @pre Driver MSP handle is initialized and opened.
 *
 * @post There is no post conditions.
 *
 * @see MSP_ERROR_TYPE
 */
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE bteContextControlSet(bteCtxMngrT * bteCtxMngr,
                                        MSP_Bte_ContextIdT contxId,
                                        MSP_Bte_ContextControlT ctxCtrlPrms);

    /* ========================================================================== 
     */
/**
 *  bteGetFreeContext()
 *
 * @brief  Set the control of a BTE virtual context.
 *
 * @param bteCtxMngr - bteCtxMngrT* - [in] pointer to the context manager
 * handle for the BTE driver instance.
 *
 * @param contxId - MSP_Bte_ContextIdT* - [out] Id of the virtual context returned.
 *
 * @return MSP_ERROR_TYPE
 *
 * @pre Driver MSP handle is initialized and opened.
 *
 * @post There is no post conditions.
 *
 * @see MSP_ERROR_TYPE
 */
    /* ========================================================================== 
     */
    MSP_ERROR_TYPE bteGetFreeContext(bteCtxMngrT * bteCtxMngr,
                                     MSP_Bte_ContextIdT * contxId);
#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _BTE_DRV_H */

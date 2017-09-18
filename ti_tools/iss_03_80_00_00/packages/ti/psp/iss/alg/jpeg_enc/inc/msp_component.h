/** ==================================================================
 *  @file   msp_component.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/jpeg_enc/inc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* ==================================================================== Texas 
 * Instruments OMAP(TM) Platform Software (c) Copyright Texas Instruments,
 * Incorporated. All Rights Reserved. Use of this software is controlled by
 * the terms and conditions found in the license agreement under which this
 * software has been supplied.
 * ==================================================================== */
/* -------------------------------------------------------------------------- 
 */
/* 
 * msp_component.h
 * This header file defines the component interface of MSP(Multimedia Service
 * Provider) to be used by specific MSP components, in addition to its public
 * interface. It is sufficient for MSP components to include this single file
 *
 * @path OMAPSW_SysDev\multimedia\services\msp_core\inc
 *
 * @rev 1.0
 */
/* -------------------------------------------------------------------------- 
 */
/* =========================================================================
 * ! ! Revision History ! =================================== ! !
 * 26-Nov-2007 Abhishek Ranka (asranka@ti.com): Initial version
 * ========================================================================= */
#ifndef _MSP_COMP_H
#define _MSP_COMP_H

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

    /* User code goes here */
    /* ------compilation control switches
     * ---------------------------------------- */
/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
    /* ----- system and platform files ---------------------------- */
/*-------program files ----------------------------------------*/
#include "msp.h"

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*******************************************************************************
* Strutures
*******************************************************************************/
    /* ======================================================================= 
     */
    /* MSP_COMPONENT_TYPE - MSP Component type, handle to this structure will 
     * be passed during component specific MSP init and for all subsequent
     * calls @param pCompPrivate : Placeholder for component specific object 
     * handle @param tAppCBParam : Application callback parameters to be
     * used during any asynchronous event notification to application.
     * @param nCompIndex : Index of this component in MSP Component list
     * @param (*open) : MSP component Open Implementation @param (*control)
     * : MSP component control Implementation @param (*process) : MSP
     * component process Implementation @param (*config) : MSP component
     * config Implementation @param (*query) : MSP component query
     * Implementation @param (*close) : MSP component close Implementation
     * @param (*deInit) : MSP component deInit Implementation */
    /* ======================================================================= 
     */
    /* typedef struct { MSP_PTR pCompPrivate;
     * 
     * MSP_APPCBPARAM_TYPE tAppCBParam;
     * 
     * MSP_S32 nCompIndex;
     * 
     * MSP_ERROR_TYPE (*open) (MSP_HANDLE hMSP, MSP_PTR pCreateParam);
     * 
     * MSP_ERROR_TYPE (*control) (MSP_HANDLE hMSP, MSP_CTRLCMD_TYPE tCmd,
     * MSP_PTR pCmdParam);
     * 
     * MSP_ERROR_TYPE (*process) (MSP_HANDLE hMSP, MSP_PTR pArg,
     * MSP_BUFHEADER_TYPE *ptBufHdr);
     * 
     * MSP_ERROR_TYPE (*config) (MSP_HANDLE hMSP, MSP_INDEXTYPE tConfigIndex,
     * MSP_PTR pConfigParam);
     * 
     * MSP_ERROR_TYPE (*query) (MSP_HANDLE hMSP, MSP_INDEXTYPE tQueryIndex,
     * MSP_PTR pQueryParam);
     * 
     * MSP_ERROR_TYPE (*close) (MSP_HANDLE hMSP);
     * 
     * MSP_ERROR_TYPE (*deInit) (MSP_HANDLE hMSP);
     * 
     * MSP_STR sName; }MSP_COMPONENT_TYPE; */

/*--------function prototypes ---------------------------------*/
    /* =========================================================================== */
/**
 * @fn MSP_componentInit() Signature of component specific Init function expected,
 * this is be the first call into a component and is used to perform any one
 * time initialization specific to a component. The component MUST fill in all
 * above function pointers into the handle. The component may assume that the
 * the MSP handle has been allocated. The component should also allocate and
 * fill in the component private data structure. In the case of an error, the
 * component private data structure must be deallocated.
 *
 * @param hMSPComp     : MSP handle (Pointing to MSP_COMPONENT_TYPE)
 *
 * @param tProfile     : Profile for MSP component is initilized
 * 
 * @pre                 : none
 *
 * @post               : MSP component successfully initialized
 */
    /* =========================================================================== */
    MSP_ERROR_TYPE MSP_componentInit(MSP_COMPONENT_TYPE * hMSPComp,
                                     MSP_PROFILE_TYPE tProfile);
/*--------macros ----------------------------------------------*/
/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------data declarations -----------------------------------*/
    /* ======================================================================= 
     */
    /* MSP_COMPLISTTABLE_TYPE - Table listing the available MSP components in 
     * the system
     *
     * @param sCompName        : A NULL terminated string with max 128 characters
     *
     * @param pCompInit        : Component's Initialization function
     */
    /* ======================================================================= 
     */
    typedef struct {
        MSP_STR sCompName;

         MSP_ERROR_TYPE(*pCompInit) (MSP_COMPONENT_TYPE * hMSPComp,
                                     MSP_PROFILE_TYPE tProfile);
    } MSP_COMPLISTTABLE_TYPE;

/*--------function prototypes ---------------------------------*/
/*--------macros ----------------------------------------------*/
#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* !_MSP_COMP_H */

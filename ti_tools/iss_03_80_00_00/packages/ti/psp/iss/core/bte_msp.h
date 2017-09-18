/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file bte_msp.h
*
* MSP itnerface header file.
*
* @path WTSD_DucatiMMSW/drivers/drv_bte/drv_bte
*
* @rev 00.01
*/
/* -------------------------------------------------------------------------- 
 */

#ifndef _BTE_MSP_H
#define _BTE_MSP_H

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
#include "inc/bte_drv.h"

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

/** @enum MSP_INDEXTYPE_BTE
* Defining all of the configurations supported by the dmm msp driver. */
    typedef enum {
        BTE_CONFIG_CONTEXT
    } MSP_INDEXTYPE_BTE;

/** @enum MSP_CTRLCMD_TYPE_BTE
* Defining all of the control commands supported by the dmm msp driver. */
    typedef enum {
        BTE_CTRLCMD_CNTX_ALLOC,
        BTE_CTRLCMD_CNTX_FREE,
        BTE_CTRLCMD_CNTX_FREE_BY_PTR,
        BTE_CTRLCMD_CNTX_CTRL,
        BTE_CTRLCMD_CNTX_CTRL_BY_PTR,
        BTE_CTRLCMD_GET_FREE_CNTX
    } MSP_CTRLCMD_TYPE_BTE;

/** @struc MSP_Bte_CmdParam
* Structure defining Bte driver MSP_CONTROL paramaters. */
    typedef struct {
        MSP_Bte_ContextIdT ctxId;
        MSP_Bte_ContextDefinitionT ctxCrtPrms;
        MSP_Bte_ContextControlT ctxCtrlPrms;
        MSP_U32 ctxSpacePtr;
    } MSP_Bte_CmdParam;

/** @struc MSP_Bte_CreateParam
* Structure defining Bte driver MSP_OPEN configuraion paramaters. */
    typedef struct {
        MSP_Bte_BaseAddressT bteBaseVirtAddr;
    } MSP_Bte_CreateParam;

/*--------function prototypes ---------------------------------*/

    MSP_ERROR_TYPE MSP_BTE_init(MSP_COMPONENT_TYPE * hMSP,
                                MSP_PROFILE_TYPE tProfile);

    MSP_ERROR_TYPE MSP_BTE_open(MSP_HANDLE hMSP, MSP_PTR pCreateParam);

    MSP_ERROR_TYPE MSP_BTE_close(MSP_HANDLE hMSP);

    MSP_ERROR_TYPE MSP_BTE_control(MSP_HANDLE hMSP, MSP_CTRLCMD_TYPE tCmd,
                                   MSP_PTR pCmdParam);

    MSP_ERROR_TYPE MSP_BTE_deInit(MSP_HANDLE hMSP);

    MSP_ERROR_TYPE MSP_BTE_query(MSP_HANDLE hMSP, MSP_INDEXTYPE tQueryIndex,
                                 MSP_PTR pQueryParam);

    MSP_ERROR_TYPE MSP_BTE_config(MSP_HANDLE hMSP, MSP_INDEXTYPE tConfigIndex,
                                  MSP_PTR pConfigParam);

    MSP_ERROR_TYPE MSP_BTE_process(MSP_HANDLE hMSP, MSP_PTR pArg,
                                   MSP_BUFHEADER_TYPE * ptBufHdr);

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _BTE_MSP_H */

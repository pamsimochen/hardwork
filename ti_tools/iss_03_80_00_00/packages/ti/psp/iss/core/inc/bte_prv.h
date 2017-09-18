/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file bte_prv.h
*
* Bte driver private header file.
*
* @path WTSD_DucatiMMSW/drivers/drv_bte/inc
*
* @rev 00.01
*/
/* -------------------------------------------------------------------------- 
 */

#ifndef _BTE_PRV_H
#define _BTE_PRV_H

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
#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/psp/iss/hal/iss/bte/bte_drv.h>
#include <ti/psp/iss/core/bte_msp.h>
/*-------program files ----------------------------------------*/
#include "../inc/bte_os_dep.h"
#include "../inc/bte_drv.h"

/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
#define BTE_MEMORY_SIZE        (22528)
#define BTE_CONTEXT_COUNT    (4)
#define BTE_ERR_COOR        ((MSP_U32)-1)
#define BTE_CTX_START_ALIGN    (511)                       // 512 - byte
                                                           // alignment
                                                           // requirement
    // #define BTE_CTX_END_ALIGN (15) // 16 - byte alignment requirement
#define BTE_CTX_END_ALIGN    (31)                          // 32 - byte
                                                           // alignment
                                                           // requirement per 
                                                           // S.B.

#define BTE_CTX_ENDX_COOR_SHIFT        (0)
#define BTE_CTX_ENDX_COOR_MASK        (0x0000FFF0)
#define BTE_CTX_GET_ENDX_COOR(x)    ((x & BTE_CTX_ENDX_COOR_MASK) >> BTE_CTX_ENDX_COOR_SHIFT)

#define BTE_CTX_ENDY_COOR_SHIFT        (16)
#define BTE_CTX_ENDY_COOR_MASK        (0x1FFF0000)
#define BTE_CTX_GET_ENDY_COOR(x)    ((x & BTE_CTX_ENDY_COOR_MASK) >> BTE_CTX_ENDY_COOR_SHIFT)

#define BTE_AUX_ALIGN(x,y)            ((x + y) & ~y)

/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

    /* ========================================================================== 
     */
/**
 *  bteContextManagementInit()
 *
 * @brief  Returns a pointer the static BTE virtual contexts manager.
 *
 * @return bteCtxMngrT* pointer to the static BTE virtual contexts manager
 *
 * @pre Driver MSP handle is initialized and opened.
 *
 * @post There is no post conditions.
 *
 * @see MSP_ERROR_TYPE
 */
    /* ========================================================================== 
     */
    bteCtxMngrT *bteGetStaticContextManager();

    /* ========================================================================== 
     */
/**
 *  bteContextApply()
 *
 * @brief  Applu BTE virtual context configuration.
 *
 * @param contxConf - MSP_Bte_VirtContextT* - [in] Pointer to virtual context configuratio.
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
    MSP_ERROR_TYPE bteContextApply(MSP_Bte_VirtContextT * contxConf);

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _BTE_PRV_H */

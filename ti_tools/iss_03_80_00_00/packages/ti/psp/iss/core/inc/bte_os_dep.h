/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file bte_os_dep.h
*
* Dmm driver os dependand definitions header file.
*
* @path WTSD_DucatiMMSW/drivers/drv_bte/inc
*
* @rev 00.01
*/
/* -------------------------------------------------------------------------- 
 */

#ifndef _BTE_OS_DEP_H
#define _BTE_OS_DEP_H

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
#include <xdc/runtime/Gate.h>
/*-------program files ----------------------------------------*/

/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

    /* ========================================================================== 
     */
/**
 *  bteEnterCriticalSection()
 *
 * @brief  Enter a critical code execution section.
 *
 * @return none
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
    /* ========================================================================== 
     */
    void bteEnterCriticalSection();

    /* ========================================================================== 
     */
/**
 *  bteLeaveCriticalSection()
 *
 * @brief  Exit a critical code execution section.
 *
 * @return none
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
    /* ========================================================================== 
     */
    void bteLeaveCriticalSection();

    /* ========================================================================== 
     */
/**
 *  BTE_ASSERT_BREAK
 *
 * @brief  Assert break.
 */
    /* ========================================================================== 
     */
#ifdef __BTE_DEBUG_BUILD__
    /* 
     * #define BTE_ASSERT_BREAK __asm("\t BKPT #1;"); */
#define BTE_ASSERT_BREAK    TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_DRIVERS, "\nBTE_ASSERT_BREAK\n"); \
                                while(1);
#else
#define BTE_ASSERT_BREAK
#endif

    /* ========================================================================== 
     */
/**
 *  BTE_ENTER_CRITICAL_SECTION
 *
 * @brief  Enter a critical code execution section.
 */
    /* ========================================================================== 
     */
#define BTE_ENTER_CRITICAL_SECTION bteEnterCriticalSection();

    /* ========================================================================== 
     */
/**
 *  BTE_EXIT_CRITICAL_SETCTION
 *
 * @brief  Exit a critical code execution section.
 */
    /* ========================================================================== 
     */
#define BTE_EXIT_CRITICAL_SETCTION bteLeaveCriticalSection();

/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _BTE_OS_DEP_H */

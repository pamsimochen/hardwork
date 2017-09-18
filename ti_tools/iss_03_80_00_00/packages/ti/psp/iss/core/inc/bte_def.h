/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file bte_def.h
*
* Bte driver public header file containing data declarations.
*
* @path WTSD_DucatiMMSW/drivers/drv_bte/inc
*
* @rev 00.01
*/
/* -------------------------------------------------------------------------- 
 */

#ifndef _BTE_DEF_H
#define _BTE_DEF_H

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

#include <ti/psp/iss/core/msp_types.h>

    /* User code goes here */
    /* ------compilation control switches
     * --------------------------------------- */
/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
    /* ----- system and platform files ---------------------------- */
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
/*--------data declarations -----------------------------------*/

#define MSP_BTE_VIRT_SPACE_SIZE (0x20000000)

#define BTE_CTX_PPLN (0x10000)

/** @enum MSP_Bte_ContextIdT
* Enumeration defining BTE virtual context's ids. */
    typedef enum {
        MSP_BTE_CNTXT_0,
        MSP_BTE_CNTXT_1,
        MSP_BTE_CNTXT_2,
        MSP_BTE_CNTXT_3,
        MSP_BTE_CNTXT_4,
        MSP_BTE_CNTXT_5,
        MSP_BTE_CNTXT_6,
        MSP_BTE_CNTXT_7,
        MSP_BTE_CNTXT_INVALID
    } MSP_Bte_ContextIdT;

/** @enum MSP_Bte_TilerGridAccessT
* Enumeration defining BTE grid for access made to TILER. */
    typedef enum {
        MSP_BTE_GRID_8BIT_NAT = 0,
        MSP_BTE_GRID_8BIT_ROT = 1,
        MSP_BTE_GRID_16BIT_NAT = 2,
        MSP_BTE_GRID_16BIT_ROT = 1,
        MSP_BTE_GRID_32BIT_NAT = 2,
        MSP_BTE_GRID_32BIT_ROT = 3
    } MSP_Bte_TilerGridAccessT;

/** @enum MSP_Bte_ContextAccessModeT
* Enumeration defining BTE context's read or write mode. */
    typedef enum {
        MSP_BTE_MODE_WRITE,
        MSP_BTE_MODE_READ
    } MSP_Bte_ContextAccessModeT;

/** @enum MSP_Bte_FeatOnOffT
* Enumeration defining BTE context's features on or off state. */
    typedef enum {
        MSP_BTE_OFF,
        MSP_BTE_ON
    } MSP_Bte_FeatEnableT;

/** @enum MSP_Bte_BaseAddressT
* Enumeration defining BTE base virtual address space. */
    typedef enum {
        MSP_BTE_ADDR_0x0,
        MSP_BTE_ADDR_0x2,
        MSP_BTE_ADDR_0x4,
        MSP_BTE_ADDR_0x6,
        MSP_BTE_ADDR_0x8,
        MSP_BTE_ADDR_0xA,
        MSP_BTE_ADDR_0xC,
        MSP_BTE_ADDR_0xE
    } MSP_Bte_BaseAddressT;

/** @struc MSP_Bte_ContextCtrlT
* Structure defining BTE virtual context control options. */
    typedef struct {
        MSP_U16 nTriggerThreshold;
        MSP_U8 nInitSx;
        MSP_U8 nInitSy;
        MSP_Bte_FeatEnableT eAddr32;
        MSP_Bte_FeatEnableT eAutoFlush;
        MSP_Bte_FeatEnableT eOneShot;
        MSP_Bte_TilerGridAccessT eGrid;
        MSP_Bte_ContextAccessModeT eMode;
        MSP_Bte_FeatEnableT eFlush;
        MSP_Bte_FeatEnableT eStop;
        MSP_Bte_FeatEnableT eStart;
    } MSP_Bte_ContextCtrlT;

/** @struc MSP_Bte_ContextDefinitionT
* Structure defining BTE virtual context create parameters. */
    typedef struct {
        MSP_U32 nWidth;
        MSP_U32 nHeight;
        MSP_Bte_ContextAccessModeT eMode;
        MSP_BOOL bAutoFlushing;
        MSP_BOOL bOneShot;
        MSP_BOOL bTiledBuffer;
        MSP_Bte_TilerGridAccessT eGrid;
        MSP_U32 pCntxDestPtr;
    } MSP_Bte_ContextDefinitionT;

/** @struc MSP_Bte_ContextControlT
* Structure defining BTE virtual context control options (short - used in MSP control calls). */
    typedef struct {
        MSP_BOOL bFlush;
        MSP_BOOL bStop;
        MSP_BOOL bStart;
    } MSP_Bte_ContextControlT;

/** @struc MSP_Bte_VirtContextT
* Structure defining a BTE virtual context set up. */
    typedef struct {
        MSP_Bte_ContextIdT eCntxtId;
        MSP_BOOL bCntxFree;
        MSP_BOOL bCntxConfigured;
        MSP_U32 nCntxAddrOffst;
        MSP_Bte_ContextCtrlT sCntxtCtrl;
        MSP_U32 nTilerBfrPtr;
        MSP_U32 pBteCtxPtr;
        MSP_U32 nCntxStart;
        MSP_U32 nCntxEnd;
    } MSP_Bte_VirtContextT;

/** @struc bteBufferCtrlT
* Structure deticated to BTE physical buffer control. */
    typedef struct {
        MSP_U32 nTotalSize;
        MSP_U32 nUsedSize;
        MSP_U32 nBteVirtAddress;
    } bteBufferCtrlT;

/** @struc bteVirtCtxListT
* Structure defining a linked list of BTE virutal context descriptors. */
    typedef struct bteVirtCtxListT__ bteVirtCtxListT;

    struct bteVirtCtxListT__ {
        bteVirtCtxListT *nextLI;
        bteVirtCtxListT *prevLI;
        MSP_Bte_VirtContextT *bteVirtCtx;
    };

/** @struc bteCtxMngrT
* Structure defining a BTE virtual context set up. */
    typedef struct {
        MSP_U32 bInstanceOpened;
        bteBufferCtrlT tBteBfrCtrl;
        MSP_U32 nBteVirtCntxCount;
        MSP_Bte_VirtContextT *pAvailableCntxs;
        bteVirtCtxListT *pSortedCntxs;
    } bteCtxMngrT;

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _BTE_DEF_H */

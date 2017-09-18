/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file _csl_simcop_dma.h
*
* This File contains declarations for _csl_simcop_dma.c
* This entire description will appear as one 
* paragraph in the generated documentation.
*
* @path  $(CSLPATH)\src\DMA\
*
* @rev  00.01
*/
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *!
 *! 19-Mar-2009 Phanish: Modified to remove the HWcontrol and GetHWstatus APIs 
 *!                 and their dependencies. And now, HWsetup calls directly the CSL_FINS
 *!
 *! 23-Sep-2008 Geetha Ravindran:  Created the file.  
 *! 
 *!
 *! 24-Dec-2000 mf: Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */
#ifndef __CSL_SIMCOP_DMA_H_
#define __CSL_SIMCOP_DMA_H_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
*  INCLUDE FILES                                                 
****************************************************************/
/*-------program files ----------------------------------------*/
#include <ti/psp/iss/hal/iss/simcop/common/csl.h>
#include <ti/psp/iss/hal/iss/simcop/common/simcop.h>
#include "../cslr_simcop_dma_1.h"
#include "../csl_simcop_dma.h"

/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/
#if 0
    /* SIMCOP DMA REGISTERS"S FIELD ENUMS */
    // SIMCOP_DMA_REVISION
    /* typedef CSL_SimcopRevisionType CSL_SimcopDmaRevisionType; */
    typedef enum {
        SCHEME,
        FUNC,
        R_RTL,
        X_MAJOR,
        CUSTOM,
        Y_MINOR
    } CSL_SimcopDmaRevisionType;

    // SIMCOP_DMA_CTRL 
    typedef enum {
        BW_LIMITER,
        TAG_CNT,
        POSTED_WRITES,
        MAX_BURST_SIZE
    } CSL_SimcopDmaCtrlType;

    // SIMCOP_DMA_CHAN(0-8)_CONTROL
    typedef enum {
        HWSTOP,
        HWSTART,
        LINKED,
        GRID,
        TILERMODE,
        DIR,
        STATUS,
        SWTRIGGER,
        DISABLE,
        ENABLE
    } CSL_SimcopDmaChanCtrlEnumType;

    // SIMCOP_DMA_CHAN(0-8)_BLOCK_SIZE
    typedef enum {
        YNUM,
        XNUM
    } CSL_SimcopDmaChanBlkSizeType;

    // SIMCOP_DMA_CHAN(0-8)_FRAME
    typedef enum {
        YCNT,
        XCNT
    } CSL_SimcopDmaChanFrameType;

    // SIMCOP_DMA_CHAN(0-8)_CURRENT_BLOCK
    typedef enum {
        BY,
        BX
    } CSL_SimcopDmaChanCurrBlkType;

    // SIMCOP_DMA_CHAN(0-8)_BLOCK_STEP
    typedef enum {
        YSTEP,
        XSTEP
    } CSL_SimcopDmaChanBlkStepType;
#endif
    // SIMCOP_DMA_IRQ(0-1)
    typedef enum {
        FRAME_DONE,
        BLOCK_DONE,
        OCP_ERROR
    } CSL_SimcopDmaIRQSetType;

    typedef enum {
        CHAN7_FRAME_DONE_IRQ,
        CHAN6_FRAME_DONE_IRQ,
        CHAN5_FRAME_DONE_IRQ,
        CHAN4_FRAME_DONE_IRQ,
        CHAN3_FRAME_DONE_IRQ,
        CHAN2_FRAME_DONE_IRQ,
        CHAN1_FRAME_DONE_IRQ,
        CHAN0_FRAME_DONE_IRQ,
        CHAN7_BLOCK_DONE_IRQ,
        CHAN6_BLOCK_DONE_IRQ,
        CHAN5_BLOCK_DONE_IRQ,
        CHAN4_BLOCK_DONE_IRQ,
        CHAN3_BLOCK_DONE_IRQ,
        CHAN2_BLOCK_DONE_IRQ,
        CHAN1_BLOCK_DONE_IRQ,
        CHAN0_BLOCK_DONE_IRQ,
        OCP_ERR
    } CSL_SimcopDmaIRQGetType;

/*--------function prototypes ---------------------------------*/
#if 0
    /* SIMCOP_DMA_REVISION */
    CSL_Status _CSL_simcopDmaGetRev(CSL_SimcopDmaHandle hndl,
                                    CSL_SimcopDmaRevisionType RevType,
                                    Uint16 * data);
    /* SIMCOP_DMA_HWINFO */
    CSL_Status _CSL_simcopDmaGetNumOfChannels(CSL_SimcopDmaHandle hndl,
                                              Uint16 * data);
    CSL_Status _CSL_simcopGetContext(CSL_SimcopDmaHandle hndl, Uint16 * data);
    /* SIMCOP_DMA_SYSCONFIG */
    CSL_Status _CSL_simcopDmaGetStandByMode(CSL_SimcopDmaHandle hndl,
                                            Uint16 * data);
    CSL_Status _CSL_simcopDmaSetStandByMode(CSL_SimcopDmaHandle hndl,
                                            Uint16 data);
    CSL_Status _CSL_simcopDmaGetSoftReset(CSL_SimcopDmaHandle hndl,
                                          Uint16 * data);
    CSL_Status _CSL_simcopDmaSetSoftReset(CSL_SimcopDmaHandle hndl,
                                          Uint16 data);
    /* SIMCOP_DMA_IRQ_EOI */
    CSL_Status _CSL_simcopDmaGet_IRQ_EOI_LineNumber(CSL_SimcopDmaHandle hndl,
                                                    Uint16 * data);
    CSL_Status _CSL_simcopDmaSet_IRQ_EOI_LineNumber(CSL_SimcopDmaHandle hndl,
                                                    Uint16 data);
    /* SIMCOP_DMA_CTRL */
    CSL_Status _CSL_simcopDmaCtrl_Get(CSL_SimcopDmaHandle hndl,
                                      CSL_SimcopDmaCtrlType event,
                                      Uint16 * data);
    CSL_Status _CSL_simcopDmaCtrl_Set(CSL_SimcopDmaHandle hndl,
                                      CSL_SimcopDmaCtrlType event, Uint16 data);

    /* SIMCOP_DMA_CHAN_CTRL(0-7) */
    CSL_Status _CSL_simcopDmaChanCtrl_Get(CSL_SimcopDmaHandle hndl,
                                          Uint8 instance,
                                          CSL_SimcopDmaChanCtrlEnumType event,
                                          Uint16 * data);
    CSL_Status _CSL_simcopDmaChanCtrl_Set(CSL_SimcopDmaHandle hndl,
                                          Uint8 instance,
                                          CSL_SimcopDmaChanCtrlEnumType event,
                                          Uint16 data);
    /* SIMCOP_DMA_CHAN_SMEM_ADDR(0-7) */
    CSL_Status _CSL_simcopDmaChanSmem_Addr_Get(CSL_SimcopDmaHandle hndl,
                                               Uint8 instance, Uint32 * data);
    CSL_Status _CSL_simcopDmaChanSmem_Addr_Set(CSL_SimcopDmaHandle hndl,
                                               Uint8 instance, Uint32 data);
    /* SIMCOP_DMA_CHAN_SMEM_OFST(0-7) */
    CSL_Status _CSL_simcopDmaChanSmem_Ofst_Get(CSL_SimcopDmaHandle hndl,
                                               Uint8 instance, Uint32 * data);
    CSL_Status _CSL_simcopDmaChanSmem_Ofst_Set(CSL_SimcopDmaHandle hndl,
                                               Uint8 instance, Uint32 data);
    /* SIMCOP_DMA_CHAN_BUF_OFST(0-7) */
    CSL_Status _CSL_simcopDmaChanBuf_Addr_Get(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint32 * data);
    CSL_Status _CSL_simcopDmaChanBuf_Addr_Set(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint32 data);
    /* SIMCOP_DMA_CHAN_BUF_ADDR(0-7) */
    CSL_Status _CSL_simcopDmaChanBuf_Ofst_Get(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint32 * data);
    CSL_Status _CSL_simcopDmaChanBuf_Ofst_Set(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint32 data);
    /* SIMCOP_DMA_CHAN_BLOCK_SIZE(0-7) */
    CSL_Status _CSL_simcopDmaChanBlkSize_Get(CSL_SimcopDmaHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopDmaChanBlkSizeType event,
                                             Uint16 * data);
    CSL_Status _CSL_simcopDmaChanBlkSize_Set(CSL_SimcopDmaHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopDmaChanBlkSizeType event,
                                             Uint16 data);
    /* SIMCOP_DMA_CHAN_FRAME(0-7) */
    CSL_Status _CSL_simcopDmaChanFrame_Get(CSL_SimcopDmaHandle hndl,
                                           Uint8 instance,
                                           CSL_SimcopDmaChanFrameType event,
                                           Uint16 * data);
    CSL_Status _CSL_simcopDmaChanFrame_Set(CSL_SimcopDmaHandle hndl,
                                           Uint8 instance,
                                           CSL_SimcopDmaChanFrameType event,
                                           Uint16 data);
    /* SIMCOP_DMA_CHAN_CURRENT_BLOCK(0-7) */
    CSL_Status _CSL_simcopDmaChanCurrBlk_Get(CSL_SimcopDmaHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopDmaChanCurrBlkType event,
                                             Uint16 * data);
    /* SIMCOP_DMA_CHAN_BLOCK_STEP(0-7) */
    CSL_Status _CSL_simcopDmaChanBlkStep_Get(CSL_SimcopDmaHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopDmaChanBlkStepType event,
                                             Uint16 * data);
    CSL_Status _CSL_simcopDmaChanBlkStep_Set(CSL_SimcopDmaHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopDmaChanBlkStepType event,
                                             Uint16 data);
#endif

    CSL_Status _CSL_simcopDmaGetAttrs(CSL_SimcopDmaNum simcopDmaNum,
                                      CSL_SimcopDmaHandle hSimcopDma);
    void _CSL_SimcopDma_Reset_Val_Read(CSL_SimcopDmaHandle hndl);

    /* SIMCOP_DMA_IRQSTATUS_RAW(0-1) */
    CSL_Status _CSL_simcopDmaIRQStatusRAW_Get(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance,
                                              CSL_SimcopDmaIRQGetType event,
                                              Bool * data);
    CSL_Status _CSL_simcopDmaIRQStatusRAW_Set(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint8 Chan_Num,
                                              CSL_SimcopDmaIRQSetType event,
                                              Bool data);
    /* SIMCOP_DMA_IRQSTATUS(0-1) */
    CSL_Status _CSL_simcopDmaIRQStatus_Get(CSL_SimcopDmaHandle hndl,
                                           Uint8 instance,
                                           CSL_SimcopDmaIRQGetType event,
                                           Bool * data);
    CSL_Status _CSL_simcopDmaIRQStatus_Set(CSL_SimcopDmaHandle hndl,
                                           Uint8 instance, Uint8 Chan_Num,
                                           CSL_SimcopDmaIRQSetType event,
                                           Bool data);
    /* SIMCOP_DMA_IRQENABLE_SET(0-1) */
    CSL_Status _CSL_simcopDmaIRQEnableSet_Get(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance,
                                              CSL_SimcopDmaIRQGetType event,
                                              Bool * data);
    CSL_Status _CSL_simcopDmaIRQEnableSet_Set(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint8 Chan_Num,
                                              CSL_SimcopDmaIRQSetType event,
                                              Bool data);
    /* SIMCOP_DMA_IRQENABLE_CLR(0-1) */
    CSL_Status _CSL_simcopDmaIRQEnableClr_Get(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance,
                                              CSL_SimcopDmaIRQGetType event,
                                              Bool * data);
    CSL_Status _CSL_simcopDmaIRQEnableClr_Set(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint8 Chan_Num,
                                              CSL_SimcopDmaIRQSetType event,
                                              Bool data);

/*--------macros ----------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif/*__CSL_SIMCOP_DMA_H_*/

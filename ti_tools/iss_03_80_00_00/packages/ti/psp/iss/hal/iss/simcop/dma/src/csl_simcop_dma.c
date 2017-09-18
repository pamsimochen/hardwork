/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file csl_simcop_dma.c
*
* This File contains CSL Level 1 functions for Simcop DMA Module
* This entire description will appear as one
* paragraph in the generated documentation.
*
* @path  $(CSLPATH)\src\DMA\
*
* @rev  00.01
*/
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *! *!
 * 16-Apr-2009 Phanish: Changes made because of Resource Manager Integration.
 * *! i.e., Removal of dependency on _CSL_certifyOpen() calls. *! 19-Mar-2009
 * Phanish: Modified to remove the HWcontrol and GetHWstatus APIs *! and their
 * dependencies. And now, HWsetup calls directly the CSL_FINS *! *! 09-Mar-2009
 * Phanish: XNUM, XSTEP registers are now writing only the required bits in REG.
 * *! And YSTEP has an additional bit now, as per the spec version 3; rev 5 *! *! 
 * 23-Sep-2008 Geetha Ravindran: Created the file. *! *! *! 24-Dec-2000 mf:
 * Revisions appear in reverse chronological order; *! that is, newest first.
 * The date format is dd-Mon-yyyy. *
 * =========================================================================== */

/****************************************************************
*  INCLUDE FILES
****************************************************************/
/*-------program files ----------------------------------------*/

#ifndef _CSL_SIMCOP_DMA_C_
#define _CSL_SIMCOP_DMA_C_

#ifdef __cplusplus
extern "C" {
#endif

    /* #include <ti/psp/iss/hal/iss/simcop/common/csl_types.h> #include
     * <ti/psp/iss/hal/iss/simcop/common/csl_error.h> #include
     * <ti/psp/iss/hal/iss/simcop/common/csl_resid.h> #include
     * <ti/psp/iss/hal/iss/simcop/common/csl.h> #include
     * <ti/psp/iss/hal/iss/simcop/common/csl_resource.h> */
#include "../csl_simcop_dma.h"
#include "../inc/_csl_simcop_dma.h"

#define CSL_SIMCOP_DMA_PER_CNT 1

#define IRQ_INSTANCE_0 0                                   // for Channel IRQ 
                                                           // values in
                                                           // StatusRaw,
                                                           // Status,
                                                           // EnableSet,
                                                           // EnableClr
                                                           // register
#define IRQ_INSTANCE_1 1                                   // for ONLY
                                                           // OCP_Error on
                                                           // the above four
                                                           // registers

    CSL_Status CSL_simcopDmaInit(CSL_SimcopDmaHandle hndl) {

        CSL_Status status = CSL_SOK;
         CSL_EXIT_IF((hndl == NULL), CSL_ESYS_BADHANDLE);

        /* Reset the registers to default values */
         _CSL_SimcopDma_Reset_Val_Read(hndl);

         EXIT: return status;
    } CSL_Status CSL_simcopDmaOpen(CSL_SimcopDmaObj * hSimcopDmaObj,
                                   CSL_SimcopDmaNum simcopDmaNum,
                                   CSL_OpenMode openMode) {
        CSL_SimcopDmaNum simcopDmaInst;

        /* CSL_SimcopDmaHandle hSimcopDma = (CSL_SimcopDmaHandle)NULL; */
        CSL_Status status = CSL_SOK;

        hSimcopDmaObj->openMode = openMode;

        if (simcopDmaNum >= 0)
        {
            status = _CSL_simcopDmaGetAttrs(simcopDmaNum, hSimcopDmaObj);
            CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_OVFL);
            /* hSimcopDma =
             * (CSL_SimcopDmaHandle)_CSL_certifyOpen((CSL_ResHandle)hSimcopDmaObj, 
             * status); *//* @todo */
        }
        else
        {
            for (simcopDmaInst = (CSL_SimcopDmaNum) 0;
                 simcopDmaInst < (CSL_SimcopDmaNum) CSL_SIMCOP_DMA_PER_CNT;
                 ++simcopDmaInst)
            {
                status = _CSL_simcopDmaGetAttrs(simcopDmaInst, hSimcopDmaObj);
                /* hSimcopDma=
                 * (CSL_SimcopDmaHandle)_CSL_certifyOpen((CSL_ResHandle)hSimcopDmaObj, 
                 * status); */
                if (status == CSL_SOK)
                    break;
            }
            /* if (simcopDmaInst == CSL_SIMCOP_DMA_PER_CNT) status =
             * CSL_ESYS_OVFL; */
        }
      EXIT:
        return status;
    }

#if 1
    CSL_Status CSL_simcopDmaHwSetup(CSL_SimcopDmaHandle hndl,
                                    CSL_SimcopDmaHwSetup * setup) {
        Uint8 i;

        Uint16 NumofContexts;

        CSL_Status status = CSL_SOK;

        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        /* CSL_SimcopDmaHwSetup *setup = pSetup; */
        /* Parameter validation */
        CSL_EXIT_IF((hndl == NULL), CSL_ESYS_BADHANDLE);
        CSL_EXIT_IF((hndl->regs == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((setup == NULL), CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        // status =
        // CSL_SimcopDmaGetHWStatus(hndl,CSL_DMA_QUERY_HWINFO_CONTEXT,&NumofContexts);
        // CSL_EXIT_IF((status != CSL_SOK), CSL_ESYS_FAIL);
        NumofContexts =
            (Uint16) CSL_FEXT(simcopDmaRegs->DMAHWINFO,
                              SIMCOP_DMA_DMAHWINFO_CONTEXT);

        CSL_EXIT_IF(((setup->DmaCtrl.TagCnt > (1 << (NumofContexts + 2)) - 1)),
                    CSL_ESYS_INVPARAMS);

        /* DMA Control Register setting */
        CSL_EXIT_IF((setup == NULL), CSL_ESYS_INVPARAMS);
        /* STANDBYMODE Field setting in DMASYSCONFIG reg */
        CSL_FINS(simcopDmaRegs->DMASYSCONFIG,
                 SIMCOP_DMA_DMASYSCONFIG_STANDBYMODE,
                 setup->DmaSysConfig.StandByMode);

        // CSL_EXIT_IF((_CSL_simcopDmaSetStandByMode(hndl,
        // setup->DmaSysConfig.StandByMode) != CSL_SOK), CSL_ESYS_INVPARAMS);
        /* simcopDmaRegs = hndl->regs;
         * _CSL_FieldSet((Uint32*)&(simcopDmaRegs->DMASYSCONFIG),
         * CSL_SIMCOP_DMA_DMASYSCONFIG_STANDBYMODE_MASK,
         * CSL_SIMCOP_DMA_DMASYSCONFIG_STANDBYMODE_SHIFT,
         * setup->DmaSysConfig.StandByMode); */
        // CSL_EXIT_IF((_CSL_simcopDmaSetSoftReset(hndl,
        // setup->DmaSysConfig.SoftResetWrite) != CSL_SOK),
        // CSL_ESYS_INVPARAMS);
        CSL_FINS(simcopDmaRegs->DMASYSCONFIG, SIMCOP_DMA_DMASYSCONFIG_SOFTRESET,
                 setup->DmaSysConfig.SoftResetWrite);

        // CSL_EXIT_IF((_CSL_simcopDmaSet_IRQ_EOI_LineNumber(hndl,
        // setup->EOILineNum) != CSL_SOK), CSL_ESYS_INVPARAMS);
        CSL_FINS(simcopDmaRegs->DMAIRQEOI, SIMCOP_DMA_DMAIRQEOI_LINE_NUMBER,
                 setup->EOILineNum);
        // CSL_EXIT_IF((_CSL_simcopDmaCtrl_Set(hndl,BW_LIMITER,
        // setup->DmaCtrl.BWLimiter) != CSL_SOK), CSL_ESYS_INVPARAMS);
        CSL_FINS(simcopDmaRegs->DMACTRL, SIMCOP_DMA_DMACTRL_BW_LIMITER,
                 setup->DmaCtrl.BWLimiter);
        // CSL_EXIT_IF((_CSL_simcopDmaCtrl_Set(hndl, TAG_CNT,
        // setup->DmaCtrl.TagCnt) != CSL_SOK), CSL_ESYS_INVPARAMS);
        CSL_FINS(simcopDmaRegs->DMACTRL, SIMCOP_DMA_DMACTRL_TAG_CNT,
                 setup->DmaCtrl.TagCnt);
        // CSL_EXIT_IF((_CSL_simcopDmaCtrl_Set(hndl, POSTED_WRITES,
        // setup->DmaCtrl.PostedWrites) != CSL_SOK), CSL_ESYS_INVPARAMS);
        CSL_FINS(simcopDmaRegs->DMACTRL, SIMCOP_DMA_DMACTRL_POSTED_WRITES,
                 setup->DmaCtrl.PostedWrites);
        CSL_FINS(simcopDmaRegs->DMACTRL, SIMCOP_DMA_DMACTRL_MAX_BURST_SIZE,
                 setup->DmaCtrl.MaxBurstSize);
        /* status = _CSL_simcopDmaCtrl_Set(hndl, MAX_BURST_SIZE,
         * setup->DmaCtrl.MaxBurstSize); CSL_EXIT_IF((status != CSL_SOK),
         * CSL_ESYS_INVPARAMS); */

        /* DMA Channel Control Register setting */

        for (i = 0; i < MAX_NUM_OF_DMACHANNELS; i++)       /* Though Simcop
                                                            * DMA has 8
                                                            * channels, OMAP4 
                                                            * UCs use only 4 */
        {
            if (setup->DmaChannelCtrl[i].SetupChan == TRUE)
            {
                // SIMCOP_PRINT("CHAN NO: %d", i);
                // CSL_EXIT_IF(setup->DmaChannelCtrl[i].SMemAddr & MAX_4BIT,
                // CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((setup->DmaChannelCtrl[i].SMemOfst & MAX_4BIT) ||
                            (setup->DmaChannelCtrl[i].SMemOfst > MAX_20BIT),
                            CSL_ESYS_INVPARAMS);

                CSL_EXIT_IF((setup->DmaChannelCtrl[i].BufAddr & MAX_4BIT) ||
                            (setup->DmaChannelCtrl[i].BufAddr > MAX_24BIT),
                            CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((setup->DmaChannelCtrl[i].BufOfst & MAX_4BIT) ||
                            (setup->DmaChannelCtrl[i].BufOfst > MAX_24BIT),
                            CSL_ESYS_INVPARAMS);

                CSL_EXIT_IF(((setup->DmaChannelCtrl[i].ChanBlkSize.YNUM == 0) ||
                             (setup->DmaChannelCtrl[i].ChanBlkSize.YNUM >
                              MAX_13BIT)), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF(((setup->DmaChannelCtrl[i].ChanBlkSize.XNUM == 0) || (setup->DmaChannelCtrl[i].ChanBlkSize.XNUM > MAX_14BIT)), CSL_ESYS_INVPARAMS); // ((setup->DmaChannelCtrl[i].ChanBlkSize.XNUM 
                                                                                                                                                                // & 
                                                                                                                                                                // MAX_4BIT) 
                                                                                                                                                                // !=0 
                                                                                                                                                                // )||

                CSL_EXIT_IF(((setup->DmaChannelCtrl[i].ChanBlkStep.YSTEP <
                              MIN_13BIT) ||
                             (setup->DmaChannelCtrl[i].ChanBlkStep.YSTEP >
                              MAX_13BIT)), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF(((setup->DmaChannelCtrl[i].ChanBlkStep.XSTEP < -(MAX_14BIT + 1)) || (setup->DmaChannelCtrl[i].ChanBlkStep.XSTEP > MAX_14BIT)), CSL_ESYS_INVPARAMS); // ((setup->DmaChannelCtrl[i].ChanBlkStep.XSTEP 
                                                                                                                                                                                // & 
                                                                                                                                                                                // MAX_4BIT) 
                                                                                                                                                                                // !=0 
                                                                                                                                                                                // )||

                CSL_EXIT_IF(((setup->DmaChannelCtrl[i].Frame.YCNT == 0) ||
                             (setup->DmaChannelCtrl[i].Frame.YCNT > MAX_10BIT)),
                            CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF(((setup->DmaChannelCtrl[i].Frame.XCNT == 0) ||
                             (setup->DmaChannelCtrl[i].Frame.XCNT > MAX_10BIT)),
                            CSL_ESYS_INVPARAMS);

                /* CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set(hndl,i, HWSTOP,
                 * setup->DmaChannelCtrl[i].ChanCtrl.HWStop) != CSL_SOK),
                 * CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set(hndl,i, HWSTART,
                 * setup->DmaChannelCtrl[i].ChanCtrl.HWStart) != CSL_SOK),
                 * CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set(hndl,i, LINKED,
                 * setup->DmaChannelCtrl[i].ChanCtrl.Linked) != CSL_SOK),
                 * CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set(hndl,i, GRID,
                 * setup->DmaChannelCtrl[i].ChanCtrl.Grid) != CSL_SOK),
                 * CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set(hndl,i, TILERMODE, 
                 * setup->DmaChannelCtrl[i].ChanCtrl.TilerMode) != CSL_SOK),
                 * CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set(hndl,i, DIR,
                 * setup->DmaChannelCtrl[i].ChanCtrl.Dir) != CSL_SOK),
                 * CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set(hndl,i, SWTRIGGER, 
                 * setup->DmaChannelCtrl[i].ChanCtrl.SWTrigger) != CSL_SOK),
                 * CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set(hndl,i, DISABLE,
                 * setup->DmaChannelCtrl[i].ChanCtrl.Disable) != CSL_SOK),
                 * CSL_ESYS_INVPARAMS); */
                CSL_FINS(simcopDmaRegs->DMACHAN[i].CTRL,
                         SIMCOP_DMA_CHANCTRL_HWSTOP,
                         setup->DmaChannelCtrl[i].ChanCtrl.HWStop);
                CSL_FINS(simcopDmaRegs->DMACHAN[i].CTRL,
                         SIMCOP_DMA_CHANCTRL_HWSTART,
                         setup->DmaChannelCtrl[i].ChanCtrl.HWStart);
                CSL_FINS(simcopDmaRegs->DMACHAN[i].CTRL,
                         SIMCOP_DMA_CHANCTRL_LINKED,
                         setup->DmaChannelCtrl[i].ChanCtrl.Linked);
                CSL_FINS(simcopDmaRegs->DMACHAN[i].CTRL,
                         SIMCOP_DMA_CHANCTRL_GRID,
                         setup->DmaChannelCtrl[i].ChanCtrl.Grid);
                CSL_FINS(simcopDmaRegs->DMACHAN[i].CTRL,
                         SIMCOP_DMA_CHANCTRL_TILERMODE,
                         setup->DmaChannelCtrl[i].ChanCtrl.TilerMode);
                CSL_FINS(simcopDmaRegs->DMACHAN[i].CTRL,
                         SIMCOP_DMA_CHANCTRL_DIR,
                         setup->DmaChannelCtrl[i].ChanCtrl.Dir);
                CSL_FINS(simcopDmaRegs->DMACHAN[i].CTRL,
                         SIMCOP_DMA_CHANCTRL_SWTRIGGER,
                         setup->DmaChannelCtrl[i].ChanCtrl.SWTrigger);
                CSL_FINS(simcopDmaRegs->DMACHAN[i].CTRL,
                         SIMCOP_DMA_CHANCTRL_DISABLE,
                         setup->DmaChannelCtrl[i].ChanCtrl.Disable);

                /* SMEM and IBUF Address and offset setting */
                /* CSL_EXIT_IF((_CSL_simcopDmaChanSmem_Addr_Set(hndl,i,
                 * setup->DmaChannelCtrl[i].SMemAddr)!= CSL_SOK),
                 * CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanSmem_Ofst_Set(hndl,i,
                 * setup->DmaChannelCtrl[i].SMemOfst)!= CSL_SOK),
                 * CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanBuf_Addr_Set(hndl,i,
                 * setup->DmaChannelCtrl[i].BufAddr)!= CSL_SOK),
                 * CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanBuf_Ofst_Set(hndl,i,
                 * setup->DmaChannelCtrl[i].BufOfst)!= CSL_SOK),
                 * CSL_ESYS_INVPARAMS); */
                /* To Do: Assumes NO modifications of these registers and val 
                 * is actual value to be inserted */
                CSL_RINS(simcopDmaRegs->DMACHAN[i].SMEMADDR,
                         setup->DmaChannelCtrl[i].SMemAddr);
                CSL_RINS(simcopDmaRegs->DMACHAN[i].SMEMOFST,
                         setup->DmaChannelCtrl[i].SMemOfst);
                CSL_RINS(simcopDmaRegs->DMACHAN[i].BUFADDR,
                         setup->DmaChannelCtrl[i].BufAddr);
                CSL_RINS(simcopDmaRegs->DMACHAN[i].BUFOFST,
                         setup->DmaChannelCtrl[i].BufOfst);

                /* Block and Frame size setting */
                /* CSL_EXIT_IF((_CSL_simcopDmaChanBlkSize_Set(hndl,i,
                 * YNUM,setup->DmaChannelCtrl[i].ChanBlkSize.YNUM)!=
                 * CSL_SOK), CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanBlkSize_Set(hndl,i,
                 * XNUM,setup->DmaChannelCtrl[i].ChanBlkSize.XNUM)!=
                 * CSL_SOK), CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanFrame_Set(hndl,i,
                 * YCNT,setup->DmaChannelCtrl[i].Frame.YCNT)!= CSL_SOK),
                 * CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanFrame_Set(hndl,i,
                 * XCNT,setup->DmaChannelCtrl[i].Frame.XCNT)!= CSL_SOK),
                 * CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanBlkStep_Set(hndl,i,YSTEP,setup->DmaChannelCtrl[i].ChanBlkStep.YSTEP)!= 
                 * CSL_SOK), CSL_ESYS_INVPARAMS);
                 * CSL_EXIT_IF((_CSL_simcopDmaChanBlkStep_Set(hndl,i,XSTEP,setup->DmaChannelCtrl[i].ChanBlkStep.XSTEP)!= 
                 * CSL_SOK), CSL_ESYS_INVPARAMS); */
                CSL_FINS(simcopDmaRegs->DMACHAN[i].BLOCKSIZE,
                         SIMCOP_DMA_CHANBLOCKSIZE_YNUM,
                         setup->DmaChannelCtrl[i].ChanBlkSize.YNUM);
                CSL_FINS(simcopDmaRegs->DMACHAN[i].BLOCKSIZE,
                         SIMCOP_DMA_CHANBLOCKSIZE_XNUM,
                         setup->DmaChannelCtrl[i].ChanBlkSize.XNUM);
                CSL_FINS(simcopDmaRegs->DMACHAN[i].FRAME,
                         SIMCOP_DMA_CHANFRAME_YCNT,
                         setup->DmaChannelCtrl[i].Frame.YCNT);
                CSL_FINS(simcopDmaRegs->DMACHAN[i].FRAME,
                         SIMCOP_DMA_CHANFRAME_XCNT,
                         setup->DmaChannelCtrl[i].Frame.XCNT);
                CSL_FINS(simcopDmaRegs->DMACHAN[i].BLOCKSTEP,
                         SIMCOP_DMA_CHANBLOCKSTEP_YSTEP,
                         setup->DmaChannelCtrl[i].ChanBlkStep.YSTEP);
                CSL_FINS(simcopDmaRegs->DMACHAN[i].BLOCKSTEP,
                         SIMCOP_DMA_CHANBLOCKSTEP_XSTEP,
                         setup->DmaChannelCtrl[i].ChanBlkStep.XSTEP);

                /* IRQ setting */
                CSL_EXIT_IF((_CSL_simcopDmaIRQEnableSet_Set
                             (hndl, IRQ_INSTANCE_0, i, FRAME_DONE,
                              setup->DmaChannelCtrl[i].IRQConfig.FrameDone.
                              IRQEnableSet) != CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaIRQEnableSet_Set
                             (hndl, IRQ_INSTANCE_0, i, BLOCK_DONE,
                              setup->DmaChannelCtrl[i].IRQConfig.BlockDone.
                              IRQEnableSet) != CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaIRQEnableSet_Set
                             (hndl, IRQ_INSTANCE_1, i, FRAME_DONE,
                              setup->DmaChannelCtrl[i].IRQConfig.FrameDone.
                              IRQEnableSet) != CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaIRQEnableSet_Set
                             (hndl, IRQ_INSTANCE_1, i, BLOCK_DONE,
                              setup->DmaChannelCtrl[i].IRQConfig.BlockDone.
                              IRQEnableSet) != CSL_SOK), CSL_ESYS_INVPARAMS);

                /* Enable the Channel */
                /* CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set(hndl,i, ENABLE,
                 * setup->DmaChannelCtrl[i].ChanCtrl.Enable) != CSL_SOK),
                 * CSL_ESYS_INVPARAMS); */
                CSL_FINS(simcopDmaRegs->DMACHAN[i].CTRL,
                         SIMCOP_DMA_CHANCTRL_ENABLE,
                         setup->DmaChannelCtrl[i].ChanCtrl.Enable);
            }
        }
        CSL_EXIT_IF((_CSL_simcopDmaIRQEnableSet_Set
                     (hndl, IRQ_INSTANCE_1, NULL, OCP_ERROR,
                      setup->OCP_ERROR) != CSL_SOK), CSL_ESYS_INVPARAMS);

      EXIT:
        return status;
    }
#else
    CSL_Status CSL_simcopDmaHwSetup(CSL_SimcopDmaHandle hndl,
                                    CSL_SimcopDmaHwSetup * setup) {
        int i;

        Uint16 NumofContexts;

        CSL_Status status = CSL_SOK;

        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        /* CSL_SimcopDmaHwSetup *setup = pSetup; */
        /* Parameter validation */
        CSL_EXIT_IF((hndl == NULL), CSL_ESYS_BADHANDLE);

        CSL_EXIT_IF((setup == NULL), CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        status =
            CSL_SimcopDmaGetHWStatus(hndl, CSL_DMA_QUERY_HWINFO_CONTEXT,
                                     &NumofContexts);
        CSL_EXIT_IF((status != CSL_SOK), CSL_ESYS_FAIL);

        CSL_EXIT_IF(((setup->DmaCtrl.TagCnt > (1 << (NumofContexts + 2)) - 1)),
                    CSL_ESYS_INVPARAMS);

        /* DMA Control Register setting */
        CSL_EXIT_IF((setup == NULL), CSL_ESYS_INVPARAMS);
        /* STANDBYMODE Field setting in DMASYSCONFIG reg */
        // CSL_FINS(hndl->regs->DMASYSCONFIG,
        // SIMCOP_DMA_DMASYSCONFIG_STANDBYMODE,
        // setup->DmaSysConfig.StandByMode);

        CSL_EXIT_IF((_CSL_simcopDmaSetStandByMode
                     (hndl, setup->DmaSysConfig.StandByMode) != CSL_SOK),
                    CSL_ESYS_INVPARAMS);
        /* simcopDmaRegs = hndl->regs;
         * _CSL_FieldSet((Uint32*)&(simcopDmaRegs->DMASYSCONFIG),
         * CSL_SIMCOP_DMA_DMASYSCONFIG_STANDBYMODE_MASK,
         * CSL_SIMCOP_DMA_DMASYSCONFIG_STANDBYMODE_SHIFT,
         * setup->DmaSysConfig.StandByMode); */
        CSL_EXIT_IF((_CSL_simcopDmaSetSoftReset
                     (hndl, setup->DmaSysConfig.SoftResetWrite) != CSL_SOK),
                    CSL_ESYS_INVPARAMS);
        // CSL_FINS(hndl->regs->DMASYSCONFIG,
        // SIMCOP_DMA_DMASYSCONFIG_SOFTRESET,
        // setup->DmaSysConfig.SoftResetWrite);

        CSL_EXIT_IF((_CSL_simcopDmaSet_IRQ_EOI_LineNumber
                     (hndl, setup->EOILineNum) != CSL_SOK), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((_CSL_simcopDmaCtrl_Set
                     (hndl, BW_LIMITER, setup->DmaCtrl.BWLimiter) != CSL_SOK),
                    CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((_CSL_simcopDmaCtrl_Set
                     (hndl, TAG_CNT, setup->DmaCtrl.TagCnt) != CSL_SOK),
                    CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((_CSL_simcopDmaCtrl_Set
                     (hndl, POSTED_WRITES,
                      setup->DmaCtrl.PostedWrites) != CSL_SOK),
                    CSL_ESYS_INVPARAMS);
        status =
            _CSL_simcopDmaCtrl_Set(hndl, MAX_BURST_SIZE,
                                   setup->DmaCtrl.MaxBurstSize);
        CSL_EXIT_IF((status != CSL_SOK), CSL_ESYS_INVPARAMS);

        /* DMA Channel Control Register setting */

        for (i = 0; i < 4; i++)                            /* Though Simcop
                                                            * DMA has 8
                                                            * channels, OMAP4 
                                                            * UCs use only 4 */
        {
            if (setup->DmaChannelCtrl[i].SetupChan == TRUE)
            {
                // SIMCOP_PRINT("CHAN NO: %d", i);
                // CSL_EXIT_IF(setup->DmaChannelCtrl[i].SMemAddr & MAX_4BIT,
                // CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((setup->DmaChannelCtrl[i].SMemOfst & MAX_4BIT) ||
                            (setup->DmaChannelCtrl[i].SMemOfst > MAX_20BIT),
                            CSL_ESYS_INVPARAMS);

                CSL_EXIT_IF((setup->DmaChannelCtrl[i].BufAddr & MAX_4BIT) ||
                            (setup->DmaChannelCtrl[i].BufAddr > MAX_24BIT),
                            CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((setup->DmaChannelCtrl[i].BufOfst & MAX_4BIT) ||
                            (setup->DmaChannelCtrl[i].BufOfst > MAX_24BIT),
                            CSL_ESYS_INVPARAMS);

                CSL_EXIT_IF(((setup->DmaChannelCtrl[i].ChanBlkSize.YNUM == 0) ||
                             (setup->DmaChannelCtrl[i].ChanBlkSize.YNUM >
                              MAX_13BIT)), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF(((setup->DmaChannelCtrl[i].ChanBlkSize.XNUM == 0) || (setup->DmaChannelCtrl[i].ChanBlkSize.XNUM > MAX_10BIT)), CSL_ESYS_INVPARAMS); // ((setup->DmaChannelCtrl[i].ChanBlkSize.XNUM 
                                                                                                                                                                // & 
                                                                                                                                                                // MAX_4BIT) 
                                                                                                                                                                // !=0 
                                                                                                                                                                // )||

                CSL_EXIT_IF(((setup->DmaChannelCtrl[i].ChanBlkStep.YSTEP <
                              MIN_13BIT) ||
                             (setup->DmaChannelCtrl[i].ChanBlkStep.YSTEP >
                              MAX_13BIT)), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF(((setup->DmaChannelCtrl[i].ChanBlkStep.XSTEP < -(MAX_10BIT + 1)) || (setup->DmaChannelCtrl[i].ChanBlkStep.XSTEP > MAX_10BIT)), CSL_ESYS_INVPARAMS); // ((setup->DmaChannelCtrl[i].ChanBlkStep.XSTEP 
                                                                                                                                                                                // & 
                                                                                                                                                                                // MAX_4BIT) 
                                                                                                                                                                                // !=0 
                                                                                                                                                                                // )||

                CSL_EXIT_IF(((setup->DmaChannelCtrl[i].Frame.YCNT == 0) ||
                             (setup->DmaChannelCtrl[i].Frame.YCNT > MAX_10BIT)),
                            CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF(((setup->DmaChannelCtrl[i].Frame.XCNT == 0) ||
                             (setup->DmaChannelCtrl[i].Frame.XCNT > MAX_10BIT)),
                            CSL_ESYS_INVPARAMS);

                CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set
                             (hndl, i, HWSTOP,
                              setup->DmaChannelCtrl[i].ChanCtrl.HWStop) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set
                             (hndl, i, HWSTART,
                              setup->DmaChannelCtrl[i].ChanCtrl.HWStart) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set
                             (hndl, i, LINKED,
                              setup->DmaChannelCtrl[i].ChanCtrl.Linked) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set
                             (hndl, i, GRID,
                              setup->DmaChannelCtrl[i].ChanCtrl.Grid) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set
                             (hndl, i, TILERMODE,
                              setup->DmaChannelCtrl[i].ChanCtrl.TilerMode) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set
                             (hndl, i, DIR,
                              setup->DmaChannelCtrl[i].ChanCtrl.Dir) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set
                             (hndl, i, SWTRIGGER,
                              setup->DmaChannelCtrl[i].ChanCtrl.SWTrigger) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set
                             (hndl, i, DISABLE,
                              setup->DmaChannelCtrl[i].ChanCtrl.Disable) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);

                /* SMEM and IBUF Address and offset setting */
                CSL_EXIT_IF((_CSL_simcopDmaChanSmem_Addr_Set
                             (hndl, i,
                              setup->DmaChannelCtrl[i].SMemAddr) != CSL_SOK),
                            CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanSmem_Ofst_Set
                             (hndl, i,
                              setup->DmaChannelCtrl[i].SMemOfst) != CSL_SOK),
                            CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanBuf_Addr_Set
                             (hndl, i,
                              setup->DmaChannelCtrl[i].BufAddr) != CSL_SOK),
                            CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanBuf_Ofst_Set
                             (hndl, i,
                              setup->DmaChannelCtrl[i].BufOfst) != CSL_SOK),
                            CSL_ESYS_INVPARAMS);
                /* Block and Frame size setting */
                CSL_EXIT_IF((_CSL_simcopDmaChanBlkSize_Set
                             (hndl, i, YNUM,
                              setup->DmaChannelCtrl[i].ChanBlkSize.YNUM) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanBlkSize_Set
                             (hndl, i, XNUM,
                              setup->DmaChannelCtrl[i].ChanBlkSize.XNUM) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanFrame_Set
                             (hndl, i, YCNT,
                              setup->DmaChannelCtrl[i].Frame.YCNT) != CSL_SOK),
                            CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanFrame_Set
                             (hndl, i, XCNT,
                              setup->DmaChannelCtrl[i].Frame.XCNT) != CSL_SOK),
                            CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanBlkStep_Set
                             (hndl, i, YSTEP,
                              setup->DmaChannelCtrl[i].ChanBlkStep.YSTEP) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaChanBlkStep_Set
                             (hndl, i, XSTEP,
                              setup->DmaChannelCtrl[i].ChanBlkStep.XSTEP) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);
                /* IRQ setting */
                CSL_EXIT_IF((_CSL_simcopDmaIRQEnableSet_Set
                             (hndl, IRQ_INSTANCE_0, i, FRAME_DONE,
                              setup->DmaChannelCtrl[i].IRQConfig.FrameDone.
                              IRQEnableSet) != CSL_SOK), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((_CSL_simcopDmaIRQEnableSet_Set
                             (hndl, IRQ_INSTANCE_0, i, BLOCK_DONE,
                              setup->DmaChannelCtrl[i].IRQConfig.BlockDone.
                              IRQEnableSet) != CSL_SOK), CSL_ESYS_INVPARAMS);
                /* Enable the Channel */
                CSL_EXIT_IF((_CSL_simcopDmaChanCtrl_Set
                             (hndl, i, ENABLE,
                              setup->DmaChannelCtrl[i].ChanCtrl.Enable) !=
                             CSL_SOK), CSL_ESYS_INVPARAMS);
            }
        }
        CSL_EXIT_IF((_CSL_simcopDmaIRQEnableSet_Set
                     (hndl, IRQ_INSTANCE_1, NULL, OCP_ERROR,
                      setup->DmaChannelCtrl[i].IRQConfig.FrameDone.
                      IRQEnableSet) != CSL_SOK), CSL_ESYS_INVPARAMS);

      EXIT:
        return status;
    }
#endif

#if 0
    CSL_Status CSL_SimcopDmaHwControl(CSL_SimcopDmaHandle hndl,
                                      CSL_SimcopDmaHwCtrlCmdType cmd,
                                      void *data) {
        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((hndl == NULL), CSL_ESYS_BADHANDLE);

        switch (cmd)
        {
                /* CHANNEL 3 HW Control */
            case CSL_DMA_CMD_SET_CHAN3_HWSTOP:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_3, HWSTOP,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_HWSTART:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_3, HWSTART,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_LINKED:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_3, LINKED,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_GRID:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_3, GRID,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_TILERMODE:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_3, TILERMODE,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_DIR:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_3, DIR,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_SWTRIGGER:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_3, SWTRIGGER,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_DISABLE:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_3, DISABLE,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_ENABLE:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_3, ENABLE,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_SMEM_ADDR:
                CSL_EXIT_IF(*(Uint32 *) data & MAX_4BIT, CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanSmem_Addr_Set(hndl, CHAN_3,
                                                    *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_SMEM_OFST:
                CSL_EXIT_IF((*(Uint32 *) data & MAX_4BIT) ||
                            (*(Uint32 *) data > MAX_20BIT), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanSmem_Ofst_Set(hndl, CHAN_3,
                                                    *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_BUF_ADDR:
                CSL_EXIT_IF((*(Uint32 *) data & MAX_4BIT) ||
                            (*(Uint32 *) data > MAX_24BIT), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBuf_Addr_Set(hndl, CHAN_3,
                                                   *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_BUF_OFST:
                CSL_EXIT_IF((*(Uint32 *) data & MAX_4BIT) ||
                            (*(Uint32 *) data > MAX_24BIT), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBuf_Ofst_Set(hndl, CHAN_3,
                                                   *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_CHANBLKSIZE_YNUM:
                CSL_EXIT_IF(((*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > MAX_13BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkSize_Set(hndl, CHAN_3, YNUM,
                                                  *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_CHANBLKSIZE_XNUM:
                CSL_EXIT_IF((((*(Uint16 *) data & MAX_4BIT) != 0) ||
                             (*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > 16368)), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkSize_Set(hndl, CHAN_3, XNUM,
                                                  *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_CHANFRAME_YCNT:
                CSL_EXIT_IF(((*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > MAX_10BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanFrame_Set(hndl, CHAN_3, YCNT,
                                                *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_CHANFRAME_XCNT:
                CSL_EXIT_IF(((*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > MAX_10BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanFrame_Set(hndl, CHAN_3, XCNT,
                                                *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_CHANBLKSTEP_YSTEP:
                CSL_EXIT_IF(((*(Int16 *) data < MIN_13BIT) ||
                             (*(Int16 *) data > MAX_13BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkStep_Set(hndl, CHAN_3, YSTEP,
                                                  *(Int16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_CHANBLKSTEP_XSTEP:
                CSL_EXIT_IF((((*(Int16 *) data & MAX_4BIT) != 0) ||
                             (*(Int16 *) data < MIN_14BIT) ||
                             (*(Uint16 *) data > 16368)), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkStep_Set(hndl, CHAN_3, XSTEP,
                                                  *(Int16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_IRQSTATUSRAW_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Set(hndl, IRQ_INSTANCE_0, 3,
                                                   FRAME_DONE, *(Bool *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN3_IRQSTATUS_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Set(hndl, IRQ_INSTANCE_0, 3,
                                                FRAME_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN3_IRQENABLESET_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Set(hndl, IRQ_INSTANCE_0, 3,
                                                   FRAME_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN3_IRQENABLECLR_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Set(hndl, IRQ_INSTANCE_0, 3,
                                                   FRAME_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN3_IRQSTATUSRAW_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Set(hndl, IRQ_INSTANCE_0, 3,
                                                   BLOCK_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN3_IRQSTATUS_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Set(hndl, IRQ_INSTANCE_0, 3,
                                                BLOCK_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN3_IRQENABLESET_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Set(hndl, IRQ_INSTANCE_0, 3,
                                                   BLOCK_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN3_IRQENABLECLR_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Set(hndl, IRQ_INSTANCE_0, 3,
                                                   BLOCK_DONE, *(Bool *) data);
                break;

                /* CHANNEL 2 HW Control */
            case CSL_DMA_CMD_SET_CHAN2_HWSTOP:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_2, HWSTOP,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_HWSTART:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_2, HWSTART,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_LINKED:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_2, LINKED,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_GRID:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_2, GRID,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_TILERMODE:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_2, TILERMODE,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_DIR:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_2, DIR,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_SWTRIGGER:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_2, SWTRIGGER,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_DISABLE:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_2, DISABLE,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_ENABLE:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_2, ENABLE,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_SMEM_ADDR:
                CSL_EXIT_IF(*(Uint32 *) data & MAX_4BIT, CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanSmem_Addr_Set(hndl, CHAN_2,
                                                    *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_SMEM_OFST:
                CSL_EXIT_IF((*(Uint32 *) data & MAX_4BIT) ||
                            (*(Uint32 *) data > MAX_20BIT), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanSmem_Ofst_Set(hndl, CHAN_2,
                                                    *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_BUF_ADDR:
                CSL_EXIT_IF((*(Uint32 *) data & MAX_4BIT) ||
                            (*(Uint32 *) data > MAX_24BIT), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBuf_Addr_Set(hndl, CHAN_2,
                                                   *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_BUF_OFST:
                CSL_EXIT_IF((*(Uint32 *) data & MAX_4BIT) ||
                            (*(Uint32 *) data > MAX_24BIT), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBuf_Ofst_Set(hndl, CHAN_2,
                                                   *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_CHANBLKSIZE_YNUM:
                CSL_EXIT_IF(((*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > MAX_13BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkSize_Set(hndl, CHAN_2, YNUM,
                                                  *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_CHANBLKSIZE_XNUM:
                CSL_EXIT_IF((((*(Uint16 *) data & MAX_4BIT) != 0) ||
                             (*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > 16368)), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkSize_Set(hndl, CHAN_2, XNUM,
                                                  *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_CHANFRAME_YCNT:
                CSL_EXIT_IF(((*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > MAX_10BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanFrame_Set(hndl, CHAN_2, YCNT,
                                                *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_CHANFRAME_XCNT:
                CSL_EXIT_IF(((*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > MAX_10BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanFrame_Set(hndl, CHAN_2, XCNT,
                                                *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_CHANBLKSTEP_YSTEP:
                CSL_EXIT_IF(((*(Int16 *) data < MIN_13BIT) ||
                             (*(Int16 *) data > MAX_13BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkStep_Set(hndl, CHAN_2, YSTEP,
                                                  *(Int16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_CHANBLKSTEP_XSTEP:
                CSL_EXIT_IF((((*(Int16 *) data & MAX_4BIT) != 0) ||
                             (*(Int16 *) data < MIN_14BIT) ||
                             (*(Uint16 *) data > 16368)), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkStep_Set(hndl, CHAN_2, XSTEP,
                                                  *(Int16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_IRQSTATUSRAW_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Set(hndl, IRQ_INSTANCE_0, 2,
                                                   FRAME_DONE, *(Bool *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN2_IRQSTATUS_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Set(hndl, IRQ_INSTANCE_0, 2,
                                                FRAME_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN2_IRQENABLESET_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Set(hndl, IRQ_INSTANCE_0, 2,
                                                   FRAME_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN2_IRQENABLECLR_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Set(hndl, IRQ_INSTANCE_0, 2,
                                                   FRAME_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN2_IRQSTATUSRAW_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Set(hndl, IRQ_INSTANCE_0, 2,
                                                   BLOCK_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN2_IRQSTATUS_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Set(hndl, IRQ_INSTANCE_0, 2,
                                                BLOCK_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN2_IRQENABLESET_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Set(hndl, IRQ_INSTANCE_0, 2,
                                                   BLOCK_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN2_IRQENABLECLR_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Set(hndl, IRQ_INSTANCE_0, 2,
                                                   BLOCK_DONE, *(Bool *) data);
                break;

                /* CHANNEL 1 HW Control */
            case CSL_DMA_CMD_SET_CHAN1_HWSTOP:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_1, HWSTOP,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_HWSTART:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_1, HWSTART,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_LINKED:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_1, LINKED,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_GRID:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_1, GRID,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_TILERMODE:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_1, TILERMODE,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_DIR:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_1, DIR,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_SWTRIGGER:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_1, SWTRIGGER,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_DISABLE:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_1, DISABLE,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_ENABLE:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_1, ENABLE,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_SMEM_ADDR:
                CSL_EXIT_IF(*(Uint32 *) data & MAX_4BIT, CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanSmem_Addr_Set(hndl, CHAN_1,
                                                    *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_SMEM_OFST:
                CSL_EXIT_IF((*(Uint32 *) data & MAX_4BIT) ||
                            (*(Uint32 *) data > MAX_20BIT), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanSmem_Ofst_Set(hndl, CHAN_1,
                                                    *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_BUF_ADDR:
                CSL_EXIT_IF((*(Uint32 *) data & MAX_4BIT) ||
                            (*(Uint32 *) data > MAX_24BIT), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBuf_Addr_Set(hndl, CHAN_1,
                                                   *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_BUF_OFST:
                CSL_EXIT_IF((*(Uint32 *) data & MAX_4BIT) ||
                            (*(Uint32 *) data > MAX_24BIT), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBuf_Ofst_Set(hndl, CHAN_1,
                                                   *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_CHANBLKSIZE_YNUM:
                CSL_EXIT_IF(((*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > MAX_13BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkSize_Set(hndl, CHAN_1, YNUM,
                                                  *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_CHANBLKSIZE_XNUM:
                CSL_EXIT_IF((((*(Uint16 *) data & MAX_4BIT) != 0) ||
                             (*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > 16368)), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkSize_Set(hndl, CHAN_1, XNUM,
                                                  *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_CHANFRAME_YCNT:
                CSL_EXIT_IF(((*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > MAX_10BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanFrame_Set(hndl, CHAN_1, YCNT,
                                                *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_CHANFRAME_XCNT:
                CSL_EXIT_IF(((*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > MAX_10BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanFrame_Set(hndl, CHAN_1, XCNT,
                                                *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_CHANBLKSTEP_YSTEP:
                CSL_EXIT_IF(((*(Int16 *) data < MIN_13BIT) ||
                             (*(Int16 *) data > MAX_13BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkStep_Set(hndl, CHAN_1, YSTEP,
                                                  *(Int16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_CHANBLKSTEP_XSTEP:
                CSL_EXIT_IF((((*(Int16 *) data & MAX_4BIT) != 0) ||
                             (*(Int16 *) data < MIN_14BIT) ||
                             (*(Uint16 *) data > 16368)), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkStep_Set(hndl, CHAN_1, XSTEP,
                                                  *(Int16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_IRQSTATUSRAW_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Set(hndl, IRQ_INSTANCE_0, 1,
                                                   FRAME_DONE, *(Bool *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN1_IRQSTATUS_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Set(hndl, IRQ_INSTANCE_0, 1,
                                                FRAME_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN1_IRQENABLESET_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Set(hndl, IRQ_INSTANCE_0, 1,
                                                   FRAME_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN1_IRQENABLECLR_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Set(hndl, IRQ_INSTANCE_0, 1,
                                                   FRAME_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN1_IRQSTATUSRAW_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Set(hndl, IRQ_INSTANCE_0, 1,
                                                   BLOCK_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN1_IRQSTATUS_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Set(hndl, IRQ_INSTANCE_0, 1,
                                                BLOCK_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN1_IRQENABLESET_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Set(hndl, IRQ_INSTANCE_0, 1,
                                                   BLOCK_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN1_IRQENABLECLR_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Set(hndl, IRQ_INSTANCE_0, 1,
                                                   BLOCK_DONE, *(Bool *) data);
                break;

                /* CHANNEL 0 HW Control */
            case CSL_DMA_CMD_SET_CHAN0_HWSTOP:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_0, HWSTOP,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_HWSTART:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_0, HWSTART,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_LINKED:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_0, LINKED,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_GRID:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_0, GRID,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_TILERMODE:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_0, TILERMODE,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_DIR:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_0, DIR,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_SWTRIGGER:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_0, SWTRIGGER,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_DISABLE:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_0, DISABLE,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_ENABLE:
                status =
                    _CSL_simcopDmaChanCtrl_Set(hndl, CHAN_0, ENABLE,
                                               *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_SMEM_ADDR:
                CSL_EXIT_IF(*(Uint32 *) data & MAX_4BIT, CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanSmem_Addr_Set(hndl, CHAN_0,
                                                    *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_SMEM_OFST:
                CSL_EXIT_IF((*(Uint32 *) data & MAX_4BIT) ||
                            (*(Uint32 *) data > MAX_20BIT), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanSmem_Ofst_Set(hndl, CHAN_0,
                                                    *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_BUF_ADDR:
                CSL_EXIT_IF((*(Uint32 *) data & MAX_4BIT) ||
                            (*(Uint32 *) data > MAX_24BIT), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBuf_Addr_Set(hndl, CHAN_0,
                                                   *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_BUF_OFST:
                CSL_EXIT_IF((*(Uint32 *) data & MAX_4BIT) ||
                            (*(Uint32 *) data > MAX_24BIT), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBuf_Ofst_Set(hndl, CHAN_0,
                                                   *(Uint32 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_CHANBLKSIZE_YNUM:
                CSL_EXIT_IF(((*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > MAX_13BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkSize_Set(hndl, CHAN_0, YNUM,
                                                  *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_CHANBLKSIZE_XNUM:
                CSL_EXIT_IF((((*(Uint16 *) data & MAX_4BIT) != 0) ||
                             (*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > 16368)), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkSize_Set(hndl, CHAN_0, XNUM,
                                                  *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_CHANFRAME_YCNT:
                CSL_EXIT_IF(((*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > MAX_10BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanFrame_Set(hndl, CHAN_0, YCNT,
                                                *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_CHANFRAME_XCNT:
                CSL_EXIT_IF(((*(Uint16 *) data == 0) ||
                             (*(Uint16 *) data > MAX_10BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanFrame_Set(hndl, CHAN_0, XCNT,
                                                *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_CHANBLKSTEP_YSTEP:
                CSL_EXIT_IF(((*(Int16 *) data < MIN_13BIT) ||
                             (*(Int16 *) data > MAX_13BIT)),
                            CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkStep_Set(hndl, CHAN_0, YSTEP,
                                                  *(Int16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_CHANBLKSTEP_XSTEP:
                CSL_EXIT_IF((((*(Int16 *) data & MAX_4BIT) != 0) ||
                             (*(Int16 *) data < MIN_14BIT) ||
                             (*(Uint16 *) data > 16368)), CSL_ESYS_INVPARAMS);
                status =
                    _CSL_simcopDmaChanBlkStep_Set(hndl, CHAN_0, XSTEP,
                                                  *(Int16 *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_IRQSTATUSRAW_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Set(hndl, IRQ_INSTANCE_0, 0,
                                                   FRAME_DONE, *(Bool *) data);
                break;
            case CSL_DMA_CMD_SET_CHAN0_IRQSTATUS_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Set(hndl, IRQ_INSTANCE_0, 0,
                                                FRAME_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN0_IRQENABLESET_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Set(hndl, IRQ_INSTANCE_0, 0,
                                                   FRAME_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN0_IRQENABLECLR_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Set(hndl, IRQ_INSTANCE_0, 0,
                                                   FRAME_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN0_IRQSTATUSRAW_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Set(hndl, IRQ_INSTANCE_0, 0,
                                                   BLOCK_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN0_IRQSTATUS_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Set(hndl, IRQ_INSTANCE_0, 0,
                                                BLOCK_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN0_IRQENABLESET_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Set(hndl, IRQ_INSTANCE_0, 0,
                                                   BLOCK_DONE, *(Bool *) data);
                break;

            case CSL_DMA_CMD_SET_CHAN0_IRQENABLECLR_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Set(hndl, IRQ_INSTANCE_0, 0,
                                                   BLOCK_DONE, *(Bool *) data);
                break;

                /* OCP Error Control */
            case CSL_DMA_CMD_SET_OCP_IRQSTATUSRAW_ERR:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Set(hndl, IRQ_INSTANCE_1, 0,
                                                   OCP_ERROR, *(Bool *) data);
                break;
            case CSL_DMA_CMD_SET_OCP_IRQSTATUS_ERR:
                status =
                    _CSL_simcopDmaIRQStatus_Set(hndl, IRQ_INSTANCE_1, 0,
                                                OCP_ERROR, *(Bool *) data);
                break;
            case CSL_DMA_CMD_SET_OCP_IRQENABLESET_ERR:
                status =
                    _CSL_simcopDmaIRQEnableSet_Set(hndl, IRQ_INSTANCE_1, 0,
                                                   OCP_ERROR, *(Bool *) data);
                break;
            case CSL_DMA_CMD_SET_OCP_IRQENABLECLR_ERR:
                status =
                    _CSL_simcopDmaIRQEnableClr_Set(hndl, IRQ_INSTANCE_1, 0,
                                                   OCP_ERROR, *(Bool *) data);
                break;
                /* DMA System Configuration Control */
            case CSL_DMA_CMD_SET_SYSCONFIG_STANDBYMODE:
                status = _CSL_simcopDmaSetStandByMode(hndl, *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_SYSCONFIG_SOFTRESET:
                status = _CSL_simcopDmaSetSoftReset(hndl, *(Uint16 *) data);
                break;
                /* DMA IRQ EOI Line Number Control */
            case CSL_DMA_CMD_SET_IRQEOI_LINE_NUMBER:
                status =
                    _CSL_simcopDmaSet_IRQ_EOI_LineNumber(hndl,
                                                         *(Uint16 *) data);
                break;
                /* DMA Control */
            case CSL_DMA_CMD_SET_BW_LIMITER:
                status =
                    _CSL_simcopDmaCtrl_Set(hndl, BW_LIMITER, *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_TAG_CNT:
                status =
                    _CSL_simcopDmaCtrl_Set(hndl, TAG_CNT, *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_POSTED_WRITES:
                status =
                    _CSL_simcopDmaCtrl_Set(hndl, POSTED_WRITES,
                                           *(Uint16 *) data);
                break;
            case CSL_DMA_CMD_SET_MAX_BURST_SIZE:
                status =
                    _CSL_simcopDmaCtrl_Set(hndl, MAX_BURST_SIZE,
                                           *(Uint16 *) data);
                break;

            default:
                return CSL_ESYS_INVCMD;

        }
      EXIT:
        return status;

    }

    CSL_Status CSL_SimcopDmaGetHWStatus(CSL_SimcopDmaHandle hndl,
                                        CSL_SimcopDmaHWQueryType query,
                                        void *data) {
        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((hndl == NULL), CSL_ESYS_BADHANDLE);

        switch (query)
        {
                /* Channel 3 */
            case CSL_DMA_QUERY_CHAN3_HWSTOP:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_3, HWSTOP,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_HWSTART:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_3, HWSTART,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_LINKED:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_3, LINKED,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_GRID:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_3, GRID,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_TILERMODE:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_3, TILERMODE,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_DIR:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_3, DIR,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_STATUS:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_3, STATUS,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_SMEM_ADDR:
                status =
                    _CSL_simcopDmaChanSmem_Addr_Get(hndl, CHAN_3,
                                                    (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_SMEM_OFST:
                status =
                    _CSL_simcopDmaChanSmem_Ofst_Get(hndl, CHAN_3,
                                                    (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_BUF_ADDR:
                status =
                    _CSL_simcopDmaChanBuf_Addr_Get(hndl, CHAN_3,
                                                   (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_BUF_OFST:
                status =
                    _CSL_simcopDmaChanBuf_Ofst_Get(hndl, CHAN_3,
                                                   (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_CHANBLKSIZE_YNUM:
                status =
                    _CSL_simcopDmaChanBlkSize_Get(hndl, CHAN_3, YNUM,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_CHANBLKSIZE_XNUM:
                status =
                    _CSL_simcopDmaChanBlkSize_Get(hndl, CHAN_3, XNUM,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_CHANFRAME_YCNT:
                status =
                    _CSL_simcopDmaChanFrame_Get(hndl, CHAN_3, YCNT,
                                                (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_CHANFRAME_XCNT:
                status =
                    _CSL_simcopDmaChanFrame_Get(hndl, CHAN_3, XCNT,
                                                (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_CHANCURRBLK_BY:
                status =
                    _CSL_simcopDmaChanCurrBlk_Get(hndl, CHAN_3, BY,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_CHANCURRBLK_BX:
                status =
                    _CSL_simcopDmaChanCurrBlk_Get(hndl, CHAN_3, BX,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_CHANBLKSTEP_YSTEP:
                status =
                    _CSL_simcopDmaChanBlkStep_Get(hndl, CHAN_3, YSTEP,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_CHANBLKSTEP_XSTEP:
                status =
                    _CSL_simcopDmaChanBlkStep_Get(hndl, CHAN_3, XSTEP,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_IRQSTATUSRAW_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN3_FRAME_DONE_IRQ,
                                                   (Bool *) data);
                break;
            case CSL_DMA_QUERY_CHAN3_IRQSTATUS_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Get(hndl, IRQ_INSTANCE_0,
                                                CHAN3_FRAME_DONE_IRQ,
                                                (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN3_IRQENABLESET_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN3_FRAME_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN3_IRQENABLECLR_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN3_FRAME_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN3_IRQSTATUSRAW_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN3_BLOCK_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN3_IRQSTATUS_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Get(hndl, IRQ_INSTANCE_0,
                                                CHAN3_BLOCK_DONE_IRQ,
                                                (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN3_IRQENABLESET_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN3_BLOCK_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN3_IRQENABLECLR_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN3_BLOCK_DONE_IRQ,
                                                   (Bool *) data);
                break;

                /* Channel 2 */
            case CSL_DMA_QUERY_CHAN2_HWSTOP:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_2, HWSTOP,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_HWSTART:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_2, HWSTART,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_LINKED:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_2, LINKED,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_GRID:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_2, GRID,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_TILERMODE:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_2, TILERMODE,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_DIR:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_2, DIR,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_STATUS:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_2, STATUS,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_SMEM_ADDR:
                status =
                    _CSL_simcopDmaChanSmem_Addr_Get(hndl, CHAN_2,
                                                    (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_SMEM_OFST:
                status =
                    _CSL_simcopDmaChanSmem_Ofst_Get(hndl, CHAN_2,
                                                    (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_BUF_ADDR:
                status =
                    _CSL_simcopDmaChanBuf_Addr_Get(hndl, CHAN_2,
                                                   (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_BUF_OFST:
                status =
                    _CSL_simcopDmaChanBuf_Ofst_Get(hndl, CHAN_2,
                                                   (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_CHANBLKSIZE_YNUM:
                status =
                    _CSL_simcopDmaChanBlkSize_Get(hndl, CHAN_2, YNUM,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_CHANBLKSIZE_XNUM:
                status =
                    _CSL_simcopDmaChanBlkSize_Get(hndl, CHAN_2, XNUM,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_CHANFRAME_YCNT:
                status =
                    _CSL_simcopDmaChanFrame_Get(hndl, CHAN_2, YCNT,
                                                (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_CHANFRAME_XCNT:
                status =
                    _CSL_simcopDmaChanFrame_Get(hndl, CHAN_2, XCNT,
                                                (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_CHANCURRBLK_BY:
                status =
                    _CSL_simcopDmaChanCurrBlk_Get(hndl, CHAN_2, BY,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_CHANCURRBLK_BX:
                status =
                    _CSL_simcopDmaChanCurrBlk_Get(hndl, CHAN_2, BX,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_CHANBLKSTEP_YSTEP:
                status =
                    _CSL_simcopDmaChanBlkStep_Get(hndl, CHAN_2, YSTEP,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_CHANBLKSTEP_XSTEP:
                status =
                    _CSL_simcopDmaChanBlkStep_Get(hndl, CHAN_2, XSTEP,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_IRQSTATUSRAW_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN2_FRAME_DONE_IRQ,
                                                   (Bool *) data);
                break;
            case CSL_DMA_QUERY_CHAN2_IRQSTATUS_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Get(hndl, IRQ_INSTANCE_0,
                                                CHAN2_FRAME_DONE_IRQ,
                                                (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN2_IRQENABLESET_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN2_FRAME_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN2_IRQENABLECLR_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN2_FRAME_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN2_IRQSTATUSRAW_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN2_BLOCK_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN2_IRQSTATUS_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Get(hndl, IRQ_INSTANCE_0,
                                                CHAN2_BLOCK_DONE_IRQ,
                                                (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN2_IRQENABLESET_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN2_BLOCK_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN2_IRQENABLECLR_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN2_BLOCK_DONE_IRQ,
                                                   (Bool *) data);
                break;

                /* Channel 1 */
            case CSL_DMA_QUERY_CHAN1_HWSTOP:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_1, HWSTOP,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_HWSTART:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_1, HWSTART,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_LINKED:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_1, LINKED,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_GRID:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_1, GRID,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_TILERMODE:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_1, TILERMODE,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_DIR:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_1, DIR,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_STATUS:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_1, STATUS,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_SMEM_ADDR:
                status =
                    _CSL_simcopDmaChanSmem_Addr_Get(hndl, CHAN_1,
                                                    (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_SMEM_OFST:
                status =
                    _CSL_simcopDmaChanSmem_Ofst_Get(hndl, CHAN_1,
                                                    (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_BUF_ADDR:
                status =
                    _CSL_simcopDmaChanBuf_Addr_Get(hndl, CHAN_1,
                                                   (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_BUF_OFST:
                status =
                    _CSL_simcopDmaChanBuf_Ofst_Get(hndl, CHAN_1,
                                                   (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_CHANBLKSIZE_YNUM:
                status =
                    _CSL_simcopDmaChanBlkSize_Get(hndl, CHAN_1, YNUM,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_CHANBLKSIZE_XNUM:
                status =
                    _CSL_simcopDmaChanBlkSize_Get(hndl, CHAN_1, XNUM,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_CHANFRAME_YCNT:
                status =
                    _CSL_simcopDmaChanFrame_Get(hndl, CHAN_1, YCNT,
                                                (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_CHANFRAME_XCNT:
                status =
                    _CSL_simcopDmaChanFrame_Get(hndl, CHAN_1, XCNT,
                                                (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_CHANCURRBLK_BY:
                status =
                    _CSL_simcopDmaChanCurrBlk_Get(hndl, CHAN_1, BY,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_CHANCURRBLK_BX:
                status =
                    _CSL_simcopDmaChanCurrBlk_Get(hndl, CHAN_1, BX,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_CHANBLKSTEP_YSTEP:
                status =
                    _CSL_simcopDmaChanBlkStep_Get(hndl, CHAN_1, YSTEP,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_CHANBLKSTEP_XSTEP:
                status =
                    _CSL_simcopDmaChanBlkStep_Get(hndl, CHAN_1, XSTEP,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_IRQSTATUSRAW_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN1_FRAME_DONE_IRQ,
                                                   (Bool *) data);
                break;
            case CSL_DMA_QUERY_CHAN1_IRQSTATUS_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Get(hndl, IRQ_INSTANCE_0,
                                                CHAN1_FRAME_DONE_IRQ,
                                                (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN1_IRQENABLESET_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN1_FRAME_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN1_IRQENABLECLR_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN1_FRAME_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN1_IRQSTATUSRAW_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN1_BLOCK_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN1_IRQSTATUS_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Get(hndl, IRQ_INSTANCE_0,
                                                CHAN1_BLOCK_DONE_IRQ,
                                                (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN1_IRQENABLESET_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN1_BLOCK_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN1_IRQENABLECLR_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN1_BLOCK_DONE_IRQ,
                                                   (Bool *) data);
                break;

                /* Channel 0 */
            case CSL_DMA_QUERY_CHAN0_HWSTOP:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_0, HWSTOP,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_HWSTART:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_0, HWSTART,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_LINKED:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_0, LINKED,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_GRID:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_0, GRID,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_TILERMODE:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_0, TILERMODE,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_DIR:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_0, DIR,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_STATUS:
                status =
                    _CSL_simcopDmaChanCtrl_Get(hndl, CHAN_0, STATUS,
                                               (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_SMEM_ADDR:
                status =
                    _CSL_simcopDmaChanSmem_Addr_Get(hndl, CHAN_0,
                                                    (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_SMEM_OFST:
                status =
                    _CSL_simcopDmaChanSmem_Ofst_Get(hndl, CHAN_0,
                                                    (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_BUF_ADDR:
                status =
                    _CSL_simcopDmaChanBuf_Addr_Get(hndl, CHAN_0,
                                                   (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_BUF_OFST:
                status =
                    _CSL_simcopDmaChanBuf_Ofst_Get(hndl, CHAN_0,
                                                   (Uint32 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_CHANBLKSIZE_YNUM:
                status =
                    _CSL_simcopDmaChanBlkSize_Get(hndl, CHAN_0, YNUM,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_CHANBLKSIZE_XNUM:
                status =
                    _CSL_simcopDmaChanBlkSize_Get(hndl, CHAN_0, XNUM,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_CHANFRAME_YCNT:
                status =
                    _CSL_simcopDmaChanFrame_Get(hndl, CHAN_0, YCNT,
                                                (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_CHANFRAME_XCNT:
                status =
                    _CSL_simcopDmaChanFrame_Get(hndl, CHAN_0, XCNT,
                                                (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_CHANCURRBLK_BY:
                status =
                    _CSL_simcopDmaChanCurrBlk_Get(hndl, CHAN_0, BY,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_CHANCURRBLK_BX:
                status =
                    _CSL_simcopDmaChanCurrBlk_Get(hndl, CHAN_0, BX,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_CHANBLKSTEP_YSTEP:
                status =
                    _CSL_simcopDmaChanBlkStep_Get(hndl, CHAN_0, YSTEP,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_CHANBLKSTEP_XSTEP:
                status =
                    _CSL_simcopDmaChanBlkStep_Get(hndl, CHAN_0, XSTEP,
                                                  (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_IRQSTATUSRAW_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN0_FRAME_DONE_IRQ,
                                                   (Bool *) data);
                break;
            case CSL_DMA_QUERY_CHAN0_IRQSTATUS_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Get(hndl, IRQ_INSTANCE_0,
                                                CHAN0_FRAME_DONE_IRQ,
                                                (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN0_IRQENABLESET_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN0_FRAME_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN0_IRQENABLECLR_FRAME_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN0_FRAME_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN0_IRQSTATUSRAW_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN0_BLOCK_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN0_IRQSTATUS_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQStatus_Get(hndl, IRQ_INSTANCE_0,
                                                CHAN0_BLOCK_DONE_IRQ,
                                                (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN0_IRQENABLESET_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableSet_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN0_BLOCK_DONE_IRQ,
                                                   (Bool *) data);
                break;

            case CSL_DMA_QUERY_CHAN0_IRQENABLECLR_BLOCK_DONE:
                status =
                    _CSL_simcopDmaIRQEnableClr_Get(hndl, IRQ_INSTANCE_0,
                                                   CHAN0_BLOCK_DONE_IRQ,
                                                   (Bool *) data);
                break;

                /* OCP Error */
            case CSL_DMA_QUERY_OCP_IRQSTATUSRAW_ERR:
                status =
                    _CSL_simcopDmaIRQStatusRAW_Get(hndl, IRQ_INSTANCE_1,
                                                   OCP_ERR, (Bool *) data);
                break;
            case CSL_DMA_QUERY_OCP_IRQSTATUS_ERR:
                status =
                    _CSL_simcopDmaIRQStatus_Get(hndl, IRQ_INSTANCE_1, OCP_ERR,
                                                (Bool *) data);
                break;
            case CSL_DMA_QUERY_OCP_IRQENABLESET_ERR:
                status =
                    _CSL_simcopDmaIRQEnableSet_Get(hndl, IRQ_INSTANCE_1,
                                                   OCP_ERR, (Bool *) data);
                break;
            case CSL_DMA_QUERY_OCP_IRQENABLECLR_ERR:
                status =
                    _CSL_simcopDmaIRQEnableClr_Get(hndl, IRQ_INSTANCE_1,
                                                   OCP_ERR, (Bool *) data);
                break;

                /* DMA Revision */
            case CSL_DMA_QUERY_REVISION_SCHEME:
                status = _CSL_simcopDmaGetRev(hndl, SCHEME, (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_REVISION_FUNC:
                status = _CSL_simcopDmaGetRev(hndl, FUNC, (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_REVISION_R_RTL:
                status = _CSL_simcopDmaGetRev(hndl, R_RTL, (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_REVISION_X_MAJOR:
                status = _CSL_simcopDmaGetRev(hndl, X_MAJOR, (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_REVISION_CUSTOM:
                status = _CSL_simcopDmaGetRev(hndl, CUSTOM, (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_REVISION_Y_MINOR:
                status = _CSL_simcopDmaGetRev(hndl, Y_MINOR, (Uint16 *) data);
                break;
                /* DMA HW Info */
            case CSL_DMA_QUERY_HWINFO_CHAN:
                status = _CSL_simcopDmaGetNumOfChannels(hndl, (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_HWINFO_CONTEXT:
                status = _CSL_simcopGetContext(hndl, (Uint16 *) data);
                break;
                /* DMA System Config */
            case CSL_DMA_QUERY_SYSCONFIG_STANDBYMODE:
                status = _CSL_simcopDmaGetStandByMode(hndl, (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_SYSCONFIG_SOFTRESET:
                status = _CSL_simcopDmaGetSoftReset(hndl, (Uint16 *) data);
                break;
                /* DMA IRQ EOI Line Number */
            case CSL_DMA_QUERY_IRQEOI_LINE_NUMBER:
                status =
                    _CSL_simcopDmaGet_IRQ_EOI_LineNumber(hndl, (Uint16 *) data);
                break;
                /* DMA Control */
            case CSL_DMA_QUERY_BW_LIMITER:
                status =
                    _CSL_simcopDmaCtrl_Get(hndl, BW_LIMITER, (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_TAG_CNT:
                status = _CSL_simcopDmaCtrl_Get(hndl, TAG_CNT, (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_POSTED_WRITES:
                status =
                    _CSL_simcopDmaCtrl_Get(hndl, POSTED_WRITES,
                                           (Uint16 *) data);
                break;
            case CSL_DMA_QUERY_MAX_BURST_SIZE:
                status =
                    _CSL_simcopDmaCtrl_Get(hndl, MAX_BURST_SIZE,
                                           (Uint16 *) data);
                break;

            default:
                return CSL_ESYS_INVQUERY;

        }
      EXIT:
        return status;
    }
#endif

    CSL_Status CSL_simcopDmaClose(CSL_SimcopDmaHandle hndl) {
        /* Indicate in the CSL global data structure that the peripheral has
         * been unreserved */
        /* return (_CSL_certifyClose((CSL_ResHandle)hndl)); */
        return CSL_SOK;
    }

#endif /*_CSL_SIMCOP_DMA_C_*/

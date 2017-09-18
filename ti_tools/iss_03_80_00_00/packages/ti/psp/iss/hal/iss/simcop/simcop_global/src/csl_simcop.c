/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file csl_simcop.c
*
* This File contains CSL APIs for Simcop Module
* This entire description will appear as one 
* paragraph in the generated documentation.
*
* @path  $(CSLPATH)\SIMCOP\src\
*
* @rev  00.01
*/
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *!
 *! 16-Oct-2009 Phanish: Inclusion of HW Sequencer Override Bool variable in HWSetup().
 *!                      And cleaning up of the commented code
 *! 16-Apr-2009 Phanish: Changes made because of Resource Manager Integration. 
 *!                       i.e., Removal of dependency on _CSL_certifyOpen() calls.
 *! 19-Mar-2009 Phanish: Modified to remove the HWcontrol and GetHWstatus APIs 
 *!                 and their dependencies. And now, HWsetup calls directly the CSL_FINS
 *!
 *! 17-Sep-2008 Padmanabha V Reddy:  Created the file.  
 *! 
 *!
 *! 24-Dec-2000 mf: Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */

/****************************************************************
*  INCLUDE FILES                                                 
****************************************************************/
/*-------program files ----------------------------------------*/
#include "../csl_simcop.h"
#include "../inc/_csl_simcop.h"
// #include <ti/psp/iss/hal/iss/simcop/common/csl_resource.h>

/* ===================================================================
 *  @func     CSL_simcopInit                                               
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
CSL_Status CSL_simcopInit(CSL_SimcopHandle hndl)
{
    _CSL_simcopRegisterReset(hndl);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     CSL_simcopOpen                                               
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
CSL_Status CSL_simcopOpen(CSL_SimcopObj * hSimcopObj, CSL_SimcopNum simcopNum,
                          CSL_OpenMode openMode)
{

    CSL_SimcopNum simcopInst;

    /* CSL_SimcopHandle hSimcop = (CSL_SimcopHandle)NULL; */
    CSL_Status status = CSL_SOK;

    hSimcopObj->openMode = openMode;

    if (simcopNum >= 0)
    {
        status = _CSL_simcopGetAttrs(simcopNum, hSimcopObj);
        CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_OVFL);
        /* hSimcop =
         * (CSL_SimcopHandle)_CSL_certifyOpen((CSL_ResHandle)hSimcopObj,
         * status); */
    }
    else
    {
        for (simcopInst = (CSL_SimcopNum) 0;
             simcopInst < (CSL_SimcopNum) CSL_SIMCOP_PER_CNT; ++simcopInst)
        {
            status = _CSL_simcopGetAttrs(simcopInst, hSimcopObj);
            /* hSimcop =
             * (CSL_SimcopHandle)_CSL_certifyOpen((CSL_ResHandle)hSimcopObj,
             * status); */
            if (status == CSL_SOK)
                break;
        }
        /* if (simcopInst == CSL_SIMCOP_PER_CNT) status = CSL_ESYS_OVFL; */
    }

  EXIT:
    return (status);
}

/* ===================================================================
 *  @func     CSL_simcopHwSetup                                               
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
CSL_Status CSL_simcopHwSetup(CSL_SimcopHandle hndl,
                             CSL_SimcopHwSetupCtrl * setup)
{
    CSL_Status status = CSL_SOK;

    Uint8 i;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((hndl->regs == NULL), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((setup == NULL), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((setup->BbmCtrl.BbmSyncChan > MAX_2BIT), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((setup->LdcCtrl.LdcRTagCnt > MAX_4BIT), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((setup->LdcCtrl.LdcRTagOfst > MAX_4BIT), CSL_ESYS_INVPARAMS);
    for (i = HWSEQ_STEP_0; i < MAX_NUM_OF_HWSEQ_STEPS; i++)
    {
        if (setup->HwseqCtrl.HwSeqStep[i].SetupStep == TRUE)
        {
            CSL_EXIT_IF((setup->HwseqCtrl.HwSeqStep[i].ImxAStart > 8191),
                        CSL_ESYS_INVPARAMS);
            CSL_EXIT_IF((setup->HwseqCtrl.HwSeqStep[i].ImxBStart > 8191),
                        CSL_ESYS_INVPARAMS);
        }
    }
    simcopRegs = hndl->regs;
    CSL_FINS(simcopRegs->HL_SYSCONFIG, SIMCOP_HL_SYSCONFIG_STANDBYMODE,
             setup->StandbyMode);
    CSL_FINS(simcopRegs->HL_SYSCONFIG, SIMCOP_HL_SYSCONFIG_SOFTRESET,
             setup->SoftReset);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_IRQ0_MODE, setup->IrqMode[0]);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_IRQ1_MODE, setup->IrqMode[1]);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_IRQ2_MODE, setup->IrqMode[2]);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_IRQ3_MODE, setup->IrqMode[3]);
    /* 
     * IRQ 
     */
    for (i = SIMCOP_IRQ_0; i < MAX_NUM_OF_SIMCOP_IRQ_LINES; i++)
    {
        if (setup->Irq[i].SetupIrq == TRUE)
        {
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_CPU_PROC_START_IRQ,
                     setup->Irq[i].CpuProcStartIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_SIMCOP_DMA_IRQ1,
                     setup->Irq[i].SimcopDmaIrq1.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_MTCR2OCP_ERR_IRQ,
                     setup->Irq[i].Mtcr2OcpErrIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_OCP_ERR_IRQ,
                     setup->Irq[i].OcpErrIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_VLCDJ_DECODE_ERR_IRQ,
                     setup->Irq[i].VlcdjDecodeErrIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_DONE_IRQ,
                     setup->Irq[i].DoneIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_STEP3_IRQ,
                     setup->Irq[i].Step3Irq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_STEP2_IRQ,
                     setup->Irq[i].Step2Irq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_STEP1_IRQ,
                     setup->Irq[i].Step1Irq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_STEP0_IRQ,
                     setup->Irq[i].Step0Irq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_LDC_BLOCK_IRQ,
                     setup->Irq[i].LdcBlockIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_ROT_A_IRQ,
                     setup->Irq[i].RotIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_IMX_B_IRQ,
                     setup->Irq[i].ImxBIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_IMX_A_IRQ,
                     setup->Irq[i].ImxAIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_NSF_IRQ_IRQ,
                     setup->Irq[i].NsfIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_VLCDJ_BLOC_IRQ,
                     setup->Irq[i].VlcdjBlocIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_DCT_IRQ,
                     setup->Irq[i].DctIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_LDC_FRAME_IRQ,
                     setup->Irq[i].LdcFrameIrq.IRQEnableSet);
            CSL_FINS(simcopRegs->HL_IRQ[i].ENABLE_SET,
                     SIMCOP_ENABLE_SET_SIMCOP_DMA_IRQ0,
                     setup->Irq[i].SimcopDmaIrq0.IRQEnableSet);

        }
    }
    /* CSL_EXIT_IF((_CSL_simcopIRQEOILineNumber_Set(hndl,setup->IrqEoi) !=
     * CSL_SOK), CSL_ESYS_INVPARAMS); */
    CSL_FINS(simcopRegs->HL_IRQ_EOI, SIMCOP_HL_IRQ_EOI_LINE_NUMBER,
             setup->IrqEoi);
    /* 
     * Common 
     */
    /* Simcop Control reg settings */
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_LDC_R_BURST_BREAK,
             setup->LdcCtrl.LdcRBurstBreak);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_LDC_R_MAX_BURST_LENGTH,
             setup->LdcCtrl.LdcRMaxBurstLength);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_LDC_R_TAG_CNT,
             setup->LdcCtrl.LdcRTagCnt);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_LDC_R_TAG_OFST,
             setup->LdcCtrl.LdcRTagOfst);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_LDC_LUT, setup->BufAccCtrl.LdcLut);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_LDC_INPUT,
             setup->BufAccCtrl.LdcInput);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_NSF_WMEM, setup->BufAccCtrl.NsfWMem);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_QUANT, setup->BufAccCtrl.Quant);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_HUFF, setup->BufAccCtrl.Huff);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_IMX_A_CMD,
             setup->BufAccCtrl.ImxACmd);
    CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_IMX_B_CMD,
             setup->BufAccCtrl.ImxBCmd);

    /* Simcop Clk Control reg settings */
    CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_DMA_CLK, setup->ClkCtrl.Dma);
    CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_LDC_CLK, setup->ClkCtrl.Ldc);
    CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_NSF2_CLK, setup->ClkCtrl.Nsf);
    CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_ROT_A_CLK, setup->ClkCtrl.Rot);
    CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_DCT_CLK, setup->ClkCtrl.Dct);
    CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_VLCDJ_CLK,
             setup->ClkCtrl.Vlcdj);
    CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_IMX_A_CLK,
             setup->ClkCtrl.ImxA);
    CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_IMX_B_CLK,
             setup->ClkCtrl.ImxB);

    CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_HW_SEQ_STEP_COUNTER,
             setup->HwseqCtrl.HwSeqStepCounter);
    CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_STEP,
             setup->HwseqCtrl.Step);
    CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_HW_SEQ_START,
             setup->HwseqCtrl.HwSeqStart);
    CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_HW_SEQ_STOP,
             setup->HwseqCtrl.HwSeqStop);

    /* 
     * HwSeq Step 
     */
    for (i = HWSEQ_STEP_0; i < MAX_NUM_OF_HWSEQ_STEPS; i++)
    {
        if (setup->HwseqCtrl.HwSeqStep[i].SetupStep == TRUE)
        {
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_CPU_SYNC,
                     setup->HwseqCtrl.HwSeqStep[i].CpuSync);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_DMA_SYNC,
                     setup->HwseqCtrl.HwSeqStep[i].DmaSync);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_ROT_A_SYNC,
                     setup->HwseqCtrl.HwSeqStep[i].RotSync);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_LDC_SYNC,
                     setup->HwseqCtrl.HwSeqStep[i].LdcSync);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_NSF_SYNC,
                     setup->HwseqCtrl.HwSeqStep[i].NsfSync);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_DCT_SYNC,
                     setup->HwseqCtrl.HwSeqStep[i].DctSync);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_VLCDJ_SYNC,
                     setup->HwseqCtrl.HwSeqStep[i].VlcdjSync);

            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_DMA_OFST,
                     setup->HwseqCtrl.HwSeqStep[i].DmaOfst);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_ROT_I_OFST,
                     setup->HwseqCtrl.HwSeqStep[i].RotIOfst);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_ROT_O_OFST,
                     setup->HwseqCtrl.HwSeqStep[i].RotOOfst);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_DCT_S_OFST,
                     setup->HwseqCtrl.HwSeqStep[i].DctSOfst);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_DCT_F_OFST,
                     setup->HwseqCtrl.HwSeqStep[i].DctFOfst);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_VLCDJ_IO_OFST,
                     setup->HwseqCtrl.HwSeqStep[i].VlcdjIOOfst);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_IMX_A_D_OFST,
                     setup->HwseqCtrl.HwSeqStep[i].ImxADataOfst);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL,
                     SIMCOP_STEP_CTRL_IMX_B_D_OFST,
                     setup->HwseqCtrl.HwSeqStep[i].ImxBDataOfst);

            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_A,
                     setup->HwseqCtrl.HwSeqStep[i].ImBuffA);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_B,
                     setup->HwseqCtrl.HwSeqStep[i].ImBuffB);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_C,
                     setup->HwseqCtrl.HwSeqStep[i].ImBuffC);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_D,
                     setup->HwseqCtrl.HwSeqStep[i].ImBuffD);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_E,
                     setup->HwseqCtrl.HwSeqStep[i].ImBuffE);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_F,
                     setup->HwseqCtrl.HwSeqStep[i].ImBuffF);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_G,
                     setup->HwseqCtrl.HwSeqStep[i].ImBuffG);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_H,
                     setup->HwseqCtrl.HwSeqStep[i].ImBuffH);

            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL2,
                     SIMCOP_STEP_CTRL2_LDC_O_OFST,
                     setup->HwseqCtrl.HwSeqStep[i].LdcOOfst);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL2,
                     SIMCOP_STEP_CTRL2_NSF2_IO_OFST,
                     setup->HwseqCtrl.HwSeqStep[i].NsfIOOfst);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL2,
                     SIMCOP_STEP_CTRL2_COEFF_A,
                     setup->HwseqCtrl.HwSeqStep[i].CoeffA);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL2,
                     SIMCOP_STEP_CTRL2_COEFF_B,
                     setup->HwseqCtrl.HwSeqStep[i].CoeffB);

            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_IMX_CTRL,
                     SIMCOP_STEP_IMX_CTRL_IMX_A_SYNC,
                     setup->HwseqCtrl.HwSeqStep[i].ImxASync);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_IMX_CTRL,
                     SIMCOP_STEP_IMX_CTRL_IMX_B_SYNC,
                     setup->HwseqCtrl.HwSeqStep[i].ImxBSync);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_IMX_CTRL,
                     SIMCOP_STEP_IMX_CTRL_IMX_A_START,
                     setup->HwseqCtrl.HwSeqStep[i].ImxAStart);
            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_IMX_CTRL,
                     SIMCOP_STEP_IMX_CTRL_IMX_B_START,
                     setup->HwseqCtrl.HwSeqStep[i].ImxBStart);

            CSL_FINS(simcopRegs->HWSEQ_STEP[i].STEP_CTRL, SIMCOP_STEP_CTRL_NEXT,
                     setup->HwseqCtrl.HwSeqStep[i].Next);
        }
    }
    /* 
     * HwSeq Override
     */
    if (setup->HwseqCtrl.HwSeqOvr.SetupHwOverRide == TRUE)
    {
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_IMBUFF_A_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffAOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_IMBUFF_B_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffBOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_IMBUFF_C_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffCOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_IMBUFF_D_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffDOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_IMBUFF_E_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffEOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_IMBUFF_F_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffFOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_IMBUFF_G_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffGOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_IMBUFF_H_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffHOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE, SIMCOP_HWSEQ_OVERRIDE_COEFF_A_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.CoeffAOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE, SIMCOP_HWSEQ_OVERRIDE_COEFF_B_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.CoeffBOvr);

        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_LDC_O_OFST_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.LdcOOfstOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_NSF_IO_OFST_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.NsfIOOfstOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_ROT_I_OFST_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.RotIOfstOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_ROT_O_OFST_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.RotOOfstOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_DCT_S_OFST_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.DctSOfstOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_DCT_F_OFST_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.DctFOfstOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_VLCDJ_IO_OFST_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.VlcdjIOOfstOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_IMX_A_D_OFST_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.ImxADataOfstOvr);
        CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                 SIMCOP_HWSEQ_OVERRIDE_IMX_B_D_OFST_CTRL,
                 setup->HwseqCtrl.HwSeqOvr.ImxBDataOfstOvr);

        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_I_OFST_OVR,
                 setup->HwseqCtrl.HwSeqOvr.RotIOfst);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_O_OFST_OVR,
                 setup->HwseqCtrl.HwSeqOvr.RotOOfst);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DCT_S_OFST_OVR,
                 setup->HwseqCtrl.HwSeqOvr.DctSOfst);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DCT_F_OFST_OVR,
                 setup->HwseqCtrl.HwSeqOvr.DctFOfst);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_VLCDJ_IO_OFST_OVR,
                 setup->HwseqCtrl.HwSeqOvr.VlcdjIOOfst);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_IMX_A_D_OFST_OVR,
                 setup->HwseqCtrl.HwSeqOvr.ImxADataOfst);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_IMX_B_D_OFST_OVR,
                 setup->HwseqCtrl.HwSeqOvr.ImxBDataOfst);

        CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_A_OVR,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffA);
        CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_B_OVR,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffB);
        CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_C_OVR,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffC);
        CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_D_OVR,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffD);
        CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_E_OVR,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffE);
        CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_F_OVR,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffF);
        CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_G_OVR,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffG);
        CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_H_OVR,
                 setup->HwseqCtrl.HwSeqOvr.ImBuffH);

        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_LDC_O_OFST_OVR,
                 setup->HwseqCtrl.HwSeqOvr.LdcOOfst);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_NSF2_IO_OFST_OVR,
                 setup->HwseqCtrl.HwSeqOvr.NsfIOOfst);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_A_OVR,
                 setup->HwseqCtrl.HwSeqOvr.CoeffA);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_B_OVR,
                 setup->HwseqCtrl.HwSeqOvr.CoeffB);

        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER,
                 setup->HwseqCtrl.HwSeqOvr.DmaTrigger);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_LDC_TRIGGER,
                 setup->HwseqCtrl.HwSeqOvr.LdcTrigger);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_NSF_TRIGGER,
                 setup->HwseqCtrl.HwSeqOvr.NsfTrigger);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_A_TRIGGER,
                 setup->HwseqCtrl.HwSeqOvr.RotTrigger);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DCT_TRIGGER,
                 setup->HwseqCtrl.HwSeqOvr.DctTrigger);
        CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                 SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_VLCDJ_TRIGGER,
                 setup->HwseqCtrl.HwSeqOvr.VlcdjTrigger);
    }
    /* 
     * Common
     */
    CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_CPU_PROC_DONE,
             setup->HwseqCtrl.CpuProcDone);
    /* 
     * BBM
     */
    CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_BBM_SYNC_CHAN,
             setup->BbmCtrl.BbmSyncChan);
    CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_BITSTREAM,
             setup->BbmCtrl.BitStream);
    CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_BITSTR_XFER_SIZE,
             setup->BbmCtrl.BitStreamXferSize);

  EXIT:
    return status;
}

/* ===================================================================
 *  @func     CSL_simcopClose                                               
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
CSL_Status CSL_simcopClose(CSL_SimcopHandle hndl)
{
    /* return (_CSL_certifyClose((CSL_ResHandle)hndl)); */
    return CSL_SOK;
}

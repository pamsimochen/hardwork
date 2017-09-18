/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file _csl_simcop.h
*
* This File contains declarations for _csl_simcop.c
* This entire description will appear as one 
* paragraph in the generated documentation.
*
* @path  $(CSLPATH)\inc\SIMCOP\
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
 *! 17-Sep-2008 Padmanabha V Reddy:  Created the file.  
 *! 
 *!
 *! 24-Dec-2000 mf: Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */

#ifndef _CSL_SIMCOP_H
#define _CSL_SIMCOP_H

#ifdef __cplusplus

extern "C" {
#endif
/****************************************************************
*  INCLUDE FILES                                                 
****************************************************************/
/*-------program files ----------------------------------------*/
#include <ti/psp/iss/hal/iss/simcop/common/csl.h>
#include "../csl_simcop.h"
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/
    /* ========================================================================== 
     */
    /* 
     * Simcop Registers's Field selection Enums
     */
    /* ========================================================================== 
     */
#if 0
    /* SIMCOP_HL_REVISION */
    typedef enum {
        SCHEME,
        FUNC,
        R_RTL,
        X_MAJOR,
        CUSTOM,
        Y_MINOR
    } CSL_SimcopRevisionType;
    /* SIMCOP_HL_HWINFO */
    typedef enum {
        LDCFIFOSIZE,
        NUMIMGBUFS,
        ROT_A,
        IMX_B,
        IMX_A,
        NSF,
        VLCDJ,
        DCT,
        LDC
    } CSL_SimcopHwInfoType;
    /* SIMCOP_HL_SYSCONFIG */
    typedef enum {
        STANDBYMODE,
        SOFTRESET
    } CSL_SimcopSysConfigType;

    /* SIMCOP_HL_IRQ*****__X(0-4) */
    typedef enum {
        CPU_PROC_START_IRQ,
        SIMCOP_DMA_IRQ1,
        MTCR2OCP_ERR_IRQ,
        OCP_ERR_IRQ,
        VLCDJ_DECODE_ERR_IRQ,
        DONE_IRQ,
        STEP3_IRQ,
        STEP2_IRQ,
        STEP1_IRQ,
        STEP0_IRQ,
        LDC_BLOCK_IRQ,
        ROT_A_IRQ,
        IMX_B_IRQ,
        IMX_A_IRQ,
        NSF_IRQ_IRQ,
        VLCDJ_BLOC_IRQ,
        DCT_IRQ,
        LDC_FRAME_IRQ,
        SIMCOP_DMA_IRQ0
    } CSL_SimcopIRQType;
    /* SIMCOP_CTRL */
    typedef enum {
        LDC_R_BURST_BREAK,
        LDC_R_MAX_BURST_LENGTH,
        LDC_R_TAG_CNT,
        LDC_R_TAG_OFST,
        IMX_B_CMD,
        IMX_A_CMD,
        HUFF,
        QUANT,
        LDC_LUT,
        LDC_INPUT,
        NSF_WMEM,
        IRQ3_MODE,
        IRQ2_MODE,
        IRQ1_MODE,
        IRQ0_MODE
    } CSL_SimcopCtrlType;
    /* SIMCOP_CLKCTRL */
    typedef enum {
        ROT_A_CLK,
        IMX_B_CLK,
        IMX_A_CLK,
        NSF2_CLK,
        VLCDJ_CLK,
        DCT_CLK,
        LDC_CLK,
        DMA_CLK,
        ALL_CLK
    } CSL_SimcopClkCtrlType;
    /* HWSEQ_CTRL */
    typedef enum {
        HW_SEQ_STEP_COUNTER,
        STEP,
        CPU_PROC_DONE,
        BBM_SYNC_CHAN,
        BBM_STATUS,
        BITSTREAM,
        BITSTR_XFER_SIZE,
        HW_SEQ_STOP,
        HW_SEQ_START
    } CSL_SimcopHwSeqCtrlType;
    /* HWSEQ_STATUS */
    typedef enum {
        HW_SEQ_STEP_COUNTER_STATUS,
        STATE
    } CSL_SimcopHwSeqStatusType;
    /* HWSEQ_OVERRIDE */
    typedef enum {
        COEFF_B_CTRL,
        COEFF_A_CTRL,
        IMBUFF_H_CTRL,
        IMBUFF_G_CTRL,
        IMBUFF_F_CTRL,
        IMBUFF_E_CTRL,
        IMBUFF_D_CTRL,
        IMBUFF_C_CTRL,
        IMBUFF_B_CTRL,
        IMBUFF_A_CTRL,
        LDC_O_OFST_CTRL,
        ROT_O_OFST_CTRL,
        ROT_I_OFST_CTRL,
        NSF_IO_OFST_CTRL,
        DCT_F_OFST_CTRL,
        DCT_S_OFST_CTRL,
        VLCDJ_IO_OFST_CTRL,
        IMX_B_D_OFST_CTRL,
        IMX_A_D_OFST_CTRL
    } CSL_SimcopHwSeqOverrideType;
    /* HWSEQ_STEP_CTRL_OVERRIDE */
    typedef enum {
        ROT_O_OFST_OVR,
        ROT_I_OFST_OVR,
        DCT_F_OFST_OVR,
        DCT_S_OFST_OVR,
        VLCDJ_IO_OFST_OVR,
        IMX_B_D_OFST_OVR,
        IMX_A_D_OFST_OVR,
        DMA_TRIGGER,
        ROT_A_TRIGGER,
        NSF_TRIGGER,
        VLCDJ_TRIGGER,
        DCT_TRIGGER,
        LDC_TRIGGER
    } CSL_SimcopHwSeqStepCtrlOverrideType;
    /* HWSEQ_STEP_SWITCH_OVERRIDE */
    typedef enum {
        IMBUFF_H_OVR,
        IMBUFF_G_OVR,
        IMBUFF_F_OVR,
        IMBUFF_E_OVR,
        IMBUFF_D_OVR,
        IMBUFF_C_OVR,
        IMBUFF_B_OVR,
        IMBUFF_A_OVR
    } CSL_SimcopHwSeqStepSwitchOverrideType;
    /* HWSEQ_STEP_CTRL2_OVERRIDE */
    typedef enum {
        NSF2_IO_OFST_OVR,
        LDC_O_OFST_OVR,
        COEFF_B_OVR,
        COEFF_A_OVR
    } CSL_SimcopHwSeqStepCtrl2OverrideType;
    /* SIMCOP_HWSEQ_STEP_CTRL__X(0-3) */
    typedef enum {
        CPU_SYNC,
        DMA_OFST,
        ROT_O_OFST,
        ROT_I_OFST,
        DCT_F_OFST,
        DCT_S_OFST,
        VLCDJ_IO_OFST,
        IMX_B_D_OFST,
        IMX_A_D_OFST,
        NEXT,
        DMA_SYNC,
        ROT_A_SYNC,
        NSF_SYNC,
        VLCDJ_SYNC,
        DCT_SYNC,
        LDC_SYNC
    } CSL_SimcopHwSeqStepCtrlType;
    /* SIMCOP_HWSEQ_STEP_SWITCH__X(0-3) */
    typedef enum {
        IMBUFF_H,
        IMBUFF_G,
        IMBUFF_F,
        IMBUFF_E,
        IMBUFF_D,
        IMBUFF_C,
        IMBUFF_B,
        IMBUFF_A
    } CSL_SimcopHwSeqStepSwitchType;
    /* SIMCOP_HWSEQ_STEP_IMX_CTRL__X(0-3) */
    typedef enum {
        IMX_B_SYNC,
        IMX_B_START,
        IMX_A_SYNC,
        IMX_A_START
    } CSL_SimcopHwSeqStepImxCtrlType;
    /* SIMCOP_HWSEQ_STEP_CTRL2__X(0-3) */
    typedef enum {
        NSF2_IO_OFST,
        LDC_O_OFST,
        COEFF_B,
        COEFF_A
    } CSL_SimcopHwSeqStepCtrl2Type;

/*--------function prototypes ---------------------------------*/
    /* SIMCOP_HL_REVISION */
    CSL_Status _CSL_simcopGetRev(CSL_SimcopHandle hndl,
                                 CSL_SimcopRevisionType RevType, Uint16 * data);
    /* SIMCOP_HL_HWINFO */
    CSL_Status _CSL_simcopHwInfo_Get(CSL_SimcopHandle hndl,
                                     CSL_SimcopHwInfoType event, Uint8 * data);
    /* SIMCOP_HL_SYSCONFIG */
    CSL_Status _CSL_simcopSysConfig_Get(CSL_SimcopHandle hndl,
                                        CSL_SimcopSysConfigType event,
                                        Uint8 * data);
    CSL_Status _CSL_simcopSysConfig_Set(CSL_SimcopHandle hndl,
                                        CSL_SimcopSysConfigType event,
                                        Uint8 data);
    /* SIMCOP_HL_IRQ_EOI */
    CSL_Status _CSL_simcopIRQEOILineNumber_Get(CSL_SimcopHandle hndl,
                                               Uint8 * data);
    CSL_Status _CSL_simcopIRQEOILineNumber_Set(CSL_SimcopHandle hndl,
                                               Uint8 data);
    /* SIMCOP_HL_IRQ */
    CSL_Status _CSL_simcopIRQRaw_IsEventPending(CSL_SimcopHandle hndl,
                                                Uint8 instance,
                                                CSL_SimcopIRQType event,
                                                Bool * data);
    CSL_Status _CSL_simcopIRQRaw_SetEventPending(CSL_SimcopHandle hndl,
                                                 Uint8 instance,
                                                 CSL_SimcopIRQType event,
                                                 Bool data);
    CSL_Status _CSL_simcopIRQ_IsEventPending(CSL_SimcopHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopIRQType event,
                                             Bool * data);
    CSL_Status _CSL_simcopIRQ_SetEventPending(CSL_SimcopHandle hndl,
                                              Uint8 instance,
                                              CSL_SimcopIRQType event,
                                              Bool data);
    CSL_Status _CSL_simcopIRQSet_IsEnabled(CSL_SimcopHandle hndl,
                                           Uint8 instance,
                                           CSL_SimcopIRQType event,
                                           Bool * data);
    CSL_Status _CSL_simcopIRQSet_Enable(CSL_SimcopHandle hndl, Uint8 instance,
                                        CSL_SimcopIRQType event, Bool data);
    CSL_Status _CSL_simcopIRQClr_IsEnabled(CSL_SimcopHandle hndl,
                                           Uint8 instance,
                                           CSL_SimcopIRQType event,
                                           Bool * data);
    CSL_Status _CSL_simcopIRQClr_Disable(CSL_SimcopHandle hndl, Uint8 instance,
                                         CSL_SimcopIRQType event, Bool data);
    /* SIMCOP CTRL */
    CSL_Status _CSL_simcopCtrl_Get(CSL_SimcopHandle hndl,
                                   CSL_SimcopCtrlType event, Uint8 * data);
    CSL_Status _CSL_simcopCtrl_Set(CSL_SimcopHandle hndl,
                                   CSL_SimcopCtrlType event, Uint8 data);
    /* SIMCOP CLKCTRL */
    CSL_Status _CSL_simcopClkCtrl_Get(CSL_SimcopHandle hndl,
                                      CSL_SimcopClkCtrlType event, Bool * data);
    CSL_Status _CSL_simcopClkCtrl_Set(CSL_SimcopHandle hndl,
                                      CSL_SimcopClkCtrlType event, Bool data);
    /* SIMCOP HWSEQ */
    CSL_Status _CSL_simcopHwSeqCtrl_Get(CSL_SimcopHandle hndl,
                                        CSL_SimcopHwSeqCtrlType event,
                                        Uint16 * data);
    CSL_Status _CSL_simcopHwSeqCtrl_Set(CSL_SimcopHandle hndl,
                                        CSL_SimcopHwSeqCtrlType event,
                                        Uint16 data);
    CSL_Status _CSL_simcopHwSeqStatus_Get(CSL_SimcopHandle hndl,
                                          CSL_SimcopHwSeqStatusType event,
                                          Uint16 * data);
    CSL_Status _CSL_simcopHwSeqOverride_Get(CSL_SimcopHandle hndl,
                                            CSL_SimcopHwSeqOverrideType event,
                                            Bool * data);
    CSL_Status _CSL_simcopHwSeqOverride_Set(CSL_SimcopHandle hndl,
                                            CSL_SimcopHwSeqOverrideType event,
                                            Bool data);
    CSL_Status _CSL_simcopHwSeqStepCtrlOverride_Get(CSL_SimcopHandle hndl,
                                                    CSL_SimcopHwSeqStepCtrlOverrideType
                                                    event, Uint8 * data);
    CSL_Status _CSL_simcopHwSeqStepCtrlOverride_Set(CSL_SimcopHandle hndl,
                                                    CSL_SimcopHwSeqStepCtrlOverrideType
                                                    event, Uint8 data);
    CSL_Status _CSL_simcopHwSeqStepSwitchOverride_Get(CSL_SimcopHandle hndl,
                                                      CSL_SimcopHwSeqStepSwitchOverrideType
                                                      event, Uint8 * data);
    CSL_Status _CSL_simcopHwSeqStepSwitchOverride_Set(CSL_SimcopHandle hndl,
                                                      CSL_SimcopHwSeqStepSwitchOverrideType
                                                      event, Uint8 data);
    CSL_Status _CSL_simcopHwSeqStepCtrl2Override_Get(CSL_SimcopHandle hndl,
                                                     CSL_SimcopHwSeqStepCtrl2OverrideType
                                                     event, Uint8 * data);
    CSL_Status _CSL_simcopHwSeqStepCtrl2Override_Set(CSL_SimcopHandle hndl,
                                                     CSL_SimcopHwSeqStepCtrl2OverrideType
                                                     event, Uint8 data);
    CSL_Status _CSL_simcopHwSeqStepCtrl_Get(CSL_SimcopHandle hndl,
                                            Uint8 instance,
                                            CSL_SimcopHwSeqStepCtrlType event,
                                            Uint8 * data);
    CSL_Status _CSL_simcopHwSeqStepCtrl_Set(CSL_SimcopHandle hndl,
                                            Uint8 instance,
                                            CSL_SimcopHwSeqStepCtrlType event,
                                            Uint8 data);
    CSL_Status _CSL_simcopHwSeqStepSwitch_Get(CSL_SimcopHandle hndl,
                                              Uint8 instance,
                                              CSL_SimcopHwSeqStepSwitchType
                                              event, Uint8 * data);
    CSL_Status _CSL_simcopHwSeqStepSwitch_Set(CSL_SimcopHandle hndl,
                                              Uint8 instance,
                                              CSL_SimcopHwSeqStepSwitchType
                                              event, Uint8 data);
    CSL_Status _CSL_simcopHwSeqStepImxCtrl_Get(CSL_SimcopHandle hndl,
                                               Uint8 instance,
                                               CSL_SimcopHwSeqStepImxCtrlType
                                               event, Uint16 * data);
    CSL_Status _CSL_simcopHwSeqStepImxCtrl_Set(CSL_SimcopHandle hndl,
                                               Uint8 instance,
                                               CSL_SimcopHwSeqStepImxCtrlType
                                               event, Uint16 data);
    CSL_Status _CSL_simcopHwSeqStepCtrl2_Get(CSL_SimcopHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopHwSeqStepCtrl2Type event,
                                             Uint8 * data);
    CSL_Status _CSL_simcopHwSeqStepCtrl2_Set(CSL_SimcopHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopHwSeqStepCtrl2Type event,
                                             Uint8 data);
    /* Buffer Access */
    CSL_Status _CSL_simcopBuffer_Get(CSL_SimcopHandle hndl,
                                     CSL_SimcopBufferType event, void *data);
    CSL_Status _CSL_simcopBuffer_Set(CSL_SimcopHandle hndl,
                                     CSL_SimcopBufferType event, void *data);
#endif
    /* Generic functions */
    CSL_Status _CSL_simcopGetAttrs(CSL_SimcopNum simcopNum,
                                   CSL_SimcopHandle hSimcop);
    void _CSL_simcopRegisterReset(CSL_SimcopHandle hndl);

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/*--------macros ----------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif                                                     /* _CSL_SIMCOP_H */

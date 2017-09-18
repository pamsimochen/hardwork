/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file csl_simcop.h
*
* This File contains declarations for csl_simcop.c
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

#ifndef CSL_SIMCOP_H
#define CSL_SIMCOP_H

#ifdef __cplusplus

extern "C" {
#endif
/****************************************************************
*  INCLUDE FILES                                                 
****************************************************************/
/*-------program files ----------------------------------------*/
#include <ti/psp/iss/hal/iss/simcop/common/csl.h>
#include <ti/psp/iss/hal/iss/simcop/common/simcop.h>
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopObj - Simcop Object structure
     * @param  openMode  This is the mode which the CSL instance is opened
     * @param  uid  This is a unique identifier to the instance of SIMCOP being referred to by this object
     * @param  xio  This is the variable that contains the current state of a resource being shared by current instance of 
     SIMCOP with other peripherals
     * @param  regs  This is a pointer to the registers of the instance of SIMCOP referred to by this object
     * @param  perNum  This is the instance of SIMCOP being referred to by this object
     */
    /* ========================================================================== 
     */
    typedef struct {
        CSL_OpenMode openMode;
        CSL_Uid uid;
        CSL_Xio xio;
        CSL_SimcopRegsOvly regs;
        CSL_SimcopBufsOvly bufs;
        CSL_SimcopNum perNum;
    } CSL_SimcopObj;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopHandle - Pointer to CSL_SimcopObj
     */
    /* ========================================================================== 
     */
    typedef CSL_SimcopObj *CSL_SimcopHandle;

    /* ========================================================================== 
     */
    /* 
     * Macros
     */
    /* ========================================================================== 
     */
#define SIMCOP_IRQ_0 0
#define SIMCOP_IRQ_1 1
#define SIMCOP_IRQ_2 2
#define SIMCOP_IRQ_3 3
#define MAX_NUM_OF_SIMCOP_IRQ_LINES 4

#define HWSEQ_STEP_0 0
#define HWSEQ_STEP_1 1
#define HWSEQ_STEP_2 2
#define HWSEQ_STEP_3 3
#define MAX_NUM_OF_HWSEQ_STEPS 4

    /* 
     * Enums grouping Field value Tokens
     */

    /* ========================================================================== 
     */
/** CSL_IrqStatusRawType  describes the possible values for all fields of STATUS_RAW register.  The following tokens 
* are directly mapped 
* 
* STATUS_RAW 
*     CPU_PROC_START_IRQ Tokens
*     SIMCOP_DMA_IRQ1 Tokens
*     MTCR2OCP_ERR_IRQ Tokens
*     OCP_ERR_IRQ Tokens
*     VLCDJ_DECODE_ERR_IRQ Tokens
*     DONE_IRQ Tokens
*     STEP3_IRQ Tokens
*     STEP2_IRQ Tokens
*     STEP1_IRQ Tokens
*     STEP0_IRQ Tokens
*     LDC_BLOCK_IRQ Tokens
*     ROT_A_IRQ Tokens
*     IMX_B_IRQ Tokens
*     IMX_A_IRQ Tokens
*     NSF_IRQ_IRQ Tokens
*     VLCDJ_BLOC_IRQ Tokens
*     DCT_IRQ Tokens
*     LDC_FRAME_IRQ Tokens
*     SIMCOP_DMA_IRQ0 Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        IRQSTATUSRAW_NO_ACTION = 0,
        IRQSTATUSRAW_NO_EVENT_PENDING = 0,
        IRQSTATUSRAW_EVENT_PENDING = 1,
        IRQSTATUSRAW_SET_EVENT = 1
    } CSL_IrqStatusRawType;

    /* ========================================================================== 
     */
/** CSL_IrqStatusType  describes the possible values for all fields of STATUS register.  The following tokens 
* are directly mapped 
* 
* STATUS 
*     CPU_PROC_START_IRQ Tokens
*     SIMCOP_DMA_IRQ1 Tokens    
*     MTCR2OCP_ERR_IRQ Tokens    
*     OCP_ERR_IRQ Tokens    
*     VLCDJ_DECODE_ERR_IRQ Tokens    
*     DONE_IRQ Tokens    
*     STEP3_IRQ Tokens    
*     STEP2_IRQ Tokens    
*     STEP1_IRQ Tokens    
*     STEP0_IRQ Tokens    
*     LDC_BLOCK_IRQ Tokens    
*     ROT_A_IRQ Tokens    
*     IMX_B_IRQ Tokens    
*     IMX_A_IRQ Tokens    
*     NSF_IRQ_IRQ Tokens    
*     VLCDJ_BLOC_IRQ Tokens    
*     DCT_IRQ Tokens    
*     LDC_FRAME_IRQ Tokens    
*     SIMCOP_DMA_IRQ0 Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        IRQSTATUS_NO_ACTION = 0,
        IRQSTATUS_NO_ENABLED_EVENT_PENDING = 0,
        IRQSTATUS_EVENT_PENDING = 1,
        IRQSTATUS_CLEAR_RAW_EVENT = 1
    } CSL_IrqStatusType;

    /* ========================================================================== 
     */
/** CSL_IrqEnableSetType  describes the possible values for all fields of ENABLE_SET register.  The following tokens 
* are directly mapped 
* 
* ENABLE_SET 
*     CPU_PROC_START_IRQ Tokens
*     SIMCOP_DMA_IRQ1 Tokens
*     MTCR2OCP_ERR_IRQ Tokens
*     OCP_ERR_IRQ Tokens
*     VLCDJ_DECODE_ERR_IRQ Tokens
*     DONE_IRQ Tokens
*     STEP3_IRQ Tokens
*     STEP2_IRQ Tokens
*     STEP1_IRQ Tokens
*     STEP0_IRQ Tokens
*     LDC_BLOCK_IRQ Tokens
*     ROT_A_IRQ Tokens
*     IMX_B_IRQ Tokens
*     IMX_A_IRQ Tokens
*     NSF_IRQ_IRQ Tokens
*     VLCDJ_BLOC_IRQ Tokens
*     DCT_IRQ Tokens
*     LDC_FRAME_IRQ Tokens
*     SIMCOP_DMA_IRQ0 Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        IRQENABLESET_NO_ACTION = 0,
        IRQENABLESET_INTERRUPT_DISABLED = 0,
        IRQENABLESET_INTERRUPT_ENABLED = 1,
        IRQENABLESET_ENABLE_INTERRUPT = 1
    } CSL_IrqEnableSetType;

    /* ========================================================================== 
     */
/** CSL_IrqEnableClearType  describes the possible values for all fields of ENABLE_CLR register.  The following tokens 
* are directly mapped 
* 
* ENABLE_CLR 
*     CPU_PROC_START_IRQ Tokens
*     SIMCOP_DMA_IRQ1 Tokens
*     MTCR2OCP_ERR_IRQ Tokens
*     OCP_ERR_IRQ Tokens
*     VLCDJ_DECODE_ERR_IRQ Tokens
*     DONE_IRQ Tokens
*     STEP3_IRQ Tokens
*     STEP2_IRQ Tokens
*     STEP1_IRQ Tokens
*     STEP0_IRQ Tokens
*     LDC_BLOCK_IRQ Tokens
*     ROT_A_IRQ Tokens
*     IMX_B_IRQ Tokens
*     IMX_A_IRQ Tokens
*     NSF_IRQ_IRQ Tokens
*     VLCDJ_BLOC_IRQ Tokens
*     DCT_IRQ Tokens
*     LDC_FRAME_IRQ Tokens
*     SIMCOP_DMA_IRQ0 Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        IRQENABLECLEAR_NO_ACTION = 0,
        IRQENABLECLEAR_INTERRUPT_DISABLED = 0,
        IRQENABLECLEAR_INTERRUPT_ENABLED = 1,
        IRQENABLECLEAR_DISABLE_INTERRUPT = 1
    } CSL_IrqEnableClearType;

    /* ========================================================================== 
     */
/** CSL_DmaOfstType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_CTRL 
*    DMA_OFST Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        DMAOFST_ABCDEFGH = CSL_SIMCOP_STEP_CTRL_DMA_OFST_ABCDEFGH,
        DMAOFST_BCDEFGHA = CSL_SIMCOP_STEP_CTRL_DMA_OFST_BCDEFGHA,
        DMAOFST_CDEFGHAB = CSL_SIMCOP_STEP_CTRL_DMA_OFST_CDEFGHAB,
        DMAOFST_DEFGHABC = CSL_SIMCOP_STEP_CTRL_DMA_OFST_DEFGHABC,
        DMAOFST_EFGHABCD = CSL_SIMCOP_STEP_CTRL_DMA_OFST_EFGHABCD,
        DMAOFST_FGHABCDE = CSL_SIMCOP_STEP_CTRL_DMA_OFST_FGHABCDE,
        DMAOFST_GHABCDEF = CSL_SIMCOP_STEP_CTRL_DMA_OFST_GHABCDEF,
        DMAOFST_HABCDEFG = CSL_SIMCOP_STEP_CTRL_DMA_OFST_HABCDEFG
    } CSL_DmaOfstType;

    /* ========================================================================== 
     */
/** CSL_RotOutLdcOutOfstType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_CTRL 
*     ROT_O_OFST Tokens
* STEP_CTRL2
*     LDC_O_OFST Tokens
* HWSEQ_STEP_CTRL_OVERRIDE
*     ROT_O_OFST_OVR Tokens
* HWSEQ_STEP_CTRL2_OVERRIDE
*     LDC_O_OFST_OVR Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        ROTOUTLDCOUT_EFGH = CSL_SIMCOP_STEP_CTRL_ROT_O_OFST_EFGH,
        ROTOUTLDCOUT_FGHE = CSL_SIMCOP_STEP_CTRL_ROT_O_OFST_FGHE,
        ROTOUTLDCOUT_GHEF = CSL_SIMCOP_STEP_CTRL_ROT_O_OFST_GHEF,
        ROTOUTLDCOUT_HEFG = CSL_SIMCOP_STEP_CTRL_ROT_O_OFST_HEFG
    } CSL_RotOutLdcOutOfstType;

    /* ========================================================================== 
     */
/** CSL_RotInOfstType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_CTRL 
*    ROT_I_OFST Tokens
* HWSEQ_STEP_CTRL_OVERRIDE
*     ROT_I_OFST_OVR Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        ROTIN_ABCD = CSL_SIMCOP_STEP_CTRL_ROT_I_OFST_ABCD,
        ROTIN_BCDA = CSL_SIMCOP_STEP_CTRL_ROT_I_OFST_BCDA,
        ROTIN_CDAB = CSL_SIMCOP_STEP_CTRL_ROT_I_OFST_CDAB,
        ROTIN_DABC = CSL_SIMCOP_STEP_CTRL_ROT_I_OFST_DABC
    } CSL_RotInOfstType;

    /* ========================================================================== 
     */
/** CSL_DctSNsfOfstType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_CTRL 
*    DCT_S_OFST Tokens
* STEP_CTRL2
*     NSF2_IO_OFST Tokens
* HWSEQ_STEP_CTRL2_OVERRIDE
*     NSF2_IO_OFST_OVR Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        DCTSNSF_EF = CSL_SIMCOP_STEP_CTRL_DCT_S_OFST_EF,
        DCTSNSF_FG = CSL_SIMCOP_STEP_CTRL_DCT_S_OFST_FG,
        DCTSNSF_GH = CSL_SIMCOP_STEP_CTRL_DCT_S_OFST_GH,
        DCTSNSF_HE = CSL_SIMCOP_STEP_CTRL_DCT_S_OFST_HE
    } CSL_DctSNsfOfstType;

    /* ========================================================================== 
     */
/** CSL_DctFVlcdjOfstType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_CTRL 
*    DCT_F_OFST Tokens
*     VLCDJ_IO_OFST Tokens
* HWSEQ_STEP_CTRL_OVERRIDE
*     DCT_F_OFST_OVR Tokens
*     VLCDJ_IO_OFST_OVR Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        DCTFVLCDJ_ABCD = CSL_SIMCOP_STEP_CTRL_DCT_F_OFST_ABCD,
        DCTFVLCDJ_BCDG = CSL_SIMCOP_STEP_CTRL_DCT_F_OFST_BCDG,
        DCTFVLCDJ_CDGH = CSL_SIMCOP_STEP_CTRL_DCT_F_OFST_CDGH,
        DCTFVLCDJ_DGHA = CSL_SIMCOP_STEP_CTRL_DCT_F_OFST_DGHA,
        DCTFVLCDJ_GHAB = CSL_SIMCOP_STEP_CTRL_DCT_F_OFST_GHAB,
        DCTFVLCDJ_HABC = CSL_SIMCOP_STEP_CTRL_DCT_F_OFST_HABC
    } CSL_DctFVlcdjOfstType;

    /* ========================================================================== 
     */
/** CSL_ImxDOfstType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_CTRL
*     IMX_B_D_OFST Tokens
*    IMX_A_D_OFST Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        IMXD_ABCD = CSL_SIMCOP_STEP_CTRL_IMX_B_D_OFST_ABCD,
        IMXD_CDEF = CSL_SIMCOP_STEP_CTRL_IMX_B_D_OFST_CDEF,
        IMXD_EFGH = CSL_SIMCOP_STEP_CTRL_IMX_B_D_OFST_EFGH,
        IMXD_GHAB = CSL_SIMCOP_STEP_CTRL_IMX_B_D_OFST_GHAB
    } CSL_ImxDOfstType;

    /* ========================================================================== 
     */
/** CSL_ImxDOfstOvrType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HWSEQ_STEP_CTRL_OVERRIDE
*    IMX_B_D_OFST_OVR Tokens
*    IMX_A_D_OFST_OVR Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        IMXDOVR_ABCDEFGH =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_IMX_B_D_OFST_OVR_ABCDEFGH,
        IMXDOVR_CDEFGHAB =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_IMX_B_D_OFST_OVR_CDEFGHAB,
        IMXDOVR_EFGHABCD =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_IMX_B_D_OFST_OVR_EFGHABCD,
        IMXDOVR_GHABCDEF =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_IMX_B_D_OFST_OVR_GHABCDEF
    } CSL_ImxDOfstOvrType;

    /* ========================================================================== 
     */
/** CSL_NextStepType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_CTRL
*     NEXT Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        NEXTSTEP_0 = CSL_SIMCOP_STEP_CTRL_NEXT_STEP_0,
        NEXTSTEP_1 = CSL_SIMCOP_STEP_CTRL_NEXT_STEP_1,
        NEXTSTEP_2 = CSL_SIMCOP_STEP_CTRL_NEXT_STEP_2,
        NEXTSTEP_3 = CSL_SIMCOP_STEP_CTRL_NEXT_STEP_3
    } CSL_NextStepType;

    /* ========================================================================== 
     */
/** CSL_DmaSyncType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_CTRL
*     DMA_SYNC Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        DMASYNC_DISABLED = CSL_SIMCOP_STEP_CTRL_DMA_SYNC_DISABLED,
        DMASYNC_CH01 = CSL_SIMCOP_STEP_CTRL_DMA_SYNC_CH_0_1,
        DMASYNC_CH012 = CSL_SIMCOP_STEP_CTRL_DMA_SYNC_CH_0_1_2,
        DMASYNC_CH0123 = CSL_SIMCOP_STEP_CTRL_DMA_SYNC_CH_0_1_2_3,
        DMASYNC_CH0 = CSL_SIMCOP_STEP_CTRL_DMA_SYNC_CH_0,
        DMASYNC_CH1 = CSL_SIMCOP_STEP_CTRL_DMA_SYNC_CH_1,
        DMASYNC_CH2 = CSL_SIMCOP_STEP_CTRL_DMA_SYNC_CH_2,
        DMASYNC_CH3 = CSL_SIMCOP_STEP_CTRL_DMA_SYNC_CH_3
    } CSL_DmaSyncType;

    /* ========================================================================== 
     */
/** CSL_SyncType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_CTRL
*    CPU_SYNC Tokens
*    ROT_A_SYNC Tokens
*    NSF_SYNC Tokens
*    VLCDJ_SYNC Tokens
*    DCT_SYNC Tokens
*    LDC_SYNC Tokens
* STEP_IMX_CTRL
*    IMX_B_SYNC Tokens
*    IMX_A_SYNC Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        SYNC_DISABLED = CSL_SIMCOP_STEP_CTRL_CPU_SYNC_DISABLED,
        SYNC_ENABLED = CSL_SIMCOP_STEP_CTRL_CPU_SYNC_ENABLED
    } CSL_SyncType;

    /* ========================================================================== 
     */
/** CSL_BufSwitchImgABCDCoeffAType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_SWITCH
*    IMBUFF_A Tokens
*    IMBUFF_B Tokens
*    IMBUFF_C Tokens
*    IMBUFF_D Tokens
* STEP_CTRL2
*    COEFF_A Tokens
* HWSEQ_STEP_SWITCH_OVERRIDE
*    IMBUFF_D_OVR Tokens
*    IMBUFF_C_OVR Tokens
*    IMBUFF_B_OVR Tokens
*    IMBUFF_A_OVR Tokens
* HWSEQ_STEP_CTRL2_OVERRIDE
*    COEFF_A_OVR Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        BUFSWITCHIMGABCDCOEFFA_COP_BUS =
            CSL_SIMCOP_STEP_SWITCH_IMBUFF_A_COP_BUS,
        BUFSWITCHIMGABCDCOEFFA_SIMCOP_DMA =
            CSL_SIMCOP_STEP_SWITCH_IMBUFF_A_SIMCOP_DMA,
        BUFSWITCHIMGABCDCOEFFA_IMXA_IMBUFF =
            CSL_SIMCOP_STEP_SWITCH_IMBUFF_A_IMXA_IMBUFF,
        BUFSWITCHIMGABCDCOEFFA_IMXB_IMBUFF =
            CSL_SIMCOP_STEP_SWITCH_IMBUFF_A_IMXB_IMBUFF,
        BUFSWITCHIMGABCDCOEFFA_VLCDJ_IO =
            CSL_SIMCOP_STEP_SWITCH_IMBUFF_A_VLCDJ_IO,
        BUFSWITCHIMGABCDCOEFFA_DCT_F = CSL_SIMCOP_STEP_SWITCH_IMBUFF_A_DCT_F,
        BUFSWITCHIMGABCDCOEFFA_ROT_A_I =
            CSL_SIMCOP_STEP_SWITCH_IMBUFF_A_ROT_A_I,
        BUFSWITCHIMGABCDCOEFFA_RESERVED =
            CSL_SIMCOP_STEP_SWITCH_IMBUFF_A_RESERVED
    } CSL_BufSwitchImgABCDCoeffAType;

    /* ========================================================================== 
     */
/** CSL_BufSwitchImgEFType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_SWITCH
*    IMBUFF_E Tokens
*    IMBUFF_F Tokens
* HWSEQ_STEP_SWITCH_OVERRIDE
*    IMBUFF_E_OVR Tokens
*    IMBUFF_F_OVR Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        BUFSWITCHIMGEF_COP_BUS = CSL_SIMCOP_STEP_SWITCH_IMBUFF_E_COP_BUS,
        BUFSWITCHIMGEF_SIMCOP_DMA = CSL_SIMCOP_STEP_SWITCH_IMBUFF_E_SIMCOP_DMA,
        BUFSWITCHIMGEF_IMX_A = CSL_SIMCOP_STEP_SWITCH_IMBUFF_E_IMX_A,
        BUFSWITCHIMGEF_IMX_B = CSL_SIMCOP_STEP_SWITCH_IMBUFF_E_IMX_B,
        BUFSWITCHIMGEF_DCT_S = CSL_SIMCOP_STEP_SWITCH_IMBUFF_E_DCT_S,
        BUFSWITCHIMGEF_NSF_IO = CSL_SIMCOP_STEP_SWITCH_IMBUFF_E_NSF_IO,
        BUFSWITCHIMGEF_LDC_O = CSL_SIMCOP_STEP_SWITCH_IMBUFF_E_LDC_O,
        BUFSWITCHIMGEF_ROT_A_O = CSL_SIMCOP_STEP_SWITCH_IMBUFF_E_ROT_A_O
    } CSL_BufSwitchImgEFType;

    /* ========================================================================== 
     */
/** CSL_BufSwitchImgGHType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_SWITCH
*    IMBUFF_G Tokens
*    IMBUFF_H Tokens
* HWSEQ_STEP_SWITCH_OVERRIDE
*    IMBUFF_H_OVR Tokens
*    IMBUFF_G_OVR Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        BUFSWITCHIMGGH_COP_BUS = CSL_SIMCOP_STEP_SWITCH_IMBUFF_G_COP_BUS,
        BUFSWITCHIMGGH_SIMCOP_DMA = CSL_SIMCOP_STEP_SWITCH_IMBUFF_G_SIMCOP_DMA,
        BUFSWITCHIMGGH_IMX_A = CSL_SIMCOP_STEP_SWITCH_IMBUFF_G_IMX_A,
        BUFSWITCHIMGGH_IMX_B = CSL_SIMCOP_STEP_SWITCH_IMBUFF_G_IMX_B,
        BUFSWITCHIMGGH_VLCDJ_IO = CSL_SIMCOP_STEP_SWITCH_IMBUFF_G_VLCDJ_IO,
        BUFSWITCHIMGGH_DCT_S = CSL_SIMCOP_STEP_SWITCH_IMBUFF_G_DCT_S,
        BUFSWITCHIMGGH_DCT_F = CSL_SIMCOP_STEP_SWITCH_IMBUFF_G_DCT_F,
        BUFSWITCHIMGGH_ROT_A_O = CSL_SIMCOP_STEP_SWITCH_IMBUFF_G_ROT_A_O,
        BUFSWITCHIMGGH_NSF_IO = CSL_SIMCOP_STEP_SWITCH_IMBUFF_G_NSF_IO,
        BUFSWITCHIMGGH_LDC_O = CSL_SIMCOP_STEP_SWITCH_IMBUFF_G_LDC_O
    } CSL_BufSwitchImgGHType;

    /* ========================================================================== 
     */
/** CSL_BufSwitchCoeffBType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* STEP_CTRL2
*    COEFF_B Tokens
* HWSEQ_STEP_CTRL2_OVERRIDE
*    COEFF_B_OVR Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        BUFSWITCHCOEFFB_COPROCESSOR_BUS =
            CSL_SIMCOP_STEP_CTRL2_COEFF_B_COPROCESSOR_BUS,
        BUFSWITCHCOEFFB_SIMCOP_DMA = CSL_SIMCOP_STEP_CTRL2_COEFF_B_SIMCOP_DMA,
        BUFSWITCHCOEFFB_IMX_A = CSL_SIMCOP_STEP_CTRL2_COEFF_B_IMX_A,
        BUFSWITCHCOEFFB_IMX_B = CSL_SIMCOP_STEP_CTRL2_COEFF_B_IMX_B,
        BUFSWITCHCOEFFB_VLCDJ_IO = CSL_SIMCOP_STEP_CTRL2_COEFF_B_VLCDJ_IO,
        BUFSWITCHCOEFFB_DCT_F = CSL_SIMCOP_STEP_CTRL2_COEFF_B_DCT_F,
        BUFSWITCHCOEFFB_ROT_A_O = CSL_SIMCOP_STEP_CTRL2_COEFF_B_ROT_A_O,
        BUFSWITCHCOEFFB_RESERVED = CSL_SIMCOP_STEP_CTRL2_COEFF_B_RESERVED
    } CSL_BufSwitchCoeffBType;

    /* ========================================================================== 
     */
/** CSL_TriggerType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HWSEQ_STEP_CTRL_OVERRIDE
*    ROT_A_TRIGGER Tokens
*    NSF_TRIGGER Tokens
*    VLCDJ_TRIGGER Tokens
*    DCT_TRIGGER Tokens
*    LDC_TRIGGER Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        TRIGGER_WRITE_0 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_A_TRIGGER_WRITE_0,
        TRIGGER_READ_0 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_A_TRIGGER_READ_0,
        TRIGGER_READ_1 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_A_TRIGGER_READ_1,
        TRIGGER_WRITE_1 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_A_TRIGGER_WRITE_1
    } CSL_TriggerType;

    /* ========================================================================== 
     */
/** CSL_DMATriggerType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HWSEQ_STEP_CTRL_OVERRIDE
*     DMA_TRIGGER Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        DMATRIGGER_WRITE_0 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_WRITE_0,
        DMATRIGGER_READ_0 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_READ_0,
        DMATRIGGER_READ_1 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_READ_1,
        DMATRIGGER_WRITE_1 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_WRITE_1,
        DMATRIGGER_WRITE_2 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_WRITE_2,
        DMATRIGGER_READ_2 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_READ_2,
        DMATRIGGER_READ_3 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_READ_3,
        DMATRIGGER_WRITE_3 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_WRITE_3,
        DMATRIGGER_READ_4 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_READ_4,
        DMATRIGGER_WRITE_4 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_WRITE_4,
        DMATRIGGER_WRITE_5 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_WRITE_5,
        DMATRIGGER_READ_5 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_READ_5,
        DMATRIGGER_READ_6 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_READ_6,
        DMATRIGGER_WRITE_6 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_WRITE_6,
        DMATRIGGER_WRITE_7 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_WRITE_7,
        DMATRIGGER_READ_7 =
            CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER_READ_7
    } CSL_DMATriggerType;

    /* ========================================================================== 
     */
/** CSL_RevSchemeType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HL_REVISION
*     SCHEME Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        REVSCHEME_WTBU = CSL_SIMCOP_HL_REVISION_SCHEME_WTBU,
        REVSCHEME_HIGHLANDER = CSL_SIMCOP_HL_REVISION_SCHEME_HIGHLANDER
    } CSL_RevSchemeType;

    /* ========================================================================== 
     */
/** CSL_RevCustomType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HL_REVISION
*     CUSTOM Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        REVCUSTOM_STANDARD = CSL_SIMCOP_HL_REVISION_CUSTOM_STANDARD
    } CSL_RevCustomType;

    /* ========================================================================== 
     */
/** CSL_LdcRespFIFOSizeType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HL_HWINFO
*     LDCR_RESP_FIFO Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        LDCRESPFIFO_8X128BITS = CSL_SIMCOP_HL_HWINFO_LDCR_RESP_FIFO_8X128BITS,
        LDCRESPFIFO_16X128BITS = CSL_SIMCOP_HL_HWINFO_LDCR_RESP_FIFO_16X128BITS,
        LDCRESPFIFO_32X128BITS = CSL_SIMCOP_HL_HWINFO_LDCR_RESP_FIFO_32X128BITS,
        LDCRESPFIFO_64X128BITS = CSL_SIMCOP_HL_HWINFO_LDCR_RESP_FIFO_64X128BITS,
        LDCRESPFIFO_128X128BITS =
            CSL_SIMCOP_HL_HWINFO_LDCR_RESP_FIFO_128X128BITS,
        LDCRESPFIFO_256X128BITS =
            CSL_SIMCOP_HL_HWINFO_LDCR_RESP_FIFO_256X128BITS
    } CSL_LdcRespFIFOSizeType;

    /* ========================================================================== 
     */
/** CSL_NoOfImgBufsType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HL_HWINFO
*     IMAGE_BUFFERS Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        NUMOFIMGBUF_4 = CSL_SIMCOP_HL_HWINFO_IMAGE_BUFFERS_4BUFFERS,
        NUMOFIMGBUF_8 = CSL_SIMCOP_HL_HWINFO_IMAGE_BUFFERS_8BUFFERS
    } CSL_NoOfImgBufsType;

    /* ========================================================================== 
     */
/** CSL_EnableType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HL_HWINFO
*     ROT_A_ENABLE Tokens 
*     IMX_B_ENABLE Tokens
*     IMX_A_ENABLE Tokens
*     NSF_ENABLE Tokens
*     VLCDJ_ENABLE Tokens
*     DCT_ENABLE Tokens
*     LDC_ENABLE Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        ABSENT = CSL_SIMCOP_HL_HWINFO_ROT_A_ENABLE_ABSENT,
        PRESENT = CSL_SIMCOP_HL_HWINFO_ROT_A_ENABLE_PRESENT
    } CSL_EnableType;

    /* ========================================================================== 
     */
/** CSL_StandbyType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HL_SYSCONFIG
*     STANDBYMODE Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        STANDBY_FORCE = CSL_SIMCOP_HL_SYSCONFIG_STANDBYMODE_FORCE_STANDBY,
        STANDBY_DISABLE = CSL_SIMCOP_HL_SYSCONFIG_STANDBYMODE_NO_STANDBY,
        STANDBY_SMART = CSL_SIMCOP_HL_SYSCONFIG_STANDBYMODE_SMART_STANDBY
    } CSL_StandbyType;

    /* ========================================================================== 
     */
/** CSL_SoftResetType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HL_SYSCONFIG
*     SOFTRESET Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        SOFTRESET_DONE =
            CSL_SIMCOP_HL_SYSCONFIG_SOFTRESET_RESET_DONE_NO_PENDING_ACTION,
        SOFTRESET_NO_ACTION = CSL_SIMCOP_HL_SYSCONFIG_SOFTRESET_NO_ACTION,
        SOFTRESET_INITIATE =
            CSL_SIMCOP_HL_SYSCONFIG_SOFTRESET_INITIATE_SOFTWARE_RESET,
        SOFTRESET_ONGOING = CSL_SIMCOP_HL_SYSCONFIG_SOFTRESET_RESET_ONGOING
    } CSL_SoftResetType;

    /* ========================================================================== 
     */
/** CSL_IrqEOIType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HL_IRQ_EOI
*     LINE_NUMBER Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        IRQEOI_NO_EOI_MEMORY = CSL_SIMCOP_HL_IRQ_EOI_LINE_NUMBER_NO_EOI_MEMORY,
        IRQEOI_LINE_0 = CSL_SIMCOP_HL_IRQ_EOI_LINE_NUMBER_EOI_OUT_LINE_0,
        IRQEOI_LINE_1 = CSL_SIMCOP_HL_IRQ_EOI_LINE_NUMBER_EOI_OUT_LINE_1,
        IRQEOI_LINE_2 = CSL_SIMCOP_HL_IRQ_EOI_LINE_NUMBER_EOI_OUT_LINE_2,
        IRQEOI_LINE_3 = CSL_SIMCOP_HL_IRQ_EOI_LINE_NUMBER_EOI_OUT_LINE_3
    } CSL_IrqEOIType;

    /* ========================================================================== 
     */
/** CSL_LdcBurstBreakType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* CTRL
*     LDC_R_BURST_BREAK Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        LDCBURSTBREAK_YES = CSL_SIMCOP_CTRL_LDC_R_BURST_BREAK_YES,
        LDCBURSTBREAK_NO = CSL_SIMCOP_CTRL_LDC_R_BURST_BREAK_NO
    } CSL_LdcBurstBreakType;

    /* ========================================================================== 
     */
/** CSL_LdcMaxBurstLengthType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* CTRL
*     LDC_R_MAX_BURST_LENGTH Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        LDCMAXBURSTLENGTH_8X128 = CSL_SIMCOP_CTRL_LDC_R_MAX_BURST_LENGTH_8X128,
        LDCMAXBURSTLENGTH_6X128 = CSL_SIMCOP_CTRL_LDC_R_MAX_BURST_LENGTH_6X128,
        LDCMAXBURSTLENGTH_4X128 = CSL_SIMCOP_CTRL_LDC_R_MAX_BURST_LENGTH_4X128,
        LDCMAXBURSTLENGTH_2X128 = CSL_SIMCOP_CTRL_LDC_R_MAX_BURST_LENGTH_2X128
    } CSL_LdcMaxBurstLengthType;

    /* ========================================================================== 
     */
/** CSL_ImxCmdBAccType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* CTRL
*     IMX_B_CMD Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        IMXCMDBACC_COPROCESSOR_BUS = CSL_SIMCOP_CTRL_IMX_B_CMD_COPROCESSOR_BUS,
        IMXCMDBACC_IMX_B_IR_IW = CSL_SIMCOP_CTRL_IMX_B_CMD_IMX_B_IR_IW
    } CSL_ImxCmdBAccType;

    /* ========================================================================== 
     */
/** CSL_ImxCmdAAccType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* CTRL
*     IMX_A_CMD Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        IMXCMDAACC_COPROCESSOR_BUS = CSL_SIMCOP_CTRL_IMX_A_CMD_COPROCESSOR_BUS,
        IMXCMDAACC_IMX_A_IR_IW = CSL_SIMCOP_CTRL_IMX_A_CMD_IMX_A_IR_IW,
        IMXCMDAACC_IMX_B_IR_IW = CSL_SIMCOP_CTRL_IMX_A_CMD_IMX_B_IR_IW
    } CSL_ImxCmdAAccType;

    /* ========================================================================== 
     */
/** CSL_HuffQuantAccType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* CTRL
*     HUFF Tokens    
*     QUANT Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        HUFFQUANTACC_COPROCESSOR = CSL_SIMCOP_CTRL_HUFF_COPROCESSOR,
        HUFFQUANTACC_VLCDJ_READ = CSL_SIMCOP_CTRL_HUFF_VLCDJ_HUFFMAN_TABLE_READ
    } CSL_HuffQuantAccType;

    /* ========================================================================== 
     */
/** CSL_LdcLutAccType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* CTRL
*     LDC_LUT Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        LDCLUTACC_COPBUS = CSL_SIMCOP_CTRL_LDC_LUT_COPROCESSOR_BUS,
        LDCLUTACC_LDC = CSL_SIMCOP_CTRL_LDC_LUT_LDC_ACCESS_TO_LUT_ENABLED
    } CSL_LdcLutAccType;

    /* ========================================================================== 
     */
/** CSL_LdcInputMemType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* CTRL
*     LDC_INPUT Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        LDCINPUTMEM_ABSENT = CSL_SIMCOP_CTRL_LDC_INPUT_NO_INPUT_MEM,
        LDCINPUTMEM_IMGBUF_AB = CSL_SIMCOP_CTRL_LDC_INPUT_USE_IMAGE_BUF_A_B,
        LDCINPUTMEM_IMGBUF_ABCD =
            CSL_SIMCOP_CTRL_LDC_INPUT_USE_IMAGE_BUF_A_B_C_D,
        LDCINPUTMEM_LDCPRIV = CSL_SIMCOP_CTRL_LDC_INPUT_USE_LDC_PRIV_INPUT_MEM
    } CSL_LdcInputMemType;

    /* ========================================================================== 
     */
/** CSL_NsfWmemType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* CTRL
*     NSF_WMEM Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        NSFWMEM_ABSENT = CSL_SIMCOP_CTRL_NSF_WMEM_NO_INPUT_MEM,
        NSFWMEM_IMXACOEFF = CSL_SIMCOP_CTRL_NSF_WMEM_USE_IMX_A_COEFF_MEM,
        NSFWMEM_IMGBUF_AB = CSL_SIMCOP_CTRL_NSF_WMEM_USE_IMAGE_BUF_A_B,
        NSFWMEM_IMGBUF_ABCD = CSL_SIMCOP_CTRL_NSF_WMEM_USE_IMAGE_BUF_A_B_C_D
    } CSL_NsfWmemType;

    /* ========================================================================== 
     */
/** CSL_IrqModeType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* CTRL
*     IRQ3_MODE Tokens    
*     IRQ2_MODE Tokens
*     IRQ1_MODE Tokens
*     IRQ0_MODE Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        IRQMODE_ONEOF = CSL_SIMCOP_CTRL_IRQ3_MODE_ONE_OF_ENABLED_EVENTS_PENDING,
        IRQMODE_ALLOF = CSL_SIMCOP_CTRL_IRQ3_MODE_ALL_OF_ENABLED_EVENTS_PENDING
    } CSL_IrqModeType;

    /* ========================================================================== 
     */
/** CSL_ClkCtrlType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* CLKCTRL
*    ROT_A_CLK Tokens
*    IMX_B_CLK Tokens
*    IMX_A_CLK Tokens
*    NSF2_CLK Tokens
*    VLCDJ_CLK Tokens
*    DCT_CLK Tokens
*    LDC_CLK Tokens
*    DMA_CLK Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */

    typedef enum {
        CLKCTRL_SHUTDOWN_REQ =
            CSL_SIMCOP_CLKCTRL_ROT_A_CLK_SUBMODULE_SHUTDOWN_REQ,
        CLKCTRL_IS_OFF = CSL_SIMCOP_CLKCTRL_ROT_A_CLK_SUBMODULE_IS_OFF,
        CLKCTRL_IS_ON = CSL_SIMCOP_CLKCTRL_ROT_A_CLK_SUBMODULE_IS_ON,
        CLKCTRL_ENABLE_REQ = CSL_SIMCOP_CLKCTRL_ROT_A_CLK_SUBMODULE_ENABLE_REQ
    } CSL_ClkCtrlType;

    /* ========================================================================== 
     */
/** CSL_CpuStatusType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HWSEQ_CTRL
*     CPU_PROC_DONE Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        CPUSTATUS_NO_EFFECT = CSL_SIMCOP_HWSEQ_CTRL_CPU_PROC_DONE_NO_EFFECT,
        CPUSTATUS_CPUPROC_DONE =
            CSL_SIMCOP_HWSEQ_CTRL_CPU_PROC_DONE_CPUPROC_DONE
    } CSL_CpuStatusType;

    /* ========================================================================== 
     */
/** CSL_BBMStatusType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HWSEQ_CTRL
*     BBM_STATUS Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef CSL_HwStatusType CSL_BBMStatusType;

    /* ========================================================================== 
     */
/** CSL_BSAccType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
*
* HWSEQ_CTRL
*     BITSTREAM Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        BSACC_COP_COP = CSL_SIMCOP_HWSEQ_CTRL_BITSTREAM_COP_COP,
        BSACC_DMA_DMA = CSL_SIMCOP_HWSEQ_CTRL_BITSTREAM_DMA_DMA,
        BSACC_VLCDB_VLCDB = CSL_SIMCOP_HWSEQ_CTRL_BITSTREAM_VLCDB_VLCDB,
        BSACC_DMA_VLCDB = CSL_SIMCOP_HWSEQ_CTRL_BITSTREAM_DMA_VLCDB,
        BSACC_VLCDB_DMA = CSL_SIMCOP_HWSEQ_CTRL_BITSTREAM_VLCDB_DMA,
        BSACC_PINGPONG_ENC = CSL_SIMCOP_HWSEQ_CTRL_BITSTREAM_PINGPONG_ENC,
        BSACC_PINGPONG_DEC = CSL_SIMCOP_HWSEQ_CTRL_BITSTREAM_PINGPONG_DEC
    } CSL_BSAccType;

    /* ========================================================================== 
     */
/** CSL_BSTransferSizeType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HWSEQ_CTRL
*    BITSTR_XFER_SIZE Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        BSTRANSFERSIZE_2048_BYTES =
            CSL_SIMCOP_HWSEQ_CTRL_BITSTR_XFER_SIZE_2048_BYTES,
        BSTRANSFERSIZE_1024_BYTES =
            CSL_SIMCOP_HWSEQ_CTRL_BITSTR_XFER_SIZE_1024_BYTES,
        BSTRANSFERSIZE_512_BYTES =
            CSL_SIMCOP_HWSEQ_CTRL_BITSTR_XFER_SIZE_512_BYTES,
        BSTRANSFERSIZE_256_BYTES =
            CSL_SIMCOP_HWSEQ_CTRL_BITSTR_XFER_SIZE_256_BYTES
    } CSL_BSTransferSizeType;

    /* ========================================================================== 
     */
/** CSL_HwSeqStartStopType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HWSEQ_CTRL
*    HW_SEQ_STOP Tokens    
*    HW_SEQ_START Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        HWSEQSTARTSTOP_NO_EFFECT = CSL_SIMCOP_HWSEQ_CTRL_HW_SEQ_STOP_NO_EFFECT,
        HWSEQSTARTSTOP_DO = CSL_SIMCOP_HWSEQ_CTRL_HW_SEQ_STOP_STOP_SEQ
    } CSL_HwSeqStartStopType;

    /* ========================================================================== 
     */
/** CSL_HwSeqStatusType    describes the possible values for listed fields of listed register.  The following tokens 
* are directly mapped 
* 
* HWSEQ_STATUS
*    STATE Tokens
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef CSL_HwStatusType CSL_HwSeqStatusType;

    /* ========================================================================== 
     */
/** CSL_SeqOvrType    describes the possible values for listed fields of listed register.  There are no tokens defined.
* 
* SIMCOP_HWSEQ_OVERRIDE
*    COEFF_B    COEFF_A    
*    IMBUFF_H    IMBUFF_G    IMBUFF_F        IMBUFF_E    IMBUFF_D    IMBUFF_C    IMBUFF_B    IMBUFF_A
*    LDC_O_OFST_OVR    ROT_O_OFST_OVR    ROT_I_OFST_OVR    NSF_IO_OFST_OVR    
*    DCT_F_OFST_OVR    DCT_S_OFST_OVR    VLCDJ_IO_OFST_OVR    IMX_B_D_OFST_OVR    IMX_A_D_OFST_OVR
* This enumeration type is used by CSL_SimcopHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        HWCTRL = 0,
        SWCTRL = 1
    } CSL_SeqOvrType;

    /* SIMCOP BUFFER */
    typedef enum {
        BITSTREAM_BUFFER,
        HUFFMAN_TABLES,
        IMXB_CMD_MEM,
        IMXA_CMD_MEM,
        QUANTIZATION_TABLE_FOR_ENCODER,
        QUANTIZATION_TABLE_FOR_DECODER,
        LDCLUT,
        IMAGE_BUFFER_A,
        IMAGE_BUFFER_B,
        IMAGE_BUFFER_C,
        IMAGE_BUFFER_D,
        IMAGE_BUFFER_E,
        IMAGE_BUFFER_F,
        IMAGE_BUFFER_G,
        IMAGE_BUFFER_H,
        IMXA_COEFF_MEM,
        IMXB_COEFF_MEM
    } CSL_SimcopBufferType;

    /* 
     * Structures for SimcopHwStatus
     */

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopHwDesignInfo - Simcop Hw Design Info structure.
     *
     * Following Params provide Revision info
     *
     * @param  Scheme  Used to distinguish between old scheme and current
     * @param  Func  Function indicates a software compatible module family
     * @param  RRtl  RTL Version (R), maintained by IP design owner
     * @param  XMajor  Major Revision (X), maintained by IP specification owner
     * @param  YMinor  Minor Revision (Y), maintained by IP specification owner
     * @param  RevCustom  Indicates a special version for a particular device
     *
     * Folowing Params provide Simcop Hw sub-module info
     *
     * @param  LdcFifoSize  Defines the size of the LDC read master response FIFO in words of 128-bits
     * @param  NImgBuf  This parameter defines the image buffer count
     * @param  Rot  The ROT #a module is present when this parameter is set
     * @param  ImxB  The iMX #b module and the CMD#b, COEFF#b memories are present when this parameter is set
     * @param  ImxA  The iMX #a module and the CMD#a, COEFF#a memories are present when this parameter is set
     * @param  Nsf  The NSF2 module is present when this parameter is set
     * @param  Vlcdj  The VLCD module and the QUANT, HUFFMAN, BITSTREAM memories are present when this parameter is set
     * @param  Dct  The DCT module is present when this parameter is set
     * @param  Ldc  The LDC module and the LDC LUT are present when this parameter is set
     */
    /* ========================================================================== 
     */
    typedef struct {
        CSL_RevSchemeType Scheme;
        Bits12 Func;
        Bits5 RRtl;
        Bits3 XMajor;
        Bits6 YMinor;
        CSL_RevCustomType RevCustom;

        CSL_LdcRespFIFOSizeType LdcFifoSize;
        CSL_NoOfImgBufsType NImgBuf;
        CSL_EnableType Rot;
        CSL_EnableType ImxB;
        CSL_EnableType ImxA;
        CSL_EnableType Nsf;
        CSL_EnableType Vlcdj;
        CSL_EnableType Dct;
        CSL_EnableType Ldc;
    } CSL_SimcopHwDesignInfo;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopHwStatus - Simcop Hw Status
     * @param  HwSeqStepCounterCurrent  Current step number
     * @param  HwSeqState  Current state of Hw Sequencer
     * @param  BbmStatus  Status of the Bitstream Buffer Management HW. Used only during automatic mode
     *                    [BITSTREAM=5 or 6]
     */
    /* ========================================================================== 
     */
    typedef struct {
        Uint16 HwSeqStepCounterCurrent;
        CSL_HwSeqStatusType HwSeqState;
        CSL_BBMStatusType BbmStatus;
    } CSL_SimcopHwStatus;

    /* 
     * Simcop Hardware Setup structure and its sub structures
     */

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopIRQReg - Simcop IRQ per event status and control structure.
     * @param  IRQStatusRaw  Per-event raw interrupt status vector. Raw status is set even if event is not enabled. Write 1 
     *                        to set the (raw) status, mostly for debug.
     * @param  IRQStatus  Per-event "enabled" interrupt status vector. Enabled status isn't set unless event is enabled.
     *                     Write 1 to clear the status after interrupt has been serviced (raw status gets cleared, i.e. even if 
     *                     not enabled).
     * @param  IRQEnableSet  Per-event interrupt enable bit vector. Write 1 to set (enable interrupt). Readout equal to 
     *                       corresponding _CLR register.
     * @param  IRQEnableClr  Per-event interrupt enable bit vector. Write 1 to clear (disable interrupt).Readout equal to 
     *                       corresponding _SET register.
     */
    /* ========================================================================== 
     */
    typedef struct {
        CSL_IrqStatusRawType IRQStatusRaw;
        CSL_IrqStatusType IRQStatus;
        CSL_IrqEnableSetType IRQEnableSet;
        CSL_IrqEnableClearType IRQEnableClr;
    } CSL_SimcopIRQReg;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDMAIRQReg - Simcop IRQ per DMA event control structure.
     * @param  IRQEnableSet  Per-event interrupt enable bit vector. Write 1 to set (enable interrupt). Readout equal to 
     *                       corresponding _CLR register.
     * @param  IRQEnableClr  Per-event interrupt enable bit vector. Write 1 to clear (disable interrupt).Readout equal to 
     *                       corresponding _SET register.
     */
    /* ========================================================================== 
     */
    typedef struct {
        CSL_IrqEnableSetType IRQEnableSet;
        CSL_IrqEnableClearType IRQEnableClr;
    } CSL_SimcopDMAIRQReg;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopIRQ - Simcop IRQ events structure.
     *
     * @param  SetupIrq  Specifies if the current IRQ line needs to be configured or not
     * 
     * @param  CpuProcStartIrq  Event triggered by the HW sequencer to instruct the CPU to process a macro block
     *
     * Following Params provide control and status of Event triggered by the SIMCOP DMA. This event is automatically cleared 
     * at SIMCOP level when it is cleared at SIMCOP DMA level Check SIMCOP DMA IRQ registers.
     *
     * @param  SimcopDmaIrq0  
     * @param  SimcopDmaIrq1  
     *
     * @param  LdcBlockIrq  Event triggered by LDC when a macro-block has been processed
     * @param  LdcFrameIrq  Event triggered by LDC when a full frame has been processed
     * @param  NsfIrq  Event triggered by the NSF2 imaging accelerator when processing of a block is done.
     * @param  RotIrq  Event triggered by the ROT #a engine
     * @param  DctIrq  Event triggered when a block has been processed by the DCT module and the filter outcome has been 
     *                 stored to an image buffer.
     * @param  VlcdjBlocIrq  This event is triggered by VLCDJ when a macro-bloc has been processed (encode/decode)
     * @param  ImxAIrq  Event triggered when iMX has executed a SLEEP instruction. 
     * @param  ImxBIrq  Event triggered when iMX has executed a SLEEP instruction. 
     *   
     * Following Params relate to Event triggered when STEP<0-3> is activated by the HW sequencer
     *
     * @param  Step0Irq  Step 0
     * @param  Step1Irq  Step 1
     * @param  Step2Irq  Step 2
     * @param  Step3Irq  Step 3
     *
     * @param  DoneIrq  Event triggered when the HW sequencer finishes the sequence:
     *                    - the sequence step counter has reached the limit 
     *                    - all accelerator and DMA events for the last sequence step have been received.
     *
     * Following Params signal error condition
     *
     * @param  Mtcr2OcpErrIrq  The MTCR2OCP bridge has received an MTC_READ_SOF pulse while it still had outstanding 
     *                         OCP transactions.
     * @param  OcpErrIrq  An OCP error has been received on the SIMCOP master port.
     * @param  VlcdjDecodeErrIrq  A decode error has been signaled by the VLCDJ module
     */
    /* ========================================================================== 
     */
    typedef struct {
        Bool SetupIrq;

        CSL_SimcopIRQReg CpuProcStartIrq;
        CSL_SimcopDMAIRQReg SimcopDmaIrq0;
        CSL_SimcopDMAIRQReg SimcopDmaIrq1;
        CSL_SimcopIRQReg LdcBlockIrq;
        CSL_SimcopIRQReg LdcFrameIrq;
        CSL_SimcopIRQReg NsfIrq;
        CSL_SimcopIRQReg RotIrq;
        CSL_SimcopIRQReg DctIrq;
        CSL_SimcopIRQReg VlcdjBlocIrq;
        CSL_SimcopIRQReg ImxAIrq;
        CSL_SimcopIRQReg ImxBIrq;

        CSL_SimcopIRQReg Step0Irq;
        CSL_SimcopIRQReg Step1Irq;
        CSL_SimcopIRQReg Step2Irq;
        CSL_SimcopIRQReg Step3Irq;
        CSL_SimcopIRQReg DoneIrq;

        CSL_SimcopIRQReg Mtcr2OcpErrIrq;
        CSL_SimcopIRQReg OcpErrIrq;
        CSL_SimcopIRQReg VlcdjDecodeErrIrq;
    } CSL_SimcopIRQ;

    /* ========================================================================== 
     */
    /* 
     * CSL_HwSeqStep - Simcop Hw Sequencer per Step Control structure.
     *
     * @param  SetupStep  Specifies if the current step needs to be configured or not
     *
     * Following Params enable HW synchronization with a sub-module(listed below) so that it can used in the macro-block 
     * pipeline.
     *
     * @param  CpuSync  CPU is enabled to be used for some processing in the pipeline.
     * @param  DmaSync  SIMCOP DMA module
     * @param  RotSync  ROT#A module
     * @param  LdcSync  LDC module
     * @param  NsfSync  NSF module
     * @param  DctSync  DCT module
     * @param  VlcdjSync  VLCDJ module  
     * @param  ImxASync  IMX#A module
     * @param  ImxBSync  IMX#B module
     *
     * Following Params Control sub-module bus(listed below) mapping to Image Buffers
     *
     * @param  DmaOfst  DMA 
     * @param  LdcOOfst  LDC.O 
     * @param  Nsf2IOOfst  NSF.IO
     * @param  RotIOfst  ROT.I
     * @param  RotOOfst  ROT.O
     * @param  DctSOfst  DCT.S
     * @param  DctFOfst  DCT.F
     * @param  VlcdjIOOfst  VLCDJ.IO
     * @param  ImxADataOfst  IMX#A Data
     * @param  ImxBDataOfst  IMX#B Data
     *
     * Following Params switch Image buffer and Coefficient Memory (listed below) connection with various sub-module buses. 
     * The configuration of step #0 is used when HW sequencer is idle.
     *
     * @param  ImbuffA
     * @param  ImbuffB
     * @param  ImbuffC
     * @param  ImbuffD
     * @param  ImbuffE
     * @param  ImbuffF
     * @param  ImbuffG
     * @param  ImbuffH
     * @param  CoeffA
     * @param  CoeffB
     *
     * Following Params are only used when IMX_CTRL.IMX_<A or B>_SYNC=1. It contains the address, in 16-bit words, 
     * of the first instruction iMX will execute when it gets started.
     *
     * @param  ImxAStart  IMX#A program start address
     * @param  ImxBStart  IMX#B program start address
     */
    /* ========================================================================== 
     */
    typedef struct {
        Bool SetupStep;

        CSL_SyncType CpuSync;
        CSL_DmaSyncType DmaSync;
        CSL_SyncType RotSync;
        CSL_SyncType LdcSync;
        CSL_SyncType NsfSync;
        CSL_SyncType DctSync;
        CSL_SyncType VlcdjSync;
        CSL_SyncType ImxASync;
        CSL_SyncType ImxBSync;

        CSL_DmaOfstType DmaOfst;
        CSL_RotOutLdcOutOfstType LdcOOfst;
        CSL_DctSNsfOfstType NsfIOOfst;
        CSL_RotInOfstType RotIOfst;
        CSL_RotOutLdcOutOfstType RotOOfst;
        CSL_DctSNsfOfstType DctSOfst;
        CSL_DctFVlcdjOfstType DctFOfst;
        CSL_DctFVlcdjOfstType VlcdjIOOfst;
        CSL_ImxDOfstType ImxADataOfst;
        CSL_ImxDOfstType ImxBDataOfst;

        CSL_BufSwitchImgABCDCoeffAType ImBuffA;
        CSL_BufSwitchImgABCDCoeffAType ImBuffB;
        CSL_BufSwitchImgABCDCoeffAType ImBuffC;
        CSL_BufSwitchImgABCDCoeffAType ImBuffD;
        CSL_BufSwitchImgEFType ImBuffE;
        CSL_BufSwitchImgEFType ImBuffF;
        CSL_BufSwitchImgGHType ImBuffG;
        CSL_BufSwitchImgGHType ImBuffH;
        CSL_BufSwitchImgABCDCoeffAType CoeffA;
        CSL_BufSwitchCoeffBType CoeffB;

        Bits13 ImxAStart;
        Bits13 ImxBStart;

        CSL_NextStepType Next;
    } CSL_HwSeqStep;

    /* ========================================================================== 
     */
    /* 
     * CSL_HwSeqOvr - Simcop Hw Sequencer Override Control structure. 
     *
     * Following Params select what configuration register(HwSeq STEP registers or HwSeq override register) control a 
     * resource(listed below)
     *
     * @param  ImBuffAOvr  Image Buffer A
     * @param  ImBuffBOvr  Image Buffer B
     * @param  ImBuffCOvr  Image Buffer C
     * @param  ImBuffDOvr  Image Buffer D
     * @param  ImBuffEOvr  Image Buffer E
     * @param  ImBuffFOvr  Image Buffer F
     * @param  ImBuffGOvr  Image Buffer G
     * @param  ImBuffHOvr  Image Buffer H
     * @param  CoeffAOvr  Coefficient Memory A
     * @param  CoeffBOvr  Coefficient Memory B
     *
     * @param  LdcOOfstOvr        LDC Output Offset
     * @param  NsfIOOfstOvr    NSF Input-Output Offset
     * @param  RotIOfstOvr        ROT Input Offset
     * @param  RotOOfstOvr        ROT Output Offset
     * @param  DctSOfstOvr        DCT S Offset
     * @param  DctFOfstOvr        DCT F Offset
     * @param  VlcdjIOOfstOvr    VLCDJ Input-Output Offset
     * @param  ImxADataOfstOvr     IMX A Data Memory Offset
     * @param  ImxBDataOfstOvr     IMX B Data Memory Offset
     *
     * Following Params override values set in HW sequencer Step Params if the corresponding ovr param above is set.
     * This override mechanism is used to execute SW sequences in parallel to HW sequencing steps
     *
     * @param  LdcOOfst
     * @param  Nsf2IOOfst
     * @param  RotIOfst
     * @param  RotOOfst
     * @param  DctSOfst
     * @param  DctFOfst
     * @param  VlcdjIOOfst
     * @param  ImxADataOfst 
     * @param  ImxBDataOfst 
     *
     * @param  ImBuffA
     * @param  ImBuffB
     * @param  ImBuffC
     * @param  ImBuffD
     * @param  ImBuffE
     * @param  ImBuffF
     * @param  ImBuffG
     * @param  ImBuffH
     * @param  CoeffA
     * @param  CoeffB
     *
     * Following Params achieve SW controlled START/DONE synchronization of the listed resources
     *
     * @param  DmaTrigger  SIMCOP DMA
     * @param  LdcTrigger    LDC
     * @param  NsfTrigger    NSF
     * @param  RotATrigger  ROT#A
     * @param  DctTrigger     DCT
     * @param  VlcdjTrigger  VLCDJ
     */
    /* ========================================================================== 
     */
    typedef struct {

        Bool SetupHwOverRide;

        CSL_SeqOvrType ImBuffAOvr;
        CSL_SeqOvrType ImBuffBOvr;
        CSL_SeqOvrType ImBuffCOvr;
        CSL_SeqOvrType ImBuffDOvr;
        CSL_SeqOvrType ImBuffEOvr;
        CSL_SeqOvrType ImBuffFOvr;
        CSL_SeqOvrType ImBuffGOvr;
        CSL_SeqOvrType ImBuffHOvr;
        CSL_SeqOvrType CoeffAOvr;
        CSL_SeqOvrType CoeffBOvr;

        CSL_SeqOvrType LdcOOfstOvr;
        CSL_SeqOvrType NsfIOOfstOvr;
        CSL_SeqOvrType RotIOfstOvr;
        CSL_SeqOvrType RotOOfstOvr;
        CSL_SeqOvrType DctSOfstOvr;
        CSL_SeqOvrType DctFOfstOvr;
        CSL_SeqOvrType VlcdjIOOfstOvr;
        CSL_SeqOvrType ImxADataOfstOvr;
        CSL_SeqOvrType ImxBDataOfstOvr;

        CSL_RotOutLdcOutOfstType LdcOOfst;
        CSL_DctSNsfOfstType NsfIOOfst;
        CSL_RotInOfstType RotIOfst;
        CSL_RotOutLdcOutOfstType RotOOfst;
        CSL_DctSNsfOfstType DctSOfst;
        CSL_DctFVlcdjOfstType DctFOfst;
        CSL_DctFVlcdjOfstType VlcdjIOOfst;
        CSL_ImxDOfstOvrType ImxADataOfst;
        CSL_ImxDOfstOvrType ImxBDataOfst;

        CSL_BufSwitchImgABCDCoeffAType ImBuffA;
        CSL_BufSwitchImgABCDCoeffAType ImBuffB;
        CSL_BufSwitchImgABCDCoeffAType ImBuffC;
        CSL_BufSwitchImgABCDCoeffAType ImBuffD;
        CSL_BufSwitchImgEFType ImBuffE;
        CSL_BufSwitchImgEFType ImBuffF;
        CSL_BufSwitchImgGHType ImBuffG;
        CSL_BufSwitchImgGHType ImBuffH;
        CSL_BufSwitchImgABCDCoeffAType CoeffA;
        CSL_BufSwitchCoeffBType CoeffB;

        CSL_DMATriggerType DmaTrigger;
        CSL_TriggerType LdcTrigger;
        CSL_TriggerType NsfTrigger;
        CSL_TriggerType RotTrigger;
        CSL_TriggerType DctTrigger;
        CSL_TriggerType VlcdjTrigger;
    } CSL_HwSeqOvr;

    /* ========================================================================== 
     */
    /* 
     * CSL_LdcCtrl - structure for LDC Control at Simcop level.
     * @param  LdcRBurstBreak  Controls if bursts issued by the MTCR2OCP bridge could cross burst length boundaries. 
     *                                            When it is set, the MTCR2OCP bridge only issues OCP aligned bursts. This control can only 
     *                                            be used when Ldc_r_max_burst_length is 32, 64 or 128 bytes.
     * @param  LdcRMaxBurstLength  Limits the maximum burst length that could be used by the MTCR2OCP bridge 
     * @param  LdcRTagCnt  Limits the maximum number of outstanding requests to LDC_R_TAG_CNT+1; 
     *                        LDC_TAG_OFST + LDC_R_TAG_CNT must be <= 15
     * @param  LdcRTagOfst  First OCP tag ID that can be used by LDC reads. It is SW's responsibility to prevent overlap with tags 
     *                         generated by the SIMCOP DMA. Typically this value should be equal to SIMCP_DMA_CTRL.TAG_CNT+1
     */
    /* ========================================================================== 
     */
    typedef struct {
        CSL_LdcBurstBreakType LdcRBurstBreak;
        CSL_LdcMaxBurstLengthType LdcRMaxBurstLength;
        Bits4 LdcRTagCnt;
        Bits4 LdcRTagOfst;
    } CSL_LdcCtrl;

    /* ========================================================================== 
     */
    /* 
     * CSL_HwSeqCtrl - Simcop Buffer Access Control structure.
     * @param  LdcLut  LDC LUT access control
     * @param  LdcInput  Determines the memory used for input to LDC
     * @param  NsfWMem  Determines the memory used as working memory to NSF  
     * @param  Quant  Quantization Table memory access control
     * @param  Huff  Huffman Table memory access control
     * @param  ImxACmd  IMX_A command memory access control
     * @param  ImxBCmd  IMX_B command memory access control
     */
    /* ========================================================================== 
     */
    typedef struct {
        CSL_LdcLutAccType LdcLut;
        CSL_LdcInputMemType LdcInput;
        CSL_NsfWmemType NsfWMem;
        CSL_HuffQuantAccType Quant;
        CSL_HuffQuantAccType Huff;
        CSL_ImxCmdAAccType ImxACmd;
        CSL_ImxCmdBAccType ImxBCmd;
    } CSL_BufAccCtrl;

    /* ========================================================================== 
     */
    /* 
     * CSL_HwSeqCtrl - Simcop Sub-module clock Control structure.
     * @param  Dma  Clock for Simcop DMA
     * @param  Ldc  Clock for LDC
     * @param  Nsf  Clock for NSF
     * @param  Rot  Clock for ROT
     * @param  Dct  Clock for DCT
     * @param  Vlcdj  Clock for VLCDJ
     * @param  ImxA  Clock for IMX A
     * @param  ImxB  Clock for IMX B
     */
    /* ========================================================================== 
     */
    typedef struct {
        CSL_ClkCtrlType Dma;
        CSL_ClkCtrlType Ldc;
        CSL_ClkCtrlType Nsf;
        CSL_ClkCtrlType Rot;
        CSL_ClkCtrlType Dct;
        CSL_ClkCtrlType Vlcdj;
        CSL_ClkCtrlType ImxA;
        CSL_ClkCtrlType ImxB;
    } CSL_ClkCtrl;

    /* ========================================================================== 
     */
    /* 
     * CSL_HwSeqCtrl - Simcop Hw Sequencer Control structure.
     * @param  HwSeqStepCounter  Specifies number of hardware sequencer steps. 0 corresponds to manual sequencing
     * @param  Step  Reading recons the step currently executed by Hw Seq; writing mentions the step to start with
     * @param  HwSeqStart  Controls starting of Hw Seq
     * @param  HwSeqStop  Controls stopping of Hw Seq
     * @param  HwSeqStep  Configuration for each step of Hw Seq
     * @param  HwSeqOvr  Configuration to override Hw Seq Step Control during Sw Sequencing
     * @param  CpuProcDone  Mechanism to intimate that Cpu is done with assigned processing
     */
    /* ========================================================================== 
     */
    typedef struct {
        Uint16 HwSeqStepCounter;
        CSL_NextStepType Step;
        CSL_HwSeqStartStopType HwSeqStart;
        CSL_HwSeqStartStopType HwSeqStop;
        CSL_HwSeqStep HwSeqStep[4];
        CSL_HwSeqOvr HwSeqOvr;
        CSL_CpuStatusType CpuProcDone;
    } CSL_HwSeqCtrl;

    /* ========================================================================== 
     */
    /* 
     * CSL_BBMCtrl - Simcop BBM Control structure.
     * @param  BbmSyncChan  Determines Hw Sync Channel used to sync with SIMCOP DMA 
     * @param  BitStream  Determines how Bitstream buffer is used
     * @param  BitStreamXferSize  determines number of bitstream bytes transferred per SIMCOP DMA transfer
     */
    /* ========================================================================== 
     */
    typedef struct {
        Bits2 BbmSyncChan;
        CSL_BSAccType BitStream;
        CSL_BSTransferSizeType BitStreamXferSize;
    } CSL_BBMCtrl;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopHwSetup - Simcop Hardware Setup structure.
     * @param  Standby  Simcop Standby Mode
     * @param  Softreset  Softreset of Simcop
     * @param  IrqMode  determines condition to generate IRQ signal: when ALL or ONE OF the enabled events occur
     * @param  Irq  Contains IRQ event control and Status
     * @param  Irqeoi  EOI event for which of avaiable IRQ lines
     * @param  Ldcctrl  controls for LDC
     * @param  Bufaccctrl controls coeff memory access switching
     * @param  Clkctrl  Controls clocking of sub-modules
     * @param  Hwseqctrl  Controls H/w Seq 
     * @param  Bbmctrl  Controls BBM
     * @see 
     */
    /* ========================================================================== 
     */
    typedef struct {
        CSL_StandbyType StandbyMode;
        CSL_SoftResetType SoftReset;
        CSL_IrqModeType IrqMode[4];
        CSL_SimcopIRQ Irq[4];
        CSL_IrqEOIType IrqEoi;
        CSL_LdcCtrl LdcCtrl;
        CSL_BufAccCtrl BufAccCtrl;
        CSL_ClkCtrl ClkCtrl;
        CSL_HwSeqCtrl HwseqCtrl;
        CSL_BBMCtrl BbmCtrl;
    } CSL_SimcopHwSetupCtrl;

#if 0
    /* 
     * Simcop Hardware Control and Query Enums
     */
    /* ========================================================================== 
     */
/** CSL_SimcopHwCtrlCmdType  describes the possible commands issued to write values to Simcop registers.
*  
* This enumeration type is used by CSL_SimcopHwControl API
*/
    /* ========================================================================== 
     */

    typedef enum {
        /* 
         * Common
         */
        CSL_SIMCOP_CMD_SET_STANDBYMODE,
        CSL_SIMCOP_CMD_SET_SOFTRESET,
        /* 
         * IRQ
         */
        CSL_SIMCOP_CMD_SET_IRQMODE0,
        CSL_SIMCOP_CMD_SET_IRQMODE1,
        CSL_SIMCOP_CMD_SET_IRQMODE2,
        CSL_SIMCOP_CMD_SET_IRQMODE3,

        CSL_SIMCOP_CMD_SET_STATUSRAW_CPU_PROC_START_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_MTCR2OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_DONE_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP3_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP2_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP1_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP0_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_LDC_BLOCK_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_ROT_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_IMX_B_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_IMX_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_NSF_IRQ_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_VLCDJ_BLOC_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_DCT_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUSRAW_LDC_FRAME_IRQ_FOR_IRQLINE0,

        CSL_SIMCOP_CMD_SET_STATUSRAW_CPU_PROC_START_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_MTCR2OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_DONE_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP3_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP2_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP1_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP0_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_LDC_BLOCK_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_ROT_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_IMX_B_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_IMX_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_NSF_IRQ_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_VLCDJ_BLOC_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_DCT_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUSRAW_LDC_FRAME_IRQ_FOR_IRQLINE1,

        CSL_SIMCOP_CMD_SET_STATUSRAW_CPU_PROC_START_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_MTCR2OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_DONE_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP3_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP2_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP1_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP0_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_LDC_BLOCK_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_ROT_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_IMX_B_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_IMX_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_NSF_IRQ_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_VLCDJ_BLOC_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_DCT_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUSRAW_LDC_FRAME_IRQ_FOR_IRQLINE2,

        CSL_SIMCOP_CMD_SET_STATUSRAW_CPU_PROC_START_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_MTCR2OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_DONE_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP3_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP2_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP1_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_STEP0_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_LDC_BLOCK_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_ROT_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_IMX_B_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_IMX_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_NSF_IRQ_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_VLCDJ_BLOC_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_DCT_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUSRAW_LDC_FRAME_IRQ_FOR_IRQLINE3,

        CSL_SIMCOP_CMD_SET_STATUS_CPU_PROC_START_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_MTCR2OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_DONE_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_STEP3_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_STEP2_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_STEP1_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_STEP0_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_LDC_BLOCK_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_ROT_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_IMX_B_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_IMX_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_NSF_IRQ_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_VLCDJ_BLOC_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_DCT_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STATUS_LDC_FRAME_IRQ_FOR_IRQLINE0,

        CSL_SIMCOP_CMD_SET_STATUS_CPU_PROC_START_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_MTCR2OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_DONE_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_STEP3_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_STEP2_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_STEP1_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_STEP0_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_LDC_BLOCK_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_ROT_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_IMX_B_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_IMX_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_NSF_IRQ_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_VLCDJ_BLOC_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_DCT_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STATUS_LDC_FRAME_IRQ_FOR_IRQLINE1,

        CSL_SIMCOP_CMD_SET_STATUS_CPU_PROC_START_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_MTCR2OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_DONE_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_STEP3_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_STEP2_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_STEP1_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_STEP0_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_LDC_BLOCK_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_ROT_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_IMX_B_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_IMX_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_NSF_IRQ_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_VLCDJ_BLOC_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_DCT_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STATUS_LDC_FRAME_IRQ_FOR_IRQLINE2,

        CSL_SIMCOP_CMD_SET_STATUS_CPU_PROC_START_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_MTCR2OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_DONE_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_STEP3_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_STEP2_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_STEP1_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_STEP0_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_LDC_BLOCK_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_ROT_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_IMX_B_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_IMX_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_NSF_IRQ_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_VLCDJ_BLOC_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_DCT_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STATUS_LDC_FRAME_IRQ_FOR_IRQLINE3,

        CSL_SIMCOP_CMD_SET_CPU_PROC_START_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_SIMCOP_DMA_IRQ1_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_MTCR2OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_DONE_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STEP3_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STEP2_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STEP1_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_STEP0_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_LDC_BLOCK_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_ROT_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_IMX_B_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_IMX_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_NSF_IRQ_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_VLCDJ_BLOC_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_DCT_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_LDC_FRAME_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_SET_SIMCOP_DMA_IRQ0_FOR_IRQLINE0,

        CSL_SIMCOP_CMD_SET_CPU_PROC_START_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_SIMCOP_DMA_IRQ1_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_MTCR2OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_DONE_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STEP3_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STEP2_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STEP1_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_STEP0_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_LDC_BLOCK_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_ROT_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_IMX_B_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_IMX_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_NSF_IRQ_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_VLCDJ_BLOC_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_DCT_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_LDC_FRAME_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_SET_SIMCOP_DMA_IRQ0_FOR_IRQLINE1,

        CSL_SIMCOP_CMD_SET_CPU_PROC_START_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_SIMCOP_DMA_IRQ1_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_MTCR2OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_DONE_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STEP3_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STEP2_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STEP1_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_STEP0_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_LDC_BLOCK_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_ROT_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_IMX_B_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_IMX_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_NSF_IRQ_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_VLCDJ_BLOC_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_DCT_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_LDC_FRAME_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_SET_SIMCOP_DMA_IRQ0_FOR_IRQLINE2,

        CSL_SIMCOP_CMD_SET_CPU_PROC_START_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_SIMCOP_DMA_IRQ1_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_MTCR2OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_DONE_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STEP3_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STEP2_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STEP1_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_STEP0_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_LDC_BLOCK_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_ROT_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_IMX_B_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_IMX_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_NSF_IRQ_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_VLCDJ_BLOC_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_DCT_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_LDC_FRAME_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_SET_SIMCOP_DMA_IRQ0_FOR_IRQLINE3,

        CSL_SIMCOP_CMD_CLR_CPU_PROC_START_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_SIMCOP_DMA_IRQ1_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_MTCR2OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_DONE_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_STEP3_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_STEP2_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_STEP1_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_STEP0_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_LDC_BLOCK_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_ROT_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_IMX_B_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_IMX_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_NSF_IRQ_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_VLCDJ_BLOC_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_DCT_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_LDC_FRAME_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_CMD_CLR_SIMCOP_DMA_IRQ0_FOR_IRQLINE0,

        CSL_SIMCOP_CMD_CLR_CPU_PROC_START_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_SIMCOP_DMA_IRQ1_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_MTCR2OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_DONE_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_STEP3_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_STEP2_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_STEP1_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_STEP0_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_LDC_BLOCK_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_ROT_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_IMX_B_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_IMX_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_NSF_IRQ_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_VLCDJ_BLOC_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_DCT_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_LDC_FRAME_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_CMD_CLR_SIMCOP_DMA_IRQ0_FOR_IRQLINE1,

        CSL_SIMCOP_CMD_CLR_CPU_PROC_START_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_SIMCOP_DMA_IRQ1_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_MTCR2OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_DONE_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_STEP3_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_STEP2_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_STEP1_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_STEP0_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_LDC_BLOCK_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_ROT_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_IMX_B_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_IMX_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_NSF_IRQ_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_VLCDJ_BLOC_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_DCT_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_LDC_FRAME_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_CMD_CLR_SIMCOP_DMA_IRQ0_FOR_IRQLINE2,

        CSL_SIMCOP_CMD_CLR_CPU_PROC_START_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_SIMCOP_DMA_IRQ1_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_MTCR2OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_DONE_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_STEP3_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_STEP2_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_STEP1_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_STEP0_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_LDC_BLOCK_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_ROT_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_IMX_B_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_IMX_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_NSF_IRQ_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_VLCDJ_BLOC_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_DCT_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_LDC_FRAME_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_CMD_CLR_SIMCOP_DMA_IRQ0_FOR_IRQLINE3,
        /* 
         * Common
         */
        CSL_SIMCOP_CMD_SET_EOI_LINE_NUMBER,
        CSL_SIMCOP_CMD_SET_LDC_R_BURST_BREAK,
        CSL_SIMCOP_CMD_SET_LDC_R_MAX_BURST_LENGTH,
        CSL_SIMCOP_CMD_SET_LDC_R_TAG_CNT,
        CSL_SIMCOP_CMD_SET_LDC_R_TAG_OFST,
        CSL_SIMCOP_CMD_SET_LDC_LUT,
        CSL_SIMCOP_CMD_SET_LDC_INPUT,
        CSL_SIMCOP_CMD_SET_NSF_WMEM,
        CSL_SIMCOP_CMD_SET_QUANT,
        CSL_SIMCOP_CMD_SET_HUFF,
        CSL_SIMCOP_CMD_SET_IMX_A_CMD,
        CSL_SIMCOP_CMD_SET_IMX_B_CMD,
        CSL_SIMCOP_CMD_SET_DMA_CLK,
        CSL_SIMCOP_CMD_SET_LDC_CLK,
        CSL_SIMCOP_CMD_SET_NSF2_CLK,
        CSL_SIMCOP_CMD_SET_ROT_A_CLK,
        CSL_SIMCOP_CMD_SET_DCT_CLK,
        CSL_SIMCOP_CMD_SET_VLCDJ_CLK,
        CSL_SIMCOP_CMD_SET_IMX_A_CLK,
        CSL_SIMCOP_CMD_SET_IMX_B_CLK,
        CSL_SIMCOP_CMD_SET_HW_SEQ_STEP_COUNTER,
        CSL_SIMCOP_CMD_SET_STEP,
        CSL_SIMCOP_CMD_SET_HW_SEQ_START,
        CSL_SIMCOP_CMD_SET_HW_SEQ_STOP,
        /* 
         * HwSeq Step 0
         */
        CSL_SIMCOP_CMD_SET_CPU_SYNC_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_DMA_SYNC_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_ROT_A_SYNC_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_LDC_SYNC_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_NSF_SYNC_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_DCT_SYNC_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_VLCDJ_SYNC_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_IMX_A_SYNC_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_IMX_B_SYNC_FOR_STEP0,

        CSL_SIMCOP_CMD_SET_DMA_OFST_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_LDC_O_OFST_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_NSF2_IO_OFST_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_ROT_I_OFST_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_ROT_O_OFST_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_DCT_S_OFST_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_DCT_F_OFST_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_VLCDJ_IO_OFST_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_IMX_A_D_OFST_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_IMX_B_D_OFST_FOR_STEP0,

        CSL_SIMCOP_CMD_SET_IMBUFF_A_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_IMBUFF_B_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_IMBUFF_C_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_IMBUFF_D_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_IMBUFF_E_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_IMBUFF_F_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_IMBUFF_G_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_IMBUFF_H_FOR_STEP0,

        CSL_SIMCOP_CMD_SET_COEFF_A_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_COEFF_B_FOR_STEP0,

        CSL_SIMCOP_CMD_SET_IMX_A_START_FOR_STEP0,
        CSL_SIMCOP_CMD_SET_IMX_B_START_FOR_STEP0,

        CSL_SIMCOP_CMD_SET_NEXT_FOR_STEP0,
        /* 
         * HwSeq Step 1
         */
        CSL_SIMCOP_CMD_SET_CPU_SYNC_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_DMA_SYNC_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_ROT_A_SYNC_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_LDC_SYNC_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_NSF_SYNC_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_DCT_SYNC_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_VLCDJ_SYNC_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_IMX_A_SYNC_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_IMX_B_SYNC_FOR_STEP1,

        CSL_SIMCOP_CMD_SET_DMA_OFST_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_LDC_O_OFST_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_NSF2_IO_OFST_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_ROT_I_OFST_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_ROT_O_OFST_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_DCT_S_OFST_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_DCT_F_OFST_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_VLCDJ_IO_OFST_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_IMX_A_D_OFST_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_IMX_B_D_OFST_FOR_STEP1,

        CSL_SIMCOP_CMD_SET_IMBUFF_A_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_IMBUFF_B_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_IMBUFF_C_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_IMBUFF_D_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_IMBUFF_E_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_IMBUFF_F_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_IMBUFF_G_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_IMBUFF_H_FOR_STEP1,

        CSL_SIMCOP_CMD_SET_COEFF_A_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_COEFF_B_FOR_STEP1,

        CSL_SIMCOP_CMD_SET_IMX_A_START_FOR_STEP1,
        CSL_SIMCOP_CMD_SET_IMX_B_START_FOR_STEP1,

        CSL_SIMCOP_CMD_SET_NEXT_FOR_STEP1,
        /* 
         * HwSeq Step 2
         */
        CSL_SIMCOP_CMD_SET_CPU_SYNC_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_DMA_SYNC_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_ROT_A_SYNC_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_LDC_SYNC_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_NSF_SYNC_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_DCT_SYNC_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_VLCDJ_SYNC_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_IMX_A_SYNC_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_IMX_B_SYNC_FOR_STEP2,

        CSL_SIMCOP_CMD_SET_DMA_OFST_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_LDC_O_OFST_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_NSF2_IO_OFST_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_ROT_I_OFST_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_ROT_O_OFST_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_DCT_S_OFST_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_DCT_F_OFST_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_VLCDJ_IO_OFST_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_IMX_A_D_OFST_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_IMX_B_D_OFST_FOR_STEP2,

        CSL_SIMCOP_CMD_SET_IMBUFF_A_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_IMBUFF_B_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_IMBUFF_C_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_IMBUFF_D_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_IMBUFF_E_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_IMBUFF_F_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_IMBUFF_G_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_IMBUFF_H_FOR_STEP2,

        CSL_SIMCOP_CMD_SET_COEFF_A_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_COEFF_B_FOR_STEP2,

        CSL_SIMCOP_CMD_SET_IMX_A_START_FOR_STEP2,
        CSL_SIMCOP_CMD_SET_IMX_B_START_FOR_STEP2,

        CSL_SIMCOP_CMD_SET_NEXT_FOR_STEP2,
        /* 
         * HwSeq Step 3
         */
        CSL_SIMCOP_CMD_SET_CPU_SYNC_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_DMA_SYNC_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_ROT_A_SYNC_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_LDC_SYNC_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_NSF_SYNC_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_DCT_SYNC_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_VLCDJ_SYNC_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_IMX_A_SYNC_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_IMX_B_SYNC_FOR_STEP3,

        CSL_SIMCOP_CMD_SET_DMA_OFST_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_LDC_O_OFST_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_NSF2_IO_OFST_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_ROT_I_OFST_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_ROT_O_OFST_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_DCT_S_OFST_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_DCT_F_OFST_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_VLCDJ_IO_OFST_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_IMX_A_D_OFST_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_IMX_B_D_OFST_FOR_STEP3,

        CSL_SIMCOP_CMD_SET_IMBUFF_A_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_IMBUFF_B_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_IMBUFF_C_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_IMBUFF_D_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_IMBUFF_E_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_IMBUFF_F_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_IMBUFF_G_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_IMBUFF_H_FOR_STEP3,

        CSL_SIMCOP_CMD_SET_COEFF_A_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_COEFF_B_FOR_STEP3,

        CSL_SIMCOP_CMD_SET_IMX_A_START_FOR_STEP3,
        CSL_SIMCOP_CMD_SET_IMX_B_START_FOR_STEP3,

        CSL_SIMCOP_CMD_SET_NEXT_FOR_STEP3,
        /* 
         * HwSeq Override
         */
        CSL_SIMCOP_CMD_SET_IMBUFF_A_CTRL,
        CSL_SIMCOP_CMD_SET_IMBUFF_B_CTRL,
        CSL_SIMCOP_CMD_SET_IMBUFF_C_CTRL,
        CSL_SIMCOP_CMD_SET_IMBUFF_D_CTRL,
        CSL_SIMCOP_CMD_SET_IMBUFF_E_CTRL,
        CSL_SIMCOP_CMD_SET_IMBUFF_F_CTRL,
        CSL_SIMCOP_CMD_SET_IMBUFF_G_CTRL,
        CSL_SIMCOP_CMD_SET_IMBUFF_H_CTRL,
        CSL_SIMCOP_CMD_SET_COEFF_A_CTRL,
        CSL_SIMCOP_CMD_SET_COEFF_B_CTRL,

        CSL_SIMCOP_CMD_SET_LDC_O_OFST_CTRL,
        CSL_SIMCOP_CMD_SET_NSF_IO_OFST_CTRL,
        CSL_SIMCOP_CMD_SET_ROT_I_OFST_CTRL,
        CSL_SIMCOP_CMD_SET_ROT_O_OFST_CTRL,
        CSL_SIMCOP_CMD_SET_DCT_S_OFST_CTRL,
        CSL_SIMCOP_CMD_SET_DCT_F_OFST_CTRL,
        CSL_SIMCOP_CMD_SET_VLCDJ_IO_OFST_CTRL,
        CSL_SIMCOP_CMD_SET_IMX_A_D_OFST_CTRL,
        CSL_SIMCOP_CMD_SET_IMX_B_D_OFST_CTRL,

        CSL_SIMCOP_CMD_SET_LDC_O_OFST_OVR,
        CSL_SIMCOP_CMD_SET_NSF2_IO_OFST_OVR,
        CSL_SIMCOP_CMD_SET_ROT_I_OFST_OVR,
        CSL_SIMCOP_CMD_SET_ROT_O_OFST_OVR,
        CSL_SIMCOP_CMD_SET_DCT_S_OFST_OVR,
        CSL_SIMCOP_CMD_SET_DCT_F_OFST_OVR,
        CSL_SIMCOP_CMD_SET_VLCDJ_IO_OFST_OVR,
        CSL_SIMCOP_CMD_SET_IMX_A_D_OFST_OVR,
        CSL_SIMCOP_CMD_SET_IMX_B_D_OFST_OVR,

        CSL_SIMCOP_CMD_SET_IMBUFF_A_OVR,
        CSL_SIMCOP_CMD_SET_IMBUFF_B_OVR,
        CSL_SIMCOP_CMD_SET_IMBUFF_C_OVR,
        CSL_SIMCOP_CMD_SET_IMBUFF_D_OVR,
        CSL_SIMCOP_CMD_SET_IMBUFF_E_OVR,
        CSL_SIMCOP_CMD_SET_IMBUFF_F_OVR,
        CSL_SIMCOP_CMD_SET_IMBUFF_G_OVR,
        CSL_SIMCOP_CMD_SET_IMBUFF_H_OVR,

        CSL_SIMCOP_CMD_SET_COEFF_A_OVR,
        CSL_SIMCOP_CMD_SET_COEFF_B_OVR,

        CSL_SIMCOP_CMD_SET_DMA_TRIGGER,
        CSL_SIMCOP_CMD_SET_LDC_TRIGGER,
        CSL_SIMCOP_CMD_SET_NSF_TRIGGER,
        CSL_SIMCOP_CMD_SET_ROT_A_TRIGGER,
        CSL_SIMCOP_CMD_SET_DCT_TRIGGER,
        CSL_SIMCOP_CMD_SET_VLCDJ_TRIGGER,
        /* 
         * Common
         */
        CSL_SIMCOP_CMD_SET_CPU_PROC_DONE,
        /* 
         * BBM
         */
        CSL_SIMCOP_CMD_SET_BBM_SYNC_CHAN,
        CSL_SIMCOP_CMD_SET_BITSTREAM,
        CSL_SIMCOP_CMD_SET_BITSTR_XFER_SIZE,
        /* 
         * BUFFERS
         */
        CSL_SIMCOP_CMD_SET_BITSTR_BUFFER,
        CSL_SIMCOP_CMD_SET_HUFFMAN,
        CSL_SIMCOP_CMD_SET_IMXB_CMD_MEM,
        CSL_SIMCOP_CMD_SET_IMXA_CMD_MEM,
        CSL_SIMCOP_CMD_SET_QUANTIZATION_TABLE_FOR_ENCODER,
        CSL_SIMCOP_CMD_SET_QUANTIZATION_TABLE_FOR_DECODER,
        CSL_SIMCOP_CMD_SET_LDCLUT,
        CSL_SIMCOP_CMD_SET_IMAGE_BUFFER_A,
        CSL_SIMCOP_CMD_SET_IMAGE_BUFFER_B,
        CSL_SIMCOP_CMD_SET_IMAGE_BUFFER_C,
        CSL_SIMCOP_CMD_SET_IMAGE_BUFFER_D,
        CSL_SIMCOP_CMD_SET_IMAGE_BUFFER_E,
        CSL_SIMCOP_CMD_SET_IMAGE_BUFFER_F,
        CSL_SIMCOP_CMD_SET_IMAGE_BUFFER_G,
        CSL_SIMCOP_CMD_SET_IMAGE_BUFFER_H,
        CSL_SIMCOP_CMD_SET_IMXA_COEFF_MEM,
        CSL_SIMCOP_CMD_SET_IMXB_COEFF_MEM
    } CSL_SimcopHwCtrlCmdType;

    /* ========================================================================== 
     */
/** CSL_SimcopHWQueryType  describes the possible commands issued to read values from Simcop registers.
* This enumeration type is used by CSL_SimcopGetHwStatus API
*/
    /* ========================================================================== 
     */

    typedef enum {
        /* 
         * Common
         */
        CSL_SIMCOP_QUERY_STANDBYMODE,
        CSL_SIMCOP_QUERY_SOFTRESET,
        /* 
         * IRQ
         */
        CSL_SIMCOP_QUERY_IRQMODE0,
        CSL_SIMCOP_QUERY_IRQMODE1,
        CSL_SIMCOP_QUERY_IRQMODE2,
        CSL_SIMCOP_QUERY_IRQMODE3,

        CSL_SIMCOP_QUERY_STATUSRAW_CPU_PROC_START_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_SIMCOP_DMA_IRQ1_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_MTCR2OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_DONE_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP3_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP2_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP1_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP0_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_LDC_BLOCK_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_ROT_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_IMX_B_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_IMX_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_NSF_IRQ_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_VLCDJ_BLOC_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_DCT_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_LDC_FRAME_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUSRAW_SIMCOP_DMA_IRQ0_FOR_IRQLINE0,

        CSL_SIMCOP_QUERY_STATUSRAW_CPU_PROC_START_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_SIMCOP_DMA_IRQ1_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_MTCR2OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_DONE_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP3_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP2_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP1_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP0_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_LDC_BLOCK_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_ROT_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_IMX_B_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_IMX_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_NSF_IRQ_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_VLCDJ_BLOC_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_DCT_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_LDC_FRAME_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUSRAW_SIMCOP_DMA_IRQ0_FOR_IRQLINE1,

        CSL_SIMCOP_QUERY_STATUSRAW_CPU_PROC_START_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_SIMCOP_DMA_IRQ1_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_MTCR2OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_DONE_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP3_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP2_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP1_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP0_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_LDC_BLOCK_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_ROT_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_IMX_B_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_IMX_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_NSF_IRQ_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_VLCDJ_BLOC_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_DCT_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_LDC_FRAME_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUSRAW_SIMCOP_DMA_IRQ0_FOR_IRQLINE2,

        CSL_SIMCOP_QUERY_STATUSRAW_CPU_PROC_START_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_SIMCOP_DMA_IRQ1_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_MTCR2OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_DONE_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP3_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP2_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP1_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_STEP0_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_LDC_BLOCK_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_ROT_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_IMX_B_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_IMX_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_NSF_IRQ_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_VLCDJ_BLOC_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_DCT_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_LDC_FRAME_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUSRAW_SIMCOP_DMA_IRQ0_FOR_IRQLINE3,

        CSL_SIMCOP_QUERY_STATUS_CPU_PROC_START_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_SIMCOP_DMA_IRQ1_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_MTCR2OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_DONE_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_STEP3_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_STEP2_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_STEP1_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_STEP0_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_LDC_BLOCK_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_ROT_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_IMX_B_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_IMX_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_NSF_IRQ_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_VLCDJ_BLOC_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_DCT_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_LDC_FRAME_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_STATUS_SIMCOP_DMA_IRQ0_FOR_IRQLINE0,

        CSL_SIMCOP_QUERY_STATUS_CPU_PROC_START_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_SIMCOP_DMA_IRQ1_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_MTCR2OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_DONE_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_STEP3_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_STEP2_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_STEP1_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_STEP0_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_LDC_BLOCK_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_ROT_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_IMX_B_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_IMX_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_NSF_IRQ_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_VLCDJ_BLOC_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_DCT_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_LDC_FRAME_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_STATUS_SIMCOP_DMA_IRQ0_FOR_IRQLINE1,

        CSL_SIMCOP_QUERY_STATUS_CPU_PROC_START_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_SIMCOP_DMA_IRQ1_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_MTCR2OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_DONE_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_STEP3_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_STEP2_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_STEP1_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_STEP0_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_LDC_BLOCK_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_ROT_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_IMX_B_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_IMX_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_NSF_IRQ_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_VLCDJ_BLOC_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_DCT_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_LDC_FRAME_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_STATUS_SIMCOP_DMA_IRQ0_FOR_IRQLINE2,

        CSL_SIMCOP_QUERY_STATUS_CPU_PROC_START_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_SIMCOP_DMA_IRQ1_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_MTCR2OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_DONE_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_STEP3_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_STEP2_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_STEP1_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_STEP0_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_LDC_BLOCK_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_ROT_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_IMX_B_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_IMX_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_NSF_IRQ_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_VLCDJ_BLOC_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_DCT_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_LDC_FRAME_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_STATUS_SIMCOP_DMA_IRQ0_FOR_IRQLINE3,

        CSL_SIMCOP_QUERY_SET_CPU_PROC_START_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_SIMCOP_DMA_IRQ1_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_MTCR2OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_DONE_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_STEP3_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_STEP2_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_STEP1_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_STEP0_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_LDC_BLOCK_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_ROT_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_IMX_B_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_IMX_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_NSF_IRQ_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_VLCDJ_BLOC_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_DCT_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_LDC_FRAME_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_SET_SIMCOP_DMA_IRQ0_FOR_IRQLINE0,

        CSL_SIMCOP_QUERY_SET_CPU_PROC_START_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_SIMCOP_DMA_IRQ1_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_MTCR2OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_DONE_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_STEP3_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_STEP2_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_STEP1_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_STEP0_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_LDC_BLOCK_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_ROT_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_IMX_B_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_IMX_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_NSF_IRQ_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_VLCDJ_BLOC_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_DCT_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_LDC_FRAME_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_SET_SIMCOP_DMA_IRQ0_FOR_IRQLINE1,

        CSL_SIMCOP_QUERY_SET_CPU_PROC_START_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_SIMCOP_DMA_IRQ1_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_MTCR2OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_DONE_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_STEP3_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_STEP2_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_STEP1_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_STEP0_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_LDC_BLOCK_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_ROT_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_IMX_B_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_IMX_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_NSF_IRQ_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_VLCDJ_BLOC_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_DCT_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_LDC_FRAME_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_SET_SIMCOP_DMA_IRQ0_FOR_IRQLINE2,

        CSL_SIMCOP_QUERY_SET_CPU_PROC_START_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_SIMCOP_DMA_IRQ1_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_MTCR2OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_DONE_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_STEP3_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_STEP2_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_STEP1_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_STEP0_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_LDC_BLOCK_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_ROT_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_IMX_B_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_IMX_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_NSF_IRQ_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_VLCDJ_BLOC_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_DCT_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_LDC_FRAME_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_SET_SIMCOP_DMA_IRQ0_FOR_IRQLINE3,

        CSL_SIMCOP_QUERY_CLR_CPU_PROC_START_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_SIMCOP_DMA_IRQ1_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_MTCR2OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_OCP_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_DONE_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_STEP3_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_STEP2_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_STEP1_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_STEP0_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_LDC_BLOCK_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_ROT_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_IMX_B_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_IMX_A_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_NSF_IRQ_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_VLCDJ_BLOC_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_DCT_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_LDC_FRAME_IRQ_FOR_IRQLINE0,
        CSL_SIMCOP_QUERY_CLR_SIMCOP_DMA_IRQ0_FOR_IRQLINE0,

        CSL_SIMCOP_QUERY_CLR_CPU_PROC_START_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_SIMCOP_DMA_IRQ1_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_MTCR2OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_OCP_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_DONE_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_STEP3_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_STEP2_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_STEP1_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_STEP0_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_LDC_BLOCK_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_ROT_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_IMX_B_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_IMX_A_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_NSF_IRQ_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_VLCDJ_BLOC_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_DCT_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_LDC_FRAME_IRQ_FOR_IRQLINE1,
        CSL_SIMCOP_QUERY_CLR_SIMCOP_DMA_IRQ0_FOR_IRQLINE1,

        CSL_SIMCOP_QUERY_CLR_CPU_PROC_START_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_SIMCOP_DMA_IRQ1_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_MTCR2OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_OCP_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_DONE_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_STEP3_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_STEP2_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_STEP1_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_STEP0_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_LDC_BLOCK_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_ROT_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_IMX_B_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_IMX_A_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_NSF_IRQ_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_VLCDJ_BLOC_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_DCT_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_LDC_FRAME_IRQ_FOR_IRQLINE2,
        CSL_SIMCOP_QUERY_CLR_SIMCOP_DMA_IRQ0_FOR_IRQLINE2,

        CSL_SIMCOP_QUERY_CLR_CPU_PROC_START_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_SIMCOP_DMA_IRQ1_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_MTCR2OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_OCP_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_VLCDJ_DECODE_ERR_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_DONE_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_STEP3_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_STEP2_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_STEP1_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_STEP0_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_LDC_BLOCK_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_ROT_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_IMX_B_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_IMX_A_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_NSF_IRQ_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_VLCDJ_BLOC_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_DCT_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_LDC_FRAME_IRQ_FOR_IRQLINE3,
        CSL_SIMCOP_QUERY_CLR_SIMCOP_DMA_IRQ0_FOR_IRQLINE3,
        /* 
         * Common
         */
        CSL_SIMCOP_QUERY_EOI_LINE_NUMBER,
        CSL_SIMCOP_QUERY_LDC_R_BURST_BREAK,
        CSL_SIMCOP_QUERY_LDC_R_MAX_BURST_LENGTH,
        CSL_SIMCOP_QUERY_LDC_R_TAG_CNT,
        CSL_SIMCOP_QUERY_LDC_R_TAG_OFST,
        CSL_SIMCOP_QUERY_LDC_LUT,
        CSL_SIMCOP_QUERY_LDC_INPUT,
        CSL_SIMCOP_QUERY_NSF_WMEM,
        CSL_SIMCOP_QUERY_QUANT,
        CSL_SIMCOP_QUERY_HUFF,
        CSL_SIMCOP_QUERY_IMX_A_CMD,
        CSL_SIMCOP_QUERY_IMX_B_CMD,
        CSL_SIMCOP_QUERY_DMA_CLK,
        CSL_SIMCOP_QUERY_LDC_CLK,
        CSL_SIMCOP_QUERY_NSF2_CLK,
        CSL_SIMCOP_QUERY_ROT_A_CLK,
        CSL_SIMCOP_QUERY_DCT_CLK,
        CSL_SIMCOP_QUERY_VLCDJ_CLK,
        CSL_SIMCOP_QUERY_IMX_A_CLK,
        CSL_SIMCOP_QUERY_IMX_B_CLK,
        CSL_SIMCOP_QUERY_HW_SEQ_STEP_COUNTER,
        CSL_SIMCOP_QUERY_STEP,
        CSL_SIMCOP_QUERY_HW_SEQ_STEP_COUNTER_CURRENT,
        CSL_SIMCOP_QUERY_HW_SEQ_STATUS,
        /* 
         * HwSeq Step 0
         */
        CSL_SIMCOP_QUERY_CPU_SYNC_FOR_STEP0,
        CSL_SIMCOP_QUERY_DMA_SYNC_FOR_STEP0,
        CSL_SIMCOP_QUERY_ROT_A_SYNC_FOR_STEP0,
        CSL_SIMCOP_QUERY_LDC_SYNC_FOR_STEP0,
        CSL_SIMCOP_QUERY_NSF_SYNC_FOR_STEP0,
        CSL_SIMCOP_QUERY_DCT_SYNC_FOR_STEP0,
        CSL_SIMCOP_QUERY_VLCDJ_SYNC_FOR_STEP0,
        CSL_SIMCOP_QUERY_IMX_A_SYNC_FOR_STEP0,
        CSL_SIMCOP_QUERY_IMX_B_SYNC_FOR_STEP0,

        CSL_SIMCOP_QUERY_DMA_OFST_FOR_STEP0,
        CSL_SIMCOP_QUERY_LDC_O_OFST_FOR_STEP0,
        CSL_SIMCOP_QUERY_NSF2_IO_OFST_FOR_STEP0,
        CSL_SIMCOP_QUERY_ROT_I_OFST_FOR_STEP0,
        CSL_SIMCOP_QUERY_ROT_O_OFST_FOR_STEP0,
        CSL_SIMCOP_QUERY_DCT_S_OFST_FOR_STEP0,
        CSL_SIMCOP_QUERY_DCT_F_OFST_FOR_STEP0,
        CSL_SIMCOP_QUERY_VLCDJ_IO_OFST_FOR_STEP0,
        CSL_SIMCOP_QUERY_IMX_A_D_OFST_FOR_STEP0,
        CSL_SIMCOP_QUERY_IMX_B_D_OFST_FOR_STEP0,

        CSL_SIMCOP_QUERY_IMBUFF_A_FOR_STEP0,
        CSL_SIMCOP_QUERY_IMBUFF_B_FOR_STEP0,
        CSL_SIMCOP_QUERY_IMBUFF_C_FOR_STEP0,
        CSL_SIMCOP_QUERY_IMBUFF_D_FOR_STEP0,
        CSL_SIMCOP_QUERY_IMBUFF_E_FOR_STEP0,
        CSL_SIMCOP_QUERY_IMBUFF_F_FOR_STEP0,
        CSL_SIMCOP_QUERY_IMBUFF_G_FOR_STEP0,
        CSL_SIMCOP_QUERY_IMBUFF_H_FOR_STEP0,

        CSL_SIMCOP_QUERY_COEFF_A_FOR_STEP0,
        CSL_SIMCOP_QUERY_COEFF_B_FOR_STEP0,

        CSL_SIMCOP_QUERY_IMX_A_START_FOR_STEP0,
        CSL_SIMCOP_QUERY_IMX_B_START_FOR_STEP0,

        CSL_SIMCOP_QUERY_NEXT_FOR_STEP0,
        /* 
         * HwSeq Step 1
         */
        CSL_SIMCOP_QUERY_CPU_SYNC_FOR_STEP1,
        CSL_SIMCOP_QUERY_DMA_SYNC_FOR_STEP1,
        CSL_SIMCOP_QUERY_ROT_A_SYNC_FOR_STEP1,
        CSL_SIMCOP_QUERY_LDC_SYNC_FOR_STEP1,
        CSL_SIMCOP_QUERY_NSF_SYNC_FOR_STEP1,
        CSL_SIMCOP_QUERY_DCT_SYNC_FOR_STEP1,
        CSL_SIMCOP_QUERY_VLCDJ_SYNC_FOR_STEP1,
        CSL_SIMCOP_QUERY_IMX_A_SYNC_FOR_STEP1,
        CSL_SIMCOP_QUERY_IMX_B_SYNC_FOR_STEP1,

        CSL_SIMCOP_QUERY_DMA_OFST_FOR_STEP1,
        CSL_SIMCOP_QUERY_LDC_O_OFST_FOR_STEP1,
        CSL_SIMCOP_QUERY_NSF2_IO_OFST_FOR_STEP1,
        CSL_SIMCOP_QUERY_ROT_I_OFST_FOR_STEP1,
        CSL_SIMCOP_QUERY_ROT_O_OFST_FOR_STEP1,
        CSL_SIMCOP_QUERY_DCT_S_OFST_FOR_STEP1,
        CSL_SIMCOP_QUERY_DCT_F_OFST_FOR_STEP1,
        CSL_SIMCOP_QUERY_VLCDJ_IO_OFST_FOR_STEP1,
        CSL_SIMCOP_QUERY_IMX_A_D_OFST_FOR_STEP1,
        CSL_SIMCOP_QUERY_IMX_B_D_OFST_FOR_STEP1,

        CSL_SIMCOP_QUERY_IMBUFF_A_FOR_STEP1,
        CSL_SIMCOP_QUERY_IMBUFF_B_FOR_STEP1,
        CSL_SIMCOP_QUERY_IMBUFF_C_FOR_STEP1,
        CSL_SIMCOP_QUERY_IMBUFF_D_FOR_STEP1,
        CSL_SIMCOP_QUERY_IMBUFF_E_FOR_STEP1,
        CSL_SIMCOP_QUERY_IMBUFF_F_FOR_STEP1,
        CSL_SIMCOP_QUERY_IMBUFF_G_FOR_STEP1,
        CSL_SIMCOP_QUERY_IMBUFF_H_FOR_STEP1,

        CSL_SIMCOP_QUERY_COEFF_A_FOR_STEP1,
        CSL_SIMCOP_QUERY_COEFF_B_FOR_STEP1,

        CSL_SIMCOP_QUERY_IMX_A_START_FOR_STEP1,
        CSL_SIMCOP_QUERY_IMX_B_START_FOR_STEP1,

        CSL_SIMCOP_QUERY_NEXT_FOR_STEP1,
        /* 
         * HwSeq Step 2
         */
        CSL_SIMCOP_QUERY_CPU_SYNC_FOR_STEP2,
        CSL_SIMCOP_QUERY_DMA_SYNC_FOR_STEP2,
        CSL_SIMCOP_QUERY_ROT_A_SYNC_FOR_STEP2,
        CSL_SIMCOP_QUERY_LDC_SYNC_FOR_STEP2,
        CSL_SIMCOP_QUERY_NSF_SYNC_FOR_STEP2,
        CSL_SIMCOP_QUERY_DCT_SYNC_FOR_STEP2,
        CSL_SIMCOP_QUERY_VLCDJ_SYNC_FOR_STEP2,
        CSL_SIMCOP_QUERY_IMX_A_SYNC_FOR_STEP2,
        CSL_SIMCOP_QUERY_IMX_B_SYNC_FOR_STEP2,

        CSL_SIMCOP_QUERY_DMA_OFST_FOR_STEP2,
        CSL_SIMCOP_QUERY_LDC_O_OFST_FOR_STEP2,
        CSL_SIMCOP_QUERY_NSF2_IO_OFST_FOR_STEP2,
        CSL_SIMCOP_QUERY_ROT_I_OFST_FOR_STEP2,
        CSL_SIMCOP_QUERY_ROT_O_OFST_FOR_STEP2,
        CSL_SIMCOP_QUERY_DCT_S_OFST_FOR_STEP2,
        CSL_SIMCOP_QUERY_DCT_F_OFST_FOR_STEP2,
        CSL_SIMCOP_QUERY_VLCDJ_IO_OFST_FOR_STEP2,
        CSL_SIMCOP_QUERY_IMX_A_D_OFST_FOR_STEP2,
        CSL_SIMCOP_QUERY_IMX_B_D_OFST_FOR_STEP2,

        CSL_SIMCOP_QUERY_IMBUFF_A_FOR_STEP2,
        CSL_SIMCOP_QUERY_IMBUFF_B_FOR_STEP2,
        CSL_SIMCOP_QUERY_IMBUFF_C_FOR_STEP2,
        CSL_SIMCOP_QUERY_IMBUFF_D_FOR_STEP2,
        CSL_SIMCOP_QUERY_IMBUFF_E_FOR_STEP2,
        CSL_SIMCOP_QUERY_IMBUFF_F_FOR_STEP2,
        CSL_SIMCOP_QUERY_IMBUFF_G_FOR_STEP2,
        CSL_SIMCOP_QUERY_IMBUFF_H_FOR_STEP2,

        CSL_SIMCOP_QUERY_COEFF_A_FOR_STEP2,
        CSL_SIMCOP_QUERY_COEFF_B_FOR_STEP2,

        CSL_SIMCOP_QUERY_IMX_A_START_FOR_STEP2,
        CSL_SIMCOP_QUERY_IMX_B_START_FOR_STEP2,

        CSL_SIMCOP_QUERY_NEXT_FOR_STEP2,
        /* 
         * HwSeq Step 3
         */
        CSL_SIMCOP_QUERY_CPU_SYNC_FOR_STEP3,
        CSL_SIMCOP_QUERY_DMA_SYNC_FOR_STEP3,
        CSL_SIMCOP_QUERY_ROT_A_SYNC_FOR_STEP3,
        CSL_SIMCOP_QUERY_LDC_SYNC_FOR_STEP3,
        CSL_SIMCOP_QUERY_NSF_SYNC_FOR_STEP3,
        CSL_SIMCOP_QUERY_DCT_SYNC_FOR_STEP3,
        CSL_SIMCOP_QUERY_VLCDJ_SYNC_FOR_STEP3,
        CSL_SIMCOP_QUERY_IMX_A_SYNC_FOR_STEP3,
        CSL_SIMCOP_QUERY_IMX_B_SYNC_FOR_STEP3,

        CSL_SIMCOP_QUERY_DMA_OFST_FOR_STEP3,
        CSL_SIMCOP_QUERY_LDC_O_OFST_FOR_STEP3,
        CSL_SIMCOP_QUERY_NSF2_IO_OFST_FOR_STEP3,
        CSL_SIMCOP_QUERY_ROT_I_OFST_FOR_STEP3,
        CSL_SIMCOP_QUERY_ROT_O_OFST_FOR_STEP3,
        CSL_SIMCOP_QUERY_DCT_S_OFST_FOR_STEP3,
        CSL_SIMCOP_QUERY_DCT_F_OFST_FOR_STEP3,
        CSL_SIMCOP_QUERY_VLCDJ_IO_OFST_FOR_STEP3,
        CSL_SIMCOP_QUERY_IMX_A_D_OFST_FOR_STEP3,
        CSL_SIMCOP_QUERY_IMX_B_D_OFST_FOR_STEP3,

        CSL_SIMCOP_QUERY_IMBUFF_A_FOR_STEP3,
        CSL_SIMCOP_QUERY_IMBUFF_B_FOR_STEP3,
        CSL_SIMCOP_QUERY_IMBUFF_C_FOR_STEP3,
        CSL_SIMCOP_QUERY_IMBUFF_D_FOR_STEP3,
        CSL_SIMCOP_QUERY_IMBUFF_E_FOR_STEP3,
        CSL_SIMCOP_QUERY_IMBUFF_F_FOR_STEP3,
        CSL_SIMCOP_QUERY_IMBUFF_G_FOR_STEP3,
        CSL_SIMCOP_QUERY_IMBUFF_H_FOR_STEP3,

        CSL_SIMCOP_QUERY_COEFF_A_FOR_STEP3,
        CSL_SIMCOP_QUERY_COEFF_B_FOR_STEP3,

        CSL_SIMCOP_QUERY_IMX_A_START_FOR_STEP3,
        CSL_SIMCOP_QUERY_IMX_B_START_FOR_STEP3,

        CSL_SIMCOP_QUERY_NEXT_FOR_STEP3,
        /* 
         * HwSeq Override
         */
        CSL_SIMCOP_QUERY_IMBUFF_A_CTRL,
        CSL_SIMCOP_QUERY_IMBUFF_B_CTRL,
        CSL_SIMCOP_QUERY_IMBUFF_C_CTRL,
        CSL_SIMCOP_QUERY_IMBUFF_D_CTRL,
        CSL_SIMCOP_QUERY_IMBUFF_E_CTRL,
        CSL_SIMCOP_QUERY_IMBUFF_F_CTRL,
        CSL_SIMCOP_QUERY_IMBUFF_G_CTRL,
        CSL_SIMCOP_QUERY_IMBUFF_H_CTRL,
        CSL_SIMCOP_QUERY_COEFF_A_CTRL,
        CSL_SIMCOP_QUERY_COEFF_B_CTRL,

        CSL_SIMCOP_QUERY_LDC_O_OFST_CTRL,
        CSL_SIMCOP_QUERY_NSF_IO_OFST_CTRL,
        CSL_SIMCOP_QUERY_ROT_I_OFST_CTRL,
        CSL_SIMCOP_QUERY_ROT_O_OFST_CTRL,
        CSL_SIMCOP_QUERY_DCT_S_OFST_CTRL,
        CSL_SIMCOP_QUERY_DCT_F_OFST_CTRL,
        CSL_SIMCOP_QUERY_VLCDJ_IO_OFST_CTRL,
        CSL_SIMCOP_QUERY_IMX_A_D_OFST_CTRL,
        CSL_SIMCOP_QUERY_IMX_B_D_OFST_CTRL,

        CSL_SIMCOP_QUERY_LDC_O_OFST_OVR,
        CSL_SIMCOP_QUERY_NSF2_IO_OFST_OVR,
        CSL_SIMCOP_QUERY_ROT_I_OFST_OVR,
        CSL_SIMCOP_QUERY_ROT_O_OFST_OVR,
        CSL_SIMCOP_QUERY_DCT_S_OFST_OVR,
        CSL_SIMCOP_QUERY_DCT_F_OFST_OVR,
        CSL_SIMCOP_QUERY_VLCDJ_IO_OFST_OVR,
        CSL_SIMCOP_QUERY_IMX_A_D_OFST_OVR,
        CSL_SIMCOP_QUERY_IMX_B_D_OFST_OVR,

        CSL_SIMCOP_QUERY_IMBUFF_A_OVR,
        CSL_SIMCOP_QUERY_IMBUFF_B_OVR,
        CSL_SIMCOP_QUERY_IMBUFF_C_OVR,
        CSL_SIMCOP_QUERY_IMBUFF_D_OVR,
        CSL_SIMCOP_QUERY_IMBUFF_E_OVR,
        CSL_SIMCOP_QUERY_IMBUFF_F_OVR,
        CSL_SIMCOP_QUERY_IMBUFF_G_OVR,
        CSL_SIMCOP_QUERY_IMBUFF_H_OVR,

        CSL_SIMCOP_QUERY_COEFF_A_OVR,
        CSL_SIMCOP_QUERY_COEFF_B_OVR,

        CSL_SIMCOP_QUERY_DMA_TRIGGER,
        CSL_SIMCOP_QUERY_LDC_TRIGGER,
        CSL_SIMCOP_QUERY_NSF_TRIGGER,
        CSL_SIMCOP_QUERY_ROT_A_TRIGGER,
        CSL_SIMCOP_QUERY_DCT_TRIGGER,
        CSL_SIMCOP_QUERY_VLCDJ_TRIGGER,
        /* 
         * BBM
         */
        CSL_SIMCOP_QUERY_BBM_SYNC_CHAN,
        CSL_SIMCOP_QUERY_BBM_STATUS,
        CSL_SIMCOP_QUERY_BITSTREAM,
        CSL_SIMCOP_QUERY_BITSTR_XFER_SIZE,
        /* 
         * BUFFERS
         */
        CSL_SIMCOP_QUERY_BITSTR_BUFFER,
        CSL_SIMCOP_QUERY_HUFFMAN,
        CSL_SIMCOP_QUERY_IMXB_CMD_MEM,
        CSL_SIMCOP_QUERY_IMXA_CMD_MEM,
        CSL_SIMCOP_QUERY_QUANTIZATION_TABLE_FOR_ENCODER,
        CSL_SIMCOP_QUERY_QUANTIZATION_TABLE_FOR_DECODER,
        CSL_SIMCOP_QUERY_LDCLUT,
        CSL_SIMCOP_QUERY_IMAGE_BUFFER_A,
        CSL_SIMCOP_QUERY_IMAGE_BUFFER_B,
        CSL_SIMCOP_QUERY_IMAGE_BUFFER_C,
        CSL_SIMCOP_QUERY_IMAGE_BUFFER_D,
        CSL_SIMCOP_QUERY_IMAGE_BUFFER_E,
        CSL_SIMCOP_QUERY_IMAGE_BUFFER_F,
        CSL_SIMCOP_QUERY_IMAGE_BUFFER_G,
        CSL_SIMCOP_QUERY_IMAGE_BUFFER_H,
        CSL_SIMCOP_QUERY_IMXA_COEFF_MEM,
        CSL_SIMCOP_QUERY_IMXB_COEFF_MEM
    } CSL_SimcopHWQueryType;
#endif

/*--------function prototypes ---------------------------------*/

    CSL_Status CSL_simcopInit(CSL_SimcopHandle hndl);
    CSL_Status CSL_simcopOpen(CSL_SimcopObj * hObj, CSL_SimcopNum simcopNum,
                              CSL_OpenMode openMode);
    CSL_Status CSL_simcopHwSetup(CSL_SimcopHandle hndl,
                                 CSL_SimcopHwSetupCtrl * setup);
#if 0
    CSL_Status CSL_simcopHwControl(CSL_SimcopHandle hndl,
                                   CSL_SimcopHwCtrlCmdType cmd, void *data);
    CSL_Status CSL_simcopGetHWStatus(CSL_SimcopHandle hndl,
                                     CSL_SimcopHWQueryType query, void *data);
#endif
    CSL_Status CSL_simcopClose(CSL_SimcopHandle hndl);

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/*--------macros ----------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif                                                     /* CSL_SIMCOP_H */

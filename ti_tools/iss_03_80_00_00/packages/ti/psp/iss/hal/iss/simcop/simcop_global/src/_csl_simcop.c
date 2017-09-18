/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file _csl_simcop.c
*
* This File contains CSL Level 0 functions for Simcop Module
* This entire description will appear as one
* paragraph in the generated documentation.
*
* @path  $(CSLPATH)\src\SIMCOP\
*
* @rev  00.01
*/
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *! *!
 * 16-Apr-2009 Phanish: Changes made because of Resource Manager Integration.
 * *! i.e., Removal of dependency on _CSL_certifyOpen() calls. *! 19-Mar-2009
 * Phanish: Modified to remove the HWcontrol and GetHWstatus APIs *! and their
 * dependencies. And now, HWsetup calls directly the CSL_FINS *! *! 17-Sep-2008
 * Padmanabha V Reddy: Created the file. *! *! *! 24-Dec-2000 mf: Revisions
 * appear in reverse chronological order; *! that is, newest first.  The date
 * format is dd-Mon-yyyy. *
 * =========================================================================== */

/****************************************************************
*  INCLUDE FILES
****************************************************************/
/*-------program files ----------------------------------------*/
#include "../cslr_simcop_2.h"
#include "../inc/_csl_simcop.h"

#if 0
/* SIMCOP_HL_REVISION */
/* ===================================================================
 *  @func     _CSL_simcopGetRev                                               
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
CSL_Status _CSL_simcopGetRev(CSL_SimcopHandle hndl,
                             CSL_SimcopRevisionType event, Uint16 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case SCHEME:
            *data =
                CSL_FEXT(simcopRegs->HL_REVISION, SIMCOP_HL_REVISION_SCHEME);
            break;
        case FUNC:
            *data = CSL_FEXT(simcopRegs->HL_REVISION, SIMCOP_HL_REVISION_FUNC);
            break;
        case R_RTL:
            *data = CSL_FEXT(simcopRegs->HL_REVISION, SIMCOP_HL_REVISION_R_RTL);
            break;
        case X_MAJOR:
            *data =
                CSL_FEXT(simcopRegs->HL_REVISION, SIMCOP_HL_REVISION_X_MAJOR);
            break;
        case CUSTOM:
            *data =
                CSL_FEXT(simcopRegs->HL_REVISION, SIMCOP_HL_REVISION_CUSTOM);
            break;
        case Y_MINOR:
            *data =
                CSL_FEXT(simcopRegs->HL_REVISION, SIMCOP_HL_REVISION_Y_MINOR);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HL_REVISION */
/* SIMCOP_HL_HWINFO */
/* ===================================================================
 *  @func     _CSL_simcopHwInfo_Get                                               
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
CSL_Status _CSL_simcopHwInfo_Get(CSL_SimcopHandle hndl,
                                 CSL_SimcopHwInfoType event, Uint8 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case LDCFIFOSIZE:
            *data =
                CSL_FEXT(simcopRegs->HL_HWINFO,
                         SIMCOP_HL_HWINFO_LDCR_RESP_FIFO);
            break;
        case NUMIMGBUFS:
            *data =
                CSL_FEXT(simcopRegs->HL_HWINFO, SIMCOP_HL_HWINFO_IMAGE_BUFFERS);
            break;
        case ROT_A:
            *data =
                CSL_FEXT(simcopRegs->HL_HWINFO, SIMCOP_HL_HWINFO_ROT_A_ENABLE);
            break;
        case IMX_B:
            *data =
                CSL_FEXT(simcopRegs->HL_HWINFO, SIMCOP_HL_HWINFO_IMX_B_ENABLE);
            break;
        case IMX_A:
            *data =
                CSL_FEXT(simcopRegs->HL_HWINFO, SIMCOP_HL_HWINFO_IMX_A_ENABLE);
            break;
        case NSF:
            *data =
                CSL_FEXT(simcopRegs->HL_HWINFO, SIMCOP_HL_HWINFO_NSF_ENABLE);
            break;
        case VLCDJ:
            *data =
                CSL_FEXT(simcopRegs->HL_HWINFO, SIMCOP_HL_HWINFO_VLCDJ_ENABLE);
            break;
        case DCT:
            *data =
                CSL_FEXT(simcopRegs->HL_HWINFO, SIMCOP_HL_HWINFO_DCT_ENABLE);
            break;
        case LDC:
            *data =
                CSL_FEXT(simcopRegs->HL_HWINFO, SIMCOP_HL_HWINFO_LDC_ENABLE);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HL_HWINFO */
/* SIMCOP_HL_SYSCONFIG */
/* ===================================================================
 *  @func     _CSL_simcopSysConfig_Get                                               
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
CSL_Status _CSL_simcopSysConfig_Get(CSL_SimcopHandle hndl,
                                    CSL_SimcopSysConfigType event, Uint8 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case STANDBYMODE:
            *data =
                CSL_FEXT(simcopRegs->HL_SYSCONFIG,
                         SIMCOP_HL_SYSCONFIG_STANDBYMODE);
            break;
        case SOFTRESET:
            *data =
                CSL_FEXT(simcopRegs->HL_SYSCONFIG,
                         SIMCOP_HL_SYSCONFIG_SOFTRESET);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopSysConfig_Set                                               
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
CSL_Status _CSL_simcopSysConfig_Set(CSL_SimcopHandle hndl,
                                    CSL_SimcopSysConfigType event, Uint8 data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case STANDBYMODE:
            CSL_FINS(simcopRegs->HL_SYSCONFIG, SIMCOP_HL_SYSCONFIG_STANDBYMODE,
                     data);
            break;
        case SOFTRESET:
            CSL_FINS(simcopRegs->HL_SYSCONFIG, SIMCOP_HL_SYSCONFIG_SOFTRESET,
                     data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HL_SYSCONFIG */
/* SIMCOP_HL_IRQ_EOI */

/* ===================================================================
 *  @func     _CSL_simcopIRQEOILineNumber_Get                                               
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
CSL_Status _CSL_simcopIRQEOILineNumber_Get(CSL_SimcopHandle hndl, Uint8 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(simcopRegs->HL_IRQ_EOI, SIMCOP_HL_IRQ_EOI_LINE_NUMBER);
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopIRQEOILineNumber_Set                                               
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
CSL_Status _CSL_simcopIRQEOILineNumber_Set(CSL_SimcopHandle hndl, Uint8 data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    CSL_FINS(simcopRegs->HL_IRQ_EOI, SIMCOP_HL_IRQ_EOI_LINE_NUMBER, data);
  EXIT:
    return status;
}

/* SIMCOP_HL_IRQSTATUS_RAW__X(0-4) */

/* ===================================================================
 *  @func     _CSL_simcopIRQRaw_IsEventPending                                               
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
CSL_Status _CSL_simcopIRQRaw_IsEventPending(CSL_SimcopHandle hndl,
                                            Uint8 instance,
                                            CSL_SimcopIRQType event,
                                            Bool * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case CPU_PROC_START_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_CPU_PROC_START_IRQ);
            break;
        case SIMCOP_DMA_IRQ1:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_SIMCOP_DMA_IRQ1);
            break;
        case MTCR2OCP_ERR_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_MTCR2OCP_ERR_IRQ);
            break;
        case OCP_ERR_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_OCP_ERR_IRQ);
            break;
        case VLCDJ_DECODE_ERR_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_VLCDJ_DECODE_ERR_IRQ);
            break;
        case DONE_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_DONE_IRQ);
            break;
        case STEP3_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_STEP3_IRQ);
            break;
        case STEP2_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_STEP2_IRQ);
            break;
        case STEP1_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_STEP1_IRQ);
            break;
        case STEP0_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_STEP0_IRQ);
            break;
        case LDC_BLOCK_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_LDC_BLOCK_IRQ);
            break;
        case ROT_A_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_ROT_A_IRQ);
            break;
        case IMX_B_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_IMX_B_IRQ);
            break;
        case IMX_A_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_IMX_A_IRQ);
            break;
        case NSF_IRQ_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_NSF_IRQ_IRQ);
            break;
        case VLCDJ_BLOC_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_VLCDJ_BLOC_IRQ);
            break;
        case DCT_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_DCT_IRQ);
            break;
        case LDC_FRAME_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_LDC_FRAME_IRQ);
            break;
        case SIMCOP_DMA_IRQ0:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                         SIMCOP_STATUS_RAW_SIMCOP_DMA_IRQ0);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopIRQRaw_SetEventPending                                               
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
CSL_Status _CSL_simcopIRQRaw_SetEventPending(CSL_SimcopHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopIRQType event, Bool data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case CPU_PROC_START_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_CPU_PROC_START_IRQ, data);
            break;
        case MTCR2OCP_ERR_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_MTCR2OCP_ERR_IRQ, data);
            break;
        case OCP_ERR_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_OCP_ERR_IRQ, data);
            break;
        case VLCDJ_DECODE_ERR_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_VLCDJ_DECODE_ERR_IRQ, data);
            break;
        case DONE_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_DONE_IRQ, data);
            break;
        case STEP3_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_STEP3_IRQ, data);
            break;
        case STEP2_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_STEP2_IRQ, data);
            break;
        case STEP1_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_STEP1_IRQ, data);
            break;
        case STEP0_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_STEP0_IRQ, data);
            break;
        case LDC_BLOCK_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_LDC_BLOCK_IRQ, data);
            break;
        case ROT_A_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_ROT_A_IRQ, data);
            break;
        case IMX_B_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_IMX_B_IRQ, data);
            break;
        case IMX_A_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_IMX_A_IRQ, data);
            break;
        case NSF_IRQ_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_NSF_IRQ_IRQ, data);
            break;
        case VLCDJ_BLOC_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_VLCDJ_BLOC_IRQ, data);
            break;
        case DCT_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_DCT_IRQ, data);
            break;
        case LDC_FRAME_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS_RAW,
                     SIMCOP_STATUS_RAW_LDC_FRAME_IRQ, data);
            break;
        case SIMCOP_DMA_IRQ0:
        case SIMCOP_DMA_IRQ1:
            status = CSL_ESYS_NOTSUPPORTED;
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HL_IRQSTATUS_RAW__X(0-4) */
/* SIMCOP_HL_IRQSTATUS__X(0-4) */

/* ===================================================================
 *  @func     _CSL_simcopIRQ_IsEventPending                                               
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
CSL_Status _CSL_simcopIRQ_IsEventPending(CSL_SimcopHandle hndl, Uint8 instance,
                                         CSL_SimcopIRQType event, Bool * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case CPU_PROC_START_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_CPU_PROC_START_IRQ);
            break;
        case SIMCOP_DMA_IRQ1:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_SIMCOP_DMA_IRQ1);
            break;
        case MTCR2OCP_ERR_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_MTCR2OCP_ERR_IRQ);
            break;
        case OCP_ERR_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_OCP_ERR_IRQ);
            break;
        case VLCDJ_DECODE_ERR_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_VLCDJ_DECODE_ERR_IRQ);
            break;
        case DONE_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_DONE_IRQ);
            break;
        case STEP3_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_STEP3_IRQ);
            break;
        case STEP2_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_STEP2_IRQ);
            break;
        case STEP1_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_STEP1_IRQ);
            break;
        case STEP0_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_STEP0_IRQ);
            break;
        case LDC_BLOCK_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_LDC_BLOCK_IRQ);
            break;
        case ROT_A_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_ROT_A_IRQ);
            break;
        case IMX_B_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_IMX_B_IRQ);
            break;
        case IMX_A_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_IMX_A_IRQ);
            break;
        case NSF_IRQ_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_NSF_IRQ_IRQ);
            break;
        case VLCDJ_BLOC_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_VLCDJ_BLOC_IRQ);
            break;
        case DCT_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_DCT_IRQ);
            break;
        case LDC_FRAME_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_LDC_FRAME_IRQ);
            break;
        case SIMCOP_DMA_IRQ0:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].STATUS,
                         SIMCOP_STATUS_SIMCOP_DMA_IRQ0);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopIRQ_SetEventPending                                               
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
CSL_Status _CSL_simcopIRQ_SetEventPending(CSL_SimcopHandle hndl, Uint8 instance,
                                          CSL_SimcopIRQType event, Bool data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case CPU_PROC_START_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_CPU_PROC_START_IRQ, data);
            break;
        case MTCR2OCP_ERR_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_MTCR2OCP_ERR_IRQ, data);
            break;
        case OCP_ERR_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_OCP_ERR_IRQ, data);
            break;
        case VLCDJ_DECODE_ERR_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_VLCDJ_DECODE_ERR_IRQ, data);
            break;
        case DONE_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_DONE_IRQ, data);
            break;
        case STEP3_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_STEP3_IRQ, data);
            break;
        case STEP2_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_STEP2_IRQ, data);
            break;
        case STEP1_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_STEP1_IRQ, data);
            break;
        case STEP0_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_STEP0_IRQ, data);
            break;
        case LDC_BLOCK_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_LDC_BLOCK_IRQ, data);
            break;
        case ROT_A_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_ROT_A_IRQ, data);
            break;
        case IMX_B_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_IMX_B_IRQ, data);
            break;
        case IMX_A_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_IMX_A_IRQ, data);
            break;
        case NSF_IRQ_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_NSF_IRQ_IRQ, data);
            break;
        case VLCDJ_BLOC_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_VLCDJ_BLOC_IRQ, data);
            break;
        case DCT_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS, SIMCOP_STATUS_DCT_IRQ,
                     data);
            break;
        case LDC_FRAME_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].STATUS,
                     SIMCOP_STATUS_LDC_FRAME_IRQ, data);
            break;
        case SIMCOP_DMA_IRQ0:
        case SIMCOP_DMA_IRQ1:
            status = CSL_ESYS_NOTSUPPORTED;
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HL_IRQSTATUS_RAW__X(0-4) */
/* SIMCOP_HL_IRQENABLE_SET__X(0-3) */

/* ===================================================================
 *  @func     _CSL_simcopIRQSet_IsEnabled                                               
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
CSL_Status _CSL_simcopIRQSet_IsEnabled(CSL_SimcopHandle hndl, Uint8 instance,
                                       CSL_SimcopIRQType event, Bool * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case CPU_PROC_START_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_CPU_PROC_START_IRQ);
            break;
        case SIMCOP_DMA_IRQ1:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_SIMCOP_DMA_IRQ1);
            break;
        case MTCR2OCP_ERR_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_MTCR2OCP_ERR_IRQ);
            break;
        case OCP_ERR_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_OCP_ERR_IRQ);
            break;
        case VLCDJ_DECODE_ERR_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_VLCDJ_DECODE_ERR_IRQ);
            break;
        case DONE_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_DONE_IRQ);
            break;
        case STEP3_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_STEP3_IRQ);
            break;
        case STEP2_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_STEP2_IRQ);
            break;
        case STEP1_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_STEP1_IRQ);
            break;
        case STEP0_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_STEP0_IRQ);
            break;
        case LDC_BLOCK_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_LDC_BLOCK_IRQ);
            break;
        case ROT_A_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_ROT_A_IRQ);
            break;
        case IMX_B_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_IMX_B_IRQ);
            break;
        case IMX_A_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_IMX_A_IRQ);
            break;
        case NSF_IRQ_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_NSF_IRQ_IRQ);
            break;
        case VLCDJ_BLOC_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_VLCDJ_BLOC_IRQ);
            break;
        case DCT_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_DCT_IRQ);
            break;
        case LDC_FRAME_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_LDC_FRAME_IRQ);
            break;
        case SIMCOP_DMA_IRQ0:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                         SIMCOP_ENABLE_SET_SIMCOP_DMA_IRQ0);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopIRQSet_Enable                                               
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
CSL_Status _CSL_simcopIRQSet_Enable(CSL_SimcopHandle hndl, Uint8 instance,
                                    CSL_SimcopIRQType event, Bool data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case CPU_PROC_START_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_CPU_PROC_START_IRQ, data);
            break;
        case SIMCOP_DMA_IRQ1:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_SIMCOP_DMA_IRQ1, data);
            break;
        case MTCR2OCP_ERR_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_MTCR2OCP_ERR_IRQ, data);
            break;
        case OCP_ERR_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_OCP_ERR_IRQ, data);
            break;
        case VLCDJ_DECODE_ERR_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_VLCDJ_DECODE_ERR_IRQ, data);
            break;
        case DONE_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_DONE_IRQ, data);
            break;
        case STEP3_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_STEP3_IRQ, data);
            break;
        case STEP2_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_STEP2_IRQ, data);
            break;
        case STEP1_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_STEP1_IRQ, data);
            break;
        case STEP0_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_STEP0_IRQ, data);
            break;
        case LDC_BLOCK_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_LDC_BLOCK_IRQ, data);
            break;
        case ROT_A_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_ROT_A_IRQ, data);
            break;
        case IMX_B_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_IMX_B_IRQ, data);
            break;
        case IMX_A_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_IMX_A_IRQ, data);
            break;
        case NSF_IRQ_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_NSF_IRQ_IRQ, data);
            break;
        case VLCDJ_BLOC_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_VLCDJ_BLOC_IRQ, data);
            break;
        case DCT_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_DCT_IRQ, data);
            break;
        case LDC_FRAME_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_LDC_FRAME_IRQ, data);
            break;
        case SIMCOP_DMA_IRQ0:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_SET,
                     SIMCOP_ENABLE_SET_SIMCOP_DMA_IRQ0, data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HL_IRQENABLE_SET__X(0-3) */
/* SIMCOP_HL_IRQENABLE_CLR__X(0-3) */

/* ===================================================================
 *  @func     _CSL_simcopIRQClr_IsEnabled                                               
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
CSL_Status _CSL_simcopIRQClr_IsEnabled(CSL_SimcopHandle hndl, Uint8 instance,
                                       CSL_SimcopIRQType event, Bool * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case CPU_PROC_START_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_CPU_PROC_START_IRQ);
            break;
        case SIMCOP_DMA_IRQ1:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_SIMCOP_DMA_IRQ1);
            break;
        case MTCR2OCP_ERR_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_MTCR2OCP_ERR_IRQ);
            break;
        case OCP_ERR_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_OCP_ERR_IRQ);
            break;
        case VLCDJ_DECODE_ERR_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_VLCDJ_DECODE_ERR_IRQ);
            break;
        case DONE_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_DONE_IRQ);
            break;
        case STEP3_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_STEP3_IRQ);
            break;
        case STEP2_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_STEP2_IRQ);
            break;
        case STEP1_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_STEP1_IRQ);
            break;
        case STEP0_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_STEP0_IRQ);
            break;
        case LDC_BLOCK_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_LDC_BLOCK_IRQ);
            break;
        case ROT_A_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_ROT_A_IRQ);
            break;
        case IMX_B_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_IMX_B_IRQ);
            break;
        case IMX_A_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_IMX_A_IRQ);
            break;
        case NSF_IRQ_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_NSF_IRQ_IRQ);
            break;
        case VLCDJ_BLOC_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_VLCDJ_BLOC_IRQ);
            break;
        case DCT_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_DCT_IRQ);
            break;
        case LDC_FRAME_IRQ:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_LDC_FRAME_IRQ);
            break;
        case SIMCOP_DMA_IRQ0:
            *data =
                CSL_FEXT(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                         SIMCOP_ENABLE_CLR_SIMCOP_DMA_IRQ0);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopIRQClr_Disable                                               
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
CSL_Status _CSL_simcopIRQClr_Disable(CSL_SimcopHandle hndl, Uint8 instance,
                                     CSL_SimcopIRQType event, Bool data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case CPU_PROC_START_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_CPU_PROC_START_IRQ, data);
            break;
        case SIMCOP_DMA_IRQ1:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_SIMCOP_DMA_IRQ1, data);
            break;
        case MTCR2OCP_ERR_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_MTCR2OCP_ERR_IRQ, data);
            break;
        case OCP_ERR_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_OCP_ERR_IRQ, data);
            break;
        case VLCDJ_DECODE_ERR_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_VLCDJ_DECODE_ERR_IRQ, data);
            break;
        case DONE_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_DONE_IRQ, data);
            break;
        case STEP3_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_STEP3_IRQ, data);
            break;
        case STEP2_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_STEP2_IRQ, data);
            break;
        case STEP1_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_STEP1_IRQ, data);
            break;
        case STEP0_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_STEP0_IRQ, data);
            break;
        case LDC_BLOCK_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_LDC_BLOCK_IRQ, data);
            break;
        case ROT_A_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_ROT_A_IRQ, data);
            break;
        case IMX_B_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_IMX_B_IRQ, data);
            break;
        case IMX_A_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_IMX_A_IRQ, data);
            break;
        case NSF_IRQ_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_NSF_IRQ_IRQ, data);
            break;
        case VLCDJ_BLOC_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_VLCDJ_BLOC_IRQ, data);
            break;
        case DCT_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_DCT_IRQ, data);
            break;
        case LDC_FRAME_IRQ:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_LDC_FRAME_IRQ, data);
            break;
        case SIMCOP_DMA_IRQ0:
            CSL_FINS(simcopRegs->HL_IRQ[instance].ENABLE_CLR,
                     SIMCOP_ENABLE_CLR_SIMCOP_DMA_IRQ0, data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HL_IRQENABLE_CLR__X(0-3) */
/* SIMCOP_CTRL */

/* ===================================================================
 *  @func     _CSL_simcopCtrl_Get                                               
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
CSL_Status _CSL_simcopCtrl_Get(CSL_SimcopHandle hndl, CSL_SimcopCtrlType event,
                               Uint8 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case LDC_R_BURST_BREAK:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_LDC_R_BURST_BREAK);
            break;
        case LDC_R_MAX_BURST_LENGTH:
            *data =
                CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_LDC_R_MAX_BURST_LENGTH);
            break;
        case LDC_R_TAG_CNT:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_LDC_R_TAG_CNT);
            break;
        case LDC_R_TAG_OFST:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_LDC_R_TAG_OFST);
            break;
        case IMX_B_CMD:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_IMX_B_CMD);
            break;
        case IMX_A_CMD:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_IMX_A_CMD);
            break;
        case HUFF:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_HUFF);
            break;
        case QUANT:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_QUANT);
            break;
        case LDC_LUT:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_LDC_LUT);
            break;
        case LDC_INPUT:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_LDC_INPUT);
            break;
        case NSF_WMEM:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_NSF_WMEM);
            break;
        case IRQ3_MODE:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_IRQ3_MODE);
            break;
        case IRQ2_MODE:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_IRQ2_MODE);
            break;
        case IRQ1_MODE:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_IRQ1_MODE);
            break;
        case IRQ0_MODE:
            *data = CSL_FEXT(simcopRegs->CTRL, SIMCOP_CTRL_IRQ0_MODE);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopCtrl_Set                                               
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
CSL_Status _CSL_simcopCtrl_Set(CSL_SimcopHandle hndl, CSL_SimcopCtrlType event,
                               Uint8 data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case LDC_R_BURST_BREAK:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_LDC_R_BURST_BREAK, data);
            break;
        case LDC_R_MAX_BURST_LENGTH:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_LDC_R_MAX_BURST_LENGTH,
                     data);
            break;
        case LDC_R_TAG_CNT:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_LDC_R_TAG_CNT, data);
            break;
        case LDC_R_TAG_OFST:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_LDC_R_TAG_OFST, data);
            break;
        case IMX_B_CMD:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_IMX_B_CMD, data);
            break;
        case IMX_A_CMD:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_IMX_A_CMD, data);
            break;
        case HUFF:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_HUFF, data);
            break;
        case QUANT:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_QUANT, data);
            break;
        case LDC_LUT:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_LDC_LUT, data);
            break;
        case LDC_INPUT:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_LDC_INPUT, data);
            break;
        case NSF_WMEM:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_NSF_WMEM, data);
            break;
        case IRQ3_MODE:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_IRQ3_MODE, data);
            break;
        case IRQ2_MODE:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_IRQ2_MODE, data);
            break;
        case IRQ1_MODE:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_IRQ1_MODE, data);
            break;
        case IRQ0_MODE:
            CSL_FINS(simcopRegs->CTRL, SIMCOP_CTRL_IRQ0_MODE, data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_CTRL */
/* SIMCOP_CLKCTRL */

/* ===================================================================
 *  @func     _CSL_simcopClkCtrl_Get                                               
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
CSL_Status _CSL_simcopClkCtrl_Get(CSL_SimcopHandle hndl,
                                  CSL_SimcopClkCtrlType event, Bool * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case ROT_A_CLK:
            *data = CSL_FEXT(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_ROT_A_CLK);
            break;
        case IMX_B_CLK:
            *data = CSL_FEXT(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_IMX_B_CLK);
            break;
        case IMX_A_CLK:
            *data = CSL_FEXT(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_IMX_A_CLK);
            break;
        case NSF2_CLK:
            *data = CSL_FEXT(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_NSF2_CLK);
            break;
        case VLCDJ_CLK:
            *data = CSL_FEXT(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_VLCDJ_CLK);
            break;
        case DCT_CLK:
            *data = CSL_FEXT(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_DCT_CLK);
            break;
        case LDC_CLK:
            *data = CSL_FEXT(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_LDC_CLK);
            break;
        case DMA_CLK:
            *data = CSL_FEXT(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_DMA_CLK);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopClkCtrl_Set                                               
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
CSL_Status _CSL_simcopClkCtrl_Set(CSL_SimcopHandle hndl,
                                  CSL_SimcopClkCtrlType event, Bool data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case ROT_A_CLK:
            CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_ROT_A_CLK, data);
            break;
        case IMX_B_CLK:
            CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_IMX_B_CLK, data);
            break;
        case IMX_A_CLK:
            CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_IMX_A_CLK, data);
            break;
        case NSF2_CLK:
            CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_NSF2_CLK, data);
            break;
        case VLCDJ_CLK:
            CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_VLCDJ_CLK, data);
            break;
        case DCT_CLK:
            CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_DCT_CLK, data);
            break;
        case LDC_CLK:
            CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_LDC_CLK, data);
            break;
        case DMA_CLK:
            CSL_FINS(simcopRegs->CLKCTRL, SIMCOP_CLKCTRL_DMA_CLK, data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_CLKCTRL */
/* SIMCOP_HWSEQ_CTRL */

/* ===================================================================
 *  @func     _CSL_simcopHwSeqCtrl_Get                                               
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
CSL_Status _CSL_simcopHwSeqCtrl_Get(CSL_SimcopHandle hndl,
                                    CSL_SimcopHwSeqCtrlType event,
                                    Uint16 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case HW_SEQ_STEP_COUNTER:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_CTRL,
                         SIMCOP_HWSEQ_CTRL_HW_SEQ_STEP_COUNTER);
            break;
        case STEP:
            *data = CSL_FEXT(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_STEP);
            break;
        case BBM_SYNC_CHAN:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_CTRL,
                         SIMCOP_HWSEQ_CTRL_BBM_SYNC_CHAN);
            break;
        case BBM_STATUS:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_BBM_STATUS);
            break;
        case BITSTREAM:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_BITSTREAM);
            break;
        case BITSTR_XFER_SIZE:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_CTRL,
                         SIMCOP_HWSEQ_CTRL_BITSTR_XFER_SIZE);
            break;
        case CPU_PROC_DONE:
        case HW_SEQ_STOP:
        case HW_SEQ_START:
            status = CSL_ESYS_NOTSUPPORTED;
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopHwSeqCtrl_Set                                               
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
CSL_Status _CSL_simcopHwSeqCtrl_Set(CSL_SimcopHandle hndl,
                                    CSL_SimcopHwSeqCtrlType event, Uint16 data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case HW_SEQ_STEP_COUNTER:
            CSL_FINS(simcopRegs->HWSEQ_CTRL,
                     SIMCOP_HWSEQ_CTRL_HW_SEQ_STEP_COUNTER, data);
            break;
        case STEP:
            CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_STEP, data);
            break;
        case CPU_PROC_DONE:
            CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_CPU_PROC_DONE,
                     data);
            break;
        case BBM_SYNC_CHAN:
            CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_BBM_SYNC_CHAN,
                     data);
            break;
        case BITSTREAM:
            CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_BITSTREAM, data);
            break;
        case BITSTR_XFER_SIZE:
            CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_BITSTR_XFER_SIZE,
                     data);
            break;
        case HW_SEQ_STOP:
            CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_HW_SEQ_STOP,
                     data);
            break;
        case HW_SEQ_START:
            CSL_FINS(simcopRegs->HWSEQ_CTRL, SIMCOP_HWSEQ_CTRL_HW_SEQ_START,
                     data);
            break;
        case BBM_STATUS:
            status = CSL_ESYS_NOTSUPPORTED;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HWSEQ_CTRL */
/* SIMCOP_HWSEQ_STATUS */

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStatus_Get                                               
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
CSL_Status _CSL_simcopHwSeqStatus_Get(CSL_SimcopHandle hndl,
                                      CSL_SimcopHwSeqStatusType event,
                                      Uint16 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case HW_SEQ_STEP_COUNTER_STATUS:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STATUS,
                         SIMCOP_HWSEQ_STATUS_HW_SEQ_STEP_COUNTER_STATUS);
            break;
        case STATE:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STATUS, SIMCOP_HWSEQ_STATUS_STATE);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HWSEQ_STATUS */
/* SIMCOP_HWSEQ_OVERRIDE */

/* ===================================================================
 *  @func     _CSL_simcopHwSeqOverride_Get                                               
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
CSL_Status _CSL_simcopHwSeqOverride_Get(CSL_SimcopHandle hndl,
                                        CSL_SimcopHwSeqOverrideType event,
                                        Bool * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case COEFF_B_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_COEFF_B_CTRL);
            break;
        case COEFF_A_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_COEFF_A_CTRL);
            break;
        case IMBUFF_H_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_IMBUFF_H_CTRL);
            break;
        case IMBUFF_G_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_IMBUFF_G_CTRL);
            break;
        case IMBUFF_F_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_IMBUFF_F_CTRL);
            break;
        case IMBUFF_E_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_IMBUFF_E_CTRL);
            break;
        case IMBUFF_D_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_IMBUFF_D_CTRL);
            break;
        case IMBUFF_C_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_IMBUFF_C_CTRL);
            break;
        case IMBUFF_B_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_IMBUFF_B_CTRL);
            break;
        case IMBUFF_A_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_IMBUFF_A_CTRL);
            break;
        case LDC_O_OFST_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_LDC_O_OFST_CTRL);
            break;
        case ROT_O_OFST_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_ROT_O_OFST_CTRL);
            break;
        case ROT_I_OFST_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_ROT_I_OFST_CTRL);
            break;
        case NSF_IO_OFST_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_NSF_IO_OFST_CTRL);
            break;
        case DCT_F_OFST_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_DCT_F_OFST_CTRL);
            break;
        case DCT_S_OFST_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_DCT_S_OFST_CTRL);
            break;
        case VLCDJ_IO_OFST_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_VLCDJ_IO_OFST_CTRL);
            break;
        case IMX_B_D_OFST_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_IMX_B_D_OFST_CTRL);
            break;
        case IMX_A_D_OFST_CTRL:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_OVERRIDE,
                         SIMCOP_HWSEQ_OVERRIDE_IMX_A_D_OFST_CTRL);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopHwSeqOverride_Set                                               
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
CSL_Status _CSL_simcopHwSeqOverride_Set(CSL_SimcopHandle hndl,
                                        CSL_SimcopHwSeqOverrideType event,
                                        Bool data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case COEFF_B_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_COEFF_B_CTRL, data);
            break;
        case COEFF_A_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_COEFF_A_CTRL, data);
            break;
        case IMBUFF_H_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_IMBUFF_H_CTRL, data);
            break;
        case IMBUFF_G_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_IMBUFF_G_CTRL, data);
            break;
        case IMBUFF_F_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_IMBUFF_F_CTRL, data);
            break;
        case IMBUFF_E_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_IMBUFF_E_CTRL, data);
            break;
        case IMBUFF_D_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_IMBUFF_D_CTRL, data);
            break;
        case IMBUFF_C_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_IMBUFF_C_CTRL, data);
            break;
        case IMBUFF_B_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_IMBUFF_B_CTRL, data);
            break;
        case IMBUFF_A_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_IMBUFF_A_CTRL, data);
            break;
        case LDC_O_OFST_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_LDC_O_OFST_CTRL, data);
            break;
        case ROT_O_OFST_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_ROT_O_OFST_CTRL, data);
            break;
        case ROT_I_OFST_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_ROT_I_OFST_CTRL, data);
            break;
        case NSF_IO_OFST_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_NSF_IO_OFST_CTRL, data);
            break;
        case DCT_F_OFST_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_DCT_F_OFST_CTRL, data);
            break;
        case DCT_S_OFST_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_DCT_S_OFST_CTRL, data);
            break;
        case VLCDJ_IO_OFST_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_VLCDJ_IO_OFST_CTRL, data);
            break;
        case IMX_B_D_OFST_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_IMX_B_D_OFST_CTRL, data);
            break;
        case IMX_A_D_OFST_CTRL:
            CSL_FINS(simcopRegs->HWSEQ_OVERRIDE,
                     SIMCOP_HWSEQ_OVERRIDE_IMX_A_D_OFST_CTRL, data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HWSEQ_OVERRIDE */
/* SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE */

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepCtrlOverride_Get                                               
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
CSL_Status _CSL_simcopHwSeqStepCtrlOverride_Get(CSL_SimcopHandle hndl,
                                                CSL_SimcopHwSeqStepCtrlOverrideType
                                                event, Uint8 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case ROT_O_OFST_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_O_OFST_OVR);
            break;
        case ROT_I_OFST_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_I_OFST_OVR);
            break;
        case DCT_F_OFST_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DCT_F_OFST_OVR);
            break;
        case DCT_S_OFST_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DCT_S_OFST_OVR);
            break;
        case VLCDJ_IO_OFST_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_VLCDJ_IO_OFST_OVR);
            break;
        case IMX_B_D_OFST_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_IMX_B_D_OFST_OVR);
            break;
        case IMX_A_D_OFST_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_IMX_A_D_OFST_OVR);
            break;
        case DMA_TRIGGER:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER);
            break;
        case ROT_A_TRIGGER:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_A_TRIGGER);
            break;
        case NSF_TRIGGER:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_NSF_TRIGGER);
            break;
        case VLCDJ_TRIGGER:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_VLCDJ_TRIGGER);
            break;
        case DCT_TRIGGER:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DCT_TRIGGER);
            break;
        case LDC_TRIGGER:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_LDC_TRIGGER);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepCtrlOverride_Set                                               
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
CSL_Status _CSL_simcopHwSeqStepCtrlOverride_Set(CSL_SimcopHandle hndl,
                                                CSL_SimcopHwSeqStepCtrlOverrideType
                                                event, Uint8 data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case ROT_O_OFST_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_O_OFST_OVR, data);
            break;
        case ROT_I_OFST_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_I_OFST_OVR, data);
            break;
        case DCT_F_OFST_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DCT_F_OFST_OVR, data);
            break;
        case DCT_S_OFST_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DCT_S_OFST_OVR, data);
            break;
        case VLCDJ_IO_OFST_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_VLCDJ_IO_OFST_OVR, data);
            break;
        case IMX_B_D_OFST_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_IMX_B_D_OFST_OVR, data);
            break;
        case IMX_A_D_OFST_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_IMX_A_D_OFST_OVR, data);
            break;
        case DMA_TRIGGER:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DMA_TRIGGER, data);
            break;
        case ROT_A_TRIGGER:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_ROT_A_TRIGGER, data);
            break;
        case NSF_TRIGGER:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_NSF_TRIGGER, data);
            break;
        case VLCDJ_TRIGGER:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_VLCDJ_TRIGGER, data);
            break;
        case DCT_TRIGGER:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_DCT_TRIGGER, data);
            break;
        case LDC_TRIGGER:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_LDC_TRIGGER, data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE */
/* SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE */

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepSwitchOverride_Get                                               
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
CSL_Status _CSL_simcopHwSeqStepSwitchOverride_Get(CSL_SimcopHandle hndl,
                                                  CSL_SimcopHwSeqStepSwitchOverrideType
                                                  event, Uint8 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case IMBUFF_H_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_H_OVR);
            break;
        case IMBUFF_G_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_G_OVR);
            break;
        case IMBUFF_F_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_F_OVR);
            break;
        case IMBUFF_E_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_E_OVR);
            break;
        case IMBUFF_D_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_D_OVR);
            break;
        case IMBUFF_C_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_C_OVR);
            break;
        case IMBUFF_B_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_B_OVR);
            break;
        case IMBUFF_A_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_A_OVR);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepSwitchOverride_Set                                               
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
CSL_Status _CSL_simcopHwSeqStepSwitchOverride_Set(CSL_SimcopHandle hndl,
                                                  CSL_SimcopHwSeqStepSwitchOverrideType
                                                  event, Uint8 data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case IMBUFF_H_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_H_OVR, data);
            break;
        case IMBUFF_G_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_G_OVR, data);
            break;
        case IMBUFF_F_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_F_OVR, data);
            break;
        case IMBUFF_E_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_E_OVR, data);
            break;
        case IMBUFF_D_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_D_OVR, data);
            break;
        case IMBUFF_C_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_C_OVR, data);
            break;
        case IMBUFF_B_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_B_OVR, data);
            break;
        case IMBUFF_A_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_A_OVR, data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE */
/* SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE */

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepCtrl2Override_Get                                               
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
CSL_Status _CSL_simcopHwSeqStepCtrl2Override_Get(CSL_SimcopHandle hndl,
                                                 CSL_SimcopHwSeqStepCtrl2OverrideType
                                                 event, Uint8 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case NSF2_IO_OFST_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_NSF2_IO_OFST_OVR);
            break;
        case LDC_O_OFST_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_LDC_O_OFST_OVR);
            break;
        case COEFF_B_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_B_OVR);
            break;
        case COEFF_A_OVR:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE,
                         SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_A_OVR);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepCtrl2Override_Set                                               
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
CSL_Status _CSL_simcopHwSeqStepCtrl2Override_Set(CSL_SimcopHandle hndl,
                                                 CSL_SimcopHwSeqStepCtrl2OverrideType
                                                 event, Uint8 data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case NSF2_IO_OFST_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_NSF2_IO_OFST_OVR, data);
            break;
        case LDC_O_OFST_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_LDC_O_OFST_OVR, data);
            break;
        case COEFF_B_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_B_OVR, data);
            break;
        case COEFF_A_OVR:
            CSL_FINS(simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE,
                     SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_A_OVR, data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE */
/* SIMCOP_HWSEQ_STEP_CTRL__X(0-3) */

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepCtrl_Get                                               
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
CSL_Status _CSL_simcopHwSeqStepCtrl_Get(CSL_SimcopHandle hndl, Uint8 instance,
                                        CSL_SimcopHwSeqStepCtrlType event,
                                        Uint8 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case CPU_SYNC:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_CPU_SYNC);
            break;
        case DMA_OFST:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_DMA_OFST);
            break;
        case ROT_O_OFST:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_ROT_O_OFST);
            break;
        case ROT_I_OFST:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_ROT_I_OFST);
            break;
        case DCT_F_OFST:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_DCT_F_OFST);
            break;
        case DCT_S_OFST:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_DCT_S_OFST);
            break;
        case VLCDJ_IO_OFST:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_VLCDJ_IO_OFST);
            break;
        case IMX_B_D_OFST:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_IMX_B_D_OFST);
            break;
        case IMX_A_D_OFST:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_IMX_A_D_OFST);
            break;
        case NEXT:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_NEXT);
            break;
        case DMA_SYNC:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_DMA_SYNC);
            break;
        case ROT_A_SYNC:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_ROT_A_SYNC);
            break;
        case NSF_SYNC:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_NSF_SYNC);
            break;
        case VLCDJ_SYNC:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_VLCDJ_SYNC);
            break;
        case DCT_SYNC:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_DCT_SYNC);
            break;
        case LDC_SYNC:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                         SIMCOP_STEP_CTRL_LDC_SYNC);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepCtrl_Set                                               
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
CSL_Status _CSL_simcopHwSeqStepCtrl_Set(CSL_SimcopHandle hndl, Uint8 instance,
                                        CSL_SimcopHwSeqStepCtrlType event,
                                        Uint8 data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case CPU_SYNC:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_CPU_SYNC, data);
            break;
        case DMA_OFST:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_DMA_OFST, data);
            break;
        case ROT_O_OFST:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_ROT_O_OFST, data);
            break;
        case ROT_I_OFST:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_ROT_I_OFST, data);
            break;
        case DCT_F_OFST:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_DCT_F_OFST, data);
            break;
        case DCT_S_OFST:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_DCT_S_OFST, data);
            break;
        case VLCDJ_IO_OFST:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_VLCDJ_IO_OFST, data);
            break;
        case IMX_B_D_OFST:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_IMX_B_D_OFST, data);
            break;
        case IMX_A_D_OFST:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_IMX_A_D_OFST, data);
            break;
        case NEXT:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_NEXT, data);
            break;
        case DMA_SYNC:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_DMA_SYNC, data);
            break;
        case ROT_A_SYNC:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_ROT_A_SYNC, data);
            break;
        case NSF_SYNC:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_NSF_SYNC, data);
            break;
        case VLCDJ_SYNC:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_VLCDJ_SYNC, data);
            break;
        case DCT_SYNC:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_DCT_SYNC, data);
            break;
        case LDC_SYNC:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL,
                     SIMCOP_STEP_CTRL_LDC_SYNC, data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HWSEQ_STEP_CTRL__X(0-3) */
/* SIMCOP_HWSEQ_STEP_SWITCH__X(0-3) */

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepSwitch_Get                                               
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
CSL_Status _CSL_simcopHwSeqStepSwitch_Get(CSL_SimcopHandle hndl, Uint8 instance,
                                          CSL_SimcopHwSeqStepSwitchType event,
                                          Uint8 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case IMBUFF_H:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                         SIMCOP_STEP_SWITCH_IMBUFF_H);
            break;
        case IMBUFF_G:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                         SIMCOP_STEP_SWITCH_IMBUFF_G);
            break;
        case IMBUFF_F:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                         SIMCOP_STEP_SWITCH_IMBUFF_F);
            break;
        case IMBUFF_E:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                         SIMCOP_STEP_SWITCH_IMBUFF_E);
            break;
        case IMBUFF_D:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                         SIMCOP_STEP_SWITCH_IMBUFF_D);
            break;
        case IMBUFF_C:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                         SIMCOP_STEP_SWITCH_IMBUFF_C);
            break;
        case IMBUFF_B:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                         SIMCOP_STEP_SWITCH_IMBUFF_B);
            break;
        case IMBUFF_A:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                         SIMCOP_STEP_SWITCH_IMBUFF_A);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepSwitch_Set                                               
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
CSL_Status _CSL_simcopHwSeqStepSwitch_Set(CSL_SimcopHandle hndl, Uint8 instance,
                                          CSL_SimcopHwSeqStepSwitchType event,
                                          Uint8 data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case IMBUFF_H:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_H, data);
            break;
        case IMBUFF_G:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_G, data);
            break;
        case IMBUFF_F:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_F, data);
            break;
        case IMBUFF_E:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_E, data);
            break;
        case IMBUFF_D:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_D, data);
            break;
        case IMBUFF_C:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_C, data);
            break;
        case IMBUFF_B:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_B, data);
            break;
        case IMBUFF_A:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_SWITCH,
                     SIMCOP_STEP_SWITCH_IMBUFF_A, data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HWSEQ_STEP_SWITCH__X(0-3) */
/* SIMCOP_HWSEQ_STEP_IMX_CTRL__X(0-3) */

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepImxCtrl_Get                                               
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
CSL_Status _CSL_simcopHwSeqStepImxCtrl_Get(CSL_SimcopHandle hndl,
                                           Uint8 instance,
                                           CSL_SimcopHwSeqStepImxCtrlType event,
                                           Uint16 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case IMX_B_SYNC:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_IMX_CTRL,
                         SIMCOP_STEP_IMX_CTRL_IMX_B_SYNC);
            break;
        case IMX_B_START:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_IMX_CTRL,
                         SIMCOP_STEP_IMX_CTRL_IMX_B_START);
            break;
        case IMX_A_SYNC:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_IMX_CTRL,
                         SIMCOP_STEP_IMX_CTRL_IMX_A_SYNC);
            break;
        case IMX_A_START:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_IMX_CTRL,
                         SIMCOP_STEP_IMX_CTRL_IMX_A_START);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepImxCtrl_Set                                               
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
CSL_Status _CSL_simcopHwSeqStepImxCtrl_Set(CSL_SimcopHandle hndl,
                                           Uint8 instance,
                                           CSL_SimcopHwSeqStepImxCtrlType event,
                                           Uint16 data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case IMX_B_SYNC:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_IMX_CTRL,
                     SIMCOP_STEP_IMX_CTRL_IMX_B_SYNC, data);
            break;
        case IMX_B_START:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_IMX_CTRL,
                     SIMCOP_STEP_IMX_CTRL_IMX_B_START, data);
            break;
        case IMX_A_SYNC:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_IMX_CTRL,
                     SIMCOP_STEP_IMX_CTRL_IMX_A_SYNC, data);
            break;
        case IMX_A_START:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_IMX_CTRL,
                     SIMCOP_STEP_IMX_CTRL_IMX_A_START, data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HWSEQ_STEP_IMX_CTRL__X(0-3) */
/* SIMCOP_HWSEQ_STEP_CTRL2__X(0-3) */

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepCtrl2_Get                                               
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
CSL_Status _CSL_simcopHwSeqStepCtrl2_Get(CSL_SimcopHandle hndl, Uint8 instance,
                                         CSL_SimcopHwSeqStepCtrl2Type event,
                                         Uint8 * data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    switch (event)
    {
        case NSF2_IO_OFST:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL2,
                         SIMCOP_STEP_CTRL2_NSF2_IO_OFST);
            break;
        case LDC_O_OFST:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL2,
                         SIMCOP_STEP_CTRL2_LDC_O_OFST);
            break;
        case COEFF_B:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL2,
                         SIMCOP_STEP_CTRL2_COEFF_B);
            break;
        case COEFF_A:
            *data =
                CSL_FEXT(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL2,
                         SIMCOP_STEP_CTRL2_COEFF_A);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopHwSeqStepCtrl2_Set                                               
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
CSL_Status _CSL_simcopHwSeqStepCtrl2_Set(CSL_SimcopHandle hndl, Uint8 instance,
                                         CSL_SimcopHwSeqStepCtrl2Type event,
                                         Uint8 data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopRegsOvly simcopRegs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
    simcopRegs = hndl->regs;
    switch (event)
    {
        case NSF2_IO_OFST:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL2,
                     SIMCOP_STEP_CTRL2_NSF2_IO_OFST, data);
            break;
        case LDC_O_OFST:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL2,
                     SIMCOP_STEP_CTRL2_LDC_O_OFST, data);
            break;
        case COEFF_B:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL2,
                     SIMCOP_STEP_CTRL2_COEFF_B, data);
            break;
        case COEFF_A:
            CSL_FINS(simcopRegs->HWSEQ_STEP[instance].STEP_CTRL2,
                     SIMCOP_STEP_CTRL2_COEFF_A, data);
            break;
        default:
            status = CSL_ESYS_INVCMD;
    }
  EXIT:
    return status;
}

/* !SIMCOP_HWSEQ_STEP_CTRL2__X(0-3) */

/* ===================================================================
 *  @func     _CSL_simcopBuffer_Get                                               
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
CSL_Status _CSL_simcopBuffer_Get(CSL_SimcopHandle hndl,
                                 CSL_SimcopBufferType event, void *data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopBufsOvly simcopBufs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_BADHANDLE);
    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    simcopBufs = hndl->bufs;
    switch (event)
    {
        case BITSTREAM_BUFFER:
            memcpy(data, (void *) simcopBufs->BITSTREAM,
                   CSL_BITSTREAM_BUFFER_LEN);
            break;
        case HUFFMAN_TABLES:
            memcpy(data, (void *) simcopBufs->HUFFMAN, CSL_HUFFMAN_TABLES_LEN);
            break;
        case IMXB_CMD_MEM:
            memcpy(data, (void *) simcopBufs->IMX_B_CMDMEM,
                   CSL_IMXB_CMDMEM_LEN);
            break;
        case IMXA_CMD_MEM:
            memcpy(data, (void *) simcopBufs->IMX_A_CMDMEM,
                   CSL_IMXA_CMDMEM_LEN);
            break;
        case QUANTIZATION_TABLE_FOR_ENCODER:
            memcpy(data, (void *) simcopBufs->QUANT, CSL_QUANT_TABLES_LEN >> 1);
            break;
        case QUANTIZATION_TABLE_FOR_DECODER:
            memcpy(data,
                   (void *) ((Uint8 *) simcopBufs->QUANT +
                             (CSL_QUANT_TABLES_LEN >> 1)),
                   CSL_QUANT_TABLES_LEN >> 1);
            break;
        case LDCLUT:
            memcpy(data, (void *) simcopBufs->LDC_LUT, CSL_LDC_LUT_LEN);
            break;
        case IMAGE_BUFFER_A:
            memcpy(data, (void *) simcopBufs->IMGBUFA, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_B:
            memcpy(data, (void *) simcopBufs->IMGBUFB, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_C:
            memcpy(data, (void *) simcopBufs->IMGBUFC, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_D:
            memcpy(data, (void *) simcopBufs->IMGBUFD, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_E:
            memcpy(data, (void *) simcopBufs->IMGBUFE, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_F:
            memcpy(data, (void *) simcopBufs->IMGBUFF, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_G:
            memcpy(data, (void *) simcopBufs->IMGBUFG, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_H:
            memcpy(data, (void *) simcopBufs->IMGBUFH, CSL_IMGBUF_LEN);
            break;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopBuffer_Set                                               
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
CSL_Status _CSL_simcopBuffer_Set(CSL_SimcopHandle hndl,
                                 CSL_SimcopBufferType event, void *data)
{
    CSL_Status status = CSL_SOK;

    CSL_SimcopBufsOvly simcopBufs;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_BADHANDLE);
    CSL_EXIT_IF((data == NULL), CSL_ESYS_INVPARAMS);
    simcopBufs = hndl->bufs;
    switch (event)
    {
        case BITSTREAM_BUFFER:
            memcpy((void *) simcopBufs->BITSTREAM, data,
                   CSL_BITSTREAM_BUFFER_LEN);
            break;
        case HUFFMAN_TABLES:
            memcpy((void *) simcopBufs->HUFFMAN, data, CSL_HUFFMAN_TABLES_LEN);
            break;
        case IMXB_CMD_MEM:
            memcpy((void *) simcopBufs->IMX_B_CMDMEM, data,
                   CSL_IMXB_CMDMEM_LEN);
            break;
        case IMXA_CMD_MEM:
            memcpy((void *) simcopBufs->IMX_A_CMDMEM, data,
                   CSL_IMXA_CMDMEM_LEN);
            break;
        case QUANTIZATION_TABLE_FOR_ENCODER:
            memcpy((void *) simcopBufs->QUANT, data, CSL_QUANT_TABLES_LEN >> 1);
            break;
        case QUANTIZATION_TABLE_FOR_DECODER:
            memcpy((void *) ((Uint8 *) simcopBufs->QUANT +
                             (CSL_QUANT_TABLES_LEN >> 1)), data,
                   CSL_QUANT_TABLES_LEN >> 1);
            break;
        case LDCLUT:
            memcpy((void *) simcopBufs->LDC_LUT, data, CSL_LDC_LUT_LEN);
            break;
        case IMAGE_BUFFER_A:
            memcpy((void *) simcopBufs->IMGBUFA, data, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_B:
            memcpy((void *) simcopBufs->IMGBUFB, data, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_C:
            memcpy((void *) simcopBufs->IMGBUFC, data, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_D:
            memcpy((void *) simcopBufs->IMGBUFD, data, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_E:
            memcpy((void *) simcopBufs->IMGBUFE, data, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_F:
            memcpy((void *) simcopBufs->IMGBUFF, data, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_G:
            memcpy((void *) simcopBufs->IMGBUFG, data, CSL_IMGBUF_LEN);
            break;
        case IMAGE_BUFFER_H:
            memcpy((void *) simcopBufs->IMGBUFH, data, CSL_IMGBUF_LEN);
            break;
    }
  EXIT:
    return status;
}

/* ===================================================================
 *  @func     _CSL_simcopGetBaseAddr                                               
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
CSL_SimcopRegsOvly _CSL_simcopGetBaseAddr(CSL_SimcopNum SimcopNum)
{
    switch (SimcopNum)
    {
        case CSL_SIMCOP_0:
            /* get the SIMCOP unique identifier & resource allocation mask
             * into the handle */
            return CSL_SIMCOP_REGS;
    }
    return NULL;
}

/* ===================================================================
 *  @func     _CSL_simcopGetBaseBufAddr                                               
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
CSL_SimcopBufsOvly _CSL_simcopGetBaseBufAddr(CSL_SimcopNum SimcopNum)
{
    switch (SimcopNum)
    {
        case CSL_SIMCOP_0:
            /* get the SIMCOP unique identifier & resource allocation mask
             * into the handle */
            return CSL_SIMCOP_BUFS;
    }
    return NULL;
}
#endif
/** @brief Gets the unique identifier and  resource allocation mask
 *  specified for the simcop module. This is a CSL internal function.
 *
 */
/* ===================================================================
 *  @func     _CSL_simcopGetAttrs                                               
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
CSL_Status _CSL_simcopGetAttrs(
                                  /* instance number of simcop */
                                  CSL_SimcopNum simcopNum,
                                  /* Pointer to the object that holds
                                   * reference to the instance of simcop
                                   * requested after the call */
                                  CSL_SimcopHandle hSimcop)
{
    CSL_Status status = CSL_SOK;

    switch (simcopNum)
    {
        case CSL_SIMCOP_0:
            /* get the simcop unique identifier & resource allocation mask
             * into the handle */
            hSimcop->xio = CSL_SIMCOP_0_XIO;
            hSimcop->uid = CSL_SIMCOP_0_UID;
            /* get the simcop base address into the handle */
            hSimcop->regs = (CSL_SimcopRegsOvly) CSL_SIMCOP_REGS;
                                                            /*_CSL_simcopGetBaseAddr(simcopNum);*/
            hSimcop->bufs = (CSL_SimcopBufsOvly) CSL_SIMCOP_BUFS;
                                                            /*_CSL_simcopGetBaseBufAddr(simcopNum);*/
            /* get the simcop instance number into the handle */
            hSimcop->perNum = simcopNum;
            break;
        default:
            status = CSL_ESYS_OVFL;
    }

    return (status);
}

/* ===================================================================
 *  @func     _CSL_simcopRegisterReset                                               
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
void _CSL_simcopRegisterReset(CSL_SimcopHandle hndl)
{
    CSL_SimcopRegsOvly simcopRegs = hndl->regs;

    simcopRegs->HL_REVISION = CSL_SIMCOP_HL_REVISION_RESETVAL;
    simcopRegs->HL_HWINFO = CSL_SIMCOP_HL_HWINFO_RESETVAL;
    simcopRegs->HL_SYSCONFIG = CSL_SIMCOP_HL_SYSCONFIG_RESETVAL;
    simcopRegs->HL_IRQ_EOI = CSL_SIMCOP_HL_IRQ_EOI_RESETVAL;
    simcopRegs->CTRL = CSL_SIMCOP_CTRL_RESETVAL;
    simcopRegs->CLKCTRL = CSL_SIMCOP_CLKCTRL_RESETVAL;
    simcopRegs->HWSEQ_CTRL = CSL_SIMCOP_HWSEQ_CTRL_RESETVAL;
    simcopRegs->HWSEQ_STATUS = CSL_SIMCOP_HWSEQ_STATUS_RESETVAL;
    simcopRegs->HWSEQ_OVERRIDE = CSL_SIMCOP_HWSEQ_OVERRIDE_RESETVAL;
    simcopRegs->HWSEQ_STEP_CTRL_OVERRIDE =
        CSL_SIMCOP_HWSEQ_STEP_CTRL_OVERRIDE_RESETVAL;
    simcopRegs->HWSEQ_STEP_SWITCH_OVERRIDE =
        CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_RESETVAL;
    simcopRegs->HWSEQ_STEP_CTRL2_OVERRIDE =
        CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_RESETVAL;
    simcopRegs->HL_IRQ[0].STATUS_RAW = CSL_SIMCOP_STATUS_RAW_RESETVAL;
    simcopRegs->HL_IRQ[0].STATUS = CSL_SIMCOP_STATUS_RESETVAL;
    simcopRegs->HL_IRQ[0].ENABLE_SET = CSL_SIMCOP_ENABLE_SET_RESETVAL;
    simcopRegs->HL_IRQ[0].ENABLE_CLR = CSL_SIMCOP_ENABLE_CLR_RESETVAL;
    simcopRegs->HL_IRQ[1].STATUS_RAW = CSL_SIMCOP_STATUS_RAW_RESETVAL;
    simcopRegs->HL_IRQ[1].STATUS = CSL_SIMCOP_STATUS_RESETVAL;
    simcopRegs->HL_IRQ[1].ENABLE_SET = CSL_SIMCOP_ENABLE_SET_RESETVAL;
    simcopRegs->HL_IRQ[1].ENABLE_CLR = CSL_SIMCOP_ENABLE_CLR_RESETVAL;
    simcopRegs->HL_IRQ[2].STATUS_RAW = CSL_SIMCOP_STATUS_RAW_RESETVAL;
    simcopRegs->HL_IRQ[2].STATUS = CSL_SIMCOP_STATUS_RESETVAL;
    simcopRegs->HL_IRQ[2].ENABLE_SET = CSL_SIMCOP_ENABLE_SET_RESETVAL;
    simcopRegs->HL_IRQ[2].ENABLE_CLR = CSL_SIMCOP_ENABLE_CLR_RESETVAL;
    simcopRegs->HL_IRQ[3].STATUS_RAW = CSL_SIMCOP_STATUS_RAW_RESETVAL;
    simcopRegs->HL_IRQ[3].STATUS = CSL_SIMCOP_STATUS_RESETVAL;
    simcopRegs->HL_IRQ[3].ENABLE_SET = CSL_SIMCOP_ENABLE_SET_RESETVAL;
    simcopRegs->HL_IRQ[3].ENABLE_CLR = CSL_SIMCOP_ENABLE_CLR_RESETVAL;
    simcopRegs->HWSEQ_STEP[0].STEP_CTRL = CSL_SIMCOP_STEP_CTRL_RESETVAL;
    simcopRegs->HWSEQ_STEP[0].STEP_SWITCH = CSL_SIMCOP_STEP_SWITCH_RESETVAL;
    simcopRegs->HWSEQ_STEP[0].STEP_IMX_CTRL = CSL_SIMCOP_STEP_IMX_CTRL_RESETVAL;
    simcopRegs->HWSEQ_STEP[0].STEP_CTRL2 = CSL_SIMCOP_STEP_CTRL2_RESETVAL;
    simcopRegs->HWSEQ_STEP[1].STEP_CTRL = CSL_SIMCOP_STEP_CTRL_RESETVAL;
    simcopRegs->HWSEQ_STEP[1].STEP_SWITCH = CSL_SIMCOP_STEP_SWITCH_RESETVAL;
    simcopRegs->HWSEQ_STEP[1].STEP_IMX_CTRL = CSL_SIMCOP_STEP_IMX_CTRL_RESETVAL;
    simcopRegs->HWSEQ_STEP[1].STEP_CTRL2 = CSL_SIMCOP_STEP_CTRL2_RESETVAL;
    simcopRegs->HWSEQ_STEP[2].STEP_CTRL = CSL_SIMCOP_STEP_CTRL_RESETVAL;
    simcopRegs->HWSEQ_STEP[2].STEP_SWITCH = CSL_SIMCOP_STEP_SWITCH_RESETVAL;
    simcopRegs->HWSEQ_STEP[2].STEP_IMX_CTRL = CSL_SIMCOP_STEP_IMX_CTRL_RESETVAL;
    simcopRegs->HWSEQ_STEP[2].STEP_CTRL2 = CSL_SIMCOP_STEP_CTRL2_RESETVAL;
    simcopRegs->HWSEQ_STEP[3].STEP_CTRL = CSL_SIMCOP_STEP_CTRL_RESETVAL;
    simcopRegs->HWSEQ_STEP[3].STEP_SWITCH = CSL_SIMCOP_STEP_SWITCH_RESETVAL;
    simcopRegs->HWSEQ_STEP[3].STEP_IMX_CTRL = CSL_SIMCOP_STEP_IMX_CTRL_RESETVAL;
    simcopRegs->HWSEQ_STEP[3].STEP_CTRL2 = CSL_SIMCOP_STEP_CTRL2_RESETVAL;
}

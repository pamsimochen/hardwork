/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file _csl_simcop_dma.c
*
* This File contains CSL Level 0 functions for Simcop DMA Module
* This entire description will appear as one 
* paragraph in the generated documentation.
*
* @path  $(CSLPATH)\DMA\src
*
* @rev  00.01
*/
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *!
 *! 16-Apr-2009 Phanish: Changes made because of Resource Manager Integration. 
 *!                       i.e., Removal of dependency on _CSL_certifyOpen() calls.
 *! 19-Mar-2009 Phanish: Modified to remove the HWcontrol and GetHWstatus APIs 
 *!                 and their dependencies. And now, HWsetup calls directly the CSL_FINS
 *!                 Extended the IRQ EN/CLR registers calls to support 8 logical channels
 *!
 *! 23-Sep-2008 Geetha Ravindran:  Created the file.  
 *! 
 *!
 *! 24-Dec-2000 mf: Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */

/****************************************************************
*  INCLUDE FILES                                                 
****************************************************************/
/*-------program files ----------------------------------------*/

#ifndef __CSL_SIMCOP_DMA_C_
#define __CSL_SIMCOP_DMA_C_

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/psp/iss/hal/iss/simcop/common/csl_error.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_resid.h>
    // #include <ti/psp/iss/hal/iss/simcop/simcop_global/_csl_simcop.h>

#include "../inc/_csl_simcop_dma.h"

#if 0
    /* SIMCOP_DMA_REVISION */
    CSL_Status _CSL_simcopDmaGetRev(CSL_SimcopDmaHandle hndl,
                                    CSL_SimcopDmaRevisionType RevType,
                                    Uint16 * data) {
        CSL_SimcopDmaRegsOvly simcopdmaRegs = hndl->regs;
        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        switch (RevType)
        {
            case SCHEME:
                *data =
                    CSL_FEXT(simcopdmaRegs->REVISION,
                             SIMCOP_DMA_REVISION_SCHEME);
                break;
                case FUNC:*data =
                    CSL_FEXT(simcopdmaRegs->REVISION, SIMCOP_DMA_REVISION_FUNC);
                break;
                case R_RTL:*data =
                    CSL_FEXT(simcopdmaRegs->REVISION,
                             SIMCOP_DMA_REVISION_R_RTL);
                break;
                case X_MAJOR:*data =
                    CSL_FEXT(simcopdmaRegs->REVISION,
                             SIMCOP_DMA_REVISION_X_MAJOR);
                break;
                case CUSTOM:*data =
                    CSL_FEXT(simcopdmaRegs->REVISION,
                             SIMCOP_DMA_REVISION_CUSTOM);
                break;
                case Y_MINOR:*data =
                    CSL_FEXT(simcopdmaRegs->REVISION,
                             SIMCOP_DMA_REVISION_Y_MINOR);
                break;
                default:return (CSL_ESYS_INVCMD);
        } return CSL_SOK;
    }
    /* !SIMCOP_DMA_REVISION */

    /* SIMCOP_DMA_HWINFO */
    CSL_Status _CSL_simcopDmaGetNumOfChannels(CSL_SimcopDmaHandle hndl,
                                              Uint16 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(simcopDmaRegs->DMAHWINFO, SIMCOP_DMA_DMAHWINFO_CHAN);
        return CSL_SOK;
    }
    CSL_Status _CSL_simcopGetContext(CSL_SimcopDmaHandle hndl, Uint16 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data =
            CSL_FEXT(simcopDmaRegs->DMAHWINFO, SIMCOP_DMA_DMAHWINFO_CONTEXT);
        return CSL_SOK;
    }

    /* !SIMCOP_DMA_HWINFO */

    /* SIMCOP_DMA_SYSCONFIG */
    // Standby Mode
    CSL_Status _CSL_simcopDmaGetStandByMode(CSL_SimcopDmaHandle hndl,
                                            Uint16 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data =
            CSL_FEXT(simcopDmaRegs->DMASYSCONFIG,
                     SIMCOP_DMA_DMASYSCONFIG_STANDBYMODE);
        return CSL_SOK;
    }
    CSL_Status _CSL_simcopDmaSetStandByMode(CSL_SimcopDmaHandle hndl,
                                            Uint16 data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        CSL_FINS(simcopDmaRegs->DMASYSCONFIG,
                 SIMCOP_DMA_DMASYSCONFIG_STANDBYMODE, data);
        return CSL_SOK;
    }
    // SoftReset
    CSL_Status _CSL_simcopDmaGetSoftReset(CSL_SimcopDmaHandle hndl,
                                          Uint16 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data =
            CSL_FEXT(simcopDmaRegs->DMASYSCONFIG,
                     SIMCOP_DMA_DMASYSCONFIG_SOFTRESET);
        return CSL_SOK;
    }
    CSL_Status _CSL_simcopDmaSetSoftReset(CSL_SimcopDmaHandle hndl, Uint16 data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        CSL_FINS(simcopDmaRegs->DMASYSCONFIG, SIMCOP_DMA_DMASYSCONFIG_SOFTRESET,
                 data);
        return CSL_SOK;
    }
    /* !SIMCOP_DMA_SYSCONFIG */

    /* SIMCOP_DMA_IRQ_EOI */
    // Line Number
    CSL_Status _CSL_simcopDmaGet_IRQ_EOI_LineNumber(CSL_SimcopDmaHandle hndl,
                                                    Uint16 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data =
            CSL_FEXT(simcopDmaRegs->DMAIRQEOI,
                     SIMCOP_DMA_DMAIRQEOI_LINE_NUMBER);
        return CSL_SOK;
    }
    CSL_Status _CSL_simcopDmaSet_IRQ_EOI_LineNumber(CSL_SimcopDmaHandle hndl,
                                                    Uint16 data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        CSL_FINS(simcopDmaRegs->DMAIRQEOI, SIMCOP_DMA_DMAIRQEOI_LINE_NUMBER,
                 data);
        return CSL_SOK;
    }
    /* !SIMCOP_DMA_IRQ_EOI */
    /* SIMCOP_DMA_CTRL */
    CSL_Status _CSL_simcopDmaCtrl_Get(CSL_SimcopDmaHandle hndl,
                                      CSL_SimcopDmaCtrlType event,
                                      Uint16 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        switch (event)
        {
            case BW_LIMITER:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACTRL,
                             SIMCOP_DMA_DMACTRL_BW_LIMITER);
                break;
            case TAG_CNT:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACTRL,
                             SIMCOP_DMA_DMACTRL_TAG_CNT);
                break;
            case POSTED_WRITES:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACTRL,
                             SIMCOP_DMA_DMACTRL_POSTED_WRITES);
                break;
            case MAX_BURST_SIZE:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACTRL,
                             SIMCOP_DMA_DMACTRL_MAX_BURST_SIZE);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }
    CSL_Status _CSL_simcopDmaCtrl_Set(CSL_SimcopDmaHandle hndl,
                                      CSL_SimcopDmaCtrlType event,
                                      Uint16 data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        switch (event)
        {
            case BW_LIMITER:
                CSL_FINS(simcopDmaRegs->DMACTRL, SIMCOP_DMA_DMACTRL_BW_LIMITER,
                         data);
                break;
            case TAG_CNT:
                CSL_FINS(simcopDmaRegs->DMACTRL, SIMCOP_DMA_DMACTRL_TAG_CNT,
                         data);
                break;
            case POSTED_WRITES:
                CSL_FINS(simcopDmaRegs->DMACTRL,
                         SIMCOP_DMA_DMACTRL_POSTED_WRITES, data);
                break;
            case MAX_BURST_SIZE:
                CSL_FINS(simcopDmaRegs->DMACTRL,
                         SIMCOP_DMA_DMACTRL_MAX_BURST_SIZE, data);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }

    /* !SIMCOP_DMA_CTRL */

#endif

    /* SIMCOP_DMA_IRQSTATUS_RAW_X(0-1) */
    CSL_Status _CSL_simcopDmaIRQStatusRAW_Get(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance,
                                              CSL_SimcopDmaIRQGetType event,
                                              Bool * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        switch (event)
        {
            case CHAN7_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN7_FRAME_DONE_IRQ);
                break;
            case CHAN6_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN6_FRAME_DONE_IRQ);
                break;
            case CHAN5_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN5_FRAME_DONE_IRQ);
                break;
            case CHAN4_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN4_FRAME_DONE_IRQ);
                break;
            case CHAN3_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN3_FRAME_DONE_IRQ);
                break;
            case CHAN2_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN2_FRAME_DONE_IRQ);
                break;
            case CHAN1_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN1_FRAME_DONE_IRQ);
                break;
            case CHAN0_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN0_FRAME_DONE_IRQ);
                break;
            case CHAN7_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN7_BLOCK_DONE_IRQ);
                break;
            case CHAN6_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN6_BLOCK_DONE_IRQ);
                break;
            case CHAN5_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN5_BLOCK_DONE_IRQ);
                break;
            case CHAN4_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN4_BLOCK_DONE_IRQ);
                break;
            case CHAN3_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN3_BLOCK_DONE_IRQ);
                break;
            case CHAN2_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN2_BLOCK_DONE_IRQ);
                break;
            case CHAN1_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN1_BLOCK_DONE_IRQ);
                break;
            case CHAN0_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN0_BLOCK_DONE_IRQ);
                break;
            case OCP_ERR:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                    SIMCOP_DMA_DMAIRQSTATUSRAW_OCP_ERR);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }

    CSL_Status _CSL_simcopDmaIRQStatusRAW_Set(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint8 Chan_Num,
                                              CSL_SimcopDmaIRQSetType event,
                                              Bool data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        switch (event)
        {
            case FRAME_DONE:
                switch (Chan_Num)
                {
                    case 0:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN0_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 1:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN1_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 2:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN2_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 3:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN3_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 4:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN4_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 5:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN5_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 6:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN6_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 7:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN7_FRAME_DONE_IRQ,
                                 data);
                        break;
                    default:
                        return (CSL_ESYS_NOTSUPPORTED);
                }
                break;
            case BLOCK_DONE:
                switch (Chan_Num)
                {
                    case 0:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN0_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 1:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN1_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 2:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN2_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 3:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN3_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 4:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN4_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 5:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN5_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 6:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN6_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 7:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                                 SIMCOP_DMA_DMAIRQSTATUSRAW_CHAN7_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    default:
                        return (CSL_ESYS_NOTSUPPORTED);
                }
                break;
            case OCP_ERROR:
                CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUSRAW,
                         SIMCOP_DMA_DMAIRQSTATUSRAW_OCP_ERR, data);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }

    /* !SIMCOP_DMA_IRQSTATUS_RAW_X(0-1) */

    /* SIMCOP_DMA_IRQSTATUS_X(0-1) */
    CSL_Status _CSL_simcopDmaIRQStatus_Get(CSL_SimcopDmaHandle hndl,
                                           Uint8 instance,
                                           CSL_SimcopDmaIRQGetType event,
                                           Bool * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        switch (event)
        {
            case CHAN7_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN7_FRAME_DONE_IRQ);
                break;
            case CHAN6_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN6_FRAME_DONE_IRQ);
                break;
            case CHAN5_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN5_FRAME_DONE_IRQ);
                break;
            case CHAN4_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN4_FRAME_DONE_IRQ);
                break;
            case CHAN3_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN3_FRAME_DONE_IRQ);
                break;
            case CHAN2_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN2_FRAME_DONE_IRQ);
                break;
            case CHAN1_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN1_FRAME_DONE_IRQ);
                break;
            case CHAN0_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN0_FRAME_DONE_IRQ);
                break;
            case CHAN7_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN7_BLOCK_DONE_IRQ);
                break;
            case CHAN6_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN6_BLOCK_DONE_IRQ);
                break;
            case CHAN5_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN5_BLOCK_DONE_IRQ);
                break;
            case CHAN4_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN4_BLOCK_DONE_IRQ);
                break;
            case CHAN3_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN3_BLOCK_DONE_IRQ);
                break;
            case CHAN2_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN2_BLOCK_DONE_IRQ);
                break;
            case CHAN1_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN1_BLOCK_DONE_IRQ);
                break;
            case CHAN0_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_CHAN0_BLOCK_DONE_IRQ);
                break;
            case OCP_ERR:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                    SIMCOP_DMA_DMAIRQSTATUS_OCP_ERR);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }
    CSL_Status _CSL_simcopDmaIRQStatus_Set(CSL_SimcopDmaHandle hndl,
                                           Uint8 instance, Uint8 Chan_Num,
                                           CSL_SimcopDmaIRQSetType event,
                                           Bool data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        switch (event)
        {
            case FRAME_DONE:
                switch (Chan_Num)
                {
                    case 0:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN0_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 1:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN1_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 2:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN2_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 3:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN3_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 4:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN4_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 5:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN5_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 6:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN6_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 7:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN7_FRAME_DONE_IRQ,
                                 data);
                        break;
                    default:
                        return (CSL_ESYS_NOTSUPPORTED);
                }
                break;
            case BLOCK_DONE:
                switch (Chan_Num)
                {
                    case 0:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN0_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 1:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN1_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 2:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN2_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 3:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN3_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 4:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN4_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 5:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN5_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 6:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN6_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 7:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                                 SIMCOP_DMA_DMAIRQSTATUS_CHAN7_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    default:
                        return (CSL_ESYS_NOTSUPPORTED);
                }
                break;
            case OCP_ERROR:
                CSL_FINS(simcopDmaRegs->DMAIRQ[instance].STATUS,
                         SIMCOP_DMA_DMAIRQSTATUS_OCP_ERR, data);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }

    /* !SIMCOP_DMA_IRQSTATUS_X(0-1) */

    /* SIMCOP_DMA_IRQENABLE_SET_X(0-1) */
    CSL_Status _CSL_simcopDmaIRQEnableSet_Get(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance,
                                              CSL_SimcopDmaIRQGetType event,
                                              Bool * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        switch (event)
        {
            case CHAN7_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN7_FRAME_DONE_IRQ);
                break;
            case CHAN6_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN6_FRAME_DONE_IRQ);
                break;
            case CHAN5_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN5_FRAME_DONE_IRQ);
                break;
            case CHAN4_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN4_FRAME_DONE_IRQ);
                break;
            case CHAN3_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN3_FRAME_DONE_IRQ);
                break;
            case CHAN2_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN2_FRAME_DONE_IRQ);
                break;
            case CHAN1_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN1_FRAME_DONE_IRQ);
                break;
            case CHAN0_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN0_FRAME_DONE_IRQ);
                break;
            case CHAN7_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN7_BLOCK_DONE_IRQ);
                break;
            case CHAN6_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN6_BLOCK_DONE_IRQ);
                break;
            case CHAN5_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN5_BLOCK_DONE_IRQ);
                break;
            case CHAN4_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN4_BLOCK_DONE_IRQ);
                break;
            case CHAN3_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN3_BLOCK_DONE_IRQ);
                break;
            case CHAN2_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN2_BLOCK_DONE_IRQ);
                break;
            case CHAN1_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN1_BLOCK_DONE_IRQ);
                break;
            case CHAN0_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_CHAN0_BLOCK_DONE_IRQ);
                break;
            case OCP_ERR:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                    SIMCOP_DMA_DMAIRQENABLESET_OCP_ERR);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }
    CSL_Status _CSL_simcopDmaIRQEnableSet_Set(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint8 Chan_Num,
                                              CSL_SimcopDmaIRQSetType event,
                                              Bool data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        switch (event)
        {
            case FRAME_DONE:
                switch (Chan_Num)
                {
                    case 0:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN0_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 1:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN1_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 2:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN2_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 3:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN3_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 4:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN4_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 5:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN5_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 6:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN6_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 7:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN7_FRAME_DONE_IRQ,
                                 data);
                        break;
                    default:
                        return (CSL_ESYS_NOTSUPPORTED);
                }
                break;
            case BLOCK_DONE:
                switch (Chan_Num)
                {
                    case 0:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN0_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 1:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN1_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 2:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN2_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 3:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN3_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 4:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN4_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 5:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN5_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 6:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN6_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 7:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                                 SIMCOP_DMA_DMAIRQENABLESET_CHAN7_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    default:
                        return (CSL_ESYS_NOTSUPPORTED);
                }
                break;
            case OCP_ERROR:
                CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLESET,
                         SIMCOP_DMA_DMAIRQENABLESET_OCP_ERR, data);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }

    /* !SIMCOP_DMA_IRQENABLE_SET_X(0-1) */

    /* SIMCOP_DMA_IRQENABLE_CLR_X(0-1) */
    CSL_Status _CSL_simcopDmaIRQEnableClr_Get(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance,
                                              CSL_SimcopDmaIRQGetType event,
                                              Bool * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        switch (event)
        {
            case CHAN7_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN7_FRAME_DONE_IRQ);
                break;
            case CHAN6_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN6_FRAME_DONE_IRQ);
                break;
            case CHAN5_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN5_FRAME_DONE_IRQ);
                break;
            case CHAN4_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN4_FRAME_DONE_IRQ);
                break;
            case CHAN3_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN3_FRAME_DONE_IRQ);
                break;
            case CHAN2_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN2_FRAME_DONE_IRQ);
                break;
            case CHAN1_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN1_FRAME_DONE_IRQ);
                break;
            case CHAN0_FRAME_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN0_FRAME_DONE_IRQ);
                break;
            case CHAN7_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN7_BLOCK_DONE_IRQ);
                break;
            case CHAN6_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN6_BLOCK_DONE_IRQ);
                break;
            case CHAN5_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN5_BLOCK_DONE_IRQ);
                break;
            case CHAN4_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN4_BLOCK_DONE_IRQ);
                break;
            case CHAN3_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN3_BLOCK_DONE_IRQ);
                break;
            case CHAN2_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN2_BLOCK_DONE_IRQ);
                break;
            case CHAN1_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN1_BLOCK_DONE_IRQ);
                break;
            case CHAN0_BLOCK_DONE_IRQ:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_CHAN0_BLOCK_DONE_IRQ);
                break;
            case OCP_ERR:
                *data =
                    (Bool) CSL_FEXT(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                    SIMCOP_DMA_DMAIRQENABLECLR_OCP_ERR);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }
    CSL_Status _CSL_simcopDmaIRQEnableClr_Set(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint8 Chan_Num,
                                              CSL_SimcopDmaIRQSetType event,
                                              Bool data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        switch (event)
        {
            case FRAME_DONE:
                switch (Chan_Num)
                {
                    case 0:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN0_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 1:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN1_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 2:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN2_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 3:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN3_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 4:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN4_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 5:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN5_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 6:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN6_FRAME_DONE_IRQ,
                                 data);
                        break;
                    case 7:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN7_FRAME_DONE_IRQ,
                                 data);
                        break;
                    default:
                        return (CSL_ESYS_NOTSUPPORTED);
                }
                break;
            case BLOCK_DONE:
                switch (Chan_Num)
                {
                    case 0:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN0_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 1:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN1_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 2:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN2_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 3:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN3_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 4:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN4_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 5:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN5_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 6:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN6_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    case 7:
                        CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                                 SIMCOP_DMA_DMAIRQENABLECLR_CHAN7_BLOCK_DONE_IRQ,
                                 data);
                        break;
                    default:
                        return (CSL_ESYS_NOTSUPPORTED);
                }
                break;
            case OCP_ERROR:
                CSL_FINS(simcopDmaRegs->DMAIRQ[instance].ENABLECLR,
                         SIMCOP_DMA_DMAIRQENABLECLR_OCP_ERR, data);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }

    /* !SIMCOP_DMA_IRQENABLE_SET_X(0-1) */

#if 0
    /* SIMCOP_DMA_CHAN_CTRL_X(0-7) */
    CSL_Status _CSL_simcopDmaChanCtrl_Get(CSL_SimcopDmaHandle hndl,
                                          Uint8 instance,
                                          CSL_SimcopDmaChanCtrlEnumType event,
                                          Uint16 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        switch (event)
        {
            case HWSTOP:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].CTRL,
                             SIMCOP_DMA_CHANCTRL_HWSTOP);
                break;
            case HWSTART:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].CTRL,
                             SIMCOP_DMA_CHANCTRL_HWSTART);
                break;
            case LINKED:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].CTRL,
                             SIMCOP_DMA_CHANCTRL_LINKED);
                break;
            case GRID:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].CTRL,
                             SIMCOP_DMA_CHANCTRL_GRID);
                break;
            case TILERMODE:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].CTRL,
                             SIMCOP_DMA_CHANCTRL_TILERMODE);
                break;
            case DIR:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].CTRL,
                             SIMCOP_DMA_CHANCTRL_DIR);
                break;
            case STATUS:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].CTRL,
                             SIMCOP_DMA_CHANCTRL_STATUS);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }
    CSL_Status _CSL_simcopDmaChanCtrl_Set(CSL_SimcopDmaHandle hndl,
                                          Uint8 instance,
                                          CSL_SimcopDmaChanCtrlEnumType event,
                                          Uint16 data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        switch (event)
        {
            case HWSTOP:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].CTRL,
                         SIMCOP_DMA_CHANCTRL_HWSTOP, data);
                break;
            case HWSTART:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].CTRL,
                         SIMCOP_DMA_CHANCTRL_HWSTART, data);
                break;
            case LINKED:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].CTRL,
                         SIMCOP_DMA_CHANCTRL_LINKED, data);
                break;
            case GRID:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].CTRL,
                         SIMCOP_DMA_CHANCTRL_GRID, data);
                break;
            case TILERMODE:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].CTRL,
                         SIMCOP_DMA_CHANCTRL_TILERMODE, data);
                break;
            case DIR:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].CTRL,
                         SIMCOP_DMA_CHANCTRL_DIR, data);
                break;
            case SWTRIGGER:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].CTRL,
                         SIMCOP_DMA_CHANCTRL_SWTRIGGER, data);
                break;
            case DISABLE:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].CTRL,
                         SIMCOP_DMA_CHANCTRL_DISABLE, data);
                break;
            case ENABLE:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].CTRL,
                         SIMCOP_DMA_CHANCTRL_ENABLE, data);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }

    /* !SIMCOP_DMA_CHAN_CTRL_X(0-7) */

    /* SIMCOP_DMA_CHAN_SMEM_ADDR_X(0-7) */
    CSL_Status _CSL_simcopDmaChanSmem_Addr_Get(CSL_SimcopDmaHandle hndl,
                                               Uint8 instance, Uint32 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = simcopDmaRegs->DMACHAN[instance].SMEMADDR;
        return CSL_SOK;
    }

    CSL_Status _CSL_simcopDmaChanSmem_Addr_Set(CSL_SimcopDmaHandle hndl,
                                               Uint8 instance, Uint32 data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        simcopDmaRegs->DMACHAN[instance].SMEMADDR = data;
        return CSL_SOK;
    }
    /* !SIMCOP_DMA_CHAN_SMEM_ADDR_X(0-7) */

    /* SIMCOP_DMA_CHAN_SMEM_OFST_X(0-7) */
    CSL_Status _CSL_simcopDmaChanSmem_Ofst_Get(CSL_SimcopDmaHandle hndl,
                                               Uint8 instance, Uint32 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = simcopDmaRegs->DMACHAN[instance].SMEMOFST;
        return CSL_SOK;
    }

    CSL_Status _CSL_simcopDmaChanSmem_Ofst_Set(CSL_SimcopDmaHandle hndl,
                                               Uint8 instance, Uint32 data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        simcopDmaRegs->DMACHAN[instance].SMEMOFST = data;
        return CSL_SOK;
    }
    /* !SIMCOP_DMA_CHAN_SMEM_OFST_X(0-7) */

    /* SIMCOP_DMA_CHAN_BUF_ADDR_X(0-7) */
    CSL_Status _CSL_simcopDmaChanBuf_Addr_Get(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint32 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = simcopDmaRegs->DMACHAN[instance].BUFADDR;
        return CSL_SOK;
    }

    CSL_Status _CSL_simcopDmaChanBuf_Addr_Set(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint32 data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        simcopDmaRegs->DMACHAN[instance].BUFADDR = data;
        return CSL_SOK;
    }
    /* !SIMCOP_DMA_CHAN_BUF_ADDR_X(0-7) */

    /* SIMCOP_DMA_CHAN_BUF_OFST_X(0-7) */
    CSL_Status _CSL_simcopDmaChanBuf_Ofst_Get(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint32 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = simcopDmaRegs->DMACHAN[instance].BUFOFST;
        return CSL_SOK;
    }

    CSL_Status _CSL_simcopDmaChanBuf_Ofst_Set(CSL_SimcopDmaHandle hndl,
                                              Uint8 instance, Uint32 data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        simcopDmaRegs->DMACHAN[instance].BUFOFST = data;
        return CSL_SOK;
    }
    /* !SIMCOP_DMA_CHAN_BUF_OFST_X(0-7) */

    /* SIMCOP_DMA_CHAN_BLOCK_SIZE_X(0-7) */
    CSL_Status _CSL_simcopDmaChanBlkSize_Get(CSL_SimcopDmaHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopDmaChanBlkSizeType event,
                                             Uint16 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        switch (event)
        {
            case YNUM:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].BLOCKSIZE,
                             SIMCOP_DMA_CHANBLOCKSIZE_YNUM);
                break;
            case XNUM:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].BLOCKSIZE,
                             SIMCOP_DMA_CHANBLOCKSIZE_XNUM);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }
    CSL_Status _CSL_simcopDmaChanBlkSize_Set(CSL_SimcopDmaHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopDmaChanBlkSizeType event,
                                             Uint16 data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        switch (event)
        {
            case YNUM:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].BLOCKSIZE,
                         SIMCOP_DMA_CHANBLOCKSIZE_YNUM, data);
                break;
            case XNUM:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].BLOCKSIZE,
                         SIMCOP_DMA_CHANBLOCKSIZE_XNUM, data);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }

    /* !SIMCOP_DMA_CHAN_BLOCK_SIZE_X(0-7) */
    /* SIMCOP_DMA_CHAN_FRAME_X(0-7) */
    CSL_Status _CSL_simcopDmaChanFrame_Get(CSL_SimcopDmaHandle hndl,
                                           Uint8 instance,
                                           CSL_SimcopDmaChanFrameType event,
                                           Uint16 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        switch (event)
        {
            case YCNT:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].FRAME,
                             SIMCOP_DMA_CHANFRAME_YCNT);
                break;
            case XCNT:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].FRAME,
                             SIMCOP_DMA_CHANFRAME_XCNT);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }
    CSL_Status _CSL_simcopDmaChanFrame_Set(CSL_SimcopDmaHandle hndl,
                                           Uint8 instance,
                                           CSL_SimcopDmaChanFrameType event,
                                           Uint16 data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        switch (event)
        {
            case YCNT:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].FRAME,
                         SIMCOP_DMA_CHANFRAME_YCNT, data);
                break;
            case XCNT:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].FRAME,
                         SIMCOP_DMA_CHANFRAME_XCNT, data);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }

    /* !SIMCOP_DMA_CHAN_FRAME_X(0-7) */
    /* SIMCOP_DMA_CHAN_CURRENT_BLOCK_X(0-7) */
    CSL_Status _CSL_simcopDmaChanCurrBlk_Get(CSL_SimcopDmaHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopDmaChanCurrBlkType event,
                                             Uint16 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        switch (event)
        {
            case BY:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].CURRENTBLOCK,
                             SIMCOP_DMA_CHANCURRENTBLOCK_BY);
                break;
            case BX:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].CURRENTBLOCK,
                             SIMCOP_DMA_CHANCURRENTBLOCK_BX);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }

    /* !SIMCOP_DMA_CHAN_CURRENT_BLOCK_X(0-7) */

    /* SIMCOP_DMA_CHAN_BLOCK_STEP_X(0-7) */

    CSL_Status _CSL_simcopDmaChanBlkStep_Get(CSL_SimcopDmaHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopDmaChanBlkStepType event,
                                             Uint16 * data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        switch (event)
        {
            case YSTEP:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].BLOCKSTEP,
                             SIMCOP_DMA_CHANBLOCKSTEP_YSTEP);
                break;
            case XSTEP:
                *data =
                    CSL_FEXT(simcopDmaRegs->DMACHAN[instance].BLOCKSTEP,
                             SIMCOP_DMA_CHANBLOCKSTEP_XSTEP);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }

    CSL_Status _CSL_simcopDmaChanBlkStep_Set(CSL_SimcopDmaHandle hndl,
                                             Uint8 instance,
                                             CSL_SimcopDmaChanBlkStepType event,
                                             Uint16 data) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        simcopDmaRegs = hndl->regs;
        switch (event)
        {
            case YSTEP:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].BLOCKSTEP,
                         SIMCOP_DMA_CHANBLOCKSTEP_YSTEP, data);
                break;
            case XSTEP:
                CSL_FINS(simcopDmaRegs->DMACHAN[instance].BLOCKSTEP,
                         SIMCOP_DMA_CHANBLOCKSTEP_XSTEP, data);
                break;
            default:
                return (CSL_ESYS_INVCMD);
        }
        return CSL_SOK;
    }

    /* !SIMCOP_DMA_CHAN_BLOCK_STEP_X(0-7) */

    CSL_SimcopDmaRegsOvly _CSL_simcopDmaGetBaseAddr(CSL_SimcopDmaNum
                                                    SimcopDmaNum) {
        switch (SimcopDmaNum)
        {
            case CSL_COPDMA_0:
                /* get the LDC unique identifier & resource allocation mask
                 * into the handle */
                return CSL_COPDMA_0_REGS;
        }
        return NULL;
    }
#endif
/*-----------------------------------------------------------------------------------------------------*/
    /* NOT COMPLETED #pragma CODE_SECTION (_CSL_simcopDmaGetAttrs,
     * ".text:csl_section:init"); */
/** @brief Gets the unique identifier and  resource allocation mask
 *  specified for the simcop module. This is a CSL internal function.
 *
 */

    CSL_Status _CSL_simcopDmaGetAttrs(
                                         /* instance number of simcop */
                                         CSL_SimcopDmaNum simcopDmaNum,
                                         /* Pointer to the object that holds
                                          * reference to the instance of
                                          * simcop requested after the call */
                                         CSL_SimcopDmaHandle hSimcopDma) {
        CSL_Status status = CSL_SOK;

        switch (simcopDmaNum)
        {
            case CSL_COPDMA_0:
                /* get the simcop unique identifier & resource allocation
                 * mask into the handle */
                hSimcopDma->xio = CSL_COPDMA_CHA0_XIO;     /* @todo */
                hSimcopDma->uid = CSL_COPDMA_CHA0_UID;     /* @todo */
                /* get the simcop base address into the handle */
                hSimcopDma->regs = (CSL_SimcopDmaRegsOvly) CSL_COPDMA_0_REGS;
                                                                    /*_CSL_simcopDmaGetBaseAddr(simcopDmaNum);*/
                /* get the simcop instance number into the handle */
                hSimcopDma->perNum = simcopDmaNum;
                break;
            default:
                status = CSL_ESYS_OVFL;
        }
        return (status);
    }

    void _CSL_SimcopDma_Reset_Val_Read(CSL_SimcopDmaHandle hndl) {
        CSL_SimcopDmaRegsOvly simcopDmaRegs = hndl->regs;

        simcopDmaRegs->REVISION = CSL_SIMCOP_DMA_REVISION_RESETVAL;
        simcopDmaRegs->DMAHWINFO = CSL_SIMCOP_DMA_DMAHWINFO_RESETVAL;
        simcopDmaRegs->DMASYSCONFIG = CSL_SIMCOP_DMA_DMASYSCONFIG_RESETVAL;
        simcopDmaRegs->DMAIRQEOI = CSL_SIMCOP_DMA_DMAIRQEOI_RESETVAL;
        simcopDmaRegs->DMACTRL = CSL_SIMCOP_DMA_DMACTRL_RESETVAL;
        simcopDmaRegs->DMAIRQ[0].STATUSRAW =
            CSL_SIMCOP_DMA_DMAIRQSTATUSRAW_RESETVAL;
        simcopDmaRegs->DMAIRQ[0].STATUS = CSL_SIMCOP_DMA_DMAIRQSTATUS_RESETVAL;
        simcopDmaRegs->DMAIRQ[0].ENABLESET =
            CSL_SIMCOP_DMA_DMAIRQENABLESET_RESETVAL;
        simcopDmaRegs->DMAIRQ[0].ENABLECLR =
            CSL_SIMCOP_DMA_DMAIRQENABLECLR_RESETVAL;
        simcopDmaRegs->DMAIRQ[1].STATUSRAW =
            CSL_SIMCOP_DMA_DMAIRQSTATUSRAW_RESETVAL;
        simcopDmaRegs->DMAIRQ[1].STATUS = CSL_SIMCOP_DMA_DMAIRQSTATUS_RESETVAL;
        simcopDmaRegs->DMAIRQ[1].ENABLESET =
            CSL_SIMCOP_DMA_DMAIRQENABLESET_RESETVAL;
        simcopDmaRegs->DMAIRQ[1].ENABLECLR =
            CSL_SIMCOP_DMA_DMAIRQENABLECLR_RESETVAL;
        simcopDmaRegs->DMACHAN[0].CTRL = CSL_SIMCOP_DMA_CHANCTRL_RESETVAL;
        simcopDmaRegs->DMACHAN[0].SMEMADDR =
            CSL_SIMCOP_DMA_CHANSMEMADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[0].SMEMOFST =
            CSL_SIMCOP_DMA_CHANSMEMOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[0].BUFOFST = CSL_SIMCOP_DMA_CHANBUFOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[0].BUFADDR = CSL_SIMCOP_DMA_CHANBUFADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[0].BLOCKSIZE =
            CSL_SIMCOP_DMA_CHANBLOCKSIZE_RESETVAL;
        simcopDmaRegs->DMACHAN[0].FRAME = CSL_SIMCOP_DMA_CHANFRAME_RESETVAL;
        simcopDmaRegs->DMACHAN[0].CURRENTBLOCK =
            CSL_SIMCOP_DMA_CHANCURRENTBLOCK_RESETVAL;
        simcopDmaRegs->DMACHAN[0].BLOCKSTEP =
            CSL_SIMCOP_DMA_CHANBLOCKSTEP_RESETVAL;
        simcopDmaRegs->DMACHAN[1].CTRL = CSL_SIMCOP_DMA_CHANCTRL_RESETVAL;
        simcopDmaRegs->DMACHAN[1].SMEMADDR =
            CSL_SIMCOP_DMA_CHANSMEMADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[1].SMEMOFST =
            CSL_SIMCOP_DMA_CHANSMEMOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[1].BUFOFST = CSL_SIMCOP_DMA_CHANBUFOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[1].BUFADDR = CSL_SIMCOP_DMA_CHANBUFADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[1].BLOCKSIZE =
            CSL_SIMCOP_DMA_CHANBLOCKSIZE_RESETVAL;
        simcopDmaRegs->DMACHAN[1].FRAME = CSL_SIMCOP_DMA_CHANFRAME_RESETVAL;
        simcopDmaRegs->DMACHAN[1].CURRENTBLOCK =
            CSL_SIMCOP_DMA_CHANCURRENTBLOCK_RESETVAL;
        simcopDmaRegs->DMACHAN[1].BLOCKSTEP =
            CSL_SIMCOP_DMA_CHANBLOCKSTEP_RESETVAL;
        simcopDmaRegs->DMACHAN[2].CTRL = CSL_SIMCOP_DMA_CHANCTRL_RESETVAL;
        simcopDmaRegs->DMACHAN[2].SMEMADDR =
            CSL_SIMCOP_DMA_CHANSMEMADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[2].SMEMOFST =
            CSL_SIMCOP_DMA_CHANSMEMOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[2].BUFOFST = CSL_SIMCOP_DMA_CHANBUFOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[2].BUFADDR = CSL_SIMCOP_DMA_CHANBUFADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[2].BLOCKSIZE =
            CSL_SIMCOP_DMA_CHANBLOCKSIZE_RESETVAL;
        simcopDmaRegs->DMACHAN[2].FRAME = CSL_SIMCOP_DMA_CHANFRAME_RESETVAL;
        simcopDmaRegs->DMACHAN[2].CURRENTBLOCK =
            CSL_SIMCOP_DMA_CHANCURRENTBLOCK_RESETVAL;
        simcopDmaRegs->DMACHAN[2].BLOCKSTEP =
            CSL_SIMCOP_DMA_CHANBLOCKSTEP_RESETVAL;
        simcopDmaRegs->DMACHAN[3].CTRL = CSL_SIMCOP_DMA_CHANCTRL_RESETVAL;
        simcopDmaRegs->DMACHAN[3].SMEMADDR =
            CSL_SIMCOP_DMA_CHANSMEMADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[3].SMEMOFST =
            CSL_SIMCOP_DMA_CHANSMEMOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[3].BUFOFST = CSL_SIMCOP_DMA_CHANBUFOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[3].BUFADDR = CSL_SIMCOP_DMA_CHANBUFADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[3].BLOCKSIZE =
            CSL_SIMCOP_DMA_CHANBLOCKSIZE_RESETVAL;
        simcopDmaRegs->DMACHAN[3].FRAME = CSL_SIMCOP_DMA_CHANFRAME_RESETVAL;
        simcopDmaRegs->DMACHAN[3].CURRENTBLOCK =
            CSL_SIMCOP_DMA_CHANCURRENTBLOCK_RESETVAL;
        simcopDmaRegs->DMACHAN[3].BLOCKSTEP =
            CSL_SIMCOP_DMA_CHANBLOCKSTEP_RESETVAL;
        simcopDmaRegs->DMACHAN[4].CTRL = CSL_SIMCOP_DMA_CHANCTRL_RESETVAL;
        simcopDmaRegs->DMACHAN[4].SMEMADDR =
            CSL_SIMCOP_DMA_CHANSMEMADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[4].SMEMOFST =
            CSL_SIMCOP_DMA_CHANSMEMOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[4].BUFOFST = CSL_SIMCOP_DMA_CHANBUFOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[4].BUFADDR = CSL_SIMCOP_DMA_CHANBUFADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[4].BLOCKSIZE =
            CSL_SIMCOP_DMA_CHANBLOCKSIZE_RESETVAL;
        simcopDmaRegs->DMACHAN[4].FRAME = CSL_SIMCOP_DMA_CHANFRAME_RESETVAL;
        simcopDmaRegs->DMACHAN[4].CURRENTBLOCK =
            CSL_SIMCOP_DMA_CHANCURRENTBLOCK_RESETVAL;
        simcopDmaRegs->DMACHAN[4].BLOCKSTEP =
            CSL_SIMCOP_DMA_CHANBLOCKSTEP_RESETVAL;
        simcopDmaRegs->DMACHAN[5].CTRL = CSL_SIMCOP_DMA_CHANCTRL_RESETVAL;
        simcopDmaRegs->DMACHAN[5].SMEMADDR =
            CSL_SIMCOP_DMA_CHANSMEMADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[5].SMEMOFST =
            CSL_SIMCOP_DMA_CHANSMEMOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[5].BUFOFST = CSL_SIMCOP_DMA_CHANBUFOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[5].BUFADDR = CSL_SIMCOP_DMA_CHANBUFADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[5].BLOCKSIZE =
            CSL_SIMCOP_DMA_CHANBLOCKSIZE_RESETVAL;
        simcopDmaRegs->DMACHAN[5].FRAME = CSL_SIMCOP_DMA_CHANFRAME_RESETVAL;
        simcopDmaRegs->DMACHAN[5].CURRENTBLOCK =
            CSL_SIMCOP_DMA_CHANCURRENTBLOCK_RESETVAL;
        simcopDmaRegs->DMACHAN[5].BLOCKSTEP =
            CSL_SIMCOP_DMA_CHANBLOCKSTEP_RESETVAL;
        simcopDmaRegs->DMACHAN[6].CTRL = CSL_SIMCOP_DMA_CHANCTRL_RESETVAL;
        simcopDmaRegs->DMACHAN[6].SMEMADDR =
            CSL_SIMCOP_DMA_CHANSMEMADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[6].SMEMOFST =
            CSL_SIMCOP_DMA_CHANSMEMOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[6].BUFOFST = CSL_SIMCOP_DMA_CHANBUFOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[6].BUFADDR = CSL_SIMCOP_DMA_CHANBUFADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[6].BLOCKSIZE =
            CSL_SIMCOP_DMA_CHANBLOCKSIZE_RESETVAL;
        simcopDmaRegs->DMACHAN[6].FRAME = CSL_SIMCOP_DMA_CHANFRAME_RESETVAL;
        simcopDmaRegs->DMACHAN[6].CURRENTBLOCK =
            CSL_SIMCOP_DMA_CHANCURRENTBLOCK_RESETVAL;
        simcopDmaRegs->DMACHAN[6].BLOCKSTEP =
            CSL_SIMCOP_DMA_CHANBLOCKSTEP_RESETVAL;
        simcopDmaRegs->DMACHAN[7].CTRL = CSL_SIMCOP_DMA_CHANCTRL_RESETVAL;
        simcopDmaRegs->DMACHAN[7].SMEMADDR =
            CSL_SIMCOP_DMA_CHANSMEMADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[7].SMEMOFST =
            CSL_SIMCOP_DMA_CHANSMEMOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[7].BUFOFST = CSL_SIMCOP_DMA_CHANBUFOFST_RESETVAL;
        simcopDmaRegs->DMACHAN[7].BUFADDR = CSL_SIMCOP_DMA_CHANBUFADDR_RESETVAL;
        simcopDmaRegs->DMACHAN[7].BLOCKSIZE =
            CSL_SIMCOP_DMA_CHANBLOCKSIZE_RESETVAL;
        simcopDmaRegs->DMACHAN[7].FRAME = CSL_SIMCOP_DMA_CHANFRAME_RESETVAL;
        simcopDmaRegs->DMACHAN[7].CURRENTBLOCK =
            CSL_SIMCOP_DMA_CHANCURRENTBLOCK_RESETVAL;
        simcopDmaRegs->DMACHAN[7].BLOCKSTEP =
            CSL_SIMCOP_DMA_CHANBLOCKSTEP_RESETVAL;

    }

#endif

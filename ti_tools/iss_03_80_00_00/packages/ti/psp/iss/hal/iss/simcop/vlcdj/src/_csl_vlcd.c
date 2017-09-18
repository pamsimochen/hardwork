/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file _csl_vlcd.c
*
* This file contains register level values for VLCDJ module SIMCOP in OMAP4/Monica
*
* @path VLCDJ\src
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 8-August 2008 Sowmya Priya: Initial Release
*! 9-Sep     2008 Sowmya Priya : Updates in function Protypes
*! 19-Mar-2009 Phanish: Modified to remove the HWcontrol and GetHWstatus APIs 
*!                 and their dependencies. And now, HWsetup calls directly the CSL_FINS 
*! 16-Apr-2009 Phanish: Changes made because of Resource Manager Integration. 
*!                       i.e., Removal of dependency on _CSL_certifyOpen() calls.
*========================================================================= */
#ifndef __CSLR_VLCD_C_
#define __CSLR_VLCD_C_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/
#include "../cslr__vlcdj_1.h"
#include "../csl_vlcd.h"
#include "../inc/_csl_vlcd.h"

#if 0
/*****************************************************************************/
    /* individual functions to Set writable registers in the * VLCDJ module
     * of SIMCOP */
 /*****************************************************************************/
    CSL_Status _CSL_VlcdjSetRevId(CSL_VlcdjHandle hndl, Uint16 data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;
        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
         CSL_FINS(VlcdjRegs->VLCDJ_REVISION, VLCDJ_VLCDJ_REVISION_REV, data);
         return CSL_SOK;
    } CSL_Status _CSL_VlcdjEnable(CSL_VlcdjHandle hndl,
                                  CSL_VlcdjHwEnableType data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_EN, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetModeType(CSL_VlcdjHandle hndl,
                                     CSL_VlcdjModeType data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_MODE, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetTrigSrcType(CSL_VlcdjHandle hndl,
                                        CSL_VlcdjTrgSrcType data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_TRIG_SRC, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetAutoGating(CSL_VlcdjHandle hndl,
                                       CSL_VlcdjAutoGatingType data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_AUTOGATING, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetInterruptEnable(CSL_VlcdjHandle hndl,
                                            CSL_VlcdjIntEnableType data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_INTEN_DONE, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetInterruptDecErr(CSL_VlcdjHandle hndl, Uint16 data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_INTEN_ERR, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetRbEnable(CSL_VlcdjHandle hndl, Uint16 data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_RBEN, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetClrrb(CSL_VlcdjHandle hndl, Uint16 data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_CLRRB, data);
        return CSL_SOK;
    }

    /* VLCDJ Encode CFG settings */
    CSL_Status _CSL_VlcdjSetEncNMcus(CSL_VlcdjHandle hndl, Uint16 data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJE_CFG, VLCDJ_VLCDJE_CFG_NMCUS, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetEncFmtDataType(CSL_VlcdjHandle hndl,
                                           CSL_VlcdjEncodeFmtType data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJE_CFG, VLCDJ_VLCDJE_CFG_FMT, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetRSTEnc(CSL_VlcdjHandle hndl, Uint16 data,
                                   CSL_VlcdjRstEnType type) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        switch (type)
        {
            case CSL_VLCDJ_RSTEN:
                CSL_FINS(VlcdjRegs->VLCDJE_CFG, VLCDJ_VLCDJE_CFG_RSTEN, data);
                break;

            case CSL_VLCDJ_RLOCEN:
                CSL_FINS(VlcdjRegs->VLCDJE_CFG, VLCDJ_VLCDJE_CFG_RLOCEN, data);
                break;

            default:
                return (CSL_ESYS_INVPARAMS);

        }
        return CSL_SOK;
    }

    /* DC predictor encode settings */

    CSL_Status _CSL_VlcdjSetPREDEnc(CSL_VlcdjHandle hndl, Uint16 data,
                                    CSL_VlcdjDCPredictorType dcpred) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);

        switch (dcpred)
        {
            case CSL_VLCDJ_DCPREDY:
                CSL_FINS(VlcdjRegs->VLCDJE_DCPREDY, VLCDJ_VLCDJE_DCPREDY_PREDY,
                         data);
                break;
            case CSL_VLCDJ_DCPREDU:
                CSL_FINS(VlcdjRegs->VLCDJE_DCPREDUV,
                         VLCDJ_VLCDJE_DCPREDUV_PREDU, data);
                break;
            case CSL_VLCDJ_DCPREDV:
                CSL_FINS(VlcdjRegs->VLCDJE_DCPREDUV,
                         VLCDJ_VLCDJE_DCPREDUV_PREDV, data);
                break;

            default:
                return (CSL_ESYS_INVPARAMS);

        }
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetEncPtrTable(CSL_VlcdjHandle hndl, Uint16 data,
                                        CSL_VlcdjPtrType ptrtype) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);

        switch (ptrtype)

        {
            case CSL_VLCDJ_BSP_BITPTR:
                CSL_FINS(VlcdjRegs->VLCDJE_BSPTR, VLCDJ_VLCDJE_BSPTR_BITPTR,
                         data);
                break;

            case CSL_VLCDJ_BSP_BYTEPTR:
                CSL_FINS(VlcdjRegs->VLCDJE_BSPTR, VLCDJ_VLCDJE_BSPTR_BYTEPTR,
                         data);
                break;

            case CSL_VLCDJ_CBUF_START:
                CSL_FINS(VlcdjRegs->VLCDJE_CBUF, VLCDJ_VLCDJE_CBUF_START, data);
                break;

            case CSL_VLCDJ_CBUF_END:
                CSL_FINS(VlcdjRegs->VLCDJE_CBUF, VLCDJ_VLCDJE_CBUF_END, data);
                break;

            case CSL_VLCDJ_QMR_PTR:                       /* QUANTIZATION
                                                            * MATRIX */
                CSL_FINS(VlcdjRegs->VLCDJE_DCTQM, VLCDJ_VLCDJE_DCTQM_QMR, data);
                break;

            case CSL_VLCDJ_DCT_PTR:                       /* DCT TABLES */
                CSL_FINS(VlcdjRegs->VLCDJE_DCTQM, VLCDJ_VLCDJE_DCTQM_DCT, data);
                // VlcdjRegs->VLCDJE_DCTQM=(VlcdjRegs->VLCDJE_DCTQM) | data ;
                break;

            case CSL_VLCDJ_HUFF_PTR:
                CSL_FINS(VlcdjRegs->VLCDJE_VLCTBL, VLCDJ_VLCDJE_VLCTBL_ADDR,
                         data);
                // VlcdjRegs->VLCDJE_VLCTBL=(VlcdjRegs->VLCDJE_VLCTBL) | data 
                // ;
                break;

            case CSL_VLCDJ_RST_PTR:
                CSL_FINS(VlcdjRegs->VLCDJE_RSTPTR, VLCDJ_VLCDJE_RSTPTR_ADDR,
                         data);
                // VlcdjRegs->VLCDJE_RSTPTR = (VlcdjRegs->VLCDJE_RSTPTR) |
                // data ;
                break;

            default:
                return (CSL_ESYS_INVPARAMS);

        }
        return CSL_SOK;
    }

    /* settings for Restart Marker configuration */

    CSL_Status _CSL_VlcdjSetRSTConfig(CSL_VlcdjHandle hndl, Uint16 data,
                                      CSL_VlcdjRstConfigType rstconfig) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);

        switch (rstconfig)

        {
            case CSL_VLCDJ_INC:
                CSL_FINS(VlcdjRegs->VLCDJE_RSTCFG, VLCDJ_VLCDJE_RSTCFG_INC,
                         data);
                break;

            case CSL_VLCDJ_INIT:
                CSL_FINS(VlcdjRegs->VLCDJE_RSTCFG, VLCDJ_VLCDJE_RSTCFG_INIT,
                         data);
                break;

            case CSL_VLCDJ_PHASE:
                CSL_FINS(VlcdjRegs->VLCDJE_RSTCFG, VLCDJ_VLCDJE_RSTCFG_PHASE,
                         data);
                break;

            case CSL_VLCDJ_INTRVL:
                CSL_FINS(VlcdjRegs->VLCDJE_RSTCFG, VLCDJ_VLCDJE_RSTCFG_INTRVL,
                         data);
                break;

            default:
                return (CSL_ESYS_INVPARAMS);

        }
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetEncRSTOffset(CSL_VlcdjHandle hndl, Uint32 data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJE_RSTOFST, VLCDJ_VLCDJE_RSTOFST_OFFSET, data);
        return CSL_SOK;

    }

    /* VLCDJ Decode CFG settings */

    CSL_Status _CSL_VlcdjSetDecNMcus(CSL_VlcdjHandle hndl, Uint16 data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJD_CFG, VLCDJ_VLCDJD_CFG_NMCUS, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetDecFmtDataType(CSL_VlcdjHandle hndl,
                                           CSL_VlcdjDecodeFmtType data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJD_CFG, VLCDJ_VLCDJD_CFG_FMT, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetRSTDec(CSL_VlcdjHandle hndl, Uint16 data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(VlcdjRegs->VLCDJD_CFG, VLCDJ_VLCDJD_CFG_RSTEN, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetPREDDec(CSL_VlcdjHandle hndl, Uint16 data,
                                    CSL_VlcdjDCPredictorType dcpred) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);

        switch (dcpred)

        {
            case CSL_VLCDJ_DCPREDY:
                CSL_FINS(VlcdjRegs->VLCDJD_DCPREDY, VLCDJ_VLCDJD_DCPREDY_PREDY,
                         data);
                break;
            case CSL_VLCDJ_DCPREDU:
                CSL_FINS(VlcdjRegs->VLCDJD_DCPREDUV,
                         VLCDJ_VLCDJD_DCPREDUV_PREDU, data);
                break;
            case CSL_VLCDJ_DCPREDV:
                CSL_FINS(VlcdjRegs->VLCDJD_DCPREDUV,
                         VLCDJ_VLCDJD_DCPREDUV_PREDV, data);
                break;

            default:
                return (CSL_ESYS_INVPARAMS);

        }
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjSetDecPtrTable(CSL_VlcdjHandle hndl, Uint16 data,
                                        CSL_VlcdjPtrType ptrtype) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);

        switch (ptrtype)

        {
            case CSL_VLCDJ_BSP_BITPTR:
                CSL_FINS(VlcdjRegs->VLCDJD_BSPTR, VLCDJ_VLCDJD_BSPTR_BITPTR,
                         data);
                break;

            case CSL_VLCDJ_BSP_BYTEPTR:
                CSL_FINS(VlcdjRegs->VLCDJD_BSPTR, VLCDJ_VLCDJD_BSPTR_BYTEPTR,
                         data);
                break;

            case CSL_VLCDJ_CBUF_START:
                CSL_FINS(VlcdjRegs->VLCDJD_CBUF, VLCDJ_VLCDJD_CBUF_START, data);
                break;

            case CSL_VLCDJ_CBUF_END:
                CSL_FINS(VlcdjRegs->VLCDJD_CBUF, VLCDJ_VLCDJD_CBUF_END, data);
                break;

            case CSL_VLCDJ_QMR_PTR:                       /* QUANTIZATION
                                                            * MATRIX */
                CSL_FINS(VlcdjRegs->VLCDJD_DCTQM, VLCDJ_VLCDJD_DCTQM_QMR, data);
                break;

            case CSL_VLCDJ_DCT_PTR:                       /* DCT TABLES */
                CSL_FINS(VlcdjRegs->VLCDJD_DCTQM, VLCDJ_VLCDJD_DCTQM_DCT, data);
                // VlcdjRegs->VLCDJD_DCTQM = data &
                // CSL_VLCDJ_VLCDJD_DCTQM_DCT_MASK;
                break;

            case CSL_VLCDJ_DEC_CTRLTBL:                   /* HUFF CONTROL
                                                            * TABLES */
                CSL_FINS(VlcdjRegs->VLCDJD_CTRLTBL, VLCDJ_VLCDJD_CTRLTBL_ADDR,
                         data);
                // VlcdjRegs->VLCDJD_CTRLTBL = data &
                // CSL_VLCDJ_VLCDJD_CTRLTBL_ADDR_MASK;
                break;

            case CSL_VLCDJ_DEC_HUFF0:                     /* HUFF TABLES */
                CSL_FINS(VlcdjRegs->VLCDJD_DCDTBL01,
                         VLCDJ_VLCDJD_DCDTBL01_DCDTBL0, data);
                break;

            case CSL_VLCDJ_DEC_HUFF1:                     /* HUFF TABLES */
                CSL_FINS(VlcdjRegs->VLCDJD_DCDTBL01,
                         VLCDJ_VLCDJD_DCDTBL01_DCDTBL1, data);
                break;

            case CSL_VLCDJ_DEC_HUFF2:                     /* HUFF TABLES */
                CSL_FINS(VlcdjRegs->VLCDJD_DCDTBL23,
                         VLCDJ_VLCDJD_DCDTBL23_DCDTBL2, data);
                break;

            case CSL_VLCDJ_DEC_HUFF3:                     /* HUFF TABLES */
                CSL_FINS(VlcdjRegs->VLCDJD_DCDTBL23,
                         VLCDJ_VLCDJD_DCDTBL23_DCDTBL3, data);
                break;

            default:
                return (CSL_ESYS_INVPARAMS);

        }

        return CSL_SOK;

    }

/*****************************************************************************/
    /* individual functions to Query readable registers in the * VLCDJ
     * module of SIMCOP */
 /*****************************************************************************/

    CSL_Status _CSL_VlcdjGetRevId(CSL_VlcdjHandle hndl, Uint16 * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(VlcdjRegs->VLCDJ_REVISION, VLCDJ_VLCDJ_REVISION_REV);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetModeType(CSL_VlcdjHandle hndl,
                                     CSL_VlcdjModeType * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *((Uint16 *) data) =
            CSL_FEXT(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_MODE);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetTrigSrcType(CSL_VlcdjHandle hndl,
                                        CSL_VlcdjTrgSrcType * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *((Uint16 *) data) =
            CSL_FEXT(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_TRIG_SRC);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetAutoGating(CSL_VlcdjHandle hndl,
                                       CSL_VlcdjAutoGatingType * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *((Uint16 *) data) =
            CSL_FEXT(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_AUTOGATING);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetnterruptEnable(CSL_VlcdjHandle hndl, Uint16 * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_INTEN_DONE);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetInterruptDecErr(CSL_VlcdjHandle hndl, Uint16 * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_INTEN_ERR);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetRbEnable(CSL_VlcdjHandle hndl, Uint16 * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_RBEN);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetState(CSL_VlcdjHandle hndl,
                                  CSL_VlcdjHwStatusType * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *((Uint16 *) data) =
            CSL_FEXT(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_BUSY);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjStatusBnk0(CSL_VlcdjHandle hndl, Uint16 * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_RB0_STATUS);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjStatusBnk1(CSL_VlcdjHandle hndl, Uint16 * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_RB1_STATUS);
        return CSL_SOK;
    }

    /* VLCDJ Encode CFG Query */

    CSL_Status _CSL_VlcdjGetEncNMcus(CSL_VlcdjHandle hndl, Uint16 * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(VlcdjRegs->VLCDJE_CFG, VLCDJ_VLCDJE_CFG_NMCUS);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetEncFmtDataType(CSL_VlcdjHandle hndl,
                                           CSL_VlcdjEncodeFmtType * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *((Uint16 *) data) =
            CSL_FEXT(VlcdjRegs->VLCDJE_CFG, VLCDJ_VLCDJE_CFG_FMT);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetRSTEnc(CSL_VlcdjHandle hndl, Uint16 * data,
                                   CSL_VlcdjRstEnType type) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);

        switch (type)

        {
            case CSL_VLCDJ_RSTEN:
                *data = CSL_FEXT(VlcdjRegs->VLCDJE_CFG, VLCDJ_VLCDJE_CFG_RSTEN);
                break;

            case CSL_VLCDJ_RLOCEN:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJE_CFG, VLCDJ_VLCDJE_CFG_RLOCEN);
                break;

            default:
                return (CSL_ESYS_INVPARAMS);

        }
        return CSL_SOK;
    }

    /* DC predictor encode Query */

    CSL_Status _CSL_VlcdjGetPREDEnc(CSL_VlcdjHandle hndl, Uint16 * data,
                                    CSL_VlcdjDCPredictorType dcpred) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);

        switch (dcpred)

        {
            case CSL_VLCDJ_DCPREDY:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJE_DCPREDY,
                             VLCDJ_VLCDJE_DCPREDY_PREDY);
                break;
            case CSL_VLCDJ_DCPREDU:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJE_DCPREDUV,
                             VLCDJ_VLCDJE_DCPREDUV_PREDU);
                break;
            case CSL_VLCDJ_DCPREDV:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJE_DCPREDUV,
                             VLCDJ_VLCDJE_DCPREDUV_PREDV);
                break;

            default:
                return (CSL_ESYS_INVPARAMS);

        }
        return CSL_SOK;
    }

    /* Query for different encode PTR tables */

    CSL_Status _CSL_VlcdjGetEncPtrTable(CSL_VlcdjHandle hndl, Uint16 * data,
                                        CSL_VlcdjPtrType ptrtype) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);

        switch (ptrtype)

        {
            case CSL_VLCDJ_BSP_BITPTR:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJE_BSPTR,
                             VLCDJ_VLCDJE_BSPTR_BITPTR);
                break;

            case CSL_VLCDJ_BSP_BYTEPTR:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJE_BSPTR,
                             VLCDJ_VLCDJE_BSPTR_BYTEPTR);
                break;

            case CSL_VLCDJ_CBUF_START:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJE_CBUF, VLCDJ_VLCDJE_CBUF_START);
                break;

            case CSL_VLCDJ_CBUF_END:
                *data = CSL_FEXT(VlcdjRegs->VLCDJE_CBUF, VLCDJ_VLCDJE_CBUF_END);
                break;

            case CSL_VLCDJ_QMR_PTR:                       /* QUANTIZATION
                                                            * MATRIX */
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJE_DCTQM, VLCDJ_VLCDJE_DCTQM_QMR);
                break;

            case CSL_VLCDJ_DCT_PTR:                       /* DCT TABLES */
                // *data =
                // CSL_FEXT(VlcdjRegs->VLCDJE_DCTQM,VLCDJ_VLCDJE_DCTQM_DCT);
                *data =
                    (VlcdjRegs->VLCDJE_DCTQM) & CSL_VLCDJ_VLCDJE_DCTQM_DCT_MASK;
                break;

            case CSL_VLCDJ_HUFF_PTR:
                // *data =
                // CSL_FEXT(VlcdjRegs->VLCDJE_VLCTBL,VLCDJ_VLCDJE_VLCTBL_ADDR);
                *data =
                    (VlcdjRegs->
                     VLCDJE_VLCTBL) & CSL_VLCDJ_VLCDJE_VLCTBL_ADDR_MASK;
                break;

            case CSL_VLCDJ_RST_PTR:
                // *data =
                // CSL_FEXT(VlcdjRegs->VLCDJE_RSTPTR,VLCDJ_VLCDJE_RSTPTR_ADDR);
                *data =
                    (VlcdjRegs->
                     VLCDJE_RSTPTR) & CSL_VLCDJ_VLCDJE_RSTPTR_ADDR_MASK;
                break;

            default:
                return (CSL_ESYS_INVPARAMS);

        }
        return CSL_SOK;
    }

    /* Query for Restart Marker configuration */

    CSL_Status _CSL_VlcdjGetRSTConfig(CSL_VlcdjHandle hndl, Uint16 * data,
                                      CSL_VlcdjRstConfigType rstconfig) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);

        switch (rstconfig)

        {
            case CSL_VLCDJ_INC:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJE_RSTCFG, VLCDJ_VLCDJE_RSTCFG_INC);
                break;
            case CSL_VLCDJ_INIT:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJE_RSTCFG,
                             VLCDJ_VLCDJE_RSTCFG_INIT);
                break;
            case CSL_VLCDJ_PHASE:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJE_RSTCFG,
                             VLCDJ_VLCDJE_RSTCFG_PHASE);
                break;
            case CSL_VLCDJ_INTRVL:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJE_RSTCFG,
                             VLCDJ_VLCDJE_RSTCFG_INTRVL);
                break;

            default:
                return (CSL_ESYS_INVPARAMS);

        }
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetRSTOffSet(CSL_VlcdjHandle hndl, Uint32 * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data =
            CSL_FEXT(VlcdjRegs->VLCDJE_RSTOFST, VLCDJ_VLCDJE_RSTOFST_OFFSET);
        return CSL_SOK;

    }

    CSL_Status _CSL_VlcdjGetDecNMcus(CSL_VlcdjHandle hndl, Uint16 * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(VlcdjRegs->VLCDJD_CFG, VLCDJ_VLCDJD_CFG_NMCUS);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetDecFmtDataType(CSL_VlcdjHandle hndl,
                                           CSL_VlcdjDecodeFmtType * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *((Uint16 *) data) =
            CSL_FEXT(VlcdjRegs->VLCDJD_CFG, VLCDJ_VLCDJD_CFG_FMT);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetRSTDec(CSL_VlcdjHandle hndl, Uint16 * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(VlcdjRegs->VLCDJD_CFG, VLCDJ_VLCDJD_CFG_RSTEN);
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetPREDDec(CSL_VlcdjHandle hndl, Uint16 * data,
                                    CSL_VlcdjDCPredictorType dcpred) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        switch (dcpred)

        {
            case CSL_VLCDJ_DCPREDY:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJD_DCPREDY,
                             VLCDJ_VLCDJD_DCPREDY_PREDY);
                break;
            case CSL_VLCDJ_DCPREDU:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJD_DCPREDUV,
                             VLCDJ_VLCDJD_DCPREDUV_PREDU);
                break;
            case CSL_VLCDJ_DCPREDV:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJD_DCPREDUV,
                             VLCDJ_VLCDJD_DCPREDUV_PREDV);
                break;

            default:
                return (CSL_ESYS_INVPARAMS);
        }
        return CSL_SOK;
    }

    CSL_Status _CSL_VlcdjGetDecPtrTable(CSL_VlcdjHandle hndl, Uint16 * data,
                                        CSL_VlcdjPtrType ptrtype) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        switch (ptrtype)
        {
            case CSL_VLCDJ_BSP_BITPTR:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJD_BSPTR,
                             VLCDJ_VLCDJD_BSPTR_BITPTR);
                break;

            case CSL_VLCDJ_BSP_BYTEPTR:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJD_BSPTR,
                             VLCDJ_VLCDJD_BSPTR_BYTEPTR);
                break;

            case CSL_VLCDJ_CBUF_START:
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJD_CBUF, VLCDJ_VLCDJD_CBUF_START);
                break;

            case CSL_VLCDJ_CBUF_END:
                *data = CSL_FEXT(VlcdjRegs->VLCDJD_CBUF, VLCDJ_VLCDJD_CBUF_END);
                break;

            case CSL_VLCDJ_QMR_PTR:                       /* QUANTIZATION
                                                            * MATRIX */
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJD_DCTQM, VLCDJ_VLCDJD_DCTQM_QMR);
                break;

            case CSL_VLCDJ_DCT_PTR:                       /* DCT TABLES */
                // *data =
                // CSL_FEXT(VlcdjRegs->VLCDJD_DCTQM,VLCDJ_VLCDJD_DCTQM_DCT);
                *data =
                    (VlcdjRegs->VLCDJD_DCTQM) & CSL_VLCDJ_VLCDJD_DCTQM_DCT_MASK;
                break;

            case CSL_VLCDJ_DEC_CTRLTBL:                   /* HUFF TABLES */
                // *data =
                // CSL_FEXT(VlcdjRegs->VLCDJD_CTRLTBL,VLCDJ_VLCDJD_CTRLTBL_ADDR);
                *data =
                    (VlcdjRegs->
                     VLCDJD_CTRLTBL) & CSL_VLCDJ_VLCDJD_CTRLTBL_ADDR_MASK;
                break;

            case CSL_VLCDJ_DEC_HUFF0:                     /* HUFF TABLES */
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJD_DCDTBL01,
                             VLCDJ_VLCDJD_DCDTBL01_DCDTBL0);
                break;

            case CSL_VLCDJ_DEC_HUFF1:                     /* HUFF TABLES */
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJD_DCDTBL01,
                             VLCDJ_VLCDJD_DCDTBL01_DCDTBL1);
                break;

            case CSL_VLCDJ_DEC_HUFF2:                     /* HUFF TABLES */
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJD_DCDTBL23,
                             VLCDJ_VLCDJD_DCDTBL23_DCDTBL2);
                break;

            case CSL_VLCDJ_DEC_HUFF3:                     /* HUFF TABLES */
                *data =
                    CSL_FEXT(VlcdjRegs->VLCDJD_DCDTBL23,
                             VLCDJ_VLCDJD_DCDTBL23_DCDTBL3);
                break;

            default:
                return (CSL_ESYS_INVPARAMS);
        }

        return CSL_SOK;

    }

    CSL_Status _CSL_VlcdjGetDecErr(CSL_VlcdjHandle hndl, Uint16 * data) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(VlcdjRegs->VLCDJD_CFG, VLCDJ_VLCDJD_DCTERR_ERRPTR);
        return CSL_SOK;
    }

/*****************************************************************************/
    /* Functions to get the VLCDJ unique identifier & resource allocation
     * mask into the handle */
 /*****************************************************************************/

    CSL_VlcdjRegsOvly _CSL_VlcdjGetBaseAddr(CSL_VlcdjNum VlcdjNum) {
        switch (VlcdjNum)
        {
            case CSL_VLCDJ_0:
                /* get the VLCDJ unique identifier & resource * allocation
                 * mask into the handle */
                return CSL_VLCDJ_0_REGS;
        }
        return NULL;
    }
#endif

    CSL_Status _CSL_VlcdjGetAttrs(CSL_VlcdjNum vlcdjNum, CSL_VlcdjHandle hVlcdj) {
        CSL_Status status = CSL_SOK;

        switch (vlcdjNum)
        {
            case CSL_VLCDJ_0:
                /* get the VLCDJ unique identifier & resource * allocation
                 * mask into the handle */
                hVlcdj->xio = CSL_VLCDJ_0_XIO;
                hVlcdj->uid = CSL_VLCDJ_0_UID;
                /* get the VLCDJ base address into the handle */
                hVlcdj->regs = (CSL_VlcdjRegsOvly) CSL_VLCDJ_0_REGS;
                                                               /*_CSL_VlcdjGetBaseAddr(vlcdjNum);*/
                /* get the Vlcdj instance number into the handle */
                hVlcdj->perNum = vlcdjNum;
                break;
            default:
                status = CSL_ESYS_OVFL;
        }

        return status;
    }

    void _CSL_Vlcdj_Reset_Val_Read(CSL_VlcdjHandle hndl) {
        CSL_VlcdjRegsOvly VlcdjRegs = hndl->regs;

        VlcdjRegs->VLCDJ_REVISION = CSL_VLCDJ_VLCDJ_REVISION_RESETVAL;
        VlcdjRegs->VLCDJ_CTRL = CSL_VLCDJ_VLCDJ_CTRL_RESETVAL;
        VlcdjRegs->VLCDJE_CFG = CSL_VLCDJ_VLCDJE_CFG_RESETVAL;
        VlcdjRegs->VLCDJE_DCPREDY = CSL_VLCDJ_VLCDJE_DCPREDY_RESETVAL;
        VlcdjRegs->VLCDJE_DCPREDUV = CSL_VLCDJ_VLCDJE_DCPREDUV_RESETVAL;
        VlcdjRegs->VLCDJE_BSPTR = CSL_VLCDJ_VLCDJE_BSPTR_RESETVAL;
        VlcdjRegs->VLCDJE_CBUF = CSL_VLCDJ_VLCDJE_CBUF_RESETVAL;
        VlcdjRegs->VLCDJE_RSTCFG = CSL_VLCDJ_VLCDJE_RSTCFG_RESETVAL;
        VlcdjRegs->VLCDJE_DCTQM = CSL_VLCDJ_VLCDJE_DCTQM_RESETVAL;
        VlcdjRegs->VLCDJE_VLCTBL = CSL_VLCDJ_VLCDJE_VLCTBL_RESETVAL;
        VlcdjRegs->VLCDJE_RSTPTR = CSL_VLCDJ_VLCDJE_RSTPTR_RESETVAL;
        VlcdjRegs->VLCDJE_RSTOFST = CSL_VLCDJ_VLCDJE_RSTOFST_RESETVAL;
        VlcdjRegs->VLCDJD_CFG = CSL_VLCDJ_VLCDJD_CFG_RESETVAL;
        VlcdjRegs->VLCDJD_DCPREDY = CSL_VLCDJ_VLCDJD_DCPREDY_RESETVAL;
        VlcdjRegs->VLCDJD_DCPREDUV = CSL_VLCDJ_VLCDJD_DCPREDUV_RESETVAL;
        VlcdjRegs->VLCDJD_BSPTR = CSL_VLCDJ_VLCDJD_BSPTR_RESETVAL;
        VlcdjRegs->VLCDJD_CBUF = CSL_VLCDJ_VLCDJD_CBUF_RESETVAL;
        VlcdjRegs->VLCDJD_DCTQM = CSL_VLCDJ_VLCDJD_DCTQM_RESETVAL;
        VlcdjRegs->VLCDJD_CTRLTBL = CSL_VLCDJ_VLCDJD_CTRLTBL_RESETVAL;
        VlcdjRegs->VLCDJD_DCDTBL01 = CSL_VLCDJ_VLCDJD_DCDTBL01_RESETVAL;
        VlcdjRegs->VLCDJD_DCDTBL23 = CSL_VLCDJ_VLCDJD_DCDTBL23_RESETVAL;
        VlcdjRegs->VLCDJD_DCTERR = CSL_VLCDJ_VLCDJD_DCTERR_RESETVAL;

    }

#ifdef __cplusplus
}
#endif

#endif /*__CSL_VLCD_C_*/

/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file _csl_rot.c
*
* This file contains Level 0 CSL function prototypes
*used for setting the registers of H/W  ROT module in SIMCOP in OMAP4/Monica
*
* @path ROT\src
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*!
*! 16-Apr-2009 Phanish: Changes made because of Resource Manager Integration. 
*!                       i.e., Removal of dependency on _CSL_certifyOpen() calls.
*! 19-Mar-2009 Phanish: Modified to remove the HWcontrol and GetHWstatus APIs 
*!                 and their dependencies. And now, HWsetup calls directly the CSL_FINS
*! 08-Aug-2008 Anandhi: Initial version/Created this file
*========================================================================= */

#ifndef __CSL_ROT_C_
#define __CSL_ROT_C_

#ifdef __cplusplus
extern "C" {
#endif

#include "../cslr__rot_1.h"
#include "../inc/_csl_rot.h"
#include <ti/psp/iss/hal/iss/simcop/common/csl_error.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl.h>

#if 0
    CSL_Status _CSL_rotEnable(CSL_RotHandle hndl, CSL_RotHwEnableType data) {
        CSL_RotRegsOvly rotRegs = hndl->regs;
         CSL_FINS(rotRegs->CTRL, ROT_CTRL_EN, data);
         return CSL_SOK;
    } CSL_Status _CSL_rotSetOpType(CSL_RotHandle hndl, CSL_RotOpType data) {
        CSL_RotRegsOvly rotRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        rotRegs = hndl->regs;
        CSL_FINS(rotRegs->CFG, ROT_CFG_OP, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotSetDataType(CSL_RotHandle hndl, CSL_RotFmtType data) {
        CSL_RotRegsOvly rotRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        rotRegs = hndl->regs;
        CSL_FINS(rotRegs->CFG, ROT_CFG_FMT, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotSetTrigSrcType(CSL_RotHandle hndl,
                                      CSL_RotTrgSrcType data) {
        CSL_RotRegsOvly rotRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        rotRegs = hndl->regs;
        CSL_FINS(rotRegs->CFG, ROT_CFG_TRIG_SRC, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotSetAutoGating(CSL_RotHandle hndl, CSL_RotAutoGatingType data)    // Earlier 
                                                                                        // the 
                                                                                        // handle 
                                                                                        // was 
                                                                                        // CSL_IpipeHandle 
                                                                                        // but 
                                                                                        // changed 
                                                                                        // to 
                                                                                        // ROT 
                                                                                        // for 
                                                                                        // compling
    {
        CSL_RotRegsOvly rotRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        rotRegs = hndl->regs;
        CSL_FINS(rotRegs->CFG, ROT_CFG_AUTOGATING, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotSetBlockSize(CSL_RotHandle hndl,
                                    CSL_RotBlkSizeConfig data) {
        CSL_RotRegsOvly rotRegs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        rotRegs = hndl->regs;
        CSL_FINS(rotRegs->CFG, ROT_CFG_NBLKS, data.nBlocks);
        rotRegs->BLKSZ = CSL_FMK(ROT_BLKSZ_BLKW, data.nBlockWidth) |
            CSL_FMK(ROT_BLKSZ_BLKH, data.nBlockHeight);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotSetStartAddrConfig(CSL_RotHandle hndl,
                                          CSL_RotStartAddrConfig data) {
        CSL_RotRegsOvly rotRegs;

        CSL_RotFmtType tFmt;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        rotRegs = hndl->regs;
        tFmt = CSL_FEXT(rotRegs->CFG, ROT_CFG_FMT);
        if (tFmt == CSL_ROT_CFG_FMT_YUV_420)
            rotRegs->SRC_START2 =
                CSL_FMK(ROT_SRC_START2_ADDR, data.nSourceStartUV);
        rotRegs->SRC_START1 = CSL_FMK(ROT_SRC_START1_ADDR, data.nSourceStart);
        rotRegs->SRC_LOFST =
            CSL_FMK(ROT_SRC_LOFST_LOFST, data.nSourceLineOffset);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotSetDestAddrConfig(CSL_RotHandle hndl,
                                         CSL_RotDestAddrConfig data) {
        CSL_RotRegsOvly rotRegs;

        CSL_RotFmtType tFmt;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        rotRegs = hndl->regs;
        tFmt = CSL_FEXT(rotRegs->CFG, ROT_CFG_FMT);
        if (tFmt == CSL_ROT_CFG_FMT_YUV_420)
            rotRegs->DST_START2 =
                CSL_FMK(ROT_DST_START2_ADDR, data.nDestStartUV);
        rotRegs->DST_START1 = CSL_FMK(ROT_DST_START1_ADDR, data.nDestStart);
        rotRegs->DST_LOFST = CSL_FMK(ROT_DST_LOFST_LOFST, data.nDestLineOffset);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotIsBusy(CSL_RotHandle hndl, CSL_RotHwStatusType * data) {
        CSL_RotRegsOvly rotRegs = hndl->regs;

        *data = CSL_FEXT(rotRegs->CTRL, ROT_CTRL_BUSY);
        return CSL_SOK;
    }
    CSL_Status _CSL_rotGetRevId(CSL_RotHandle hndl, Uint16 * data) {
        CSL_RotRegsOvly rotRegs = hndl->regs;

        *data = CSL_FEXT(rotRegs->REVISION, ROT_REVISION_REV);
        return CSL_SOK;
    }
    CSL_Status _CSL_rotGetOpType(CSL_RotHandle hndl, CSL_RotOpType * data) {
        CSL_RotRegsOvly rotRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(rotRegs->CFG, ROT_CFG_OP);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotGetDataType(CSL_RotHandle hndl, CSL_RotFmtType * data) {
        CSL_RotRegsOvly rotRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(rotRegs->CFG, ROT_CFG_FMT);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotGetTrigSrcType(CSL_RotHandle hndl,
                                      CSL_RotTrgSrcType * data) {
        CSL_RotRegsOvly rotRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(rotRegs->CFG, ROT_CFG_TRIG_SRC, *data);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotGetAutoGating(CSL_RotHandle hndl, CSL_RotAutoGatingType * data)  // Hndl 
                                                                                        // was 
                                                                                        // CSL_IpipeHandle. 
                                                                                        // Changed 
                                                                                        // for 
                                                                                        // compling 
                                                                                        // 
    {
        CSL_RotRegsOvly rotRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(rotRegs->CFG, ROT_CFG_AUTOGATING);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotGetBlockSize(CSL_RotHandle hndl,
                                    CSL_RotBlkSizeConfig * data) {
        CSL_RotRegsOvly rotRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        data->nBlockWidth = CSL_FEXT(rotRegs->BLKSZ, ROT_BLKSZ_BLKW);
        data->nBlockHeight = CSL_FEXT(rotRegs->BLKSZ, ROT_BLKSZ_BLKH);
        data->nBlocks = CSL_FEXT(rotRegs->CFG, ROT_CFG_NBLKS);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotGetStartAddrConfig(CSL_RotHandle hndl,
                                          CSL_RotStartAddrConfig * data) {
        CSL_RotRegsOvly rotRegs = hndl->regs;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        data->nSourceStartUV =
            CSL_FEXT(rotRegs->SRC_START2, ROT_SRC_START2_ADDR);
        data->nSourceStart = CSL_FEXT(rotRegs->SRC_START1, ROT_SRC_START1_ADDR);
        data->nSourceLineOffset =
            CSL_FEXT(rotRegs->SRC_LOFST, ROT_SRC_LOFST_LOFST);
        return CSL_SOK;
    }

    CSL_Status _CSL_rotGetDestAddrConfig(CSL_RotHandle hndl,
                                         CSL_RotDestAddrConfig * data) {
        CSL_RotRegsOvly rotRegs = hndl->regs;

        CSL_RotFmtType tFmt;

        if (data == NULL)
            return (CSL_ESYS_INVPARAMS);
        data->nDestStartUV = CSL_FEXT(rotRegs->DST_START2, ROT_DST_START2_ADDR);
        data->nDestStart = CSL_FEXT(rotRegs->DST_START1, ROT_DST_START1_ADDR);
        data->nDestLineOffset =
            CSL_FEXT(rotRegs->DST_LOFST, ROT_DST_LOFST_LOFST);
        return CSL_SOK;
    }

    CSL_RotRegsOvly _CSL_rotGetBaseAddr(CSL_RotNum RotNum) {
        switch (RotNum)
        {
            case CSL_ROT_0:
                /* get the ROT unique identifier & resource allocation mask
                 * into the handle */
                return CSL_ROT_0_REGS;
        }
        return NULL;
    }
#endif
    CSL_Status _CSL_rotGetAttrs(CSL_RotNum rotNum, CSL_RotHandle hRot) {
        CSL_Status status = CSL_SOK;

        switch (rotNum)
        {
            case CSL_ROT_0:
                /* get the ROT unique identifier & resource allocation mask
                 * into the handle */
                hRot->xio = CSL_ROT_0_XIO;
                hRot->uid = CSL_ROT_0_UID;
                /* get the ROT base address into the handle */
                hRot->regs = (CSL_RotRegsOvly) CSL_ROT_0_REGS;
                                                     /*_CSL_rotGetBaseAddr(rotNum);*/
                /* get the ROT instance number into the handle */
                hRot->perNum = rotNum;
                break;
            default:
                status = CSL_ESYS_OVFL;
        }
        return status;
    }

    void _CSL_Rot_Reset_Val_Read(CSL_RotHandle hndl) {
        CSL_RotRegsOvly RotRegs = hndl->regs;

        RotRegs->REVISION = CSL_ROT_REVISION_RESETVAL;
        RotRegs->CTRL = CSL_ROT_CTRL_RESETVAL;
        RotRegs->CFG = CSL_ROT_CFG_RESETVAL;
        RotRegs->BLKSZ = CSL_ROT_BLKSZ_RESETVAL;
        RotRegs->SRC_START1 = CSL_ROT_SRC_START1_RESETVAL;
        RotRegs->SRC_LOFST = CSL_ROT_SRC_LOFST_RESETVAL;
        RotRegs->DST_START1 = CSL_ROT_DST_START1_RESETVAL;
        RotRegs->DST_LOFST = CSL_ROT_DST_LOFST_RESETVAL;
        RotRegs->SRC_START2 = CSL_ROT_SRC_START2_RESETVAL;
        RotRegs->DST_START2 = CSL_ROT_DST_START2_RESETVAL;

    }

#endif

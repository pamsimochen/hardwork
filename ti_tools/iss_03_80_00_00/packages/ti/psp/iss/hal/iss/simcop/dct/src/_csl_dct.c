/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file _csl_dct.c
*
* This file contains Level 0 CSL functions used for setting the registers of H/W  DCT module on SIMCOP in OMAP4/Monica
*
* @path DCT\src
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
*! 9-Sep     2008 Sowmya Priya : Updates in functions
*!
*!20-July    2008 Anandhi Ramesh: Initial Release
*!
*========================================================================= */

#ifndef __CSL_DCT_C_
#define __CSL_DCT_C_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/
#include "../cslr__dct_001.h"
#include "../inc/_csl_dct.h"
#include <ti/psp/iss/hal/iss/simcop/common/csl_error.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_resid.h>
/****************************************************************/
#if 0
    CSL_Status _CSL_dctEnable(CSL_DctHandle hndl, CSL_DctHwEnableType data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;
         CSL_FINS(dctRegs->CTRL, DCT_CTRL_EN, data);
         return CSL_SOK;
    } CSL_Status _CSL_dctSetModeType(CSL_DctHandle hndl, CSL_DctModeType data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(dctRegs->CFG, DCT_CFG_MODE, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctSetSpDataType(CSL_DctHandle hndl, CSL_DctSpDataType data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(dctRegs->CFG, DCT_CFG_FMT, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctSetTrigSrcType(CSL_DctHandle hndl,
                                      CSL_DctTrgSrcType data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(dctRegs->CFG, DCT_CFG_TRIG_SRC, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctSetAutoGating(CSL_DctHandle hndl,
                                     CSL_DctAutoGatingType data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(dctRegs->CFG, DCT_CFG_AUTOGATING, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctSetNMcus(CSL_DctHandle hndl, Uint32 data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(dctRegs->CFG, DCT_CFG_NMCUS, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctSetSpDataAddr(CSL_DctHandle hndl, Uint8 data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        dctRegs->SPTR = CSL_FMK(DCT_SPTR_ADDR, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctSetFrDataAddr(CSL_DctHandle hndl, Uint16 data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        dctRegs->FPTR = CSL_FMK(DCT_FPTR_ADDR, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctSetIntEnable(CSL_DctHandle hndl,
                                    CSL_DctIntEnableType data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(dctRegs->CFG, DCT_CFG_INTEN, data);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctGetState(CSL_DctHandle hndl, CSL_DctHwStatusType * data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        *((Uint16 *) data) = CSL_FEXT(dctRegs->CTRL, DCT_CTRL_BUSY);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctIsIntEnabled(CSL_DctHandle hndl,
                                    CSL_DctIntEnableType * data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        *((Uint16 *) data) = CSL_FEXT(dctRegs->CFG, DCT_CFG_INTEN);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctGetRevId(CSL_DctHandle hndl, Uint16 * data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        *data = CSL_FEXT(dctRegs->REVISION, DCT_REVISION_REV);
        return CSL_SOK;
    }
    CSL_Status _CSL_dctGetModeType(CSL_DctHandle hndl, CSL_DctModeType * data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL)
            return (CSL_ESYS_INVPARAMS);
        *((Uint16 *) data) = CSL_FEXT(dctRegs->CFG, DCT_CFG_MODE);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctGetSpDataFmt(CSL_DctHandle hndl,
                                    CSL_DctSpDataType * data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *((Uint16 *) data) = CSL_FEXT(dctRegs->CFG, DCT_CFG_FMT);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctGetTrigSrcType(CSL_DctHandle hndl,
                                      CSL_DctTrgSrcType * data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        CSL_FINS(dctRegs->CFG, DCT_CFG_TRIG_SRC, *data);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctGetAutoGating(CSL_DctHandle hndl,
                                     CSL_DctAutoGatingType * data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *((Uint16 *) data) = CSL_FEXT(dctRegs->CFG, DCT_CFG_AUTOGATING);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctGetNMcus(CSL_DctHandle hndl, Uint32 * data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(dctRegs->CFG, DCT_CFG_NMCUS);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctGetSpDataAddr(CSL_DctHandle hndl, Uint8 * data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(dctRegs->SPTR, DCT_SPTR_ADDR);
        return CSL_SOK;
    }

    CSL_Status _CSL_dctGetFrDataAddr(CSL_DctHandle hndl, Uint16 * data) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        if (hndl == NULL || data == NULL)
            return (CSL_ESYS_INVPARAMS);
        *data = CSL_FEXT(dctRegs->FPTR, DCT_FPTR_ADDR);
        return CSL_SOK;
    }

    CSL_DctRegsOvly _CSL_dctGetBaseAddr(CSL_DctNum DctNum) {
        switch (DctNum)
        {
            case CSL_DCT_0:
                /* get the DCT unique identifier & resource */
                /* allocation mask into the handle */
                return CSL_DCT_0_REGS;
        }
        return NULL;
    }
#endif

    CSL_Status _CSL_dctGetAttrs(CSL_DctNum dctNum, CSL_DctHandle hDct) {
        CSL_Status status = CSL_SOK;

        switch (dctNum)
        {
            case CSL_DCT_0:
                /* get the DCT unique identifier & resource */
                /* allocation mask into the handle */
                hDct->xio = CSL_DCT_0_XIO;
                hDct->uid = CSL_DCT_0_UID;
                /* get the DCT base address into the handle */
                hDct->regs = CSL_DCT_0_REGS;
                                        /*_CSL_dctGetBaseAddr(dctNum);*/
                /* get the DCT instance number into the handle */
                hDct->perNum = dctNum;
                break;
            default:
                status = CSL_ESYS_OVFL;
        }
        return status;
    }

    void _CSL_Dct_Reset_Val_Read(CSL_DctHandle hndl) {
        CSL_DctRegsOvly dctRegs = hndl->regs;

        dctRegs->REVISION = CSL_DCT_REVISION_RESETVAL;
        dctRegs->CTRL = CSL_DCT_CTRL_RESETVAL;
        dctRegs->CFG = CSL_DCT_CFG_RESETVAL;
        dctRegs->SPTR = CSL_DCT_SPTR_RESETVAL;
        dctRegs->FPTR = CSL_DCT_FPTR_RESETVAL;
    }

#ifdef __cplusplus
}
#endif
#endif /*__CSL_DCT_C_*/

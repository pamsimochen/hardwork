/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file _csl_imx4.c
*
* This file contains register level APIs for IMX register manioulation in SIMCOP
*
* @path ???
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 25-Sep 2008 Phanish: Initial Version! 
*========================================================================= */
#ifndef __CSLR_IMX4_C_
#define __CSLR_IMX4_C_

#ifdef __cplusplus
extern "C" {
#endif

#include "../cslr__imx4_1.h"
#include "../csl_imx4.h"
#include <ti/psp/iss/hal/iss/simcop/common/csl_error.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl.h>

    CSL_Status _CSL_ImxStart(CSL_ImxHandle pHandle) {
        CSL_ImxRegsOvly pRegs;
        CSL_Status status = CSL_SOK;
         CSL_EXIT_IF((pHandle == NULL ||
                      pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);

         pRegs = pHandle->pRegs;
         CSL_FINS(pRegs->START, IMX4_START_START, CSL_IMX4_START_START_BUSY);
         EXIT: return status;
    }
    /* 
     * Read back 0 = idle, 1 = busy
     */ CSL_Status _CSL_ImxGetStatus(CSL_ImxHandle pHandle,
                                     CSL_ImxHwStatusType * usData)
    {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);

        CSL_EXIT_IF((usData == NULL), CSL_ESYS_INVPARAMS);
        pRegs = pHandle->pRegs;
        *usData =
            (CSL_ImxHwStatusType) CSL_FEXT(pRegs->START, IMX4_START_START);
      EXIT:
        return status;
    }

    CSL_Status _CSL_ImxSetCmdAddr(CSL_ImxHandle pHandle, Uint16 usData) {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);

        pRegs = pHandle->pRegs;
        CSL_FINS(pRegs->CMDADR, IMX4_CMDADR_ADR, usData);
      EXIT:
        return status;
    }

    /* Command memory address (16-bit per word) from where iMX has started
     * decoding the command (read-only) */
    CSL_Status _CSL_ImxGetCmdStartAddr(CSL_ImxHandle pHandle, Uint16 * usData) {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((usData == NULL), CSL_ESYS_INVPARAMS);

        pRegs = pHandle->pRegs;
        *usData = CSL_FEXT(pRegs->CMDADR, IMX4_CMDADR_ADR);
      EXIT:
        return status;
    }

    /* Command memory address (16-bit per word) from where iMX is currently
     * decoding the command (read-only) */
    CSL_Status _CSL_ImxGetCmdCurrAddr(CSL_ImxHandle pHandle, Uint16 * usData) {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((usData == NULL), CSL_ESYS_INVPARAMS);

        pRegs = pHandle->pRegs;
        *usData = CSL_FEXT(pRegs->CMDPTR, IMX4_CMDPTR_CMDPTR);
      EXIT:
        return status;
    }

    /* 
     * @param usData
     *                             0: clock is on when busy, clock is off when idle
     *                             1: clock is on, all the time
     */
    CSL_Status _CSL_ImxConfigClkControlType(CSL_ImxHandle pHandle,
                                            CSL_ImxClkCtrlType usData) {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);

        pRegs = pHandle->pRegs;
        CSL_FINS(pRegs->CLKCNTRL, IMX4_CLKCNTRL_CLKCNTRL, usData);
      EXIT:
        return status;
    }

    /* 
     * @param usData
     *                             0: clock is on when busy, clock is off when idle
     *                             1: clock is on, all the time
     */
    CSL_Status _CSL_ImxGetClkControlType(CSL_ImxHandle pHandle,
                                         CSL_ImxClkCtrlType * usData) {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((usData == NULL), CSL_ESYS_INVPARAMS);

        pRegs = pHandle->pRegs;
        *usData =
            (CSL_ImxClkCtrlType) CSL_FEXT(pRegs->CLKCNTRL,
                                          IMX4_CLKCNTRL_CLKCNTRL);
      EXIT:
        return status;
    }

    CSL_Status _CSL_ImxGetMinMaxId(CSL_ImxHandle pHandle, Uint16 * usData) {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((usData == NULL), CSL_ESYS_INVPARAMS);

        pRegs = pHandle->pRegs;
        *usData = CSL_FEXT(pRegs->MINMAX_ID, IMX4_MINMAX_ID_ID);
      EXIT:
        return status;
    }

    CSL_Status _CSL_ImxGetMinMaxVal(CSL_ImxHandle pHandle, Uint16 * usData) {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((usData == NULL), CSL_ESYS_INVPARAMS);

        pRegs = pHandle->pRegs;
        *usData = CSL_FEXT(pRegs->MINMAX_VAL, IMX4_MINMAX_VAL_VAL);
      EXIT:
        return status;
    }

    /* 
     * Breakpoint enable, 0: disable, 1: enable
     * When iMX is pausing due to breakpoint, write 1 to resume execution
     */
    CSL_Status _CSL_ImxConfigBreakPoint(CSL_ImxHandle pHandle,
                                        CSL_ImxBkPtMode usData) {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);

        pRegs = pHandle->pRegs;
        CSL_FINS(pRegs->BRKPTR, IMX4_BRKPTR_EN, usData);
      EXIT:
        return status;
    }

    CSL_Status _CSL_ImxGetBreakPointConfig(CSL_ImxHandle pHandle,
                                           CSL_ImxBkPtMode * usData) {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((usData == NULL), CSL_ESYS_INVPARAMS);

        pRegs = pHandle->pRegs;
        *usData = (CSL_ImxBkPtMode) CSL_FEXT(pRegs->BRKPTR, IMX4_BRKPTR_EN);
      EXIT:
        return status;
    }

    CSL_Status _CSL_ImxSetBreakPointAddr(CSL_ImxHandle pHandle, Uint16 usData) {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);

        pRegs = pHandle->pRegs;
        CSL_FINS(pRegs->BRKPTR, IMX4_BRKPTR_ADR, usData);
      EXIT:
        return status;
    }

    CSL_Status _CSL_ImxGetBreakPointAddr(CSL_ImxHandle pHandle, Uint16 * usData) {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((usData == NULL), CSL_ESYS_INVPARAMS);

        pRegs = pHandle->pRegs;
        *usData = CSL_FEXT(pRegs->BRKPTR, IMX4_BRKPTR_ADR);
      EXIT:
        return status;
    }

    CSL_Status _CSL_ImxGetBreakPointStatus(CSL_ImxHandle pHandle,
                                           CSL_ImxBkPtHitType * usData) {
        CSL_ImxRegsOvly pRegs;

        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL ||
                     pHandle->pRegs == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((usData == NULL), CSL_ESYS_INVPARAMS);

        pRegs = pHandle->pRegs;
        *usData =
            (CSL_ImxBkPtHitType) CSL_FEXT(pRegs->BRKPTR, IMX4_BRKPTR_STAT);
      EXIT:
        return status;
    }

    CSL_Status _CSL_ImxCommandMemWrite(CSL_ImxHandle pHandle,
                                       CSL_ImxCmdMemType * pCmd) {
        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF((pHandle == NULL || pCmd == NULL), CSL_ESYS_INVPARAMS);

        memcpy(pCmd->pCmdMemAddr, pCmd->pCmdBuf, pCmd->usLength);
      EXIT:
        return status;
    }

    CSL_ImxRegsOvly _CSL_ImxGetBaseAddr(CSL_ImxInstNum eImxInstNum) {
        switch (eImxInstNum)
        {
            case CSL_IMX_A:
                /* get the IMX A base addr */
                return CSL_IMX_A_REGS;
            case CSL_IMX_B:
                /* get the IMX B base addr */
                return CSL_IMX_B_REGS;
        }
        return NULL;
    }

    void _CSL_ImxGetAttrs(CSL_ImxInstNum eImxInstNum, CSL_ImxHandle hImx) {
        /* get the ROT base address into the handle */
        hImx->pRegs = (CSL_ImxRegsOvly) _CSL_ImxGetBaseAddr(eImxInstNum);

        switch (eImxInstNum)
        {
            case CSL_IMX_A:
                /* get the IMX unique identifier & resource allocation mask
                 * into the handle */
                hImx->xio = CSL_IMXA_0_XIO;
                hImx->uid = CSL_IMXA_0_UID;
                break;
            case CSL_IMX_B:
                /* get the IMX unique identifier & resource allocation mask
                 * into the handle */
                hImx->xio = CSL_IMXB_0_XIO;
                hImx->uid = CSL_IMXB_0_UID;
                break;
        }

        /* get the ROT instance number into the handle */
        hImx->eInstanceNum = eImxInstNum;

    }

#ifdef __cplusplus
}
#endif
#endif /*__CSLR_IMX4_C_*/

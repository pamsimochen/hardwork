/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file Csl_imx4.c
*
* This file contains register level APIs for IMX module in SIMCOP
*
* @path ???
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 26-Sep 2008 Phanish: Initial Version! 
*========================================================================= */
#ifndef __CSL_IMX4_C_
#define __CSL_IMX4_C_

#ifdef __cplusplus
extern "C" {
#endif

#include "../csl_imx4.h"
#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_error.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_resid.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl.h>
#include <ti/psp/iss/hal/iss/simcop/common/simcop.h>

    extern CSL_Status _CSL_ImxStart(CSL_ImxHandle pHandle);
    extern CSL_Status _CSL_ImxSetCmdAddr(CSL_ImxHandle pHandle, Uint16 usData);
    extern CSL_Status _CSL_ImxSetBreakPointAddr(CSL_ImxHandle pHandle,
                                                Uint16 usData);
    extern CSL_Status _CSL_ImxConfigBreakPoint(CSL_ImxHandle pHandle,
                                               CSL_ImxBkPtMode usData);
    extern CSL_Status _CSL_ImxConfigClkControlType(CSL_ImxHandle pHandle,
                                                   CSL_ImxClkCtrlType usData);
    extern CSL_Status _CSL_ImxCommandMemWrite(CSL_ImxHandle pHandle,
                                              CSL_ImxCmdMemType * pCmd);
    extern CSL_Status _CSL_ImxGetStatus(CSL_ImxHandle pHandle,
                                        CSL_ImxHwStatusType * usData);
    extern CSL_Status _CSL_ImxGetCmdStartAddr(CSL_ImxHandle pHandle,
                                              Uint16 * usData);
    extern CSL_Status _CSL_ImxGetCmdCurrAddr(CSL_ImxHandle pHandle,
                                             Uint16 * usData);
    extern CSL_Status _CSL_ImxGetMinMaxId(CSL_ImxHandle pHandle,
                                          Uint16 * usData);
    extern CSL_Status _CSL_ImxGetMinMaxVal(CSL_ImxHandle pHandle,
                                           Uint16 * usData);
    extern CSL_Status _CSL_ImxGetBreakPointStatus(CSL_ImxHandle pHandle,
                                                  CSL_ImxBkPtHitType * usData);
    extern CSL_Status _CSL_ImxGetBreakPointAddr(CSL_ImxHandle pHandle,
                                                Uint16 * usData);
    extern CSL_Status _CSL_ImxGetBreakPointConfig(CSL_ImxHandle pHandle,
                                                  CSL_ImxBkPtMode * usData);
    extern CSL_Status _CSL_ImxGetClkControlType(CSL_ImxHandle pHandle,
                                                CSL_ImxClkCtrlType * usData);

    void _CSL_ImxGetAttrs(CSL_ImxInstNum eImxInstNum, CSL_ImxHandle hImx);

    /* ================================================================ */
/**
*  CSL_ImxInit()
*
*  @param   hImx          Handle  to the IMX object
*
*  @return                    CSL_SOK always \n
*                                   
*================================================================== */
    CSL_Status CSL_ImxInit(CSL_ImxHandle hImx) {
        return CSL_SOK;
    }
/* ================================================================ *//**
*  CSL_ImxOpen() The open call sets up the data structures for the 
*  particular instance of the IMX. It gets a handle to the IMX module of SIMCOP
*
*  @param   hImxObj            Pointer to the IMX object
*
*  @param   eImxInst           Instance of IMX to be used
*
*  @param   eOpenMode       Open mode (Shared/Exclusive)\
*
*  @param   sStatus             CSL_SOK Succesfull open \n
*                                        Otherwise Open failed and the error code is returned.
*
*  @return   hImx                 Handle to IMX object instance
                                          or NULL, in case of failure
*================================================================== */
        CSL_ImxHandle CSL_ImxOpen(CSL_ImxObj * hImxObj,
                                  CSL_ImxInstNum eImxInst,
                                  CSL_OpenMode eOpenMode, CSL_Status * status) {
        CSL_ImxInstNum eImxInstNum = CSL_IMX_ANY;

        CSL_ImxHandle hImx = (CSL_ImxHandle) NULL;

        hImxObj->eOpenMode = eOpenMode;

        if (eImxInst >= 0)
        {
            _CSL_ImxGetAttrs(eImxInst, hImxObj);
            // ########### BY SANISH hImx =
            // (CSL_ImxHandle)_CSL_certifyOpen((CSL_ResHandle)hImxObj,
            // status);
        }
        else
        {
            for (eImxInstNum = (CSL_ImxInstNum) 0;
                 eImxInstNum < (CSL_ImxInstNum) CSL_IMX_PER_CNT; ++eImxInstNum)
            {
                _CSL_ImxGetAttrs(eImxInstNum, hImxObj);
                // ########### BY SANISH hImx =
                // (CSL_ImxHandle)_CSL_certifyOpen((CSL_ResHandle)hImxObj,
                // status);
                // ########### BY SANISH if (*status == CSL_SOK) 
                break;
            }
        }

        if (eImxInstNum == CSL_IMX_PER_CNT)
            *status = CSL_ESYS_OVFL;

        return hImx;

    }

    /* ================================================================ */
/**
*  CSL_ImxClose() The close API releases resources for the 
*  particular instance of the IMX for the given handle 
*
*  @param   hImx          Handle  to an instance of IMX object 

*  @return                     CSL_SOK on Succesfull Closing \n
*                                   Otherwise Close failed and the error code is returned.
*================================================================== */
    CSL_Status CSL_ImxClose(CSL_ImxHandle hImx) {
        /* Indicate in the CSL global data structure that the peripheral has
         * been unreserved */

        // return (_CSL_certifyClose((CSL_ResHandle)hImx)); ##### BY SANISH
        // ########

        return CSL_SOK;
    }

    /* ================================================================ */
/**
*  CSL_ImxHwSetup() writes imx command mem depending upon the operation to be performed
*
*  @param   hImx                    Handle  to the IMX object
*
*  @param   pSetupParam       Pointer which Points to the Imx setup data structure parameter
*
*  @return   eStatus                CSL_SOK on Succesful exit \n
*                                            Other value = Open failed and the error code is returned.
*================================================================== */
    CSL_Status CSL_ImxHwSetup(CSL_ImxHandle hImx, CSL_ImxSetup * pSetupParam) {
        // Int16 cmdLen=0;
        CSL_Status status = CSL_SOK;

        /* CSL_ImxBkPtMode eBkPtMode = CSL_IMX_BKPT_ENABLE; const int
         * ulInstrnMem[] = {0x49002C00, 0x07070000, 0x80000000, 0x20081000,
         * 0x20080008, 0x20080008, 0x80000008}; const int ulInstrnMem[] =
         * {0x84802DC0, 0x07010102, 0x80000000, 0xF5811000, 0x216B756C,
         * 0x00000003, 0x80000001}; */

        /* Load Cmd and Coeff memory with the commands and the coefficients
         * (if used) */
        // switch(pSetupParam->eImxOp)
        // {
        // case CSL_IMX_MOVE:
        // cmdLen += imxenc_array_op(pSetupParam);

        // cmdLen += imxenc_sleep(pSetupParam->pCmdMem+cmdLen); 
        // _CSL_ImxConfigBreakPoint(hImx, eBkPtMode);
        // break;
        // case CSL_IMX_VNF:
        // break;
        // case CSL_IMX_VSTAB:
        // break;
        // default:
        // break;
        // }
        /* EXIT: */
        return status;
    }

    /* ================================================================ */
/**
*  CSL_ImxHwControl() controls the IMX H/w setting through various commands
*
*  @param   hImx                    Handle  to the IMX object
*
*  @param   eCmd                   CMD used to select a particular field name/register
*
*  @param   data                     Pointer which Points to the data/value being used
*
*  @return   eStatus                CSL_SOK on Succesful exit \n
*                                            Other value = Open failed and the error code is returned.
*================================================================== */
    CSL_Status CSL_ImxHwControl(CSL_ImxHandle hImx, CSL_ImxHwCtrlCmd * eCmd,
                                void *data) {
        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF(hImx == NULL, CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF(data == NULL, CSL_ESYS_INVPARAMS);
        switch (*eCmd)
        {
            case CSL_IMX_CMD_START:
                status = _CSL_ImxStart(hImx);
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_CMD_STADDR:
                status = _CSL_ImxSetCmdAddr(hImx, *((Uint16 *) data));
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_CMD_BKADDR:
                status = _CSL_ImxSetBreakPointAddr(hImx, *((Uint16 *) data));
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_CMD_BKENABLE:
                status =
                    _CSL_ImxConfigBreakPoint(hImx, *((CSL_ImxBkPtMode *) data));
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_CMD_AUTOGATING:
                status =
                    _CSL_ImxConfigClkControlType(hImx,
                                                 *((CSL_ImxClkCtrlType *)
                                                   data));
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_CMD_WRITECMDMEM:                 /* Assumes the CMD 
                                                            * mem is writable 
                                                            * from CPU */
                status =
                    _CSL_ImxCommandMemWrite(hImx, (CSL_ImxCmdMemType *) data);
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            default:
                status = CSL_ESYS_INVCMD;
        }
      EXIT:
        return status;
    }

    /* ================================================================ */
/**
*  CSL_ImxGetHwStatus() extracts the IMX H/w settings/status of various registers 
*
*  @param   hImx                    Handle  to the IMX object
*
*  @param   eQuery                 CMD used to select a particular field name/register
*
*  @param   data                     Pointer which Points to the data/value  being queried
*
*  @return   eStatus                CSL_SOK on Succesful exit \n
*                                            Other value = Open failed and the error code is returned.
*================================================================== */

    CSL_Status CSL_ImxGetHwStatus(CSL_ImxHandle hImx, CSL_ImxHwQuery eQuery,
                                  void *data) {
        CSL_Status status = CSL_SOK;

        CSL_EXIT_IF(hImx == NULL, CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF(data == NULL, CSL_ESYS_INVPARAMS);
        switch (eQuery)
        {
            case CSL_IMX_QUERY_START:
                status = _CSL_ImxGetStatus(hImx, (CSL_ImxHwStatusType *) data);
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_QUERY_STADDR:
                status = _CSL_ImxGetCmdStartAddr(hImx, (Uint16 *) data);
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_QUERY_CURRADDR:
                status = _CSL_ImxGetCmdCurrAddr(hImx, (Uint16 *) data);
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_QUERY_MINMAXID:
                status = _CSL_ImxGetMinMaxId(hImx, (Uint16 *) data);
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_QUERY_MINMAXVAL:
                status = _CSL_ImxGetMinMaxVal(hImx, (Uint16 *) data);
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_QUERY_BKSTAT:
                status =
                    _CSL_ImxGetBreakPointStatus(hImx,
                                                (CSL_ImxBkPtHitType *) data);
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_QUERY_BKADDR:
                status = _CSL_ImxGetBreakPointAddr(hImx, (Uint16 *) data);
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_QUERY_BKENABLE:
                status =
                    _CSL_ImxGetBreakPointConfig(hImx, (CSL_ImxBkPtMode *) data);
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            case CSL_IMX_QUERY_AUTOGATING:
                status =
                    _CSL_ImxGetClkControlType(hImx,
                                              (CSL_ImxClkCtrlType *) data);
                CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_INVPARAMS);
                break;
            default:
                status = CSL_ESYS_INVQUERY;
        }
      EXIT:
        return status;
    }
#ifdef __cplusplus
}
#endif
#endif /*__CSL_IMX4_C_*/

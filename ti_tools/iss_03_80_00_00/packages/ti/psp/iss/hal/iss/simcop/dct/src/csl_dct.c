/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file csl_dct.c
*
* This file contains Level 1 CSL functions used for setting the registers of H/W  DCT module on SIMCOP in OMAP4/Monica
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
*!
*! 09-Sep-2008 Sowmya Priya : Updates in functions, code clean up
*!
*!20-July-2008 Anandhi Ramesh: Initial Release
*========================================================================= */

#ifndef _CSL_DCT_C_
#define _CSL_DCT_C_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/
#include <ti/psp/iss/hal/iss/simcop/common/csl.h>
    // #include <ti/psp/iss/hal/iss/simcop/common/csl_resource.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>

#include "../csl_dct.h"
#include "../inc/_csl_dct.h"

    /* ================================================================ */
/**
*  CSL_dctInit() Initializes the DCT settings
*
*  @param   hndl          Handle  to the DCT object
*
*  @return         status      CSL_SOK Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
*================================================================== */

    CSL_Status CSL_dctInit(CSL_DctHandle hndl) {
        _CSL_Dct_Reset_Val_Read(hndl);
        return CSL_SOK;
    }
/* ================================================================ *//**
*  CSL_dctOpen() The open call sets up the data structures for the 
*  particular instance of the DCT It gets a handle to the DCT module of SIMCOP
*
*  @param   hDctObj          Pointer to the DCT object
*
*  @param   dctNum           Instance of DCT device=
*
*  @param   openMode       Open mode (Shared/Exclusive)\
*
*  @param   status      CSL_SOK Succesfull open \n
*                               Other value = Open failed and the error code is returned.
*
*  @return              hDct =  Handle to DCT  object
                                    NULL =failed 
*================================================================== */
        CSL_Status CSL_dctOpen(CSL_DctObj * hDctObj,
                               CSL_DctNum dctNum, CSL_OpenMode openMode) {
        CSL_DctNum dctInst;

        /* CSL_DctHandle hDct = (CSL_DctHandle)NULL; */
        CSL_Status status = CSL_SOK;

        hDctObj->openMode = openMode;

        if (dctNum >= 0)
        {
            status = _CSL_dctGetAttrs(dctNum, hDctObj);
            CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_OVFL);
            /* hDct = (CSL_DctHandle)_CSL_certifyOpen((CSL_ResHandle)hDctObj, 
             * status); */
        }
        else
        {
            for (dctInst = (CSL_DctNum) 0;
                 dctInst < (CSL_DctNum) CSL_DCT_PER_CNT; ++dctInst)
            {
                status = _CSL_dctGetAttrs(dctInst, hDctObj);
                /* hDct =
                 * (CSL_DctHandle)_CSL_certifyOpen((CSL_ResHandle)hDctObj,
                 * status); */

                if (status == CSL_SOK)
                    break;
            }
            /* if (dctInst == CSL_DCT_PER_CNT) *status = CSL_ESYS_OVFL; */
        }
      EXIT:
        return status;
    }

    /* ================================================================ */
/**
*  CSL_dctClose() The close call deallocates resources for DCT  for the 
*  particular instance of the DCT for which the handle is passed 
*
*  @param   hndl          Handle  to the DCT object

*  @return         status      CSL_SOK Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
*================================================================== */

    CSL_Status CSL_dctClose(CSL_DctHandle hndl) {
        /* Indicate in the CSL global data structure that the peripheral has
         * been unreserved */
        /* return (_CSL_certifyClose((CSL_ResHandle)hndl)); */
        return CSL_SOK;
    }

    /* ================================================================ */
/**
*  CSL_dctHwSetup() Sets the H/w for DCT module at one shot with the sttings provided by the 
* application
*
*  @param   hndl                    Handle  to the DCT object
*  @param   setup................. Setup structure containing values for register settings

*  @return         status      CSL_SOK Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
*================================================================== */

    CSL_Status CSL_dctHwSetup(CSL_DctHandle hndl, CSL_DctHwSetup * setup) {
        CSL_Status status = CSL_SOK;

        CSL_DctRegsOvly dctRegs;

        CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((hndl->regs == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((setup == NULL), CSL_ESYS_INVPARAMS);
        /* 
         * CSL_EXIT_IF((_CSL_dctEnable(hndl, setup->HwEnable)!= CSL_SOK),
         * CSL_ESYS_INVPARAMS); CSL_EXIT_IF((_CSL_dctSetModeType(hndl,
         * setup->nModeType) != CSL_SOK), CSL_ESYS_INVPARAMS);
         * CSL_EXIT_IF((_CSL_dctSetSpDataType(hndl, setup->SpFmtType) !=
         * CSL_SOK), CSL_ESYS_INVPARAMS);
         * CSL_EXIT_IF((_CSL_dctSetTrigSrcType(hndl, setup->TrgSrcType) !=
         * CSL_SOK) , CSL_ESYS_INVPARAMS);
         * CSL_EXIT_IF((_CSL_dctSetAutoGating(hndl, setup->Autogating) !=
         * CSL_SOK) , CSL_ESYS_INVPARAMS);
         * CSL_EXIT_IF((_CSL_dctSetNMcus(hndl, setup->nMcus) != CSL_SOK) ,
         * CSL_ESYS_INVPARAMS); CSL_EXIT_IF((_CSL_dctSetSpDataAddr(hndl,
         * setup->pSpDataAddr) != CSL_SOK) , CSL_ESYS_INVPARAMS);
         * CSL_EXIT_IF((_CSL_dctSetFrDataAddr(hndl, setup->pFreqDataAddr) !=
         * CSL_SOK) , CSL_ESYS_INVPARAMS);
         * CSL_EXIT_IF((_CSL_dctSetIntEnable(hndl, setup->IntEnable) !=
         * CSL_SOK), CSL_ESYS_INVPARAMS); */
        dctRegs = hndl->regs;
        CSL_FINS(dctRegs->CTRL, DCT_CTRL_EN, setup->HwEnable);
        CSL_FINS(dctRegs->CFG, DCT_CFG_MODE, setup->nModeType);
        CSL_FINS(dctRegs->CFG, DCT_CFG_FMT, setup->SpFmtType);
        CSL_FINS(dctRegs->CFG, DCT_CFG_TRIG_SRC, setup->TrgSrcType);
        CSL_FINS(dctRegs->CFG, DCT_CFG_AUTOGATING, setup->Autogating);
        CSL_FINS(dctRegs->CFG, DCT_CFG_NMCUS, setup->nMcus);
        dctRegs->SPTR = CSL_FMK(DCT_SPTR_ADDR, setup->pSpDataAddr);
        dctRegs->FPTR = CSL_FMK(DCT_FPTR_ADDR, setup->pFreqDataAddr);
        CSL_FINS(dctRegs->CFG, DCT_CFG_INTEN, setup->IntEnable);

      EXIT:
        return status;
    }

#if 0
    /* ================================================================ */
/**
*  CSL_dctHwControl() Sets the H/w for DCT module at one shot with the sttings provided by the 
* application
*
*  @param   hndl                    Handle  to the DCT object
*  @param   cmd                    CMD used to select a particular field name/register
*  @param...data                    pointer which Points to the data/value  being passed
*
*  @return         status      CSL_SOK Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
*================================================================== */

    CSL_Status CSL_dctHwControl(CSL_DctHandle hndl, CSL_DctHwCtrlCmd cmd,
                                void *data) {
        CSL_Status status = CSL_SOK;

        switch (cmd)
        {
            case CSL_DCT_CMD_SETMODE:
                status = _CSL_dctSetModeType(hndl, *((CSL_DctModeType *) data));
                break;

            case CSL_DCT_CMD_SETSPDATAFMT:
                status =
                    _CSL_dctSetSpDataType(hndl, *((CSL_DctSpDataType *) data));
                break;

            case CSL_DCT_CMD_SETTRIGGERSOURCE:
                status =
                    _CSL_dctSetTrigSrcType(hndl, *((CSL_DctTrgSrcType *) data));
                break;

            case CSL_DCT_CMD_SETAUTOGATING:
                status =
                    _CSL_dctSetAutoGating(hndl,
                                          *((CSL_DctAutoGatingType *) data));
                break;

            case CSL_DCT_CMD_SETSPDATAADDR:
                status = _CSL_dctSetSpDataAddr(hndl, *((Uint8 *) data));
                break;

            case CSL_DCT_CMD_SETFRDATAADDR:
                status = _CSL_dctSetFrDataAddr(hndl, *((Uint16 *) data));
                break;

            case CSL_DCT_CMD_SETNMCUS:
                status = _CSL_dctSetNMcus(hndl, *((Uint32 *) data));
                break;

            case CSL_DCT_CMD_ENABLE:
                status = _CSL_dctEnable(hndl, *((CSL_DctHwEnableType *) data));

            case CSL_DCT_CMD_ENABLEINT:
                status =
                    _CSL_dctSetIntEnable(hndl,
                                         *((CSL_DctIntEnableType *) data));
                break;

            default:
                status = CSL_ESYS_INVCMD;

        }
        return status;
    }

    /* ================================================================ */
/**
*  CSL_dctGetHwStatus() Sets the H/w for DCT module at one shot with the sttings provided by the 
* application
*
*  @param   hndl                    Handle  to the DCT object
*
*  @param   query                    CMD used to select a particular field name/register
*
*  @param...data                    pointer which Points to the data/value  being queried
*
*  @return         status      CSL_SOK Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
*================================================================== */

    CSL_Status CSL_dctGetHwStatus(CSL_DctHandle hndl, CSL_DctHwQuery query,
                                  void *data) {
        CSL_Status status = CSL_SOK;

        switch (query)
        {
            case CSL_DCT_QUERY_REV_ID:
                status = _CSL_dctGetRevId(hndl, (Uint16 *) data);
                break;

            case CSL_DCT_QUERY_MODE:
                status = _CSL_dctGetModeType(hndl, (CSL_DctModeType *) data);
                break;

            case CSL_DCT_QUERY_SPDATAFMT:
                status = _CSL_dctGetSpDataFmt(hndl, (CSL_DctSpDataType *) data);
                break;

            case CSL_DCT_QUERY_TRIGGERSOURCE:
                status =
                    _CSL_dctGetTrigSrcType(hndl, (CSL_DctTrgSrcType *) data);
                break;

            case CSL_DCT_QUERY_AUTOGATING:
                status =
                    _CSL_dctGetAutoGating(hndl, (CSL_DctAutoGatingType *) data);
                break;

            case CSL_DCT_QUERY_SPDATAADDR:
                status = _CSL_dctGetSpDataAddr(hndl, (Uint8 *) data);
                break;

            case CSL_DCT_QUERY_FRDATAADDR:
                status = _CSL_dctGetFrDataAddr(hndl, (Uint16 *) data);
                break;

            case CSL_DCT_QUERY_NMCUS:
                status = _CSL_dctGetNMcus(hndl, (Uint32 *) data);
                break;

            case CSL_DCT_QUERY_HWSTATUS:
                status = _CSL_dctGetState(hndl, (CSL_DctHwStatusType *) data);
                break;

            case CSL_DCT_QUERY_INTEN:
                status =
                    _CSL_dctIsIntEnabled(hndl, (CSL_DctIntEnableType *) data);
                break;

            default:
                status = CSL_ESYS_INVQUERY;

        }
        return status;
    }
#endif

#ifdef __cplusplus
}
#endif
#endif /*_CSL_DCT_C_*/

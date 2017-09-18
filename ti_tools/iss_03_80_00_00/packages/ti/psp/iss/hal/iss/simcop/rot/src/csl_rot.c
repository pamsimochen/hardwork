/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file csl_rot.c
*
* This file contains Level 1 CSL function prototypes
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

#ifndef _CSL_ROT_C_
#define _CSL_ROT_C_

#ifdef __cplusplus
extern "C" {
#endif

#include "../csl_rot.h"
#include "../inc/_csl_rot.h"
#include <ti/psp/iss/hal/iss/simcop/common/csl.h>
    /* #include <ti/psp/iss/hal/iss/simcop/common/csl_resource.h>
     * 
     * #include <ti/psp/iss/hal/iss/simcop/common/csl_types.h> #include
     * <ti/psp/iss/hal/iss/simcop/common/csl_error.h> #include
     * <ti/psp/iss/hal/iss/simcop/common/csl_resid.h> #include
     * <ti/psp/iss/hal/iss/simcop/common/simcop.h> */

    CSL_Status CSL_rotInit(CSL_RotHandle hndl) {

        _CSL_Rot_Reset_Val_Read(hndl);
        return CSL_SOK;
    } CSL_Status CSL_rotOpen(CSL_RotObj * hRotObj,
                             CSL_RotNum rotNum, CSL_OpenMode openMode) {
        CSL_Status status = CSL_SOK;

        CSL_RotNum rotInst;

        /* CSL_RotHandle hRot = (CSL_RotHandle)NULL; */

        hRotObj->openMode = openMode;

        if (rotNum >= 0)
        {
            status = _CSL_rotGetAttrs(rotNum, hRotObj);
            CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_OVFL);
            /* hRot = (CSL_RotHandle)_CSL_certifyOpen((CSL_ResHandle)hRotObj, 
             * status); */
        }
        else
        {
            for (rotInst = (CSL_RotNum) 0;
                 rotInst < (CSL_RotNum) CSL_ROT_PER_CNT; ++rotInst)
            {
                status = _CSL_rotGetAttrs(rotInst, hRotObj);
                /* hRot =
                 * (CSL_RotHandle)_CSL_certifyOpen((CSL_ResHandle)hRotObj,
                 * status); */
                if (status == CSL_SOK)
                    break;
            }
            /* if (rotInst == CSL_ROT_PER_CNT) status = CSL_ESYS_OVFL; */
        }
      EXIT:
        return status;
    }

    CSL_Status CSL_rotClose(CSL_RotHandle hndl) {
        /* Indicate in the CSL global data structure that the peripheral has
         * been unreserved */
        /* return (_CSL_certifyClose((CSL_ResHandle)hndl)); */
        return CSL_SOK;
    }

    CSL_Status CSL_rotHwSetup(CSL_RotHandle hndl, CSL_RotHwSetup * setup) {
        CSL_Status status = CSL_SOK;

        CSL_RotRegsOvly rotRegs;

        CSL_RotFmtType tFmt;

        CSL_EXIT_IF((hndl == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((hndl->regs == NULL), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((setup == NULL), CSL_ESYS_INVPARAMS);
        /* CSL_EXIT_IF((_CSL_rotEnable(hndl, setup->HWEnable) != CSL_SOK),
         * CSL_ESYS_INVPARAMS); CSL_EXIT_IF((_CSL_rotSetOpType(hndl,
         * setup->OpType) != CSL_SOK), CSL_ESYS_INVPARAMS);
         * CSL_EXIT_IF((_CSL_rotSetDataType(hndl, setup->FmtType) !=
         * CSL_SOK), CSL_ESYS_INVPARAMS);
         * CSL_EXIT_IF((_CSL_rotSetTrigSrcType(hndl, setup->TrgSrcType) !=
         * CSL_SOK) , CSL_ESYS_INVPARAMS);
         * CSL_EXIT_IF((_CSL_rotSetAutoGating(hndl, setup->Autogating) !=
         * CSL_SOK) , CSL_ESYS_INVPARAMS);
         * CSL_EXIT_IF((_CSL_rotSetBlockSize(hndl, setup->BlkParam) !=
         * CSL_SOK) , CSL_ESYS_INVPARAMS);
         * CSL_EXIT_IF((_CSL_rotSetStartAddrConfig(hndl,
         * setup->StartAddrConfig) != CSL_SOK) , CSL_ESYS_INVPARAMS);
         * CSL_EXIT_IF((_CSL_rotSetDestAddrConfig(hndl,
         * setup->DestAddrConfig) != CSL_SOK) , CSL_ESYS_INVPARAMS);
         * 
         * _CSL_Rot_Reset_Val_Read(hndl); */
        rotRegs = hndl->regs;
        CSL_FINS(rotRegs->CTRL, ROT_CTRL_EN, setup->HWEnable);
        CSL_FINS(rotRegs->CFG, ROT_CFG_OP, setup->OpType);
        CSL_FINS(rotRegs->CFG, ROT_CFG_FMT, setup->FmtType);
        CSL_FINS(rotRegs->CFG, ROT_CFG_TRIG_SRC, setup->TrgSrcType);
        CSL_FINS(rotRegs->CFG, ROT_CFG_AUTOGATING, setup->Autogating);
        CSL_FINS(rotRegs->CFG, ROT_CFG_NBLKS, setup->BlkParam.nBlocks);
        rotRegs->BLKSZ = CSL_FMK(ROT_BLKSZ_BLKW, setup->BlkParam.nBlockWidth) |
            CSL_FMK(ROT_BLKSZ_BLKH, setup->BlkParam.nBlockHeight);
        tFmt = setup->FmtType;
        /* tFmt = (CSL_RotFmtType)CSL_FEXT(rotRegs->CFG, ROT_CFG_FMT); */
        if (tFmt == CSL_ROT_CFG_FMT_YUV_420)
        {
            rotRegs->SRC_START2 =
                CSL_FMK(ROT_SRC_START2_ADDR,
                        setup->StartAddrConfig.nSourceStartUV);
        }
        rotRegs->SRC_START1 =
            CSL_FMK(ROT_SRC_START1_ADDR, setup->StartAddrConfig.nSourceStart);
        rotRegs->SRC_LOFST =
            CSL_FMK(ROT_SRC_LOFST_LOFST,
                    setup->StartAddrConfig.nSourceLineOffset);

        /* tFmt = (CSL_RotFmtType)CSL_FEXT(rotRegs->CFG, ROT_CFG_FMT); */
        if (tFmt == CSL_ROT_CFG_FMT_YUV_420)
        {
            rotRegs->DST_START2 =
                CSL_FMK(ROT_DST_START2_ADDR,
                        setup->DestAddrConfig.nDestStartUV);
        }
        rotRegs->DST_START1 =
            CSL_FMK(ROT_DST_START1_ADDR, setup->DestAddrConfig.nDestStart);
        rotRegs->DST_LOFST =
            CSL_FMK(ROT_DST_LOFST_LOFST, setup->DestAddrConfig.nDestLineOffset);

      EXIT:
        return status;
    }

#if 0
    CSL_Status CSL_rotHwControl(CSL_RotHandle hndl, CSL_RotHwCtrlCmd * cmd,
                                void *data) {
        CSL_Status status = CSL_SOK;

        switch (*cmd)
        {
            case CSL_ROT_CMD_SETOPERATION:
                status = _CSL_rotSetOpType(hndl, *((CSL_RotOpType *) data));
                break;

            case CSL_ROT_CMD_SETFMT:
                status = _CSL_rotSetDataType(hndl, *((CSL_RotFmtType *) data));
                break;

            case CSL_ROT_CMD_SETTRIGGERSOURCE:
                status =
                    _CSL_rotSetTrigSrcType(hndl, *((CSL_RotTrgSrcType *) data));
                break;

            case CSL_ROT_CMD_SETAUTOGATING:
                status =
                    _CSL_rotSetAutoGating(hndl,
                                          *((CSL_RotAutoGatingType *) data));
                break;

            case CSL_ROT_CMD_SETSTARTADDR:
                status =
                    _CSL_rotSetStartAddrConfig(hndl,
                                               *((CSL_RotStartAddrConfig *)
                                                 data));
                break;

            case CSL_ROT_CMD_SETDESTADDR:
                status =
                    _CSL_rotSetDestAddrConfig(hndl,
                                              *((CSL_RotDestAddrConfig *)
                                                data));
                break;

            case CSL_ROT_CMD_SETBLOCKPARAM:
                status =
                    _CSL_rotSetBlockSize(hndl,
                                         *((CSL_RotBlkSizeConfig *) data));
                break;

            case CSL_ROT_CMD_ENABLE:
                status = _CSL_rotEnable(hndl, *((CSL_RotHwEnableType *) data));
                break;

        }
        return status;
    }

    CSL_Status CSL_rotGetHwStatus(CSL_RotHandle hndl, CSL_RotHwQuery * query,
                                  void *data) {
        CSL_Status status = CSL_SOK;

        switch (*query)
        {
            case CSL_ROT_QUERY_REV_ID:
                status = _CSL_rotGetRevId(hndl, (Uint16 *) data);
                break;

            case CSL_ROT_QUERY_OPERATION:
                status = _CSL_rotGetOpType(hndl, (CSL_RotOpType *) data);
                break;

            case CSL_ROT_QUERY_FMT:
                status = _CSL_rotGetDataType(hndl, (CSL_RotFmtType *) data);
                break;

            case CSL_ROT_QUERY_TRIGGERSOURCE:
                status =
                    _CSL_rotGetTrigSrcType(hndl, (CSL_RotTrgSrcType *) data);
                break;

            case CSL_ROT_QUERY_AUTOGATING:
                status =
                    _CSL_rotGetAutoGating(hndl, (CSL_RotAutoGatingType *) data);
                break;

            case CSL_ROT_QUERY_STARTADDR:
                status =
                    _CSL_rotGetStartAddrConfig(hndl,
                                               (CSL_RotStartAddrConfig *) data);
                break;

            case CSL_ROT_QUERY_DESTADDR:
                status =
                    _CSL_rotGetDestAddrConfig(hndl,
                                              (CSL_RotDestAddrConfig *) data);
                break;

            case CSL_ROT_QUERY_BLOCKPARAM:
                status =
                    _CSL_rotGetBlockSize(hndl, (CSL_RotBlkSizeConfig *) data);
                break;

            case CSL_ROT_QUERY_HWSTATUS:
                status = _CSL_rotIsBusy(hndl, (CSL_RotHwStatusType *) data);
                break;

        }
        return status;
    }
#endif

#endif

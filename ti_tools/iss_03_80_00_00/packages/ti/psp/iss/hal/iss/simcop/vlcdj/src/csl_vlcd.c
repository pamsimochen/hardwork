/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file csl_vlcd.c
*
* This file contains register level values for VLCDJ module in SIMCOP in OMAP4/Monica
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
*! 19-Nov   2008 Sowmya Priya : Changes for byte alignment
*! 19-Mar-2009 Phanish: Modified to remove the HWcontrol and GetHWstatus APIs 
*!                 and their dependencies. And now, HWsetup calls directly the CSL_FINS 
*! 16-Apr-2009 Phanish: Changes made because of Resource Manager Integration. 
*!                       i.e., Removal of dependency on _CSL_certifyOpen() calls.
*========================================================================= */

#ifndef _CSL_VLCD_C_
#define _CSL_VLCD_C_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/
#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>
    // #include <ti/psp/iss/hal/iss/simcop/common/csl_resource.h"
#include "../cslr__vlcdj_1.h"
#include "../csl_vlcd.h"
#include "../inc/_csl_vlcd.h"

    /* ================================================================ */
/**
*  CSL_VlcdjInit() Initializes the VLCD settings
*
*  @param   hndl          Handle  to the VLCDJ object
*
*  @return         status      CSL_SOK Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
*================================================================== */

    CSL_Status CSL_VlcdjInit(CSL_VlcdjHandle hndl) {
        _CSL_Vlcdj_Reset_Val_Read(hndl);
        return CSL_SOK;
    }
/* ================================================================ *//**
*  CSL_VlcdjOpen() The open call sets up the data structures for the 
*  particular instance of the VLCDJ It gets a handle to the VLCDJ module of SIMCOP
*
*  @param   hVlcdjObj          Pointer to the VLCDJ object
*
*  @param   VlcdjNum           Instance of VLCDJ device=
*
*  @param   openMode       Open mode (Shared/Exclusive)\
*
*  @param   status      CSL_SOK Succesfull open \n
*                               Other value = Open failed and the error code is returned.
*
*  @return              hVlcdj =  Handle to VLCDJ  object
                                    NULL =failed 
*================================================================== */
        CSL_Status CSL_VlcdjOpen(CSL_VlcdjObj * hVlcdjObj,
                                 CSL_VlcdjNum VlcdjNum, CSL_OpenMode openMode) {

        CSL_Status status = CSL_SOK;

        CSL_VlcdjNum VlcdjInst;

        /* CSL_VlcdjHandle hVlcdj = (CSL_VlcdjHandle)NULL; */

        hVlcdjObj->openMode = openMode;

        if (VlcdjNum >= 0)
        {
            status = _CSL_VlcdjGetAttrs(VlcdjNum, hVlcdjObj);
            CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_OVFL);
            /* hVlcdj =
             * (CSL_VlcdjHandle)_CSL_certifyOpen((CSL_ResHandle)hVlcdjObj,
             * status); */
        }
        else
        {
            for (VlcdjInst = (CSL_VlcdjNum) 0;
                 VlcdjInst < (CSL_VlcdjNum) CSL_VLCDJ_PER_CNT; ++VlcdjInst)
            {
                status = _CSL_VlcdjGetAttrs(VlcdjInst, hVlcdjObj);
                /* hVlcdj =
                 * (CSL_VlcdjHandle)_CSL_certifyOpen((CSL_ResHandle)hVlcdjObj, 
                 * status); */
                if (status == CSL_SOK)
                    break;
            }

            /* if (VlcdjInst == CSL_VLCDJ_PER_CNT) status = CSL_ESYS_OVFL; */
        }

      EXIT:
        return status;
    }

    /* ================================================================ */
/**
*  CSL_VlcdClose() The close call deallocates resources for VLCDJ  for the 
*  particular instance of the VLCDJ for which the handle is passed 
*
*  @param   hndl          Handle  to the VLCDJ object

*  @return         status      CSL_SOK Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
*================================================================== */
    CSL_Status CSL_VlcdjClose(CSL_VlcdjHandle hndl) {
        /* Indicate in the CSL global data structure that the peripheral has
         * been unreserved */
        /* return (_CSL_certifyClose((CSL_ResHandle)hndl)); */
        return CSL_SOK;
    }

    /* ================================================================ */
/**
*  CSL_VlcdjHwSetup() Sets the H/w for VLCDJ module at one shot with the sttings provided by the 
* application
*
*  @param   hndl                    Handle  to the VLCDJ object
*  @param   setup................. Setup structure containing values for register settings

*  @return         status      CSL_SOK Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
*================================================================== */

    CSL_Status CSL_VlcdjHwSetup(CSL_VlcdjHandle hndl, CSL_VlcdHwSetup * setup) {
        CSL_Status status = CSL_ESYS_FAIL;

        // const Uint8 leastval =0;
        CSL_VlcdjRegsOvly VlcdjRegs;

        CSL_VLCDJ_REQUIRE((setup != NULL), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((hndl != NULL), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((hndl->regs != NULL), CSL_ESYS_INVPARAMS);

        CSL_VLCDJ_REQUIRE((setup->nIntenErr == 0 ||
                           setup->nIntenErr == 1), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->nRben == 0 ||
                           setup->nRben == 1), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->nClrrb == 0 ||
                           setup->nClrrb == 1), CSL_ESYS_INVPARAMS);
        /* Encode Param check */
        CSL_VLCDJ_REQUIRE((setup->nEncNmcus <= MAX_NMCUS), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->nEncRsten == 0 ||
                           setup->nEncRsten == 1), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->nEncRlocen == 0 ||
                           setup->nEncRlocen == 1), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->nEncDcpred[0] <= MAX_12BIT &&
                           setup->nEncDcpred[1] <= MAX_12BIT &&
                           setup->nEncDcpred[2] <= MAX_12BIT),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->pEncBitptr <= 8), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->pEncByteptr <= MAX_12BIT),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE(((setup->pEncCbufStart <= MAX_12BIT) &&
                           ((setup->pEncCbufStart & MAX_10BIT) == 0)),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE(((setup->pEncCbufEnd <= MAX_12BIT) &&
                           ((setup->pEncCbufEnd & MAX_10BIT) == 0)),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->nEncRstInc <= MAX_3BIT), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->nEncRstInit <= MAX_3BIT), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->nEncRstIntrvl <= MAX_10BIT),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->nEncRstPhase <= MAX_10BIT),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE(((setup->pEncQmr <= MAX_9BIT) &&
                           ((setup->pEncQmr & MAX_7BIT) == 0)),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE(((setup->pEncDct <= MAX_14BIT) &&
                           ((setup->pEncDct & MAX_4BIT) == 0)),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE(((setup->pEncVlctbl <= MAX_12BIT) &&
                           ((setup->pEncVlctbl & MAX_2BIT) == 0)),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE(((setup->pEncRstptr <= MAX_14BIT) &&
                           ((setup->pEncRstptr & MAX_4BIT) == 0)),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->nEncRstofst <= MAX_32BIT),
                          CSL_ESYS_INVPARAMS);
        /* Decoder Param check */
        CSL_VLCDJ_REQUIRE((setup->nDecNmcus <= MAX_NMCUS), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->nDecRsten == 0 ||
                           setup->nDecRsten == 1), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->nDecDcpred[0] <= MAX_12BIT &&
                           setup->nDecDcpred[1] <= MAX_12BIT &&
                           setup->nDecDcpred[2] <= MAX_12BIT),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->pDecBitptr <= 8), CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE((setup->pDecByteptr <= MAX_12BIT),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE(((setup->pDecCbufStart <= MAX_12BIT) &&
                           ((setup->pDecCbufStart & MAX_10BIT) == 0)),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE(((setup->pDecCbufEnd <= MAX_12BIT) &&
                           ((setup->pDecCbufEnd & MAX_10BIT) == 0)),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE(((setup->pDecQmr <= MAX_9BIT) &&
                           ((setup->pDecQmr & MAX_7BIT) == 0)),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE(((setup->pDecDct <= MAX_14BIT) &&
                           ((setup->pDecDct & MAX_4BIT) == 0)),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE(((setup->pDecCtrltbl <= MAX_12BIT) &&
                           ((setup->pDecCtrltbl & MAX_2BIT) == 0)),
                          CSL_ESYS_INVPARAMS);
        CSL_VLCDJ_REQUIRE(((setup->pDecDcdtbl[0] <= MAX_12BIT) &&
                           ((setup->pDecDcdtbl[0] & MAX_2BIT) == 0) &&
                           (setup->pDecDcdtbl[1] <= MAX_12BIT) &&
                           ((setup->pDecDcdtbl[1] & MAX_2BIT) == 0) &&
                           (setup->pDecDcdtbl[2] <= MAX_12BIT) &&
                           ((setup->pDecDcdtbl[2] & MAX_2BIT) == 0) &&
                           (setup->pDecDcdtbl[3] <= MAX_12BIT) &&
                           ((setup->pDecDcdtbl[3] & MAX_2BIT) == 0)),
                          CSL_ESYS_INVPARAMS);

        /* CSL_VLCDJ_ASSERT((_CSL_VlcdjSetRevId(hndl,setup->nRevId) !=
         * CSL_SOK), CSL_ESYS_INVPARAMS);
         * CSL_VLCDJ_ASSERT((_CSL_VlcdjEnable(hndl, setup->nEnable) !=
         * CSL_SOK), CSL_ESYS_INVPARAMS);
         * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetModeType (hndl, setup->nModeType)
         * != CSL_SOK), CSL_ESYS_INVPARAMS);
         * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetTrigSrcType (hndl,
         * setup->nTrgSrcType) != CSL_SOK), CSL_ESYS_INVPARAMS);
         * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetAutoGating
         * (hndl,setup->nAutogating) != CSL_SOK), CSL_ESYS_INVPARAMS);
         * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetInterruptEnable (hndl,
         * setup->nIntenDone) != CSL_SOK), CSL_ESYS_INVPARAMS);
         * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetInterruptDecErr (hndl,
         * setup->nIntenErr) != CSL_SOK), CSL_ESYS_INVPARAMS);
         * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetRbEnable (hndl,setup->nRben) !=
         * CSL_SOK), CSL_ESYS_INVPARAMS);
         * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetClrrb (hndl, setup->nClrrb) !=
         * CSL_SOK), CSL_ESYS_INVPARAMS); */

        VlcdjRegs = hndl->regs;
        CSL_FINS(VlcdjRegs->VLCDJ_REVISION, VLCDJ_VLCDJ_REVISION_REV,
                 setup->nRevId);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_EN, setup->nEnable);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_MODE,
                 setup->nModeType);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_TRIG_SRC,
                 setup->nTrgSrcType);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_AUTOGATING,
                 setup->nAutogating);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_INTEN_DONE,
                 setup->nIntenDone);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_INTEN_ERR,
                 setup->nIntenErr);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_RBEN, setup->nRben);
        CSL_FINS(VlcdjRegs->VLCDJ_CTRL, VLCDJ_VLCDJ_CTRL_CLRRB, setup->nClrrb);

        if (setup->nModeType == 0)                         /* Encode */
        {
            /* CSL_VLCDJ_ASSERT((_CSL_VlcdjSetEncNMcus
             * (hndl,setup->nEncNmcus) != CSL_SOK), CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetEncFmtDataType
             * (hndl,setup->nEncFmtType) != CSL_SOK), CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetEncRSTOffset
             * (hndl,setup->nEncRstofst) != CSL_SOK), CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetRSTEnc (hndl,
             * setup->nEncRsten,CSL_VLCDJ_RSTEN) != CSL_SOK),
             * CSL_ESYS_INVPARAMS); CSL_VLCDJ_ASSERT((_CSL_VlcdjSetRSTEnc
             * (hndl, setup->nEncRlocen,CSL_VLCDJ_RLOCEN) != CSL_SOK),
             * CSL_ESYS_INVPARAMS); CSL_VLCDJ_ASSERT((_CSL_VlcdjSetPREDEnc
             * (hndl,setup->nEncDcpred[0],CSL_VLCDJ_DCPREDY) != CSL_SOK),
             * CSL_ESYS_INVPARAMS); CSL_VLCDJ_ASSERT((_CSL_VlcdjSetPREDEnc
             * (hndl,setup->nEncDcpred[1],CSL_VLCDJ_DCPREDU) != CSL_SOK),
             * CSL_ESYS_INVPARAMS); CSL_VLCDJ_ASSERT((_CSL_VlcdjSetPREDEnc
             * (hndl,setup->nEncDcpred[2],CSL_VLCDJ_DCPREDV) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetEncPtrTable
             * (hndl,setup->pEncBitptr, CSL_VLCDJ_BSP_BITPTR) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetEncPtrTable
             * (hndl,setup->pEncByteptr, CSL_VLCDJ_BSP_BYTEPTR) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetEncPtrTable
             * (hndl,setup->pEncCbufStart, CSL_VLCDJ_CBUF_START) != CSL_SOK), 
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetEncPtrTable
             * (hndl,setup->pEncCbufEnd, CSL_VLCDJ_CBUF_END) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetEncPtrTable
             * (hndl,setup->pEncQmr, CSL_VLCDJ_QMR_PTR) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetEncPtrTable
             * (hndl,setup->pEncDct, CSL_VLCDJ_DCT_PTR) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetEncPtrTable
             * (hndl,setup->pEncVlctbl, CSL_VLCDJ_HUFF_PTR) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetEncPtrTable
             * (hndl,setup->pEncRstptr, CSL_VLCDJ_RST_PTR) != CSL_SOK),
             * CSL_ESYS_INVPARAMS); CSL_VLCDJ_ASSERT((_CSL_VlcdjSetRSTConfig
             * (hndl, setup->nEncRstInc,CSL_VLCDJ_INC) != CSL_SOK),
             * CSL_ESYS_INVPARAMS); CSL_VLCDJ_ASSERT((_CSL_VlcdjSetRSTConfig
             * (hndl, setup->nEncRstInit,CSL_VLCDJ_INIT) != CSL_SOK),
             * CSL_ESYS_INVPARAMS); CSL_VLCDJ_ASSERT((_CSL_VlcdjSetRSTConfig
             * (hndl, setup->nEncRstPhase, CSL_VLCDJ_PHASE) != CSL_SOK),
             * CSL_ESYS_INVPARAMS); CSL_VLCDJ_ASSERT((_CSL_VlcdjSetRSTConfig
             * (hndl, setup->nEncRstIntrvl, CSL_VLCDJ_INTRVL) != CSL_SOK),
             * CSL_ESYS_INVPARAMS); */
            CSL_FINS(VlcdjRegs->VLCDJE_CFG, VLCDJ_VLCDJE_CFG_NMCUS,
                     setup->nEncNmcus);
            CSL_FINS(VlcdjRegs->VLCDJE_CFG, VLCDJ_VLCDJE_CFG_FMT,
                     setup->nEncFmtType);
            CSL_FINS(VlcdjRegs->VLCDJE_RSTOFST, VLCDJ_VLCDJE_RSTOFST_OFFSET,
                     setup->nEncRstofst);
            CSL_FINS(VlcdjRegs->VLCDJE_CFG, VLCDJ_VLCDJE_CFG_RSTEN,
                     setup->nEncRsten);
            CSL_FINS(VlcdjRegs->VLCDJE_CFG, VLCDJ_VLCDJE_CFG_RLOCEN,
                     setup->nEncRlocen);
            CSL_FINS(VlcdjRegs->VLCDJE_DCPREDY, VLCDJ_VLCDJE_DCPREDY_PREDY,
                     setup->nEncDcpred[0]);
            CSL_FINS(VlcdjRegs->VLCDJE_DCPREDUV, VLCDJ_VLCDJE_DCPREDUV_PREDU,
                     setup->nEncDcpred[1]);
            CSL_FINS(VlcdjRegs->VLCDJE_DCPREDUV, VLCDJ_VLCDJE_DCPREDUV_PREDV,
                     setup->nEncDcpred[2]);
            CSL_FINS(VlcdjRegs->VLCDJE_BSPTR, VLCDJ_VLCDJE_BSPTR_BITPTR,
                     setup->pEncBitptr);
            CSL_FINS(VlcdjRegs->VLCDJE_BSPTR, VLCDJ_VLCDJE_BSPTR_BYTEPTR,
                     setup->pEncByteptr);
            CSL_FINS(VlcdjRegs->VLCDJE_CBUF, VLCDJ_VLCDJE_CBUF_START,
                     setup->pEncCbufStart);
            CSL_FINS(VlcdjRegs->VLCDJE_CBUF, VLCDJ_VLCDJE_CBUF_END,
                     setup->pEncCbufEnd);
            /* QUANTIZATION MATRIX */
            CSL_FINS(VlcdjRegs->VLCDJE_DCTQM, VLCDJ_VLCDJE_DCTQM_QMR,
                     setup->pEncQmr);
            /* DCT TABLES */
            CSL_FINS(VlcdjRegs->VLCDJE_DCTQM, VLCDJ_VLCDJE_DCTQM_DCT,
                     setup->pEncDct);
            CSL_FINS(VlcdjRegs->VLCDJE_VLCTBL, VLCDJ_VLCDJE_VLCTBL_ADDR,
                     setup->pEncVlctbl);
            CSL_FINS(VlcdjRegs->VLCDJE_RSTPTR, VLCDJ_VLCDJE_RSTPTR_ADDR,
                     setup->pEncRstptr);

            CSL_FINS(VlcdjRegs->VLCDJE_RSTCFG, VLCDJ_VLCDJE_RSTCFG_INC,
                     setup->nEncRstInc);
            CSL_FINS(VlcdjRegs->VLCDJE_RSTCFG, VLCDJ_VLCDJE_RSTCFG_INIT,
                     setup->nEncRstInit);
            CSL_FINS(VlcdjRegs->VLCDJE_RSTCFG, VLCDJ_VLCDJE_RSTCFG_PHASE,
                     setup->nEncRstPhase);
            CSL_FINS(VlcdjRegs->VLCDJE_RSTCFG, VLCDJ_VLCDJE_RSTCFG_INTRVL,
                     setup->nEncRstIntrvl);
        }

        else if (setup->nModeType == 1)                    /* Decode */
        {
            /* CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecNMcus (hndl,
             * setup->nDecNmcus) != CSL_SOK), CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecFmtDataType
             * (hndl,setup->nDecFmtType) != CSL_SOK), CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetRSTDec (hndl, setup->nDecRsten) 
             * != CSL_SOK), CSL_ESYS_INVPARAMS); CSL_VLCDJ_ASSERT((
             * _CSL_VlcdjSetPREDDec ( hndl,
             * setup->nDecDcpred[0],CSL_VLCDJ_DCPREDY)!= CSL_SOK),
             * CSL_ESYS_INVPARAMS); CSL_VLCDJ_ASSERT(( _CSL_VlcdjSetPREDDec ( 
             * hndl, setup->nDecDcpred[1],CSL_VLCDJ_DCPREDU)!= CSL_SOK),
             * CSL_ESYS_INVPARAMS); CSL_VLCDJ_ASSERT(( _CSL_VlcdjSetPREDDec ( 
             * hndl, setup->nDecDcpred[2],CSL_VLCDJ_DCPREDV)!= CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecPtrTable
             * (hndl,setup->pDecBitptr,CSL_VLCDJ_BSP_BITPTR) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecPtrTable
             * (hndl,setup->pDecByteptr,CSL_VLCDJ_BSP_BYTEPTR) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecPtrTable
             * (hndl,setup->pDecCbufStart,CSL_VLCDJ_CBUF_START) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecPtrTable
             * (hndl,setup->pDecCbufEnd,CSL_VLCDJ_CBUF_END) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecPtrTable
             * (hndl,setup->pDecQmr,CSL_VLCDJ_QMR_PTR) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecPtrTable
             * (hndl,setup->pDecDct,CSL_VLCDJ_DCT_PTR) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecPtrTable
             * (hndl,setup->pDecCtrltbl,CSL_VLCDJ_DEC_CTRLTBL) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecPtrTable
             * (hndl,setup->pDecDcdtbl[0],CSL_VLCDJ_DEC_HUFF0) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecPtrTable
             * (hndl,setup->pDecDcdtbl[1],CSL_VLCDJ_DEC_HUFF1) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecPtrTable
             * (hndl,setup->pDecDcdtbl[2],CSL_VLCDJ_DEC_HUFF2) != CSL_SOK),
             * CSL_ESYS_INVPARAMS);
             * CSL_VLCDJ_ASSERT((_CSL_VlcdjSetDecPtrTable
             * (hndl,setup->pDecDcdtbl[3],CSL_VLCDJ_DEC_HUFF3) != CSL_SOK),
             * CSL_ESYS_INVPARAMS); */
            CSL_FINS(VlcdjRegs->VLCDJD_CFG, VLCDJ_VLCDJD_CFG_NMCUS,
                     setup->nDecNmcus);
            CSL_FINS(VlcdjRegs->VLCDJD_CFG, VLCDJ_VLCDJD_CFG_FMT,
                     setup->nDecFmtType);
            CSL_FINS(VlcdjRegs->VLCDJD_CFG, VLCDJ_VLCDJD_CFG_RSTEN,
                     setup->nDecRsten);
            CSL_FINS(VlcdjRegs->VLCDJD_DCPREDY, VLCDJ_VLCDJD_DCPREDY_PREDY,
                     setup->nDecDcpred[0]);
            CSL_FINS(VlcdjRegs->VLCDJD_DCPREDUV, VLCDJ_VLCDJD_DCPREDUV_PREDU,
                     setup->nDecDcpred[1]);
            CSL_FINS(VlcdjRegs->VLCDJD_DCPREDUV, VLCDJ_VLCDJD_DCPREDUV_PREDV,
                     setup->nDecDcpred[2]);
            CSL_FINS(VlcdjRegs->VLCDJD_BSPTR, VLCDJ_VLCDJD_BSPTR_BITPTR,
                     setup->pDecBitptr);
            CSL_FINS(VlcdjRegs->VLCDJD_BSPTR, VLCDJ_VLCDJD_BSPTR_BYTEPTR,
                     setup->pDecByteptr);
            CSL_FINS(VlcdjRegs->VLCDJD_CBUF, VLCDJ_VLCDJD_CBUF_START,
                     setup->pDecCbufStart);

            CSL_FINS(VlcdjRegs->VLCDJD_CBUF, VLCDJ_VLCDJD_CBUF_END,
                     setup->pDecCbufEnd);
            /* QUANTIZATION MATRIX */
            CSL_FINS(VlcdjRegs->VLCDJD_DCTQM, VLCDJ_VLCDJD_DCTQM_QMR,
                     setup->pDecQmr);
            /* DCT TABLES */
            CSL_FINS(VlcdjRegs->VLCDJD_DCTQM, VLCDJ_VLCDJD_DCTQM_DCT,
                     setup->pDecDct);
            /* HUFF CONTROL TABLES */
            CSL_FINS(VlcdjRegs->VLCDJD_CTRLTBL, VLCDJ_VLCDJD_CTRLTBL_ADDR,
                     setup->pDecCtrltbl);
            /* HUFF TABLES */
            CSL_FINS(VlcdjRegs->VLCDJD_DCDTBL01, VLCDJ_VLCDJD_DCDTBL01_DCDTBL0,
                     setup->pDecDcdtbl[0]);
            /* HUFF TABLES */
            CSL_FINS(VlcdjRegs->VLCDJD_DCDTBL01, VLCDJ_VLCDJD_DCDTBL01_DCDTBL1,
                     setup->pDecDcdtbl[1]);
            /* HUFF TABLES */
            CSL_FINS(VlcdjRegs->VLCDJD_DCDTBL23, VLCDJ_VLCDJD_DCDTBL23_DCDTBL2,
                     setup->pDecDcdtbl[2]);
            /* HUFF TABLES */
            CSL_FINS(VlcdjRegs->VLCDJD_DCDTBL23, VLCDJ_VLCDJD_DCDTBL23_DCDTBL3,
                     setup->pDecDcdtbl[3]);
        }

        else
        {
            return CSL_ESYS_INVPARAMS;
        }

        CSL_VLCDJ_ENSURE((hndl != NULL), CSL_ESYS_BADHANDLE);
        status = CSL_SOK;
      EXIT:
        return status;

    }

#if 0
    /* ================================================================ */
/**
*  CSL_VlcdjHwControl() Sets the H/w for VLCDJ module at one shot with the sttings provided by the 
* application
*
*  @param   hndl                    Handle  to the VLCDJ object
*  @param   cmd                    CMD used to select a particular field name/register
*  @param...data                    pointer which Points to the data/value  being passed
*
*  @return         status      CSL_SOK Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
*================================================================== */

    CSL_Status CSL_VlcdjHwControl(CSL_VlcdjHandle hndl, CSL_VlcdjHwCtrlCmd cmd,
                                  void *data) {
        CSL_Status status = CSL_ESYS_FAIL;

        const Uint8 leastval = 0;

        switch (cmd)
        {

            case CSL_VLCDJ_CMD_SETREVID:
                status = _CSL_VlcdjSetRevId(hndl, *((Uint16 *) data));
                break;

            case CSL_VLCDJ_CMD_ENABLE:
                status =
                    _CSL_VlcdjEnable(hndl, *((CSL_VlcdjHwEnableType *) data));
                break;

            case CSL_VLCDJ_CMD_SETMODE:
                status =
                    _CSL_VlcdjSetModeType(hndl, *((CSL_VlcdjModeType *) data));
                break;

            case CSL_VLCDJ_CMD_SETTRIGGERSOURCE:
                status =
                    _CSL_VlcdjSetTrigSrcType(hndl,
                                             *((CSL_VlcdjTrgSrcType *) data));
                break;

            case CSL_VLCDJ_CMD_SETAUTOGATING:
                status =
                    _CSL_VlcdjSetAutoGating(hndl,
                                            *((CSL_VlcdjAutoGatingType *)
                                              data));
                break;

            case CSL_VLCDJ_CMD_ENABLEINT:
                status =
                    _CSL_VlcdjSetInterruptEnable(hndl,
                                                 *((CSL_VlcdjIntEnableType *)
                                                   data));
                break;

            case CSL_VLCDJ_CMD_ENABLEINT_ERR:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) == 0 ||
                                   *((Uint16 *) data) == 1),
                                  CSL_ESYS_INVPARAMS);
                status = _CSL_VlcdjSetInterruptDecErr(hndl, *((Uint16 *) data));
                break;

            case CSL_VLCDJ_CMD_CONFIGBANK:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) == 0 ||
                                   *((Uint16 *) data) == 1),
                                  CSL_ESYS_INVPARAMS);
                status = _CSL_VlcdjSetRbEnable(hndl, *((Uint16 *) data));
                break;

            case CSL_VLCDJ_CMD_CLEARBANK:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) == 0 ||
                                   *((Uint16 *) data) == 1),
                                  CSL_ESYS_INVPARAMS);
                status = _CSL_VlcdjSetClrrb(hndl, *((Uint16 *) data));
                break;

            case CSL_VLCDJE_CMD_SETNMCUS:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) > leastval &&
                                   *((Uint16 *) data) < MAX_NMCUS),
                                  CSL_ESYS_INVPARAMS);
                status = _CSL_VlcdjSetEncNMcus(hndl, *((Uint16 *) data));
                break;

            case CSL_VLCDJE_CMD_SETENCFMT:
                status =
                    _CSL_VlcdjSetEncFmtDataType(hndl,
                                                *((CSL_VlcdjEncodeFmtType *)
                                                  data));
                break;

            case CSL_VLCDJE_CMD_SETRST_OFFSET:
                CSL_VLCDJ_REQUIRE((*((Uint32 *) data) <= MAX_32BIT),
                                  CSL_ESYS_INVPARAMS);
                status = _CSL_VlcdjSetEncRSTOffset(hndl, *((Uint32 *) data));
                break;

            case CSL_VLCDJE_CMD_SETRST_EN:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) == 0 ||
                                   *((Uint16 *) data) == 1),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetRSTEnc(hndl, *((Uint16 *) data),
                                        CSL_VLCDJ_RSTEN);
                break;

            case CSL_VLCDJE_CMD_SETRST_LOC:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) == 0 ||
                                   *((Uint16 *) data) == 1),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetRSTEnc(hndl, *((Uint16 *) data),
                                        CSL_VLCDJ_RLOCEN);
                break;

            case CSL_VLCDJE_CMD_SETDCPREDY:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) <= MAX_12BIT),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetPREDEnc(hndl, *((Uint16 *) data),
                                         CSL_VLCDJ_DCPREDY);
                break;

            case CSL_VLCDJE_CMD_SETDCPREDU:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) <= MAX_12BIT),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetPREDEnc(hndl, *((Uint16 *) data),
                                         CSL_VLCDJ_DCPREDU);
                break;

            case CSL_VLCDJE_CMD_SETDCPREDV:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) <= MAX_12BIT),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetPREDEnc(hndl, *((Uint16 *) data),
                                         CSL_VLCDJ_DCPREDV);
                break;

            case CSL_VLCDJE_CMD_SETBITPTR:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) >= leastval &&
                                   *((Uint16 *) data) <= 8),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetEncPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_BSP_BITPTR);
                break;

            case CSL_VLCDJE_CMD_SETBYTEPTR:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) >= leastval &&
                                   *((Uint16 *) data) <= MAX_12BIT),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetEncPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_BSP_BYTEPTR);
                break;

            case CSL_VLCDJE_CMD_SETCBUFS:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_12BIT) &&
                                   ((*((Uint16 *) data) & MAX_10BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetEncPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_CBUF_START);
                break;

            case CSL_VLCDJE_CMD_SETCBUFE:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_12BIT) &&
                                   ((*((Uint16 *) data) & MAX_10BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetEncPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_CBUF_END);
                break;

            case CSL_VLCDJE_CMD_SETQMR:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_9BIT) &&
                                   ((*((Uint16 *) data) & MAX_7BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetEncPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_QMR_PTR);
                break;

            case CSL_VLCDJE_CMD_SETDCTPTR:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_14BIT) &&
                                   ((*((Uint16 *) data) & MAX_4BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetEncPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_DCT_PTR);
                break;

            case CSL_VLCDJE_CMD_SETVLCTBL:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_12BIT) &&
                                   ((*((Uint16 *) data) & MAX_2BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetEncPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_HUFF_PTR);
                break;

            case CSL_VLCDJE_CMD_RSTPTR:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_14BIT) &&
                                   ((*((Uint16 *) data) & MAX_4BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetEncPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_RST_PTR);
                break;

            case CSL_VLCDJE_CMD_RSTINC:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) <= MAX_3BIT),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetRSTConfig(hndl, *((Uint16 *) data),
                                           CSL_VLCDJ_INC);
                break;

            case CSL_VLCDJE_CMD_RSTINIT:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) <= MAX_3BIT),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetRSTConfig(hndl, *((Uint16 *) data),
                                           CSL_VLCDJ_INIT);
                break;

            case CSL_VLCDJE_CMD_RSTPHASE:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) <= MAX_10BIT),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetRSTConfig(hndl, *((Uint16 *) data),
                                           CSL_VLCDJ_PHASE);
                break;

            case CSL_VLCDJE_CMD_RSTINTRVL:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) <= MAX_10BIT),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetRSTConfig(hndl, *((Uint16 *) data),
                                           CSL_VLCDJ_INTRVL);
                break;

            case CSL_VLCDJD_CMD_SETNMCUS:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) > leastval &&
                                   *((Uint16 *) data) < MAX_NMCUS),
                                  CSL_ESYS_INVPARAMS);
                status = _CSL_VlcdjSetDecNMcus(hndl, *((Uint16 *) data));
                break;

            case CSL_VLCDJD_CMD_SETDECFMT:
                status =
                    _CSL_VlcdjSetDecFmtDataType(hndl,
                                                *((CSL_VlcdjDecodeFmtType *)
                                                  data));
                break;

            case CSL_VLCDJD_CMD_SETRSTEN:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) == 0 ||
                                   *((Uint16 *) data) == 1),
                                  CSL_ESYS_INVPARAMS);
                status = _CSL_VlcdjSetRSTDec(hndl, *((Uint16 *) data));
                break;

            case CSL_VLCDJD_CMD_SETDCPREDY:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) <= MAX_12BIT),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetPREDDec(hndl, *((Uint16 *) data),
                                         CSL_VLCDJ_DCPREDY);
                break;

            case CSL_VLCDJD_CMD_SETDCPREDU:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) <= MAX_12BIT),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetPREDDec(hndl, *((Uint16 *) data),
                                         CSL_VLCDJ_DCPREDU);
                break;

            case CSL_VLCDJD_CMD_SETDCPREDV:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) <= MAX_12BIT),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetPREDDec(hndl, *((Uint16 *) data),
                                         CSL_VLCDJ_DCPREDV);
                break;

            case CSL_VLCDJD_CMD_SETSETBITPTR:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) >= leastval &&
                                   *((Uint16 *) data) <= 8),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetDecPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_BSP_BITPTR);
                break;

            case CSL_VLCDJD_CMD_SETBYTEPTR:
                CSL_VLCDJ_REQUIRE((*((Uint16 *) data) >= leastval &&
                                   *((Uint16 *) data) <= MAX_12BIT),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetDecPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_BSP_BYTEPTR);
                break;

            case CSL_VLCDJD_CMD_SETCBUFS:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_12BIT) &&
                                   ((*((Uint16 *) data) & MAX_10BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetDecPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_CBUF_START);
                break;

            case CSL_VLCDJD_CMD_SETCBUFE:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_12BIT) &&
                                   ((*((Uint16 *) data) & MAX_10BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetDecPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_CBUF_END);
                break;

            case CSL_VLCDJD_CMD_SETQMR:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_9BIT) &&
                                   ((*((Uint16 *) data) & MAX_7BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetDecPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_QMR_PTR);
                break;

            case CSL_VLCDJD_CMD_SETDCTPTR:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_14BIT) &&
                                   ((*((Uint16 *) data) & MAX_4BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetDecPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_DCT_PTR);
                break;

            case CSL_VLCDJD_CMD_SETVLCTBL:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_12BIT) &&
                                   ((*((Uint16 *) data) & MAX_2BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetDecPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_DEC_CTRLTBL);
                break;

            case CSL_VLCDJD_CMD_SETDEC_HUFF0:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_12BIT) &&
                                   ((*((Uint16 *) data) & MAX_2BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetDecPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_DEC_HUFF0);
                break;

            case CSL_VLCDJD_CMD_SETDEC_HUFF1:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_12BIT) &&
                                   ((*((Uint16 *) data) & MAX_2BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetDecPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_DEC_HUFF1);
                break;
            case CSL_VLCDJD_CMD_SETDEC_HUFF2:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_12BIT) &&
                                   ((*((Uint16 *) data) & MAX_2BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetDecPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_DEC_HUFF2);
                break;

            case CSL_VLCDJD_CMD_SETDEC_HUFF3:
                CSL_VLCDJ_REQUIRE(((*((Uint16 *) data) <= MAX_12BIT) &&
                                   ((*((Uint16 *) data) & MAX_2BIT) == 0)),
                                  CSL_ESYS_INVPARAMS);
                status =
                    _CSL_VlcdjSetDecPtrTable(hndl, *((Uint16 *) data),
                                             CSL_VLCDJ_DEC_HUFF3);
                break;

            default:
                return CSL_ESYS_INVCMD;

        }

        CSL_VLCDJ_ENSURE((hndl != NULL), CSL_ESYS_BADHANDLE);
      EXIT:
        return status;

    }

    /* ================================================================ */
/**
*  CSL_VlcdjGetHwStatus() Sets the H/w for VLCDJ module at one shot with the sttings provided by the 
* application
*
*  @param   hndl                    Handle  to the VLCDJ object
*
*  @param   query                    CMD used to select a particular field name/register
*
*  @param...data                    pointer which Points to the data/value  being queried
*
*  @return         status      CSL_SOK Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
*================================================================== */

    CSL_Status CSL_VlcdjGetHwStatus(CSL_VlcdjHandle hndl,
                                    CSL_VlcdjHwQuery query, void *data) {
        CSL_Status status = CSL_ESYS_FAIL;

        switch (query)
        {
            case CSL_VLCDJ_QUERY_REVID:
                status = _CSL_VlcdjGetRevId(hndl, (Uint16 *) data);
                break;

            case CSL_VLCDJ_QUERY_MODE:
                status =
                    _CSL_VlcdjGetModeType(hndl, (CSL_VlcdjModeType *) data);
                break;

            case CSL_VLCDJ_QUERY_TRIGGERSOURCE:
                status =
                    _CSL_VlcdjGetTrigSrcType(hndl,
                                             (CSL_VlcdjTrgSrcType *) data);
                break;

            case CSL_VLCDJ_QUERY_AUTOGATING:
                status =
                    _CSL_VlcdjGetAutoGating(hndl,
                                            (CSL_VlcdjAutoGatingType *) data);
                break;

            case CSL_VLCDJ_QUERY_ENABLEINT:
                status = _CSL_VlcdjGetnterruptEnable(hndl, (Uint16 *) data);
                break;

            case CSL_VLCDJ_QUERY_INTDECERR:
                status = _CSL_VlcdjGetInterruptDecErr(hndl, (Uint16 *) data);
                break;

            case CSL_VLCDJ_QUERY_ENABLEBANK:
                status = _CSL_VlcdjGetRbEnable(hndl, (Uint16 *) data);
                break;

            case CSL_VLCDJ_QUERY_BUSY:
                status =
                    _CSL_VlcdjGetState(hndl, (CSL_VlcdjHwStatusType *) data);
                break;

            case CSL_VLCDJ_QUERY_BANK0:
                status = _CSL_VlcdjStatusBnk0(hndl, (Uint16 *) data);
                break;

            case CSL_VLCDJ_QUERY_BANK1:
                status = _CSL_VlcdjStatusBnk1(hndl, (Uint16 *) data);
                break;

            case CSL_VLCDJE_QUERY_NMCUS:
                status = _CSL_VlcdjGetEncNMcus(hndl, (Uint16 *) data);
                break;

            case CSL_VLCDJE_QUERY_ENCFMT:
                status =
                    _CSL_VlcdjGetEncFmtDataType(hndl,
                                                (CSL_VlcdjEncodeFmtType *)
                                                data);
                break;

            case CSL_VLCDJE_QUERY_RST_LOC:
                status =
                    _CSL_VlcdjGetRSTEnc(hndl, (Uint16 *) data,
                                        CSL_VLCDJ_RLOCEN);
                break;

            case CSL_VLCDJE_QUERY_RST_EN:
                status =
                    _CSL_VlcdjGetRSTEnc(hndl, (Uint16 *) data, CSL_VLCDJ_RSTEN);
                break;

            case CSL_VLCDJE_QUERY_RST_OFFSET:
                status = _CSL_VlcdjGetRSTOffSet(hndl, (Uint32 *) data);
                break;

            case CSL_VLCDJE_QUERY_DCPREDY:
                status =
                    _CSL_VlcdjGetPREDEnc(hndl, (Uint16 *) data,
                                         CSL_VLCDJ_DCPREDY);
                break;

            case CSL_VLCDJE_QUERY_DCPREDU:
                status =
                    _CSL_VlcdjGetPREDEnc(hndl, (Uint16 *) data,
                                         CSL_VLCDJ_DCPREDU);
                break;

            case CSL_VLCDJE_QUERY_DCPREDV:
                status =
                    _CSL_VlcdjGetPREDEnc(hndl, (Uint16 *) data,
                                         CSL_VLCDJ_DCPREDV);

            case CSL_VLCDJE_QUERY_BITPTR:
                status =
                    _CSL_VlcdjGetEncPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_BSP_BITPTR);
                break;

            case CSL_VLCDJE_QUERY_BYTEPTR:
                status =
                    _CSL_VlcdjGetEncPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_BSP_BYTEPTR);
                break;

            case CSL_VLCDJE_QUERY_CBUFS:
                status =
                    _CSL_VlcdjGetEncPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_CBUF_START);
                break;

            case CSL_VLCDJE_QUERY_CBUFE:
                status =
                    _CSL_VlcdjGetEncPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_CBUF_END);
                break;

            case CSL_VLCDJE_QUERY_QMR:
                status =
                    _CSL_VlcdjGetEncPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_QMR_PTR);
                break;

            case CSL_VLCDJE_QUERY_DCTPTR:
                status =
                    _CSL_VlcdjGetEncPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_DCT_PTR);
                break;

            case CSL_VLCDJE_QUERY_VLCTBL:
                status =
                    _CSL_VlcdjGetEncPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_HUFF_PTR);
                break;

            case CSL_VLCDJE_QUERY_RSTPTR:
                status =
                    _CSL_VlcdjGetEncPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_RST_PTR);
                break;

            case CSL_VLCDJE_QUERY_RSTINC:
                status =
                    _CSL_VlcdjGetRSTConfig(hndl, (Uint16 *) data,
                                           CSL_VLCDJ_INC);
                break;

            case CSL_VLCDJE_QUERY_RSTINIT:
                status =
                    _CSL_VlcdjGetRSTConfig(hndl, (Uint16 *) data,
                                           CSL_VLCDJ_INIT);
                break;

            case CSL_VLCDJE_QUERY_RSTPHASE:
                status =
                    _CSL_VlcdjGetRSTConfig(hndl, (Uint16 *) data,
                                           CSL_VLCDJ_PHASE);
                break;

            case CSL_VLCDJE_QUERY_RSTINTRVL:
                status =
                    _CSL_VlcdjGetRSTConfig(hndl, (Uint16 *) data,
                                           CSL_VLCDJ_INTRVL);
                break;

            case CSL_VLCDJD_QUERY_NMCUS:
                status = _CSL_VlcdjGetDecNMcus(hndl, (Uint16 *) data);
                break;

            case CSL_VLCDJD_QUERY_DECFMT:
                status =
                    _CSL_VlcdjGetDecFmtDataType(hndl,
                                                (CSL_VlcdjDecodeFmtType *)
                                                data);
                break;

            case CSL_VLCDJD_QUERY_RSTEN:
                status = _CSL_VlcdjGetRSTDec(hndl, (Uint16 *) data);
                break;

            case CSL_VLCDJD_QUERY_DCPREDY:
                status =
                    _CSL_VlcdjGetPREDDec(hndl, (Uint16 *) data,
                                         CSL_VLCDJ_DCPREDY);
                break;

            case CSL_VLCDJD_QUERY_DCPREDU:
                status =
                    _CSL_VlcdjGetPREDDec(hndl, (Uint16 *) data,
                                         CSL_VLCDJ_DCPREDU);
                break;

            case CSL_VLCDJD_QUERY_DCPREDV:
                status =
                    _CSL_VlcdjGetPREDDec(hndl, (Uint16 *) data,
                                         CSL_VLCDJ_DCPREDV);
                break;

            case CSL_VLCDJD_QUERY_GETBITPTR:
                status =
                    _CSL_VlcdjGetDecPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_BSP_BITPTR);
                break;

            case CSL_VLCDJD_QUERY_BYTEPTR:
                status =
                    _CSL_VlcdjGetDecPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_BSP_BYTEPTR);
                break;

            case CSL_VLCDJD_QUERY_CBUFS:
                status =
                    _CSL_VlcdjGetDecPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_CBUF_START);
                break;

            case CSL_VLCDJD_QUERY_CBUFE:
                status =
                    _CSL_VlcdjGetDecPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_CBUF_END);
                break;

            case CSL_VLCDJD_QUERY_QMR:
                status =
                    _CSL_VlcdjGetDecPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_QMR_PTR);
                break;

            case CSL_VLCDJD_QUERY_DCTPTR:
                status =
                    _CSL_VlcdjGetDecPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_DCT_PTR);
                break;

            case CSL_VLCDJD_QUERY_VLCTBL:
                status =
                    _CSL_VlcdjGetDecPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_DEC_CTRLTBL);
                break;

            case CSL_VLCDJD_QUERY_DEC_HUFF0:
                status =
                    _CSL_VlcdjGetDecPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_DEC_HUFF0);
                break;

            case CSL_VLCDJD_QUERY_DEC_HUFF1:
                status =
                    _CSL_VlcdjGetDecPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_DEC_HUFF1);
                break;

            case CSL_VLCDJD_QUERY_DEC_HUFF2:
                status =
                    _CSL_VlcdjGetDecPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_DEC_HUFF2);
                break;

            case CSL_VLCDJD_QUERY_DEC_HUFF3:
                status =
                    _CSL_VlcdjGetDecPtrTable(hndl, (Uint16 *) data,
                                             CSL_VLCDJ_DEC_HUFF3);
                break;

            case CSL_VLCDJD_QUERY_DEC_ERROR:
                status = _CSL_VlcdjGetDecErr(hndl, (Uint16 *) data);
                break;

            default:
                return CSL_ESYS_INVPARAMS;

        }
        CSL_VLCDJ_ENSURE((hndl != NULL), CSL_ESYS_BADHANDLE);
      EXIT:
        return status;

    }
#endif

#endif /*_CSL_VLCD_C_*/

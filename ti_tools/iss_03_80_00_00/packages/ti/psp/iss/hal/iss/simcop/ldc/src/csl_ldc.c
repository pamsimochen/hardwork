/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file csl_ldc.c
*
* This File contains CSL APIs for LDC Module
* This entire description will appear as one 
* paragraph in the generated documentation.
*
* @path  $(CSLPATH)\src\LDC\
*
* @rev  00.01
*/
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 19-Aug -2008 Padmanabha V Reddy:  Created the file.  
 *! 
 *!
 *! 24-Dec-2000 mf: Revisions appear in reverse chronological order;
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */

/****************************************************************
*  INCLUDE FILES                                                 
****************************************************************/
/*-------program files ----------------------------------------*/
#include "../csl_ldc.h"
#include "../inc/_csl_ldc.h"
// #include <ti/psp/iss/hal/iss/simcop/common/csl_resource.h>

/* ========================================================================== 
 */
/**
* CSL_ldcInit() Initializes the LDC settings
* @param  hndl  Handle to the LDC object
* @return  status  CSL_SOK Succesfull open
*                         Other value = Open failed and the error code is returned.
*/
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     CSL_ldcInit                                               
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
CSL_Status CSL_ldcInit(CSL_LdcHandle hndl)
{
    _CSL_ldcRegisterReset(hndl);
    return CSL_SOK;
}

/* ========================================================================== 
 */
/**
* CSL_ldcOpen() The open call sets up the data structures for the particular instance of the DCT. 
* It gets a handle to the LDC module of SIMCOP
* @param  hndl  Handle to the LDC object
* @param  hLdcObj  Pointer to the DCT object
* @param  ldcNum  Instance of DCT device=
* @param  openMode  Open mode (Shared/Exclusive)
* @param  status  CSL_SOK Succesfull open
*                         Other value = Open failed and the error code is returned.
* @return   hLdc =  Handle to LDC  object
*                       =  NULL => failed 
*/
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     CSL_ldcOpen                                               
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
CSL_Status CSL_ldcOpen(CSL_LdcObj * hLdcObj, CSL_LdcNum ldcNum,
                       CSL_OpenMode openMode)
{
    CSL_Status status = CSL_SOK;

    CSL_LdcNum ldcInst;

    // CSL_LdcHandle hLdc = (CSL_LdcHandle)NULL;

    hLdcObj->openMode = openMode;

    if (ldcNum >= 0)
    {
        _CSL_ldcGetAttrs(ldcNum, hLdcObj);
        // hLdc = (CSL_LdcHandle)_CSL_certifyOpen((CSL_ResHandle)hLdcObj,
        // status);
    }
    else
    {
        for (ldcInst = (CSL_LdcNum) 0; ldcInst < (CSL_LdcNum) CSL_LDC_PER_CNT;
             ++ldcInst)
        {
            _CSL_ldcGetAttrs(ldcInst, hLdcObj);
            // hLdc = (CSL_LdcHandle)_CSL_certifyOpen((CSL_ResHandle)hLdcObj, 
            // status);
            if (status == CSL_SOK)
                break;
        }
        // if (ldcInst == CSL_LDC_PER_CNT) 
        // *status = CSL_ESYS_OVFL;
    }

    return status;
}

/* ========================================================================== 
 */
/**
* CSL_ldcHwSetup() Sets the H/w for LDC module at one shot with the sttings provided by the application
* @param  hndl  Handle to the LDC object
* @param  setup  Setup structure containing values for register settings
* @return  status  CSL_SOK Succesfull Close
*                         Other value = Close failed and the error code is returned.
*/
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     CSL_ldcHwSetup                                               
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
CSL_Status CSL_ldcHwSetup(CSL_LdcHandle hndl, CSL_LdcHwSetupCtrl * setup)
{
    CSL_Status status = CSL_SOK;

    CSL_EXIT_IF((setup == NULL), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((setup->frame_sizeh > 16383), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((setup->frame_sizeh % setup->ld_obh), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((setup->frame_sizew > 16383), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((setup->frame_sizew % setup->ld_obw), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((setup->ld_initx > 16383), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((setup->ld_initx & 0x1), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((setup->ld_inity > 16383), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((setup->ld_inity & 0x0001), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF(setup->frame_wrbase & 0x1F, CSL_ESYS_INVPARAMS);
#ifndef LDC_SUPPORTS_FRAME_OFST_TO_BE_NON_MULTIPLES_OF_32
    CSL_EXIT_IF(setup->frame_wroffset & 0x1F, CSL_ESYS_INVPARAMS);
#endif
    CSL_EXIT_IF(setup->frame_rdbase & 0x1F, CSL_ESYS_INVPARAMS);
#ifndef LDC_SUPPORTS_FRAME_OFST_TO_BE_NON_MULTIPLES_OF_32
    CSL_EXIT_IF(setup->frame_rdoffset & 0x1F, CSL_ESYS_INVPARAMS);
#endif
    if (CSL_LDC_MODE_YCBCR420LD == setup->data_format)
    {
        CSL_EXIT_IF(setup->frame_rdbase420c & 0x1F, CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF(setup->frame_wrbase420c & 0x1F, CSL_ESYS_INVPARAMS);
    }
    CSL_EXIT_IF(setup->ld_rth > 16383, CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF(setup->ld_t > 15, CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF(setup->ld_v0 > 16383, CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF(setup->ld_h0 > 16383, CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF(setup->ld_pixelpad < 2 ||
                setup->ld_pixelpad > 15, CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF(setup->ld_obh < 1 || setup->ld_obh & 0x1 ||
                setup->ld_obh > setup->frame_sizeh, CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF(setup->ld_obw < 1 ||
                setup->ld_obw > setup->frame_sizew, CSL_ESYS_INVPARAMS);
#ifndef LDC_SUPPORTS_BLOCK_WIDTH_TO_BE_NON_MULTIPLES_OF_32
    if (CSL_LDC_MODE_YCBCR420LD == setup->data_format)
        CSL_EXIT_IF(setup->ld_obw & 0x1F, CSL_ESYS_INVPARAMS)
        else
#endif
    if (CSL_LDC_MODE_YCBCR422LD == setup->data_format)
        CSL_EXIT_IF(setup->ld_obw & 0xF, CSL_ESYS_INVPARAMS)
        else
    if (CSL_LDC_MODE_BAYERCA == setup->data_format)
    {
        if (CSL_LDC_BMODE_ALAW == setup->bayer_format ||
            CSL_LDC_BMODE_PKD8BIT == setup->bayer_format)
            CSL_EXIT_IF(setup->ld_obw & 0x1F, CSL_ESYS_INVPARAMS)
            else
        if (CSL_LDC_BMODE_PKD12BIT == setup->bayer_format)
            CSL_EXIT_IF(setup->ld_obw & 0x3F, CSL_ESYS_INVPARAMS)
            else
        if (CSL_LDC_BMODE_UNPKD12BIT == setup->bayer_format)
            CSL_EXIT_IF(setup->ld_obw & 0xF, CSL_ESYS_INVPARAMS);
    }
    CSL_EXIT_IF(setup->affine_a > 16383, CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF(setup->affine_b > 16383, CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF(setup->affine_d > 16383, CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF(setup->affine_e > 16383, CSL_ESYS_INVPARAMS);

    CSL_EXIT_IF((_CSL_ldcEnable(hndl, setup->en) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcmapEnable(hndl, setup->ldmapen) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetMode(hndl, setup->data_format) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetBayerMode(hndl, setup->bayer_format) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetFrameReadBase(hndl, setup->frame_rdbase) !=
                 CSL_SOK), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetReadOffset(hndl, setup->frame_rdoffset) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetFrameHeight(hndl, setup->frame_sizeh) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetFrameWidth(hndl, setup->frame_sizew) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetInitY(hndl, setup->ld_inity) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetInitX(hndl, setup->ld_initx) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetFrameWriteBase(hndl, setup->frame_wrbase) !=
                 CSL_SOK), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetWriteOffset(hndl, setup->frame_wroffset) !=
                 CSL_SOK), CSL_ESYS_INVPARAMS);
    if (CSL_LDC_MODE_YCBCR420LD == setup->data_format)
    {
        CSL_EXIT_IF((_CSL_ldcSetReadBase420C(hndl, setup->frame_rdbase420c) !=
                     CSL_SOK), CSL_ESYS_INVPARAMS);
        CSL_EXIT_IF((_CSL_ldcSetWriteBase420C(hndl, setup->frame_wrbase420c) !=
                     CSL_SOK), CSL_ESYS_INVPARAMS);
    }
    CSL_EXIT_IF((_CSL_ldcSetBackMappingThreshold(hndl, setup->ld_rth) !=
                 CSL_SOK), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetConstantOutputAddressMode(hndl, setup->ld_const_md)
                 != CSL_SOK), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetYInterpolationType(hndl, setup->ld_yint_typ) !=
                 CSL_SOK), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetInitialColor(hndl, setup->ld_initc) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetRightShiftBits(hndl, setup->ld_t) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetLensCenterY(hndl, setup->ld_v0) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetLensCenterX(hndl, setup->ld_h0) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetScalingFactorKvl(hndl, setup->ld_kvL) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetScalingFactorKvu(hndl, setup->ld_kvU) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetScalingFactorKhr(hndl, setup->ld_khR) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetScalingFactorKhl(hndl, setup->ld_khL) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetPixPad(hndl, setup->ld_pixelpad) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetOBH(hndl, setup->ld_obh) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetOBW(hndl, setup->ld_obw) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetAffineCoeffA(hndl, setup->affine_a) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetAffineCoeffB(hndl, setup->affine_b) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetAffineCoeffC(hndl, setup->affine_c) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetAffineCoeffD(hndl, setup->affine_d) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetAffineCoeffE(hndl, setup->affine_e) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_ldcSetAffineCoeffF(hndl, setup->affine_f) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    if (setup->ldmapen)
        CSL_EXIT_IF((_CSL_ldcSetLUT(hndl, setup->ld_lut) != CSL_SOK),
                    CSL_ESYS_INVPARAMS);
    // CSL_EXIT_IF((_CSL_ldcSetLUTAddress(hndl, setup->ld_lutadd) != CSL_SOK) 
    // , CSL_ESYS_INVPARAMS); 
    // CSL_EXIT_IF((_CSL_ldcSetLUTWriteData(hndl, setup->ld_lutwrdata) !=
    // CSL_SOK) , CSL_ESYS_INVPARAMS); 

  EXIT:
    return status;
}

/* ========================================================================== 
 */
/**
* CSL_ldcHwControl() Sets the H/w for LDC module as requested by the application
* @param  hndl  Handle to the LDC object
* @param  cmd  CMD used to select a particular field name/register
* @param  data  pointer which Points to the data/value  being passed
* @return  status  CSL_SOK Succesfull Setup
*                         Other value = Setup failed and the error code is returned.
*/
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     CSL_ldcHwControl                                               
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
CSL_Status CSL_ldcHwControl(CSL_LdcHandle hndl, CSL_LdcHwCtrlCmdType * cmd,
                            void *data)
{
    CSL_Status status = CSL_SOK;

    switch (*cmd)
    {
        case CSL_LDC_CMD_ENABLE:
            status = _CSL_ldcEnable(hndl, *(CSL_LdcHwEnableType *) data);
            break;

        case CSL_LDC_CMD_ENABLEMAP:
            status = _CSL_ldcmapEnable(hndl, *(CSL_LdcHwEnableType *) data);
            break;

        case CSL_LDC_CMD_SETREADBASE:
            CSL_EXIT_IF(*(Uint32 *) data & 0x1F, CSL_ESYS_INVPARAMS);
            status = _CSL_ldcSetFrameReadBase(hndl, *(Uint32 *) data);
            break;

        case CSL_LDC_CMD_SETREADOFFSET:
            CSL_EXIT_IF(*(Uint32 *) data & 0x1F, CSL_ESYS_INVPARAMS);
            status = _CSL_ldcSetReadOffset(hndl, *(Uint16 *) data);
            break;

        case CSL_LDC_CMD_SET_DATAFORMAT_FRAMESIZE_OUTPUTBLOCKSIZE:
            status =
                _CSL_ldcSetMode(hndl,
                                ((CSL_LdcFrameSizeDataFormat *) data)->
                                data_format);
            status =
                _CSL_ldcSetBayerMode(hndl,
                                     ((CSL_LdcFrameSizeDataFormat *) data)->
                                     bayer_format);
            CSL_EXIT_IF(((CSL_LdcFrameSizeDataFormat *) data)->frame_sizeh >
                        16383, CSL_ESYS_INVPARAMS);
            CSL_EXIT_IF(((CSL_LdcFrameSizeDataFormat *) data)->frame_sizeh %
                        ((CSL_LdcFrameSizeDataFormat *) data)->ld_obh,
                        CSL_ESYS_INVPARAMS);
            status =
                _CSL_ldcSetFrameHeight(hndl,
                                       ((CSL_LdcFrameSizeDataFormat *) data)->
                                       frame_sizeh);
            CSL_EXIT_IF(((CSL_LdcFrameSizeDataFormat *) data)->frame_sizew >
                        16383, CSL_ESYS_INVPARAMS);
            CSL_EXIT_IF(((CSL_LdcFrameSizeDataFormat *) data)->frame_sizew %
                        ((CSL_LdcFrameSizeDataFormat *) data)->ld_obw,
                        CSL_ESYS_INVPARAMS);
            status =
                _CSL_ldcSetFrameWidth(hndl,
                                      ((CSL_LdcFrameSizeDataFormat *) data)->
                                      frame_sizew);
            CSL_EXIT_IF(((CSL_LdcFrameSizeDataFormat *) data)->ld_obh < 1 ||
                        ((CSL_LdcFrameSizeDataFormat *) data)->ld_obh & 0x1 ||
                        ((CSL_LdcFrameSizeDataFormat *) data)->ld_obh >
                        ((CSL_LdcFrameSizeDataFormat *) data)->frame_sizeh,
                        CSL_ESYS_INVPARAMS);
            status =
                _CSL_ldcSetOBH(hndl,
                               ((CSL_LdcFrameSizeDataFormat *) data)->ld_obh);

            CSL_EXIT_IF(((CSL_LdcFrameSizeDataFormat *) data)->ld_obw < 1 ||
                        ((CSL_LdcFrameSizeDataFormat *) data)->ld_obw >
                        ((CSL_LdcFrameSizeDataFormat *) data)->frame_sizew,
                        CSL_ESYS_INVPARAMS);
            if (CSL_LDC_MODE_YCBCR420LD ==
                ((CSL_LdcFrameSizeDataFormat *) data)->data_format)
                CSL_EXIT_IF(((CSL_LdcFrameSizeDataFormat *) data)->
                            ld_obw & 0x1F, CSL_ESYS_INVPARAMS)
                else
            if (CSL_LDC_MODE_YCBCR422LD ==
                ((CSL_LdcFrameSizeDataFormat *) data)->data_format)
                CSL_EXIT_IF(((CSL_LdcFrameSizeDataFormat *) data)->ld_obw & 0xF,
                            CSL_ESYS_INVPARAMS)
                else
            if (CSL_LDC_MODE_BAYERCA ==
                ((CSL_LdcFrameSizeDataFormat *) data)->data_format)
            {
                if (CSL_LDC_BMODE_ALAW ==
                    ((CSL_LdcFrameSizeDataFormat *) data)->bayer_format ||
                    CSL_LDC_BMODE_PKD8BIT ==
                    ((CSL_LdcFrameSizeDataFormat *) data)->bayer_format)
                    CSL_EXIT_IF(((CSL_LdcFrameSizeDataFormat *) data)->
                                ld_obw & 0x1F, CSL_ESYS_INVPARAMS)
                    else
                if (CSL_LDC_BMODE_PKD12BIT ==
                    ((CSL_LdcFrameSizeDataFormat *) data)->bayer_format)
                    CSL_EXIT_IF(((CSL_LdcFrameSizeDataFormat *) data)->
                                ld_obw & 0x3F, CSL_ESYS_INVPARAMS)
                    else
                if (CSL_LDC_BMODE_UNPKD12BIT ==
                    ((CSL_LdcFrameSizeDataFormat *) data)->bayer_format)
                    CSL_EXIT_IF(((CSL_LdcFrameSizeDataFormat *) data)->
                                ld_obw & 0xF, CSL_ESYS_INVPARAMS)}
                    status =
                        _CSL_ldcSetOBW(hndl,
                                       ((CSL_LdcFrameSizeDataFormat *) data)->
                                       ld_obw);

                break;

        case CSL_LDC_CMD_SETINITY:
                CSL_EXIT_IF((*(Uint16 *) data > 16383), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((*(Uint16 *) data & 0x1), CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetInitY(hndl, *(Uint16 *) data);
                break;

        case CSL_LDC_CMD_SETINITX:
                CSL_EXIT_IF((*(Uint16 *) data > 16383), CSL_ESYS_INVPARAMS);
                CSL_EXIT_IF((*(Uint16 *) data & 0x1), CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetInitX(hndl, *(Uint16 *) data);
                break;

        case CSL_LDC_CMD_SETWRITEBASE:
                CSL_EXIT_IF(*(Uint32 *) data & 0x1F, CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetFrameWriteBase(hndl, *(Uint32 *) data);
                break;

        case CSL_LDC_CMD_SETWRITEOFFSET:
                CSL_EXIT_IF(*(Uint16 *) data & 0x1F, CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetWriteOffset(hndl, *(Uint16 *) data);
                break;

        case CSL_LDC_CMD_SETREADBASE420C:
                CSL_EXIT_IF(*(Uint32 *) data & 0x1F, CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetReadBase420C(hndl, *(Uint32 *) data);
                break;

        case CSL_LDC_CMD_SETWRITEBASE420C:
                CSL_EXIT_IF(*(Uint32 *) data & 0x1F, CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetWriteBase420C(hndl, *(Uint32 *) data);
                break;

        case CSL_LDC_CMD_SETTHRESHOLD:
                CSL_EXIT_IF(*(Uint16 *) data > 16383, CSL_ESYS_INVPARAMS);
                status =
                    _CSL_ldcSetBackMappingThreshold(hndl, *(Uint16 *) data);
                break;

        case CSL_LDC_CMD_SETCONSTANTOOUTPUTADDRESSMODE:
                status =
                    _CSL_ldcSetConstantOutputAddressMode(hndl,
                                                         *(Uint16 *) data);
                break;

        case CSL_LDC_CMD_SETYINTERPOLATIONTYPE:
                status =
                    _CSL_ldcSetYInterpolationType(hndl,
                                                  *
                                                  (CSL_LdcYInterpolateMethodType
                                                   *) data);
                break;

        case CSL_LDC_CMD_SETINITIALCOLOR:
                status =
                    _CSL_ldcSetInitialColor(hndl,
                                            *(CSL_LdcInitColorForLDBackMapType
                                              *) data);
                break;

        case CSL_LDC_CMD_SETRIGHTSHIFTBITS:
                CSL_EXIT_IF(*(Bits4 *) data > 15, CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetRightShiftBits(hndl, *(Bits4 *) data);
                break;

        case CSL_LDC_CMD_SETLENSCENTERY:
                CSL_EXIT_IF(*(Bits14 *) data > 16383, CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetLensCenterY(hndl, *(Bits14 *) data);
                break;

        case CSL_LDC_CMD_SETLENSCENTERX:
                CSL_EXIT_IF(*(Bits14 *) data > 16383, CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetLensCenterX(hndl, *(Bits14 *) data);
                break;

        case CSL_LDC_CMD_SETKVL:
                status = _CSL_ldcSetScalingFactorKvl(hndl, *(Uint8 *) data);
                break;

        case CSL_LDC_CMD_SETKVU:
                status = _CSL_ldcSetScalingFactorKvu(hndl, *(Uint8 *) data);
                break;

        case CSL_LDC_CMD_SETKHR:
                status = _CSL_ldcSetScalingFactorKhr(hndl, *(Uint8 *) data);
                break;

        case CSL_LDC_CMD_SETKHL:
                status = _CSL_ldcSetScalingFactorKhl(hndl, *(Uint8 *) data);
                break;

        case CSL_LDC_CMD_SETPIXPAD:
                CSL_EXIT_IF(*(Bits4 *) data < 2 ||
                            *(Bits4 *) data > 15, CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetPixPad(hndl, *(Bits4 *) data);
                break;

        case CSL_LDC_CMD_SETLUTADDRESS:
                status = _CSL_ldcSetLUTAddress(hndl, *(Uint8 *) data);
                break;

        case CSL_LDC_CMD_SETLUTWRITEDATA:
                status = _CSL_ldcSetLUTWriteData(hndl, *(Uint16 *) data);
                break;

        case CSL_LDC_CMD_SETAFFINEA:
                CSL_EXIT_IF(*(Bits14 *) data > 16383, CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetAffineCoeffA(hndl, *(Bits14 *) data);
                break;

        case CSL_LDC_CMD_SETAFFINEB:
                CSL_EXIT_IF(*(Bits14 *) data > 16383, CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetAffineCoeffB(hndl, *(Bits14 *) data);
                break;

        case CSL_LDC_CMD_SETAFFINEC:
                status = _CSL_ldcSetAffineCoeffC(hndl, *(Uint16 *) data);
                break;

        case CSL_LDC_CMD_SETAFFINED:
                CSL_EXIT_IF(*(Bits14 *) data > 16383, CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetAffineCoeffD(hndl, *(Bits14 *) data);
                break;

        case CSL_LDC_CMD_SETAFFINEE:
                CSL_EXIT_IF(*(Bits14 *) data > 16383, CSL_ESYS_INVPARAMS);
                status = _CSL_ldcSetAffineCoeffE(hndl, *(Bits14 *) data);
                break;

        case CSL_LDC_CMD_SETAFFINEF:
                status = _CSL_ldcSetAffineCoeffF(hndl, *(Uint16 *) data);
                break;

        case CSL_LDC_CMD_LUT:
                status = _CSL_ldcSetLUT(hndl, (Uint16 *) data);
                break;
            }
          EXIT:
            return status;
    }

    /* ========================================================================== 
     */
/**
* CSL_ldcGetHwStatus() Gets the H/w Status for LDC module requested by the application
* @param  hndl  Handle to the LDC object
* @param  cmd  CMD used to select a particular field name/register
* @param  data  pointer which Points to the data/value  being queried
* @return  status  CSL_SOK Succesfull Query
*                         Other value = Query failed and the error code is returned.
*/
    /* ========================================================================== 
     */

    CSL_Status CSL_ldcGetHwStatus(CSL_LdcHandle hndl,
                                  CSL_LdcHwQueryType * query, void *data) {
        CSL_Status status = CSL_SOK;

        switch (*query)
        {
            case CSL_LDC_QUERY_REV_ID:
                status = _CSL_ldcGetRevisionID(hndl, (Uint8 *) data);
                break;

            case CSL_LDC_QUERY_CLASS_ID:
                status = _CSL_ldcGetClassID(hndl, (Uint8 *) data);
                break;

            case CSL_LDC_QUERY_PERIPHERAL_ID:
                status = _CSL_ldcGetPeripheralID(hndl, (Uint8 *) data);
                break;

            case CSL_LDC_QUERY_ENABLE:
                status = _CSL_ldcIsEnabled(hndl, (CSL_LdcHwEnableType *) data);
                break;

            case CSL_LDC_QUERY_ENABLEMAP:
                status =
                    _CSL_ldcmapIsEnabled(hndl, (CSL_LdcHwEnableType *) data);
                break;

            case CSL_LDC_QUERY_BUSY:
                status = _CSL_ldcIsBusy(hndl, (CSL_LdcHwStatusType *) data);
                break;

            case CSL_LDC_QUERY_MODE:
                status = _CSL_ldcGetMode(hndl, (CSL_LdcModeType *) data);
                break;

            case CSL_LDC_QUERY_BAYERMODE:
                status =
                    _CSL_ldcGetBayerMode(hndl, (CSL_LdcBayerModeType *) data);
                break;

            case CSL_LDC_QUERY_READBASE:
                status = _CSL_ldcGetFrameReadBase(hndl, (Uint32 *) data);
                break;

            case CSL_LDC_QUERY_READOFFSET:
                status = _CSL_ldcGetReadOffset(hndl, (Uint16 *) data);
                break;

            case CSL_LDC_QUERY_FRAMEHEIGHT:
                status = _CSL_ldcGetFrameHeight(hndl, (Uint16 *) data);
                break;

            case CSL_LDC_QUERY_FRAMEWIDTH:
                status = _CSL_ldcGetFrameWidth(hndl, (Uint16 *) data);
                break;

            case CSL_LDC_QUERY_INITY:
                status = _CSL_ldcGetInitY(hndl, (Uint16 *) data);
                break;

            case CSL_LDC_QUERY_INITX:
                status = _CSL_ldcGetInitX(hndl, (Uint16 *) data);
                break;

            case CSL_LDC_QUERY_WRITEBASE:
                status = _CSL_ldcGetFrameWriteBase(hndl, (Uint32 *) data);
                break;

            case CSL_LDC_QUERY_WRITEOFFSET:
                status = _CSL_ldcGetWriteOffset(hndl, (Uint16 *) data);
                break;

            case CSL_LDC_QUERY_READBASE420C:
                status = _CSL_ldcGetReadBase420C(hndl, (Uint32 *) data);
                break;

            case CSL_LDC_QUERY_WRITEBASE420C:
                status = _CSL_ldcGetWriteBase420C(hndl, (Uint32 *) data);
                break;

            case CSL_LDC_QUERY_THRESHOLD:
                status = _CSL_ldcGetBackMappingThreshold(hndl, (Uint16 *) data);
                break;

            case CSL_LDC_QUERY_CONSTANTOOUTPUTADDRESSMODE:
                status =
                    _CSL_ldcGetConstantOutputAddressMode(hndl, (Uint16 *) data);
                break;

            case CSL_LDC_QUERY_YINTERPOLATIONTYPE:
                status =
                    _CSL_ldcGetYInterpolationType(hndl,
                                                  (CSL_LdcYInterpolateMethodType
                                                   *) data);
                break;

            case CSL_LDC_QUERY_INITIALCOLOR:
                status =
                    _CSL_ldcGetInitialColor(hndl,
                                            (CSL_LdcInitColorForLDBackMapType *)
                                            data);
                break;

            case CSL_LDC_QUERY_RIGHTSHIFTBITS:
                status = _CSL_ldcGetRightShiftBits(hndl, (Bits4 *) data);
                break;

            case CSL_LDC_QUERY_LENSCENTERY:
                status = _CSL_ldcGetLensCenterY(hndl, (Bits14 *) data);
                break;

            case CSL_LDC_QUERY_LENSCENTERX:
                status = _CSL_ldcGetLensCenterX(hndl, (Bits14 *) data);
                break;

            case CSL_LDC_QUERY_KVL:
                status = _CSL_ldcGetScalingFactorKvl(hndl, (Uint8 *) data);
                break;

            case CSL_LDC_QUERY_KVU:
                status = _CSL_ldcGetScalingFactorKvu(hndl, (Uint8 *) data);
                break;

            case CSL_LDC_QUERY_KHR:
                status = _CSL_ldcGetScalingFactorKhr(hndl, (Uint8 *) data);
                break;

            case CSL_LDC_QUERY_KHL:
                status = _CSL_ldcGetScalingFactorKhl(hndl, (Uint8 *) data);
                break;

            case CSL_LDC_QUERY_PIXPAD:
                status = _CSL_ldcGetPixPad(hndl, (Bits4 *) data);
                break;

            case CSL_LDC_QUERY_OBH:
                status = _CSL_ldcGetOBH(hndl, (Uint8 *) data);
                break;

            case CSL_LDC_QUERY_OBW:
                status = _CSL_ldcGetOBW(hndl, (Uint8 *) data);
                break;

            case CSL_LDC_QUERY_LUTADDRESS:
                status = _CSL_ldcGetLUTAddress(hndl, (Uint8 *) data);
                break;

            case CSL_LDC_QUERY_LUTREADDATA:
                status = _CSL_ldcGetLUTReadData(hndl, (Bits14 *) data);
                break;

            case CSL_LDC_QUERY_AFFINEA:
                status = _CSL_ldcGetAffineCoeffA(hndl, (Bits14 *) data);
                break;

            case CSL_LDC_QUERY_AFFINEB:
                status = _CSL_ldcGetAffineCoeffB(hndl, (Bits14 *) data);
                break;

            case CSL_LDC_QUERY_AFFINEC:
                status = _CSL_ldcGetAffineCoeffC(hndl, (Uint16 *) data);
                break;

            case CSL_LDC_QUERY_AFFINED:
                status = _CSL_ldcGetAffineCoeffD(hndl, (Bits14 *) data);
                break;

            case CSL_LDC_QUERY_AFFINEE:
                status = _CSL_ldcGetAffineCoeffE(hndl, (Bits14 *) data);
                break;

            case CSL_LDC_QUERY_AFFINEF:
                status = _CSL_ldcGetAffineCoeffF(hndl, (Uint16 *) data);
                break;
            case CSL_LDC_QUERY_LUT:
                status = _CSL_ldcGetLUT(hndl, (Uint16 *) data);
                break;
        }
        return status;
    }

    /* ========================================================================== 
     */
/**
* CSL_ldcClose() The close call deallocates resources for LDC  for the particular instance of the LDC for which the handle is passed 
* @param  hndl  Handle to the LDC object
* @return  status  CSL_SOK Succesfull Close
*                         Other value = Close failed and the error code is returned.
*/
    /* ========================================================================== 
     */

    CSL_Status CSL_ldcClose(CSL_LdcHandle hndl) {
        /* Indicate in the CSL global data structure that the peripheral has
         * been unreserved */
        // return (_CSL_certifyClose((CSL_ResHandle)hndl));
        return CSL_SOK;
    }

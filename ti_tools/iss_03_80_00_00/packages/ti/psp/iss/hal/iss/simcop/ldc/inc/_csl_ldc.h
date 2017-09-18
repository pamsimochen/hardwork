/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file _csl_ldc.h
*
* This File contains declarations for _csl_ldc.c
* This entire description will appear as one 
* paragraph in the generated documentation.
*
* @path  $(CSLPATH)\inc\LDC\
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

#ifndef _CSL_LDC_H
#define _CSL_LDC_H

#ifdef __cplusplus

extern "C" {
#endif
/****************************************************************
*  INCLUDE FILES                                                 
****************************************************************/
/*-------program files ----------------------------------------*/
#include <ti/psp/iss/hal/iss/simcop/common/csl.h>
#include "csl_ldc.h"
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/
    CSL_Status _CSL_ldcGetPeripheralID(CSL_LdcHandle hndl, Uint8 * data);
    CSL_Status _CSL_ldcGetClassID(CSL_LdcHandle hndl, Uint8 * data);
    CSL_Status _CSL_ldcGetRevisionID(CSL_LdcHandle hndl, Uint8 * data);
    CSL_Status _CSL_ldcEnable(CSL_LdcHandle hndl, CSL_LdcHwEnableType data);
    CSL_Status _CSL_ldcIsEnabled(CSL_LdcHandle hndl,
                                 CSL_LdcHwEnableType * data);
    CSL_Status _CSL_ldcmapEnable(CSL_LdcHandle hndl, CSL_LdcHwEnableType data);
    CSL_Status _CSL_ldcmapIsEnabled(CSL_LdcHandle hndl,
                                    CSL_LdcHwEnableType * data);
    CSL_Status _CSL_ldcIsBusy(CSL_LdcHandle hndl, CSL_LdcHwStatusType * data);
    CSL_Status _CSL_ldcSetMode(CSL_LdcHandle hndl, CSL_LdcModeType data);
    CSL_Status _CSL_ldcGetMode(CSL_LdcHandle hndl, CSL_LdcModeType * data);
    CSL_Status _CSL_ldcSetBayerMode(CSL_LdcHandle hndl,
                                    CSL_LdcBayerModeType data);
    CSL_Status _CSL_ldcGetBayerMode(CSL_LdcHandle hndl,
                                    CSL_LdcBayerModeType * data);
    CSL_Status _CSL_ldcSetFrameReadBase(CSL_LdcHandle hndl, Uint32 data);
    CSL_Status _CSL_ldcGetFrameReadBase(CSL_LdcHandle hndl, Uint32 * data);
    CSL_Status _CSL_ldcSetReadOffset(CSL_LdcHandle hndl, Uint16 data);
    CSL_Status _CSL_ldcGetReadOffset(CSL_LdcHandle hndl, Uint16 * data);
    CSL_Status _CSL_ldcSetFrameHeight(CSL_LdcHandle hndl, Uint16 data);
    CSL_Status _CSL_ldcGetFrameHeight(CSL_LdcHandle hndl, Uint16 * data);
    CSL_Status _CSL_ldcSetFrameWidth(CSL_LdcHandle hndl, Uint16 data);
    CSL_Status _CSL_ldcGetFrameWidth(CSL_LdcHandle hndl, Uint16 * data);
    CSL_Status _CSL_ldcSetInitY(CSL_LdcHandle hndl, Uint16 data);
    CSL_Status _CSL_ldcGetInitY(CSL_LdcHandle hndl, Uint16 * data);
    CSL_Status _CSL_ldcSetInitX(CSL_LdcHandle hndl, Uint16 data);
    CSL_Status _CSL_ldcGetInitX(CSL_LdcHandle hndl, Uint16 * data);
    CSL_Status _CSL_ldcSetFrameWriteBase(CSL_LdcHandle hndl, Uint32 data);
    CSL_Status _CSL_ldcGetFrameWriteBase(CSL_LdcHandle hndl, Uint32 * data);
    CSL_Status _CSL_ldcSetWriteOffset(CSL_LdcHandle hndl, Uint16 data);
    CSL_Status _CSL_ldcGetWriteOffset(CSL_LdcHandle hndl, Uint16 * data);
    CSL_Status _CSL_ldcSetReadBase420C(CSL_LdcHandle hndl, Uint32 data);
    CSL_Status _CSL_ldcGetReadBase420C(CSL_LdcHandle hndl, Uint32 * data);
    CSL_Status _CSL_ldcSetWriteBase420C(CSL_LdcHandle hndl, Uint32 data);
    CSL_Status _CSL_ldcGetWriteBase420C(CSL_LdcHandle hndl, Uint32 * data);
    CSL_Status _CSL_ldcSetBackMappingThreshold(CSL_LdcHandle hndl, Uint16 data);
    CSL_Status _CSL_ldcGetBackMappingThreshold(CSL_LdcHandle hndl,
                                               Uint16 * data);
    CSL_Status _CSL_ldcSetConstantOutputAddressMode(CSL_LdcHandle hndl,
                                                    Uint16 data);
    CSL_Status _CSL_ldcGetConstantOutputAddressMode(CSL_LdcHandle hndl,
                                                    Uint16 * data);
    CSL_Status _CSL_ldcSetYInterpolationType(CSL_LdcHandle hndl,
                                             CSL_LdcYInterpolateMethodType
                                             data);
    CSL_Status _CSL_ldcGetYInterpolationType(CSL_LdcHandle hndl,
                                             CSL_LdcYInterpolateMethodType *
                                             data);
    CSL_Status _CSL_ldcSetInitialColor(CSL_LdcHandle hndl,
                                       CSL_LdcInitColorForLDBackMapType data);
    CSL_Status _CSL_ldcGetInitialColor(CSL_LdcHandle hndl,
                                       CSL_LdcInitColorForLDBackMapType * data);
    CSL_Status _CSL_ldcSetRightShiftBits(CSL_LdcHandle hndl, Bits4 data);
    CSL_Status _CSL_ldcGetRightShiftBits(CSL_LdcHandle hndl, Bits4 * data);
    CSL_Status _CSL_ldcSetLensCenterY(CSL_LdcHandle hndl, Bits14 data);
    CSL_Status _CSL_ldcGetLensCenterY(CSL_LdcHandle hndl, Bits14 * data);
    CSL_Status _CSL_ldcSetLensCenterX(CSL_LdcHandle hndl, Bits14 data);
    CSL_Status _CSL_ldcGetLensCenterX(CSL_LdcHandle hndl, Bits14 * data);
    CSL_Status _CSL_ldcSetScalingFactorKvl(CSL_LdcHandle hndl, Uint8 data);
    CSL_Status _CSL_ldcGetScalingFactorKvl(CSL_LdcHandle hndl, Uint8 * data);
    CSL_Status _CSL_ldcSetScalingFactorKvu(CSL_LdcHandle hndl, Uint8 data);
    CSL_Status _CSL_ldcGetScalingFactorKvu(CSL_LdcHandle hndl, Uint8 * data);
    CSL_Status _CSL_ldcSetScalingFactorKhr(CSL_LdcHandle hndl, Uint8 data);
    CSL_Status _CSL_ldcGetScalingFactorKhr(CSL_LdcHandle hndl, Uint8 * data);
    CSL_Status _CSL_ldcSetScalingFactorKhl(CSL_LdcHandle hndl, Uint8 data);
    CSL_Status _CSL_ldcGetScalingFactorKhl(CSL_LdcHandle hndl, Uint8 * data);
    CSL_Status _CSL_ldcSetPixPad(CSL_LdcHandle hndl, Bits4 data);
    CSL_Status _CSL_ldcGetPixPad(CSL_LdcHandle hndl, Bits4 * data);
    CSL_Status _CSL_ldcSetOBH(CSL_LdcHandle hndl, Uint8 data);
    CSL_Status _CSL_ldcGetOBH(CSL_LdcHandle hndl, Uint8 * data);
    CSL_Status _CSL_ldcSetOBW(CSL_LdcHandle hndl, Uint8 data);
    CSL_Status _CSL_ldcGetOBW(CSL_LdcHandle hndl, Uint8 * data);
    CSL_Status _CSL_ldcSetLUTAddress(CSL_LdcHandle hndl, Uint8 data);
    CSL_Status _CSL_ldcGetLUTAddress(CSL_LdcHandle hndl, Uint8 * data);
    CSL_Status _CSL_ldcSetLUTWriteData(CSL_LdcHandle hndl, Uint16 data);
    CSL_Status _CSL_ldcGetLUTReadData(CSL_LdcHandle hndl, Bits14 * data);
    CSL_Status _CSL_ldcSetAffineCoeffA(CSL_LdcHandle hndl, Bits14 data);
    CSL_Status _CSL_ldcGetAffineCoeffA(CSL_LdcHandle hndl, Bits14 * data);
    CSL_Status _CSL_ldcSetAffineCoeffB(CSL_LdcHandle hndl, Bits14 data);
    CSL_Status _CSL_ldcGetAffineCoeffB(CSL_LdcHandle hndl, Bits14 * data);
    CSL_Status _CSL_ldcSetAffineCoeffC(CSL_LdcHandle hndl, Uint16 data);
    CSL_Status _CSL_ldcGetAffineCoeffC(CSL_LdcHandle hndl, Uint16 * data);
    CSL_Status _CSL_ldcSetAffineCoeffD(CSL_LdcHandle hndl, Bits14 data);
    CSL_Status _CSL_ldcGetAffineCoeffD(CSL_LdcHandle hndl, Bits14 * data);
    CSL_Status _CSL_ldcSetAffineCoeffE(CSL_LdcHandle hndl, Bits14 data);
    CSL_Status _CSL_ldcGetAffineCoeffE(CSL_LdcHandle hndl, Bits14 * data);
    CSL_Status _CSL_ldcSetAffineCoeffF(CSL_LdcHandle hndl, Uint16 data);
    CSL_Status _CSL_ldcGetAffineCoeffF(CSL_LdcHandle hndl, Uint16 * data);
    CSL_Status _CSL_ldcSetLUT(CSL_LdcHandle hndl, Uint16 * data);
    CSL_Status _CSL_ldcGetLUT(CSL_LdcHandle hndl, Uint16 * data);

    /* Generic functions */
    void _CSL_ldcGetAttrs(CSL_LdcNum ldcNum, CSL_LdcHandle hLdc);
    CSL_Status _CSL_ldcRegisterReset(CSL_LdcHandle hndl);

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/*--------macros ----------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif                                                     /* _CSL_LDC_H */

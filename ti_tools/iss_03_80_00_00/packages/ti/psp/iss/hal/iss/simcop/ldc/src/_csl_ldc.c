/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file _csl_ldc.c
*
* This File contains CSL Level 0 functions for LDC Module
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
#include "cslr__ldc.h"
#include "csl_ldc.h"

/* ===================================================================
 *  @func     _CSL_ldcGetPeripheralID                                               
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
CSL_Status _CSL_ldcGetPeripheralID(CSL_LdcHandle hndl, Uint8 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->PID, LDC_PID_TID);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetClassID                                               
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
CSL_Status _CSL_ldcGetClassID(CSL_LdcHandle hndl, Uint8 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->PID, LDC_PID_CID);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetRevisionID                                               
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
CSL_Status _CSL_ldcGetRevisionID(CSL_LdcHandle hndl, Uint8 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->PID, LDC_PID_PREV);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcEnable                                               
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
CSL_Status _CSL_ldcEnable(CSL_LdcHandle hndl, CSL_LdcHwEnableType data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->PCR, LDC_PCR_EN, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcIsEnabled                                               
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
CSL_Status _CSL_ldcIsEnabled(CSL_LdcHandle hndl, CSL_LdcHwEnableType * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = (CSL_LdcHwEnableType) (CSL_FEXT(ldcRegs->PCR, LDC_PCR_EN));
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcmapEnable                                               
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
CSL_Status _CSL_ldcmapEnable(CSL_LdcHandle hndl, CSL_LdcHwEnableType data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->PCR, LDC_PCR_LDMAPEN, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcmapIsEnabled                                               
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
CSL_Status _CSL_ldcmapIsEnabled(CSL_LdcHandle hndl, CSL_LdcHwEnableType * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = (CSL_LdcHwEnableType) (CSL_FEXT(ldcRegs->PCR, LDC_PCR_LDMAPEN));
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcIsBusy                                               
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
CSL_Status _CSL_ldcIsBusy(CSL_LdcHandle hndl, CSL_LdcHwStatusType * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = (CSL_LdcHwStatusType) (CSL_FEXT(ldcRegs->PCR, LDC_PCR_BUSY));
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetMode                                               
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
CSL_Status _CSL_ldcSetMode(CSL_LdcHandle hndl, CSL_LdcModeType data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->PCR, LDC_PCR_MODE, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetMode                                               
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
CSL_Status _CSL_ldcGetMode(CSL_LdcHandle hndl, CSL_LdcModeType * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = (CSL_LdcModeType) (CSL_FEXT(ldcRegs->PCR, LDC_PCR_MODE));
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetBayerMode                                               
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
CSL_Status _CSL_ldcSetBayerMode(CSL_LdcHandle hndl, CSL_LdcBayerModeType data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->PCR, LDC_PCR_BMODE, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetBayerMode                                               
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
CSL_Status _CSL_ldcGetBayerMode(CSL_LdcHandle hndl, CSL_LdcBayerModeType * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = (CSL_LdcBayerModeType) (CSL_FEXT(ldcRegs->PCR, LDC_PCR_BMODE));
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetFrameReadBase                                               
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
CSL_Status _CSL_ldcSetFrameReadBase(CSL_LdcHandle hndl, Uint32 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    ldcRegs->RD_BASE = data;
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetFrameReadBase                                               
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
CSL_Status _CSL_ldcGetFrameReadBase(CSL_LdcHandle hndl, Uint32 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = ldcRegs->RD_BASE;
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetReadOffset                                               
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
CSL_Status _CSL_ldcSetReadOffset(CSL_LdcHandle hndl, Uint16 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    ldcRegs->RD_OFST = CSL_FMK(LDC_RD_OFST_ROFST, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetReadOffset                                               
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
CSL_Status _CSL_ldcGetReadOffset(CSL_LdcHandle hndl, Uint16 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->RD_OFST, LDC_RD_OFST_ROFST);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetFrameHeight                                               
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
CSL_Status _CSL_ldcSetFrameHeight(CSL_LdcHandle hndl, Uint16 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->FRAME_SIZE, LDC_FRAME_SIZE_H, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetFrameHeight                                               
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
CSL_Status _CSL_ldcGetFrameHeight(CSL_LdcHandle hndl, Uint16 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->FRAME_SIZE, LDC_FRAME_SIZE_H);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetFrameWidth                                               
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
CSL_Status _CSL_ldcSetFrameWidth(CSL_LdcHandle hndl, Uint16 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->FRAME_SIZE, LDC_FRAME_SIZE_W, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetFrameWidth                                               
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
CSL_Status _CSL_ldcGetFrameWidth(CSL_LdcHandle hndl, Uint16 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->FRAME_SIZE, LDC_FRAME_SIZE_W);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetInitY                                               
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
CSL_Status _CSL_ldcSetInitY(CSL_LdcHandle hndl, Uint16 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->INITXY, LDC_INITXY_INITY, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetInitY                                               
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
CSL_Status _CSL_ldcGetInitY(CSL_LdcHandle hndl, Uint16 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->INITXY, LDC_INITXY_INITY);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetInitX                                               
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
CSL_Status _CSL_ldcSetInitX(CSL_LdcHandle hndl, Uint16 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->INITXY, LDC_INITXY_INITX, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetInitX                                               
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
CSL_Status _CSL_ldcGetInitX(CSL_LdcHandle hndl, Uint16 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->INITXY, LDC_INITXY_INITX);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetFrameWriteBase                                               
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
CSL_Status _CSL_ldcSetFrameWriteBase(CSL_LdcHandle hndl, Uint32 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    ldcRegs->WR_BASE = data;
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetFrameWriteBase                                               
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
CSL_Status _CSL_ldcGetFrameWriteBase(CSL_LdcHandle hndl, Uint32 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = ldcRegs->WR_BASE;
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetWriteOffset                                               
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
CSL_Status _CSL_ldcSetWriteOffset(CSL_LdcHandle hndl, Uint16 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    ldcRegs->WR_OFST = CSL_FMK(LDC_WR_OFST_WOFST, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetWriteOffset                                               
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
CSL_Status _CSL_ldcGetWriteOffset(CSL_LdcHandle hndl, Uint16 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->WR_OFST, LDC_WR_OFST_WOFST);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetReadBase420C                                               
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
CSL_Status _CSL_ldcSetReadBase420C(CSL_LdcHandle hndl, Uint32 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    ldcRegs->RD_BASE_420C = data;
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetReadBase420C                                               
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
CSL_Status _CSL_ldcGetReadBase420C(CSL_LdcHandle hndl, Uint32 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = ldcRegs->RD_BASE_420C;
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetWriteBase420C                                               
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
CSL_Status _CSL_ldcSetWriteBase420C(CSL_LdcHandle hndl, Uint32 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    ldcRegs->WR_BASE_420C = data;
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetWriteBase420C                                               
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
CSL_Status _CSL_ldcGetWriteBase420C(CSL_LdcHandle hndl, Uint32 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = ldcRegs->WR_BASE_420C;
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetBackMappingThreshold                                               
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
CSL_Status _CSL_ldcSetBackMappingThreshold(CSL_LdcHandle hndl, Uint16 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->CONFIG, LDC_CONFIG_RTH, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetBackMappingThreshold                                               
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
CSL_Status _CSL_ldcGetBackMappingThreshold(CSL_LdcHandle hndl, Uint16 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->CONFIG, LDC_CONFIG_RTH);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetConstantOutputAddressMode                                               
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
CSL_Status _CSL_ldcSetConstantOutputAddressMode(CSL_LdcHandle hndl, Uint16 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->CONFIG, LDC_CONFIG_CNST_MD, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetConstantOutputAddressMode                                               
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
CSL_Status _CSL_ldcGetConstantOutputAddressMode(CSL_LdcHandle hndl,
                                                Uint16 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->CONFIG, LDC_CONFIG_CNST_MD);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetYInterpolationType                                               
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
CSL_Status _CSL_ldcSetYInterpolationType(CSL_LdcHandle hndl,
                                         CSL_LdcYInterpolateMethodType data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->CONFIG, LDC_CONFIG_YINT_TYP, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetYInterpolationType                                               
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
CSL_Status _CSL_ldcGetYInterpolationType(CSL_LdcHandle hndl,
                                         CSL_LdcYInterpolateMethodType * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data =
        (CSL_LdcYInterpolateMethodType) (CSL_FEXT
                                         (ldcRegs->CONFIG,
                                          LDC_CONFIG_YINT_TYP));
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetInitialColor                                               
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
CSL_Status _CSL_ldcSetInitialColor(CSL_LdcHandle hndl,
                                   CSL_LdcInitColorForLDBackMapType data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->CONFIG, LDC_CONFIG_INTC, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetInitialColor                                               
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
CSL_Status _CSL_ldcGetInitialColor(CSL_LdcHandle hndl,
                                   CSL_LdcInitColorForLDBackMapType * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data =
        (CSL_LdcInitColorForLDBackMapType) (CSL_FEXT
                                            (ldcRegs->CONFIG, LDC_CONFIG_INTC));
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetRightShiftBits                                               
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
CSL_Status _CSL_ldcSetRightShiftBits(CSL_LdcHandle hndl, Bits4 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->CONFIG, LDC_CONFIG_T, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetRightShiftBits                                               
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
CSL_Status _CSL_ldcGetRightShiftBits(CSL_LdcHandle hndl, Bits4 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = (Bits4) (CSL_FEXT(ldcRegs->CONFIG, LDC_CONFIG_T));
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetLensCenterY                                               
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
CSL_Status _CSL_ldcSetLensCenterY(CSL_LdcHandle hndl, Bits14 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->CENTER, LDC_CENTER_V0, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetLensCenterY                                               
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
CSL_Status _CSL_ldcGetLensCenterY(CSL_LdcHandle hndl, Bits14 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = (Bits14) (CSL_FEXT(ldcRegs->CENTER, LDC_CENTER_V0));
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetLensCenterX                                               
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
CSL_Status _CSL_ldcSetLensCenterX(CSL_LdcHandle hndl, Bits14 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->CENTER, LDC_CENTER_H0, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetLensCenterX                                               
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
CSL_Status _CSL_ldcGetLensCenterX(CSL_LdcHandle hndl, Bits14 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = (Bits14) (CSL_FEXT(ldcRegs->CENTER, LDC_CENTER_H0));
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetScalingFactorKvl                                               
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
CSL_Status _CSL_ldcSetScalingFactorKvl(CSL_LdcHandle hndl, Uint8 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->KHV, LDC_KHV_KVL, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetScalingFactorKvl                                               
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
CSL_Status _CSL_ldcGetScalingFactorKvl(CSL_LdcHandle hndl, Uint8 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->KHV, LDC_KHV_KVL);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetScalingFactorKvu                                               
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
CSL_Status _CSL_ldcSetScalingFactorKvu(CSL_LdcHandle hndl, Uint8 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->KHV, LDC_KHV_KVU, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetScalingFactorKvu                                               
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
CSL_Status _CSL_ldcGetScalingFactorKvu(CSL_LdcHandle hndl, Uint8 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->KHV, LDC_KHV_KVU);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetScalingFactorKhr                                               
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
CSL_Status _CSL_ldcSetScalingFactorKhr(CSL_LdcHandle hndl, Uint8 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->KHV, LDC_KHV_KHR, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetScalingFactorKhr                                               
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
CSL_Status _CSL_ldcGetScalingFactorKhr(CSL_LdcHandle hndl, Uint8 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->KHV, LDC_KHV_KHR);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetScalingFactorKhl                                               
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
CSL_Status _CSL_ldcSetScalingFactorKhl(CSL_LdcHandle hndl, Uint8 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->KHV, LDC_KHV_KHL, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetScalingFactorKhl                                               
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
CSL_Status _CSL_ldcGetScalingFactorKhl(CSL_LdcHandle hndl, Uint8 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->KHV, LDC_KHV_KHL);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetPixPad                                               
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
CSL_Status _CSL_ldcSetPixPad(CSL_LdcHandle hndl, Bits4 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->BLOCK, LDC_BLOCK_PIXPAD, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetPixPad                                               
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
CSL_Status _CSL_ldcGetPixPad(CSL_LdcHandle hndl, Bits4 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = (Bits4) (CSL_FEXT(ldcRegs->BLOCK, LDC_BLOCK_PIXPAD));
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetOBH                                               
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
CSL_Status _CSL_ldcSetOBH(CSL_LdcHandle hndl, Uint8 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->BLOCK, LDC_BLOCK_OBH, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetOBH                                               
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
CSL_Status _CSL_ldcGetOBH(CSL_LdcHandle hndl, Uint8 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->BLOCK, LDC_BLOCK_OBH);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetOBW                                               
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
CSL_Status _CSL_ldcSetOBW(CSL_LdcHandle hndl, Uint8 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->BLOCK, LDC_BLOCK_OBW, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetOBW                                               
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
CSL_Status _CSL_ldcGetOBW(CSL_LdcHandle hndl, Uint8 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->BLOCK, LDC_BLOCK_OBW);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetLUTAddress                                               
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
CSL_Status _CSL_ldcSetLUTAddress(CSL_LdcHandle hndl, Uint8 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->LUT_ADDR, LDC_LUT_ADDR_ADDR, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetLUTAddress                                               
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
CSL_Status _CSL_ldcGetLUTAddress(CSL_LdcHandle hndl, Uint8 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->LUT_ADDR, LDC_LUT_ADDR_ADDR);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetLUTWriteData                                               
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
CSL_Status _CSL_ldcSetLUTWriteData(CSL_LdcHandle hndl, Uint16 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->LUT_WDATA, LDC_LUT_WDATA_WDATA, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetLUTReadData                                               
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
CSL_Status _CSL_ldcGetLUTReadData(CSL_LdcHandle hndl, Bits14 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->LUT_RDATA, LDC_LUT_RDATA_RDATA);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetAffineCoeffA                                               
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
CSL_Status _CSL_ldcSetAffineCoeffA(CSL_LdcHandle hndl, Bits14 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->AB, LDC_AB_A, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetAffineCoeffA                                               
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
CSL_Status _CSL_ldcGetAffineCoeffA(CSL_LdcHandle hndl, Bits14 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->AB, LDC_AB_A);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetAffineCoeffB                                               
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
CSL_Status _CSL_ldcSetAffineCoeffB(CSL_LdcHandle hndl, Bits14 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->AB, LDC_AB_B, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetAffineCoeffB                                               
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
CSL_Status _CSL_ldcGetAffineCoeffB(CSL_LdcHandle hndl, Bits14 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->AB, LDC_AB_B);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetAffineCoeffC                                               
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
CSL_Status _CSL_ldcSetAffineCoeffC(CSL_LdcHandle hndl, Uint16 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->CD, LDC_CD_C, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetAffineCoeffC                                               
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
CSL_Status _CSL_ldcGetAffineCoeffC(CSL_LdcHandle hndl, Uint16 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->CD, LDC_CD_C);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetAffineCoeffD                                               
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
CSL_Status _CSL_ldcSetAffineCoeffD(CSL_LdcHandle hndl, Bits14 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->CD, LDC_CD_D, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetAffineCoeffD                                               
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
CSL_Status _CSL_ldcGetAffineCoeffD(CSL_LdcHandle hndl, Bits14 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->CD, LDC_CD_D);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetAffineCoeffE                                               
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
CSL_Status _CSL_ldcSetAffineCoeffE(CSL_LdcHandle hndl, Bits14 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->EF, LDC_EF_E, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetAffineCoeffE                                               
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
CSL_Status _CSL_ldcGetAffineCoeffE(CSL_LdcHandle hndl, Bits14 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->EF, LDC_EF_E);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetAffineCoeffF                                               
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
CSL_Status _CSL_ldcSetAffineCoeffF(CSL_LdcHandle hndl, Uint16 data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    CSL_FINS(ldcRegs->EF, LDC_EF_F, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetAffineCoeffF                                               
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
CSL_Status _CSL_ldcGetAffineCoeffF(CSL_LdcHandle hndl, Uint16 * data)
{
    CSL_LdcRegsOvly ldcRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(ldcRegs->EF, LDC_EF_F);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcSetLUT                                               
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
CSL_Status _CSL_ldcSetLUT(CSL_LdcHandle hndl, Uint16 * data)
{
    CSL_LdcLutOvly ldclut;

    Uint16 offset;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldclut = hndl->lut;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    for (offset = 0; offset < CSL_LDC_LUT_LEN / 2; offset++)
        ldclut->LUT[offset] = data[offset];
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetLUT                                               
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
CSL_Status _CSL_ldcGetLUT(CSL_LdcHandle hndl, Uint16 * data)
{
    CSL_LdcLutOvly ldclut;

    Uint16 offset;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    ldclut = hndl->lut;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    for (offset = 0; offset < CSL_LDC_LUT_LEN / 2; offset++)
        data[offset] = ldclut->LUT[offset];
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_ldcGetBaseAddr                                               
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
CSL_LdcRegsOvly _CSL_ldcGetBaseAddr(CSL_LdcNum ldcNum)
{
    switch (ldcNum)
    {
        case CSL_LDC_0:
            /* get the LDC unique identifier & resource allocation mask into
             * the handle */
            return CSL_LDC_0_REGS;
    }
    return NULL;
}

/* ===================================================================
 *  @func     _CSL_ldcGetLutAddr                                               
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
CSL_LdcLutOvly _CSL_ldcGetLutAddr(CSL_LdcNum eLdcNum)
{
    CSL_LdcLutOvly pLdcLut = NULL;

    switch (eLdcNum)
    {
        case CSL_LDC_0:
            /* get the LDC unique identifier & resource * allocation mask
             * into the handle */
            pLdcLut = CSL_LDC_LUT;
            break;
    }
    return pLdcLut;
}

/* ===================================================================
 *  @func     _CSL_ldcGetAttrs                                               
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
void _CSL_ldcGetAttrs(CSL_LdcNum ldcNum, CSL_LdcHandle hLdc)
{
    /* get the LDC base address into the handle */
    hLdc->regs = (CSL_LdcRegsOvly) _CSL_ldcGetBaseAddr(ldcNum);
    hLdc->lut = (CSL_LdcLutOvly) _CSL_ldcGetLutAddr(ldcNum);

    switch (ldcNum)
    {
        case CSL_LDC_0:
            /* get the LDC unique identifier & resource allocation mask into
             * the handle */
            hLdc->xio = CSL_LDC_0_XIO;
            hLdc->uid = CSL_LDC_0_UID;
            break;
    }

    /* get the LDC instance number into the handle */
    hLdc->perNum = ldcNum;

}

/* ===================================================================
 *  @func     _CSL_ldcRegisterReset                                               
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
CSL_Status _CSL_ldcRegisterReset(CSL_LdcHandle hndl)
{
    CSL_Status status = CSL_SOK;

    CSL_LdcRegsOvly ldcRegs;

    CSL_EXIT_IF(hndl == NULL, CSL_ESYS_BADHANDLE);
    ldcRegs = hndl->regs;
    ldcRegs->PID = CSL_LDC_PID_RESETVAL;
    ldcRegs->PCR = CSL_LDC_PCR_RESETVAL;
    ldcRegs->RD_BASE = CSL_LDC_RD_BASE_RESETVAL;
    ldcRegs->RD_OFST = CSL_LDC_RD_OFST_RESETVAL;
    ldcRegs->FRAME_SIZE = CSL_LDC_FRAME_SIZE_RESETVAL;
    ldcRegs->INITXY = CSL_LDC_INITXY_RESETVAL;
    ldcRegs->WR_BASE = CSL_LDC_WR_BASE_RESETVAL;
    ldcRegs->WR_OFST = CSL_LDC_WR_OFST_RESETVAL;
    ldcRegs->RD_BASE_420C = CSL_LDC_RD_BASE_420C_RESETVAL;
    ldcRegs->WR_BASE_420C = CSL_LDC_WR_BASE_420C_RESETVAL;
    ldcRegs->CONFIG = CSL_LDC_CONFIG_RESETVAL;
    ldcRegs->CENTER = CSL_LDC_CENTER_RESETVAL;
    ldcRegs->KHV = CSL_LDC_KHV_RESETVAL;
    ldcRegs->BLOCK = CSL_LDC_BLOCK_RESETVAL;
    ldcRegs->LUT_ADDR = CSL_LDC_LUT_ADDR_RESETVAL;
    ldcRegs->LUT_WDATA = CSL_LDC_LUT_WDATA_RESETVAL;
    ldcRegs->LUT_RDATA = CSL_LDC_LUT_RDATA_RESETVAL;
    ldcRegs->AB = CSL_LDC_AB_RESETVAL;
    ldcRegs->CD = CSL_LDC_CD_RESETVAL;
    ldcRegs->EF = CSL_LDC_EF_RESETVAL;
  EXIT:
    return status;
}

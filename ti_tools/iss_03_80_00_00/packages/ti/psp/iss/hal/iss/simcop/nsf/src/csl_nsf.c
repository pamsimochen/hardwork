/* ============================================================================== 
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved.  Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * =========================================================================== 
 */
/**
 * @file csl_nsf.c
 *
 * This File contains function definitions for the NSF CSL configurations. 
 *
 * @path  $(DUCATIVOB)\drivers\csl\iss\simcop\nsf\src\
 *
 * @rev  00.01
 */
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 27-Jan - 2009 Sanish Mahdik: Modified the data-types and added few more 
 *! 09-Dec -2008 Sherin Sasidharan:  Created the file.  
 *! 
 *!
 *!  mf: Revisions appear in reverse chronological order;
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */

/****************************************************************
 *  INCLUDE FILES                                                 
 ****************************************************************/
/*-------program files ----------------------------------------*/
#include "../inc/_csl_nsf.h"
#include "../csl_nsf.h"

/* ========================================================================== 
 */
/**
 * @fn CSL_nsf2Init() Initializes the NSF2 settings
 *
 *  @see csl_nsf.h
 */
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     CSL_nf2Init                                               
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
CSL_Status CSL_nf2Init(CSL_Nf2Handle hndl)
{
    CSL_Status status = CSL_SOK;

    CSL_EXIT_IF((hndl == NULL), CSL_ESYS_BADHANDLE);

    status = _CSL_nsf2RegisterReset(hndl);

  EXIT:
    return status;
}

/* ========================================================================== 
 */
/**
 * @fn CSL_nf2Open() :sets up the data structures for the particular instance of the NSF 
 *
 *  @see csl_nsf.h
 */
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     CSL_nf2Open                                               
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
CSL_Status CSL_nf2Open(CSL_Nf2Obj * hNsf2Obj, CSL_NsfNum nsf2Num,
                       CSL_OpenMode openMode)
{
    CSL_NsfNum nsf2Inst;

    CSL_Status status = CSL_SOK;

    hNsf2Obj->openMode = openMode;

    if (nsf2Num >= 0)
    {
        status = _CSL_nsf2GetAttrs(nsf2Num, hNsf2Obj);
        CSL_EXIT_IF(status != CSL_SOK, CSL_ESYS_OVFL);
    }

    else
    {
        for (nsf2Inst = (CSL_NsfNum) 0; nsf2Inst < (CSL_NsfNum) CSL_NSF_PER_CNT;
             ++nsf2Inst)
        {
            status = _CSL_nsf2GetAttrs(nsf2Num, hNsf2Obj);

            if (status == CSL_SOK)
            {
                break;
            }
        }

    }
  EXIT:
    return status;
}

/* ========================================================================== 
 */
/**
 * @fn CSL_nsf2HwSetup() :Sets the H/w for NSF2 module at one shot with the sttings provided by the application 
 *
 *  @see csl_nsf.h
 */
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     CSL_nf2HwSetup                                               
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
CSL_Status CSL_nf2HwSetup(CSL_Nf2Handle hndl, CSL_Nf2HwSetupCtrl * setup)
{
    CSL_Status status = CSL_SOK;

    CSL_EXIT_IF((setup == NULL), CSL_ESYS_INVPARAMS);

    CSL_FINS(hndl->regs->CFG, NF2_CFG_INT_EN, setup->INT_EN);
    CSL_FINS(hndl->regs->CFG, NF2_CFG_OUT_EN, setup->OUT_EN);
    CSL_FINS(hndl->regs->CFG, NF2_CFG_MODE, setup->MODE);
    CSL_FINS(hndl->regs->CFG, NF2_CFG_TRIG_SRC, setup->TRIG_SRC);
    CSL_FINS(hndl->regs->CFG, NF2_CFG_C_AS_INTRLV_Y, setup->C_AS_INTRLV_Y);
    CSL_FINS(hndl->regs->CFG, NF2_CFG_SOFT_THR_EN_CHROMA,
             setup->SOFT_THR_EN_CHROMA);
    CSL_FINS(hndl->regs->CFG, NF2_CFG_SOFT_THR_EN_LUMA,
             setup->SOFT_THR_EN_LUMA);
    CSL_FINS(hndl->regs->CFG, NF2_CFG_CBCR_EN, setup->CBCR_EN);
    CSL_FINS(hndl->regs->CFG, NF2_CFG_Y_EN, setup->Y_EN);
    CSL_FINS(hndl->regs->CFG, NF2_CFG_LBKEEP, setup->LBKEEP);
    CSL_FINS(hndl->regs->CFG, NF2_CFG_EE_EN, setup->EE_EN);
    CSL_FINS(hndl->regs->CFG, NF2_CFG_SHD_EN, setup->SHD_EN);
    CSL_FINS(hndl->regs->CFG, NF2_CFG_DESAT_EN, setup->DESAT_EN);

    hndl->regs->SFT_SLOPE =
        CSL_FMK(NF2_SFT_SLOPE_SLOPE, setup->soft_slope_value);
    CSL_EXIT_IF((_CSL_nsfAddrConfig(hndl, setup->address_config) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_nsfShdConfig(hndl, setup->shading_config) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_nsfThrConfig(hndl, setup->threshold_config) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_nsfDsConfig(hndl, setup->desaturation_config) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_nsfEeLxConfig(hndl, setup->edge_config) != CSL_SOK),
                CSL_ESYS_INVPARAMS);

    /* Important to start the NSF2 module at the end of all HW configurations 
     * for proper functioning */
    CSL_FINS(hndl->regs->CFG, NF2_CFG_START, setup->START);

  EXIT:
    return status;
}

/* ========================================================================== 
 */
/**
 * @fn CSL_nsf2Close() :close call deallocates resources for NSF  for the particular instance of the NSF  
 *
 *  @see csl_nsf.h
 */
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     CSL_nf2Close                                               
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
CSL_Status CSL_nf2Close(CSL_Nf2Handle hndl)
{
    /* Indicate in the CSL global data structure that the peripheral has been 
     * unreserved */
    return CSL_SOK;
}

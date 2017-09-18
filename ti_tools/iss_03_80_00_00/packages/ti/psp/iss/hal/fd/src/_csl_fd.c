/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file _csl_fd.c
*
* This File contains CSL Level 0 functions for FD Module
* This entire description will appear as one
* paragraph in the generated documentation.
*
* @path  $(CSLPATH)\src\FD\
*
* @rev  00.01
*/
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *! 23-Nov-2009
 * Vijay Badiger: Created the file. *! *! *! Revisions appear in reverse
 * chronological order; *! that is, newest first.  The date format is
 * dd-Mon-yyyy. *
 * =========================================================================== */

/****************************************************************
*  INCLUDE FILES
****************************************************************/
/*-------program files ----------------------------------------*/
#include "../iss/simcop/common/csl_error.h"
#include "cslr__fd.h"
#include "csl_fd.h"

CSL_Status _CSL_fdIrqClrEvent0(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FDIF_IRQSTATUS__0, FDIF_IRQSTATUS__0_FINISH_IRQ, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdIrqEOIEnable                                               
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
CSL_Status _CSL_fdIrqEOIEnable(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FDIF_IRQ_EOI, FDIF_IRQ_EOI_LINE_NUMBER, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdInt0Enable                                               
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
CSL_Status _CSL_fdInt0Enable(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FDIF_IRQENABLE_SET__0, FDIF_IRQENABLE_SET__0_FINISH_IRQ,
             data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdInt0EnableClear                                               
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
CSL_Status _CSL_fdInt0EnableClear(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FDIF_IRQENABLE_CLR__0, FDIF_IRQENABLE_CLR__0_FINISH_IRQ,
             data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdInt1Enable                                               
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
CSL_Status _CSL_fdInt1Enable(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FDIF_IRQENABLE_SET__1, FDIF_IRQENABLE_SET__1_FINISH_IRQ,
             data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdInt1EnableClear                                               
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
CSL_Status _CSL_fdInt1EnableClear(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FDIF_IRQENABLE_CLR__1, FDIF_IRQENABLE_CLR__1_FINISH_IRQ,
             data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdInt2Enable                                               
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
CSL_Status _CSL_fdInt2Enable(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FDIF_IRQENABLE_SET__2, FDIF_IRQENABLE_SET__2_FINISH_IRQ,
             data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdInt2EnableClear                                               
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
CSL_Status _CSL_fdInt2EnableClear(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FDIF_IRQENABLE_CLR__2, FDIF_IRQENABLE_CLR__2_FINISH_IRQ,
             data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdSetImageStartX                                               
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
CSL_Status _CSL_fdSetImageStartX(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FD_STARTX, FD_STARTX, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdGetImageStartX                                               
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
CSL_Status _CSL_fdGetImageStartX(CSL_FdHandle hndl, Uint32 * data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->FD_STARTX, FD_STARTX);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdSetImageStartY                                               
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
CSL_Status _CSL_fdSetImageStartY(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FD_STARTY, FD_STARTY, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdGetImageStartY                                               
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
CSL_Status _CSL_fdGetImageStartY(CSL_FdHandle hndl, Uint32 * data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->FD_STARTY, FD_STARTY);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdSetSizeX                                               
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
CSL_Status _CSL_fdSetSizeX(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FD_SIZEX, FD_SIZEX, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdGetSizeX                                               
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
CSL_Status _CSL_fdGetSizeX(CSL_FdHandle hndl, Uint32 * data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->FD_SIZEX, FD_SIZEX);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdSetSizeY                                               
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
CSL_Status _CSL_fdSetSizeY(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FD_SIZEY, FD_SIZEY, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdGetSizeY                                               
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
CSL_Status _CSL_fdGetSizeY(CSL_FdHandle hndl, Uint32 * data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->FD_SIZEY, FD_SIZEY);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdSetFace_Direction                                               
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
CSL_Status _CSL_fdSetFace_Direction(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FD_DCOND, FD_DCOND_DIR, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdGetFaceDirection                                               
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
CSL_Status _CSL_fdGetFaceDirection(CSL_FdHandle hndl, Uint32 * data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->FD_DCOND, FD_DCOND_DIR);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdSetMinfaceSize                                               
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
CSL_Status _CSL_fdSetMinfaceSize(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FD_DCOND, FD_DCOND_MIN, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdGetMinfaceSize                                               
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
CSL_Status _CSL_fdGetMinfaceSize(CSL_FdHandle hndl, Uint32 * data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->FD_DCOND, FD_DCOND_MIN);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdSetThresholdValue                                               
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
CSL_Status _CSL_fdSetThresholdValue(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FD_LHIT, FD_LHIT, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdGetThresholdValue                                               
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
CSL_Status _CSL_fdGetThresholdValue(CSL_FdHandle hndl, Uint32 * data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->FD_LHIT, FD_LHIT);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdSetPictureAddress                                               
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
CSL_Status _CSL_fdSetPictureAddress(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FDIF_PICADDR, FDIF_PICADDR, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdGetPictureAddress                                               
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
CSL_Status _CSL_fdGetPictureAddress(CSL_FdHandle hndl, Uint32 * data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->FDIF_PICADDR, FDIF_PICADDR);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdSetWorkAreaAddress                                               
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
CSL_Status _CSL_fdSetWorkAreaAddress(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FDIF_WKADDR, FDIF_WKADDR, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdGetWorkAreaAddress                                               
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
CSL_Status _CSL_fdGetWorkAreaAddress(CSL_FdHandle hndl, Uint32 * data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->FDIF_WKADDR, FDIF_WKADDR);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdGetNumofFacesDetected                                               
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
CSL_Status _CSL_fdGetNumofFacesDetected(CSL_FdHandle hndl, Uint32 * data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->FD_DNUM, FD_DNUM);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdGetResultXpostion                                               
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
CSL_Status _CSL_fdGetResultXpostion(CSL_FdHandle hndl, Uint32 * data,
                                    Uint16 loop_index)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->RESULTS[loop_index].FD_CENTERX, FD_CENTERX);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdGetResultYpostion                                               
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
CSL_Status _CSL_fdGetResultYpostion(CSL_FdHandle hndl, Uint32 * data,
                                    Uint16 loop_index)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->RESULTS[loop_index].FD_CENTERY, FD_CENTERY);
    return CSL_SOK;

}

/* ===================================================================
 *  @func     _CSL_fdGetResultConfidence                                               
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
CSL_Status _CSL_fdGetResultConfidence(CSL_FdHandle hndl, Uint32 * data,
                                      Uint16 loop_index)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->RESULTS[loop_index].FD_CONFSIZE, FD_CONFSIZE_CONF);
    return CSL_SOK;

}

/* ===================================================================
 *  @func     _CSL_fdGetResultSize                                               
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
CSL_Status _CSL_fdGetResultSize(CSL_FdHandle hndl, Uint32 * data,
                                Uint16 loop_index)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->RESULTS[loop_index].FD_CONFSIZE, FD_CONFSIZE_SIZE);
    return CSL_SOK;

}

/* ===================================================================
 *  @func     _CSL_fdGetResultAngle                                               
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
CSL_Status _CSL_fdGetResultAngle(CSL_FdHandle hndl, Uint32 * data,
                                 Uint16 loop_index)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->RESULTS[loop_index].FD_ANGLE, FD_ANGLE);
    return CSL_SOK;

}

/* ===================================================================
 *  @func     _CSL_fdGetFinishStatus                                               
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
CSL_Status _CSL_fdGetFinishStatus(CSL_FdHandle hndl, Uint32 * data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    if (data == NULL)
        return (CSL_ESYS_INVPARAMS);
    *data = CSL_FEXT(fdRegs->FD_CTRL, FD_CTRL_FINISH);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdSoftReset                                               
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
CSL_Status _CSL_fdSoftReset(CSL_FdHandle hndl, Uint32 data)
{
    CSL_Status status = CSL_SOK;

    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FDIF_SYSCONFIG, FDIF_SYSCONFIG_SOFTRESET, data);
    return status;
}

/* ===================================================================
 *  @func     _CSL_fdRun                                               
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
CSL_Status _CSL_fdRun(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FD_CTRL, FD_CTRL_RUN, data);
    return CSL_SOK;
}

/* ===================================================================
 *  @func     _CSL_fdWrnp                                               
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
CSL_Status _CSL_fdWrnp(CSL_FdHandle hndl, Uint32 data)
{
    CSL_FdRegsOvly fdRegs;

    if (hndl == NULL)
        return (CSL_ESYS_BADHANDLE);
    fdRegs = hndl->regs;
    CSL_FINS(fdRegs->FDIF_CTRL, FDIF_CTRL_OCP_WRNP, data);
    return CSL_SOK;
}

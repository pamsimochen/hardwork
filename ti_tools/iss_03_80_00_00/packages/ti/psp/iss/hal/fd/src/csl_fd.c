/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file csl_fd.c
*
* This File contains CSL APIs for FD Module
* This entire description will appear as one
* paragraph in the generated documentation.
*
* @path  $(CSLPATH)\src\FD\
*
* @rev  00.01
*/
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *! 23-Nov -2009 
 * Vijay Badiger: Created the file. *! *! *! 24-Dec-2000 mf: Revisions appear in 
 * reverse chronological order; *! that is, newest first.  The date format is
 * dd-Mon-yyyy. *
 * =========================================================================== */

/****************************************************************
*  INCLUDE FILES
****************************************************************/
/*-------program files ----------------------------------------*/
#include "csl_fd.h"
#include "../inc/_csl_fd.h"

CSL_FdHandle Fdhndl;

/* ========================================================================== 
 */
/**
* CSL_fdOpen() The open call sets up the data structures for the particular instance of the DCT.
* It gets a handle to the FD module of SIMCOP
* @param  hndl  Handle to the FD object
* @param  hFdObj  Pointer to the DCT object
* @param  fdNum  Instance of DCT device=
* @param  openMode  Open mode (Shared/Exclusive)
* @param  status  CSL_SOK Succesfull open
*                         Other value = Open failed and the error code is returned.
* @return   hFd =  Handle to FD  object
*                       =  NULL => failed
*/
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     CSL_fdOpen                                               
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
CSL_Status CSL_fdOpen(CSL_FdObj * hFdObj, CSL_FdNum fdNum,
                      CSL_OpenMode openMode)
{

    CSL_Status status = CSL_SOK;

    hFdObj->openMode = openMode;

    hFdObj->xio = CSL_FD_0_XIO;                            // hardcoded
    hFdObj->uid = CSL_FD_0_UID;                            // hardcoded
    hFdObj->regs = (CSL_FdRegsOvly) CSL_FD_REGS;           // need to confirm 
                                                           // base adress
    hFdObj->perNum = fdNum;

    return status;
}

/* ========================================================================== 
 */
/**
* CSL_fdHwSetup() Sets the H/w for FD module at one shot with the sttings provided by the application
* @param  hndl  Handle to the FD object
* @param  setup  Setup structure containing values for register settings
* @return  status  CSL_SOK Succesfull Close
*                         Other value = Close failed and the error code is returned.
*/
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     CSL_fdHwSetup                                               
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
CSL_Status CSL_fdHwSetup(CSL_FdHandle hndl, CSL_FdHwSetupCtrl * setup)
{
    CSL_Status status = CSL_SOK;

    volatile int cnt = 0;

    CSL_EXIT_IF((setup == NULL), CSL_ESYS_INVPARAMS);

    CSL_EXIT_IF(setup->ImageStartX > 160, CSL_ESYS_INVPARAMS);

    CSL_EXIT_IF(setup->ImageStartY > 120, CSL_ESYS_INVPARAMS);

    CSL_EXIT_IF(setup->MinfaceSize > 3, CSL_ESYS_INVPARAMS);

    CSL_EXIT_IF(setup->ThresholdValue > 9, CSL_ESYS_INVPARAMS);

    CSL_EXIT_IF(setup->Face_Direction > 2, CSL_ESYS_INVPARAMS);

    CSL_EXIT_IF((_CSL_fdSoftReset(hndl, setup->Soft_Reset_bit) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    for (cnt = 0; cnt < 10000; cnt++) ;

    CSL_EXIT_IF((_CSL_fdSetImageStartX(hndl, setup->ImageStartX) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_fdSetImageStartY(hndl, setup->ImageStartY) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_fdSetSizeX(hndl, setup->SizeX) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_fdSetSizeY(hndl, setup->SizeY) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_fdSetThresholdValue(hndl, setup->ThresholdValue) !=
                 CSL_SOK), CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_fdSetMinfaceSize(hndl, setup->MinfaceSize) != CSL_SOK),
                CSL_ESYS_INVPARAMS);
    CSL_EXIT_IF((_CSL_fdSetFace_Direction(hndl, setup->Face_Direction) !=
                 CSL_SOK), CSL_ESYS_INVPARAMS);

  EXIT:
    return status;
}

/* ========================================================================== 
 */
/**
* CSL_fdHwControl() Sets the H/w for FD module as requested by the application
* @param  hndl  Handle to the FD object
* @param  cmd  CMD used to select a particular field name/register
* @param  data  pointer which Points to the data/value  being passed
* @return  status  CSL_SOK Succesfull Setup
*                         Other value = Setup failed and the error code is returned.
*/
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     CSL_fdHwControl                                               
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
CSL_Status CSL_fdHwControl(CSL_FdHandle hndl, CSL_FdHwCtrlCmdType * cmd,
                           void *data)
{
    CSL_Status status = CSL_SOK;

    switch (*cmd)
    {

        case CSL_FD_CMD_SETPICADDR:
            status = _CSL_fdSetPictureAddress(hndl, *(Uint32 *) data);
            break;

        case CSL_FD_CMD_SETWKADDR:
            status = _CSL_fdSetWorkAreaAddress(hndl, *(Uint32 *) data);
            break;

        case CSL_FD_CMD_SETDCOND_DIR:
            CSL_EXIT_IF(*(Int32 *) data < 0, CSL_ESYS_INVPARAMS);
            CSL_EXIT_IF(*(Int32 *) data > 2, CSL_ESYS_INVPARAMS);

            status = _CSL_fdSetFace_Direction(hndl, *(Uint32 *) data);
            break;

        case CSL_FD_CMD_SETDCOND_MINFACESIZE:
            CSL_EXIT_IF((*(Int32 *) data < 0), CSL_ESYS_INVPARAMS);
            CSL_EXIT_IF((*(Int32 *) data > 3), CSL_ESYS_INVPARAMS);
            status = _CSL_fdSetMinfaceSize(hndl, *(Uint32 *) data);
            break;

        case CSL_FD_CMD_SETLHIT:
            CSL_EXIT_IF((*(Int32 *) data > 9), CSL_ESYS_INVPARAMS);
            CSL_EXIT_IF((*(Int32 *) data < 0), CSL_ESYS_INVPARAMS);
            status = _CSL_fdSetThresholdValue(hndl, *(Uint32 *) data);
            break;

    }
  EXIT:
    return status;
}

/* ========================================================================== 
 */
/**
* CSL_fdGetHwStatus() Gets the H/w Status for FD module requested by the application
* @param  hndl  Handle to the FD object
* @param  cmd  CMD used to select a particular field name/register
* @param  data  pointer which Points to the data/value  being queried
* @return  status  CSL_SOK Succesfull Query
*                         Other value = Query failed and the error code is returned.
*/
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     CSL_fdGetHwStatus                                               
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
CSL_Status CSL_fdGetHwStatus(CSL_FdHandle hndl, CSL_FdHwQueryType * query,
                             void *data)
{
    CSL_Status status = CSL_SOK;

    switch (*query)
    {

        case CSL_FD_QUERY_PICADDR:
            status = _CSL_fdGetPictureAddress(hndl, (Uint32 *) data);
            break;

        case CSL_FD_QUERY_WKADDR:
            status = _CSL_fdGetWorkAreaAddress(hndl, (Uint32 *) data);
            break;

        case CSL_FD_QUERY_DNUM:
            status = _CSL_fdGetNumofFacesDetected(hndl, (Uint32 *) data);
            break;

        case CSL_FD_QUERY_DCOND_DIR:
            status = _CSL_fdGetFaceDirection(hndl, (Uint32 *) data);
            break;

        case CSL_FD_QUERY_DCOND_MINFACESIZE:
            status = _CSL_fdGetMinfaceSize(hndl, (Uint32 *) data);
            break;

        case CSL_FD_QUERY_STARTX:
            status = _CSL_fdGetImageStartX(hndl, (Uint32 *) data);
            break;

        case CSL_FD_QUERY_STARTY:
            status = _CSL_fdGetImageStartY(hndl, (Uint32 *) data);
            break;

        case CSL_FD_QUERY_SIZEX:
            status = _CSL_fdGetSizeX(hndl, (Uint32 *) data);
            break;

        case CSL_FD_QUERY_SIZEY:
            status = _CSL_fdGetSizeY(hndl, (Uint32 *) data);
            break;

        case CSL_FD_QUERY_LHIT:
            status = _CSL_fdGetThresholdValue(hndl, (Uint32 *) data);
            break;

        case CSL_FD_QUERY_IRQ_STATUS:
            status = _CSL_fdGetFinishStatus(hndl, (Uint32 *) data);
            break;

    }
    return status;
}

/* ========================================================================== 
 */
/**
* CSL_fdClose() The close call deallocates resources for FD  for the particular instance of the FD for which the handle is passed
* @param  hndl  Handle to the FD object
* @return  status  CSL_SOK Succesfull Close
*                         Other value = Close failed and the error code is returned.
*/
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     CSL_fdClose                                               
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
CSL_Status CSL_fdClose(CSL_FdHandle hndl)
{
    /* Indicate in the CSL global data structure that the peripheral has been 
     * unreserved */
    // return (_CSL_certifyClose((CSL_ResHandle)hndl));
    return CSL_SOK;
}

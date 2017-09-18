/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file _csl_fd.h
*
* This File contains declarations for _csl_fd.c
* This entire description will appear as one 
* paragraph in the generated documentation.
*
* @path  $(DUCATIVOB)\drivers\csl\fd\inc
* @rev  00.01
*/
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 20_Nov_2009 Vijay Badiger:  Created the file.  
 *! 
 *!
 *! Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */

#ifndef _CSL_FD_H
#define _CSL_FD_H

#ifdef __cplusplus

extern "C" {
#endif
/****************************************************************
*  INCLUDE FILES                                                 
****************************************************************/
/*-------program files ----------------------------------------*/
#include "../iss/hal/iss/simcop/common/csl.h"
#include "../csl_fd.h"

/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/
    CSL_Status _CSL_fdIntEnable(CSL_FdHandle hndl, Uint32 data);

    CSL_Status _CSL_fdIntEnableClear(CSL_FdHandle hndl, Uint32 data);

    CSL_Status _CSL_fdIrqClrEvent0(CSL_FdHandle hndl, Uint32 data);

    CSL_Status _CSL_fdIrqEOIEnable(CSL_FdHandle hndl, Uint32 data);

    CSL_Status _CSL_fdInt0Enable(CSL_FdHandle hndl, Uint32 data);

    CSL_Status _CSL_fdInt0EnableClear(CSL_FdHandle hndl, Uint32 data);

    CSL_Status _CSL_fdInt1Enable(CSL_FdHandle hndl, Uint32 data);

    CSL_Status _CSL_fdInt1EnableClear(CSL_FdHandle hndl, Uint32 data);

    CSL_Status _CSL_fdInt2Enable(CSL_FdHandle hndl, Uint32 data);

    CSL_Status _CSL_fdInt2EnableClear(CSL_FdHandle hndl, Uint32 data);

    CSL_Status _CSL_fdSetImageStartX(CSL_FdHandle hndl, Uint32 data);
    CSL_Status _CSL_fdGetImageStartX(CSL_FdHandle hndl, Uint32 * data);

    CSL_Status _CSL_fdSetImageStartY(CSL_FdHandle hndl, Uint32 data);
    CSL_Status _CSL_fdGetImageStartY(CSL_FdHandle hndl, Uint32 * data);

    CSL_Status _CSL_fdSetSizeX(CSL_FdHandle hndl, Uint32 data);
    CSL_Status _CSL_fdGetSizeX(CSL_FdHandle hndl, Uint32 * data);

    CSL_Status _CSL_fdSetSizeY(CSL_FdHandle hndl, Uint32 data);
    CSL_Status _CSL_fdGetSizeY(CSL_FdHandle hndl, Uint32 * data);

    CSL_Status _CSL_fdSetFace_Direction(CSL_FdHandle hndl, Uint32 data);
    CSL_Status _CSL_fdGetFaceDirection(CSL_FdHandle hndl, Uint32 * data);

    CSL_Status _CSL_fdSetMinfaceSize(CSL_FdHandle hndl, Uint32 data);
    CSL_Status _CSL_fdGetMinfaceSize(CSL_FdHandle hndl, Uint32 * data);

    CSL_Status _CSL_fdSetThresholdValue(CSL_FdHandle hndl, Uint32 data);
    CSL_Status _CSL_fdGetThresholdValue(CSL_FdHandle hndl, Uint32 * data);

    CSL_Status _CSL_fdSetPictureAddress(CSL_FdHandle hndl, Uint32 data);
    CSL_Status _CSL_fdGetPictureAddress(CSL_FdHandle hndl, Uint32 * data);

    CSL_Status _CSL_fdSetWorkAreaAddress(CSL_FdHandle hndl, Uint32 data);
    CSL_Status _CSL_fdGetWorkAreaAddress(CSL_FdHandle hndl, Uint32 * data);

    CSL_Status _CSL_fdGetNumofFacesDetected(CSL_FdHandle hndl, Uint32 * data);

    CSL_Status _CSL_fdSoftReset(CSL_FdHandle hndl, Uint32 data);
    CSL_Status _CSL_fdGetFinishStatus(CSL_FdHandle hndl, Uint32 * data);

    CSL_Status _CSL_fdGetResultXpostion(CSL_FdHandle hndl, Uint32 * data,
                                        Uint16 loop_index);
    CSL_Status _CSL_fdGetResultYpostion(CSL_FdHandle hndl, Uint32 * data,
                                        Uint16 loop_index);
    CSL_Status _CSL_fdGetResultConfidence(CSL_FdHandle hndl, Uint32 * data,
                                          Uint16 loop_index);
    CSL_Status _CSL_fdGetResultSize(CSL_FdHandle hndl, Uint32 * data,
                                    Uint16 loop_index);
    CSL_Status _CSL_fdGetResultAngle(CSL_FdHandle hndl, Uint32 * data,
                                     Uint16 loop_index);

    CSL_Status _CSL_fdRun(CSL_FdHandle hndl, Uint32 data);
    CSL_Status _CSL_fdWrnp(CSL_FdHandle hndl, Uint32 data);

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/*--------macros ----------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif                                                     /* _CSL_FD_H */

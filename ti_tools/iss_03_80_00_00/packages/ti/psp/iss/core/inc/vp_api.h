/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file vp_api.h
*
* This file contains routines declarations and defines for Parallel video interface
*       driver core
*
* @path iss/drivers/drv_vp/inc
*
* @rev 01.01
*/
/* -------------------------------------------------------------------------- 
 */
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *!
 * 05-Apr-2011 LVasilev - Created *! *
 * =========================================================================== */
#ifndef _VP_API_H
#define _VP_API_H

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

    /* User code goes here */
    /* ------compilation control switches
     * --------------------------------------- */
/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
    /* ----- system and platform files ---------------------------- */
#include <msp/msp.h>

    // #include <csl/iss/csi2/csi2.h>
/*-------program files ----------------------------------------*/
    // #include <csl/iss/csi2/csi2.h"
/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/

/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/
    MSP_ERROR_TYPE vp_init(void);

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _VP_API_H */

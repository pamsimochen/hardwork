/* ============================================================================== 
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved.  Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * =========================================================================== 
 */
/**
 * @file _csl_nsf.h
 *
 * This File contains declarations for _csl_nsf.c
 *
 * @path  $(DUCATIVOB)\drivers\csl\iss\simcop\nsf\inc\
 *
 * @rev  1.0
 */

/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 23-Jan-2009   Sanish Mahadik    : Added the basic register updation functionality
 *! 05-Dec -2008 Sherin Sasidharan:  Created the file.  
 *! 
 *!
 *! Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */

#ifndef _CSL_NF2_H0_
#define _CSL_NF2_H0_

#ifdef __cplusplus
extern "C" {
#endif

   /****************************************************************
    *  INCLUDE FILES                                                 
    ****************************************************************/
   /*-------program files ----------------------------------------*/
#include "../../common/csl.h"
#include "../cslr__nsf.h"
#include "../csl_nsf.h"

   /****************************************************************
    *  PUBLIC DECLARATIONS Defined here, used elsewhere
    ****************************************************************/
   /*--------data declarations -----------------------------------*/

   /*--------function prototypes ---------------------------------*/

    /* ========================================================================== 
     */
    /* ! _CSL_nsf2RegisterReset() will set the registers of NSF to their
     * RESET values @param hndl Handle to the NSF object @return status
     * CSL_SOK Succesfull Other value = failed and the error code is
     * returned. */
    /* ========================================================================== 
     */
    CSL_Status _CSL_nsf2RegisterReset(CSL_Nf2Handle hndl);

    /* ========================================================================== 
     */
    /* ! _CSL_nsf2GetAttrs() will return the attributes of the NSF instance
     * 
     * * @param nsfNum instance of the NSF being queried @param hNfs handle
     * to the NSF instance @return status CSL_SOK Succesfull Query Other
     * value = Query failed and the error code is returned. */
    /* ========================================================================== 
     */
    CSL_Status _CSL_nsf2GetAttrs(CSL_NsfNum nsfNum, CSL_Nf2Handle hNfs);

    /* ========================================================================== 
     */
    /* ! _CSL_nsfAddrConfig() will configure the address fields of the NSF
     * module in one shot (bulk configuration)
     * 
     * * @param hndl handle to the NSF instance @param data pointer to the
     * values which are to be filled in the register fields @return status
     * CSL_SOK Succesfull configuration Other value = configuration failed
     * and the error code is returned. */
    /* ========================================================================== 
     */
    CSL_Status _CSL_nsfAddrConfig(CSL_Nf2Handle hndl, CSL_Nf2AddrConfig * data);

    /* ========================================================================== 
     */
    /* ! _CSL_nsfThrConfig() will configure the threshold parameters of the
     * NSF module (bulk configuration)
     * 
     * * @param hndl handle to the NSF instance @param data pointer to the
     * values which are to be filled in the register fields @return status
     * CSL_SOK Succesfull configuration Other value = configuration failed
     * and the error code is returned. */
    /* ========================================================================== 
     */
    CSL_Status _CSL_nsfThrConfig(CSL_Nf2Handle hndl, CSL_Nf2ThrConfig * data);

    /* ========================================================================== 
     */
    /* ! _CSL_nsfShdConfig() will configure the shading correction parameters 
     * of the NSF module (bulk configuration)
     * 
     * * @param hndl handle to the NSF instance @param data pointer to the
     * values which are to be filled in the register fields @return status
     * CSL_SOK Succesfull configuration Other value = configuration failed
     * and the error code is returned. */
    /* ========================================================================== 
     */
    CSL_Status _CSL_nsfShdConfig(CSL_Nf2Handle hndl, CSL_Nf2ShdConfig * data);

    /* ========================================================================== 
     */
    /* ! _CSL_nsfEeLxConfig() will configure the edge enhancement parameters
     * of the NSF module (bulk configuration)
     * 
     * * @param hndl handle to the NSF instance @param data pointer to the
     * values which are to be filled in the register fields @return status
     * CSL_SOK Succesfull configuration Other value = configuration failed
     * and the error code is returned. */
    /* ========================================================================== 
     */
    CSL_Status _CSL_nsfEeLxConfig(CSL_Nf2Handle hndl, CSL_Nf2EeLxConfig * data);

    /* ========================================================================== 
     */
    /* ! _CSL_nsfDsConfig() will configure the desaturation parameters of the 
     * NSF module (bulk configuration)
     * 
     * * @param hndl handle to the NSF instance @param data pointer to the
     * values which are to be filled in the register fields @return status
     * CSL_SOK Succesfull configuration Other value = configuration failed
     * and the error code is returned. */
    /* ========================================================================== 
     */
    CSL_Status _CSL_nsfDsConfig(CSL_Nf2Handle hndl, CSL_Nf2DsConfig * data);

#ifdef __cplusplus
}
#endif
#endif

/*
********************************************************************************
 * HDVICP2.0 Based MPEG4 SP Encoder
 *  
 * "HDVICP2.0 Based MPEG4 SP Encoder" is software module developed on TI's
 *  HDVICP2 based SOCs. This module is capable of compressing a 4:2:0 Raw
 *  video into a simple profile bit-stream. Based on ISO/IEC 14496-2."
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
********************************************************************************
*/
/**
********************************************************************************
 * @file <mpeg4enc_ti.h>
 *
 * @brief Interface for the MPEG4ENC_TI module; TI's implementation 
 *  of the IMPEG4ENC interface
 *
 * @author: Venugopala Krishna
 *
 * @version 0.0 (Feb 2009) : Initial version.
 *                           [Venugopala Krishna]
 * @version 0.1 (Apr 2009) : Updated version.
 *                           [Radhesh Bhat]
 *
 *******************************************************************************
 */
/* -------------------- compilation control switches -------------------------*/
#ifndef MPEG4ENC_TI_
#define MPEG4ENC_TI_

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/* -------------------- system and platform files ----------------------------*/

/*--------------------- program files ----------------------------------------*/
#include <ti/xdais/ialg.h>

#include "impeg4enc.h"

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/*******************************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/
/*---------------------- function prototypes ---------------------------------*/

/*
 *  ======== MPEG4ENC_TI_IALG ========
 *  TI's implementation of the IALG interface for MPEG4ENC
 */
  extern IALG_Fxns MPEG4ENC_TI_IALG;

/*
 *  ======== MPEG4ENC_TI_IMPEG4ENC ========
 *  TI's implementation of the IVIDENC1 interface
 */
  extern IVIDENC2_Fxns MPEG4ENC_TI_IMPEG4ENC;

/*
 *  ======== MPEG4ENC_TI_exit ========
 *  Required module finalization function
 */
  extern Void MPEG4ENC_TI_exit(Void);


#ifdef __cplusplus
}
#endif             /* extern "C" */
#endif             /* MPEG4ENC_TI_ */

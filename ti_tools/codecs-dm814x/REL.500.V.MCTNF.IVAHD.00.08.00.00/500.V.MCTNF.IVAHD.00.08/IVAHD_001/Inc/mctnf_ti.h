/*
********************************************************************************
 * HDVICP2.0 Based Motion Compensated Temporal Noise Filter(MCTNF)
 *
 * "HDVICP2.0 Based MCTNF" is software module developed on TI's
 *  HDVICP2 based SOCs. This module is capable of filtering noise from
 *  4:2:0 Raw video.
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
********************************************************************************
*/

/*!
 *****************************************************************************
 * \file
 *  mctnf_ti.h
 *
 * \brief
 *  TI Interface related header file
 *
 * \version 0.1
 *
 * \date Nov 2013
 *
 * \author
 *  MMCodecs TI India
 *****************************************************************************
 */

#ifndef MCTNF_TI_
#define MCTNF_TI_

#include <ti/xdais/ialg.h>

#include "imctnf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== MCTNF_TI_IALG ========
 *  TI's implementation of the IALG interface for H264ENC
 */
  extern const IALG_Fxns MCTNF_TI_IALG;

/*
 *  ======== MCTNF_TI_IMCTNF ========
 *  TI's implementation of the IVIDNF1 interface
 */
  extern const IVIDNF1_Fxns MCTNF_TI_IMCTNF;

/*
 *  ======== MCTNF_TI_exit ========
 *  Required module finalization function
 */
  extern Void MCTNF_TI_exit(Void);

/*
 *  ======== MCTNF_TI_init ========
 *  Required module initialization function
 */
  //extern Void MCTNF_TI_init(Void);

#ifdef __cplusplus
}
#endif             /* extern "C" */
#endif             /* MCTNF_TI_ */

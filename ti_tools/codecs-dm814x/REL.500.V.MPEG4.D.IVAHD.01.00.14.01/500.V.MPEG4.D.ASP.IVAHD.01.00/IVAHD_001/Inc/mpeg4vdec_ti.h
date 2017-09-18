/*
********************************************************************************
* HDVICP2.0 Based MPEG4 ASP Decoder
*
* "HDVICP2.0 Based MPEG4 ASP Decoder" is software module developed on TI's
*  HDVICP2 based SOCs. This module is capable of decode a 4:2:0 Raw
*  video stream of Advanced/Simple profile and also H.263 bit-stream.
*  Based on  ISO/IEC 14496-2:2003."
* Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
* ALL RIGHTS RESERVED
********************************************************************************
*/

/**
********************************************************************************
* @file <mpeg4vdec_ti.h>
*
* @brief This file provides definisions for the interface handles.
*
*  @author: Ashish Singh   (ashish.singh@ti.com)
*
* @version 0.0 (June 2009) : Base version created [Ashish]
*
* @version 0.1 (Apr 2010) : Review Comments Added [Ananya]
*
*
*******************************************************************************
*/

/* -------------------- compilation control switches ---------------------- */

#ifndef _MPEG4VDEC_TI_H_
#define _MPEG4VDEC_TI_H_


/****************************************************************************
*   INCLUDE FILES
*****************************************************************************/
/* -------------------- system and platform files ------------------------- */

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/ividdec3.h>


/* ------------------------- program files -------------------------------- */
#include "impeg4vdec.h"

/****************************************************************************
*   EXTERNAL REFERENCES NOTE : only use if not found in header file
*****************************************************************************/
/* ------------------------ data declarations ----------------------------- */

/* Referene to external symbol MPEG4VDEC_TI_IALG, holding the pointers to the
* all the basic algorithm creation related functions.
*/
extern IALG_Fxns     MPEG4VDEC_TI_IALG;

/* Referene to external symbol MPEG4VDEC_TI_IMPEG4VDEC,holding the pointers to
* the all the mpeg4 decoder algorithm interfaces
*/
extern IVIDDEC3_Fxns MPEG4VDEC_TI_IMPEG4VDEC;

/* ----------------------- function prototypes ---------------------------- */

/****************************************************************************
*   PUBLIC DECLARATIONS Defined here, used elsewhere
*****************************************************************************/
/* ----------------------- data declarations ------------------------------ */
/* ----------------------- function prototypes ---------------------------- */

/****************************************************************************
*   PRIVATE DECLARATIONS Defined here, used only here
*****************************************************************************/
/* ---------------------- data declarations ------------------------------- */
/* --------------------- function prototypes ------------------------------ */

/* ---------------------------- macros ------------------------------------ */

#endif  /* _MPEG4VDEC_TI_H_ */



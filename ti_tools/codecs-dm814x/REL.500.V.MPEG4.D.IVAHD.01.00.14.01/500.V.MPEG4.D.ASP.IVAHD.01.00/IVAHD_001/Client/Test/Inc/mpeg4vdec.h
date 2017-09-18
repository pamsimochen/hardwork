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
*****************************************************************************
* @file   <mpeg4vdec.h>
*
* @brief  A header file describing funtion protypes for moeg4d decoder
*         interface functions with rapers.
*
* @author:  Prashanth
*
* @version 0.1 (Apr 2010) : Review Comments Added [Ananya]
* @version 0.2 (Oct 2010) : Cleared doxygen warning
*
****************************************************************************
*/

/* -------------------- compilation control switches ---------------------- */

#ifndef _MPEG4VDEC_H_
#define _MPEG4VDEC_H_

/****************************************************************************
*   INCLUDE FILES
*****************************************************************************/
/* -------------------- system and platform files ------------------------- */
#include <ti/sdo/fc/utils/api/alg.h>
#include <ti/xdais/ialg.h>
#include <ti/xdais/dm/ividdec3.h>
#include <ti/sdo/fc/ires/hdvicp/ires_hdvicp2.h>
#include <impeg4vdec.h>


extern IRES_Fxns MPEG4VDEC_TI_IRES;


/* ------------------------- program files -------------------------------- */

/****************************************************************************
*   EXTERNAL REFERENCES NOTE : only use if not found in header file
*****************************************************************************/
/* ------------------------ data declarations ----------------------------- */


/*---------------------- function prototypes ---------------------------- */

/**
********************************************************************************
*  @fn   MPEG4VDEC_create(const IMPEG4VDEC_Fxns   *fxns,
*                                           const IMPEG4VDEC_Params *prms)
*
*  @brief  Create an mp4VDEC instance object using specified parameters
*
*  @param    *fxns : Handle to ALG functions
*
*  @param     *prms : Parameter specific to current instance
*
*  @return mp4VDEC_Handle         : Algorithm instance handle.
********************************************************************************
*/
extern IMPEG4VDEC_Handle MPEG4VDEC_create(
const IMPEG4VDEC_Fxns   *fxns,
const IMPEG4VDEC_Params *prms
);

/**
********************************************************************************
*  @fn   MPEG4VDEC_control(IMPEG4VDEC_Handle        handle,
*                IMPEG4VDEC_Cmd           cmd, IMPEG4VDEC_DynamicParams *params,
*                IMPEG4VDEC_Status        *status)
*
*  @brief   Function to either write to the read/write parameters in the
*           status structure or to read all the parameters in the status
*           structure. This function will change the configuration of the
*           decoder before the process call of each frame by using the
*           command type
*
*  @param   handle : Algorithm instance handle
*
*  @param    cmd       : instance to the control command of MPEG4
*                                   video decoder
*
*  @param    *params : pointer to the structure containing
*                                 run-time parameters for mp4VDEC objects
*
*  @param    *status : pointer to the mp4VDEC status structure
*
*  @return  Decode call status (Pass/Error)
*
********************************************************************************
*/
extern Int  MPEG4VDEC_control(
IMPEG4VDEC_Handle        handle,
IMPEG4VDEC_Cmd           cmd,
IMPEG4VDEC_DynamicParams *params,
IMPEG4VDEC_Status        *status
);


/*
 *  control method commands
*/
#define MPEG4VDEC_GETSTATUS    IMPEG4VDEC_GETSTATUS
#define MPEG4VDEC_SETPARAMS    IMPEG4VDEC_SETPARAMS
#define MPEG4VDEC_PREPROCESS   IMPEG4VDEC_PREPROCESS
#define MPEG4VDEC_RESET        IMPEG4VDEC_RESET
#define MPEG4VDEC_FLUSH        IMPEG4VDEC_FLUSH
#define MPEG4VDEC_SETDEFAULT   IMPEG4VDEC_SETDEFAULT
#define MPEG4VDEC_GETBUFINFO   IMPEG4VDEC_GETBUFINFO

/**
********************************************************************************
*  @fn   MPEG4VDEC_delete(IMPEG4VDEC_Handle handle)
*
*  @brief  Delete the mp4VDEC instance object specified by handle
*
*  @param   handle : Algorithm instance handle
*
*  @return  None
*
********************************************************************************
*/
extern void  MPEG4VDEC_delete(IMPEG4VDEC_Handle handle);

/**
********************************************************************************
*  @fn    MPEG4VDEC_init(void)
*
*  @brief   mp4VDEC module initialization
*
*  @return  None
*
********************************************************************************
*/
extern void  MPEG4VDEC_init(void);

/**
********************************************************************************
*  @fn    MPEG4VDEC_exit(void)
*
*  @brief   mp4VDEC module finalization
*
*  @return  None
*
********************************************************************************
*/
extern void  MPEG4VDEC_exit(void);

/**
********************************************************************************
*  @fn   MPEG4VDEC_decodeFrame(IMPEG4VDEC_Handle    handle,
*              XDM2_BufDesc     * input,XDM2_BufDesc      * output,
*              IMPEG4VDEC_InArgs  * inarg,IMPEG4VDEC_OutArgs * outarg)
*
*  @brief  This function will call the process of decoding for each frame
*
*  @param  handle  : Algorithm instance handle
*
*  @param  *input : Input buffer descriptors
*
*  @param   output: Output buffer descriptors
*
*  @param  *inarg : Input arguments handle
*
*  @param   outarg: Output arguments handle
*
*  @return  Decode call status (Pass/Error)
*
********************************************************************************
*/
extern Int MPEG4VDEC_decodeFrame(
IMPEG4VDEC_Handle    handle,
XDM2_BufDesc     * input,
XDM2_BufDesc      * output,
IMPEG4VDEC_InArgs  * inarg,
IMPEG4VDEC_OutArgs * outarg
);



#endif /*_MPEG4VDEC_H_*/


  /*
*******************************************************************************
* HDVICP2.0 Based MPEG4 ASP Decoder
*
* "HDVICP2.0 Based MPEG4 ASP Decoder" is software module developed on TI's
*  HDVICP2 based SOCs. This module is capable of decode a 4:2:0 Raw
*  video stream of Advanced/Simple profile and also H.263 bit-stream.
*  Based on  ISO/IEC 14496-2:2003."
* Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
* ALL RIGHTS RESERVED
*******************************************************************************
*/

/**
*******************************************************************************
* @file  <mpeg4vdec.c>
*
* @brief This File contains the definition of xDM APIs
*
* @author Prashanth
*
* @version 0.0 (Jul 2006) : Base version created [Prashanth]
*
* @version 0.1 (Apr 2010) : Incorporated code review comments [Ananya]
*
* @version 0.2 (Oct 2010) : Cleared doxygen warning
******************************************************************************
*/
/* -------------------- compilation control switches ----------------------*/


/***************************************************************************
*   INCLUDE FILES
****************************************************************************/

/* -------------------- system and platform files ------------------------- */
#include<xdc/std.h>
#include <ti/sdo/fc/utils/api/alg.h>
/* ------------------------- program files -------------------------------- */
#include "impeg4vdec.h"


/**
*******************************************************************************
*  @fn   MPEG4VDEC_create(const IMPEG4VDEC_Fxns   *fxns,
*                                               const IMPEG4VDEC_Params *prms)
*
*  @brief  Create an mp4VDEC instance object using specified parameters
*
*  @param  *fxns : Handle to ALG functions
*
*  @param  prms : Parameter specific to current instance
*
*  @return mp4VDEC_Handle         : Algorithm instance handle.
*******************************************************************************
*/
IMPEG4VDEC_Handle MPEG4VDEC_create(
const IMPEG4VDEC_Fxns   *fxns,
const IMPEG4VDEC_Params *prms
)
{
  IMPEG4VDEC_Handle handle;
  handle =  ((IMPEG4VDEC_Handle)ALG_create((IALG_Fxns   *)fxns, NULL,
  (IALG_Params *)prms));
  return handle;
}

/**
*******************************************************************************
*  @fn   MPEG4VDEC_delete(IMPEG4VDEC_Handle handle)
*
*  @brief  Delete the MPEG4 Decoder instance object specified by handle
*
*  @param  handle : Algorithm instance handle
*
*  @return  None
*
*******************************************************************************
*/
void MPEG4VDEC_delete(IMPEG4VDEC_Handle handle)
{
  ALG_delete((IALG_Handle)handle);
}


/**
*******************************************************************************
*  @fn    MPEG4VDEC_init(void)
*
*  @brief   mp4VDEC module initialization
*
*  @return  None
*
*******************************************************************************
*/
void  mp4VDEC_init(void)
{
  return;
}

/**
*******************************************************************************
*  @fn    MPEG4VDEC_exit(void)
*
*  @brief   mp4VDEC module finalization
*
*  @return  None
*
*******************************************************************************
*/
void  MPEG4VDEC_exit(void)
{
  return;
}

/**
*******************************************************************************
*  @fn   MPEG4VDEC_control(IMPEG4VDEC_Handle      handle,
*             IMPEG4VDEC_Cmd           cmd, IMPEG4VDEC_DynamicParams *params,
*             IMPEG4VDEC_Status        *status)
*
*  @brief   Function to either write to the read/write parameters in the
*           status structure or to read all the parameters in the status
*           structure. This function will change the configuration of the
*           decoder before the process call of each frame by using the
*           command type
*
*  @param   handle : Algorithm instance handle
*
*  @param   cmd       : instance to the control command of MPEG4
*                                   video decoder
*
*  @param   params : pointer to the structure containing
*                                 run-time parameters for mp4VDEC objects
*
*  @param   status : pointer to the mp4VDEC status structure
*
*  @return  Decode call status (Pass/Error)
*
*******************************************************************************
*/
Int MPEG4VDEC_control(IMPEG4VDEC_Handle      handle,
IMPEG4VDEC_Cmd           cmd,
IMPEG4VDEC_DynamicParams *params,
IMPEG4VDEC_Status        *status
)
{
  Int error;
  /*-------------------------------------------------------------------------*/
  /* ALG_activate function to be called before each control call to copy     */
  /* image of handle structure data in the external persistant memory to     */
  /* scarcth memory                                                          */
  /*-------------------------------------------------------------------------*/
  ALG_activate((IALG_Handle)handle);
  error =   handle->fxns->ividdec3.control ((IVIDDEC3_Handle)handle,
  cmd,
  (IVIDDEC3_DynamicParams *)params,
  (IVIDDEC3_Status *)status
  );
  /*-------------------------------------------------------------------------*/
  /* ALG_deactivate function to be called after each control call to store   */
  /* image of handle structure data in the scarcth  memory to                */
  /* external persistant memory                                              */
  /*-------------------------------------------------------------------------*/
  ALG_deactivate((IALG_Handle)handle);
  return (error);
}

/**
*******************************************************************************
*  @fn   MPEG4VDEC_decodeFrame(IMPEG4VDEC_Handle  handle,
*           XDM2_BufDesc       *input,XDM2_BufDesc       *output,
*           IMPEG4VDEC_InArgs  *inarg,IMPEG4VDEC_OutArgs *outarg)
*
*  @brief  This function will call the process of decoding for each frame
*
*  @param   handle  : Algorithm instance handle
*
*  @param   *input : Input buffer descriptors
*  @param    output: Output buffer descriptors
*
*  @param   *inarg : Input arguments handle
*
*  @param   *outarg: Output arguments handle
*
*  @return  Decode call status (Pass/Error)
*
*******************************************************************************
*/
Int MPEG4VDEC_decodeFrame(IMPEG4VDEC_Handle  handle,
XDM2_BufDesc       *input,
XDM2_BufDesc       *output,
IMPEG4VDEC_InArgs  *inarg,
IMPEG4VDEC_OutArgs *outarg)
{
  Int error;
  /*-------------------------------------------------------------------------*/
  /* ALG_activate function to be called before each process call to copy     */
  /* image of handle structure data in the external persistant memory to     */
  /* scarcth memory                                                          */
  /*-------------------------------------------------------------------------*/

  ALG_activate((IALG_Handle)handle);

  error = handle->fxns->ividdec3.process ((IVIDDEC3_Handle)handle,
  input,
  output,
  (IVIDDEC3_InArgs * )inarg,
  (IVIDDEC3_OutArgs *)outarg
  );

  /*--------------------------------------------------------------------------*/
  /* ALG_deactivate function to be called before each process call to store   */
  /* image of handle structure data in the scarcth  memory to                 */
  /* external persistant memory                                               */
  /*--------------------------------------------------------------------------*/

  ALG_deactivate((IALG_Handle)handle);
  return(error);
}




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
********************************************************************************
 * @file <alg_control.c>                          
 *                                       
 * @brief This file contains Functions related to Alg control implementation. 
 *        This is common to all implementations on the ALG module 
 *         
 * @author: Multimedia Codecs TI
 *
 *******************************************************************************
*/
 
/*******************************************************************************
*                             INCLUDE FILES                           
*******************************************************************************/
#include <xdc/std.h>
#include <ti/xdais/ialg.h>
#include <ti/sdo/fc/utils/api/alg.h>

/** 
********************************************************************************
 *  @fn     ALG_control
 *  @brief  This is common to all implementations of the ALG module.
 *          
 *  @param[in] alg          : Handle to algorithm structure.
 *  @param[in] cmd          : Algorith command.
 *  @param[out] statusPtr   : Pointer to the algorithm status structure.
 *          
 *  @return IALG_OK or IALG_EFAIL
********************************************************************************
*/
Int ALG_control(ALG_Handle alg, IALG_Cmd cmd, IALG_Status * statusPtr)
{
  /*--------------------------------------------------------------------------*/
  /* Check for a valid handle & function pointers.                            */
  /* Return a failure in case one of these are NULL pointers.                 */
  /*--------------------------------------------------------------------------*/
  if (alg && alg->fxns->algControl)
  {
    /*------------------------------------------------------------------------*/
    /* Make XDM control call with a XDM Command                               */
    /*------------------------------------------------------------------------*/
    return (alg->fxns->algControl(alg, cmd, statusPtr));
  }

  return (IALG_EFAIL);
}



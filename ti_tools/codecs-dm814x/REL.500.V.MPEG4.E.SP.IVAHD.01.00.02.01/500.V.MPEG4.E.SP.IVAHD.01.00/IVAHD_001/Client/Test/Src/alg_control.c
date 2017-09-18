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
 * @file <alg_control.c>
 *
 * @brief ALG_control implementation.  This is common to all implementations
 *        of the ALG module.
 *
 * @author:
 *
 * @version 0.0 (Mon Year) : Change description
 *                           [author name]
 *
 *******************************************************************************
*/


/* -------------------- compilation control switches -------------------------*/

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/* -------------------- system and platform files ----------------------------*/

/*--------------------- program files ----------------------------------------*/
#include <TestAppComDataType.h>
#include <ti/xdais/ialg.h>

/*******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/*******************************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/*---------------------- function prototypes ---------------------------------*/



/**
********************************************************************************
 *  @func     ALG_control
 *  @brief  This is common to all implementations of the ALG module.
 *
 *  @param[in]  alg       : Pointer to the algorithm structure
 *
 *  @param[in]  cmd       : algorithm command
 *
 *  @param[in]  statusPtr : Pointer to the IALG_Status structure
 *
 *  @return     IALG_OK or IALG_EFAIL
********************************************************************************
*/
Int ALG_control(IALG_Handle alg, IALG_Cmd cmd, IALG_Status * statusPtr)
{
  if (alg && alg->fxns->algControl) {
    return (alg->fxns->algControl(alg, cmd, statusPtr));
  }

  return (IALG_EFAIL);
}

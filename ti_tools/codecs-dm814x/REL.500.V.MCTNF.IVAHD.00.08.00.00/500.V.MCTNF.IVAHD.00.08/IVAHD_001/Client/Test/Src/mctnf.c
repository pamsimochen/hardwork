/*
********************************************************************************
 * HDVICP2.0 Based Motion Compensated Temporal Noise Filter(MCTNF)
 *
 * "HDVICP2.0 Based MCTNF" is software module developed on TI's
 *  HDVICP2 based SOCs. This module is capable of filtering noise from a
 *  4:2:0 semi planar Raw data.
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
********************************************************************************
*/

/*!
 *****************************************************************************
 * \file
 *  mctnf.c
 *
 * \brief
 *  MCTNF specific XDM functions
 *
 * \version 0.1
 *
 * \date Nov 2013
 *
 * \author
 *  Shyam Jagannathan
 *****************************************************************************
 */

#include <xdc/std.h>

#include <ti/xdais/ialg.h>
#include <ti/sdo/fc/utils/api/alg.h>
#include "ividnf1.h"
#include <imctnf.h>
#include <mctnf.h>

/*
 *  ======== MCTNF_create ========
 *  Create an MCTNF instance object (using parameters specified by prms)
 */

MCTNF_Handle MCTNF_create
(
    const MCTNF_Fxns   *fxns,
    const MCTNF_Params *prms
)
{
  return ((MCTNF_Handle) ALG_create((IALG_Fxns *) fxns,
                                      NULL, (IALG_Params *) prms));
}

/*
 *  ======== MCTNF_delete ========
 *  Delete the MCTNF instance object specified by handle
 */

Void MCTNF_delete(MCTNF_Handle handle)
{
  ALG_delete((IALG_Handle) handle);
}

/*
 *  ======== MCTNF_init ========
 *  MCTNF module initialization
 */

Void MCTNF_init(Void)
{
}

/*
 *  ======== MCTNF_exit ========
 *  MCTNF module finalization
 */

Void MCTNF_exit(Void)
{
}

/*
 *  ======== MCTNF_control ========
 */

XDAS_Int32 MCTNF_control
(
    MCTNF_Handle         handle,
    MCTNF_Cmd            cmd,
    MCTNF_DynamicParams *params,
    MCTNF_Status        *status
)
{
  int error = 0;
  IALG_Fxns *fxns = (IALG_Fxns*)handle->fxns;
  fxns->algActivate((IALG_Handle)handle);

  error = handle->fxns->ividnf.control((IVIDNF1_Handle) handle, cmd,
                                        (IVIDNF1_DynamicParams *) params,
                                        (IVIDNF1_Status *) status);
  fxns->algDeactivate((IALG_Handle)handle);

  return error;
}

/*
 *  ======== MCTNF_process ========
 */

XDAS_Int32 MCTNF_processFrame
(
    MCTNF_Handle      handle,
    IVIDEO2_BufDesc  *inBufs,
    IVIDEO2_BufDesc  *outBufs,
    MCTNF_InArgs     *inargs,
    MCTNF_OutArgs    *outargs
)
{
  int error;
  IALG_Fxns *fxns = (IALG_Fxns*)handle->fxns;
  fxns->algActivate((IALG_Handle)handle);
  error = handle->fxns->ividnf.process((IVIDNF1_Handle) handle,
                                        inBufs,
                                        outBufs,
                                        (IVIDNF1_InArgs *) inargs,
                                        (IVIDNF1_OutArgs *) outargs);
  fxns->algDeactivate((IALG_Handle)handle);

  return (error);
}

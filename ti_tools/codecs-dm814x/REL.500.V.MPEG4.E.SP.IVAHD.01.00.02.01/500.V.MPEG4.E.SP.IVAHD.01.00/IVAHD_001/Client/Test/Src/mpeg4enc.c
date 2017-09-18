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
 * @file <mpeg4enc.c>
 *
 * @brief
 *
 * @author: Venugopala Krishna (venugopala@ti.com)
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
#include <ti/xdais/dm/ividenc2.h>
#include <impeg4enc.h>
#include <mpeg4enc.h>

/*******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/*******************************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/*---------------------- function prototypes ---------------------------------*/
extern IALG_Handle ALG_create(
  IALG_Fxns * fxns,
  IALG_Handle p,
  IALG_Params * params
  );
extern Void ALG_delete(IALG_Handle alg);


/**
********************************************************************************
 *  @func     MPEG4ENC_create
 *  @brief  This function creates an MPEG4VENC instance object (using parameters
 *          specified by prms)
 *
 *  @param[in]  fxns : Pointer to the MPEG4ENC_Fxns structure
 *
 *  @param[in]  prms : Pointer to the MPEG4ENC_Params structure
 *
 *  @return     Handle to the newly created mpeg4 encoder
********************************************************************************
*/
MPEG4ENC_Handle MPEG4ENC_create(const MPEG4ENC_Fxns * fxns,
                                const MPEG4ENC_Params * prms)
{
  return ((MPEG4ENC_Handle) ALG_create((IALG_Fxns *) fxns, NULL,
    (IALG_Params *) prms));
}


/**
********************************************************************************
 *  @func     MPEG4ENC_delete
 *  @brief  This function deletes the MPEG4VENC instance object specified
 *          by handle
 *
 *  @param[in]  handle : Pointer to the MPEG4ENC_Handle structure
 *
 *  @return None
********************************************************************************
*/
Void MPEG4ENC_delete(MPEG4ENC_Handle handle)
{
  ALG_delete((IALG_Handle) handle);
}


/**
********************************************************************************
 *  @func     MPEG4ENC_init
 *  @brief  MPEG4VENC module initialization
 *
 *  @return None
********************************************************************************
*/
Void MPEG4ENC_init(Void)
{
}


/**
********************************************************************************
 *  @func     MPEG4ENC_exit
 *  @brief  MPEG4VENC module finalization
 *
 *  @return None
********************************************************************************
*/
Void MPEG4ENC_exit(Void)
{
}


/**
********************************************************************************
 *  @func     MPEG4ENC_control
 *  @brief  This function will change the configuration of the encoder before
 *          the process call of each frame by using the command type
 *
 *  @param[in]  handle : Pointer to the MPEG4ENC_Handle structure
 *
 *  @param[in]  cmd    : command type to the mpeg4 encoder
 *
 *  @param[in]  params : Pointer to the MPEG4ENC_DynamicParams structure
 *
 *  @param[in]  status : Pointer to the MPEG4ENC_Status structure
 *
 *  @return     IVIDENC2_EOK if success
 *              IVIDENC2_EUNSUPPORTED  - unsupported command or params
 *              IVIDENC2_EFAIL         - NULL pointer handle
********************************************************************************
*/
XDAS_Int32 MPEG4ENC_control(
  MPEG4ENC_Handle handle,
  MPEG4ENC_Cmd cmd,
  MPEG4ENC_DynamicParams * params,
  MPEG4ENC_Status * status
)
{
  int error = 0;

  error = handle->fxns->ividenc.control(
    (IVIDENC2_Handle) handle,
    cmd,
    (IVIDENC2_DynamicParams *) params,
    (IVIDENC2_Status *) status
    );

  return error;
}


/**
********************************************************************************
 *  @func     MPEG4ENC_encodeFrame
 *  @brief  This function will call the process of encoding for each frame
 *
 *  @param[in]  handle  : Pointer to the MPEG4ENC_Handle structure
 *
 *  @param[in]  inBufs  : Pointer to the IVIDE02 input buffer descriptors
 *
 *  @param[in]  outBufs : Pointer to the XDM2 output buffer descriptors
 *
 *  @param[in]  inargs  : Input arguments to the encoder process call
 *
 *  @param[in]  outargs : Input arguments from the encoder process call
 *
 *  @return     Error Code : IVIDENC2_EFAIL if any error happened else
 *                   IVIDENC2_EOK for sucessful encoding of frame
********************************************************************************
*/
XDAS_Int32 MPEG4ENC_encodeFrame(
  MPEG4ENC_Handle handle,
  IVIDEO2_BufDesc * inBufs,
  XDM2_BufDesc * outBufs,
  MPEG4ENC_InArgs * inargs,
  MPEG4ENC_OutArgs * outargs
)
{
  int error;

  error = handle->fxns->ividenc.process(
    (IVIDENC2_Handle) handle,
    inBufs,
    outBufs,
    (IVIDENC2_InArgs *) inargs,
    (IVIDENC2_OutArgs *) outargs
    );

  return (error);
}

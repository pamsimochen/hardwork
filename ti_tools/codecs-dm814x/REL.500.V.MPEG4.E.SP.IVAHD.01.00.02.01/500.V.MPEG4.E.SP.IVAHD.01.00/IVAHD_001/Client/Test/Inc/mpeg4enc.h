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
 * @file <mpeg4enc.h>
 *
 * @brief Type definitions and type casting of MPEG4 Enc Module used by the Test
 *            Applications
 *
 * @author: Venugopala Krishna   <venugopala@ti.com, TI India>
 *
 * @version 0.0 (Jun 2009) : Initial version
 *                           [Venugopala Krishna]
 *
 *******************************************************************************
*/
/*--------------------- compilation control switches -------------------------*/

#ifndef MPEG4ENC_
#define MPEG4ENC_

#include <ti/xdais/ialg.h>
#include <ti/xdais/dm/ividenc2.h>
#include <ti/sdo/fc/ires/hdvicp/ires_hdvicp2.h>
#include <impeg4enc.h>


/**
 *  ======== MPEG4ENC_Handle ========
 *  This pointer is used to reference all MPEG4ENC instance objects
 */
typedef struct IMPEG4ENC_Obj *MPEG4ENC_Handle;
extern IRES_Fxns MPEG4ENC_TI_IRES;

/**
 *  ======== MPEG4ENC_Params ========
 *  This structure defines the creation parameters for all MPEG4ENC objects
 */
typedef IMPEG4ENC_Params MPEG4ENC_Params;

/**
 *  ======== MPEG4ENC_PARAMS ========
 *  This structure defines the default creation parameters for MPEG4ENC objects
 */
#define MPEG4ENC_PARAMS   IMPEG4ENC_PARAMS

/**
 * ===========================================================================
 * MP4VE_DynamicParams
 *
 * This structure defines the Dynamic parameters for all MP4VE objects
*/
typedef IMPEG4ENC_DynamicParams MPEG4ENC_DynamicParams;

/**
 *  ======== MPEG4ENC_DYNAMICPARAMS ========
 *  This structure defines the default creation parameters for MPEG4ENC objects
 */
#define MPEG4ENC_DYNAMICPARAMS   IMPEG4ENC_DYNAMICPARAMS


/**
* ===========================================================================
* MP4VE_Status
*
* This structure defines the real-time parameters for MP4VE objects
*/
typedef IMPEG4ENC_Status MPEG4ENC_Status;

/**
 *  ======== IMPEG4ENC_InArgs ========
 *  This structure defines the runtime input arguments for IMPEG4ENC::process
 */

typedef IMPEG4ENC_InArgs MPEG4ENC_InArgs;

/**
 *  ======== IMPEG4ENC_OutArgs ========
 *  This structure defines the run time output arguments for IMPEG4ENC::process
 *  function.
 */

typedef IMPEG4ENC_OutArgs MPEG4ENC_OutArgs;

/**
 *  ======== IMPEG4ENC_Cmd ========
 *  This structure/enumaration defines the run time commands for IMPEG4ENC::
 *  control function.
 */
typedef IMPEG4ENC_Cmd MPEG4ENC_Cmd;
/**
 *  ======== IMPEG4ENC_Fxns ========
 *  This structure captures all the functions of MPEG4Encoder Algorithm Object.
 */
typedef IMPEG4ENC_Fxns MPEG4ENC_Fxns;

/**
* ===========================================================================
* control method commands
*/
#define MPEG4ENC_GETSTATUS    IMPEG4ENC_GETSTATUS
#define MPEG4ENC_SETPARAMS    IMPEG4ENC_SETPARAMS
#define MPEG4ENC_PREPROCESS   IMPEG4ENC_PREPROCESS
#define MPEG4ENC_RESET        IMPEG4ENC_RESET
#define MPEG4ENC_FLUSH        IMPEG4ENC_FLUSH
#define MPEG4ENC_SETDEFAULT   IMPEG4ENC_SETDEFAULT
#define MPEG4ENC_GETBUFINFO   IMPEG4ENC_GETBUFINFO

/**
 *  ======== MPEG4ENC_create ========
 *  Create an MPEG4VENC instance object (using parameters specified by prms)
 */
extern MPEG4ENC_Handle MPEG4ENC_create(const MPEG4ENC_Fxns * fxns,
                                       const MPEG4ENC_Params * prms);

/**
 *  ======== MPEG4ENC_delete ========
 *  Delete the MPEG4VENC instance object specified by handle
 */
extern Void MPEG4ENC_delete(MPEG4ENC_Handle handle);

/**
 *  ======== MPEG4ENC_init ========
 */
extern Void MPEG4ENC_init(void);

/**
 *  ======== MPEG4ENC_exit ========
 */
extern Void MPEG4ENC_exit(void);

/**
 *  ======== MPEG4ENC_enodeFrame ========
 */
extern XDAS_Int32 MPEG4ENC_encodeFrame(MPEG4ENC_Handle handle,
                                       IVIDEO2_BufDesc * InBufs,
                                       XDM2_BufDesc * OutBufs,
                                       MPEG4ENC_InArgs * Inargs,
                                       MPEG4ENC_OutArgs * Outargs);

/**
 *  ======== MPEG4ENC_control ========
 * Get, set, and change the parameters of the MP4VE function
 *  (using parameters specified by status).
 */
extern XDAS_Int32 MPEG4ENC_control(MPEG4ENC_Handle handle, MPEG4ENC_Cmd cmd,
                                   MPEG4ENC_DynamicParams * params,
                                   MPEG4ENC_Status * status);


#endif   /* MPEG4ENC_ */

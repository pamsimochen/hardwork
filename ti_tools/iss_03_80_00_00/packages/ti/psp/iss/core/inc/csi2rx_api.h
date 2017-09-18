/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file csi2rx_api.h
*
* This file contains routines declarations and defines for CSI receiver
*       driver core
*
* @path iss/drivers/drv_csi2rx/inc
*
* @rev 01.01
*/
/* -------------------------------------------------------------------------- 
 */
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *!
 * 13-Feb-2009 Petar Sivenov - Created *! *
 * =========================================================================== */
#ifndef _CSI2_RX_API_H
#define _CSI2_RX_API_H

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

    /* User code goes here */
    /* ------compilation control switches
     * --------------------------------------- */
/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
    /* ----- system and platform files ---------------------------- */
    // #include <msp/msp.h>
#include <ti/psp/iss/hal/iss/csi2/csi2.h>
/*-------program files ----------------------------------------*/
    // #include <ti/iss/drivers/hal/iss/csi2/csi2.h"
/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/

#define CAM_CSI_NUM_CTX    (8)

    typedef enum {
        ISS_CSI_DEV_A = 0,
        ISS_CSI_DEV_B = 1,
    } IssCSI2DevT;

    typedef unsigned short int IssCSI2CtxT;

    typedef void (*issCsi2rxCallbackT) (MSP_ERROR_TYPE status, uint32 arg1,
                                        void *arg2);

    typedef struct {
        MSP_PTR private;
        issCsi2rxCallbackT callback;
        MSP_U32 arg1;
        void *arg2;

    } IssCsi2rxIsrHandleT;

    typedef enum {
        ISS_CSI_PING = 0,
        ISS_CSI_PONG = 1,
    } IssCSI_BufferIdx;

    typedef enum {
        ISS_CSI_TRANSCODE_DISABLE,
        ISS_CSI_TRANSCODE_RAW10_EXP16,
        ISS_CSI_TRANSCODE_DPCM_RAW10,
        ISS_CSI_TRANSCODE_DPCM_RAW12,
        ISS_CSI_TRANSCODE_ALAW_RAW10
    } ISS_CSI_TRANSCODE_MODE;

    typedef struct {
        MSP_U32 h_start;
        MSP_U32 h_size;
        MSP_U32 v_start;
        MSP_U32 v_size;

    } IssCsi2rxTranscodeSizes_t;

    typedef struct {
        IssCsi2rxTranscodeSizes_t sizes;
        ISS_CSI_TRANSCODE_MODE mode;
        MSP_U32 valid;

    } IssCsi2rxTranscodeParams_t;

/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/

/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/

    MSP_ERROR_TYPE issCsi2rxInit(void);
    MSP_ERROR_TYPE issCsi2rxStop(IssCSI2DevT device_num);
    MSP_ERROR_TYPE issCsi2rxReset(IssCSI2DevT deviceNum);
    MSP_ERROR_TYPE issCsi2rxDeinit(void);
    MSP_ERROR_TYPE issCsi2rxConfigPingPong(IssCSI_BufferIdx idx, void *imgBuff,
                                           IssCSI2DevT deviceNum,
                                           IssCSI2CtxT number);

    MSP_ERROR_TYPE issCsi2rxConfigCtx(IssCSI2DevT deviceNum, IssCSI2CtxT number,
                                      csi2_cfg_t * pIssCsi2Config,
                                      void *imgBuff, void *imgBuff1,
                                      MSP_U32 lineByteOffst, MSP_U8 numLanes,
                                      MSP_U32 csiDDRspeedMhz);

    MSP_ERROR_TYPE issCsi2rxConfig(IssCSI2DevT deviceNum,
                                   csi2_cfg_t * pIssCsi2Config,
                                   IssCsi2rxTranscodeParams_t * pTransParams);

    MSP_ERROR_TYPE issCsi2rxTranscodeConfig(IssCSI2DevT deviceNum,
                                            IssCsi2rxTranscodeParams_t *
                                            pTransParams, MSP_U32 nCtxNumber);
    unsigned char issCsi2rxGetLatestBuffer(IssCSI2DevT deviceNum,
                                           IssCSI2CtxT number);
    MSP_ERROR_TYPE issCsi2rxStart(IssCSI2DevT deviceNum, IssCSI2CtxT number);
    MSP_ERROR_TYPE issCsi2rxUnhookFrameEndIsr(IssCSI2DevT deviceNum,
                                              IssCSI2CtxT number,
                                              IssCsi2rxIsrHandleT * handler);
    MSP_ERROR_TYPE issCsi2rxSetLineNum(IssCSI2DevT deviceNum,
                                       IssCSI2CtxT number, MSP_U32 lineNum);
    MSP_ERROR_TYPE issCsi2rxHookFrameEndIsr(IssCSI2DevT deviceNum,
                                            IssCSI2CtxT number,
                                            IssCsi2rxIsrHandleT * handler);
    MSP_ERROR_TYPE issCsi2rxSetIntLineNum(IssCSI2DevT deviceNum,
                                          IssCSI2CtxT number, MSP_U32 lineNum);
    MSP_ERROR_TYPE issCsi2rxFrameEndIrqEnable(IssCSI2DevT deviceNum,
                                              IssCSI2CtxT number);
    MSP_ERROR_TYPE issCsi2rxFrameEndIrqDisable(IssCSI2DevT deviceNum,
                                               IssCSI2CtxT number);
    MSP_ERROR_TYPE issCsi2rxFrameStartIrqEnable(IssCSI2DevT deviceNum,
                                                IssCSI2CtxT number);
    MSP_ERROR_TYPE issCsi2rxFrameStartIrqDisable(IssCSI2DevT deviceNum,
                                                 IssCSI2CtxT number);
    MSP_ERROR_TYPE issCsi2rxLineNumIrqDisable(IssCSI2DevT deviceNum,
                                              IssCSI2CtxT number);
    MSP_ERROR_TYPE issCsi2rxLineNumIrqEnable(IssCSI2DevT deviceNum,
                                             IssCSI2CtxT number);
    MSP_ERROR_TYPE issCsi2rxHookLineNumIsr(IssCSI2DevT deviceNum,
                                           IssCSI2CtxT number,
                                           IssCsi2rxIsrHandleT * handler,
                                           MSP_U32 lineNum);
    MSP_ERROR_TYPE issCsi2rxUnhookLineNumIsr(IssCSI2DevT deviceNum,
                                             IssCSI2CtxT number,
                                             IssCsi2rxIsrHandleT * handler);

    MSP_ERROR_TYPE issCsi2rxUnhookFrameStartIsr(IssCSI2DevT deviceNum,
                                                IssCSI2CtxT number,
                                                IssCsi2rxIsrHandleT * handler);
    MSP_ERROR_TYPE issCsi2rxHookFrameStartIsr(IssCSI2DevT deviceNum,
                                              IssCSI2CtxT number,
                                              IssCsi2rxIsrHandleT * handler);

    MSP_ERROR_TYPE issCsi2rxFifoOvfDisable(IssCSI2DevT deviceNum);
    MSP_ERROR_TYPE issCsi2rxFifoOvfEnable(IssCSI2DevT deviceNum);
    MSP_ERROR_TYPE issCsi2rxHookFifoOvfIsr(IssCSI2DevT deviceNum,
                                           IssCsi2rxIsrHandleT * handler);
    MSP_ERROR_TYPE issCsi2rxUnhookFifoOvfIsr(IssCSI2DevT deviceNum,
                                             IssCsi2rxIsrHandleT * handler);

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _CSI2_RX_API_H */

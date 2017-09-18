/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/
/**
  \file vps_capture_driver.h

  \brief VPS Driver layer - VIP Capture API
*/

/**
  \ingroup VPS_DRV_DRV_API
  \defgroup VPS_DRV_DRV_CAPTURE VPS Driver layer - VIP Capture API

  The API from this modules are used by FVID2 layer to make call to
  specific drivers.
  User's do not need to know all details about this API.

  This is documented for reference purposes and advanced level users.

  Refer to \ref VPS_DRV_FVID2_VIP_CAPTURE for details of user level VIP
  capture API

  @{
*/

#ifndef _VPS_CAPTURE_DRIVER_H_
#define _VPS_CAPTURE_DRIVER_H_

#include <ti/psp/vps/vps_capture.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>

/* functions's  */

/**
 * \brief One-time capture sub-system init
 *
 * Gets called as part of FVID2_init()
 *
 * Initializes internal data structures and common system level resources
 * related to capture
 *
 * \return 0 on success, else failure
*/
Int32 Vps_captInit (  );

/**
 * \brief One-time capture sub-system de-init
 *
 * Free's all resources allocated during Vps_captInit
 *
 * \return 0 on success, else failure
*/
Int32 Vps_captDeInit (  );

/**
  \brief Create a capture instance

  This API gets called when FVID2_create() is called

  \param drvId           [I ] Must be FVID2_VPS_CAPT_VIP_DRV
  \param instanceId      [I ] VPS_CAPT_INST_VIP0_PORTA..VPS_CAPT_INST_VIP1_PORTB
  \param createArgsVps   [I ] Instance creation arguments
  \param createStatusArgs[O ] Instance creation status
  \param cbPrm           [I ] Capture callback related parameters

  \return Capture handle on success else NULL
*/
Fdrv_Handle Vps_captCreate (UInt32 drvId,
                            UInt32 instanceId,
                            Ptr createArgsVps,
                            Ptr createStatusArgs,
                            const FVID2_DrvCbParams *cbPrm);

/**
  \brief Delete a capture instance

  This API gets called when FVID2_delete() is called

  \param handle     [I ] Capture handle
  \param reserved   [I ] NOT USED, set as NULL

  \return 0 on success, else failure
*/
Int32 Vps_captDelete ( Fdrv_Handle handle, Ptr reserved );

/**
  \brief Start capture

  This API gets called when FVID2_start() is called

  Make sure capture handle is created before starting it

  \param handle     [I ] Capture handle

  \return 0 on success, else failure
*/
Int32 Vps_captStart ( Fdrv_Handle handle );

/**
  \brief Start capture

  This API gets called when FVID2_stop() is called

  \param handle     [I ] Capture handle

  \return 0 on success, else failure
*/
Int32 Vps_captStop ( Fdrv_Handle handle, UInt32 doReset );

/**
  \brief Queue frames in capture handle

  This API gets called when FVID2_queue() is called

  \param handle     [I ] Capture handle
  \param frameList  [I ] Frames to queue
  \param streamId   [I ] Stream to which the frames belong

  \return 0 on success, else failure
*/
Int32 Vps_captQueue ( Fdrv_Handle handle,
                      FVID2_FrameList * frameList, UInt32 streamId );

/**
  \brief De-Queue frames from capture handle

  This API gets called when FVID2_dequeue() is called

  \param handle     [I ] Capture handle
  \param frameList  [ O] De-queued frames
  \param streamId   [I ] Stream from which to dequeue the frames
  \param timeout    [I ] BIOS_WAIT_FOREVER to block the call
                         until atleast one frame is available,
                         BIOS_NO_WAIT for non-blocking call

  \return 0 on success, else failure
*/
Int32 Vps_captDequeue ( Fdrv_Handle handle,
                        FVID2_FrameList * frameList, UInt32 streamId,
                        UInt32 timeout );

/**
  \brief Capture control

  This API gets called when FVID2_control() is called

  \param handle        [I ] Capture handle
  \param cmd           [I ] command ID
  \param cmdArgs       [I ] command arguments
  \param cmdStatusArgs [ O] command return status

  \return 0 on success, else failure
*/
Int32 Vps_captControl ( Fdrv_Handle handle, UInt32 cmd,
                        Ptr cmdArgs, Ptr cmdStatusArgs );


Int32 Vps_captPrintAdvancedStatistics(UInt32 totalTimeInMsecs);

#endif

/* @}*/
